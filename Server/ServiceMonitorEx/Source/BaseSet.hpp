/*//===============================================================

	<< BASE-SET >>

	PRGM : milkji, B4nFter

	FILE : BASESET.HPP
	DESC : 프로젝트의 기본 세팅을 체크하기 위한 클래스
	INIT BUILT DATE : 2007. 03. 13
	LAST BUILT DATE : 2007. 03. 13

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

// P.S.> SYSBASEBASE.H 에 의해 참조됨 (따라서 본 헤더는 WINDOWS.H 등 기본헤더 제외시킴)


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CBaseSet
//*---------------------------------------------------------------
// DESC : 프로젝트의 기본 세팅을 체크하기 위한 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------

class CBaseSet
{
public:
	static BOOL IsDebug();
	static BOOL IsUnicode();
	static BOOL IsMultiThread();
	static BOOL IsLittleEndian();
	static BOOL IsWindowStationVisible();
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::
//*---------------------------------------------------------------
// DESC : 현재 프로젝트 구성이 _DEBUG 인지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsDebug()
{
	return(MODE_CMP(TRUE,FALSE));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsUnicode
//*---------------------------------------------------------------
// DESC : 현재 프로젝트의 언어 옵션이 _UNICODE 인지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsUnicode()
{
	return(MODE_CHARSET(FALSE, TRUE));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsMultiThread
//*---------------------------------------------------------------
// DESC : 현재 프로젝트의 코드생성 옵션이 _MT 인지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsMultiThread()
{
	return(MODE_MT(FALSE,TRUE));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsLittleEndian
//*---------------------------------------------------------------
// DESC : 현재 플랫폼이 little-endian 기반인지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsLittleEndian()
{
	const unsigned long	ulHost = 0x12345678;
	return(::htonl(ulHost) != ulHost);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsWindowStationVisible
//*---------------------------------------------------------------
// DESC : 현재 프로세스의 윈도우 스테이션이 USER-INTERACTIVE 한지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsWindowStationVisible()
{
	HWINSTA hWindowStation = ::GetProcessWindowStation();
	if (hWindowStation != NULL) {
		USEROBJECTFLAGS	stUOF;
		DWORD dwNeed = 0;
		if (::GetUserObjectInformation(hWindowStation, UOI_FLAGS, &stUOF, sizeof(USEROBJECTFLAGS), &dwNeed) == TRUE) {
			return ((WSF_VISIBLE & stUOF.dwFlags) == WSF_VISIBLE);
		}
	}
	return FALSE;
}

