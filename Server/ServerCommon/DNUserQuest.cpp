#include "Stdafx.h"
#include "DNUserQuest.h"
#include "DNUserSendManager.h"
#include "DNQuestManager.h"
#include "DNQuest.h"
#include "DNGameDataManager.h"
#include "DNLogConnection.h"
#include "DNDBConnectionManager.h"
#include "DNUserSession.h"
#include "DNMissionSystem.h"
#include "DNDBConnection.h"
#include "DNQuestCondition.h"
#include "DNPeriodQuestSystem.h"
#if defined( PRE_ADD_STAMPSYSTEM )
#include "DNStampSystem.h"
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(_VILLAGESERVER)
#include "DNScriptAPI.h"

extern TVillageConfig g_Config;

#elif defined(_GAMESERVER)
#include "DNGameRoom.h"
#include "DNActor.h"
#include "DNGameServerScriptAPI.h"
#endif

CDNUserQuest::CDNUserQuest(CDNUserSession *pSession)
{
	m_pSession = pSession;
	m_RewardQuestID = 0;
	m_RewardCheck = 0;
	m_pQuest = pSession->GetQuestData();

	memset(&m_PacketReward, 0x00, sizeof(m_PacketReward));
	memset(&m_QuestReward, 0, sizeof(m_QuestReward));
}

CDNUserQuest::~CDNUserQuest()
{
#if defined(PRE_ADD_REMOTE_QUEST)
	_ResetRemoteQuest();
#endif
}

int	CDNUserQuest::AddHuntingQuest(int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	if( _FindPlayingQuest(nQuestID) > -1 ) 
	{
#ifndef _FINAL_BUILD
		// 이미 진행중이다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"이미 진행중인 퀘스트 입니다. : %d" , nQuestID );
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
		return -2;
	}

	int nEmptyIdx = _FindEmptyPlayQuestIndex();
	if( nEmptyIdx == -1 )
	{
#ifndef _FINAL_BUILD
		// 더이상 퀘스트를 받을수 없다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"더이상 퀘스트를 받을 수 없습니다. 진행 중인 퀘스트를 완료하세요. : %d" , nQuestID );

		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
		return -1;
	}

	int nQuestType = g_pQuestManager->GetQuestType(nQuestID);
	if( nQuestType <= QuestType_Min || nQuestType >= QuestType_Max )
	{
#ifndef _FINAL_BUILD
		// 더이상 퀘스트를 받을수 없다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 타입 오류 : %d" , nQuestID );

		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
		return -1;
	}

	// 퀘스트조건을 검사합니다.	
	QuestCondition * pQuestCondition = g_pDataManager->GetQuestCondition( nQuestID );
	if( pQuestCondition )
	{
		if( pQuestCondition->Check( m_pSession ) == false )
		{
			g_Log.Log(LogType::_QUESTHACK, 0, m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), 0, L"[ADBID:%u, CDBID:%I64d] [AddQuest - ConditionCheck] [QID:%d]\r\n", 
				m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), nQuestID);

#ifdef _WORK
			std::wstring wszMsg;
			wszMsg = FormatW( L"퀘스트조건이 맞지않습니다");
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#else
			if( m_pSession->GetAccountLevel() == AccountLevel_QA )
			{
				std::wstring wszMsg;
				wszMsg = FormatW( L"퀘스트조건이 맞지않습니다");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
			}
			else
			{
				m_pSession->DetachConnection(L"AddQuest - ConditionCheck");
				return -1;
			}
#endif	// #ifdef _WORK
		}
	}

	if( nCountingSlot < 0 || nCountingSlot >= QUEST_EXTRACOUNT_MAX) return -3;
	// 각각의 변수의 범위 초과 체크 
	if( nCountingType < 1 || nCountingType >= CountingType_Max || nTargetCnt < 1 || nTargetCnt > 31000 || nCountingIndex < 0  ) return -4;

	// 퀘스트 추가 
	m_pQuest->Quest[nEmptyIdx].nQuestID = nQuestID;

	m_pQuest->Quest[nEmptyIdx].cQuestState = QuestState_Playing;
	m_pQuest->Quest[nEmptyIdx].nQuestStep = nQuestStep;
	m_pQuest->Quest[nEmptyIdx].cQuestJournal = nJournalStep;

	TCount* pCount = (TCount*)m_pQuest->Quest[nEmptyIdx].Extra;
	pCount[nCountingSlot].cType  = nCountingType;
	pCount[nCountingSlot].nIndex = nCountingIndex;
	pCount[nCountingSlot].nCnt = 0;
	pCount[nCountingSlot].nTargetCnt = nTargetCnt;

	CDNUserItem *pItem = m_pSession->GetItem();

	// 아이템 카운팅을 세어야 하는 경우에
	if( nCountingType == CountingType_Item && pItem )
	{
		// 이미 인벤토리에 가지고 있는 아이템이라면 
		int nIdx = pItem->GetInventoryItemCount( nCountingIndex );

		if( nIdx > 0 )
			OnAddItem(nCountingIndex, nIdx);

		// 혹은 이미 퀘스트인벤토리에 가지고 있는 아이템일 경우라면 
		nIdx = pItem->GetQuestInventoryItemCount( nCountingIndex );
		if( nIdx > 0 )
			OnAddItem(nCountingIndex,nIdx );
	}

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 추가 : %d" , nQuestID );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	SendQuestRefresh(RefreshType::ADD_QUEST, nQuestID);

	m_pSession->GetDBConnection()->QueryAddQuest( m_pSession, nEmptyIdx, m_pQuest->Quest[nEmptyIdx]);	// 디비저장
	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnQuestGain, 1, EventSystem::QuestID, nQuestID );

	return 1;
}

int CDNUserQuest::AddQuest(int nQuestID, bool bSendErrorMsg/*=false*/)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	if(IsClearQuest(nQuestID)) {
		g_Log.Log(LogType::_QUESTHACK, 0, m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), 0, L"[ADBID:%u, CDBID:%I64d] [AddQuest - IsClearQuest] [QID:%d]\r\n", 
			m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), nQuestID
			);

		m_pSession->DetachConnection(L"AddQuest - IsClearQuest");
		return -1;
	}

	if( _FindPlayingQuest(nQuestID) > -1 ) 
	{
#ifndef _FINAL_BUILD
		// 이미 진행중이다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"이미 진행중인 퀘스트 입니다. : %d" , nQuestID );
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
#if defined(PRE_ADD_REMOTE_QUEST)
		if(bSendErrorMsg)
			m_pSession->SendQuestResult(ERROR_QUEST_ALREADY_PLAYING);
#endif
		return -2;
	}

	int nEmptyIdx = _FindEmptyPlayQuestIndex();
	if( nEmptyIdx == -1 )
	{
#ifndef _FINAL_BUILD
		// 더이상 퀘스트를 받을수 없다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"더이상 퀘스트를 받을 수 없습니다. 진행 중인 퀘스트를 완료하세요. : %d" , nQuestID );

		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
#if defined(PRE_ADD_REMOTE_QUEST)
		if(bSendErrorMsg)
			m_pSession->SendQuestResult(ERROR_QUEST_FULLOFCOUNT_PLAYING);
#endif

		return -1;
	}

	int nQuestType = g_pQuestManager->GetQuestType(nQuestID);
	if( nQuestType <= QuestType_Min || nQuestType >= QuestType_Max )
	{
#ifndef _FINAL_BUILD
		// 더이상 퀘스트를 받을수 없다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 타입 오류 : %d" , nQuestID );

		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
		return -1;
	}

	// 퀘스트조건을검사합니다.	
	QuestCondition * pQuestCondition = g_pDataManager->GetQuestCondition( nQuestID );
	if( pQuestCondition )
	{
		if( pQuestCondition->Check( m_pSession ) == false )
		{
			g_Log.Log(LogType::_QUESTHACK, 0, m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), 0, L"[ADBID:%u, CDBID:%I64d] [AddQuest - ConditionCheck] [QID:%d]\r\n", 
				m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), nQuestID);

#ifdef _WORK
			std::wstring wszMsg;
			wszMsg = FormatW( L"퀘스트조건이 맞지않습니다");
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#else
			if( m_pSession->GetAccountLevel() == AccountLevel_QA )
			{
				std::wstring wszMsg;
				wszMsg = FormatW( L"퀘스트조건이 맞지않습니다");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
			}
			else
			{
				m_pSession->DetachConnection(L"AddQuest - ConditionCheck");
				return -1;
			}
#endif	// #ifdef _WORK
			
		}
	}

	// 퀘스트 추가 
	m_pQuest->Quest[nEmptyIdx].nQuestID = nQuestID;
	m_pQuest->Quest[nEmptyIdx].cQuestState = QuestState_Playing;

#if defined(PRE_ADD_REMOTE_QUEST)
	RemoveRemoteQuestFromAcceptWaitList(nQuestID);
#endif

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 추가 : %d" , nQuestID );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	SendQuestRefresh(RefreshType::ADD_QUEST, nQuestID);

	m_pSession->GetDBConnection()->QueryAddQuest(m_pSession, nEmptyIdx, m_pQuest->Quest[nEmptyIdx]);	// 디비저장
	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnQuestGain, 1, EventSystem::QuestID, nQuestID );

	return 1;
}


