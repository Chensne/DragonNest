#include "StdAfx.h"
#include "DnCheckBubbleCancelSkillBlow.h"

#include "DnPlayerActor.h"
#include "DnBubbleSystem.h"

#ifdef _CLIENT
#include "DnInterface.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCheckBubbleCancelSkillBlow::CDnCheckBubbleCancelSkillBlow(DnActorHandle hActor, const char* szValue) : CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_370;
	DNVector(std::string) vecTemp;
	TokenizeA(szValue, vecTemp, ";");

	if (vecTemp.size() == 3)
	{
		m_nBubbleID = atoi(vecTemp[0].c_str());
		m_nBubbleCount = atoi(vecTemp[1].c_str());
		m_nSkillID = atoi(vecTemp[2].c_str());
	} else {
		OutputDebug("%s vecTempParam size : %d", __FUNCTION__, vecTemp.size());
	}

	m_nProcessTime = 1.0f;


	SetValue(szValue);
}

CDnCheckBubbleCancelSkillBlow::~CDnCheckBubbleCancelSkillBlow(void)
{

}

void CDnCheckBubbleCancelSkillBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta)
{
	OutputDebug("CDnCheckBubbleCancelSkillBlow::OnBegin\n");
}


void CDnCheckBubbleCancelSkillBlow::Process(LOCAL_TIME LocalTime, float fDelta)
{
	CDnBlow::Process(LocalTime, fDelta);

	m_nProcessTime -= fDelta;
	if (m_nProcessTime > 0.0f)
		return;

	if (false == m_hActor->IsPlayerActor())
		return;

	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
	if(pPlayerActor)
	{
		BubbleSystem::CDnBubbleSystem* pBubbleSystem = pPlayerActor->GetBubbleSystem();
		if (pBubbleSystem)
		{
			int iNowBubbleCount = pBubbleSystem->GetBubbleCountByTypeID(m_nBubbleID);
			
			if (iNowBubbleCount == m_nBubbleCount)
			{
				pPlayerActor->MASkillUser::EndStateEffectSkill(m_nSkillID);
		//		pPlayerActor->MASkillUser::RemoveSkill(m_nSkillID);
		//				m_hActor->RemoveSkill(m_nSkillID);
#ifdef _CLIENT
		//		GetInterface().SendChatMessage(eChatType::CHATTYPE_NOTICE, L"", FormatW(L"%ws Remove Skill", __FUNCTIONW__));
#endif
				//m_hActor->TransformToNormal();
			}
		}
	}

	m_nProcessTime = 1.0f; //reset to 1secound!
}


void CDnCheckBubbleCancelSkillBlow::OnEnd(LOCAL_TIME LocalTime, float fDelta)
{
	OutputDebug("CDnCheckBubbleCancelSkillBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCheckBubbleCancelSkillBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}

void CDnCheckBubbleCancelSkillBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW