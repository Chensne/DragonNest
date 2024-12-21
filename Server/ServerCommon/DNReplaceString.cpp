#include "stdafx.h"
#include "DNReplaceString.h"
#include "EtUIXML.h"

DNReplaceString g_ReplaceString;
DNReplaceString::DNReplaceString()
{
	m_StaticReplaceList.clear();
	m_StaticReplaceList.push_back( __StaticReplaceStringInfo( __StaticReplaceStringInfo::user_nick,		L"user_nick") );
	m_StaticReplaceList.push_back( __StaticReplaceStringInfo( __StaticReplaceStringInfo::user_class,	L"user_class") );
	m_StaticReplaceList.push_back( __StaticReplaceStringInfo( __StaticReplaceStringInfo::monster,		L"monster") );
	m_StaticReplaceList.push_back( __StaticReplaceStringInfo( __StaticReplaceStringInfo::npc,			L"npc") );
	m_StaticReplaceList.push_back( __StaticReplaceStringInfo( __StaticReplaceStringInfo::item,			L"item") );
	m_StaticReplaceList.push_back( __StaticReplaceStringInfo( __StaticReplaceStringInfo::skill,			L"skill") );
	m_StaticReplaceList.push_back( __StaticReplaceStringInfo( __StaticReplaceStringInfo::map,			L"map") );
}

DNReplaceString::~DNReplaceString()
{

}

#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
std::wstring DNReplaceString::_GetStaticString(int nType, std::vector<std::wstring>& tokens, MultiLanguage::SupportLanguage::eSupportLanguage eNation)
{
	switch( nType )
	{

	case __StaticReplaceStringInfo::user_nick:
		{
			//if ( CDnActor::s_hLocalActor ) return CDnActor::s_hLocalActor->GetName();
		}
		break;
	case __StaticReplaceStringInfo::user_class:
		break;
	case __StaticReplaceStringInfo::monster:
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMONSTER );
			int nMonID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nMonID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID, eNation );
			return wszName;
		}
		break;
	case __StaticReplaceStringInfo::npc:	
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TNPC );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID, eNation );
			return wszName;

		}
		break;
	case __StaticReplaceStringInfo::item:
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TITEM );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID, eNation );
			return wszName;
		}
		break;
	case __StaticReplaceStringInfo::skill:	
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TSKILL );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID, eNation );
			return wszName;
		}
		break;
	case __StaticReplaceStringInfo::map:
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMAP );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_MapNameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID, eNation );
			return wszName;
		}
		break;
	}

	return std::wstring(L"");
}

std::wstring DNReplaceString::_GetString(int nStringID, MultiLanguage::SupportLanguage::eSupportLanguage eNation)
{
	std::wstring wszString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID, eNation );

	return wszString;
}
#else		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
std::wstring DNReplaceString::_GetStaticString(int nType, std::vector<std::wstring>& tokens)
{
	switch( nType )
	{

	case __StaticReplaceStringInfo::user_nick:
		{
			//if ( CDnActor::s_hLocalActor ) return CDnActor::s_hLocalActor->GetName();
		}
		break;
	case __StaticReplaceStringInfo::user_class:
		break;
	case __StaticReplaceStringInfo::monster:
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMONSTER );
			int nMonID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nMonID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
			return wszName;
		}
		break;
	case __StaticReplaceStringInfo::npc:	
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TNPC );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
			return wszName;

		}
		break;
	case __StaticReplaceStringInfo::item:
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TITEM );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
			return wszName;
		}
		break;
	case __StaticReplaceStringInfo::skill:	
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TSKILL );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
			return wszName;
		}
		break;
	case __StaticReplaceStringInfo::map:
		{
			DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMAP );
			int nID = _wtoi(tokens[1].c_str());
			int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_MapNameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
			return wszName;
		}
		break;
	}

	return std::wstring(L"");
}

std::wstring DNReplaceString::_GetString(int nStringID)
{
	std::wstring wszString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID );

	return wszString;
}
#endif		//#if defined(_SERVER) && defined(PRE_ADD_MULTILANGUAGE)


