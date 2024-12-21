#include "StdAfx.h"
#include "FarmPVP.h"
#include "DnGameRoom.h"
#include "DNUserSession.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnPvPGameTask.h"
#include "DnDLGameTask.h"
#include "DnItemTask.h"
#include "DnPartyTask.h"
#include "DnDLPartyTask.h"
#include "DnGuildTask.h"
#include "DnPvPPartyTask.h"
#include "DnSkillTask.h"
#include "DnWorld.h"
#include "PerfCheck.h"
#include "GameListener.h"
#include "DNGameServerManager.h"
#include "DNRUDPGameServer.h"
#include "DNBackGroundLoader.h"
#include "EtCollisionMng.h"
#include "GameSendPacket.h"
#include "DnSkill.h"
#include "DNUserTcpConnection.h"
#include "DNIocpManager.h"
#include "DnWorldSector.h"
#include "DnWorldActProp.h"
#include "DNLogConnection.h"
#include "DNBreakIntoUserSession.h"
#include "DnBlow.h"
#include "ExceptionReport.h"
#include "DnWeapon.h"
#include "DnParts.h"
#include "DNServiceConnection.h"
#include "DnPlayerActor.h"
#include "NoticeSystem.h"

#include "DNDBConnectionManager.h"

#include "DnTaskFactory.hpp"
#include "DNMissionSystem.h"
#include "DnMonsterActor.h"
#include "DNDBConnection.h"
#include "CloseSystem.h"
#include "DNFriend.h"
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "ReputationSystemRepository.h"
#include "ReputationSystemEventHandler.h"
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "MasterRewardSystem.h"
#include "DNMasterConnectionManager.h"
#include "DNMasterConnection.h"
#include "DNFarmUserSession.h"
#if defined (PRE_ADD_BESTFRIEND)
#include "DNBestFriend.h"
#endif
#ifdef PRE_ADD_BEGINNERGUILD
#include "DNGuildSystem.h"
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
#include "DNChatTask.h"
#include "DNChatRoom.h"
#include "DNTimeEventSystem.h"
#include "PvPGameMode.h"
#if defined( PRE_ADD_VIP_FARM )
#include "DNCashRepository.h"
#endif // #if defined( PRE_ADD_VIP_FARM )
#include "DNGameDataManager.h"


FarmPVP::FarmPVP(void)
{
}


FarmPVP::~FarmPVP(void)
{
}

void FarmPVP::OnJoinPVP()
{

}

void FarmPVP::OnDie(DnActorHandle hActor, DnActorHandle hHitter)
{
	if( !hActor || !hActor->GetActorHandle() ) return;
	CDNGameRoom *pRoom = hActor->GetGameRoom();
	if (!pRoom) return;
	if (!pRoom->GetTaskMng()) return;
	
	//fix when die in normal dungeons
	//
	if(pRoom->GetFarmIndex() == NULL)
	{
		return;
	}

	int nItemID = 1;
	int nItemLevel = 0;
	int nItemPotential = 0;
	int nItemOption = 0;


	CDnItemTask *pTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
	if (!pTask) return;

	
	int randitem = rand() % 30 + 1; /// ?? test
	//get user session
	CDNUserSession* pSession = pRoom->GetUserSession(hActor->GetSessionID());
	//get item 
	CDnItem *DropItem = pTask->GetInventoryItem(pSession , randitem);
	

	//
	//GetInventoryItemCount

	if( DropItem == NULL ) 
		return;

	// remove item
	int Qty = DropItem->GetOverlapCount();
	if(Qty > 0)
	{
		CDNUserItem* pItem = pSession->GetItem();
		pItem->DeleteInventoryBySlot(randitem, Qty, DropItem->GetSerialID(), DBDNWorldDef::UseItem::Use);

		printf(" !!! CDNGameRoom::OnDie : drop item %d \n",DropItem->GetClassID());

		CMultiRoom *pMultiRoom = pTask->GetRoom();
		pTask->RequestDropItem(STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++, *(hActor->GetPosition()), DropItem->GetClassID() , _rand(pMultiRoom), Qty, 0);
	}

	wprintf(L" !!! CDNGameRoom::OnDie : %ws killed %ws \n",hActor->GetName(),hHitter->GetName());
	//end

	//announce
	WCHAR szMsg[CHATLENMAX] = { 0, };
	wsprintfW(szMsg,L"[ũ�� PVP] %ws �ոջ�ɱ�� %ws!",hHitter->GetName(),hActor->GetName());

	
	g_pMasterConnectionManager->SendNoticeFromClinet((WCHAR*)szMsg, (int)wcslen(szMsg)*sizeof(WCHAR));
}