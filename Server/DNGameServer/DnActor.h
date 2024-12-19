#pragma once

#include "DnUnknownRenderObject.h"
#include "EtMatrixEx.h"
#include "Timer.h"
#include "EtOctree.h"
#include "DnActionBase.h"
#include "DnActorState.h"
#include "MATransAction.h"
#include "MAActorRenderBase.h"
#include "MADamageBase.h"
#include "MASkillUser.h"
#include "DnBlowDef.h"
#include "MultiRoom.h"
#include "MAMovementInterface.hpp"
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
#include "FrameSync.h"
#endif

class CDnStateBlow;
class CDnProjectile;
class CDNAggroSystem;

class CDnStateBlowSignalProcessor;

class CDnActor : public CDnUnknownRenderObject< CDnActor, MAX_SESSION_COUNT >, 
				 public CDnActionBase, 
				 virtual public CDnActorState,
				 public MASkillUser, 
				 public MATransAction,
				 virtual public MADamageBase,
				 public MAMovementInterface,
				 virtual public MAActorRenderBase
{
public:

	enum eStateBlowAfterProcessType
	{
		eRebirth	= 1,
		eDelZombie	= (1<<1),
	};

	CDnActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnActor();

	friend class CDnActorActionStateCache;

	enum HitCheckTypeEnum {
		BoundingBox,
		Collision,
	};

	STATIC_DECL( DWORD s_dwUniqueCount );
	static float s_fDieDelta;

protected:

	STATIC_DECL( CEtOctree<DnActorHandle> *s_pOctree );
	typedef std::map<DWORD, DnActorHandle> mapActorSearch;
	STATIC_DECL( mapActorSearch s_dwMapActorSearch );

	int m_nClassID;
	DWORD m_dwUniqueID;
	MatrixEx m_Cross;
	EtVector3 m_vStaticPosition;
	SHitParam m_HitParam;
	DnWeaponHandle m_hWeapon[2];
	bool m_bSelfDeleteWeapon[2];

	CDNAggroSystem*	m_pAggroSystem;
	CEtOctreeNode<DnActorHandle> *m_pCurrentNode;

	bool m_bToggleIngnoreHit;

	int m_nTeam;
	float m_fStiffDelta;
	float m_fDownDelta;
	float m_fDieDelta;
	float m_fMaxDieDelta;
	float m_fLastDownRatio;

#ifdef PRE_ADD_GRAVITY_PROPERTY
	float m_fGravityEnd;
#endif // PRE_ADD_GRAVITY_PROPERTY

	map<DWORD, LOCAL_TIME> m_mapLastDamageTime;
	int m_nLastDamageHitterActionIndex;

	LOCAL_TIME m_LastHitSignalTime;
	
	// 마찬가지로 발사체에서 사용하는 마지막 hit 시그널의 유니크 아이디를 피격자별로 갖고 있는다. 
	// 여러명이 동시에 타격하면 OnDamage 함수에서 -1로 리셋되어 버린다.
	map<DWORD, int> m_mapLastHitUniqueID;
	int m_nLastHitSignalIndex;
	int m_nAirComboCount;

	bool m_bEnableNormalSuperAmmor;
	int m_nNormalSuperAmmorTime;
	float m_fBreakNormalSuperAmmorDurability;

	int m_nLastUpdateSkillSuperAmmorValue[4];
	int m_nSkillSuperAmmorValue[4];
	int m_nSkillSuperAmmorTime;
	float m_fSkillSuperAmmorDamageDecreaseProb;

	float m_fAddHeight;
	HitCheckTypeEnum m_HitCheckType;

	bool m_bModifyPlaySpeed;
	LOCAL_TIME m_PlaySpeedStartTime;
	DWORD m_dwPlaySpeedTime;
	float m_fPlaySpeed;
	bool m_bShow;
	bool m_bOctreeUpdate;
	DWORD	m_dwGenTick;

	EtVector3 m_vPrevPosition;

	struct PreStateStruct {
		int nState;
		int nOffset;

		PreStateStruct( int State, int Offset ) {
			nState = State;
			nOffset = Offset;
		};
	};

	std::vector<std::vector<PreStateStruct>> m_VecPreActionState;
	std::vector<std::vector<PreStateStruct>> m_VecPreActionCustomState;
	std::vector<std::vector<PreStateStruct>> m_VecPreCanMoveState;
	
	DNVector(DnActorHandle) m_hVecLastHitListByRemainTime;
	DNVector(DnActorHandle) m_hVecLastHitList;

	CDnStateBlow *m_pStateBlow;

	// 노말 슈퍼 아머를 무시할 것인지
	bool m_bIngnoreNormalSuperArmor;
	
	// 시그널로 액션 중간에 스킬 사용 가능 체크를 처리했는지
	bool m_bUseSignalSkillCheck;
	bool m_abSignalSkillCheck[ 3 ];	// 0:Movable 1:Jumpable 2:GroundMovable

	// 한기 2009.7.27 
	// 자신에게 사용하는 효과 적용 시간 관련 (#1911)
	// 서버로 패킷을 보내지 않고 서버/클라 각자 적용되는 상태효과 큐.
	// 스킬 액션에 ApplyStateEffect 시그널이 따로 있을 경우 이 큐에 들어가서 
	// 액션툴에 정의된 타이밍에 적용됨.
	struct S_NO_PACKET_SELF_STATEBLOW
	{
		CDnSkill::SkillInfo ParentSkillInfo;
		CDnSkill::StateEffectStruct StateEffect;
		bool bUsed;

		S_NO_PACKET_SELF_STATEBLOW( const CDnSkill::SkillInfo& SkillInfo, CDnSkill::StateEffectStruct SE ) : 
		ParentSkillInfo( SkillInfo ), StateEffect(SE), bUsed( false )
		{

		};

		void Used( void ) { bUsed = true; };
	};
	deque<S_NO_PACKET_SELF_STATEBLOW> m_dqApplySelfStateBlowQ;
	//
	

	// Signal Rotate.
	LOCAL_TIME m_dwRotateStartTime;
	LOCAL_TIME m_dwRotateTime;	
	bool m_bRotate;
	bool m_bRotLeft;	  // 회전방향.	
	float m_fSpeedRot;    // 회전속도.
	float m_fStartSpeed;  // 시작속도.
	float m_fEndSpeed;    // 종료속도.
	EtVector3 m_vRotAxis; // 회전축좌표.
		

	INT64 m_iLastDamage;
	int m_iLastProcessPressCount;

	//Die 호출에 필요한 최종 Hit액터 저장.
	DnActorHandle m_hCompleteKillActor;
	bool m_bCompleteKill_AfterProcessStateBlow;

#ifdef _SHADOW_TEST
	bool m_bIsShadowActor;
	bool m_bEnableShadowActor;
#endif
	std::set<std::string> m_setUseActionName;

	int m_iCantMoveReferenceCount;
	int m_iCantActionReferenceCount;
	int m_iCantUseSkillReferenceCount;
	int m_iCantAttackReferenceCount;

	int m_iCantXZMoveSE;

	// 차져 상태효과로 적의 위치에서 시그널을 돌리고 있을 경우.
	bool m_bOnSignalFromChargerSE;
	DnActorHandle m_hChargetDestActor;
	DnBlowHandle m_hChargerBlowCalledOnSignal;

	// 차져 상태효과 등에서 나간 발사체에서 맞아서 OnDamage 호출될 때는 콤보 카운트 계산을 하지 않기 위한 플래그
	bool m_bAllowCalcCombo;

	std::list<DnMonsterActorHandle>	m_listSummonMonster;

	// #32426 소환체 컨트롤 기능 추가 관련. 
	// STE_Summon 으로 소환된 몬스터에 그룹 ID 가 정의 되어있다면 m_listSummonMonster 로 추가되지 않고, 이쪽으로 분류되어 관리된다.
	// 같은 그룹의 소환체에 대해서 최대 갯수 이상 소환되지 않는다.
	std::map<int, std::list<DnMonsterActorHandle> > m_mapSummonMonsterByGroup;

	int m_iFrameStopRefCount;

	vector<DnBlowHandle> m_vlReservedRemoveBlows;
	std::vector<int> m_vTriggerBuff;

	// #33312 강제로 셋팅한 속성. #36689 여러군데에서 사용될 수 있으므로 스택 개념으로 처리한다.
	DNVector(int) m_vlForceSettedHitElement;

#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	STATIC_DECL( CFrameSkip s_MonsterProcess );
	STATIC_DECL( float s_fMonsterProcessDelta );

	STATIC_DECL( CFrameSkip s_NpcProcess );
	STATIC_DECL( float s_fNpcProcessDelta );
#endif
public:

	DWORD	GetGenTick(){ return m_dwGenTick; }
	bool bIsCanSummonMonster( const SummonMonsterStruct* pSummonMonsterStruct );
	virtual void PushSummonMonster( DnMonsterActorHandle hMonster, const SummonMonsterStruct* pSummonMonsterStruct, bool bReCreateFollowStageMonster = false );

	// Note: bOnPlayerInit 은 처음에 스테이지가 로딩되면서 캐릭터 생성되고 초기화 될 시에 패시브 스킬 추가할 때 호출되었다느 뜻
	//		 AddStateBlow 함수 내부에서 IsDie() 체크하는데, 스테이지 로딩될 시에 캐릭터 초기화 될 때
	//		 HP가 0 이라서 패시브 스킬 상태효과 추가 안되고 넘어가므로 이 플래그로 구분한다
	int AddStateBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex, const CDnSkill::SkillInfo* pParentSkill, int nDurationTime, 
					  const char *szParam, bool bOnPlayerInit = false, bool bCheckCanBegin = true , bool bEternity = false );

	void DelStateBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	DnBlowHandle GetStateBlowFromID( int nStateBlowID );
	int RemoveStateBlowByBlowDefineIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	void RemoveStateBlowFromID( int nStateBlowID );
	void RemoveAllBlow();
	void RemoveAllBlowExpectPassiveSkill();
	bool IsShouldRemainBlowWhenDie( DnBlowHandle hBlow );
	int GetNumAppliedStateBlow( void );
	DnBlowHandle GetAppliedStateBlow( int iIndex );
	void GetAllAppliedStateBlow(DNVector(DnBlowHandle)& /*IN OUT*/ out);
	void GatherAppliedStateBlowByBlowIndex( STATE_BLOW::emBLOW_INDEX BlowIndex, /*IN OUT*/ DNVector(DnBlowHandle)& out );
	bool IsAppliedThisStateBlow( STATE_BLOW::emBLOW_INDEX iBlowIndex );
	CDnStateBlow *GetStateBlow() { return m_pStateBlow; }

	void GetAllAppliedStateBlowBySkillID(int nSkillID, DNVector(DnBlowHandle)& /*IN OUT*/ out);

	virtual void OnBeginStateBlow( DnBlowHandle hBlow ) {}
	virtual void OnEndStateBlow( DnBlowHandle hBlow );// {}

	void SetOnSignalFromChargerSE( bool bOnSignalFromChargerSE ) { m_bOnSignalFromChargerSE = bOnSignalFromChargerSE; };
	bool IsOnSignalFromChargerSE( void ) { return m_bOnSignalFromChargerSE; };
	void SetChargerDestActor( DnActorHandle hChargerDestActor ) { m_hChargetDestActor = hChargerDestActor; };
	void SetChargerBlow( DnBlowHandle hBlow ) { m_hChargerBlowCalledOnSignal = hBlow; };

	// Hit 체크가 되는 놈들이 있을때마다 불려진다. 때리는 놈 입장에서 콜
	virtual void OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor, HitStruct *pStruct ) {}
	// 한번의 HitSignal 체크가 끝나고 불려진다. 때리는 놈 입장에서 콜
	virtual void OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct );
	// 발사체가 hit 되었을 때 발사체를 쏜 액터 객체로 호출된다. 
	virtual void OnHitProjectile( LOCAL_TIME LocalTime, DnActorHandle hHittedTarget, const CDnDamageBase::SHitParam& HitParam ) {};
	// 액션 체크
	virtual bool IsLimitAction( const char* pszActionName ){ return false; }
	virtual bool IsLimitAction( DnSkillHandle hSkill ){ return false; }

	void Die( DnActorHandle hHitter );
	void RequestKillAfterProcessStateBlow(DnActorHandle hHitter);
	void ExecuteKillAfterProcessStateBlow();

	virtual void OnStateBlowProcessAfter(){}
	virtual void OnAddStateBlowProcessAfterType( eStateBlowAfterProcessType Type ){}
	virtual void OnDie( DnActorHandle hHitter );
	virtual void OnBreakSkillSuperAmmor( int nIndex, int nOriginalSupperAmmor, int nDescreaseSupperAmmor ) {} // 깨졋을때 때린놈
	virtual void OnBreakSkillSuperAmmor( SHitParam &HitParam ) {}	// 꺠졋을때 깨진놈
	virtual void OnAirCombo( int nComboCount ) {}

	virtual int	 OnGetJobClassID(){ return 0; }
	void OnRequestHPMPDelta( /*IN OUT*/ INT64& nHPMPDelta, const DWORD dwHitterUniqueID, bool bIsMPDelta = false );
#ifdef PRE_FIX_77172
	void RequestHPMPDelta( ElementEnum eElement, INT64 nHPMPDelta, const DWORD dwHitterUniqueID, bool bIsMPDelta = false, 
							bool bShowValue = true, CDnDamageBase::SHitParam* pHitParam = NULL, bool bKeepJumpMovement = false );
#else // PRE_FIX_77172
	void RequestHPMPDelta( ElementEnum eElement, INT64 nHPMPDelta, const DWORD dwHitterUniqueID, bool bIsMPDelta = false, bool bShowValue = true, CDnDamageBase::SHitParam* pHitParam = NULL );
#endif // PRE_FIX_77172
	void RequestSEProbSuccess( int iSkillID, STATE_BLOW::emBLOW_INDEX eBlowIndex );
	void RequestCooltimeReset( int iSkillID );
#ifdef PRE_ADD_GRAVITY_PROPERTY
	void SetGravityEnd( float fGravityEnd ) { m_fGravityEnd = fGravityEnd; }
#endif // PRE_ADD_GRAVITY_PROPERTY

protected:

	void RequestDamage( CDnDamageBase *pHitter, int nSeed, INT64 nDamage );

	virtual void ProcessState( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessDown( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessDie( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessStiff( LOCAL_TIME LocalTime, float fDelta );
	void ProcessPlaySpeed( LOCAL_TIME LocalTime, float fDelta );
	void ProcessRotate( LOCAL_TIME LocalTime, float fDelta ); // Rotate.

	virtual void ProcessPress();
	void ProcessPress( DnActorHandle hTarget );
	bool CheckPressCircle2Clrcle2( DnActorHandle hActor1, DnActorHandle hActor2, EtVector2 &Dir, float &Dist );		// 2차원원-2차원원
	bool CheckPressCapsule2Capsule2( DnActorHandle hActor1, DnActorHandle hActor2, EtVector2 &Dir, float &Dist );	// 2차원캡슐-2차원캡슐
	bool CheckPressCapsule2Circle2( DnActorHandle hActor1, DnActorHandle hActor2, EtVector2 &Dir, float &Dist );	// 2차원캡슐-2차원원

	void LinkWeapon( int nEquipIndex );

	static bool Compare_PreStateOrder( CDnActor::PreStateStruct &a, CDnActor::PreStateStruct &b );
	virtual const char *CheckChangeActionBlow( const char *szActionName );

	void ApplyStateEffectSignalProcess( const S_NO_PACKET_SELF_STATEBLOW &StateBlowInfo, ApplyStateEffectStruct* pStruct );

	// Signal Rotate.
	void SetRotate( DWORD dwTime, float fStart, float fEnd, EtVector3 & vPos, bool bLeft );

public:

	// MASkillUser
	virtual void OnSkillProjectile( CDnProjectile *pProjectile );

public:
	// Local
	int GetClassID() { return m_nClassID; }

	// Movement Message
	virtual void OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash );
	__forceinline MatrixEx *GetMatEx() { return &m_Cross; }

	__forceinline void SetPosition( const EtVector3 &vPos ) { m_Cross.m_vPosition = vPos; }
	__forceinline EtVector3 *GetPosition() { return &m_Cross.m_vPosition; }

	__forceinline void SetStaticPosition( const EtVector3 &vPos ) { m_vStaticPosition = vPos; }
	__forceinline EtVector3 *GetStaticPosition() { return &m_vStaticPosition; }

	__forceinline void SetPrevPosition( EtVector3 &vPos ) { m_vPrevPosition = vPos; }
	__forceinline EtVector3 *GetPrevPosition() { return &m_vPrevPosition; }

	DWORD GetUniqueID() { return m_dwUniqueID; }
	virtual void SetUniqueID( DWORD dwUniqueID );

	CDNAggroSystem* GetAggroSystem() const { return m_pAggroSystem; }

	virtual void SetTeam( int nValue ) { m_nTeam = nValue; }
	int GetTeam() { return m_nTeam; }

	float GetAddHeight() { return m_fAddHeight; }
	void SetAddHeight( float fValue ) { m_fAddHeight = fValue; }

	void SetDownRemainDelta( float fDownDelta ) { m_fDownDelta = fDownDelta; };
	float GetDownRemainDelta() { return m_fDownDelta; }

	LOCAL_TIME GetDamageRemainTime( DWORD dwActorUniqueID );
	void SetDamageRemainTime( DWORD dwActorUniqueID, LOCAL_TIME LastDamageTime, int nCurrentActionIndex = -1 );
	void SetLastDamageHitUniqueID( DWORD dwActorUniqueID, int iLastHitUniqueID );
	int GetLastDamageHitUniqueID( DWORD dwActorUniqueID );

	void ResetDamageRemainTime();
	void SetStiffDelta( float fStiffDelta ) { m_fStiffDelta = fStiffDelta; };
	float GetStiffDelta() { return m_fStiffDelta; };

	bool IsModifyPlaySpeed() { return m_bModifyPlaySpeed; }
	void SetPlaySpeed( DWORD dwFrame, float fSpeed );
	void ResetPlaySpeed( void );
	void SetDieDelta(float fDelta) { m_fDieDelta = fDelta; }
	void SetMaxDieDelta(float fDelta) { m_fMaxDieDelta = fDelta; }
	DWORD GetPlaySpeedFrame() { return m_dwPlaySpeedTime; }

	void SetOctreeUpdate( bool bFlag ){ m_bOctreeUpdate = bFlag; }

	// 노말 슈퍼아머를 무시할 것인지 true 면 무시한다.
	void SetIgnoreNormalSuperArmor(bool bIgnore) { m_bIngnoreNormalSuperArmor = bIgnore; }
	bool GetIgnoreNormalSuperArmor() { return m_bIngnoreNormalSuperArmor; }

	void InsertOctreeNode();
	void RemoveOctreeNode();

	SHitParam *GetHitParam() { return &m_HitParam; }
	HitCheckTypeEnum GetHitCheckType() { return m_HitCheckType; }

	virtual void ResetActor();

	virtual bool Initialize() { return true; }
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessAI( LOCAL_TIME LocalTime, float fDelta ){}
	virtual void ProcessLook( LOCAL_TIME LocalTime, float fDelta ){};
	virtual bool SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bCheck = true, bool bCheckStateEffect = true );
	virtual void SetCustomAction( const char *szActionName, float fFrame );

	virtual void SyncClassTime( LOCAL_TIME LocalTime );

	// Octree 를 위한..
	virtual void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false );
	virtual void GetBoundingBox( SAABox &Box );

	bool IsShow() { return m_bShow; }
	void Show( bool bShow );

	// ActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual bool LoadAction( const char *szFullPathName );
	virtual void FreeAction();
	void OnLoopAction( float fFrame, float fPrevFrame );

	// Actor Command
	virtual void CmdRefreshHPSP( INT64 nHP, int nSP );
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, bool bFromStateBlow = false, bool bSkillChain = false );
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f ) {}
	virtual void CmdMove( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f ) {}
	virtual void CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f ) {}
	virtual void CmdMoveNavi( EtVector3& vTargetPos, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f ) {}
	virtual void CmdMove( EtVector2 &vDir, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f ) {}
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f ) {}
	virtual void CmdLook( EtVector2 &vVec, bool bForce = false ) {}
	virtual void CmdLook( DnActorHandle hActor, bool bLock = true ) {}

	// Note: bOnPlayerInit 은 처음에 스테이지가 로딩되면서 캐릭터 생성되고 초기화 될 시에 패시브 스킬 추가할 때 호출되었다느 뜻
	//		 AddStateBlow 함수 내부에서 IsDie() 체크하는데, 스테이지 로딩될 시에 캐릭터 초기화 될 때
	//		 HP가 0 이라서 패시브 스킬 상태효과 추가 안되고 넘어가므로 이 플래그로 구분한다
	// [2011/03/18 semozz]
	//		접두어 스킬에서 추가되는 상태 효과 생성과 동시에 OnBegin 적용 여부를 패킷으로 처리 해야 할듯..
	virtual int CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit = false, bool bCheckCanBegin = true , bool bEternity = false );
	virtual void CmdModifyStateEffect( int iBlowID, STATE_BLOW& StateBlowInfo );

	virtual void CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bRemoveFromServerToo = true );
	virtual void SendRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex ) {};
	virtual void CmdRemoveStateEffectFromID( int nID );
	virtual void CmdRemoveStateEffectImmediately( STATE_BLOW::emBLOW_INDEX emBlowIndex );

	void SendRemoveStateEffectFromID( int nID );
	void SendAddSEFail( int iAddSEResult, STATE_BLOW::emBLOW_INDEX emBlowIndex );
	
	virtual void CmdSuicide( bool bDropItem, bool bDropExp ) {}
	virtual void CmdWarp();
	virtual void CmdWarp( EtVector3 &vPos, EtVector2 &vLook, CDNUserSession* pGameSession=NULL, bool bCheckPlayerFollowSummonedMonster=false );
	virtual void CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, float fFrame, float fBlendFrame = 3.f ) { }
	virtual void CmdChangeTeam( int nTeam );
	virtual void CmdForceVelocity( EtVector3 &vVelocity, EtVector3 &vResistance );
	virtual void CmdChatBalloon( LPCWSTR wszMessage );
