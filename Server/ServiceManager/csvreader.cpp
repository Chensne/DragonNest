
#include "Stdafx.h"
#include "csvreader.h"

/*************************************************************
 * FUNCTIONS: CCSVReader::CCSVReader
 * SYNOPSIS : CSV 파일 리더 생성자
 *
 *************************************************************/
CCSVReader :: CCSVReader()
{
	m_pBuffer = NULL;
	m_pOffset = NULL;
	m_iLine = 0;
}


/*************************************************************
 * FUNCTIONS: CCSVReader::~CCSVReader
 * SYNOPSIS : CSV 파일 리더 소멸자
 *
 *************************************************************/
CCSVReader :: ~CCSVReader()
{
	Clear();
}


void CCSVReader :: Clear()
{
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	if (m_pOffset != NULL)
	{
		delete m_pOffset;
		m_pOffset = NULL;
	}

	m_iLine = 0;
}




/*************************************************************
 * FUNCTIONS: CCSVReader::ReadFile
 * SYNOPSIS : 파일 몽땅 읽기
 *
 * INPUT PARAMETERS: 파일 IO
 *************************************************************/
TCHAR * CCSVReader :: ReadFile(const TCHAR *fname)
{
	char * ptr;
	int len;

	FILE *fp;
	fp = _tfopen(fname, _T("rb"));
	if (fp == NULL)
		return NULL;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	if( len < 0)
	{
		fclose(fp);
		return NULL;
	}
	fseek(fp, 0, SEEK_SET);
	ptr = new char [len+2];
	fread(ptr, 1, len, fp);
	fclose(fp);

	ptr[len] = '\0';
	ptr[len+1] = '\0';

	static unsigned char _unicodeset[] = {
		0xff, 0xfe
	} ;
	if (!memcmp(ptr, _unicodeset, 2))
	{	//	유니코드로 저장되어 있음
#ifdef UNICODE
		TCHAR * wptr = new TCHAR [(len-2) / sizeof(TCHAR) + 1];
		memcpy(wptr, &ptr[2], (len+2) - 2);
		delete [] ptr;
		return wptr;
#else
		char * bptr = new char [len*2 + 1];
		memset(bptr, 0, len*2 + 1);
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&ptr[2], (len-2)/2, bptr, len*2 + 1, NULL, NULL);
		delete [] ptr;
		return bptr;
#endif
	}	else
	{
#ifdef UNICODE
		TCHAR * wptr = new TCHAR [len + 1];
		memset(wptr, 0, sizeof(TCHAR) * (len+1));
		MultiByteToWideChar(CP_ACP, 0, ptr, len, wptr, len+1);
		delete [] ptr;
		return wptr;
#else
		return ptr;
#endif
	}
}


/*************************************************************
 * FUNCTIONS: CCSVReader::Load
 * SYNOPSIS : 읽어온다
 *
 * INPUT PARAMETERS: 파일 IO
 *************************************************************/
bool CCSVReader :: Load(const TCHAR *fp)
{
	Clear();

//	파일 전체를 읽는다
//
	m_pBuffer = ReadFile(fp);

	if (m_pBuffer == NULL)
		return false;

//	라인수를 센 후 각 오프셋을 구한다
//
	m_iLine = CountLine(m_pBuffer, NULL);
	m_pOffset = new int [m_iLine];
	CountLine(m_pBuffer, m_pOffset);

	return true;
}


/*************************************************************
 * FUNCTIONS: CCSVReader::CountLine
 * SYNOPSIS : 라인수를 카운트하고, 각 오프셋을 write 한다
 *
 * INPUT PARAMETERS: 내용, 오프셋용 버퍼
 *************************************************************/
int CCSVReader :: CountLine(const TCHAR *buffer, int * offset)
{
	const TCHAR * p = buffer;
	int line;

	for(line=0; *p != '\0'; line++)
	{
		if (line > 0)
			p++;

		if (offset)
			offset[line] = (int)(p - buffer);

		for(; *p != '\0' && *p != '\n'; p++)
		{
			if (*p == '\"')
			{
				//for(p++; 1; )
				for(p++; ; )
				{
					if (p[0] == '\"')
					{
						if (p[1] == '\"')
							p += 2;
						else
							break;
					}	else
						p++;
				}
			}
		}
	}

	return line;
}


