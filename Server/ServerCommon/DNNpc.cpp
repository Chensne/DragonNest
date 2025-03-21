#include "StdAfx.h"
#include "DNNpc.h"
#include "DnQuest.h"
#include "DnQuestManager.h"
#include "DNGameDataManager.h"

CDnNpc::CDnNpc()
{
}

CDnNpc::~CDnNpc()
{
}

bool CDnNpc::Create(TNpcData* pNpcData)
{
	std::wstring _wszTalkFileName;
	std::wstring _wszScriptFileName;

	WCHAR wszTalkFileName[512] = {0,};
	WCHAR wszScriptFileName[512] = {0,};

//	ZeroMemory(&wszTalkFileName, sizeof(WCHAR)*512);
//	MultiByteToWideChar(CP_ACP, 0, pNpcData->szName, -1, wszNPCName, 512 );

	ZeroMemory(&wszTalkFileName, sizeof(WCHAR)*512);
	if( pNpcData )
		MultiByteToWideChar(CP_ACP, 0, pNpcData->szTalkFile, -1, wszTalkFileName, 512 );

	ZeroMemory(&wszScriptFileName, sizeof(WCHAR)*512);
	if( pNpcData )
		MultiByteToWideChar(CP_ACP, 0, pNpcData->szScriptFile, -1, wszScriptFileName, 512 );

	_wszTalkFileName   = wszTalkFileName;
	_wszScriptFileName = wszScriptFileName;

	std::transform(_wszTalkFileName.begin(), _wszTalkFileName.end(), _wszTalkFileName.begin(), towlower); 
	std::transform(_wszScriptFileName.begin(), _wszScriptFileName.end(), _wszScriptFileName.begin(), towlower); 

	// 토크 파일 초기화
	if ( CDNTalk::Create(_wszTalkFileName.c_str(), _wszScriptFileName.c_str()) == false ) 
	{
		return false;
	}

	// 토크파일 초기화후 npc 토크를 토크맵에 등록
	TALK_MAP_IT iter = m_TalkMap.find(_wszTalkFileName);
	if ( iter != m_TalkMap.end() )
	{
		return false;
	}
	else
	{
		m_TalkMap.insert(make_pair(_wszTalkFileName, this));
	}

	return true;
}

bool CDnNpc::HasQuest(int nQuestIdx)
{
	if ( nQuestIdx < 1 )
		return false;

	for ( int i = 0 ; i < QUEST_MAX_CNT ; i++ )
	{
		if ( m_NpcData.QuestIndexArr[i] == nQuestIdx )
			return true;
	}

	return false;
}

