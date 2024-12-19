#include "StdAfx.h"
#include "DnCantUseMPUsingSkillBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCantUseMPUsingSkillBlow::CDnCantUseMPUsingSkillBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_078;

	// 이 상태효과는 따로 값을 쓰는 것은 없다..
	SetValue( szValue );

}

CDnCantUseMPUsingSkillBlow::~CDnCantUseMPUsingSkillBlow(void)
{

}

void CDnCantUseMPUsingSkillBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	m_hActor->AddedCantUseSkillSE();

	OutputDebug( "CDnCantUseMPUsingSkillBlow::OnBegin\n" );
}


void CDnCantUseMPUsingSkillBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnCantUseMPUsingSkillBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	m_hActor->RemovedCantUseSkillSE();

	OutputDebug( "CDnCantUseMPUsingSkillBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCantUseMPUsingSkillBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };
	
	szNewValue = szOrigValue;
}

void CDnCantUseMPUsingSkillBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
