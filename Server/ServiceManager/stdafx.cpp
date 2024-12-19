
#include "Stdafx.h"

struct tm _wtotm(const WCHAR * pTimeFromSql)
{
	int nCnt = 0, nTmCnt = 0;
	struct tm Tm;
	memset(&Tm, 0, sizeof(struct tm));

	WCHAR wszTemp[TIMELENMAX];
	memset(wszTemp, 0, sizeof(wszTemp));

	for (int i = 0; i < TIMELENMAX; i++)
	{
		if (pTimeFromSql[i] == NULL || pTimeFromSql[i] == '\0')
		{
			if (nTmCnt == 4)
			{
				Tm.tm_min = _wtoi(wszTemp);
				return Tm;
			}
			break;
		}
		if (pTimeFromSql[i] == '.')	break;
		if (pTimeFromSql[i] != '-' && pTimeFromSql[i] != ':' && pTimeFromSql[i] != ' ')
		{
			wszTemp[nCnt] = pTimeFromSql[i];
			nCnt++;
		}
		else
		{
			if (nTmCnt == 0)
				Tm.tm_year = _wtoi(wszTemp) - 1900;		//struct tm year since 1900
			else if (nTmCnt == 1)
				Tm.tm_mon = _wtoi(wszTemp) - 1;
			else if (nTmCnt == 2)
				Tm.tm_mday = _wtoi(wszTemp);
			else if (nTmCnt == 3)
				Tm.tm_hour = _wtoi(wszTemp);
			else if (nTmCnt == 4)
			{
				Tm.tm_min = _wtoi(wszTemp);
				return Tm;
			}

			memset(wszTemp, 0, sizeof(wszTemp));
			nTmCnt++;
			nCnt = 0;
		}
	}
	return Tm;
}

int GetServerType(const WCHAR * pType)
{
	if (!wcsicmp(pType, L"db"))
		return MANAGED_TYPE_DB;
	else if (!wcsicmp(pType, L"log"))
		return MANAGED_TYPE_LOG;
	else if (!wcsicmp(pType, L"login"))
		return MANAGED_TYPE_LOGIN;
	else if (!wcsicmp(pType, L"master"))
		return MANAGED_TYPE_MASTER;
	else if (!wcsicmp(pType, L"village"))
		return MANAGED_TYPE_VILLAGE;
	else if (!wcsicmp(pType, L"game"))
		return MANAGED_TYPE_GAME;
	else if (!wcsicmp(pType, L"cash"))
		return MANAGED_TYPE_CASH;
	return 0;
}

int GetMonitorLevel(const WCHAR * pLevel)
{
	if (!wcsicmp(L"MONITOR", pLevel))
		return SERVERMONITOR_LEVEL_MONITOR;
	else if (!wcsicmp(L"ADMIN", pLevel))
		return SERVERMONITOR_LEVEL_ADMIN;
	else if (!wcsicmp(L"SUPERADMIN", pLevel))
		return SERVERMONITOR_LEVEL_SUPERADMIN;
	else
		_ASSERT_EXPR(0, L"CheckMonitor User Level");
	return SERVERMONITOR_LEVEL_NONE;
}

int GetSIDType(const WCHAR * pwszType)
{
	if (!_wcsicmp(L"Log", pwszType))
		return SID_CONNECTION_TYPE_LOG;
	else if (!_wcsicmp(L"DBWare", pwszType))
		return SID_CONNECTION_TYPE_DB;
	_ASSERT_EXPR(0, L"[GetSIDType] Invalid pwszType");
	return 0;
}

int GetServerTypeForConvert(const WCHAR * pType)
{
	if (!wcsicmp(pType, L"db") || !wcsicmp(pType, L"dbware"))
		return MANAGED_TYPE_DB;
	else if (!wcsicmp(pType, L"log"))
		return MANAGED_TYPE_LOG;
	else if (!wcsicmp(pType, L"login"))
		return MANAGED_TYPE_LOGIN;
	else if (!wcsicmp(pType, L"master"))
		return MANAGED_TYPE_MASTER;
	else if (!wcsicmp(pType, L"village"))
		return MANAGED_TYPE_VILLAGE;
	else if (!wcsicmp(pType, L"game"))
		return MANAGED_TYPE_GAME;
	else if (!wcsicmp(pType, L"cash"))
		return MANAGED_TYPE_CASH;
	return 0;
}

time_t _wtoTime_t(const WCHAR * pTimeFromSql)
{
	return mktime(&_wtotm(pTimeFromSql));
}
