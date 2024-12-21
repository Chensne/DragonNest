#include "StdAfx.h"
#include "DnTimeEventTask.h"
#include "DnTableDB.h"
#include "DnMissionTask.h"
#include "DnInterface.h"
#include "DnMainDlg.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnMainMenuDlg.h"
#include "DnTimeEventDlg.h"

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
#include "DnIntegrateEventDlg.h"

	#ifdef PRE_ADD_STAMPSYSTEM
		#include "DnStampEventDlg.h"
		#include "GameOption.h"
	#endif // PRE_ADD_STAMPSYSTEM

#endif // PRE_ADD_INTEGERATE_EVENTUI

CDnTimeEventTask::CDnTimeEventTask()
: CTaskListener( false )
{
	m_nSelectAlarmEventID = 0;
}

CDnTimeEventTask::~CDnTimeEventTask()
{
	Finalize();
}

bool CDnTimeEventTask::Initialize()
{
	return true;
}

void CDnTimeEventTask::Finalize()
{
	SAFE_DELETE_PVEC( m_pVecTimeEventList );
}

void CDnTimeEventTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !IsProcess() ) return;

	CTimeSet tCurTime;
	tCurTime.Reset();

	for( DWORD i=0; i<m_pVecTimeEventList.size(); i++ ) {
		TimeEventInfoStruct *pStruct = m_pVecTimeEventList[i];
		if( pStruct->nRemainTime == -1 ) continue;
		if( tCurTime.GetTimeT64_LC() < pStruct->BeginTime.GetTimeT64_LC() ) continue;
		if( pStruct->RepeatType == CDnTimeEventTask::Daily && pStruct->bDailyFlag ) continue;

		pStruct->nRemainTime -= (int)( fDelta * 1000 );

		if( pStruct->nRemainTime <= 0 ) {
			pStruct->nRemainTime = 0;
		}
	}
}

void CDnTimeEventTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch( nMainCmd ) {
		case SC_TIMEEVENT: OnRecvTimeEventMessage( nSubCmd, pData, nSize ); break;

#ifdef PRE_ADD_STAMPSYSTEM
		case SC_STAMPSYSTEM: OnRecvStampEventMessage( nSubCmd, pData, nSize ); break;
#endif // PRE_ADD_STAMPSYSTEM

	}
}

void CDnTimeEventTask::OnRecvTimeEventMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eTimeEvent::SC_TIMEEVENTLIST:	OnRecvTimeEventList( (SCTimeEventList*)pData ); break;
		case eTimeEvent::SC_ACHIEVE_EVENT:	OnRecvTimeEventAchieve( (SCTimeEventAchieve*)pData ); break;
		case eTimeEvent::SC_EXPIRE_EVENT:	OnRecvTimeEventExpire( (SCTimeEventExpire*)pData); break;
	}
}

void CDnTimeEventTask::OnRecvTimeEventList( SCTimeEventList* pPacket )
{
	SAFE_DELETE_PVEC( m_pVecTimeEventList );
	for( int i=0; i<pPacket->nCount; i++ ) {
		InsertTimeEvent( pPacket->Event[i].nItemID, pPacket->Event[i].nRemainTime, pPacket->Event[i].bCheckFlag );
	}

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
	CDnIntegrateEventDlg *pIntegrateEventDlg = (CDnIntegrateEventDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG );
	m_nSelectAlarmEventID = FindUrgentEventID();
	if( pIntegrateEventDlg  ) 
	{
		DWORD timeEventCount = GetTimeEventCount();
		pIntegrateEventDlg->SetEnableEventTab(timeEventCount != 0, CDnIntegrateEventDlg::TIME_EVENT_TAB);
		pIntegrateEventDlg->RefreshTimeEvent();
	}


#else
	CDnTimeEventDlg *pTimeEventDlg = (CDnTimeEventDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::TIMEEVENT_DIALOG );

	m_nSelectAlarmEventID = FindUrgentEventID();

	if( pTimeEventDlg  ) pTimeEventDlg->RefreshInfo();
#endif // PRE_ADD_INTEGERATE_EVENTUI
}

void CDnTimeEventTask::OnRecvTimeEventAchieve( SCTimeEventAchieve* pPacket )
{
	TimeEventInfoStruct *pStruct = FindTimeEventInfo( pPacket->nItemID );
	if( !pStruct ) return;

	pStruct->nRemainTime = pPacket->nRemainTime;

	if( pStruct->RepeatType == CDnTimeEventTask::Daily ) pStruct->bDailyFlag = true;

	if( m_nSelectAlarmEventID == pStruct->nEventID ) {
		CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
		if( pMainBarDlg ) pMainBarDlg->UpdateTimeEventAlarm( 0, GetTimeEventAlarm() );

		m_nSelectAlarmEventID = FindUrgentEventID();
	}
}

