// DNMasterServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DNConfig.h"
#include "DNIocpManager.h"
#include "DNLoginConnection.h"
#include "Util.h"
#include "Log.h"
#include "ExceptionReport.h"
#include "DNExtManager.h"
#include "DNDivisionManager.h"
#include "DNServiceConnection.h"
#include "IniFile.h"
#include "Version.h"
#include "Stream.h"
#include "./EtStringManager/EtUIXML.h"
#include "./EtResManager/EtResourceMng.h"
#if defined(_WORK)
#include "PsUpdater.h"
#endif	// #if defined(_WORK)
#include "DNWaitUserManager.h"
#include "DNLogConnection.h"
#if defined(_TW) 
#include ".\\TW\\DNGamaniaAuth.h"
#endif	// #if defined(_TW)

#ifdef PRE_ADD_DOORS
#include "DNDoorsConnection.h"
#endif		//#ifdef PRE_ADD_DOORS

#if defined( STRESS_TEST )
#include "./boost/algorithm/string.hpp"
#include "./boost/lexical_cast.hpp"

#define MAX_SESSION_COUNT 1200
#endif
#include "ServiceUtil.h"
#if defined(_KR)
#include "DNNexonAuth.h"
#elif defined(_KRAZ)
#include "DNActozShield.h"
#elif defined(_CH)
#include "DNShandaFCM.h"
#elif defined (_JP) && defined (WIN64)
#include "DNNHNNetCafe.h"
#elif defined(_US)
#include "DNNexonPI.h"
#elif defined(_TH)
#include "DNAsiaSoftPCCafe.h"
#elif defined(_ID)
#include "DNKreonPCCafe.h"
#endif	// _CH
#include "DNGuildWarManager.h"
//#include "../../Server/RLKT_LICENSE/license_rlkt.h"

TMasterConfig g_Config;
#if !defined(_FINAL_BUILD)
int g_nPCBangGrade = 0;
#endif	// _WORK

#if defined(PRE_ADD_CHNC2C)
#include "../ServerCommon/CH/properties.h"
#include "DNC2C.h"
#endif
#if defined(PRE_FIX_DEBUGSET_INIT)
#include "DebugSet.h"
#endif

void LoadPartyID()
{
	string szFileName = "./Config/PartyIndex.dat";
	FILE * fp = NULL;
	if (_access(szFileName.c_str(), 0) == -1)
	{
		//파일이 없다면 생성한다.
		fp = fopen(szFileName.c_str(), "a");
		fclose(fp);
	}
	fp = fopen(szFileName.c_str(), "rb");

	if (!fp) return;

	USHORT nPartyIdx = 0;
	fread(&nPartyIdx, 1, sizeof(USHORT), fp);

	fclose(fp);

	nPartyIdx += 10000;	//혹시 저장 제대로 안되서 건너뛸 수 있으므로 꽁수로 10000 더해줌!
	g_IDGenerator.SetPartyID(nPartyIdx);
}

void SavePartyID()
{
	string szFileName = "./Config/PartyIndex.dat";
	FILE *fp = fopen(szFileName.c_str(), "wb");

	if (!fp) return;

	USHORT nPartyIdx = g_IDGenerator.GetPartyID();
	fwrite(&nPartyIdx, 1, sizeof(UINT), fp);

	fclose(fp);
}

void LoadVoiceChannelID()
{
	string szFileName = "./Config/VoiceChannelID.dat";
	FILE * fp = NULL;
	if (_access(szFileName.c_str(), 0) == -1)
	{
		//파일이 없다면 생성한다.
		fp = fopen(szFileName.c_str(), "a");
		fclose(fp);
	}
	fp = fopen(szFileName.c_str(), "rb");

	if (!fp) return;

	USHORT nChannelID = 0;
	fread(&nChannelID, 1, sizeof(USHORT), fp);

	fclose(fp);

	nChannelID += 10000;	//혹시 저장 제대로 안되서 건너뛸 수 있으므로 꽁수로 10000 더해줌!
	g_IDGenerator.SetVoiceChannelID(nChannelID);
}

void SaveVoiceChannelID()
{
	string szFileName = "./Config/VoiceChannelID.dat";
	FILE *fp = fopen(szFileName.c_str(), "wb");

	if (!fp) return;

	USHORT nVoiceChannelID = g_IDGenerator.GetVoiceChannelID();
	fwrite(&nVoiceChannelID, 1, sizeof(UINT), fp);

	fclose(fp);
}

bool LoadConfig(int argc, TCHAR * argv[])
{
	memset(&g_Config, 0, sizeof(TMasterConfig));

	//Version
	_strcpy(g_Config.szVersion, _countof(g_Config.szVersion), szMasterVersion, (int)strlen(szMasterVersion));
	_strcpy(g_Config.szResVersion, _countof(g_Config.szResVersion), "Unknown Version", (int)strlen("Unknown Version"));

	if (!g_IniFile.Open(L"./Config/DNMaster.ini")){
		g_Log.Log(LogType::_FILELOG, L"DNMaster.ini File not Found!!\r\n");
		return false;
	}

#ifdef PRE_MOD_OPERATINGFARM	
	g_Config.nFarmServerID = 0;
#endif		//#ifdef PRE_MOD_OPERATINGFARM

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

		if (GetFirstRightValue(L"mwi", argv[1], wstrTempConfig)){
			g_Config.nWorldSetID = _wtoi(wstrTempConfig.c_str());
		}
		else _ASSERT(0);
		g_Log.Log(LogType::_FILELOG, L"WorldID[%d]\n", g_Config.nWorldSetID);

		if (GetFirstRightValue(L"mvp", argv[1], wstrTempConfig))
			g_Config.nVillageAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

		if (GetFirstRightValue(L"title", argv[1], wstrTempConfig))
			SetConsoleTitleW(wstrTempConfig.c_str());

		if (GetFirstRightValue(L"mgp", argv[1], wstrTempConfig))
			g_Config.nGameAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);
		g_Log.Log(LogType::_FILELOG, L"VillageAcceptPort:%d, GameAcceptPort:%d\r\n", g_Config.nVillageAcceptPort, g_Config.nGameAcceptPort);



		GetDefaultConInfo(argv[1], L"log", &g_Config.LogInfo, 1);
		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);

		GetDefaultConInfo(argv[1], L"login", g_Config.LoginInfo, LOGINCOUNTMAX);
		for (int nIndex = 0; nIndex < LOGINCOUNTMAX; nIndex++)
		{
			if (g_Config.LoginInfo[nIndex].nPort <= 0) continue;
			g_Log.Log(LogType::_FILELOG, L"LoginInfo[%04d] (Ip:%S, Port:%d) Success\r\n", nIndex, g_Config.LoginInfo[nIndex].szIP, g_Config.LoginInfo[nIndex].nPort);
		}

