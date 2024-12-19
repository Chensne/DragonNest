// DNLoginServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// #include "FireWall.h"
#include "IniFile.h"
#include "DNIocpManager.h"
#include "DNSQLMembershipManager.h"
#include "DNSQLWorldManager.h"
#include "DNMasterConnectionManager.h"
#include "DNUserConnectionManager.h"
#include "DNExtManager.h"
#include "Log.h"
#include "Util.h"
#include "ExceptionReport.h"
#include "DNLogConnection.h"
#include "DNServiceConnection.h"
#include "Version.h"
#include "DNAuthManager.h"
#include "DNSecure.h"
#include "EtResourceMng.h"
#if !defined( _FINAL_BUILD )
#include "DNQueryTest.h"
#endif // #if !defined( _FINAL_BUILD )
#if defined(_HSHIELD)
#include "HShieldSvrWrapper.h"
#endif	// #if defined(_HSHIELD)
#include "ServiceUtil.h"

#if defined(_CH)
#include "DNShandaAuth.h"
#elif defined(_TW)
#include "DNGamaniaAuth.h"
#elif defined(_US)
#include "DNNexonPI.h"
#elif defined(_TH)
#include "DNAsiaSoftAuth.h"
#include "DNAsiaSoftOTPManager.h"
#elif defined(_ID) 
#include "HttpClientManager.h"
#include "DNKreonAuth.h"	
#elif defined(_RU)
#include "HttpClientManager.h"
#include "DNMailRUAuth.h"
#elif defined(_EU)
#include "DNEUAuth.h"
#elif defined(_KRAZ)
#include "DNActozAuth.h"
#include "DNActozShield.h"
#include "DNSQLActozCommonManager.h"
#endif	// 

#if defined(PRE_FIX_DEBUGSET_INIT)
#include "DebugSet.h"
#endif
#include <oledb.h>

TLoginConfig g_Config;
TGameOptions g_GameOption;
#ifdef PRE_ADD_BEGINNERGUILD
#include "./EtStringManager/EtUIXML.h"
CEtUIXML *g_pUIXML;
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#if defined( PRE_ADD_DWC )
#include "DNDWCChannelManager.h"
#endif // #if defined( PRE_ADD_DWC )


#include "rlkt_Revision.h"
//#include "../../Server/RLKT_LICENSE/license_rlkt.h"

//int AddToFirewall()
//{
//	HRESULT hr = S_OK;
//	HRESULT comInit = E_FAIL;
//	INetFwProfile* fwProfile = NULL;
//
//	// Initialize COM.
//	comInit = CoInitializeEx(
//		0,
//		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
//		);
//
//	// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
//	// initialized with a different mode. Since we don't care what the mode is,
//	// we'll just use the existing mode.
//	if (comInit != RPC_E_CHANGED_MODE)
//	{
//		hr = comInit;
//		if (FAILED(hr))
//		{
//			printf("CoInitializeEx failed: 0x%08lx\n", hr);
//			goto error;
//		}
//	}
//
//	// Retrieve the firewall profile currently in effect.
//	hr = WindowsFirewallInitialize(&fwProfile);
//	if (FAILED(hr))
//	{
//		printf("WindowsFirewallInitialize failed: 0x%08lx\n", hr);
//		goto error;
//	}
//
//	WCHAR wszFileName[ _MAX_PATH ];
//	HMODULE hModule;
//	hModule = GetModuleHandle( NULL );
//	GetModuleFileNameW( hModule, wszFileName, _MAX_PATH );
//	// Add Windows Messenger to the authorized application collection.
//	hr = WindowsFirewallAddApp(
//		fwProfile,
//		wszFileName,
//		L"Dragon Nest"
//		);
//	if (FAILED(hr))
//	{
//		printf("WindowsFirewallAddApp failed: 0x%08lx\n", hr);
//		goto error;
//	}
//
//error:
//
//	// Release the firewall profile.
//	WindowsFirewallCleanup(fwProfile);
//
//	// Uninitialize COM.
//	if (SUCCEEDED(comInit))
//	{
//		CoUninitialize();
//	}
//
//	return 0;
//}

void LoadUserSessionID()
{
	DN_ASSERT(0 != g_Config.nManagedID,	"Check!");

	g_IDGenerator.SetUserConnectionMask(static_cast<BYTE>(g_Config.nManagedID));

	FILE *fp = fopen(CVarArgA<MAX_PATH>("./Config/SessionID_%08d.dat", g_Config.nManagedID), "rb");

	if (!fp) return;

	UINT nSessionID = 0;
	fread(&nSessionID, 1, sizeof(UINT), fp);

	fclose(fp);

	nSessionID += DEFAULTUSERSESSIONID;	// 혹시 저장 제대로 안되서 건너뛸 수 있으므로 꽁수로 N 더해줌!
	if (0x00FFFFFF <= nSessionID) nSessionID = DEFAULTUSERSESSIONID;

	g_IDGenerator.SetUserConnectionID(nSessionID);
}

void SaveUserSessionID()
{
	FILE *fp = fopen(CVarArgA<MAX_PATH>("./Config/SessionID_%08d.dat", g_Config.nManagedID), "wb");

	if (!fp) return;

	UINT nSessionID = g_IDGenerator.GetUserConnectionID(true);

	fwrite(&nSessionID, 1, sizeof(UINT), fp);

	fclose(fp);
}