// 이 함수에서 모든 퀘스트 끝장을 본다!(일부 퀘는 ForceCompleteQuest로 처리한다.)
// 일단 절차식으로 만들어야 할것 같아서 다음에 소스 보시는 분에게는 죄송!
// 0. 보상없는 퀘스트 처리
// 1. 보상검사
// 2. 퀘스트 검사
// 3. 퀘스트 종료 마킹
// 4. 퀘스트 삭제
// 5. 보상처리
int CDNUserQuest::RewardAfterCompletingQuest(int nQuestID, bool bDelPlayList)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE)
	{
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	// 반복퀘스트 여부에 대해서 LUA스크립트값을 신뢰하지 말고 QuestTable 정보만을 신뢰한다.
	CDNQuest* pQuest = g_pQuestManager->GetQuest(nQuestID);
	if(!pQuest)
	{
		m_pSession->SendQuestResult(ERROR_QUEST_NOTFOUND);
		return -2;
	}

	bool bRepeat = (pQuest->GetQuestInfo().cRepeatable == 1) ? true : false;

//--------------------------------------------------------------------------------------------------------------------------
// 0. 보상이 없는 퀘스트처리(마킹->완료)
	if(m_RewardQuestID == 0 && m_PacketReward.nIndex == 0 )
	{
		// 반복퀘스트는 마킹하지 않는다.
		if(bRepeat == false)
			MarkingCompleteQuest(nQuestID);
		
		return CompleteQuest(nQuestID, bDelPlayList, bRepeat);
	}


//--------------------------------------------------------------------------------------------------------------------------
// 1. 보상검사

	// 선택인덱스검사(보상관련API에서 세팅하는변수와 클라이언트에서 오는패킷값을 비교하여 예외처리한다)
#if defined(PRE_FIX_QUEST_GUILD_BOARD)
	if( m_PacketReward.nIndex != m_QuestReward.nIndex || m_QuestReward.nQuestID != nQuestID )
#else
	if( m_PacketReward.nIndex != m_QuestReward.nIndex )
#endif
	{
		m_pSession->SendQuestResult(ERROR_QUEST_REWARD_FAIL);
		return -1;
	}	

#if defined(_VILLAGESERVER)
	UINT nUserID =  m_pSession->GetObjectID();
#elif defined(_GAMESERVER)
	UINT nUserID = m_pSession->GetSessionID();
#endif

	// 최대 코인 확인
	if(0 != m_QuestReward.nCoin && !m_pSession->CheckMaxCoin(m_QuestReward.nCoin))
	{
		m_pSession->SendQuestResult(ERROR_QUEST_REWARD_FAIL);
		return -1;
	}

	int nResult = CheckInvenForQuestReward(m_pSession, m_QuestReward, m_PacketReward.SelectArray);
	if (nResult < 1)
		return -1;

//--------------------------------------------------------------------------------------------------------------------------
// 2. 퀘스트 검사
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot == -1 || nSlot >= MAX_PLAY_QUEST )
	{
		m_pSession->SendQuestResult(ERROR_QUEST_NOTFOUND);
		return -2;
	}

#if defined(PRE_FIX_QUEST_GUILD_BOARD)
// 퀘스트 완료 스텝 검증
	if( GetQuestStep(nQuestID) != m_QuestReward.nQuestStep )
	{
		m_pSession->SendQuestResult(ERROR_QUEST_REWARD_FAIL);
		return -1;
	}
#endif

//--------------------------------------------------------------------------------------------------------------------------
// 3. 퀘스트 완료 마킹

	std::wstring wszMsg;

	// 반복퀘스트일 경우 마킹하지 않는다.
	if(bRepeat == false)
		MarkingCompleteQuest(nQuestID);

//--------------------------------------------------------------------------------------------------------------------------
// 4. 퀘스트 삭제
	CompleteQuest(nQuestID, bDelPlayList, bRepeat);
//--------------------------------------------------------------------------------------------------------------------------
// 5. 보상처리
	OnSelectRewardEx(m_PacketReward.SelectArray, m_PacketReward.SelectCashArray);

	return 1;
}

void CDNUserQuest::ResetRewardFlag()
{
	m_RewardQuestID = 0;
	m_RewardCheck = 0;

	memset(&m_PacketReward, 0x00, sizeof(m_PacketReward));
#if defined(PRE_FIX_QUEST_GUILD_BOARD)
	memset(&m_QuestReward, 0, sizeof(m_QuestReward));
#endif
}

int CDNUserQuest::CompleteQuest(int nQuestID, bool bDelPlayList, bool bRepeat)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot == -1 || nSlot >= MAX_PLAY_QUEST )
	{
		// 진행중인 퀘스트가 아님
		return -2;
	}

#ifndef _FINAL_BUILD
	// 퀘스트 완료 시킨후
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 완료 : %d" , nQuestID );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	// 현재 플레이 퀘스트 목록에서 제거
	if( bDelPlayList )
	{
		_RemoveQuest(nQuestID, true, bRepeat);

		// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
		m_pSession->SendCompleteQuest(true, nSlot, nQuestID, false);
#if defined( PRE_ADD_STAMPSYSTEM )
		m_pSession->GetStampSystem()->OnCompleteQuest( nQuestID );
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
		return 1;
	}

	m_pSession->SendCompleteQuest(false, nSlot, nQuestID, false);
#if defined( PRE_ADD_STAMPSYSTEM )
	m_pSession->GetStampSystem()->OnCompleteQuest( nQuestID );
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
	return 1;
}

int	CDNUserQuest::MarkingCompleteQuest(int nQuestID)
{
	/////////////////////////////////////////////////////////////////
	//방어 코드 입니다. 스크립트 단에서 호출이 되기 때문입니다.
	CDNQuest* pQuest = g_pQuestManager->GetQuest(nQuestID);
	if(!pQuest)
	{
		ASSERT(0);
		return -2;
	}

	if(pQuest->GetQuestInfo().cRepeatable == 1)
		return 1;
	/////////////////////////////////////////////////////////////////

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot == -1 || nSlot >= MAX_PLAY_QUEST )
	{
		// 진행중인 퀘스트가 아님
		return -2;
	}

#ifndef _FINAL_BUILD
	// 퀘스트 완료 시킨후
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 완료 마킹!: %d" , nQuestID );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	// 여기서 완료된 목록에 추가 해준다.
	SetCompleteQuestFlag(nQuestID, true);

	if (g_pPeriodQuestSystem->IsPeriodQuest(nQuestID))
		AddCompletedPeriodQuest(nQuestID);

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	m_pSession->SendMarkingCompleteQuest(nQuestID);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnQuestClear, 1,	EventSystem::QuestID, nQuestID );
	return 1;
}

int CDNUserQuest::ForceCompleteQuest(int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat)
{
	// 현재 진행 중인 퀘스트인지 여부는 상관 없음

	if(bDoMark) {
#ifndef _FINAL_BUILD
		// 퀘스트 완료 시킨후
		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 완료 마킹!: %d" , nQuestID );
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

		// 여기서 완료된 목록에 추가 해준다.
		SetCompleteQuestFlag(nQuestID, true);

		if (g_pPeriodQuestSystem->IsPeriodQuest(nQuestID))
			AddCompletedPeriodQuest(nQuestID);

		// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
		m_pSession->SendMarkingCompleteQuest(nQuestID);
	}

	int nSlot = _FindPlayingQuest(nQuestID);
	if(0 <= nSlot) 
	{
		int nResult = CompleteQuest(nQuestID, ((bDoDelete)?(true):(false)), ((bDoRepeat)?(true):(false)));

		if( nResult < 0 ) {
#ifndef _FINAL_BUILD
			wstring wszString = FormatW(L"퀘스트 완료에 실패하였습니다.\r\n");
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif	// _FINAL_BUILD
			return -3;
		}
	}
	else {
		m_pSession->GetDBConnection()->QueryForceCompleteQuest(m_pSession, nQuestID, nQuestCode, GetQuestCompleteBinary());
		
//		m_pSession->SendCompleteQuest(true, nSlot, nQuestID, false);	// 어차피 소유하지 않은 퀘스트 이므로 클라이언트에 퀘스트 완료 정보는 않보내도 됨 ???
	}

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnQuestClear, 1, EventSystem::QuestID, nQuestID );

	return 1;
}

int	CDNUserQuest::GetPlayingQuestCount()
{
	int nCnt = 0;
	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
#if defined(PRE_ADD_REMOTE_QUEST)
		if( m_pQuest->Quest[i].nQuestID > 0 && m_pQuest->Quest[i].cQuestState >= QuestState_Playing ) 
#else
		if( m_pQuest->Quest[i].nQuestID > 0 && m_pQuest->Quest[i].cQuestState == QuestState_Playing ) 
#endif
		{
			nCnt++;
		}
	}
	return nCnt;
}

short CDNUserQuest::HasQuest(int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}

	return m_pQuest->Quest[nSlot].nQuestStep;
}


int	CDNUserQuest::_RemoveQuest(int nQuestID, bool bComplete, bool bRepeat)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}
	
#if defined(_VILLAGESERVER)
	UINT nUserID =  m_pSession->GetObjectID();
