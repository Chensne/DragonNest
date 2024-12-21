#include "stdafx.h"
#include "LiteHTMLReader.h"
#include "LiteHTMLEntityResolver.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#pragma warning(push, 4)
UINT CLiteHTMLReader::parseDocument(void)
{
	ASSERT(m_lpszBuffer != NULL);

	bool	bAbort = false;			// continue parsing or abort?
	bool	bIsClosingTag = false;	// tag parsed is a closing tag?
	bool	bIsOpeningTag = false;	// tag parsed is an opening tag?
	CStringW	strCharacters;			// character data 
	CStringW	strComment;				// comment data
	CStringW	strT;					// temporary storage
	DWORD	dwCharDataStart = 0L;	// starting position of character data
	DWORD	dwCharDataLen = 0L;		// length of character data
	LONG	lTemp = 0L;				// temporary storage
	wchar_t	ch = 0;					// character at current buffer position
	CLiteHTMLTag	oTag;			// tag information

	if ( (!m_lpszBuffer) || (!m_dwBufLen) )
		return (0U);

	// reset seek pointer to beginning
	ResetSeekPointer();

	// notify event handler about parsing startup
	if (getEventNotify(notifyStartStop))
	{
		bAbort = false;
		m_pEventHandler->BeginParse(m_dwAppData, bAbort);
		if (bAbort)	goto LEndParse;
	}

	// skip leading white-space characters
	while (isWhiteSpace(ReadChar()))
		;
	
	ch = UngetChar();
	while ((ch = ReadChar()) != NULL)
	{
		switch (ch)
		{

		// tag starting delimeter?
		case L'<':
			{
				UngetChar();
				
				strComment.Empty();
				if (!parseComment(strComment))
				{
					bIsOpeningTag = false;
					bIsClosingTag = false;
					if (!parseTag(oTag, bIsOpeningTag, bIsClosingTag))
					{
						++dwCharDataLen;

						// manually advance buffer position
						// because the last call to UngetChar()
						// moved it back one character
						ch = ReadChar();

						break;
					}
				}

				// clear pending notifications
				if ( (dwCharDataLen) || (strCharacters.GetLength()) )
				{
					strCharacters += CStringW(&m_lpszBuffer[dwCharDataStart], dwCharDataLen);
					NormalizeCharacters(strCharacters);
					//strCharacters.Replace(L"\r\n", L"");
					//strCharacters.Remove(L'\n');
					//strCharacters.Replace(L'\r', L' ');
					//strCharacters.Replace(L'\t', L' ');
					
					if ( (strCharacters.GetLength()) && 
						 (getEventNotify(notifyCharacters)) )
					{
						bAbort = false;
						m_pEventHandler->Characters(strCharacters, m_dwAppData, bAbort);
						if (bAbort)	goto LEndParse;
					}

					strCharacters.Empty();
				}

				dwCharDataLen = 0L;
				dwCharDataStart = m_dwBufPos;

				if (strComment.GetLength())
				{
					if (getEventNotify(notifyComment))
					{
						bAbort = false;
						m_pEventHandler->Comment(strComment, m_dwAppData, bAbort);
						if (bAbort)	goto LEndParse;
					}
				}
				else
				{
					if ( (bIsOpeningTag) && (getEventNotify(notifyTagStart)) )
					{
						bAbort = false;
						m_pEventHandler->StartTag(&oTag, m_dwAppData, bAbort);
						if (bAbort)	goto LEndParse;
					}

					if ( (bIsClosingTag) && (getEventNotify(notifyTagEnd)) )
					{
						bAbort = false;
						m_pEventHandler->EndTag(&oTag, m_dwAppData, bAbort);
						if (bAbort)	goto LEndParse;
					}
				}

				break;
			}

		// entity reference beginning delimeter?
		case L'&':
			{
				UngetChar();

				lTemp = 0;
				if (m_bResolveEntities)
					lTemp = CLiteHTMLEntityResolver::resolveEntity(&m_lpszBuffer[m_dwBufPos], ch);
				
				if (lTemp)
				{
					strCharacters += CStringW(&m_lpszBuffer[dwCharDataStart], dwCharDataLen) + ch;
					m_dwBufPos += lTemp;
					dwCharDataStart = m_dwBufPos;
					dwCharDataLen = 0L;
				}
				else
				{
					ch = ReadChar();
					++dwCharDataLen;
				}
				
				break;
			}
		
		// any other character
		default:
			{
				++dwCharDataLen;
				break;
			}
		}
	}

	// clear pending notifications
	if ( (dwCharDataLen) || (strCharacters.GetLength()) )
	{
		strCharacters += CStringW(&m_lpszBuffer[dwCharDataStart], dwCharDataLen) + ch;
		NormalizeCharacters(strCharacters);
		strCharacters.TrimRight();	// explicit trailing white-space removal

		if ( (strCharacters.GetLength()) && 
			 (getEventNotify(notifyCharacters)) )
		{
			bAbort = false;
			m_pEventHandler->Characters(strCharacters, m_dwAppData, bAbort);
			if (bAbort)	goto LEndParse;
		}
	}

LEndParse:
	// notify event handler about parsing completion
	if (getEventNotify(notifyStartStop))
		m_pEventHandler->EndParse(m_dwAppData, bAbort);

	m_lpszBuffer = NULL;
	m_dwBufLen = 0L;
	return (m_dwBufPos);
}

