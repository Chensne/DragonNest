#pragma once

#include "IocpManager.h"

class CSpinBuffer;

#if defined( PRE_FIX_67546 )
class CDNConnection;
#endif
class CDNIocpManager: public CIocpManager
{
protected:
	static UINT __stdcall ReconnectThread(void *pParam);

	void OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort);
	void OnConnected(CSocketContext *pSocketContext);
	void OnDisconnected(CSocketContext *pSocketContext);
	void OnConnectFail(CSocketContext *pSocketContext);

public:
	CDNIocpManager(void);
	virtual ~CDNIocpManager(void);

	int CreateThread();
	void OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred);
	CSpinBuffer* GetSpinBuffer( UINT uiIndex );

	void DoUpdate( UINT uiCurTick );

#if defined( PRE_FIX_67546 )
	void AddConnectionCount( int nIndex );
	void GetConnectionCount( int * ConnectionCount );
	void AddVillageGameConnection( CDNConnection* pCon );
	void DelVillageGameConnection( CDNConnection* pCon );
	CDNConnection* GetVillageGameConnection( int nIndex );
	int GetVillageGameConnectionCount() {return (int)m_vVillageGameConnection.size();}
#endif

private:
	std::vector<CSpinBuffer*>	m_vSpinBuffer;
	std::vector<bool>			m_vCallEvent;

	UINT	m_uiLastUpdateTick;
#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	ULONG m_nLastHearbeatTick;
#endif
#if defined( PRE_FIX_67546 )
	int m_nConnectionCount[THREADMAX];
	std::vector<CDNConnection*> m_vVillageGameConnection;
#endif
};

extern CDNIocpManager* g_pIocpManager;
