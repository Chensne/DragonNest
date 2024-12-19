#pragma once

#include "DnActor.h"
#include "DnPlayerState.h"
#include "DnPlayerGaugeDlg.h"

//Rotha Vehilce
#include "DnVehicleActor.h"
#include "DnLocalVehicleActor.h"
#include "DnSimpleParts.h"

// include Method
#include "MAPartsBody.h"
#include "MAWalkMovement.h"
#include "MAPlateUser.h"
#if defined(PRE_ADD_TALISMAN_SYSTEM) 
#include "MATalismanUser.h"
#endif

#define	FISHING_EFFECT_GAP	1.5f

class CDnActorPreview;
class CDnPetActor;
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
class CDnTotalLevelSkillSystem;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

namespace BubbleSystem
{
	class CDnBubbleSystem;
}

class CDnPlayerActor : public CDnActor, virtual public MAPartsBody, virtual public CDnPlayerState, virtual public MAPlateUser 
#if defined(PRE_ADD_TALISMAN_SYSTEM) 
	, virtual public MATalismanUser
#endif
{
public:
	CDnPlayerActor( int nClassID, bool bProcess = true );
	virtual ~CDnPlayerActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAWalkMovement();
		return pMovement;
	}
	// 나 말고는 전부 숨긴다.
	static bool s_bEnableAnotherPlayer;
	static bool s_bHideAnotherPlayer;
	static float s_fHideAnotherPlayerDelta;

	struct S_EFFECT_SKILL
	{
		int iItemID;
		int iSkillID;
		int nEffectSkillLeftTime;
		bool bUsedInGameServer;
		bool bEternity;
		S_EFFECT_SKILL( void ) : iItemID( 0 ), iSkillID( 0 ), nEffectSkillLeftTime( 0 ), bUsedInGameServer(false) , bEternity(false){};
	};

protected:
	std::string m_szMaintenanceBoneName;
	std::string m_szActionBoneName;
	char m_cMovePushKeyFlag;
	bool m_bBattleMode;

	int m_nComboCount;
	int m_nComboDelay;

	DnPartsHandle m_hNudeParts[ CDnParts::DefaultPartsTypeEnum_Amount ];
	int m_nDefaultPartsIndex[ CDnParts::DefaultPartsTypeEnum_Amount ];

	bool m_bGhost;
	int m_nSwapSingleSkinActorID;
	EtAniObjectHandle m_hSwapOriginalHandle;
	CDnActionBase *m_pSwapOriginalAction;

	DnWeaponHandle m_hCashWeapon[ CDnWeapon::EquipSlot_Amount ];
	bool m_bCashSelfDeleteWeapon[ CDnWeapon::EquipSlot_Amount ];
	bool m_bWeaponViewOrder[ CDnWeapon::EquipSlot_Amount ];

	bool m_bHideWeaponBySignalInCutScene[ CDnWeapon::EquipSlot_Amount ];

	std::wstring m_wszAppellationName;
	std::wstring m_wszJobName;
	std::wstring m_wszPvPLevelName;

	float m_fExposureTime;
	bool m_isExposure;//적일 경우 노출 되었는지 체크
	bool m_isLocalExposure;//내 화면에 노출 되었는지 체크


	TGuildSelfView m_GuildSelfView;

#ifdef PRE_ADD_DWC
	WCHAR m_wszDWCTeamName[GUILDNAME_MAX];
