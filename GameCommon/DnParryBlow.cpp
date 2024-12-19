#include "StdAfx.h"
#include "DnParryBlow.h"
#ifdef _GAMESERVER
#include "DnStateBlow.h"
#include "DnPlayerActor.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnParryBlow::CDnParryBlow(DnActorHandle hActor, const char *szValue)
	: CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_031;
	AddCallBackType(SB_ONDEFENSEATTACK);
	SetValue(szValue);
	//m_nValue = atoi( szValue );
	m_fValue = (float)atof(szValue);

#ifdef _GAMESERVER
	m_strParringActionName.assign( "Skill_Parrying" );
	m_bEnable = false;
	m_bEnableLastLoop = false;
	m_fAdditionalSignalProb = 0.0f;
	m_fAdditionalSignalProbLastLoop = 0.0f;
#endif
}

CDnParryBlow::~CDnParryBlow(void)
{
}

void CDnParryBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	// 액터 process 거의 끝부분에 상태효과 process 를 돌기 때문에 여기서 리셋해주면 패링시그널의 구간과 동일하게 enable 이 처리될 것.
	m_bEnableLastLoop = m_bEnable;
	m_fAdditionalSignalProbLastLoop = m_fAdditionalSignalProb;
	m_bEnable = false;
	m_fAdditionalSignalProb = 0.0f;
#endif
}

void CDnParryBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnParryBlow::OnBegin\n" );
}

void CDnParryBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnParryBlow::OnEnd\n" );
}

#ifdef _GAMESERVER
bool CDnParryBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	// #25797 히트 시그널에 bIgnoreParring 이 켜져 있으면 발동하지 않는다.
	if( HitParam.bIgnoreParring )
		return false;

	if( !m_bEnableLastLoop )
		return false;

	if( !bHitSuccess ) 
		return false;

	/*
	// hit percent 가 0% 일 경우 발동하지 않는 것으로 됨. (#21175)
	if( 0.0f == HitParam.fDamage )
	{
		// #29204 fDamage가 0일때 패링 풀리는 현상 막음.
		// 같은 팀일 경우만 방어 실패 [2011/02/24 semozz]
		// 다른 팀일 경우는 원래 루틴 처리...
		if (hHitter && m_hActor &&
			(hHitter->GetTeam() == m_hActor->GetTeam()))
		{
			return false;
		}
	}
	*/

#if defined(PRE_FIX_BLOCK_CONDITION)
	//블럭의 발동이 fDamage가 0이라도 적이라면 블럭 발동 가능 하도록..
	//아군인 경우 기존 처럼 fDamage 가0이면 블럭 되지 않도록 함.
	if (IsCanBlock(hHitter, m_hActor, HitParam) == false)
		return false;
#else
	// hit percent 가 0% 일 경우 발동하지 않는 것으로 됨. (#21175)
	if( 0.0f == HitParam.fDamage )
		return false;
#endif // PRE_FIX_BLOCK_CONDITION

#if defined(PRE_ADD_49166)
	// 피격자가 디버프 (Freezing / FrameStop) 상태효과가 적용되어 있는 경우는 블럭 발동 되지 않도록..
	if (IsInVaildBlockCondition(m_hActor) == true)
		return false;
#endif // PRE_ADD_49166

	// 플레이어인 경우엔,
	if( m_hActor->IsPlayerActor() )
	{
		// 대포모드일때는 발동하지 않음. 
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor->IsCannonMode() )
			return false;

		// 무기를 들고 있지 않은 경우엔 발동하지 않음. #26772
		if( false == (pPlayerActor->IsBattleMode() && pPlayerActor->GetWeapon( 0 ) && pPlayerActor->GetWeapon( 1 )) )
			return false;
		if( pPlayerActor->IsTransformMode() )
			return false;
	}
	
	float fAddProb = m_fAdditionalSignalProbLastLoop;
	if( m_hActor->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_056 ) )
	{
		DNVector(DnBlowHandle) vlhParryingProbBlow;
		m_hActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_056, vlhParryingProbBlow );
		int iNumBlow = (int)vlhParryingProbBlow.size();
		for( int i = 0; i < iNumBlow; ++i )
			fAddProb += vlhParryingProbBlow.at( i )->GetFloatValue();
	}

	int iProb = int((m_fValue+fAddProb) * 10000.0f);
	int iRandValue = (_rand(GetRoom())%10000);
	if( iRandValue <= iProb )
	{
		// HitParam에 적절한 값을 셋팅한다.
		HitParam.szActionName = m_strParringActionName;
		OutputDebug( "CDnParryingBlow::OnDefenseAttack\n" );

		// 걸어놓은 애들한테 알려줌. /////////////////////////////////////////////
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_PARRING_SUCCESS ) );
		pEvent->SetSkillID( m_ParentSkillInfo.iSkillID );
		Notify( pEvent );
		//////////////////////////////////////////////////////////////////////////

		return true;
	}
	else
	{
		return false;
	}
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnParryBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnParryBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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