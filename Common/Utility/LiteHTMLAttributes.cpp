#include "stdafx.h"
#include "LiteHTMLAttributes.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#pragma warning(push, 4)

const COLORREF CLiteHTMLElemAttr::_clrInvalid = (COLORREF)0xFFFFFFFF;
const unsigned short CLiteHTMLElemAttr::_percentMax = USHRT_MAX;

NAMEDCOLOR_MAP CLiteHTMLElemAttr::_namedColors;

#pragma warning(pop)
