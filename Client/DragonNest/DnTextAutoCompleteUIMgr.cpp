#include "StdAfx.h"
#include "DnTextAutoCompleteUIMgr.h"

#ifdef PRE_PARTY_DB

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTextAutoCompleteUIMgr::CDnTextAutoCompleteUIMgr()
{

}

bool CDnTextAutoCompleteUIMgr::AddTextUnit(const wchar_t& filterToken, const std::wstring& text)
{
	if (filterToken == '\0' || text.empty())
		return false;

	m_TextMap.insert(std::make_pair(filterToken, text));

	return true;
}

bool CDnTextAutoCompleteUIMgr::AddTextUnit(const std::vector<SPartyTargetMapInfo>& mapInfoList)
{
	std::vector<SPartyTargetMapInfo>::const_iterator iter = mapInfoList.begin();
	for (; iter != mapInfoList.end(); ++iter)
	{
		const SPartyTargetMapInfo& info = (*iter);
		if (AddTextUnit(info.mapName[0], info.mapName) == false)
		{
			_ASSERT(0);
			continue;
		}
	}

	return true;
}

void CDnTextAutoCompleteUIMgr::Clear()
{
	m_TextMap.clear();
}

void CDnTextAutoCompleteUIMgr::GetAlmostMatchingString(std::wstring& result, const std::wstring& strName) const
{
	if (strName.size() < 1) return;

	const std::pair<std::multimap<wchar_t, std::wstring>::const_iterator, std::multimap<wchar_t, std::wstring>::const_iterator> range = m_TextMap.equal_range(strName[0]);
	result.clear();

	std::wstring::size_type longest = 0;
	std::multimap<wchar_t, std::wstring>::const_iterator it = range.first;
	for (; it != range.second; ++it)
	{
		const std::wstring& current = (*it).second;
		if (current == strName)
			return;

		std::wstring::size_type size = current.find(strName);
		if (size != std::wstring::npos && size >= longest)
			result = current;
	}
}

#endif // PRE_PARTY_DB