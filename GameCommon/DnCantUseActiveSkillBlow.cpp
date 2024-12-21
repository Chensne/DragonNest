#include "StdAfx.h"
#include "DnCantUseActiveSkillBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCantUseActiveSkillBlow::CDnCantUseActiveSkillBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_067;

	// 이 상태효과는 따로 값을 쓰는 것은 없다..
	SetValue( szValue );

}

CDnCantUseActiveSkillBlow::~CDnCantUseActiveSkillBlow(void)
{

}

void CDnCantUseActiveSkillBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnCantUseActiveSkillBlow::OnBegin\n" );
}


void CDnCantUseActiveSkillBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnCantUseActiveSkillBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnCantUseActiveSkillBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCantUseActiveSkillBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnCantUseActiveSkillBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