#endif

	// 스킬트리 초기화 되었는지 여부. 캐릭터의 job id 가 필요하기 때문에 플레이어 정보보다 스킬리스트가 먼저오는 튜토리 얼때문에..
	bool m_bInitializedSkillTree;

	bool m_bChangedEquipAction;				// 무기별 공격액션으로 바뀐 경우.
	std::string m_strOriginalNoneEquipAction;	// 무기별 공격액션으로 바뀐 경우 무기와 관계없는 일반 액션이름.

	bool m_bPlayerAway;
	EtTextureHandle m_hAbsenceMarkTex;
	EtDecalHandle m_hDecal;
	EtTextureHandle m_hDecalTexture;

	EtAniObjectHandle m_hSaveMergedObject; // 컴바인파츠를 피하기위한 용도로 사용한다.

	BubbleSystem::CDnBubbleSystem* m_pBubbleSystem;

	bool m_bPartyLeader;
	EtTextureHandle m_hPartyLeaderIconTex;

	std::string m_szLastWeaponAction[2];

	bool m_bPlayerCannonMode;
	DnActorHandle m_hCannonMonsterActor;

	// PVP System //
	bool m_bObserver;
	bool m_bCaptain;
	DnSimplePartsHandle m_hCaptainHelmetParts;

	DnEtcHandle m_hPlayerEffect; // 지속적으로 상주하는 이펙트를 여기서 관리
	bool m_bTogglePlayerEffect;

	bool m_bForceEnableRideByTrigger;
	float m_fRideDelay;
	bool m_bEnableVehicleThread;
	bool m_bVehicleMode;                 // Rotha Vehicle Mode Added
	UINT m_nVehicleSessionID;
	DnActorHandle m_hVehicleActor;
	EtAniObjectHandle m_hPostParentHandle;   // 내릴대 붙을 용도로 필요하다.
	TVehicleCompact m_sVehicleCompact;

	DnSimplePartsHandle m_hFishingRodParts;	// 낚싯대 파츠
	bool m_bShowFishingRod;
	bool m_bStartCast;
	bool m_bStartLift;
	bool m_bRecieveFishingReward;
	float m_fFishingTimer;
	int m_nFishingRetCode;
	int m_nFishingRewardItemID;

	DnActorHandle	m_hPet;
	DnEtcHandle		m_hPetEffect;
	bool			m_bSummonPet;
	bool			m_bFollowPetAni;
	bool			m_bChatExpire;
	bool			m_bPetSlipAni;
	bool			m_bPetSummonableMap;
	float			m_fPetFollowDeltaTime;
	float			m_fPetPickUpRange;
	std::string		m_strPetPickUpActionName;
	TVehicleCompact	m_sPetCompact;

	std::deque<S_EFFECT_SKILL> m_dqEffectSkill;

#ifdef PRE_ADD_CASH_COSTUME_AURA // : proceted 
	enum
	{
		eParts_Head = 0,
		eParts_Body  ,
		eParts_L_Arm ,
		eParts_R_Arm ,
		eParts_L_Leg ,
		eParts_R_Leg ,
		eParts_L2_Leg ,
		eParts_R2_Leg ,

		eParts_Max
	};
	DnEtcHandle				 m_hPlayerAuraEffect[eParts_Max];
	std::vector<std::string> m_vecActionName;
	std::vector<std::string> m_vecBoneName;
	bool					 m_bEnableCostumeAura;			 // 오오라 On / Off 상태 플래그.
	bool					 m_bIsCompletParts;				 // 파츠 아이템 풀셋인지 체크 플래그.
	int						 m_nEquipeditemID;
	RenderType				 m_AuraRenderType;				 // 랜더타겟 / 씬에 그릴지 체크


	struct sCurrentAuraCostume
	{
		int nSetItemID;
		int nItemID;
		sCurrentAuraCostume() :nItemID(0) , nSetItemID(0) { }
	};
	sCurrentAuraCostume		 m_SelectedCostumeData;

	int GetSetItemMaxPartsCnt(int nTableEnum, int nCurrentItemID, int nCurrentSetItemID);
	int GetMyEquipSetItemPartsCnt(int nTableEnum, int nItemID, int nSetItemID, bool bIsDetach = false);

	void ComputeCostumePartsCount_ByAttach(DnPartsHandle pParts);
	void ComputeCostumePartsCount_ByDetach(int nSlotIndex );

	void ComputeCostumeWeaponCount_ByAttach(DnWeaponHandle pParts);
	void ComputeCostumeWeaponCount_ByDetach(int nSlotIndex );
#endif //PRE_ADD_CASH_COSTUME_AURA

	bool m_bShootMode;
	bool m_bRefreshTransformMode;
	bool m_bTransformMode;
	int m_nTransformMoveSpeed;
	int m_nTransformTableID;
	std::vector<int> m_vecTransformSkillList;
	std::string m_strTransformEndAction;

#ifdef PRE_ADD_TRANSFORM_POTION
	bool m_bRefreshVillageTransformMode;
	int m_nTransformItemIndex;
#endif

#ifdef PRE_ADD_MONSTER_CATCH
	DnActorHandle m_hCatcherMonster;		// 날 잡고 있는 몬스터.
#endif // #ifdef PRE_ADD_MONSTER_CATCH

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	std::string m_szLastRecomposeActionElement;
#endif

#ifdef PRE_ADD_VIP
	bool m_bVIP;
