#include "StdAfx.h"
#include "DNSQLMembershipManager.h"
#include "DNUserConnectionManager.h"
#include "DNMasterConnectionManager.h"
#include "DNAuthManager.h"
#include "DNLogConnection.h"
#include "DNIocpManager.h"
#if defined(_TW)
#include "DNGamaniaAuth.h"
#endif	// _TW
#ifdef PRE_ADD_COMEBACK
#include "DNExtManager.h"
#endif		//#ifdef PRE_ADD_COMEBACK

CDNSQLMembershipManager* g_pSQLMembershipManager = NULL;

extern TLoginConfig g_Config;
extern TGameOptions g_GameOption;

CDNSQLMembershipManager::CDNSQLMembershipManager(UINT uiWorkerThreadCount)
{
	Clear();
#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	m_nLastHearbeatTick = timeGetTime();
#endif
}

CDNSQLMembershipManager::~CDNSQLMembershipManager(void)
{
	Clear();
}

void CDNSQLMembershipManager::Clear()
{
	TVecMembership::iterator iter = m_pVecMembership.begin();
	for (; m_pVecMembership.end() != iter ; ++iter) {
		CDNSQLMembership* pSQLCon = (*iter);
		DN_ASSERT(NULL != pSQLCon,	"Check!");
		delete pSQLCon;
	}
	m_pVecMembership.clear();

	while(!m_pQueueMembership.empty()) {
		m_pQueueMembership.pop();
	}

#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	m_pVecMembershipHeartbeat.clear();
#endif
}

bool CDNSQLMembershipManager::CreateDB()
{
	CDNSQLMembership *pMembership = NULL;
	for (int i = 0; i < g_Config.nSQLMax; i++){
		pMembership = new CDNSQLMembership;

		pMembership->m_cThreadID = i;
#if defined(_KRAZ)
		if (!g_Config.bTestServer)
			pMembership->SetMembershipDB();
#endif	// #if defined(_KRAZ)
	
		if (pMembership->Connect(g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID) < 0)
		{
			g_Log.Log(LogType::_FILELOG, L"Not Connect MembershipDB Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
			SAFE_DELETE(pMembership);
			return false;
		}

		m_pVecMembership.push_back(pMembership);
		m_pQueueMembership.push(pMembership);

		if( i==0 )
		{
			int iRet = pMembership->QueryGetDatabaseVersion( &m_DatabaseVersion );
			if( iRet == ERROR_NONE )
			{
				g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Success!! Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
			}
			else
			{
				g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Failed!! Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
				return false;
			}
		}
		Sleep(1);
	}

	return true;
}

void CDNSQLMembershipManager::DoUpdate(DWORD nCurTick)
{
#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	if (m_nLastHearbeatTick + DNProcessCheck::SQLHeartbeatInterval < nCurTick)
	{
		m_nLastHearbeatTick = timeGetTime();

		CDNSQLMembership* pMembership = NULL;
		ScopeLock<CSyncLock> aLock(m_Sync);

		//일단 사용중이지 않은 것들에 대해서만 해주면 데겠다. 사용중이라면 구지해줄 필요 없음
		while (!m_pQueueMembership.empty())
		{
			pMembership = m_pQueueMembership.front();
			LONG nRefCount = pMembership->IncRefCount();
			if (1 != nRefCount) {
				DN_ASSERT(0,	"Check!");
			}
			m_pQueueMembership.pop();
			m_pVecMembershipHeartbeat.push_back(pMembership);
		}

		//싸그리 다 뽑았다
		for (TVecMembership::iterator ii = m_pVecMembershipHeartbeat.begin(); ii != m_pVecMembershipHeartbeat.end(); )
		{
			pMembership = (*ii);
			if (pMembership->QueryHeartbeat() != ERROR_NONE)		//쿵떡쿵떡 신호보내고
				_DANGER_POINT();

			LONG nRefCount = pMembership->DecRefCount();			
			_ASSERT_EXPR(0 == nRefCount, L"RefCount MisMatch");			//머임? 이럼 곤란하심
			m_pQueueMembership.push(pMembership);

			ii = m_pVecMembershipHeartbeat.erase(ii);
		}
		m_pVecMembershipHeartbeat.clear();
	}
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
}

CDNSQLMembership* CDNSQLMembershipManager::FindMembershipDB()
{
	CDNSQLMembership* pMembership = NULL;
	{
		ScopeLock<CSyncLock> aLock(m_Sync);

		DWORD dwThreadID = ::GetCurrentThreadId();
		DN_ASSERT(0 != dwThreadID,	"Check!");

		TMapMembership::iterator iter = m_pMapMembership.find(dwThreadID);
		if (m_pMapMembership.end() != iter) {
			pMembership = (iter->second);
			LONG nRefCount = pMembership->IncRefCount();
			if (1 >= nRefCount) {
				DN_ASSERT(0,	"Check!");
			}
			return pMembership;
		}

		if (m_pQueueMembership.empty()) {
			return NULL;
		}

		pMembership = m_pQueueMembership.front();
		LONG nRefCount = pMembership->IncRefCount();
		if (1 != nRefCount) {
			DN_ASSERT(0,	"Check!");
		}
		m_pQueueMembership.pop();
		m_pMapMembership.insert(TMapMembership::value_type(dwThreadID, pMembership));	// 동기화 상태이므로 결과체크 필요없음 ?
	}
	DN_ASSERT(NULL != pMembership,	"Invalid!");

	return pMembership;
}

void CDNSQLMembershipManager::FreeMembershipDB(CDNSQLMembership* pMembershipDB)
{
	if (!pMembershipDB) return;

	{
		ScopeLock<CSyncLock> aLock(m_Sync);

		LONG nRefCount = pMembershipDB->DecRefCount();
		if (0 == nRefCount) {
			m_pMapMembership.erase(::GetCurrentThreadId());
			m_pQueueMembership.push(pMembershipDB);
		}
		DN_ASSERT(0 <= nRefCount,	"Invalid!");
	}
}

int CDNSQLMembershipManager::QueryGetAccountID(const char *username)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);
	return pMembershipDB->QueryGetAccountID(username);

}

int CDNSQLMembershipManager::QueryCheckLogin(CSCheckLoginTW *pLogin)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);
	return pMembershipDB->QueryCheckLogin(pLogin);

}
int CDNSQLMembershipManager::QueryLogin(CDNUserConnection *pUserCon, WCHAR *pPassword, BOOL bDoLock)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) return ERROR_GENERIC_DBCON_NOT_FOUND;

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	BYTE cStep = 0;
#if defined(PRE_ADD_LOGINLOGOUT_LOG)
	// Step 1 -- 계정 조회,LOGIN로그 기록 없음x
	cStep = 1;
#endif	// #if defined(PRE_ADD_LOGINLOGOUT_LOG)

	Login::TQueryLoginOutput LoginOutput = {0,};
	int nRet = pMembershipDB->QueryLogin(pUserCon, pPassword, cStep, LoginOutput);
	if (nRet != ERROR_NONE)
	{
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
		if (nRet == ERROR_DB_RESTRICTED_IP)
		{
			g_pMasterConnectionManager->SendDetachUserbyIP(pUserCon->GetIp());
#ifdef _USE_ACCEPTEX
			CSyncLock* pConSync = (bDoLock)?(&g_pUserConnectionManager->m_ConSync):(NULL);
			ScopeLock<CSyncLock> Lock(pConSync);
#endif
			g_pUserConnectionManager->DetachUserByIP(pUserCon->GetAccountDBID(), pUserCon->GetIp());
		}
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
		return nRet;	// 제대로 처리 안된경우면 나간다
	}

#ifdef _USE_ACCEPTEX
	CSyncLock* pConSync = (bDoLock)?(&g_pUserConnectionManager->m_ConSync):(NULL);
	ScopeLock<CSyncLock> Lock(pConSync);
