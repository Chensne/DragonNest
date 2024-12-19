#pragma once

#include "NetSession.h"
#include "NetPacketParser.h"
#include "Singleton.hpp"
#include "CriticalSection.hpp"
#include "ServiceInfo.h"


class CNetConnection
{
public:
	CNetConnection();
	virtual ~CNetConnection();

	DWORD Open();
	VOID Close();
	BOOL IsOpen() const { return(m_IsOpen);}
	
	CNetSession* GetSession();
	VOID ReleaseSession(CNetSession* pNetSession);

	CServiceInfo* GetServiceInfo ();

private:
	static VOID TcpClientErrorNotifyProc(LPVOID pModule, INT pErrorCode, LPCTSTR pErrorMessage, LPVOID pErrorNotifyProcParam);
	static VOID TcpClientSessionBaseNotifyProc(EF_NETWORK_BASE_NOFITY pNotifyType, CSessionBase* pSession, LPVOID pResultParam, LPVOID pBaseNotifyProcParam);
	static DWORD PacketNotify(LPVOID pSession, SHORT pMainCmd, SHORT pSubCmd, LPBYTE pBuffer, SHORT pPacketLen);

public:
	static VOID SendRequestServiceStruct(CNetSession* pNetSession, INT pContainerVersion /* = -1 */);

private:
	BOOL m_IsOpen;
	CEventSelectTcpClient<CNetSession> m_TcpClient;
	CNetPacketParser		m_PacketParser;
	CNetSession*			m_pNetSession;
	
};

class CNetConnectionMgr
{
private:
	DECLARE_SINGLETON_CLASS(CNetConnectionMgr);

public:
	CNetConnectionMgr();
	~CNetConnectionMgr();

	CNetConnection* CreateConnection();
	CNetConnection* GetConnection (UINT nConID);
	void RemoveConnection (UINT nConID);
	void RegistConnection (CNetConnection* pNetCon);

	CNetSession* GetCurrentSession();

private:
	std::map<UINT, CNetConnection*>	m_MapNetConnections;
	INT								m_nConnectionID;
	mutable CCriticalSection		m_Lock;

	CNetSession*					m_CurrentSession;
};

