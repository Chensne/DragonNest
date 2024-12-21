
#include "stdafx.h"
#include "DNCountryUnicodeSet.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDNCountryUnicodeSet g_CountryUnicodeSet;

CDNCountryUnicodeSet::CDNCountryUnicodeSet() 
{
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_ALL,				MIN_MAX(UNICODE_TYPE_ALL_START, UNICODE_TYPE_ALL_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_NUMBER,			MIN_MAX(UNICODE_TYPE_NUM_START, UNICODE_TYPE_NUM_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_ENG,				MIN_MAX(UNICODE_TYPE_ENG_BIG_START, UNICODE_TYPE_ENG_BIG_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_ENG,				MIN_MAX(UNICODE_TYPE_ENG_SMALL_START, UNICODE_TYPE_ENG_SMALL_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_HAN,				MIN_MAX(UNICODE_TYPE_HAN_SYLLABLES_START, UNICODE_TYPE_HAN_SYLLABLES_END)));	
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_SPECIAL,			MIN_MAX(UNICODE_TYPE_SPECIAL_START, UNICODE_TYPE_SPECIAL_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_JPN,				MIN_MAX(UNICODE_TYPE_JPN_HIRAGANA_START, UNICODE_TYPE_JPN_HIRAGANA_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_JPN,				MIN_MAX(UNICODE_TYPE_JPN_KATAKANA_START, UNICODE_TYPE_JPN_KATAKANA_END)));	
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_CJK_UNIFIED,		MIN_MAX(UNICODE_TYPE_CJK_UNIFIED_START, UNICODE_TYPE_CJK_UNIFIED_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_CJK_COMP,			MIN_MAX(UNICODE_TYPE_CJK_COMP_START, UNICODE_TYPE_CJK_COMP_END)));	
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_SG_SPECIAL,		MIN_MAX(UNICODE_TYPE_SG_SPECIAL_A_START, UNICODE_TYPE_SG_SPECIAL_A_END)));	
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_SG_SPECIAL,		MIN_MAX(UNICODE_TYPE_SG_SPECIAL_B_START, UNICODE_TYPE_SG_SPECIAL_B_END)));	
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_SG_SPECIAL,		MIN_MAX(UNICODE_TYPE_SG_SPECIAL_C_START, UNICODE_TYPE_SG_SPECIAL_C_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_SG_PASS_SPECIAL,	MIN_MAX(UNICODE_TYPE_SG_SPECIAL_PASS_START, UNICODE_TYPE_SG_SPECIAL_PASS_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH,				MIN_MAX(UNICODE_TYPE_TH_START,UNICODE_TYPE_TH_THAI_END )));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_A_START, UNICODE_TYPE_TH_SPECIAL_A_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_B_START, UNICODE_TYPE_TH_SPECIAL_B_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_C_START, UNICODE_TYPE_TH_SPECIAL_C_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_D_START, UNICODE_TYPE_TH_SPECIAL_D_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_E_START, UNICODE_TYPE_TH_SPECIAL_E_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_F_START, UNICODE_TYPE_TH_SPECIAL_F_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_G_START, UNICODE_TYPE_TH_SPECIAL_G_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_TH_SPECIAL,		MIN_MAX(UNICODE_TYPE_TH_SPECIAL_H_START, UNICODE_TYPE_TH_SPECIAL_H_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_RU,				MIN_MAX(UNICODE_TYPE_RU_CYRILLIC_START, UNICODE_TYPE_RU_CYRILLIC_END)));	
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_RU,				MIN_MAX(UNICODE_TYPE_SG_SPECIAL_C_START, UNICODE_TYPE_SG_SPECIAL_C_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_EU,				MIN_MAX(UNICODE_TYPE_EU_SPECIAL_A_START, UNICODE_TYPE_EU_SPECIAL_A_END)));	
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_EU,				MIN_MAX(UNICODE_TYPE_EU_SPECIAL_B_START, UNICODE_TYPE_EU_SPECIAL_B_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_EU,				MIN_MAX(UNICODE_TYPE_EU_SPECIAL_C_START, UNICODE_TYPE_EU_SPECIAL_C_END)));
	m_mapAllowString.insert(HASH_MAP_ALLOW_STRING::value_type(CHECK_TYPE_EU,				MIN_MAX(UNICODE_TYPE_EU_SPECIAL_D_START, UNICODE_TYPE_EU_SPECIAL_D_END)));
}

CDNCountryUnicodeSet::~CDNCountryUnicodeSet()
{
	m_mapAllowString.clear();
}

bool CDNCountryUnicodeSet::Check(const wchar_t* pChr, DWORD dwCheckType)
{
	if (pChr)
	{
		wchar_t szChar;
		int nLength = (int) wcslen(pChr);
		for (int i=0; i < nLength; i++)
		{
			szChar = pChr[i];
			if (dwCheckType & CHECK_TYPE_ALL				&& IsExist(szChar, CHECK_TYPE_ALL))				continue;			
			if (dwCheckType & CHECK_TYPE_NUMBER				&& IsExist(szChar, CHECK_TYPE_NUMBER))			continue;
			if (dwCheckType & CHECK_TYPE_ENG				&& IsExist(szChar, CHECK_TYPE_ENG))				continue;
			if (dwCheckType & CHECK_TYPE_HAN				&& IsExist(szChar, CHECK_TYPE_HAN))				continue;
			if (dwCheckType & CHECK_TYPE_SPECIAL			&& IsExist(szChar, CHECK_TYPE_SPECIAL))			continue;
			if (dwCheckType & CHECK_TYPE_JPN				&& IsExist(szChar, CHECK_TYPE_JPN))				continue;
			if (dwCheckType & CHECK_TYPE_CJK_UNIFIED		&& IsExist(szChar, CHECK_TYPE_CJK_UNIFIED))		continue;
			if (dwCheckType & CHECK_TYPE_CJK_COMP			&& IsExist(szChar, CHECK_TYPE_CJK_COMP))		continue;
			if (dwCheckType & CHECK_TYPE_SG_SPECIAL			&& IsExist(szChar, CHECK_TYPE_SG_SPECIAL))		continue;
			if (dwCheckType & CHECK_TYPE_SG_PASS_SPECIAL	&& IsExist(szChar, CHECK_TYPE_SG_PASS_SPECIAL))	continue;
			if (dwCheckType & CHECK_TYPE_TH					&& IsExist(szChar, CHECK_TYPE_TH))				continue;
			if (dwCheckType & CHECK_TYPE_TH_SPECIAL			&& IsExist(szChar, CHECK_TYPE_TH_SPECIAL))		continue;
			if (dwCheckType & CHECK_TYPE_RU					&& IsExist(szChar, CHECK_TYPE_RU))				continue;
			if (dwCheckType & CHECK_TYPE_EU					&& IsExist(szChar, CHECK_TYPE_EU))				continue;
			return false;
		}
	}

	return true;
}

bool CDNCountryUnicodeSet::IsExist(wchar_t pChr, STRING_CHECK_TYPE eType)
{
    //TODO(Cussrro): û��ƥ��ĺ����ӿ�
	//HASH_MAP_ALLOW_STRING_I iter = m_mapAllowString.lower_bound(eType);
	//for ( ; iter != m_mapAllowString.upper_bound(eType); iter++)
	//{
	//	if (IS_IN_RANGE(iter->second.nMin, iter->second.nMax, pChr))
	//		return true;
	//}
	return true;
}
