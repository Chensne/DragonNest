// DNDBServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IniFile.h"
#include "DNIocpManager.h"
#include "DNThreadManager.h"
#include "DNSQLConnectionManager.h"
#include "DNConfig.h"
#include "Log.h"
#include "ExceptionReport.h"
#include "DnServiceConnection.h"
#include "Version.h"
#if !defined( _FINAL_BUILD )
#include "DNQueryTest.h"
#endif // #if !defined( _FINAL_BUILD )
#include "ServiceUtil.h"
#include "DNConnectionManager.h"

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

#include <iostream>

//#include "..//../Server/RLKT_LICENSE/license_rlkt.h"

TLogConfig g_Config;

bool LoadConfig(int argc, TCHAR * argv[])
{
	memset(&g_Config, 0, sizeof(TLogConfig));
	WCHAR wszBuf[128] = { 0, };

	//Version
	strcpy_s(g_Config.szVersion, szLogVersion);
	strcpy_s(g_Config.szResVersion, "Unknown Version");

	g_Config.nThreadMax = THREADMAX;
	g_Config.bUseCmd = argc >= 2 ? IsUseCmd(argv[1]) : false;

	if (g_Config.bUseCmd)
	{
		GetDefaultInfo(argv[1], g_Config.nManagedID, g_Config.szResourcePath, g_Config.szResVersion, g_Config.ServiceInfo.szIP, g_Config.ServiceInfo.nPort);
		g_Log.Log(LogType::_FILELOG, L"ManagedID(SID) [%d]\n", g_Config.nManagedID);
		g_Log.Log(LogType::_FILELOG, L"ResourcePath [%S] \nResourceRevision [%S]\n", g_Config.szResourcePath.c_str(), g_Config.szResVersion);
		g_Log.Log(LogType::_FILELOG, L"ServiceManager [IP:%S][Port:%d]\n", g_Config.ServiceInfo.szIP, g_Config.ServiceInfo.nPort);

		std::wstring wstrTempConfig;
		std::string strTempConfig;

		if (GetFirstRightValue(L"lgp", argv[1], wstrTempConfig))
			g_Config.nAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);
		g_Log.Log(LogType::_FILELOG, L"LogServer Port [%d]\n", g_Config.nAcceptPort);

		if (GetFirstRightValue(L"title", argv[1], wstrTempConfig))
			SetConsoleTitleW(wstrTempConfig.c_str());

		GetLogdbInfo(argv[1], &g_Config.ServerLogDB);
		g_Log.Log(LogType::_FILELOG, L"ServerLogDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s) Success\r\n", g_Config.ServerLogDB.szIP, g_Config.ServerLogDB.nPort, g_Config.ServerLogDB.wszDBID, g_Config.ServerLogDB.wszDBName);
	}
	else
	{
		if (!g_IniFile.Open(L"./Config/DNLog.ini")){
			g_Log.Log(LogType::_FILELOG, L"DNLog.ini File not Found!!\r\n");
			return false;
		}

		g_IniFile.GetValue(L"Info", L"ThreadMax", &g_Config.nThreadMax);
		g_IniFile.GetValue(L"Connection", L"AcceptPort", &g_Config.nAcceptPort);

		WCHAR wszBuf[128] = { 0, };

		g_IniFile.GetValue(L"MasterLogDB", L"DBIP", wszBuf);
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.MasterLogDB.szIP, sizeof(g_Config.MasterLogDB.szIP), NULL, NULL);
		g_IniFile.GetValue(L"MasterLogDB", L"DBPort", &g_Config.MasterLogDB.nPort);
		g_IniFile.GetValue(L"MasterLogDB", L"DBID", g_Config.MasterLogDB.wszDBID);
		g_IniFile.GetValue(L"MasterLogDB", L"DBName", g_Config.MasterLogDB.wszDBName);

		g_IniFile.GetValue(L"ServerLogDB", L"DBIP", wszBuf);
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.ServerLogDB.szIP, sizeof(g_Config.ServerLogDB.szIP), NULL, NULL);
		g_IniFile.GetValue(L"ServerLogDB", L"DBPort", &g_Config.ServerLogDB.nPort);
		g_IniFile.GetValue(L"ServerLogDB", L"DBID", g_Config.ServerLogDB.wszDBID);
		g_IniFile.GetValue(L"ServerLogDB", L"DBName", g_Config.ServerLogDB.wszDBName);

		g_IniFile.GetValue(L"LogDB", L"LogDBCount", &g_Config.nLogDBCount);

		int nWorldID = 0;
		WCHAR wszData[128] = { 0, };
		for (int i = 0; i < g_Config.nLogDBCount; i++){
			swprintf(wszData, L"Log%dWorldID", i + 1);
			g_IniFile.GetValue(L"LogDB", wszData, &nWorldID);
			if (nWorldID <= 0) continue;

			g_Config.LogDB[i].nWorldSetID = nWorldID;
			swprintf(wszData, L"DBIP%d", i + 1);
			g_IniFile.GetValue(L"LogDB", wszData, wszBuf);
			WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.LogDB[i].szIP, sizeof(g_Config.LogDB[i].szIP), NULL, NULL);
			swprintf(wszData, L"DBPort%d", i + 1);
			g_IniFile.GetValue(L"LogDB", wszData, &g_Config.LogDB[i].nPort);
			swprintf(wszData, L"Log%dDBID", i + 1);
			g_IniFile.GetValue(L"LogDB", wszData, g_Config.LogDB[i].wszDBID);
			swprintf(wszData, L"Log%dDBName", i + 1);
			g_IniFile.GetValue(L"LogDB", wszData, g_Config.LogDB[i].wszDBName);
		}
	}
	
	if (g_Config.nThreadMax > THREADMAX) g_Config.nThreadMax = THREADMAX;

	return true;
}