#endif	// _USE_ACCEPTEX

	CDNUserConnection *pCheckUserCon = g_pUserConnectionManager->GetConnectionByAccountDBID(LoginOutput.nAccountDBID);
	if (pCheckUserCon){
		pCheckUserCon->DetachConnection(L"Duplicate User");
		return ERROR_GENERIC_DUPLICATEUSER;
	}

	pUserCon->SetAccountDBID(LoginOutput.nAccountDBID);

	//option check
	bool bNeedDefault = false;
	if (g_GameOption.cCommunityDisplayOptionCount != pUserCon->m_GameOptions.cCommunityDisplayOptionCount ||
		g_GameOption.cCommunityOptionCount != pUserCon->m_GameOptions.cCommunityOptionCount ||
		g_GameOption.cDisplayOptionCount != pUserCon->m_GameOptions.cDisplayOptionCount)
		bNeedDefault = true;

	if (LoginOutput.IsNewAccount || bNeedDefault){
		TGameOptions Option;
		memset(&Option, 0, sizeof(Option));

		Option.SetDefault();		//기본 옵션으로 초기화.

		if ((nRet = pMembershipDB->QueryModGameOption(pUserCon->GetAccountDBID(), Option)) != ERROR_NONE)
			g_Log.Log(LogType::_ERROR, pUserCon, L"[SID:%u] [QueryLogin] QuerySetOption Error (Ret:%d)\r\n", pUserCon->GetSessionID(), nRet);

		//에러지만 그냥 진행한다(게임진행에 영향없습니다)
		pUserCon->m_GameOptions = Option;
	}

	std::vector<TRestraintForAccountAndCharacter> VecRestraint;
	VecRestraint.clear();

	nRet = pMembershipDB->QueryGetListRestraintForAccount(pUserCon->GetAccountDBID(), VecRestraint);
	if (nRet == ERROR_NONE){
		if (!VecRestraint.empty()){
			__time64_t servertime;
			time(&servertime);

			for (int i = 0; i < (int)VecRestraint.size(); i++){
				pUserCon->SendCheckBlock(servertime, &VecRestraint[i] );
				//지금 서버에서 강제로 끊진 않는다 강제로 끊는 처리 하는게 좋을 듯
			}

			return nRet;
		}
	}
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	if(LoginOutput.IsNewAccount)
		pUserCon->m_eUserGameQuitReward = GameQuitReward::RewardType::NewbieReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#ifdef PRE_ADD_COMEBACK
	if (g_pExtManager->GetGlobalWeightIntValue(ComebackTerm_Day) > 0)
	{
		__time64_t _tLogOutTime;
		CTimeParamSet LogOutParamTime(NULL, _tLogOutTime, LoginOutput.LogOutDate, 0, LoginOutput.nAccountDBID);
		if (LogOutParamTime.IsValid())
		{
			__time64_t _tNowTime;
			time(&_tNowTime);

			static __time64_t s_tTerm = 0;
			if (s_tTerm == 0)
			{
				s_tTerm = 86400 * g_pExtManager->GetGlobalWeightIntValue(ComebackTerm_Day);		//음 변경되어지지 않는 값이라 상수가 들어가는데 글로벌웨이트 테이블이 변신되어지면 변경해야한다.
			}

#if defined( PRE_ADD_NEWCOMEBACK )
			if (_tNowTime - _tLogOutTime > s_tTerm || pUserCon->m_bComebackUser)
#else
			if (_tNowTime - _tLogOutTime > s_tTerm)
#endif
			{
				pUserCon->m_bComebackUser = true;
#if defined(PRE_ADD_GAMEQUIT_REWARD)
				pUserCon->m_eUserGameQuitReward = GameQuitReward::RewardType::ComeBackReward;
				if(pUserCon->m_bReConnectNewbieReward)
				{
					//귀환자 접속 보상과 신규 유저 접속 보상이 겹치는 경우, 신규 유저 재접속 보상은 받은걸로 체크
					pMembershipDB->QueryModNewbieRewardFlag(pUserCon->GetAccountDBID(), true);
					pUserCon->m_bReConnectNewbieReward = false;
				}
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined( PRE_ADD_NEWCOMEBACK )
				pMembershipDB->QueryModComebackFlag(pUserCon->GetAccountDBID(), true);
#endif
				g_Log.Log(LogType::_COMEBACK, pUserCon, L"AccountDBID[%d] ComebackUser!\n", pUserCon->GetAccountDBID());
			}
		}
		else{
			g_Log.Log(LogType::_COMEBACK, pUserCon, L"AccountDBID[%d] LogOutParamTime.IsValid() LogOutDate(%d-%d-%d %d:%d)!\n", 
				pUserCon->GetAccountDBID(), LoginOutput.LogOutDate.year, LoginOutput.LogOutDate.month, LoginOutput.LogOutDate.day, LoginOutput.LogOutDate.hour, LoginOutput.LogOutDate.minute);
		}
	}
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)	
	if(pUserCon->m_bReConnectNewbieReward)
	{
		__time64_t _tRegistDate;
		CTimeParamSet LogOutParamTime(NULL, _tRegistDate, LoginOutput.RegistDate, 0, LoginOutput.nAccountDBID);
		if (LogOutParamTime.IsValid())
		{
			__time64_t _tNowTime;
			time(&_tNowTime);

			static __time64_t s_tTerm = 0;
			if (s_tTerm == 0)
			{
				s_tTerm = 3600 * g_pExtManager->GetGlobalWeightIntValue(NewbieReConnectReward_Hour);		//GlobalWeightIntTable이 변경되면 같이 수정해줄것!!
			}

			if (_tNowTime - _tRegistDate < s_tTerm)
				pUserCon->m_bReConnectNewbieReward = false;
			else
				g_Log.Log(LogType::_GAMEQUITREWARD, pUserCon, L"AccountDBID[%d] NewUser ReConnect Reward!\n", pUserCon->GetAccountDBID());

			pUserCon->m_tAccountRegistDate = _tRegistDate;
		}
		else
		{
			pUserCon->m_bReConnectNewbieReward = false;
			pUserCon->m_tAccountRegistDate = 0;
			g_Log.Log(LogType::_GAMEQUITREWARD, pUserCon, L"AccountDBID[%d] RegistDate ParamTime.IsValid() RegistDate(%d-%d-%d %d:%d)!\n", 
				pUserCon->GetAccountDBID(), LoginOutput.RegistDate.year, LoginOutput.RegistDate.month, LoginOutput.RegistDate.day, LoginOutput.RegistDate.hour, LoginOutput.RegistDate.minute);
		}
	}
	else
		pUserCon->m_tAccountRegistDate = 0;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)

	char cWorldSetID = 0, cCertifyingStep = 0;
	int nServerID = g_Config.nManagedID;
	UINT nSessionID = pUserCon->GetSessionID();
	INT64 biCharacterDBID = pUserCon->GetSelectCharacterDBID();

	int nResult = pMembershipDB->QueryBeginAuth(pUserCon->GetAccountDBID(), cCertifyingStep, nServerID, cWorldSetID, nSessionID, biCharacterDBID, pUserCon->GetAccountName(), 
		(pUserCon->GetSelectCharacterName())?(pUserCon->GetSelectCharacterName()):(_T("")), pUserCon->GetAccountLevel(), pUserCon->GetAdult(), pUserCon->GetAge(), pUserCon->GetDailyCreateCount(), pUserCon->GetPrmInt1());

	g_Log.Log((0 > nResult)?(LogType::_ERROR):(LogType::_NORMAL), pUserCon, L"[Auth] BeginAuth (REQ) - Result:%d, nAccountDBID:%d (%s) LogOutDate(%d-%d-%d %d:%d)\r\n", 
		nResult, pUserCon->GetAccountDBID(), pUserCon->GetAccountName(), LoginOutput.LogOutDate.year, LoginOutput.LogOutDate.month, LoginOutput.LogOutDate.day, LoginOutput.LogOutDate.hour, LoginOutput.LogOutDate.minute);

	if (ERROR_NONE != nResult) {
		switch(nResult) {
		case ERROR_GENERIC_DUPLICATEUSER:
			{				
#if defined(_WORK)
				// 사내서버는 그냥 밀고 들어가불자.
				g_pAuthManager->QueryResetAuth(cWorldSetID, pUserCon->GetAccountDBID(), nSessionID);
#else
				if (nServerID == g_Config.nManagedID)  {
					nResult = pMembershipDB->QueryResetAuth(pUserCon->GetAccountDBID(), nSessionID);	// 서버 ID 가 고유하다는 전제로 같은 서버였다면 세션 ID 까지 출력받은 값으로 맞추어 강제로 초기화 시킴
					if (ERROR_NONE != nResult) {
						g_Log.Log(LogType::_ERROR, pUserCon, L"[QueryResetAuth] Query Error Result:%d\r\n", nResult);
					}
				}
				else 
				{				
					//그냥 끊으라고 보내주자...DB랑 다르게 다른곳에 있는 넘은 말도 안됨..제발..
					// 여기서 마스터서버만 다운된 경우도 못들어 가게 막아준다..마스터가 다시 올라오면 ResetAuth 해줌.
#if defined(PRE_MOD_SELECT_CHAR)
					if( g_pMasterConnectionManager->SendDetachUserConnectionByWorldSetID(cWorldSetID, pUserCon->GetAccountDBID(), false, true, nSessionID) == true)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
					if( g_pMasterConnectionManager->SendDetachUserConnectionByWorldSetID(cWorldSetID, pUserCon->GetAccountDBID(), pUserCon->GetAccountLevel(), false, NULL, 0, true, nSessionID, pUserCon->GetIp()) == true)
#else		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
					if( g_pMasterConnectionManager->SendDetachUserConnectionByWorldSetID(cWorldSetID, pUserCon->GetAccountDBID(), pUserCon->GetAccountLevel(), false, NULL, 0, true, nSessionID) == true)
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
					{
						// 여기서 문제...일시적으로 마스터서버가 다운된 애들은 마스터서버가 올라왔을때?? 중복로그인 처리로 걸리게 하자.
						if( cWorldSetID == 0 ) //로그인 서버에 있는 넘이면 맨처음 마스터 서버에 자신을 제외한 로그인서버의 유저를 끊으라고 요청
						{
							g_pMasterConnectionManager->SendDetachUserOtherLogin(pUserCon->GetAccountDBID(), g_pAuthManager->GetServerID());
							// 로그인 일때는 그냥 ResetAuth 하자.						
							g_pAuthManager->QueryResetAuth(cWorldSetID, pUserCon->GetAccountDBID(), nSessionID);
						}
					}
				}
#endif //#if defined(_WORK)
			}
			break;
		default:
			break;
		}

		return ERROR_GENERIC_DUPLICATEUSER;		// 사용자에게 너무 세부적인 인증정보 오류내용을 알려줄 경우 악용의 소지가 있음
	}

	pUserCon->m_bCertified = true;
