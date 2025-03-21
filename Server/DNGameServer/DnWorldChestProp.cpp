#include "stdafx.h"
#include "DnWorldChestProp.h"
#include "DNProtocol.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnItemTask.h"
#include "DNUserSession.h"
#include "DnWorld.h"
#include "DnPropState.h"
#include "DnPropCondition.h"
#include "DnPropCondiNeedItem.h"
#include "DnPropStateDoAction.h"
#include "DnPropStateCmdOperation.h"
#include "DnPropStateTrigger.h"
#include "DnPropStateItemDrop.h"
#include "DnPropOperatedCondition.h"
#include "DNLogConnection.h"
#include "DNGameTask.h"


CDnWorldChestProp::CDnWorldChestProp( CMultiRoom *pRoom )
: CDnWorldOperationProp( pRoom ), m_iOperateState( CLOSED )
{
	//m_nCloseActionIndex = -1;
	m_iNeedKeyID = 0;
	m_iItemDropGroupTableID = 0;
	m_bRemoveNeedKeyItem = false;
	m_iNumNeedKeyItem = 0;

	m_pNeedItemCondition = NULL;
	m_pItemDropState = NULL;
}

CDnWorldChestProp::~CDnWorldChestProp()
{
	ReleasePostCustomParam();
}

bool CDnWorldChestProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;
	if( GetData() ) {
		ChestStruct *pStruct = (ChestStruct*)GetData();
		m_iNeedKeyID = pStruct->nNeedKeyID;
		m_iItemDropGroupTableID = pStruct->nItemDropGroupTableID;
		m_bRemoveNeedKeyItem = (pStruct->bRemoveKeyItem == TRUE);
		m_iNumNeedKeyItem = pStruct->nNumNeedItem;
		m_bEnableOwnership = (pStruct->bEnableOwnership == TRUE);

		string strLevelBoundDescription;
		string strLevelBoundRewardItemDropGroupID;

		if( pStruct->strLevelBoundDescription )
			strLevelBoundDescription.assign( pStruct->strLevelBoundDescription );

		if( pStruct->strLevelBoundRewardItemDropGroupID )
			strLevelBoundRewardItemDropGroupID.assign( pStruct->strLevelBoundRewardItemDropGroupID );

		//// 테스트용.
		//strLevelBoundDescription.assign( "1~20;21~30;31~40;41~50" );
		//strLevelBoundRewardItemDropGroupID.assign( "1;2;3;4" );

		bool bValidLevelBoundSetting = true;
		vector<string> vlLevelPair;
		TokenizeA( strLevelBoundDescription, vlLevelPair, ";" );
		vector<string> vlRewards;
		TokenizeA( strLevelBoundRewardItemDropGroupID, vlRewards, ";" );
		if( (false == vlLevelPair.empty()) &&
			(vlLevelPair.size() == vlRewards.size()) )
		{
			for( int i = 0; i < (int)vlLevelPair.size(); ++i )
			{
				string& strLevelBound = vlLevelPair.at( i );
				vector<string> vlLevelBound;
				TokenizeA( strLevelBound, vlLevelBound, "~" );
				if( 2 == (int)vlLevelBound.size() )
				{
					S_LEVEL_BOUNDS_REWARD Reward;
					Reward.iLevelStart = atoi( vlLevelBound.at(0).c_str() );
					Reward.iLevelEnd = atoi( vlLevelBound.at(1).c_str() );
					Reward.iItemDropGroupTableID = atoi( vlRewards.at(i).c_str() );
					
					m_vlLevelBoundsReward.push_back( Reward );
				}
				else
				{
					bValidLevelBoundSetting = false;
					break;
				}
			}

			if( false == bValidLevelBoundSetting )
				m_vlLevelBoundsReward.clear();
		}
		

		if( m_vlLevelBoundsReward.empty() &&
			m_iItemDropGroupTableID > 0 )
		{
			CalcDropItemList();
		}
	}

	return true;
}

bool CDnWorldChestProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	CDnPropState* pNormalState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	CDnPropState* pCmdOpenedOpState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::CMD_OPERATION );
	CDnPropState* pOpenActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	CDnPropState* pItemDropState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::ITEM_DROP );
	CDnPropState* pTriggerState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::TRIGGER );
	static_cast<CDnPropStateCmdOperation*>(pCmdOpenedOpState)->SetOperateIDToSend( OPEN_SUCCESS );
	static_cast<CDnPropStateDoAction*>(pOpenActionState)->AddActionName( "Close_Open" );
	static_cast<CDnPropStateTrigger*>(pTriggerState)->SetFuncName( "CDnWorldProp::OnClickProp" );
	m_pTriggerActionState = static_cast<CDnPropStateTrigger*>(pTriggerState);
	m_pItemDropState = static_cast<CDnPropStateItemDrop*>(pItemDropState);

	CDnPropCondition* pOpCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::OPERATED );
	CDnPropCondition* pNeedKeyCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::NEED_ITEM );
	m_pNeedItemCondition = static_cast<CDnPropCondiNeedItem*>(pNeedKeyCondition);
	CDnPropCondition* pNULLCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::NULL_CONDITION );
	static_cast<CDnPropOperatedCondition*>(pOpCondition)->SetRequireState( TRY_OPEN );

	m_pFSM->AddCondition( pOpCondition );
	m_pFSM->AddCondition( pNeedKeyCondition );
	m_pFSM->AddCondition( pNULLCondition );
	
	m_pFSM->AddState( pNormalState );
	m_pFSM->AddState( pOpenActionState );
	m_pFSM->AddState( pItemDropState );
	m_pFSM->AddState( pTriggerState );
	m_pFSM->AddState( pCmdOpenedOpState );

	vector< TDnFSMTransitCondition<DnPropHandle>* > vlConditions;
	vlConditions.push_back( pOpCondition );
	vlConditions.push_back( pNeedKeyCondition );
	pNormalState->AddTransitState( pCmdOpenedOpState, vlConditions );
	pCmdOpenedOpState->AddTransitState( pOpenActionState, pNULLCondition );
	pOpenActionState->AddTransitState( pItemDropState, pNULLCondition );
	pItemDropState->AddTransitState( pTriggerState, pNULLCondition );

	m_pFSM->SetEntryState( pNormalState );

	return CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
}

void CDnWorldChestProp::OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket )
{
	if( false == (pSession && pSession->GetActorHandle()) )
		return;

	switch( dwProtocol ) {
		case eProp::CS_CMDOPERATION:
			{
#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
				if (IsVisible() == false)
					return;
#else
	#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
				if( false == m_bRandomResultIsVisible )
					return;
	#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE
#endif

				if( false == IsShow() )
					return;

#if !defined( PRE_TRIGGER_TEST )
				if( std::find( m_hVecDisableActor.begin(), m_hVecDisableActor.end(), pSession->GetActorHandle() ) != m_hVecDisableActor.end() ) break;
#endif // #if defined( PRE_TRIGGER_TEST )

				// 이미 열린 상태면 더 이상 조작되지 않도록 방어.
				CDnPropState* pNowState = static_cast<CDnPropState*>(m_pFSM->GetCurrentState());
				if( false == (pNowState && pNowState->GetType() == CDnPropState::NORMAL) )
					break;

				m_iOperateState = TRY_OPEN;

				// 곧바로 전이시킴. 메시지 디스패치 되는 즉시 처리해야 패킷이 동시에 오거나 밀려왔을 때 잘 처리된다.
				m_pNeedItemCondition->SetAccessActor( pSession->GetActorHandle() );
				m_pNeedItemCondition->SetNeedItemID( m_iNeedKeyID );
				m_pNeedItemCondition->SetNumNeedItem( m_iNumNeedKeyItem );
				m_pNeedItemCondition->SetRemoveNeedItem( m_bRemoveNeedKeyItem );
				m_pTriggerActionState->AddFuncParam( "LastOperationActor", ( pSession && pSession->GetActorHandle() ) ? pSession->GetActorHandle()->GetUniqueID() : -1 );

				// 레벨 구간별 보상이 설정되어있다면 드란 아이템 값 갱신.
				if( false == m_vlLevelBoundsReward.empty() )
				{
					// 조작한 녀석의 레벨에 맞는 보상 드랍 아이템 ID 셋팅.
					m_iItemDropGroupTableID = 0;
					int iAccessActorLevel = pSession->GetActorHandle()->GetLevel();
					for( int i = 0; i < (int)m_vlLevelBoundsReward.size(); ++i )
					{
						const S_LEVEL_BOUNDS_REWARD LevelBoundsReward = m_vlLevelBoundsReward.at( i );
						if( LevelBoundsReward.iLevelStart <= iAccessActorLevel &&
							iAccessActorLevel <= LevelBoundsReward.iLevelEnd )
						{
							m_iItemDropGroupTableID = LevelBoundsReward.iItemDropGroupTableID;
						}
					}

					// 구간을 찾을 수 없다면 맨 나중 구간 보상으로 처리.
					if( 0 == m_iItemDropGroupTableID )
						m_iItemDropGroupTableID = m_vlLevelBoundsReward.back().iItemDropGroupTableID;

					m_VecDropItemList.clear();
					CalcDropItemList();
				}

				// #37265 소유권 셋팅이 되어있다면 아이템 드랍 state 에 셋팅해줌.
				if( m_bEnableOwnership )
				{
					m_pItemDropState->SetDropItemOwner( pSession->GetActorHandle() );
				}
				else
				{
					m_pItemDropState->SetDropItemOwner( DnActorHandle() );
				}

				m_pFSM->Process( 0, 0.0f );
				m_pNeedItemCondition->SetAccessActor( CDnActor::Identity() );
			}
			break;
	};
	CDnWorldActProp::OnDispatchMessage( pSession, dwProtocol, pPacket );
}

void CDnWorldChestProp::CmdOperation( int iOperateID )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	//ChestStateEnum State = ChestStateEnum::Opening;

	Stream.Write( &iOperateID, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	m_iOperateState = iOperateID;

	Send( eProp::SC_CMDOPERATION, &Stream );
}

void CDnWorldChestProp::CalcDropItemList()
{
	CDnDropItem::CalcDropItemList( GetRoom(), Dungeon::Difficulty::Max, m_iItemDropGroupTableID, m_VecDropItemList );

	if (m_VecDropItemList.empty())
	{
		// 프랍로그
		//g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, 0, nTableID);	// 스테이지 로그 090226
	}
	else 
	{
		for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID, m_VecDropItemList[i].nEnchantID ) == false )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID ) == false )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			{
				m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
				i--;
				continue;
			}

			// 프랍로그
			//g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, m_VecDropItemList[i].nItemID, nTableID);	// 스테이지 로그 090226
		}
	}
}
