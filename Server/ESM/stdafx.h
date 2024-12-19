
#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "DNDefine.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <map>
#include <vector>
#include <direct.h>
#include <io.h>
#include "SundriesFunc.h"
#include "DNPacket.h"
#include "DNProtocol.h"
#include "DNCommonDef.h"
#include "dnserverdef.h"
#include "DNServerPacket.h"
#include "DnServerProtocol.h"
#include "strdef.h"
#include "CommonMacros.h"
#include "ServiceUtil.h"
#include "SafeStringMacros.h"
#include "StringUtil.h"

inline void _strcpy(char * pDest, const void * pSrc, int nLen)
{
	if (nLen > 0)
		memcpy(pDest, pSrc, nLen);
	pDest[nLen] = '\0';
}

inline void _wcscpy(WCHAR * pDest, const void * pSrc, int nLen)
{
	if (nLen > 0)
		memcpy(pDest, pSrc, nLen*sizeof(WCHAR));
	pDest[nLen] = '\0';
}

inline void __tcscpy(TCHAR * pDest, const void * pSrc, int nLen)
{
	if (nLen > 0)
		memcpy(pDest, pSrc, nLen*sizeof(TCHAR));
	pDest[nLen] = '\0';
}