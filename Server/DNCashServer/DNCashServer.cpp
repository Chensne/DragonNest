// DNCashServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DNConfig.h"
#include "IniFile.h"
#include "Log.h"
#include "ExceptionReport.h"
#include "Util.h"
#include "DNIocpManager.h"
#include "DNServiceConnection.h"
#include "DNSQLManager.h"
#include "DNConnection.h"
#include "DNLogConnection.h"
#include "DNExtManager.h"
#include "DNThreadManager.h"
#include "DNManager.h"
#include "Version.h"
#include "Stream.h"
#include "./EtStringManager/EtUIXML.h"
#include "./EtResManager/EtResourceMng.h"
#include "DNUserDeleteManager.h"
#include "ServiceUtil.h"
#ifdef PRE_ADD_LIMITED_CASHITEM
#include "DNLimitedCashItemRepository.h"
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

#if defined(_WORK)
#include "DNBillingConnection.h"
#elif defined(_KR) || defined(_US)
#include "DNBillingConnectionKR.h"
#elif defined(_KRAZ)
#include "DNBillingConnectionKRAZ.h"
#include "DNActozShield.h"
#elif defined(_JP)
#include "DNBillingConnectionJP.h"
#elif defined(_CH) || defined(_EU)
#include "DNBillingConnectionCH.h"
#elif defined(_TW)
#include "DNBillingConnectionTW.h"
#elif defined(_SG)
#include "DNBillingConnectionSG.h"
#elif defined(_TH)
#include "DNBillingConnectionTH.h"
#elif defined(_ID)
#include "DNBillingConnectionID.h"
#elif defined(_RU)  || defined(_KAMO)  //[OK_Cash]
#include "DNBillingConnectionRU.h"
#endif
#if defined(PRE_FIX_DEBUGSET_INIT)
#include "DebugSet.h"
#endif

TCashConfig g_Config;

bool LoadConfig(int argc, TCHAR * argv[])
{
	memset(&g_Config, 0, sizeof(TCashConfig));
	WCHAR wszBuf[128] = { 0, }, wszStr[64] = { 0, };
	char szData[128] = { 0, };

	//Version
	_strcpy(g_Config.szVersion, _countof(g_Config.szVersion), szCashVersion, (int)strlen(szCashVersion));
	_strcpy(g_Config.szResVersion, _countof(g_Config.szResVersion), "Unknown Version", (int)strlen("Unknown Version"));

	if (!g_IniFile.Open(L"./Config/DNCash.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNCash.ini File not Found!!\r\n");
		return false;
	}

	g_Config.nThreadMax = 8;
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

		if (GetFirstRightValue(L"cp", argv[1], wstrTempConfig))
			g_Config.nAcceptPort = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

		if (GetFirstRightValue(L"cwid", argv[1], wstrTempConfig))
			g_Config.nWorldSetID = _wtoi(wstrTempConfig.c_str());
		else _ASSERT(0);

		if (GetFirstRightValue(L"title", argv[1], wstrTempConfig))
			SetConsoleTitleW(wstrTempConfig.c_str());

#if defined(_KR) || defined(_US)
		if (GetFirstRightValue(L"svrno", argv[1], wstrTempConfig))
			g_Config.nServerNo = _wtoi(wstrTempConfig.c_str());
#endif		//#if defined(_KR) || defined(_US)
		
		g_Log.Log(LogType::_FILELOG, L"CashServer (WorldID:%d, AcceptPort:%d)\r\n", g_Config.nWorldSetID, g_Config.nAcceptPort);

		GetDefaultConInfo(argv[1], L"log", &g_Config.LogInfo, 1);
		g_Log.Log(LogType::_FILELOG, L"LogInfo (Ip:%S, Port:%d)\r\n", g_Config.LogInfo.szIP, g_Config.LogInfo.nPort);

		GetMemberShipInfo(argv[1], &g_Config.MembershipDB);
		g_Log.Log(LogType::_FILELOG, L"MembershipDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s)\r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBID, g_Config.MembershipDB.wszDBName);

		GetWorldInfo(argv[1], g_Config.WorldDB, WORLDCOUNTMAX);
		for (int nCnt = 0; nCnt < WORLDCOUNTMAX; nCnt++)
		{
			if (g_Config.WorldDB[nCnt].nWorldSetID <= 0) continue;
			g_Log.Log(LogType::_FILELOG, L"WorldDB Connect Info(Ip:%S, Port:%d DBID:%s DBName:%s)\r\n", g_Config.WorldDB[nCnt].szIP, g_Config.WorldDB[nCnt].nPort, g_Config.WorldDB[nCnt].wszDBID, g_Config.WorldDB[nCnt].wszDBName);
			g_Config.nWorldDBCount++;
		}
	}
	else
	{
		g_IniFile.GetValue(L"Info", L"ThreadMax", &g_Config.nThreadMax);
		g_IniFile.GetValue(L"Connection", L"AcceptPort", &g_Config.nAcceptPort);
		g_Log.Log(LogType::_FILELOG, L"AcceptPort:%d\r\n", g_Config.nAcceptPort);

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

		std::vector<std::string> Tokens;

		g_IniFile.GetValue(L"Connection", L"Log", wszStr, 64);
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

		// ResourcePath 殿废秦霖促.
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
		g_IniFile.GetValue( L"Resource", L"Nation", wszResNation, 64 );
		if (wszResNation[0] != '\0'){
			WideCharToMultiByte(CP_ACP, NULL, wszResNation, -1, szResNation, sizeof(szResNation), NULL, NULL);
			if( strlen(szResNation) > 0 ) {
				g_Config.szResourceNation = "_";
				g_Config.szResourceNation += szResNation;
			}
		}
	}

#if defined(_KR)
	if (!g_IniFile.Open(L"./Config/DNNexonBilling.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNNexonBilling.ini File not Found!!\r\n");
		return false;
	}
	std::vector<std::string> Tokens;

	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"NexonBilling", L"Billing", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.CashInfo.szIP, _countof(g_Config.CashInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.CashInfo.nPort = atoi(Tokens[1].c_str());
		}
	}

	if (g_Config.nServerNo <= 0 && g_Config.bUseCmd)
		g_Config.nServerNo = g_Config.nWorldSetID - 1;
	else
		g_IniFile.GetValue(L"NexonBilling", L"ServerNo", &g_Config.nServerNo);

	g_Log.Log(LogType::_FILELOG, L"CashInfo (Ip:%S, Port:%d, SN:%d)\r\n", g_Config.CashInfo.szIP, g_Config.CashInfo.nPort, g_Config.nServerNo);
	
