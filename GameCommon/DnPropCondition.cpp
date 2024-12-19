#include "StdAfx.h"
#include "DnPropCondition.h"
#include "DnWorldActProp.h"
#include "DnPropNULLCondition.h"
#include "DnPropCondiDurability.h"
#include "DnPropHitCondition.h"
#include "DnPropOperatedCondition.h"
#ifdef _GAMESERVER
#include "DnPropCondiNeedItem.h"
#include "DnPropActionCondition.h"
#include "DnPropCondiSubDurability.h"
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnPropCondition::CDnPropCondition( DnPropHandle hEntity ) : TDnFSMTransitCondition<DnPropHandle>(hEntity),
															 m_iType( COUNT )
{
	// CDnWorldActProp 타입만 받아들여요~
	_ASSERT( hEntity );
	_ASSERT( dynamic_cast<CDnWorldActProp*>(hEntity.GetPointer()) );
}

CDnPropCondition::~CDnPropCondition(void)
{

}


CDnPropCondition* CDnPropCondition::Create( DnPropHandle hEntity, int iType )
{
	CDnPropCondition* pNewCondition = NULL;

	switch( iType )
	{
		case NULL_CONDITION:
			pNewCondition = new CDnPropNULLCondition( hEntity );
			break;

		case COMPARE_DURABILITY:
			pNewCondition = new CDnPropCondiDurability( hEntity );
			break;

		case IS_HIT:
			pNewCondition = new CDnPropHitCondition( hEntity );
			break;

		case OPERATED:
			pNewCondition = new CDnPropOperatedCondition( hEntity );
			break;
		
#ifdef _GAMESERVER
		case NEED_ITEM:
			pNewCondition = new CDnPropCondiNeedItem( hEntity );
			break;

		case ACTION_CHECK:
			pNewCondition = new CDnPropActionCondition( hEntity );
			break;

		case COMPARE_SUB_DURABILITY:
			pNewCondition = new CDnPropCondiSubDurability( hEntity );
			break;
#endif
	}

	return pNewCondition;
}
