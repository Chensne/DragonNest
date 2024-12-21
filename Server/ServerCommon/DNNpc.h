#pragma once

#include "DNTalk.h"

class CDNUserBase;

class CDnNpc : public CDNTalk
{
public:
	enum NpcTypeEnum {
		ActorNpc,
		PropNpc,
	};

protected:
	// first - 대화파일네임, second
	typedef std::map<std::wstring, CDNTalk*>	TALK_MAP;
	typedef TALK_MAP::iterator					TALK_MAP_IT;

	TALK_MAP	m_TalkMap;
	TNpcData	m_NpcData;
	NpcTypeEnum m_NpcType;

public:
	CDnNpc();
	virtual ~CDnNpc();

	virtual bool Create(TNpcData* pNpcData);
	virtual TNpcData* GetNpcData() { return &m_NpcData; }
	virtual bool Talk(CDNUserBase* pUserBase, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget) { DN_RETURN(false); }

	bool HasQuest(int nQuestIdx);
	int GetNpcClassID() { return m_NpcData.nNpcID; }
	NpcTypeEnum GetNpcType() { return m_NpcType; }
};
