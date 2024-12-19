#pragma once

#include "DnDataManager.h"

inline void SendNpcTalk(UINT nUniqueID, const std::wstring& szIndex, const std::wstring& szTarget)
{
	CSNpcTalk talk;
	memset(&talk, 0, sizeof(CSNpcTalk));

	talk.nNpcUniqueID = nUniqueID;

	talk.dwIndexHashCode = g_DataManager.GetStringHashCode(szIndex.c_str());
	talk.dwTargetHashCode = g_DataManager.GetStringHashCode(szTarget.c_str());
	
	CClientSessionManager::GetInstance().SendPacket(CS_NPC, eNpc::CS_NPCTALK, (char*)&talk, sizeof(CSNpcTalk));
}

inline void SendCancelQuest(int nQuestID)
{
	CSCancelQuest cancel;
	memset(&cancel, 0 , sizeof(CSCancelQuest));

	cancel.nQuestIndex = nQuestID;

	CClientSessionManager::GetInstance().SendPacket(CS_QUEST, eQuest::CS_CANCEL_QUEST, (char*)&cancel, sizeof(CSCancelQuest));
}

inline void SendShortCutQuest(int nQuestID)
{
	CSShortCutQuest shortcutquest;
	memset(&shortcutquest, 0 , sizeof(CSShortCutQuest));
	shortcutquest.nQuestID = nQuestID;

	CClientSessionManager::GetInstance().SendPacket(CS_QUEST, eQuest::CS_SHORTCUT_QUEST, (char*)&shortcutquest, sizeof(CSShortCutQuest));
}

inline void SendSelectQuestReward(int nRewardIdx, bool bSelectedArray[], bool bSelectedCashArray[])
{
	CSSelectQuestReward selected;
	memset(&selected, 0 , sizeof(CSSelectQuestReward));

	selected.nIndex = nRewardIdx;
	CopyMemory( &(selected.SelectArray[0]), bSelectedArray, sizeof(bool)*QUESTREWARD_INVENTORYITEMMAX);
	CopyMemory( &(selected.SelectCashArray[0]), bSelectedCashArray, sizeof(bool)*MAILATTACHITEMMAX);
	
	CClientSessionManager::GetInstance().SendPacket(CS_QUEST, eQuest::CS_SELECT_QUEST_REWARD, (char*)&selected, sizeof(CSSelectQuestReward));
}

inline void SendNpcTalkEnd( UINT nUniqueID )
{
	CSNpcTalkEnd packet;
	memset( &packet, 0, sizeof(CSNpcTalkEnd) );

	packet.nNpcUniqueID = nUniqueID;

	CClientSessionManager::GetInstance().SendPacket( CS_NPC, eNpc::CS_NPCTALKEND, (char*)&packet, sizeof(CSNpcTalkEnd) );
}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

inline void SendGiveNpcPresent( UINT uiNpcID, int iPresentID, int iPresentCount )
{
	CSGiveNpcPresent Packet;
	memset( &Packet, 0, sizeof(CSGiveNpcPresent) );

	Packet.nNpcID = uiNpcID;
	Packet.nPresentID = iPresentID;
	Packet.nPresentCount = iPresentCount;

	CClientSessionManager::GetInstance().SendPacket( CS_REPUTATION, eReputation::CS_GIVENPCPRESENT, (char*)&Packet, sizeof(CSGiveNpcPresent) );
}

#endif // PRE_ADD_NPC_REPUTATION_SYSTEM