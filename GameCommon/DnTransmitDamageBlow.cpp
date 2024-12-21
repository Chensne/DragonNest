#include "StdAfx.h"
#include "DnTransmitDamageBlow.h"

#if defined(_GAMESERVER)
#include "DnMonsterActor.h"
#endif // _GAMESERVER

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnTransmitDamageBlow::CDnTransmitDamageBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_051;

	SetValue( szValue );
	m_fValue = 0.0f;

#if defined(PRE_FIX_61382)
#else
#if defined(_GAMESERVER)
	AddCallBackType( SB_ONCALCDAMAGE );
#endif // _GAMESERVER
#endif // PRE_FIX_61382
}

CDnTransmitDamageBlow::~CDnTransmitDamageBlow(void)
{

}

void CDnTransmitDamageBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug( "%s\n", __FUNCTION__ );
}


void CDnTransmitDamageBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

}

void CDnTransmitDamageBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug( "%s\n", __FUNCTION__);
}

#if defined(_GAMESERVER)
#if defined(PRE_FIX_61382)
void CDnTransmitDamageBlow::TransmitDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	if (m_hActor->IsMonsterActor())
	{
		DnActorHandle hSummonerActor;

		CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(m_hActor.GetPointer());
		if (pMonsterActor)
			hSummonerActor = pMonsterActor->GetSummonerPlayerActor();

		if (hSummonerActor && hSummonerActor->IsDie() == false)
		{

#if defined(PRE_FIX_59347) && !defined(PRE_FIX_67656)
			hSummonerActor->SetApplyPartsDamage(true);
#endif // PRE_FIX_59347

			//일단 hSkillUser를 HitParam의 Hitter로 변경해서 호출 한다.
			//몬스터 생성시 패시브로 적용 된 상태효과라 스킬정보가 올바르지 않다.
			//실제 타격자를 이시점에 변경 해서 적용 해야 실제 타격자의 타격 처리가 될듯..
			m_ParentSkillInfo.hSkillUser = HitParam.hHitter;
			hSummonerActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)fOriginalDamage, &HitParam);

#if defined(PRE_FIX_59347) && !defined(PRE_FIX_67656)
			hSummonerActor->SetApplyPartsDamage(false);
#endif // PRE_FIX_59347

			if (hSummonerActor->IsDie())
				hSummonerActor->Die(HitParam.hHitter);
		}
	}
}

#else
float CDnTransmitDamageBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	if (m_hActor->IsMonsterActor())
	{
		DnActorHandle hSummonerActor;

		CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(m_hActor.GetPointer());
		if (pMonsterActor)
			hSummonerActor = pMonsterActor->GetSummonerPlayerActor();

		if (hSummonerActor && hSummonerActor->IsDie() == false)
		{

#if defined(PRE_FIX_59347) && !defined(PRE_FIX_67656)
			hSummonerActor->SetApplyPartsDamage(true);
#endif // PRE_FIX_59347

			//일단 hSkillUser를 HitParam의 Hitter로 변경해서 호출 한다.
			//몬스터 생성시 패시브로 적용 된 상태효과라 스킬정보가 올바르지 않다.
			//실제 타격자를 이시점에 변경 해서 적용 해야 실제 타격자의 타격 처리가 될듯..
			m_ParentSkillInfo.hSkillUser = HitParam.hHitter;
			hSummonerActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)fOriginalDamage, &HitParam);

#if defined(PRE_FIX_59347) && !defined(PRE_FIX_67656)
			hSummonerActor->SetApplyPartsDamage(false);
#endif // PRE_FIX_59347

			if (hSummonerActor->IsDie())
				hSummonerActor->Die(HitParam.hHitter);
		}
	}
	return -fOriginalDamage;
}
#endif // PRE_FIX_61382
#endif // _GAMESERVER


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnTransmitDamageBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnTransmitDamageBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW