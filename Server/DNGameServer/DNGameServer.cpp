// DNFieldServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FireWall.h"
#include "IniFile.h"
#include "DNIocpManager.h"
#include "DNDBConnectionManager.h"
#include "DNMasterConnectionManager.h"
#include "DNGameServerManager.h"
#include "DNGameDataManager.h"
#include "Log.h"
#include "Util.h"
#include "DnMainFrame.h"
#include "DNQuestManager.h"
#include "ExceptionReport.h"
#include "MAAiScript.h"
#include "DnScriptManager.h"
#include "DNRUDPGameServer.h"
#include "DNTalk.h"
#include "DNLogConnection.h"
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#include "DNGameWorldUserState.hpp"
#else
#include "DNWorldUserState.h"
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#include "DNGameServerScriptAPI.h"
#include "BugReporter.h"
#include "DNServiceConnection.h"
#include "DNQuest.h"
#include "DNEvent.h"
#include "Version.h"
#include "MemPool.h"
#include "DNMissionScheduler.h"
#include "NoticeSystem.h"
#include "PerfCheck.h"
#include "DNAuthManager.h"
#include "DNGuildSystem.h"
#include "MAScanner.h"
#ifdef _USE_VOICECHAT
#include "DNVoiceChat.h"
#endif
#include "ServiceUtil.h"
#include "CloseSystem.h"
#include "MasterSystemCacheRepository.h"
#include "DNSecure.h"
#if defined(_WORK)
#include "PsUpdater.h"
#include "EtActionCoreMng.h"
#endif	// #if defined(_WORK)
#include "DNCashConnection.h"
#include "DNChatRoomManager.h"
#include "DNPeriodQuestSystem.h"
#include "DNGuildRecruitCacheRepository.h"
#ifdef PRE_ADD_LIMITED_CASHITEM
#include "DNLimitedCashItemRepository.h"
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
#if defined( PRE_PRIVATECHAT_CHANNEL )
#include "DNPrivateChatChannel.h"
#include "DnPrivateChatManager.h"
#endif

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

#ifdef _AUTH_
#include "RLKTAuth.h"
#endif

#include "rlkt_Revision.h"
//#include "../../Server/RLKT_LICENSE/license_rlkt.h"

struct AssertReportMode {
	AssertReportMode() {
		_set_error_mode( _OUT_TO_MSGBOX );
	}
}assertReportMode;

#if !defined(_FINAL_BUILD)
CLog g_ScriptLog;
#endif

TGameConfig g_Config;

#if defined (_WORK)
ConfigWork g_ConfigWork;
#endif // #if defined (_WORK)

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

int AddToFirewall()
{
	HRESULT hr = S_OK;
	HRESULT comInit = E_FAIL;
	INetFwProfile* fwProfile = NULL;

	// Initialize COM.
	comInit = CoInitializeEx(
		0,
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
		);

	// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
	// initialized with a different mode. Since we don't care what the mode is,
	// we'll just use the existing mode.
	if (comInit != RPC_E_CHANGED_MODE)
	{
		hr = comInit;
		if (FAILED(hr))
		{
			printf("CoInitializeEx failed: 0x%08lx\n", hr);
			goto error;
		}
	}

	// Retrieve the firewall profile currently in effect.
	hr = WindowsFirewallInitialize(&fwProfile);
	if (FAILED(hr))
	{
		printf("WindowsFirewallInitialize failed: 0x%08lx\n", hr);
		goto error;
	}

	WCHAR wszFileName[ _MAX_PATH ];
	HMODULE hModule;
	hModule = GetModuleHandle( NULL );
	GetModuleFileNameW( hModule, wszFileName, _MAX_PATH );
	// Add Windows Messenger to the authorized application collection.
	hr = WindowsFirewallAddApp(
		fwProfile,
		wszFileName,
		L"Dragon Nest"
		);
	if (FAILED(hr))
	{
		printf("WindowsFirewallAddApp failed: 0x%08lx\n", hr);
		goto error;
	}

error:

	// Release the firewall profile.
	WindowsFirewallCleanup(fwProfile);

	// Uninitialize COM.
	if (SUCCEEDED(comInit))
	{
		CoUninitialize();
	}

	return 0;
}

