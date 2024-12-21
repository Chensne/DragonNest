#pragma once

#include "Connection.h"

#if defined(_TW) && defined(_FINAL_BUILD)

class CDNGamaniaAuth: public CConnection
{
private:
	DWORD m_dwReconnectTick;
	eConnectionKey m_ConnectionKey;

	int ConvertResult(char* strOutString);

public:
	CDNGamaniaAuth(eConnectionKey pConnectionKey, char szIP[IPLENMAX], int nPort);
	~CDNGamaniaAuth(void);

	void DoUpdate(DWORD CurTick);
	void Reconnect(DWORD CurTick);
	int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	int AddSendData(char *pData, int nLen);

	int SendLogin(char *pAccountName, char *pPassword, char *pIp, bool bAuthenticationFlag, bool bDuplicationCheck, UINT pSessionID);
	int SendLogout(const char *pAccountName, char *pIp);
};

extern CDNGamaniaAuth* g_pGamaniaAuthLogin;
extern CDNGamaniaAuth* g_pGamaniaAuthLogOut;

#endif	// #if defined(_TW)
