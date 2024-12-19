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
#include "Util.h"
#include "DNLogConnection.h"
#include "DNExtManager.h"
#include "EtResourceMng.h"
#include "ServiceUtil.h"
#include "DNServerPacketDB.h"
#include "DNMessageTask.h"
//#include "../../Server/RLKT_LICENSE/license_rlkt.h"

#include <iostream>

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

TDBConfig g_Config;

bool LoadConfig(int argc, TCHAR * argv[])
{
	memset(&g_Config, 0, sizeof(TDBConfig));
	WCHAR wszBuf[128] = { 0, };
	
	//Version
	_strcpy(g_Config.szVersion, _countof(g_Config.szVersion), szDBVersion, (int)strlen(szDBVersion));
	_strcpy(g_Config.szResVersion, _countof(g_Config.szResVersion), "Unknown Version", (int)strlen("Unknown Version"));

	g_Config.nThreadMax = THREADMAX;
	g_Config.bUseCmd = argc >= 2 ? IsUseCmd(argv[1]) : false;

	if (g_Config.bUseCmd)
	{
		std::wstring wstrTempConfig;
		std::string strTempConfig;

		GetFirstRightValue(L"nation", argv[1], wstrTempConfig);
		if (wstrTempConfig.size() > 0 && wstrTempConfig != L"dev")
		{
			ToMultiString(wstrTempConfig, g_Config.szResourceNation);		
			g_Log.Log(LogType::_FILELOG, L"ResourceNation String [%s]\r\n", wstrTempConfig.empty() ? L"None or Dev" : wstrTempConfig.c_str());
		}

		GetDefaultInfo(argv[1], g_Config.nManagedID, g_Config.szResourcePath, g_Config.szResVersion, g_Config.ServiceInfo.szIP, g_Config.ServiceInfo.nPort);
		g_Log.Log(LogType::_FILELOG, L"ManagedID(SID) [%d]\n", g_Config.nManagedID);
		g_Log.Log(LogType::_FILELOG, L"ResourcePath [%S] \nResourceRevision [%S]\n", g_Config.szResourcePath.c_str(), g_Config.szResVersion);
		g_Log.Log(LogType::_FILELOG, L"ServiceManager [IP:%S][Port:%d]\n", g_Config.ServiceInfo.szIP, g_Config.ServiceInfo.nPort);

		if (GetFirstRightValue(L"dbp", argv[1], wstrTempConfig))
			g_Config.nAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);
		g_Log.Log(LogType::_FILELOG, L"DBServer Port [%d]\n", g_Config.nAcceptPort);

		GetDefaultConInfo(argv[1], L"log", &g_Config.LogInfo, 1);
		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);

		if (GetFirstRightValue(L"title", argv[1], wstrTempConfig))
			SetConsoleTitleW(wstrTempConfig.c_str());

		GetMemberShipInfo(argv[1], &g_Config.MembershipDB);
		g_Log.Log(LogType::_FILELOG, L"MembershipDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s) Success\r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBID, g_Config.MembershipDB.wszDBName);

		GetWorldInfo(argv[1], g_Config.WorldDB, WORLDCOUNTMAX);
		for (int nCnt = 0; nCnt < WORLDCOUNTMAX; nCnt++)
		{
			if (g_Config.WorldDB[nCnt].nWorldSetID <= 0) continue;
			g_Log.Log(LogType::_FILELOG, L"WorldDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s) Success\r\n", g_Config.WorldDB[nCnt].szIP, g_Config.WorldDB[nCnt].nPort, g_Config.WorldDB[nCnt].wszDBID, g_Config.WorldDB[nCnt].wszDBName);
			g_Config.nWorldDBCount++;
		}
#if defined( PRE_WORLDCOMBINE_PARTY )
		int nCombineWorldID = 0;
		for( int i = 0;i<WORLDCOUNTMAX;i++ )
		{
			if (g_Config.WorldDB[i].nWorldSetID > 0)
			{
				if( nCombineWorldID == 0 )
					nCombineWorldID = g_Config.WorldDB[i].nWorldSetID;
				else
				{
					if( nCombineWorldID > g_Config.WorldDB[i].nWorldSetID)
						nCombineWorldID = g_Config.WorldDB[i].nWorldSetID;
				}
			}
			else
				break;
		}
		
		g_Config.nCombineWorldDBID = nCombineWorldID;

		if( g_Config.nCombineWorldDBID <= 0 )
			g_Log.Log(LogType::_FILELOG, L"월드통합 관련 DBID가 설정되었는지 확인해주세요. 설정하지 않으면 제대로 이용할 수가 없습니다.\r\n");
		else
			g_Log.Log(LogType::_FILELOG, L"CombineWorld = %d\r\n", g_Config.nCombineWorldDBID);
#endif
	}
	else
	{
		if (!g_IniFile.Open(L"./Config/DNDB.ini")){
			g_Log.Log(LogType::_FILELOG, L"DNDB.ini File not Found!!\r\n");
			return false;
		}
		g_IniFile.GetValue(L"Info", L"ThreadMax", &g_Config.nThreadMax);
		g_IniFile.GetValue(L"Connection", L"AcceptPort", &g_Config.nAcceptPort);

		WCHAR wszBuf[128] = { 0, };
		
		g_IniFile.GetValue(L"DB_DNMembership", L"DBIP", wszBuf);
		if (wszBuf[0] != '\0')
			WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.MembershipDB.szIP, sizeof(g_Config.MembershipDB.szIP), NULL, NULL);
		g_IniFile.GetValue(L"DB_DNMembership", L"DBPort", &g_Config.MembershipDB.nPort);
		g_IniFile.GetValue(L"DB_DNMembership", L"DBID", g_Config.MembershipDB.wszDBID);
		g_IniFile.GetValue(L"DB_DNMembership", L"DBName", g_Config.MembershipDB.wszDBName);

		g_IniFile.GetValue(L"DB_DNWorld", L"WorldDBCount", &g_Config.nWorldDBCount);

#if defined( PRE_WORLDCOMBINE_PARTY )
		g_IniFile.GetValue(L"DB_DNWorld", L"CombineWorldDB", &g_Config.nCombineWorldDBID);
		if( g_Config.nCombineWorldDBID <= 0 )
			g_Log.Log(LogType::_FILELOG, L"월드통합 관련 DBID가 설정되었는지 확인해주세요. 설정하지 않으면 제대로 이용할 수가 없습니다.\r\n");
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )

		WCHAR wszLogStr[128] = {0,};	
		char szData[MAX_PATH];

		std::vector<std::string> Tokens;

		g_IniFile.GetValue(L"Connection", L"Log", wszLogStr);
		if (wszLogStr[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszLogStr, -1, szData, sizeof(szData), NULL, NULL);

			Tokens.clear();
			TokenizeA(szData, Tokens, ":");
			if (!Tokens.empty()){
				_strcpy(g_Config.LogInfo.szIP, _countof(g_Config.LogInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
				g_Config.LogInfo.nPort = atoi(Tokens[1].c_str());
			}
		}

		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);		

		WCHAR wszData[128] = { 0, };
		for (int i = 0; i < g_Config.nWorldDBCount; i++){
			swprintf(wszData, L"WorldID%d", i + 1);
			g_IniFile.GetValue(L"DB_DNWorld", wszData, &g_Config.WorldDB[i].nWorldSetID);
			if (g_Config.WorldDB[i].nWorldSetID <= 0) continue;

			swprintf(wszData, L"DBIP%d", i + 1);
			g_IniFile.GetValue(L"DB_DNWorld", wszData, wszBuf);
			if (wszBuf[0] != '\0')
				WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.WorldDB[i].szIP, sizeof(g_Config.WorldDB[i].szIP), NULL, NULL);
			swprintf(wszData, L"DBPort%d", i + 1);
			g_IniFile.GetValue(L"DB_DNWorld", wszData, &g_Config.WorldDB[i].nPort);
			swprintf(wszData, L"DBID%d", i + 1);
			g_IniFile.GetValue(L"DB_DNWorld", wszData, g_Config.WorldDB[i].wszDBID);
			swprintf(wszData, L"DBName%d", i + 1);
			g_IniFile.GetValue(L"DB_DNWorld", wszData, g_Config.WorldDB[i].wszDBName);
		}

		// ResourcePath 등록해준다.
		WCHAR wszPath[_MAX_PATH] = { 0, };
		char szPath[_MAX_PATH] = { 0, };

		g_IniFile.GetValue( L"Resource", L"Path", wszPath );
		if (wszPath[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszPath, -1, szPath, sizeof(szPath), NULL, NULL);
			g_Config.szResourcePath = szPath;
			if( g_Config.szResourcePath.empty() ) g_Config.szResourcePath = ".";
		}

		WCHAR wszResNation[64] = { 0, };
		char szResNation[64] = { 0, };
		g_IniFile.GetValue( L"Resource", L"Nation", wszResNation );
		if (wszResNation[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszResNation, -1, szResNation, sizeof(szResNation), NULL, NULL);
			if( strlen(szResNation) > 0 ) {
				g_Config.szResourceNation = "_";
				g_Config.szResourceNation += szResNation;
			}
		}
	}

	//나라마다 틀리고 고정 값은 공통으로 config에서 읽는다.
	if (g_Config.nThreadMax > THREADMAX) g_Config.nThreadMax = THREADMAX;

	// 미들웨어 쓰레드 인자값과 상관없이 코드에 박음.
	g_Config.nThreadMax = THREADMAX;