bool LoadConfig(int argc, TCHAR * argv[])
{
	memset(&g_Config, 0, sizeof(TLoginConfig));

#if defined(_JP)
	g_Config.nNation = NATION_JAPAN;
#elif defined(_CH)
	g_Config.nNation = NATION_CHINA;
#elif defined(_US)
	g_Config.nNation = NATION_USA;
#elif defined(_TW)
	g_Config.nNation = NATION_TAIWAN;
#elif defined(_SG)
	g_Config.nNation = NATION_SINGAPORE;
#elif defined(_TH)
	g_Config.nNation = NATION_THAILAND;
#elif defined(_ID)
	g_Config.nNation = NATION_INDONESIA;
#elif defined(_RU)
	g_Config.nNation = NATION_RUSSIA;
#elif defined(_EU)
	g_Config.nNation = NATION_EUROPE;
#else

#if 0
	g_Config.nNation = NATION_KOREA;
#else
	g_Config.nNation = NATION_TAIWAN;
#endif //[debug]

#endif

#if defined(_HSHIELD)
	g_Config.hHSServer = ANTICPX_INVALID_HANDLE_VALUE;
#endif	// _HSHIELD			

	WCHAR wszBuf[128] = { 0, }; //, wszStr[64] = { 0, };
	char szData[128] = { 0, }, *pStr = NULL;

	wstring wszFileName = L"./Config/DNLogin.ini";			// 한국

	if (!g_IniFile.Open(wszFileName.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszFileName.c_str());
		// return false;
	}
	
	//Version
	_strcpy(g_Config.szVersion, SERVERVERSIONMAX, szLoginVersion, (int)strlen(szLoginVersion));
	_strcpy(g_Config.szResVersion, SERVERVERSIONMAX, "Unknown Version", (int)strlen("Unknown Version"));

	std::string strCmd;
	strCmd = g_Config.szVersion;
	ToLowerA(strCmd);

	std::vector<std::string> tokens;
	TokenizeA(strCmd, tokens, ".");

	g_Config.nMajorVersion = atoi(tokens[0].c_str());
	g_Config.nMinorVersion = atoi(tokens[1].c_str());

//#if defined (_FINAL_BUILD)
	g_Config.nSQLMax = 50;
//#else
	//g_Config.nSQLMax = 8;
//#endif // #if defined (_FINAL_BUILD)

//#ifdef _USE_ACCEPTEX
//#ifdef _WORK
//	g_Config.nWorkerThreadMax = 4; //  (SysInfo.dwNumberOfProcessors * 2) + 1;
//#else
//	g_Config.nWorkerThreadMax = 50;
//#endif
//#else
//	g_Config.nWorkerThreadMax = 0;
//#endif

	g_Config.bUseCmd = argc >= 2 ? IsUseCmd(argv[1]) : false;	
	if (g_Config.bUseCmd)
	{
		g_Log.Log(LogType::_FILELOG, L"## Ver:%d, BuildNation:%d\r\n", g_Config.nVersion, g_Config.nNation);

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

		if (GetFirstRightValue(L"smc", argv[1], wstrTempConfig))
			g_Config.nIocpMax = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);
		g_Log.Log(LogType::_FILELOG, L"## SocketMax:%d\r\n", g_Config.nIocpMax);

		if (GetFirstRightValue(L"lmp", argv[1], wstrTempConfig))
			g_Config.nMasterAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

        if (GetFirstRightValue(L"lcp", argv[1], wstrTempConfig))
            g_Config.nClientAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);
		g_Log.Log(LogType::_FILELOG, L"## ClientAcceptPort:%d MasterAcceptPort:%d\r\n", g_Config.nClientAcceptPort, g_Config.nMasterAcceptPort);

		GetDefaultConInfo(argv[1], L"log", &g_Config.LogInfo, 1);
		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);

		GetMemberShipInfo(argv[1], &g_Config.MembershipDB);
		g_Log.Log(LogType::_FILELOG, L"MembershipDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s) Success\r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBID, g_Config.MembershipDB.wszDBName);

		GetWorldInfo(argv[1], g_Config.WorldDB, WORLDCOUNTMAX);
		for (int nCnt = 0; nCnt < WORLDCOUNTMAX; nCnt++)
		{
			if (g_Config.WorldDB[nCnt].nWorldSetID <= 0) continue;
			g_Log.Log(LogType::_FILELOG, L"WorldDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s) Success\r\n", g_Config.WorldDB[nCnt].szIP, g_Config.WorldDB[nCnt].nPort, g_Config.WorldDB[nCnt].wszDBID, g_Config.WorldDB[nCnt].wszDBName);
			g_Config.nWorldDBCount++;
		}
		if (GetFirstRightValue(L"title", argv[1], wstrTempConfig))
			SetConsoleTitleW(wstrTempConfig.c_str());
	}
	else
	{
		g_IniFile.GetValue(L"Info", L"Version", &g_Config.nVersion);
		g_IniFile.GetValue(L"Info", L"Nation", &g_Config.nNation);
		g_IniFile.GetValue(L"Info", L"IocpMax", &g_Config.nIocpMax);
		g_IniFile.GetValue(L"Info", L"SQLMax", &g_Config.nSQLMax);
		g_IniFile.GetValue(L"Info", L"WorkerThreadMax", &g_Config.nWorkerThreadMax);

		g_Log.Log(LogType::_FILELOG, L"## Ver:%d, Nation:%d IocpMax:%d SQLMax:%d\r\n", g_Config.nVersion, g_Config.nNation, g_Config.nIocpMax, g_Config.nSQLMax);

		g_IniFile.GetValue(L"Connection", L"MasterAcceptPort", &g_Config.nMasterAcceptPort);
		g_IniFile.GetValue(L"Connection", L"ClientAcceptPort", &g_Config.nClientAcceptPort);
		g_Log.Log(LogType::_FILELOG, L"## ClientAcceptPort:%d\r\n", g_Config.nClientAcceptPort);

		WCHAR wszStr[128] = {0,};
		std::vector<std::string> Tokens;

		g_IniFile.GetValue(L"Connection", L"Log", wszStr);
		if (wszStr[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);

			Tokens.clear();
			TokenizeA(szData, Tokens, ":");
			if (!Tokens.empty()){
				_strcpy(g_Config.LogInfo.szIP, _countof(g_Config.LogInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
				g_Config.LogInfo.nPort = atoi(Tokens[1].c_str());
			}
		}

		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d) Success\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);

		g_IniFile.GetValue(L"DB_DNMembership", L"DBIP", wszBuf);
		if (wszBuf[0] != '\0')
			WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.MembershipDB.szIP, sizeof(g_Config.MembershipDB.szIP), NULL, NULL);
		g_IniFile.GetValue(L"DB_DNMembership", L"DBPort", &g_Config.MembershipDB.nPort);
		g_IniFile.GetValue(L"DB_DNMembership", L"DBID", g_Config.MembershipDB.wszDBID);
		g_IniFile.GetValue(L"DB_DNMembership", L"DBName", g_Config.MembershipDB.wszDBName);

		g_IniFile.GetValue(L"DB_DNWorld", L"WorldDBCount", &g_Config.nWorldDBCount);

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
#if defined(_TW) 
	USES_CONVERSION;

	wstring wszGamaniaAuth = L"./Config/DNGamaniaAuth.ini";					// 대만

	if (!g_IniFile.Open(wszGamaniaAuth.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszGamaniaAuth.c_str());
		return false;
	}

	g_IniFile.GetValue(L"GamaniaAuth", L"IpAddr1", wszBuf);
	if (wszBuf[0] != '\0')
		_strcpy(g_Config.AuthInfoTW1.szIP, _countof(g_Config.AuthInfoTW1.szIP), W2CA(wszBuf), (int)strlen(W2CA(wszBuf)));
	g_IniFile.GetValue(L"GamaniaAuth", L"Port1", &g_Config.AuthInfoTW1.nPort);

	g_IniFile.GetValue(L"GamaniaAuth", L"IpAddr2", wszBuf);
	if (wszBuf[0] != '\0')
		_strcpy(g_Config.AuthInfoTW2.szIP, _countof(g_Config.AuthInfoTW2.szIP), W2CA(wszBuf), (int)strlen(W2CA(wszBuf)));
	g_IniFile.GetValue(L"GamaniaAuth", L"Port2", &g_Config.AuthInfoTW2.nPort);
	g_IniFile.GetValue(L"GamaniaAuth", L"DenyWorldID",  &g_Config.nDenyWorld);