#endif

#ifdef PRE_ADD_NEWCOMEBACK	
	bool m_bComebackLocalUser;		
#endif // PRE_ADD_NEWCOMEBACK	

#ifdef PRE_ADD_COSTUME_SKILL
	int m_nCostumeSkillID;
#endif
#ifdef PRE_ADD_GM_APPELLATION
	bool m_bGMAppellation;
#endif // PRE_ADD_GM_APPELLATION

#ifdef PRE_ADD_GM_APPELLAThongse
	bool m_hongse;
#endif
#ifdef PRE_ADD_GM_APPELLAchengse
	bool m_jinse;
#endif
protected:
	// Movement Message
	virtual void OnDrop( float fCurVelocity );
	virtual void OnStop( EtVector3 &vPosition );
	virtual void OnFall( float fCurVelocity );
	virtual void OnStopReady() {}

	// Actor Message
	virtual void ProcessDie( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnComboFinish( int nCombo ) {}

	void ProcessCombo( LOCAL_TIME LocalTime, float fDelta );

	// CDnActorState
	virtual void OnLevelUp( int nLevel, int nLevelUpAmount );
	virtual void OnRefreshState();

	void AttachNudeParts( CDnParts::PartsTypeEnum Index );
	void DetachNudeParts( CDnParts::PartsTypeEnum Index );

	std::string CheckBattleModeAction( const char *szActionName );

	void LinkCashWeapon( int nEquipIndex );
	
	void RenderHeadIcon( float fElapsedTime );

public:
	virtual void OnChangeJob( int nJobID );

	virtual void OnDie( DnActorHandle hHitter );
	virtual int  OnGetJobClassID(){ return CDnPlayerState::GetJobClassID(); }
	// Actor Message
	virtual bool Initialize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void PreProcess( LOCAL_TIME LocalTime, float fDelta );
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	virtual bool LoadAction( const char *szFullPathName );
#endif

	void ProcessDecal();
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
	virtual void SyncClassTime( LOCAL_TIME LocalTime );
	virtual void ResetActor();
	virtual void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false );
	virtual void SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop = false );
	virtual bool SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bCheck = true, bool bCheckStateEffect = true );
	virtual void OnChangeAction( const char *szPrevAction );

	// Actor Command
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bResetStop = false, bool bForce = false );
	virtual void CmdLook( EtVector2 &vVec, bool bForce = false );
	virtual void CmdToggleBattle( bool bBattleMode );
	virtual void CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.0f, bool bChargeKey = false, bool bCheckOverlapAction = true, bool bOnlyCheck = false );
	virtual void CmdToggleWeaponViewOrder( int nEquipIndex, bool bShowCash );
	virtual void CmdTogglePartsViewOrder( int nEquipIndex, bool bShowCash );
	virtual void CmdToggleHideHelmet( bool bHideHelmet );

	virtual void CmdWarp( EtVector3 &vPos, EtVector2 &vLook );

	// TransAction Message
	virtual void OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket );

	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachWeapon( int nEquipIndex = 0 );

	virtual DnBlowHandle CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkillInfo, 
											STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit = false );

	// MAPartsBody
	virtual bool AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false, bool bUseCombine = true );
	virtual bool DetachParts( CDnParts::PartsTypeEnum Index );

	// CDnActorState
	virtual void SetAppellationIndex( int nCoverIndex, int nIndex );
	virtual void SetJobHistory( const std::vector<int> &nVecList );
	virtual void SetJobHistory( int nValue );
	virtual void PopJobHistory( int nValue );		// #26902 마지막으로 전직한 것을 롤백. 현재 서버에서 이런 거 하는 경우는 없다. 임시 전직에 대해서 클라에서 사용됨.
	virtual void SetPvPLevel( char cLevel );

	// Local
	virtual bool IsLocalActor() { return false; }
	virtual bool IsPlayerActor() { return true; }
	bool IsGhost() { return m_bGhost; }
	bool IsJump();

	virtual void Show( bool bShow );
	virtual void ShowWeapon( int nEquipIndex, bool bShow );

	virtual int GetMoveSpeed();
	virtual void RenderCustom( float fElapsedTime );
#ifdef PRE_FIX_MAIL_HEADICON
	void RenderMailBoxMark(int nRenderLayer);
