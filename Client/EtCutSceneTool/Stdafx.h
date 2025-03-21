
#pragma once

#pragma warning(disable:4819)

// 미리 컴파일된 헤더.
// 자주 사용되는 헤더 파일은 여기에 인클루드 합니다.
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_WINNT 0x0501

#include "DNDefine.h"
#include "AssertX.h"

#include <windows.h>
#include <mmsystem.h>

#include "Timer.h"


#include <vector>
#include <deque>
#include <string>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

#pragma comment(lib,"d3d9.lib")
#ifdef _DEBUG
#pragma comment(lib,"d3dx9d.lib")
#else 
#pragma comment(lib,"d3dx9.lib")
#endif // _DEBUG
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dbghelp.lib")


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

#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
using namespace boost;

typedef basic_string<wchar_t> tstring;


#define PRE_ADD_FILTEREVENT // #75661
