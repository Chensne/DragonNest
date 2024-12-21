#include "StdAfx.h"
#include "DnConsumeBubbleBlow.h"
#ifdef _CLIENT
#include "DnInterface.h"
#endif

#include "DnPlayerActor.h"
#include "DnBubbleSystem.h"
#ifdef _GAMESERVER
#endif
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnConsumeBubbleBlow::CDnConsumeBubbleBlow(DnActorHandle hActor, const char* szValue) : CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_350;

	
	DNVector(std::string) vecParam;
	TokenizeA(szValue, vecParam, ";");

	if (vecParam.size() == 2)
	{
		m_nTime = (float)atof(vecParam[0].c_str()) / 1000.0f;
		m_nBubbleID = atoi(vecParam[1].c_str());
	} else {
		OutputDebug("%s VecParamSize: %d", __FUNCTION__, vecParam.size());
	}

	m_nProcessTime = m_nTime;
	SetValue(szValue);
}

CDnConsumeBubbleBlow::~CDnConsumeBubbleBlow(void)
{

}
	

void CDnConsumeBubbleBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta)
{
	OutputDebug("CDnConsumeBubbleBlow::OnBegin\n");
}


void CDnConsumeBubbleBlow::Process(LOCAL_TIME LocalTime, float fDelta)
{
	CDnBlow::Process(LocalTime, fDelta);


	if (false == m_hActor->IsPlayerActor())
		return;

	m_nProcessTime -= fDelta;
	if (m_nProcessTime > 0.0f)
		return;

	m_nProcessTime = m_nTime;
	//
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
	if(pPlayerActor)
	{
		BubbleSystem::CDnBubbleSystem* pBubbleSystem = pPlayerActor->GetBubbleSystem();
		if(pBubbleSystem)
		{
			int iNowBubbleCount = pBubbleSystem->GetBubbleCountByTypeID(m_nBubbleID);
			if (iNowBubbleCount > 0)
			{
				pBubbleSystem->RemoveBubbleByTypeID(m_nBubbleID, 1); 
			}
		}
	}

#ifdef _CLIENT
//	GetInterface().AddChatMessage(eChatType::CHATTYPE_NORMAL, L"", FormatW(L"%ws Consume Bubble.", __FUNCTION__).c_str());
#endif
}


void CDnConsumeBubbleBlow::OnEnd(LOCAL_TIME LocalTime, float fDelta)
{

	OutputDebug("CDnConsumeBubbleBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnConsumeBubbleBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}

void CDnConsumeBubbleBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = { 0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW