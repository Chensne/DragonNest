#include "StdAfx.h"
#include "DnIgnorePrefixSkillBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnIgnorePrefixSkillBlow::CDnIgnorePrefixSkillBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_183;
	SetValue( szValue );

	m_fValue = 0.0f;

}

CDnIgnorePrefixSkillBlow::~CDnIgnorePrefixSkillBlow(void)
{

}

void CDnIgnorePrefixSkillBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

}

void CDnIgnorePrefixSkillBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
	
}


void CDnIgnorePrefixSkillBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnIgnorePrefixSkillBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnIgnorePrefixSkillBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