void CDnTimeEventTask::OnRecvTimeEventExpire( SCTimeEventExpire* pPacket )
{
	RemoveTimeEvent( pPacket->nItemID );

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
	CDnIntegrateEventDlg *pIntegrateEventDlg = (CDnIntegrateEventDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG );
	if( pIntegrateEventDlg && pIntegrateEventDlg->IsShow() ) pIntegrateEventDlg->RefreshTimeEvent();

	if( m_nSelectAlarmEventID == pPacket->nItemID ) {
		CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
		if( pMainBarDlg ) pMainBarDlg->UpdateTimeEventAlarm( -1, GetTimeEventAlarm() );

		m_nSelectAlarmEventID = FindUrgentEventID();
	}

#else
	CDnTimeEventDlg *pTimeEventDlg = (CDnTimeEventDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::TIMEEVENT_DIALOG );
	if( pTimeEventDlg && pTimeEventDlg->IsShow() ) pTimeEventDlg->RefreshInfo();

	if( m_nSelectAlarmEventID == pPacket->nItemID ) {
		CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
		if( pMainBarDlg ) pMainBarDlg->UpdateTimeEventAlarm( -1, GetTimeEventAlarm() );

		m_nSelectAlarmEventID = FindUrgentEventID();
	}
#endif // PRE_ADD_INTEGERATE_EVENTUI
}

int CDnTimeEventTask::FindUrgentEventID()
{
	INT64 nValue = LLONG_MAX;
	int nEventID = 0;
	for( DWORD i=0; i<m_pVecTimeEventList.size(); i++ ) {
		TimeEventInfoStruct *pStruct = m_pVecTimeEventList[i];
		if( pStruct->nRemainTime != -1 && pStruct->nRemainTime < nValue ) {
			nValue = pStruct->nRemainTime;
			nEventID = pStruct->nEventID;
		}
	}
	return nEventID;
}

void CDnTimeEventTask::InsertTimeEvent( int nEventID, INT64 nRemainTime, bool bCheckFlag )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCONNECTINGTIME );
	DNTableFileFormat* pMissionSox = GetDNTable( CDnTableDB::TMISSION );
	DNTableFileFormat* pCashCommodity = GetDNTable( CDnTableDB::TCASHCOMMODITY );
	if( !pSox->IsExistItem( nEventID ) ) return;

	TimeEventInfoStruct *pStruct = FindTimeEventInfo( nEventID );
	if( pStruct ) {
		pStruct->nRemainTime = nRemainTime;
		return;
	}

	pStruct = new TimeEventInfoStruct;
	pStruct->nEventID = nEventID;
	pStruct->szTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nEventID, "_NameID" )->GetInteger() );
	pStruct->szDescription = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nEventID, "_DescriptionID" )->GetInteger() );

	char *szPeriod = pSox->GetFieldFromLablePtr( nEventID, "_EventPeriod" )->GetString();
	std::vector<string> vEventPeriod;
	TokenizeA( szPeriod, vEventPeriod, ":" );
	
	pStruct->BeginTime = CTimeSet( vEventPeriod[0].c_str(), true );
	pStruct->EndTime = CTimeSet( vEventPeriod[1].c_str(), true );
	pStruct->RepeatType = (CDnTimeEventTask::RepeatTypeEnum)pSox->GetFieldFromLablePtr( nEventID, "_RepeatType" )->GetInteger();
	pStruct->nMaintenanceTime = (INT64)( pSox->GetFieldFromLablePtr( nEventID, "_TimeCheck" )->GetInteger() * 60 * 1000 );
	pStruct->nRemainTime = nRemainTime;
	pStruct->bDailyFlag = bCheckFlag;

	int nMissionTableID = pSox->GetFieldFromLablePtr( nEventID, "_MissionID" )->GetInteger();	
	int nMailID = pMissionSox->GetFieldFromLablePtr( nMissionTableID, "_MailID" )->GetInteger();
	pStruct->nRewardCoin = CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardCoin );
	std::vector<int> nVecRewardItemList, nVecRewardCashItemList, nVecRewardItemCountList, nVecRewardCashItemCountList;
	for( int i=0; i<CDnMissionTask::RewardItem_Amount; i++ ) {
		int nValue = CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardItemID, false, i );
		if( nValue > 0 ) 
		{
			nVecRewardItemList.push_back( nValue );
			nVecRewardItemCountList.push_back( CDnSlotButton::ITEM_ORIGINAL_COUNT );
			int nCount = CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardItemCount, false, i );
			if( nCount > 0 ) 
				nVecRewardItemCountList.back() = nCount;
				
		}
	}
	for( int i=0; i<CDnMissionTask::RewardItem_Amount; i++ ) {
		int nValue = CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardItemID, true, i );
		if( nValue > 0 ) 
		{
			nVecRewardCashItemList.push_back( nValue );
			nVecRewardCashItemCountList.push_back( CDnSlotButton::ITEM_ORIGINAL_COUNT );

			// 캐쉬템은 cashcommodity 테이블에 갯수가 지정되어 있음.
			int nCount = pCashCommodity->GetFieldFromLablePtr( nValue, "_Count" )->GetInteger();
			if( nCount > 0 ) 
				nVecRewardCashItemCountList.back() = nCount;
		}
	}

	CalcRewardItemList( pStruct->pVecRewardItemList, pStruct->VecRewardItemCountList, 
						nVecRewardItemList, nVecRewardCashItemList,
						nVecRewardItemCountList, nVecRewardCashItemCountList );

	m_pVecTimeEventList.push_back( pStruct );
}

