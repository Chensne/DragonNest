#include "StdAfx.h"
#include "DnIgnoreEffectBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnIngnoreEffectBlow::CDnIngnoreEffectBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_252;
	SetValue( szValue );
	
	AddInvincibleAt(szValue);
	m_fValue = 0.0f;
}

CDnIngnoreEffectBlow::~CDnIngnoreEffectBlow(void)
{
}

void CDnIngnoreEffectBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}


void CDnIngnoreEffectBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}

void CDnIngnoreEffectBlow::AddInvincibleAt(const char* szValue)
{
	std::string str = szValue;
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	TokenizeA(str, tokens, delimiters);

	std::vector<std::string>::iterator iter = tokens.begin();
	for ( ; iter != tokens.end(); ++iter)
	{
		AddInvincibleAt(atoi(iter->c_str()));
	}
}

void CDnIngnoreEffectBlow::AddInvincibleAt(int nSkillID)
{
	m_InvincibleAtList.insert(INVINCIBLEAT_LIST::value_type(nSkillID, nSkillID));
}

bool CDnIngnoreEffectBlow::IsInvincibleAt(int nSkillID)
{
	//해당 스킬ID를 리스트에서 찾는다.
	INVINCIBLEAT_LIST::iterator iter = m_InvincibleAtList.find(nSkillID);
	
	//리스트에 있으면 true, 없으면 false
	return (iter != m_InvincibleAtList.end());
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnIngnoreEffectBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
	//szOrigValue 마지막에 ";"가 없으면 추가 해서 szAddValue를 추가 한다.
	int nLength = (int)strlen(szOrigValue);

	if (nLength == 0 || szOrigValue[nLength - 1] == ';')
		szNewValue += szAddValue;
	else
	{
		szNewValue += ";";
		szNewValue += szAddValue;
	}
}

void CDnIngnoreEffectBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//"###;###;###;###;....;##;##;##"
	//|<--이전 문자열----->|<-AddValue->|

	//szOrigValue에서 szAddValue 문자열길이 만큼 뒤에서 자른다.
	int nOrigLength = (int)strlen(szOrigValue);
	int nAddLength = (int)strlen(szAddValue);
	int nCount = nOrigLength - nAddLength;

	sprintf_s(szBuff, "%s", szOrigValue);
	szBuff[nCount] = 0;

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
