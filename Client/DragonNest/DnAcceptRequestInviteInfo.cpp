#include "StdAfx.h"
#include "DnAcceptRequestInviteInfo.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_MOD_INTEG_SYSTEM_STATE

void CDnPartyAcceptRequestInviteInfo::Clear()
{
	m_PartyID = 0;
	m_szPartyName = L"";
	m_szPassword = L"";
	m_nGameServerID	= 0;
	m_nGameMapIdx = -1;
}

bool CDnPartyAcceptRequestInviteInfo::OnProcess(float fElapsedTime)
{
	CDnAcceptRequestInfo::OnProcess(fElapsedTime);

	if (m_fAcceptTime <= 0.f || GetInterface().IsOpenBlind())
	{
		if (CDnPartyTask::IsActive())
			CDnPartyTask::GetInstance().ReqDenyPartyInvite(*this);
	}

	return true;
}

bool CDnPartyAcceptRequestInviteInfo::OnClear()
{
	return true;
}

bool CDnPartyAcceptRequestInviteInfo::OnTerminate()
{
	if (CDnPartyTask::IsActive())
		CDnPartyTask::GetInstance().ReqDenyPartyInvite(*this);
	return true;
}

const CDnPartyAcceptRequestInviteInfo& CDnPartyAcceptRequestInviteInfo::MakeDenyInfo(TPARTYID PartyID, const std::wstring& inviterName, int nGameMapIdx, int nGameServerId)
{
	Clear();
	m_nGameMapIdx = nGameMapIdx;
	m_nGameServerID = nGameServerId;
	m_PartyID = PartyID;
	m_szOtherSideName = inviterName;

	return *this;
}

#endif // PRE_MOD_INTEG_SYSTEM_STATE