#elif defined(_GAMESERVER)
	UINT nUserID = m_pSession->GetSessionID();
#endif

	//---------------------------------------------------------------------------------------------
	{
		// 여기는 퀘스트아이템 지우는거
		std::set<int> questItemSet;
		g_pDataManager->GetQuestItemSet(nQuestID, questItemSet);

		// 취소하는 퀘스트와 관련된 심볼은 모두 지움
		std::set<int>::iterator it = questItemSet.begin();
		for( it ; it != questItemSet.end() ; it++ )
		{
			int nQuestItemIndex = *it;
#if defined(_VILLAGESERVER)
			int nCount = api_quest_HasQuestItem(nUserID, nQuestItemIndex, 1);
#elif defined(_GAMESERVER)
			int nCount = api_quest_HasQuestItem(m_pSession->GetGameRoom(), nUserID, nQuestItemIndex, 1);
#endif	// _VILLAGESERVER
			if( nCount > 0 )
			{
#if defined(_VILLAGESERVER)
				int nReturn = api_quest_DelQuestItem(nUserID, nQuestItemIndex, nCount, nQuestID);
#elif defined(_GAMESERVER)
				int nReturn = api_quest_DelQuestItem(m_pSession->GetGameRoom(), nUserID, nQuestItemIndex, nCount, nQuestID);
#endif	// _VILLAGESERVER
				//if(nReturn < 0 ) return nReturn;
			}
			// 0 보다 작다면 에러.
			//else return nCount;
		}
	}

	// 퀘스트 지움
	m_pQuest->Quest[nSlot].nQuestID = 0;
	m_pQuest->Quest[nSlot].cQuestState = QuestState_None;

	ZeroMemory( &(m_pQuest->Quest[nSlot]), sizeof(TQuest));

	m_pSession->GetDBConnection()->QueryDelQuest( m_pSession, nSlot, nQuestID, bComplete, bRepeat, m_pQuest->CompleteQuest);

	if(bComplete)
	{
		int nScheduleID = -1;
		__time64_t tExpireDate = 0;
		if(g_pPeriodQuestSystem->FindEventScheduleID(nQuestID, nScheduleID, tExpireDate) && nScheduleID > 0 && tExpireDate > 0)
			m_pSession->GetDBConnection()->QueryCompleteEventQuest(m_pSession, nScheduleID, tExpireDate);
	}

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 제거 : %d" , nQuestID );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	return nSlot;
}

int CDNUserQuest::_FindPlayingQuest(int nQuestID)
{
	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
#if defined(PRE_ADD_REMOTE_QUEST)
		if( m_pQuest->Quest[i].nQuestID == nQuestID && m_pQuest->Quest[i].cQuestState >= QuestState_Playing ) 
#else
		if( m_pQuest->Quest[i].nQuestID == nQuestID && m_pQuest->Quest[i].cQuestState == QuestState_Playing ) 
#endif
			return i;
	}

	return -1;
}

int CDNUserQuest::_FindEmptyPlayQuestIndex()
{
	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		if( m_pQuest->Quest[i].nQuestID < 1 ) 
			return i;
	}
	return -1;
}

int	CDNUserQuest::SetQuestStepAndJournalStep(int nQuestID, short nQuestStep, int nJournalStep, bool bFromScript)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  ) // 퀘스트를 찾을수 없다.
		return -1;

	// 퀘스트 스텝 갱신하고..
	m_pQuest->Quest[nSlot].nQuestStep = nQuestStep;
	m_pQuest->Quest[nSlot].cQuestJournal = nJournalStep;

#ifndef _FINAL_BUILD
	// 이거 클라이언트 한테도 알려줘야 갱신될텐데.
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 저널 스텝 변경 : %d step: %d, journal: %d" , nQuestID, (int)nQuestStep, (int)nJournalStep );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	SendQuestRefresh(RefreshType::REFRESH_STEPANDJOURNAL, nQuestID);

#if defined( PRE_ADD_REMOTE_QUEST )
	CDNQuest * pQuest = g_pQuestManager->GetQuest( nQuestID );
	if( bFromScript && pQuest && QuestType_RemoteQuest == pQuest->GetQuestInfo().cQuestType && pQuest->GetQuestInfo().bCheckRemoteComplete )
	{
#if defined(_VILLAGESERVER)
		if( pQuest->CanRemoteCompleteStep( m_pSession->GetObjectID(), nQuestStep ) )
#elif defined(_GAMESERVER)
		if( pQuest->CanRemoteCompleteStep( m_pSession->GetGameRoom(), m_pSession->GetSessionID(), nQuestStep ) )
#endif	// _VILLAGESERVER
		{
			m_pQuest->Quest[nSlot].cQuestState = QuestState_Recompense;
			m_pSession->SendCompleteRemoteQuest( nQuestID );
		}
	}
#endif // #if defined( PRE_ADD_REMOTE_QUEST )

	m_pSession->GetDBConnection()->QueryModQuest(m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 100128

	return 1;
}

int CDNUserQuest::SetQuestStep(int nQuestID, short nQuestStep)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -1;
	}

	// 퀘스트 스텝 갱신하고..
	m_pQuest->Quest[nSlot].nQuestStep = nQuestStep;

#ifndef _FINAL_BUILD
	// 이거 클라이언트 한테도 알려줘야 갱신될텐데.
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 스텝 변경 : %d step: %d" , nQuestID, (int)nQuestStep );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	SendQuestRefresh(RefreshType::REFRESH_STEP, nQuestID);

#if defined( PRE_ADD_REMOTE_QUEST )
	CDNQuest * pQuest = g_pQuestManager->GetQuest( nQuestID );
	if( pQuest && QuestType_RemoteQuest == pQuest->GetQuestInfo().cQuestType && pQuest->GetQuestInfo().bCheckRemoteComplete )
	{
#if defined(_VILLAGESERVER)
		if( pQuest->CanRemoteCompleteStep( m_pSession->GetObjectID(), nQuestStep ) )
#elif defined(_GAMESERVER)
		if( pQuest->CanRemoteCompleteStep( m_pSession->GetGameRoom(), m_pSession->GetSessionID(), nQuestStep ) )
#endif	// _VILLAGESERVER
		{
			m_pQuest->Quest[nSlot].cQuestState = QuestState_Recompense;
			m_pSession->SendCompleteRemoteQuest( nQuestID );
		}
	}
#endif // #if defined( PRE_ADD_REMOTE_QUEST )

	m_pSession->GetDBConnection()->QueryModQuest(m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 100128

	return 1;
}

short CDNUserQuest::GetQuestStep(int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}

	// 퀘스트 스텝 갱신하고..
	return m_pQuest->Quest[nSlot].nQuestStep;
}

int	CDNUserQuest::SetJournalStep(int nQuestID, int nJournalStep)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -1;
	}

	// 퀘스트 스텝 갱신하고..
	m_pQuest->Quest[nSlot].cQuestJournal = nJournalStep;

#ifndef _FINAL_BUILD
	// 이거 클라이언트 한테도 알려줘야 갱신될텐데.
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 저널 스텝 변경 : %d step: %d" , nQuestID, (int)nJournalStep );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	SendQuestRefresh(RefreshType::REFRESH_JOURNAL, nQuestID);

	m_pSession->GetDBConnection()->QueryModQuest(m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 100128
	
	return 1;
}

int CDNUserQuest::GetJournalStep(int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}

	// 퀘스트 스텝 갱신하고..
	return m_pQuest->Quest[nSlot].cQuestJournal;
}

int CDNUserQuest::SetQuestMemo(int nQuestID, char nMemoIndex, TP_QUESTMEMO nVal)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	if(!CHECK_LIMIT(nMemoIndex, QUESTMEMOMAX)) {
		// 퀘스트 메모 인덱스 범위를 벗어남
		DN_RETURN(-3);
	}

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		DN_RETURN(-2);
	}

	// 퀘스트 메모 갱신
	m_pQuest->Quest[nSlot].nMemo[nMemoIndex] = nVal;

#ifndef _FINAL_BUILD
	// 이거 클라이언트 한테도 알려줘야 갱신될텐데.
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 메모 변경 : %d Memo: %d / %d" , nQuestID, (int)nMemoIndex, nVal );
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	m_pSession->GetDBConnection()->QueryModQuest( m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 20100128

	return 1;
}

TP_QUESTMEMO CDNUserQuest::GetQuestMemo(int nQuestID, char nMemoIndex)
{
	if(!CHECK_LIMIT(nMemoIndex, QUESTMEMOMAX)) {
		// 퀘스트 메모 인덱스 범위를 벗어남
		DN_RETURN(-3);
	}

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		DN_RETURN(-2);
	}

	return(m_pQuest->Quest[nSlot].nMemo[nMemoIndex]);
}

int	CDNUserQuest::IncQuestMemo(int nQuestID, char nMemoIndex)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return LONG_MIN;
	}
#endif

	if(!CHECK_LIMIT(nMemoIndex, QUESTMEMOMAX)) {
		// 퀘스트 메모 인덱스 범위를 벗어남
		DN_RETURN(LONG_MIN);
	}

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		DN_RETURN(LONG_MIN);
	}

	// 퀘스트 메모 증가
	TP_QUESTMEMO nRetVal = ++m_pQuest->Quest[nSlot].nMemo[nMemoIndex];

