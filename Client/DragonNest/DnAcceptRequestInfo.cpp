#include "StdAfx.h"
#include "DnInterface.h"
#include "DnAcceptRequestDataManager.h"
#include "PvPSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_MOD_INTEG_SYSTEM_STATE

bool CDnPartyAcceptRequestAskInfo::OnProcess(float fElapsedTime)
{
	CDnAcceptRequestInfo::OnProcess(fElapsedTime);

	if (m_fAcceptTime <= 0.f || GetInterface().IsOpenBlind())
	{
		if (CDnPartyTask::IsActive())
			CDnPartyTask::GetInstance().AnswerPartyAsking(false, m_szOtherSideName);
	}

	return true;
}

bool CDnPartyAcceptRequestAskInfo::OnClear()
{
	return true;
}

bool CDnPartyAcceptRequestAskInfo::OnTerminate()
{
	if (CDnPartyTask::IsActive())
		CDnPartyTask::GetInstance().AnswerPartyAsking(false, m_szOtherSideName);
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CDnQuickPvPAcceptRequestInfo::OnProcess(float fElapsedTime)
{
	CDnAcceptRequestInfo::OnProcess(fElapsedTime);

	if (m_fAcceptTime <= 0.f || GetInterface().IsOpenBlind())
	{
		CDnInterface::GetInstance().TimeUpAcceptRequestDialog();
	}

	return true;
}

bool CDnQuickPvPAcceptRequestInfo::OnTerminate()
{
	CDnInterface::GetInstance().TimeUpAcceptRequestDialog();

	return true;
}

#endif // PRE_MOD_INTEG_SYSTEM_STATE