#elif defined(_KRAZ)
	// 胶抛捞隆锭巩俊 test牢瘤父 眉农窍妨绊 ini 佬澜. 促弗扒 鞘夸绝澜
	if (g_IniFile.Open(L"./Config/ActozCommonDB.ini")){
		memset(&wszBuf, 0, sizeof(wszBuf));

		g_IniFile.GetValue(L"Info", L"ServerType", wszBuf);
		if (wcscmp(wszBuf, L"Test") == 0)
			g_Config.bTestServer = true;
	}

#elif defined(_JP)
	if (!g_IniFile.Open(L"./Config/DNNHNJPBilling.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNNHNJPBilling.ini File not Found!!\r\n");
		return false;
	}

	g_IniFile.GetValue(L"Billing", L"ServiceType", g_Config.wszServiceType);
	g_Log.Log(LogType::_FILELOG, L"NHNJPBilling ServerType:%s\r\n", g_Config.wszServiceType);

#elif defined(_TW) 
	if (!g_IniFile.Open(L"./Config/DNGamaniaBilling.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNGamaniaBilling.ini File not Found!!\r\n");
		return false;
	}

	std::vector<std::string> Tokens;

	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"GamaniaBilling", L"QueryServer", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);	

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.QueryPointInfo.szIP, _countof(g_Config.QueryPointInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.QueryPointInfo.nPort = atoi(Tokens[1].c_str());
		}
	}

	g_IniFile.GetValue(L"GamaniaBilling", L"ShopItemServer", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.ShopItemInfo.szIP, _countof(g_Config.ShopItemInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.ShopItemInfo.nPort = atoi(Tokens[1].c_str());
		}
	}

	g_IniFile.GetValue(L"GamaniaBilling", L"CouponServer", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.CouponInfo.szIP, _countof(g_Config.CouponInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.CouponInfo.nPort = atoi(Tokens[1].c_str());
		}
	}

	g_IniFile.GetValue(L"GamaniaBilling", L"CouponRollBack", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);
		
		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.CouponRollBackInfo.szIP, _countof(g_Config.CouponRollBackInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.CouponRollBackInfo.nPort = atoi(Tokens[1].c_str());	
		}
	}

	g_IniFile.GetValue(L"GamaniaBilling", L"ProtocolType", &g_Config.nProtocolType, 1);