#endif	// #if defined(_TW) 

#if defined(_US)
	wstring wstrPI = L"./Config/DNNexonUS_PI.ini";

	if (!g_IniFile.Open(wstrPI.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wstrPI.c_str());
		return false;
	}

	memset(&wszBuf, 0, sizeof(wszBuf));
	g_IniFile.GetValue(L"PassportImmigrator", L"HostAddress", wszBuf);
	if (wszBuf[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.PIInfo.szIP, sizeof(g_Config.PIInfo.szIP), NULL, NULL);
	g_IniFile.GetValue(L"PassportImmigrator", L"Port", &g_Config.PIInfo.nPort);

	memset(&wszBuf, 0, sizeof(wszBuf));
	g_IniFile.GetValue(L"PassportImmigrator", L"RequestPage", wszBuf);
	if (wszBuf[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.szRequestPage, sizeof(g_Config.szRequestPage), NULL, NULL);

	memset(&wszBuf, 0, sizeof(wszBuf));
	g_IniFile.GetValue(L"PassportImmigrator", L"UserAgent", wszBuf);
	if (wszBuf[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.szUserAgent, sizeof(g_Config.szUserAgent), NULL, NULL);

	memset(&wszBuf, 0, sizeof(wszBuf));
	g_IniFile.GetValue(L"PassportImmigrator", L"ServiceCode", wszBuf);
	if (wszBuf[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.szServiceCode, sizeof(g_Config.szServiceCode), NULL, NULL);

	g_IniFile.GetValue(L"AuthCheck", L"NMLocaleID", &g_Config.nLocaleID);
#endif	// #if defined(_US)

#ifdef _SG
	std::wstring wstrCherryAuth = L"./Config/DNCherryAuth.ini";

	if (!g_IniFile.Open(wstrCherryAuth.c_str()))
	{
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wstrCherryAuth.c_str());
		return false;
	}

	memset(&wszBuf, 0, sizeof(wszBuf));
	g_IniFile.GetValue(L"CherryAuth", L"GameID", &g_Config.nCherryGameID);

	memset(&wszBuf, 0, sizeof(wszBuf));
	g_IniFile.GetValue(L"CherryAuth", L"Addr", wszBuf);
	if (wszBuf[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.szCherryAuthAddr, sizeof(g_Config.szCherryAuthAddr), NULL, NULL);
#endif		//#ifdef _SG

#if defined(_TH) 
	USES_CONVERSION;

	wstring wszAsiaSoftAuth = L"./Config/DNAsiaSoftAuth.ini";

	if (!g_IniFile.Open(wszAsiaSoftAuth.c_str()))
	{
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszAsiaSoftAuth.c_str());
		return false;
	}

	g_IniFile.GetValue(L"AsiaSoftAuth", L"IpAddr1", wszBuf);
	if (wszBuf[0] != '\0')
		_strcpy(g_Config.AuthInfoTH.szIP, _countof(g_Config.AuthInfoTH.szIP), W2CA(wszBuf), (int)strlen(W2CA(wszBuf)));
	g_IniFile.GetValue(L"AsiaSoftAuth", L"Port1", &g_Config.AuthInfoTH.nPort);

	g_IniFile.GetValue(L"AsiaSoftAuth", L"IpAddr2", wszBuf);
	if (wszBuf[0] != '\0')
		_strcpy(g_Config.OTPInfoTH.szIP, _countof(g_Config.OTPInfoTH.szIP), W2CA(wszBuf), (int)strlen(W2CA(wszBuf)));
	g_IniFile.GetValue(L"AsiaSoftAuth", L"Port2", &g_Config.OTPInfoTH.nPort);

#endif	// #if defined(_TH)
#if defined(PRE_IDN_PVP)
	USES_CONVERSION;

	wstring wszKreon = L"./Config/DNKreon.ini";

	if (!g_IniFile.Open(wszKreon.c_str()))
	{
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszKreon.c_str());
		return false;
	}	
	g_IniFile.GetValue(L"Kreon", L"PvPWorldID", &g_Config.nPvPWorldID);	
#endif

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

bool InitApp(int argc, TCHAR * argv[])
{
	//if (!CheckSerial()) return false;
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
			swprintf(wszLogName, L"LoginServer_%d", nSID);
		}
		else
			swprintf(wszLogName, L"LoginServer");
	}
	else
		swprintf(wszLogName, L"LoginServer");

#if defined(_FINAL_BUILD)
	g_Log.Init(wszLogName, LOGTYPE_FILE_DAY);
#else
	g_Log.Init(wszLogName, LOGTYPE_CRT_FILE_DAY);
#endif	

	if (!LoadConfig(argc, argv)){
		g_Log.Log(LogType::_FILELOG, L"LoadConfig Failed\r\n");
		return false;
	}
	g_Log.SetServerID(g_Config.nManagedID);

	// ResourceMng 생성
	CEtResourceMng::CreateInstance();

	// Path 설정
	std::string szResource = g_Config.szResourcePath + "\\Resource";
	std::string szNationStr;
	if( szNationStr.empty() && !g_Config.szResourceNation.empty() ) szNationStr = g_Config.szResourceNation;
	if( !szNationStr.empty() ) 
	{
		szResource += szNationStr;
		CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
		szResource = g_Config.szResourcePath + "\\Resource";
	}

	CEtResourceMng::GetInstance().AddResourcePath( szResource, true );

#if defined(PRE_FIX_DEBUGSET_INIT)
	CDebugSet::Initialize( CEtResourceMng::GetInstance().GetCurrentDirectory().c_str(), true );
	bool bParsingComplete = true;
	std::string strErrorString = "";
#endif
	// UI XML String File Load
#ifdef PRE_ADD_BEGINNERGUILD
	g_pUIXML = new CEtUIXML;
#if defined(PRE_ADD_MULTILANGUAGE)
	std::string strNationFileName;
#ifdef PRE_ADD_UISTRING_DIVIDE
	std::string strNationItemFileName;
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE
	for (int i = 0; i < MultiLanguage::SupportLanguage::NationMax; i++)
	{
		strNationFileName.clear();

		strNationFileName = "uistring";
		if (i != 0)		//0번은 디폴트
			strNationFileName.append(MultiLanguage::NationString[i]);
		strNationFileName.append(".xml");	

		CStream *pStream = CEtResourceMng::GetInstance().GetStream( strNationFileName.c_str() );
 #if defined(PRE_FIX_DEBUGSET_INIT)
		if( g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1, i ) == false )
		{
			bParsingComplete = false;
			strErrorString.append( "[" + strNationFileName + "]" );
		}
#else	// #if defined(PRE_FIX_DEBUGSET_INIT)
		g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1, i );