bool LoadConfig(int argc, TCHAR * argv[])
{
#if defined(_HSHIELD)
	g_Config.hHSServer = ANTICPX_INVALID_HANDLE_VALUE;
#endif	// _HSHIELD
	
	memset(&g_Config, 0, sizeof(TGameConfig));

	WCHAR wszBuf[128] = { 0, }, wszStr[64] = { 0, };
	char szData[128] = { 0, };
	memset(wszBuf, 0, sizeof(wszBuf));
	memset(szData, 0, sizeof(szData));

	//Version
	_strcpy(g_Config.szVersion, _countof(g_Config.szVersion), szGameVersion, (int)strlen(szGameVersion));
	_strcpy(g_Config.szResVersion, _countof(g_Config.szResVersion), "Unknown Version", (int)strlen("Unknown Version"));

	wstring wszFileName = L"./Config/DNGame.ini";

	if (!g_IniFile.Open(wszFileName.c_str())){
		g_Log.Log(LogType::_FILELOG, L"DNGame.ini File not Found!!\r\n");
		return false;
	}

	//rlkt_assert
	int iEnableAssert = 0;
	g_IniFile.GetValue(L"Debug", L"Assert", &iEnableAssert);
	if (iEnableAssert > 0)
		EnableAssert(true);

#if defined( _WORK )
	int iEnableAssert=0;
	g_IniFile.GetValue(L"Debug", L"Assert", &iEnableAssert );
	if( iEnableAssert > 0 )
		EnableAssert( true );

	int iDisableFarm=0;
	g_IniFile.GetValue(L"Debug", L"DisableFarm", &iDisableFarm );
	if( iDisableFarm > 0 )
		g_Config.bDisableFarm = true;
#endif // #if defined( _WORK )

	g_Config.cAffinityType = _GAMESERVER_AFFINITYTYPE_HYBRYD;

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

		if (GetFirstRightValue(L"pcc", argv[1], wstrTempConfig))
			g_Config.nCreateCount = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

		if (GetFirstRightValue(L"pci", argv[1], wstrTempConfig))
			g_Config.nCreateIndex = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

		if (g_Config.nCreateCount - 1 < g_Config.nCreateIndex)
		{
			g_Log.Log(LogType::_FILELOG, L"Process CreateInfo Error (Count:%d, Index:%d)\r\n", g_Config.nCreateCount, g_Config.nCreateIndex);
			return false;
		}

		g_Log.Log(LogType::_FILELOG, L"Process CreateInfo (Count:%d, Index:%d)\r\n", g_Config.nCreateCount, g_Config.nCreateIndex);

		if (GetFirstRightValue(L"smc", argv[1], wstrTempConfig))
			g_Config.nIocpMax = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

		if (GetFirstRightValue(L"gpl", argv[1], wstrTempConfig))
			g_Config.bPreLoad = _wtoi(wstrTempConfig.c_str()) > 0 ? true : false;
		else _ASSERT(0);

		if (GetFirstRightValue(L"gucp", argv[1], wstrTempConfig))
			g_Config.nGameAcceptPortBegin = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

		if (GetFirstRightValue(L"title", argv[1], wstrTempConfig))
			SetConsoleTitleW(wstrTempConfig.c_str());

		if (GetFirstRightValue(L"gtcp", argv[1], wstrTempConfig))
			g_Config.nClientAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);
		g_Log.Log(LogType::_FILELOG, L"SocketMax:%d PreLoad:%s RUDPStartPort:%d TCPPort:%d\n", g_Config.nIocpMax, g_Config.bPreLoad == true ? L"TRUE" : L"FALSE", g_Config.nGameAcceptPortBegin, g_Config.nClientAcceptPort);

#ifdef PRE_MOD_OPERATINGFARM
		if (GetFirstRightValue(L"gsat", argv[1], wstrTempConfig))
			g_Config.cAffinityType = _wtoi(wstrTempConfig.c_str());
#endif		//#ifdef PRE_MOD_OPERATINGFARM

		GetDefaultConInfo(argv[1], L"log", &g_Config.LogInfo, 1);
		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);

		GetDefaultConInfo(argv[1], L"cash", &g_Config.CashInfo, 1);
		g_Log.Log(LogType::_FILELOG, L"CashInfo (Ip:%S, Port:%d)\r\n", g_Config.CashInfo.szIP, g_Config.CashInfo.nPort);

		GetDefaultConInfo(argv[1], L"master", g_Config.MasterInfo, WORLDCOUNTMAX);
		for (int h = 0; h < WORLDCOUNTMAX; h++)
		{
			if (g_Config.MasterInfo[h].nPort <= 0) continue;
			g_Log.Log(LogType::_FILELOG, L"MasterCon Ip:%S, Port:%d\r\n", g_Config.MasterInfo[h].szIP, g_Config.MasterInfo[h].nPort);
			g_Config.nMasterCount++;
		}

		GetDefaultConInfo(argv[1], L"db", g_Config.DBInfos, DBSERVERMAX);
		for (int h = 0; h < DBSERVERMAX; h++)
		{
			if (g_Config.DBInfos[h].nPort <= 0) continue;
			g_Log.Log(LogType::_FILELOG, L"DBCon Ip:%S, Port:%d\r\n", g_Config.DBInfos[h].szIP, g_Config.DBInfos[h].nPort);
			g_Config.nDBCount++;
		}

#if defined( PRE_FIX_WORLDCOMBINEPARTY )
		if (GetFirstRightValue(L"gcwg", argv[1], wstrTempConfig))
			g_Config.bWorldCombineGameServer = _wtoi(wstrTempConfig.c_str()) > 0 ? true : false;
