#include "Stdafx.h"
#include "StringUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const int STR_LEN_ = 16384;

bool ToMultiString(IN std::wstring& wstr, OUT std::string& str )
{
	char szBuff[STR_LEN_] = "";
	ZeroMemory(szBuff, sizeof(char)*STR_LEN_);
	WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, szBuff, STR_LEN_, NULL, NULL );
	str = szBuff;
	return true;

}

bool ToWideString(IN std::string& str, OUT std::wstring& wstr )
{
	WCHAR wszBuff[STR_LEN_] = L"";
	ZeroMemory(wszBuff, sizeof(WCHAR)*STR_LEN_);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszBuff, STR_LEN_ );
	wstr = wszBuff;
	return true;
}

bool ToMultiString(IN WCHAR* wstr, OUT std::string& str )
{
	return ToMultiString(std::wstring(wstr), str);
}

bool ToWideString(IN CHAR* str, OUT std::wstring& wstr )
{
	return ToWideString(std::string(str), wstr);

}

void ToLowerA(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower); 
}

void ToLowerA( CFileNameString& str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower); 
}

void ToLowerW(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), towlower); 
}

void ToUpperA(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper); 
}
void ToUpperW(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), towupper); 
}

std::wstring FormatW( const WCHAR* fmt, ... )
{
	WCHAR wszBuffer[8192] = L"";

	ZeroMemory(wszBuffer, sizeof(WCHAR) * 8192);
	va_list args;
	va_start( args, fmt );
	_vsnwprintf_s( wszBuffer, 8192-1, fmt, args );
	va_end( args );
	wszBuffer[8192-1] = 0;
	return std::wstring(wszBuffer);
}

std::string FormatA( const CHAR* fmt, ... )
{
	CHAR szBuffer[8192] = "";

	ZeroMemory(szBuffer, sizeof(CHAR) * 8192);
	va_list args;
	va_start( args, fmt );
	_vsnprintf_s( szBuffer, 8192-1, fmt, args );
	va_end( args );
	szBuffer[8192-1] = 0;
	return std::string(szBuffer);
}

void TokenizeA( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters /* = " " */ )
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// 구분자가 아닌 첫 글자를 찾는다
	std::string::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::string::npos != pos || std::string::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of( delimiters, pos );
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find_first_of( delimiters, lastPos );
	}
}

void TokenizeW_NewLine( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters /* = L" "*/ )
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::wstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// 구분자가 아닌 첫 글자를 찾는다
	std::wstring::size_type pos = str.find( delimiters, lastPos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of( delimiters, pos );
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find( delimiters, lastPos );
	}
}

void TokenizeW( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters /* = L" "*/ )
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::wstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// 구분자가 아닌 첫 글자를 찾는다
	std::wstring::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of( delimiters, pos );
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find_first_of( delimiters, lastPos );
	}
}

void TokenizeW( const std::wstring& str, std::deque<std::wstring>& tokens, const std::wstring& delimiters /* = L" "*/ )
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::wstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// 구분자가 아닌 첫 글자를 찾는다
	std::wstring::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of( delimiters, pos );
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find_first_of( delimiters, lastPos );
	}
}

void TokenizeW( std::wstring& str, std::vector<std::wstring>& tokens, std::wstring& szBegin, std::wstring& szEnd, bool bIncludeDelimiters )
{
	std::wstring::size_type pos = str.find_first_of( szBegin, 0 );
	std::wstring::size_type lastPos = str.find_first_of( szEnd, pos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		if ( bIncludeDelimiters )
			tokens.push_back( str.substr( (pos), lastPos - pos+1 ) );
		else
			tokens.push_back( str.substr( (pos+1), lastPos - (pos+1)) );

		pos = str.find_first_of( szBegin, lastPos );
		lastPos = str.find_first_of( szEnd, pos );
	}
}

void TokenizeA( std::string& str, std::vector<std::string>& tokens, std::string& szBegin, std::string& szEnd, bool bIncludeDelimiters )
{
	std::string::size_type pos = str.find_first_of( szBegin, 0 );
	std::string::size_type lastPos = str.find_first_of( szEnd, pos );

	while( std::string::npos != pos || std::string::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		if ( bIncludeDelimiters )
			tokens.push_back( str.substr( (pos), lastPos - pos+1 ) );
		else
			tokens.push_back( str.substr( (pos+1), lastPos - (pos+1)) );

		pos = str.find_first_of( szBegin, lastPos );
		lastPos = str.find_first_of( szEnd, pos );
	}
}


// 문자열의 모든 공백을 없애준다.
void RemoveSpaceA(std::string& str)
{
	str.erase(remove_if(str.begin(), str.end(), isspace), str.end());	
}