bool CheckWorkingFolder(const WCHAR* szFolderName)
{
	WCHAR szCurrentDir[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szCurrentDir);
	std::wstring wszCurrentDir;
	wszCurrentDir = szCurrentDir;
	std::transform(wszCurrentDir.begin(), wszCurrentDir.end(), wszCurrentDir.begin(), towlower); 
	if ( wszCurrentDir.find(szFolderName) > wszCurrentDir.size() )
		return false;

	return true;
}

bool InitApp(int argc, TCHAR * argv[])
{
	//if (!CheckSerial()) return false;

#if defined(_DEBUG) && defined(_WORK)
	if ( CheckWorkingFolder(L"out") == false )
	{
		wprintf(L"작업 디렉토리를 설정해주세요!!!! setworking folder error " );
	}
#endif

#if !defined( _FINAL_BUILD )
	CDNQueryTest::CreateInstance();
#endif // #if !defined( _FINAL_BUILD )

	WCHAR wszLogName[128];
	memset(wszLogName, 0, sizeof(wszLogName));

	if (argc >= 2)
	{
		if (IsUseCmd(argv[1]))
		{
			int nSID = 0;
			std::wstring wstrTempArgv;

			if (GetFirstRightValue(L"sid", argv[1], wstrTempArgv))
				nSID = _wtoi(wstrTempArgv.c_str());
			swprintf(wszLogName, L"LogServer_%d", nSID);
		}
		else
			swprintf(wszLogName, L"LogServer");
	}
	else
		swprintf(wszLogName, L"LogServer");
	
	//g_Log.Init(L"LogServer", LOGTYPE_CRT_FILE_DAY);
	g_Log.Init(wszLogName, LOGTYPE_CRT_FILE_DAY);	

	if (!LoadConfig(argc, argv)){
		g_Log.Log(LogType::_FILELOG, L"LoadConfig Failed\r\n");
		return false;
	}
	g_Log.SetServerID(g_Config.nManagedID);

	g_pConnectionManager = new CDNConnectionManager;
	if (!g_pConnectionManager) return false;

	g_pIocpManager = new CDNIocpManager;
	if (!g_pIocpManager) return false;

#if defined(_WORK)
	if (g_pIocpManager->Init(500) < 0){
#else		// #if defined(_WORK)
	if (g_pIocpManager->Init(100) < 0){
#endif		// #if defined(_WORK)
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize Fail(%d)\r\n", 100);
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## Iocp Initialize Success(%d)\r\n", 100);
	}

	if (!g_ThreadManager.Init(g_Config.nThreadMax)){
		g_Log.Log(LogType::_FILELOG, L"ThreadManager Initialize Fail\r\n");
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## ThreadManager Initialize Success(%d)\r\n", g_Config.nThreadMax);
	}

	if (!g_SQLConnectionManager.CreateDB()){
		g_Log.Log(LogType::_FILELOG, L"CreateDB Fail\r\n");
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## CreateDB OK!!\r\n");
	}

	if (g_pIocpManager->AddAcceptConnection(CONNECTIONKEY_DEFAULT, g_Config.nAcceptPort, 1000) < 0){
		g_Log.Log(LogType::_FILELOG, L"AcceptPort Fail(%d)\r\n", g_Config.nAcceptPort);
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## AcceptPort (%d)\r\n", g_Config.nAcceptPort);
	}	
	g_pIocpManager->CreateUpdateThread();

	if (argc > 1 && g_Config.nManagedID > 0)
	{
		g_pIocpManager->CreateThread();
		g_pServiceConnection = new CDNServiceConnection(g_Config.nManagedID);
		if (!g_pServiceConnection)
			return false;
		g_pServiceConnection->SetIp(g_Config.ServiceInfo.szIP);
		g_pServiceConnection->SetPort(g_Config.ServiceInfo.nPort);
	}

#if defined(_KRAZ) && defined(_FINAL_BUILD)
	g_pActozShield = new CDNActozShield;
	if (!g_pActozShield)
		return false;
	if (!g_pActozShield->Init())
		return false;
#endif	// #if defined(_KRAZ)

	g_Log.Log(LogType::_FILEDBLOG, L"Application Initialize Success\r\n");
	return true;
}

void ClearApp()
{
#if defined(_KRAZ)
	SAFE_DELETE(g_pActozShield);
#endif	// #if defined(_KRAZ)

#if !defined( _FINAL_BUILD )
	if( CDNQueryTest::IsActive() )
		CDNQueryTest::DestroyInstance();
#endif // #if !defined( _FINAL_BUILD )

	if(g_pIocpManager)
	{
		g_pIocpManager->CloseAcceptors();
		g_pIocpManager->ThreadStop();
	}
	SAFE_DELETE(g_pServiceConnection);
	SAFE_DELETE(g_pIocpManager);
}


int _tmain(int argc, TCHAR* argv[])
{
#if defined(_CH)
	setlocale(LC_ALL, "chinese-simplified");
#elif defined(_TW)
	setlocale(LC_ALL, "chinese-traditional");
#elif defined(_JP)
	setlocale(LC_ALL, "japanese");
#elif defined(_US)
	setlocale(LC_ALL, "us");
#else
	setlocale(LC_ALL, "Korean");
#endif

	// 예외 처리자 준비
	DWORD dwRetVal = CExceptionReport::GetInstancePtr()->Open(_T(".\\"), TRUE, TRUE);	// Release 모드 컴파일 시 C4744 경고가 발생하여 Singleton 구현 변경, CExceptionReport::GetInstancePtr() 을 inline 화 하지 않음 (참고 : http://msdn.microsoft.com/ko-kr/library/a7za416f.aspx)
	if (NOERROR != dwRetVal) {
		DWORD dwErrNo = ::GetLastError();
		DN_RETURN(dwErrNo);
	}

	if (!InitApp(argc, argv)){
		ClearApp();
		return 0;
	}

	wprintf(L"exit 명령을 치면 종료\r\n");

	char szCmd[256] = {0};	
	while (1)
	{
#if !defined( _FINAL_BUILD )
		CDNQueryTest::GetInstance().ProcessQueryTest( szCmd );
#endif // #if !defined( _FINAL_BUILD )

		if (strcmp(szCmd, "exit") == 0)	break;		

		printf("CMD>");
        std::cin >> szCmd;
	}

	ClearApp();

	return 0;
}