void CDnTimeEventTask::RemoveTimeEvent( int nEventID )
{
	for( DWORD i=0; i<m_pVecTimeEventList.size(); i++ ) {
		if( m_pVecTimeEventList[i]->nEventID == nEventID ) {
			SAFE_DELETE( m_pVecTimeEventList[i] );
			m_pVecTimeEventList.erase( m_pVecTimeEventList.begin() + i );
			return;
		}
	}
}

DWORD CDnTimeEventTask::GetTimeEventCount()
{
	return (DWORD)m_pVecTimeEventList.size();
}

CDnTimeEventTask::TimeEventInfoStruct *CDnTimeEventTask::GetTimeEventInfo( DWORD dwIndex )
{
	if( dwIndex < 0 || dwIndex >= m_pVecTimeEventList.size() ) return NULL;
	return m_pVecTimeEventList[dwIndex];
}

CDnTimeEventTask::TimeEventInfoStruct *CDnTimeEventTask::FindTimeEventInfo( int nEventID )
{
	for( DWORD i=0; i<m_pVecTimeEventList.size(); i++ ) {
		if( m_pVecTimeEventList[i]->nEventID == nEventID ) return m_pVecTimeEventList[i];
	}
	return NULL;
}

void CDnTimeEventTask::SetTimeEventAlarm( int nEventID )
{
	m_nSelectAlarmEventID = nEventID;
	CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
	if( pMainBarDlg ) {
		pMainBarDlg->ShowTimeEventAlarm( ( nEventID > 0 ) ? true : false );
	}
}

void CDnTimeEventTask::CalcRewardItemList( /*OUT */ std::vector<CDnItem *> &pVecResult,  /*OUT*/ std::vector<int> &VecItemCountResult,
										   std::vector<int> &nVecItemList, std::vector<int> &nVecCashItemList,
										   std::vector<int> &nVecRewardItemCountList, std::vector<int> &nVecRewardCashItemCountList )
{
	for( DWORD i=0; i<nVecItemList.size(); i++ ) {
		CDnItem *pRewardItem = NULL;
		int nRewardItemID = nVecItemList[i];

		TItemInfo Info;
		if( CDnItem::MakeItemInfo( nRewardItemID, 1, Info ) == false ) continue;

		pRewardItem = CDnItemTask::GetInstance().CreateItem( Info );
		if( !pRewardItem ) continue;

		pVecResult.push_back( pRewardItem );
		VecItemCountResult.push_back( nVecRewardItemCountList.at(i) );
	}
	for( DWORD i=0; i<nVecCashItemList.size(); i++ ) {
		CDnItem *pRewardItem = NULL;
		int nRewardItemID = nVecCashItemList[i];

		const SCashShopItemInfo* pCashInfo = GetCashShopTask().GetItemInfo( nRewardItemID );
		if( !pCashInfo ) continue;

		TItemInfo Info;
		if( !CDnItem::MakeItemInfo( pCashInfo->presentItemId, pCashInfo->count, Info ) ) continue;

		pRewardItem = CDnItemTask::GetInstance().CreateItem( Info );
		if( !pRewardItem ) continue;

		pRewardItem->SetCashItemSN( nRewardItemID );
		pVecResult.push_back( pRewardItem );
		VecItemCountResult.push_back( nVecRewardCashItemCountList.at(i) );
	}
}

