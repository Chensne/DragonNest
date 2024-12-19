#include "StdAfx.h"
#include "DnFrameBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnFrameBlow::CDnFrameBlow(DnActorHandle hActor, const char *szValue)
	: CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_025;
	SetValue(szValue);
	m_fValue = (float)atof(szValue);
}

CDnFrameBlow::~CDnFrameBlow(void)
{
}

void CDnFrameBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	m_hActor->UpdateFPS();

	OutputDebug( "CDnFrameBlow::OnBegin, Value:%f \n", m_fValue );
}

void CDnFrameBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{ 
	// 제한시간이 정해져 있지 않을 경우 STATE_DURATION 으로 되어있는 경우가 있어서 UpdateFPS() 에서 유효한 상태효과로
	// 분류되어 프레임 속도값이 적용되어 버리는 경우가 생김.
	SetState( STATE_BLOW::STATE_END );
	
	m_hActor->UpdateFPS();

	OutputDebug( "CDnFrameBlow::OnEnd\n" );
}

void CDnFrameBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	m_hActor->UpdateFPS();
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFrameBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnFrameBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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