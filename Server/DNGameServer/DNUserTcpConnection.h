
#pragma once

#include "Connection.h"

class CDNUserSession;
class CDNGameServerManager;

class CDNTcpConnection : public CConnection, public TBoostMemoryPool< CDNTcpConnection >
{
public:
	CDNTcpConnection(CDNGameServerManager * pManager);
	~CDNTcpConnection();

	CIocpManager * GetIocpManager() { return m_pIocpManager; }
	CSocketContext * GetSocketContext() { return m_pSocketContext; }

	void TcpConnected(char * pData, int iLen);
	void TcpDisconnected(wchar_t *pIdent);
	bool SetSession(CDNUserSession * pSession);

	int MessageProcess(int iMainCmd, int iSubCmd, char * pData, int iSize);

	bool IsAttachedToSession() { return m_bAttached; }
	virtual bool bIsUserSession(){ return true; }
	CDNUserSession* GetUserSession(){ return m_pSession; }

protected:
	volatile bool m_bAttached;
	CDNUserSession * m_pSession;
	CDNGameServerManager * m_pManager;

private:
	volatile bool m_bAttachVerify;

	friend class CDNIocpManager;
	bool GetAttachVerify() { return m_bAttachVerify; }
	void SetAttachSession();
};