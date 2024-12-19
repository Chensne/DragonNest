// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_WINNT 0x0501

#include <windows.h>

#pragma warning(disable:4819)


#include <vector>
#include <deque>
#include <string>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

// 힙 메모리 할당을 해제하는 함수 객체.
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

typedef basic_string<wchar_t> tstring;

#include "../DNDefine.h"
// Engine include
#include "EternityEngine.h"
#include "EngineUtil.h"

// Sound Engine
#include "EtSoundEngine.h"

#include "EtResourceMng.h"

#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
using namespace boost;


//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here
