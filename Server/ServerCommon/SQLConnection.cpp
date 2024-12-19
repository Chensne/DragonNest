#include "StdAfx.h"
#include "SQLConnection.h"
#include "Log.h"

#if defined(_LOGINSERVER)
extern TLoginConfig g_Config;
#endif	// 
#if defined(_DBSERVER)
extern TDBConfig g_Config;
#endif	// 

CSQLConnection::CSQLConnection(void): m_henv(0), m_hdbc(0), m_hstmt(0), m_cThreadID(0), m_nWorldSetID(0)
{
	memset(&m_szIp, 0, sizeof(m_szIp));
	m_nPort = 0;
	memset(&m_wszDBName, 0, sizeof(m_wszDBName));
	memset(&m_wszID, 0, sizeof(m_wszID));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	m_RefCount = 0;

	m_bMembershipDB = false;
	m_bWorldDB = false;
	m_bActozCommonDB = false;
	m_bActozCommonTestDB = false;
}

CSQLConnection::~CSQLConnection(void)
{
	Disconnect();
}

int CSQLConnection::Connect(char *pIp, int nPort, WCHAR *pwszDBName, WCHAR *pwszID)
{
	SQLRETURN RetCode;
	WCHAR wszQuery[128] = { 0, };
	WCHAR wszConnStrIn[512] = { 0, };
	SQLWCHAR wszConnStrOut[512] = { 0, };
	SQLSMALLINT cbConnStrOutMax = 0;

	Disconnect();

	RetCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);

	if (CheckRetCode(RetCode, L"SQLAllocHandle") == 0){
		RetCode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);

		if (CheckRetCode(RetCode, L"SQLSetEnvAttr") == 0){
			RetCode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);

			if (CheckRetCode(RetCode, L"SQLAllocHandle") == 0){
				RetCode = SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER )5, 0);

#if defined(_KRAZ) && defined(_FINAL_BUILD)
				if (m_bMembershipDB)
					swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;wsid=%s;App=%s;", pIp, nPort, pwszDBName, pwszID, SQLActozMembershipDBPassword, SQLHOSTNAME, SQLAPPNAME );
				else if (m_bWorldDB)
					swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;wsid=%s;App=%s;", pIp, nPort, pwszDBName, pwszID, SQLActozWorldDBPassword, SQLHOSTNAME, SQLAPPNAME );
				else if (m_bActozCommonDB)
					swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;wsid=%s;App=%s;", pIp, nPort, pwszDBName, pwszID, SQLActozCommonDBPassword, SQLHOSTNAME, SQLAPPNAME );
				else if (m_bActozCommonTestDB)
					swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;wsid=%s;App=%s;", pIp, nPort, pwszDBName, pwszID, SQLActozCommonDBPassword_Test, SQLHOSTNAME, SQLAPPNAME );
				else
					swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;wsid=%s;App=%s;", pIp, nPort, pwszDBName, pwszID, SQLPASSWORD, SQLHOSTNAME, SQLAPPNAME );
#else	// 
				swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;wsid=%s;App=%s;", pIp, nPort, pwszDBName, pwszID, SQLPASSWORD, SQLHOSTNAME, SQLAPPNAME );

#if 0
				swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;wsid=%s;App=%s;", pIp, nPort, pwszDBName, pwszID, SQLPASSWORD, SQLHOSTNAME, SQLAPPNAME );

#else
				//printf("DB Password is 3A6A7D31A8c6415 \n");  // ÎªÁËÅäºÏÆäËû³ÌÐò£¬ÔÝÊ±ÃÜÂëÐ´ËÀÎªÌ¨ÍåÃÜÂë
				swprintf(wszConnStrIn, L"Driver={SQL Server};Server=%S,%d;Network=dbmssocn;Database=%s;Uid=%s;Pwd=%s;", pIp, nPort, pwszDBName, pwszID, L"3A6A7D31A8c6415" );

#endif  //[debug]

