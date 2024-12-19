#include "StdAfx.h"
#include "DnWorldChestProp.h"
#include "DNProtocol.h"
#include "DnPartyTask.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DNProtocol.h"
#include "DnPropState.h"
#include "DnPropCondition.h"
#include "DnPropStateDoAction.h"
#include "DnPropOperatedCondition.h"
#include "DnLocalPlayerActor.h"
#include "DnItemTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldChestProp::CDnWorldChestProp()
{
	//m_nCloseActionIndex = -1;
	m_iNeedKeyID = 0;
	m_iItemDropGroupTableID = 0;
	m_bRemoveNeedKeyItem = false;
	m_iNumNeedKeyItem = 0;

	m_iServerOperateResult = CLOSED;
}

CDnWorldChestProp::~CDnWorldChestProp()
{
	ReleasePostCustomParam();
}

bool CDnWorldChestProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	if( GetData() )
	{
		ChestStruct *pStruct = (ChestStruct*)GetData();
		m_iNeedKeyID = pStruct->nNeedKeyID;
		m_iItemDropGroupTableID = pStruct->nItemDropGroupTableID;
		m_bRemoveNeedKeyItem = (pStruct->bRemoveKeyItem == TRUE);
		m_iNumNeedKeyItem = pStruct->nNumNeedItem;

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
			// #38984 클라이언트에서 생성만 하고 실제로 뿌려지진 않는다. 
			// 프랍 생성시 테이블의 _VisibleProp 이나 CDnDropItem::CalcDropItemList 내부에서 사용되는
			// _rand 함수는 중간에 시드값을 맞춰주지 않기 때문에 서버 클라간 호출하는 횟수가 동일해야한다.
			CDnDropItem::CalcDropItemList( -1, m_iItemDropGroupTableID, m_VecDropItemList );

			for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
				if( CDnDropItem::PreInitializeItem( m_VecDropItemList[i].nItemID ) == false ) {
					m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
					i--;
				}
			}
		}
	}

	return true;
}


bool CDnWorldChestProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	CDnPropState* pNormalState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	CDnPropState* pOpenActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	CDnPropState* pItemDropState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::ITEM_DROP );
	static_cast<CDnPropStateDoAction*>(pOpenActionState)->AddActionName( "Close_Open" );
	
	CDnPropCondition* pOpenOperateCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::OPERATED );
	CDnPropCondition* pCloseOperateCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::OPERATED );
	CDnPropCondition* pNULLCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::NULL_CONDITION );
	static_cast<CDnPropOperatedCondition*>(pOpenOperateCondition)->SetRequireState( OPEN_SUCCESS );
	static_cast<CDnPropOperatedCondition*>(pCloseOperateCondition)->SetRequireState( OPEN_FAILED );

	m_pFSM->AddCondition( pOpenOperateCondition );
	m_pFSM->AddCondition( pCloseOperateCondition );
	m_pFSM->AddCondition( pNULLCondition );

	m_pFSM->AddState( pNormalState );
	m_pFSM->AddState( pOpenActionState );
	m_pFSM->AddState( pItemDropState );

	pNormalState->AddTransitState( pOpenActionState, pOpenOperateCondition );
	pOpenActionState->AddTransitState( pItemDropState, pNULLCondition );

	m_pFSM->SetEntryState( pNormalState );

	return CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
}


// 프랍에 마우스를 클릭하면 이걸 호출해 줌.
void CDnWorldChestProp::CmdOperation()
{
	Send( eProp::CS_CMDOPERATION, NULL );
}


bool CDnWorldChestProp::CanOpen( void )
{
	// 키 아이템이 정의 되어있는 경우 갯수까지 충족되어야 열 수 있는 것임.
	if( 0 < m_iNeedKeyID )
	{
		CDnCharInventory& Inventory = GetItemTask().GetCharInventory();
		int iHasItemCount = Inventory.GetItemCount( m_iNeedKeyID );
		if( m_iNumNeedKeyItem <= iHasItemCount )
		{
			return true;
		}

		return false;
	}
	
	return true;
}

void CDnWorldChestProp::OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket )
{
	switch( dwProtocol ) {
		case eProp::SC_CMDOPERATION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				//ChestStateEnum State;
				Stream.Read( &m_iServerOperateResult, sizeof(int), CPacketCompressStream::INTEGER_SHORT );

				// 만약 오픈 성공이면 드랍 아이템을 레벨에 맞춰서 셋팅
				if( OPEN_SUCCESS == m_iServerOperateResult )
				{
					// 레벨 구간별 보상이 설정되어있다면 드란 아이템 값 갱신.
					if( false == m_vlLevelBoundsReward.empty() )
					{
						// 조작한 녀석의 레벨에 맞는 보상 드랍 아이템 ID 셋팅.
						m_iItemDropGroupTableID = 0;
						int iAccessActorLevel = CDnLocalPlayerActor::s_hLocalActor->GetLevel();
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

						// #38984 클라이언트에서 생성만 하고 실제로 뿌려지진 않는다. 
						// 프랍 생성시 테이블의 _VisibleProp 이나 CDnDropItem::CalcDropItemList 내부에서 사용되는
						// _rand 함수는 중간에 시드값을 맞춰주지 않기 때문에 서버 클라간 호출하는 횟수가 동일해야한다.
						CDnDropItem::CalcDropItemList( -1, m_iItemDropGroupTableID, m_VecDropItemList );

						for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
							if( CDnDropItem::PreInitializeItem( m_VecDropItemList[i].nItemID ) == false ) {
								m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
								i--;
							}
						}
					}
				}
			}
			break;
	}
	CDnWorldOperationProp::OnDispatchMessage( dwProtocol, pPacket );
}

int CDnWorldChestProp::GetCrosshairType()
{
	switch( GetOperateState() ) {
		case CDnWorldChestProp::CLOSED:
			if( CanOpen() ) return CDnLocalPlayerActor::CrossHairType::Chest_UnLock;
			else return CDnLocalPlayerActor::CrossHairType::Chest_Lock;
			break;

		case CDnWorldChestProp::OPEN_SUCCESS:
			break;
	}
	return CDnLocalPlayerActor::CrossHairType::Normal;
}