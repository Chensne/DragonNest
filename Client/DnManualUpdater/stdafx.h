// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <Windows.h>
#include <vector>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include <utility>
#include <functional>
#include <afxdhtml.h>
#include "LogWnd.h"
#include "DnString.h"

const int __MAX_PATH = 8192;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#define UPDATER_TEMP_LOCAL_DIR "update" 

//#define  PRE_ADD_MAKE_PAK	//자체적으로 팩만들기 //추후추가
//#define  PRE_DELETE_VERSION


#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }
#endif // SAFE_DELETE

#ifndef SAFE_DELETEA
#define SAFE_DELETEA(x) if(x) { delete []x; x=NULL; }
#endif // SAFE_DELETEA

//작업 define 


bool ToMultiString(IN std::wstring& wstr, OUT std::string& str );
bool ToWideString(IN std::string& str, OUT std::wstring& wstr );

bool ToMultiString(IN WCHAR* wstr, OUT std::string& str );
bool ToWideString(IN CHAR* str, OUT std::wstring& wstr );

extern bool g_bSilenceMode;


#if defined(_THA) || defined(_IDN)
#define	USE_SERVER_VERSION_CHECK
extern bool g_bPassCheckVersion;
#endif // _THA or _IDN