#endif	// 

				if (CheckRetCode(RetCode, L"SQLSetConnectAttr") != 0)
					g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"DBConnect Error SQLExecDirect RetCode[%d]\n", RetCode);

				RetCode = SQLDriverConnect(m_hdbc, NULL, (SQLTCHAR*)wszConnStrIn, SQL_NTS, wszConnStrOut, _countof(wszConnStrOut), (SQLSMALLINT *)&cbConnStrOutMax, SQL_DRIVER_NOPROMPT);

				if (RetCode == SQL_SUCCESS || RetCode == SQL_SUCCESS_WITH_INFO){
					RetCode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt); 

					if (CheckRetCode(RetCode, L"SQLAllocHandle") == 0){
						swprintf(wszQuery, L"use %s", pwszDBName);
						RetCode = SQLExecDirect(m_hstmt, wszQuery, SQL_NTS);

						if (RetCode == SQL_SUCCESS || RetCode == SQL_SUCCESS_WITH_INFO) 
						{
							memcpy(m_szIp, pIp, sizeof(m_szIp));
							m_nPort = nPort;
							memcpy(m_wszDBName, pwszDBName, sizeof(m_wszDBName));
							memcpy(m_wszID, pwszID, sizeof(m_wszID));
							g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L">> [T:%d, WorldID:%d] %S:%d:%s\r\n", m_cThreadID, m_nWorldSetID, pIp, nPort, pwszDBName);

							return ERROR_NONE;
						}
						else
						{
							CheckRetCode(RetCode, L"SQLExecDirect");
							g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"DBConnect Error SQLExecDirect RetCode[%d]\n", RetCode);
						}
					}
					else
						g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"DBConnect Error SQLAllocHandle2 RetCode[%d]\n", RetCode);
				}
				else
#if defined(PRE_FIX_SQLCONNECTFAIL_LOG)
					DisplaySQLConnectError();
#else	// #if defined(PRE_FIX_SQLCONNECTFAIL_LOG)
					g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"DBConnect Error SQLDriverConnect RetCode[%d]\n", RetCode);
#endif	// #if defined(PRE_FIX_SQLCONNECTFAIL_LOG)
			}
			else
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"DBConnect Error SQLAllocHandle1 RetCode[%d]\n", RetCode);
		}
		else
			g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"DBConnect Error SQLSetEnvAttr RetCode[%d]\n", RetCode);
	}
	else
		g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"DBConnect Error SQLAllocHandle0 RetCode[%d]\n", RetCode);
	
	Disconnect();
	return ERROR_DB;
}

