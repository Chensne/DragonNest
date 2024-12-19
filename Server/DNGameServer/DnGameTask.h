#pragma once

#include "Task.h"
#include "FrameSync.h"
#include "GameServerSession.h"
#include "DnWorld.h"

class CDnWorld;
class CDNGameRoom;
class CDnDungeonHistory;
class CPvPRespawnLogic;
class CDnPlayerActor;

class CDnGameTask : public CTask, public CGameServerSession
{
public:
	CDnGameTask( CMultiRoom *pRoom );
	virtual ~CDnGameTask();
#ifdef __COVERITY__
#else
	FRIEND_TEST( Allocator_unittest, vector );
#endif

	enum DungeonClearStateEnum {
		DCS_None,
		DCS_RequestDungeonClear,
		DCS_ClearResultStay,
		DSC_RequestRankResult,
		DSC_RankResultStay,
		DCS_RequestSelectRewardItem,
		DCS_SelectRewardItemStay,
		DCS_RequestRewardItemStay,
		DCS_RewardItemStay,
		DCS_RequestRewardItemResult,
		DCS_RewardItemResultStay,
		DCS_RequestWarpDungeon,
		DCS_WarpDungeonStay,
		DCS_WarpStandBy,
		DCS_DLRequestChallenge,
	};
protected:
	CFrameSkip	m_FrameSkip;
	CFrameSkip	m_FrameSkipProjectile;
	CDnWorld *m_pWorld;

	int m_nGameTaskType;
	int m_nMapTableID;
#if defined(PRE_ADD_RENEW_RANDOM_MAP)
	int m_nRootMapTableID;
#endif
	std::vector <int> m_vPermitPassClassIds;
	int m_nPermitMapLevel;
	TDUNGEONDIFFICULTY m_StageDifficulty; // 원래 사용 목적 변경!! 스테이지 레벨이 5가지로 변경된다!! Easy, Normal, Hard, Hell, Special
	int m_nStartPositionIndex;

	bool m_bReleaseWorld;

	bool m_bEnteredDungeon;
	int m_nEnterDungeonTableID;
	int m_nEnterMapTableID;
	int m_nReturnDungeonClearWorldID;
	int m_nReturnDungeonClearStartPositionIndex;
#if defined(PRE_SHARE_MAP_CLEARCOUNT)
	int m_nShareCountMapIndex;
#endif

	DNVector(DnActorHandle) m_hVecMonsterList;
	DNVector(DnActorHandle) m_hVecNpcList;

	struct ActorBirthStruct :public TBoostMemoryPool<ActorBirthStruct>
	{
		int nAreaUniqueID;
		DNVector(DnActorHandle) hVecList;
	};
	std::vector<ActorBirthStruct *> m_pVecActorBirthList;

	std::map<int,CDnDungeonHistory *> m_pMapDungeonHistory;

	bool m_bGameProcessDelay;
	float m_fGameProcessDelayDelta;

	bool m_bWaitPlayCutScene;
	bool m_bCutSceneAutoFadeIn;
	bool m_bDungeonFailed;

	DungeonClearStateEnum m_DungeonClearState;
	float m_fDungeonClearDelta;
	bool m_bDungeonClearQuestComplete;
	bool m_bDungeonClearSendWarpEnable;
	bool m_bIgnoreDungeonClearRewardItem;
#if defined( PRE_SKIP_REWARDBOX )
	int m_nRewardBoxUserCount;
#endif

	float m_fAIDelta;
	float m_fProjectileDelta;
	DWORD m_dwStageCreateTime;
	bool	m_bIsTimeAttack;
	float	m_fTimeAttackDelta;
	float	m_fOriginTimeAttackDelta;

	std::map<int, int> m_nMapSetMonsterReference;

#ifdef PRE_ADD_STAGE_DAMAGE_LIMIT
	struct StageDamageLimitStruct 
	{
		float fDamageLimit;
		float fStateLimit;
	};
	StageDamageLimitStruct m_sStageDamageLimitStruct;
#endif

#if defined( PRE_ADD_HEAL_TABLE_REGULATION )
	struct StageHealLimitStruct
	{
		float fHealLimit_Type1;
		float fHealLimit_Type2;
	};
	StageHealLimitStruct m_sStageHealLimitStruct;
#endif

	// 몬스터 생성시에만 쓰이는 스트럭쳐입니다. 외부에서 접근할일 없습니다.
	struct MonsterTableStruct {
		std::string szName;
		int nActorTableID;
	};

	struct GenerationMonsterStruct {
		int nActorTableID;
		int nMonsterTableID;
		CEtWorldEventArea *pArea;
		int nMonsterSpawn;
	};