#ifdef PRE_MOD_OPERATINGFARM
		if (GetFirstRightValue(L"fsid", argv[1], wstrTempConfig))
			g_Config.nFarmServerID = _wtoi(wstrTempConfig.c_str());
#endif		//#ifdef PRE_MOD_OPERATINGFARM

#ifdef PRE_ADD_DOORS
		GetDefaultConInfo(argv[1], L"door", &g_Config.DoorsInfo, 1);
		GetDefaultConInfo(argv[1], L"did1", &g_Config.DoorsIdenty[0], 1);
		GetDefaultConInfo(argv[1], L"did2", &g_Config.DoorsIdenty[1], 1);

		if(g_Config.DoorsInfo.nPort > 0) g_Log.Log(LogType::_FILELOG, L"DoorsInfo (Ip:%S, Port:%d) Success\r\n", g_Config.DoorsInfo.szIP, g_Config.DoorsInfo.nPort);
		if(g_Config.DoorsIdenty[0].nPort > 0) g_Log.Log(LogType::_FILELOG, L"DoorsIdenty1 (Ip:%S, Port:%d) Success\r\n", g_Config.DoorsIdenty[0].szIP, g_Config.DoorsIdenty[0].nPort);
		if(g_Config.DoorsIdenty[1].nPort > 0) g_Log.Log(LogType::_FILELOG, L"DoorsIdenty2 (Ip:%S, Port:%d) Success\r\n", g_Config.DoorsIdenty[1].szIP, g_Config.DoorsIdenty[1].nPort);
#endif		//#ifdef PRE_ADD_DOORS
	}
	else
	{
#if !defined(_WORK)
		g_IniFile.GetValue(L"Info", L"WorldID", &g_Config.nWorldSetID);
#endif	// _WORK
#if defined( PRE_WORLDCOMBINE_PARTY )
#if defined( _WORK )
		g_IniFile.GetValue(L"Info", L"WorldID", &g_Config.nWorldSetID);
#endif // #if defined( _WORK )
		g_IniFile.GetValue(L"Info", L"CombinePartyWorld", &g_Config.nCombinePartyWorld);
#endif

		if (g_Config.nWorldSetID <= 0){
			g_Log.Log(LogType::_FILELOG, L"WorldID(%d) Failed!!!!!!!!\r\n", g_Config.nWorldSetID);
		}

		g_IniFile.GetValue(L"Connection", L"VillageAcceptPort", &g_Config.nVillageAcceptPort);
		g_IniFile.GetValue(L"Connection", L"GameAcceptPort", &g_Config.nGameAcceptPort);
		g_Log.Log(LogType::_FILELOG, L"VillageAcceptPort:%d, GameAcceptPort:%d\r\n", g_Config.nVillageAcceptPort, g_Config.nGameAcceptPort);

		WCHAR wszBuf[128] = { 0, };
		char szData[128] = { 0, };

		std::vector<std::string> Tokens;

		for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
			::memset(wszBuf, 0, sizeof(wszBuf));

			USES_CONVERSION;

			g_IniFile.GetValue(L"Connection", CVarArgW<MAX_PATH>(L"Login%d", nIndex + 1), wszBuf);
			if (L'\0' == wszBuf[0])
				break;

			_strcpy(szData, _countof(szData), W2CA(wszBuf), (int)strlen(W2CA(wszBuf)));

			Tokens.clear();
			TokenizeA(szData, Tokens, ":");
			if (!Tokens.empty()){
				_strcpy(g_Config.LoginInfo[nIndex].szIP, _countof(g_Config.LoginInfo[nIndex].szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
				g_Config.LoginInfo[nIndex].nPort = atoi(Tokens[1].c_str());
			}

			g_Log.Log(LogType::_FILELOG, L"LoginInfo[%04d] (Ip:%S, Port:%d)\r\n", nIndex, g_Config.LoginInfo[nIndex].szIP, g_Config.LoginInfo[nIndex].nPort);
		}

		WCHAR wszLogStr[128] = {0,};	
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

#ifdef PRE_ADD_DOORS
		WCHAR wszDoorsStr[128] = {0,};	
		g_IniFile.GetValue(L"Connection", L"Doors", wszDoorsStr);
		if (wszDoorsStr[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszDoorsStr, -1, szData, sizeof(szData), NULL, NULL);

			Tokens.clear();
			TokenizeA(szData, Tokens, ":");
			if (!Tokens.empty()){
				_strcpy(g_Config.DoorsInfo.szIP, _countof(g_Config.DoorsInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
				g_Config.DoorsInfo.nPort = atoi(Tokens[1].c_str());
			}
		}

		memset(wszDoorsStr, 0, sizeof(wszDoorsStr));
		for (int i = 0; i < 2; i++)
		{
			g_IniFile.GetValue(L"Connection", i == 0 ? L"DoorsIdenty1" : L"DoorsIdenty2", wszDoorsStr);
			if (wszDoorsStr[0] != '\0'){
				WideCharToMultiByte(CP_ACP, NULL, wszDoorsStr, -1, szData, sizeof(szData), NULL, NULL);

				Tokens.clear();
				TokenizeA(szData, Tokens, ":");
				if (!Tokens.empty()){
					_strcpy(g_Config.DoorsIdenty[i].szIP, _countof(g_Config.DoorsIdenty[i].szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
					g_Config.DoorsIdenty[i].nPort = atoi(Tokens[1].c_str());
				}
			}
		}
#endif		//PRE_ADD_DOORS

		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);		

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

#if !defined(_FINAL_BUILD)
	g_IniFile.GetValue(L"Info", L"PCBangGrade", &g_nPCBangGrade);
#endif	// #if defined(_FINAL_BUILD)

	//나라마다 틀리고 고정 값은 공통으로 config에서 읽는다.
#if defined(_KR) && defined(_FINAL_BUILD)
	wstring wszAuthFileName = L"./Config/DNNexonAuth.ini";			// 한국

	if (!g_IniFile.Open(wszAuthFileName.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszAuthFileName.c_str());
		return false;
	}

	std::vector<std::string> Tokens;

	WCHAR wszStr[64] = { 0, };
	char szData[128] = { 0, }, *pStr = NULL;
	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"NexonAuth", L"Auth", wszStr);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);	

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.AuthInfo.szIP, _countof(g_Config.AuthInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.AuthInfo.nPort = atoi(Tokens[1].c_str());
		}
	}

#ifdef _FINAL_BUILD
	g_Config.nAuthDomainSN = g_Config.nWorldSetID - 1;
	g_Log.Log(LogType::_FILELOG, L"AuthInfo (Ip:%S, Port:%d, SN:%d) Success\r\n", g_Config.AuthInfo.szIP, g_Config.AuthInfo.nPort, g_Config.nAuthDomainSN);
#else //_FINAL_BUILD
	g_IniFile.GetValue(L"NexonAuth", L"DomainSN", &g_Config.nAuthDomainSN);
	g_Log.Log(LogType::_FILELOG, L"AuthInfo (Ip:%S, Port:%d, SN:%d) Success\r\n", g_Config.AuthInfo.szIP, g_Config.AuthInfo.nPort, g_Config.nAuthDomainSN);
#endif //_FINAL_BUILD

#elif defined(_TW) && defined(_FINAL_BUILD) 
	USES_CONVERSION;

	wstring wszGamaniaAuth = L"./Config/DNGamaniaAuth.ini";			// 대만

	if (!g_IniFile.Open(wszGamaniaAuth.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszGamaniaAuth.c_str());
		return false;
	}

	WCHAR wszStr[64] = { 0, };
	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"GamaniaAuth", L"IpAddr2", wszStr);
	if (wszStr[0] != '\0')
		_strcpy(g_Config.AuthInfoTW.szIP, _countof(g_Config.AuthInfoTW.szIP), W2CA(wszStr), (int)strlen(W2CA(wszStr)));
	g_IniFile.GetValue(L"GamaniaAuth", L"Port2", &g_Config.AuthInfoTW.nPort);

#elif defined(_TH)
	wstring wszAsiaSoftPCCafe = L"./Config/DNAsiaSoftPCCafe.ini";			// 태국

	if (!g_IniFile.Open(wszAsiaSoftPCCafe.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszAsiaSoftPCCafe.c_str());
		return false;
	}
	WCHAR wszStr[64] = { 0, };	
	char szData[128] = { 0, };

	std::vector<std::string> Tokens;
	g_IniFile.GetValue(L"AsiaSoftPCCafe", L"PCCafeServer", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);	

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.AsiaSoftPCCafe.szIP, _countof(g_Config.AsiaSoftPCCafe.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.AsiaSoftPCCafe.nPort = atoi(Tokens[1].c_str());	
		}
	}
#endif	// _KR _TW

#if defined(_US)
	WCHAR wszBuf[128] = { 0, };

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
#endif	// #if defined(_US)

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
			swprintf(wszLogName, L"MasterServer_%d", nSID);
		}
		else
			swprintf(wszLogName, L"MasterServer");
	}
	else
		swprintf(wszLogName, L"MasterServer");

#if defined(_FINAL_BUILD)
	g_Log.Init(wszLogName, LOGTYPE_FILE_HOUR);
#else
	g_Log.Init(wszLogName, LOGTYPE_CRT_FILE_DAY);