#if defined(_SERVER) && defined(PRE_ADD_MULTILANGUAGE)
bool DNReplaceString::Relpace(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info, MultiLanguage::SupportLanguage::eSupportLanguage eNation)
{

	ReplaceDynamicString(str,info, eNation);
	ReplaceStaticString(str, eNation);

	return true;
}

void DNReplaceString::ReplaceStaticString(IN OUT std::wstring& str, MultiLanguage::SupportLanguage::eSupportLanguage eNation)
{
	// {} 로 감싸진 키워드들을 찾는다.
	std::vector<std::wstring> tokens;
	TokenizeW(str, tokens, std::wstring(L"{"), std::wstring(L"}"), false);

	for ( int j = 0 ; j < (int)tokens.size() ; j++ )
	{
		std::vector<std::wstring> child_tokens;
		TokenizeW(tokens[j], child_tokens, L":");

		// 키워드 뒤에 : 가 붙어있는 녀석들을 분리 해낸다.
		for ( int k = 0 ; k < (int)child_tokens.size() ; k++ )
		{
			for ( int i = 0 ; i < (int)m_StaticReplaceList.size() ; i++ )
			{
				if ( child_tokens[0] == m_StaticReplaceList[i].szKey )
				{
					std::wstring wsz = _GetStaticString(m_StaticReplaceList[i].nType, child_tokens, eNation);
					AllReplaceW(str, tokens[j], wsz);
					RemoveStringW(str, std::wstring(L"{"));
					RemoveStringW(str, std::wstring(L"}"));
				}
			}
		}
	}
}

void DNReplaceString::ReplaceDynamicString(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info, MultiLanguage::SupportLanguage::eSupportLanguage eNation)
{
	for ( size_t i = 0 ; i < info.size() ; i++ )
	{
		std::wstring wszKey;
		std::wstring wsz;
		if ( info[i].cValueType == DynamicReplaceStringInfo::INT )
		{
			wsz = FormatW( L"%d", info[i].nValue );
		}
		else if ( info[i].cValueType == DynamicReplaceStringInfo::STRING )
		{
			wsz = _GetString(info[i].nValue, eNation);
		}
		else
		{
			// 뷁
		}

		if ( !wsz.empty() )
		{
			AllReplaceW( str, info[i].szKey, wsz);
		}
	}

}
#else		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
bool DNReplaceString::Relpace(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info)
{

	ReplaceDynamicString(str,info);
	ReplaceStaticString(str);

	return true;
}

void DNReplaceString::ReplaceStaticString(IN OUT std::wstring& str)
{
	// {} 로 감싸진 키워드들을 찾는다.
	std::vector<std::wstring> tokens;
	TokenizeW(str, tokens, std::wstring(L"{"), std::wstring(L"}"), false);

	for ( int j = 0 ; j < (int)tokens.size() ; j++ )
	{
		std::vector<std::wstring> child_tokens;
		TokenizeW(tokens[j], child_tokens, L":");

		// 키워드 뒤에 : 가 붙어있는 녀석들을 분리 해낸다.
		for ( int k = 0 ; k < (int)child_tokens.size() ; k++ )
		{
			for ( int i = 0 ; i < (int)m_StaticReplaceList.size() ; i++ )
			{
				if ( child_tokens[0] == m_StaticReplaceList[i].szKey )
				{
					std::wstring wsz = _GetStaticString(m_StaticReplaceList[i].nType, child_tokens);
					AllReplaceW(str, tokens[j], wsz);
					RemoveStringW(str, std::wstring(L"{"));
					RemoveStringW(str, std::wstring(L"}"));
				}
			}
		}
	}
}

void DNReplaceString::ReplaceDynamicString(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info)
{
	for ( size_t i = 0 ; i < info.size() ; i++ )
	{
		std::wstring wszKey;
		std::wstring wsz;
		if ( info[i].cValueType == DynamicReplaceStringInfo::INT )
		{
			wsz = FormatW( L"%d", info[i].nValue );
		}
		else if ( info[i].cValueType == DynamicReplaceStringInfo::STRING )
		{
			wsz = _GetString(info[i].nValue);
		}
		else
		{
			// 뷁
		}

		if ( !wsz.empty() )
		{
			AllReplaceW( str, info[i].szKey, wsz);
		}
	}

}
#endif		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)