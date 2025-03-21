#include "StdAfx.h"
#include "DNSQLActozCommon.h"
#include "TimeSet.h"

#if defined(_KRAZ) && defined(_FINAL_BUILD)

CDNSQLActozCommon::CDNSQLActozCommon(void): CSQLConnection()
{
}

CDNSQLActozCommon::~CDNSQLActozCommon(void)
{
}

int CDNSQLActozCommon::QueryAddCharacterInfo(INT64 biCharacterDBID, int nWorldID, char *pCharName, UINT nAccountDBID, char *pAccountName, BYTE cJob, BYTE cLevel, int nExp, INT64 biCoin, char cStatus, TIMESTAMP_STRUCT &CreateDate, char *pIp)
{
	CQueryTimeLog QueryTimeLog( "[ActozCommon]prAddCharInfo" );

	std::string strCreateDate = FormatA("%d-%d-%d %d:%d:%d", CreateDate.year, CreateDate.month, CreateDate.day, CreateDate.hour, CreateDate.minute, CreateDate.second);
	swprintf(m_wszQuery, L"{?=CALL dbo.prAddCharInfo(%I64d,%d,'%S',%d,'%S',%d,%d,%d,%I64d,%d,'%S','%S')}", biCharacterDBID, nWorldID, pCharName, nAccountDBID, pAccountName, cJob, cLevel, nExp, biCoin, cStatus, strCreateDate.c_str(), pIp);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLActozCommon::QueryUpdateCharacterInfo(char cUpdateType, INT64 biCharacterDBID, BYTE cJob, BYTE cLevel, int nExp, INT64 biMoney, char *pIp)
{
	CQueryTimeLog QueryTimeLog( "[ActozCommon]prUpdateCharInfo" );

	CTimeSet CurTime;
	TIMESTAMP_STRUCT UpdateDate = CurTime.GetDBTimeStampStruct();
	std::string strUpdateDate = FormatA("%d-%d-%d %d:%d:%d", UpdateDate.year, UpdateDate.month, UpdateDate.day, UpdateDate.hour, UpdateDate.minute, UpdateDate.second);

	swprintf(m_wszQuery, L"{?=CALL dbo.prUpdateCharInfo(%d,%I64d,%d,%d,%d,%I64d,'%S','%S')}", cUpdateType, biCharacterDBID, cJob, cLevel, nExp, biMoney, strUpdateDate.c_str(), pIp);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLActozCommon::QueryUpdateCharacterName(INT64 biCharacterDBID, char *pCharName)
{
	CQueryTimeLog QueryTimeLog( "[ActozCommon]prUpdateCharName" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.prUpdateCharName(%I64d,'%S')}", biCharacterDBID, pCharName);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLActozCommon::QueryUpdateCharacterStatus(INT64 biCharacterDBID, char cStatus)
{
	CQueryTimeLog QueryTimeLog( "[ActozCommon]prUpdateCharStatus" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.prUpdateCharStatus(%I64d,%d)}", biCharacterDBID, cStatus);

	return CommonReturnValueQuery(m_wszQuery);
}

#endif	// #if defined(_KRAZ)