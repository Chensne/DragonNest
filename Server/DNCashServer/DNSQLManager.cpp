#include "StdAfx.h"
#include "DNSQLManager.h"
#include "DNExtManager.h"
#include "Log.h"

extern TCashConfig g_Config;

CDNSQLManager *g_pSQLManager = NULL;

CDNSQLManager::CDNSQLManager(void)
{
	Clear();
}

CDNSQLManager::~CDNSQLManager(void)
{
	DestroyDB();
}

void CDNSQLManager::Clear()
{
	memset(&m_pMembershipDBArray, 0, sizeof(m_pMembershipDBArray));
	memset(&m_pWorldDBArray, 0, sizeof(m_pWorldDBArray));
}

bool CDNSQLManager::CreateMembershipDB()
{
	CDNSQLMembership *pDB = NULL;
	for (int i = 0; i < g_Config.nThreadMax; i++){
		pDB = new CDNSQLMembership;

		m_pMembershipDBArray[i] = pDB;
		m_pMembershipDBArray[i]->m_cThreadID = i;
#if defined(_KRAZ)
		if (!g_Config.bTestServer)
			m_pMembershipDBArray[i]->SetMembershipDB();
#endif	// #if defined(_KRAZ)

		if (pDB->Connect(g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID) < 0){
			g_Log.Log(LogType::_FILEDBLOG, L"Not Connect MembershipDB Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
			SAFE_DELETE(pDB);
			return false;
		}

		Sleep(1);
	}

	return true;	
}

bool CDNSQLManager::CreateWorldDB()
{
	int nWorldSetID = 0;
	CDNSQLWorld *pDB = NULL;
	for (int j = 0; j < g_Config.nWorldDBCount; j++){
		for (int i = 0; i < g_Config.nThreadMax; i++){
			nWorldSetID = g_Config.WorldDB[j].nWorldSetID;
			pDB = new CDNSQLWorld;

			m_pWorldDBArray[i][nWorldSetID - 1] = pDB;
			m_pWorldDBArray[i][nWorldSetID - 1]->m_cThreadID = i;
			m_pWorldDBArray[i][nWorldSetID - 1]->m_nWorldSetID = nWorldSetID;
#if defined(_KRAZ)
			if (!g_Config.bTestServer)
				m_pWorldDBArray[i][nWorldSetID - 1]->SetWorldDB();
#endif	// #if defined(_KRAZ)

			if (pDB->Connect(g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID) < 0){
				g_Log.Log(LogType::_FILEDBLOG, L"Not Connect WorldDB Ip:%S Port:%d DBName:%s, DBID:%s \r\n", g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID);
				SAFE_DELETE(pDB);
				return false;
			}

			Sleep(100);
		}
	}

	return true;
}

bool CDNSQLManager::CreateDB()
{
	if (!CreateMembershipDB()) return false;
	if (!CreateWorldDB()) return false;

	// 캐쉬리스트 판매중지 있는지 디비에서 체크
	g_pExtManager->SetProhibitSaleList();

	return true;
}

void CDNSQLManager::DestroyDB()
{
	int i, j;

	for (i = 0; i < g_Config.nThreadMax; i++){
		SAFE_DELETE(m_pMembershipDBArray[i]);
	}

	for (i = 0; i < WORLDCOUNTMAX; i++){
		for (j = 0; j < g_Config.nThreadMax; j++){
			SAFE_DELETE(m_pWorldDBArray[j][i]);
		}
	}

	Clear();
}

CDNSQLMembership* CDNSQLManager::FindMembershipDB(int nThreadIndex)
{
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;
	return m_pMembershipDBArray[nThreadIndex];
}

CDNSQLWorld* CDNSQLManager::FindWorldDB(int nThreadIndex, char cWorldSetID)
{
	if ((cWorldSetID <= 0) || (cWorldSetID > WORLDCOUNTMAX)) return NULL;
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;

	return m_pWorldDBArray[nThreadIndex][cWorldSetID - 1];
}

CDNSQLWorld* CDNSQLManager::FindWorldDB(int nThreadIndex)
{
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;
	return m_pWorldDBArray[nThreadIndex][0];
}


