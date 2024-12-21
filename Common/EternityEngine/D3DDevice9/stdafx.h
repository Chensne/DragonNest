// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#pragma warning( disable:4819 )
#pragma warning( disable:4996 )

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <hash_map>
#include <algorithm>
#include <string>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#if defined(_DEBUG) || defined(_RDEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include <d3d9.h>
#include <d3dx9.h>

#include "../../DNDefine.h"
#include "AssertX.h"
#include "Singleton.h"
#include "SmartPtr.h"
#include "Stream.h"
#include "EtError.h"
#include "SundriesFunc.h"

#include "EtD3DDevice9.h"