#endif

		GetDolbyAxonInfo(argv[1], g_Config.szPrivateDolbyIp, g_Config.szPublicDolbyIp, g_Config.nAudioPort, g_Config.nControlPort);
		g_Log.Log(LogType::_FILELOG, L"DolbyAxon Info PrivateIP:%S, PublicIP:%S, APort:%d CPort:%d\r\n", g_Config.szPrivateDolbyIp, g_Config.szPublicDolbyIp, g_Config.nAudioPort, g_Config.nControlPort);
	}
	else
	{
		int nTemp = 1;
		g_IniFile.GetValue(L"Info", L"PreLoad", &nTemp);
		g_Config.bPreLoad = ( nTemp == 0 ) ? false : true;

		g_IniFile.GetValue(L"Info", L"OpenCount", &g_Config.nGameServerOpenCount);
		g_IniFile.GetValue(L"Info", L"StartPort", &g_Config.nGameAcceptPortBegin);
		g_IniFile.GetValue(L"Info", L"TcpPort", &g_Config.nClientAcceptPort);
		_ASSERT(g_Config.nClientAcceptPort != 0 && "Check Game.ini needs TcpPort");

		g_IniFile.GetValue(L"Info", L"IocpMax", &g_Config.nIocpMax);
		g_Log.Log(LogType::_FILELOG, L"IocpMax:%d\r\n", g_Config.nIocpMax);

		std::vector<std::string> Tokens;

		g_IniFile.GetValue(L"Connection", L"Cash", wszBuf);
		if (wszBuf[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, szData, sizeof(szData), NULL, NULL);

			Tokens.clear();
			TokenizeA(szData, Tokens, ":");
			if (!Tokens.empty()){
				_strcpy(g_Config.CashInfo.szIP, _countof(g_Config.CashInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
				g_Config.CashInfo.nPort = atoi(Tokens[1].c_str());
			}
		}

		g_Log.Log(LogType::_FILELOG, L"Cash(%S, %d)\r\n", g_Config.CashInfo.szIP, g_Config.CashInfo.nPort);

		g_IniFile.GetValue(L"Connection", L"MasterCount", &g_Config.nMasterCount);

		for (int i = 0; i < g_Config.nMasterCount; i++){
			swprintf(wszStr, L"Master%d", i + 1);
			g_IniFile.GetValue(L"Connection", wszStr, wszBuf);
			if (wszBuf[0] != '\0'){
				WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, szData, sizeof(szData), NULL, NULL);

				Tokens.clear();
				TokenizeA(szData, Tokens, ":");
				if (!Tokens.empty()){
					_strcpy(g_Config.MasterInfo[i].szIP, _countof(g_Config.MasterInfo[i].szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
					g_Config.MasterInfo[i].nPort = atoi(Tokens[1].c_str());
				}
			}
		}

		g_IniFile.GetValue(L"Connection", L"DBCount", &g_Config.nDBCount);

		for (int i = 0; i < g_Config.nDBCount; i++){
			swprintf(wszStr, L"DB%d", i + 1);
			g_IniFile.GetValue(L"Connection", wszStr, wszBuf);
			if (wszBuf[0] != '\0'){
				WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, szData, sizeof(szData), NULL, NULL);

				Tokens.clear();
				TokenizeA(szData, Tokens, ":");
				if (!Tokens.empty()){
					_strcpy(g_Config.DBInfos[i].szIP, _countof(g_Config.DBInfos[i].szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
					g_Config.DBInfos[i].nPort = atoi(Tokens[1].c_str());
				}
			}
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

		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);		

		WCHAR wszProbeStr[128] = {0,};	
		g_IniFile.GetValue(L"Probe", L"ProbePort", &g_Config.nProbePort);
		g_IniFile.GetValue(L"Probe", L"ProbeIP", wszProbeStr);
		if (wszProbeStr[0] != '\0')
			WideCharToMultiByte(CP_ACP, 0, wszProbeStr, (int)wcslen(wszProbeStr), g_Config.szProbeIP, sizeof(g_Config.szProbeIP), NULL, NULL);

		// ResourcePath 등록해준다.
		WCHAR wszPath[_MAX_PATH] = { 0, };
		char szPath[_MAX_PATH] = { 0, };
		g_IniFile.GetValue( L"Resource", L"Path", wszPath );
		if (wszPath[0] != '\0')
			WideCharToMultiByte(CP_ACP, NULL, wszPath, -1, szPath, sizeof(szPath), NULL, NULL);
		g_Config.szResourcePath = szPath;
		if( g_Config.szResourcePath.empty() ) g_Config.szResourcePath = ".";

		WCHAR wszResNation[128] = { 0, };
		char szResNation[128] = { 0, };
		g_IniFile.GetValue( L"Resource", L"Nation", wszResNation );
		if (wszResNation[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszResNation, -1, szResNation, sizeof(szResNation), NULL, NULL);
			if( strlen(szResNation) > 0 ) {
				g_Config.szResourceNation = "_";
				g_Config.szResourceNation += szResNation;
			}
		}

		g_IniFile.GetValue(L"DolbyInfo", L"PrivateIp", wszBuf);
		if (wszBuf[0] != '\0')
			WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, szData, sizeof(szData), NULL, NULL);
		_strcpy(g_Config.szPrivateDolbyIp, _countof(g_Config.szPrivateDolbyIp), szData, (int)strlen(szData));

		g_IniFile.GetValue(L"DolbyInfo", L"PublicIp", wszBuf);
		if (wszBuf[0] != '\0')
			WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, szData, sizeof(szData), NULL, NULL);
		_strcpy(g_Config.szPublicDolbyIp, _countof(g_Config.szPublicDolbyIp), szData, (int)strlen(szData));

		g_IniFile.GetValue(L"DolbyInfo", L"ControlPort", &g_Config.nControlPort);
		g_IniFile.GetValue(L"DolbyInfo", L"AudioPort", &g_Config.nAudioPort);
		g_Log.Log(LogType::_FILELOG, L"DolbyInfo (PrivateIp:%S, PublicIP:%S, CPort:%d, APort:%d)\r\n", g_Config.szPrivateDolbyIp, g_Config.szPublicDolbyIp, g_Config.nControlPort, g_Config.nAudioPort);

#if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
		int nCommandMax = 0;
		g_IniFile.GetValue(L"Work", L"CommandMax", &nCommandMax);

		wchar_t wszValueName[128] = {0};
		for (int i = 1; i <= nCommandMax; ++i)
		{
			wsprintf(wszValueName, L"Command%d", i);
			g_IniFile.GetValue(L"Work", wszValueName, wszBuf);
			if (wszBuf[0] == 0)
				continue;

			g_ConfigWork.AddCommand(wszBuf);
		}
#endif // #if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
#if defined( PRE_FIX_WORLDCOMBINEPARTY )
		g_IniFile.GetValue(L"Info", L"WorldCombineServer", &nTemp);
		g_Config.bWorldCombineGameServer = ( nTemp == 0 ) ? false : true;
#endif
	}

    g_IniFile.GetValue(L"ServerManagerEx", L"sid", &g_Config.nManagedID);
    g_IniFile.GetValue(L"ServerManagerEx", L"ip", wszBuf);
    WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.ServiceInfo.szIP, sizeof(g_Config.ServiceInfo.szIP), NULL, NULL);
    g_IniFile.GetValue(L"ServerManagerEx", L"port", &g_Config.ServiceInfo.nPort);

	return true;
}

