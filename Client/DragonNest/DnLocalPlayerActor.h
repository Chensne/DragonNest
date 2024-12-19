#pragma once

#include "DnActor.h"
#include "InputReceiver.h"

#include "DnBlindDlg.h"
// include Method
#include "MACP.h"
#include "MACP_Renewal.h"
#include "DnPlayerActor.h"
#ifdef PRE_ADD_VIP
#include "DnVIPDataMgr.h"
#endif

// 현재 무기 갯수는 주무기/보조무기 2개임.
const int NUM_WEAPON = 2;

class CDnDamageReaction;
class CDnGameTask;

class CDnLocalPlayerActor : public CDnPlayerActor
						, public CInputReceiver
						, public CDnBlindCallBack
#if defined( PRE_ADD_CP_RENEWAL )
						, public MACP_Renewal
#else	// #if defined( PRE_ADD_CP_RENEWAL )
						, public MACP
#endif	// #if defined( PRE_ADD_CP_RENEWAL )
{
public:
	CDnLocalPlayerActor( int nClassID, bool bProcess = true );
	virtual ~CDnLocalPlayerActor();

	struct CrossHairType
	{
		enum
		{
			Normal = 0,
			Enemy,
			Npc,
			Blue,
			NotTalk,
			Player,
			Lever,
			Chest_UnLock,
			Chest_Lock,
			CursorMode,
			Pickup,
			Seed,
			Water,
			NotWater,
			Harvest,
			NotHarvest,
			CannonPick,
			CannonTarget,
			FlagTarget,
			//Ride, //seen in KOR
			MaxCount
		};
		
		int nType;
		char *szFileName;
	};

	static CrossHairType s_CrossHairTypeList[CrossHairType::MaxCount];

	enum eGhostScreenType
	{
		eGhostScreen_None = -1,
		eGhostScreen_Normal,
		eGhostScreen_PartyRestore,
	};

protected:
	// 네떡 Move 보내기 위한 조건들 변수들
	EtVector2 m_vPrevMoveVector;
	std::string m_szPrevMoveAction;
	LOCAL_TIME m_LastSendMoveMsg;

	// 특수 커맨드 체크 변수들
	LOCAL_TIME m_LastPushDirKeyTime[4];
	LOCAL_TIME m_LastPrevPushDirKeyTime[4];
	LOCAL_TIME m_LastReleaseDirKeyTime[4];
	BYTE m_cLastPushDirKey;
	BYTE m_cLastReleaseDirKey;
	char m_cLastPushDirKeyIndex;
	bool m_bResetMoveMsg;
	float m_fDelta;

	char m_cLastSpecialCommand;
	static int s_nSpecialCommandDelay;
	static int s_nDoubleCommandDelay;
	static int s_nVillageMaximumMoveSendTime;
	static int s_nGameMaximumMoveSendTime;

	EtTextureHandle m_hTargetCross[CrossHairType::MaxCount];
	MatrixEx m_Crosshair;

	bool m_bLockInput;
	bool m_bStandingBy;
	char m_cAutoRun;
	DnActorHandle m_hFollowActor;
	EtVector3 m_vFollowPosition;
	float m_fFollowDeltaTime;

	bool m_bProcessInputSignal;
	bool m_bApplyInputProcess;
	bool m_bCheckInputSignal;

	bool m_bClashHit;
	bool m_bLockItemMove;

	// 마지막으로 크로스헤어로 겨냥한 타겟 정보 
	struct AimTarget 
	{
		int  TargetType;

		DnActorHandle hActor;
		DnPropHandle hProp;
		void Reset() {
			hActor.Identity();
			hProp.Identity();
			TargetType = CrossHairType::Normal;
		};
	};

	AimTarget m_LastAimTarget;
	Farm::AreaState::eState m_eSeedState;

	static float s_fLastSaturationColor;
	static UINT ms_nLastHasNpcUID;
	static bool s_bShowCrosshair;

	int		m_nWarpLazyCount;
	EtVector3 m_WarpPos;
	EtVector2 m_WarpLook;	

	// 스핵관련
	DWORD		m_dwSyncDatumTick;

	// 무기 스킬 총 갯수는 파츠 타입 갯수+무기 갯수임.
	// DB user info 의 저장공간과 일치함. 추후에 캐쉬템도 필요하게 되면 추가 예정.
	// 현재는 문장만 장비 스킬 사용함.
	float			m_afLastEquipItemSkillDelayTime;
	float			m_afLastEquipItemSkillRemainTime;
	DnSkillHandle	m_ahEquipSkill;

	// Account Imp
	bool m_bAccountHide;

	CDnDamageReaction			*m_pDamageReaction;

	bool			m_bCancelStageMove;
	std::string		m_CancelStageMoveActionName;
	float			m_CancelStageMoveDir;
	float			m_CancelStageMoveDelta;

	EtDecalHandle		m_hDecal;
//	NavigationCell *m_pLastValidCell;
	EtVector3 m_vLastValidPos;
	LOCAL_TIME m_LastEscapeTime;
	std::vector<EtVector3> m_vVecPosHistory;
	float m_fPosHistoryDelta;
	bool m_bIgnoreRefreshViewCamera;

	float m_fPartyLeaderSleepCheckTimer;

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	EtSoundChannelHandle m_hNowPlayingNpcVoice;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	vector<float> m_vlSPUSingLimitByJob;

	bool m_bWaitCannonPossessRes;
	bool m_bNeedSendCannonTargeting;
	DnCameraHandle m_hCannonCamera;
	bool m_bCannonRotated;
	LOCAL_TIME m_LastSendCannonRotateSyncTime;
	EtVector3 m_vNowCannonGroundHitPos;

	float m_fForceSyncViewTime;

	int m_nPressedCount;

	DWORD m_dwPartyMemberMarkerColor;
	DWORD m_dwStigmaActorMarkerColor;

	// #27652 이슈 관련. 이 플래그가 켜져 있으면 CmdInputHasPassiveSkill() 함수에서 따로 체크 안한다.
	bool m_bInputHasPassiveSignalNotCheckPushKey;

	bool m_bFollowObserver; 
	UINT m_nObserverUniqueID;

	float m_fNpcOutlineCheckDelayTime;
	EtOutlineHandle m_hNpcOutline;
	DnActorHandle m_hActorNpcOutline;
	bool m_bCurrentNpcOutlineState;

	Ghost::Type::eCode	m_CurrentGhostTypeCache;
	EtVector4			m_LastHighLightColor;
	const EtVector4		m_PartyRestoreHighLightColor;

	std::vector<int> m_vecBackUpEventSlotItem;
	// #36908, #36693, #37625 
	// 발사체 자체에 추가 사거리 정보가 있거나,
	// RangeFallGravity 발사체를 쏘는 시그널이 있는 경우 GlobalWeightTable 에서 추가 사거리 가중치값을 얻어와서 적용.
	float m_fNormalProjectileAdditionalRange;

	LOCAL_TIME m_LastUpdatePickupPet;
	std::vector<int> m_nVecRequestPickupItemList;

#ifdef PRE_ADD_MAINQUEST_UI
	bool	m_bHideNpc;
	float	m_fHideNpcDelta;
#endif

protected:
	// Movement Message
	virtual void OnDrop( float fCurVelocity );
	virtual void OnFall( float fCurVelocity );
	virtual void OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash );
	virtual void OnMoveNavi( EtVector3 &vPosition );
