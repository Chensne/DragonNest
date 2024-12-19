#pragma once

#include "DnAcceptRequestInfo.h"

#ifdef PRE_MOD_INTEG_SYSTEM_STATE

class CDnPartyAcceptRequestInviteInfo : public CDnAcceptRequestInfo
{
public:
	CDnPartyAcceptRequestInviteInfo() 	: m_PartyID(0), m_szPartyName(_T("")), m_szPassword(_T("")), m_nMaxUserCount(0), 
		m_nCurUserCount(0), m_nAvrLevel(0), m_nGameServerID(0), m_nGameMapIdx(0) {}

	void Clear();
	bool OnProcess(float fElapsedTime);
	bool OnClear();
	bool OnTerminate();

	const CDnPartyAcceptRequestInviteInfo& MakeDenyInfo(TPARTYID PartyID, const std::wstring& inviterName, int nGameMapIdx, int nGameServerId);

	TPARTYID m_PartyID;
	std::wstring m_szPartyName;
	std::wstring m_szPassword;
	int	m_nMaxUserCount;
	int	m_nCurUserCount;
	int	m_nAvrLevel;
	int	m_nGameServerID;
	int	m_nGameMapIdx;
};

#endif // PRE_MOD_INTEG_SYSTEM_STATE