#include "StdAfx.h"
#include "DnRecoverHPByAttackBlow.h"

#if defined(_GAMESERVER)
#include "DnMonsterActor.h"
#endif // _GAMESERVER

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnRecoverHPByAttackBlow::CDnRecoverHPByAttackBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_248;

	SetValue( szValue );
	
	m_fValue = 0.0f;
	m_nLimitHP = -1;

	std::string str = szValue;//"회복량;회복량제한값";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 구분
	TokenizeA(str, tokens, delimiters);
	int nTokenSize = (int)tokens.size();
	
	if (nTokenSize >= 1)
	{
		m_fValue = (float)(atof(tokens[0].c_str()));
		if (nTokenSize >= 2)
			m_nLimitHP = atoi(tokens[1].c_str());
	}
	else
	{
		OutputDebug("%s Invalid Value %s\n", __FUNCTION__, szValue);
	}

#if defined(_GAMESERVER)
	AddCallBackType(SB_ONTARGETHIT);
#endif // _GAMESERVER

}

CDnRecoverHPByAttackBlow::~CDnRecoverHPByAttackBlow(void)
{

}

void CDnRecoverHPByAttackBlow::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}

void CDnRecoverHPByAttackBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug( "%s\n", __FUNCTION__ );
}


void CDnRecoverHPByAttackBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

}


void CDnRecoverHPByAttackBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if defined(_GAMESERVER)
	//같은 스킬에 의한 상태효과 제거...
	//#60311 상태효과 제거시 같은 스킬 상태효과 제거 기능 막음.
	//RemoveStateBlowBySkillID(m_ParentSkillInfo.iSkillID);
#endif // _GAMESERVER
	OutputDebug( "%s\n", __FUNCTION__);
}

#if defined(_GAMESERVER)
void CDnRecoverHPByAttackBlow::RemoveStateBlowBySkillID(int nSkillID)
{
	DNVector(DnBlowHandle) vlBlowList;

	FindSkillBlowList(m_hActor, nSkillID, vlBlowList);

	int nBlowCount = (int)vlBlowList.size();
	for (int n = 0; n < nBlowCount; ++n)
	{
		DnBlowHandle hBlow = vlBlowList[n];
		if (!hBlow)
			continue;

		// 스킬 유저 액터 확인은 안 하는걸로..(프랍?에 의해서 추가된 상태효과는 스킬 유저 액터가 없음.)
		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
		if (!pSkillInfo /*|| !pSkillInfo->hSkillUser*/)
			continue;

		//자신에게 적용할 상태효과를 예약한다..
		m_hActor->CmdRemoveStateEffectFromID(hBlow->GetBlowID());
	}

}

void CDnRecoverHPByAttackBlow::FindSkillBlowList(DnActorHandle hActor, int nSkillID, DNVector(DnBlowHandle)& vlBlowList)
{
	if (!hActor)
		return;

	int nStateBlowCount = hActor->GetNumAppliedStateBlow();

	for (int i = 0; i < nStateBlowCount; ++i)
	{
		DnBlowHandle hBlow = hActor->GetAppliedStateBlow(i);
		if (!hBlow)
			continue;

		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
		//스킬 정보가 없으면 건너뜀. (스킬 유저가 없을 수 있음..(프랍?에 의해 적용된 상태효과)
		if (!pSkillInfo /*|| !pSkillInfo->hSkillUser*/)
			continue;

		if (pSkillInfo->iSkillID == nSkillID)
			vlBlowList.push_back(hBlow);
	}
}


void CDnRecoverHPByAttackBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	CDnDamageBase::SHitParam *pHitParam = NULL;
#if defined(PRE_FIX_59680)
	if (hTargetActor)
	{
		//소환 몬스터 액터이고, 주인 액터가 있고, TransmitDamageBlow(51)상태효과가 있는 경우 주인 액터의 HitParam을 가져 오도록 한다.
		//RequestDamage
		if (hTargetActor->IsMonsterActor())
		{
			CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(hTargetActor.GetPointer());
			if (pMonsterActor && pMonsterActor->IsSummonedMonster() && pMonsterActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_051))
			{
				hTargetActor = pMonsterActor->GetSummonerPlayerActor();
			}
		}
		
		if (hTargetActor)
			pHitParam = hTargetActor->GetHitParam();
	}
#else
	if (hTargetActor)
		pHitParam = hTargetActor->GetHitParam();
#endif // PRE_FIX_59680

	INT64 nRecoverHP = 0;
	if (pHitParam)
		nRecoverHP = (INT64)(m_fValue * pHitParam->nCalcDamage);
	
	if (m_nLimitHP != -1)
	{
		//제한값이 정상일때만 HP회복 하도록 한다.
		if (m_nLimitHP <= 0)
		{
			OutputDebug("%s HP 회복 제한...  [%d] - [%d]\n", __FUNCTION__, m_nLimitHP, nRecoverHP);
			return;
		}

		//남은 제한 HP회복량 보다 현재 회복량이 작은값인 경우는 남은 제한량을 현재 회복량만큼 감소 시켜 놓고
		if (m_nLimitHP >= nRecoverHP)
			m_nLimitHP -= nRecoverHP;
		//남은 제한 HP회복량이 현재 회복량 보다 작은값인 경우는 남은 제한량은 0으로, 회복량은 현재 남은 제한 회복량으로 설정.
		else
		{
			nRecoverHP = m_nLimitHP;
			m_nLimitHP = 0;

			//#60311 제한 수치만큼 HP회복 된 이후 HP회복은 되지 않고, 상태효과는 계속 유지 됨.
			//남은 회복량이 없으면 상태효과 제거 됨.. 상태효과 제거될때 같은 스킬의 상태효과들 같이 제거..
			//SetState(STATE_BLOW::STATE_END);
		}
	}

	if (nRecoverHP != 0)
	{
		//MaxHP를 넘지 않게..
		INT64 nMaxHP = m_hActor->GetMaxHP();
		INT64 nCurHP = m_hActor->GetHP();
		INT64 nTempHP = nCurHP + nRecoverHP;

		//MaxHP를 넘어 가는 만큼 nRecoverHP를 보정한다.
		if (nTempHP > nMaxHP)
			nRecoverHP -= (nTempHP - nMaxHP);

		if (nRecoverHP > 0)
		{
			m_hActor->SetHP( m_hActor->GetHP() + nRecoverHP );
			m_hActor->RequestHPMPDelta( CDnState::ElementEnum::ElementEnum_Amount, nRecoverHP, UINT_MAX, false, true );
		}
	}
}
#endif // _GAMESERVER


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnRecoverHPByAttackBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnRecoverHPByAttackBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW