// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#pragma warning( disable:4996 )


#define _CRT_RAND_S
#include <stdlib.h>
#define _USE_SENDCONTEXTPOOL

#include "../../Common/DNLocaleHeader.h"
#include "../../Common/DNDefine.h"
#include "DNHeader.h"

#include "DNPacket.h"
#include "DNProtocol.h"

#include "assertx.h"

// boost
#include "./boost/algorithm/string.hpp"
#include "./boost/lexical_cast.hpp"
#include "./boost/timer.hpp"
#include "./boost/shared_ptr.hpp"
#include "./boost/format.hpp"

#pragma comment(lib,"Odbc32.lib")

// googletest
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#if defined(_JP)
#include "../ServerCommon/JP/HanBillingForSvr.h"
#endif	// #if defined(_JP)

#if defined(_CH)
#include "../../Extern/SDBS/Src/bsipfun.h"
#ifdef _DEBUG
	#ifdef _WIN64
		#pragma comment(lib, "../../Extern/SDBS/Bin/x64/bsipclientd_x64.lib")
	#else
		#pragma comment(lib, "../../Extern/SDBS/Bin/x86/bsipclientd.lib")
	#endif
#else
	#ifdef _WIN64
		#pragma comment(lib, "../../Extern/SDBS/Bin/x64/bsipclient_x64.lib")
	#else
		#pragma comment(lib, "../../Extern/SDBS/Bin/x86/bsipclient.lib")
	#endif
#endif
#endif	// _CH

#if defined(_EU)
#include "../../Extern/SDBS/Src/bsipfun.h"
#ifdef _WIN64
#pragma comment(lib, "../../Extern/SDBS/Bin/x64/bsipclient_x64.lib")
#else
#pragma comment(lib, "../../Extern/SDBS/Bin/x86/bsipclient.lib")
#endif
#endif	// _EU


// TODO: reference additional headers your program requires here
