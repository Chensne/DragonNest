#pragma once

#include "SQLConnection.h"

#if defined(_KRAZ) && defined(_FINAL_BUILD)

class CDNSQLActozCommon: public CSQLConnection
{
public:
	CDNSQLActozCommon(void);
	~CDNSQLActozCommon(void);

	int QueryAddCharacterInfo(INT64 biCharacterDBID, int nWorldID, char *pCharName, UINT nAccountDBID, char *pAccountName, BYTE cJob, BYTE cLevel, int nExp, INT64 biCoin, char cStatus, TIMESTAMP_STRUCT &CreateDate, char *pIp);
	int QueryUpdateCharacterInfo(char cUpdateType, INT64 biCharacterDBID, BYTE cJob, BYTE cLevel, int nExp, INT64 biMoney, char *pIp);
	int QueryUpdateCharacterName(INT64 biCharacterDBID, char *pCharName);
	int QueryUpdateCharacterStatus(INT64 biCharacterDBID, char cStatus);
};

#endif	// #if defined(_KRAZ)