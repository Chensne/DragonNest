#include "stdafx.h"
#include "DnQuest.h"
#include "DnQuestManager.h"
#include "DNTableFile.h"
#include "EtResourceMng.h"
#include "VarArg.h"
#include "DNUserQuest.h"
#include "DNPeriodQuestSystem.h"

CDNQuestManager* g_pQuestManager = NULL;

CDNQuestManager::CDNQuestManager()
{

}

CDNQuestManager::~CDNQuestManager()
{
	RemoveAllQuest();
}

bool CDNQuestManager::LoadAllQuest(const char* szFileName, DnScriptManager* pScriptManager)
{
	RemoveAllQuest();

	// Quest
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TQUEST );

		if( !pSox )
		{
			g_Log.Log( LogType::_FILELOG, L"QuestTable.ext failed\r\n");
			return false;
		}

		for( int i = 0; i < pSox->GetItemCount() ; i++ )
		{
			CDNQuest::CREATE_INFO info;
			info.nQuestIndex = pSox->GetItemID(i);

			if( GetQuest(info.nQuestIndex) != NULL )
				return false;

			char szQuestName[256] = {0,};
			char szTalk[256] = {0,};
			char szScript[256] = {0,};

			if( pSox->GetFieldFromLablePtr(info.nQuestIndex, "_Name") == NULL )
				continue;

			_strcpy(szQuestName, _countof(szQuestName), pSox->GetFieldFromLablePtr(info.nQuestIndex, "_Name")->GetString(), (int)strlen(pSox->GetFieldFromLablePtr(info.nQuestIndex, "_Name")->GetString()));
			_strcpy(szTalk, _countof(szTalk), pSox->GetFieldFromLablePtr(info.nQuestIndex, "_QuestTalk")->GetString(), (int)strlen(pSox->GetFieldFromLablePtr(info.nQuestIndex, "_QuestTalk")->GetString()));
			_strcpy(szScript, _countof(szScript), pSox->GetFieldFromLablePtr(info.nQuestIndex, "_QuestScript")->GetString(), (int)strlen(pSox->GetFieldFromLablePtr(info.nQuestIndex, "_QuestScript")->GetString()));
			info.cQuestType = pSox->GetFieldFromLablePtr(info.nQuestIndex, "_QuestType")->GetInteger();		
			info.cRepeatable = pSox->GetFieldFromLablePtr(info.nQuestIndex, "_QuestRepeatable")->GetInteger();
			info.btLevelCapReward = pSox->GetFieldFromLablePtr(info.nQuestIndex, "_LevelCapReward")->GetInteger();

			WCHAR wszQuestName[256]= {0,};
			MultiByteToWideChar(CP_ACP, 0, szQuestName, -1, wszQuestName, 256 );

			WCHAR wszTalk[256]= {0,};
			MultiByteToWideChar(CP_ACP, 0, szTalk, -1, wszTalk, 256 );

			WCHAR wszScript[256]= {0,};
			MultiByteToWideChar(CP_ACP, 0, szScript, -1, wszScript, 256 );

			info.szQuestName = wszQuestName;
			info.szTalkFileName = wszTalk;
			info.szScriptFileName = wszScript;
			info.bCheckRemoteComplete = true;

			if( info.szQuestName.empty() || info.szTalkFileName.empty() || info.szScriptFileName.empty() )
				continue;

			CDNQuest* pQuest = new CDNQuest();
			pQuest->Create(info);

			AddQuest(info.nQuestIndex, pQuest);
			AddQuest(info.szTalkFileName, pQuest);
		}
	}

	return true;
}

void CDNQuestManager::DumpAllQuest(IN OUT std::vector<CDNQuest*>& questList)
{
	questList.clear();
	questList.reserve(m_QuestMap.size());

	for( QUEST_MAP_ITR iter = m_QuestMap.begin() ; iter != m_QuestMap.end() ; iter++ )
	{
		CDNQuest* pQuest = iter->second;

		if( pQuest )
		{
			questList.push_back(pQuest);
		}
	}
}

