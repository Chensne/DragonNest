#pragma once

#include "DNSQLWorld.h"
#include "MultiSync.h"
#include "DNServiceConnection.h"

class CDNSQLWorldManager
{
private:
	typedef std::vector<CDNSQLWorld*> TVecWorld;
	typedef std::queue<CDNSQLWorld*> TQueueWorld;
	typedef std::map<DWORD, CDNSQLWorld*> TMapWorld;

	TVecWorld m_pVecWorld[WORLDCOUNTMAX];
	TQueueWorld m_pQueueWorld[WORLDCOUNTMAX];
	TMapWorld m_pMapWorld[WORLDCOUNTMAX];

#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	TVecWorld m_pVecWorldHeartbeat;
#endif

	TDatabaseVersion	m_DatabaseVersion[WORLDCOUNTMAX];

	std::vector<int> m_VecWorldConnectError;
	std::vector<int> m_VecWorldVersionError;
	CSyncLock m_ErrorSync;

#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	ULONG m_nLastHearbeatTick;
#endif

public:
	CSyncLock m_Sync;

public:
	CDNSQLWorldManager(UINT uiWorkerThreadCount);
	~CDNSQLWorldManager(void);

	void Clear();
	bool CreateDB();

	void DoUpdate(DWORD nCurTick);

	TDatabaseVersion* GetVersion( int nWorldSetID ){ return &m_DatabaseVersion[nWorldSetID]; }

	CDNSQLWorld* FindWorldDB(int nWorldSetID);
	CDNSQLWorld* FindWorldDB();
	void FreeWorldDB(int nWorldSetID, CDNSQLWorld* pWorldDB);

	void ClearWorldDB(int nWorldID);
	int CreateEachDB(int nWorldDB, int & nIndex);
	void ReportErrorWorldDB(CDNServiceConnection* pConnection);

	// ĳ���� ����
	int QueryAddCharacter(int nWorldSetID, INT64 biCharacterDBID, UINT nAccountDBID, WCHAR *pAccountName, char cAccountLevel, int nWorldID, WCHAR *pCharName, char cClass, char cCharIndex, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, 
		int nMapID, TPosition &Pos, float fRotate, int nRebirthCoin, int nPCBangRebirthCoin, int *nEquipArray, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray, 
		TCreateCharacterItem *CreateItemArray, BYTE cCreateItemMax, bool bJoinBeginnerGuild, UINT &nGuildDBID, WCHAR* pwszIP );	// ĳ���� �߰� (�⺻ �Ӽ�) - P_AddCharacter, P_FirstUseCharacter, P_AddMaterializedItem
	int QueryRollbackAddCharacter(int nWorldSetID, INT64 biCharacterDBID);	// ĳ���� �߰� �ѹ� - P_RollbackAddCharacter
	int QueryFirstUseCharacter(int nWorldSetID, INT64 biCharacterDBID, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray);	// ĳ���� �ΰ� �Ӽ� ���� (ĳ���� ù ���� ȣ��) - P_FirstUseCharacter
	// ĳ���� ����
	int QueryDelCharacter(int nWorldSetID, INT64 biCharacterDBID, OUT bool &bVillageFirstVisit, OUT TIMESTAMP_STRUCT& DeleteDate );	// ĳ���� ���� - P_DelCharacter
	int QueryReviveCharacter( int nWorldSetID, INT64 biCharacterDBID );
	// ĳ���� �Ӽ� ��ȸ
#if defined(PRE_MOD_SELECT_CHAR)
#if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(int nWorldSetID, int nWorldID, UINT nAccountDBID, int nCharacterMaxCount, OUT std::map<INT64, TDBListCharData> &MapCharacterList, INT64 biDWCCharacterDBID, BYTE cAccountLevel);	// ĳ���� ��� - P_GetListCharacter
#else // #if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(int nWorldSetID, int nWorldID, UINT nAccountDBID, int nCharacterMaxCount, OUT std::map<INT64, TDBListCharData> &MapCharacterList);	// ĳ���� ��� - P_GetListCharacter
#endif // #if defined( PRE_ADD_DWC )
#else	// #if defined(PRE_MOD_SELECT_CHAR)
#if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(int nWorldSetID, int nWorldID, UINT nAccountDBID, int nCharacterMaxCount, OUT TDBListCharData *CharList, INT64 biDWCCharacterDBID, BYTE cAccountLevel);	// ĳ���� ��� - P_GetListCharacter
#else // #if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(int nWorldSetID, int nWorldID, UINT nAccountDBID, int nCharacterMaxCount, OUT TDBListCharData *CharList);	// ĳ���� ��� - P_GetListCharacter
#endif // #if defined( PRE_ADD_DWC )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	int QueryModLastVillageMapID( int nWorldSetID, UINT uiAccountDBID, INT64 biCharacterDBID, int iLastVillageMapID );
	int QueryGetCharacterPartialy8(int nWorldSetID, INT64 biCharacterDBID, WCHAR *pCharName, int &nWorldID);	// select �ڵ� 8���ΰ�� : ĳ���� �Ӽ� �Ϻ� ��ȸ (by ĳ���� ID) - P_GetCharacterPartialy

