#include "stdafx.h"
#include "NxConnectionManager.h"
#include "NxConnection.h"
#include "NxCompletionPort.h"

NxConnectionManager::NxConnectionManager()
:m_pNetIOCompletionPort(NULL), m_pMsgCompletionPort(NULL), m_pMsgHandler(NULL)
{

}

NxConnectionManager::~NxConnectionManager()
{
	Destroy();
}

bool
NxConnectionManager::Create(DWORD nConnectionCnt, NxCompletionPort* pNetIOCompletionPort, 
						 NxCompletionPort* pMsgCompletionPort, NxAsyncEventHandler* pMsgHandler)
{
	m_pNetIOCompletionPort = pNetIOCompletionPort;
	m_pMsgCompletionPort = pMsgCompletionPort;
	m_pMsgHandler = pMsgHandler;

	_CreateConnectionPool(nConnectionCnt);

	return true;
}

void
NxConnectionManager::Destroy()
{
	_DeleteConnectionPool();
	

}

NxConnection*	
NxConnectionManager::Find( DWORD nUID )
{
	return m_ConnectionArray.at(nUID);
}

NxConnection*	
NxConnectionManager::FindActiveConnection( DWORD nUID )
{
	NxGuard< ConnectionMap > Sync(m_ActiveConnectionMap);
	ConnectionMapIt iter;

	iter = m_ActiveConnectionMap.find( nUID );
	if( iter != m_ActiveConnectionMap.end() )
		return iter->second;

	return 0;
}


NxConnection*
NxConnectionManager::ActiveConnection( unsigned short nPort, SOCKET hSocket, sockaddr_in Addr )
{
	NxConnection*	pConnection = NULL;

	{
		NxGuard< ConnectionList > Sync(m_InActiveConnectionList);
		if( m_InActiveConnectionList.size() == 0 )
		{
			::closesocket( hSocket );
			return 0;
		}

		// 비활성화 리스트에서 하나 할당 받는다.
		ConnectionListIt iter = m_InActiveConnectionList.begin();
		pConnection = *iter;
		m_InActiveConnectionList.erase( iter );

	}

	// 생성 시킨다. 소켓 Attach
	pConnection->Create( hSocket, Addr );

	{
		NxGuard< ConnectionMap > Sync(m_ActiveConnectionMap);

		// 활성화 세션 맵에 등록
		DWORD	dwUID = 0;
		dwUID = pConnection->GetUID();
		m_ActiveConnectionMap[dwUID] = pConnection;

	}


	// IOCP 에 등록한다.
	m_pNetIOCompletionPort->AddCompletionPort(hSocket, pConnection);

	// 생성 됨
	pConnection->OnCreate();
	pConnection->OnAccept(nPort);


	return pConnection;
}

bool	
NxConnectionManager::InactiveConnection( NxConnection* pConnection )
{

	NxGuard< ConnectionMap > Sync(m_ActiveConnectionMap);
	ConnectionMapIt iter = m_ActiveConnectionMap.find( pConnection->GetUID() );

	if( iter != m_ActiveConnectionMap.end() )
	{

		// 세션 파괴됨
		pConnection->OnClose();
	
		//  Active의 Connection을 삭제하고 Pool에 넣는다.
		m_ActiveConnectionMap.erase( iter );

		{
			NxGuard< ConnectionList > Sync(m_InActiveConnectionList);
			m_InActiveConnectionList.push_back( pConnection );
			return true;
		}
	}
	return false;
}

NxConnection*
NxConnectionManager::ActiveConnection(const TCHAR* szIPAddress, unsigned short nPort)
{
	NxConnection*	pConnection = NULL;

	{
		NxGuard< ConnectionList > Sync(m_InActiveConnectionList);
		if( m_InActiveConnectionList.size() == 0 )
		{
			return NULL;
		}

		// 비활성화 리스트에서 하나 할당 받는다.
		ConnectionListIt iter = m_InActiveConnectionList.begin();
		pConnection = *iter;
		m_InActiveConnectionList.erase( iter );

		// 접속 시킨다.
		if ( pConnection->Connect(szIPAddress, nPort) == false )
		{
			// 접속실패면 비활성화 세션으로 추가
			m_InActiveConnectionList.push_back(pConnection);
			return false;
		}


	}



	{
		NxGuard< ConnectionMap > Sync(m_ActiveConnectionMap);

		// 활성화 세션 맵에 등록
		DWORD	dwUID = 0;
		dwUID = pConnection->GetUID();
		m_ActiveConnectionMap[dwUID] = pConnection;

		// IOCP 에 등록한다.
		m_pNetIOCompletionPort->AddCompletionPort((SOCKET)pConnection->GetHandle()->GetNativeHandle(), pConnection);

		// 생성 됨
		pConnection->OnCreate();
		pConnection->OnConnect();


		return pConnection;

	}


	

}

bool
NxConnectionManager::InactiveConnection(DWORD nUID)
{
	NxConnection* pConnection = NULL;
	pConnection = Find(nUID);
	if ( pConnection )
	{
		return InactiveConnection(pConnection);
	}
	return false;
}

void
NxConnectionManager::_CreateConnectionPool(DWORD nConnectionCnt)
{
	NxGuard< ConnectionList > Sync(m_InActiveConnectionList);

	m_ConnectionArray.resize(nConnectionCnt);
	for( DWORD i = 0 ; i < nConnectionCnt ; i++ )
	{
		NxConnection* pConnection = new NxConnection(i, this, m_pMsgCompletionPort, m_pMsgHandler);
		m_InActiveConnectionList.push_back( pConnection );
		m_ConnectionArray[i] =  pConnection;
	}
}

void
NxConnectionManager::_DeleteConnectionPool()
{
	NxConnection* pConnection = NULL;

	{
		NxGuard< ConnectionMap > Sync(m_ActiveConnectionMap);	
		// 활성화 세션 삭제
		for( ConnectionMapIt iter = m_ActiveConnectionMap.begin() ; iter != m_ActiveConnectionMap.end() ; iter++ )
		{
			pConnection = iter->second;

			if ( pConnection )
			{
//				InactiveConnection(pConnection);
				delete pConnection;
			}
		}

		m_ActiveConnectionMap.clear();
	}
	
	{
		NxGuard< ConnectionList > Sync(m_InActiveConnectionList);
		
		// 비활성화 세션 삭제
		for( ConnectionListIt iter = m_InActiveConnectionList.begin() ; iter != m_InActiveConnectionList.end() ; iter++ )
		{
			pConnection = *iter;

			if ( pConnection )
			{
				delete pConnection;
			}
		}

		m_InActiveConnectionList.clear();
	}

	m_ConnectionArray.resize(0);
	
}