bool LoadNpcQuest()
{
#if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
	if (g_ConfigWork.HasCommand(L"ExceptScript"))
		return true;
#endif // #if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)

	g_pNpcQuestScriptManager->CreateLuaState(g_pGameServerManager->GetThreadCount());
	// 각 쓰레드별로 npc 대화 & 퀘스트 관련 스크립트들을 전부 로딩 한다.
	for ( int i = 0 ; i < g_pGameServerManager->GetThreadCount(); i++ )
	{
		lua_State* pLuaState = g_pNpcQuestScriptManager->OpenStateByIndex(i);
		DefAllAPIFunc(pLuaState);

		//-----------------------------------------------------------------------------------------------------------
		// 스크립트 공통 파일 먼저 로드
		std::vector<CFileNameString> CommonFileList;
		g_Log.Log(LogType::_FILELOG, L"QuestNPC_Common...Folder : %S\n", CEtResourceMng::GetInstance().GetFullPath( "QuestNPC_Common" ).c_str());
		CEtResourceMng::GetInstance().FindFileListAll_IgnoreExistFile( "QuestNPC_Common", "*.lua", CommonFileList );

		for ( int j = 0 ; j < (int)CommonFileList.size() ; j++ )
		{
			bool bResult = g_pNpcQuestScriptManager->LoadScript(CEtResourceMng::GetInstance().GetFullName(CommonFileList[j].c_str()).c_str(), false, i, false);
			if ( !bResult )
			{
				g_Log.Log(LogType::_FILELOG, L"CommonLuaFile Load FAILED...\n");
				g_Log.Log(LogType::_FILELOG, L"%S\n", CEtResourceMng::GetInstance().GetFullName(CommonFileList[j].c_str()).c_str());
			}
		}
		//-----------------------------------------------------------------------------------------------------------


		g_Log.Log(LogType::_FILELOG, L"LoadScript Data...\n");
		g_Log.Log(LogType::_FILELOG, L"Talk_Quest...Folder : %S\n", CEtResourceMng::GetInstance().GetFullPath( "Talk_Quest" ).c_str());


		std::vector<CFileNameString> FileList;
		//		FindFileListInDirectory(CEtResourceMng::GetInstance().GetFullPath( "Talk_Quest" ).c_str(), "*.lua", FileList);
		CEtResourceMng::GetInstance().FindFileListAll_IgnoreExistFile( "Talk_Quest", "*.lua", FileList );

		for ( int j = 0 ; j < (int)FileList.size() ; j++ )
		{
			bool aRetVal = g_pNpcQuestScriptManager->LoadScript(CEtResourceMng::GetInstance().GetFullName(FileList[j].c_str()).c_str(), true, i, true);
			if (!aRetVal) {
				g_Log.Log(LogType::_FILELOG, L"LoadNpcQuest() Failed !!! - g_pNpcQuestScriptManager->LoadScript(...)\n");
				return false;
				//				DN_RETURN(false);
			}
		}

		int nColor = _GREEN;
		if ( FileList.empty() )
		{
			nColor = _RED;
		}

		g_Log.Log(LogType::_FILELOG, L"Talk_Quest...Size : %d\n", (int)FileList.size());
		g_Log.Log(LogType::_FILELOG, L"LoadScript Data...\n");
		g_Log.Log(LogType::_FILELOG, L"Talk_Npc...Folder : %S\n", CEtResourceMng::GetInstance().GetFullPath( "Talk_Npc" ).c_str());

		FileList.clear();
		//		FindFileListInDirectory(CEtResourceMng::GetInstance().GetFullPath( "Talk_Npc" ).c_str(), "*.lua", FileList);
		CEtResourceMng::GetInstance().FindFileListAll_IgnoreExistFile( "Talk_Npc", "*.lua", FileList );

		for ( int j = 0 ; j < (int)FileList.size() ; j++ )
		{
			g_pNpcQuestScriptManager->LoadScript(CEtResourceMng::GetInstance().GetFullName(FileList[j].c_str()).c_str(), true, i, false);
		}

		nColor = _GREEN;
		if ( FileList.empty() )
		{
			nColor = _RED;
		}
		g_Log.Log(LogType::_FILELOG, L"Talk_Npc...Size : %d\n", (int)FileList.size());
	}

	return true;
}

