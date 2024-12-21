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
	// Ŭ���̾�Ʈ�� ��쿡�� ���İ� ����
	m_hActor->SetAlphaBlend( 0.0f, AL_STATEEFFECT );
	m_hActor->GetAniObjectHandle()->EnableShadowCast( false );
	GetInterface().HideEnemyGauge( m_hActor );
	m_fDestAlpha = 0.0f;
	m_fNowAlpha = 1.0f;
	m_bEnded = false;


	//�÷��̾� ĳ���� �ΰ�� ���⿡�� �߻�ü�� �߻�ɶ� ���� ��ġ�� ���ſ� ������ ����.
	//�׷��� ĳ���Ͱ� ������ ������ ���� �ʵ��� ���� ���İ��� 0.0�� �ƴ� ������ �����ؼ�
	//������ġ ������Ʈ�� ���������� ó�� �ǵ����Ѵ�..
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
		// ����/�ǰ�/���� ���϶��� ������ Ǯ��. idle �� ��쿣 ������ ����
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
		// delta ���� ������ �״�� ���ĸ� �����ϹǷ� �ð� ������ �д�.		
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
	// ������ ���� ��� ������ ���� ���� 50%��

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

void CDnInvisibleBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
