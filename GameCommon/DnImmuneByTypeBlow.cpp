#include "StdAfx.h"
#include "DnImmuneByTypeBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnImmuneByType::CDnImmuneByType( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_240;
	SetValue( szValue );
	m_fValue = 0.0f;

	m_nImmuneType = atoi(szValue);
}

CDnImmuneByType::~CDnImmuneByType(void)
{
}

void CDnImmuneByType::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}


void CDnImmuneByType::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnImmuneByType::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnImmuneByType::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