bool InitApp(int argc, TCHAR * argv[])
{
	//if (!CheckSerial()) return false;
	WCHAR wszLogName[128], wszScriptLogName[128] = { 0, };
	memset(wszLogName, 0, sizeof(wszLogName));
#if defined( PRE_TRIGGER_LOG )
	WCHAR wszTriggerLogName[128];
#endif // #if defined( PRE_TRIGGER_LOG )

	if (argc >= 2)
	{
		if (IsUseCmd(argv[1]))
		{
			int nSID = 0;
			std::wstring wstrTempArgv;

			if (GetFirstRightValue(L"sid", argv[1], wstrTempArgv))
				nSID = _wtoi(wstrTempArgv.c_str());
			swprintf(wszLogName, L"GameServer_%d", nSID);
			swprintf(wszScriptLogName, L"GameServerScript_%d", nSID);
#if defined( PRE_TRIGGER_LOG )
			swprintf(wszTriggerLogName, L"TriggerLog_%d", nSID);
#endif // #if defined( PRE_TRIGGER_LOG )
		}
		else
		{
			swprintf(wszLogName, L"GameServer");
			swprintf(wszScriptLogName, L"GameServerScript");
#if defined( PRE_TRIGGER_LOG )
			swprintf(wszTriggerLogName, L"TriggerLog");
#endif // #if defined( PRE_TRIGGER_LOG )
		}
	}
	else
	{
		swprintf(wszLogName, L"GameServer");
		swprintf(wszScriptLogName, L"GameServerScript");
#if defined( PRE_TRIGGER_LOG )
		swprintf(wszTriggerLogName, L"TriggerLog");
#endif // #if defined( PRE_TRIGGER_LOG )
	}

#if defined(_FINAL_BUILD)
	g_Log.Init(wszLogName, LOGTYPE_FILE_HOUR);
#else
	g_Log.Init(wszLogName, LOGTYPE_CRT_FILE_HOUR);
#endif
	g_Log.SetServerID(g_Config.nManagedID);

#if defined( PRE_TRIGGER_LOG )
	g_TriggerLog.Init( wszTriggerLogName, LOGTYPE_CRT_FILE_TRIGGER_TEST );
#endif // #if defined( PRE_TRIGGER_LOG )

#if defined( PRE_TRIGGER_UNITTEST_LOG )
	WCHAR wszTriggerUnitTestLogName[MAX_PATH];
	if (argc > 14)
		swprintf( wszTriggerUnitTestLogName, L"TriggerUnitTestLog_%s", argv[4] );
	else
		swprintf( wszTriggerUnitTestLogName, L"TriggerUnitTestLog" );
	
	g_TriggerUnitTestLog.Init( wszTriggerUnitTestLogName, LOGTYPE_FILE_HOUR );
#endif // #if defined( PRE_TRIGGER_UNITTEST_LOG )

#if defined( PRE_QUESTSCRIPT_LOG )
	WCHAR wszQuestLogName[MAX_PATH];
	if (argc > 14)
		swprintf( wszQuestLogName, L"QuestLog_%s", argv[4] );
	else
		swprintf( wszQuestLogName, L"QuestLog" );

	g_QuestLog.Init( wszQuestLogName, LOGTYPE_FILE_HOUR );
#endif // #if defined( PRE_QUESTSCRIPT_LOG )

#if !defined(_FINAL_BUILD)
	g_ScriptLog.Init(wszScriptLogName, LOGTYPE_FILE_HOUR);
#endif

#if defined(_HSHIELD)
	g_Config.hHSServer = _AhnHS_CreateServerObject("./DragonNest.hsb");

	if (g_Config.hHSServer == ANTICPX_INVALID_HANDLE_VALUE){
		g_Log.Log(LogType::_FILELOG, L"_AhnHS_CreateServerObject Failed %d\n", g_Config.hHSServer);
		return false;
	}
#endif	// _HSHIELD

	//RLKT AUTH
#ifdef _AUTH_
	RLKTAuth::GetInstance().Main();
#endif
	//
	CDNSecure::CreateInstance();

	CDnActorActionStateCache::CreateInstance();

	g_pQuestManager = new CDNQuestManager();
	if ( !g_pQuestManager ) return false;
	g_pQuestManager->LoadAllQuest("QuestTable.ext", g_pNpcQuestScriptManager);	

	g_pDataManager = new CDNGameDataManager;
	if (!g_pDataManager) return false;
	if( !g_pDataManager->AllLoad() )
		return false;

	if( !g_AiScriptLoader.AllLoadScript() )
		return false;

	g_pIocpManager = new CDNIocpManager;
	if (!g_pIocpManager) return false;	


	g_pGuildManager = new CDNGuildSystem;
	if (!g_pGuildManager) {
		return false;
	}
	DWORD aRetVal = g_pGuildManager->Open();
	if (NOERROR != aRetVal) {
		return false;
	}

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	g_pWorldUserState = new CDNGameWorldUserState;
#else
	g_pWorldUserState = new CDNWorldUserState;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	if (!g_pWorldUserState)	return false;

	g_pEvent = new CDNEvent;
	if (!g_pEvent) return false;

	// PeriodQuest System
	g_pPeriodQuestSystem = new CDNPeriodQuestSystem;
	if (!g_pPeriodQuestSystem) return false;
	if( !g_pPeriodQuestSystem->Initialize() )
		return false;

	g_pNoticeSystem = new CDNNoticeSystem;
	if (!g_pNoticeSystem) return false;

	g_pCloseSystem = new CCloseSystem;
	if (!g_pCloseSystem) return false;

	g_pMasterConnectionManager = new CDNMasterConnectionManager;
	if (!g_pMasterConnectionManager) return false;

	g_pDBConnectionManager = new CDNDBConnectionManager;
	if (!g_pDBConnectionManager) return false;

	g_pCashConnection = new CDNCashConnection;
	if (!g_pCashConnection) return false;

#ifdef PRE_ADD_LIMITED_CASHITEM
	g_pLimitedCashItemRepository = new CDNLimitedCashItemRepository;
	if (!g_pLimitedCashItemRepository) return false;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

#if defined( PRE_PRIVATECHAT_CHANNEL )
	g_pPrivateChatChannelManager = new CDNPrivateChatManager;
	if(!g_pPrivateChatChannelManager) return false;
#endif

	g_pAuthManager = new CDNAuthManager;
	if (!g_pAuthManager) return false;
	if (!g_pAuthManager->Init()) return false;

	g_pNpcQuestScriptManager = new DnScriptManager();
	if ( !g_pNpcQuestScriptManager ) return false;

	g_pGameServerManager = new CDNGameServerManager;
	if (!g_pGameServerManager) return false;

	CDNMissionScheduler::CreateInstance();
	MasterSystem::CCacheRepository::CreateInstance();
	MAScanner::CreateInstance();

	g_pGameServerManager->PreOpenGameServer();

	CDNTalk::SetScriptManager(g_pNpcQuestScriptManager);
	if (!LoadNpcQuest()) {
		g_Log.Log(LogType::_FILELOG, L"LoadNpcQuest() Failed !!!\r\n");
		return false;
	}

	//init rudp gameserver
	if (g_pGameServerManager->StartGameServer(g_Config.nGameAcceptPortBegin, g_Config.szProbeIP, g_Config.nProbePort) == false)
	{
		g_Log.Log(LogType::_FILELOG, L"RUDP GameServer Start Failed\n");
		return false;
	}

	g_Log.Log(LogType::_FILELOG, L"RUDP GameServer Start ThreadCount : %d\n", g_pGameServerManager->GetThreadCount() );

	if (g_Config.nIocpMax <= 0) g_Config.nIocpMax = 100;

#ifdef _FINAL_BUILD
	if (g_Config.nIocpMax < 2000)
	{
		g_Config.nIocpMax = 2000;
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize NeedMore socket check serverStruct.xml\r\n");
	}
#endif

	if (g_pIocpManager->Init(g_Config.nIocpMax, (2)) < 0){
		g_Log.Log(LogType::_FILELOG,L"Iocp Initialize Fail\r\n");
		return false;
	}	

	// Master Connection
	for (int i = 0; i < g_Config.nMasterCount; i++){
		if (!g_pMasterConnectionManager->AddConnection(g_Config.MasterInfo[i].szIP, g_Config.MasterInfo[i].nPort))
			g_Log.Log(LogType::_FILELOG, L"Master (Ip:%S, Port:%d) AddConnection Fail!!!!\r\n", g_Config.MasterInfo[i].szIP, g_Config.MasterInfo[i].nPort);
		else 
			g_Log.Log(LogType::_FILELOG, L"Master (Ip:%S, Port:%d) AddConnection Success\r\n", g_Config.MasterInfo[i].szIP, g_Config.MasterInfo[i].nPort);
	}
	
	// DB Connection
	for (int i = 0; i < g_Config.nDBCount; i++){
		if (!g_pDBConnectionManager->AddConnection(g_Config.DBInfos[i].szIP, g_Config.DBInfos[i].nPort)){
			g_Log.Log(LogType::_FILELOG, L"DBInfos (Ip:%S, Port:%d) AddConnection Fail!!!!\r\n", g_Config.DBInfos[i].szIP, g_Config.DBInfos[i].nPort);
		}
		g_Log.Log(LogType::_FILELOG, L"DBInfos (Ip:%S, Port:%d) AddConnection Success\r\n", g_Config.DBInfos[i].szIP, g_Config.DBInfos[i].nPort);
	}

	g_pLogConnection = new CDNLogConnection;
	if( !g_pLogConnection ) return false;
	g_pLogConnection->SetIp(g_Config.LogInfo.szIP);
	g_pLogConnection->SetPort(g_Config.LogInfo.nPort);

	// 혹시나 해서.. 쓰레드 일단 다 생성되고.
	Sleep(1000);

	if (g_Config.nManagedID > 0)
	{
		g_pServiceConnection = new CDNServiceConnection(g_Config.nManagedID);
		if (!g_pServiceConnection) return false;
		g_pServiceConnection->SetIp(g_Config.ServiceInfo.szIP);
		g_pServiceConnection->SetPort(g_Config.ServiceInfo.nPort);
	}

#ifdef _USE_VOICECHAT
	if (g_Config.nControlPort > 0)
	{
		g_pVoiceChat = new CDNVoiceChat();
		if (!g_pVoiceChat) return false;
		if (g_pVoiceChat->Initialize(0, "DragonNest", g_Config.szPrivateDolbyIp, g_Config.nControlPort) == false)
			return false;
	}
	else
	{
		g_Log.Log(LogType::_FILELOG, L"_USE_VOICECHAT check VoiceInfo\n");
	}
#endif

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

#else
#if defined(BIT64)
	sprintf(szSvrDir, "./DynCodeBin/Server64");
#else
	sprintf(szSvrDir, "./DynCodeBin/Server");
#endif
	sprintf(szCltDir, "./DynCodeBin/Client");
#endif

	int nBinCount = g_Config.pDynCode->LoadBinary(szSvrDir, szCltDir);

	if (nBinCount == 0){
		g_Log.Log(LogType::_FILELOG, L"Load DynCode failed!!!\r\n");
		return false;
	}

	g_Config.pGpkCmd = g_Config.pDynCode->AllocAuthObject();
	if (g_Config.pGpkCmd == NULL)
	{
		g_Log.Log(LogType::_FILELOG, L"AllocAuthObject Failed!\n");
		return false;
	}

	bool bChAuthRet = g_Config.pDynCode->LoadAuthFile("AuthData.dat");	// CSAuth관련된 애
	if (bChAuthRet == false)
	{
		g_Log.Log(LogType::_FILELOG, L"LoadAuthFile [AuthData.dat] failed!!!\r\n");
		return false;
	}

#endif	// _GPK	

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

	// 2008.01.20 김밥
	// Connection 과 ReconnectThread 간에 동기화 이슈로 ReconnectThread 먼저 종료
	if( g_pIocpManager )
	{
		g_pIocpManager->CloseAcceptors();
		g_pIocpManager->ThreadStop();
		g_pIocpManager->FinalReconnectThread();
	}

	SAFE_DELETE(g_pServiceConnection);
	SAFE_DELETE(g_pLogConnection);
	SAFE_DELETE(g_pQuestManager);
	SAFE_DELETE(g_pNpcQuestScriptManager);
	SAFE_DELETE(g_pCashConnection);
	if (g_pIocpManager)
		g_pIocpManager->Final();
	SAFE_DELETE(g_pDBConnectionManager);
	SAFE_DELETE(g_pMasterConnectionManager);

	SAFE_DELETE( g_pNoticeSystem );
	SAFE_DELETE( g_pCloseSystem );

	SAFE_DELETE(g_pGameServerManager);
	SAFE_DELETE(g_pWorldUserState);
	SAFE_DELETE(g_pEvent);
	if (g_pGuildManager) {
		g_pGuildManager->Close();
		delete g_pGuildManager;
		g_pGuildManager = NULL;
	}
	SAFE_DELETE(g_pIocpManager);
	SAFE_DELETE(g_pDataManager);
	SAFE_DELETE(g_pAuthManager);

#ifdef _USE_VOICECHAT
	SAFE_DELETE(g_pVoiceChat);
#endif
	SAFE_DELETE(g_pPeriodQuestSystem);

	if( MAScanner::IsActive() )
		MAScanner::DestroyInstance();

	if( CDNMissionScheduler::GetInstancePtr() )
		CDNMissionScheduler::DestroyInstance();

	if( MasterSystem::CCacheRepository::GetInstancePtr() )
		MasterSystem::CCacheRepository::DestroyInstance();

#if defined(_HSHIELD)
	if (g_Config.hHSServer != ANTICPX_INVALID_HANDLE_VALUE){
		_AhnHS_CloseServerHandle(g_Config.hHSServer);
		g_Config.hHSServer = ANTICPX_INVALID_HANDLE_VALUE;
	}
#endif	// #if defined(_HSHIELD)

#if defined(_GPK)
	if (g_Config.pDynCode) g_Config.pDynCode->Release();
	if (g_Config.pGpkCmd) g_Config.pGpkCmd->Release();

#endif	// #if defined(_GPK)

	if( CDNSecure::IsActive() )
		CDNSecure::DestroyInstance();

	if( CDnActorActionStateCache::IsActive() )
		CDnActorActionStateCache::DestroyInstance();
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

void ConsoleOutputDebug( const char *szString, ... )
{
	va_list list;
	char szTemp[65535];

	va_start( list, szString );
	vsprintf_s( szTemp, szString, list );
	printf( szTemp );
	va_end( list );
}
void (__stdcall *s_pPrevOutputDebugFunc)(const char*, ...) = NULL;

#if !defined( _FINAL_BUILD )

#define TESTTHREADCOUNT 30

UINT __stdcall LogTestThread(void *pParam)
{
	int iCount =1;
	do
	{
		g_Log.Log( (LogType::eLogType)100, L"LogTest %d %s\r\n", 23, L"test" );
	}while( iCount++ < 10000 );
	return 0;
}

#endif // #if !defined( _FINAL_BUILD )

int _tmain(int argc, TCHAR* argv[])
{
	BOOST_STATIC_ASSERT( eActor::CS_ACTOR_STATE_MAX == eActor::SC_ACTOR_STATE_MAX );
	BOOST_STATIC_ASSERT( eProp::CS_PROP_STATE_MAX == eProp::SC_PROP_STATE_MAX );
	/*
#if defined(_WORK)
	if (!g_PsUpdater)
		g_PsUpdater = new CPsUpdater;

	g_PsUpdater->AsyncUpdate ();
#endif	// #if defined(_WORK)
	*/
#ifndef _FINAL_BUILD
	if( bIsGTest( argc, argv ))
		return RUN_ALL_TESTS();
#endif

#if defined(_WORK)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetBreakAlloc(7311617);
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
//#if !defined(_FINAL_BUILD)
	//unhandledexception 인경우에는 항상 풀메모리덤프
	DWORD dwRetVal = CExceptionReport::GetInstancePtr()->Open(_T(".\\"), TRUE, TRUE, MiniDumpWithFullMemory);	// Release 모드 컴파일 시 C4744 경고가 발생하여 Singleton 구현 변경, CExceptionReport::GetInstancePtr() 을 inline 화 하지 않음 (참고 : http://msdn.microsoft.com/ko-kr/library/a7za416f.aspx)
//#else	// _FINAL_BUILD
//	DWORD dwRetVal = CExceptionReport::GetInstancePtr()->Open(_T(".\\"), TRUE, TRUE);							// Release 모드 컴파일 시 C4744 경고가 발생하여 Singleton 구현 변경, CExceptionReport::GetInstancePtr() 을 inline 화 하지 않음 (참고 : http://msdn.microsoft.com/ko-kr/library/a7za416f.aspx)
//#endif	// _FINAL_BUILD
	if (NOERROR != dwRetVal) {
		DWORD dwErrNo = ::GetLastError();
		DN_RETURN(dwErrNo);
	}
	
	::srand(timeGetTime());
	if (!LoadConfig(argc, argv)){
		g_Log.Log(LogType::_FILELOG, L"LoadConfig Failed\r\n");
		return 0;
	}

	AddToFirewall();
#ifdef NDEBUG
//	SetOutputDebugFuncPtr( NULL );
#endif

	g_Config.bAllLoaded = false;
	if (!CLfhHeap::GetInstance()->InitPool())
	{
		g_Log.Log(LogType::_FILELOG, L"** InitPool Failed!!! Check!!!!!\r\n");
		return 0;
	}

	CDnMainFrame *pMainFrame = new CDnMainFrame;
	pMainFrame->PreInitialize();

	pMainFrame->InitializeDevice();

	if (!InitApp(argc, argv)){
		g_Log.Log(LogType::_FILELOG, L"** InitApp Failed!!! Check!!!!!\r\n");
		ClearApp();
		delete pMainFrame;
		return 0;
	}
	pMainFrame->Initialize();
	g_pIocpManager->VerifyAccept(ACCEPTOPEN_VERIFY_TYPE_APPINITCOMPLETE);
	g_Config.bAllLoaded = true;

	wprintf(L"exit 명령을 치면 종료\r\n");

	//SetConsoleTitleA(FormatA("GameServer Rev.%s", revDNGameServer).c_str()); //rlkt_revision

	char szCmd[256] = {0,};	
	while (1)
	{
		if (strcmp(szCmd, "exit") == 0)	break;		

		if (strcmp(szCmd, "assert") == 0)
		{
			EnableAssert(true);
			printf("Asserts Enabled.\n");
		}

		if (strcmp(szCmd, "disable_assert") == 0)
		{
			EnableAssert(false);
			printf("Asserts Disabled.\n");
		}

		if (strcmp(szCmd, "reload") == 0)
		{
#if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
			g_ConfigWork.RemoveCommand(L"ExceptScript");
#endif // #if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
			g_pDataManager->LoadTalkData();
			g_pQuestManager->LoadAllQuest("QuestTable.ext", g_pNpcQuestScriptManager);
			g_pNpcQuestScriptManager->CloseAllState();
			LoadNpcQuest();
			printf("Reload all script..\n");
		}
		if ( strcmp(szCmd, "quest_list") == 0 )
		{
			std::vector<CDNQuest*> questList;
			g_pQuestManager->DumpAllQuest(questList);
			wprintf(L"Quest list size = %d\n", questList.size());

			for ( size_t i = 0; i < questList.size() ; i++ )
			{
				CDNQuest* pQuest = questList[i];

				wprintf(L"Cnt %d: QuestIndex[%d]\nTitle[%s]\nXML[%s]\nLUA[%s]\n\n", i,
					pQuest->GetQuestInfo().nQuestIndex,
					pQuest->GetQuestInfo().szQuestName.c_str(),
					pQuest->GetQuestInfo().szTalkFileName.c_str(),
					pQuest->GetQuestInfo().szScriptFileName.c_str());
			}
		}

		if ( strcmp(szCmd, "talk_list") == 0 )
		{
			std::vector<std::wstring> talklist;
			g_pDataManager->GetTalkFileList(talklist);

			wprintf(L"talk list size = %d\n", talklist.size());

			for ( size_t i = 0 ; i < talklist.size() ; i++ )
			{
				wprintf(L"%s\n", talklist[i].c_str());
			}

		}
		if (strcmp(szCmd, "enableprofile") == 0)
		{
			s_pPrevOutputDebugFunc = s_pProfileOutputDebugFunc;
//			SetProfileOutputDebugFuncPtr( ConsoleOutputDebug );
			g_bEnableProfile = true;
		}
		if (strcmp(szCmd, "disableprofile") == 0)
		{
			if( s_pPrevOutputDebugFunc ) 
				SetProfileOutputDebugFuncPtr( s_pPrevOutputDebugFunc );
			g_bEnableProfile = false;
		}

#if !defined( _FINAL_BUILD )
		if( strcmp( szCmd, "logtest" ) == 0 )
		{
			CPerformanceLog Perf("LogTest");
			HANDLE hThread[TESTTHREADCOUNT];
			for( int i=0 ; i<TESTTHREADCOUNT ; ++i )
			{
				UINT ThreadID;
				hThread[i] = (HANDLE)_beginthreadex(NULL, 0, &LogTestThread, NULL, 0, &ThreadID);
			}

			WaitForMultipleObjects( TESTTHREADCOUNT, hThread, true, INFINITE );
		}
#endif // #if defined( _FINAL_BUILD )


#ifdef _WORK
		if (!strcmp(szCmd, "reloadext"))
		{
			if (g_pDataManager->AllLoad() == false)
			{
				_DANGER_POINT_MSG(L"reloadext 실패!");
				break;
			}
		}
		if (!strcmp(szCmd, "reloadact"))
		{
			g_ActionCoreMng.ReleaseAllContainer();
		}
#endif		//#ifdef _WORK

		printf("CMD>");

        std::cin >> szCmd;
	}

	SAFE_DELETE( pMainFrame );

	ClearApp();

#if defined(_WORK)
	SAFE_DELETE(g_PsUpdater);
#endif	// #if defined(_WORK)

	return 0;
}
