#include "StdAfx.h"
#include "DNDBConnectionManager.h"
#include "DNDBConnection.h"
#include "DNUserSession.h"
#include "Util.h"
#ifdef PRE_MOD_PVPRANK
#include "DNGameDataManager.h"
#endif		//#ifdef PRE_MOD_PVPRANK

#if defined( _GAMESERVER )
extern TGameConfig g_Config;
#elif defined( _VILLAGESERVER )
extern TVillageConfig g_Config;
#endif // #if defined( _GAMESERVER )

CDNDBConnectionManager* g_pDBConnectionManager;

void CDNDBConnectionManager::Reconnect(DWORD CurTick)
{
	if (m_dwReconnectTick + 5000 < CurTick)
	{
		m_dwReconnectTick = CurTick;

		{
			ScopeLock<CSyncLock> Lock( m_ConSync );

			if (m_Connections.empty()) 
				return;

			for (int i = 0; i < (int)m_Connections.size(); i++)
			{
				CDNDBConnection* pCon = (CDNDBConnection*)m_Connections[i];
				if (pCon) 
				{
					pCon->Reconnect();
	#if defined( _US )
					pCon->KeepAlive();
	#endif // #if defined( _US )
					pCon->GetDBSID();
				}
			}
		}
#if defined(PRE_MOD_PVPRANK) && defined(_VILLAGESERVER)
		if (IsPvPRankCriteria() == false)
		{
			BYTE cThreadID;
			CDNDBConnection *pDBCon = static_cast<CDNDBConnection*>(GetDBConnection(cThreadID));
			if (g_pDataManager->GetPvPExpThreshold() <= 0)
				g_Log.Log(LogType::_ERROR, L"g_pDataManager->GetPvPExpThreshold() <= 0!!!\n");

			if (pDBCon && g_pDataManager->GetPvPExpThreshold() > 0)
			{
				pDBCon->QuerySetPvPRankCriteria(cThreadID, g_pDataManager->GetPvPExpThreshold());
				g_Log.Log(LogType::_NORMAL, L"QuerySetPvPRankCriteria!!! threshold[%d]\n", g_pDataManager->GetPvPExpThreshold());
			}
		}
#endif		//#if defined(PRE_MOD_PVPRANK) && defined(_VILLAGESERVER)
	}
}

CConnection* CDNDBConnectionManager::AddConnection(const char *pIp, const USHORT nPort)
{
	CDNDBConnection *pDBCon = new CDNDBConnection;
	if (!pDBCon) return NULL;

	pDBCon->SetSessionID(g_IDGenerator.GetDBConnectionID());
	pDBCon->SetIp(pIp);
	pDBCon->SetPort(nPort);

	PushConnection(pDBCon);
#if defined( PRE_FIX_67546 )
	TDBConnectionInfo ConnectionInfo ={0};
	m_mapConnectionDBInfo.insert( std::make_pair(pDBCon->GetSessionID(), ConnectionInfo) );	
#endif

	return pDBCon;
}

CDNDBConnectionManager::CDNDBConnectionManager(void): CConnectionManager(), m_LastUseDBID(0), m_dwReconnectTick(0)
#ifdef PRE_MOD_PVPRANK
, m_dwUpdatePvPRankCriteria(0), m_bUpdatedPvPRankCriteria(false)
#endif		//#ifdef PRE_MOD_PVPRANK
{
#if defined( PRE_FIX_67546 )
	m_mapChannelDBInfo.clear();
	m_mapConnectionDBInfo.clear();
#endif
}

CDNDBConnection* CDNDBConnectionManager::GetDBConnection( UINT uiRandomSeed, BYTE& cThreadID )
{	
	if (m_Connections.empty()) return NULL;

	ScopeLock<CSyncLock> Lock( m_ConSync );

	srand( uiRandomSeed );
	UINT uiConnectionIdx	= rand()%g_Config.nDBCount;
	cThreadID				= rand()%THREADMAX;
	
	if( m_Connections.size() <= uiConnectionIdx )
	{
		uiConnectionIdx = 0;
	}

	return static_cast<CDNDBConnection*>(m_Connections[uiConnectionIdx]);
}

CDNDBConnection* CDNDBConnectionManager::GetDBConnection( BYTE& cThreadID )
{
	//디비컨넥션 객체가 생성 전에 호출 될 수 있습니다. 예 (AUTHMANAGER - RUN)
	if (m_Connections.empty()) return NULL;

	ScopeLock<CSyncLock> Lock( m_ConSync );

	UINT uiConnectionIdx	= rand()%g_Config.nDBCount;
	cThreadID				= rand()%THREADMAX;

	if( m_Connections.size() <= uiConnectionIdx )
	{
		uiConnectionIdx = 0;
	}

	return static_cast<CDNDBConnection*>(m_Connections[uiConnectionIdx]);
}

