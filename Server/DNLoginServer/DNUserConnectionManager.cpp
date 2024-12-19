#include "StdAfx.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#include "DNIocpManager.h"
#include "Util.h"
#include "Log.h"
#if defined(_TW)
#include "DNGamaniaAuth.h"
#endif	// _TW

CDNUserConnectionManager* g_pUserConnectionManager;

CDNUserConnectionManager::CDNUserConnectionManager(void)
{
#if defined(PRE_ADD_LOGIN_USERCOUNT)
	m_TotalUserCount = 0;
#endif
}

CDNUserConnectionManager::~CDNUserConnectionManager(void)
{
}

CConnection* CDNUserConnectionManager::AddConnection(const char *pIp, const USHORT nPort)
{
	CDNUserConnection *pUserCon = new CDNUserConnection;
	if (!pUserCon) return NULL;

	pUserCon->SetIp(pIp);
	pUserCon->SetPort(nPort);
	pUserCon->SetSessionID(g_IDGenerator.GetUserConnectionID());

	PushConnection(pUserCon);

	return pUserCon;
}

#ifdef _USE_ACCEPTEX

// 상위에서 동기화 해줌
CConnection* CDNUserConnectionManager::GetConnection(UINT nUID)
{
	DN_ASSERT(m_ConSync.IsLock(),	"Not Locked!");

	if (m_KeyConnections.empty()) 
		return NULL;

	TMapConnections::iterator iter = m_KeyConnections.find(nUID);
	if( iter != m_KeyConnections.end())
		return iter->second;

	return NULL;
}

// 상위에서 동기화 해줌
CConnection* CDNUserConnectionManager::GetConnectionByIPPort(const char *pIp, const USHORT nPort)
{
	DN_ASSERT(m_ConSync.IsLock(),	"Not Locked!");

	if (m_Connections.empty()) 
		return NULL;

	for( UINT i=0 ; i<m_Connections.size() ; ++i )
	{
		CConnection* pCon = m_Connections[i];
		if( (pCon->GetIp() == pIp) && (pCon->GetPort() == nPort))
			return pCon;
	}

	return NULL;
}

// 상위에서 동기화 해줌
CDNUserConnection* CDNUserConnectionManager::GetConnectionByAccountDBID(UINT nAccountDBID)
{
	DN_ASSERT(m_ConSync.IsLock(),	"Not Locked!");

	if (m_Connections.empty()) return NULL;

	for( UINT i=0 ; i<m_Connections.size() ; ++i )
	{
		CDNUserConnection* pCon = (CDNUserConnection*)m_Connections[i];
		if (pCon->GetAccountDBID() == nAccountDBID) return pCon;
	}

	return NULL;
}

void CDNUserConnectionManager::DetachUserByWorldID(int nWorldID)
{
	if (nWorldID <= 0)
	{
		_DANGER_POINT();
		return;
	}

	if (m_Connections.empty()) return ;

	for( UINT i=0 ; i<m_Connections.size() ; ++i )
	{
		CDNUserConnection* pCon = (CDNUserConnection*)m_Connections[i];
		if (pCon->GetWorldID() == nWorldID || pCon->GetWaitWorldID() == nWorldID)
			pCon->DetachConnection(L"DetachUserByWorldID");
	}
}

void CDNUserConnectionManager::DetachUserByWorldSetID(int nWorldSetID)
{
	if (nWorldSetID <= 0)
	{
		_DANGER_POINT();
		return;
	}

	if (m_Connections.empty()) return ;

	for( UINT i=0 ; i<m_Connections.size() ; ++i )
	{
		CDNUserConnection* pCon = (CDNUserConnection*)m_Connections[i];
		if (pCon->GetWorldSetID() == nWorldSetID || pCon->GetWaitWorldSetID() == nWorldSetID)
			pCon->DetachConnection(L"DetachUserByWorldSetID");
	}
}

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
void CDNUserConnectionManager::DetachUserByIP(UINT nAccountDBID, const char * pszIP)
{
	if (pszIP == NULL)
		return;

	if (m_Connections.empty()) return;

	for( UINT i=0 ; i<m_Connections.size() ; ++i )
	{
		CDNUserConnection* pCon = (CDNUserConnection*)m_Connections[i];
		if (pCon == NULL || pCon->GetAccountDBID() == nAccountDBID) continue;
		if (!stricmp(pCon->GetIp(), pszIP))
			pCon->DetachConnection(L"DetachUserByIP");
	}
}
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#if defined(_TW) && defined(_FINAL_BUILD)
void CDNUserConnectionManager::SendAllLogOutTW()
{
	DN_ASSERT(m_ConSync.IsLock(),	"Not Locked!");

	if (m_Connections.empty()) return;

	for( UINT i=0 ; i<m_Connections.size() ; ++i )
	{
		CDNUserConnection* pCon = (CDNUserConnection*)m_Connections[i];
		if( pCon->GetAccountDBID() != 0 && wcslen(pCon->GetAccountName()) != 0)
		{			
			USES_CONVERSION;
			ScopeLock<CSyncLock> Lock(pCon->m_SendLogOutLock);
			if( !pCon->m_bSendLogOut )
			{
				int iResult = g_pGamaniaAuthLogOut->SendLogout(W2A(pCon->GetAccountName()), pCon->GetIp());
				if (0 >= iResult) {
					// 오류
				}
				pCon->m_bSendLogOut = true;
				g_Log.Log(LogType::_FILELOG, L"[GASH] LogOut SendAllLogOutTW %s, %d\r\n", pCon->GetAccountName(), pCon->GetSessionID());
			}
		}
	}
}
#endif	//#if defined(_TW)


#endif	// _USE_ACCEPTEX

