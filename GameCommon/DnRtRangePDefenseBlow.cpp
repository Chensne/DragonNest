#include "StdAfx.h"
#include "DnRtRangePDefenseBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnRtRangePDefenseBlow::CDnRtRangePDefenseBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_104;

	SetValue( szValue );
	m_fValue = (float)atof(szValue);
}

CDnRtRangePDefenseBlow::~CDnRtRangePDefenseBlow(void)
{

}

void CDnRtRangePDefenseBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{


	OutputDebug( "CDnRtRangePDefenseBlow::OnBegin\n" );
}


void CDnRtRangePDefenseBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnRtRangePDefenseBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

	OutputDebug( "CDnRtRangePDefenseBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnRtRangePDefenseBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnRtRangePDefenseBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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