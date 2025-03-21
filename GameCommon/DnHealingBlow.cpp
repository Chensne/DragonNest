#include "StdAfx.h"
#include "DnHealingBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// dnactor.cpp
#ifdef _GAMESERVER
extern int GetGaussianRandom( int nMin, int nMax, CMultiRoom *pRoom );
#endif

CDnHealingBlow::CDnHealingBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_140;

	// 이 상태효과는 따로 값을 쓰는 것은 없다..
	SetValue( szValue );
	m_fValue = (float)atof( szValue );
}

CDnHealingBlow::~CDnHealingBlow(void)
{

}

void CDnHealingBlow::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}

void CDnHealingBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

#ifdef _GAMESERVER
	// 걍 한방에 다 처리
	INT64 iHP = m_hActor->GetHP();
	INT64 iMaxHP = m_hActor->GetMaxHP();
	
	// 사용자(스킬 시전자)의 <마법 공격력>*<효과 비율>만큼 대상의 HP를 회복시켜준다.
	if( m_ParentSkillInfo.hSkillUser )
	{
		int iAttackMMin = m_ParentSkillInfo.hSkillUser->GetAttackMMin();
		int iAttackMMax = m_ParentSkillInfo.hSkillUser->GetAttackMMax();
		
		float fMAttack = (float)GetGaussianRandom( iAttackMMin, iAttackMMax, m_hActor->GetRoom() );


		INT64 iDelta = INT64(fMAttack * m_fValue);
		
		if(iDelta < 0 )
			return;  // 힐링블로우가 피가 깍이는 경우는 없다. 값이 꼬이거나 쓰레기값이 들어오는 경우가 있을지모르니 리턴해주자.

		INT64 iResult = iHP + iDelta;

		if( iMaxHP < iResult )
		{
			iResult = iMaxHP;
			iDelta = iMaxHP - iHP;
		}
		else if( iResult < 0 )
		{
			iResult = 1;
			iDelta = 1 - iHP;
		}

		m_hActor->SetHP( iResult );
		m_hActor->RequestHPMPDelta( CDnState::ElementEnum_Amount, iDelta, m_hActor->GetUniqueID() );

		OutputDebug( "[CDnHealingBlow::OnBegin] ActorID: %d : %d + %d = %d\n", m_hActor->GetUniqueID(), iHP, iDelta, iResult );
	}

	// 처리됐으므로 곧바로 end 시킴.
	SetState( STATE_BLOW::STATE_END );

#else
		OutputDebug( "CDnHealingBlow::OnBegin Value:%2.2f\n", m_fValue );
#endif
}


void CDnHealingBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnHealingBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

	OutputDebug( "CDnHealingBlow::OnEnd Value:%2.2f\n", m_fValue );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnHealingBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnHealingBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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