#if defined(PRE_ADD_MULTILANGUAGE)
	virtual void CmdChatBalloon( int nUIStringIndex );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)		
	virtual void CmdShowExposureInfo( bool bShow );
	virtual void CmdForceAddSkill( int nSkillID );
	virtual void CmdForceRemoveSkill( int nSkillID );

	// Actor Message
	void SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop = false );
	virtual bool CheckDamageAction( DnActorHandle hActor );
	virtual void CheckDamageVelocity( DnActorHandle hActor );
	virtual float PreCalcDamage( CDnDamageBase *pHitter, SHitParam &HitParam, const float fDefenseRate=1.f, float fStateEffectAttackM = -1.0f );
	virtual float CalcDamage( CDnDamageBase *pHitter, SHitParam &HitParam );

	// CDnDamageBase
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );

	// Weapon
	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachWeapon( int nEquipIndex = 0 );
	virtual DnWeaponHandle GetWeapon( int nEquipIndex = 0 ) { return m_hWeapon[nEquipIndex]; }
	void ShowWeapon( int nEquipIndex, bool bShow );

	virtual DnWeaponHandle GetActiveWeapon( int nEquipIndex ) { return m_hWeapon[nEquipIndex]; }

	// Projectile
	virtual void OnProjectile( CDnProjectile *pProjectile, ProjectileStruct* pStruct, MatrixEx& LocalCross, int nSignalIndex );

	// Additional Packet Message
	// Damage Packet
	virtual void OnDamageWriteAdditionalPacket( CPacketCompressStream *pStream ) {}
	virtual void OnDamageReadAdditionalPacket( CPacketCompressStream *pStream ) {}

	// AI
	virtual bool bIsAILook(){ return false; }

	// 
	virtual bool IsGMTrace() const { return false; }

	// Static 함수들
	static bool InitializeClass( CMultiRoom *pRoom );
	static void ProcessClass( CMultiRoom *pRoom, LOCAL_TIME LocalTime, float fDelta );
	static void ProcessAIClass( CMultiRoom *pRoom, LOCAL_TIME LocalTime, float fDelta );
	static void ReleaseClass( CMultiRoom *pRoom );
	static DnActorHandle FindActorFromUniqueID( CMultiRoom *pRoom, DWORD dwUniqueID );
	static DnActorHandle FindActorFromName( CMultiRoom *pRoom, TCHAR *szName );
	static void InsertUniqueSearchMap( CMultiRoom *pRoom, CDnActor *pActor );
	static void RemoveUniqueSearchMap( CMultiRoom *pRoom, CDnActor *pActor );
	static void InitializeNextStage( CMultiRoom* pRoom  );
	
	// Range Scan 함수들
	static int ScanActor( CMultiRoom *pRoom, EtVector3 &vPos, float fRadius, DNVector(DnActorHandle) &VecList, bool bInside = false );
	static void GetActorList(CMultiRoom *pRoom, DNVector(DnActorHandle) &VecList);
	static void GetOpponentActorList(CMultiRoom *pRoom, int nTeam, DNVector(DnActorHandle) &VecList, EtVector3* pPos=NULL, float fMaxRange=0.f );

	static int ScanActorByActorSize( CMultiRoom *pRoom, EtVector3 &vPos, float fRadius, DNVector(DnActorHandle) &VecList, bool bInside = false, bool bActorSize = false );

	// 게임서버는 속도를 위해 = NULL 로 안하고 함수 자체를 나눕니다.
	__inline int GetState() { return CDnActorState::GetState(); }
	int GetState( const char *szActionName );
	int GetState( int nElementIndex );

	bool IsStay( const char *szActionName );
	bool IsMove( const char *szActionName );
	bool IsAttack( const char *szActionName );
	bool IsHit( const char *szActionName );
	bool IsAir( const char *szActionName );
	bool IsDown( const char *szActionName );
	bool IsStun( const char *szActionName );
	bool IsStiff( const char *szActionName );
	bool IsFly( const char *szActionName );
	bool IsGround( const char *szActionName );
	bool IsStandHit( const char *szActionName );

	__inline bool IsStay() { return ( CDnActorState::GetState() & ActorStateEnum::Stay ) ? true : false; }
	__inline bool IsMove() { return ( CDnActorState::GetState() & ActorStateEnum::Move ) ? true : false; }
	__inline bool IsAttack() { return ( CDnActorState::GetState() & ActorStateEnum::Attack ) ? true : false; }
	__inline bool IsHit() { return ( CDnActorState::GetState() & ActorStateEnum::Hit ) ? true : false; }
	__inline bool IsAir() { return ( CDnActorState::GetState() & ActorStateEnum::Air ) ? true : false; }
	__inline bool IsDown() { return ( CDnActorState::GetState() & ActorStateEnum::Down ) ? true : false; }
	__inline bool IsStun() { return ( CDnActorState::GetState() & ActorStateEnum::Stun ) ? true : false; }
	__inline bool IsStiff() { return ( CDnActorState::GetState() & ActorStateEnum::Stiff ) ? true : false; }
	__inline bool IsFly() { return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_Fly ) ? true : false; }
	__inline bool IsFly2() { return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_Fly2 ) ? true : false; }
	__inline bool IsGround() { return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_Ground ) ? true : false; }
	__inline bool IsUnderGround() { return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_UnderGround ) ? true : false; }
	__inline bool IsStandHit() {
		int nState = CDnActorState::GetState();;
		if( nState == ActorStateEnum::Hit || nState == ( ActorStateEnum::Hit | ActorStateEnum::Stiff ) ) return true;
		return false;
	}

	virtual bool IsHittable( DnActorHandle hHitter, LOCAL_TIME LocalTime, HitStruct *pHitSignal = NULL, int iHitUniqueID = -1 );
	virtual bool IsHittableSkill( int iSkillIndex , bool &bSendResist );

	virtual void ProcessCollision( EtVector3 &vMove );

	virtual bool	IsPlayerActor() { return false; }
	virtual bool	IsNpcActor(){ return false; }
	virtual bool	IsMonsterActor(){ return false; }
	virtual bool	IsPartsMonsterActor(){ return false; }
	virtual bool    IsVehicleActor() { return false; }
	virtual UINT	GetSessionID(){ return 0; }
	virtual int GetSwapSingleSkinActorID() { return -1; }
	virtual int GetMonsterMutationTableID(){ return -1; }

	virtual bool bIsDestroyParts( const UINT uiTableID ){ return false; }
	virtual bool bIsDestroySetParts( const UINT uiSetID ){ return false; }
	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }

	virtual void OnInitializeNextStage( void );
	void OnInitializePVPRoundRestart( void );

	void AddedCantMoveSE( void ) { ++m_iCantMoveReferenceCount; };
	void RemovedCantMoveSE( void ) { if( m_iCantMoveReferenceCount > 0 ) --m_iCantMoveReferenceCount; };
	int GetCantMoveSEReferenceCount( void ) { return m_iCantMoveReferenceCount; };
	void AddedCantActionSE( void ) { ++m_iCantActionReferenceCount; };
	void RemovedCantActionSE( void ) { if( m_iCantActionReferenceCount > 0 ) --m_iCantActionReferenceCount; };
	int GetCantActionSEReferenceCount( void ) { return m_iCantActionReferenceCount; };
	void AddedCantUseSkillSE( void ) { ++m_iCantUseSkillReferenceCount; };
	void RemovedCantUseSkillSE( void ) { if( m_iCantUseSkillReferenceCount > 0 ) --m_iCantUseSkillReferenceCount; };
	int GetCantUseSkillSEReferenceCount( void ) { return m_iCantUseSkillReferenceCount; };
	void AddedCantAttackSE( void ) { ++m_iCantAttackReferenceCount; };
	void RemovedCantAttackSE( void ) { if( m_iCantAttackReferenceCount > 0 ) --m_iCantAttackReferenceCount; };
	int GetCantAttackSEReferenceCount( void ) { return m_iCantAttackReferenceCount; };
	void AddedCantXZMoveSE( void ) { ++m_iCantXZMoveSE; };
	void RemovedCantXZMoveSE( void ) { if( m_iCantXZMoveSE ) --m_iCantXZMoveSE; };
	int GetCantXZMoveSEReferenceCount( void ) { return m_iCantXZMoveSE; };

	void ResetStateEffect( int nSkillDurationType );
	void CmdSyncBlow( CDNUserSession* pGameSession );

	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return GetMySmartPtr(); };

	bool IsUseSignalSkillCheck( void ) { return m_bUseSignalSkillCheck; };
	bool IsAllowedSkillCheck( int nCheckType )
	{
		if( 0 <= nCheckType && nCheckType < 3 )
			return m_abSignalSkillCheck[ nCheckType ];
		else
			return false;
	}
	void SetSignalSkillCheck( int nCheckType, bool bCheck )
	{ 
		if( 0 <= nCheckType && nCheckType < 3 )
			m_abSignalSkillCheck[ nCheckType ] = bCheck;
	}

	void ResetProcessSkill()
	{
		if( IsProcessSkill() )
		{
			if( m_hProcessSkill )
			{
				m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
				m_hProcessSkill.Identity();
			}
		}
	}
	
	bool bIsObserver();
	static float SquaredDistance( const EtVector3& vPos, const SAABox& BBox, bool bNear = true );

	// 액션이 바뀌는 시점을 받아와서 스킬 종료시킴. 스킬 중간에 다른 액션 나갈 수 있게 되었으므로
	// 스킬을 종료시켜서 상태효과 적용 종료 시점을 정확하게 맞춘다.
	//virtual void OnChangeAction( const char *szPrevAction );

	ElementEnum CalcHitElementType( bool bUseSkillApplyWeaponElement, CDnState::ElementEnum eSkillElement = CDnState::ElementEnum_Amount, bool bUsingSkill = false );

	// 한기 2009.7.27 
	// 자신에게 사용하는 효과 적용 시간 관련 (#1911)
	void AddStateEffectQueue( const CDnSkill::SkillInfo& ParentSkillInfo, const CDnSkill::StateEffectStruct& StateEffectInfo );
	void ClearSelfStateSignalBlowQueue( bool bItemSkill = false );
	// 