#ifndef _FINAL_BUILD
	// 이거 클라이언트 한테도 알려줘야 갱신될텐데.
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 메모 증가 : %d Memo: %d / %d" , nQuestID, (int)nMemoIndex, nRetVal);
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	m_pSession->GetDBConnection()->QueryModQuest(m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 20100128

	return nRetVal;
}

int	CDNUserQuest::DecQuestMemo(int nQuestID, char nMemoIndex)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return LONG_MAX;
	}
#endif

	if(!CHECK_LIMIT(nMemoIndex, QUESTMEMOMAX)) {
		// 퀘스트 메모 인덱스 범위를 벗어남
		DN_RETURN(LONG_MAX);
	}

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		DN_RETURN(LONG_MAX);
	}

	// 퀘스트 메모 감소
	TP_QUESTMEMO nRetVal = --m_pQuest->Quest[nSlot].nMemo[nMemoIndex];

#ifndef _FINAL_BUILD
	// 이거 클라이언트 한테도 알려줘야 갱신될텐데.
	std::wstring wszMsg;
	wszMsg = FormatW( L"퀘스트 메모 감소 : %d Memo: %d / %d" , nQuestID, (int)nMemoIndex, nRetVal);
	m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD

	m_pSession->GetDBConnection()->QueryModQuest(m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 20100128

	return nRetVal;
}

int	CDNUserQuest::SetCountingInfo(int nQuestID, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}

	if( nCountingSlot < 0 || nCountingSlot >= QUEST_EXTRACOUNT_MAX)
	{
		return -3;
	}

	// 각각의 변수의 범위 초과 체크 
	if( nCountingType < 1 || nCountingType >= CountingType_Max || nTargetCnt < 1 || nTargetCnt > 31000 || nCountingIndex < 0  )
	{
		return -4;
	}

	TCount* pCount = (TCount*)m_pQuest->Quest[nSlot].Extra;
	pCount[nCountingSlot].cType  = nCountingType;
	pCount[nCountingSlot].nIndex = nCountingIndex;
	pCount[nCountingSlot].nCnt = 0;
	pCount[nCountingSlot].nTargetCnt = nTargetCnt;

	CDNUserItem* pItem = NULL;
	m_pSession->GetDBConnection()->QueryModQuest( m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 20100128

	pItem = m_pSession->GetItem();

	// 아이템 카운팅을 세어야 하는 경우에
	if( nCountingType == CountingType_Item && pItem )
	{
		// 이미 인벤토리에 가지고 있는 아이템이라면 
		int nIdx = pItem->GetInventoryItemCount( nCountingIndex );
		
		if( nIdx > 0 )
		{
			OnAddItem(nCountingIndex, nIdx);
		}

		// 혹은 이미 퀘스트인벤토리에 가지고 있는 아이템일 경우라면 
		nIdx = pItem->GetQuestInventoryItemCount( nCountingIndex );
		if( nIdx > 0 )
		{
			OnAddItem(nCountingIndex,nIdx );
		}

	}

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	SendQuestRefresh(RefreshType::REFRESH_COUNTING, nQuestID);

	return 1;
}

int CDNUserQuest::ClearCountingInfo(int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}

	ZeroMemory( &(m_pQuest->Quest[nSlot].Extra), ExtraSize);
	m_pSession->GetDBConnection()->QueryModQuest( m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 20100128

	// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
	SendQuestRefresh(RefreshType::REFRESH_COUNTING, nQuestID);

	return 1;
}

int CDNUserQuest::IsAllCompleteCounting(int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}

	if( m_pQuest->Quest[nSlot].cQuestState == QuestState_Playing )
	{
		bool CompleteAllCounting = false;

		TCount* pCount = (TCount*)m_pQuest->Quest[nSlot].Extra;

		for( int j = 0 ; j < QUEST_EXTRACOUNT_MAX ; j++ )
		{
			if( pCount[j].cType != CountingType_None && pCount[j].cType != CountingType_Max )
			{
				// 하나라도 목표를 다 못채운게 있을 경우 
				if( pCount[j].nCnt < pCount[j].nTargetCnt )
				{
					return -3;
				}
				else if( pCount[j].nCnt == pCount[j].nTargetCnt )
				{
					CompleteAllCounting = true;
				}
			}
		}

		// 현재 퀘스트의 카운팅 조건이 전부 맞을때 
		if( CompleteAllCounting )
		{
			return 1;
		}
	}

	return -3;

}

#if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
void CDNUserQuest::OnCounting(int nCountingType, int nCountItemID, int nItemCount)
#else
void CDNUserQuest::OnCounting(int nCountingType, int nCountItemID)
#endif // #if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		//m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return;
	}
#endif

	// P.S.> 본 메서드의 루틴 맨 아래부분에 TQuest::Extra 에 갱신이 필요한 경우 DB 에 일괄 기록하는 부분이 있으므로 루틴 중간에 return 하지 않도록 주의 요망

	bool bCounting[MAX_PLAY_QUEST] = {0,};
	bool bComplete[MAX_PLAY_QUEST] = {0,};
	bool bModifyQuestExtra[MAX_PLAY_QUEST] = { false, };

	// 몬스터를 죽였을땐 몬스터 카운팅을 증가 시킨다.
	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		if( m_pQuest->Quest[i].cQuestState == QuestState_Playing )
		{
			TCount* pCount = (TCount*)m_pQuest->Quest[i].Extra;

			for( int j = 0 ; j < QUEST_EXTRACOUNT_MAX ; j++ )
			{
				// 카운팅 할 타입이 몬스터 이고
				bool bNeedContinue = false;
				bool bNeedCount = true;

				// 카운팅 타입이 내가 죽인 녀석만 카운팅 할때 내가 몬스터를 죽였을때.
				if( pCount[j].cType == CountingType_MyMonsterKill && nCountingType == CountingType_MyMonsterKill )
				{
					bNeedContinue = false;
				}
				// 전체 카운팅을 하게 세팅 되어있고 몬스터가 누군가에 의해 죽었을때( 나를 포함한 )
				else if( pCount[j].cType == CountingType_AllMonsterKill && nCountingType == CountingType_AllMonsterKill )
				{
					bNeedContinue = false;
				}
				// 아이템 
				else if( pCount[j].cType == CountingType_Item && nCountingType == CountingType_Item )
				{
					bNeedContinue = false;
				}
				/*
				// 심볼 아이템 
				else if( pCount[j].cType == CountingType_SymbolItem && nCountingType == CountingType_SymbolItem )
				{
					bNeedContinue = false;
				}
				*/
				else if( pCount[j].cType == CountingType_StageClear && nCountingType == CountingType_StageClear )
				{
					bNeedContinue = false;
					bNeedCount = false;
				}
				// 그외 경우라면 카운팅 하지 않는다.
				else
				{
					bNeedContinue = true;
				}

				if( bNeedContinue )
					continue;

				//그 인덱스 일때. 증가 시킨다.
				if( pCount[j].nIndex == nCountItemID )
				{
#if defined( PRE_MOD_QUEST_COUNTING )
					if( pCount[j].nCnt < pCount[j].nTargetCnt ) // 카운트 대상이 여러개 이고, 지정한 카운트 이상 기록 할 경우 진행률이 잘못 표기된다.
#else // #if defined( PRE_MOD_QUEST_COUNTING )
					// OnCounting에 따라 다음 진행에 문제가 될 경우를 고려하여 조건을 제거한다.
#endif // #if defined( PRE_MOD_QUEST_COUNTING )
					{
						bCounting[i] = true;
						if(bNeedCount)
						{
#if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
							if( nCountingType == CountingType_Item && nItemCount > 0) // 혹시 모르니 아이템 카운트만 체크합니다.
								pCount[j].nCnt += nItemCount;
							else
#endif //#if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
								++pCount[j].nCnt;

							bModifyQuestExtra[i] = true; 
						}
						// 스크립트를 호출해준다.
						CDNQuest* pQuest = g_pQuestManager->GetQuest(m_pQuest->Quest[i].nQuestID);
						if( pQuest )
						{
#if defined(_VILLAGESERVER)
							pQuest->OnCounting(GetUserObjID(), pCount[j]);
#elif defined(_GAMESERVER)
							pQuest->OnCounting(m_pSession->GetGameRoom(), GetUserObjID(), pCount[j]);
#endif	// _VILLAGESERVER
							// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
							SendQuestRefresh(RefreshType::REFRESH_COUNTING, m_pQuest->Quest[i].nQuestID);
						}
					}

#if defined ( PRE_MOD_QUEST_COUNTING )
					if( pCount[j].nCnt >= pCount[j].nTargetCnt && true == bCounting[i] ) // OnCounting을 호출하지 않았다면 호출하지 않는다.
#else // #if defined( PRE_MOD_QUEST_COUNTING )
					if( pCount[j].nCnt >= pCount[j].nTargetCnt )
#endif // #if defined( PRE_MOD_QUEST_COUNTING )
					{
						bCounting[i] = true;
						bComplete[i] = true;
						// 스크립트를 호출해준다.
						CDNQuest* pQuest = g_pQuestManager->GetQuest(m_pQuest->Quest[i].nQuestID);
						if( pQuest )
						{
#if defined(_VILLAGESERVER)
							pQuest->OnCompleteCounting(GetUserObjID(), pCount[j] );
#elif defined(_GAMESERVER)
							pQuest->OnCompleteCounting(m_pSession->GetGameRoom(), GetUserObjID(), pCount[j] );
#endif	// _VILLAGESERVER
							// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
							SendQuestRefresh(RefreshType::REFRESH_COMPLETECOUNTING, m_pQuest->Quest[i].nQuestID);
						}
					}
				}
			}
		}
	}

	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		if( bComplete[i] && bCounting[i] &&
			 IsAllCompleteCounting(m_pQuest->Quest[i].nQuestID) > 0 )
		{
			// 스크립트를 호출해준다.
			CDNQuest* pQuest = g_pQuestManager->GetQuest(m_pQuest->Quest[i].nQuestID);
			if( pQuest )
			{
#if defined(_VILLAGESERVER)
				pQuest->OnAllCompleteCounting(GetUserObjID());
#elif defined(_GAMESERVER)
				pQuest->OnAllCompleteCounting(m_pSession->GetGameRoom(), GetUserObjID());
#endif	// _VILLAGESERVER
				// 클라이언트에게 갱신하라고 퀘스트 구조체 전송
				SendQuestRefresh(RefreshType::REFRESH_ALLCOMPLETECOUNTING, m_pQuest->Quest[i].nQuestID);
			}
		}
	}

	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ ) {
		if(bModifyQuestExtra[i]) {
			m_pSession->GetDBConnection()->QueryModQuest(m_pSession, i, m_pQuest->Quest[i], DBDNWorldDef::QuestStatusCode::Progress);	// db저장 100128
		}
	}
}

