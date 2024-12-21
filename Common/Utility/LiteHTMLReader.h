#pragma once

#include "LiteHTMLTag.h"

#pragma warning(push, 4)
class CLiteHTMLReader;	// forward declaration

class ILiteHTMLReaderEvents
{
	friend class CLiteHTMLReader;

// Events
protected:
	virtual void BeginParse(DWORD dwAppData, bool &bAbort) = 0;
	virtual void StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort) = 0;
	virtual void EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort) = 0;
	virtual void Characters(const CStringW &rText, DWORD dwAppData, bool &bAbort) = 0;
	virtual void Comment(const CStringW &rComment, DWORD dwAppData, bool &bAbort) = 0;
	virtual void EndParse(DWORD dwAppData, bool bIsAborted) = 0;

public:
	virtual ~ILiteHTMLReaderEvents() = 0
	{
	}
};

class CLiteHTMLReader
{
public:
	enum EventMaskEnum 
	{
		notifyStartStop		= 0x00000001L,	// raise BeginParse and EndParse?
		notifyTagStart		= 0x00000002L,	// raise StartTag?
		notifyTagEnd		= 0x00000004L,	// raise EndTag?
		notifyCharacters	= 0x00000008L,	// raise Characters?
		notifyComment		= 0x00000010L,	// raise Comment?
	};

	enum ReaderOptionsEnum 
	{
		resolveEntities,	// determines whether entity references should be resolved

		// TODO: 
		// TODO: add more reader options
		// TODO: 
	};

// Construction/Destruction
public:
	CLiteHTMLReader() 
	{
		m_bResolveEntities = true;	// entities are resolved, by default
		m_dwAppData = 0L;	// reasonable default!
		m_dwBufPos = 0L;	// start from the very beginning
		m_dwBufLen = 0L;	// buffer length is unknown yet
		
		// default is to raise all of the events
		m_eventMask = (EventMaskEnum)(notifyStartStop  | 
									  notifyTagStart   | 
									  notifyTagEnd     | 
									  notifyCharacters | 
									  notifyComment    );

		m_pEventHandler = NULL;	// no event handler is associated
		m_lpszBuffer = NULL;
	}

public:
	EventMaskEnum getEventMask(void) const
		{ return (m_eventMask); }

	EventMaskEnum setEventMask(DWORD dwNewEventMask)
	{
		EventMaskEnum	oldMask = m_eventMask;
		m_eventMask = (EventMaskEnum)dwNewEventMask;
		return (oldMask);
	}

	EventMaskEnum setEventMask(DWORD addFlags, DWORD removeFlags)
	{
		DWORD	dwOldMask = (DWORD)m_eventMask;
		DWORD	dwNewMask = (dwOldMask | addFlags) & ~removeFlags;
		m_eventMask = (EventMaskEnum)dwNewMask;
		return ((EventMaskEnum)dwOldMask);
	}

	DWORD getAppData(void) const
		{ return (m_dwAppData); }

	DWORD setAppData(DWORD dwNewAppData)
	{
		DWORD	dwOldAppData = m_dwAppData;
		m_dwAppData = dwNewAppData;
		return (dwOldAppData);
	}

	ILiteHTMLReaderEvents* getEventHandler(void) const
		{ return (m_pEventHandler); }

	ILiteHTMLReaderEvents* setEventHandler(ILiteHTMLReaderEvents* pNewHandler)
	{
		ILiteHTMLReaderEvents *pOldHandler = m_pEventHandler;
		m_pEventHandler = pNewHandler;
		return (pOldHandler);
	}

	// returns the current value for the specified option
	bool getBoolOption(ReaderOptionsEnum option, bool& bCurVal) const;
	// sets a new value for the specified option
	bool setBoolOption(ReaderOptionsEnum option, bool bNewVal);

// Operations
public:
	// Note : wfopen()를 이용해서 유니코드로 작성된 HTML문서를 분석한다.
	UINT ReadFile( const wchar_t *filename );
	UINT ReadFile( const char *filename );
	// parses an HTML document from the specified string
	UINT Read(LPCWSTR lpszString);
	// parses an HTML document from a file given its HANDLE
	//UINT ReadFile(HANDLE hFile);

// Helpers
protected:
	virtual UINT parseDocument(void);
	virtual bool parseComment(CStringW &rComment);
	virtual bool parseTag(CLiteHTMLTag &rTag, bool &bIsOpeningTag, bool &bIsClosingTag);
	virtual void NormalizeCharacters(CStringW &rCharacters)
	{
		rCharacters.Replace(L"\r\n", L"");
		rCharacters.Remove(L'\n');
		rCharacters.Replace(L'\r', L' ');
		rCharacters.Replace(L'\t', L' ');
		//UNUSED_ALWAYS(rCharacters);
		//rCharacters;
	}