#if defined(_KRAZ)
	if (!g_IniFile.Open(L"./Config/ActozCommonDB.ini")){
		g_Log.Log(LogType::_FILELOG, L"ActozCommonDB.ini File not Found!!\r\n");
		return false;
	}

	memset(&wszBuf, 0, sizeof(wszBuf));
	g_IniFile.GetValue(L"Info", L"ServerType", wszBuf);
	if (wcscmp(wszBuf, L"Test") == 0)
		g_Config.bTestServer = true;

	g_IniFile.GetValue(L"Info", L"DBIP", wszBuf);
	if (wszBuf[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.ActozCommonDB.szIP, sizeof(g_Config.ActozCommonDB.szIP), NULL, NULL);
	g_IniFile.GetValue(L"Info", L"DBPort", &g_Config.ActozCommonDB.nPort);
	g_IniFile.GetValue(L"Info", L"DBID", g_Config.ActozCommonDB.wszDBID);
	g_IniFile.GetValue(L"Info", L"DBName", g_Config.ActozCommonDB.wszDBName);

	g_Log.Log(LogType::_FILELOG, L"ActozCommonDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s) Success\r\n", g_Config.ActozCommonDB.szIP, g_Config.ActozCommonDB.nPort, g_Config.ActozCommonDB.wszDBID, g_Config.ActozCommonDB.wszDBName);
#endif	// #if defined(_KRAZ)

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
			swprintf(wszLogName, L"DBServer_%d", nSID);
		}
		else
			swprintf(wszLogName, L"DBServer");
	}
	else
		swprintf(wszLogName, L"DBServer");

	//g_Log.Init(L"DBServer", LOGTYPE_CRT_FILE_DAY);
#if defined (_FINAL_BUILD)
	g_Log.Init(wszLogName, LOGTYPE_FILEDB_DAY);
#else
	g_Log.Init(wszLogName, LOGTYPE_CRT_FILE_DAY);
#endif
	
	WCHAR wszSPLogName[128];
	memset(wszSPLogName, 0, sizeof(wszSPLogName));
	swprintf(wszSPLogName, L"%s_SP", wszLogName);
	g_SPLog.Init(wszSPLogName, LOGTYPE_FILE);	

	if (!LoadConfig(argc, argv)){
		g_Log.Log(LogType::_FILELOG, L"LoadConfig Failed\r\n");
		return false;
	}

	g_Log.SetServerID(g_Config.nManagedID);

	// ResourceMng 생성
	CEtResourceMng::CreateInstance();

	// Path 설정
	std::string szResource = g_Config.szResourcePath + "\\Resource";
	// 국가별 셋팅
	std::string szNationStr;
	if( szNationStr.empty() && !g_Config.szResourceNation.empty() ) szNationStr = g_Config.szResourceNation;
	if( !szNationStr.empty() ) 
	{
		szResource += szNationStr;
		CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
		szResource = g_Config.szResourcePath + "\\Resource";
	}

	CEtResourceMng::GetInstance().AddResourcePath( szResource, true );

	g_pSPErrorCheckManager = new CDNSPErrorCheckManager;
	if( !g_pSPErrorCheckManager )
		return false;

	g_pIocpManager = new CDNIocpManager;
	if (!g_pIocpManager) return false;