#endif

	WCHAR *GetJobName() { return (WCHAR*)m_wszJobName.c_str(); }
	WCHAR *GetPvPLevelName() { return (WCHAR*)m_wszPvPLevelName.c_str(); }

	// 케쉬 무기
	virtual void AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false, bool bRefreshWeaponOrder = true );
	virtual void DetachCashWeapon( int nEquipIndex = 0 );
	virtual DnWeaponHandle GetCashWeapon( int nEquipIndex = 0, bool bActionMatchWeapon = true ) { return m_hCashWeapon[nEquipIndex]; }
	virtual void ShowCashWeapon( int nEquipIndex, bool bShow );
	virtual void RefreshWeaponViewOrder( int nEquipIndex );
	void SetWeaponViewOrder( int nEquipIndex, bool bShowCash ) { m_bWeaponViewOrder[nEquipIndex] = bShowCash; }
	bool IsViewWeaponOrder( int nEquipIndex ) { return m_bWeaponViewOrder[nEquipIndex]; }
	virtual DnWeaponHandle GetActiveWeapon( int nEquipIndex );
	virtual void SetWeaponFPS( float fValue );
	virtual void ReleaseWeaponSignalImp();
	bool IsHideWeaponBySignalInCutScene( int nEquipIndex ) { return m_bHideWeaponBySignalInCutScene[nEquipIndex]; }
	void ResetHideWeaponBySignalInCutScene() { memset( m_bHideWeaponBySignalInCutScene, 0, sizeof(m_bHideWeaponBySignalInCutScene) ); }
#ifdef PRE_MOD_KALI_WEAPON
	virtual void ProcessAdditionalWeapon( LOCAL_TIME LocalTime, float fDelta ) {}
#endif

	static char *GetLinkWeaponBoneName( int nEquipIndex, bool bBattleMode );
	virtual bool IsWaitingThread();

#ifdef PRE_ADD_37745
	virtual void SetBattleMode( bool bEnable , bool bForce = false );
#else
	virtual void SetBattleMode( bool bEnable );
#endif
	bool IsBattleMode() { return m_bBattleMode; }
	
	virtual bool IsCanBattleMode();
	virtual void OnResetAttachWeaponAction( DnWeaponHandle hWeapon, int nEquipIndex );

	bool IsChangedEquipAction( void ) { return m_bChangedEquipAction; };
	void ClearChangedEuipAction( void ) { m_bChangedEquipAction = false; };
	const char* GetOriginalNoneActionEquipAction( void ) { return m_strOriginalNoneEquipAction.c_str(); };

	BubbleSystem::CDnBubbleSystem* GetBubbleSystem() const { return m_pBubbleSystem; }
	void RemoveAllBubbles( bool bRemoveEvent );

	virtual bool ChangeSocialTexture( int nSocialIndex, int nFrameIndex );
	virtual void OnComboCount( int nValue, int nDelay );

	bool UseItemFromItemID( int nItemID, char cType = 0 );			// eUseItemInvenType
	bool UseItemFromSlotIndex( int nSlotIndex, char cType = 0 );	// eUseItemInvenType

	void SetDefaultPartsInfo( int *pPartsIndex );
	int GetDefaultPartsInfo( const CDnParts::PartsTypeEnum Index );
	int GetNudePartsInfo( CDnParts::PartsTypeEnum Index );

	bool IsNudeParts( DnPartsHandle hParts ) const;
	bool IsDefaultParts(ITEMCLSID classId) const;

	void ToggleGhostMode( bool bGhost, bool bShowEffect = true );
	void SwapSingleSkin( int nChangeActorTableID , bool bSaveMergedObject = false);
	bool IsSwapSingleSkin() { return ( m_nSwapSingleSkinActorID == -1 ) ? false : true; }
	int GetSingleSkinActorID(){return m_nSwapSingleSkinActorID;}

	void ProcessHidePlayer( LOCAL_TIME LocalTime, float fDelta );

	static void EnableAnotherPlayers( bool bEnable );
	static void HideAnotherPlayers(bool bHide, float fTime);

	virtual void SetDestroy();

	void SetExposure( bool isLocal);
	bool GetExposure(){return m_isExposure;};

	// 내구도
	void OnDecreaseEquipDurability( int nValue );
	void OnDecreaseEquipDurability( float fValue );

	void SetGuildSelfView( const TGuildSelfView &GuildSelfView );
	void SetGuildView( const TGuildView &GuildSelfView );
	bool IsJoinGuild() { return m_GuildSelfView.IsSet(); }
	bool IsGuildMaster() { return (m_GuildSelfView.btGuildRole == GUILDROLE_TYPE_MASTER); }
	BYTE GetGuildRole() { return m_GuildSelfView.btGuildRole; }
	TGuildSelfView &GetGuildSelfView() { return m_GuildSelfView; }

	void InitializedSkillTree( void ) { m_bInitializedSkillTree = true; };
	bool IsNeedInitializeSkillTree( void ) { return !m_bInitializedSkillTree; };

	void SetAbsence(bool bAbsence);
	void SetPartyLeader(bool bLeader)	{ m_bPartyLeader = bLeader; }
	virtual bool IsPartyLeader() const	{ return m_bPartyLeader; }

	virtual void InitializeIdleMotion() {}
	bool IsUsableItem( CDnItem* pItem );

#ifdef PRE_ADD_DWC
	void SetDWCTeamName(WCHAR* wszTeamName); 
#endif

	// Cannon Mode //
	virtual void ToggleCannonMode( bool bCannonMode, DWORD dwCannonMonsterID, const MatrixEx& Cross );
	virtual void OnCannonMonsterDie( void );
	bool IsCannonMode( void ) { return (m_bPlayerCannonMode && m_hCannonMonsterActor); }
	void EndCannonMode( void );
	virtual void ResetWaitCannonPossess( void ) {};

	// PVP System Functions // 
	void DrawPVPName( bool isMyTeam, bool isSimple, float fElapsedTime );
	void DrawPVPCharInfo( float fElapsedTime );
	void ProcessVisual( LOCAL_TIME LocalTime, float fDelta);
#if defined( PRE_ADD_REVENGE )
	void DrawRevengeName( const UINT eRevenge );
#endif	// #if defined( PRE_ADD_REVENGE )

	DnEtcHandle GetLinkedPlayerEffect() { return m_hPlayerEffect; }
	bool IsEnalbeLinkedPlayerEffect() { return m_bTogglePlayerEffect; }
	void ToggleLinkedPlayerEffect( bool bEnable , const char *strEffectName = "");
	void ToggleCaptainMode( bool bEnable, bool bForce = false );
	bool IsCaptainMode(){return m_bCaptain;};

	bool IsObserver(){return m_bObserver;}
	void SetObserver(bool bTrue){m_bObserver = bTrue;}
	virtual void ProcessPVPChanges(LOCAL_TIME LocalTime, float fDelta);
	virtual void OnInitializePVPRoundRestart( void );

#ifdef PRE_ADD_CASH_COSTUME_AURA // : public
	bool			IsEnableCostumeAura() { return m_bEnableCostumeAura; }

	void			ShowCostumeAura();
	void			ProcessAdditionalAura( LOCAL_TIME LocalTime, float fDelta );
	bool			ComputeMyEquipCostumeAura(CDnPlayerActor *pPlayer = NULL);
	void			LoadCostumeAuraEffect(bool bEnable, int nItemID = -1);
	RenderType		GetAuraRenderType() { return m_AuraRenderType; }
	void			SetAuraRenderType(RenderType type) { m_AuraRenderType = type; }

	
	void			ComputeRTTModeCostumeAura();
	void			ComputeRTTModeCostumeAura(DnActorHandle hPlayer);
#endif

#ifdef PRE_ADD_COSTUME_SKILL
	void RefreshCostumeSkill( int nSkillIndex , int nSkilllevel);
	int GetCostumeSkillIndex(){ return m_nCostumeSkillID; }
#endif

	void MakeComparePotentialItemState(CDnState& State, CDnItem* pCurrItemState, CDnItem* pNewItemState);
	
	bool IsSpectatorMode();
	virtual bool OnApplySpectator(bool bEnable);

	void CmdShootMode(bool bTrue);
	void ProcessNonLocalShootModeAction();
	const char *GetChangeShootActionName(const char *szActionName);

	bool IsTransformMode(){ return m_bTransformMode; }
	bool IsTransformModeSkill( int nSkillID );

	bool IsShootMode() { return m_bShootMode; }
	void ToggleTransformMode(bool bTrue, int nTransformTableID = -1, bool bForce = false, const char* strEndAction = "Stand");
	virtual void RefreshTransformMode();

