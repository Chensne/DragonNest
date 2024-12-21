#include "StdAfx.h"
#include "DNSQLConnection.h"
#include "Util.h"
#include "Log.h"
#include "DNServiceConnection.h"

CDNSQLConnection::CDNSQLConnection(void): CSQLConnection()
{
}

CDNSQLConnection::~CDNSQLConnection(void)
{
}

int CDNSQLConnection::QueryLog(TLog *pLog)
{
	WCHAR wszQuery[QUERYSIZEMAX];
	memset(wszQuery, 0, sizeof(WCHAR)*QUERYSIZEMAX);

	char szParam[LOGPARAMSIZEMAX] = { 0, };
	_strcpy(szParam, LOGPARAMSIZEMAX, pLog->szParam, pLog->wParamSize);

	swprintf(wszQuery, L"{CALL GameLog('%u','%I64d','%d','%d','%d','%d','%S')}", 
		pLog->nAccountDBID, pLog->biCharacterDBID, pLog->nSessionID, pLog->cWorldID, pLog->nLogType, pLog->wTotalParamCount, szParam);

	if (CommonResultQuery(wszQuery) != ERROR_NONE){
		g_Log.Log(LogType::_FILELOG, L"[Q:%s]\r\n", wszQuery);
		return ERROR_DB;
	}

	return ERROR_NONE;
}

int CDNSQLConnection::QueryLog( TLogFile* pLog )
{
	WCHAR wszBuf[(1024 * 2) + 1];
	memset(wszBuf, 0, sizeof(wszBuf));
	ConvertQuery(pLog->wszBuf, 1024, wszBuf, _countof(wszBuf));

	WCHAR wszQuery[QUERYSIZEMAX];
	memset(wszQuery, 0, sizeof(WCHAR)*QUERYSIZEMAX);
	swprintf(wszQuery, L"{?=CALL P_AddCommonLog('%d','%d','%d','%d','%I64d','%d',N'%s','%d')}", pLog->unLogType, pLog->unServerType, pLog->unWorldSetID, pLog->uiAccountDBID, pLog->biCharDBID, pLog->uiSessionID, wszBuf, pLog->nServerID );

	if (CommonResultQuery(wszQuery) != ERROR_NONE){
		g_Log.Log(LogType::_FILELOG, L"[Q:%s]\r\n", wszQuery);
		return ERROR_DB;
	}

	return ERROR_NONE;
}