#endif	// #if defined(PRE_FIX_DEBUGSET_INIT)
		SAFE_DELETE( pStream );

#ifdef PRE_ADD_UISTRING_DIVIDE
		strNationItemFileName.clear();

		strNationItemFileName = "uistring_item";
		if (i != 0)		//0번은 디폴트
			strNationItemFileName.append(MultiLanguage::NationString[i]);
		strNationItemFileName.append(".xml");	

		pStream = CEtResourceMng::GetInstance().GetStream( strNationItemFileName.c_str() );
#if defined(PRE_FIX_DEBUGSET_INIT)
		if( g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1, i ) == false )
		{
			bParsingComplete = false;
			strErrorString.append( "[" + strNationFileName + "]" );
		}
#else	//
		g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1, i );
#endif	//#if defined(PRE_FIX_DEBUGSET_INIT)
		SAFE_DELETE( pStream );
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring.xml" );
#if defined(PRE_FIX_DEBUGSET_INIT)
	if( g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1) == false )
	{
		bParsingComplete = false;
		strErrorString.append( "[uistring.xml]" );
	}
#else	//#if defined(PRE_FIX_DEBUGSET_INIT)
	g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1);
#endif	//#if defined(PRE_FIX_DEBUGSET_INIT)
	SAFE_DELETE( pStream );