#endif

	// 로딩 시기때문에 DivisionManager에서 빼냈음. 081208
	if (!LoadConfig(argc, argv)){
		g_Log.Log(LogType::_FILELOG, L"LoadConfig Failed\r\n");
		return false;
	}
	g_Log.SetServerID(g_Config.nManagedID);

	// ResourceMng 생성
	CEtResourceMng::CreateInstance();

	// Path 설정
	std::string szResource = g_Config.szResourcePath + "\\Resource";
	std::string szMapData = g_Config.szResourcePath + "\\MapData";
	// 국가별 셋팅
	std::string szNationStr;
	if( szNationStr.empty() && !g_Config.szResourceNation.empty() ) szNationStr = g_Config.szResourceNation;

	if( !szNationStr.empty() ) 
	{
		szResource += szNationStr;
		szMapData += szNationStr;

		CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
		CEtResourceMng::GetInstance().AddResourcePath( szMapData, true );

		szResource = g_Config.szResourcePath + "\\Resource";
		szMapData = g_Config.szResourcePath + "\\MapData";
	}

	CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
	CEtResourceMng::GetInstance().AddResourcePath( szMapData, true );

	LoadPartyID();
	LoadVoiceChannelID();

	g_pIocpManager = new CDNIocpManager;
	if (!g_pIocpManager) 
	{
		g_Log.Log(LogType::_FILELOG, L"new CDNIocpManager Fail\r\n");
		return false;
	}

	if (g_pIocpManager->Init(100) < 0){
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize Fail\r\n");
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize Success(%d)\r\n", 100);
	}

	// worldid, worldname을 DataManager에서 다시 세팅하게 바꿈
	g_pExtManager = new CDNExtManager;
	if (!g_pExtManager) 
	{
		g_Log.Log(LogType::_FILELOG, L"new CDNExtManager Fail\r\n");
		return false;
	}
	if (!g_pExtManager->AllLoad())
	{
		g_Log.Log(LogType::_FILELOG, L"CDNExtManager::AllLoad Fail\r\n");
		return false;
	}

	g_pDivisionManager = new CDivisionManager();
	if (!g_pDivisionManager) 
	{
		g_Log.Log(LogType::_FILELOG, L"new CDivisionManager Fail\r\n");
		return false;	
	}

	g_pWaitUserManager = new CDNWaitUserManager;
	if (!g_pWaitUserManager)
	{
		g_Log.Log(LogType::_FILELOG, L"new CDNWaitUserManager Fail\r\n");
		return false;
	}
	g_pWaitUserManager->Initialize(g_pExtManager->GetWorldMaxUser(g_Config.nWorldSetID));
	g_pGuildWarManager = new CDNGuildWarManager;
	if (!g_pGuildWarManager)
	{
		g_Log.Log(LogType::_FILELOG, L"new CDNGuildWarManager Fail\r\n");
		return false;
	}

#ifdef PRE_ADD_DOORS
	g_pDoorsConnection = new CDNDoorsConnection;
	if (!g_pDoorsConnection) return false;
	g_pDoorsConnection->SetIp(g_Config.DoorsInfo.szIP);
	g_pDoorsConnection->SetPort(g_Config.DoorsInfo.nPort);
#endif		//#ifdef PRE_ADD_DOORS

	g_pLogConnection = new CDNLogConnection;
	if( !g_pLogConnection ) return false;
	g_pLogConnection->SetIp(g_Config.LogInfo.szIP);
	g_pLogConnection->SetPort(g_Config.LogInfo.nPort);

	if (g_Config.nManagedID > 0)
	{
		g_pServiceConnection = new CDNServiceConnection(g_Config.nManagedID);
		if (!g_pServiceConnection) return false;
		g_pServiceConnection->SetIp(g_Config.ServiceInfo.szIP);
		g_pServiceConnection->SetPort(g_Config.ServiceInfo.nPort);
	}

#if defined(PRE_FIX_DEBUGSET_INIT)
	CDebugSet::Initialize( CEtResourceMng::GetInstance().GetCurrentDirectory().c_str(), true );
	bool bParsingComplete = true;
	std::string strErrorString = "";
#endif
	// UI XML String File Load
	CEtUIXML::CreateInstance();
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
		if (pStream == NULL) continue;
#if defined(PRE_FIX_DEBUGSET_INIT)
		if( CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1, i ) == false )
		{
			bParsingComplete = false;
			strErrorString.append( "[" + strNationFileName + "]" );
		}
#else
		CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1, i );
#endif
		SAFE_DELETE( pStream );

		strNationFileName.clear();

#ifdef PRE_ADD_UISTRING_DIVIDE
		strNationItemFileName = "uistring_item";
		if (i != 0)		//0번은 디폴트
			strNationItemFileName.append(MultiLanguage::NationString[i]);
		strNationItemFileName.append(".xml");

		pStream = CEtResourceMng::GetInstance().GetStream( strNationItemFileName.c_str() );
		if (pStream == NULL) continue;
#if defined(PRE_FIX_DEBUGSET_INIT)
		if( CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1, i, true ) == false )
		{
			bParsingComplete = false;
			strErrorString.append( "[" + strNationFileName + "]" );
		}
#else
		CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1, i, true );
#endif
		SAFE_DELETE( pStream );
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring.xml" );
	
#if defined(PRE_FIX_DEBUGSET_INIT)
	if( CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1 ) == false )
	{
		bParsingComplete = false;
		strErrorString.append( "[uistring.xml]" );
	}
#else
	CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1 );
#endif
	SAFE_DELETE( pStream );

#ifdef PRE_ADD_UISTRING_DIVIDE
	CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring_item.xml" );
#if defined(PRE_FIX_DEBUGSET_INIT)
	if( CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1 ) == false )
	{
		bParsingComplete = false;
		strErrorString.append( "[uistring_item.xml]" );
	}
#else
	CEtUIXML::GetInstance().Initialize( pStream, CEtUIXML::idCategory1 );
#endif	
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

	if (g_pIocpManager->AddAcceptConnection(CONNECTIONKEY_VILLAGE, g_Config.nVillageAcceptPort, 1000) < 0){
		g_Log.Log(LogType::_FILELOG, L"VillageAcceptPort Fail(%d)\r\n", g_Config.nVillageAcceptPort);
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"VillageAcceptPort (%d)\r\n", g_Config.nVillageAcceptPort);
	}

	if (g_pIocpManager->AddAcceptConnection(CONNECTIONKEY_GAME, g_Config.nGameAcceptPort, 1000) < 0){
		g_Log.Log(LogType::_FILELOG, L"GameAcceptPort Fail(%d)\r\n", g_Config.nGameAcceptPort);
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"GameAcceptPort (%d)\r\n", g_Config.nGameAcceptPort);
	}

