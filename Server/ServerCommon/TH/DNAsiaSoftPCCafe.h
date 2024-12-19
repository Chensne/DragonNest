#pragma once

#include "Connection.h"

#if defined(_TH)

class CDNAsiaSoftPCCafe : public CConnection
{
private:
	DWORD m_dwReconnectTick;
	bool m_bConnectDelay;	
	
public:
	CDNAsiaSoftPCCafe(char szIP[IPLENMAX], int nPort);
	virtual ~CDNAsiaSoftPCCafe(void);

	int AddSendData(char *pData, int nLen);
	
	void Reconnect(DWORD dwCurTick);
	virtual int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);			

	// Request IP Bonus
	void SendCheckIPBonus(const char *pAccountName, char *pIp, char cType,  UINT nAccountDBID);	
};

extern CDNAsiaSoftPCCafe *g_pAsiaSoftPCCafe;

#endif