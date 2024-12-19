#pragma once


class DNReplaceString
{
public:
	struct  __StaticReplaceStringInfo
	{
		enum 
		{
			user_nick = 1,
			user_class,
			monster,
			npc,
			item,
			skill,
			map
		};
		int nType;
		std::wstring	szKey;
		__StaticReplaceStringInfo(int type, const WCHAR* wszKey)
		{
			nType = type;
			szKey = wszKey;
		}
	};

	struct DynamicReplaceStringInfo 
	{
		enum
		{
			STRING = 1,
			INT,
		};
		char		 cValueType; 
		std::wstring szKey;
		int			 nValue;
	};

	DNReplaceString();
	virtual ~DNReplaceString();

	// 문자열을 치환해서 완성 시켜준다.
	// ReplaceStaticString 를 먼저 호출해서 미리 지정된 치환을 시켜주고
	// ReplaceDynamicString 을 호출해서 유동적인 치환을 해준다. -_-;
#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	bool Relpace(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info, MultiLanguage::SupportLanguage::eSupportLanguage eNation);

	// 10번 몬스터의 이름
	// {%monster:10} 과 같이 미리 지정된 타입의 스트링을 치환 해준다.
	void ReplaceStaticString(IN OUT std::wstring& str, MultiLanguage::SupportLanguage::eSupportLanguage eNation);

	// uistring 에 따로 저장되어잇는 인덱스 == aaa  라고 외부에서 지정
	// {%aaa} 과 같이 유동적인 타입의 스트링을 치환 해준다.
	void ReplaceDynamicString(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info, MultiLanguage::SupportLanguage::eSupportLanguage eNation);
#else		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	bool Relpace(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info);

	// 10번 몬스터의 이름
	// {%monster:10} 과 같이 미리 지정된 타입의 스트링을 치환 해준다.
	void ReplaceStaticString(IN OUT std::wstring& str);

	// uistring 에 따로 저장되어잇는 인덱스 == aaa  라고 외부에서 지정
	// {%aaa} 과 같이 유동적인 타입의 스트링을 치환 해준다.
	void ReplaceDynamicString(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info);
#endif		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)

private:
#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	std::wstring _GetStaticString(int nType, std::vector<std::wstring>& tokens, MultiLanguage::SupportLanguage::eSupportLanguage eNation);
	std::wstring _GetString(int nStringID, MultiLanguage::SupportLanguage::eSupportLanguage eNation);
#else		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	std::wstring _GetStaticString(int nType, std::vector<std::wstring>& tokens);
	std::wstring _GetString(int nStringID);
#endif		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)

	std::vector<__StaticReplaceStringInfo>	m_StaticReplaceList;

};

extern DNReplaceString	g_ReplaceString;