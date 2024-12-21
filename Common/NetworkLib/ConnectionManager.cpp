#include "StdAfx.h"
#include "ConnectionManager.h"
#include "Log.h"

#if defined(_SERVER)

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CConnectionManager::CConnectionManager(void): m_nProcessCount(0), m_ConSync()
{
	Clear();
}

#ifdef _USE_ACCEPTEX
CConnectionManager::CConnectionManager( UINT uiWorkerThreadCount ): m_nProcessCount(0)
{
	Clear();
}
#endif

CConnectionManager::~CConnectionManager(void)
{
	Clear();
}

void CConnectionManager::Clear()
{
	if (m_Connections.empty()) return;

	CConnection *pCon = NULL;

	for (UINT i = 0; i < m_Connections.size(); i++){
		pCon = m_Connections[i];
		SAFE_DELETE(pCon);
	}
		
	m_Connections.clear();
	m_KeyConnections.clear();
}

void CConnectionManager::DoUpdate(DWORD CurTick)
{
	if (m_Connections.empty()) return;

	ScopeLock<CSyncLock> Lock( m_ConSync );

	int Count = (int)m_Connections.size();
	if (Count > CONNECTIONPROCESSCOUNT) Count = CONNECTIONPROCESSCOUNT;

	CConnection *pCon = NULL;
	for (int i = 0; i < Count; i++){
		if (m_nProcessCount >= (int)m_Connections.size())
			m_nProcessCount = 0;

		pCon = m_Connections[m_nProcessCount];
		pCon->DoUpdate(CurTick);
		//PRE_ADD_LOGSERVER_HEARTBEAT 가 켜있으면 DoUpdate 호출 후에 다른 스레드에서 pCon 이 Delete 될 수 있습니다. 주의해 주세요.
		m_nProcessCount++;
	}
}

bool CConnectionManager::DelConnection(CConnection *pCon)
{
	if (m_KeyConnections.empty() || m_Connections.empty()) return false;

	ScopeLock<CSyncLock> Lock( m_ConSync );

	TMapConnections::iterator iter = m_KeyConnections.find(pCon->GetSessionID());
	if (iter != m_KeyConnections.end()){
		m_KeyConnections.erase(iter);

		TVecConnections::iterator it = find(m_Connections.begin(), m_Connections.end(), pCon);
		if (it != m_Connections.end()){
			m_Connections.erase(it);
			return true;
		}
		else {
			for (int i = 0; i < (int)m_Connections.size(); i++){
				if (m_Connections[i]->GetSessionID() == pCon->GetSessionID()){
					g_Log.Log( LogType::_ERROR, 0, 0, 0, pCon->GetSessionID(), L"[SID:%u] [CConnectionManager::DelConnection] m_Connections invalid %x %x\r\n", pCon->GetSessionID(), m_Connections[i], pCon);
					m_Connections.erase(m_Connections.begin() + i);
					return true;
				}
			}
		}
	}

	g_Log.Log( LogType::_ERROR, 0, 0, 0, pCon->GetSessionID(), L"[SID:%u] [CConnectionManager::DelConnection] fail!\r\n", pCon->GetSessionID());
	return false;
}

int CConnectionManager::PushConnection(CConnection *pCon)
{
	if (GetConnection(pCon->GetSessionID())){
		return ERROR_GENERIC_DUPLICATESESSIONID;
	}

	ScopeLock<CSyncLock> Lock( m_ConSync );
	
	m_KeyConnections[pCon->GetSessionID()] = pCon;
	m_Connections.push_back(pCon);

	return ERROR_NONE;
}

CConnection* CConnectionManager::GetConnection(UINT nUID, bool bDoLock)
{
	if (m_KeyConnections.empty()) return NULL;

	CSyncLock* pConSync = (bDoLock)?(&m_ConSync):(NULL);
	ScopeLock<CSyncLock> Lock(pConSync);

	CConnection *pCon = NULL;
	TMapConnections::iterator iter = m_KeyConnections.find(nUID);
	if (iter != m_KeyConnections.end()){
		pCon = iter->second;
	}

	return pCon;
}

CConnection* CConnectionManager::GetConnectionByIPPort(const char *pIp, const USHORT nPort)
{
	if (m_Connections.empty()) return NULL;

	ScopeLock<CSyncLock> Lock( m_ConSync );

	CConnection *pCon = NULL;

	for (int i = 0; i < (int)m_Connections.size(); i++){
		pCon = m_Connections[i];
		if ((pCon->GetIp() == pIp) && (pCon->GetPort() == nPort)){
			return pCon;
		}
	}

	return NULL;
}

UINT CConnectionManager::GetCount(bool bDoLock)
{
	CSyncLock* pConSync = (bDoLock)?(&m_ConSync):(NULL);

	ScopeLock<CSyncLock> Lock(pConSync);

	return static_cast<UINT>(m_Connections.size());
}

#endif	// #if defined(_SERVER)