#ifdef _SHADOW_TEST
	void EnableShaodowActor( bool bValue ) { m_bEnableShadowActor = bValue; }
	bool IsShadowActor() { return m_bIsShadowActor; }
	bool IsShadowActor() const { return m_bIsShadowActor; }
#endif

	void ResetSkillSuperAmmor( void ) { ZeroMemory( m_nSkillSuperAmmorValue, sizeof(m_nSkillSuperAmmorValue) ); };
	void SetSkillSuperAmmor( int iValue ) { m_nSkillSuperAmmorValue[ 0 ] = iValue; };

	INT64 GetLastDamage( void ) { return m_iLastDamage; };

	// 상태효과에서 MP 쓰는 순간을 체크하기 위해... 현재 서버에서만 사용한다.
	// MP 소모를 줄이거나 늘이는 상태효과에서 사용하는 부분임. 
	// 이 함수를 통해서 소모된 MP 는 서버에서 패킷이 따로 오므로 클라에서 할 일은 없다!
	void UseMP( int iMPDelta );

	// 소환된 액터라면 주인을 셋팅함.
	// 몬스터 액터에서 오버라이드해서 클라로 패킷을 쏴줌. 
	virtual void SlaveOf( DWORD dwSummonerActorUniqueID, bool bSummoned = false, bool bSuicideWhenSummonerDie = false, bool bFollowSummonerStage = false, bool bReCreateFollowStageMonster = false ) {};

	void SetAllowCombo( bool bAllowCombo ) { m_bAllowCalcCombo = bAllowCombo; };
	bool IsAllowCombo( void ) { return m_bAllowCalcCombo; };

	void ToggleIgnoreHit(){ m_bToggleIngnoreHit ^= 1; }
	bool bIsIgnoreHit(){ return m_bToggleIngnoreHit; }
	
	void AddedFrameStop( void );
	void RemovedFrameStop( void );
	int GetFrameStopRefCount( void ) { return m_iFrameStopRefCount; };
	void UpdateFPS( void );

	void AddReserveRemoveBlow( DnBlowHandle hBlow ) { m_vlReservedRemoveBlows.push_back( hBlow ); };
	void ProcessReservedRemoveBlows( void );

	// 상태효과 측에서 데미지를 가하는 경우. 우선 데미지 반사에서만 쓰임. 데미지 값은 양수로 받는다. #23104
	virtual void RequestDamageFromStateBlow( DnBlowHandle hFromBlow, int iDamage, CDnDamageBase::SHitParam* pHitParam = NULL );

	// 트리거로 추가된 버프 추가, 삭제
	void ClearTriggerBuff();
	void AddTriggerBuff( int nID )	{ m_vTriggerBuff.push_back(nID); }

	void CmdFreezingPrisonDurablity(int nStateBlowID, float fDuabilityRate, bool bShowGauge);

	virtual void RemoveResetStateBlow();

	//////////////////////////////////////////////////////////////////////////
	// 때리는 녀석 넉백 시키기위한 함수 [2011/01/14 semozz]
	void ForceKnockBack(DnActorHandle hActor, CDnDamageBase::SHitParam& HitParam);
	//////////////////////////////////////////////////////////////////////////

	//해당 맵에서 허용되지 않은 아이템에 의해서 적용된 상태효과를 제거한다.
	void RemoveNonAvailableStateBlow();

	// 활성화된 상태효과 제거.
	void RemovedActivatedStateBlow( bool bIgnoreItemSkill = false );