UINT CLiteHTMLReader::ReadFile( const wchar_t *filename )
{
	wchar_t wszBuf[1024+1]={0};
	FILE* fileHandle;
	errno_t err;
	size_t nRetSize(0);

	if ((err = _wfopen_s(&fileHandle, filename, L"rt,ccs=UTF-8")) != 0)
	{
		//wprintf(L"CLiteHTMLReader::ReadFile, the file was not opened!\n";
		return(0U);
	}

	std::wstring strString;

	while( !feof( fileHandle ) )
	{
		nRetSize = fread( wszBuf, sizeof( wchar_t ), 1024, fileHandle );

		if( ferror( fileHandle ) )     
		{
			perror( "Read error" );
			break;
		}

		wszBuf[nRetSize] = NULL;
		strString += wszBuf;
	}

	fclose( fileHandle );

	return Read( strString.c_str() );
}

UINT CLiteHTMLReader::ReadFile( const char *filename )
{
	wchar_t wszBuf[1024+1]={0};
	FILE* fileHandle;
	errno_t err;
	size_t nRetSize(0);

	if ((err = fopen_s(&fileHandle, filename, "rt,ccs=UTF-8")) != 0)
	{
		//wprintf(L"CLiteHTMLReader::ReadFile, the file was not opened!\n";
		return(0U);
	}

	std::wstring strString;

	while( !feof( fileHandle ) )
	{
		nRetSize = fread( wszBuf, sizeof( wchar_t ), 1024, fileHandle );

		if( ferror( fileHandle ) )     
		{
			perror( "Read error" );
			break;
		}

		wszBuf[nRetSize] = NULL;
		strString += wszBuf;
	}

	fclose( fileHandle );

	return Read( strString.c_str() );
}

UINT CLiteHTMLReader::Read(LPCWSTR lpszString)
{
	ASSERT(lpszString);
	//ASSERT(AfxIsValidString(lpszString));

	m_dwBufLen = (UINT)::wcslen(lpszString);
	if (m_dwBufLen)
	{
		m_lpszBuffer = lpszString;
		return (parseDocument());
	}

	return (0U);
}

//UINT CLiteHTMLReader::ReadFile(HANDLE hFile)
//{
//	ASSERT(hFile != INVALID_HANDLE_VALUE);
//	ASSERT(::GetFileType(hFile) == FILE_TYPE_DISK);
//
//	HANDLE	hFileMap;
//	LPCWSTR	lpsz;
//	UINT	nRetVal;
//
//	// determine file size
//	m_dwBufLen = ::GetFileSize(hFile, NULL);
//	if (m_dwBufLen == INVALID_FILE_SIZE)
//	{
//		//TRACE1( "(Error) CLiteHTMLReader::Read:"
//		//		" GetFileSize() failed;"
//		//		" GetLastError() returns 0x%08x.\n", ::GetLastError());
//		goto LError;
//	}
//
//	// calculate length, in wchar_ts, of the buffer
//	m_dwBufLen /= sizeof(wchar_t);
//	if (!m_dwBufLen)
//		return (0U);
//
//	// create a file-mapping object for the file
//	hFileMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0L, 0L, NULL);
//	if (hFileMap == NULL)
//	{
//		//TRACE1( "(Error) CLiteHTMLReader::Read:"
//		//		" CreateFileMapping() failed;"
//		//		" GetLastError() returns 0x%08x.\n", ::GetLastError());
//		goto LError;
//	}
//
//	// map the entire file into the address-space of the application
//	lpsz = (LPCWSTR)::MapViewOfFile(hFileMap, FILE_MAP_READ, 0L, 0L, 0L);
//	if (lpsz == NULL)
//	{
//		//TRACE1( "(Error) CLiteHTMLReader::Read:"
//		//		" MapViewOfFile() failed;"
//		//		" GetLastError() returns 0x%08x.\n", ::GetLastError());
//		goto LError;
//	}
//
//	m_lpszBuffer = lpsz;
//	nRetVal = parseDocument();
//	goto LCleanExit;
//
//LError:
//	nRetVal = 0U;
//	m_dwBufLen = 0L;
//
//LCleanExit:
//	if (lpsz != NULL)
//		VERIFY(::UnmapViewOfFile(lpsz));
//	if (hFileMap)
//		VERIFY(::CloseHandle(hFileMap));
//	return (nRetVal);
//}

#pragma warning(pop)