#ifdef PRE_ADD_UISTRING_DIVIDE
	CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring_item.xml" );
#if defined(PRE_FIX_DEBUGSET_INIT)
	if( g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1) == false )
	{
		bParsingComplete = false;
		strErrorString.append( "[uistring_item.xml]" );
	}
#else	//#if defined(PRE_FIX_DEBUGSET_INIT)
	g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1);
#endif	//#if defined(PRE_FIX_DEBUGSET_INIT)
	SAFE_DELETE( pStream );
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE

#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#if defined(PRE_FIX_DEBUGSET_INIT)
	if( !bParsingComplete )
	{
		g_Log.Log(LogType::_FILELOG, L"UI XML String File Load Failed : %S\r\n", strErrorString.c_str());
		return false;
	}
#endif	//#if defined(PRE_FIX_DEBUGSET_INIT)
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#if defined(_HSHIELD)
	DWORD dwRetVal = NOERROR;
	g_Config.hHSServer = HShieldSvrWrapper::AhnHS_CreateServerObject("./DragonNest.hsb", &dwRetVal);
	if (NOERROR != dwRetVal) {
		g_Log.Log(LogType::_FILELOG, L"_AhnHS_CreateServerObject Failed [Exception]\n");
		return false;
	}

	if (g_Config.hHSServer == ANTICPX_INVALID_HANDLE_VALUE){
		g_Log.Log(LogType::_FILELOG, L"_AhnHS_CreateServerObject Failed\n");
		return false;
	}
#endif	// _HSHIELD

	CDNSecure::CreateInstance();

	g_pExtManager = new CDNExtManager;
	if (!g_pExtManager) return false;

	if (!g_pExtManager->AllLoad())
	{
		g_Log.Log(LogType::_FILELOG, L"CDNExtManager::AllLoad Fail\r\n");
		return false;
	}

	g_pMasterConnectionManager = new CDNMasterConnectionManager;
	if (!g_pMasterConnectionManager) return false;

	g_pUserConnectionManager = new CDNUserConnectionManager;
	if (!g_pUserConnectionManager) return false;

#if defined(_GPK)
	// Shanda 보안
	g_Config.pDynCode = GPKCreateSvrDynCode();
	if (!g_Config.pDynCode){
		g_Log.Log(LogType::_FILELOG, L"SvrDynCode NULL!!!\r\n");
		return false;
	}

	char szSvrDir[MAX_PATH] = { 0, };
	char szCltDir[MAX_PATH] = { 0, };

#if defined(WIN64)
	sprintf(szSvrDir, "./DynCodeBin64/Server");
	sprintf(szCltDir, "./DynCodeBin64/Client");

#else	// WIN64
#if defined(BIT64)
	sprintf(szSvrDir, "./DynCodeBin/Server64");
#else
	sprintf_s(szSvrDir, MAX_PATH - 1, "./DynCodeBin/Server");
#endif
	sprintf_s(szCltDir, MAX_PATH - 1, "./DynCodeBin/Client");
#endif	// WIN64

	int nBinCount = g_Config.pDynCode->LoadBinary(szSvrDir, szCltDir);

	if (nBinCount == 0){
		g_Log.Log(LogType::_FILELOG, L"Load DynCode failed!!!\r\n");
		// return false;
	}

	g_Config.pGpkCmd = g_Config.pDynCode->AllocAuthObject();
	if (g_Config.pGpkCmd == NULL)
	{
		g_Log.Log(LogType::_FILELOG, L"AllocAuthObject() Fail!!!\r\n");
		return false;
	}

	if (g_Config.pDynCode->LoadAuthFile("AuthData.dat") == false)	// CSAuth관련된 애
	{
		g_Log.Log(LogType::_FILELOG, L"LoadAuthFile() Fail!!!\r\n");
		return false;
	}

#endif	// _GPK

	g_pIocpManager = new CDNIocpManager;
	if (!g_pIocpManager) return false;

	if (g_Config.nIocpMax <= 0) g_Config.nIocpMax = 100;

//#ifdef _FINAL_BUILD
	//if (g_Config.nIocpMax < 2000)
	//{
	//	g_Config.nIocpMax = 2000;
	//	g_Log.Log(LogType::_FILELOG, L"Iocp Initialize NeedMore socket check serverStruct.xml\r\n");
	//}