//#if defined(스킬쿨타임비율)
	void FindSkillBySkillType(CDnSkill::SkillTypeEnum eSkillType, DNVector(DnSkillHandle)& vlSkillList);
//#endif // 스킬쿨타임비율

	// 현재 엘리멘탈로드의 아이시 프랙션에서만 예외적으로 2가지의 상태효과 필터링을 걸고 있는데
	// 둘 중에 하나만 hit 되길 원하기 때문에 먼저 체크된 것은 다음 필터링에 걸리지 않도록 한다. #28747
	// CDnPlayerActor 에서 상속 받음.
	virtual void OnHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex ) {};
	virtual bool CheckHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex ) { return true; };

	virtual bool IsMyRelicMonster( DnActorHandle hActor ) { return false; };

#ifdef PRE_ADD_MONSTER_CATCH
	bool CatchCalcSuperArmor( DnActorHandle hCatcherActor, int iSuperArmorDamage );
#endif // #ifdef PRE_ADD_MONSTER_CATCH


	void CmdUpdateStateBlow(int nBlowID);
	void CmdFinishAuraSkill(DWORD nSkillID);

	bool IsImmuned(STATE_BLOW::emBLOW_INDEX blowIndex);

	//접두어 상태효과 강제 OnBegin호출을 위한 함수
	void ForceBeginStateBlow(DnBlowHandle hBlow);

