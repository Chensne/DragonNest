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

	// ���ڿ��� ġȯ�ؼ� �ϼ� �����ش�.
	// ReplaceStaticString �� ���� ȣ���ؼ� �̸� ������ ġȯ�� �����ְ�
	// ReplaceDynamicString �� ȣ���ؼ� �������� ġȯ�� ���ش�. -_-;
#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	bool Relpace(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info, MultiLanguage::SupportLanguage::eSupportLanguage eNation);

	// 10�� ������ �̸�
	// {%monster:10} �� ���� �̸� ������ Ÿ���� ��Ʈ���� ġȯ ���ش�.
	void ReplaceStaticString(IN OUT std::wstring& str, MultiLanguage::SupportLanguage::eSupportLanguage eNation);

	// uistring �� ���� ����Ǿ��մ� �ε��� == aaa  ��� �ܺο��� ����
	// {%aaa} �� ���� �������� Ÿ���� ��Ʈ���� ġȯ ���ش�.
	void ReplaceDynamicString(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info, MultiLanguage::SupportLanguage::eSupportLanguage eNation);
#else		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	bool Relpace(IN OUT std::wstring& str, std::vector<DynamicReplaceStringInfo>& info);

	// 10�� ������ �̸�
	// {%monster:10} �� ���� �̸� ������ Ÿ���� ��Ʈ���� ġȯ ���ش�.
	void ReplaceStaticString(IN OUT std::wstring& str);

	// uistring �� ���� ����Ǿ��մ� �ε��� == aaa  ��� �ܺο��� ����
	// {%aaa} �� ���� �������� Ÿ���� ��Ʈ���� ġȯ ���ش�.
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