//#endif

	if (g_pIocpManager->Init(g_Config.nIocpMax, g_Config.nWorkerThreadMax) < 0){
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize Fail\r\n");
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize Sucess(%d)\r\n", g_Config.nIocpMax);
	}

	//변경 로그와 서비스메니저의 연결은 런처를 통한 실행에서만 동작하도록 하자(실행인자에 의한 실행)
	g_pLogConnection = new CDNLogConnection;
	if( !g_pLogConnection ) return false;
	g_pLogConnection->SetIp(g_Config.LogInfo.szIP);
	g_pLogConnection->SetPort(g_Config.LogInfo.nPort);

	if (g_Config.ServiceInfo.nPort > 8)
	{
		g_pServiceConnection = new CDNServiceConnection(g_Config.nManagedID);
		if (g_pServiceConnection == NULL) return false;
		g_pServiceConnection->SetIp(g_Config.ServiceInfo.szIP);
		g_pServiceConnection->SetPort(g_Config.ServiceInfo.nPort);
	}

	g_pSQLMembershipManager = new CDNSQLMembershipManager(g_Config.nWorkerThreadMax);
	if (!g_pSQLMembershipManager) return false;

	if (!g_pSQLMembershipManager->CreateDB()){
		g_Log.Log(LogType::_FILELOG, L"g_pSQLMembershipManager CreateDB Fail\r\n");
		return false;
	}

	g_pSQLWorldManager = new CDNSQLWorldManager(g_Config.nWorkerThreadMax);
	if (!g_pSQLWorldManager) return false;

	if (!g_pSQLWorldManager->CreateDB()){
		g_Log.Log(LogType::_FILELOG, L"g_pSQLWorldManager CreateDB Fail\r\n");
	}

#if defined(_KRAZ) && defined(_FINAL_BUILD)
	g_pSQLActozCommonManager = new CDNSQLActozCommonManager(g_Config.nWorkerThreadMax);
	if (!g_pSQLActozCommonManager) return false;

	if (!g_pSQLActozCommonManager->CreateDB()){
		g_Log.Log(LogType::_FILELOG, L"g_pSQLActozCommonManager CreateDB Fail\r\n");
		return false;
	}
#endif	// #if defined(_KRAZ)

	g_pAuthManager = new CDNAuthManager;	// P.S.> _FINAL_BUILD 가 아닌 경우 인증 관리자가 MasterServer Acceptor 보다 먼저 생성, 초기화되어야 g_Config.nManagedID 를 세팅할 수 있음
	if (!g_pAuthManager) {
		g_Log.Log(LogType::_FILELOG, L"g_pAuthManager Create Fail\r\n");
		return false;
	}
	if (!g_pAuthManager->Init()) {
		g_Log.Log(LogType::_FILELOG, L"g_pAuthManager Initialize Fail\r\n");
		return false;
	}

	// Master Accept 
	if (g_pIocpManager->AddAcceptConnection(CONNECTIONKEY_MASTER, g_Config.nMasterAcceptPort, 1000) < 0){
		g_Log.Log(LogType::_FILELOG, L"MasterAcceptPort Fail(%d)\r\n", g_Config.nMasterAcceptPort);
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"MasterAcceptPort (%d)\r\n", g_Config.nMasterAcceptPort);
	}

#if defined( PRE_ADD_DWC )
	g_pDWCChannelManager = new CDNDWCChannelManager;
	if(!g_pDWCChannelManager) return false;

	if(!g_pDWCChannelManager->Init()){
		g_Log.Log(LogType::_FILELOG, L"g_pDWCChannelManager Init Fail\r\n");
		return false;
	}

#endif // #if defined( PRE_ADD_DWC )
	
	LoadUserSessionID();

	// Client Accept
	// QUERY_RESETAUTHSERVER 응답받으면 Accept한다.	
	
#if defined(_KR)	// Nexon 인증
	// 인증
	int nRet = AuthCheck_SetLocale(kLocaleID_KR);
	if (nRet == AUTHCHECK_ERROR_OK){
		g_Log.Log(LogType::_FILELOG, L"Nexon AuthCheck OK!!\r\n");
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"Nexon AuthCheck Fail (Error:%d)!!\r\n", nRet);
		return false;
	}

#elif defined(_KRAZ) && defined(_FINAL_BUILD)
	g_pActozAuth = new CDNActozAuth;
	if (!g_pActozAuth)
		return false;
	if (!g_pActozAuth->Init())
		return false;

	g_pActozShield = new CDNActozShield;
	if (!g_pActozShield)
		return false;
	if (!g_pActozShield->Init())
		return false;

#elif defined(_US)	// Nexon 인증
	// 인증
	NMLOCALEID Locale = (NMLOCALEID)g_Config.nLocaleID;
	int nRet = AuthCheck_SetLocale(Locale);
	if (nRet == AUTHCHECK_ERROR_OK){
		g_Log.Log(LogType::_FILELOG, L"Nexon AuthCheck OK!!\r\n");
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"Nexon AuthCheck Fail (Error:%d)!!\r\n", nRet);
		return false;
	}

#if defined(_FINAL_BUILD)
	// PI인증
	g_pNexonPI = new CDNNexonPI;
	if (!g_pNexonPI)
		return false;
#endif	// #if defined(_FINAL_BUILD)

#elif defined(_JP) && defined(_FINAL_BUILD)
	// 인증
	int nRet = HanAuthInit("J_DNEST");	// 나중에 서비스 타입 바꿔야함
	if (nRet == HAN_AUTHSVR_OK){
		g_Log.Log(LogType::_FILELOG, L"HanAuthInit OK!!\r\n");
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"HanAuthInit Fail (Error:%d)!!\r\n", nRet);
	}

