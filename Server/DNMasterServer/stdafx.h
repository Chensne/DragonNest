// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#pragma warning( disable:4996 )

#define _CRT_RAND_S
#include <stdlib.h>

#include "../../Common/DNLocaleHeader.h"
#include "../../Common/DNDefine.h"
#include "DNHeader.h"

#include "DNPacket.h"
#include "DNProtocol.h"

#include "assertx.h"

// 버그리포팅에 필요한 lib들
#pragma comment (lib, "shell32.lib")
#pragma comment (lib, "ole32.lib")
#pragma comment (lib, "oleaut32.lib")

#if defined(_CH)
#include "FCMsdkBase.h"
#if defined(WIN64)
#pragma comment(lib,"../ServerCommon/CH/fcmsdk_X64.lib")		// 피로도
#else	// #if defined(WIN64)
#pragma comment(lib,"../ServerCommon/CH/fcmsdk.lib")		// 피로도
#endif	// #if defined(WIN64)
#elif defined(_JP)
#ifdef WIN64
#include "../ServerCommon/JP/HanNetCafeForSvr.h"
#pragma comment(lib, "../ServerCommon/JP/HanNetCafeForSvr_x64.lib")	//NHN NetCafe
#endif

#elif defined(_US)
#include "../ServerCommon/US/IPIObject.h"
#pragma comment(lib, "../ServerCommon/US/PI64/ImmigratorSDK.lib")

#endif	// _CH

// googletest
#include "gtest/gtest.h"
#include "gmock/gmock.h"
