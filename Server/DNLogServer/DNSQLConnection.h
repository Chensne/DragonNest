#pragma once

#include "SQLConnection.h"

class CDNSQLConnection: public CSQLConnection
{
public:
	CDNSQLConnection(void);
	~CDNSQLConnection(void);

	int QueryLog(TLog *pLog);
	int QueryLog( TLogFile* pLog );
};