// 내가 몬스터를 죽였을때.
void CDNUserQuest::OnKillMonster(int nMonsterID)
{
	OnCounting(CountingType_MyMonsterKill, nMonsterID);
}

// 몬스터가 죽었을떄.
void CDNUserQuest::OnDieMonster(int nMonsterID)
{
	OnCounting(CountingType_AllMonsterKill, nMonsterID);
}

// 아이템을 구했을때.
void CDNUserQuest::OnAddItem(int nItemID, short wCount)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return ;
	}
#endif

#if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
	OnCounting(CountingType_Item, nItemID, wCount);
#else
	for( short i = 0 ; i < wCount ; i++ )
	{
		OnCounting(CountingType_Item, nItemID);
	}
#endif // #if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
}

// 아이템을 구했을때.
void CDNUserQuest::OnAddSymbolItem(int nItemID)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return ;
	}
#endif

	OnCounting(CountingType_SymbolItem, nItemID);
}

void CDNUserQuest::OnAddQuestItem(int nItemID)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return ;
	}
#endif

	OnCounting(CountingType_Item, nItemID);
}

void CDNUserQuest::OnStageClear(int nRank)
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return ;
	}
#endif

	OnCounting(CountingType_StageClear, nRank);
}

UINT CDNUserQuest::GetUserObjID()
{
#if defined(_VILLAGESERVER)
	return m_pSession->GetObjectID();
#elif defined(_GAMESERVER)
	return m_pSession->GetSessionID();
#endif
}

bool CDNUserQuest::IsClearQuest(int nQuestID)
{
	if( nQuestID < 0 || nQuestID >= MAX_QUEST_INDEX )
		return false;

	int nIndex = nQuestID / 8;
	int nMaskType = nQuestID % 8;
	char Result = 0;

	switch(nMaskType)
	{
	case 1: Result = m_pQuest->CompleteQuest[nIndex] & 0x80; break;
	case 2: Result = m_pQuest->CompleteQuest[nIndex] & 0x40; break;
	case 3: Result = m_pQuest->CompleteQuest[nIndex] & 0x20; break;
	case 4: Result = m_pQuest->CompleteQuest[nIndex] & 0x10; break;
	case 5: Result = m_pQuest->CompleteQuest[nIndex] & 0x08; break;
	case 6: Result = m_pQuest->CompleteQuest[nIndex] & 0x04; break;
	case 7: Result = m_pQuest->CompleteQuest[nIndex] & 0x02; break;
	case 0: Result = m_pQuest->CompleteQuest[nIndex] & 0x01; break;		
	}

	if(Result == 0x00) return false;

	return true;
}

void CDNUserQuest::SetCompleteQuestFlag(int nQuestID, bool bFlag)
{
	int nIndex = nQuestID / 8;
	int nMaskType = nQuestID % 8;

	if(nIndex >= COMPLETEQUEST_BITSIZE) return; 	

	if( bFlag )
	{
		switch(nMaskType)
		{
		case 1: m_pQuest->CompleteQuest[nIndex] |= 0x80; break;
		case 2: m_pQuest->CompleteQuest[nIndex] |= 0x40; break;
		case 3: m_pQuest->CompleteQuest[nIndex] |= 0x20; break;
		case 4: m_pQuest->CompleteQuest[nIndex] |= 0x10; break;
		case 5: m_pQuest->CompleteQuest[nIndex] |= 0x08; break;
		case 6: m_pQuest->CompleteQuest[nIndex] |= 0x04; break;
		case 7: m_pQuest->CompleteQuest[nIndex] |= 0x02; break;
		case 0: m_pQuest->CompleteQuest[nIndex] |= 0x01; break;		
		}
	}
	else
	{
		switch(nMaskType)
		{
		case 1: m_pQuest->CompleteQuest[nIndex] &= ~0x80; break;
		case 2: m_pQuest->CompleteQuest[nIndex] &= ~0x40; break;
		case 3: m_pQuest->CompleteQuest[nIndex] &= ~0x20; break;
		case 4: m_pQuest->CompleteQuest[nIndex] &= ~0x10; break;
		case 5: m_pQuest->CompleteQuest[nIndex] &= ~0x08; break;
		case 6: m_pQuest->CompleteQuest[nIndex] &= ~0x04; break;
		case 7: m_pQuest->CompleteQuest[nIndex] &= ~0x02; break;
		case 0: m_pQuest->CompleteQuest[nIndex] &= ~0x01; break;		
		}
	}
}

// 상태가 변경된 퀘스트정보를 클라이언트에게 갱신 시켜준다.
int CDNUserQuest::SendQuestRefresh(int nRefreshType, int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0  || nSlot > MAX_PLAY_QUEST )
	{
		// 퀘스트를 찾을수 없다.
		return -2;
	}

	m_pSession->SendQuestRefresh((char)nSlot, nRefreshType, nQuestID, m_pQuest->Quest[nSlot] );
	return 1;
}

void CDNUserQuest::SendAllQuestRefresh()
{
	m_pSession->SendQuestInfo(m_pSession->GetQuestData());
	m_pSession->SendQuestCompleteInfo(*m_pQuest);
}

void CDNUserQuest::ClearAllQuestInfo()
{
	DN_ASSERT(NULL != m_pQuest,	"Invalid!");

	::memset(m_pQuest->Quest, 0, sizeof(m_pQuest->Quest));
	::memset(m_pQuest->CompleteQuest, 0, sizeof(m_pQuest->CompleteQuest));

	m_pSession->GetDBConnection()->QueryClearQuest(m_pSession);
}

void CDNUserQuest::DumpQuest(std::vector<TQuest>& output)
{
	for (int i = 0; i < MAX_PLAY_QUEST; i++)
	{
		if(m_pQuest->Quest[i].nQuestID > 0 ) 
		{
			output.push_back(m_pQuest->Quest[i]);
		}
	}
}

int CDNUserQuest::FindPlayingQuest(int nQuestID)
{
	DN_ASSERT(0 != nQuestID,	"Invalid!");

	return _FindPlayingQuest(nQuestID);
}

#if defined(PRE_ADD_REMOTE_QUEST)
int CDNUserQuest::FindRecompenseQuestIndex(int nQuestID)
{
	DN_ASSERT(0 != nQuestID,	"Invalid!");

	return _FindRecompenseQuestIndex(nQuestID);
}

bool CDNUserQuest::SetQuestToRecompenseState(int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0 ) return false;

	if( m_pQuest->Quest[nSlot].nQuestID != nQuestID ) return false;
	
	m_pQuest->Quest[nSlot].cQuestState = QuestState_Recompense;
	//SendQuestRefresh(RefreshType::REFRESH_STEPANDJOURNAL, nQuestID);
	m_pSession->GetDBConnection()->QueryModQuest(m_pSession, nSlot, m_pQuest->Quest[nSlot], DBDNWorldDef::QuestStatusCode::Progress);
	return true;
}

#endif

void CDNUserQuest::OnSetActiveQuest(int nQuestID)
{
	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0 )
	{
		m_pSession->SendQuestResult(ERROR_QUEST_SETACTIVEFAIL);
		return;
	}
}

