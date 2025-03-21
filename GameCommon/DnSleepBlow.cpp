#include "StdAfx.h"
#include "DnSleepBlow.h"
#include "DnCantMoveBlow.h"
#include "DnCantActionBlow.h"
#include "DnMonsterActor.h"
#include "DnTableDB.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

const int DEFAULT_SLEEP_PROBABILITY = 10000;		// 수면에 걸릴 디폴트 확률. 백분율*10 이 단위임
const LOCAL_TIME ACCEPT_HIT_GAP = 500;				// 슬립이 걸린 후에 0.5초 정도 맞아도 상태효과가 풀리지 않도록 해준다. 안그러면 걸리자마자 풀림

#define SLEEP_ACTION_NAME "Sleep"


// 슬립의 액션 관련 등은 액션 툴에서 처리..
CDnSleepBlow::CDnSleepBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_bAcceptHit = false;
																		  
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_045;
	SetValue( szValue );
	m_fValue = (float)atof(szValue);

	m_StartTime = 0;

	AddCallBackType( SB_ONDEFENSEATTACK );

	m_pCantMoveBlow =  new CDnCantMoveBlow( hActor, NULL );
	m_pCantActionBlow = new CDnCantActionBlow( hActor, NULL );
	
	if (m_pCantMoveBlow)
		m_pCantMoveBlow->SetPermanent(true);
	if (m_pCantActionBlow)
		m_pCantActionBlow->SetPermanent(true);

#ifndef _GAMESERVER
	m_pCantMoveBlow->UseTableDefinedGraphicEffect( false );
#endif
}

CDnSleepBlow::~CDnSleepBlow(void)
{
	SAFE_DELETE( m_pCantMoveBlow );
	SAFE_DELETE( m_pCantActionBlow );
}


#ifdef _GAMESERVER
bool CDnSleepBlow::CanBegin( void )
{
	float fWeight = 0.0f;
	if( m_hActor && m_hActor->IsPlayerActor() )
		fWeight = CPlayerWeightTable::GetInstance().GetValue( m_hActor->GetClassID(), CPlayerWeightTable::ElementDefense );

	float fAvoidProb = 1.f;
	fAvoidProb -= m_hActor->GetElementDefense( CDnState::Dark ) * fWeight;

	if( m_fValue > 0.f ) // #83276 값을 0으로 넣을때는 100%로 해달라는 요청  
		fAvoidProb -= (1.f - m_fValue);
		
	if( _rand(GetRoom()) % 10000 > int(fAvoidProb * 10000.0f) )
		return true;

	return false;
}
#endif


void CDnSleepBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hActor )
		return;

	if( strcmp( m_hActor->GetCurrentAction(), SLEEP_ACTION_NAME ) != 0 )
	{
		m_hActor->SetActionQueue( SLEEP_ACTION_NAME, 0, 0.0f, 0.0f, false );
		CEtActionBase::ActionElementStruct* pElement = m_hActor->GetElement( SLEEP_ACTION_NAME );
		if( pElement )
		{
			if( m_hActor->CDnActionBase::GetFPS() > 0.0f )
				m_StateBlow.fDurationTime += (pElement->dwLength / m_hActor->CDnActionBase::GetFPS());
		}
		else
		{
			SetState( STATE_BLOW::STATE_END );
		}
	}
	m_hActor->SetDownRemainDelta( m_StateBlow.fDurationTime );
	

#ifndef _GAMESERVER
	_AttachGraphicEffect();
#endif

	m_pCantMoveBlow->OnBegin( LocalTime, fDelta );
	m_pCantActionBlow->OnBegin( LocalTime, fDelta );

	m_bAcceptHit = false;
	m_StartTime = LocalTime;
}


void CDnSleepBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

	if( false == m_bAcceptHit )
	{
		if( LocalTime - m_StartTime > ACCEPT_HIT_GAP )
		{
			m_bAcceptHit = true;
		}
	}
}


void CDnSleepBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	m_pCantMoveBlow->OnEnd( LocalTime, fDelta );
	m_pCantActionBlow->OnEnd( LocalTime, fDelta );
	m_hActor->SetDownRemainDelta( 0.2f );

#ifndef _GAMESERVER
	_DetachGraphicEffect();
#else
	// 게임 서버에서 CDnActor::CmdRemoveStateEffect() 가 하위 클래스의 가상함수가 호출이 안되고
	// 곧바로 호출되어 계속 상태효과 끝나는 쪽으로 재귀호출 되는 상황이 발견되어 아예 분리 시킴.
	m_hActor->SendRemoveStateEffect( m_StateBlow.emBlowIndex );
#endif

}


#ifdef _GAMESERVER
bool CDnSleepBlow::OnDefenseAttack( DnActorHandle hActor, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	if( m_bAcceptHit )
	{
		// 한대 맞으면 수면 끝나고 기상!
		SetState( STATE_BLOW::STATE_END );
		// 클라한테 상태효과 끝내라고 패킷 보내줌.
		m_hActor->CmdRemoveStateEffect( m_StateBlow.emBlowIndex, false );
	}

	return false;
}
#endif


void CDnSleepBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// downdelta 를 새로 들어온 시간으로 갱신함.
	m_hActor->SetDownRemainDelta( m_StateBlow.fDurationTime );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnSleepBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];
	fValue[0] = (float)atof( szOrigValue );
	fValue[1] = (float)atof( szAddValue );

	float fResultValue = fValue[0] + fValue[1];
	sprintf_s(szBuff, "%f", fResultValue);
	szNewValue = szBuff;
}

void CDnSleepBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	float fValue[2];

	fValue[0] = (float)atof( szOrigValue );
	fValue[1] = (float)atof( szAddValue );

	float fResultValue = fValue[0] - fValue[1];
	sprintf_s(szBuff, "%f", fResultValue);
	szNewValue = szBuff;
}
#endif