#include "StdAfx.h"
#include "DnDisableSkillBlow.h"


#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnDisableSkillBlow::CDnDisableSkillBlow(DnActorHandle hActor, const char* szValue) : CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_351;
	SetValue(szValue);

	DNVector(std::string) nTokens;
	TokenizeA(szValue, nTokens, "/");

	for (DWORD i = 0; i < nTokens.size(); i++)
		m_vecDisabledSkills.push_back(atoi(nTokens[i].c_str()));
}

CDnDisableSkillBlow::~CDnDisableSkillBlow(void)
{

}

void CDnDisableSkillBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta)
{
	__super::OnBegin(LocalTime, fDelta);
#ifdef _CLIENT
	OutputDebug("%s count: %d", __FUNCTION__,m_vecDisabledSkills.size());
#endif // #if defined( _GAMESERVER )
}

void CDnDisableSkillBlow::Process(LOCAL_TIME LocalTime, float fDelta)
{
	__super::Process(LocalTime, fDelta);
}

void CDnDisableSkillBlow::OnEnd(LOCAL_TIME LocalTime, float fDelta)
{
#ifdef _CLIENT
	OutputDebug("%s count: %d", __FUNCTION__, m_vecDisabledSkills.size());
#endif // #if defined( _GAMESERVER )	__super::OnEnd(LocalTime, fDelta);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDisableSkillBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}

void CDnDisableSkillBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

bool CDnDisableSkillBlow::IsDisabledSkill(int nSkillID)
{
	for (DWORD i = 0; i < m_vecDisabledSkills.size(); i++)
		if (m_vecDisabledSkills[i] == nSkillID)
			return true;

	return false;
}