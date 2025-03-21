#pragma once

class CDNQuest;
class DnScriptManager;
class CDNUserQuest;

class CDNQuestManager
{

private:
	typedef std::map<int, CDNQuest*>			QUEST_MAP;
	typedef QUEST_MAP::iterator					QUEST_MAP_ITR;
	typedef QUEST_MAP::const_iterator			QUEST_MAP_CTR;
	typedef std::map<std::wstring, CDNQuest*>	STRKEY_QUEST_MAP;
	typedef STRKEY_QUEST_MAP::iterator			STRKEY_QUEST_MAP_ITR;
	typedef STRKEY_QUEST_MAP::const_iterator	STRKEY_QUEST_MAP_CTR;
	typedef std::vector<int>					QUEST_VECTOR;

public: 
	CDNQuestManager();
	virtual ~CDNQuestManager();

	bool LoadAllQuest(const char* szFileName, DnScriptManager* pScriptManager);
	void RemoveAllQuest();

	void DumpAllQuest(IN OUT std::vector<CDNQuest*>& questList);
	bool AddQuest(int nQuestIndex, CDNQuest* pQuest);
	bool AddQuest(std::wstring& szTalkFile, CDNQuest* pQuest);

	//bool RemoveQuest(int nQuestIndex);
	CDNQuest* GetQuest(int nQuestIndex);
	CDNQuest* GetQuest(std::wstring& szTalkFile);

	bool bIsMainQuest( const int nQuestIndex );
	bool bIsSubQuest( const int nQuestIndex );

	char GetQuestType(const int nQuestIndex);

	void ResetRepeatQuest(CDNUserQuest* pQuest); //정착되면 MarkingConditionalQuest를 대체한다.
	void MarkingConditionalQuest(CDNUserQuest* pQuest);

private:
	QUEST_MAP			m_QuestMap;
	STRKEY_QUEST_MAP	m_StrKeyQuestMap;
	QUEST_VECTOR		m_RepeatQuests;
};

extern CDNQuestManager* g_pQuestManager;