protected:
	CDnStateBlowSignalProcessor* m_pStateBlowSignalProcessor;

	bool CanUsePrefixSkill();
public:
	void AddStateBlowSignal(DnBlowHandle hBlow);
	void RemoveStateBlowSignal(DnBlowHandle hBlow);

	virtual bool CheckCollisionHitCondition(const EtVector3& vObjPos, const MatrixEx& objCross, EtVector3 &vTargetPos, float angleAllow);

public:
	struct HitLimitCountInfo
	{
		int nHitLimitCount;
		int nHitCount;

		HitLimitCountInfo(int nLimitCount, int hitCount)
		{
			nHitCount = hitCount;
			nHitLimitCount = nLimitCount;
		}
	};
	typedef std::map<int, HitLimitCountInfo> HIT_LIMIT_COUNT_INFO_LIST;
	HIT_LIMIT_COUNT_INFO_LIST m_HitLimitCountInfoList;

protected:
	struct ActorHandleComp
	{
		bool operator()(const DnActorHandle& h1, const DnActorHandle& h2) const
		{
			return (h1.GetHandle() < h2.GetHandle());
		}
	};

	struct StateBlowInfo
	{
		STATE_BLOW::emBLOW_INDEX blowIndex;
		std::string strValue;
	};

	typedef std::list<StateBlowInfo> STATE_BLOW_INFO_LIST;
	struct StealMagicBuffInfo
	{
		CDnSkill::SkillInfo skillInfo;	//스킬 정보
		int nDurationTime;				//상태효과 지속 시간

		STATE_BLOW_INFO_LIST stateBlowList;
	};
	
	typedef std::map<DnActorHandle, StealMagicBuffInfo, ActorHandleComp> STEAL_MAGIC_BUFF_INFO_LIST;	//스틸 매직으로 추가할 상태효과 후보 리스트
	typedef std::map<int, std::list<int>> STEAL_MAGIC_BUFF_LIST;			//스틸 매직에의해 적용된 상태효과 리스트

	STEAL_MAGIC_BUFF_INFO_LIST m_StealMagicBuffAddList;	//스틸 매직에서 추가할 후보 리스트
	STEAL_MAGIC_BUFF_LIST m_StealMagicBuffList;	//실제 적용된 리스트

	std::map<int, int> m_RemovedStealMagicStateBlowIDList;	//OnEndStateBlow에서 제거되는 상태효과가 스틸 매직에 의한 녀석일때 담아 놓는 리스트
