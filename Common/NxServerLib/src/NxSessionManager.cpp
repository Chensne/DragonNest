#include "stdafx.h"
#include "NxSession.h"
#include "NxSessionManager.h"
#include "NxServerApp.h"

NxSessionManager::NxSessionManager()
{
	m_pServerApp = NULL;

}

NxSessionManager::~NxSessionManager()
{
	Destroy();
}

bool
NxSessionManager::Create(DWORD nSessionCnt, NxServerApp* pServerApp)
{
	m_pServerApp = pServerApp;
	_CreateSessionPool(nSessionCnt);

	return true;
}

void
NxSessionManager::Destroy()
{
	_DeleteSessionPool();


}

NxSession*	
NxSessionManager::Find( DWORD nUID )
{
	return m_SessionArray.at(nUID);
}

NxSession*	
NxSessionManager::FindActiveSession( DWORD nUID )
{
	SessionMapIt iter;

	iter = m_ActiveSessionMap.find( nUID );
	if( iter != m_ActiveSessionMap.end() )
		return iter->second;

	return 0;
}


NxSession*
NxSessionManager::ActiveSession( DWORD nUID )
{
	NxSession*	pSession = NULL;

	pSession = m_SessionArray.at(nUID);

	// Ȱ��ȭ ���� �ʿ� ���
	DWORD	dwUID = 0;
	dwUID = pSession->GetUID();
	m_ActiveSessionMap[dwUID] = pSession;

	return pSession;
}

bool	
NxSessionManager::InactiveSession( NxSession* pSession )
{

	SessionMapIt iter = m_ActiveSessionMap.find( pSession->GetUID() );

	if( iter != m_ActiveSessionMap.end() )
	{
		//  Active�� session�� �����Ѵ�.
		m_ActiveSessionMap.erase( iter );
		return true;
	}
	return false;
}

bool
NxSessionManager::InactiveSession(DWORD nUID)
{
	NxSession* pSession = NULL;
	pSession = Find(nUID);
	if ( pSession )
	{
		return InactiveSession(pSession);
	}
	return false;
}

void
NxSessionManager::_CreateSessionPool(DWORD nSessionCnt)
{
	m_SessionArray.resize(nSessionCnt);
	for( DWORD i = 0 ; i < nSessionCnt ; i++ )
	{
		NxSession* pSession = new NxSession(i, m_pServerApp);
		m_SessionArray[i] = pSession;
	}
}

void
NxSessionManager::_DeleteSessionPool()
{
	NxSession* pSession = NULL;

	for ( DWORD i = 0 ; i < (DWORD)m_SessionArray.size() ; i++ )
	{
		pSession = m_SessionArray.at(i);
		delete pSession;
	}
	m_SessionArray.resize(0);
	m_ActiveSessionMap.clear();

}
