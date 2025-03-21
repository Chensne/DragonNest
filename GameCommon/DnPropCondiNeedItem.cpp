#include "StdAfx.h"
#include "DnPropCondiNeedItem.h"
#include "DnWorldActProp.h"
#include "DnPlayerActor.h"
#include "DnItemTask.h"
#include "DNUserItem.h"
#include "DNUserSession.h"
#include "DnItem.h"


CDnPropCondiNeedItem::CDnPropCondiNeedItem( DnPropHandle hEntity ) : CDnPropCondition( hEntity ),
																	 m_iNeedItemID( 0 ),
																	 m_iNumNeedItem( 0 ),
																	 m_bRemoveNeedItem( false )
{
	m_iType = NEED_ITEM;
}

CDnPropCondiNeedItem::~CDnPropCondiNeedItem(void)
{
}


bool CDnPropCondiNeedItem::IsSatisfy( void )
{
	bool bResult = false;

	if( !m_hEntity )
		return false;

	CDnWorldActProp* pActProp = static_cast<CDnWorldActProp*>(m_hEntity.GetPointer());
	if( NULL == pActProp )
	{
#ifdef ENABLE_PROP_CONDITION_LOG
		OutputDebug( "[?? Prop FSM Condition Log] CDnPropCondiNeedItem::IsSatisfy - Entity Prop is NULL!\n" );
#endif
		return false;
	}

	if( m_hAccessActor && m_hAccessActor->IsPlayerActor() )
	{
		if( 0 < m_iNeedItemID )
		{
			CDnItemTask* pItemTask = m_hEntity->GetGameRoom()->GetItemTask();
			int iHasItemCount = pItemTask->ScanItemFromID( m_hAccessActor, m_iNeedItemID, NULL );
			if( m_iNumNeedItem <= iHasItemCount )
			{
				bResult = true;

				// 추가적으로 필요 아이템을 제거 해야한다면 체크.
				if( true == m_bRemoveNeedItem )
				{
					static_cast<CDnPlayerActor*>(m_hAccessActor.GetPointer())->GetUserSession()->GetItem()->DeleteInventoryByItemID( m_iNeedItemID, m_iNumNeedItem, DBDNWorldDef::UseItem::Use );
				}
			}
		}
		else
			bResult = true;		// 아이템이 필요 없으면 바로 조건 충족.
	}

	return bResult;
}