void CDNQuestManager::RemoveAllQuest()
{
	QUEST_MAP_ITR it = m_QuestMap.begin();
	for( it ; it != m_QuestMap.end() ; it++ )
	{
		CDNQuest* pQuest = it->second;
		SAFE_DELETE(pQuest);
	}
	m_QuestMap.clear();
	m_StrKeyQuestMap.clear();
	m_RepeatQuests.clear();
}

bool CDNQuestManager::AddQuest(int nQuestIndex, CDNQuest* pQuest)
{
	if( !pQuest )
	{
		ASSERT(0);
		return false;
	}

	QUEST_MAP_ITR iter = m_QuestMap.find(nQuestIndex);
	if( iter != m_QuestMap.end() )
		return false;

	m_QuestMap[nQuestIndex] = pQuest;

	if(pQuest->GetQuestInfo().cRepeatable)
		m_RepeatQuests.push_back(nQuestIndex);
	return true;
}

bool CDNQuestManager::AddQuest(std::wstring& szTalkFile, CDNQuest* pQuest)
{
	if( !pQuest )
		return false;

	STRKEY_QUEST_MAP_ITR iter = m_StrKeyQuestMap.find(szTalkFile);

	if( iter != m_StrKeyQuestMap.end() )
	{
		return false;
	}
	else
	{
		m_StrKeyQuestMap[szTalkFile] = pQuest;
		return true;
	}
}


/*
bool CDNQuestManager::RemoveQuest(int nQuestIndex)
{
	QUEST_MAP_ITR iter = m_QuestMap.find(nQuestIndex);
	if( iter == m_QuestMap.end() )
	{
		return false;
	}
	else
	{
		m_QuestMap.erase(iter);
		return true;
	}

	return false;
}
*/

CDNQuest* CDNQuestManager::GetQuest(int nQuestIndex)
{
	QUEST_MAP_ITR iter = m_QuestMap.find(nQuestIndex);
	if( iter == m_QuestMap.end() )
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}


CDNQuest* CDNQuestManager::GetQuest(std::wstring& szTalkFile)
{
	STRKEY_QUEST_MAP_ITR iter = m_StrKeyQuestMap.find(szTalkFile);
	if( iter == m_StrKeyQuestMap.end() )
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}

bool CDNQuestManager::bIsMainQuest( const int nQuestIndex )
{
	CDNQuest* pQuest = GetQuest( nQuestIndex );
	if( !pQuest )
		return false;

	return( pQuest->GetQuestInfo().cQuestType == QuestType_MainQuest );
}

bool CDNQuestManager::bIsSubQuest( const int nQuestIndex )
{
	CDNQuest* pQuest = GetQuest( nQuestIndex );
	if( !pQuest )
		return false;

	switch(pQuest->GetQuestInfo().cQuestType)
	{
	case QuestType_SubQuest:
	case QuestType_PeriodQuest:
#if defined(PRE_ADD_REMOTE_QUEST)
	case QuestType_RemoteQuest:
#endif
		return true;
	default:
		return false;
	}
	return false;
}

char CDNQuestManager::GetQuestType(const int nQuestIndex)
{
	CDNQuest* pQuest = GetQuest(nQuestIndex);
	if(!pQuest) {
//		DN_ASSERT(0,	CVarArgA<64>("Invalid! → !pQuest : %d", nQuestIndex));
		return QuestType_Min;
	}

	return pQuest->GetQuestInfo().cQuestType;
}

void CDNQuestManager::ResetRepeatQuest(CDNUserQuest* pQuest)
{
	if(!pQuest)
		return;

	for each(int index in m_RepeatQuests)
	{
		pQuest->SetCompleteQuestFlag(index, false);
	}
}

void CDNQuestManager::MarkingConditionalQuest(CDNUserQuest* pQuest)
{
	if(!pQuest)
		return;

	QUEST_MAP_ITR iter = m_QuestMap.begin();

	int nQuestIndex = 0;

	for(; iter != m_QuestMap.end(); iter++)
	{
		// 반복퀘스트 마킹을 풀어준다.
		if(iter->second->GetQuestInfo().cRepeatable == 1)
		{
			nQuestIndex = iter->second->GetQuestInfo().nQuestIndex;
			pQuest->SetCompleteQuestFlag(nQuestIndex, false);
		}
	}
}
