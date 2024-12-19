#pragma once

#include "Connection.h"

#if defined( _GAMESERVER )
class CDNGameRoom;
class CDNUserSession;
class CDnPlayerActor;
#endif

class CDNLogConnection: public CConnection
{
private:
	DWORD m_dwReconnectTick;
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	DWORD m_dwLastRecvTick;
	DWORD m_dwSendSYNTick;
	DWORD m_dwSendSYNACKTick;
#endif

public:
	CDNLogConnection(void);
	virtual ~CDNLogConnection(void);

#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	void ResetLiveCheck();
	void LiveCheck(DWORD CurTick);	
	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);
#else	//#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen) { return ERROR_NONE; }
#endif	//#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	void Reconnect(DWORD CurTick);
};

extern CDNLogConnection* g_pLogConnection;