#if defined( PRE_FIX_67546 )
#if defined( _VILLAGESERVER )
CDNDBConnection* CDNDBConnectionManager::GetVillageDBConnection(  int nChannelID, BYTE& cThreadID  )
{
	//디비컨넥션 객체가 생성 전에 호출 될 수 있습니다. 예 (AUTHMANAGER - RUN)
	if (m_Connections.empty()) return NULL;

	ScopeLock<CSyncLock> Lock( m_ConSync );

	std::map<int,TChannelDBInfo>::iterator itor = m_mapChannelDBInfo.find(nChannelID);
	if( itor != m_mapChannelDBInfo.end() )
	{
		cThreadID = itor->second.cThreadID;
		return static_cast<CDNDBConnection*>(m_Connections[itor->second.nDBConnectionID-1]);
	}
	
	int nConnectionCount = 0;	
	BYTE cChoiceThreadID = 0;
	int nStartIndex = (rand()%g_Config.nDBCount) + 1;
	int nDBConnectionID = nStartIndex;

	bool bFirst = true;

	for( int i=0;i<m_mapConnectionDBInfo.size();i++ )
	{		
		std::map<int,TDBConnectionInfo>::iterator itor1 = m_mapConnectionDBInfo.find(nStartIndex);

		if( itor1 == m_mapConnectionDBInfo.end() )
		{
			itor1 = m_mapConnectionDBInfo.begin();
			nStartIndex = itor1->first;			
		}

		if( itor1 != m_mapConnectionDBInfo.end() )
		{
			for( int j=0;j<THREADMAX;j++)
			{
				if( bFirst )
				{
					bFirst =false;
					nConnectionCount = itor1->second.nThreadCount[j];					
				}
				if( itor1->second.nThreadCount[j] < nConnectionCount )
				{
					nConnectionCount = itor1->second.nThreadCount[j];
					nDBConnectionID = itor1->first;
					cChoiceThreadID = j;					
				}				
			}
		}
		nStartIndex++;
	}	

	if( nDBConnectionID < 1 || m_Connections.size() < nDBConnectionID )
	{
		nDBConnectionID = 1;
	}

	TChannelDBInfo ChannelDBInfo={0};
	ChannelDBInfo.nDBConnectionID = nDBConnectionID;
	ChannelDBInfo.cThreadID = cChoiceThreadID;
	cThreadID = cChoiceThreadID;

	std::map<int,TDBConnectionInfo>::iterator ii = m_mapConnectionDBInfo.find(nDBConnectionID);
	if( ii != m_mapConnectionDBInfo.end() )
	{
		ii->second.nThreadCount[cChoiceThreadID]++;
	}

	m_mapChannelDBInfo.insert( std::make_pair(nChannelID, ChannelDBInfo) );

	CDNDBConnection* pDBCon = static_cast<CDNDBConnection*>(m_Connections[nDBConnectionID-1]);
	if( pDBCon && pDBCon->GetActive() )
	{
		pDBCon->QueryAddThreadCount(cChoiceThreadID);
	}	

	g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0,0,0, L"GetVillageDBConnection ChannelID:%d DBConnectionID:%d ThreadID:%d\n", nChannelID, nDBConnectionID, cChoiceThreadID );

	return static_cast<CDNDBConnection*>(m_Connections[nDBConnectionID-1]);
}

void CDNDBConnectionManager::AddVillageDBConnectionInfo( int nSessionID, int* nCount )
{
	std::map<int,TDBConnectionInfo>::iterator itor = m_mapConnectionDBInfo.find(nSessionID);
	if( itor != m_mapConnectionDBInfo.end() )
	{
		memcpy(itor->second.nThreadCount, nCount, sizeof(itor->second.nThreadCount));
	}
}
#endif
#endif

void CDNDBConnectionManager::SendResetErrorCheck( UINT uiAccountDBID )
{
	ScopeLock<CSyncLock> Lock( m_ConSync );

	if (m_Connections.empty()) 
		return;

	for (int i = 0; i < (int)m_Connections.size(); i++)
	{
		CDNDBConnection* pCon = (CDNDBConnection*)m_Connections[i];
		if (pCon) 		
			pCon->QueryResetErrorCheck(uiAccountDBID);		
	}
}

