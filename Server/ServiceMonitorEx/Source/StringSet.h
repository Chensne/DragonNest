/*//===============================================================

	<< STRING >>

	PRGM : B4nFter

	FILE : STRINGSET.H, STRINGSET.CPP
	DESC : 문자열 데이터 클래스 (동적)
	INIT BUILT DATE : 2006. 10. 17
	LAST BUILT DATE : 2007. 11. 29

	P.S.> 
		- GetA / GetW 는 힙메모리에 할당된 문자열을 반환하므로 NULL 이 아닐 경우 사용 후에 반드시 ::free() 로 제거해야 함

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define	DF_DEFAULT_NEWLINE_TOKEN_CHAR		_T('\n')
#define	DF_DEFAULT_NEWLINE_TOKEN_STRING		_T("\n")
#define	DF_MAX_FORMAT_TEMPBUFFER_SIZE		(2048)


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : STRUCT
// NAME : STRINGHEADER
//*---------------------------------------------------------------
// DESC : 문자열 데이터의 헤더정보 구조체
// PRGM : B4nFter
//*---------------------------------------------------------------
typedef struct
{
	INT	m_iSize;		// 바이트 단위가 아닌 TCHAR 단위
	INT m_iLinePtr;		// 바이트 단위가 아닌 TCHAR 단위
} STRINGHEADER, *LPSTRINGHEADER;


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CStringSet
//*---------------------------------------------------------------
// DESC : 문자열 데이터 클래스 (동적)
// PRGM : B4nFter
//*---------------------------------------------------------------
class CStringSet
{
private:
	LPTSTR m_lpszStringBuffer;

public:
	CStringSet();
	CStringSet(LPCTSTR pString);
	CStringSet(const CStringSet& pString);
	CStringSet(const CStringSet* pString);
	~CStringSet();

	BOOL IsNull() const;
	BOOL IsEmpty() const;
	VOID Empty();

	LPSTRINGHEADER GetHeader() const;
	INT GetLength() const;
	INT GetSize() const;

	INT GetLinePtr() const;
	VOID SetLinePtr(INT iLinePtr);

	LPCTSTR Get() const;
	LPCSTR GetA() const;
	LPCWSTR GetW() const;
	LPCTSTR Get(INT nStart) const;
	LPCSTR GetA(INT nStart) const;
	LPCWSTR GetW(INT nStart) const;
	CStringSet GetLine();

	VOID Reset();
	VOID ResetLinePtr();

	TCHAR GetAt(INT nIndex) const;
	VOID SetAt(INT nIndex, TCHAR cChar);
	TCHAR operator[](INT nIndex) const;

	CStringSet& Set(LPCTSTR pString);
	CStringSet& Set(CStringSet& pString);
	CStringSet& Set(CStringSet* pString);
	CStringSet& Append(LPCTSTR pString);
	CStringSet& Append(CStringSet& pString);
	CStringSet& Append(CStringSet* pString);
	operator LPCTSTR() const;

	// 대입 연산
	CStringSet& operator=(LPCTSTR pString);
	CStringSet& operator=(CStringSet& pString);
	CStringSet& operator=(CStringSet* pString);
	CStringSet& operator+=(LPCTSTR pString);
	CStringSet& operator+=(CStringSet& pString);
	CStringSet& operator+=(CStringSet* pString);

	// 비교 연산 (외부 의존도를 줄이기 위해 std::rel_ops 사용하지 않고 모두 구현)
	bool operator==(LPCTSTR pString) const;
	bool operator==(const CStringSet& pString) const;
	bool operator==(const CStringSet* pString) const;
	bool operator!=(LPCTSTR pString) const;
	bool operator!=(const CStringSet& pString) const;
	bool operator!=(const CStringSet* pString) const;
	bool operator<(LPCTSTR pString) const;
	bool operator<(const CStringSet& pString) const;
	bool operator<(const CStringSet* pString) const;
	bool operator>(LPCTSTR pString) const;
	bool operator>(const CStringSet& pString) const;
	bool operator>(const CStringSet* pString) const;
	bool operator<=(LPCTSTR pString) const;
	bool operator<=(const CStringSet& pString) const;
	bool operator<=(const CStringSet* pString) const;
	bool operator>=(LPCTSTR pString) const;
	bool operator>=(const CStringSet& pString) const;
	bool operator>=(const CStringSet* pString) const;

	CStringSet Tokenize(LPCTSTR lpszTokens, INT& iStartPtr);
	VOID Format(LPCTSTR lpszFormat, ...);

	CStringSet& TrimLeft();
	CStringSet& TrimRight();
	CStringSet& Trim();

	CStringSet Mid(INT nFirst, INT nCount) const;
	CStringSet Mid(INT nFirst) const;
	CStringSet Left(INT nCount) const;
	CStringSet Right(INT nCount) const;

	BOOL IsAlpha() const;
	BOOL IsNumeric() const;
	BOOL IsNumeric(BOOL &bIsRealNumber) const;
	BOOL IsAlNum() const;
	BOOL IsAlNum(BOOL &bIsRealNumber) const;

	INT Compare(LPCTSTR pString) const;
	INT CompareNoCase(LPCTSTR pString) const;

	VOID MakeUpper();
	VOID MakeLower();
	VOID MakeReverse();

	INT Find(TCHAR cChar) const;
	INT Find(TCHAR cChar, INT nStart) const;
	INT Find(LPCTSTR lpszSub) const;
	INT Find(LPCTSTR lpszSub, INT nStart) const;
	INT FindOneOf(LPCTSTR lpszCharSet) const;
	INT FindOneOf(LPCTSTR lpszCharSet, INT nStart) const;
	INT ReverseFind(TCHAR cChar) const;

//private:
	LPTSTR ReAlloc(INT iNewSize, BOOL bForceTo = FALSE);

};

