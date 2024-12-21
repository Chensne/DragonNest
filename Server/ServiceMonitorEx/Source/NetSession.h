

#pragma once

#include "Define.h"
#include "EventSelectTcpClient.h"
#include "ServiceInfo.h"


class CNetSession : public CEventSelectTcpClientSession<CNetSession>
{

public:
	typedef	CEventSelectTcpClientSession<CNetSession>	TP_SUPER;

public:
	CNetSession();
	virtual ~CNetSession();

	VOID Reset();

	INT GetMonitorLevel() const { return m_MonitorLevel; }
	VOID SetMonitorLevel(INT pMonitorLevel) { m_MonitorLevel = pMonitorLevel; }

	DWORD Connect(UINT pIpAddress, WORD pPortNumber);
	VOID Disconnect();
	BOOL IsConnect();
	VOID Reconnect();
	VOID SetConnect(bool connect) {m_bConnect = connect;}

	DWORD SendData(SHORT pMainCmd, SHORT pSubCmd, PCHAR pBuffer, INT pLen);

	CServiceInfo* GetServiceInfo () {return m_pServiceInfo;}

private:
	INT m_MonitorLevel;		// eServerMonitorLevel

	CServiceInfo* m_pServiceInfo;

	UINT m_pIpAddress;
	WORD m_pPortNumber;
	bool m_bConnect;	
};

