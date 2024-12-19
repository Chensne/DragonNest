#include "StdAfx.h"
#include "DnEmptyBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnEmptyBlow::CDnEmptyBlow(DnActorHandle hActor, const char* szValue) : CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_306;

	SetValue(szValue);
}

CDnEmptyBlow::~CDnEmptyBlow(void)
{
}

void CDnEmptyBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta)
{
	__super::OnBegin(LocalTime, fDelta);
}


void CDnEmptyBlow::Process(LOCAL_TIME LocalTime, float fDelta)
{
	__super::Process(LocalTime, fDelta);
	
}


void CDnEmptyBlow::OnEnd(LOCAL_TIME LocalTime, float fDelta)
{
	__super::OnEnd(LocalTime, fDelta);
	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnEmptyBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}

void CDnEmptyBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
