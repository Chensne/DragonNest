// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <map>
#include <vector>

#include "../DNDefine.h"

// TODO: reference additional headers your program requires here
// 루아 라이브러리 링크
/*
#ifdef _DEBUG
#pragma comment(lib,"lua51_Debug(MTd).lib")
#else 
#pragma comment(lib,"lua51_Release(MT).lib")
#endif // _DEBUG
*/

// 루아 헤더파일
extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
