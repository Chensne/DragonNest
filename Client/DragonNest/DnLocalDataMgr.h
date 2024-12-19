#pragma once

#include "Singleton.h"


class CDnLocalDataMgr : public CSingleton<CDnLocalDataMgr>
{
public:
	CDnLocalDataMgr();
	virtual ~CDnLocalDataMgr();

	bool Load();
	bool Save();

#ifdef PRE_PARTY_DB
	void SetPartySearchHistoryWord(const std::wstring& word);
	const std::deque<std::wstring>& GetPartySearchHistoryWords() const { return m_PartySearchHistoryList; }
#endif
	bool SetValue(FILE *stream , WCHAR* wszString);
#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
	void SetForceGraphicOptionLow(bool bForceGraphicOptionLow);
	bool IsSetForceGraphicOptionLow() const { return m_bForceGraphicOptionLow; }
#endif
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	bool IsExistChatOption( int nChatTabType );
	bool GetChatOptionValue( int nChatTabType, int nChatType );
	void SetChatOptionValue( int nChatTabType, int nChatType, bool bChatTypeValue );
	void SetChatOption( int nChatTabType, int nChatTypeValue );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

private:
#ifdef PRE_PARTY_DB
	std::deque<std::wstring> m_PartySearchHistoryList;
#endif

#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
	bool m_bForceGraphicOptionLow;
#endif
	const std::wstring m_FileName;

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	enum { MAX_CHAT_TAB_TYPE = 6, MAX_CHAT_TAB_CHARACTER = 8 };
	std::vector<int>	m_vecChatOption;
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
};