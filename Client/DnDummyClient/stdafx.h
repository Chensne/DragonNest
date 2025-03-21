// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4018 )

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

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <winsock2.h>
#include <windows.h>


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>

// stl
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <hash_map>
#include <algorithm>
#include <string>


#define PRE_USE_TLS_FOR_RANDOM
#define PRE_PARTY_RENEW_THIRD


// tinyxml 관련, 없으면 common/tinyxml 에서 받으세요.
#include "../../Common/tinyxml/tinyxml.h"
#include "../../Common/DNDefine.h"
/*
#ifdef _DEBUG
#pragma comment(lib, "../../Common/tinyxml/Debug_STL/tinyxmld_STL.lib")
#pragma comment(lib, "../../Common/lua/lua51_Debug(MTd).lib")
#else
#pragma comment(lib, "../../Common/tinyxml/Release_STL/tinyxml_STL.lib")
#pragma comment(lib, "../../Common/lua/lua51_Release(MT).lib")
#endif 
*/
#ifndef TIXML_USE_STL
	#define TIXML_USE_STL
#endif 



// wxWidgets
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include <string>

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/aui/aui.h"
#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/imagpng.h"
#include "wx/wxhtml.h"
#include "wx/statline.h"
#include "wx/artprov.h"
#include "wx/datstrm.h"
#include "wx/event.h"

// 프로퍼티 
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/propdev.h"
#include "wx/propgrid/props.h"
#include "wx/propgrid/advprops.h"

#include "DNPacket.h"
#include "DNCommonDef.h"
#include "EtMathWrapperD3D.h"

// TODO: reference additional headers your program requires here

#pragma comment(lib,"ws2_32.lib")

// DirectX Lib 
#pragma comment(lib,"d3d9.lib")
#ifdef _DEBUG
	#pragma comment(lib,"d3dx9d.lib")
#else 
	#pragma comment(lib,"d3dx9.lib")
#endif // _DEBUG

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"rpcrt4.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"odbc32.lib")
#pragma comment(lib,"winmm.lib")

#ifdef _DEBUG
	#pragma comment(lib,"wxmsw28ud_aui.lib")
	#pragma comment(lib,"wxmsw28ud_core.lib")
	#pragma comment(lib,"wxmsw28ud_adv.lib")
	#pragma comment(lib,"wxbase28ud.lib")
	// 프로퍼티
	#pragma comment(lib,"wxcode_msw28ud_propgrid.lib")

#else
	#pragma comment(lib,"wxmsw28u_aui.lib")
	#pragma comment(lib,"wxmsw28u_core.lib")
	#pragma comment(lib,"wxmsw28u_adv.lib")
	#pragma comment(lib,"wxbase28u.lib")
	// 프로퍼티
	#pragma comment(lib,"wxcode_msw28u_propgrid.lib")

#endif



typedef std::basic_string< TCHAR > tstring;


void NxTrace( LPCTSTR szMsg, ... );

// 스트링 관련 유틸.. 걍 귀찮아서 여기 붙여넣자.
bool ToMultiString(IN std::wstring& wstr, OUT std::string& str );
bool ToWideString(IN std::string& str, OUT std::wstring& wstr );

bool ToMultiString(IN WCHAR* wstr, OUT std::string& str );
bool ToWideString(IN CHAR* str, OUT std::wstring& wstr );

// 소문자로 변환
void ToLowerA(std::string& str);
void ToLowerW(std::wstring& str);

// 대문자로 변환
void ToUpperA(std::string& str);
void ToUpperW(std::wstring& str);

std::wstring FormatW( const WCHAR* fmt, ... );
std::string FormatA( const CHAR* fmt, ... );

std::wstring GetTimeString(time_t time);
void RemoveStringA(std::string& str, std::string& del);
void TokenizeW( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters /* = L" "*/ );


#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }
#endif // SAFE_DELETE

#ifndef SAFE_DELETEA
#define SAFE_DELETEA(x) if(x) { delete []x; x=NULL; }
#endif // SAFE_DELETEA

#if defined(_FILELOG_PACKET) || defined(_FILELOG_SPOS)
void _WriteLog( int nLogType, const char *szStr, ... ) ;
void _WriteLogA( const char *szStr, ... ) ;
void _WriteDisplayLog( int Logtype , const char *szStr  , ...);
extern bool g_bInvalidDisconnect;
#define WriteLog( type , fmt, ... ) _WriteLog(type ,  fmt, __VA_ARGS__ );

#define InfoLog( fmt, ... ) _WriteDisplayLog(1,fmt , __VA_ARGS__  );
#define ErrorLog( fmt, ... ) _WriteDisplayLog( 3,fmt , __VA_ARGS__  );

#ifndef _FINAL_BUILD
#define DebugLog( fmt, ... ) _WriteDisplayLog( 4,fmt , __VA_ARGS__  );
#define WarningLog( fmt, ... ) _WriteDisplayLog(2,fmt , __VA_ARGS__  );
#else
#define DebugLog( fmt, ... ) 
#define WarningLog( fmt, ... ) 
#endif
#else
#define WriteLog( type, fmt, ... )
#define InfoLog( fmt, ... )
#define ErrorLog( fmt, ... )
#define DebugLog( fmt, ... )
#define WarningLog( fmt, ... ) 
#endif



