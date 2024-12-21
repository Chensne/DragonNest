#include "StdAfx.h"
#include "DnRemoveStateBlow.h"
#if defined( _GAMESERVER )
#include "DnPlayerActor.h"
#endif // #if defined( _GAMESERVER )

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnRemoveStateBlow::CDnRemoveStateBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_211;
	SetValue( szValue );
	m_fValue = 0.0f;

	SetInfo(szValue);
}

CDnRemoveStateBlow::~CDnRemoveStateBlow(void)
{

}

void CDnRemoveStateBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"ID1;ID2;.....;";//제거할 스킬 ID리스트 나열..
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//
	TokenizeA(str, tokens, delimiters);
	
	int nCount = (int)tokens.size();

	for (int i = 0; i < nCount; ++i)
	{
		int nSkillID = atoi(tokens[i].c_str());
		if (nSkillID != 0)
			m_SkillIDList.insert(std::make_pair(nSkillID, nSkillID));
		else
			OutputDebug("%s SkillID[%d]=%s is invalid!!!\n", __FUNCTION__, i, tokens[i].c_str());
	}	
}

void CDnRemoveStateBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(_GAMESERVER)
	RemoveStateBlowBySkillID();
#endif // _GAMESERVER
}

void CDnRemoveStateBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnRemoveStateBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}

#if defined(_GAMESERVER)
void CDnRemoveStateBlow::RemoveStateBlowBySkillID()
{
	DNVector(DnBlowHandle) vlBlowList;

	FindSkillBlowList(m_hActor, vlBlowList);
	
	int nBlowCount = (int)vlBlowList.size();
	for (int n = 0; n < nBlowCount; ++n)
	{
		DnBlowHandle hBlow = vlBlowList[n];
		if (!hBlow)
			continue;

		// 스킬 유저 액터 확인은 안 하는걸로..(프랍?에 의해서 추가된 상태효과는 스킬 유저 액터가 없음.)
		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
		if (!pSkillInfo /*|| !pSkillInfo->hSkillUser*/)
			continue;

		//자신에게 적용할 상태효과를 예약한다..
		m_hActor->CmdRemoveStateEffectFromID(hBlow->GetBlowID());
	}

}

bool CDnRemoveStateBlow::FindSkillID(int nSkillID)
{
	std::map<int, int>::iterator iter = m_SkillIDList.find(nSkillID);
	
	return (iter != m_SkillIDList.end());
}

void CDnRemoveStateBlow::FindSkillBlowList(DnActorHandle hActor, DNVector(DnBlowHandle)& vlBlowList)
{
	if (!hActor)
		return;

	int nStateBlowCount = hActor->GetNumAppliedStateBlow();

	for (int i = 0; i < nStateBlowCount; ++i)
	{
		DnBlowHandle hBlow = hActor->GetAppliedStateBlow(i);
		if (!hBlow)
			continue;

		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
		//스킬 정보가 없으면 건너뜀. (스킬 유저가 없을 수 있음..(프랍?에 의해 적용된 상태효과)
		if (!pSkillInfo /*|| !pSkillInfo->hSkillUser*/)
			continue;

		if (FindSkillID(pSkillInfo->iSkillID))
			vlBlowList.push_back(hBlow);
	}
}
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnRemoveStateBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnRemoveStateBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
