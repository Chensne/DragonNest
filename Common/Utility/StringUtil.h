#pragma once

#include <deque>

// std::string 은 크기가 16이상인 문자열은 전부 동적할당을 해버려서 
// 파일명의 대부분이 16길이를 넘기므로 파일명 체크시 마다의 잦은 동적할당의 부담이 크다.
// 파일명 스트링에는 버퍼크기가 적당히 큰 클래스를 직접 만들어서 사용하도록 한다.

// typedef std::string CFileNameString;		// 사용시 문제가 생겼을때는 이것을 킨다.

class CFileNameString
{
public:
	enum {
		INTERNAL_BUFFER_SIZE = 50,			// 대부분의 파일명의 길이가 100이하이며, 100이넘는것만 동적 할당 한다.
	};
	typedef size_t size_type;

private:
	char m_szInternalBuffer[ INTERNAL_BUFFER_SIZE + 1 ];
	char *m_dynamicBuffer;	
	size_type m_nSize;
	size_type m_nCapacity;

private:
	void _Assign( const char *pString ) 
	{
		size_type nLen = strlen( pString );
		if( nLen <= INTERNAL_BUFFER_SIZE ) {
			if( nLen == 0 ) {
				m_szInternalBuffer[0]='\0';
			}
			else {				
				strcpy_s( m_szInternalBuffer, pString );				
			}
		}
		else {
			m_szInternalBuffer[ 0 ] = '\0';
			if( m_nCapacity < nLen ) {
				if( m_nCapacity != 0 ) {
					delete [] m_dynamicBuffer;
				}
				m_dynamicBuffer = new char[ nLen + 1];
				m_nCapacity = nLen;
			}
			strcpy_s( m_dynamicBuffer, nLen+1, pString );
		}
		m_nSize = nLen;
	}

	void _Reset() {
		m_szInternalBuffer[ 0 ] = '\0';				
		m_dynamicBuffer = NULL;
		m_nSize = 0;
		m_nCapacity = 0;
	}

	void _Destroy() {
		if( m_dynamicBuffer ) {
			delete [] m_dynamicBuffer;
			m_dynamicBuffer = NULL;
			m_nCapacity = 0;
		}
	}

public:
	CFileNameString() {
		_Reset();		
	}

	CFileNameString( const char *pString ) {
		_Reset();
		_Assign( pString );
	}

	CFileNameString( std::string &szString ) {
		_Reset();
		_Assign( szString.c_str() );
	}
	CFileNameString( const CFileNameString &szString )
	{
		_Reset();
		_Assign( szString.c_str() );
	}

	~CFileNameString() {
		_Destroy();
	}

public:
	void operator = ( const CFileNameString &rhs )
	{
		_Assign( rhs.c_str() );
	}

	void operator = ( const char *pString )
	{
		_Assign( pString );
	}

	void operator = ( char *pString ) 
	{
		_Assign( pString );
	}

	void operator = ( std::string &szString ) 
	{
		_Assign( szString.c_str() );
	}

	CFileNameString& operator += ( const CFileNameString& rhs ) 
	{
		std::string szFullString = (char*)c_str();
		szFullString += (char*)rhs.c_str();
		_Assign( (char*)szFullString.c_str() );
		return *this;
	}

	CFileNameString operator + ( const CFileNameString& rhs ) 
	{
		std::string szFullString = (char*)c_str();
		szFullString += (char*)rhs.c_str();		
		return CFileNameString( szFullString.c_str() ) ;
	}
	size_t size()
	{
		return m_nSize;
	}

	const char * c_str() const
	{
		return ( m_nSize <= INTERNAL_BUFFER_SIZE ) ? m_szInternalBuffer : m_dynamicBuffer;
	}

	char * begin()
	{
		return const_cast<char*>(c_str());
	}

	char * end()
	{
		return const_cast<char*>(c_str()) + m_nSize;
	}

	const char & operator [] ( unsigned int index ) const
	{
		return c_str()[ index ];
	}

	const bool operator < ( const CFileNameString & rhs ) const
	{
		return (strcmp( c_str(), rhs.c_str() )	< 0 );
	}

	bool operator == ( char *rhs )
	{
		return (strcmp( c_str(), rhs ) == 0 );
	}

	bool operator == ( CFileNameString &rhs )
	{
		return (strcmp( c_str(), rhs.c_str() ) == 0 );
	}

	bool empty() 
	{ 
		return ( m_nSize == 0 );
	}
};

