
#pragma once

#include "DNSQLMembership.h"
#include "MultiSync.h"

class CDNSQLMembershipManager
{
private:
	typedef std::vector<CDNSQLMembership*> TVecMembership;
	typedef std::queue<CDNSQLMembership*> TQueueMembership;
	typedef std::map<DWORD, CDNSQLMembership*> TMapMembership;

	TVecMembership m_pVecMembership;
	TQueueMembership m_pQueueMembership;
	TMapMembership m_pMapMembership;
#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	TVecMembership m_pVecMembershipHeartbeat;
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL)

	TDatabaseVersion m_DatabaseVersion;

#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	ULONG m_nLastHearbeatTick;
#endif

public:
	CSyncLock m_Sync;

public:
	CDNSQLMembershipManager(UINT uiWorkerThreadCount);
	~CDNSQLMembershipManager(void);

	void Clear();
	bool CreateDB();

	void DoUpdate(DWORD nCurTick);

	TDatabaseVersion* GetVersion(){ return &m_DatabaseVersion; }

	CDNSQLMembership* FindMembershipDB();
	void FreeMembershipDB(CDNSQLMembership* pMembershipDB);

	int QueryLogin(CDNUserConnection *pUserCon, WCHAR *pPassword, BOOL bDoLock);

	int QueryLogout(UINT nAccountDBID, UINT nSessionID, const BYTE * pMachineID = NULL);
	int QueryAddAccountKey(UINT nAccountDBID, UINT nUserNo);

	int QueryAddCharacter(UINT nAccountDBID, WCHAR *pCharName, int nWorldID, int nDefaultMaxCharacterCountPerAccount, OUT INT64 &biCharacterDBID, OUT TIMESTAMP_STRUCT &CreateDate);
	int QueryRollbackAddCharacter(INT64 biCharacterDBID);
	int QueryDelCharacter(INT64 biCharacterDBID, WCHAR *pPrivateIp, WCHAR *pPublicIp, bool bFirstVillage, TIMESTAMP_STRUCT& DeleteDate );
	int QueryReviveCharacter( INT64 biCharacterDBID, WCHAR* wszPrivateIP, WCHAR* wszPublicIP );
	int QueryGetCharacterCount( CDNUserConnection* pUserCon, std::map<int,int>& mWorldUserCount );

	int QueryStoreAuth(UINT uiAccountDBID, INT64 biCertifyingKey, int nCurServerID, char cWorldID, UINT nSessionID, INT64 biCharacterDBID, LPCWSTR wszAccountName, LPCWSTR wszCharacterName, BYTE bIsAdult, char nAge, BYTE nDailyCreateCount, int nPrmInt1, char cLastServerType);
	int QueryCheckAuth(UINT uiAccountDBID, INT64 biCertifyingKey, int nCurServerID, OUT char& cWorldSetID, OUT UINT& nSessionID, OUT INT64& biCharacterDBID, OUT LPWSTR* wszAccountName, OUT LPWSTR* wszCharacterName, OUT char& cAccountLevel, OUT BYTE& bIsAdult, OUT char& nAge, OUT BYTE& nDailyCreateCount, OUT int& nPrmInt1, OUT char& cLastServerType);
	int QueryResetAuth(UINT uiAccountDBID, UINT nSessionID);	
	int QueryResetAuthServer(int nServerID);
	int QueryResetAuthByAccountWorld(UINT uiAccountDBID, char cWorldID, UINT nSessionID);

	int QueryAuthUserCount();
	int QuerySetWorldID(char cWorldID, UINT nAccountDBID);

	// 2차 인증 관련 쿼리
	int QueryValidataSecondAuthPassphrase( UINT uiAccountDBID, const WCHAR* pszPW, BYTE& cFailCount );
	int QueryModSecondAuthPassphrase( UINT uiAccountDBID, const WCHAR* pszOldPW, const WCHAR* pszNewPW, BYTE& cFailCount );
	int QueryModSecondAuthLockFlag( UINT uiAccountDBID, bool bLock );
	int QueryGetSecondAuthStatus( UINT uiAccountDBID, bool& bSetPW, bool& bLock, __time64_t& tResetDate );
	int QueryInitSecondAuth( UINT uiAccountDBID );

#ifdef PRE_ADD_23829
	int QueryCheckLastSecondAuthNotifyDate(UINT nAccountDBID, int nCheckPeriod);		//Period : day
	int QueryModLastSecondAuthNotifyDate(UINT nAccountDBID);
#endif	//#ifdef PRE_ADD_23829

	int QueryAddRestraint( DBDNWorldDef::RestraintTargetCode::eCode TargetCode, UINT uiAccountDBID, INT64 biCharacterDBID, int iReasonID, DBDNWorldDef::RestraintTypeCode::eCode TypeCode, WCHAR* pwszMemo, WCHAR* pwszRestraintMsg, TIMESTAMP_STRUCT StartDate, TIMESTAMP_STRUCT EndDate );
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	int QueryAddBlockedIP(int nWorldID, UINT nAccountDBID, const char * pszIP);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	int QueryCheckPCRoomIP(char *pIp, bool &bPCbang, char &cPCbangGrade);	// 5.PC방 IP 여부 확인 - P_CheckPCRoomIP

	int QueryGetCharacterMaxCount(UINT nAccountDBID, char &cCharMaxCount);	// P_GetCharacterMaxCount
	int QueryGetCharacterSlotCount(UINT nAccountDBID, int nWorldID, INT64 biCharacterDBID, char &cCharMaxCount);	// P_GetCharacterSlotCount
	int QueryGetNationalityCode(UINT nAccountDBID, BYTE &cRegion);

#ifdef PRE_ADD_DOORS
	int QueryGetAuthenticationFlag(UINT nAccountDBID, bool &bFlag);
	int QueryGetDoorsAuthentication(UINT nACcountDBID, char * pszAuthKey);
	int QueryCancelDoorsAuthentication(UINT nAccountDBID);
#endif		//#ifdef PRE_ADD_DOORS

	int QueryModCharacterSortCode(UINT nAccountDBID, BYTE cCharacterSortCode);
	int QueryGetCharacterSortCode(UINT nAccountDBID, BYTE &cCharacterSortCode);
	int QueryCheckLogin(CSCheckLoginTW *pLogin);
	int QueryGetAccountID(const char *username);
};

extern CDNSQLMembershipManager* g_pSQLMembershipManager;


class CDNSQLMembershipAuto
{
public:
	CDNSQLMembershipAuto(CDNSQLMembership* pThis, CDNSQLMembershipManager* pOwner)
	{
		DN_ASSERT(NULL != pThis,	"Invalid!");
		DN_ASSERT(NULL != pOwner,	"Invalid!");
		m_This = pThis;
		m_Owner = pOwner;
	}
	~CDNSQLMembershipAuto()
	{
		if (m_This && m_Owner) {
			m_Owner->FreeMembershipDB(m_This);
		}
	}
private:
	CDNSQLMembership* m_This;
	CDNSQLMembershipManager* m_Owner;
};

