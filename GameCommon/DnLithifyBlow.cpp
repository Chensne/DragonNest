#include "StdAfx.h"
#include "DnLithifyBlow.h"
#include "DnPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnLithifyBlow::CDnLithifyBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), 
																			  m_fStepTime( 0.0f ),
																			  m_iNowStep( 0 )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_063;
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

	m_fOriDurationTime = 0.0f;
}

CDnLithifyBlow::~CDnLithifyBlow(void)
{

}


void CDnLithifyBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnLithifyBlow::OnBegin, Value: %2.2f\n", m_fValue );

	// 클라이언트일때.. 플레이어 액터인 경우엔 위기 탈출 UI 표시
#ifndef _GAMESERVER
	if( dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer()) )
	{
		// TODO: 표시!
	}
#endif
	
	float fDurationTime = GetDurationTime();
	m_fOriDurationTime = fDurationTime;
	m_fStepTime = fDurationTime / 3.0f;
}


void CDnLithifyBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifndef _GAMESERVER
	float fDurationTime = GetDurationTime();
	// 클라인 경우엔 위기 탈출 시스템 처리!

#endif

	switch( m_iNowStep )
	{
		case 0:
			if( m_fStepTime*2.0f >= m_fOriDurationTime )
			{
				// 1단계 통과
				++m_iNowStep;
			}
			break;

		case 1:
			if( m_fStepTime >= m_fOriDurationTime )
			{
				// 게임 서버인 경우엔 2단계 통과 데미지 주고 상태효과 종료
#ifdef _GAMESERVER
				DWORD dwHitterUniqueID = m_ParentSkillInfo.hSkillUser ? m_ParentSkillInfo.hSkillUser->GetUniqueID() : -1;
				INT64 nHPDelta = -INT64(m_fValue*(float)m_hActor->GetHP());
				m_hActor->SetHP( m_hActor->GetHP() + nHPDelta );
				m_hActor->RequestHPMPDelta( CDnState::ElementEnum_Amount, nHPDelta, dwHitterUniqueID );
#endif

				SetState( STATE_BLOW::STATE_END );
				++m_iNowStep;
			}
			break;
	}
}


void CDnLithifyBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// 플레이어 액터인 경우엔 위기 탈출 UI 제거
#ifndef _GAMESERVER
	if( dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer()) )
	{
		// TODO: 제거!
	}
#endif

	OutputDebug( "CDnLithifyBlow::OnEnd, Value: %2.2f\n", m_fValue );
}