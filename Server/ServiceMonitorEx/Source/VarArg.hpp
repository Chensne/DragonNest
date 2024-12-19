/*//===============================================================

	<< VARIABLE-ARGUMENT >>

	PRGM : milkji, B4nFter

	FILE : VARARG.HPP
	DESC : 가변 인자들로 문자열을 구성해주는 클래스
	INIT BUILT DATE : 2007. 09. 06
	LAST BUILT DATE : 2007. 09. 07

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"
#include <CSTDARG>		// std::var_list


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#if defined(_UNICODE)
#define CVarArg		CVarArgW
#else	// _UNICODE
#define CVarArg		CVarArgA
#endif	// _UNICODE


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CVarArgA
//*---------------------------------------------------------------
// DESC : 가변 인자들로 문자열을 구성해주는 클래스 (멀티바이트)
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
class CVarArgA
{
	DISABLE_COPY_ASSIGN(CVarArgA)	// 객체간 복사, 대입 금지

public:
	explicit CVarArgA(const CHAR* lpFormat, ...);

	operator const CHAR*() const;
	const CHAR* GetMsg() const;

private:
	CHAR m_cBuffer[TP_MSGSIZE];
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CVarArgW
//*---------------------------------------------------------------
// DESC : 가변 인자들로 문자열을 구성해주는 클래스 (유니코드)
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
class CVarArgW
{
	DISABLE_COPY_ASSIGN(CVarArgW)	// 객체간 복사, 대입 금지

public:
	explicit CVarArgW(const WCHAR* lpFormat, ...);

	operator const WCHAR*() const;
	const WCHAR* GetMsg() const;

private:
	WCHAR m_cBuffer[TP_MSGSIZE];
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CVarArgA::CVarArgA
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpFormat - 출력 포맷
//			2 . ... - 가변인자
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
CVarArgA<TP_MSGSIZE>::CVarArgA(const CHAR* lpFormat, ...)
{
	CHECK_STRINGA(lpFormat, INFINITE);
	
	std::va_list aArgs;
	va_start(aArgs, lpFormat);
	VSNPRINTFA(m_cBuffer, COUNT_OF(m_cBuffer), lpFormat, aArgs);
	va_end(aArgs);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CVarArgA::operator CHAR*
//*---------------------------------------------------------------
// DESC : 객체를 CHAR* 형으로 치환
// PARM : N/A
// RETV : 텍스트 버퍼 포인터
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
inline CVarArgA<TP_MSGSIZE>::operator const CHAR*() const
{
	return (GetMsg());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CVarArgA::GetMsg
//*---------------------------------------------------------------
// DESC : 객체의 텍스트 버퍼 반환
// PARM : N/A
// RETV : 텍스트 버퍼 포인터
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
inline const CHAR* CVarArgA<TP_MSGSIZE>::GetMsg() const
{
	return (m_cBuffer);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CVarArgW::CVarArgW
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpFormat - 출력 포맷
//			2 . ... - 가변인자
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
CVarArgW<TP_MSGSIZE>::CVarArgW(const WCHAR* lpFormat, ...)
{
	CHECK_STRINGW(lpFormat, INFINITE);
	
	std::va_list aArgs;
	va_start(aArgs, lpFormat);
	VSNPRINTFW(m_cBuffer, COUNT_OF(m_cBuffer), lpFormat, aArgs);
	va_end(aArgs);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CVarArgW::operator WCHAR*
//*---------------------------------------------------------------
// DESC : 객체를 WCHAR* 형으로 치환
// PARM : N/A
// RETV : 텍스트 버퍼 포인터
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
inline CVarArgW<TP_MSGSIZE>::operator const WCHAR*() const
{
	return (GetMsg());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CVarArgW::GetMsg
//*---------------------------------------------------------------
// DESC : 객체의 텍스트 버퍼 반환
// PARM : N/A
// RETV : 텍스트 버퍼 포인터
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
template<DWORD TP_MSGSIZE>
inline const WCHAR* CVarArgW<TP_MSGSIZE>::GetMsg() const
{
	return (m_cBuffer);
}