void CSQLConnection::Disconnect()
{
	if (m_hstmt){
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = 0;
	}
	if (m_hdbc){
		SQLDisconnect (m_hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
		m_hdbc = 0;
	}

	if (m_henv){
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
		m_henv = 0;
	}	
}

int CSQLConnection::CheckRetCode(SQLRETURN RetCode, WCHAR *pDesc)
{
	if (SQL_SUCCEEDED(RetCode) || RetCode == SQL_NEED_DATA || RetCode == SQL_NO_DATA){
		return ERROR_NONE;
	}

	DisplayInfo (RetCode, pDesc);

	return ERROR_DB; 
}

#if defined(PRE_FIX_SQLCONNECTFAIL_LOG)
int CSQLConnection::DisplaySQLConnectError()
{
	SQLINTEGER Error;
	SQLSMALLINT MsgLen;
	WCHAR InfoMsg[10] = { 0, }, ErrorMsg[1025] = { 0, }, SqlState[10] = { 0, };
	_wcscpy(InfoMsg, _countof(InfoMsg), L"SQLError", (int)wcslen(L"SQLError"));

	SQLRETURN sqlReturn = SQLGetDiagRec(SQL_HANDLE_DBC, m_hdbc, 1, SqlState, &Error, ErrorMsg, 1024, &MsgLen);
	if (sqlReturn == SQL_SUCCESS)
		g_Log.Log(LogType::_DBSYSTEM_ERROR, m_nWorldSetID, 0, 0, 0, L"SQLError #SQLDriveConnect# (%s) %ld, %s\r\n", SqlState, Error, ErrorMsg);
	else
		g_Log.Log(LogType::_DBSYSTEM_ERROR, m_nWorldSetID, 0, 0, 0, L"SQLError #SQLDriveConnect# (%s) unknown (%d)\r\n", SqlState, sqlReturn);

	return ERROR_DB;
}
#endif	// #if defined(PRE_FIX_SQLCONNECTFAIL_LOG)

void CSQLConnection::DisplayInfo(SQLRETURN RetCode, WCHAR *pDesc)
{
	SQLINTEGER Error;
	SQLSMALLINT MsgLen;
	WCHAR InfoMsg[10] = { 0, }, ErrorMsg[1025] = { 0, }, SqlState[10] = { 0, };

	if (SQL_SUCCEEDED(RetCode))
		_wcscpy(InfoMsg, _countof(InfoMsg), L"SQLInfo", (int)wcslen(L"SQLInfo"));
	else
		_wcscpy(InfoMsg, _countof(InfoMsg), L"SQLError", (int)wcslen(L"SQLError"));

	SQLRETURN sqlReturn = SQLGetDiagRec(SQL_HANDLE_STMT, m_hstmt, 1, SqlState, &Error, ErrorMsg, 1024, &MsgLen);
	if (sqlReturn == SQL_SUCCESS)
	{
		if (Error == 2601) return;

		g_Log.Log(LogType::_DBSYSTEM_ERROR, L"%s: #%s# (%s) %ld, %s\r\n", InfoMsg, pDesc, SqlState, Error, ErrorMsg);
		g_Log.Log(LogType::_DBSYSTEM_ERROR, L"Query:%s\r\n", m_wszQuery );

		if (Error == WSAECONNRESET || 
			wcscmp(SqlState, L"08S01") == 0 ||	// ²÷¾îÁø»óÅÂ
			wcscmp(SqlState, L"24000") == 0)	// Àß¸øµÈ Ä¿¼­ »óÅÂ
		{
			Disconnect();	// ²÷°í ´Ù½Ã Àç¿¬°á
		}
	}
	else
	{
		g_Log.Log(LogType::_DBSYSTEM_ERROR, L"%s: #%s# (%s) unknown (%d)\r\n", InfoMsg, pDesc, SqlState, sqlReturn);
	}
}

int CSQLConnection::CheckConnect()
{
	if (m_hstmt == 0) return Reconnect();	// ²÷¾îÁø»óÅÂ¸é Àç¿¬°áÇÏÀð
	return ERROR_NONE;
}

int CSQLConnection::Reconnect()
{
	int nResult = Connect(m_szIp, m_nPort, m_wszDBName, m_wszID);
	if (nResult < 0)
		g_Log.Log(LogType::_FILELOG, m_nWorldSetID, 0, 0, 0, L"SQL Reconnect Error~! (T:%d W:%d (%S:%d) DBName:%s)\r\n", m_cThreadID, m_nWorldSetID, m_szIp, m_nPort, m_wszDBName);

	return nResult;
}

int CSQLConnection::CommonRowcountQuery(WCHAR *pQuery)
{
	if (CheckConnect() < 0){
		g_Log.Log(LogType::_ERROR, L"[CommonRowcountQuery] Check Connect Fail\r\n");
		return ERROR_DB;
	}	

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = 0;

	RetCode = SQLPrepare (m_hstmt, pQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		RetCode = SQLBindParameter (m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	// @@rowcount °ªÀ» ¸»ÇÔ

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{			
			if (nResult == ERROR_NONE_ROWCOUNT) return ERROR_NONE;	// ¼º°ø
			// return ERROR_NONE;
			return nResult;			
		}
	}	
	return ERROR_DB;
}

int CSQLConnection::CommonResultQuery(WCHAR *pQuery)
{
	if (CheckConnect() < 0){
		g_Log.Log(LogType::_ERROR, L"[CommonResultQuery] Check Connect Fail\r\n");
		return ERROR_DB;
	}	

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = 0;

	RetCode = SQLPrepare (m_hstmt, pQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		RetCode = SQLBindParameter (m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	// return°ªÀ» ¸»ÇÔ

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){			
			// if (nResult == 0) return ERROR_DB;
			return nResult;
		}
	}	
	return ERROR_DB;
}

int CSQLConnection::CommonReturnValueQuery(WCHAR *pQuery)
{
	if (CheckConnect() < 0){
		g_Log.Log(LogType::_ERROR, L"[CommonReturnValueQuery] CheckConnect Fail\r\n");
		return ERROR_DB;
	}	

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, pQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		RetCode = SQLBindParameter (m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	// return°ªÀ» ¸»ÇÔ

		RetCode = SQLExecute (m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

// ÆíÁö ³»¿ëÀÌ³ª Á¦¸ñÁß¿¡ ÀÛÀº µû¿ÈÇ¥°¡ µé¾î°£°æ¿ì Äõ¸®¹® ³»¿¡¼­ ¿¡·¯°¡ ¹ß»ýÇÏ±â ¶§¹®¿¡ ÀÛÀº µû¿ÈÇ¥ ÇÏ³ª¸¦ ÀÛÀºµû¿ÈÇ¥ µÎ°³·Î ¹Ù²ãÁÖ´Â ÇÔ¼ö
void CSQLConnection::ConvertQuery(WCHAR *pSrc, int nSrcMaxSize, WCHAR *pResultStr, int nDesMaxSize)
{
	if( nSrcMaxSize * 2 + 1 >  nDesMaxSize )
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"ConvertQuery MaxSize Error\r\n");
		return;
	}
	int nLen = (int)wcslen(pSrc);
	if (nSrcMaxSize < nLen) nLen = nSrcMaxSize;

	int j = 0;
	for (int i = 0; i < nLen; i++){
		if (pSrc[i] == '\'')
		{
			pResultStr[j] = pSrc[i];
			j++;			
			pResultStr[j] = pSrc[i];
		}
		else
		{
			pResultStr[j] = pSrc[i];
		}
		j++;		
	}

	if( j >= nDesMaxSize)
	{			
		j -= 1;		
	}

	pResultStr[j] = NULL;
}

void CSQLConnection::CheckColumnCount(int nBindCount, const char* pszQuery)
{
	SQLSMALLINT nColCount = 0;
	SQLRETURN RetCode;
	RetCode = SQLNumResultCols(m_hstmt, &nColCount);
	if (CheckRetCode(RetCode, L"SQLNumResultCols") == ERROR_NONE)
	{
		if( nBindCount-1 != nColCount )
			g_Log.Log(LogType::_ERROR, L"[%S] BindCount:%d, ResultCount:%d\r\n", pszQuery, nBindCount, nColCount);
	}
}

#if defined(PRE_ADD_SQL_RENEW)
CSQLRecordSet::CSQLRecordSet(CSQLConnection* pConnection)
: m_pConnection(pConnection), m_bFetch(false), m_nBindCount(1)
{
	if ( m_pConnection && m_pConnection->CheckConnect() < 0){
		//g_Log.Log(LogType::_ERROR, L"[CommonReturnValueQuery] CheckConnect Fail\r\n");		
	}
	if( m_pConnection )
		m_hstmt = m_pConnection->GetStmt();
}

CSQLRecordSet::~CSQLRecordSet()
{
	m_vBindParam.clear();
	if( m_bFetch )
		SQLCloseCursor(m_hstmt);
}

void CSQLRecordSet::AddParamBit(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_BIT, SQL_BIT, sizeof(bool), pValue, bInputNull));
}

void CSQLRecordSet::AddParamByte(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_TINYINT, SQL_TINYINT, sizeof(BYTE), pValue, bInputNull));
}

