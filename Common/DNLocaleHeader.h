
#pragma once

#define __wcsicmp_l _wcsicmp
#define __stricmp_l _stricmp

#define PRE_ADD_CLIENT_SECONDAUTH					// hicom15		2010-01-25		// 2차인증 임시로 잠깐 넣겠습니다.

#if defined (_JP)
//CompareString((MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_NEUTRAL), SORT_DEFAULT)), NORM_IGNOREWIDTH, aa.c_str(), -1, AA.c_str(), -1) == CSTR_EQUAL
#ifdef __wcsicmp_l
#undef __wcsicmp_l
#define __wcsicmp_l wcscmp
#endif
#ifdef __stricmp_l
#undef __stricmp_l
#define __stricmp_l strcmp
#endif
#endif