// 진행중인 서브 퀘스트중에 가장 높은 퀘스트 스탭을 찾는다. return QuestID; 없으면 -1
TQuest	CDNUserQuest::_FindHighestQStepSubQuest()
{
	TQuest FoundQuest;
	ZeroMemory(&FoundQuest, sizeof(TQuest));

	int nQuest = -1;
	int nMaxQuestStep = -1;

	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		// 서브퀘스트 이고, 
		if( m_pQuest->Quest[i].nQuestID > 0 
			&& g_pQuestManager->GetQuestType(m_pQuest->Quest[i].nQuestID) == QuestType_SubQuest 
			 && m_pQuest->Quest[i].nQuestStep > 0 
			 == false )
		{
			if( nMaxQuestStep <= m_pQuest->Quest[i].nQuestStep )
			{
				nQuest = m_pQuest->Quest[i].nQuestID;
				nMaxQuestStep = m_pQuest->Quest[i].nQuestStep;

				FoundQuest = m_pQuest->Quest[i];
			}
		}
	}

	return FoundQuest;
}

// 퀘스트 보상을 세팅한다.
void CDNUserQuest::SetReward(TQuestReward& Reward, int nQuestID, int nRewardCheck)
{
	m_QuestReward = Reward;
	m_RewardQuestID = nQuestID;
	m_RewardCheck = nRewardCheck;
	
#if defined(PRE_ADD_REMOTE_QUEST)
	if( g_pQuestManager->GetQuestType(nQuestID) != QuestType_RemoteQuest )	//원격퀘인 경우 m_PacketReward 가 이미 설정되어 있음.
#endif
	// 보상패킷을 저장할 임시데이터를 초기화한다.
	// uIndex = 0 : 보상 없음으로 규정한다.
	memset(&m_PacketReward, 0x00, sizeof(m_PacketReward));
}

// 보상패킷을 임시로 저장한다.
void CDNUserQuest::SavePacketReward(CSSelectQuestReward* pPacket)
{
	memcpy(&m_PacketReward, pPacket, sizeof(CSSelectQuestReward));
}

bool CDNUserQuest::PreparePacketReward(int nQuestID)
{
	TQuestReward table;
	memset(&table, 0, sizeof(TQuestReward));

	bool bResult = g_pDataManager->GetQuestCommonRewardByQuestID(nQuestID, table);	//클래스 공통 리워드 정보만 처리하도록 수정

	if(!bResult)
	{
#ifndef _FINAL_BUILD

#if defined(_VILLAGESERVER)
		UINT nUserID =  m_pSession->GetObjectID();
#elif defined(_GAMESERVER)
		UINT nUserID = m_pSession->GetSessionID();
#endif

		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 보상테이블 조회실패UserObjID: %d 퀘스트ID:%d " , nUserID, nQuestID);
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif
		return false;
	}

	// 보상받을 수 있도록 데이터를 세팅해준다.
	memset(&m_PacketReward, 0x00, sizeof(m_PacketReward));

	m_PacketReward.nIndex = table.nIndex;

	m_QuestReward = table;
	m_RewardQuestID = nQuestID;
	m_RewardCheck = m_PacketReward.nIndex;

	return true;
}

int CDNUserQuest::_AddItemByQuestReward( const TQuestReward::_ITEMSET* pItemSet )
{
#if defined(_CH)
	if( m_pSession->GetFCMState() != FCMSTATE_NONE )
	{
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	CDNUserItem* pUserItem = m_pSession->GetItem();
	if( !pUserItem )
		return -2;

#if defined(_VILLAGESERVER)
	UINT nUserObjectID =  m_pSession->GetObjectID();
#elif defined(_GAMESERVER)
	UINT nUserObjectID = m_pSession->GetSessionID();
#endif

#if defined( _GAMESERVER )
	int nResult = api_user_CheckInvenForAddItem( m_pSession->GetGameRoom(), nUserObjectID, pItemSet->nItemID, pItemSet->nItemCount );
#else
	int nResult = api_user_CheckInvenForAddItem(nUserObjectID, pItemSet->nItemID, pItemSet->nItemCount );
#endif // #if defined( _GAMESERVER )
	if( nResult < 0 )
		return nResult;

	bool bResult = pUserItem->AddInventoryByQuest( pItemSet, m_RewardQuestID, 0 );

#ifndef _FINAL_BUILD
	if(bResult == false)
	{
		std::wstring wszMsg;
		wszMsg = FormatW( L"아이템 지급 실패 %s: id %d count : %d" , bResult ? L"" : L"실패", pItemSet->nItemID, pItemSet->nItemCount );
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
	}
#endif

	return 1;
}

void CDNUserQuest::OnSelectRewardEx(bool bSelectedArray[], bool bSelectedCashArray[])
{
#if defined(_CH)
	if(m_pSession->GetFCMState() != FCMSTATE_NONE){
		m_pSession->SendQuestResult(ERROR_QUEST_FCM);
		return ;
	}
#endif

#if defined(_VILLAGESERVER)
	UINT nUserID =  m_pSession->GetObjectID();
#elif defined(_GAMESERVER)
	UINT nUserID = m_pSession->GetSessionID();
#endif

	TMailTableData *pMailData = g_pDataManager->GetMailTableData(m_QuestReward.nMailID);	

	if (m_QuestReward.nFatigue > 0)
	{
		int nFatigue = m_QuestReward.nFatigue;
		int nMaxFatigue = g_pDataManager->GetFatigue(m_pSession->GetUserJob(), m_pSession->GetLevel());
		if (nMaxFatigue - m_pSession->GetFatigue() < nFatigue)
			nFatigue = nMaxFatigue - m_pSession->GetFatigue();

		if (nFatigue > 0)
			m_pSession->IncreaseFatigue(nFatigue);
	}

	switch( m_QuestReward.cType)
	{
		//--------------------------------------------------------------------------------------------------------------------------
		// 전체보상일경우
	case 1:
		{

#if defined(_VILLAGESERVER)

			if(m_QuestReward.nCoin != 0) 
				api_user_AddCoin(nUserID, m_QuestReward.nCoin, m_RewardQuestID);
			if(m_QuestReward.nExp != 0)
				api_user_AddExp(nUserID, m_RewardQuestID, m_QuestReward.nExp);

			for( int i = 0 ; i < QUESTREWARD_INVENTORYITEMMAX ; i++ )
			{
				if( m_QuestReward.ItemArray[i].nItemID > 0 && m_QuestReward.ItemArray[i].nItemCount > 0 ) 
				{
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
					// 만약호감도올리는보상이면아이템만표시해줄뿐실제인벤에넣으면안된다.
					if( NPC_FAVOR_QUEST_REWARD_FAVOR_UP_ITEM_ID == m_QuestReward.ItemArray[i].nItemID ||
						NPC_FAVOR_QUEST_REWARD_BROMIDE_ITEM_ID == m_QuestReward.ItemArray[i].nItemID )
						continue;
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)

					_AddItemByQuestReward( &m_QuestReward.ItemArray[i] );
				}
			}

#elif defined(_GAMESERVER)
			if(m_QuestReward.nCoin != 0) 
				api_user_AddCoin(m_pSession->GetGameRoom(), nUserID, m_QuestReward.nCoin, m_RewardQuestID);
			if(m_QuestReward.nExp != 0)
				api_user_AddExp(m_pSession->GetGameRoom(), nUserID, m_RewardQuestID, m_QuestReward.nExp);

			for( int i = 0 ; i < QUESTREWARD_INVENTORYITEMMAX ; i++ )
			{
				if( m_QuestReward.ItemArray[i].nItemID > 0 && m_QuestReward.ItemArray[i].nItemCount > 0 ) 
				{
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
					// 만약 호감도 올리는 보상이면 아이템만 표시해줄뿐 실제 인벤에 넣으면 안된다.
					if( NPC_FAVOR_QUEST_REWARD_FAVOR_UP_ITEM_ID == m_QuestReward.ItemArray[i].nItemID || 
						NPC_FAVOR_QUEST_REWARD_BROMIDE_ITEM_ID == m_QuestReward.ItemArray[i].nItemID )
						continue;
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
					_AddItemByQuestReward( &m_QuestReward.ItemArray[i] );
				}
			}

#endif	// _VILLAGESERVER

			for (int i = 0; i < MAX_QUEST_LEVEL_CAP_REWARD; ++i)
			{
				if( m_QuestReward.LevelCapItemArray[i].nItemID > 0 && m_QuestReward.LevelCapItemArray[i].nItemCount > 0 ) 
				{
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
					// 만약 호감도 올리는 보상이면 아이템만 표시해줄뿐 실제 인벤에 넣으면 안된다.
					if( NPC_FAVOR_QUEST_REWARD_FAVOR_UP_ITEM_ID == m_QuestReward.LevelCapItemArray[i].nItemID || 
						NPC_FAVOR_QUEST_REWARD_BROMIDE_ITEM_ID == m_QuestReward.LevelCapItemArray[i].nItemID )
						continue;
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
					_AddItemByQuestReward( &m_QuestReward.LevelCapItemArray[i] );
				}
			}

			if(IsRewardCashItem())
			{
				if(pMailData)
					m_pSession->MakeGiftByQuest(m_RewardQuestID, pMailData->ItemSNArr);
				else
					return;
			}
		}
		break;

		//--------------------------------------------------------------------------------------------------------------------------
		// 선택보상일경우
	case 2:
		{
			int nRewardItem[MAILATTACHITEMMAX] = { 0, };
			int nClientItemCnt = 0, nClientCashItemCnt = 0;

			// 클라이언트가몇개의아이템을선택해서보냈는지확인한다.
			for( int i = 0 ; i < MAILATTACHITEMMAX ; i++ )
			{
				if(bSelectedArray[i])
					nClientItemCnt++;

				if(bSelectedCashArray[i]){
					if(pMailData)
						nRewardItem[i] = pMailData->ItemSNArr[i];
					else
						return;

					nClientCashItemCnt++;
				}
			}

			// 원래받을수있는아이템과클라이언트가선택한아이템의갯수가다르면실패
			if( m_QuestReward.cSelectMax != (nClientItemCnt + nClientCashItemCnt) )
			{
				m_pSession->SendQuestResult(ERROR_QUEST_REWARD_FAIL);
				return;
			}

#if defined(_VILLAGESERVER)
			if(m_QuestReward.nCoin != 0) 
				api_user_AddCoin(nUserID, m_QuestReward.nCoin, m_RewardQuestID);
			if(m_QuestReward.nExp != 0)
				api_user_AddExp(nUserID, m_RewardQuestID, m_QuestReward.nExp);
#elif defined(_GAMESERVER)
			if(m_QuestReward.nCoin != 0) 
				api_user_AddCoin(m_pSession->GetGameRoom(), nUserID, m_QuestReward.nCoin, m_RewardQuestID);
			if(m_QuestReward.nExp != 0)
				api_user_AddExp(m_pSession->GetGameRoom(), nUserID, m_RewardQuestID, m_QuestReward.nExp);
#endif	// _VILLAGESERVER

			// 아니라면이제증정하자.
			if(nClientItemCnt > 0){
				for( int i = 0 ; i < QUESTREWARD_INVENTORYITEMMAX ; i++ )
				{
					// 선택한아이템이면
					if( bSelectedArray[i] )
					{
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
						// 만약호감도올리는보상이면아이템만표시해줄뿐실제인벤에넣으면안된다.
						if( NPC_FAVOR_QUEST_REWARD_FAVOR_UP_ITEM_ID == m_QuestReward.ItemArray[i].nItemID ||
							NPC_FAVOR_QUEST_REWARD_BROMIDE_ITEM_ID == m_QuestReward.ItemArray[i].nItemID )
							continue;
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)

						_AddItemByQuestReward( &m_QuestReward.ItemArray[i] );
					}
				}
			}

			for (int i = 0; i < MAX_QUEST_LEVEL_CAP_REWARD; ++i)
			{
				if( m_QuestReward.LevelCapItemArray[i].nItemID > 0 && m_QuestReward.LevelCapItemArray[i].nItemCount > 0 ) 
				{
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
					// 만약 호감도 올리는 보상이면 아이템만 표시해줄뿐 실제 인벤에 넣으면 안된다.
					if( NPC_FAVOR_QUEST_REWARD_FAVOR_UP_ITEM_ID == m_QuestReward.LevelCapItemArray[i].nItemID || 
						NPC_FAVOR_QUEST_REWARD_BROMIDE_ITEM_ID == m_QuestReward.LevelCapItemArray[i].nItemID )
						continue;
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
					_AddItemByQuestReward( &m_QuestReward.LevelCapItemArray[i] );
				}
			}

			if((nClientCashItemCnt > 0) && IsRewardCashItem())
			{
				// 아니라면이제증정하자.
				m_pSession->MakeGiftByQuest(m_RewardQuestID, nRewardItem);
			}
		}
		break;

	}
}