#if defined(PRE_ADD_LOGINLOGOUT_LOG)
	// Step 2 -- LOGIN로그 기록
	pMembershipDB->QueryLogin(pUserCon, pPassword, 2, LoginOutput);
#endif

	// pcbang체크
	pUserCon->CheckPCBangIp();

	return nRet;
}

int CDNSQLMembershipManager::QueryLogout(UINT nAccountDBID, UINT nSessionID, const BYTE * pMachineID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);
	return pMembershipDB->QueryLogout(nAccountDBID, nSessionID, pMachineID);
}

int CDNSQLMembershipManager::QueryAddAccountKey(UINT nAccountDBID, UINT nUserNo)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryAddAccountKey(nAccountDBID, nUserNo);
}

int CDNSQLMembershipManager::QueryAddCharacter(UINT nAccountDBID, WCHAR *pCharName, int nWorldID, int nDefaultMaxCharacterCountPerAccount, OUT INT64 &biCharacterDBID, OUT TIMESTAMP_STRUCT &CreateDate)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);
	return pMembershipDB->QueryAddCharacter(nAccountDBID, pCharName, nWorldID, nDefaultMaxCharacterCountPerAccount, biCharacterDBID, CreateDate);
}

int CDNSQLMembershipManager::QueryRollbackAddCharacter(INT64 biCharacterDBID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryRollbackAddCharacter(biCharacterDBID);
}

int CDNSQLMembershipManager::QueryDelCharacter(INT64 biCharacterDBID, WCHAR *pPrivateIp, WCHAR *pPublicIp, bool bFirstVillage, TIMESTAMP_STRUCT& DeleteDate )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryDelCharacter(biCharacterDBID, pPrivateIp, pPublicIp, bFirstVillage, DeleteDate );
}

int CDNSQLMembershipManager::QueryReviveCharacter( INT64 biCharacterDBID, WCHAR* wszPrivateIP, WCHAR* wszPublicIP )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryReviveCharacter(biCharacterDBID, wszPrivateIP, wszPublicIP );
}

int CDNSQLMembershipManager::QueryGetCharacterCount( CDNUserConnection* pUserCon, std::map<int,int>& mWorldUserCount )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	mWorldUserCount.clear();

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetCharacterCount( pUserCon->GetAccountName(), mWorldUserCount );
}

int CDNSQLMembershipManager::QueryStoreAuth(UINT uiAccountDBID, INT64 biCertifyingKey, int nCurServerID, char cWorldID, UINT nSessionID, INT64 biCharacterDBID, LPCWSTR wszAccountName, LPCWSTR wszCharacterName, BYTE bIsAdult, char nAge, BYTE nDailyCreateCount, int nPrmInt1, char cLastServerType)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);
	return pMembershipDB->QueryStoreAuth(uiAccountDBID, biCertifyingKey, nCurServerID, cWorldID, nSessionID, biCharacterDBID, wszAccountName, wszCharacterName, bIsAdult, nAge, nDailyCreateCount, nPrmInt1, cLastServerType);
}

