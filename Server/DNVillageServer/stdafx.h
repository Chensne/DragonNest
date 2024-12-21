// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif		

#define _CRT_RAND_S

#include "../../Common/DNLocaleHeader.h"
#include "../../Common/DNDefine.h"
#include "DNHeader.h"

#include "DNPacket.h"
#include "DNProtocol.h"

#include "EternityEngine.h"

#include "Random.h"

#define _rand() CRandom::GetInstance().rand()
#define _srand( nValue ) CRandom::GetInstance().srand( nValue )

#include "Log.h"

#include "../../GameCommon/EventControlHeader.h"

// tinyxml 관련, 없으면 common/tinyxml 에서 받으세요.
#include "../../Common/tinyxml/tinyxml.h"

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif 

#pragma comment (lib, "ole32.lib")
#pragma comment (lib, "oleaut32.lib")
#pragma comment( lib, "Odbc32.lib" )

#ifdef _USE_VOICECHAT
#ifdef _DEBUG
#pragma comment(lib, "..\\..\\Common\\VoiceChat\\DolbyAxon\\IceLib64\\ICE_d.lib")
#else
#pragma comment(lib, "..\\..\\Common\\VoiceChat\\DolbyAxon\\IceLib64\\ICE.lib")
#endif
#endif

// 루아 헤더파일
extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};
#include "lua_tinker.h"
#include "StringUtil.h"

#include "EtResourceMng.h"
#include "../../GameCommon/DnTableDB.h"

#if defined(_HSHIELD)
#include "../../Extern/HShield/SDKs/Include/AntiCpXSvr.h"
#if defined(WIN64)
#pragma comment(lib, "../../Extern/HShield/SDKs/Lib/Server/x64/Win/Multithreaded/AntiCrack/AntiCpXSvr64.lib")
#else
#pragma comment(lib, "../../Extern/HShield/SDKs/Lib/Server/x86/Win/Multithreaded/AntiCrack/AntiCpXSvr.lib")
#endif
#endif	// _HSHIELD

#if defined(_GPK)
#include "../../Extern/GPK/Include/GPKitSvr.h"

#if defined(WIN64)
#pragma comment(lib, "../../Extern/GPK/Lib/GPKitSvr64.lib")
#else	// WIN32

#if defined(_DEBUG)
#pragma comment(lib, "../../Extern/GPK/Lib/GPKitSvrD.lib")
#else	// _DEBUG
#pragma comment(lib, "../../Extern/GPK/Lib/GPKitSvr.lib")
#endif	// _DEBUG

#endif	// WIN32

using namespace SGPK;
#endif	// _GPK

#ifdef __COVERITY__
#else
// googletest
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#endif

// boost
#include "./boost/format.hpp"

// 할때마다 쓰기 귀찮아서 여기다 선언!
using namespace DB;

using namespace eSystem;
using namespace eChar;
using namespace eActor;
using namespace eProp;
using namespace eParty;
using namespace eItem;
using namespace eItemGoods;
using namespace eNpc;
using namespace eQuest;
using namespace eChat;
using namespace eTrade;
using namespace eSkill;

#if defined (_WORK)
#include "../ServerCommon/DNConfig_Work.h"
#endif // #if defined (_WORK)