#ifdef PRE_ADD_TRANSFORM_POTION // 시간이 생기면 위의 모드와 통합해봅시다.
	void ToggleVillageTransformMode( bool bEnable , int nItemIndex , int nDurationTime );
	void SetRefrehVillageTransformMode( int nIndex );
	bool IsVillageTransformMode(){ return m_nTransformItemIndex > 0; }
#endif

	// Additional Functions //

	DnPartsVehicleEffectHandle m_hPartsVehicleEffect;
	bool m_bSelfDeleteVehicleEffect;
	DnPartsVehicleEffectHandle GetPartsVehicleEffect(){return m_hPartsVehicleEffect;}
	void SetPartsVehicleEffect(DnPartsVehicleEffectHandle hHandle, bool bDelete = false);

	void InsertRideVehicleThread( DnActorHandle hActor ); // 탈것 생성시 CreateActor 가 수행되므로 쓰레드를 이용해줍니다.
	static int __stdcall OnLoadRideVehicleUniqueID( void *pParam, int nSize );	
	static bool __stdcall OnLoadRideVehicle( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime );

	int GetVehicleClassType();
	UINT GetVehicleSessionID(){return m_nVehicleSessionID;}
	void SetVehicleSessionID(UINT nSessionID){m_nVehicleSessionID = nSessionID;}
	bool IsVehicleMode(){return m_bVehicleMode;};
	bool IsCanVehicleMode();
	void SetForceEnableRide( const bool bForceEnableRide );

	void ProcessVehicleCall(LOCAL_TIME LocalTime, float fDelta);
	void SetVehicleInfo(TVehicleCompact VehicleInfo) { m_sVehicleCompact = VehicleInfo; }
	TVehicleCompact& GetVehicleInfo() { return m_sVehicleCompact; }

	void CallVehicle();
	void CancelCallVehicle();
	bool IsCallingVehicle(){return (m_fRideDelay != -1);}

	virtual void RideVehicle(TVehicleCompact sInfo , bool bForce = false); // bForce값이 True가 되면 생성이펙트 설정안함.
	virtual void UnRideVehicle(bool bIgnoreEffect = false,bool bIgnoreAction = false);
	void SetVehicleMode(bool bVehicle) { m_bVehicleMode = bVehicle;  };       
	void SetMyVehicleActor(DnActorHandle hVehicle){m_hVehicleActor = hVehicle; };
	CDnVehicleActor* GetMyVehicleActor();
	EtAniObjectHandle GetPostParentHandle(){return m_hPostParentHandle;}

	void SummonPet( int nPetItemID, LPCWSTR strPetName = L"", bool bSummonAni = false );
	DnActorHandle GetSummonPet() { return m_hPet; }
	virtual bool ProcessPet( LOCAL_TIME LocalTime, float fDelta );
	void ProcessFollowPet( LOCAL_TIME LocalTime, float fDelta );
	bool IsSummonPet() { return m_bSummonPet; }
	void HideSummonPet( bool bHide );
	void SetPetInfo( TVehicleCompact VehicleInfo ) { m_sPetCompact = VehicleInfo; }
	TVehicleCompact& GetPetInfo() { return m_sPetCompact; }
	int GetPetClassType();
	CDnPetActor* GetMyPetActor();
	bool IsInPetSummonableMap();
	bool IsPetSummonableMap() { return m_bPetSummonableMap; }
	void SetPetEffectAction( const char* szActionName );
	void AddPetNotEatItemList(DWORD dropItemUniqueID);
	virtual bool AttachSEEffect( const CDnSkill::SkillInfo& ParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nIndex, const EffectOutputInfo* pEffectInfo, 
								/*OUT*/ DnEtcHandle& hEtcObject, /*OUT*/ DNVector(DnEtcHandle)& vlhDummyBoneEtcObjects, bool bAllowSameSkillDuplicate = false );
	virtual bool DetachSEEffect( const CDnSkill::SkillInfo& ParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nIndex, const EffectOutputInfo* pEffectInfo );

	void ClearFishingRod();
#ifdef PRE_ADD_CASHFISHINGITEM
	void CreateFishingRod( int nFishingRodItemID );
#else // PRE_ADD_CASHFISHINGITEM
	void CreateFishingRod( const char* pszSkinName, const char* pszAniName, const char* pszActName );
#endif // PRE_ADD_CASHFISHINGITEM
	void ShowFishingRod( bool bShowFishingRod );
	void SetFishingRodAction( const char* pszActionName );
	void SetFishingReward( int nRetCode, int nRewardItemID );
	void ProcessFishing( LOCAL_TIME LocalTime, float fDelta );
	
