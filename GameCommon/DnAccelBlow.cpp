#include "StdAfx.h"
#include "DnAccelBlow.h"
#include "DnBasicBlow.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CDnAccelBlow::CDnAccelBlow(DnActorHandle hActor, const char *szValue) : CDnBlow(hActor)
,m_IntervalChecker( hActor, GetMySmartPtr() )
,m_pMoveSpeedBlow(NULL)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_220;
	SetValue(szValue);
	
	m_fIncValue = 0.0f;

	SetInfo(szValue);
}

CDnAccelBlow::~CDnAccelBlow(void)
{
	SAFE_DELETE( m_pMoveSpeedBlow );
}

void CDnAccelBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"[수치1;수치2]";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		SetState( STATE_BLOW::STATE_END );

		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);
	}
	else
	{
		m_fStartValue = m_fValue = (float)atof(tokens[0].c_str()) * 0.01f;
		m_fEndValue = (float)atof(tokens[1].c_str()) * 0.01f;

		//값 설정에 따른 증감 값 설정..
		if (m_fStartValue <= m_fEndValue)
			m_fIncValue = 0.1f;
		else
			m_fIncValue = -0.1f;
	}
}

bool CDnAccelBlow::CanBegin( void )
{
	//대상에게 이동속도/프레임/동상?상태효과 적용되어 있으면 Accel상태효과는 적용 되지 않는다.
	bool bCanBegin = m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_076 ) ||
						m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_025) ||
						m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_144);
	
	return !bCanBegin;
}

void CDnAccelBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	//1초에 한번씩 Frame/속도 비율을 10%씩 증가 시킨다...
	m_IntervalChecker.OnBegin( LocalTime, 1000);

	m_hActor->UpdateFPS();

	char buff[64];
	sprintf_s(buff, "%f", m_fValue);
	m_pMoveSpeedBlow = new CDnBasicBlow( m_hActor, buff );
	m_pMoveSpeedBlow->SetBlow( STATE_BLOW::BLOW_076 );		// 이동속도 변화 비율.
	m_pMoveSpeedBlow->SetParentSkillInfo(&m_ParentSkillInfo);

	m_pMoveSpeedBlow->SetDurationTime(m_StateBlow.fDurationTime);
	m_pMoveSpeedBlow->OnBegin(LocalTime, fDelta);

	OutputDebug( "%s, Value:%f \n", __FUNCTION__, m_fValue );
}

void CDnAccelBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

	if (m_pMoveSpeedBlow)
		m_pMoveSpeedBlow->Process(LocalTime, fDelta);

	m_IntervalChecker.Process( LocalTime, fDelta );

}

bool CDnAccelBlow::OnCustomIntervalProcess( void )
{
	//갱신되기전 값을 임시로 저장해 놓는다..
	float fOldValue = m_fValue;

	//1초에 한번씩 증가.
	m_fValue += m_fIncValue;

	//EndValue를 넘어 가지 않도록
	if (m_fIncValue >= 0.0f)
		m_fValue = min(m_fEndValue, m_fValue);
	else
		m_fValue = max(m_fEndValue, m_fValue);

	//값이 변경이 있을때만 다시 계산 되도록한다..
	if (m_fValue != fOldValue)
	{
		if (m_hActor)
			m_hActor->UpdateFPS();

		if (m_pMoveSpeedBlow)
			m_pMoveSpeedBlow->SetModifyFloatValue(m_fValue);
	}
	
	return true;
}

void CDnAccelBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{ 
	SetState( STATE_BLOW::STATE_END );
	
	m_hActor->UpdateFPS();
	
	if (m_pMoveSpeedBlow)
		m_pMoveSpeedBlow->OnEnd(LocalTime, fDelta);
	
	OutputDebug( "%s\n", __FUNCTION__ );
}

// 일단 아무것도 안하도록 설정해 두겠습니다.
void CDnAccelBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{

}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAccelBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fStartValue[2] = {0.0f, };
	float fEndValue[2] = {0.0f, };

	std::vector<string> vlTokens[2];
	string strArgument[2];
	std::string delimiters = ";";

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	strArgument[0] = szOrigValue;
	//1. 구분
	TokenizeA(strArgument[0], vlTokens[0], delimiters);
	
	if( vlTokens[0].size() == 2 )
	{
		fStartValue[0] = (float)atof(vlTokens[0][0].c_str());
		fEndValue[0] = (float)atof(vlTokens[0][1].c_str());
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	strArgument[1] = szAddValue;

	TokenizeA(strArgument[1], vlTokens[1], delimiters);

	if( vlTokens[1].size() == 2 )
	{
		fStartValue[1] = (float)atof(vlTokens[1][0].c_str());
		fEndValue[1] = (float)atof(vlTokens[1][1].c_str());
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultStartValue = fStartValue[0] + fStartValue[1];
	float fResultEndValue = fEndValue[0] + fEndValue[1];

	sprintf_s(szBuff, "%f;%f", fResultStartValue, fResultEndValue);

	szNewValue = szBuff;
}

void CDnAccelBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW