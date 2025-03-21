
#include "StdAfx.h"
#include "MAAiBase.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnPartyTask.h"
#include "TaskManager.h"
#include "MATransAction.h"
#include "DnMonsterActor.h"
#include "DnSkill.h"
#include "DnBlow.h"
#include "MultiRoom.h"
#include "DNUserSession.h"
#include "DnStateBlow.h"
#include "MAScanner.h"
#include "DNAggroSystem.h"

MAAiBase::MAAiBase( DnActorHandle hActor, MAAiReceiver *pReceiver )
{
	m_hActor = hActor;
	m_pMonsterActor = NULL;
	if ( m_hActor && m_hActor->IsMonsterActor())
	{
		m_pMonsterActor = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
	}
	
	m_pReceiver							= pReceiver;
	m_bEnableAggroProcess				= true;
	m_PrevLocalTime						= 0;
	
	// Delay 초기화
	m_nVecDelay.reserve( AIDelayType::Max );
	for( int i=0 ; i<AIDelayType::Max ; ++i )
		m_nVecDelay.push_back( 0 );

	ResetAILook();

	ResetNotifyDieAnnounce();
}

MAAiBase::~MAAiBase()
{

}

void MAAiBase::ResetAILook()
{
	m_bIsAILook			= false;
	m_vAILook			= EtVector2( 0.f, 0.f );
	m_vProjectileTarget	= EtVector3( 0.f, 0.f, 0.f );
}

bool MAAiBase::bIsProjectileTargetSignal()
{
	if( m_bIsAILook )
		return ( EtVec3LengthSq( &m_vProjectileTarget ) > 0.f ) ? true : false;

	return false;
}

bool MAAiBase::Initialize()
{
	return true;
}

void MAAiBase::ProcessDelay( const LOCAL_TIME LocalTime )
{
	if( m_PrevLocalTime == 0 ) 
		m_PrevLocalTime = LocalTime;

	const LOCAL_TIME ElapsedTime = LocalTime - m_PrevLocalTime;

	for( int i=0 ; i<AIDelayType::Max ; ++i )
	{
		if( m_nVecDelay[i] > ElapsedTime )
			m_nVecDelay[i] -= ElapsedTime;
		else
			m_nVecDelay[i] = 0;
	}

	m_PrevLocalTime = LocalTime;
}

void MAAiBase::SetDelay( const AIDelayType Type, const int nDelay )
{
	if( Type >= AIDelayType::Max )
		return;

	m_nVecDelay[Type] = nDelay;
}

bool MAAiBase::IsDelay( const AIDelayType Type ) const
{
	if( Type >= AIDelayType::Max )
		return false;

	return (m_nVecDelay[Type] > 0) ? true : false;
}

void MAAiBase::ResetDelay()
{
	for( int i=0 ; i<AIDelayType::Max ; ++i )
		m_nVecDelay[i] = 0;
}

void MAAiBase::Process( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessDelay( LocalTime );
	ProcessAggro( LocalTime, fDelta );
}

void MAAiBase::ProcessAggro( const LOCAL_TIME LocalTime, const float fDelta )
{
	if( !m_bEnableAggroProcess ) 
		return;

	_ASSERT( m_hActor->GetAggroSystem() != NULL );
	m_hActor->GetAggroSystem()->OnProcessAggro( LocalTime, fDelta );
}

void MAAiBase::ChangeTarget( DnActorHandle hActor, bool bStop/*=true*/ )
{
	DnActorHandle hPrevTarget = m_hTarget;
	SetTarget( hActor );

	// 액션파트 의견으로 Target 이 바뀌었을 때 처리를 바꿔봅니다.
	if( m_hTarget )
	{
		if( !hPrevTarget )
		{
			if( m_hActor->IsMove() && bStop ) 
				m_hActor->CmdStop( "Stand" );
		}
		if( m_hActor->GetLookTarget() != m_hTarget )	
			m_hActor->CmdLook(m_hTarget);			
	}
}

int MAAiBase::_CalcAdditionalAggro( const int nAggro, const float fValue )
{
	if( fValue <= 0.f )
		return -nAggro;
	else if( fValue >= 1.f )
	{
		return static_cast<int>(nAggro*(fValue-1.f));
	}

	return static_cast<int>(-nAggro*(1.f-fValue));
}

