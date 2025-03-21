#include "StdAfx.h"
#include "DnPropStateDoAction.h"
#include "DnWorldActProp.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPropStateDoAction::CDnPropStateDoAction( DnPropHandle hEntity ) : CDnPropState( hEntity ),
																	 m_bUseCmdAction( false )
{
	m_iType = DO_ACTION;
}

CDnPropStateDoAction::~CDnPropStateDoAction(void)
{

}



// 1개 이상이면 랜덤으로 돌림. 이 때 시드값은 프랍한테 받는다. 반드시 패킷으로 나간 클라랑 동일한 시드를 받아야 하기 땜시..
bool CDnPropStateDoAction::OnEntry( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnEntry( LocalTime, fDelta );

	if( !m_hEntity )
		return false;

	CDnWorldActProp* pActProp = static_cast<CDnWorldActProp*>(m_hEntity.GetPointer());
	if( NULL == pActProp )
	{
#ifdef ENABLE_PROP_STATE_LOG
		OutputDebug( "[?? Prop FSM Log] CDnPropStateDoAction::OnEntry Entity Prop is NULL!\n" );
#endif
		return false;
	}

#ifdef ENABLE_PROP_STATE_LOG
	string strActions;

	for( int i = 0; i < (int)m_vlAvailActionList.size(); ++i )
	{
		strActions.append( m_vlAvailActionList.at(i) );
		strActions.append( ", " );
	}

	if( 1 < (int)m_vlAvailActionList.size() )
		OutputDebug( "[Prop(%d) FSM Log] CDnPropStateDoAction::OnEntry - %s actions HitRandomSeed: %d\n", m_hEntity->GetUniqueID(), strActions.c_str(), pActProp->GetLastHitRandomSeed() );
	else
	if( m_vlAvailActionList.empty() )
		OutputDebug( "[??Prop(%d) FSM Log] CDnPropStateDoAction::OnEntry action list is EMPTY!!\n", m_hEntity->GetUniqueID(), strActions.c_str() );
	else
	if( m_bUseCmdAction )
		OutputDebug( "[Prop(%d) FSM Log] CDnPropStateDoAction::OnEntry - %s action (Use CmdAction)\n", m_hEntity->GetUniqueID(), strActions.c_str() );
	else
		OutputDebug( "[Prop(%d) FSM Log] CDnPropStateDoAction::OnEntry - %s action\n", m_hEntity->GetUniqueID(), strActions.c_str() );
#endif

	if( m_vlAvailActionList.empty() )
		return false;

	// 액션이 여러개인 경우엔 랜덤으로 출력..
	int iActionIndex = 0;
	int iNumActionList = (int)m_vlAvailActionList.size();
	if( iNumActionList > 1 )
	{
		int iSeed = pActProp->GetLastHitRandomSeed();

#ifdef _GAMESERVER
		_srand( m_hEntity->GetRoom(), iSeed );
		iActionIndex = _rand( m_hEntity->GetRoom() ) % (int)m_vlAvailActionList.size();
#else
		_srand( iSeed );
		iActionIndex = _rand() % (int)m_vlAvailActionList.size();
#endif
	}

	// 액션 실행!
	if( false == m_bUseCmdAction )
		pActProp->SetActionQueue( m_vlAvailActionList.at(iActionIndex).c_str() );
	else
		pActProp->CmdAction( m_vlAvailActionList.at(iActionIndex).c_str() ); 

	return true;
}


bool CDnPropStateDoAction::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnProcess( LocalTime, fDelta );

	// 암것도 안함..

	return true;
}


bool CDnPropStateDoAction::OnLeave( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPropState::OnLeave( LocalTime, fDelta );

#ifdef ENABLE_PROP_STATE_LOG
	OutputDebug( "[Prop(%d) FSM Log] CDnPropStateDoAction::OnLeave", m_hEntity->GetUniqueID() );
#endif

	return true;
}