void RemoveSpaceW(std::wstring& str)
{
	str.erase(remove_if(str.begin(), str.end(), iswspace), str.end());
}


void RemoveStringA(std::string& str, const std::string& del)
{
	while (true)
	{
		std::string::size_type npos = str.find(del);
		if ( npos < str.size() )
		{
			str.erase( npos, del.size());
		}
		else
			break;
	}
}


void RemoveStringW(std::wstring& str, const std::wstring& del)
{
	while (true)
	{
		std::wstring::size_type npos = str.find(del);
		if ( npos < str.size() )
		{
			str.erase( npos, del.size());
		}
		else
			break;
	}
}

// str 문자열 중에서 szOld 가 있으면 전부 szNew 로 변경한다.
void AllReplaceW( std::wstring& str, std::wstring& szOld, std::wstring& szNew )
{
	while (true)
	{
		std::wstring::size_type npos = str.find(szOld);
		if ( npos < str.size() )
		{
			str.replace(npos, szOld.size(), szNew);
		}
		else
			break;
	}
}


// str 문자열 중에서 szOld 가 있으면 전부 szNew 로 변경한다.
void AllReplaceA( std::string& str, std::string& szOld, std::string& szNew )
{
	while (true)
	{
		std::string::size_type npos = str.find(szOld);
		if ( npos < str.size() )
		{
			str.replace(npos, szOld.size(), szNew);
		}
		else
			break;
	}
}

void TrimString( char *pString )
{
	int nLen = (int)strlen( pString );
	if( nLen <= 0 ) return;
	if( pString[0] != ' ' && pString[nLen-1] != ' ') {
		return;
	}
	char *pSource = new char[ nLen+1 ];
	char *pPtr = pSource;
	strcpy_s( pSource, nLen+1, pString);

	while( *pPtr == ' ' ) pPtr++;
	char *pStartPtr = pPtr;
	pPtr = pSource + nLen - 1;
	while( *pPtr == ' ') pPtr--;
	pPtr[1] = '\0';
	strcpy( pString, pStartPtr);

	delete [] pSource;
}


// 골드 Gold - 3자리마다 콤마 ',' 처리.
std::wstring GoldToMoneyString( INT64 nGold )
{
	wchar_t buf[64]={0,};
	std::wstring strGold(::_i64tow(nGold/10000,buf,10));
	int nGoldSize = (int)strGold.size();
	if( nGoldSize > 3 )
	{
		int comCnt = (nGoldSize - 1) / 3; // 콤마개수.
		for( int i=1; i<=comCnt; ++i )
			strGold.insert( (size_t)(nGoldSize - 3 * i), L"," );
	}
	return strGold;
}

// 콤마스트링.
std::wstring AtoCommaString( int nValue )
{
	wchar_t buf[64]={0,};
	std::wstring str( ::_itow( nValue,buf,10 ) );
	int nSize = (int)str.size();
	if( nSize > 3 )
	{
		int comCnt = (nSize - 1) / 3; // 콤마개수.
		for( int i=1; i<=comCnt; ++i )
			str.insert( (size_t)(nSize - 3 * i), L"," );
	}
	return str;
}


// : 골드획득 - 0인 단위는 출력하지 않음. (ex> 1골드0실버1쿠퍼 => 1골드1쿠퍼 ).
void GoldToString( std::wstring & rStr, INT64 coin, const wchar_t * pStrGold, const wchar_t * pStrSilver, const wchar_t * pStrCopper )
{
	std::wstring strText;
	wchar_t str[24] = {0,};

	// 골드.	
	INT64 nValue = coin / 10000;
	if( nValue > 0 )
	{
		swprintf_s( str, _countof(str), pStrGold, nValue );
		strText.assign( str );
	}

	// 실버.
	nValue = (coin % 10000) / 100;
	if( nValue > 0 )
	{
		strText.append( L" " );
		swprintf_s( str, _countof(str), pStrSilver, nValue );			
		strText.append( str );
	}

	// 코퍼.
	nValue = coin % 100;
	if( nValue > 0 )
	{
		strText.append( L" " );
		swprintf_s( str, _countof(str), pStrCopper, nValue );			
		strText.append( str );
	}

	rStr.assign( strText.c_str() );
}

bool IsExistString( const std::wstring& wszString, const std::wstring& wszFindString )
{
	std::wstring wszCopy = wszString;
	std::wstring wszFindCopy = wszFindString;

	ToLowerW( wszCopy ); 	
	ToLowerW( wszFindCopy ); 

	size_t found = 0;
	found = wszCopy.find(wszFindCopy);

	if (found == std::wstring::npos)
		return false;

	return true;
}