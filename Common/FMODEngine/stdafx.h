// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <hash_map>
#include <algorithm>
#include <string>
using namespace std;

#if defined(_DEBUG) || defined(_RDEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include <d3d9.h>
#include <d3dx9.h>

#include "../DNDefine.h"

//#define USE_FMOD_DLL	// 프로젝트 속성에다 디파인 걸어두었으니 소스에서 따로 할필요 없습니다. 우선 안정화되기 전까지 릴리즈에선 ReleaseUseDLL 빌드설정을 사용하셔요.
#if defined(USE_FMOD_DLL)
#include "fmod.hpp"
#include "fmod_errors.h"

#pragma comment(lib,"fmodex_vc.lib")
#else	// #if defined(USE_FMOD_DLL)
#pragma comment(lib, "msacm32")
#endif	// #if defined(USE_FMOD_DLL)

#include "EternityEngine.h"
#include "EtResourceMng.h"