template <class T, int _PoolSize = 10240 >
class StringPool
{
public:
	StringPool() : m_pBuffer( NULL ), m_nAllocSize(0), m_nSize(0) {}
	~StringPool() {
		Clear();
	} 

private:
	std::vector<T*> m_BufferList;
	T *m_pBuffer;
	size_t m_nAllocSize;
	size_t m_nSize;

public:
	void Clear() {
		std::vector<T*>::iterator it = m_BufferList.begin();
		while( it != m_BufferList.end() ) {
			delete [] (*it);
			++it;
		}
		m_BufferList.clear();
	}

	void Reserve( size_t nSize ) {		
		if( m_pBuffer == NULL ) {
			m_pBuffer = new T[ _PoolSize ];
			m_BufferList.push_back( m_pBuffer );
		}
		else {
			if( m_nSize + nSize > _PoolSize ) {
				m_pBuffer = new T[ _PoolSize ];
				m_BufferList.push_back( m_pBuffer );
				m_nSize = 0;
			}
		}		
	}

	T *Alloc( T *pSource , size_t nLength ) {		
		ASSERT( (nLength+1 < _PoolSize) && (nLength >= 0));
		if( nLength == 0 ) {
			static T cNullTerminate = '\0';
			return &cNullTerminate;
		}
		Reserve( (nLength + 1) );
		if( m_nSize > nLength+1 ) {
			if( memcmp( m_pBuffer + m_nSize - (nLength+1) , pSource, (nLength+1)*sizeof(T) ) == 0 ) {
				return m_pBuffer + m_nSize - (nLength+1);
			}
		}
		memcpy_s( m_pBuffer + m_nSize, (_PoolSize - m_nSize)*sizeof(T), pSource, sizeof(T) * nLength );
		m_pBuffer[ m_nSize + nLength ] = 0;
		T *pResult = m_pBuffer + m_nSize;
		m_nSize += (nLength+1);
		return pResult;
	}
};

bool ToMultiString(IN std::wstring& wstr, OUT std::string& str );
bool ToWideString(IN std::string& str, OUT std::wstring& wstr );

bool ToMultiString(IN WCHAR* wstr, OUT std::string& str );
bool ToWideString(IN CHAR* str, OUT std::wstring& wstr );

// 소문자로 변환
void ToLowerA(std::string& str);
void ToLowerA( CFileNameString& str);
void ToLowerW(std::wstring& str);

// 대문자로 변환
void ToUpperA(std::string& str);
void ToUpperW(std::wstring& str);

std::wstring FormatW( const WCHAR* fmt, ... );
std::string FormatA( const CHAR* fmt, ... );

/*
	공백으로 문자를 나눌 경우.. 대충 이런식으로 쓴다.
	std::vector<std::string> tokens;
	std::string str("-IP 127.0.0.1 -Port 5000", );
	Tokenize( str, tokens, " ");
*/

void TokenizeA( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " " );
void TokenizeW( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters = L" " );
void TokenizeW( const std::wstring& str, std::deque<std::wstring>& tokens, const std::wstring& delimiters = L" " );
void TokenizeW_NewLine( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters = L" " );

/*
	std::vector<std::string> tokens;
	std::string str("{user_nick} {user_class}", );
	Tokenize( str, tokens, "{", "}" ); 마지막 인자는 검색구분자를 포함할것인지 여부
*/
void TokenizeW( std::wstring& str, std::vector<std::wstring>& tokens, std::wstring& szBegin, std::wstring& szEnd, bool bIncludeDelimiters );
void TokenizeA( std::string& str, std::vector<std::string>& tokens, std::string& szBegin, std::string& szEnd, bool bIncludeDelimiters  );


// 문자열의 모든 공백을 없애준다.
void RemoveSpaceA(std::string& str);
void RemoveSpaceW(std::wstring& str);

// 문자열에서 del 을 지운다.
void RemoveStringA(std::string& str, const std::string& del);
void RemoveStringW(std::wstring& str, const std::wstring& del);

// str 문자열 중에서 szOld 가 있으면 전부 szNew 로 변경한다.
void AllReplaceW( std::wstring& str, std::wstring& szOld, std::wstring& szNew );
void AllReplaceA( std::string& str, std::string& szOld, std::string& szNew );

// 문자열 앞뒤 공백 제거
void TrimString( char *pString );


// 골드 Gold - 3자리마다 콤마 ',' 처리.
std::wstring GoldToMoneyString( INT64 nGold );

// 콤마스트링.
std::wstring AtoCommaString( int nValue );


// : 골드획득 - 0인 단위는 출력하지 않음. (ex> 1골드0실버1쿠퍼 => 1골드1쿠퍼 ).
void GoldToString( std::wstring & rStr, INT64 coin, const wchar_t * pStrGold, const wchar_t * pStrSilver, const wchar_t * pStrCopper );
bool IsExistString( const std::wstring& wszString, const std::wstring& wszFindString );