	struct AreaStruct {
		int nCompoundIndex;
		int nMin;
		int nMax;
		CEtWorldEventArea *pArea;
		int nMonsterSpawn;
	};

	struct MonsterCompoundStruct {
		int nMonsterTableID;
		int nPossessionProb;
		int nValue;
	};

	struct MonsterSetStruct {
		int nSetID;
		int nPosID;
		CEtWorldEventArea *pArea;
		int nMonsterSpawn;
	};

	struct MonsterSetProbStruct {
		int nItemID;
		int nProb;
	};
	struct MonsterGroupStruct {
		int nGroupID;
		int nCount;
	};

	//ChannelMerit
	const TMeritInfo * m_pMeritInfo;

	// 던전 클리어시 사용하는 스트럭쳐
	struct TreasureBoxLevelStruct {
		char cTreasureLevel;
		int nOffset;
	};

	//
	struct sWarpStruct
	{
		bool bActivated;
		CSWarpDungeon sWarp;
	} m_sWarpDungeonStruct;

#if defined( PRE_ADD_STAGE_WEIGHT )
	TStageWeightData m_StageWeightData;
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )

protected:
	bool GenerationMonster( int nRandomSeed );
	bool GenerationNpc( int nRandomSeed );
	bool GenerationSetMonster( int nRandomSeed );
	void ResetSetMonsterReference();
	int CalcMonsterIDFromMonsterGroupID( int nItemID );
	int CalcSetMonsterReference( int nSetID, bool bNewReference = false );
	void CalcSetMonsterGroupList( int nSetTableID, std::map<int, DNVector(MonsterGroupStruct)> &nMapResult );
	char GetTreasureBoxType( int nOffset, DNVector(TreasureBoxLevelStruct) &VecList );

	void SetStartPosition( CDNUserSession* pSession, int nPartyIndex );

	bool InitializeWorld( const char *szGridName );
	void InitializeMonster( DnActorHandle hMonster, DWORD dwUniqueID, int nMonsterID, SOBB &GenerationArea, CEtWorldEventArea *pBirthArea = NULL, int nForceSkillLevel = -1 );

	void BackupDungeonHistory();
	void RecoveryDungeonHistory( int nMapIndex );
	void ClearDungeonHistory();

	void CalcDungeonClear();
#if defined(PRE_SHARE_MAP_CLEARCOUNT)
	void ResetDungeonClear( bool bCreate, int nMapIndex = -1, int nGateIndex = -1, int nEnterMapTabeID = -1, int nDungeonClearTableID = -1, int nShareCountMapIndex = 0 );
#else
	void ResetDungeonClear( bool bCreate, int nMapIndex = -1, int nGateIndex = -1, int nEnterMapTabeID = -1, int nDungeonClearTableID = -1 );
#endif

	void SetGameProcessDelay( int nDelay );
