#pragma once

#include "DNSQLConnection.h"

class CDNSQLConnectionManager
{
private:
	CDNSQLConnection* m_LogDBArray[THREADMAX][WORLDCOUNTMAX];
	CDNSQLConnection* m_ServerLogDBArray[THREADMAX];
	
	bool CreateServerLogDB();
	bool CreateLogDB();

public:
	CDNSQLConnectionManager(void);
	~CDNSQLConnectionManager(void);

	void Clear();
	bool CreateDB();
	void DestroyDB();

	CDNSQLConnection* FindServerLogDB(int nThreadIndex);
	CDNSQLConnection* FindLogDB(int nThreadIndex, char cWorldID);
	CDNSQLConnection* FindLogDB(int nThreadIndex);
};

extern CDNSQLConnectionManager g_SQLConnectionManager;

