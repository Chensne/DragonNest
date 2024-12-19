#include "StdAfx.h"
#include "DNActozShield.h"
#include "../ServerCommon/KRAZ/SHIELD.h"
#include "Log.h"

#if defined(_LOGINSERVER)
extern TLoginConfig g_Config;
#endif

#if defined(_KRAZ)

#if defined(_DEBUG)
#pragma comment(lib, "../ServerCommon/KRAZ/Shield_x64_MTD.lib")
#else	// #if defined(_DEBUG)
#pragma comment(lib, "../ServerCommon/KRAZ/Shield_x64_MT.lib")
#endif	// #if defined(_DEBUG)

const int ActozGameCode = 31;
const int ActozRepeatTick = 60000 * 10;

CDNActozShield *g_pActozShield = NULL;

CDNActozShield::CDNActozShield(void)
{
	m_dwReconnectTick = timeGetTime();
	m_strReserved.clear();

	std::string strTemp;
#if defined(_LOGINSERVER)
	m_strReserved = FormatA("%d N:%d L:%s", _SERVER_TYPEID, g_Config.nNation, szLoginVersion);
#elif defined(_DBSERVER)
	m_strReserved = FormatA("%d D:%s", _SERVER_TYPEID, szDBVersion);
#elif defined(_MASTERSERVER)
	m_strReserved = FormatA("%d M:%s", _SERVER_TYPEID, szMasterVersion);
#elif defined(_VILLAGESERVER)
	m_strReserved = FormatA("%d V:%s", _SERVER_TYPEID, szVillageVersion);
#elif defined(_GAMESERVER)
	m_strReserved = FormatA("%d G:%s", _SERVER_TYPEID, szGameVersion);
#elif defined(_CASHSERVER)
	m_strReserved = FormatA("%d C:%s", _SERVER_TYPEID, szCashVersion);
#elif defined(_LOGSERVER)
	m_strReserved = FormatA("%d L:%s", _SERVER_TYPEID, szLogVersion);
#elif defined(_SERVICEMANAGER)
	m_strReserved = FormatA("%d S:%s", _SERVER_TYPEID, szServiceManagerVersion);
#endif
	strTemp = FormatA(" %d-%d-%d %d:%d R:%d S:%d (%s)", 
		g_nBuildTimeYear, g_nBuildTimeMonth, g_nBuildTimeDay, g_nBuildTimeHour, g_nBuildTimeMinute, g_nBuildResourceRevision, g_nBuildSourceRevision, g_szBuildString);

	m_strReserved.append(strTemp.c_str());
}

CDNActozShield::~CDNActozShield(void)
{
}

bool CDNActozShield::Init()
{
	if (Atz_Checker_V1()){
		g_Log.Log(LogType::_FILELOG, L"ActozShield Atz_Checker_V1 OK!!\r\n");
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"ActozShield Atz_Checker_V1 Fail!!\r\n");
		return false;
	}

	if (Atz_Checker_V2(ActozGameCode, (char*)m_strReserved.c_str()))
		g_Log.Log(LogType::_FILELOG, L"ActozShield Atz_Checker_V2 OK!!\r\n");
	else
		g_Log.Log(LogType::_FILELOG, L"ActozShield Atz_Checker_V2 Fail!!\r\n");

	CheckerResult();

	return true;
}

void CDNActozShield::CheckerResult()
{
	if (Atz_Checker_Result(ActozGameCode, (char*)m_strReserved.c_str()))
		g_Log.Log(LogType::_FILELOG, L"ActozShield Atz_Checker_Result OK!!\r\n");
	else
		g_Log.Log(LogType::_FILELOG, L"ActozShield Atz_Checker_Result Fail!!\r\n");
}

void CDNActozShield::RepeatCheckerResult(DWORD dwCurTick)
{
	if (m_dwReconnectTick + ActozRepeatTick < dwCurTick){
		m_dwReconnectTick = dwCurTick;

		CheckerResult();
	}
}

#endif	// #if defined(_KRAZ)