#if defined(_KR) && defined(_FINAL_BUILD)	// Nexon 인증
	// pc방
	g_pNexonAuth = new CDNNexonAuth;
	if (!g_pNexonAuth) return false;

#elif defined(_KRAZ) && defined(_FINAL_BUILD)
	g_pActozShield = new CDNActozShield;
	if (!g_pActozShield)
		return false;
	if (!g_pActozShield->Init())
		return false;

#elif defined(_US) && defined(_FINAL_BUILD)	// Nexon 인증
	// PI인증
	g_pNexonPI = new CDNNexonPI;
	if (!g_pNexonPI)
		return false;	

#elif defined(_CH) && defined(_FINAL_BUILD)
	g_pShandaFCM = new CDNShandaFCM;
	if (!g_pShandaFCM) return false;

	if (!g_pShandaFCM->InitService()){	// FCM 초기화작업
		g_Log.Log(LogType::_FILELOG, L"g_pShandaFCM->InitService() Fail!!\r\n");
	}
#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
	g_pNHNNetCafe = new CDNNHNNetCafe;
	if (!g_pNHNNetCafe) return false;

	if (g_pNHNNetCafe->NetCafe_Initialize() == false)
	{	//NHN NetCafe Module Init
		g_Log.Log(LogType::_FILELOG, L"g_pNHNNetCafe->Initialize() Fail!!\r\n");
		//return false;
	}
	else
		g_Log.Log(LogType::_FILELOG, L"g_pNHNNetCafe->Initialize() OK!!\r\n");
#elif defined(_TH) && defined(_FINAL_BUILD)
	g_pAsiaSoftPCCafe = new CDNAsiaSoftPCCafe(g_Config.AsiaSoftPCCafe.szIP, g_Config.AsiaSoftPCCafe.nPort);
	if( !g_pAsiaSoftPCCafe ) return false;
#elif defined(_ID) && defined(_FINAL_BUILD)
	g_pKreonPCCafe = new CDNKreonPCCafe;
	if( !g_pKreonPCCafe ) return false;
#endif	// _KR, _CH

#if defined(_TW) && defined(_FINAL_BUILD)
	g_pGamaniaAuthLogOut = new CDNGamaniaAuth(CONNECTIONKEY_TW_AUTHLOGOUT, g_Config.AuthInfoTW.szIP, g_Config.AuthInfoTW.nPort);
	if (!g_pGamaniaAuthLogOut) {
		return false;
	}
#endif	// #if defined(_TW) 

#if defined(PRE_ADD_CHNC2C)
	CProperties Properties;
	if( Properties.ImportProperties("./config/DNC2C.ini") < 0)
	{
		g_Log.Log(LogType::_FILELOG, L"C2C Service DNC2C.ini file Load Fail!!\r\n");
		return false;
	}

	if( Properties.GetProperty<std::string>("Switch", "Off") == "On")
	{
		g_pDnC2C = new CDNC2C();
		if( !g_pDnC2C ) return false;
		if( !g_pDnC2C->Init(Properties.GetProperty<std::string>("RemoteIP", "127.0.0.1"), 
			Properties.GetProperty("RemotePort", 9527), 
			Properties.GetProperty<std::string>("MatrixID", "89-1-1"),
			Properties.GetProperty<std::string>("DeviceID", "ServerID")))
		{
			g_Log.Log(LogType::_FILELOG, L"C2C Service Logic Init Error!!\r\n");
			return false;		
		}
		g_Log.Log(LogType::_FILELOG, L"C2C ON!!\r\n");	
	}
	else
		g_Log.Log(LogType::_FILELOG, L"C2C OFF!!\r\n");	
#endif

	::srand( timeGetTime() );

	g_Log.Log(LogType::_FILEDBLOG, L"Application Initialize Success\r\n");

	return true;
}