#elif defined(_SG)
	if (!g_IniFile.Open(L"./Config/DNCherryBilling.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNCherryBilling.ini File not Found!!\r\n");
		return false;
	}
	
	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"CherryBilling", L"CouponServer", wszStr, 64);
	if (wszStr[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);
	g_Config.CouponServerInfo = szData;

	g_IniFile.GetValue(L"CherryBilling", L"AuthToken", wszStr, 64);
	if (wszStr[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);
	g_Config.AuthToken = szData;

#elif defined(_US)
	if (!g_IniFile.Open(L"./Config/DNNexonBillingUS.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNNexonBillingUS.ini File not Found!!\r\n");
		return false;
	}

	std::vector<std::string> Tokens;

	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"NexonBilling", L"Billing", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.CashInfo.szIP, _countof(g_Config.CashInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.CashInfo.nPort = atoi(Tokens[1].c_str());
		}
	}

	if (g_Config.nServerNo <= 0 && g_Config.bUseCmd){
		g_Config.nServerNo = g_Config.nWorldSetID - 1;
	}
	else
		g_IniFile.GetValue(L"NexonBilling", L"ServerNo", &g_Config.nServerNo);

	g_Log.Log(LogType::_FILELOG, L"CashInfo (Ip:%S, Port:%d, SN:%d)\r\n", g_Config.CashInfo.szIP, g_Config.CashInfo.nPort, g_Config.nServerNo);

#elif defined(_TH)
	if (!g_IniFile.Open(L"./Config/DNAsiaSoftBilling.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNAsiaSoftBilling.ini File not Found!!\r\n");
		return false;
	}
	std::vector<std::string> Tokens;

	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"AsiaSoftBilling", L"QueryServer", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);	

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.QueryPointInfo.szIP, _countof(g_Config.QueryPointInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.QueryPointInfo.nPort = atoi(Tokens[1].c_str());
		}
	}

	g_IniFile.GetValue(L"AsiaSoftBilling", L"ShopItemServer", wszStr, 64);
	if (wszStr[0] != '\0'){
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);	

		Tokens.clear();
		TokenizeA(szData, Tokens, ":");
		if (!Tokens.empty()){
			_strcpy(g_Config.ShopItemInfo.szIP, _countof(g_Config.ShopItemInfo.szIP), Tokens[0].c_str(), (int)strlen(Tokens[0].c_str()));
			g_Config.ShopItemInfo.nPort = atoi(Tokens[1].c_str());	
		}
	}
#elif defined(_ID)
	if (!g_IniFile.Open(L"./Config/DNKreonBilling.ini")){
		g_Log.Log(LogType::_FILEDBLOG, L"DNKreonBilling.ini File not Found!!\r\n");
		return false;
	}
	memset(&wszStr, 0, sizeof(wszStr));
	g_IniFile.GetValue(L"KreonBilling", L"QueryPointURL", wszStr, 64);
	if (wszStr[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);
	g_Config.QueryPointURL = szData;

	g_IniFile.GetValue(L"KreonBilling", L"ItemShopURL", wszStr, 64);
	if (wszStr[0] != '\0')
		WideCharToMultiByte(CP_ACP, NULL, wszStr, -1, szData, sizeof(szData), NULL, NULL);
	g_Config.ItemShopURL = szData;
#endif	// _KR

	return true;
}

bool InitApp(int argc, TCHAR * argv[])
{
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
			swprintf(wszLogName, L"CashServer_%d", nSID);
		}
		else
			swprintf(wszLogName, L"CashServer");
	}
	else
		swprintf(wszLogName, L"CashServer");

#if defined(_FINAL_BUILD)
	g_Log.Init(wszLogName, LOGTYPE_FILE_HOUR);
#else
	g_Log.Init(wszLogName, LOGTYPE_CRT_FILE_DAY);
#endif

	if (!LoadConfig(argc, argv)){
		g_Log.Log(LogType::_FILEDBLOG, L"LoadConfig Failed\r\n");
		return false;
	}
	g_Log.SetServerID(g_Config.nManagedID);

	// ResourceMng 积己
	CEtResourceMng::CreateInstance();

	// Path 汲沥
	std::string szResource = g_Config.szResourcePath + "\\Resource";
	// 惫啊喊 悸泼
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
		if (i != 0)		//0锅篮 叼弃飘
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

