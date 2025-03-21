#include "stdafx.h"
#include "LiteHTMLEntityResolver.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#pragma warning(push, 4)
ENTITYREFS_MAP CLiteHTMLEntityResolver::m_CharEntityRefs;

void CLiteHTMLEntityResolver::InitEntityRefs()
{
	if(!m_CharEntityRefs.empty())
		return;

	/** C0 Controls and Basic Latin */
	m_CharEntityRefs[L"quot"]		= L'\x22';
	m_CharEntityRefs[L"amp"]		= L'\x26';
	m_CharEntityRefs[L"apos"]		= L'\x27';
	m_CharEntityRefs[L"lt"]			= L'\x3C';
	m_CharEntityRefs[L"gt"]			= L'\x3E';
	/** ISO 8859-1 (Latin-1) characters */
	m_CharEntityRefs[L"nbsp"]		= L'\xA0';
	m_CharEntityRefs[L"iexcl"]		= L'\xA1';
	m_CharEntityRefs[L"cent"]		= L'\xA2';
	m_CharEntityRefs[L"pound"]		= L'\xA3';
	m_CharEntityRefs[L"current"]	= L'\xA4';
	m_CharEntityRefs[L"yen"]		= L'\xA5';
	m_CharEntityRefs[L"brvbar"]		= L'\xA6';
	m_CharEntityRefs[L"sect"]		= L'\xA7';
	m_CharEntityRefs[L"uml"]		= L'\xA8';
	m_CharEntityRefs[L"copy"]		= L'\xA9';
	m_CharEntityRefs[L"ordf"]		= L'\xAA';
	m_CharEntityRefs[L"laquo"]		= L'\xAB';
	m_CharEntityRefs[L"not"]		= L'\xAC';
	m_CharEntityRefs[L"shy"]		= L'\xAD';
	m_CharEntityRefs[L"reg"]		= L'\xAE';
	m_CharEntityRefs[L"macr"]		= L'\xAF';
	m_CharEntityRefs[L"deg"]		= L'\xB0';
	m_CharEntityRefs[L"plusmn"]		= L'\xB1';
	m_CharEntityRefs[L"sup2"]		= L'\xB2';
	m_CharEntityRefs[L"sup3"]		= L'\xB3';
	m_CharEntityRefs[L"acute"]		= L'\xB4';
	m_CharEntityRefs[L"micro"]		= L'\xB5';
	m_CharEntityRefs[L"para"]		= L'\xB6';
	m_CharEntityRefs[L"middot"]		= L'\xB7';
	m_CharEntityRefs[L"cedil"]		= L'\xB8';
	m_CharEntityRefs[L"sup1"]		= L'\xB9';
	m_CharEntityRefs[L"ordm"]		= L'\xBA';
	m_CharEntityRefs[L"raquo"]		= L'\xBB';
	m_CharEntityRefs[L"frac14"]		= L'\xBC';
	m_CharEntityRefs[L"frac12"]		= L'\xBD';
	m_CharEntityRefs[L"frac34"]		= L'\xBE';
	m_CharEntityRefs[L"iquest"]		= L'\xBF';
	m_CharEntityRefs[L"Agrave"]		= L'\xC0';
	m_CharEntityRefs[L"Aacute"]		= L'\xC1';
	m_CharEntityRefs[L"Acirc"]		= L'\xC2';
	m_CharEntityRefs[L"Atilde"]		= L'\xC3';
	m_CharEntityRefs[L"Auml"]		= L'\xC4';
	m_CharEntityRefs[L"Aring"]		= L'\xC5';
	m_CharEntityRefs[L"AElig"]		= L'\xC6';
	m_CharEntityRefs[L"Ccedil"]		= L'\xC7';
	m_CharEntityRefs[L"Egrave"]		= L'\xC8';
	m_CharEntityRefs[L"Eacute"]		= L'\xC9';
	m_CharEntityRefs[L"Ecirc"]		= L'\xCA';
	m_CharEntityRefs[L"Euml"]		= L'\xCB';
	m_CharEntityRefs[L"Igrave"]		= L'\xCC';
	m_CharEntityRefs[L"Iacute"]		= L'\xCD';
	m_CharEntityRefs[L"Icirc"]		= L'\xCE';
	m_CharEntityRefs[L"Iuml"]		= L'\xCF';
	m_CharEntityRefs[L"ETH"]		= L'\xD0';
	m_CharEntityRefs[L"Ntilde"]		= L'\xD1';
	m_CharEntityRefs[L"Ograve"]		= L'\xD2';
	m_CharEntityRefs[L"Oacute"]		= L'\xD3';
	m_CharEntityRefs[L"Ocirc"]		= L'\xD4';
	m_CharEntityRefs[L"Otilde"]		= L'\xD5';
	m_CharEntityRefs[L"Ouml"]		= L'\xD6';
	m_CharEntityRefs[L"times"]		= L'\xD7';
	m_CharEntityRefs[L"Oslash"]		= L'\xD8';
	m_CharEntityRefs[L"Ugrave"]		= L'\xD9';
	m_CharEntityRefs[L"Uacute"]		= L'\xDA';
	m_CharEntityRefs[L"Ucirc"]		= L'\xDB';
	m_CharEntityRefs[L"Uuml"]		= L'\xDC';
	m_CharEntityRefs[L"Yacute"]		= L'\xDD';
	m_CharEntityRefs[L"THORN"]		= L'\xDE';
	m_CharEntityRefs[L"szlig"]		= L'\xDF';
	m_CharEntityRefs[L"agrave"]		= L'\xE0';
	m_CharEntityRefs[L"aacute"]		= L'\xE1';
	m_CharEntityRefs[L"acirc"]		= L'\xE2';
	m_CharEntityRefs[L"atilde"]		= L'\xE3';
	m_CharEntityRefs[L"auml"]		= L'\xE4';
	m_CharEntityRefs[L"aring"]		= L'\xE5';
	m_CharEntityRefs[L"aelig"]		= L'\xE6';
	m_CharEntityRefs[L"ccedil"]		= L'\xE7';
	m_CharEntityRefs[L"egrave"]		= L'\xE8';
	m_CharEntityRefs[L"eacute"]		= L'\xE9';
	m_CharEntityRefs[L"ecirc"]		= L'\xEA';
	m_CharEntityRefs[L"euml"]		= L'\xEB';
	m_CharEntityRefs[L"igrave"]		= L'\xEC';
	m_CharEntityRefs[L"iacute"]		= L'\xED';
	m_CharEntityRefs[L"icirc"]		= L'\xEE';
	m_CharEntityRefs[L"iuml"]		= L'\xEF';
	m_CharEntityRefs[L"eth"]		= L'\xF0';
	m_CharEntityRefs[L"ntilde"]		= L'\xF1';
	m_CharEntityRefs[L"ograve"]		= L'\xF2';
	m_CharEntityRefs[L"oacute"]		= L'\xF3';
	m_CharEntityRefs[L"ocirc"]		= L'\xF4';
	m_CharEntityRefs[L"otilde"]		= L'\xF5';
	m_CharEntityRefs[L"ouml"]		= L'\xF6';
	m_CharEntityRefs[L"divide"]		= L'\xF7';
	m_CharEntityRefs[L"oslash"]		= L'\xF8';
	m_CharEntityRefs[L"ugrave"]		= L'\xF9';
	m_CharEntityRefs[L"uacute"]		= L'\xFA';
	m_CharEntityRefs[L"ucirc"]		= L'\xFB';
	m_CharEntityRefs[L"uuml"]		= L'\xFC';
	m_CharEntityRefs[L"yacute"]		= L'\xFD';
	m_CharEntityRefs[L"thorn"]		= L'\xFE';
	m_CharEntityRefs[L"yuml"]		= L'\xFF';
}
#pragma warning(pop)
