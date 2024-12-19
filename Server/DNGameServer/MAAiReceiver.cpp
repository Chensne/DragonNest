#include "StdAfx.h"
#include "MAAiReceiver.h"
#include "DnActor.h"
#include "DnMonsterActor.h"
#include "MAAiBase.h"
#include "DnActorState.h"
#include "DnTableDB.h"
#include "MAAiScript.h"
#include "DnWorldProp.h"

MAAiReceiver::MAAiReceiver()
:m_pAi(NULL)
{
	m_AIState = AIState::Threat;
}

MAAiReceiver::~MAAiReceiver()
{
	SAFE_DELETE(m_pAi);
}

bool MAAiReceiver::Initialize( int nMonsterTableID, const char *szLuaFileName )
{
	m_hActor = (dynamic_cast<CDnActor*>(this))->GetMySmartPtr();
	if( !m_hActor )
	{
		g_Log.Log(LogType::_ERROR, L"MAAiReceiver::Initialize MonsterTableID:%d LuaFileName:%S\r\n", nMonsterTableID, szLuaFileName );
	}
	
	// AI 가 루아 파일이 있으면 읽고
	if( szLuaFileName && strlen(szLuaFileName) )
	{
		MAAiScript* pAi = new MAAiScript( m_hActor, this );
		pAi->LoadScript( szLuaFileName );
		m_pAi = pAi;
	}
	else
	{
		if( !m_hActor )
			return false;

		CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
		if( pMonster->GetPropHandle() )
			return true;

		_DANGER_POINT();
		return false;
	}

	if( m_pAi ) 
		return m_pAi->Initialize();

	_DANGER_POINT();
	return false;
}

void MAAiReceiver::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_pAi ) return;
	if( m_AIState == AIState::Disable ) return;

	m_pAi->Process( LocalTime, fDelta );
}

void MAAiReceiver::SetAggroTarget( DnActorHandle hActor, int nAggroValue )
{
	if( m_pAi ) m_pAi->SetTarget( hActor, nAggroValue );
}

DnActorHandle MAAiReceiver::GetAggroTarget()
{
	return ( m_pAi ) ? m_pAi->GetTarget() : CDnActor::Identity();
}

DnActorHandle MAAiReceiver::GetReservedProjectileTarget()
{
	return ( m_pAi ) ? m_pAi->GetReservedProjectileTarget() : CDnActor::Identity();
}


void MAAiReceiver::SetAIState( AIState State, bool bRefreshAggro )
{
	m_AIState = State;
	if( m_pAi && bRefreshAggro ) {
		switch( State ) {
			case AIState::Disable:
				m_pAi->EnableAggorProcess( false );
				break;
			case AIState::Gentle:
				m_pAi->EnableAggorProcess( false );
				break;
			case AIState::Threat:
				m_pAi->EnableAggorProcess( true );
				break;
		}
	}
}