public:
	void ReserveStealMagicBuff(DnActorHandle hActor, CDnSkill::SkillInfo *pSkillInfo, int nDurationTime, STATE_BLOW::emBLOW_INDEX blowIndex, const char* szValue);	//스틸 매직으로 추가할 상태효과 정보를 예약한다..
	void UpdateStealMagicBuff();					//예약된 <스킬, 상태효과 리스트> 중 하나를 선택해서 실제 액터에 상태효과 적용
	
protected:
	void AddStealMagicStateBlow(int nSkillID, int nBlowID);	
	void RemoveStealMagicStateBlow(int nDeletedBlowID);

	LOCAL_TIME CheckRemainFrameTime();


protected:
	int m_nOriginalTeam;
public:
	void SetOriginalTeam(int nTeam) { m_nOriginalTeam = nTeam; }
	int GetOriginalTeam() { return m_nOriginalTeam; }

	void PushForceHitElement( ElementEnum eElement ) { m_vlForceSettedHitElement.push_back(eElement); };
	void PopForceHitElement( void ) { m_vlForceSettedHitElement.pop_back(); };

	const std::list<DnMonsterActorHandle>& GetSummonedMonsterList( void );
	const std::map<int, std::list<DnMonsterActorHandle> >& GetGroupingSummonedMonsterList( void );

	DnMonsterActorHandle FindOldSummonMonster(const SummonMonsterStruct* pSummonMonsterStruct);

	void RequestActionChange(int nActionIndex);

	void SendChainAttackProjectile(DnActorHandle hRootAttacker, DWORD dwPrevAttackerActorUniqueID, int iActionIndex, int iProjectileSignalArrayIndex, DnActorHandle hActorToAttack, int iSkillID);

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	virtual bool IsAllowCallSkillProcess( float fDelta ) { return false; };
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

	void RemoveStateEffectByHitSignal(HitStruct* pHitStruct);

	virtual void UpdateMissionByMonsterKillCount(int nCount) {};

	bool ProcessIgnoreHitSignal();
	bool ProcessIgnoreGravitySignal();

//상태효과 CDnApplyStateBlowOnTimeBlow에 의해서 스킬의 상태효과 추가
protected:
	std::vector<DnSkillHandle> m_ApplySkillStateEffectList;
public:
	void AddSkillStateEffect(DnSkillHandle hSkill);
	void ApplySkillStateEffect();

	void SendProbInvincibleSuccess( void );

protected:
	std::string m_FindAutoTargetName;
public:
	void SetFindAutoTargetName(const char* szBipName) { m_FindAutoTargetName = szBipName ? szBipName : ""; }
	std::string& GetFindAutoTargetName() { return m_FindAutoTargetName; }

	//실제 캐릭터의 위치가 이동 되는 몬스터가 아닌경우 "Bip01"의 위치를 반환하기 위한 함수
	//그린드래곤에서 가이디드샷 문제점. 일단 다른 액터들은 기존의 액터 위치를 반환 한다.
	EtVector3 FindAutoTargetPos();


//////////////////////////////////////////////////////////////////////////
// 무기 변경시 장착된 무기의 SelfDelete정보 얻기위한 함수.
public:
	bool GetWeaponSelfDelete(int nIndex) { return m_bSelfDeleteWeapon[nIndex]; }
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//무기 변경 상태효과에 사용될 Attach/Detach Weapon Lock 함수
protected:
	bool m_bChangeWeaponLocked;
	bool m_bSkipChangeWeaponAction;	//무기 교체 액션 스킵..
	bool m_bSkipOnAttatchDetachWeapon;	//무기 교체후 OnAttachWeapon/OnDetachWeapon함수 호출 스킵 설정
public:
	void SetChangeWeaponLock(bool bLock) { m_bChangeWeaponLocked = bLock; }
	bool IsChangeWeaponLock() { return m_bChangeWeaponLocked; }
	void SetSkipChangeWeaponAction(bool bSkip) { m_bSkipChangeWeaponAction = bSkip; }
	bool IsSkipChangeWeaponAction() { return m_bSkipChangeWeaponAction; }
	void SetSkipOnAttatchDetachWeapon(bool bSkip) { m_bSkipOnAttatchDetachWeapon = bSkip; }
	bool IsSkipOnAttatchDetachWeapon() { return m_bSkipOnAttatchDetachWeapon; }
//////////////////////////////////////////////////////////////////////////

#if defined(PRE_ADD_50917)
protected:
	int m_ChangeWeaponRefCount;

	DnWeaponHandle m_hOrigWeaponWhenChangeWeapon;		//무기 해제 시점의 원래 무기 정보
	bool m_bOrigWeaponWhenChangeWeaponSelfDelete;		//무기 해제 시점의 원래 SelfDelete정보

public:
	void AddChangeWeaponRefCount() { ++m_ChangeWeaponRefCount; }
	void RemoveChangeWeaponRefCount() { --m_ChangeWeaponRefCount; }
	int GetChangeWeaponRefCount() { return m_ChangeWeaponRefCount; }

	void SetOrigWeaponWhenChangeWeapon(DnWeaponHandle hWeapon, bool bSelfDelete) { m_hOrigWeaponWhenDisarmament = hWeapon, m_bOrigWeaponWhenDisarmamentSelfDelete = bSelfDelete; }
	DnWeaponHandle GetOrigWeaponWhenChangeWeapon() { return m_hOrigWeaponWhenDisarmament; }
	bool GetOrigWeaponWhenChangeWeaponSelfDelete() { return m_bOrigWeaponWhenDisarmamentSelfDelete; }
#endif // PRE_ADD_50917

#if defined(PRE_ADD_50907)
protected:
	int m_DisarmamentRefCount;

	DnWeaponHandle m_hOrigWeaponWhenDisarmament;		//무기 해제 시점의 원래 무기 정보
	bool m_bOrigWeaponWhenDisarmamentSelfDelete;		//무기 해제 시점의 원래 SelfDelete정보

public:
	void AddDisarmamentRefCount() { ++m_DisarmamentRefCount; }
	void RemoveDisarmamentRefCount() { --m_DisarmamentRefCount; }
	int GetDisarmamentRefCount() { return m_DisarmamentRefCount; }

	void SetOrigWeaponWhenDisarmament(DnWeaponHandle hWeapon, bool bSelfDelete) { m_hOrigWeaponWhenDisarmament = hWeapon, m_bOrigWeaponWhenDisarmamentSelfDelete = bSelfDelete; }
	DnWeaponHandle GetOrigWeaponWhenDisarmament() { return m_hOrigWeaponWhenDisarmament; }
	bool GetOrigWeaponWhenDisarmamentSelfDelete() { return m_bOrigWeaponWhenDisarmamentSelfDelete; }