void MAAiBase::FindTarget()
{
	/*
	// 아군에게 스킬을 사용할 경우 Target 이 스킬이 종료될때까지 고정해야 하기때문에...
	if( m_hActor->IsProcessSkill() )
	{
		if( m_hTarget && !m_hTarget->IsDie() && m_hTarget->GetTeam() == m_hActor->GetTeam() )
			return;
	}
	*/

	// 스킬을 사용할 경우 Target 이 스킬이 종료될때까지 고정해야 하기때문에...
	if( m_hActor->IsProcessSkill() )
		return;

	CDNAggroSystem* pAggroSystem	= m_hActor->GetAggroSystem();
	bool bIsProvocationTarget;
	DnActorHandle	hNewTarget		= m_bEnableAggroProcess ? pAggroSystem->OnGetAggroTarget( bIsProvocationTarget ) : CDnActor::Identity();

	// 타겟이 없으면 타겟을 찾는다.
	if( !m_hTarget )
	{
		ChangeTarget( hNewTarget );
	}
	// 타겟이 있으면 타겟을 변경해야 할지 계산해본다.
	else
	{
		if( m_hTarget == hNewTarget )
			return;

		if( hNewTarget && bIsProvocationTarget )
		{
			ChangeTarget( hNewTarget );
			return;
		}

		if( m_hActor->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_148)  )
		{
			ChangeTarget( hNewTarget );
			return;
		}

		CDNAggroSystem::AggroStruct* pTargetStruct		= pAggroSystem->GetAggroStruct( m_hTarget );
		CDNAggroSystem::AggroStruct* pNewTargetStruct	= pAggroSystem->GetAggroStruct( hNewTarget );

		if( pTargetStruct == NULL && pNewTargetStruct )
		{
			ChangeTarget( hNewTarget );
		}
		else if( pTargetStruct == NULL && pNewTargetStruct == NULL )
		{
			ChangeTarget( CDnActor::Identity() );
		}
		else if( pTargetStruct && pNewTargetStruct )
		{
			if( hNewTarget && hNewTarget->GetStateBlow() )
			{
				// 프로보크 예외 처리
				if( hNewTarget->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_132 ) )
				{
					ChangeTarget( hNewTarget );
					return;
				}
			}
	
			// 현재 타겟보다 120% 어그로 높으면 타겟 변경
			if( static_cast<int>(pTargetStruct->iModifierAggro*g_fAggroChangeTragetRate) <= pNewTargetStruct->iModifierAggro )
				ChangeTarget( hNewTarget );
		}
	}
}

void MAAiBase::SetTarget( DnActorHandle hActor, int nAggroValue )
{
	if( m_hTarget == hActor ) 
		return;

	m_hTarget = hActor;

	if( !m_hActor )
		return;

	MATransAction *pTrans = static_cast<MATransAction *>(m_hActor.GetPointer());

	if( pTrans ) 
	{
		m_hActor->ResetMove();

		BYTE pBuffer[128];
		CMemoryStream Stream( pBuffer, 128 );
		
		DWORD dwUniqueID = ( m_hTarget ) ? m_hTarget->GetUniqueID() : -1;

		Stream.Write( &dwUniqueID, sizeof(DWORD) );

		pTrans->Send( eActor::SC_AGGROTARGET, &Stream );
	}
	
	if( m_hTarget && nAggroValue ) 
	{
		if( m_hActor->IsMove() ) 
			m_hActor->CmdStop( "Stand" );
		if( m_hActor->GetLookTarget() != m_hTarget )	
			m_hActor->CmdLook(m_hTarget);			

		CDNAggroSystem::AggroStruct* pStruct = m_hActor->GetAggroSystem()->GetAggroStruct( m_hTarget );
		if( pStruct ) 
			pStruct->iAggro = nAggroValue;
		else 
			m_hActor->GetAggroSystem()->AddAggro( hActor, nAggroValue );
	}
}

void MAAiBase::SetProjectileTarget()
{
	if( m_hTarget && m_hActor )
	{
		m_bIsAILook			= true;

#ifdef PRE_FIX_PARTSMONSTER_AI_TARGETTING
		EtVector3 vTargetPosition = m_hTarget->FindAutoTargetPos();
		m_vAILook = EtVec3toVec2( vTargetPosition - *m_hActor->GetPosition() );
#else
		m_vAILook = EtVec3toVec2( *m_hTarget->GetPosition() - *m_hActor->GetPosition() );
#endif
		m_vProjectileTarget	= vTargetPosition;
		EtVec2Normalize( &m_vAILook, &m_vAILook );
		_OnSetProjectileTarget();
	}
}

void MAAiBase::EnableAggorProcess( bool bEnable )
{
	m_bEnableAggroProcess = bEnable;
	
	if( bEnable == false ) 
	{
		ResetAggro();
		SetTarget( CDnActor::Identity() );
		m_hActor->ResetMove();
	}
}

void MAAiBase::ResetAggro()
{
	m_hActor->GetAggroSystem()->ResetAggro();
}

void MAAiBase::ResetNotifyDieAnnounce()
{
	m_bNotifyDieAnnounce		= false;
	m_dwNotifyDieAnnounceTick	= 0;
}

void MAAiBase::NotifyDieAnnounce()
{
	m_bNotifyDieAnnounce		= true;
	m_dwNotifyDieAnnounceTick	= timeGetTime();
}

bool MAAiBase::bIsNotifyDieAnnounce( DWORD dwGap )
{
	if( !m_bNotifyDieAnnounce )
		return false;

	if( timeGetTime()-m_dwNotifyDieAnnounceTick <= dwGap )
		return true;

	return false;
}
