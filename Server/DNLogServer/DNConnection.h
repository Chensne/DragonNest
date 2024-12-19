#pragma once

#include "Connection.h"

class CDNConnection: public CConnection
{
public:
	CDNConnection(void);
	virtual ~CDNConnection(void);

	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);
	void DBMessageProcess(char *pData, int nThreadID);

	void DoUpdate(DWORD CurTick);
	void SendLiveCheck();

private:
#if defined(PRE_MODIFY_CONNECTIONSYNC_01)
	virtual void Release();
#endif	// #if defined(PRE_MODIFY_CONNECTIONSYNC_01)
	DWORD m_dwLiveTick;
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	DWORD m_dwLastRecvTick;
	DWORD m_dwSendSYNTick;
	DWORD m_dwSendSYNACKTick;
#endif
};