#endif // PRE_ADD_50907

#if defined(PRE_FIX_50482)
protected:
	int m_nChangeTeamRefCount;
public:
	void AddChangeTeamRefCount() { ++m_nChangeTeamRefCount; }
	void RemoveChangeTeamRefCount() { --m_nChangeTeamRefCount; }
	int GetChangeTeamRefCount() { return m_nChangeTeamRefCount; }
#endif // PRE_FIX_50482

#if defined(PRE_FIX_51048)
	void RemoveDebufAction(LOCAL_TIME LocalTime, float fDelta);
#endif // PRE_FIX_51048

	virtual void SetState(ActorStateEnum State);
	virtual void OnAttackChange();

	void ScanActorByStateIndex(DNVector(DnActorHandle) &Veclist, STATE_BLOW::emBLOW_INDEX blowIndex);

#if defined(PRE_FIX_55378)
	//차저 상태효과의 SignalProcess에서 Player액터인 경우 프로젝타일 생성은 서버/클라이언트가 각자 생성 하기때문에 상관 없으나
	//몬스터 액터인 경우 클라이언트에서는 발사체 생성을 하지 않고, 서버에서 패킷을 받아 발사체를 생성하게 되느데,
	//이 시점에서 받는 ActionIndex정보가 일지 하지 않아서 클라이언트쪽에서 발사체 생성이 되지 않는다.
	//차저 상태효과의 SignalProcess에서 DnActor::OnSignal함수 호출 전에 차저 액션을 설정 해 놓고, 끝날때 리셋 시켜 놓는다.
	//발사체 패킷 전송에서 ChargerAction이 설정 되어 있으면 m_nActionIndex 대신 ChargerAction의 액션Index로 바꾸어 패킷 전송한다.
protected:
	std::string m_ChargerAction;

public:
	void SetChargerAction(const char* szActionName) { m_ChargerAction = szActionName ? szActionName : ""; }
	std::string& GetChargerAction() { return m_ChargerAction; }
#endif // PRE_FIX_55378

	//#53454 꼭두각시 Hit시그널 처리시 꼭두각시를 소환한 액터와 꼭두각시 2액터가 Hit시그널 처리가 되어
	//데미지가 2배 적용이 되는데, 같은 시그널 처리시에 Hit액터 리스트에서 꼭두각시/꼭두각시 주인 액터를 조사 해서, 
	//꼭두각시 주인 액터는 Hit시그널 처리에서 제외 시킨다.
public:
	static int ExceptionHitList(DNVector(DnActorHandle) &hVecList, MatrixEx& Cross, DnActorHandle hHiterActor, HitStruct* pStruct);
	static int ExceptionHitList2(DNVector(DnActorHandle) &hVecList, MatrixEx& Cross, DnActorHandle hHiterActor, HitStruct* pStruct, 
								 DNVector(DnActorHandle)& hAddStateEffectActorList, 
								 int checkType, float fScanLength, EtVector3& vPrePos);
	static void CheckHitAreaActorList(DNVector(DnActorHandle)& hVecList, MatrixEx& Cross, DnActorHandle hHiterActor, HitStruct* pStruct, int checkType, float fScanLength, EtVector3& vPrePos);

#if defined(PRE_FIX_59347)
protected:
	bool m_bApplyPartsDamage;

public:
	void SetApplyPartsDamage(bool bEnable) { m_bApplyPartsDamage = bEnable; }
	bool GetApplyPartsDamage() { return m_bApplyPartsDamage; }

	virtual void ApplyPartsDamage(float& fDamage, DnActorHandle hHitterActor) {}
#endif // PRE_FIX_59347

#if defined(PRE_FIX_61382)
	//꼭두각시 몬스터일 경우 주인 액터 반환.
	static DnActorHandle GetOwnerActorHandle(DnActorHandle hActor);
#endif // PRE_FIX_61382

#if defined(PRE_ADD_65808)
protected:
	//몬스터에 스킬 문장 여러개 가능..
	std::map<int, std::list<int>> m_SummonMonsterGlyphInfo;
	//몬스터, 스킬문장, 상태효과..
	struct _StateEffectInfo
	{
		int nStateEffectID;
		DWORD dwUniqueID;
	};
	std::map<int, std::map<int, std::list<_StateEffectInfo>>> m_SummonMonsterGlyphStateEffectIDs;

public:
	void AddSummonMonsterGlyphInfo(int monsterID, int glyphID);
	void AddSummonMonsterGlyphStateEffectID(int monsterID, int glyphID, DWORD dwMonsterUniqueID, int stateEffectID);

	//스킬 문장이 제거 될때 호출..
	void RemoveSummonMonsterGlyphInfo(int monsterID, int glyphID);
	void RemoveSummonMonsterStateEffectByGlyph(DnMonsterActorHandle hMonsterActor, int monsterID, int glyphID);

	//몬스터가 죽은 경우 이펙트 ID등록 해놓은거 전부 제거..
	void RemoveSummonMonsterGlyphStateEffects(int monsterID);

	bool GetSummonMonsterGlyphInfo(int monsterID, std::list<int>& glyphIDs);

	//monsterID가 같은 소환 몬스터들을 찾는다.
	void FindSummonMonster(int monsterID, std::list<DnMonsterActorHandle>& monsterActorList);
#endif // PRE_ADD_65808


#if defined(PRE_FIX_68898)
public:
	void SetSkipEndAction(bool isSkipEndAction);
#endif // PRE_FIX_68898
};

class CDnActorActionStateCache:public CSingleton<CDnActorActionStateCache>
{
public:

	void LoadAction( CDnActor* pActor, const char* szFullPathName, std::vector<CEtActionBase::ActionElementStruct*>* pVecActionElementList,	\
					 std::vector<std::vector<CDnActor::PreStateStruct>>& ActionState,														\
					 std::vector<std::vector<CDnActor::PreStateStruct>>& ActionCustomState,													\
					 std::vector<std::vector<CDnActor::PreStateStruct>>& CanMoveState );

private:

	typedef struct tagData
	{
		std::vector<std::vector<CDnActor::PreStateStruct>> VecPreActionState;
		std::vector<std::vector<CDnActor::PreStateStruct>> VecPreActionCustomState;
		std::vector<std::vector<CDnActor::PreStateStruct>> VecPreCanMoveState;
	}SData;

	CSyncLock m_Lock;
	std::map<std::string,SData>	m_mRepository;
};