#elif defined(_CH) && defined(_FINAL_BUILD)
	g_pShandaAuth = new CDNShandaAuth;
	if (!g_pShandaAuth) {
		g_Log.Log(LogType::_FILELOG, L"g_pShandaAuth Create Fail\r\n");
		return false;
	}
	if (!g_pShandaAuth->Open()) {
		g_Log.Log(LogType::_FILELOG, L"g_pShandaAuth Initialize Fail\r\n");
		return false;
	}

#elif defined(_TW) && defined(_FINAL_BUILD)
	g_pGamaniaAuthLogin = new CDNGamaniaAuth(CONNECTIONKEY_TW_AUTHLOGIN, g_Config.AuthInfoTW1.szIP, g_Config.AuthInfoTW1.nPort);
	if (!g_pGamaniaAuthLogin) {
		g_Log.Log(LogType::_FILELOG, L"g_pGamaniaAuthLogin Initialize Fail\r\n");
		return false;
	}

	g_pGamaniaAuthLogOut = new CDNGamaniaAuth(CONNECTIONKEY_TW_AUTHLOGOUT, g_Config.AuthInfoTW2.szIP, g_Config.AuthInfoTW2.nPort);
	if (!g_pGamaniaAuthLogOut) {
		g_Log.Log(LogType::_FILELOG, L"g_pGamaniaAuthLogOut Initialize Fail\r\n");
		return false;
	}

#elif defined(_TH) && defined(_FINAL_BUILD)
	g_pAsiaSoftAuth = new CDNAsiaSoftAuth(g_Config.AuthInfoTH.szIP, g_Config.AuthInfoTH.nPort);
	if (!g_pAsiaSoftAuth)
	{
		g_Log.Log(LogType::_FILELOG, L"g_pAsiaSoftAuth Initialize Fail\r\n");
		return false;
	}

	g_pAsiaSoftOTPManager = new CDNAsiaSoftOTPManager;
	if (!g_pAsiaSoftOTPManager)
	{
		g_Log.Log(LogType::_FILELOG, L"g_pAsiaSoftOTPManager Initialize Fail\r\n");
		return false;
	}

	if (g_pAsiaSoftOTPManager->Init(g_Config.OTPInfoTH.szIP, g_Config.OTPInfoTH.nPort, g_Config.nWorkerThreadMax) == false)
	{
		g_Log.Log(LogType::_FILELOG, L"g_pAsiaSoftOTPManager g_pAsiaSoftOTPManager->Init(g_Config.nWorkerThreadMax) Fail\r\n");
		return false;
	}
#elif defined(_ID) && defined(_FINAL_BUILD)
	g_pHttpClientManager = new CHttpClientManager;
	if( !g_pHttpClientManager )
	{
		g_Log.Log(LogType::_FILELOG, L"g_pHttpClientManager Initialize Fail\r\n");
		return false;
	}
	g_pHttpClientManager->CreateHttpClinet(g_Config.nWorkerThreadMax, KreonAuth::AuthServerAddress);
#elif defined(_RU) && defined(_FINAL_BUILD)
	g_pHttpClientManager = new CHttpClientManager;
	if( !g_pHttpClientManager )
	{
		g_Log.Log(LogType::_FILELOG, L"g_pHttpClientManager Initialize Fail\r\n");
		return false;
	}
	g_pHttpClientManager->CreateHttpClinet(g_Config.nWorkerThreadMax, MailRUAuth::AuthServerAddress, MailRUAuth::AuthLoginID, MailRUAuth::AuthPassWord);	

#elif defined(_EU) && defined(_FINAL_BUILD)
	g_pEUAuth = new CDNEUAuth;
	if (!g_pEUAuth) {
		g_Log.Log(LogType::_FILELOG, L"g_pEUAuth Create Fail\r\n");
		return false;
	}
	if (!g_pEUAuth->Init()) {
		g_Log.Log(LogType::_FILELOG, L"g_pEUAuth Initialize Fail\r\n");
		return false;
	}
#endif // _KR, _JP, _CH, _TW, _TH

	g_Log.Log(LogType::_FILEDBLOG, L"Application Initialize Success\r\n");
	return true;
}

