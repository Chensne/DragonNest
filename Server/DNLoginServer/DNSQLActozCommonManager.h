#pragma once

#include "DNSQLActozCommon.h"
#include "MultiSync.h"

#if defined(_KRAZ) && defined(_FINAL_BUILD)

class CDNSQLActozCommonManager
{
private:
	typedef std::vector<CDNSQLActozCommon*> TVecActozCommon;
	typedef std::queue<CDNSQLActozCommon*> TQueueActozCommon;
	typedef std::map<DWORD, CDNSQLActozCommon*> TMapActozCommon;

	TVecActozCommon m_pVecActozCommon;
	TQueueActozCommon m_pQueueActozCommon;
	TMapActozCommon m_pMapActozCommon;

public:
	CSyncLock m_Sync;

public:
	CDNSQLActozCommonManager(UINT uiWorkerThreadCount);
	~CDNSQLActozCommonManager(void);

	void Clear();
	bool CreateDB();

	void DoUpdate(DWORD nCurTick);

	CDNSQLActozCommon* FindActozCommonDB();
	void FreeActozCommonDB(CDNSQLActozCommon* pActozCommonDB);

	int QueryAddCharacterInfo(INT64 biCharacterDBID, int nWorldID, WCHAR *pCharName, UINT nAccountDBID, WCHAR *pAccountName, BYTE cJob, TIMESTAMP_STRUCT &CreateDate, char *pIp);
	int QueryUpdateCharacterStatus(INT64 biCharacterDBID, char cStatus);
};

extern CDNSQLActozCommonManager* g_pSQLActozCommonManager;


class CDNSQLActozCommonAuto
{
public:
	CDNSQLActozCommonAuto(CDNSQLActozCommon* pThis, CDNSQLActozCommonManager* pOwner)
	{
		m_This = pThis;
		m_Owner = pOwner;
	}
	~CDNSQLActozCommonAuto()
	{
		if (m_This && m_Owner) {
			m_Owner->FreeActozCommonDB(m_This);
		}
	}
private:
	CDNSQLActozCommon* m_This;
	CDNSQLActozCommonManager* m_Owner;
};


#endif	// #if defined(_KRAZ)