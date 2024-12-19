#pragma once

#include "ConnectionManager.h"

class CDNAgentConnection;
class CDNUserConnection;
class CDNUserConnectionManager: public CConnectionManager
{
#if defined(PRE_ADD_LOGIN_USERCOUNT)
private:
	LONG m_TotalUserCount;	
#endif
	
public:
	CDNUserConnectionManager(void);
	virtual ~CDNUserConnectionManager(void);

	CConnection* AddConnection(const char *pIp, const USHORT nPort);

#ifdef _USE_ACCEPTEX
	virtual CConnection* GetConnection(UINT nUID);
	virtual CConnection* GetConnectionByIPPort(const char *pIp, const USHORT nPort);
	CDNUserConnection* GetConnectionByAccountDBID(UINT nAccountDBID);
	void DetachUserByWorldID(int nWorldID);
	void DetachUserByWorldSetID(int nWorldSetID);
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	void DetachUserByIP(UINT nAccountDBID, const char * pszIP);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#endif
#if defined(_TW) 
	void SendAllLogOutTW();
#endif	// #if defined(_TW)

#if defined(PRE_ADD_LOGIN_USERCOUNT)
	inline void AddUserCount() { InterlockedIncrement(&m_TotalUserCount);};
	inline void DelUserCount() { InterlockedDecrement(&m_TotalUserCount);};
	inline UINT GetUserCount() { return static_cast<UINT>(m_TotalUserCount);};
#endif
};

extern CDNUserConnectionManager* g_pUserConnectionManager;
