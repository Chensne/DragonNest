#include "StdAfx.h"
#include "DnOrderMySummonedMonsterBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnOrderMySummonedMonsterBlow::CDnOrderMySummonedMonsterBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_215;
	m_iSkillID = 0;

	SetValue( szValue );
	m_iSkillID = atoi( szValue );
}

CDnOrderMySummonedMonsterBlow::~CDnOrderMySummonedMonsterBlow(void)
{

}

void CDnOrderMySummonedMonsterBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

}

void CDnOrderMySummonedMonsterBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnOrderMySummonedMonsterBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	int nValue[2];

	nValue[0] = atoi( szOrigValue );
	nValue[1] = atoi( szAddValue );

	int nResultValue = nValue[0] + nValue[1];

	char szBuff[128] = {0, };
	sprintf_s( szBuff, "%d", nResultValue );

	szNewValue = szBuff;
}

void CDnOrderMySummonedMonsterBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	int nValue[2];

	nValue[0] = atoi( szOrigValue );
	nValue[1] = atoi( szAddValue );

	int nResultValue = nValue[0] - nValue[1];

	char szBuff[128] = {0, };
	sprintf_s (szBuff, "%f", nResultValue );
	szNewValue = szBuff;
	
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW