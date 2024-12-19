#include "StdAfx.h"
#include "DnChangeWeightBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnChangeWeightBlow::CDnChangeWeightBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), m_fWeightDelta( 0.0f )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_131;

	SetValue( szValue );
	m_fValue = (float)atof( szValue );
}

CDnChangeWeightBlow::~CDnChangeWeightBlow(void)
{
}


void CDnChangeWeightBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 예전 무게값을 받아둔다. 
	// 복수개의 상태효과가 적용될 수 있으므로 델타값만 받아서 나중에 복구 시킨다.
	float fNowWeightValue = m_hActor->GetWeight();
	m_fWeightDelta = fNowWeightValue - m_fValue;

	// 새롭게 무게값을 적용.
	m_hActor->SetWeight( m_fValue );

	OutputDebug( "CDnChangeWeightBlow::OnBegin Value: Now: %2.2f, Dest: %2.2f, Delta: %2.2f\n", fNowWeightValue, m_fValue, m_fWeightDelta );
}


void CDnChangeWeightBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	float fNowWeightValue = m_hActor->GetWeight();
	m_hActor->SetWeight( fNowWeightValue + m_fWeightDelta );

	OutputDebug( "CDnChangeWeightBlow::OnEnd Value: Now: %2.2f, Dest: %2.2f, Delta: %2.2f\n", fNowWeightValue, m_fValue, m_fWeightDelta );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangeWeightBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnChangeWeightBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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