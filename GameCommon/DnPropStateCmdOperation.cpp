#include "StdAfx.h"
#include "DnPropStateCmdOperation.h"
#include "DnWorldActProp.h"


CDnPropStateCmdOperation::CDnPropStateCmdOperation( DnPropHandle hEntity ) : CDnPropState( hEntity )
{
	m_iType = CMD_OPERATION;
	m_iOperateIDToSend = 0;
}

CDnPropStateCmdOperation::~CDnPropStateCmdOperation(void)
{
}


bool CDnPropStateCmdOperation::OnEntry( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnEntry( LocalTime, fDelta );

	if( !m_hEntity )
		return false;

	CDnWorldActProp* pActProp = static_cast<CDnWorldActProp*>(m_hEntity.GetPointer());
	if( NULL == pActProp )
	{
#ifdef ENABLE_PROP_STATE_LOG
		OutputDebug( "[?? Prop FSM Log] CDnPropStateCmdOperation::OnEntry Entity Prop is NULL!\n" );
#endif
		return false;
	}

	pActProp->CmdOperation( m_iOperateIDToSend );

#ifdef _DEBUG
	OutputDebug( "[Prop(%d) FSM Log] CDnPropStateCmdOperation::OnEntry (Send Operate ID:%d)\n", m_hEntity->GetUniqueID(), m_iOperateIDToSend );
#endif

	return true;
}


bool CDnPropStateCmdOperation::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnProcess( LocalTime, fDelta );

	return true;
}


bool CDnPropStateCmdOperation::OnLeave( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnLeave( LocalTime, fDelta );

#ifdef ENABLE_PROP_STATE_LOG
	OutputDebug( "[Prop(%d) FSM Log] CDnPropStateCmdOperation::OnLeave (Send Operate ID:%d)\n", m_hEntity->GetUniqueID(), m_iOperateIDToSend );
#endif

	return true;
}
