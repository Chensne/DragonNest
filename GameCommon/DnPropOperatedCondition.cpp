#include "StdAfx.h"
#include "DnPropOperatedCondition.h"
#include "DnWorldActProp.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPropOperatedCondition::CDnPropOperatedCondition( DnPropHandle hEntity ) : CDnPropCondition( hEntity ), 
																			 m_iRequireOperateState( -1 )
																			 
{
	m_iType = OPERATED;
}


CDnPropOperatedCondition::~CDnPropOperatedCondition(void)
{

}


bool CDnPropOperatedCondition::IsSatisfy( void )
{
	bool bResult = false;

	if( !m_hEntity )
		return false;

	CDnWorldActProp* pActProp = static_cast<CDnWorldActProp*>(m_hEntity.GetPointer());
	if( NULL == pActProp )
	{
#ifdef ENABLE_PROP_CONDITION_LOG
		OutputDebug( "[?? Prop FSM Condition Log] CDnPropOperatedCondition::IsSatisfy - Entity Prop is NULL!\n" );
#endif
		return false;
	}

	int iNowOperateState = pActProp->GetOperateState();
	if( m_iRequireOperateState == iNowOperateState )
		bResult = true;

#ifdef ENABLE_PROP_CONDITION_LOG
	if( bResult )
		OutputDebug( "[Prop(%d) FSM Condition Log] CDnPropOperatedCondition::IsSatisfy - Result: \"true\"(Require:%d == Now:%d)\n", m_hEntity->GetUniqueID(), 
					 m_iRequireOperateState, iNowOperateState );
	else
		OutputDebug( "[Prop(%d) FSM Condition Log] CDnPropOperatedCondition::IsSatisfy - Result: \"false\"(Require:%d == Now:%d)\n", m_hEntity->GetUniqueID(), 
					 m_iRequireOperateState, iNowOperateState );
#endif

	return bResult;
}