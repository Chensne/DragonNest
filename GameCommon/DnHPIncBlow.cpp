#include "StdAfx.h"
#include "DnHPIncBlow.h"
#include "DnStateBlow.h"

#if defined(PRE_FIX_66687)
#include "DnPartsMonsterActor.h"
#endif // PRE_FIX_66687

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnHPIncBlow::CDnHPIncBlow(DnActorHandle hActor, const char *szValue) : CDnBlow(hActor)
{
	m_fElapsedTime = 0.f;
	m_bFromRebirth = false;
	m_bKillingBlow = false;

#if defined(PRE_FIX_66687)
	m_bPartsHP = false;
#endif // PRE_FIX_66687

	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_016;
	SetValue(szValue);
	
	std::string str = szValue;
	std::vector<std::string> tokens;
	TokenizeA(str, tokens, ";" );

	if( tokens.size() > 0 )
		m_fValue = (float)atof( tokens[0].c_str() );

	if( tokens.size() == 2 )
		m_bKillingBlow = (int)atoi( tokens[1].c_str() ) == 1 ? true : false;

#if defined(PRE_FIX_66687)
	if (tokens.size() == 3)
		m_bPartsHP = (int)atoi( tokens[2].c_str() ) == 1 ? true : false;
#endif // PRE_FIX_66687

	if( m_fValue == -1.0f )
		m_bKillingBlow = true;
}

CDnHPIncBlow::~CDnHPIncBlow(void)
{
}

void CDnHPIncBlow::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}

void CDnHPIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}

void CDnHPIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 부활 시에 추가된 게 아니고, 죽은 상태라면 패스!
	if( false == m_bFromRebirth && m_hActor->IsDie() )
	{
		SetState( STATE_BLOW::STATE_END );
		return;
	}

	// 재호씨 요청으로 일단 hp 변화값을 서버에서 쏴주는 것으로 처리.
#ifdef _GAMESERVER
	bool isPartsMonsterHP = false;
	
#if defined(PRE_FIX_66687)
	isPartsMonsterHP = (m_hActor->IsPartsMonsterActor() && m_bPartsHP);
#endif // PRE_FIX_66687

	if( GetParentSkillInfo() && GetParentSkillInfo()->bIsItemSkill == true && m_hActor && m_hActor->GetStateBlow() )
	{
		if( m_fValue > 0 )
		{
			float fResultValue = 1.f;
			DNVector(DnBlowHandle) vBlows;
			m_hActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_272, vBlows );
			for( int i = 0; i < (int)vBlows.size(); ++i )
			{
				fResultValue += vBlows[i]->GetFloatValue();
			}
			m_fValue *= fResultValue;
		}
	}
	
	if (isPartsMonsterHP == false)
	{
		INT64 iHP = m_hActor->GetHP();
		INT64 iMaxHP = m_hActor->GetMaxHP();

		INT64 iDelta = INT64((float)iMaxHP * m_fValue);
		if( iDelta < 0 ) iDelta += -1; // 반올림으로 음수영역이 줄어드는 부분 보간.
		INT64 iResult = iHP + iDelta;

		if( iMaxHP < iResult )
		{
			iResult = iMaxHP;
			iDelta = iMaxHP - iHP;
		}
		else if( iResult <= 0 )
		{
			if( m_bKillingBlow )
			{
				m_hActor->RequestKillAfterProcessStateBlow(m_ParentSkillInfo.hSkillUser);
			}
			else
			{
				iResult = 1;
				iDelta = 1 - iHP;
			}
		}

		m_hActor->SetHP( iResult );
		m_hActor->RequestHPMPDelta( CDnState::ElementEnum_Amount, iDelta, m_hActor->GetUniqueID() );
	}
#if defined(PRE_FIX_66687)
	else
	{
		CDnPartsMonsterActor* pPartsMonsterActor = static_cast<CDnPartsMonsterActor*>(m_hActor.GetPointer());
		if (pPartsMonsterActor)
		{
			pPartsMonsterActor->ApplyPartsHP(m_fValue);
		}
	}
#endif // PRE_FIX_66687

	SetState( STATE_BLOW::STATE_END ); // 처리됐으므로 곧바로 end 시킴.
#endif

}

void CDnHPIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnHPIncBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnHPIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnHPIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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