void ClearApp()
{
	if(g_pIocpManager)
	{
		g_pIocpManager->CloseAcceptors();
		g_pIocpManager->ThreadStop();
	}

#if defined(_KR) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pNexonAuth);
#elif defined(_KRAZ)
	SAFE_DELETE(g_pActozShield);
#elif defined(_CH) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pShandaFCM);
#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
	SAFE_DELETE(g_pNHNNetCafe);
#elif defined(_TH) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pAsiaSoftPCCafe);
#elif defined(_ID) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pKreonPCCafe);
#endif	// _CH
#if defined(_TW) && defined(_FINAL_BUILD)
	SAFE_DELETE(g_pGamaniaAuthLogOut);
#endif	// #if defined(_TW) 

	SAFE_DELETE(g_pServiceConnection);
	SAFE_DELETE(g_pExtManager);
	SAFE_DELETE(g_pDivisionManager);
	SAFE_DELETE(g_pIocpManager); 

	if( CEtUIXML::IsActive() )
		delete CEtUIXML::GetInstancePtr();
	if( CEtResourceMng::IsActive() )
		delete CEtResourceMng::GetInstancePtr();
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
	/*
#if defined(_WORK)
	if (!g_PsUpdater)
		g_PsUpdater = new(std::nothrow) CPsUpdater;

	g_PsUpdater->AsyncUpdate ();
#endif	// #if defined(_WORK)
	*/
#ifndef _FINAL_BUILD
	if( bIsGTest( argc, argv ))
		return RUN_ALL_TESTS();
#endif

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
	DWORD dwRetVal = CExceptionReport::GetInstancePtr()->Open(_T(".\\"), TRUE, TRUE, MiniDumpWithFullMemory);	// Release 모드 컴파일 시 C4744 경고가 발생하여 Singleton 구현 변경, CExceptionReport::GetInstancePtr() 을 inline 화 하지 않음 (참고 : http://msdn.microsoft.com/ko-kr/library/a7za416f.aspx)
	if (NOERROR != dwRetVal) {
		DWORD dwErrNo = ::GetLastError();
		DN_RETURN(dwErrNo);
	}

	if (!InitApp(argc, argv)){
		g_Log.Log(LogType::_FILEDBLOG, L"** InitApp Failed!!! Check!!!!!\r\n");
		ClearApp();
		return 0;
	}

	wprintf(L"exit 명령을 치면 종료\r\n");

	char szCmd[256] = {0};	
	while (1)
	{
		if (strcmp(szCmd, "exit") == 0)	break;

#ifdef _WORK
		if (g_pDivisionManager)
			g_pDivisionManager->Parse(szCmd);
#endif		
#if defined( STRESS_TEST )
		if( strstr( szCmd, "create") )
		{
			int iDefaultMapIndex	= 15005;
			int iDefaultCreateCount	= 100;
			int iRandomSeed			= 0;

			std::vector<int>	vDefaultMapIndex;
			vDefaultMapIndex.push_back( 271 );
			vDefaultMapIndex.push_back( 272 );
			vDefaultMapIndex.push_back( 273 );
			vDefaultMapIndex.push_back( 274 );
			vDefaultMapIndex.push_back( 311 );
			vDefaultMapIndex.push_back( 312 );
			vDefaultMapIndex.push_back( 313 );
			vDefaultMapIndex.push_back( 314 );
			vDefaultMapIndex.push_back( 51 );
			vDefaultMapIndex.push_back( 52 );
			vDefaultMapIndex.push_back( 53 );
			vDefaultMapIndex.push_back( 56 );
			vDefaultMapIndex.push_back( 57 );
			vDefaultMapIndex.push_back( 58 );
			vDefaultMapIndex.push_back( 61 );
			vDefaultMapIndex.push_back( 62 );
			vDefaultMapIndex.push_back( 71 );
			vDefaultMapIndex.push_back( 72 );
			vDefaultMapIndex.push_back( 73 );
			vDefaultMapIndex.push_back( 81 );
			vDefaultMapIndex.push_back( 82 );
			vDefaultMapIndex.push_back( 83 );
			vDefaultMapIndex.push_back( 86 );
			vDefaultMapIndex.push_back( 87 );
			vDefaultMapIndex.push_back( 88 );
			vDefaultMapIndex.push_back( 91 );
			vDefaultMapIndex.push_back( 92 );
			vDefaultMapIndex.push_back( 93 );
			vDefaultMapIndex.push_back( 96 );
			vDefaultMapIndex.push_back( 97 );
			vDefaultMapIndex.push_back( 98 );
			vDefaultMapIndex.push_back( 101 );
			vDefaultMapIndex.push_back( 102 );
			vDefaultMapIndex.push_back( 103 );
			vDefaultMapIndex.push_back( 117 );
			vDefaultMapIndex.push_back( 118 );
			vDefaultMapIndex.push_back( 121 );
			vDefaultMapIndex.push_back( 122 );
			vDefaultMapIndex.push_back( 123 );
			vDefaultMapIndex.push_back( 124 );
			vDefaultMapIndex.push_back( 126 );
			vDefaultMapIndex.push_back( 127 );
			vDefaultMapIndex.push_back( 128 );
			vDefaultMapIndex.push_back( 129 );
			vDefaultMapIndex.push_back( 130 );
			vDefaultMapIndex.push_back( 131 );
			vDefaultMapIndex.push_back( 132 );
			vDefaultMapIndex.push_back( 133 );
			vDefaultMapIndex.push_back( 134 );
			vDefaultMapIndex.push_back( 136 );
			vDefaultMapIndex.push_back( 137 );
			vDefaultMapIndex.push_back( 138 );
			vDefaultMapIndex.push_back( 139 );
			vDefaultMapIndex.push_back( 141 );
			vDefaultMapIndex.push_back( 142 );
			vDefaultMapIndex.push_back( 143 );
			vDefaultMapIndex.push_back( 144 );
			vDefaultMapIndex.push_back( 146 );
			vDefaultMapIndex.push_back( 147 );
			vDefaultMapIndex.push_back( 148 );
			vDefaultMapIndex.push_back( 149 );
			vDefaultMapIndex.push_back( 151 );
			vDefaultMapIndex.push_back( 152 );
			vDefaultMapIndex.push_back( 153 );
			vDefaultMapIndex.push_back( 154 );
			vDefaultMapIndex.push_back( 161 );
			vDefaultMapIndex.push_back( 162 );
			vDefaultMapIndex.push_back( 163 );
			vDefaultMapIndex.push_back( 164 );
			vDefaultMapIndex.push_back( 171 );
			vDefaultMapIndex.push_back( 172 );
			vDefaultMapIndex.push_back( 173 );
			vDefaultMapIndex.push_back( 174 );
			vDefaultMapIndex.push_back( 241 );
			vDefaultMapIndex.push_back( 242 );
			vDefaultMapIndex.push_back( 243 );
			vDefaultMapIndex.push_back( 244 );
			vDefaultMapIndex.push_back( 251 );
			vDefaultMapIndex.push_back( 252 );
			vDefaultMapIndex.push_back( 253 );
			vDefaultMapIndex.push_back( 254 );
			vDefaultMapIndex.push_back( 261 );
			vDefaultMapIndex.push_back( 262 );
			vDefaultMapIndex.push_back( 263 );
			vDefaultMapIndex.push_back( 264 );
			vDefaultMapIndex.push_back( 271 );
			vDefaultMapIndex.push_back( 272 );
			vDefaultMapIndex.push_back( 273 );
			vDefaultMapIndex.push_back( 274 );
			vDefaultMapIndex.push_back( 281 );
			vDefaultMapIndex.push_back( 282 );
			vDefaultMapIndex.push_back( 283 );
			vDefaultMapIndex.push_back( 284 );
			vDefaultMapIndex.push_back( 291 );
			vDefaultMapIndex.push_back( 292 );
			vDefaultMapIndex.push_back( 293 );
			vDefaultMapIndex.push_back( 294 );
			vDefaultMapIndex.push_back( 311 );
			vDefaultMapIndex.push_back( 312 );
			vDefaultMapIndex.push_back( 313 );
			vDefaultMapIndex.push_back( 314 );


			UINT randVal;
			rand_s(&randVal);
			iDefaultMapIndex = vDefaultMapIndex[randVal%vDefaultMapIndex.size()];

			std::string					strValue(szCmd);
			std::vector<std::string>	vSplit;
			boost::algorithm::split( vSplit, strValue, boost::algorithm::is_any_of("/") );

			// 명령어
			if( vSplit.size() >= 1 )
			{
				if( stricmp( vSplit[0].c_str(), "create" ) )
					break;
			}
			// 방개수
			if( vSplit.size() >= 2 )
			{
				iDefaultCreateCount = boost::lexical_cast<int>( vSplit[1] );
			}
			// 랜덤시드
			if( vSplit.size() >= 3 )
			{
				iRandomSeed = boost::lexical_cast<int>( vSplit[2] );
			}
			// 맵번호
			if( vSplit.size() >= 4 )
			{
				iDefaultMapIndex = boost::lexical_cast<int>( vSplit[3] );
			}
			
			static bool bLoop = true;
			static int nLoop1 = 1000;
			static int nLoop2 = 10000;
			static int nRandDelay = 100;
			while(bLoop){
				int iAccountDBIDArray[] = { 1,1,1,2,2,2,3,3,3,3,5,6,6,10,10,10,10,12,12,12,12,13,13,15,15,16,16,16,16,17,19,19,22,24,24,27,27,28,28,28,29,30,30,30,32,35,35,35,37,38,38,38,38,39,39,39,39,40,40,40,40,41,41,41,41,46,46,46,46,49,49,49,50,52,53,53,53,54,54,55,55,55,55,56,57,88,90,90,90,90,91,91,91,91,140,140,140,140,142,142,143,143,143,145,145,145,145,147,151,151,151,152,152,153,153,153,153,154,154,155,156,156,156,157,158,158,158,158,159,160,160,161,161,161,161,162,162,164,164,175,175,176,177,177,177,177,178,178,178,180,180,194,194,194,194,195,205,205,205,205,209,209,210,210,210,210,211,211,211,211,212,212,212,212,213,213,213,213,214,214,215,215,215,216,216,216,216,217,217,217,217,218,218,218,218,219,219,219,219,220,220,221,221,221,224,224,224,225,225,225,225,226,226,226,226,227,227,227,227,228,228,228,228,229,229,229,229,230,230,230,230,231,231,231,231,232,232,232,232,233,233,233,233,234,234,234,234,235,235,235,235,236,236,236,236,237,237,237,237,238,238,238,238,239,239,239,239,240,240,240,240,241,241,241,241,242,242,249,249,260,260,260,260,262,263,263,263,263,264,264,264,264,266,266,267,267,267,267,269,270,15271,15272,15273,15274,15275,15276,15277,15278,15279,15279,15279,15279,15280,15280,15280,15280,15281,15281,15281,15282,15282,15282,15282,15283,15283,15283,15284,15284,15284,15285,15285,15285,15286,15288,15288,15289,15289,15289,15289,15290,15290,15290,15290,15291,15291,15291,15291,15293,15293,15293,15293,15299,15299,15299,15299,15300,15300,15300,15301,15301,15301,15301,15304,15309,15309,15309,15310,15341,15341,15341,15341,15348,15348,15348,15348,15349,15349,15349,15349,15350,15350,15350,15350,15351,15351,15351,15351,15352,15352,15352,15353,15353,15353,15354,15354,15354,15354,15355,15355,15355,15355,15356,15356,15356,15356,15357,15357,15357,15357,15358,15358,15358,15358,15359,15359,15359,15359,15360,15360,15360,15360,15361,15361,15361,15361,15362,15362,15362,15362,15363,15363,15363,15363,15364,15364,15364,15364,15365,15365,15365,15365,15366,15366,15366,15366,15367,15367,15367,15367,15368,15368,15368,15368,15369,15369,15369,15370,15370,15370,15371,15371,15371,15372,15372,15372,15373,15373,15373,15374,15374,15374,15375,15375,15375,15376,15376,15376,15377,15377,15377,15378,15378,15378,15378,15379,15379,15379,15379,15380,15380,15381,15384,15400,15442,15482,15482,15482,15483,15483,15483,15484,15484,15484,15485,15485,15485,15486,15486,15486,15487,15487,15487,15488,15488,15488,15489,15489,15489,15490,15490,15490,15491,15491,15491,15492,15492,15492,15493,15493,15493,15494,15494,15494,15495,15495,15495,15496,15496,15496,15497,15497,15497,15498,15498,15498,15499,15499,15499,15500,15500,15500,15501,15501,15501,15502,15502,15502,15503,15503,15503,15504,15504,15504,15505,15505,15505,15506,15506,15506,15507,15507,15507,15508,15508,15508,15509,15509,15509,15510,15510,15510,15511,15511,15511,15512,15512,15512,15513,15513,15513,15514,15581,15581 };
				INT64 biCharacterDBIDArray[] = { 4,120,60432,5,60428,60429,352,22333,60598,60607,60711,101,102,25,60404,60635,60641,313,321,328,60410,30,317,60,377,52,53,141,312,34,65,67,376,60772,61489,111,60730,3,78,88,48,28,60493,60498,58,61,298,391,43,61079,61083,61086,61087,60539,60595,60604,61217,60542,60543,60545,60599,60549,60550,61422,61442,362,60398,60431,60480,288,311,372,97,36,148,150,153,22,379,76,241,257,60615,60773,61629,326,60456,61850,61852,61858,261,266,60574,61645,392,60587,60634,61640,41,60460,385,60828,60874,146,286,60453,61423,37,80,95,117,29,371,31,356,60412,60517,60434,60437,96,327,60475,60637,60619,393,60478,60487,60564,14,12,85,33,45,314,384,32,60433,38,374,71,369,56,61051,61053,61505,61622,84,388,60445,255,256,60804,60867,61550,61758,61564,74,60742,60743,60745,39,394,50,322,60423,60532,73,283,61638,61639,197,61553,61571,61770,260,60602,61462,61803,51,380,397,60447,60510,54,325,395,60435,61752,61840,61841,61842,61741,61742,61743,61746,303,60506,60523,60620,386,60399,61045,61804,61836,60457,61717,61822,61440,61725,61744,61745,128,60426,61427,61805,60650,60785,61636,61648,61499,61529,61655,61656,185,61729,61783,61810,61721,61824,61825,61835,61719,61787,61819,61826,184,60633,60787,61464,61637,61795,61816,61839,60626,61535,61562,61731,60797,61539,61541,61718,60798,61530,61794,61809,60799,61509,61511,61526,60800,61093,61520,61551,60801,61560,61567,61650,60802,61506,61559,61775,60803,61496,61508,61515,64,373,60581,60738,60482,60483,60488,60577,61845,60751,61428,61763,61833,271,302,61633,61747,59,258,216,60508,61438,61793,357,60489,61061,61062,61063,61064,61065,61066,61067,61554,61047,61049,61050,61691,61769,61792,61799,61800,61807,61808,61814,61605,61610,61617,61623,61621,61624,61627,61594,61596,61599,61619,61625,61626,61620,61632,61760,61677,61678,61679,61817,61671,61681,61683,61838,61672,61673,61674,61675,61658,61823,61827,61828,61684,61688,61704,61756,61703,61712,61777,61772,61773,61774,61802,61798,61705,61707,61709,61592,61686,61687,61692,61730,61501,61503,61510,61514,60822,60825,60827,60830,60832,60837,60842,60844,60846,60849,60850,60852,60853,60855,60857,60858,60859,60861,60937,60938,60939,60940,60941,60942,60943,60944,60945,60946,60947,60948,60949,60950,60951,60952,60953,60954,60955,60956,60957,60958,60959,60960,60961,60962,60963,60964,60965,60966,60967,60968,60969,60970,60971,60972,60973,60974,60975,60976,60977,60978,60979,60980,60981,60982,60983,60984,60985,60986,60987,60988,60989,60990,60991,60992,60993,60994,60995,60996,60997,60998,60999,61000,61001,61002,61003,61004,61005,61006,61007,61008,61009,61010,61011,61012,61013,61014,61015,61016,61017,61018,61019,61020,61021,61022,61023,61025,61026,61487,61493,61284,61447,61461,61481,61569,61570,61606,61659,61755,61722,61395,61396,61397,61398,61399,61400,61401,61402,61403,61404,61405,61406,61407,61408,61409,61410,61411,61412,61413,61414,61415,61416,61417,61418,61419,61420,61421,61137,61138,61139,61140,61141,61142,61143,61144,61145,61146,61147,61148,61149,61150,61151,61152,61153,61154,61155,61156,61157,61158,61159,61160,61161,61162,61163,61164,61165,61166,61167,61168,61169,61170,61171,61172,61173,61174,61175,61176,61177,61178,61179,61180,61181,61182,61183,61184,61185,61186,61187,61188,61189,61190,61191,61192,61193,61194,61195,61196,61197,61198,61199,61200,61201,61202,61203,61204,61205,61206,61732,61754 };

				int nRandomSeed = rand()%593;
				int iAccountDBID = iAccountDBIDArray[nRandomSeed];
				INT64 biCharacterDBID = biCharacterDBIDArray[nRandomSeed];
				g_pDivisionManager->AddUser( 0, 0, 0, 0, iAccountDBID, 0, biCharacterDBID, L"test", L"test", false, "127.0.0.1", "127.0.0.1", 0, NULL, 0, 0 );
				g_pDivisionManager->RequestGameRoom( REQINFO_TYPE_SINGLE, iAccountDBID, 10000, vDefaultMapIndex[rand()%vDefaultMapIndex.size()], 0, 2, true, false, 0);

				Sleep(3000);
				/*
				if( rand()%nRandDelay != 0 ) 
					Sleep( rand()%nLoop1 );
				else 
					Sleep(nLoop2);
				*/
			}
		}
#endif
		
		printf("CMD>");
        std::cin >> szCmd;
	}

	SavePartyID();
	SaveVoiceChannelID();	

	ClearApp();

#if defined(_WORK)
	SAFE_DELETE(g_PsUpdater);
#endif	// #if defined(_WORK)

	return 0;
}