#ifdef PRE_ADD_UISTRING_DIVIDE
		strNationItemFileName.clear();

		strNationItemFileName = "uistring_item";
		if (i != 0)		//0锅篮 叼弃飘
			strNationItemFileName.append(MultiLanguage::NationString[i]);
		strNationItemFileName.append(".xml");	

		pStream = CEtResourceMng::GetInstance().GetStream( strNationItemFileName.c_str() );
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
	pStream = CEtResourceMng::GetInstance().GetStream( "uistring_item.xml" );
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

	g_pExtManager = new CDNExtManager;
	if (!g_pExtManager) return false;	
	if (!g_pExtManager->AllLoad())
	{
		g_Log.Log(LogType::_FILEDBLOG, L"CDNExtManager::AllLoad Fail\r\n");
		return false;
	}

	g_pIocpManager = new CDNIocpManager;
	if (!g_pIocpManager){
		g_Log.Log(LogType::_FILEDBLOG, L"new CDNIocpManager Fail\r\n");
		return false;
	}

#if defined(_WORK)
	if (g_pIocpManager->Init(500) < 0){
#else	// #if defined(_WORK)
	if (g_pIocpManager->Init(100) < 0){
#endif	// #if defined(_WORK)
		g_Log.Log(LogType::_FILEDBLOG, L"Iocp Initialize Fail\r\n");
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"Iocp Initialize Sucess(%d)\r\n", 100);
	}

	g_pThreadManager = new CDNThreadManager;
	if (!g_pThreadManager) return false;

	if (!g_pThreadManager->Init(g_Config.nThreadMax)){
		g_Log.Log(LogType::_FILEDBLOG, L"ThreadManager Initialize Fail\r\n");
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## ThreadManager Initialize Success(%d)\r\n", g_Config.nThreadMax);
	}

	g_pLogConnection = new CDNLogConnection;
	if( !g_pLogConnection ) return false;
	g_pLogConnection->SetIp(g_Config.LogInfo.szIP);
	g_pLogConnection->SetPort(g_Config.LogInfo.nPort);

	g_pUserDeleteManager = new CDNUserDeleteManager;
	if (!g_pUserDeleteManager) return false;

	g_pIocpManager->CreateThread();

	g_pSQLManager = new CDNSQLManager;
	if (!g_pSQLManager) return false;

	if (!g_pSQLManager->CreateDB()){
		g_Log.Log(LogType::_FILEDBLOG, L"CreateDB Fail\r\n");
		return false;
	}

#ifdef PRE_ADD_LIMITED_CASHITEM
	g_pLimitedCashItemRepository = new CDNLimitedCashItemRepository;
	if (!g_pLimitedCashItemRepository) return false;

	g_pLimitedCashItemRepository->QueryGetLimitedItem();
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	if (g_Config.nManagedID > 0)
	{
		g_pServiceConnection = new CDNServiceConnection(g_Config.nManagedID);
		if (!g_pServiceConnection) return false;
		g_pServiceConnection->SetIp(g_Config.ServiceInfo.szIP);
		g_pServiceConnection->SetPort(g_Config.ServiceInfo.nPort);
	}

	g_pManager = new CDNManager;
	if (!g_pManager) return false;

	// Billing Server
#if defined(_WORK)
	g_pBillingConnection = new CDNBillingConnection;
	if (!g_pBillingConnection) return false;

#elif defined(_KR) || defined(_US)
	g_pBillingConnection = new CDNBillingConnectionKR;
	if (!g_pBillingConnection) return false;

#elif defined(_KRAZ)
	g_pBillingConnection = new CDNBillingConnectionKRAZ;
	if (!g_pBillingConnection) return false;

	if (!g_pBillingConnection->Init())
		return false;

#elif defined(_JP)
	g_pBillingConnection = new CDNBillingConnectionJP;
	if (!g_pBillingConnection) return false;

#elif defined(_CH) || defined(_EU)
	g_pBillingConnection = new CDNBillingConnectionCH;
	if (!g_pBillingConnection) return false;

	if (!g_pBillingConnection->Init())
		return false;

#elif defined(_TW)
	g_pBillingPointConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_QUERY, g_Config.QueryPointInfo.szIP, g_Config.QueryPointInfo.nPort);
	if (!g_pBillingPointConnection) return false;

	g_pBillingShopConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_SHOPITEM, g_Config.ShopItemInfo.szIP, g_Config.ShopItemInfo.nPort);
	if (!g_pBillingShopConnection) return false;

	g_pBillingCouponConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_COUPON, g_Config.CouponInfo.szIP, g_Config.CouponInfo.nPort);
	if (!g_pBillingCouponConnection) return false;

	g_pBillingCouponRollBackConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_COUPON_ROLLBACK, g_Config.CouponRollBackInfo.szIP, g_Config.CouponRollBackInfo.nPort);
	if (!g_pBillingCouponRollBackConnection) return false;

#elif defined(_SG)
	g_pBillingConnection = new CDNBillingConnectionSG;
	if (!g_pBillingConnection) return false;

