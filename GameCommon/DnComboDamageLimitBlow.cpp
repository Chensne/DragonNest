#include "StdAfx.h"
#include "DnComboDamageLimitBlow.h"
#include "DnProjectile.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

/*
	이 상태 효과는 해당 스킬 액션의 최초 Hit시그널에서 추가를 하고
	그 이후의 hit시그널에서는 szSkipStateBlows에 이 상태효과 스킵을 설정해서 다시 추가 되지 않도록 해야 한다..
*/
CDnComboDamageLimitBlow::CDnComboDamageLimitBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_242;
	SetValue( szValue );
	m_fValue = 0.0f;

	std::string str = szValue;//"제한 Hit수;데미지 제한 수치(0.1 => 10%)";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);
	if (2 == tokens.size())
	{
		m_nHitLimitCount = atoi( tokens[0].c_str() );
		m_fDamageRate = (float)atof(tokens[1].c_str());

		//설정값 오류? 보정..
		if (m_nHitLimitCount == 0)
		{
			m_nHitLimitCount = -1;
			m_fDamageRate = -1.0f;
		}
	}
	else
	{
		//상태효과 설정이 잘 못 된 경우 적용 되지 않도록 한다.
		m_nHitLimitCount = -1;
		m_fDamageRate = -1.0f;

		OutputDebug("%s Invalid Value!!!!\n", __FUNCTION__);
	}

	m_bActivated = false;

	m_SkillStartTime = 0;

#if defined(_GAMESERVER)
	AddCallBackType(SB_ONCALCDAMAGE);
#endif // _GAMESERVER
}

CDnComboDamageLimitBlow::~CDnComboDamageLimitBlow(void)
{
}

void CDnComboDamageLimitBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}


void CDnComboDamageLimitBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s 상태효과 시작 시간 %d\n", __FUNCTION__, m_SkillStartTime);
}

void CDnComboDamageLimitBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

#if defined(_GAMESERVER)
	if (m_ParentSkillInfo.hSkillUser)
	{
		//스킬 사용이 끝나고, 스킬에서 사용된 발사체가 전부 없어 졌으면 상태효과 종료 한다.
		DnSkillHandle hSkill = m_ParentSkillInfo.hSkillUser->FindSkill(m_ParentSkillInfo.iSkillID);
		if (hSkill && hSkill->IsFinished() && hSkill->GetProjectileCount(m_SkillStartTime) == 0)
		{
#ifdef PRE_FIX_REMOVE_STATE_EFFECT_PACKET
#else
			m_hActor->SendRemoveStateEffectFromID(GetBlowID());
#endif
			m_StateBlow.fDurationTime = 0.0f;
			SetState(STATE_BLOW::STATE_END);
		}
	}
#endif // _GAMESERVER
}

#if defined(_GAMESERVER)
float CDnComboDamageLimitBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	float fDamage = 0.0f;

	//발사체에 의한 히트 인지 확인...
	LOCAL_TIME projectileSkillStartTime = 0;
	CDnProjectile* pProjectile = NULL;
	if (HitParam.bFromProjectile && HitParam.hWeapon)
		pProjectile = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() );

	//발사체의 스킬 시작 시간과 상태효과의 스킬 시작 시간이 같아야 한다..
	if (pProjectile)
		projectileSkillStartTime = pProjectile->GetSkillStartTime();

	//스킬 사용자와 지금 Hitter가 같은지 확인한다..
	bool bSameHitter = false;
	if (m_ParentSkillInfo.hSkillUser && HitParam.hHitter &&
		m_ParentSkillInfo.hSkillUser == HitParam.hHitter &&
		m_SkillStartTime == projectileSkillStartTime)
		bSameHitter = true;

	// m_bActivated가 활성화 되어 있고, 같은 Hitter인 경우에 데미지 보정 계산
	if (m_bActivated == true && bSameHitter)
	{
		//원래 데미지 100, 데미지 제한이 0.3(30%)인경우
		//100 - (1.0 - 0.3) = 70 
		//-70만큼 데미지 보정을 해줘야 데미지가 30만큼만 적용됨.
		fDamage = -(fOriginalDamage * (1.0f - m_fDamageRate));

		//설정값이 잘못 된경우 적용 되지 않도록 함.
		if (m_nHitLimitCount == -1 && m_fDamageRate == -1.0f)
			fDamage = 0.0f;
	}

	return fDamage;
}

void CDnComboDamageLimitBlow::UpdateHitCount(CDnSkill::SkillInfo* pSkillInfo, CDnDamageBase::SHitParam& HitParam)
{
	if (pSkillInfo)
	{
		//발사체에 의한 히트 인지 확인...
		LOCAL_TIME projectileSkillStartTime = 0;
		CDnProjectile* pProjectile = NULL;
		if (HitParam.bFromProjectile && HitParam.hWeapon)
			pProjectile = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() );
		
		//발사체의 스킬 시작 시간과 상태효과의 스킬 시작 시간이 같아야 한다..
		if (pProjectile)
			projectileSkillStartTime = pProjectile->GetSkillStartTime();

		//스킬 사용자가 같고, 스킬이 같은 경우만 HitCount감소
		if (pSkillInfo->hSkillUser && pSkillInfo->hSkillUser == m_ParentSkillInfo.hSkillUser &&
			pSkillInfo->iSkillID == m_ParentSkillInfo.iSkillID &&
			pSkillInfo->iLevel == m_ParentSkillInfo.iLevel &&
			m_SkillStartTime == projectileSkillStartTime)
		{
			if (m_bActivated == false && m_nHitLimitCount >= 0)
			{
				m_nHitLimitCount--;

				if (m_nHitLimitCount <= 0)
				{
					m_bActivated = true;
					m_nHitLimitCount = 0;
				}
			}
		}
	}
}

#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnComboDamageLimitBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnComboDamageLimitBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
