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

	// Ŭ���̾�Ʈ�϶�.. �÷��̾� ������ ��쿣 ���� Ż�� UI ǥ��
#ifndef _GAMESERVER
	if( dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer()) )
	{
		// TODO: ǥ��!
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
	// Ŭ���� ��쿣 ���� Ż�� �ý��� ó��!

#endif

	switch( m_iNowStep )
	{
		case 0:
			if( m_fStepTime*2.0f >= m_fOriDurationTime )
			{
				// 1�ܰ� ���
				++m_iNowStep;
			}
			break;

		case 1:
			if( m_fStepTime >= m_fOriDurationTime )
			{
				// ���� ������ ��쿣 2�ܰ� ��� ������ �ְ� ����ȿ�� ����
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
	// �÷��̾� ������ ��쿣 ���� Ż�� UI ����
#ifndef _GAMESERVER
	if( dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer()) )
	{
		// TODO: ����!
	}
#endif

	OutputDebug( "CDnLithifyBlow::OnEnd, Value: %2.2f\n", m_fValue );
}