void ClearApp()
{
#if !defined( _FINAL_BUILD )
	if( CDNQueryTest::IsActive() )
		CDNQueryTest::DestroyInstance();
#endif // #if !defined( _FINAL_BUILD )
	if(g_pIocpManager)
	{
		g_pIocpManager->CloseAcceptors();
		g_pIocpManager->ThreadStop();
	}
	SAFE_DELETE(g_pExtManager);
	SAFE_DELETE(g_pMasterConnectionManager);
#if defined(_KRAZ) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pSQLActozCommonManager);
#endif	// #if defined(_KRAZ)
	SAFE_DELETE(g_pSQLWorldManager);
	SAFE_DELETE(g_pSQLMembershipManager);
	SAFE_DELETE(g_pUserConnectionManager);
	SAFE_DELETE(g_pIocpManager);
	SAFE_DELETE(g_pLogConnection);
	SAFE_DELETE(g_pServiceConnection);
	SAFE_DELETE(g_pAuthManager);

#if defined(_HSHIELD)
	if (g_Config.hHSServer != ANTICPX_INVALID_HANDLE_VALUE){
		DWORD dwRetVal = NOERROR;
		HShieldSvrWrapper::AhnHS_CloseServerHandle(g_Config.hHSServer, &dwRetVal);
		if (NOERROR != dwRetVal) {
			// 예외 발생
		}

		g_Config.hHSServer = ANTICPX_INVALID_HANDLE_VALUE;
	}

#elif defined(_GPK)
	if (g_Config.pDynCode) g_Config.pDynCode->Release();
	if (g_Config.pGpkCmd) g_Config.pGpkCmd->Release();

#endif	// _HSHIELD

	if( CDNSecure::IsActive() )
		CDNSecure::DestroyInstance();

#if defined(_EU) && defined(_FINAL_BUILD)
	if (g_pEUAuth)
		g_pEUAuth->Terminate();
	SAFE_DELETE(g_pEUAuth);
#endif	// #if defined(_EU)
#if (defined(_ID) || defined(_RU)) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pHttpClientManager);
#endif 
#if defined(_TH) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pAsiaSoftAuth);
	SAFE_DELETE(g_pAsiaSoftOTPManager);
#endif	//#if defined(_TH)
#if defined(_TW) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pGamaniaAuthLogin);
	SAFE_DELETE(g_pGamaniaAuthLogOut);
#endif	//#if defined(_TW)
#if defined(_CH) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pShandaAuth);
#endif
#if defined(_US) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pNexonPI);
#endif	// #if defined(_US) && defined(_FINAL_BUILD)
#if defined(_KRAZ) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pActozAuth);
	SAFE_DELETE(g_pActozShield);
#endif	// #if defined(_KRAZ)
}

#ifndef _FINAL_BUILD
bool bIsGTest( int argc, TCHAR* argv[] )
{
	for( int i=0 ; i<argc ; ++i )
	{
		if( wcswcs( argv[i], L"--gtest_filter" ) )
		{
			testing::InitGoogleMock(&argc, argv);
			return true;
		}
	}

	return false;
}
#endif

int _tmain(int argc, TCHAR* argv[])
{
	BOOST_STATIC_ASSERT( sizeof(TIMESTAMP_STRUCT) == sizeof(DBTIMESTAMP) );

#ifdef _NewGameRes
	wprintf(L"[RLKT]Login_NewGameRes... \n");
#else
	wprintf(L"[RLKT]Login_OldGameRes... \n");
#endif

#ifndef _FINAL_BUILD
	if( bIsGTest( argc, argv ))
		return RUN_ALL_TESTS();
#endif

	g_GameOption.SetDefault();

#if defined(_CH)
	setlocale(LC_ALL, "chinese-simplified");
#elif defined(_TW)
	setlocale(LC_ALL, "chinese-traditional");
#elif defined(_JP)
	setlocale(LC_ALL, "japanese");
#elif defined(_US)
	setlocale(LC_ALL, "us");
#elif defined(_TH)
	setlocale(LC_ALL, "thai");
#else
	setlocale(LC_ALL, "Korean");
#endif

#if defined(_WORK)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// 예외 처리자 준비
	DWORD dwRetVal = CExceptionReport::GetInstancePtr()->Open(_T(".\\"), TRUE, TRUE, MiniDumpWithFullMemory);	// Release 모드 컴파일 시 C4744 경고가 발생하여 Singleton 구현 변경, CExceptionReport::GetInstancePtr() 을 inline 화 하지 않음 (참고 : http://msdn.microsoft.com/ko-kr/library/a7za416f.aspx)
	if (NOERROR != dwRetVal) {
		DWORD dwErrNo = ::GetLastError();
		DN_RETURN(dwErrNo);
	}	

	//AddToFirewall();

	if (!InitApp(argc, argv)){
		g_Log.Log(LogType::_FILEDBLOG, L"** InitApp Failed!!! Check!!!!!\r\n");
		ClearApp();
		return 0;
	}

	g_Log.Log(LogType::_FILELOG, L"[Thread-Start] MainThread - PID ; %d, TID : %d\r\n", ::GetCurrentProcessId(), ::GetCurrentThreadId());

	wprintf(L"exit 명령을 치면 종료\r\n");

	//SetConsoleTitleA(FormatA("LoginServer Rev.%s", revDNLoginServer).c_str());

	char szCmd[256] = {0,};	
	while (1)
	{
#if !defined( _FINAL_BUILD )
		CDNQueryTest::GetInstance().ProcessQueryTest( szCmd );
#endif // #if !defined( _FINAL_BUILD )

		if (strcmp(szCmd, "exit") == 0)	break;		

		if (!strcmp(szCmd, "report"))
		{
			UINT nA, nP, nR;
			g_pIocpManager->GetSendReport(&nA, &nP, &nR);
			printf("send A[%d]P[%d]R[%d]\n", nA, nP, nR);
		}

		if (!strcmp(szCmd, "recvreport"))
		{
			UINT nA, nP, nR;
			g_pIocpManager->GetRecvReport(&nA, &nP, &nR);
			printf("recv A[%d]P[%d]R[%d]\n", nA, nP, nR);
		}

		printf("CMD>");
        std::cin >> szCmd;
	}

	SaveUserSessionID();	

	g_Log.Log(LogType::_FILELOG, L"[Thread-End] MainThread - PID ; %d, TID : %d\r\n", ::GetCurrentProcessId(), ::GetCurrentThreadId());

	ClearApp();

#ifdef _WORK
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