	int QueryAddSkill(int nWorldSetID, INT64 biCharacterDBID, int nSkillID, char cSkillLevel, int nCoolTime, char cSkillChangeCode, INT64 biCoin, int nChannelID, int nMapID, char cSkillPage);
#ifdef PRE_ADD_BEGINNERGUILD
	int QueryAddWillSendMail(int nWorldSetID, INT64 biCharacterDBID, int nMailID, const WCHAR * pMemo);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
#ifdef PRE_ADD_DOORS
	int QueryCancelDoorsAuthentication(int nWorldSetID, UINT nAccountDBID);
#endif		//#ifdef PRE_ADD_DOORS

#if defined( PRE_ADD_DWC )
	int QueryAddDWCCharacter( int nWorldSetID, INT64 biCharacterDBID, UINT nAccountDBID, WCHAR *pAccountName, int nWorldID, WCHAR *pCharName, char cClass, char cJobCode1, char cJobCode2, char cLevel, int nExp, char cCharIndex, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, int nMapID, TPosition &Pos, float fRotate, int nRebirthCoin, int nPCBangRebirthCoin, 
		int *nEquipArray, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray, TCreateCharacterItem *CreateItemArray, BYTE cCreateItemMax, WCHAR* pwszIP, short nSkillPoint, int nGlod );
	int QueryGetDWCCharacterID( int nWorldSetID, UINT nAccountDBID, OUT INT64& biDWCCharacterDBID, int nDeleteWaitingTime );
	int QueryGetDWCChannelInfo( int nWorldSetID, OUT TDWCChannelInfo * pChannelInfo );
#endif // #if defined( PRE_ADD_DWC )
};

extern CDNSQLWorldManager* g_pSQLWorldManager;


class CDNSQLWorldAuto
{
public:
	CDNSQLWorldAuto(int nWorldSetID, CDNSQLWorld* pThis, CDNSQLWorldManager* pOwner)
	{
		DN_ASSERT(CHECK_RANGE(nWorldSetID, 1, WORLDCOUNTMAX),	"Invalid!");
		DN_ASSERT(NULL != pThis,							"Invalid!");
		DN_ASSERT(NULL != pOwner,							"Invalid!");
		m_nWorldSetID = nWorldSetID;
		m_This = pThis;
		m_Owner = pOwner;
	}
	~CDNSQLWorldAuto()
	{
		if (m_This && m_Owner) {
			m_Owner->FreeWorldDB(m_nWorldSetID, m_This);
		}
	}
private:
	char m_nWorldSetID;
	CDNSQLWorld* m_This;
	CDNSQLWorldManager* m_Owner;
};

