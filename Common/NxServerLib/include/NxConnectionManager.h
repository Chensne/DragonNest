#pragma once

#include "NxDataStructure.h"

class NxAsyncEventHandler;
class NxCompletionPort;
class NxConnection;

class NxConnectionManager
{
public:
	NxConnectionManager();
	virtual	~NxConnectionManager();

	bool		Create(DWORD nConnectionCnt, NxCompletionPort* pNetIOCompletionPort, 
					   NxCompletionPort* pMsgCompletionPort, NxAsyncEventHandler* pMsgHandler);
	void		Destroy();

	NxConnection*	FindActiveConnection(DWORD nUID);
	NxConnection*	Find(DWORD nUID);

	NxConnection*	ActiveConnection(unsigned short nPort, SOCKET hSocket, sockaddr_in pAddr);
	bool			InactiveConnection(NxConnection* pConnection);

	NxConnection*	ActiveConnection(const TCHAR* szIPAddress, unsigned short nPort);
	bool			InactiveConnection(DWORD nUID);

	
	DWORD		GetActiveConnectionCnt()	{ return ( DWORD )m_ActiveConnectionMap.size();	}
	DWORD		GetInactiveConnectionCnt()	{ return ( DWORD )m_InActiveConnectionList.size();	}

protected:

	virtual	void	_CreateConnectionPool(DWORD nConnectionCnt);
	virtual	void	_DeleteConnectionPool();

protected:

	typedef	NxMap<DWORD, NxConnection*>		ConnectionMap;
	typedef	ConnectionMap::iterator			ConnectionMapIt;

	typedef std::vector<NxConnection*>		ConnectionArray;

	typedef NxList<NxConnection*>			ConnectionList;
	typedef ConnectionList::iterator		ConnectionListIt;

	ConnectionArray			m_ConnectionArray;
	ConnectionMap			m_ActiveConnectionMap;
	ConnectionList			m_InActiveConnectionList;

	// IO �� CP
	NxCompletionPort*	m_pNetIOCompletionPort;
	// Msg ó���� CP
	NxCompletionPort*	m_pMsgCompletionPort;
	
	// Msg �޴� �ڵ鷯 NxServerApp ��ü �̴�.
	NxAsyncEventHandler* m_pMsgHandler;


};

