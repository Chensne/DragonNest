#include "StdAfx.h"
#include "DnRangePReflectDamageBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef _GAMESERVER
extern int GetGaussianRandom( int nMin, int nMax, CMultiRoom *pRoom );
#endif

CDnRangePReflectDamageBlow::CDnRangePReflectDamageBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_112;

	AddCallBackType( SB_ONDEFENSEATTACK );
	SetValue( szValue );

#ifdef _GAMESERVER
	m_fReflectRatio = (float)atof( szValue );
	m_fReflectRatioMax = 1.0f;
#endif // #ifdef _GAMESERVER
}

CDnRangePReflectDamageBlow::~CDnRangePReflectDamageBlow(void)
{

}

void CDnRangePReflectDamageBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnRangePReflectDamageBlow::OnBegin\n" );
}


void CDnRangePReflectDamageBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnRangePReflectDamageBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnRangePReflectDamageBlow::OnEnd\n" );
}

#ifdef _GAMESERVER
bool CDnRangePReflectDamageBlow::OnDefenseAttack( DnActorHandle hActor, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	if( hActor && pAttackerState &&
		CDnActor::PropActor != hActor->GetActorType() &&
		CDnDamageBase::DistanceTypeEnum::Range == HitParam.DistanceType &&
		0 == HitParam.cAttackType && !hActor->IsDie() )
	{
		// 원거리는 발사체에 스킬 쓸 당시의 캐릭터의 능력을 담고 있는 AttackerState 를 뽑아서 이 함수의 인자로 넣어줌.
		//int iMAttackMin = hActor->GetAttackMMin();
		//int iMAttackMax = hActor->GetAttackMMax();
		int iPAttackMin = pAttackerState->GetAttackPMin();
		int iPAttackMax = pAttackerState->GetAttackPMax();

		int iReflectDamage = int(GetGaussianRandom( iPAttackMin, iPAttackMax, hActor->GetRoom() ) * m_fReflectRatio * HitParam.fDamage);
		
		// #29810 이슈 관련.. 정확한 원인을 알 수 없어 이상한 값인 경우 패스하도록 처리.
		if( iReflectDamage < 0 )
			return false;
		//////////////////////////////////////////////////////////////////////////

		// 빛속성 공격력만큼 증가.
#ifdef PRE_ADD_49660
		if( DVINE_PUNISHMENT_SKILL_ID == m_ParentSkillInfo.iSkillID )
		{
			float fLightElementAttack = m_hActor->GetElementAttack( CDnState::Light );
			iReflectDamage += int((float)iReflectDamage * fLightElementAttack);
		}
#endif // #ifdef PRE_ADD_49660

#ifdef PRE_FIX_REFLECT_SE_LIMIT
		// 기본 공격력의 최대치를 넘지 않도록.
		CDnState* pState = m_hActor->GetStateStep( 0 );
		int iLimit = int((float)pState->GetAttackPMax() * m_fReflectRatioMax);
		if( iLimit < iReflectDamage )
			iReflectDamage = iLimit;
#endif // #ifdef PRE_FIX_REFLECT_SE_LIMIT
		
#if defined(PRE_FIX_61382)
		DnActorHandle hOwnerActor = CDnActor::GetOwnerActorHandle(hActor);
		if (hOwnerActor &&
			hOwnerActor->IsDie() == false)
		{
			hActor->RequestDamageFromStateBlow(GetMySmartPtr(), iReflectDamage);

			if (hOwnerActor->IsDie())
				hOwnerActor->Die(m_hActor);
		}
#else
		hActor->RequestDamageFromStateBlow( GetMySmartPtr(), iReflectDamage );

		// 죽었나 체크
		if( hActor->GetHP() <= 0.f )
			hActor->Die( m_hActor );
#endif // PRE_FIX_61382

	}

	return false;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnRangePReflectDamageBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnRangePReflectDamageBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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