#ifdef PRE_MOD_NAVIGATION_PATH
	virtual void OnAutoMoving( EtVector3& vPosition, EtVector3& vCurrentPosition );
#endif // PRE_MOD_NAVIGATION_PATH
	virtual void OnStop( EtVector3 &vPosition );

	// Actor Message
	virtual void OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor );
	virtual void OnComboFinish( int nCombo );
	virtual void OnChangeJob( int nJobID );
	virtual void OnCalcPlayerState(); // From PlayerActorState

	bool IsAllowMovement();
	void ProcessSpecialCommand( LOCAL_TIME LocalTime );
	bool CheckSpecialCommand( LOCAL_TIME LocalTime, char cCommandIndex );

	void ProcessActionSyncView();
	void ProcessPositionRevision( bool bForce = false );
	void ProcessAccountImp( LOCAL_TIME LocalTime, float fDelta );
	void ProcessLastValidNavCell( LOCAL_TIME LocalTime, float fDelta );
	void ProcessSyncPressedPos();
	void ProcessNpcOutline( float fDelta );
#ifdef PRE_ADD_MAINQUEST_UI
	void ProcessNpcHide( LOCAL_TIME LocalTime, float fDelta );
#endif

	virtual void ProcessDie( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessInput( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime = -1 );

	void RefreshMoveVector( DnCameraHandle hCamera );

	char CanTalkNpc( DnActorHandle hActor );

	int GetLevelUpSkillPoint( int nPrevLevel, int nCurLevel );

	virtual void OnEventCP( CPTypeEnum Type, int nResult );
	void UpdateAttackedCPPoint( CDnDamageBase *pHitter , CDnWeapon::HitTypeEnum eHitType );

	void RenderGateName();

	bool CheckAttributeEscape();
	bool CheckCollisionEscape();
	void UpdatePositioinHistory( float fDelta );
	bool IsExpectCollisionEscape();

	bool CheckSyncChangeActionCondition( int nCondition, int nOperator, char *szValue );
	bool CheckSyncChangeActionOperatorString( int nOperator, char *szValue1, char *szValue2 );
	bool CheckSyncChangeActionOperatorInt( int nOperator, int nValue1, int nValue2 );
	bool CheckSyncChangeActionOperatorFloat( int nOperator, float fValue1, float fValue2 );

public:
	void _OnAttachEquip( CDnItem* pItem, int Index );
	void _OnDetachEquip( CDnItem* pItem, int Index );

	void AttachDecal();
	bool CalcCrosshairOffset( EtVector3 *pvStartPos, MatrixEx *pResultCross = NULL, float *pfResultLength = NULL, DnActorHandle *phResultHandle = NULL, float fExpectationMinDistance = 0.f );
	bool CalcCrosshairOffsetActor( float fTargetLength, OUT EtVector3 *pvStartPos, OUT MatrixEx *pResultCross, 
		OUT float *pfResultLength, OUT DnActorHandle *phResultHandle, OUT int* pResultTargetType );
	bool CalcCrosshairOffsetProp( float fTargetLength, EtVector3 *pvStartPos, MatrixEx *pResultCross, 
		float *pfResultLength, DnPropHandle *phResultHandle, int* pResultTargetType );

	void SetStandingBy( bool bStandingBy ) { m_bStandingBy = bStandingBy; }
	bool IsStandingBy() { return m_bStandingBy; }
	bool IsProcessIntpuSignal() { return m_bProcessInputSignal; }

	void ReplacementGlyph( DnSkillHandle hNewSkill );

	void ProcessCrosshair();
	void DrawCrosshair( int nCrossHairIndex );

	void AddPressedCount( int nCount = 1 ) { m_nPressedCount += nCount; }

	// Actor Message
	virtual bool Initialize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessPVPChanges(LOCAL_TIME LocalTime, float fDelta); // Rotha PVP 관련 예외 처리
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
#ifdef PRE_MOD_PROJECTILE_HACK
	virtual void OnProjectile( CDnProjectile *pProjectile );
#else
	virtual void OnProjectile( CDnProjectile *pProjectile, int nSignalIndex );
#endif
	virtual void ResetActor();
	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachWeapon( int nEquipIndex = 0 );
	virtual void AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false, bool bRefreshWeaponOrder = true );
	virtual void DetachCashWeapon( int nEquipIndex = 0 );

	// MASkillUser
	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta, 
							   bool bCheckValid = true, bool bAutoUsedFromServer = false, bool bSendPacketToServer = true );

	// MAPlateUser

	bool ApplyGlyphToExpendedSkillPage( DnGlyphHandle hGlyph , int nSkillPage );

	virtual bool AttachGlyph( DnGlyphHandle hGlyph, CDnGlyph::GlyphSlotEnum Index, bool bDelete = false );
	virtual bool DetachGlyph( CDnGlyph::GlyphSlotEnum Index );

#if defined(PRE_ADD_TALISMAN_SYSTEM) 
	virtual bool AttachTalisman( DnTalismanHandle hTalisman, int Index, float fRatio, bool bDelete = false);
	virtual bool DetachTalisman( int Index );
#endif

	virtual void InitializeIdleMotion();

	// Actor Command
	virtual void CmdMove( EtVector2 &vDir, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bResetStop = false, bool bForce = false );
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, 
							bool bFromStateBlow = false, bool bSkillChain = false );
	virtual void CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, int nLoopCount = 0, float fFrame = 0.f, float fBlendFrame = 3.f );
	virtual void CmdLook( EtVector2 &vVec, bool bForce = false );
	virtual void CmdToggleBattle( bool bBattleMode );
	virtual void CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.0f, bool bChargeKey = false, bool bCheckOverlapAction = true, bool bOnlyCheck = false );
	virtual void CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bSendServer );
	virtual void CmdWarp( EtVector3 &vPos, EtVector2 &vLook );
	virtual void CmdToggleWeaponViewOrder( int nEquipIndex, bool bShowCash );
	virtual void CmdTogglePartsViewOrder( int nEquipIndex, bool bShowCash );
	virtual void CmdToggleHideHelmet( bool bHideHelmet );

	virtual void CmdEscape();

	virtual bool IsLocalActor() { return true; }

	// EtCustomRender
	virtual void RenderCustom( float fElapsedTime );
	// TransAction Message
	virtual void OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket );

	void RenderPartyMemberMarker(float fElapsedTime);
	void RenderStigmaActorMarker(float fElapsedTime);
	bool IsPartyLeader() const;

	// ActorState
	virtual void OnLevelUp( int nLevel, int nLevelUpAmount );
	virtual void OnDie( DnActorHandle hHitter );
	void SetWorldColorFilterOnDie();
	eGhostScreenType IsSetGhostColorFilter() const;

	virtual void OnChangeAction( const char *szPrevAction );
	virtual void OnBeginProcessSignal();
	virtual void OnEndProcessSignal();

	// Blind Callback
	virtual void OnBlindClosed();

	// Local
	static void LockInput( bool bLock );
	static bool IsLockInput();
	static void ShowCrosshair( bool bShow ) { s_bShowCrosshair = bShow; }
	virtual bool IsCanBattleMode(); 

	static void LockItemMove(bool bLock); // 특정 상황에서 강제로 아이템이동을 못하도록 셋팅.
	static bool IsLockItemMove();         // ItemTask에서 CheckDie 에서 사용하게됩니다.

	MatrixEx *GetCrossHairVector() { return &m_Crosshair; }

	DnActorHandle GetLastAimActor();
	DnPropHandle GetLastAimProp();

	static UINT GetTakeNpcUID() { return ms_nLastHasNpcUID; }
	static void SetTakeNpcUID(UINT nNpcUID) { ms_nLastHasNpcUID = nNpcUID; }

	static void SetSaturationColor( float fColor ) { s_fLastSaturationColor = fColor; }
	static float GetSaturationColor() { return s_fLastSaturationColor; }

	void SetCurrentGhostType(Ghost::Type::eCode code) { m_CurrentGhostTypeCache = code; }
	const Ghost::Type::eCode& GetCurrentGhostType() const { return m_CurrentGhostTypeCache; }
	const EtVector4& GetLastHighLightColor() { return m_LastHighLightColor; }

	static void StopAllPartyPlayer();
	static void ResetCameraLook( bool bResetWhenActivated = false );

	virtual void OnComboCount( int nValue, int nDelay );

	virtual bool IsCanToggleBattleMode();

	void ResetAutoRun();
	bool IsAutoRun() { return ( m_cAutoRun == 2 ) ? true : false; }
	void FollowActor( DnActorHandle hFollowActor );
	void FollowPosition( EtVector3 &vPos );
	void ProcessFollow( float fDelta );
	bool IsFollowing();

	// 무기/악세사리에 스킬이 달려 있는 경우 스킬 추가. 빠지는 경우엔 스킬 제거.
	virtual bool AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false, bool bUseCombine = true );
	virtual bool DetachParts( CDnParts::PartsTypeEnum Index );

	virtual bool AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false, bool bUseCombine = true );
	virtual bool DetachCashParts( CDnParts::PartsTypeEnum Index );

	// 서버에 저장된 장비아이템 쿨타임
	void SetEquipCoolTime( const int aiDelayTime, const int aiRemainTime, int iEquipCount );

	void	SetSyncDatumTick(){ m_dwSyncDatumTick = GetTickCount(); }
	DWORD	GetSyncDatumGap() const { return GetTickCount()-m_dwSyncDatumTick; }


	void SetAccountHide( bool bHide ) { m_bAccountHide = bHide; }

	void OnDecreaseInvenDurability( int nValue );
	void OnDecreaseInvenDurability( float fValue );

	void SetInteractivePos( EtVector3 *vPos, float fDelta );

	bool ChangeSocialTexture( int nSocialIndex, int nFrameIndex ) override;

	void OnCancelStage(const EtVector3& targetDir);
	bool IsIgnoreRefreshViewCamera() { return m_bIgnoreRefreshViewCamera; }
	void SetIgnoreRefreshViewCamera( bool bEnable ) { m_bIgnoreRefreshViewCamera = bEnable; } 
	
	virtual void RideVehicle(TVehicleCompact sInfo , bool bForce = false);
	virtual void UnRideVehicle(bool bIgnoreEffect = false,bool bIgnoreAction = false);
	void SendCompleteRideVehicle(bool bComplete);

#ifdef PRE_ADD_MAINQUEST_UI
	void SetHideNpc(bool bFlag, float fDelta);
#endif

#ifdef PRE_ADD_VIP
	virtual void			SetVIP(bool bVIP)		{ _ASSERT(0); }
	virtual bool			IsVIP() const;
	time_t					GetVIPTime();
	CDnVIPDataMgr&			GetVIPDataMgr()			{ return m_VIPDataMgr; }
	const CDnVIPDataMgr&	GetVIPDataMgr() const	{ return m_VIPDataMgr; }
	CDnVIPDataMgr			m_VIPDataMgr;
#endif

#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)
	bool IsVipFarm();
#endif	//#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	void UseNpcVoicePlayer( CDnItem* pNpcVoicePlayerItem );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

#ifdef PRE_MOD_NAVIGATION_PATH
	void AutoMovingToPosition( EtVector3 &vPos );
#endif // PRE_MOD_NAVIGATION_PATH

	void UpdateSPUsingLimitByJob( void );
	int GetSPUsingLimitByJobArrayIndex( int iJobArrayIndex );
	int GetJobArrayIndex( const int nJobID );

	void ProcessPartyAbsenceCheck(float fDelta);
	void ProcessIdleMotion(float fDelta);

	bool m_bPlayerAwayForIdle;
	float m_fIdleMotionTimer;
	struct SIdleMotionInfo
	{
		float fStartTime;
		float fCoolTimeMin;
		float fCoolTimeMax;
		std::string motionName;

		SIdleMotionInfo() : fStartTime(0.f), fCoolTimeMin(0.f), fCoolTimeMax(0.f) {}
		bool IsValid() const { return (motionName.empty() == false && fStartTime > 0.f); }
	} m_IdleMotionInfo;

	void SendPossessCannonReq( void );
	virtual void ToggleCannonMode( bool bCannonMode, DWORD dwCannonMonsterID, const MatrixEx& Cross );
	void ExitCannonMode( void );		// dninterface 에서만 사용됨.
	void ProcessCannonRotateSync( void );
	void CannonRotated( void ) { m_bCannonRotated = true; };		// CannonCamera 쪽에서 호출됨.
	virtual void OnCannonMonsterDie( void );
	void EndLocalPlayerCannonMode( void );
	virtual void ResetWaitCannonPossess( void ) { m_bWaitCannonPossessRes = false; };
	void OnCannonCameraProcessEnd( void );

	void ForceSyncViewTime( float fTime ) { m_fForceSyncViewTime = fTime; }

	bool IsFollowObserver(){return m_bFollowObserver;}
	void SetFollowObserver(bool bTrue,int nUniqueID);
	UINT GetFollowObserverUniqueID(){return m_nObserverUniqueID;}
	void ProcessAttachObserver();
	virtual void RefreshTransformMode();
	virtual void ChangeShootModeInputAction(); // Processinput 에서 생기는 슛모드액션관련한 예외처리 공통으로쓰인다.
	virtual bool OnApplySpectator(bool bEnable);

#ifdef PRE_ADD_MONSTER_CATCH
	virtual void OnReleaseFromMonster( DnActorHandle hCatcherMonsterActor );
#endif // #ifdef PRE_ADD_MONSTER_CATCH

	void GuildWarAlarm();

	virtual void ChangeSkillLevelUp(int nSkillID, int nOrigLevel);

	virtual bool ProcessPet( LOCAL_TIME LocalTime, float fDelta );
	void ProcessPickupPet( LOCAL_TIME LocalTime, float fDelta );

	// #45326 이 스킬에 대해 조건 체크 무시 플래그가 켜져 있다면 스킬 사용 조건 무시하도록 처리.
	void BeginCanUsePassiveSkill( LOCAL_TIME SignalEndTime );
	void EndCanUsePassiveSkill( void );

#if defined(PRE_FIX_45899)
public:
	void DisableSkillByItemMove(int nSkillID);
#endif // PRE_FIX_45899

	virtual int GetPlayerGuildRewardEffectValue(int nType);

#ifdef PRE_SOURCEITEM_TIMECHEAT
public:
	bool m_bSourceItemTimeCheat;
#endif

#if defined(PRE_FIX_57706)
//////////////////////////////////////////////////////////////////////////
//클라이언트에서 상태효과 제거 조건이 되었을때 서버로 패킷 보낼때 사용..
//FrameStop인 경우 시간 변경이 LocalPlayer액터에서만 동작 하기 때문에 클라이언트에서 시간 확인해서
//제거되어야 할 상태효과 ID를 서버에 전달 하고, 서버에서 다시 브로트 캐스팅 해서 각각의 클라이언트가
//상태효과 제거 되도록 한다.
public:
	void CmdRemoveStateEffectByServerBlowID( int nServerBlowID );
//////////////////////////////////////////////////////////////////////////
#endif // PRE_FIX_57706

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
	void OpenMailBoxByShortCutKey();
	void CloseMailBoxByShortCutKey();
#endif // PRE_ADD_MAILBOX_OPEN

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
protected:
	void UpdateTotalLevelUI();

public:
	virtual void UpdateTotalLevel(int nLevel);
	virtual void UpdateTotalLevelByCharLevel();

	virtual void AddTotalLevelSkill(int nSlotIndex, int nSkillID, bool isInitialize = false);
	virtual void RemoveTotalLevelSkill(int nSlotIndex);

	virtual void ActivateTotalLevelSkillSlot(int nSlotIndex, bool bActivate);
	virtual void ActivateTotalLevelSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate);

	virtual void RequestAddTotalLevelSkill(int nSlotIndex, int nSkillID);
	virtual void RequestRemoveTotalLevelSkill(int nSlotIndex);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL


#ifdef PRE_ADD_NEWCOMEBACK
	void ShowComebackUserEffect();
#endif // PRE_ADD_NEWCOMEBACK

#if defined(PRE_FIX_68898)
public:
	virtual void SetSkipEndAction(bool isSkipEndAction);
#endif // PRE_FIX_68898
};