//	void UpdatePlayerWorldLevel();
	bool CheckAndSyncCutScenePlay();
	void CheckAndSyncCutSceneSkip( CDNUserSession *pSession );

	virtual void ChangeDungeonClearState( DungeonClearStateEnum State );
	virtual bool ProcessDungeonClearState( LOCAL_TIME LocalTime, float fDelta );

	void InitializeCP();
	void InitializeBreakIntoPlayerCP( CDNUserSession * pSession );
	virtual bool OnInitializePlayerActor( CDNUserSession* pSession, const int iVectorIndex );

	void InitializeMerit();

	static bool Sort_SetMonster_SetID( MonsterSetStruct &a, MonsterSetStruct &b );
	static bool Sort_SetMonster_PosID( MonsterSetStruct &a, MonsterSetStruct &b );

	void RequestDungeonClearBase();
	void AddDungeonClearUserInfo( std::vector<TDungeonClearInfo> & vInfo, std::vector<CDNGameRoom::PartyStruct *> & vPartyStruct );
	int  CalcBonusCP( const int nDungeonClearID, const int nMaxComboCount, const int nKillBossCount, CDnPlayerActor * pPlayerActor );
	int  CalcCP( std::vector<TDungeonClearInfo> & vInfo, std::vector<CDNGameRoom::PartyStruct *> & vPartyStruct, const int nDungeonClearID );
	BYTE CalcRank( std::vector<float> & vecRankRatio, const int nStandardCP, const int nCP );

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	bool Initialize();
	bool InitializePlayerActor();
	virtual bool OnInitializeBreakIntoActor( CDNUserSession* pSession, const int iVectorIndex );
	virtual bool InitializeStage( int nCurrentMapIndex, int nGateIndex, TDUNGEONDIFFICULTY StageDifficulty, int nRandomSeed, bool bContinueStage, bool bDirectConnect, int nGateSelect );

	// nStageConstructionLevel, nStartPositionIndex 은 1부터 시작.. 주의하시길
	bool InitializeStage( const char *szGridName, int nMapTableID, TDUNGEONDIFFICULTY StageDifficulty, int nStartPositionIndex);
	virtual bool InitializeNextStage( const char *szGridName, int nMapTableID, TDUNGEONDIFFICULTY StageDifficulty, int nStartPositionIndex );
	virtual bool PostInitializeStage( int nRandomSeed );
	void FinalizeStage();

	int GetGameTaskType() { return m_nGameTaskType; }
	int GetMapTableID() { return m_nMapTableID; }
	int GetStartPositionIndex(){ return m_nStartPositionIndex; }
	TDUNGEONDIFFICULTY GetStageDifficulty() { return m_StageDifficulty; }
	void SetStageDifficulty( TDUNGEONDIFFICULTY Difficulty ) { m_StageDifficulty = Difficulty; }
	void GetPermitPassClassIDs(int * pClassArr, int nArrMaxSize);
	int GetPermitMapLevel() { return m_nPermitMapLevel; }

	int CheckRecoveryDungeonHistorySeed( int nMapIndex, int nRandomSeed );
	void CheckMerit(CDnPlayerActor * pActor, int nCheckType, int nCalcVal, UINT &nOutVal);

	DnActorHandle CreateNpc(int nNpcIndex, TNpcData* pNpcData, EtVector3 vPos, float fRot);
	void DestroyNpc(UINT nUniqueID);
	void RequestGenerationNpc( int nNpcID, float x, float y, float z, float fRotate, int nAreaUniqueID = -1 );
	void RequestDestroyNpc(int nAreaUniqueID);

	DnActorHandle RequestGenerationMonsterFromMonsterID( int nMonsterID, EtVector3 &vPosition, EtVector3& vVel, EtVector3& vResistance, SOBB *pGenerationArea = NULL, int nAreaUniqueID = -1, int nTeamSetting = -1 );
	DnActorHandle RequestGenerationMonsterFromActorID( int nMonsterID, int nActorID, EtVector3 &vPosition, EtVector3& vVel, EtVector3& vResistance, SOBB *pGenerationArea = NULL, int nAreaUniqueID = -1, int nTeamSetting = -1, bool bRandomFrameSummon = true, EtQuat* pqRotation = NULL, int nForceSkillLevel = -1 );
	void RequestGenerationMonsterFromMonsterGroupID( int nMonsterGroupID, int nCount, SOBB &GenerationArea, int nAreaUniqueID = -1, DNVector(DnActorHandle) *pVecResult = NULL, int nTeamSetting = -1 );
	void RequestGenerationMonsterFromSetMonsterID( int nSetMonsterID, int nPositionID, SOBB &GenerationArea, int nAreaUniqueID = -1, DNVector(DnActorHandle) *pVecResult = NULL, int nTeamSetting = -1, bool bResetReference = false );
	void RequestGenerationMonsterFromSetMonsterID( int nSetMonsterID, int nPositionID, std::vector<CEtWorldEventArea *> &pVecAreaUniqueList, DNVector(DnActorHandle) *pVecResult = NULL, int nTeamSetting = -1, bool bResetReference = false );

	bool CheckMonsterIsInsideArea( int nAreaUniqueID, int nTargetUniqueID );
	bool CheckMonsterLessHP( int iMonsterID, int iHP );
	bool CheckInsideAreaMonsterLessHP( int iAreaUniqueID, int iHP );
	int	 GetInsideAreaMonsterHPPercent( int iAreaUniqueID );
	EtVector3 GetGenerationRandomPosition( SOBB *pOBB );

	void RequestDestroyAllMonster( bool bDropItem, int nTeam = -1 );

	void RequestChangeGameSpeed( float fSpeed, DWORD dwDelay );

	// ServerSession
	virtual int OnRecvNpcTalkMsg( CDNUserSession * pSession, CSNpcTalk *pPacket, int nLen );
	virtual int OnRecvNpcTalkEndMsg( CDNUserSession * pSession, CSNpcTalkEnd *pPacket, int nLen );

	virtual int OnRecvChatChatMsg( CDNUserSession * pSession, CSChat *pPacket, int nLen );
	virtual int OnRecvChatPrivateMsg( CDNUserSession * pSession, CSChatPrivate *pPacket, int nLen );
	virtual int OnRecvDiceMsg( CDNUserSession * pSession, CSDice *pPacket, int nLen );
	virtual int OnRecvChatChatRoomMsg( CDNUserSession * pSession, CSChatChatRoom *pPacket, int nLen );
