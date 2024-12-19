#include "StdAfx.h"
#include "DnDamageRatioBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDamageRatioBlow::CDnDamageRatioBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_050;
	SetValue( szValue );
	m_fValue = (float)atof( szValue ); 
}

CDnDamageRatioBlow::~CDnDamageRatioBlow(void)
{
}


void CDnDamageRatioBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnDamageRatioBlow::OnBegin\n");
}


void CDnDamageRatioBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnDamageRatioBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnDamageRatioBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDamageRatioBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };
	szNewValue = szOrigValue;
}

void CDnDamageRatioBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };
	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
