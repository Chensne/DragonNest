
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
#include <vector>
#include <direct.h>
#include <io.h>
#include <tchar.h>
#include <string>
#include "DNHeader.h"
#include "DNPacket.h"
#include "DNProtocol.h"
#include "DNCommonDef.h"
#include "minidump.h"
#include "CommonMacros.h"
#include "ServiceUtil.h"

#ifndef UNICODE
#define std_str std::string
#else
#define std_str std::wstring
#endif

#include <map>
inline void _wcscpy(WCHAR * pDest, const void * pSrc, int nLen)
{
	if (nLen > 0)
		memcpy(pDest, pSrc, nLen*sizeof(WCHAR));
	pDest[nLen] = '\0';
}

inline void _strcpy(char * pDest, const void * pSrc, int nLen)
{
	if (nLen > 0)
		memcpy(pDest, pSrc, nLen);
	pDest[nLen] = '\0';
}



static unsigned long _inet_addr(const WCHAR * cp)
{
	int s_b[4] = { 0, 0, 0, 0};
	struct in_addr in;
	swscanf(cp, L"%d.%d.%d.%d", &s_b[0], &s_b[1], &s_b[2], &s_b[3]);
	in.S_un.S_un_b.s_b1 = s_b[0];
	in.S_un.S_un_b.s_b2 = s_b[1];
	in.S_un.S_un_b.s_b3 = s_b[2];
	in.S_un.S_un_b.s_b4 = s_b[3];
	return in.S_un.S_addr;
}

int GetServerType(const WCHAR * pType);
int GetMonitorLevel(const WCHAR * pLevel);
int GetSIDType(const WCHAR * pwszType);

int GetServerTypeForConvert(const WCHAR * pType);

struct tm _wtotm(const WCHAR * pTimeFromSql);
time_t _wtoTime_t(const WCHAR * pTimeFromSql);