#if defined (PRE_ADD_ANTI_CHAT_SPAM)
	virtual int OnRecvChatSpammer( CDNUserSession * pSession, bool* bSpammer );
#endif

	virtual int OnRecvRoomRequestDungeonFailed( CDNUserSession *pSession, char *pPacket );
	virtual int OnRecvRoomWarpDungeon( CDNUserSession * pSession, CSWarpDungeon *pPacket );
	virtual int OnRecvRoomSelectRewardItem( CDNUserSession * pSession, CSSelectRewardItem *pPacket );
	virtual int OnRecvRoomIdentifyRewardItem( CDNUserSession * pSession, CSIdentifyRewardItem *pPacket );

	virtual int OnRecvQuestCompleteCutScene( CDNUserSession * pSession, char *pPacket );
	virtual int OnRecvQuestSkipCutScene( CDNUserSession * pSession, char *pPacket );

	virtual int OnRecvSelectAppellation( CDNUserSession *pSession, CSSelectAppellation *pPacket );
	virtual int OnRecvCollectionBook( CDNUserSession *pSession, CSCollectionBook *pPacket );

	// RUDPGameSession 에서 들어오는거.
	void OnRecvVillageState( CDNUserSession *pSession, MAGAVillageState *pPacket );

	virtual void SetSyncComplete( bool bFlag );

	void InsertBirthAreaList( DnActorHandle hActor, int nAreaUniqueID );
	void RemoveBirthAreaList( int nAreaUniqueID );
	void RemoveBirthAreaList( DnActorHandle hActor, int nAreaUniqueID );
	DWORD GetBirthAreaLiveCount( int nAreaUniqueID );
	DWORD GetBirthAreaTotalCount( int nAreaUniqueID );
	bool GetBirthAreaLiveActors( int nAreaUniqueID, DNVector(DnActorHandle)& out );

	void RequestGateInfo( CDNUserSession* pBreakIntoGameSession=NULL );
	virtual void RequestDungeonClear( bool bClear = true, DnActorHandle hIgnoreActor = CDnActor::Identity(), bool bIgnoreRewardItem = false );
	void RequestDungeonFailed( bool bForce=false, bool bTimeOut=false );
	void RequestChangeGateInfo( char cGateIndex, EWorldEnum::PermitGateEnum PermitFlag );
#ifdef _TEST_CODE_KAL
	void RequestDungeonClear_ForCheat(int mapTableID, DWORD clearTime, int meritBonusID, int enterGateIdx, char partyMemeberCount, int completeExp, int rankIdx, int difficulty);
#endif

	DnPropHandle RequestGenerationProp( int nPropTableID, int nReqClassID, EtVector3 &vPosition, EtVector3 &vRotation, EtVector3 &vScale, int nLifeTime, void* pAdditionalPropInfo );

	void RequestPlayCutScene( int nCutSceneTableID, bool bFadeIn, int nQuestIndex, int nQuestStep);
	bool IsWaitPlayCutScene() { return m_bWaitPlayCutScene; }

	void RequestChangeMap( int nMapIndex, char cGateNo );
	//#52874 몬스터 소환을 발사체로 할 경우 STE_SummonMonster 시그널을 처리 하는 시점이 스킬 사용 시점과 다른 경우가 있다.
	//발사체에서 이 함수을 호출 할 경우 발사체 스킬ID를 입력 받아서 hOwner의 현시점에서 사용되는 스킬과 비교함.
	void RequestSummonMonster( DnActorHandle hOwner, SummonMonsterStruct* pStruct, bool bReCreateFollowStageMonster = false, int nSkillID = -1 );

	//칼리의 Puppet상태효과에서 소환되는 몬스터인 경우 hOwner의 위치가 아니라 스킬user의 위치를 기준으로 몬스터 소환이 이루어저야 한다.
	//그래서 부득이 하게 함수를 추가한다.
	DnActorHandle RequestSummonMonsterBySkill( DnActorHandle hOwner, DnActorHandle hSkillUser, SummonMonsterStruct* pStruct, bool bReCreateFollowStageMonster = false, int nSkillID = -1 );

	void RequestNestDungeonClear( DnActorHandle hActor );

#ifdef PRE_ADD_STAGE_DAMAGE_LIMIT
	void CalcStageDamageLimit(  int nMapIndex  );
	StageDamageLimitStruct *GetStageDamageLimit(){ return &m_sStageDamageLimitStruct; }
#endif