void CSQLRecordSet::AddParamShort(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), pValue, bInputNull));
}

void CSQLRecordSet::AddParamInt(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_SLONG, SQL_INTEGER, sizeof(int), pValue, bInputNull));
}

void CSQLRecordSet::AddParamInt64(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), pValue, bInputNull)); 
}

void CSQLRecordSet::AddParamTimeStamp(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), pValue, bInputNull)); 
}

void CSQLRecordSet::AddParamString(SQLSMALLINT type, SQLPOINTER pValue, int nSize, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_CHAR, SQL_VARCHAR, nSize, pValue, bInputNull));
}

void CSQLRecordSet::AddParamWString(SQLSMALLINT type, SQLPOINTER pValue, int nSize, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_WCHAR, SQL_WVARCHAR, nSize, pValue, bInputNull));
}

void CSQLRecordSet::AddParamBinary(SQLSMALLINT type, SQLPOINTER pValue, int nSize, bool bInputNull)
{
	m_vBindParam.push_back(CParamODBC(type, SQL_C_BINARY, SQL_BINARY, nSize, pValue, bInputNull));
}

void CSQLRecordSet::AddBindColBit(SQLPOINTER pValue)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_BIT, pValue, sizeof(bool), &cblen);
}

void CSQLRecordSet::AddBindColByte(SQLPOINTER pValue)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_TINYINT, pValue, sizeof(BYTE), &cblen);
}

