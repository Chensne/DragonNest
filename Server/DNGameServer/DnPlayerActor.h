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

// ���� ���� ������ �ֹ���/�������� 2����.
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

	// ���� ��ų �� ������ ���� Ÿ�� ����+���� ������.
	// ĳ������ ���� DB �� ��������� �� �ʿ��ϹǷ� ���Ŀ� �ʿ��ϰ� �Ǹ� ��û�� ����.
	// ����� ���常 ��� ��ų �����.
	float			m_afLastEquipItemSkillDelayTime;
	float			m_afLastEquipItemSkillRemainTime;
	DnSkillHandle	m_ahEquipSkill;

	// �����ʿ��� ��ȭ������ �߰��ϴµ� ����� ��ų���� �÷��̾ ���� �����ؼ� �־��־�� �Ѵ�.
	map<int, DnSkillHandle> m_mapEnchantSkillFromBubble;

	// ��ų Ʈ�� ����. ������ ���� 1�� �Ǿ� ȹ���� ��ų�� MASkillUser ������ �־��ش�.
	// (Ŭ���̾�Ʈ������ SkillTask ���� ������ ��ų Ʈ��/��� ���� ó���� ���� �մϴ�.)
	vector<int> m_vlUnlockZeroLevelSkills;		// ����� �� ���� 0������ ��ų.
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> m_vlPossessedSkill;

	// OnChangeAction() ���� ��üũ�� �߻�ü/�׼� ������ ������Ʈ �������� ��Ŷ�� �и��ų� ������������ �з���
	// CS_CMDACTION ��Ŷ�� �߻�ü ��Ŷ�� ���ÿ� ó���Ǿ� OnChangeAction() �� ȣ��Ǿ� ������ ������Ʈ �Ǳ� ����
	// �߻�ü�� ��� �� ��� �������� �� ó�� �ǹǷ� �켱 CS_CMDACTION ��Ŷ�� ���ڸ��� ������Ʈ ���ְ� �� �÷��׸� 
	// ���ݴϴ�. OnChangeAction() �ʿ����� �� �÷��װ� �������� ��쿡�� ������Ʈ�� �����մϴ�. 
	// ���� �׼ǿ��� �ѹ��� ������Ʈ �Ǵ� ���� ��Ģ�Դϴ�. (#21773)
	bool m_bUpdatedProjectileInfoFromCmdAction;

	int m_iNowMaxProjectileCount;				// ���� �׼ǿ��� �ִ��� �߻��� �� �ִ� �߻�ü ����. �׼��� �ٲ�� �ش� �׼��� ������ ���µȴ�.
	int m_iReservedProjectileCount;				// �׼� ����� ���� ���� Ȯ���� �߻�ü ����. (�߻�ü���� �߻�ü ��� ���)
	multiset<int> m_setWeaponIDUsingProjectileSignal;	// �߻�ü �ñ׳ο��� ����ϴ� ���� �ε������� ����. ���⿡�� �߻�Ǵ� �߻�ü���� ����.
	deque<int> m_dqProjectileSignalOffset;		// ���� �÷��̾� ���Ͱ� ���ϰ� �ִ� �׼ǿ��� �߻�ü �ñ׳��� �����ִ� �������� �޾Ƶ�.
	DWORD m_dwLastBasicShootActionTime;			// ���������� �Ϲ� ���� �� �׼��� �� �ð�.
	DWORD m_dwLastBasicShootCoolTime;			// ���������� ������Ʈ�� �Ϲ� ���� �� �׼��� ��Ÿ��.
	float m_fFrameSpeed;						// ���� �ִϸ��̼� ������ ��� �ӵ�. 1.0�� ����Ʈ ��. �׼� ��Ÿ���� ������ �� ����Ѵ�.

	// �߻�ü �߻� ���� üũ�Ѵ�. �Ϲ����� ��쿡�� true ������ �÷��̰Ű� �ǰݽ� ���۾Ƹ� ������ �������� ���������� �� �� �����ϸ�
	// ������ ������ ���Ⱑ ���� �� �̻� ����� ��찡 �־� Term üũ���� �ɸ��Ƿ� �̷� ��쿣 üũ�� ���� �ʵ��� �Ѵ�. (#21133)
	bool m_bCheckProjectileSignalTerm;

	// Tumble_ �� �ٴ� �нú� ��ų �ߵ��� ���ؼ��� ĳ���Ͱ� Move_ �� �׼��� �������̾�� �ϴµ�
	// �׼��������� CmdStop() ó���ǰ� Stand �� �� �� �İ� �Ǿ�����Ƿ� �ȵȴ�. ���� ���⼭ ���� �׼� �ε����� �̾��ֵ��� �Ѵ�.
	// Tumble_Front �� ������ Move_Front �׼��� �ϰ� �־�� �ϹǷ� �׼� �ε����� ��Ƶ�.
	map<int, int> m_mapTumbleHelper;
	DWORD m_dwLastChargeShootTime;				// �������� ���� �׼� �ý��۰� �ٸ��� ���ư���. �������� �׼� ��Ÿ���� ���� �����Ѵ�.



	bool m_bPlayerCannonMode;
	DnActorHandle m_hCannonMonsterActor;

	int m_nSwapSingleSkinActorID;
	int m_nMonsterMutationTableID;
	EtAniObjectHandle m_hSwapOriginalHandle;
	CDnActionBase *m_pSwapOriginalAction;

	LOCAL_TIME m_LastEscapeTime;
	int m_nDeathCount;		// �� ���������� ����Ƚ��..�̵��� �ʱ�ȭ

	BubbleSystem::CDnBubbleSystem* m_pBubbleSystem;

	map<DWORD, bool> m_mapIcyFractionHitted;

	// #26902 �ӽ÷� ��ų �߰��� ���� �� �ӽ÷� �߰��� ���� ID.
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
	std::vector<int> m_vecTransformSkillList; // �߰��� ���ͺ��ſ� ��ų����Ʈ
	std::string m_strTransformEndAction;

#ifdef PRE_ADD_MONSTER_CATCH
	DnActorHandle m_hCatcherMonster;		// �� ��� �ִ� ����.
#endif // #ifdef PRE_ADD_MONSTER_CATCH
	int m_nInvalidPlayerCheckCounter;

	// #32426 ��ȯü ��Ʈ�� ��� - �������� �̵� Ȥ�� �� �̵�(CmdWarp)�� �� �� ���󰡾� �Ǵ�
	// �� �÷��̾ ��ȯ�� ���� ��ü��.
	struct S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO
	{
		// �������� �̵��� �ϸ� ���� ��ü�� �����ǹǷ� ���� �����͸� �����ؼ� ���� �ִٰ� 
		// ���ο� ���������� �̵��ϸ� �ٽ� ��ü�� �����ϰ� State �� �������� ������ ��ü �ڵ���
		// ���� ������ ���� ��ü�� �����༭ �������� ��ȿ���� ������Ų��.
		DnMonsterActorHandle hMonster;
		DWORD dwMonsterClassID;
		SummonMonsterStruct SummonMonsterSignalData;
		LOCAL_TIME iRemainDestroyTime;

		// �������� �̵����� �� ���� ó���� ���� ���ʹ� �÷��� �ھƵּ� ���� MonsterClassID �� �� ���� ��û�� ���͵�
		// �� �÷��׿� ���� ���ؼ� ó���� ���� ���� ��ü�� �����Ѵ�.
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
	// ���Ӽ��� �����ս� ������ �÷��̾��� ��ų ����Ʈ���� CDnSkill::Process() ȣ���ϴ� ���� 20������ �Ź� ȣ������ �ʵ��� ����.
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
	virtual void OnAddExperience( int nAddExperience, int nLogCode, INT64 biFKey );	// -> nLogCode�� ���� biFKey���� �־��ش� (eCharacterExpChangeCode����)

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

	virtual void CmdAddExperience( TExpData &ExpData, int nLogCode, INT64 biFKey );	// -> nLogCode�� ���� biFKey���� �־��ش� (eCharacterExpChangeCode����)
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
	// ȹ������ ����.. ����� �� ��ų�� ������ MASkillUser ���� �־����� �ʰ� �߰����� ��� �ֽ��ϴ�.
	// MASkillUser::AddSkill �� �����Լ� ����� �����ϱ� ���� iSkillLevelApplyType ���� �����س������� ���� ���� �������� ����
	// �� Ŭ�������� �������̵��� AddSkill ���� pvp/pve �� ���ؼ� MASkillUser::AddSkill() �� ȣ�����ݴϴ�.
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
	// ����/�Ǽ��縮�� ��ų�� �޷� �ִ� ��� ��ų �߰�. ������ ��쿣 ��ų ����.
	virtual bool AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false );
	virtual bool DetachParts( CDnParts::PartsTypeEnum Index );
	virtual bool AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false );
	virtual bool DetachCashParts( CDnParts::PartsTypeEnum Index );

	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachWeapon( int nEquipIndex = 0 );

	// ��� ���ò�.
	virtual void AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachCashWeapon( int nEquipIndex = 0 );
	virtual DnWeaponHandle GetCashWeapon( int nEquipIndex = 0 ) { return m_hCashWeapon[nEquipIndex]; }
	void ShowCashWeapon( int nEquipIndex, bool bShow );
	void SetWeaponViewOrder( int nEquipIndex, bool bShowCash );

	// ���ٰ���
	void	SetSyncDatumTick(){ m_dwSyncDatumTick = GetTickCount(); }
	void	SetSyncDatumSendTick(){ m_dwSyncDatumSendTick = GetTickCount(); }
	DWORD	GetSyncDatumGap() const { return GetTickCount()-m_dwSyncDatumTick; }
	DWORD	GetSyncDatumSendTick() const { return m_dwSyncDatumSendTick; }

	// ��ų Ʈ�� ����
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>& GetPossessedSkillInfo( void ) { return m_vlPossessedSkill; };
	void UnLockSkill( int nSkillID, INT64 nUnlockPrice=0 );
	// ��ų������ ��� �� ȹ����� �ϰ��� ó���ǵ��� ����Ǿ��� ������ ����ȴٸ�
	// ��ų�� ȹ���� �� �ִ� ������ �Ǵ��� üũ. ������ ����Ǳ� ���� ȣ��Ǿ� ����� �ȴٸ� ȹ����� �������� Ȯ���Ѵ�.
	int CanAcquireSkillIfUnlock( int nSkillID );
	int AcquireSkill( int nSkillID );

	void AddNowMaxProjectileCount( int nAddCount ) { m_iNowMaxProjectileCount += nAddCount; };

	IDnPlayerChecker* GetPlayerSpeedHackChecker( void ) { return m_pPlayerSpeedHackChecker; };
	bool IsInvalidPlayerChecker();
	virtual void OnInvalidPlayerChecker( int nValue );

	// �߻�ü �ٰ��� ���� üũ �ø���.
	void AddReservedProjectileCount( void ) { ++m_iReservedProjectileCount; };
	bool UseAndCheckAvailProjectileCount( void );
	//////////////////////////////////////////////////////////////////////////

	// �߻�ü �ñ׳ο��� ����ϴ� ���� ���̺� ID üũ.
	bool CheckAndEraseWeaponIDUsingProjectileSignal( int iWeaponTableID )
	{
		bool bResult = false;
		if( false == m_setWeaponIDUsingProjectileSignal.empty() )
		{
			multiset<int>::iterator iter = m_setWeaponIDUsingProjectileSignal.find( iWeaponTableID );
			if( m_setWeaponIDUsingProjectileSignal.end() != iter )
			{
				// ������ erase �� ȣ���ϸ� ���� �� �� �������Ƿ� �ϳ��� �����.
				m_setWeaponIDUsingProjectileSignal.erase( iter );
				bResult = true;
			}
		}

		return bResult;
	}

	void OnChangePlaySpeed( DWORD dwFrame, float fSpeed );

	bool CheckProjectileSignalFrameTerm( void )
	{
		// ���� �߻�ü ��Ŷ�� ���� ������ ĳ������ �߻� �׼ǿ��� ������ �߻�ü�� �����Ӱ�
		// ���� ���Ӽ������� �ش� ĳ���Ͱ� ���ϰ� �ִ� �׼��� �������� ���մϴ�.
		// ĳ������ �����Ӻ��� ���� �߻�ü �ñ׳� �������� �ϳ��� ���ٸ� ������ �����մϴ�.
		// ��ų�׼��� ������ �Ϲ� ���� �׼Ǹ� üũ�ص� �˴ϴ�..
		if( m_dqProjectileSignalOffset.empty() )
			return false;

		// ����ü �и��׼� ���̶��
		float fCurrentFrame = 0.0f;
		if( IsCustomAction() )
		{
			fCurrentFrame = GetCustomActionCurrentFrame();
		}
		else
		{
			fCurrentFrame = CDnActionBase::m_fFrame;
		}

		// ���ų� ���� �������� �ϳ��� ������ ���� �����ӿ� �� �� ���� �߻�ü ��Ŷ��.
		// ���ĵǾ������Ƿ� �� ���� �͸� ���ϸ� �ȴ�.
		// ������ 20���������� ���Ƿ� +6 �������� ���뵵 ���� ����Ѵ�.
		int iSignalOffset = int((float)m_dqProjectileSignalOffset.front()/* / m_fFrameSpeed*/);		// ������ �����̹Ƿ� �������� �������´�� �����ӵ� �ʰ� �þ�Ƿ� �׳� ���Ѵ�.

		if( m_bCheckProjectileSignalTerm )
		{
			// ������ ���ǵ尡 �������� ������ �� Ŀ�� �� �����Ƿ� ����.
			if( fCurrentFrame+(8.0f*m_fFrameSpeed) < iSignalOffset )
			{
				// TODO: ���̴�! ���� ����������� ���ڴµ�..
				return false;
			}
		}
		
		// �߻� ó�� �Ǿ����Ƿ� �ѹ� ���ְ�.
		m_dqProjectileSignalOffset.pop_front();

		return true;
	}

	bool CheckSkipAirCondition(int iSkill); // ���簡 Air�ΰ�� ������ Ŭ�� �ν������� ������ GroundMovable üũ���� �����ϴ� ��찡�ִ�.
	void ReplacementGlyph( DnSkillHandle hNewSkill );
	bool IsExclusiveSkill( int iSkillID, int iExclusiveID );
	int GetAvailSkillPointByJob( int iSkillID );
	int GetUsedSkillPointInThisJob( const int nJobID );

	// Ŭ���̾�Ʈ���� ��Ÿ�� �и��� ���������� �˷���.
	void RequestCooltimeParrySuccess( int iSkillID );

	// ��Ÿ �¾��� �� ����Ǿ�� �� ������ ���ξ� ��ų ���� ó���� ���� ������ ó��.
	void CheckNormalHitSEProcessor( DnActorHandle hHittedTarget, const CDnDamageBase::SHitParam& HitParam );

	void AddStageDeathCount() { ++m_nDeathCount; };
	void ClearStageDeathCount() { m_nDeathCount = 0;};
	int GetStageDeathCount() { return m_nDeathCount;};

	void InitializeEnchantPassiveSkills( void );
	// from MASkillUser
	virtual void OnReplacementSkill( DnSkillHandle hLegacySkill, DnSkillHandle hNewSkill );
	// �ش� ��ų ID �� �´� ��ȭ ��ų�� ���� �ִ��� üũ�ؼ� ���� �ִٸ� ������� �ش�.
	void CheckAndApplyEnchantPassiveSkill( DnSkillHandle hBaseSkill );

	// ���� ������Ż�ε��� ���̽� �����ǿ����� ���������� 2������ ����ȿ�� ���͸��� �ɰ� �ִµ�
	// �� �߿� �ϳ��� hit �Ǳ� ���ϱ� ������ ���� üũ�� ���� ���� ���͸��� �ɸ��� �ʵ��� �Ѵ�. #28747
	// CDnPlayerActor ���� ��� ����.
	virtual void OnHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex );
	virtual bool CheckHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex );

	virtual bool IsMyRelicMonster( DnActorHandle hActor );

	BubbleSystem::CDnBubbleSystem* GetBubbleSystem( void ) { return m_pBubbleSystem; };
	void RemoveAllBubbles( bool bRemoveEvent );

	// #26902
	// 2�� ���� ��ų�� �ӽ÷� ����غ� �� �ִ� ���·� ������ֱ� ���� ����Ʈ���� ���Ǵ� api ���� ȣ��Ǵ� �Լ�.
	// ��� ���� ���� �� ��� ���� ������ ���� ĳ������ ������ �����ش�.
	// Ŭ���̾�Ʈ �ʿ����� ��ų Ʈ���� �����ֱ� ���� �ӽ÷� ������ �����ش�.
	bool IsTempSkillAdded( void ) { return m_bTempSkillAdded; };
	bool CanChangeJob( int iJobID );
	void SendTempJobChange( int iJobID );
	void EndAddTempSkillAndSendRestoreTempJobChange( void );
	void AddTempSkill( int iSkillID );
	void RemoveAllTempSkill( void );

	bool IsCannonMode( void ) { return (m_hCannonMonsterActor && m_bPlayerCannonMode); };
	void OnCannonMonsterDie(void );
	void EndCannonMode();

	// Ż�� ����Ʈ ��ü�� ������ �����ų� ���ӿ� ������ ��ġ�°�쿡�� �߰��� ����� �ϰ����� �ϴ� �׷���찡 ����� ���⶧���� �ϴ� ������ �ε����� ������ �ֵ��� �ϰڽ��ϴ�. 
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

	// Pet / Vehicle�� ���ؼ� �������� Process������ �ϴ� ���� �� �Լ� ���ο� �߰����ݽô�.
	// Ŭ���̾�Ʈ�� Task�� ������.

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
	//��ųƮ��â������ �������� DnSkillTask���� ���� �ǰ� ������
	//�����ʿ��� ��ų ������ ������ ��/Ż���� ���� ȿ�� ���� �κ� �����Ǿ� ����.
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
