// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#pragma warning( disable:4996 )

#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <map>
#include <string>
#include <xstring>
using namespace std;

typedef std::basic_string< TCHAR > tstring;

#pragma warning(disable:4819)
#define DIRECTINPUT_VERSION 0x0800

#if defined(_DEBUG) || defined(_RDEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#pragma warning(default:4819)

#include "../DNDefine.h"
#include "AssertX.h"
#include "Singleton.h"
#include "SmartPtr.h"
#include "Stream.h"
#include "SundriesFunc.h"


#include "EtMathWrapperD3D.h"
#include "EtPrimitive.h"
#include "EtCollisionPrimitive.h"
