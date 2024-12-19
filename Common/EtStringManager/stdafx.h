#pragma once

#pragma warning( disable:4996 )

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

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

#include "../DNDefine.h"
#include "AssertX.h"