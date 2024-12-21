/*//===============================================================

	<< CONSOLE OUTPUT >>

	PRGM : B4nFter

	FILE : CONSOLEOUTPUT.H
	DESC : Console 출력(전용) 클래스
	INIT BUILT DATE : 2001. 06. 16
	LAST BUILT DATE : 2006. 01. 05

	Copyrightⓒ 2002~2007 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"
#include "CRITICALSECTION.H"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define	MAX_CONSOLE_OT_TITLE_CHAR			(256)


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CConsoleOutput
//*---------------------------------------------------------------
// DESC : 콘솔 출력 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CConsoleOutput
{
public:
	enum EF_FGCOLOR {
		EV_FGCL_BLACK			= 0x0000,
		EV_FGCL_DARKBLUE		= 0x0001,
		EV_FGCL_DARKGREEN		= 0x0002,
		EV_FGCL_DARKCYAN		= 0x0003,
		EV_FGCL_DARKRED			= 0x0004,
		EV_FGCL_DARKMAGENTA		= 0x0005,
		EV_FGCL_DARKYELLOW		= 0x0006,
		EV_FGCL_GREY			= 0x0007,
		EV_FGCL_DARKGREY		= 0x0008,
		EV_FGCL_BLUE			= 0x0009,
		EV_FGCL_GREEN			= 0x000A,
		EV_FGCL_CYAN			= 0x000B,
		EV_FGCL_RED				= 0x000C,
		EV_FGCL_MAGENTA			= 0x000D,
		EV_FGCL_YELLOW			= 0x000E,
		EV_FGCL_WHITE			= 0x000F,
	};
	enum EF_BGCOLOR {
		EV_BGCL_BLACK			= 0x0000,
		EV_BGCL_DARKBLUE		= 0x0010,
		EV_BGCL_DARKGREEN		= 0x0020,
		EV_BGCL_DARKCYAN		= 0x0030,
		EV_BGCL_DARKRED			= 0x0040,
		EV_BGCL_DARKMAGENTA		= 0x0050,
		EV_BGCL_DARKYELLOW		= 0x0060,
		EV_BGCL_GREY			= 0x0070,
		EV_BGCL_DARKGREY		= 0x0080,
		EV_BGCL_BLUE			= 0x0090,
		EV_BGCL_GREEN			= 0x00A0,
		EV_BGCL_CYAN			= 0x00B0,
		EV_BGCL_RED				= 0x00C0,
		EV_BGCL_MAGENTA			= 0x00D0,
		EV_BGCL_YELLOW			= 0x00E0,
		EV_BGCL_WHITE			= 0x00F0,
	};

private:
	HANDLE m_hConsoleOutput;
	HWND m_hConsoleWindow;

	EF_FGCOLOR m_eConsoleFgColor;	// 콘솔의 전면색
	EF_BGCOLOR m_eConsoleBgColor;	// 콘솔의 후면색

	TCHAR m_szConsoleTitle[MAX_CONSOLE_OT_TITLE_CHAR*sizeof(TCHAR)];

	TCHAR m_szBuffer[1024];
	CSyncLock m_Lock;

public:
	inline CConsoleOutput();
	inline ~CConsoleOutput();

	inline BOOL Initialize(LPCTSTR lpszConsoleTitle = NULL);
	inline BOOL Finalize();

	inline VOID Write(LPCTSTR lpszFormat, ...);
	inline VOID ShowWindow(BOOL bShow);
	inline VOID SetConsoleTitle(LPCTSTR lpszConsoleTitle);

	inline VOID SetColor(EF_FGCOLOR eFgColor, EF_BGCOLOR eBgColor);
	inline VOID SetFgColor(EF_FGCOLOR eFgColor);
	inline VOID SetBgColor(EF_BGCOLOR eBgColor);
};


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::CConsoleOutput
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CConsoleOutput::CConsoleOutput()
{
	m_hConsoleOutput = INVALID_HANDLE_VALUE;
	m_hConsoleWindow = NULL;
	m_eConsoleFgColor = EV_FGCL_GREY;
	m_eConsoleBgColor = EV_BGCL_BLACK;
	::memset(m_szConsoleTitle, 0, sizeof(m_szConsoleTitle));

/*
	if (NOERROR != m_Lock.Open()) {
//		DN_ASSERT(0,	"Failed!");
	}
*/
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::~CConsoleOutput
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CConsoleOutput::~CConsoleOutput()
{
	Finalize();

//	m_Lock.Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::Initialize
//*---------------------------------------------------------------
// DESC : 객체자원 초기화
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CConsoleOutput::Initialize(LPCTSTR lpszConsoleTitle)
{
	if (m_hConsoleOutput != INVALID_HANDLE_VALUE) {
		DN_TRACE(_T("m_hConsoleOutput != INVALID_HANDLE_VALUE - CConsoleOutput::Initialize()"));
		return FALSE;
	}

#if !defined(_CONSOLE)
	if (!::AllocConsole()) {
//		DN_MESSAGEBOX(HWND_DESKTOP, MB_ICONERROR, _T("ERROR"), _T("!AllocConsole() - CConsoleOutput::Initialize()"));
		return FALSE;
	}
#endif	// _CONSOLE

	m_hConsoleOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (m_hConsoleOutput == INVALID_HANDLE_VALUE) {
//		DN_MESSAGEBOX(HWND_DESKTOP, MB_ICONERROR, _T("ERROR"), _T("m_hConsoleOutput == INVALID_HANDLE_VALUE - CConsoleOutput::Initialize()"));
		return FALSE;
	}

	// 콘솔 윈도우핸들을 얻음
	m_hConsoleWindow = ::GetConsoleWindow();

	// 콘솔 제목 지정
	if (lpszConsoleTitle) {
		::SetConsoleTitle(lpszConsoleTitle);
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::Finalize
//*---------------------------------------------------------------
// DESC : 객체자원 해제
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CConsoleOutput::Finalize()
{
	if (m_hConsoleOutput != INVALID_HANDLE_VALUE) {
#if !defined(_CONSOLE)
		if (!::FreeConsole()) {
//			DN_MESSAGEBOX(HWND_DESKTOP, MB_ICONERROR, _T("ERROR"), _T("!FreeConsole() - - CConsoleOutput::FreeConsole()"));
			return FALSE;
		}
#endif	// _CONSOLE

		m_hConsoleOutput = INVALID_HANDLE_VALUE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleUserInterface::Write
//*---------------------------------------------------------------
// DESC : 콘솔화면에 텍스트 출력
// PARM :	1 . lpszFormat - 콘솔화면에 출력할 텍스트의 포맷
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CConsoleOutput::Write(LPCTSTR lpszFormat, ...)
{
	va_list szArgs;
	int iTextCount;
	DWORD dwWrittenBytes;

	ScopeLock<CSyncLock> aLockAuto(&m_Lock);

	va_start(szArgs, lpszFormat);
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	iTextCount = _vsntprintf_s(m_szBuffer, COUNT_OF(m_szBuffer), lpszFormat, szArgs);
#else	// #if	(_MSC_VER >= 1400)
	#pragma warning (disable:4995)
	iTextCount = _vsntprintf(m_szBuffer, COUNT_OF(m_szBuffer), lpszFormat, szArgs);
	#pragma warning (default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	va_end(szArgs);

	::WriteConsole(m_hConsoleOutput, m_szBuffer, iTextCount, &dwWrittenBytes, NULL);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::ShowWindow
//*---------------------------------------------------------------
// DESC : show or hide console window
// PARM :	1 . bShow - TRUE - 성공 / FALSE - 실패
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CConsoleOutput::ShowWindow(BOOL bShow)
{
	if (m_hConsoleWindow) {
		::ShowWindow(m_hConsoleWindow, bShow);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::SetConsoleTitle
//*---------------------------------------------------------------
// DESC : set or change title of console window
// PARM :	1 . lpszConsoleTitle - title of console window
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CConsoleOutput::SetConsoleTitle(LPCTSTR lpszConsoleTitle)
{
	if (lpszConsoleTitle != NULL && lstrcmp(lpszConsoleTitle, _T('\0'))) {
		// lstrcpyn() 은 TCHAR를 처리하므로 유니코드에서 글자1개는 2바이트이다.
		STRNCPY(m_szConsoleTitle, _countof(m_szConsoleTitle), lpszConsoleTitle);
//		_tcsncpy(m_szConsoleTitle, lpszConsoleTitle, COUNT_OF(m_szConsoleTitle));
		::SetConsoleTitle(m_szConsoleTitle);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::SetColor
//*---------------------------------------------------------------
// DESC : set or change console text color, foreground and background
// PARM :	1 . eFgColor - foreground console text color
//			2 . eBgColor - background console text color
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CConsoleOutput::SetColor(EF_FGCOLOR eFgColor, EF_BGCOLOR eBgColor)
{
	m_eConsoleFgColor = eFgColor;
	m_eConsoleBgColor = eBgColor;
	if (m_hConsoleOutput != INVALID_HANDLE_VALUE) {
		::SetConsoleTextAttribute(m_hConsoleOutput, (WORD)(eFgColor | eBgColor));
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::SetFgColor
//*---------------------------------------------------------------
// DESC : set or change console text color, foreground
// PARM :	1 . eFgColor - foreground console text color
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CConsoleOutput::SetFgColor(EF_FGCOLOR eFgColor)
{
	m_eConsoleFgColor = eFgColor;
	if (m_hConsoleOutput != INVALID_HANDLE_VALUE) {
		::SetConsoleTextAttribute(m_hConsoleOutput, (WORD)(eFgColor | m_eConsoleBgColor));
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CConsoleOutput::SetBgColor
//*---------------------------------------------------------------
// DESC : set or change console text color, background
// PARM :	1 . eBgColor - background console text color
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CConsoleOutput::SetBgColor(EF_BGCOLOR eBgColor)
{
	m_eConsoleBgColor = eBgColor;
	if (m_hConsoleOutput != INVALID_HANDLE_VALUE) {
		::SetConsoleTextAttribute(m_hConsoleOutput, (WORD)(m_eConsoleFgColor | eBgColor));
	}	
}

