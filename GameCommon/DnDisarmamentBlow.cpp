#include "StdAfx.h"
#include "DnDisarmamentBlow.h"
#include "DnPlayerActor.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnDisarmamentBlow::CDnDisarmamentBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_237;
	SetValue( szValue );
	m_fValue = 0.0f;

}

CDnDisarmamentBlow::~CDnDisarmamentBlow(void)
{

}

void CDnDisarmamentBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);

#if defined(PRE_ADD_50907)
	if (m_hActor)
		m_hActor->AddDisarmamentRefCount();

	//최초에 원래 무기를 받아 놓는다.
	if (m_hActor && m_hActor->GetDisarmamentRefCount() == 1)
	{
		DnWeaponHandle hOrigWeapon = m_hActor->GetWeapon(0);
		bool bOrigSelefDelete = m_hActor->GetWeaponSelfDelete(0);

		//ChangeWeaponBlow상태에 상관없이 현재 무기로만 되돌리면 된다..

// 		//ChangeWeapon으로 무기가 변경 되었다면 원래 무기는 ChangeWeapon에 저장된 무기로 설정되어야 한다.
// 		if (m_hActor->GetChangeWeaponRefCount() > 0)
// 		{
// 			hOrigWeapon = m_hActor->GetOrigWeaponWhenChangeWeapon();
// 			bOrigSelefDelete = m_hActor->GetOrigWeaponWhenChangeWeaponSelfDelete();
// 		}

		m_hActor->SetOrigWeaponWhenDisarmament(hOrigWeapon, bOrigSelefDelete);

		m_hActor->SetSkipOnAttatchDetachWeapon(true); //이 설정을 해놓아야 DetachWeapon에서 MASkillUser::OnDetachWeapon함수 호출이 안됨.(함수 내부에 상태효과 제거 호출이 될 수 있음)
		m_hActor->SetChangeWeaponLock(false);
		m_hActor->SetSkipChangeWeaponAction(true);
		m_hActor->DetachWeapon(0);
		m_hActor->SetSkipChangeWeaponAction(false);
		m_hActor->SetChangeWeaponLock(true);
		m_hActor->SetSkipOnAttatchDetachWeapon(false);

		if (m_hActor->IsPlayerActor())
		{
			CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
			if (pPlayerActor)
				pPlayerActor->SetBattleMode(false);
		}
	}
#endif // PRE_ADD_50907
}


void CDnDisarmamentBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);

#if defined(PRE_ADD_50907)
	if (m_hActor)
		m_hActor->RemoveDisarmamentRefCount();

	if (m_hActor && m_hActor->GetDisarmamentRefCount() == 0)
	{
		//무기 다시 착용시 동작을 하지 않으면 기본 공격이 들어 가지 않게 됨.
		//그래서 무기 해제시는 동작 안하게 하고, 원래 무기로 착용시 동작을 하도록함.
		m_hActor->SetChangeWeaponLock(false);
		//m_hActor->SetSkipChangeWeaponAction(true);
		m_hActor->SetSkipOnAttatchDetachWeapon(true);	//이 설정을 해놓아야 AttachWeapon/DetachWeapon에서 MASkillUser::OnAttachWeapon/OnDetachWeapon함수 호출이 안됨.(함수 내부에서 상태효과 제거/추가 호출이 될 수 있음)

		DnWeaponHandle hOrigWeapon = m_hActor->GetOrigWeaponWhenDisarmament();
		bool bSelfDelete = m_hActor->GetOrigWeaponWhenDisarmamentSelfDelete();

		if (hOrigWeapon)
			m_hActor->AttachWeapon(hOrigWeapon, 0, bSelfDelete);
		else
			m_hActor->DetachWeapon(0);

		//m_hActor->SetSkipChangeWeaponAction(false);
		m_hActor->SetSkipOnAttatchDetachWeapon(false);

		if (m_hActor->IsPlayerActor())
		{
			CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
			if (pPlayerActor)
			{
				if (hOrigWeapon)
					pPlayerActor->SetBattleMode(true);
				else
					pPlayerActor->SetBattleMode(false);
			}
		}
	}
#endif // PRE_ADD_50907
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDisarmamentBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnDisarmamentBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
