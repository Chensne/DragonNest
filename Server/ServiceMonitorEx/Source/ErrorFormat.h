/*//===============================================================

	<< ERROR FORMAT >>

	PRGM : B4nFter

	FILE : ERRORFORMAT.H, ERRORFORMAT.CPP
	DESC : 에러 메시지 관리 클래스
	INIT BUILT DATE : 2005. 12. 27
	LAST BUILT DATE : 2005. 12. 31

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

#define DF_ERRMSG_SYSERRSIZE		(1024)
#define DF_ERRMSG_TOTERRSIZE		(2048)


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CErrorFormat
//*---------------------------------------------------------------
// DESC : 에러메시지를 해석, 저장
// PRGM : B4nFter
//*---------------------------------------------------------------
class CErrorFormat
{
private:
	INT m_iErrorCode;
	TCHAR m_szSystemErrorMessage[DF_ERRMSG_SYSERRSIZE+1];
	TCHAR m_szTotalErrorMessage[DF_ERRMSG_TOTERRSIZE+1];
	static HMODULE m_hNetMsg;

public:
	CErrorFormat();
	~CErrorFormat();

	VOID Clear() { 
		m_szSystemErrorMessage[0] = _T('\0'); 
		m_szTotalErrorMessage[0] = _T('\0'); 
		m_iErrorCode = 0; 
	}

	CONST LPTSTR Write(INT iErrorCode, LPCTSTR lpszErrorMessage = NULL);
	CONST LPTSTR Write(LPCTSTR lpszErrorMessage);

	CONST LPTSTR Read() { return m_szTotalErrorMessage; }
	INT GetErrorCode() { return m_iErrorCode; }
};

