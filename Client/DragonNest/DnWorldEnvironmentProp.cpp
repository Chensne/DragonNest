#include "stdafx.h"
#include "DnWorldEnvironmentProp.h"
#include "TaskManager.h"
#include "DnPartyTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



CDnWorldEnvironmentProp::CDnWorldEnvironmentProp( void ) : m_fRange( 0.0f ), m_bActorIsInRange( false )
{

}


CDnWorldEnvironmentProp::~CDnWorldEnvironmentProp( void )
{
	ReleasePostCustomParam();
}

bool CDnWorldEnvironmentProp::InitializeTable( int nTableID )
{
	bool bResult = CDnWorldActProp::InitializeTable( nTableID );
	if( bResult )
	{
		if( GetData() )
		{
			EnvironmentPropStruct* pEnvironmentPropStruct = static_cast<EnvironmentPropStruct*>( m_pData );
			m_fRange = pEnvironmentPropStruct->fRange;

			if( pEnvironmentPropStruct->OnRangeInActionName && pEnvironmentPropStruct->OnRangeOutActionName &&
				strlen( pEnvironmentPropStruct->OnRangeInActionName ) > 0 && strlen( pEnvironmentPropStruct->OnRangeOutActionName ) > 0 )
			{
				m_strOnRangeInAction = pEnvironmentPropStruct->OnRangeInActionName;
				m_strOnRangeOutAction = pEnvironmentPropStruct->OnRangeOutActionName;
			}
		}
	}

	return bResult;
}


void CDnWorldEnvironmentProp::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnWorldActProp::Process( LocalTime, fDelta );

	if( m_strOnRangeOutAction.empty() || m_strOnRangeOutAction.empty() )
		return;

	// 거리에 파티원 들어왔을 때와 나갔을 때 특정 액션 취함.
	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>( CTaskManager::GetInstance().GetTask( "PartyTask" ) );

	if( pPartyTask )
	{
		m_bActorIsInRange = false;
		DWORD dwNumParty = pPartyTask->GetPartyCount();
		for( DWORD i = 0; i < dwNumParty; ++i )
		{
			CDnPartyTask::PartyStruct* pPartyStruct = pPartyTask->GetPartyData( i );
			if( pPartyStruct->hActor )
			{
				EtVector3* pActorPos = pPartyStruct->hActor->GetPosition();
				float fLength = EtVec3Length( &EtVector3(m_matExWorld.m_vPosition - *pActorPos) );
				if( fLength < m_fRange )
				{
					m_bActorIsInRange = true;
					break;
				}
			}
		}
	}

	// 영역 안에 처음 들어온 경우. 바로 반응해야 함.
	if( m_bActorIsInRange )
	{
		if( strcmp( GetCurrentAction(), "Idle" ) == 0 )
			SetActionQueue( m_strOnRangeInAction.c_str() );
	}
	else
	{
		// Summon_Off 뒤에 next action 지정이 안되어 있음.
		// 따라서 끝까지 액션이 재생되면 OnFinishAction 이 호출 안되므로 여기서 체크한다.
		if( strlen( GetCurrentAction() ) == 0 )
			SetActionQueue( m_strOnRangeOutAction.c_str() );
	}
}


void CDnWorldEnvironmentProp::OnFinishAction( const char* szPrevAction, const char *szNextAction, LOCAL_TIME time )
{
	if( m_strOnRangeOutAction.empty() || m_strOnRangeOutAction.empty() )
		return;

	// 영역 안에서 벗어난 경우.
	// Summon_On 액션 중에 액션이 바로 바뀌면 안되므로 여기서 Summon_On 액션이 끝나는 시점에 체크해 준다.
	if( false == m_bActorIsInRange )
	{
		if( strcmp( szPrevAction, m_strOnRangeInAction.c_str() ) == 0 )
			SetActionQueue( m_strOnRangeOutAction.c_str() );
	}
}