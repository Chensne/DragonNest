#include "StdAfx.h"
#include "DnManaShieldBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnManaShieldBlow::CDnManaShieldBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_243;
	SetValue( szValue );

	m_fAbsorbDamageRatio = 0.0f;
	m_fAbsorbMPRatio = 0.0f;

	std::string str = szValue;
	std::vector<std::string> tokens;
	TokenizeA( str, tokens, ";" );

	if(tokens.size() == 2)
	{
		m_fAbsorbDamageRatio = (float)atof(tokens[0].c_str());
		m_fAbsorbMPRatio = (float)atof(tokens[1].c_str());
	}
}

CDnManaShieldBlow::~CDnManaShieldBlow(void)
{

}

void CDnManaShieldBlow::CalcManaShield( const float fOriginalDamage , float &fAbsorbDamage, int &nAbsorbSP )
{
	fAbsorbDamage += fOriginalDamage * m_fAbsorbDamageRatio;
	nAbsorbSP += (int)(fAbsorbDamage * m_fAbsorbMPRatio);
}

void CDnManaShieldBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
}

void CDnManaShieldBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnManaShieldBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnManaShieldBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}

void CDnManaShieldBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