int CDNSQLMembershipManager::QueryCheckAuth(UINT uiAccountDBID, INT64 biCertifyingKey, int nCurServerID, OUT char& cWorldSetID, OUT UINT& nSessionID, OUT INT64& biCharacterDBID, OUT LPWSTR* wszAccountName, OUT LPWSTR* wszCharacterName, OUT char& cAccountLevel, OUT BYTE& bIsAdult, OUT char& nAge, OUT BYTE& nDailyCreateCount, OUT int& nPrmInt1, OUT char& cLastServerType)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryCheckAuth(uiAccountDBID, biCertifyingKey, nCurServerID, cWorldSetID, nSessionID, biCharacterDBID, wszAccountName, wszCharacterName, cAccountLevel, bIsAdult, nAge, nDailyCreateCount, nPrmInt1, cLastServerType);
}

int CDNSQLMembershipManager::QueryResetAuth(UINT uiAccountDBID, UINT nSessionID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryResetAuth(uiAccountDBID, nSessionID);
}

int CDNSQLMembershipManager::QueryResetAuthServer(int nServerID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	int nRet = pMembershipDB->QueryResetAuthServer(nServerID);

	if (!g_pAuthManager->IsResetAuthServer()) {
		if (ERROR_NONE != nRet) {
			_DANGER_POINT();
			return nRet;
		}		
	}

	return ERROR_NONE;
}

int CDNSQLMembershipManager::QueryResetAuthByAccountWorld(UINT uiAccountDBID, char cWorldID, UINT nSessionID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryResetAuthByAccountWorld(uiAccountDBID, cWorldID, nSessionID);
}

int CDNSQLMembershipManager::QueryAuthUserCount()
{
	return 0;
}

int CDNSQLMembershipManager::QuerySetWorldID(char cWorldID, UINT nAccountDBID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QuerySetWorldIDAuth(cWorldID, nAccountDBID);
}

// 2차 인증 관련 쿼리
int CDNSQLMembershipManager::QueryValidataSecondAuthPassphrase( UINT uiAccountDBID, const WCHAR* pwszPW, BYTE& cFailCount )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

#if defined(PRE_ADD_SHA256)
	int iRet = pMembershipDB->QueryValidataSecondAuthPassphraseByServer( uiAccountDBID, pwszPW, cFailCount );
#else
	int iRet = pMembershipDB->QueryValidataSecondAuthPassphrase( uiAccountDBID, pwszPW, cFailCount );
#endif//#if defined(PRE_ADD_SHA256)
	// 2차 인증 비밀번호가 일치하지 않습니다.(101127)
	if( iRet == ERROR_SECONDAUTH_CHECK_FAILED )
	{
		// 연속으로 틀린 횟수가 일정 횟수 이상 연속되었다면
		if( cFailCount >= SecondAuth::Common::LimitCount )
		{

		}
	}

	return iRet;
}

int CDNSQLMembershipManager::QueryModSecondAuthPassphrase( UINT uiAccountDBID, const WCHAR* pwszOldPW, const WCHAR* pwszNewPW, BYTE& cFailCount )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

#if defined(PRE_ADD_SHA256)
	int iRet = pMembershipDB->QueryModSecondAuthPassphraseByServer( uiAccountDBID, pwszOldPW, pwszNewPW, cFailCount );
#else
	int iRet = pMembershipDB->QueryModSecondAuthPassphrase( uiAccountDBID, pwszOldPW, pwszNewPW, cFailCount );
#endif // #if defined(PRE_ADD_SHA256)
	// 기존 2차 인증 비밀번호가 일치하지 않습니다.(101126)
	if( iRet == ERROR_SECONDAUTH_CHECK_OLDPWINVALID )
	{
		// 연속으로 틀린 횟수가 일정 횟수 이상 연속되었다면
		if( cFailCount >= SecondAuth::Common::LimitCount )
		{

		}
	}

	return iRet;
}

int CDNSQLMembershipManager::QueryModSecondAuthLockFlag( UINT uiAccountDBID, bool bLock )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryModSecondAuthLockFlag( uiAccountDBID, bLock );
}

int CDNSQLMembershipManager::QueryGetSecondAuthStatus( UINT uiAccountDBID, bool& bSetPW, bool& bLock, __time64_t& tResetDate )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetSecondAuthStatus( uiAccountDBID, bSetPW, bLock, tResetDate );
}


int CDNSQLMembershipManager::QueryInitSecondAuth( UINT uiAccountDBID )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryInitSecondAuth( uiAccountDBID );
}

#ifdef PRE_ADD_23829
int CDNSQLMembershipManager::QueryCheckLastSecondAuthNotifyDate(UINT nAccountDBID, int nCheckPeriod)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryCheckLastSecondAuthNotifyDate( nAccountDBID, nCheckPeriod );
}

int CDNSQLMembershipManager::QueryModLastSecondAuthNotifyDate(UINT nAccountDBID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryModLastSecondAuthNotifyDate(nAccountDBID);
}
#endif	//#ifdef PRE_ADD_23829

int CDNSQLMembershipManager::QueryAddRestraint( DBDNWorldDef::RestraintTargetCode::eCode TargetCode, UINT uiAccountDBID, INT64 biCharacterDBID, int iReasonID, DBDNWorldDef::RestraintTypeCode::eCode TypeCode, WCHAR* pwszMemo, WCHAR* pwszRestraintMsg, TIMESTAMP_STRUCT StartDate, TIMESTAMP_STRUCT EndDate )
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryAddRestraint( TargetCode, uiAccountDBID, biCharacterDBID, iReasonID, TypeCode, pwszMemo, pwszRestraintMsg, StartDate, EndDate );
}

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
int CDNSQLMembershipManager::QueryAddBlockedIP(int nWorldID, UINT nAccountDBID, const char * pszIP)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryAddBlockedIP(nWorldID, nAccountDBID, pszIP);
}
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

int CDNSQLMembershipManager::QueryCheckPCRoomIP(char *pIp, bool &bPCbang, char &cPCbangGrade)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryCheckPCRoomIP(pIp, bPCbang, cPCbangGrade);
}

int CDNSQLMembershipManager::QueryGetCharacterMaxCount(UINT nAccountDBID, char &cCharMaxCount)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetCharacterMaxCount(nAccountDBID, cCharMaxCount);
}

int CDNSQLMembershipManager::QueryGetCharacterSlotCount(UINT nAccountDBID, int nWorldID, INT64 biCharacterDBID, char &cCharMaxCount)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetCharacterSlotCount(nAccountDBID, nWorldID, biCharacterDBID, cCharMaxCount);
}

int CDNSQLMembershipManager::QueryGetNationalityCode(UINT nAccountDBID, BYTE &cRegion)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetNationalityCode(nAccountDBID, cRegion);
}

#ifdef PRE_ADD_DOORS
int CDNSQLMembershipManager::QueryGetAuthenticationFlag(UINT nAccountDBID, bool &bFlag)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetAuthenticationFlag(nAccountDBID, bFlag);
}

int CDNSQLMembershipManager::QueryGetDoorsAuthentication(UINT nACcountDBID, char * pszAuthKey)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetDoorsAuthentication(nACcountDBID, pszAuthKey);
}

int CDNSQLMembershipManager::QueryCancelDoorsAuthentication(UINT nAccountDBID)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryCancelDoorsAuthentication(nAccountDBID);
}
#endif		//#ifdef PRE_ADD_DOORS

int CDNSQLMembershipManager::QueryModCharacterSortCode(UINT nAccountDBID, BYTE cCharacterSortCode)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryModCharacterSortCode(nAccountDBID, cCharacterSortCode);
}

int CDNSQLMembershipManager::QueryGetCharacterSortCode(UINT nAccountDBID, BYTE &cCharacterSortCode)
{
	CDNSQLMembership *pMembershipDB = FindMembershipDB();
	if (!pMembershipDB) {
		return ERROR_GENERIC_DBCON_NOT_FOUND;
	}

	CDNSQLMembershipAuto Auto(pMembershipDB, this);

	return pMembershipDB->QueryGetCharacterSortCode(nAccountDBID, cCharacterSortCode);
}