/*************************************************************
 * FUNCTIONS: CCSVReader::PassToken
 * SYNOPSIS : 한 토큰을 skip 한다
 *
 * INPUT PARAMETERS: 내용, 오프셋용 버퍼
 *************************************************************/
int CCSVReader :: PassToken(const TCHAR * str)
{
	int i = 0;

	if (str[0] == '\"')
	{
		for(i=1; ;)
		{
			if (str[i] == '\"')
			{
				if (str[i+1] == '\"')
					i += 2;
				else
					break;
			}	else
				i += 1;
		}
		return i+1;
	}	else
	{
		for(i=0; !_tcschr(_T("\n\t"), str[i]); i++) ;
		return i;
	}
}


/*************************************************************
 * FUNCTIONS: CCSVReader::GetData
 * SYNOPSIS : 특정 위치에서 데이타를 읽어온다
 *
 * INPUT PARAMETERS: 데이타가 없으면 0 리턴
 *************************************************************/
int CCSVReader :: GetData(int col, int row, TCHAR * outptr, int outlen)
{
	int i, off;

	if (row >= m_iLine)
		return 0;

	for(i=0, off=m_pOffset[row]; i<col; off++, i++)
	{
		off += PassToken(m_pBuffer + off);
		if (_tcschr(_T("\r\n"), m_pBuffer[off]))
		{
			outptr[0] = '\0';
			return 0;
		}
	}

	if (m_pBuffer[off] == '\"')
	{
		for(i=0, off+=1; ; i++)
		{
			if (m_pBuffer[off] == '\"' && m_pBuffer[off+1] == '\"')
			{
				if (i < outlen-1)
					outptr[i] = m_pBuffer[off];
				off += 2;
			}	else
			if (m_pBuffer[off] != '\"')
			{
				outptr[i] = m_pBuffer[off];
				off += 1;
			}	else
				break;
		}
	}	else
	{
		for(i=0; !_tcschr(_T("\r\n\t"), m_pBuffer[off]); i++, off++)
		{
			if (i < outlen-1)
				outptr[i] = m_pBuffer[off];
		}
	}

	if (i >= outlen-1)
		outptr[outlen-1] = '\0';
	else
		outptr[i] = '\0';
	return i;
}


/*************************************************************
 * FUNCTIONS: CCSVReader::GetData
 * SYNOPSIS : 특정 위치에서 데이타를 읽어온다
 *
 * INPUT PARAMETERS: 데이타가 없으면 0 리턴
 *************************************************************/
int CCSVReader :: Scan(int col, int row, TCHAR * fmt, ...)
{
	TCHAR szData[256], *ptr = szData;
	int len;

	len = GetData(col, row, szData, _countof(szData));

	if (len == 0)
		return 0;

	va_list arg;
	int i, cnt=0, j;
	TCHAR token[256];

	va_start(arg, fmt);

	for(i=0; fmt[i]; i++)
	{
		if (fmt[i] == '%')
		{
			for(; *ptr && _tcschr(_T("\n\r\t "), *ptr); ptr++) ;

			for(j=0; *ptr && !_tcschr(_T("\n\r\t "), *ptr); j++, ptr++)
				token[j] = *ptr;

			if (j == 0)
				return cnt;

			token[j] = '\0';

			switch(fmt[i+1])
			{
				case 'x' : case 'X' :
					_stscanf(token, _T("%x"), va_arg(arg, int*));
					break;
				case 'f' : case 'F' :
					_stscanf(token, _T("%f"), va_arg(arg, float*));
					break;
				case 'd' : case 'u' :
					_stscanf(token, _T("%d"), va_arg(arg, int*));
					break;
				case 's' :
					_tcscpy(va_arg(arg, TCHAR*), token);
					break;
			}
			cnt++;
		}
	}

	va_end(arg);
	return cnt;
}
