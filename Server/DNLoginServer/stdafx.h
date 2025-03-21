// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include "../../Common/DNLocaleHeader.h"
#include "../../Common/DNDefine.h"
#include "DNHeader.h"

#include "DNPacket.h"
#include "DNProtocol.h"

#include "assertx.h"

using namespace eLogin;
using namespace eSystem;

#ifdef __COVERITY__
#else
// googletest
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#endif

#if defined(_HSHIELD)
#include "../../Extern/HShield/SDKs/Include/AntiCpXSvr.h"
#if defined(WIN64)
#pragma comment(lib, "../../Extern/HShield/SDKs/Lib/Server/x64/Win/Multithreaded/AntiCrack/AntiCpXSvr64.lib")
#else
#pragma comment(lib, "../../Extern/HShield/SDKs/Lib/Server/x86/Win/Multithreaded/AntiCrack/AntiCpXSvr.lib")
#endif
#endif	// _HSHIELD

#if defined(_KR)
#include "../ServerCommon/KR/AuthCheck.h"
#pragma comment(lib, "../ServerCommon/KR/AuthCheck.lib")

#elif defined(_US)
#include "../ServerCommon/US/AuthCheck.h"
#pragma comment(lib, "../ServerCommon/US/AuthCheck.lib")

#include "../ServerCommon/US/IPIObject.h"
#pragma comment(lib, "../ServerCommon/US/PI/ImmigratorSDK.lib")

#elif defined(_JP)
#include "../ServerCommon/JP/HanAuthForSvr.h"
#if defined(_DEBUG)
#pragma comment(lib, "../ServerCommon/JP/HanAuthForSvrD_x64.lib")
#else	// #if defined(_DEBUG)
#pragma comment(lib, "../ServerCommon/JP/HanAuthForSvr_x64.lib")
#endif	// #if defined(_DEBUG)

#elif defined(_CH)
#include "../ServerCommon/CH/SDKStuff.h"

#if defined(WIN64)

#if defined(_DEBUG)
#pragma comment(lib, "../../Extern/SDOA/Server/Bin/x64/SDOA4ServerDLLD_x64.lib")	// 인증 (SDOA)
#else	// #if defined(_DEBUG)
#pragma comment(lib, "../../Extern/SDOA/Server/Bin/x64/SDOA4ServerDLL_x64.lib")		// 인증 (SDOA)
#endif	// #if defined(_DEBUG)

#else	// WIN64

#if defined(_DEBUG)
#pragma comment(lib, "../../Extern/SDOA/Server/Bin/x86/SDOA4ServerDLLD.lib")		// 인증 (SDOA)
#else	// _DEBUG
#pragma comment(lib, "../../Extern/SDOA/Server/Bin/x86/SDOA4ServerDLL.lib")			// 인증 (SDOA)
#endif	// _DEBUG

#endif	// #if defined(WIN64)

#if defined(WIN64)
#if defined(_DEBUG)
#pragma comment(lib, "../../Extern/SDOUA/libs/x64/UserAuthenSDKd_x64.lib")
#pragma comment(lib, "../../Extern/SDOUA/libs/x64/CommonSDKDLLd_x64.lib")
#else // #if defined(_DEBUG)
#pragma comment(lib, "../../Extern/SDOUA/libs/x64/UserAuthenSDK_x64.lib")
#pragma comment(lib, "../../Extern/SDOUA/libs/x64/CommonSDKDLL_x64.lib")
#endif // #if defined(_DEBUG)
#else // #if defined(WIN64)
#if defined(_DEBUG)
#pragma comment(lib, "../../Extern/SDOUA/libs/x86/UserAuthenSDKd.lib")
#pragma comment(lib, "../../Extern/SDOUA/libs/x86/CommonSDKDLLd.lib")
#else // #if defined(_DEBUG)
#pragma comment(lib, "../../Extern/SDOUA/libs/x86/UserAuthenSDK.lib")
#pragma comment(lib, "../../Extern/SDOUA/libs/x86/CommonSDKDLL.lib")
#endif // #if defined(_DEBUG)
#endif // #if defined(WIN64)

#elif defined (_SG)
#include "../ServerCommon/sg/Soap/stdsoap2.h"
#include "../ServerCommon/sg/Soap/DNCherryAuth.h"

#elif defined (_TH)
#include "../ServerCommon/th/DNAsiaSoftAuth.h"

#elif defined(_EU)
#pragma comment(lib, "../ServerCommon/EU/Auth/AuthenSDK.lib")
#include "../ServerCommon/EU/Auth/AuthenSDK.h"
#endif // _KR, _CH

#if defined (_GPK)
using namespace SGPK;
#ifdef WIN64
#pragma comment(lib, "../../Extern/GPK/Lib/GPKitSvr64.lib")
#else		//#ifdef WIN64
#ifdef _DEBUG
#pragma comment(lib, "../../Extern/GPK/Lib/GPKitSvrD.lib")
#else		//#ifdef _DEBUG
#pragma comment(lib, "../../Extern/GPK/Lib/GPKitSvr.lib")
#endif		//#ifdef _DEBUG
#endif		//#ifdef WIN64

#include "../../Extern/GPK/Include/GPKitSvr.h"

#endif		//#if defined (_GPK)

// 버그리포팅에 필요한 lib들
#pragma comment (lib, "shell32.lib")
#pragma comment (lib, "ole32.lib")
#pragma comment (lib, "oleaut32.lib")

// boost
#include "./boost/algorithm/string.hpp"
#include "./boost/lexical_cast.hpp"
#include "./boost/timer.hpp"
#include "./boost/shared_ptr.hpp"
#include "./boost/format.hpp"

#pragma comment(lib,"Odbc32.lib")

// TODO: reference additional headers your program requires here

using namespace DB;
