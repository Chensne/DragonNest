#include "StdAfx.h"
#include "DNSQLActozCommonManager.h"

extern TLoginConfig g_Config;

#if defined(_KRAZ) && defined(_FINAL_BUILD)

CDNSQLActozCommonManager* g_pSQLActozCommonManager = NULL;

CDNSQLActozCommonManager::CDNSQLActozCommonManager(UINT uiWorkerThreadCount)
{
	Clear();
}

CDNSQLActozCommonManager::~CDNSQLActozCommonManager(void)
{
	Clear();
}

void CDNSQLActozCommonManager::Clear()
{
	TVecActozCommon::iterator iter = m_pVecActozCommon.begin();
	for (; m_pVecActozCommon.end() != iter ; ++iter) {
		CDNSQLActozCommon* pSQLCon = (*iter);
		SAFE_DELETE(pSQLCon);
	}
	m_pVecActozCommon.clear();

	while(!m_pQueueActozCommon.empty()) {
		m_pQueueActozCommon.pop();
	}
}

bool CDNSQLActozCommonManager::CreateDB()
{
	CDNSQLActozCommon *pActozCommon = NULL;
	for (int i = 0; i < g_Config.nSQLMax; i++){
		pActozCommon = new CDNSQLActozCommon;

		pActozCommon->m_cThreadID = i;
		if (g_Config.bTestServer)
			pActozCommon->SetActozCommonTestDB();
		else
			pActozCommon->SetActozCommonDB();

		if (pActozCommon->Connect(g_Config.ActozCommonDB.szIP, g_Config.ActozCommonDB.nPort, g_Config.ActozCommonDB.wszDBName, g_Config.ActozCommonDB.wszDBID) < 0)
		{
			g_Log.Log(LogType::_FILELOG, L"Not Connect ActozCommonDB Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.ActozCommonDB.szIP, g_Config.ActozCommonDB.nPort, g_Config.ActozCommonDB.wszDBName, g_Config.ActozCommonDB.wszDBID);
			SAFE_DELETE(pActozCommon);
			return false;
		}

		m_pVecActozCommon.push_back(pActozCommon);
		m_pQueueActozCommon.push(pActozCommon);

		Sleep(1);
	}

	return true;
}

void CDNSQLActozCommonManager::DoUpdate(DWORD nCurTick)
{
}

CDNSQLActozCommon* CDNSQLActozCommonManager::FindActozCommonDB()
{
	CDNSQLActozCommon* pActozCommon = NULL;
	{
		ScopeLock<CSyncLock> Lock(m_Sync);

		DWORD dwThreadID = ::GetCurrentThreadId();
		DN_ASSERT(0 != dwThreadID,	"Check!");

		TMapActozCommon::iterator iter = m_pMapActozCommon.find(dwThreadID);
		if (m_pMapActozCommon.end() != iter) {
			pActozCommon = (iter->second);
			LONG nRefCount = pActozCommon->IncRefCount();
			if (1 >= nRefCount) {
				DN_ASSERT(0,	"Check!");
			}
			return pActozCommon;
		}

		if (m_pQueueActozCommon.empty()) {
			return NULL;
		}

		pActozCommon = m_pQueueActozCommon.front();
		LONG nRefCount = pActozCommon->IncRefCount();
		if (1 != nRefCount) {
			DN_ASSERT(0,	"Check!");
		}
		m_pQueueActozCommon.pop();
		m_pMapActozCommon.insert(TMapActozCommon::value_type(dwThreadID, pActozCommon));	// 동기화 상태이므로 결과체크 필요없음 ?
	}
	DN_ASSERT(NULL != pActozCommon,	"Invalid!");

	return pActozCommon;
}

void CDNSQLActozCommonManager::FreeActozCommonDB(CDNSQLActozCommon* pActozCommonDB)
{
	if (!pActozCommonDB) return;

	{
		ScopeLock<CSyncLock> aLock(m_Sync);

		LONG nRefCount = pActozCommonDB->DecRefCount();
		if (0 == nRefCount){
			m_pMapActozCommon.erase(::GetCurrentThreadId());
			m_pQueueActozCommon.push(pActozCommonDB);
		}
		DN_ASSERT(0 <= nRefCount,	"Invalid!");
	}
}

int CDNSQLActozCommonManager::QueryAddCharacterInfo(INT64 biCharacterDBID, int nWorldID, WCHAR *pCharName, UINT nAccountDBID, WCHAR *pAccountName, BYTE cJob, TIMESTAMP_STRUCT &CreateDate, char *pIp)
{
	CDNSQLActozCommon *pActozCommonDB = FindActozCommonDB();
	if (!pActozCommonDB)
		return ERROR_GENERIC_DBCON_NOT_FOUND;

	CDNSQLActozCommonAuto Auto(pActozCommonDB, this);

	USES_CONVERSION;
	return pActozCommonDB->QueryAddCharacterInfo(biCharacterDBID, nWorldID, (char*)W2CA(pCharName), nAccountDBID, (char*)W2CA(pAccountName), cJob, 1, 0, 0, ActozCommon::CharacterStatus::Create, CreateDate, pIp);
}

int CDNSQLActozCommonManager::QueryUpdateCharacterStatus(INT64 biCharacterDBID, char cStatus)
{
	CDNSQLActozCommon *pActozCommonDB = FindActozCommonDB();
	if (!pActozCommonDB)
		return ERROR_GENERIC_DBCON_NOT_FOUND;

	CDNSQLActozCommonAuto Auto(pActozCommonDB, this);

	return pActozCommonDB->QueryUpdateCharacterStatus(biCharacterDBID, cStatus);
}

#endif	// #if defined(_KRAZ)