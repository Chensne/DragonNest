#include "StdAfx.h"
#include "DnActorStatIntervalManipulator.h"
#include "DnDamageBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



CDnActorStatIntervalManipulator::CDnActorStatIntervalManipulator( DnActorHandle hActor, DnBlowHandle hParentBlow ) : m_hActor( hActor ),
																						   m_hParentBlow( hParentBlow ),
																						   m_LocalStartTime( 0 ),
																						   m_dwInterval( 0 ),
																						   m_LocalElapsedTime( 0 ),
																						   m_LastIntervalTime( 0 )
																	 
{
}

CDnActorStatIntervalManipulator::~CDnActorStatIntervalManipulator(void)
{
}


void CDnActorStatIntervalManipulator::OnBegin( LOCAL_TIME LocalTime, DWORD dwInterval )
{
	m_LocalElapsedTime = 0;

	m_LocalStartTime = LocalTime;
	m_LastIntervalTime = LocalTime;
	m_dwInterval = dwInterval;
}




//void CDnActorStatIntervalManipulator::_ProcessInterval( int& iProcessManipulateCount, int iDestProcessCount )
//{
//	if( !m_hActor || m_hActor->IsDie() )
//		return;
//
//#ifdef _GAMESERVER
//	// 이 객체를 소유한 상태효과는 반드시 주기적으로 처리하는 함수를 구현한 상태여야 함.
//	bool bResult = m_hParentBlow->OnCustomIntervalProcess();
//	_ASSERT( bResult );
//
//	// Blow 로 죽었으면 Blow 시전 Actor 를 Hitter 로 설정해준다.
//	if( m_hActor->IsDie() )
//	{
//		DnActorHandle hHitter;
//		if( m_hParentBlow )
//		{
//			const CDnSkill::SkillInfo* pSkillInfo = m_hParentBlow->GetParentSkillInfo();
//			if( pSkillInfo )
//				hHitter = pSkillInfo->hSkillUser;
//		}
//
//		m_hActor->Die( hHitter );
//	}
//#endif
//}




void CDnActorStatIntervalManipulator::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( 0 == LocalTime )
		return;

	if( !m_hActor || m_hActor->IsDie() )
		return;

	// 존 이동하면 LocalTime 이 리셋되므로..
	if( LocalTime < m_LastIntervalTime )
		m_LastIntervalTime = LocalTime;

	if( m_dwInterval < LocalTime - m_LastIntervalTime )
	{
		// 이 객체를 소유한 상태효과는 반드시 주기적으로 처리하는 함수를 구현한 상태여야 함.
		bool bResult = m_hParentBlow->OnCustomIntervalProcess();
		_ASSERT( bResult );

		m_LastIntervalTime = LocalTime;

#ifdef _GAMESERVER
		// Blow 로 죽었으면 Blow 시전 Actor 를 Hitter 로 설정해준다.
		// 중요!!
		// 반드시 Die 처리가 가장 나중에 되어야 한다.
		// Die 처리가 되면 상태효과가 사라지므로 해당 상태효과가 가지고 있는 이 객체 또한 사라짐.
		// 그러므로 어떠한 조작도 있어선 안된다.

#if defined(PRE_FIX_61382)
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
		if( hActor && hActor->IsDie() )
		{
			DnActorHandle hHitter;
			if( m_hParentBlow )
			{
				const CDnSkill::SkillInfo* pSkillInfo = m_hParentBlow->GetParentSkillInfo();
				if( pSkillInfo )
					hHitter = pSkillInfo->hSkillUser;
			}

			hActor->Die( hHitter );

			// 이 아래에서 어떠한 변수 값 변경이나 처리를 하면 안됨!
			return;
		}
#else
		if( m_hActor->IsDie() )
		{
			DnActorHandle hHitter;
			if( m_hParentBlow )
			{
				const CDnSkill::SkillInfo* pSkillInfo = m_hParentBlow->GetParentSkillInfo();
				if( pSkillInfo )
					hHitter = pSkillInfo->hSkillUser;
			}

			m_hActor->Die( hHitter );

			// 이 아래에서 어떠한 변수 값 변경이나 처리를 하면 안됨!
			return;
		}
#endif // PRE_FIX_61382
#endif
	}
}


void CDnActorStatIntervalManipulator::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	//if( false == m_hActor || m_hActor->IsDie() )
	//	return;

	//if( m_hParentBlow->GetDurationTime() > 0.0f )
	//{
	//	// 중간에 끊어진 상태라면 ServerEndTime은 이미 CmdRemoveStateEffect 패킷을 쏘는 시점에 그 시간으로 갱신된 상태임..
	//	LOCAL_TIME WholeElapsed = m_ServerEndTime - m_ServerStartTime;
	//	int iChangedProcessCount = int(WholeElapsed / m_dwInterval) + 1;

	//	if( m_iProcessedManipulateCount < iChangedProcessCount )
	//		_ProcessInterval( m_iProcessedManipulateCount, iChangedProcessCount );
	//}
	//else
	//{	
	//	_ProcessInterval( m_iProcessedManipulateCount, m_iExpectedManipulateCount );
	//}
}
