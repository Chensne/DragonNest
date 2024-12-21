#include "StdAfx.h"
#include "DnTargetCriticalBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnTargetCriticalBlow::CDnTargetCriticalBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_250;
	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}

CDnTargetCriticalBlow::~CDnTargetCriticalBlow(void)
{

}


void CDnTargetCriticalBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s Critical Value : %f\n", __FUNCTION__, m_fValue);
}

void CDnTargetCriticalBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnTargetCriticalBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s \n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnTargetCriticalBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	szNewValue = szOrigValue;
}

void CDnTargetCriticalBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
