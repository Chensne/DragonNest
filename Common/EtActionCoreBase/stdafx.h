// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4819)

// TODO: reference additional headers your program requires here
#include <Windows.h>

#include <vector>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

#include "../DNDefine.h"
#include "SundriesFunc.h"
#include "Stream.h"
#include "EtMathWrapperD3D.h"

#pragma warning(default:4819)