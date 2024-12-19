#include "StdAfx.h"
#include "DnCantActionBlow.h"
#include "DnChangeStandActionBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCantActionBlow::CDnCantActionBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), 
																					m_strActionWhenCancelAttack( "Stand" )
																					,m_isAvailableNormalState(false)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_071;
}

CDnCantActionBlow::~CDnCantActionBlow(void)
{
}


void CDnCantActionBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

	if( m_hActor->GetCantActionSEReferenceCount() == 0 )
	{
		// ���� �� ��ų�� ������� ���ϰ� �Ѵ�. 
		// ��ų�� UseSkill �� �� ���� ȿ���� üũ��� �Ѵ�.
		// ���� �������� �׼ǵ� ���
		if( false == m_hActor->IsDie() )
		{
			bool availableAction = false;
			if (m_isAvailableNormalState)
				availableAction = true;
			else
				availableAction = m_hActor->IsMove() || m_hActor->IsAttack();

			if (availableAction)
				m_hActor->SetAction( m_strActionWhenCancelAttack.c_str(), 0.0f, 3.0f );

		}
		
		m_hActor->SetStateEffect( m_hActor->GetStateEffect() | CDnActorState::Cant_AttackAction );
	}

	m_hActor->AddedCantActionSE();

	if( m_hActor->IsPlayerActor() == true )
	{
#ifdef PRE_FIX_83091
#ifdef _GAMESERVER
		CDnChangeStandActionBlow::ReleaseStandChangeSkill( m_hActor , true );
#endif
#endif
	}

	OutputDebug( "CDnCantActionBlow::OnBegin Ref [%d --> %d]\n", m_hActor->GetCantActionSEReferenceCount()-1, m_hActor->GetCantActionSEReferenceCount() );
}


void CDnCantActionBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	m_hActor->RemovedCantActionSE();
	
	if( m_hActor->GetCantActionSEReferenceCount() == 0 )
	{
		// �� ���� ȸ��
		m_hActor->SetStateEffect( m_hActor->GetStateEffect() & ~CDnActorState::Cant_AttackAction );
	}
	
	OutputDebug( "CDnCantActionBlow::OnEnd Ref [%d --> %d]\n", m_hActor->GetCantActionSEReferenceCount()+1, m_hActor->GetCantActionSEReferenceCount() );

}

void CDnCantActionBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

	// [2010/11/26 semozz]
	// �������� ����ȭ�� �ʿ��� StateBlow�� �������� ��Ŷ �޾Ƽ� ���� �ϵ���
	// Ŭ���̾�Ʈ������ STATE_END�� ��ȯ���� �ʴ´�.
	// [2010/12/13 semozz]
	// IsPermanent�� ���� �Ǿ� ���� �ʰ�
	if( !IsPermanent() &&
		m_StateBlow.fDurationTime <= 0.0f )
	{
#if defined(_GAMESERVER)
		//�������� StateBlow���� ��Ŷ�� ������
		m_hActor->SendRemoveStateEffectFromID(GetBlowID());
		m_StateBlow.fDurationTime = 0.0f;
		SetState( STATE_BLOW::STATE_END );

		// [2010/12/22 semozz]
		// �̹� �����ӿ��� ������ ���� ���� ����Ʈ�� ����Ѵ�.
		// ���⼭ ��� �� �ϸ� ������ �����ӿ��� ������.
		m_hActor->RemoveStateBlowFromID(GetBlowID());
#else
		m_StateBlow.fDurationTime = 0.01f;
		SetState( STATE_BLOW::STATE_DURATION );
#endif	
	}
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCantActionBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnCantActionBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

