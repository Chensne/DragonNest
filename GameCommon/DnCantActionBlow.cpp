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
		// 공격 및 스킬을 사용하지 못하게 한다. 
		// 스킬은 UseSkill 할 때 상태 효과를 체크토록 한다.
		// 현재 진행중인 액션도 취소
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
		// 원 상태 회복
		m_hActor->SetStateEffect( m_hActor->GetStateEffect() & ~CDnActorState::Cant_AttackAction );
	}
	
	OutputDebug( "CDnCantActionBlow::OnEnd Ref [%d --> %d]\n", m_hActor->GetCantActionSEReferenceCount()+1, m_hActor->GetCantActionSEReferenceCount() );

}

void CDnCantActionBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

	// [2010/11/26 semozz]
	// 서버와의 동기화기 필요한 StateBlow는 서버에서 패킷 받아서 제거 하도록
	// 클라이언트에서는 STATE_END로 전환하지 않는다.
	// [2010/12/13 semozz]
	// IsPermanent가 설정 되어 있지 않고
	if( !IsPermanent() &&
		m_StateBlow.fDurationTime <= 0.0f )
	{
#if defined(_GAMESERVER)
		//서버에서 StateBlow제거 패킷을 보낸다
		m_hActor->SendRemoveStateEffectFromID(GetBlowID());
		m_StateBlow.fDurationTime = 0.0f;
		SetState( STATE_BLOW::STATE_END );

		// [2010/12/22 semozz]
		// 이번 프레임에서 삭제를 위해 삭제 리스트에 등록한다.
		// 여기서 등록 안 하면 다음번 프레임에서 빠진다.
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

