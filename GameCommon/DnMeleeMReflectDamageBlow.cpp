#include "StdAfx.h"
#include "DnMeleeMReflectDamageBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// dnactor.cpp
#ifdef _GAMESERVER
extern int GetGaussianRandom( int nMin, int nMax, CMultiRoom *pRoom );
#endif

CDnMeleeMReflectDamageBlow::CDnMeleeMReflectDamageBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#ifdef _GAMESERVER
																									, m_fReflectRatio( 0.0f )
																									, m_fReflectRatioMax( 0.0f )
#endif // #ifdef _GAMESERVER
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_113;

	AddCallBackType(SB_ONDEFENSEATTACK);
	SetValue( szValue );

#ifdef _GAMESERVER
	m_fReflectRatio = (float)atof( szValue );
	m_fReflectRatioMax = 1.0f;
#endif // #ifdef _GAMESERVER
}

CDnMeleeMReflectDamageBlow::~CDnMeleeMReflectDamageBlow(void)
{

}

void CDnMeleeMReflectDamageBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnMeleeMReflectDamageBlow::OnBegin\n" );
}


void CDnMeleeMReflectDamageBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnMeleeMReflectDamageBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnMeleeMReflectDamageBlow::OnEnd\n" );
}

#ifdef _GAMESERVER
bool CDnMeleeMReflectDamageBlow::OnDefenseAttack( DnActorHandle hActor, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	// Ʈ�� �������� �ſ� �ִ� ���͸� �ݻ簡 �Ǹ� �ȵȴ�.
	if( hActor && 
		CDnActor::PropActor != hActor->GetActorType() &&
		CDnDamageBase::DistanceTypeEnum::Melee == HitParam.DistanceType &&
		1 == HitParam.cAttackType && !hActor->IsDie() )
	{
		int iMAttackMin = hActor->GetAttackMMin();
		int iMAttackMax = hActor->GetAttackMMax();

		int iReflectDamage = int(GetGaussianRandom( iMAttackMin, iMAttackMax, hActor->GetRoom() ) * m_fReflectRatio * HitParam.fDamage);

		// #29810 �̽� ����.. ��Ȯ�� ������ �� �� ���� �̻��� ���� ��� �н��ϵ��� ó��.
		if( iReflectDamage < 0 )
			return false;
		//////////////////////////////////////////////////////////////////////////

		// ���Ӽ� ���ݷ¸�ŭ ����.
#ifdef PRE_ADD_49660
		if( DVINE_PUNISHMENT_SKILL_ID == m_ParentSkillInfo.iSkillID )
		{
			float fLightElementAttack = m_hActor->GetElementAttack( CDnState::Light );
			iReflectDamage += int((float)iReflectDamage * fLightElementAttack);
		}
#endif // #ifdef PRE_ADD_49660

#ifdef PRE_FIX_REFLECT_SE_LIMIT
		// �⺻ ���ݷ��� �ִ�ġ�� ���� �ʵ���.
		CDnState* pState = m_hActor->GetStateStep( 0 );
		int iLimit = int((float)pState->GetAttackMMax() * m_fReflectRatioMax);
		if( iLimit < iReflectDamage )
			iReflectDamage = iLimit;
#endif // #ifdef PRE_FIX_REFLECT_SE_LIMIT

#if defined(PRE_FIX_61382)
		DnActorHandle hOwnerActor = CDnActor::GetOwnerActorHandle(hActor);
		if (hOwnerActor &&
			hOwnerActor->IsDie() == false)
		{
			hActor->RequestDamageFromStateBlow( GetMySmartPtr(), iReflectDamage );

			if (hOwnerActor->IsDie())
				hOwnerActor->Die(m_hActor);
		}
#else
		hActor->RequestDamageFromStateBlow( GetMySmartPtr(), iReflectDamage );

		// �׾��� üũ
		if( hActor->GetHP() <= 0.f )
			hActor->Die( m_hActor );
#endif // PRE_FIX_61382
	}
	
	return false;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnMeleeMReflectDamageBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnMeleeMReflectDamageBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW