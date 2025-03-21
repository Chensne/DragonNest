#include "StdAfx.h"
#include "DnAddStateOnTimer.h"
#if defined( _GAMESERVER )
#include "DnPlayerActor.h"
#endif // #if defined( _GAMESERVER )

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnAddStateOnTimerBlow::CDnAddStateOnTimerBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#if defined(_GAMESERVER)
,m_IntervalChecker( hActor, GetMySmartPtr() )
#endif // _GAMESERVER
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_212;
	SetValue( szValue );
	
	SetInfo(szValue);
}

CDnAddStateOnTimerBlow::~CDnAddStateOnTimerBlow(void)
{

}



void CDnAddStateOnTimerBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(_GAMESERVER)
	m_IntervalChecker.OnBegin( LocalTime, m_TimerTime);
#endif // _GAMESERVER
}



void CDnAddStateOnTimerBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

#if defined(_GAMESERVER)
	m_IntervalChecker.Process( LocalTime, fDelta );
#endif // _GAMESERVER
}


void CDnAddStateOnTimerBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if defined(_GAMESERVER)
	m_IntervalChecker.OnEnd(LocalTime, fDelta);
#endif // _GAMESERVER
}

void CDnAddStateOnTimerBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"[확률(1.0);탐색범위(cm);시간간격(ms);상태효과Index;상태효과지속시간][상태효과설정값]";
	std::vector<std::string> tokens;
	std::string delimiters = "[]";

	//1. 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (5 == infoTokens.size())
		{
			m_fRate = m_fValue = (float)atof(infoTokens[0].c_str());
			m_fRange = (float)atof(infoTokens[1].c_str());
			m_TimerTime = atoi(infoTokens[2].c_str());
			m_nDestStateBlowIndex = atoi(infoTokens[3].c_str());
			m_nDestStateBlowIndex = 42;
			m_nStateDurationTime = atoi(infoTokens[4].c_str());
			m_nStateDurationTime = 5000;
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);

		//3. 상태효과 설정값
		m_strStateAttribute = tokens[1];
	}
}


#ifdef _GAMESERVER
bool CDnAddStateOnTimerBlow::OnCustomIntervalProcess( void )
{
	//확률 계산..
	bool bExecuteable = rand() % 10000 <= (m_fRate * 10000.0f);
	if (!bExecuteable)
	{
		OutputDebug("%s 확률 걸러짐\n", __FUNCTION__);
		return true;
	}

	DNVector(DnActorHandle) VecList;
	int nCount = m_hActor->ScanActor(m_hActor->GetRoom(), *m_hActor->GetPosition(), m_fRange, VecList);
	
	for (int i = 0; i < nCount; ++i)
	{
		DnActorHandle hActor = VecList[i];
		if (!hActor || hActor->IsDie())
			continue;

		//같은 편이거나 자신일경우
		if (hActor->GetTeam() == m_hActor->GetTeam() || hActor == m_hActor)
			continue;

		//같은 상태효과 적용 되어 있으면 건너뜀.
		if (hActor->IsAppliedThisStateBlow((STATE_BLOW::emBLOW_INDEX)m_nDestStateBlowIndex))
			continue;

		hActor->CmdAddStateEffect(&m_ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)m_nDestStateBlowIndex, m_nStateDurationTime, m_strStateAttribute.c_str());
	}

	return true;
}

#endif


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAddStateOnTimerBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnAddStateOnTimerBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
