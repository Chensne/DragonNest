#include "StdAfx.h"
#include "DnBloodSuckingBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnBloodSuckingBlow::CDnBloodSuckingBlow(DnActorHandle hActor, const char *szValue)
	: CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_227;
	SetValue(szValue);
	
	m_fValue = (float)atof(szValue);

#ifdef _GAMESERVER
	AddCallBackType( SB_ONCALCDAMAGE );
#endif

}

CDnBloodSuckingBlow::~CDnBloodSuckingBlow(void)
{
}

void CDnBloodSuckingBlow::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}

void CDnBloodSuckingBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}

void CDnBloodSuckingBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_hActor->IsDie() )
	{
		SetState( STATE_BLOW::STATE_END );
		return;
	}
}

void CDnBloodSuckingBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug( "%s\n", __FUNCTION__ );
}

#ifdef _GAMESERVER
float CDnBloodSuckingBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	//같은 팀/다른 팀 경우 구분한다.
	if (m_ParentSkillInfo.hSkillUser)
	{
		//다른 팀인 경우
		if (m_ParentSkillInfo.hSkillUser->GetTeam() != m_hActor->GetTeam() &&
			fOriginalDamage > 0.0f )
		{
			//데미지의 %만큼..
			INT64 iDelta = INT64(fOriginalDamage * m_fValue);

			//받은 데미지의 %만큼을 자신(스킬 사용자)의 HP를 회복합니다.
			INT64 iSkillUserHP = m_ParentSkillInfo.hSkillUser->GetHP();
			INT64 iSkillUserMaxHP = m_ParentSkillInfo.hSkillUser->GetMaxHP();

			//스킬 사용자 maxHP를 넘지 않도록 보정 한다.
			INT64 iResult = iSkillUserHP + iDelta;
			if( iSkillUserMaxHP < iResult )
			{
				iResult = iSkillUserMaxHP;
				iDelta = iSkillUserMaxHP - iSkillUserHP;
			}
			
			m_ParentSkillInfo.hSkillUser->SetHP(iResult);
			m_ParentSkillInfo.hSkillUser->RequestHPMPDelta( CDnState::ElementEnum_Amount, iDelta, m_ParentSkillInfo.hSkillUser->GetUniqueID() );

			OutputDebug( "%s 스킬 사용자 HP증가 ActorID: %d : %d + %d = %d\n", __FUNCTION__, m_ParentSkillInfo.hSkillUser->GetUniqueID(), iSkillUserHP, iDelta, iResult );
		}
	}

	return 0.0f;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnBloodSuckingBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];
	
	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];
	
	sprintf_s(szBuff, "%f", fResultValue);
	
	szNewValue = szBuff;
}

void CDnBloodSuckingBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//값 계산
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW