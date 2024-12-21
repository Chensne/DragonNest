#pragma once

#include "DnActor.h"
#include "DnPlayerState.h"

// include Method
#include "MAPartsBody.h"
#include "MAWalkMovement.h"
#include "MACP.h"
#include "MACP_Renewal.h"
#include "DNGameRoom.h"
#include "DnSkillTreeSystem.h"

#if defined(_WORK) || defined(_KRAZ)
#include "DnPlayerTestFlag.h"
#endif
#include "DnDropItem.h"
#include "DnActionSpecificInfo.h"
#include "MAPlateUser.h"
#if defined(PRE_ADD_TALISMAN_SYSTEM)
#include "MATalismanUser.h"
#endif

#include "DnVehicleActor.h"
#include "DnActorClassDefine.h"

#include "DnObservable.h"

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
#include "FrameSync.h"
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE
#ifdef PRE_FIX_MEMOPT_SIGNALH
#include "DnCommonUtil.h"
#endif


namespace BubbleSystem
{
class CDnBubbleSystem;
}

// 현재 무기 갯수는 주무기/보조무기 2개임.
const int NUM_WEAPON = 2;


class CDNUserSession;
class IDnPlayerChecker;
class CDnVehicleActor;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
class CDnTotalLevelSkillSystem;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

class CDnPlayerActor : public CDnActor, 
#if defined( PRE_ADD_CP_RENEWAL )
					   public MACP_Renewal,
#else	// #if defined( PRE_ADD_CP_RENEWAL )
					   public MACP,
#endif	// #if defined( PRE_ADD_CP_RENEWAL )
					   public CDnPlayerState,
#if defined(_WORK) || defined(_KRAZ)
					   public CDnPlayerTestFlag,
#endif
					   virtual public MAPartsBody, 
					   public CDnObservable,
					   virtual public MAPlateUser 
#if defined(PRE_ADD_TALISMAN_SYSTEM) 
					   , virtual public MATalismanUser
#endif
{
public:

	CDnPlayerActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnPlayerActor();

	virtual MAMovementBase* CreateMovement();

private:

	DWORD m_dwSyncDatumTick;
	DWORD m_dwSyncDatumSendTick;
	UINT m_nVoiceChannelID;

	IDnPlayerChecker* m_pPlayerSpeedHackChecker;
	IDnPlayerChecker* m_pPlayerDoNotEnterChecker;
	IDnPlayerChecker* m_pPlayerPickupChecker;
	IDnPlayerChecker* m_pPlayerSkillChecker;
	IDnPlayerChecker* m_pPlayerActionChecker;


	//rlkt_mastery
	int m_nLastUsedSkill;


#ifdef _USE_VOICECHAT
	int m_nVoiceRotate;
#endif
	bool _bIsMasterSystemDurabilityReward();

protected:
	std::string m_szMaintenanceBoneName;
	std::string m_szActionBoneName;
	char m_cMovePushKeyFlag;
	bool m_bBattleMode;
	int m_nWorldLevel;

	int m_nComboDelay;
	int m_nComboCount;
	int m_nTotalComboCount;

	float m_fRecoverySPDelta;
	static float s_fRecoverySPTime;

	LOCAL_TIME m_nFatigueTick;

	CDNUserSession * m_pSession;
	CDNGameRoom::PartyStruct *m_pPartyData;

	bool m_bCompleteCutScene;
	bool m_bCheckCompleteCutScene;
	bool m_bSkipCutScene;
	bool m_bGhost;
	UINT m_uiStateBlowProcessAfterBit;

	DnWeaponHandle m_hCashWeapon[2];
	bool m_bCashSelfDeleteWeapon[2];
	bool m_bWeaponViewOrder[2];

	// 무기 스킬 총 갯수는 파츠 타입 갯수+무기 갯수임.
	// 캐쉬템은 서버 DB 에 저장공간이 더 필요하므로 추후에 필요하게 되면 요청할 예정.
	// 현재는 문장만 장비 스킬 사용함.
	float			m_afLastEquipItemSkillDelayTime;
	float			m_afLastEquipItemSkillRemainTime;
	DnSkillHandle	m_ahEquipSkill;

	// 버블쪽에서 강화용으로 추가하는데 사용할 스킬들은 플레이어가 직접 생성해서 넣어주어야 한다.
	map<int, DnSkillHandle> m_mapEnchantSkillFromBubble;

	// 스킬 트리 관련. 실제로 레벨 1이 되어 획득한 스킬만 MASkillUser 쪽으로 넣어준다.
	// (클라이언트에서는 SkillTask 에서 개인의 스킬 트리/목록 관련 처리를 전부 합니다.)
	vector<int> m_vlUnlockZeroLevelSkills;		// 언락만 된 레벨 0상태인 스킬.
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> m_vlPossessedSkill;

	// OnChangeAction() 에서 핵체크용 발사체/액션 정보를 업데이트 해주지만 패킷이 밀리거나 서버프레임이 밀려서
	// CS_CMDACTION 패킷과 발사체 패킷이 동시에 처리되어 OnChangeAction() 이 호출되어 정보가 업데이트 되기 전에
	// 발사체를 쏘게 될 경우 서버에서 핵 처리 되므로 우선 CS_CMDACTION 패킷이 오자마자 업데이트 해주고 이 플래그를 
	// 켜줍니다. OnChangeAction() 쪽에서는 이 플래그가 꺼져있을 경우에만 업데이트를 수행합니다. 
	// 같은 액션에서 한번만 업데이트 되는 것이 원칙입니다. (#21773)
	bool m_bUpdatedProjectileInfoFromCmdAction;

	int m_iNowMaxProjectileCount;				// 현재 액션에서 최대한 발사할 수 있는 발사체 갯수. 액션이 바뀌면 해당 액션의 값으로 리셋된다.
	int m_iReservedProjectileCount;				// 액션 변경과 관계 없이 확보된 발사체 갯수. (발사체에서 발사체 쏘는 경우)
	multiset<int> m_setWeaponIDUsingProjectileSignal;	// 발사체 시그널에서 사용하는 무기 인덱스들의 모음. 무기에서 발사되는 발사체까지 포함.
	deque<int> m_dqProjectileSignalOffset;		// 현재 플레이어 액터가 취하고 있는 액션에서 발사체 시그널이 박혀있는 프레임을 받아둠.
	DWORD m_dwLastBasicShootActionTime;			// 마지막으로 일반 공격 슛 액션을 한 시간.
	DWORD m_dwLastBasicShootCoolTime;			// 마지막으로 업데이트된 일반 공격 슛 액션의 쿨타임.
	float m_fFrameSpeed;						// 현재 애니메이션 프레임 재생 속도. 1.0이 디폴트 값. 액션 쿨타임을 감안할 때 사용한다.

	// 발사체 발사 텀을 체크한다. 일반적인 경우에선 true 이지만 플레이거가 피격시 슈퍼아머 때문에 프레임이 순간적으로 몇 번 정지하면
	// 서버와 프레임 동기가 일정 값 이상 벗어나는 경우가 있어 Term 체크에서 걸리므로 이런 경우엔 체크를 하지 않도록 한다. (#21133)
	bool m_bCheckProjectileSignalTerm;

	// Tumble_ 이 붙는 패시브 스킬 발동을 위해서는 캐릭터가 Move_ 류 액션을 진행중이어야 하는데
	// 겜서버에서는 CmdStop() 처리되고 Stand 가 된 난 후가 되어버리므로 안된다. 따라서 여기서 직접 액션 인덱스를 이어주도록 한다.
	// Tumble_Front 가 왔으면 Move_Front 액션을 하고 있어야 하므로 액션 인덱스를 모아둠.
	map<int, int> m_mapTumbleHelper;
	DWORD m_dwLastChargeShootTime;				// 차지샷은 기존 액션 시스템과 다르게 돌아간다. 차지샷의 액션 쿨타임을 따로 측정한다.



	bool m_bPlayerCannonMode;
	DnActorHandle m_hCannonMonsterActor;

	int m_nSwapSingleSkinActorID;
	int m_nMonsterMutationTableID;
	EtAniObjectHandle m_hSwapOriginalHandle;
	CDnActionBase *m_pSwapOriginalAction;

	LOCAL_TIME m_LastEscapeTime;
	int m_nDeathCount;		// 각 스테이지별 죽은횟수..이동시 초기화

	BubbleSystem::CDnBubbleSystem* m_pBubbleSystem;

	map<DWORD, bool> m_mapIcyFractionHitted;

	// #26902 임시로 스킬 추가된 상태 및 임시로 추가된 직업 ID.
	bool m_bTempSkillAdded;
	int m_iTempChangedJob;	
	//////////////////////////////////////////////////////////////////////////

	bool m_bForceEnableRideByTrigger;
	UINT m_nVehicleEffectIndex;
	bool m_bVehicleMode;
	DnActorHandle m_hVehicleActor;

	std::vector<int> m_vecEventEffectList;
	bool m_bDeletedPetSkill[Pet::Skill::Max];

	int m_nAllowedSkill;    
	bool m_bRefreshTransformMode; 
	bool m_bTransformMode;
	bool m_bShootMode; 
	std::vector<int> m_vecTransformSkillList; // 추가된 몬스터변신용 스킬리스트
	std::string m_strTransformEndAction;

#ifdef PRE_ADD_MONSTER_CATCH
	DnActorHandle m_hCatcherMonster;		// 날 잡고 있는 몬스터.
#endif // #ifdef PRE_ADD_MONSTER_CATCH
	int m_nInvalidPlayerCheckCounter;

	// #32426 소환체 컨트롤 기능 - 스테이지 이동 혹은 존 이동(CmdWarp)를 할 때 따라가야 되는
	// 이 플레이어가 소환한 몬스터 객체들.
	struct S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO
	{
		// 스테이지 이동을 하면 몬스터 객체가 삭제되므로 따로 데이터를 복사해서 갖고 있다가 
		// 새로운 스테이지로 이동하면 다시 객체를 생성하고 State 를 복사해준 다음에 객체 핸들을
		// 새로 생성된 몬스터 객체로 물려줘서 데이터의 유효성을 유지시킨다.
		DnMonsterActorHandle hMonster;
		DWORD dwMonsterClassID;
		SummonMonsterStruct SummonMonsterSignalData;
		LOCAL_TIME iRemainDestroyTime;

		// 스테이지 이동으로 재 생성 처리가 끝난 몬스터는 플래그 박아둬서 같은 MonsterClassID 가 재 생성 요청이 들어와도
		// 이 플래그와 같이 비교해서 처리가 끝난 몬스터 객체를 구분한다.
		bool bReCreatedFollowStageMonster;

		S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO( void ) : iRemainDestroyTime( 0 ), dwMonsterClassID( 0 ), bReCreatedFollowStageMonster( false ) {};
#ifdef PRE_FIX_MEMOPT_SIGNALH
		S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO( const S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO& rhs )
		{
			hMonster = rhs.hMonster;
			dwMonsterClassID = rhs.dwMonsterClassID;
			CopyShallow_SummonMonsterStruct(SummonMonsterSignalData, &rhs.SummonMonsterSignalData);
			iRemainDestroyTime = rhs.iRemainDestroyTime;
		}
#endif
	};
	std::list<S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO> m_listSummonedMonstersFollowStageInfos;

	LOCAL_TIME m_MixedActionTimeStamp;

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	// 게임서버 퍼포먼스 때문에 플레이어의 스킬 리스트별로 CDnSkill::Process() 호출하는 것을 20프레임 매번 호출하지 않도록 제한.
	CFrameSkip	m_FrameSkipCallSkillProcess;
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

#ifdef PRE_ADD_COSTUME_SKILL
	int m_nCostumeSkillID;
#endif

protected:
	// Movement Message
	virtual void OnDrop( float fCurVelocity );
	virtual void OnStop( EtVector3 &vPosition );
	virtual void OnFall( float fCurVelocity );

	// Actor Message
	virtual void OnStateBlowProcessAfter();
	virtual void OnAddStateBlowProcessAfterType( eStateBlowAfterProcessType Type );
	virtual void OnDie( DnActorHandle hHitter );
	virtual void ProcessDie( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor, HitStruct *pStruct );
	virtual void OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct );
	virtual void OnComboFinish( int nCombo );
	virtual void OnHitProjectile( LOCAL_TIME LocalTime, DnActorHandle hHittedTarget, const CDnDamageBase::SHitParam& HitParam );

	virtual void	OnBreakSkillSuperAmmor( int nIndex, int nOriginalSupperAmmor, int nDescreaseSupperAmmor );
	virtual void	OnAirCombo( int nComboCount );
	virtual int		OnGetJobClassID(){ return CDnPlayerState::GetJobClassID(); }

	virtual void PushSummonMonster( DnMonsterActorHandle hMonster, const SummonMonsterStruct* pSummonMonsterStruct, bool bReCreateFollowStageMonster = false );

	// CDnActorState
	virtual void OnLevelUp( int nLevel, int nLevelUpAmount );
	virtual void OnAddExperience( int nAddExperience, int nLogCode, INT64 biFKey );	// -> nLogCode에 따라 biFKey값도 넣어준다 (eCharacterExpChangeCode참조)

	// MASkillUser
	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta );

	void ProcessCombo( LOCAL_TIME LocalTime, float fDelta );
	void ProcessRecoverySP( LOCAL_TIME LocalTime, float fDelta );

	void OnFinishProcessDie();

	void OnChangeAction( const char* szPrevAction );

	void RefreshWeaponViewOrder( int nEquipIndex );

	void LinkCashWeapon( int nEquipIndex );

	virtual float PreCalcDamage( CDnDamageBase *pHitter, SHitParam &HitParam, const float fDefenseRate=1.f, float fStateEffectAttackM = -1.0f );

	// CP
	virtual void OnEventCP( CPTypeEnum Type, int nResult );
	void UpdateAttackedCPPoint( CDnDamageBase *pHitter , CDnWeapon::HitTypeEnum eHitType );

	void _UpdateMaxProjectileCount( int nActionIndex, bool bUpdateReservedCount = false );
	void _CheckActionWithProcessPassiveActionSkill( const char* szPrevAction );
	void _CheckProcessSkillActioncChange( const char* pAction );

public:
	virtual bool Initialize();

	// Actor Message
	virtual bool SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bCheck = true, bool bCheckStateEffect = true );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	void OrderUseSkillToMySummonedMonster( OrderMySummonedMonsterStruct* pStruct );

	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );
	virtual void SyncClassTime( LOCAL_TIME LocalTime );
	virtual void ResetActor();
	virtual void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false );
	virtual void OnBeginStateBlow( DnBlowHandle hBlow );

	// Quest Callback Message
	virtual void OnKillMonster(DnActorHandle hMonster);

	//MAPlateUser
	virtual bool AttachGlyph( DnGlyphHandle hGlyph, CDnGlyph::GlyphSlotEnum Index, bool bDelete = false );
	virtual bool DetachGlyph( CDnGlyph::GlyphSlotEnum Index );

	// Actor Command
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f );
	virtual void CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.0f, bool bChargeKey = false, bool bCheckOverlapAction = true, bool bOnlyCheck = false );
	virtual int CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit = false, bool bCheckCanBegin = true, bool bEternity = false );
	virtual void CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bRemoveFromServerToo = true );
	virtual void SendRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	virtual void SendRemoveStateEffectGraphic( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	virtual void CmdToggleBattle( bool bBattleMode );

	bool CheckSkillAction( const char *szActionName );

	virtual void CmdAddExperience( TExpData &ExpData, int nLogCode, INT64 biFKey );	// -> nLogCode에 따라 biFKey값도 넣어준다 (eCharacterExpChangeCode참조)
	virtual void CmdAddCoin( INT64 nCoin, int nLogCode, int nFKey, bool bSync = true );

	virtual void CmdToggleWeaponViewOrder( int nEquipIndex, bool bShowCash );
	virtual void CmdTogglePartsViewOrder( int nEquipIndex, bool bShowCash );
	virtual void CmdToggleHideHelmet( bool bHideHelmet );
	virtual void CmdChangeJob( int nJobID );
	virtual void CmdPickupItem( PickupItemStruct* pStruct, DnDropItemHandle hDropItem=CDnDropItem::Identity() );
	virtual void CmdEscape( EtVector3 &vPos );
	virtual void CmdWarp( EtVector3 &vPos, EtVector2 &vLook, CDNUserSession* pGameSession=NULL, bool bCheckPlayerFollowSummonedMonster=false );

	int GetLevelUpSkillPoint( int nPrevLevel, int nCurLevel );

	// TransAction Message
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwActorProtocol, BYTE *pPacket );

	// MASkillUser Message
	// 획득하지 않은.. 언락만 된 스킬은 실제로 MASkillUser 에게 넣어주지 않고 중간에서 쥐고 있습니다.
	// MASkillUser::AddSkill 의 가상함수 상속을 유지하기 위해 iSkillLevelApplyType 까지 선언해놓았지만 실제 값이 정해지는 것은
	// 이 클래스에서 오버라이드한 AddSkill 에서 pvp/pve 를 정해서 MASkillUser::AddSkill() 을 호출해줍니다.
	virtual bool CanAddSkill( int nSkillTableID, int nLevel = 1 );
	virtual bool AddSkill( int nSkillTableID, int nLevel = 1, int iSkillLevelApplyType = CDnSkill::PVE );
	virtual void OnAddSkill( DnSkillHandle hSkill, bool isInitialize = false );
	virtual void OnRemoveSkill( DnSkillHandle hSkill );

	virtual void OnApplyPassiveSkillBlow( int iBlowID );
	void CheckAndRegisterObserverStateBlow( DnBlowHandle hBlow );

	void SaveUserData(TUserData &UserData);

	void SetUserSession(CDNUserSession * pSession);
	CDNUserSession * GetUserSession() { return m_pSession; }
	CDNGameRoom::PartyStruct *GetPartyData() { return m_pPartyData; }

	bool IsLocalActor() { return false; }
	virtual bool IsDie();

	void SetBattleMode( bool bEnable );
	bool IsBattleMode() { return m_bBattleMode; }
	bool IsCanBattleMode();
	void OnBattleToggle( bool bBattle );

	virtual bool IsPlayerActor() { return true; }
	virtual bool IsGMTrace() const;
	virtual UINT GetSessionID();
	virtual int GetMoveSpeed();

	INT64	GetCharacterDBID();
	int GetComboCount() { return m_nComboCount; }
	int GetComboDelay() { return m_nComboDelay; }
	int GetTotalComboCount() { return m_nTotalComboCount; }
	void ResetTotalComboCount() { m_nTotalComboCount = 0; }

	void OnRepairEquipDurability( bool bDBSave, INT64 nPriceCoin );
	void OnDecreaseEquipDurability( int nValue, bool bDBSave );
	void OnDecreaseEquipDurability( float fValue, bool bDBSave );
	void OnDecreaseInvenDurability( int nValue, bool bDBSave );
	void OnDecreaseInvenDurability( float fValue, bool bDBSave );
	bool OnStageGiveUp();
	bool IsPenaltyStageGiveUp();

	bool IsCompleteCutScene() { return m_bCompleteCutScene; }
	void SetCompleteCutScene( bool bValue ) { m_bCompleteCutScene = bValue; }
	void SetCheckCompleteCutScene( bool bValue ){ m_bCheckCompleteCutScene = bValue; }
	bool IsCheckCompleteCutScene(){ return m_bCheckCompleteCutScene; }

	bool IsSkipCutScene() { return m_bSkipCutScene; }
	void SetSkipCutScene( bool bValue ) { m_bSkipCutScene = bValue; }

	void ToggleGhostMode( bool bGhost );
	bool IsGhost() { return m_bGhost; }
	void SwapSingleSkin( int nChangeActorTableID);
	bool IsSwapSingleSkin(){ return ( m_nSwapSingleSkinActorID == -1 ) ? false : true; }
	int GetSwapSingleSkinActorID() { return m_nSwapSingleSkinActorID; }
	int GetMonsterMutationTableID() { return m_nMonsterMutationTableID; }

	void RecvPartyRefreshGateInfo( const EtVector3& Pos );

	//rlkt_mastery
	int GetLastUsedSkill(void) { return m_nLastUsedSkill; }

	virtual DnWeaponHandle GetActiveWeapon( int nEquipIndex );
	// 무기/악세사리에 스킬이 달려 있는 경우 스킬 추가. 빠지는 경우엔 스킬 제거.
	virtual bool AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false );
	virtual bool DetachParts( CDnParts::PartsTypeEnum Index );
	virtual bool AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false );
	virtual bool DetachCashParts( CDnParts::PartsTypeEnum Index );

	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachWeapon( int nEquipIndex = 0 );

	// 요건 로컬꺼.
	virtual void AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachCashWeapon( int nEquipIndex = 0 );
	virtual DnWeaponHandle GetCashWeapon( int nEquipIndex = 0 ) { return m_hCashWeapon[nEquipIndex]; }
	void ShowCashWeapon( int nEquipIndex, bool bShow );
	void SetWeaponViewOrder( int nEquipIndex, bool bShowCash );

	// 스핵관련
	void	SetSyncDatumTick(){ m_dwSyncDatumTick = GetTickCount(); }
	void	SetSyncDatumSendTick(){ m_dwSyncDatumSendTick = GetTickCount(); }
	DWORD	GetSyncDatumGap() const { return GetTickCount()-m_dwSyncDatumTick; }
	DWORD	GetSyncDatumSendTick() const { return m_dwSyncDatumSendTick; }

	// 스킬 트리 관련
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>& GetPossessedSkillInfo( void ) { return m_vlPossessedSkill; };
	void UnLockSkill( int nSkillID, INT64 nUnlockPrice=0 );
	// 스킬북으로 언락 및 획득까지 일괄로 처리되도록 변경되었기 때문에 언락된다면
	// 스킬을 획득할 수 있는 조건이 되는지 체크. 실제로 언락되기 전에 호출되어 언락이 된다면 획득까지 가능한지 확인한다.
	int CanAcquireSkillIfUnlock( int nSkillID );
	int AcquireSkill( int nSkillID );

	void AddNowMaxProjectileCount( int nAddCount ) { m_iNowMaxProjectileCount += nAddCount; };

	IDnPlayerChecker* GetPlayerSpeedHackChecker( void ) { return m_pPlayerSpeedHackChecker; };
	bool IsInvalidPlayerChecker();
	virtual void OnInvalidPlayerChecker( int nValue );

	// 발사체 핵관련 갯수 체크 시리즈.
	void AddReservedProjectileCount( void ) { ++m_iReservedProjectileCount; };
	bool UseAndCheckAvailProjectileCount( void );
	//////////////////////////////////////////////////////////////////////////

	// 발사체 시그널에서 사용하는 무기 테이블 ID 체크.
	bool CheckAndEraseWeaponIDUsingProjectileSignal( int iWeaponTableID )
	{
		bool bResult = false;
		if( false == m_setWeaponIDUsingProjectileSignal.empty() )
		{
			multiset<int>::iterator iter = m_setWeaponIDUsingProjectileSignal.find( iWeaponTableID );
			if( m_setWeaponIDUsingProjectileSignal.end() != iter )
			{
				// 값으로 erase 를 호출하면 같은 값 다 지워지므로 하나만 지우기.
				m_setWeaponIDUsingProjectileSignal.erase( iter );
				bResult = true;
			}
		}

		return bResult;
	}

	void OnChangePlaySpeed( DWORD dwFrame, float fSpeed );

	bool CheckProjectileSignalFrameTerm( void )
	{
		// 현재 발사체 패킷이 들어온 시점에 캐릭터의 발사 액션에서 쏴지는 발사체의 프레임과
		// 현재 게임서버에서 해당 캐릭터가 취하고 있는 액션의 프레임을 비교합니다.
		// 캐릭터의 프레임보다 작은 발사체 시그널 프레임이 하나도 없다면 핵으로 간주합니다.
		// 스킬액션을 제외한 일반 공격 액션만 체크해도 됩니다..
		if( m_dqProjectileSignalOffset.empty() )
			return false;

		// 상하체 분리액션 중이라면
		float fCurrentFrame = 0.0f;
		if( IsCustomAction() )
		{
			fCurrentFrame = GetCustomActionCurrentFrame();
		}
		else
		{
			fCurrentFrame = CDnActionBase::m_fFrame;
		}

		// 같거나 작은 프레임이 하나도 없으면 현재 프레임에 올 수 없는 발사체 패킷임.
		// 정렬되어있으므로 맨 앞의 것만 비교하면 된다.
		// 서버는 20프레임으로 도므로 +6 프레임의 관용도 값을 허용한다.
		int iSignalOffset = int((float)m_dqProjectileSignalOffset.front()/* / m_fFrameSpeed*/);		// 프레임 기준이므로 느려지면 느려지는대로 프레임도 늦게 늘어나므로 그냥 비교한다.

		if( m_bCheckProjectileSignalTerm )
		{
			// 프레임 스피드가 빨라지면 오차가 더 커질 수 있으므로 감안.
			if( fCurrentFrame+(8.0f*m_fFrameSpeed) < iSignalOffset )
			{
				// TODO: 핵이다! 접속 끊어버렸으면 좋겠는데..
				return false;
			}
		}
		
		// 발사 처리 되었으므로 한발 빼주고.
		m_dqProjectileSignalOffset.pop_front();

		return true;
	}

	bool CheckSkipAirCondition(int iSkill); // 현재가 Air인경우 서버와 클라간 인식차이의 갭으로 GroundMovable 체크에서 실패하는 경우가있다.
	void ReplacementGlyph( DnSkillHandle hNewSkill );
	bool IsExclusiveSkill( int iSkillID, int iExclusiveID );
	int GetAvailSkillPointByJob( int iSkillID );
	int GetUsedSkillPointInThisJob( const int nJobID );

	// 클라이언트에게 쿨타임 패링이 성공했음을 알려줌.
	void RequestCooltimeParrySuccess( int iSkillID );

	// 평타 맞았을 때 실행되어야 할 아이템 접두어 스킬 관련 처리할 것이 있으면 처리.
	void CheckNormalHitSEProcessor( DnActorHandle hHittedTarget, const CDnDamageBase::SHitParam& HitParam );

	void AddStageDeathCount() { ++m_nDeathCount; };
	void ClearStageDeathCount() { m_nDeathCount = 0;};
	int GetStageDeathCount() { return m_nDeathCount;};

	void InitializeEnchantPassiveSkills( void );
	// from MASkillUser
	virtual void OnReplacementSkill( DnSkillHandle hLegacySkill, DnSkillHandle hNewSkill );
	// 해당 스킬 ID 에 맞는 강화 스킬을 갖고 있는지 체크해서 갖고 있다면 적용시켜 준다.
	void CheckAndApplyEnchantPassiveSkill( DnSkillHandle hBaseSkill );

	// 현재 엘리멘탈로드의 아이시 프랙션에서만 예외적으로 2가지의 상태효과 필터링을 걸고 있는데
	// 둘 중에 하나만 hit 되길 원하기 때문에 먼저 체크된 것은 다음 필터링에 걸리지 않도록 한다. #28747
	// CDnPlayerActor 에서 상속 받음.
	virtual void OnHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex );
	virtual bool CheckHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex );

	virtual bool IsMyRelicMonster( DnActorHandle hActor );

	BubbleSystem::CDnBubbleSystem* GetBubbleSystem( void ) { return m_pBubbleSystem; };
	void RemoveAllBubbles( bool bRemoveEvent );

	// #26902
	// 2차 전직 스킬을 임시로 사용해볼 수 있는 상태로 만들어주기 위해 퀘스트에서 사용되는 api 에서 호출되는 함수.
	// 사용 가능 직업 및 사용 가능 레벨을 현재 캐릭터의 레벨로 맞춰준다.
	// 클라이언트 쪽에서만 스킬 트리를 보여주기 위해 임시로 전직을 시켜준다.
	bool IsTempSkillAdded( void ) { return m_bTempSkillAdded; };
	bool CanChangeJob( int iJobID );
	void SendTempJobChange( int iJobID );
	void EndAddTempSkillAndSendRestoreTempJobChange( void );
	void AddTempSkill( int iSkillID );
	void RemoveAllTempSkill( void );

	bool IsCannonMode( void ) { return (m_hCannonMonsterActor && m_bPlayerCannonMode); };
	void OnCannonMonsterDie(void );
	void EndCannonMode();

	// 탈것 이펙트 자체가 스텟을 가지거나 게임에 영향을 끼치는경우에는 추가를 해줘야 하겟지만 일단 그럴경우가 현재는 없기때문에 일단 아이템 인덱스만 가지고 있도록 하겠습니다. 
	void SetVehicleEffectIndex(UINT nIndex){m_nVehicleEffectIndex = nIndex;}
	bool IsCanVehicleMode();
	bool IsCanPetMode();
	void RideVehicle(TVehicle *pInfo);
	void UnRideVehicle(bool bForce=true);
	void ForceUnRideVehicle();

	bool IsVehicleMode(){return  m_bVehicleMode;}
	void SetVehicleMode(bool bVehicle){m_bVehicleMode = bVehicle;}
	CDnVehicleActor *GetMyVehicleActor();
	void SetMyVehicleActor(DnActorHandle hVehicleActor){m_hVehicleActor = hVehicleActor;}
	void RemoveVehicleStateEffectImmediately(int nBlowIndex );

	void SetForceEnableRide( const bool bForceEnableRide );

	// Pet / Vehicle을 위해서 서버에서 Process돌려야 하는 경우는 이 함수 내부에 추가해줍시다.
	// 클라이언트는 Task로 관리됨.

	void ProcessCompanion( LOCAL_TIME LocalTime, float fDelta ); 

	void ApplyEventStateBlow();
	void RemoveEventStateBlow();

	void CheckPetSatietyPercent();
	bool IsTransformMode(){return m_bTransformMode;}
	bool IsTransformSkill( int nSkillID );

	virtual void RefreshTransformMode();
	virtual void ToggleTransformMode(bool bTrue, int nMonsterMutationTableID=-1, bool bForce = false, const char* strEndAction = "Stand" );
	virtual void CmdShootMode(bool bTrue);
	virtual const char *GetChangeShootActionName(const char *szActionName);
	virtual void ProcessNonLocalShootModeAction();

	bool IsSpectatorMode();
	virtual bool OnApplySpectator(bool bEnable);