bool CDnTimeEventTask::IsProcess()
{
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && !pGameTask->IsSyncComplete() ) return false;

	return true;
}


#ifdef PRE_ADD_STAMPSYSTEM

using namespace StampSystem;

void CDnTimeEventTask::OnRecvStampEventMessage( int nSubCmd, char *pData, int nSize )
{
	CDnIntegrateEventDlg *pIntegrateEventDlg = (CDnIntegrateEventDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG );	
	if( pIntegrateEventDlg  ) 
	{
		CDnStampEventDlg * pStampDlg = pIntegrateEventDlg->GetStampEventDlg();
		if( pStampDlg )
		{
			switch( nSubCmd )
			{
				case eStampSystem::SC_STAMPSYSTEM_INIT:	OnRecvStampInit( pData, pStampDlg ); break;
				case eStampSystem:: SC_STAMPSYSTEM_ADD_COMPLETE: OnRecvStampAddComplete( pData, pStampDlg ); break;
				case eStampSystem::SC_STAMPSYSTEM_CLEAR_COMPLETE: OnRecvStampClearComplete( pStampDlg ); break;
				case eStampSystem::SC_STAMPSYSTEM_CHANGE_WEEKDAY: OnRecvStampChangeWeekDay( pData, pStampDlg ); break;
				case eStampSystem::SC_STAMPSYSTEM_CHANGE_TABLE: OnRecvStampChangeTable( pData, pStampDlg ); break;
			}
		}	
	}

}

// 접속할때 수신.
void CDnTimeEventTask::OnRecvStampInit( char * pData, CDnStampEventDlg * pDlg )
{
	SCStampSystemInit * pPacket = (SCStampSystemInit*)pData;
	
	// 리스트목록생성.
	pDlg->BuildList( pPacket->biTableStartTime, pPacket->nWeekDay, pPacket->bCompleteFlagData );
	
	// 요일변경.
	pDlg->ChangeWeekDay( pPacket->nWeekDay );

	// 알람상태설정.
	GetInterface().AddStamp();
	
#ifdef PRE_ADD_START_POPUP_QUEUE
	if (CGameOption::GetInstance().m_nWeekDay != pPacket->nWeekDay)
	{
		CDnStartPopupMgr& mgr = GetInterface().GetStartPopupMgr();
		mgr.RegisterLow(CDnStartPopupMgr::eStamp, NULL);

		mgr.StartShow();
	}
#else
	// 하루동안열지않음.
	if( CGameOption::GetInstance().m_nWeekDay != pPacket->nWeekDay )
	{
		// 이벤트창열기.
		GetInterface().ShowStampDlg();
	}
#endif
}

// 1개 완료시 수신.
void CDnTimeEventTask::OnRecvStampAddComplete( char * pData, CDnStampEventDlg * pDlg )
{
	SCStampSystemAddComplete * pPacket = (SCStampSystemAddComplete*)pData;

	pDlg->SetStamp( pPacket->nChallengeIndex, pPacket->nWeekDay );

	GetInterface().AddStamp( true );
}

// 한주가 넘어갈때 수신 - 모든 도전과제 제거.
void CDnTimeEventTask::OnRecvStampClearComplete( CDnStampEventDlg * pDlg )
{
	pDlg->EndStamp();
	GetInterface().ShowStampAlarmDlg( false );
}

// 요일변경시 수신.
void CDnTimeEventTask::OnRecvStampChangeWeekDay( char * pData, CDnStampEventDlg * pDlg )
{
	SCStampSystemChangeWeekDay * pPacket = (SCStampSystemChangeWeekDay*)pData;

	// 요일변경.
	pDlg->ChangeWeekDay( pPacket->nWeekDay );

}

// 한주가 변경될때 OnRecvStampClearComplete() 다음으로 넘어오는 패킷.
// biTableStartTime 값과 동일한 필드목록만 리스트에 추가한다.
void CDnTimeEventTask::OnRecvStampChangeTable( char * pData, CDnStampEventDlg * pDlg )
{
	SCStampSystemChangeTable * pPacket = (SCStampSystemChangeTable*)pData;


	// 리스트목록생성.
	pDlg->BuildList( pPacket->biTableStartTime );
}


#endif // PRE_ADD_STAMPSYSTEM