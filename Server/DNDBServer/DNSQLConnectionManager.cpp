#include "StdAfx.h"
#include "DNSQLConnectionManager.h"
#include "Log.h"
#include "DNExtManager.h"
#include <iostream>

extern TDBConfig g_Config;

CDNSQLConnectionManager g_SQLConnectionManager;
CDNSQLConnectionManager::CDNSQLConnectionManager(void)
{
	Clear();
}

CDNSQLConnectionManager::~CDNSQLConnectionManager(void)
{
	DestroyDB();
}

void CDNSQLConnectionManager::Clear()
{
	memset(&m_pMembershipArray, 0, sizeof(m_pMembershipArray));
	memset(&m_pWorldArray, 0, sizeof(m_pWorldArray));
#if defined(_KRAZ) && defined(_FINAL_BUILD)
	memset(&m_pActozCommonArray, 0, sizeof(m_pActozCommonArray));
#endif	// #if defined(_KRAZ)
}

bool CDNSQLConnectionManager::CreateMembership()
{
	CDNSQLMembership *pDB = NULL;
	for (int i = 0; i < g_Config.nThreadMax; i++){
		pDB = new CDNSQLMembership;

		m_pMembershipArray[i] = pDB;
		m_pMembershipArray[i]->m_cThreadID = i;
#if defined(_KRAZ)
		if (!g_Config.bTestServer)
			m_pMembershipArray[i]->SetMembershipDB();
#endif	// #if defined(_KRAZ)

		for (int nTryCnt = 0; nTryCnt < 5; nTryCnt++)
		{
			if (pDB->Connect(g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID) < 0)
			{
				g_Log.Log(LogType::_FILELOG, L"Not Connect MembershipDB Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
				Sleep(500);
				continue;
			}
			break;
		}

		if( i==0 )
		{
			TDatabaseVersion Version;
			int iRet = pDB->QueryGetDatabaseVersion( &Version );
			if( iRet == ERROR_NONE )
			{
				std::wcout << L"DBName:" << Version.wszName << L" Version:" << Version.iVersion << std::endl;
				g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Success!! Ip:%S Port:%d DBName:%s DBID:%s Version:%s,%d\r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID, Version.wszName, Version.iVersion );
			}
			else
			{
				g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Failed!! Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
				return false;
			}
		}

		Sleep(1);
	}

	return true;	
}

bool CDNSQLConnectionManager::CreateWorld()
{
	int nWorldSetID = 0;
	CDNSQLWorld *pDB = NULL;
	for (int j = 0; j < g_Config.nWorldDBCount; j++){
		for (int i = 0; i < g_Config.nThreadMax; i++){
			nWorldSetID = g_Config.WorldDB[j].nWorldSetID;
			pDB = new CDNSQLWorld;

			m_pWorldArray[i][nWorldSetID - 1] = pDB;
			m_pWorldArray[i][nWorldSetID - 1]->m_cThreadID = i;
			m_pWorldArray[i][nWorldSetID - 1]->m_nWorldSetID = nWorldSetID;
#if defined(_KRAZ)
			if (!g_Config.bTestServer)
				m_pWorldArray[i][nWorldSetID - 1]->SetWorldDB();
#endif	// #if defined(_KRAZ)

			for (int nTryCnt = 0; nTryCnt < 5; nTryCnt++)
			{
				if (pDB->Connect(g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID) < 0)
				{
					g_Log.Log(LogType::_FILELOG, L"Not Connect WorldDB Ip:%S Port:%d DBName:%s, DBID:%s \r\n", g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID);
					Sleep(500);
					continue;
				}
				break;
			}

			if( i==0 )
			{
				TDatabaseVersion Version;
				int iRet = pDB->QueryGetDatabaseVersion( &Version );
				if( iRet == ERROR_NONE )
				{
					std::wcout << L"WorldID[" << j << L"] " << L"DBName:" << Version.wszName << L" Version:" << Version.iVersion << std::endl;
					g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Success!! WorldDB Ip:%S Port:%d DBName:%s, DBID:%s Version:%s,%d\r\n", g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID, Version.wszName, Version.iVersion );
				}
				else
				{
					g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Failed!! WorldDB Ip:%S Port:%d DBName:%s, DBID:%s \r\n", g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID);
					return false;
				}
			}

			// g_Log.Log(_SUCCESS, L">> [T:%d, WorldID:%d] Connected to SQLSERVER (%s:%s)\r\n", i, j + 1, g_Config.World[j].wszDSN, g_Config.World[j].wszDBName);

			Sleep(1);
		}
	}

	return true;
}