#if defined( PRE_ADD_HEAL_TABLE_REGULATION )
	void CalcStageHealLimit( int nMapIndex );
	StageHealLimitStruct & GetStageHealLimit() { return m_sStageHealLimitStruct; }
#endif

	void RequestNestDungeonClear( int iForceMapIndex=0 );
	void RequestEnableDungeonClearWarpAlarm( bool bEnable );

	bool IsEnteredDungeon() { return m_bEnteredDungeon; }
	int GetDungeonEnterTableID() { return m_nEnterDungeonTableID; }
	int GetEnterMapTableID() { return m_nEnterMapTableID; }
	bool IsDungeonFailed() { return m_bDungeonFailed; }
	void RefreshDungeonClearState();
#if defined(PRE_SHARE_MAP_CLEARCOUNT)
	int GetShareCountMapIndex() { return m_nShareCountMapIndex; }
#endif

	DungeonClearStateEnum	GetDungeonClearState() const { return m_DungeonClearState; }
	CDNUserSession*			GetPartyLeaderSession();
	void					EnableDungeonClearWarpQuestFlag();

	virtual void OnInitializeStartPosition( CDNUserSession* pSession, const int iPartyIndex );
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter ) {}
	virtual void OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage ) {}
	virtual void OnGhost( DnActorHandle hActor ) {}
	virtual void OnLeaveUser( UINT uiSessionID ){}

	void SyncMonster( CDNUserSession* pBreakIntoGameSession );
	void SyncNpc( CDNUserSession* pBreakIntoGameSession );

	bool	IsTimeAttack() const { return m_bIsTimeAttack; }
	void	ProcessTimeAttack( float fDelta );
	void	StartTimeAttack( int iMin, int iSec );
	void	StopTimeAttack();
	int		GetRemainTimeAttackSec() const;
	int		GetOriginTimeAttackSec() const;
	void	SyncTimeAttack( CDNUserSession* pSession=NULL );

	void EnableGameWarpDungeonClearToLeader();
	void SendGameWarpDungeonClearToLeader();

#if defined(PRE_ADD_STAGECLEAR_TIMECHECK)
	bool CheckDungeonClearAbuseTime(DWORD dwClearTime, int nMapID);
#endif	// #if defined(PRE_ADD_STAGECLEAR_TIMECHECK)

#ifdef PRE_FIX_PARTY_STAGECLEAR_CHANGEMASTER
	void SetWarpDungeonClearToLeaderCheck(bool bSet);
#endif
	void	SendActorShow( CDNUserSession* pBreakIntoGameSession, DnActorHandle hActor );
	void	SendActorHide( CDNUserSession* pBreakIntoGameSession, DnActorHandle hActor );

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	virtual void CheckSelectNamedItem( CDNUserSession * pSession, TItem& RewardItem, BYTE cIndex );
	virtual void SwapNamedItemToNormalItem( CDNUserSession * pSession, TItem& RewardItem );
	void	CheckSelectNamedItemResult( CDNUserSession * pSession, TACheckNamedItemCount* pA );
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	void ResetAlteiaWorldmap();
#endif

	DWORD GetStageCreateTime() { return m_dwStageCreateTime; }

#if defined( PRE_ADD_STAGE_WEIGHT )
	const TStageWeightData * GetStageWeightData();
	void SetStageWeightData( int nStageWeightID );
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )

private:

	bool    _CheckRequestNestDungeonClear( int iMapIndex );
	void	_SendActorShow( CDNUserSession* pBreakIntoGameSession, DnActorHandle hActor );
	void	_SendActorHide( CDNUserSession* pBreakIntoGameSession, DnActorHandle hActor );
	void	_SendActorTeam( CDNUserSession* pBreakIntoGameSession, DnActorHandle hActor );
//////////////////////////////////////////////////////////////////////////
// 몬스터 생성 위치(Random/Center)를 위한 함수 추가
protected:
	bool GenerationMonster(DNVector(MonsterSetStruct) &VecSetMonsterList, int nInitalizeType, int nTeamSetting, DNVector(DnActorHandle) * pVecResult);
public:
	int GetMonsterSpawnType(int nAreaUniqueID);
//////////////////////////////////////////////////////////////////////////

#if defined(PRE_ADD_65808)
	void ApplySummonMonsterGlyph(DnActorHandle hOwner, CDnMonsterActor* pMonsterActor, int nGlyphID);
#endif // PRE_ADD_65808
};

class IBoostPoolDnGameTask : public CDnGameTask, public TBoostMemoryPool< IBoostPoolDnGameTask >
{
public:
	IBoostPoolDnGameTask( CMultiRoom *pRoom ):CDnGameTask(pRoom){}
	virtual ~IBoostPoolDnGameTask(){}
};