bool CDNUserQuest::IsRewardItem()
{
	for (int i = 0; i < QUESTREWARD_INVENTORYITEMMAX; i++){
		if(m_QuestReward.ItemArray[i].nItemID > 0) return true;
	}

	return false;
}

bool CDNUserQuest::IsRewardCashItem()
{
	return(m_QuestReward.nMailID > 0) ? true : false;
}

void CDNUserQuest::OnCancelQuest(int nQuestID)
{
#if defined(_VILLAGESERVER)
	UINT nUserID =  m_pSession->GetObjectID();
#elif defined(_GAMESERVER)
	UINT nUserID = m_pSession->GetSessionID();
#endif

	int nSlot = _FindPlayingQuest(nQuestID);
	if( nSlot < 0 )
	{
		m_pSession->SendRemoveQuest(0, nQuestID, ERROR_QUEST_CANCELFAIL);
		return;
	}

	// 퀘스트 지우고
	int nSlotID = _RemoveQuest(nQuestID, false, false);

	if(nSlotID == -2)
		m_pSession->SendRemoveQuest(nSlotID, nQuestID, ERROR_QUEST_NOTFOUND);
	else		// 지웟다고 보내자.
		m_pSession->SendRemoveQuest(nSlotID, nQuestID, ERROR_NONE);

#if defined(PRE_ADD_REMOTE_QUEST)
	if( nSlotID >= 0 && g_pQuestManager->GetQuestType(nQuestID) == QuestType_RemoteQuest )
	{	//원격퀘를 수락받고 포기한 경우 다시 수락대기 리스트로
		m_pSession->GetQuest()->AddRemoteQuestToAcceptWaitList(nQuestID);
	}
#endif

}

void CDNUserQuest::OnPeriodEvent(int nQuestID, bool bFlag)
{
	if(true == bFlag)
	{
		int nSlot = _FindPlayingQuest(nQuestID);
		if( nSlot > 0 )
			_RemoveQuest(nQuestID, false, false);
	}
	
	SetCompleteQuestFlag(nQuestID, bFlag);
}

void CDNUserQuest::OnAddItemEx(int nItemID, short wCount)
{
	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		if( m_pQuest->Quest[i].cQuestState == QuestState_Playing )
		{
			TCount* pCount = (TCount*)m_pQuest->Quest[i].Extra;
			for (int j=0; j<QUEST_EXTRACOUNT_MAX ; j++)
			{
				if(pCount[j].cType == CountingType_Item && 
					pCount[j].nIndex == nItemID &&
					wCount >= pCount[j].nTargetCnt)
				{
					CDNQuest* pQuest = g_pQuestManager->GetQuest(m_pQuest->Quest[i].nQuestID);
					if(pQuest)
					{
#if defined(_VILLAGESERVER)
						pQuest->OnItemCollecting(GetUserObjID(), nItemID, wCount);
#elif defined(_GAMESERVER)
						pQuest->OnItemCollecting(m_pSession->GetGameRoom(), GetUserObjID(), nItemID, wCount);
#endif	// _VILLAGESERVER
					}
				}
			}
		}
	}
}

void CDNUserQuest::AddCompletedPeriodQuest(int questID)
{
	m_CompletedPeriodQuests.insert(questID);
}

void CDNUserQuest::RemoveCompletedPeriodQuest(int questID)
{
	std::set<int>::iterator it = m_CompletedPeriodQuests.find(questID);
	if (it == m_CompletedPeriodQuests.end())
		return;

	m_CompletedPeriodQuests.erase(it);
#if defined(PRE_ADD_REMOTE_QUEST)
	SetCompleteQuestFlag(questID, false);	//리셋되는 일일퀘 완료 플래그 제거
#endif
}

bool CDNUserQuest::IsCompletedPeriodQuest(int questID) const
{
	return (m_CompletedPeriodQuests.find(questID) != m_CompletedPeriodQuests.end()) ? true : false;
}

#if defined(PRE_ADD_REMOTE_QUEST)

bool CDNUserQuest::OnEvent( EventSystem::EventTypeEnum Event )
{
	CheckRemoteQuestGain(Event);

	return true;
}

void CDNUserQuest::CheckRemoteQuestGain(EventSystem::EventTypeEnum Event)
{
	bool bQuestGain = false;
	int nArrayIndex;
	for( DWORD i=0; i<m_nVecRemoteQuestGainList[Event].size(); i++ ) {
		nArrayIndex = m_nVecRemoteQuestGainList[Event][i];

 		TRemoteQuestData *pData = g_pDataManager->GetRemoteQuestData(nArrayIndex);
		if(!pData) continue;

		bool bGain = false;
		bGain = m_pSession->GetEventSystem()->CheckEventCondition( &pData->GainCondition );
		if( bGain )
		{
			bGain = CheckRemotePreQuestCondition( pData );
		}

		if( bGain ) 
		{
			CDNQuest* pQuest = g_pQuestManager->GetQuest(pData->nQuestID);

			if( pQuest && _CheckRemoteQuestGainValidation(pData->nQuestID, pQuest) )
			{
				// 원격퀘를 획득할 수 없는 경우
				if( !AddRemoteQuestToAcceptWaitList(pData->nQuestID) )
					continue;
			}

			if(!pQuest || pQuest->GetQuestInfo().cRepeatable != 1)
			{
				m_nVecRemoteQuestGainList[Event].erase( m_nVecRemoteQuestGainList[Event].begin() + i );
				i--;
			}
		}
	}
}

