// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif		

//#ifdef NDEBUG
//#ifdef _SECURE_SCL_THROWS
//	#undef _SECURE_SCL_THROWS
//	#define _SECURE_SCL_THROWS 1
//#else 
//	#define _SECURE_SCL_THROWS 1
//#endif
//#endif

///////////////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4482)
#pragma warning(disable:4250)

#if !defined (_FINAL_BUILD)
#define _USE_NATPROBE
#endif

#define _CRT_RAND_S
#include <stdlib.h>
#include <stdio.h>

#include "../../Common/DNLocaleHeader.h"
#include "../../Common/DNDefine.h"

#include "DNHeader.h"
#include "DNPrjSafe.h"
#include "DNPacket.h"
#include "DNProtocol.h"
#include "DNCommonDef.h"
#include "EternityEngine.h"
#include "Random.h"

#define _rand( room ) CRandom::GetInstance(room).rand()
#define _roomrand( room ) (reinterpret_cast<CDNGameRoom*>(room))->rand()
#define _srand( room, nValue ) CRandom::GetInstance(room).srand( nValue )
#define GetRandom( room ) CRandom::GetInstance(room)

#include <vector>
#include <map>
#include <set>
#include <string>
#include <xstring>
using namespace std;

typedef std::basic_string< TCHAR > tstring;

#define SMARTPTR_VALID_CHECK

#ifdef _USE_VOICECHAT
#ifdef _DEBUG
#pragma comment(lib, "..\\..\\Common\\VoiceChat\\DolbyAxon\\IceLib64\\ICE_d.lib")
#else
#pragma comment(lib, "..\\..\\Common\\VoiceChat\\DolbyAxon\\IceLib64\\ICE.lib")
#endif
#endif

// D3D
#pragma warning(disable:4819)
#define DIRECTINPUT_VERSION 0x0800

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#pragma warning(default:4819)
// DirectX Lib 
#pragma comment(lib,"d3d9.lib")
#ifdef _DEBUG
#pragma comment(lib,"d3dx9d.lib")
#else 
#pragma comment(lib,"d3dx9.lib")
#endif // _DEBUG
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dbghelp.lib")


// 루아 헤더파일
extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

// tinyxml 관련, 없으면 common/tinyxml 에서 받으세요.
#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif //TIXML_USE_STL

#include "tinyxml.h"

// Engine include
#include "MultiCommon.h"
#include "MultiSmartPtr.h"
#include "Timer.h"
#include "EternityEngine.h"
#include "EngineUtil.h"
#include "StringUtil.h"
#include "SmartPtrDef.h"
#include "EtResourceMng.h"
#include "lua_tinker.h"

const static string __NULL_STR("");

//#pragma warning(default:4482)

#include "Log.h"
#include "../../GameCommon/DnTableDB.h"
#include "MultiCommon.h"
#include "MultiElement.h"
#include "EtUIXML.h"

#include "./boost/algorithm/string.hpp"
#include "./boost/lexical_cast.hpp"
#include "./boost/timer.hpp"
#include "./boost/shared_ptr.hpp"
#include "./boost/format.hpp"

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

// googletest
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define INSTANCE(cls) cls::GetInstance(static_cast<CMultiElement*>(this)->GetRoom())
#define INSTANCE_(cls,func) cls::func(static_cast<CMultiElement*>(this)->GetRoom())
#define STATICFUNC() static_cast<CMultiElement*>(this)->GetRoom(),
#define GETROOM(obj) static_cast<CMultiElement*>(obj)->GetRoom()
#define ARG_STATIC_FUNC(object) static_cast<CMultiElement*>(object.GetPointer())->GetRoom(),

#pragma comment (lib, "ole32.lib")
#pragma comment (lib, "oleaut32.lib")

#ifndef DEL_TEMPLATE
#define DEL_TEMPLATE
template <class T>
class DeleteData
{
public:
	void operator() ( T elem )
	{
		if( elem )
		{
			delete elem;
			elem = NULL;
		}
	}

};
#endif

using namespace DB;

#undef MSDT_DECL
#define MSDT_DECL(pTYPE)			pTYPE
#define MSDT_SETTERM(pMSDT,pTIME)

#if defined (_WORK)
#include "../ServerCommon/DNConfig_Work.h"
#endif // #if defined (_WORK)s