// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning( disable:4819 )
#pragma warning( disable:4996 )


// TODO: reference additional headers your program requires here

#include "../DNDefine.h"
#include "NetCommon.h"
#include "DNCommonDef.h"
#include "DNServerDef.h"

#include <mmsystem.h>

extern void _WriteLog( int nLogType, const char *szStr, ... );
#if defined(_FILELOG_PACKET) || defined(_FILELOG_SPOS)
#define WriteLog( type, fmt, ... ) _WriteLog( type, fmt, __VA_ARGS__ );
#else
#define WriteLog( type, fmt, ... ) 
#endif
