#pragma once

#include "Connection.h"

#if defined(_KR) && defined(_FINAL_BUILD)

class CDNUser;
class CDNNexonAuth: public CConnection
{
private:
	DWORD m_dwReconnectTick;
	DWORD m_dwAuthAliveTick;

public:
	CDNNexonAuth(void);
	~CDNNexonAuth(void);

	void DoUpdate(DWORD CurTick);
	void Reconnect(DWORD CurTick);
	int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	void SendInitialize(int nDomainSn);
	void SendLogin(CDNUser *pUser);
	void SendLogout(CDNUser *pUser);
	void SendSynchronize(int nCount, std::vector<INT64> &VecSessionList, BYTE cIsMonitoring);
	void SendAlive();
};

extern CDNNexonAuth* g_pNexonAuth;

#endif	// _KR
