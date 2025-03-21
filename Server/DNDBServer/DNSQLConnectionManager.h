#pragma once

#include "DNSQLWorld.h"
#include "DNSQLMembership.h"
#include "DNSQLActozCommon.h"

class CDNSQLConnectionManager
{
private:
	CDNSQLMembership *m_pMembershipArray[THREADMAX];
	CDNSQLWorld *m_pWorldArray[THREADMAX][WORLDCOUNTMAX];
#if defined(_KRAZ) && defined(_FINAL_BUILD)
	CDNSQLActozCommon *m_pActozCommonArray[THREADMAX];
#endif	// #if defined(_KRAZ)

	bool CreateMembership();
	bool CreateWorld();
#if defined(_KRAZ) && defined(_FINAL_BUILD)
	bool CreateActozCommon();
#endif	// #if defined(_KRAZ)

public:
	CDNSQLConnectionManager(void);
	~CDNSQLConnectionManager(void);

	void Clear();
	bool CreateDB();
	void DestroyDB();

	CDNSQLMembership* FindMembershipDB(int nThreadIndex);
	CDNSQLWorld* FindWorldDB(int nThreadIndex, char cWorldID);
	CDNSQLWorld* FindWorldDB(int nThreadIndex);
#if defined(_KRAZ) && defined(_FINAL_BUILD)
	CDNSQLActozCommon* FindActozCommonDB(int nThreadIndex);
#endif	// #if defined(_KRAZ)
};

extern CDNSQLConnectionManager g_SQLConnectionManager;

