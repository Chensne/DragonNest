#pragma once

#include <vector>
#include <map>
#include <string>
#include <assert.h>
#include <atlstr.h>

//#define ASSERT( x )		_ASSERT( x )

#define LITEHTML_ARGB(a,r,g,b) \
((COLORREF)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

typedef std::pair<CStringW, wchar_t>		ENTITYREFS_PAIR;
typedef std::map<CStringW, wchar_t>		ENTITYREFS_MAP;
typedef ENTITYREFS_MAP::iterator		ENTITYREFS_MAP_ITER;

typedef std::pair<CStringW, COLORREF>	NAMEDCOLOR_PAIR;
typedef std::map<CStringW, COLORREF>		NAMEDCOLOR_MAP;
typedef NAMEDCOLOR_MAP::iterator		NAMEDCOLOR_MAP_ITER;

#pragma warning(push, 4)

#define SAFE_DELETE_POINTER(_P) \
(void)(_P != NULL ? delete _P, _P = NULL : 0) \

#pragma warning(pop)