#elif defined(_TH)
	g_pBillingPointConnection = new CDNBillingConnectionTH(CONNECTIONKEY_TH_QUERY, g_Config.QueryPointInfo.szIP, g_Config.QueryPointInfo.nPort);
	if (!g_pBillingPointConnection) return false;

	g_pBillingShopConnection = new CDNBillingConnectionTH(CONNECTIONKEY_TH_SHOPITEM, g_Config.ShopItemInfo.szIP, g_Config.ShopItemInfo.nPort);
	if (!g_pBillingShopConnection) return false;

#elif defined(_ID)
	g_pBillingConnection = new CDNBillingConnectionID(g_Config.nThreadMax);
	if (!g_pBillingConnection) return false;

#elif defined(_RU)  || defined(_KAMO)  //[OK_Cash] 
	g_pBillingConnection = new CDNBillingConnectionRU;
	if (!g_pBillingConnection) return false;

#endif

	if (g_pIocpManager->AddAcceptConnection(CONNECTIONKEY_DEFAULT, g_Config.nAcceptPort, 1000) < 0){
		g_Log.Log(LogType::_FILEDBLOG, L"AcceptPort Fail(%d)\r\n", g_Config.nAcceptPort);
		return false;
	}
	else {
		g_Log.Log(LogType::_FILELOG, L"## AcceptPort (%d)\r\n", g_Config.nAcceptPort);
	}

	::srand( timeGetTime() );

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

	if( g_pIocpManager )
	{
		g_pIocpManager->CloseAcceptors();
		g_pIocpManager->ThreadStop();
	}
#if defined(_CH) || defined(_EU)
	if (g_pBillingConnection)
		g_pBillingConnection->Final();

#elif defined(_TW) || defined(_TH)
	SAFE_DELETE(g_pBillingPointConnection);
	SAFE_DELETE(g_pBillingShopConnection);
#if defined(_TW)
	SAFE_DELETE(g_pBillingCouponConnection);
	SAFE_DELETE(g_pBillingCouponRollBackConnection);
#endif //#if defined(_TW)
#else	//

	SAFE_DELETE(g_pBillingConnection);
#endif	// 

	SAFE_DELETE(g_pManager);
	SAFE_DELETE(g_pUserDeleteManager);
	SAFE_DELETE(g_pLogConnection);
	SAFE_DELETE(g_pServiceConnection);
	SAFE_DELETE(g_pThreadManager);
	SAFE_DELETE(g_pIocpManager);
	SAFE_DELETE(g_pSQLManager);
	SAFE_DELETE(g_pExtManager);
#ifdef PRE_ADD_LIMITED_CASHITEM
	SAFE_DELETE(g_pLimitedCashItemRepository);
#endif	// #ifdef PRE_ADD_LIMITED_CASHITEM
}

#ifndef _FINAL_BUILD
bool bIsGTest( int argc, TCHAR* argv[] )
{
	for( int i=0 ; i<argc ; ++i )
	{
		if( wcswcs( argv[i], L"--gtest_filter" ) )
		{
			LoadConfig( argc, argv );

			testing::InitGoogleMock(&argc, argv);
			return true;
		}
	}

	return false;
}
#endif

int _tmain(int argc, _TCHAR* argv[])
{
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

	// 抗寇 贸府磊 霖厚
	DWORD dwRetVal = CExceptionReport::GetInstancePtr()->Open(_T(".\\"), TRUE, TRUE, MiniDumpWithFullMemory);	// Release 葛靛 哪颇老 矫 C4744 版绊啊 惯积窍咯 Singleton 备泅 函版, CExceptionReport::GetInstancePtr() 阑 inline 拳 窍瘤 臼澜 (曼绊 : http://msdn.microsoft.com/ko-kr/library/a7za416f.aspx)
	if (NOERROR != dwRetVal) {
		DWORD dwErrNo = ::GetLastError();
		DN_RETURN(dwErrNo);
	}

	if (!InitApp(argc, argv)){
		g_Log.Log(LogType::_FILEDBLOG, L"** InitApp Failed!!! Check!!!!!\r\n");
		ClearApp();
		return 0;
	}

	wprintf(L"exit 疙飞阑 摹搁 辆丰\r\n");

	char szCmd[256] = {0};	
	while (1)
	{
		if (strcmp(szCmd, "exit") == 0)	break;

#if defined(_WORK)
		if (strcmp(szCmd, "prohibitsale") == 0){
			g_pExtManager->SetProhibitSaleList();
		}
#endif	// #if defined(_WORK)

		printf("CMD>");
        std::cin >> szCmd;
	}	

	ClearApp();
	return 0;
}

