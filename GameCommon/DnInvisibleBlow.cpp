#include "StdAfx.h"
#include "DnInvisibleBlow.h"
#ifndef _GAMESERVER
#include "DnInterface.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInvisibleBlow::CDnInvisibleBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_073;
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

#if defined(_GAMESERVER)
#else
	m_fDestAlpha = 0.0f;
	m_fNowAlpha = 1.0f;
	m_bEnded = false;
#endif // _GAMESERVER
}

CDnInvisibleBlow::~CDnInvisibleBlow(void)
{

}


void CDnInvisibleBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	// 클라이언트인 경우에만 알파값 빼줌
	m_hActor->SetAlphaBlend( 0.0f, AL_STATEEFFECT );
	m_hActor->GetAniObjectHandle()->EnableShadowCast( false );
	GetInterface().HideEnemyGauge( m_hActor );
	m_fDestAlpha = 0.0f;
	m_fNowAlpha = 1.0f;
	m_bEnded = false;


	//플레이어 캐릭터 인경우 무기에서 발사체가 발사될때 무기 위치값 갱신에 문제가 있음.
	//그래서 캐릭터가 완전히 투명이 되지 않도록 최종 알파값을 0.0이 아닌 값으로 설정해서
	//무기위치 업데이트가 정상적으로 처리 되도록한다..
	if (m_hActor && m_hActor->IsPlayerActor())
		m_fDestAlpha = 0.0001f;
#endif

	OutputDebug( "CDnInvisibleBlow::OnBegin \n");
}


void CDnInvisibleBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifndef _GAMESERVER	
	if( false == m_bEnded )
	{
		// 공격/피격/경직 중일때는 투명이 풀림. idle 인 경우엔 투명한 상태
		const char* pCurrentActionName = m_hActor->GetCurrentAction();
		if( (/*m_hActor->IsMove() ||*/  m_hActor->IsAttack() || m_hActor->IsProcessSkill() ||
			m_hActor->IsStiff() || m_hActor->IsHit() /*|| m_hActor->IsAir()*/) && 
			0 != strcmp(pCurrentActionName, "Idle") )
		{
			m_fDestAlpha = 0.5f;

			//m_hActor->SetAlphaBlend( 0.5f );
		}
		else
		{
			m_fDestAlpha = 0.0f;

			if (m_hActor && m_hActor->IsPlayerActor())
				m_fDestAlpha = 0.0001f;

			//m_hActor->SetAlphaBlend( 0.0f );
		}
	}

	if( m_fNowAlpha < m_fDestAlpha )
	{
		m_fNowAlpha += fDelta;
	}
	else
	if( m_fNowAlpha > m_fDestAlpha )
	{
		m_fNowAlpha -= fDelta;
	}

	if( false == m_bEnded )
	{
		// delta 값을 가지고 그대로 알파를 조절하므로 시간 여유를 둔다.		
		if( (0.0f < GetDurationTime()) && (GetDurationTime() < (1.0f-m_fNowAlpha)) )
		{
			m_fDestAlpha = 1.0f;

			if( false == m_bEnded )
				SetDurationTime( 1.0f-m_fNowAlpha );

			m_bEnded = true;
		}
	}

	if( m_fNowAlpha != m_fDestAlpha )
	{
		float fGap = m_fNowAlpha - m_fDestAlpha;
		if( fGap < 0.05f && fGap > -0.05f )
			m_fNowAlpha = m_fDestAlpha;

		//OutputDebug( "[Invisible] %2.2f\n", m_fNowAlpha );
	}

	m_hActor->SetAlphaBlend( m_fNowAlpha , AL_STATEEFFECT );

#endif
}


bool CDnInvisibleBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	// 맞으면 경직 모션 나오는 동안 알파 50%임

	return false;
}


void CDnInvisibleBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	m_hActor->SetAlphaBlend( 1.0f , AL_STATEEFFECT );
	m_hActor->GetAniObjectHandle()->EnableShadowCast( true );
	GetInterface().ShowEnemyGauge( m_hActor, false );
#endif

	OutputDebug( "CDnInvisibleBlow::OnEnd \n");
}
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnInvisibleBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnInvisibleBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