#ifdef PRE_ADD_MONSTER_CATCH
	void SetCatcherMonsterActor( DnActorHandle hActor ) { m_hCatcherMonster = hActor; };
	void ReleaseCatcherMonsterActor( void ) { m_hCatcherMonster.Identity(); };
	bool IsCatchedByMonster( void ) { return m_hCatcherMonster ? true : false; };
#endif // #ifdef PRE_ADD_MONSTER_CATCH

	void OnInitializeNextStageFinished( void );
	void OnDieSummonedMonster( DnMonsterActorHandle hSummonedMonster );
	void OnBeforeDestroyStageMonsters( void );

	bool HasSameGlobalIDSkill( int iSkillID );

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	virtual bool IsAllowCallSkillProcess( float fDelta );
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

	void UpdateMissionByMonsterKillCount(int nCount);

	void ApplyEnchantSkillOnceFromBubble( int iTargetSkillID, int iEnchantSkillID );

	//#45680
	//스킬트리창에서의 레벨업은 DnSkillTask에서 동작 되고 있으나
	//서버쪽에서 스킬 레벨업 아이템 장/탈착시 문장 효과 적용 부분 누락되어 있음.
	virtual void ChangeSkillLevelUp(int nSkillID, int nOrigLevel);

	void MakeEquipAndPassiveState( CDnState &State );

#ifdef PRE_ADD_COSTUME_SKILL
	void RefreshCostumeSkill( int nSkillIndex , int nSkilllevel);
#endif
#ifdef PRE_ADD_VEHICLE_SPECIAL_ACTION
	void ReportInvalidAction();
#endif

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
protected:
	CDnTotalLevelSkillSystem* m_pTotalLevelSkillSystem;

public:
	CDnTotalLevelSkillSystem* GetTotalLevelSkillSystem() { return m_pTotalLevelSkillSystem; }

	virtual void UpdateTotalLevel(int nLevel);
	virtual void UpdateTotalLevelByCharLevel();
	virtual void AddTotalLevelSkill(int nSlotIndex, int nSkillID, bool isInitialize = false);
	virtual void RemoveTotalLevelSkill(int nSlotIndex);
	virtual void ActivateTotalLevelSkillSlot(int nSlotIndex, bool bActivate);
	virtual void ActivateTotalLevelSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate);

	virtual void RequestAddTotalLevelSkill(int nSlotIndex, int nSkillID) {};
	virtual void RequestRemoveTotalLevelSkill(int nSlotIndex) {};

	virtual void OnLevelChange();
#endif // PRE_ADD_TOTAL_LEVEL_SKILL


};