	void ResetSeekPointer(void)
		{ m_dwBufPos = 0L; }

	wchar_t ReadChar(void)
	{
		ASSERT(m_lpszBuffer != NULL);
		if (m_dwBufPos >= m_dwBufLen)
			return (NULL);
		return (m_lpszBuffer[m_dwBufPos++]);
	}

	wchar_t UngetChar(void)
	{
		ASSERT(m_lpszBuffer != NULL);
		ASSERT(m_dwBufPos);
		return (m_lpszBuffer[--m_dwBufPos]);
	}

	bool getEventNotify(DWORD dwEvent) const 
	{
		ASSERT(dwEvent == notifyStartStop  || 
			   dwEvent == notifyTagStart   || 
			   dwEvent == notifyTagEnd     || 
			   dwEvent == notifyCharacters || 
			   dwEvent == notifyComment);
		if (m_pEventHandler == NULL)
			return (false);
		return ((m_eventMask & dwEvent) == dwEvent);
	}

	bool isWhiteSpace(wchar_t ch) const
		{ return (::iswspace(ch) ? true : false); }

protected:
	bool	m_bResolveEntities;
	DWORD	m_dwAppData;
	DWORD	m_dwBufPos;
	DWORD	m_dwBufLen;
	EventMaskEnum	m_eventMask;
	ILiteHTMLReaderEvents*	m_pEventHandler;
	LPCWSTR	m_lpszBuffer;
};

inline bool CLiteHTMLReader::getBoolOption(ReaderOptionsEnum option, bool& bCurVal) const
{
	bool bSuccess = false;

	switch (option)
	{
	case resolveEntities:
		{
			bCurVal = m_bResolveEntities;
			bSuccess = true;
			break;
		}
	default:
		{
			bSuccess = false;
			break;
		}
	}
	return (bSuccess);
}

inline bool CLiteHTMLReader::setBoolOption(ReaderOptionsEnum option, bool bNewVal)
{
	bool bSuccess = false;

	switch (option)
	{
	case resolveEntities:
		{
			m_bResolveEntities = bNewVal;
			bSuccess = true;
			break;
		}
	default:
		{
			bSuccess = false;
			break;
		}
	}
	return (bSuccess);
}

inline bool CLiteHTMLReader::parseComment(CStringW &rComment)
{
	ASSERT(m_lpszBuffer != NULL);
	ASSERT(m_dwBufPos >= 0L);
	ASSERT(m_dwBufPos + 4 < m_dwBufLen);

	// HTML comments begin with '<!' delimeter and 
	// are immediately followed by two hyphens '--'
	if (::wcsncmp(&m_lpszBuffer[m_dwBufPos], L"<!--", 4))
		return (false);

	LPCWSTR	lpszBegin = &m_lpszBuffer[m_dwBufPos + 4];	
	// HTML comments end with two hyphen symbols '--'
	LPCWSTR	lpszEnd = ::wcsstr(lpszBegin, L"--");

	// comment ending delimeter could not be found?
	if (lpszEnd == NULL)
		// consider everything after current buffer position a comment
	{
		rComment = lpszBegin;
		m_dwBufPos += (4 + rComment.GetLength());
		return (true);
	}

	CStringW	strComment(lpszBegin, UINT(lpszEnd - lpszBegin));
	
	// end of buffer?
	if (lpszEnd + (sizeof(wchar_t) * 2) >= m_lpszBuffer + m_dwBufLen)
		return (false);

	// Note : skip comment ending delimeter '--'
	//
	while( *lpszEnd == L'-' )
		//lpszEnd = ::_tcsinc(lpszEnd);
		lpszEnd++;

	// Note : skip white-space characters after comment ending delimeter '--'
	//
	while (::iswspace(*lpszEnd))
		//lpszEnd = ::_tcsinc(lpszEnd);
		lpszEnd++;

	// comment has not been terminated properly
	if (*lpszEnd != L'>')
		return (false);

	//lpszEnd = ::_tcsinc(lpszEnd);
	lpszEnd++;
	m_dwBufPos += UINT(lpszEnd - &m_lpszBuffer[m_dwBufPos]);
	rComment = strComment;
	return (true);
}

inline bool CLiteHTMLReader::parseTag(CLiteHTMLTag &rTag, 
									  bool &bIsOpeningTag, 
									  bool &bIsClosingTag)
{
	ASSERT(m_lpszBuffer != NULL);
	ASSERT(m_dwBufPos >= 0L);
	ASSERT(m_dwBufPos + 4 < m_dwBufLen);

	UINT nRetVal = rTag.parseFromStr(&m_lpszBuffer[m_dwBufPos], 
					bIsOpeningTag, bIsClosingTag);
	if (!nRetVal)
		return (false);

	m_dwBufPos += nRetVal;
	return (true);
}

#pragma warning(pop)