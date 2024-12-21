#include "StdAfx.h"
#include "DnIgnoreCanHitBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnIgnoreCanHitBlow::CDnIgnoreCanHitBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_138;

	SetValue( szValue );
	m_fValue = (float)atof( szValue );
}

CDnIgnoreCanHitBlow::~CDnIgnoreCanHitBlow(void)
{
}


void CDnIgnoreCanHitBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnIgnoreCanHitBlow::OnBegin: %2.2f\n", m_fValue );
}


void CDnIgnoreCanHitBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnIgnoreCanHitBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnIgnoreCanHitBlow::OnEnd: %2.2f\n", m_fValue );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnIgnoreCanHitBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnIgnoreCanHitBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW