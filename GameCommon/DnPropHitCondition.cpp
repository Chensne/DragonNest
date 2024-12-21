#include "StdAfx.h"
#include "DnPropHitCondition.h"
#include "DnWorldActProp.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPropHitCondition::CDnPropHitCondition( DnPropHandle hEntity ) : CDnPropCondition(hEntity)
{

}

CDnPropHitCondition::~CDnPropHitCondition(void)
{
}


bool CDnPropHitCondition::IsSatisfy( void )
{
	bool bResult = false;

	if( !m_hEntity )
		return false;

	CDnWorldActProp* pActProp = static_cast<CDnWorldActProp*>(m_hEntity.GetPointer());
	if( NULL == pActProp )
	{
#ifdef ENABLE_PROP_CONDITION_LOG
		OutputDebug( "[?? Prop FSM Condition Log] CDnPropHitCondition::IsSatisfy - Entity Prop is NULL!\n" );
#endif
		return false;
	}

	if( pActProp->IsHittedAndReset() )
		bResult = true;

#ifdef ENABLE_PROP_CONDITION_LOG
	char* pResult = NULL;
	if( bResult )
		pResult = "true";
	else
		pResult = "false";

	OutputDebug( "[Prop(%d) FSM Condition Log] CDnPropHitCondition::IsSatisfy Result: \"%s\"\n", m_hEntity->GetUniqueID(), pResult );
#endif

	return bResult;
}