// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "gtest/gtest.h"
#include "boost/shared_ptr.hpp"

#include <string>
#include <vector>
#include <set>
#include <deque>
#include <map>
#include <algorithm>

typedef std::basic_string< TCHAR > tstring;

#include "DNDefine.h"
#include "AssertX.h"

#include "DNTableFile.h"
#include "SundriesFunc.h"

#include "EtResourceMng.h"
#include "EternityEngine.h"

using namespace std;
using namespace boost;

extern wstring g_strResourcePath;
extern string g_strResourcePathA;
extern map<wstring, string> g_mapExtFolderPath;
extern wstring g_strNation;
extern string g_strNationExtFolder;
extern string g_strTableName;

void GatherConcernedExt( /*IN*/ const char* pTableFileName, /*IN OUT*/ vector<string>& vlFilePathsToVerify, /*IN OUT*/ string& strTableFullPath );
DNTableFileFormat* LoadExtFile( const char* pTableFileFullName, char* pInverseSearchLabelFilter = NULL );
