#include "StdAfx.h"
#include "DnParryingProbBlow.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnParryingProbBlow::CDnParryingProbBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_056;

	SetValue( szValue );

	// 0.0~1.0 인 이 확률 값을 받아서 31번 패링 상태효과에서 패링할때 더해준다.
	m_fValue = (float)atof( szValue );
}

CDnParryingProbBlow::~CDnParryingProbBlow(void)
{

}

void CDnParryingProbBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnParryingProbBlow::OnBegin\n" );
}


void CDnParryingProbBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnParryingProbBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnParryingProbBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnParryingProbBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnParryingProbBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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