bool CDNUserQuest::CheckRemotePreQuestCondition( TRemoteQuestData *pData )
{
	if(!pData) return false;

	if( 0 == pData->nVecPreQuestCondition.size() )
	{
		return true;
	}

	if( pData->bPreQuestCheckType == false ) 
	{
		for( int i = 0 ; i < pData->nVecPreQuestCondition.size() ; ++ i ) 
		{
			if( IsClearQuest( pData->nVecPreQuestCondition[i] ) == false ) 
				return false;
		}
		return true;
	}
	else 
	{
		for( int i = 0 ; i < pData->nVecPreQuestCondition.size() ; ++ i ) 
		{
			if( IsClearQuest( pData->nVecPreQuestCondition[i] ) == true ) 
				return true;
		}
		return false;
	}
	return false;	
}

void CDNUserQuest::CheckAcceptWaitRemoteQuestList()
{
	for(int i = 0;  i < m_nVecRemoteQuestAcceptWaitList.size(); i++)
	{
		CDNQuest* pQuest = g_pQuestManager->GetQuest(m_nVecRemoteQuestAcceptWaitList[i]);
		if( !pQuest || (IsClearQuest(m_nVecRemoteQuestAcceptWaitList[i]) && pQuest->GetQuestInfo().cRepeatable != 1)
			|| FindPlayingQuest(m_nVecRemoteQuestAcceptWaitList[i]) >= 0 )
		{
			m_pSession->SendRemoveRemoteQuest(m_nVecRemoteQuestAcceptWaitList[i]);
			m_nVecRemoteQuestAcceptWaitList.erase(m_nVecRemoteQuestAcceptWaitList.begin() + i);
			i--;
		}
	}
}

bool CDNUserQuest::_CheckRemoteQuestGainValidation(int nQuestID, CDNQuest* pQuest)
{
	if(!pQuest) return false;

	//반복퀘가 아닌대 깬 경우 버린다.
	if( IsClearQuest(nQuestID) && pQuest->GetQuestInfo().cRepeatable != 1 )
		return false;	

	//이미 진행중인 퀘면 안댐
	if( FindPlayingQuest(nQuestID) >= 0 )
		return false;

	//이미 획득한 원격퀘
	if(_FindAcceptWaitQuest(nQuestID) >= 0)
		return false;

	return true;
}

bool CDNUserQuest::AddRemoteQuestToAcceptWaitList(int nQuestID)
{
	if( m_nVecRemoteQuestAcceptWaitList.size() >= MAX_PLAY_QUEST )
	{
#ifndef _FINAL_BUILD
		std::wstring wszMsg;
		wszMsg = FormatW( L"더이상 원격퀘스트를 획득할 수 없습니다. : %d" , nQuestID );
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
		return false;
	}

	m_nVecRemoteQuestAcceptWaitList.push_back(nQuestID);
	m_pSession->SendGainRemoteQuest(nQuestID);
	return true;
}

bool CDNUserQuest::RemoveRemoteQuestFromAcceptWaitList(int nQuestID)
{
	int nIndex = _FindAcceptWaitQuest(nQuestID);
	if(nIndex < 0) return false;

	m_nVecRemoteQuestAcceptWaitList.erase(m_nVecRemoteQuestAcceptWaitList.begin() + nIndex);
	m_pSession->SendRemoveRemoteQuest(nQuestID);
	return true;
}

void CDNUserQuest::AcceptRemoteQuest(int nQuestID)
{
	int nIndex = _FindAcceptWaitQuest(nQuestID);
	if(nIndex < 0)
	{
		_DANGER_POINT();	//획득도 안해놓고 퀘스트 받겠다고 하는넘들(어뷰징 의심유저)
		return;
	}

	CDNQuest* pQuest = g_pQuestManager->GetQuest(nQuestID);
	if(!pQuest)
		return;
	
	if( AddQuest(nQuestID, true) == 1 )
	{
		SetQuestStepAndJournalStep(nQuestID, 1, 1, false);
#if defined(_VILLAGESERVER)
		pQuest->OnRemoteStart(m_pSession->GetObjectID());
#elif defined(_GAMESERVER)
		pQuest->OnRemoteStart(m_pSession->GetGameRoom(), m_pSession->GetSessionID());
#endif
	}
}

void CDNUserQuest::CancelRemoteQuest(int nQuestID)
{
	if(!RemoveRemoteQuestFromAcceptWaitList(nQuestID))
	{
		_DANGER_POINT();	//획득도 안해놓고 퀘스트 포기하겠다고 하는 넘들
		return;
	}

	//획득한 원격 퀘스트를 포기하는 경우 -> GainList 에 다시 넣어줘야 된다(다시 획득 가능하게 해야 되는건가??)
	TRemoteQuestData *pData = g_pDataManager->GetRemoteQuestDataByQuestID(nQuestID);
	if(pData)
	{
		CDNQuest* pQuest = g_pQuestManager->GetQuest(pData->nQuestID);
		if(!pQuest)
			return;

		if(pQuest->GetQuestInfo().cRepeatable != 1)	//반복퀘면 안지우고 이미 들어있음
			m_nVecRemoteQuestGainList[pData->GainCondition.cEvent].push_back(pData->nIndex);
	}
}

void CDNUserQuest::CompleteRemoteQuest(int nQuestID, CSSelectQuestReward* pPacket)
{
	if(FindRecompenseQuestIndex(nQuestID) < 0)
	{
		_DANGER_POINT();	//완료된 퀘스트가 아님 -> 어뷰징 처리 하기
		ResetRewardFlag();
		return;
	}

	CDNQuest* pQuest = g_pQuestManager->GetQuest(nQuestID);
	if( pQuest )
	{
		SavePacketReward(pPacket);
#if defined(_VILLAGESERVER)
		pQuest->OnRemoteComplete(m_pSession->GetObjectID());
#elif defined(_GAMESERVER)
		pQuest->OnRemoteComplete(m_pSession->GetGameRoom(), m_pSession->GetSessionID());
#endif
	}
}

void CDNUserQuest::RefreshRemoteQuest(int nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList)
{
	_ResetRemoteQuest();
	//획득한 원격퀘 리스트 정보 셋팅
	if( nAcceptWaitRemoteQuestCount > MAX_PLAY_QUEST) nAcceptWaitRemoteQuestCount = MAX_PLAY_QUEST;
	for(int  i = 0 ; i < nAcceptWaitRemoteQuestCount ; i++)
			m_nVecRemoteQuestAcceptWaitList.push_back(AcceptWaitRemoteQuestList[i]);

	_RefreshRemoteQuestList();
}

int CDNUserQuest::_FindRecompenseQuestIndex(int nQuestID)
{
	for( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		if( m_pQuest->Quest[i].nQuestID == nQuestID && m_pQuest->Quest[i].cQuestState == QuestState_Recompense ) 
			return i;
	}

	return -1;
}

int CDNUserQuest::_FindAcceptWaitQuest(int nQuestID)
{
	for( int i = 0 ; i < m_nVecRemoteQuestAcceptWaitList.size() ; i++ )
	{
		if(m_nVecRemoteQuestAcceptWaitList[i] == nQuestID)
			return i;
	}

	return -1;
}

void CDNUserQuest::_ResetRemoteQuest()
{
	for( int i=0; i<EventSystem::EventTypeEnum_Amount; i++ )
		SAFE_DELETE_VEC( m_nVecRemoteQuestGainList[i] );

	SAFE_DELETE_VEC(m_nVecRemoteQuestAcceptWaitList);
}

/*
일일 퀘스트의 경우 ModifyCompleteQuest() 에서 선택되지 않은 일일퀘를 Complete 마킹 처리한다.
아래 _RefreshRemoteQuestList() 호출 시점이 ModifyCompleteQuest() 보다 더 빨라서 일단 리스트에는 다 들어가지만
실제 Gain 이벤트가 호출되는 시점에서 선택되지 않은 일일퀘들은 Complete 마킹에 걸려서 획득되지 않고 삭제된다.
*/
void CDNUserQuest::_RefreshRemoteQuestList()
{
	TQuestGroup *pQuest = m_pSession->GetQuestData();
	if(!pQuest)  return;

	int nSize = g_pDataManager->GetRemoteQuestDataSize();
	for(int i = 0 ; i < nSize; i++)
	{
		TRemoteQuestData *pData = g_pDataManager->GetRemoteQuestData(i);
		if(!pData || !pData->bActivate) continue;

		CDNQuest* pQuest = g_pQuestManager->GetQuest(pData->nQuestID);
		if( pQuest && _CheckRemoteQuestGainValidation(pData->nQuestID, pQuest) )
			m_nVecRemoteQuestGainList[pData->GainCondition.cEvent].push_back(i);
	}	
}

void CDNUserQuest::GetAcceptWaitRemoteQuest(int *nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList)
{	
	for(int i = 0 ; i < m_nVecRemoteQuestAcceptWaitList.size() ; i++)
	{
		if( i >= MAX_PLAY_QUEST ) break;
		AcceptWaitRemoteQuestList[i] = m_nVecRemoteQuestAcceptWaitList[i];
	}
	*nAcceptWaitRemoteQuestCount = (int)m_nVecRemoteQuestAcceptWaitList.size();
	if(*nAcceptWaitRemoteQuestCount > MAX_PLAY_QUEST)
		*nAcceptWaitRemoteQuestCount = MAX_PLAY_QUEST;
}
#endif	//#if defined(PRE_ADD_REMOTE_QUEST)