#ifdef PRE_ADD_MONSTER_CATCH
	void SetCatcherMonsterActor( DnActorHandle hActor ) { m_hCatcherMonster = hActor; };
	void ReleaseCatcherMonsterActor( void ) { m_hCatcherMonster.Identity(); };
	virtual void OnReleaseFromMonster( DnActorHandle hCatcherMonsterActor );
#endif // #ifdef PRE_ADD_MONSTER_CATCH

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	virtual void RecomposeAction();
	virtual std::string GetRecomposeActionStr() { return std::string(""); }
#endif

#ifdef PRE_ADD_VIP
	virtual void SetVIP(bool bVIP)	{ m_bVIP = bVIP; }
	virtual bool IsVIP() const		{ return m_bVIP; }
	virtual bool IsAlwaysVIPHeadRender() const;
#endif
	
	void ProcessEffectSkill( LOCAL_TIME LocalTime, float fDelta );
	void SyncEffectSkill();
	void AddEffectSkill( int iItemID, int iSkillID, int iDurationTime, bool bUsedInGameServer, bool bEternity = false );
	void DelEffectSkill( int iItemID );
	void ClearEffectSkill( void );
	int GetNumEffectSkill( void ) { return (int)m_dqEffectSkill.size(); };
	const S_EFFECT_SKILL* GetEffectSkillFromIndex( int iIndex );

	virtual bool CanAddSkill( int nSkillTableID, int nLevel = 1 );


#ifdef PRE_ADD_NEWCOMEBACK		
	bool GetComebackLocalUser(){
		return m_bComebackLocalUser;
	}
#endif // PRE_ADD_NEWCOMEBACK	


private:
	void AttachDecal();

	virtual void PreThreadRelease();

	// from MASkillUser
	virtual void OnAddSkill( DnSkillHandle hSkill, bool isInitialize/* = false*/ );
	virtual void OnRemoveSkill( DnSkillHandle hSkill );
	virtual void OnReplacementSkill( DnSkillHandle hLegacySkill, DnSkillHandle hNewSkill );
	// 해당 스킬 객체에 맞는 강화 스킬을 갖고 있는지 체크해서 갖고 있다면 적용시켜 준다.
	void CheckAndApplyEnchantPassiveSkill( DnSkillHandle hBaseSkill );

public:
	virtual bool AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false, bool bUseCombine = true );
	virtual bool DetachCashParts( CDnParts::PartsTypeEnum Index );

#if defined(PRE_FIX_44486)
	bool CheckSkillLevelUp(int nSkillID);
#endif // PRE_FIX_44486


protected:
	SCPlayerGuildInfo m_PlayerGuildInfo;

public:
	SCPlayerGuildInfo& GetPlayerGuildInfo() { return m_PlayerGuildInfo; }
	void SetPlayerGuildInfo(SCPlayerGuildInfo& Info) 
	{ 
		_wcscpy(m_PlayerGuildInfo.wszGuildName, _countof(m_PlayerGuildInfo.wszGuildName), Info.wszGuildName, (int)wcslen(Info.wszGuildName));
		_wcscpy(m_PlayerGuildInfo.wszMasterName, _countof(m_PlayerGuildInfo.wszMasterName), Info.wszMasterName, (int)wcslen(Info.wszMasterName));
		
		m_PlayerGuildInfo.nMemberSize = Info.nMemberSize;
		m_PlayerGuildInfo.nGuildSize = Info.nGuildSize;
		m_PlayerGuildInfo.nGuildLevel = Info.nGuildLevel;

		memcpy( m_PlayerGuildInfo.GuildRewardItem, Info.GuildRewardItem, sizeof(m_PlayerGuildInfo.GuildRewardItem) );
	}

	virtual int GetPlayerGuildRewardEffectValue(int nType)
	{
		if (nType < 0 || nType >= GUILDREWARDEFFECT_TYPE_CNT )
			return 0;

		return m_PlayerGuildInfo.GuildRewardItem[nType].nEffectValue;
	}

#ifdef PRE_ADD_BESTFRIEND
	void SetAppellationName( WCHAR * bfName );
#endif

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
protected:
	EtTextureHandle m_hMailBoxOpenMarkTex;
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	bool PlayerSkillEffectCheck( SignalTypeEnum Type, void *pPtr );
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
