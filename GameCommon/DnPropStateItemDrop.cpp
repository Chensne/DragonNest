#include "StdAfx.h"
#include "DnPropStateItemDrop.h"
#include "DnWorldActProp.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnItemTask.h"
#ifdef _GAMESERVER
#include "DNUserSession.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



CDnPropStateItemDrop::CDnPropStateItemDrop( DnPropHandle hEntity ) : CDnPropState(hEntity)
{
	m_iType = ITEM_DROP;
}

CDnPropStateItemDrop::~CDnPropStateItemDrop(void)
{
}

bool CDnPropStateItemDrop::OnEntry( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnEntry( LocalTime, fDelta );

#ifdef _GAMESERVER
	if( !m_hEntity )
		return false;

	CDnWorldActProp* pActProp = static_cast<CDnWorldActProp*>( m_hEntity.GetPointer() );

#ifdef ENABLE_PROP_STATE_LOG
	if( NULL == pActProp )
		OutputDebug( "[?? Prop FSM Log] CDnPropStateItemDrop::OnEntry Entity Prop is NULL!\n" );
	else
		OutputDebug( "[Prop(%d) FSM Log] CDnPropStateItemDrop::OnEntry Entity Prop is Valid \n", m_hEntity->GetUniqueID() );
#endif

	if( NULL == pActProp )
		return false;

	CDnPartyTask* pTask = (CDnPartyTask *)CTaskManager::GetInstance(m_hEntity->GetRoom()).GetTask( "PartyTask" );
	if( !pTask ) return false;

	int iNumDropItem = pActProp->GetNumDropItem();
	int nItemCount = iNumDropItem;

	// #37265 아이템 드랍을 서버 사이드로 변경.
	// 아이템 뿌려준다~
	CDnItemTask *pItemTask = (CDnItemTask *)CTaskManager::GetInstance(m_hEntity->GetRoom()).GetTask( "ItemTask" );
	EtVector3 vPos = m_hEntity->GetMatEx()->m_vPosition;//m_matExWorld.m_vPosition;
	//	vPos.y = CDnWorld::GetInstance(GetRoom()).GetHeight( vPos );

	for( int i = 0; i < iNumDropItem; i++ ) 
	{
		const CDnItem::DropItemStruct *pItem = pActProp->GetDropItemByIndex( i );

		int nRotate = (int)( ( ( ( m_hEntity->GetUniqueID() * 10 ) + 360 ) / (float)iNumDropItem ) * i );
		
		// #37265 주인이 있는 경우엔 소유권 쥐어줘서 드랍.
		if( m_hDropItemOwner )
		{
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pItemTask->RequestDropItem( pItem->dwUniqueID, vPos, pItem->nItemID, pItem->nSeed, pItem->nCount, nRotate, m_hDropItemOwner->GetSessionID(), pItem->nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pItemTask->RequestDropItem( pItem->dwUniqueID, vPos, pItem->nItemID, pItem->nSeed, pItem->nCount, nRotate, m_hDropItemOwner->GetSessionID() );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			
		}
		else
		{
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pItemTask->RequestDropItem( pItem->dwUniqueID, vPos, pItem->nItemID, pItem->nSeed, pItem->nCount, nRotate, -1, pItem->nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pItemTask->RequestDropItem( pItem->dwUniqueID, vPos, pItem->nItemID, pItem->nSeed, pItem->nCount, nRotate );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		}
	}
	
	pActProp->ClearDropItemList();
#endif

	return true;
}


bool CDnPropStateItemDrop::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnProcess( LocalTime, fDelta );

	return true;
}


bool CDnPropStateItemDrop::OnLeave( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnLeave( LocalTime, fDelta );

#ifdef ENABLE_PROP_STATE_LOG
	OutputDebug( "[Prop(%d) FSM Log] CDnPropStateItemDrop::OnLeave\n", m_hEntity->GetUniqueID() );
#endif

	return true;
}