#if defined(_WORK)
	if (g_pIocpManager->Init(500) < 0){
#else	// #if defined(_WORK)
	if (g_pIocpManager->Init(100) < 0){
#endif	// #if defined(_WORK)
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize Fail(%d)\r\n", 100);
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## Iocp Initialize Success(%d)\r\n", 100);
	}

	g_pExtManager = new CDNExtManager;
	if (!g_pExtManager) return false;
	if (!g_pExtManager->AllLoad())
	{
		g_Log.Log(LogType::_FILELOG, L"CDNExtManager::AllLoad Fail\r\n");
		return false;
	}

	if (!g_ThreadManager.Init(g_Config.nThreadMax)){
		g_Log.Log(LogType::_FILELOG, L"ThreadManager Initialize Fail\r\n");
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## ThreadManager Initialize Success(%d)\r\n", g_Config.nThreadMax);
	}

	g_pLogConnection = new CDNLogConnection;
	if( !g_pLogConnection ) return false;
	g_pLogConnection->SetIp(g_Config.LogInfo.szIP);
	g_pLogConnection->SetPort(g_Config.LogInfo.nPort);

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

	if (argc > 1 && g_Config.nManagedID > 0)
	{
		g_pIocpManager->CreateThread();
		g_pServiceConnection = new CDNServiceConnection(g_Config.nManagedID);
		if (!g_pServiceConnection)
			return false;
		g_pServiceConnection->SetIp(g_Config.ServiceInfo.szIP);
		g_pServiceConnection->SetPort(g_Config.ServiceInfo.nPort);
	}
	else
	{
		g_pIocpManager->CreateThread();
	}

#if defined(_KRAZ) && defined(_FINAL_BUILD)
	g_pActozShield = new CDNActozShield;
	if (!g_pActozShield)
		return false;
	if (!g_pActozShield->Init())
		return false;
#endif	// #if defined(_KRAZ)

	g_Log.Log(LogType::_FILELOG, L"Application Initialize Success\r\n");

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
	g_ThreadManager.Final();
	if( g_pIocpManager )
	{
		g_pIocpManager->CloseAcceptors();
		g_pIocpManager->ThreadStop();
	}	
	SAFE_DELETE(g_pServiceConnection);
	SAFE_DELETE(g_pIocpManager);
	SAFE_DELETE(g_pSPErrorCheckManager);
}

int _tmain(int argc, TCHAR* argv[])
{
#if defined( PRE_PARTY_DB )
	// PartyList 를 쪼개서 보내는데 한 패킷의 최대사이즈가 Network 상에서 보낼수 있는 최대 크기가 넘어가는지 컴파일타임시 검사한다.
	BOOST_STATIC_ASSERT( (40*1024) >= sizeof(TAGetListParty) );
#endif // #if defined( PRE_PARTY_DB )

	// DB에 KeySetting 저장할 수 있는 바이너리 크기가 59바이트로 고정되어 있으므로 
	// 지정된 크기보다 커지면 컴파일 에러를 내버린다.
#if defined(PRE_ADD_SKILLSLOT_EXTEND)
	BOOST_STATIC_ASSERT( 79 >= sizeof(TKeySetting) );
	BOOST_STATIC_ASSERT( 201 >= sizeof(TPadSetting) );
#else	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)
	BOOST_STATIC_ASSERT( 69 >= sizeof(TKeySetting) );
	BOOST_STATIC_ASSERT( 161 >= sizeof(TPadSetting) );
#endif	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)

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
		g_Log.Log(LogType::_FILELOG, L"** InitApp Failed!!! Check!\r\n");		
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

		if (strcmp(szCmd, "exit") == 0)	
			break;		

		if (strcmp(szCmd, "onsp") == 0)
			g_SPLog.SetActive(true);

		if (strcmp(szCmd, "offsp") == 0)
			g_SPLog.SetActive(false);

		printf("CMD>");


        std::cin >> szCmd;
	}

	ClearApp();

	return 0;
}

