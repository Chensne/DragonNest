#pragma once

#ifdef PRE_PARTY_DB

#include "DnPartyData.h"

class CDnTextAutoCompleteUIMgr
{
public:
	CDnTextAutoCompleteUIMgr();
	virtual ~CDnTextAutoCompleteUIMgr() {}

	bool AddTextUnit(const wchar_t& filterToken, const std::wstring& text);
	void Clear();
	bool IsEmpty() const { m_TextMap.empty(); }

	void GetAlmostMatchingString(std::wstring& result, const std::wstring& strName) const;
	bool AddTextUnit(const std::vector<SPartyTargetMapInfo>& mapInfoList);

private:
	std::multimap<wchar_t, std::wstring> m_TextMap;
};

#endif // PRE_PARTY_DB