#if defined(_KRAZ) && defined(_FINAL_BUILD)
bool CDNSQLConnectionManager::CreateActozCommon()
{
	CDNSQLActozCommon *pDB = NULL;
	for (int i = 0; i < g_Config.nThreadMax; i++){
		pDB = new CDNSQLActozCommon;

		m_pActozCommonArray[i] = pDB;
		m_pActozCommonArray[i]->m_cThreadID = i;
		if (g_Config.bTestServer)
			m_pActozCommonArray[i]->SetActozCommonTestDB();
		else
			m_pActozCommonArray[i]->SetActozCommonDB();

		for (int nTryCnt = 0; nTryCnt < 5; nTryCnt++)
		{
			if (pDB->Connect(g_Config.ActozCommonDB.szIP, g_Config.ActozCommonDB.nPort, g_Config.ActozCommonDB.wszDBName, g_Config.ActozCommonDB.wszDBID) < 0)
			{
				g_Log.Log(LogType::_FILELOG, L"Not Connect ActozCommonDB Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.ActozCommonDB.szIP, g_Config.ActozCommonDB.nPort, g_Config.ActozCommonDB.wszDBName, g_Config.ActozCommonDB.wszDBID);
				Sleep(500);
				continue;
			}
			break;
		}

		Sleep(1);
	}

	return true;
}
#endif	// #if defined(_KRAZ)

bool CDNSQLConnectionManager::CreateDB()
{
	if (!CreateMembership()) return false;
	if (!CreateWorld()) return false;
#if defined(_KRAZ) && defined(_FINAL_BUILD)
	if (!CreateActozCommon()) return false;
#endif	// #if defined(_KRAZ)

	// 캐쉬리스트 판매중지 있는지 디비에서 체크
	g_pExtManager->SetProhibitSaleList();

	return true;
}

void CDNSQLConnectionManager::DestroyDB()
{
	int i, j;

	for (i = 0; i < g_Config.nThreadMax; i++){
		SAFE_DELETE(m_pMembershipArray[i]);
#if defined(_KRAZ) && defined(_FINAL_BUILD)
		SAFE_DELETE(m_pActozCommonArray[i]);
#endif	// #if defined(_KRAZ)
	}

	for (i = 0; i < WORLDCOUNTMAX; i++){
		for (j = 0; j < g_Config.nThreadMax; j++){
			SAFE_DELETE(m_pWorldArray[j][i]);
		}
	}

	Clear();
}

CDNSQLMembership* CDNSQLConnectionManager::FindMembershipDB(int nThreadIndex)
{
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;
	return m_pMembershipArray[nThreadIndex];
}

CDNSQLWorld* CDNSQLConnectionManager::FindWorldDB(int nThreadIndex, char cWorldSetID)
{
	if ((cWorldSetID <= 0) || (cWorldSetID > WORLDCOUNTMAX)) return NULL;
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;

	return m_pWorldArray[nThreadIndex][cWorldSetID - 1];
}

CDNSQLWorld* CDNSQLConnectionManager::FindWorldDB(int nThreadIndex)
{
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;
	return m_pWorldArray[nThreadIndex][0];
}

#if defined(_KRAZ) && defined(_FINAL_BUILD)
CDNSQLActozCommon* CDNSQLConnectionManager::FindActozCommonDB(int nThreadIndex)
{
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;
	return m_pActozCommonArray[nThreadIndex];
}
#endif	// #if defined(_KRAZ)