void CSQLRecordSet::AddBindColShort(SQLPOINTER pValue)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_SMALLINT, pValue, sizeof(short), &cblen);
}

void CSQLRecordSet::AddBindColInt(SQLPOINTER pValue)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_INTEGER, pValue, sizeof(INT), &cblen);
}

void CSQLRecordSet::AddBindColInt64(SQLPOINTER pValue)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_C_SBIGINT, pValue, sizeof(INT64), &cblen);
}

void CSQLRecordSet::AddBindColTimeStamp(SQLPOINTER pValue)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_TIMESTAMP, pValue, sizeof(TIMESTAMP_STRUCT), &cblen);
}

void CSQLRecordSet::AddBindColString(SQLPOINTER pValue, int nSize)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_CHAR, pValue, nSize, &cblen);
}

void CSQLRecordSet::AddBindColWString(SQLPOINTER pValue, int nSize)
{
	SQLLEN cblen;
	SQLBindCol(m_pConnection->GetStmt(), m_nBindCount++, SQL_WCHAR, pValue, nSize, &cblen);
}

bool CSQLRecordSet::Execute(const WCHAR* strQuery)
{
	if( !m_pConnection || m_pConnection->GetStmt() == 0)
		return false;

	SQLRETURN RetCode;	

	RetCode = SQLExecDirect(m_hstmt, (SQLTCHAR*)strQuery, SQL_NTS);
	if( m_pConnection->CheckRetCode( RetCode, L"SQLExecDirect" ) != ERROR_NONE )	
		return false;

	return true;
}

bool CSQLRecordSet::Execute_Procedure(const WCHAR* strProcName, bool bReturn)
{
	if( !m_pConnection || m_pConnection->GetStmt() == 0)
		return false;

	WCHAR			strQuery[4096];
	SQLRETURN		RetCode;
	int				nCount = 1;	

	if (!m_vBindParam.empty())
	{
		BIND_PARAM_ARRAY_IT	it;

		for (it = m_vBindParam.begin(); it != m_vBindParam.end(); it++)
		{
			if((*it).m_bNull)			
				(*it).m_StrLenIndPtr = SQL_NULL_DATA;
			else if( (*it).m_sValueType == SQL_C_CHAR)
				(*it).m_StrLenIndPtr = SQL_NTS;
			else if( (*it).m_sValueType == SQL_C_WCHAR)
				(*it).m_StrLenIndPtr = SQL_NTSL;

			RetCode = SQLBindParameter(m_hstmt, nCount++, (*it).m_sType, (*it).m_sValueType, 
				(*it).m_sParamType, (*it).m_unColumnSize, 0, (*it).m_pValue, 0, &(*it).m_StrLenIndPtr);

			if (RetCode != SQL_SUCCESS && RetCode != SQL_SUCCESS_WITH_INFO)
				return false;

			if (it == m_vBindParam.begin())
			{
				if (bReturn)
					swprintf(strQuery,4095, L"{ ? = CALL %s(", strProcName);
				else
					swprintf(strQuery,4095, L"{ CALL %s(?, ", strProcName);
			}
			else
				wcscat(strQuery, _T("?, "));
		}

		if (strQuery[wcslen(strQuery) - 2] == L',')
			wcscpy(&strQuery[wcslen(strQuery) - 2], L") }");
		else
			wcscat(strQuery, _T(") }"));
	}
	else
		swprintf(strQuery, 4095, L"{ CALL %s() }", strProcName);

	RetCode = SQLPrepare( m_hstmt, strQuery, SQL_NTSL );
	if( m_pConnection->CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		RetCode = SQLExecute( m_hstmt );
		if( m_pConnection->CheckRetCode( RetCode, L"SQLExecute" ) != ERROR_NONE )
			return false;
	}
	return true;
}

bool CSQLRecordSet::Fetch()
{
	SQLRETURN		RetCode;
	RetCode = SQLFetch(m_pConnection->GetStmt());
	if( m_pConnection->CheckRetCode( RetCode, L"SQLFetch" ) != ERROR_NONE || RetCode == SQL_NO_DATA )
		return false;
	m_bFetch = true;
	return true;
}
#endif //#if defined(PRE_ADD_SQL_RENEW)
