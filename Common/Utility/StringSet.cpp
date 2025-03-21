

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "STRINGSET.H"
#include <ALGORITHM>
#include <SET>
#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::CStringSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet::CStringSet()
{
	m_lpszStringBuffer = NULL;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::CStringSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet::CStringSet(LPCTSTR pString)
{
	m_lpszStringBuffer = NULL;

	operator=(pString);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::CStringSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet::CStringSet(const CStringSet& pString)
{
	m_lpszStringBuffer = NULL;

	operator=(pString.Get());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::CStringSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet::CStringSet(const CStringSet* pString)
{
	m_lpszStringBuffer = NULL;

	operator=(pString->Get());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::~CStringSet
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet::~CStringSet()
{
	Empty();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::IsEmpty
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 유무 체크
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CStringSet::IsEmpty() const
{
	return (BOOL)(!m_lpszStringBuffer || _T('\0') == m_lpszStringBuffer[0]);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::IsNull
//*---------------------------------------------------------------
// DESC : 문자열 버퍼 포인터의 NULL 여부 체크
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CStringSet::IsNull() const
{
	return(!m_lpszStringBuffer);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Empty
//*---------------------------------------------------------------
// DESC : 문자열 버퍼를 제거, 초기화
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::Empty()
{
	if (m_lpszStringBuffer) {
		::free((LPVOID)((LPBYTE)(m_lpszStringBuffer) - sizeof(STRINGHEADER)));
		m_lpszStringBuffer = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::ReAlloc
//*---------------------------------------------------------------
// DESC : 문자열 데이터 버퍼를 제거, 초기화
// PARM :	1 . iNewSize - 새로 할당할 버퍼크기
//			2 . bForceTo - 버퍼크기를 앞에서 입력받은 값으로 강제로 맞출지 여부
// RETV : 새로 할당된 문자열 버퍼 포인터 반환
// PRGM : B4nFter
//*---------------------------------------------------------------
LPTSTR CStringSet::ReAlloc(INT iNewSize, BOOL bForceTo)
{
	if (iNewSize <= 0) {
		return NULL;
	}

	LPBYTE lpbtTempBuffer;
	LPSTRINGHEADER lpstStringHeader;
	if (m_lpszStringBuffer) {
		lpbtTempBuffer = (LPBYTE)(m_lpszStringBuffer) - sizeof(STRINGHEADER);
		if (bForceTo) {
			lpbtTempBuffer = (LPBYTE)::realloc(lpbtTempBuffer, iNewSize*sizeof(TCHAR)+sizeof(STRINGHEADER));
			lpstStringHeader = (LPSTRINGHEADER)(lpbtTempBuffer);
			lpstStringHeader->m_iSize = iNewSize;
			if (lpstStringHeader->m_iLinePtr > iNewSize-1) {
				lpstStringHeader->m_iLinePtr = iNewSize-1;
			}
		}
		else {
			lpstStringHeader = (LPSTRINGHEADER)(lpbtTempBuffer);
			if (lpstStringHeader->m_iSize < iNewSize) {
				lpbtTempBuffer = (LPBYTE)::realloc(lpbtTempBuffer, iNewSize*sizeof(TCHAR)+sizeof(STRINGHEADER));
				lpstStringHeader = (LPSTRINGHEADER)(lpbtTempBuffer);
				lpstStringHeader->m_iSize = iNewSize;
			}
		}
	}
	else {
		lpbtTempBuffer = (LPBYTE)::calloc(iNewSize*sizeof(TCHAR)+sizeof(STRINGHEADER), sizeof(BYTE));
		if (lpbtTempBuffer) {
			lpstStringHeader = (LPSTRINGHEADER)(lpbtTempBuffer);
			lpstStringHeader->m_iLinePtr = 0;
			lpstStringHeader->m_iSize = iNewSize;
		}
	}

	if (lpbtTempBuffer) {
		m_lpszStringBuffer = (LPTSTR)(lpbtTempBuffer+sizeof(STRINGHEADER));
		lpstStringHeader = (LPSTRINGHEADER)(lpbtTempBuffer);
		m_lpszStringBuffer[lpstStringHeader->m_iSize-1] = _T('\0');
		return m_lpszStringBuffer;
	}
	return NULL;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Get
//*---------------------------------------------------------------
// DESC : 문자열 버퍼 포인터를 반환
// PARM : N/A
// RETV : 문자열 버퍼 포인터 반환
// PRGM : B4nFter
//*---------------------------------------------------------------
LPCTSTR CStringSet::Get() const
{
	return m_lpszStringBuffer;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetA
//*---------------------------------------------------------------
// DESC : 문자열 버퍼 포인터를 반환 (ASCII 형식)
// PARM : N/A
// RETV : 문자열 버퍼 포인터 반환 (ASCII 형식, HEAP 메모리)
// PRGM : B4nFter
// P.S.>
//		- 반환된 문자열 데이터는 NULL이 아닐 경우 반드시 ::free() 로 해제되어야 함
//*---------------------------------------------------------------
LPCSTR CStringSet::GetA() const
{
	return ::strdup(CT2A(m_lpszStringBuffer));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetW
//*---------------------------------------------------------------
// DESC : 문자열 버퍼 포인터를 반환 (UNICODE 형식)
// PARM : N/A
// RETV : 문자열 버퍼 포인터 반환 (UNICODE 형식, HEAP 메모리)
// PRGM : B4nFter
// P.S.>
//		- 반환된 문자열 데이터는 NULL이 아닐 경우 반드시 ::free() 로 해제되어야 함
//*---------------------------------------------------------------
LPCWSTR CStringSet::GetW() const
{
	return ::wcsdup(CT2W(m_lpszStringBuffer));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Get
//*---------------------------------------------------------------
// DESC : 문자열 버퍼 포인터를 반환 (UNICODE 형식)
// PARM :	1 . nStart - 문자열 버퍼 포인터의 시작점
// RETV : 문자열 버퍼 포인터 반환 (UNICODE 형식)
// PRGM : B4nFter
//*---------------------------------------------------------------
LPCTSTR CStringSet::Get(INT nStart) const
{
	if (!m_lpszStringBuffer) {
		return NULL;
	}

	INT nSize = GetSize();
	if (nSize <= 0 || nStart < 0 || nSize-1 < nStart) {
		return NULL;
	}

	return m_lpszStringBuffer + nStart;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetA
//*---------------------------------------------------------------
// DESC : 문자열 버퍼 포인터를 반환 (ASCII 형식)
// PARM :	1 . nStart - 문자열 버퍼 포인터의 시작점
// RETV : 문자열 버퍼 포인터 반환 (ASCII 형식, HEAP 메모리)
// PRGM : B4nFter
// P.S.>
//		- 반환된 문자열 데이터는 NULL이 아닐 경우 반드시 ::free() 로 해제되어야 함
//*---------------------------------------------------------------
LPCSTR CStringSet::GetA(INT nStart) const
{
	if (!m_lpszStringBuffer) {
		return NULL;
	}

	INT nSize = GetSize();
	if (nSize <= 0 || nStart < 0 || nSize-1 < nStart) {
		return NULL;
	}

	return ::strdup(CT2A(m_lpszStringBuffer + nStart));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetW
//*---------------------------------------------------------------
// DESC : 문자열 버퍼 포인터를 반환 (UNICODE 형식)
// PARM :	1 . nStart - 문자열 버퍼 포인터의 시작점
// RETV : 문자열 버퍼 포인터 반환 (UNICODE 형식, HEAP 메모리)
// PRGM : B4nFter
// P.S.>
//		- 반환된 문자열 데이터는 NULL이 아닐 경우 반드시 ::free() 로 해제되어야 함
//*---------------------------------------------------------------
LPCWSTR CStringSet::GetW(INT nStart) const
{
	if (!m_lpszStringBuffer) {
		return NULL;
	}

	INT nSize = GetSize();
	if (nSize <= 0 || nStart < 0 || nSize-1 < nStart) {
		return NULL;
	}

	return ::wcsdup(CT2W(m_lpszStringBuffer + nStart));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetLine
//*---------------------------------------------------------------
// DESC : 전체 문자열 중 한 줄을 반환
// PARM : N/A
// PARM :	1 . nStart - 문자열 버퍼 포인터의 시작점
// RETV : 전체 문자열 중 한 줄을 반환한 CStringSet 객체
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet CStringSet::GetLine()
{ 
	CStringSet aString;
	if (!IsEmpty()) {
		INT iStartPtr = GetLinePtr();
		aString = Tokenize(DF_DEFAULT_NEWLINE_TOKEN_STRING, iStartPtr);
		SetLinePtr(iStartPtr);
	}
	return aString;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetLength
//*---------------------------------------------------------------
// DESC : 문자열의 길이를 반환 (TCHAR 기준)
// PARM : N/A
// RETV : 문자열의 길이
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::GetLength() const
{
	if (m_lpszStringBuffer) {
		return (INT)_tcslen(m_lpszStringBuffer);
	}
	else {
		return 0;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetHeader
//*---------------------------------------------------------------
// DESC : 문자열 버퍼헤더를 반환
// PARM : N/A
// RETV : 문자열 버퍼헤더
// PRGM : B4nFter
//*---------------------------------------------------------------
LPSTRINGHEADER CStringSet::GetHeader() const
{
	if (m_lpszStringBuffer) {
		return (LPSTRINGHEADER)((LPBYTE)(m_lpszStringBuffer) - sizeof(STRINGHEADER));
	}
	else {
		return NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetSize
//*---------------------------------------------------------------
// DESC : 문자열 버퍼의 길이를 반환 (TCHAR 기준)
// PARM : N/A
// RETV : 문자열 버퍼의 길이
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::GetSize() const
{
	if (m_lpszStringBuffer) {
		return GetHeader()->m_iSize;
	}
	else {
		return 0;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetLinePtr
//*---------------------------------------------------------------
// DESC : 문자열의 한 줄을 구분하는 포인터를 반환
// PARM : N/A
// RETV : 문자열 줄구분 포인터
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::GetLinePtr() const
{
	if (m_lpszStringBuffer) {
		return GetHeader()->m_iLinePtr;
	}
	else {
		return 0;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::SetLinePtr
//*---------------------------------------------------------------
// DESC : 문자열의 한 줄을 구분하는 포인터를 지정
// PARM :	1 . iLinePtr - 문자열의 한 줄을 구분하는 포인터 값
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::SetLinePtr(INT iLinePtr)
{
	if (m_lpszStringBuffer) {
		GetHeader()->m_iLinePtr = iLinePtr;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Reset
//*---------------------------------------------------------------
// DESC : 문자열 데이터와 헤더를 리셋
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::Reset()
{
	if (m_lpszStringBuffer) {
		m_lpszStringBuffer[0] = _T('\0');
		GetHeader()->m_iLinePtr = 0;
	}	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::ResetLinePtr
//*---------------------------------------------------------------
// DESC : 문자열의 한 줄을 구분하는 포인터를 초기화
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::ResetLinePtr()
{
	if (m_lpszStringBuffer) {
		GetHeader()->m_iLinePtr = 0;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::GetAt
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 특정 위치에 있는 문자를 반환 (TCHAR 기준)
// PARM :	1 . nIndex - 문자열 데이터의 특정 위치
// RETV : 문자열 데이터의 특정 위치에 있는 문자
// PRGM : B4nFter
//*---------------------------------------------------------------
#pragma warning (disable:4310)
TCHAR CStringSet::GetAt(INT nIndex) const
{
	INT nLength = GetLength();
	if (nIndex >= 0 && nIndex < nLength) {
		return m_lpszStringBuffer[nIndex];
	}

	return (TCHAR)(0xFFFF);
}
#pragma warning (default:4310)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::SetAt
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 특정 위치에 문자를 지정 (TCHAR 기준)
// PARM :	1 . nIndex - 문자열 데이터의 특정 위치
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::SetAt(INT nIndex, TCHAR cChar)
{
	INT nLength = GetLength();
	if (nIndex >= 0 && nIndex < nLength) {
		m_lpszStringBuffer[nIndex] = cChar;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator[]
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 특정 위치에 있는 문자를 반환, GetAt() 과 같음 (TCHAR 기준)
// PARM :	1 . nIndex - 문자열 데이터의 특정 위치
// RETV : 문자열 데이터의 특정 위치에 있는 문자
// PRGM : B4nFter
//*---------------------------------------------------------------
TCHAR CStringSet::operator[](INT nIndex) const
{
	INT nLength = GetLength();
	if (nIndex >= 0 && nIndex < nLength-1) {
		return m_lpszStringBuffer[nIndex];
	}
	return _T('\0');
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator LPCTSTR
//*---------------------------------------------------------------
// DESC : CStringSet 클래스의 LPCTSTR 형 변환연산자
// PARM : N/A
// RETV : LPCTSTR 형 문자열 데이터
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet::operator LPCTSTR() const
{
	if (m_lpszStringBuffer) {
		return const_cast<LPCTSTR>(m_lpszStringBuffer);
	}
	return NULL;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator=
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pString - CStringSet 객체에 치환할 문자열 데이터
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::operator=(LPCTSTR pString)
{
	if (pString) {
		LPTSTR aStringBuffer = m_lpszStringBuffer;
		INT iSize = (INT)_tcslen(pString)+1;
		if (iSize > 0 && GetSize() < iSize) {
			aStringBuffer = ReAlloc(iSize);
		}
		if (aStringBuffer) {
			_tcsncpy(aStringBuffer, pString, GetSize()-1);
		}
	}
	else {
		Reset();
	}
	ResetLinePtr();
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator=
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pString - CStringSet 객체에 치환할 CStringSet 객체
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::operator=(CStringSet& pString)
{
	if (!pString.IsEmpty()) {
		LPTSTR aStringBuffer = m_lpszStringBuffer;
		INT iSize = pString.GetSize();
		if (iSize > 0 && GetSize() < iSize) {
			aStringBuffer = ReAlloc(iSize);
		}
		if (aStringBuffer) {
			_tcsncpy(aStringBuffer, pString.Get(), GetSize()-1);
		}
	}
	else {
		Reset();
	}
	ResetLinePtr();
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator=
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pString - CStringSet 객체에 치환할 CStringSet 객체 포인터
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::operator=(CStringSet* pString)
{
	if (pString && !pString->IsEmpty()) {
		LPTSTR aStringBuffer = m_lpszStringBuffer;
		INT iSize = pString->GetSize();
		if (iSize > 0 && GetSize() < iSize) {
			aStringBuffer = ReAlloc(iSize);
		}
		if (aStringBuffer) {
			_tcsncpy(aStringBuffer, pString->Get(), GetSize()-1);
		}
	}
	else {
		Reset();
	}
	ResetLinePtr();
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator+=
//*---------------------------------------------------------------
// DESC : 증가 연산자 구현
// PARM :	1 . pString - CStringSet 객체의 문자열 데이터에 추가될 문자열 데이터
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::operator+=(LPCTSTR pString)
{
	if (pString) {
		LPTSTR aStringBuffer = m_lpszStringBuffer;
		INT iSize1 = (INT)_tcslen(pString)+1;
		INT iSize2 = GetSize();
		INT nLength = GetLength();
		if (iSize1 > iSize2 - nLength - 1) {
			aStringBuffer = ReAlloc(iSize1 + iSize2);
		}
		if (aStringBuffer) {
			_tcsncat(aStringBuffer, pString, GetSize()-1);
		}
	}
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator+=
//*---------------------------------------------------------------
// DESC : 증가 연산자 구현
// PARM :	1 . pString - CStringSet 객체의 문자열 데이터에 추가될 CStringSet 객체 (문자열 데이터)
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::operator+=(CStringSet& pString)
{
	if (!pString.IsEmpty()) {
		LPTSTR aStringBuffer = m_lpszStringBuffer;
		INT iSize1 = pString.GetSize();
		INT iSize2 = GetSize();
		INT nLength = GetLength();
		if (iSize1 > iSize2 - nLength - 1) {
			aStringBuffer = ReAlloc(iSize1 + iSize2);
		}
		if (aStringBuffer) {
			_tcsncat(aStringBuffer, pString.Get(), GetSize()-1);
		}
	}
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator+=
//*---------------------------------------------------------------
// DESC : 증가 연산자 구현
// PARM :	1 . pString - CStringSet 객체의 문자열 데이터에 추가될 CStringSet 객체 포인터 (문자열 데이터)
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::operator+=(CStringSet* pString)
{
	if (pString && !pString->IsEmpty()) {
		LPTSTR aStringBuffer = m_lpszStringBuffer;
		INT iSize1 = pString->GetSize();
		INT iSize2 = GetSize();
		INT nLength = GetLength();
		if (iSize1 > iSize2 - nLength - 1) {
			aStringBuffer = ReAlloc(iSize1 + iSize2);
		}
		if (aStringBuffer) {
			_tcsncat(aStringBuffer, pString->Get(), GetSize()-1);
		}
	}
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator==
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 == 비교연산
// PARM :	1 . pString - 비교할 문자열
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator==(LPCTSTR pString) const
{
	return (Compare(pString) == 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator==
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 == 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator==(const CStringSet& pString) const
{
	return (Compare(pString.Get()) == 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator==
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 == 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 포인터 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator==(const CStringSet* pString) const
{
	return (Compare(pString->Get()) == 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator!=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 != 비교연산
// PARM :	1 . pString - 비교할 문자열
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator!=(LPCTSTR pString) const
{
	return (Compare(pString) != 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator!=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 == 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator!=(const CStringSet& pString) const
{
	return (Compare(pString.Get()) != 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator!=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 != 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 포인터 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator!=(const CStringSet* pString) const
{
	return (Compare(pString->Get()) != 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator<
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 < 비교연산
// PARM :	1 . pString - 비교할 문자열
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator<(LPCTSTR pString) const
{
	return (Compare(pString) < 0);
}

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator<
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 < 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator<(const CStringSet& pString) const
{
	return (Compare(pString.Get()) < 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator<
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 < 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 포인터 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator<(const CStringSet* pString) const
{
	return (Compare(pString->Get()) < 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator>
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 > 비교연산
// PARM :	1 . pString - 비교할 문자열
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator>(LPCTSTR pString) const
{
	return (Compare(pString) > 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator>
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 > 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator>(const CStringSet& pString) const
{
	return (Compare(pString.Get()) > 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator>
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 > 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 포인터 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator>(const CStringSet* pString) const
{
	return (Compare(pString->Get()) > 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator<=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 <= 비교연산
// PARM :	1 . pString - 비교할 문자열
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator<=(LPCTSTR pString) const
{
	return (Compare(pString) <= 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator<=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 <= 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator<=(const CStringSet& pString) const
{
	return (Compare(pString.Get()) <= 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator<=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 <= 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 포인터 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator<=(const CStringSet* pString) const
{
	return (Compare(pString->Get()) <= 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator>=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 >= 비교연산
// PARM :	1 . pString - 비교할 문자열
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator>=(LPCTSTR pString) const
{
	return (Compare(pString) >= 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator>=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 >= 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator>=(const CStringSet& pString) const
{
	return (Compare(pString.Get()) >= 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::operator>=
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 >= 비교연산
// PARM :	1 . pString - 비교할 CStringSet 객체 포인터 (문자열 데이터)
// RETV : true - 성공 / false - 실패 (stl 컨테이너의 인자로 사용하기 위해 BOOL 대신 bool 사용)
// PRGM : B4nFter
//*---------------------------------------------------------------
bool CStringSet::operator>=(const CStringSet* pString) const
{
	return (Compare(pString->Get()) >= 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Set
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pString - CStringSet 객체에 치환할 문자열 데이터
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::Set(LPCTSTR pString)
{
	return(operator=(pString));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Set
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pString - CStringSet 객체에 치환할 CStringSet 객체
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::Set(CStringSet& pString)
{
	return(operator=(pString));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Set
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pString - CStringSet 객체에 치환할 CStringSet 객체 포인터
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::Set(CStringSet* pString)
{
	return(operator=(pString));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Append
//*---------------------------------------------------------------
// DESC : 증가 연산자 구현
// PARM :	1 . pString - CStringSet 객체의 문자열 데이터에 추가될 문자열 데이터
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::Append(LPCTSTR pString)
{
	return(operator+=(pString));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Append
//*---------------------------------------------------------------
// DESC : 증가 연산자 구현
// PARM :	1 . pString - CStringSet 객체의 문자열 데이터에 추가될 CStringSet 객체 (문자열 데이터)
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::Append(CStringSet& pString)
{
	return(operator+=(pString));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Append
//*---------------------------------------------------------------
// DESC : 증가 연산자 구현
// PARM :	1 . pString - CStringSet 객체의 문자열 데이터에 추가될 CStringSet 객체 포인터 (문자열 데이터)
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::Append(CStringSet* pString)
{
	return(operator+=(pString));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Tokenize
//*---------------------------------------------------------------
// DESC : 구분자 (문자)들을 경계로 부분으로 나눔
// PARM :	1 . lpszTokens - 구분자 (문자집합, \0으로 끝남)
//			2 . iStartPtr - 문자열 데이터 중 특정 위치
// RETV : 
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet CStringSet::Tokenize(LPCTSTR lpszTokens, INT& iStartPtr)
{
	CStringSet aString;
	INT nLength = GetLength();
	if (IsEmpty() || iStartPtr < 0 || iStartPtr >= nLength) {
		return aString;
	}

	std::set<TCHAR> setTokens;
	INT iIndex;

	if (!lpszTokens || lpszTokens[0] == _T('\0')) {
		setTokens.insert(DF_DEFAULT_NEWLINE_TOKEN_CHAR);
	}
	else {
		for (iIndex = 0 ; lpszTokens[iIndex] != _T('\0') ; ++iIndex) {
			setTokens.insert(lpszTokens[iIndex]);
		}
	}
	
	for (iIndex = iStartPtr ; iIndex < nLength ; ++iIndex) {
		if (setTokens.find(m_lpszStringBuffer[iIndex]) != setTokens.end()) {
			break;
		}
	}

	if (iStartPtr < iIndex) {
		LPTSTR aStringBuffer = aString.ReAlloc(iStartPtr+iIndex+1);
		if (aStringBuffer) {
			::memcpy(aStringBuffer, m_lpszStringBuffer+iStartPtr, (iIndex - iStartPtr)*sizeof(TCHAR));
			aStringBuffer[iIndex - iStartPtr] = _T('\0');
		}
	}
	iStartPtr = ++iIndex;
	return(aString);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Format
//*---------------------------------------------------------------
// DESC : 특정 문자열 형식의 값으로 치환
// PARM :	1 . lpszFormat - 특정 문자열 형식
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::Format(LPCTSTR lpszFormat, ...)
{
	if (!lpszFormat) {
		return;
	}

	TCHAR szFormat[DF_MAX_FORMAT_TEMPBUFFER_SIZE] = {0,};

	va_list lpArgPtr;
	INT iChrWritten;

	va_start(lpArgPtr, lpszFormat);
	iChrWritten = VSNPRINTF(szFormat, COUNT_OF(szFormat), lpszFormat, lpArgPtr);
	DN_ASSERT(0 < iChrWritten,	"Invalid!");
	va_end(lpArgPtr);
	
	INT iLength = (INT)_tcslen(szFormat);
	if (iLength > 0) {
		LPTSTR aStringBuffer = ReAlloc(iLength+1);
		if (aStringBuffer) {
			_tcsncpy(aStringBuffer, szFormat, GetSize());
		}
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::TrimLeft
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 왼쪽 공백을 제거
// PARM : N/A
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::TrimLeft()
{
	INT nLength = GetLength();
	if (nLength > 0) {
		INT iStartPtr = 0;
		for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
			if (m_lpszStringBuffer[iIndex] == _T(' ')) {
				iStartPtr = iIndex + 1;
			}
			else {
				break;
			}
		}
		if (iStartPtr > 1) {
			::memmove(m_lpszStringBuffer, m_lpszStringBuffer+iStartPtr, (nLength-iStartPtr)*sizeof(TCHAR));
			m_lpszStringBuffer[nLength-iStartPtr] = _T('\0');
		}
	}
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::TrimRight
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 오른쪽 공백을 제거
// PARM : N/A
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::TrimRight()
{
	INT nLength = GetLength();
	if (nLength > 0) {
		for (INT iIndex = nLength - 1 ; iIndex >= 0 ; --iIndex) {
			if (m_lpszStringBuffer[iIndex] == _T(' ')) {
				m_lpszStringBuffer[iIndex] = _T('\0');
			}
			else {
				break;
			}
		}
	}
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Trim
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 양쪽 공백을 제거
// PARM : N/A
// RETV : CStringSet 객체 자신의 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet& CStringSet::Trim()
{
	TrimLeft();
	TrimRight();
	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Mid
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 가운데 특정 범위의 문자열을 얻음
// PARM :	1 . nFirst - 문자열 데이터를 얻을 시작위치
//			2 . nCount - 얻어올 문자의 개수
// RETV : 얻은 문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet CStringSet::Mid(INT nFirst, INT nCount) const
{
	CStringSet aString;
	INT nLength = GetLength();
	if (nFirst >= 0 && 
		nCount > 0 &&
		!IsEmpty() &&
		nLength > nFirst
		) 
	{
		if (nFirst + nCount > nLength) {
			nCount = nLength - nFirst;
		}
		LPTSTR aStringBuffer1 = aString.ReAlloc(nCount+1);
		LPTSTR aStringBuffer2 = m_lpszStringBuffer;
		::memcpy(aStringBuffer1, aStringBuffer2+nFirst, nCount*sizeof(TCHAR));
		aStringBuffer1[nCount] = _T('\0');
	}
	return(aString);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Mid
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 가운데 특정 위치부터 끝까지의 문자열을 얻음
// PARM :	1 . nFirst - 문자열 데이터를 얻을 시작위치
// RETV : 얻은 문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet CStringSet::Mid(INT nFirst) const
{
	CStringSet aString;
	INT nLength = GetLength();
	INT nCount = 0;
	if (nFirst >= 0 && 
		!IsEmpty() &&
		nLength > nFirst
		) 
	{
		nCount = nLength - nFirst;
		LPTSTR aStringBuffer1 = aString.ReAlloc(nCount+1);
		LPTSTR aStringBuffer2 = m_lpszStringBuffer;
		::memcpy(aStringBuffer1, aStringBuffer2+nFirst, nCount*sizeof(TCHAR));
		aStringBuffer1[nCount] = _T('\0');
	}
	return(aString);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Left
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 왼쪽부터 특정 개수만큼 문자열을 얻음
// PARM :	1 . nCount - 얻어올 문자의 개수
// RETV : 얻은 문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet CStringSet::Left(INT nCount) const
{
	CStringSet aString;
	INT nLength = GetLength();
	if (!IsEmpty() &&
		nCount > 0
		) 
	{
		if (nCount > nLength) {
			nCount = nLength;
		}
		LPTSTR aStringBuffer1 = aString.ReAlloc(nCount+1);
		LPTSTR aStringBuffer2 = m_lpszStringBuffer;
		::memcpy(aStringBuffer1, aStringBuffer2, nCount*sizeof(TCHAR));
		aStringBuffer1[nCount] = _T('\0');
	}
	return(aString);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Right
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 오른쪽부터 특정 개수만큼 문자열을 얻음
// PARM :	1 . nCount - 얻어올 문자의 개수
// RETV : 얻은 문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
CStringSet CStringSet::Right(INT nCount) const
{
	CStringSet aString;
	INT nLength = GetLength();
	if (!IsEmpty() &&
		nCount > 0
		) 
	{
		if (nCount > nLength) {
			nCount = nLength;
		}
		LPTSTR aStringBuffer1 = aString.ReAlloc(nCount+1);
		LPTSTR aStringBuffer2 = m_lpszStringBuffer;
		::memcpy(aStringBuffer1, aStringBuffer2+nLength-nCount, nCount*sizeof(TCHAR));
		aStringBuffer1[nCount] = _T('\0');
	}
	return(aString);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::IsAlpha
//*---------------------------------------------------------------
// DESC : 문자열 데이터가 영문자인지 체크
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CStringSet::IsAlpha() const
{
	if (!m_lpszStringBuffer) {
		return FALSE;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return FALSE;
	}

	for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
		if (!_istalpha(m_lpszStringBuffer[iIndex])) {
			return FALSE;
		}
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::IsNumeric
//*---------------------------------------------------------------
// DESC : 문자열 데이터가 숫자인지 체크
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CStringSet::IsNumeric() const
{
	if (!m_lpszStringBuffer) {
		return FALSE;
	}

	BOOL bHasNumeric = FALSE;
	INT nLength = GetLength();
	if (nLength <= 0) {
		return FALSE;
	}

	for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
		if (iIndex == 0 && m_lpszStringBuffer[iIndex] == _T('-')) {
			continue;
		}
		if (!_istdigit(m_lpszStringBuffer[iIndex])) {
			return FALSE;
		}
		bHasNumeric = TRUE;
	}
	return bHasNumeric;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::IsNumeric
//*---------------------------------------------------------------
// DESC : 문자열 데이터가 숫자 (실수 포함) 인지 체크
// PARM :	1 . bIsRealNumber - 문자열 데이터가 실수인지 여부를 반환받음
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CStringSet::IsNumeric(BOOL &bIsRealNumber) const
{
	if (!m_lpszStringBuffer) {
		return FALSE;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return FALSE;
	}

	BOOL bHasNumeric = FALSE;
	INT nDotCount = 0;	//  소수점 ('.') 의 개수
	bIsRealNumber = FALSE;

	for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
		if (iIndex == 0 && m_lpszStringBuffer[iIndex] == _T('-')) {
			continue;
		}
		if (iIndex > 0 && m_lpszStringBuffer[iIndex] == _T('.')) {
			++nDotCount;
			continue;
		}
		if (!_istdigit(m_lpszStringBuffer[iIndex])) {
			return FALSE;
		}
		bHasNumeric = TRUE;
	}
	if (nDotCount > 1) {
		return FALSE;
	}
	if (nDotCount == 1) {
		bIsRealNumber = TRUE;
	}
	return bHasNumeric;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::IsAlNum
//*---------------------------------------------------------------
// DESC : 문자열 데이터가 영문자 혹은 숫자인지 체크
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CStringSet::IsAlNum() const
{
	if (!m_lpszStringBuffer) {
		return FALSE;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return FALSE;
	}

	for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
		if (!_istalnum(m_lpszStringBuffer[iIndex])) {
			return FALSE;
		}
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::IsAlNum
//*---------------------------------------------------------------
// DESC : 문자열 데이터가 영문자 혹은 숫자 (실수 포함) 인지 체크
// PARM :	1 . bIsRealNumber - 문자열 데이터가 실수인지 여부를 반환받음
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CStringSet::IsAlNum(BOOL &bIsRealNumber) const
{
	if (!m_lpszStringBuffer) {
		return FALSE;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return FALSE;
	}

	INT nDotCount = 0;	//  소수점 ('.') 의 개수
	bIsRealNumber = FALSE;

	for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
		if (_istalnum(m_lpszStringBuffer[iIndex])) {
			continue;
		}
		if (m_lpszStringBuffer[iIndex] == _T('.')) {
			nDotCount++;
			continue;
		}
		return FALSE;
	}
	if (nDotCount > 1) {
		return FALSE;
	}
	if (nDotCount == 1) {
		bIsRealNumber = TRUE;
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Compare
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 동등, 대소관계를 비교
// PARM :	1 . pString - 비교할 문자열
// RETV : 0:같음/음수:작음/양수:큼/_NLSCMPERROR:에러
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::Compare(LPCTSTR pString) const
{
	return _tcscmp(m_lpszStringBuffer, pString);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::CompareNoCase
//*---------------------------------------------------------------
// DESC : 객체의 문자열 데이터와 특정 문자열의 동등, 대소관계를 비교 (대소문자 구분없음)
// PARM :	1 . pString - 비교할 문자열
// RETV : 0:같음/음수:작음/양수:큼/_NLSCMPERROR:에러
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::CompareNoCase(LPCTSTR pString) const
{
	return _tcsnicmp(m_lpszStringBuffer, pString, GetLength());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::MakeUpper
//*---------------------------------------------------------------
// DESC : 문자열 데이터를 대문자로 변환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::MakeUpper()
{
	if (!m_lpszStringBuffer) {
		return;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return;
	}

	for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
		m_lpszStringBuffer[iIndex] = (TCHAR)_totupper(m_lpszStringBuffer[iIndex]);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::MakeLower
//*---------------------------------------------------------------
// DESC : 문자열 데이터를 소문자로 변환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::MakeLower()
{
	if (!m_lpszStringBuffer) {
		return;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return;
	}

	for (INT iIndex = 0 ; iIndex < nLength ; ++iIndex) {
		m_lpszStringBuffer[iIndex] = (TCHAR)_totlower(m_lpszStringBuffer[iIndex]);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::MakeReverse
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 좌우를 뒤집음
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CStringSet::MakeReverse()
{
	if (!m_lpszStringBuffer) {
		return;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return;
	}

	std::reverse(&m_lpszStringBuffer[0], &m_lpszStringBuffer[nLength]);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Find
//*---------------------------------------------------------------
// DESC : 문자열 데이터 중 특정 문자의 위치를 찾아서 반환
// PARM :	1 . cChar - 찾을 문자
// RETV : 찾은 문자의 위치 (없으면 -1 반환)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::Find(TCHAR cChar) const
{
	if (!m_lpszStringBuffer) {
		return -1;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return -1;
	}

	TCHAR *lpcChar = _tcschr(m_lpszStringBuffer, cChar);
	if (lpcChar) {
		return (INT)(lpcChar - m_lpszStringBuffer);
	}
	return -1;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::ReverseFind
//*---------------------------------------------------------------
// DESC : 문자열 데이터 중 특정 문자의 위치를 뒤에서부터 찾아서 반환
// PARM :	1 . cChar - 
// RETV : 찾은 문자의 위치 (없으면 -1 반환)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::ReverseFind(TCHAR cChar) const
{
	if (!m_lpszStringBuffer) {
		return -1;
	}

	INT nLength = GetLength();
	if (nLength <= 0) {
		return -1;
	}

	TCHAR *lpcChar = _tcsrchr(m_lpszStringBuffer, cChar);
	if (lpcChar) {
		return (INT)(lpcChar - m_lpszStringBuffer);
	}
	return -1;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Find
//*---------------------------------------------------------------
// DESC : 문자열 데이터 중 특정 문자의 위치를 특정 위치부터 시작하여 찾아서 반환
// PARM :	1 . cChar - 찾을 문자
//			2 . nStart - 문자을 찾기 시작할 위치
// RETV : 찾은 문자의 위치 (없으면 -1 반환)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::Find(TCHAR cChar, INT nStart) const
{
	if (!m_lpszStringBuffer) {
		return -1;
	}

	INT nLength = GetLength();
	if (nLength <= 0 || nStart < 0 || nLength-1 < nStart) {
		return -1;
	}

	TCHAR *lpcChar = _tcschr(m_lpszStringBuffer+nStart, cChar);
	if (lpcChar) {
		return (INT)(lpcChar - m_lpszStringBuffer);
	}
	return -1;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::FindOneOf
//*---------------------------------------------------------------
// DESC : 문자열 데이터 중 특정 문자집합에 속하는 문자의 위치를 찾아서 반환
// PARM :	1 . lpszCharSet - 찾을 문자집합
// RETV : 찾은 문자의 위치 (없으면 -1 반환)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::FindOneOf(LPCTSTR lpszCharSet) const
{
	if (!m_lpszStringBuffer || !lpszCharSet) {
		return -1;
	}

	INT nLength1 = GetLength();
	if (nLength1 <= 0) {
		return -1;
	}
	INT nLength2 = (INT)_tcslen(lpszCharSet);
	if (nLength2 <= 0) {
		return -1;
	}

	std::set<TCHAR> setTokens;
	for (INT iIndex = 0 ; iIndex < nLength2 ; ++iIndex) {
		setTokens.insert(lpszCharSet[iIndex]);
	}

	for (INT iIndex = 0 ; iIndex < nLength1 ; ++iIndex) {
		if (setTokens.find(m_lpszStringBuffer[iIndex]) != setTokens.end()) {
			return iIndex;
		}
	}
	return -1;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::FindOneOf
//*---------------------------------------------------------------
// DESC : 문자열 데이터 중 특정 문자집합에 속하는 문자의 위치를 찾아서 반환 (특정 위치부터 시작)
// PARM :	1 . lpszCharSet - 찾을 문자집합
//			2 . nStart - 찾기 시작할 위치
// RETV : 찾은 문자의 위치 (없으면 -1 반환)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::FindOneOf(LPCTSTR lpszCharSet, INT nStart) const
{
	if (!m_lpszStringBuffer || !lpszCharSet) {
		return -1;
	}

	INT nLength1 = GetLength();
	if (nLength1 <= 0 || nStart < 0 || nLength1-1 < nStart) {
		return -1;
	}
	INT nLength2 = (INT)_tcslen(lpszCharSet);
	if (nLength2 <= 0) {
		return -1;
	}

	std::set<TCHAR> setTokens;
	for (INT iIndex = 0 ; iIndex < nLength2 ; ++iIndex) {
		setTokens.insert(lpszCharSet[iIndex]);
	}

	for (INT iIndex = nStart ; iIndex < nLength1 ; ++iIndex) {
		if (setTokens.find(m_lpszStringBuffer[iIndex]) != setTokens.end()) {
			return iIndex;
		}
	}
	return -1;
}



//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Find
//*---------------------------------------------------------------
// DESC : 문자열 데이터 중 특정 문자열의 위치를 찾아서 반환
// PARM :	1 . lpszSub - 찾을 문자열
// RETV : 찾은 문자의 위치 (없으면 -1 반환)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::Find(LPCTSTR lpszSub) const
{
	if (!m_lpszStringBuffer || !lpszSub) {
		return -1;
	}

	INT nLength1 = GetLength();
	if (nLength1 <= 0) {
		return -1;
	}
	INT nLength2 = (INT)_tcslen(lpszSub);
	if (nLength2 <= 0) {
		return -1;
	}
	
	LPTSTR lpszStr = _tcsstr(m_lpszStringBuffer, lpszSub);
	if (lpszStr) {
		return (INT)(lpszStr - m_lpszStringBuffer);
	}
	return -1;	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CStringSet::Find
//*---------------------------------------------------------------
// DESC : 문자열 데이터 중 특정 문자열의 위치를 특정 위치부처 시작하여 찾아서 반환
// PARM :	1 . lpszSub - 찾을 문자열
//			2 . nStart - 문자열을 찾기 시작할 위치
// RETV : 찾은 문자의 위치 (없으면 -1 반환)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CStringSet::Find(LPCTSTR lpszSub, INT nStart) const
{
	if (!m_lpszStringBuffer || !lpszSub) {
		return -1;
	}

	INT nLength1 = GetLength();
	if (nLength1 <= 0 || nStart < 0 || nLength1-1 < nStart) {
		return -1;
	}
	INT nLength2 = (INT)_tcslen(lpszSub);
	if (nLength2 <= 0) {
		return -1;
	}

	LPTSTR lpszStr = _tcsstr(m_lpszStringBuffer+nStart, lpszSub);
	if (lpszStr) {
		return (INT)(lpszStr - m_lpszStringBuffer);
	}
	return -1;	
}

