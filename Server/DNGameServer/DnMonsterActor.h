#pragma once

#include "DnActor.h"
#include "DnMonsterState.h"
#include "MAAiBase.h"
#include "MAAiReceiver.h"
#include "DnDropItem.h"
#include "PerfCheck.h"

class CDnMonsterActor : public CDnActor, public MAAiReceiver, public CDnMonsterState
{
public:
	CDnMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnMonsterActor();

	static int s_nPositionRevisionTime;
protected:
	SOBB m_GenerationArea;
	DNVector(CDnItem::DropItemStruct) m_VecDropItemList;
	int m_nMonsterClassID;
	TDUNGEONDIFFICULTY m_AIDifficult;
	float m_fScale;
	float m_fRevisionWeight;
	int m_nMonsterWeightTableID;
	
	bool m_bTimeMonster;		// 특정 시간이 지나면 사라지는 몬스터인가.?
	bool m_bNoDamage;
	LOCAL_TIME m_nDestroyTime;		// 얼마나 시간이 지난후 사라질지?
	
	ElementEnum m_eElementType;	// 몬스터는 플레이어와는 다르게 자체적으로 ElementType 을 갖고 있다.

#if defined( PRE_MOD_LOCK_TARGET_LOOK )
	int			m_nLockLookEventArea;
#endif	// #if defined( PRE_MOD_LOCK_TARGET_LOOK )
	bool		m_bLockLookTarget;
	char*		m_pszCanBumpActionName;
	
	bool		m_bCanBumpWall;		// 벽에 부딪혀서 특정 액션 수행할지 플래그
	LOCAL_TIME	m_LastSendMoveMsg;

	float m_fRotateResistance;

	int m_nPartyComboCount;
	int m_nPartyComboDelay;
	DnActorHandle m_hPartyHitActor;

	bool	m_bIsTriggerMonster;	// 트리거에 의해 생성된 몬스터
	int		m_iTriggerRandomSeed;	// 트리거에 의해 생성된 몬스터 RandomSeed 값
#ifdef PRE_MOD_DARKLAIR_RECONNECT
	int		m_nEventAreaUniqueID;	// 트리거에서 생성할 때의 EventAreaID
#endif // PRE_MOD_DARKLAIR_RECONNECT

	DnPropHandle m_hProp;
	UINT		m_uiForcePositionRevisionTick;
	UINT		m_uiPrevForcePositionRevisionTick;
	float		m_fForcePositionRevisionDelta;

	bool		m_bEnableDropItem;

	DnActorHandle m_hSummonerPlayerActor;
	bool m_bSuicideWhenSummonerDie;
	bool m_bFollowSummonerStage;
	DWORD m_dwSummonerActorID;
	int m_iSummonGroupID;
	float m_fLimitSummonerDistanceSQ;
	int m_nBirthAreaHandle;

	bool m_bSummoned;
	bool m_bReCreatedFollowStageMonster;

	std::map<int,int> m_mUseSkillCount;

#ifdef PRE_ADD_MONSTER_CATCH
	string m_strCatchBoneName;
	string m_strTargetActorCatchBoneName;
	string m_strCatchedActorAction;
	int m_iCatchedActorActionIndex;

	struct S_CATCH_ACTOR_INFO
	{
		DnActorHandle hCatchedActor;
		int iCatchCantMoveBlowID;
		int iCatchCantActionBlowID;

		S_CATCH_ACTOR_INFO( void ) : iCatchCantMoveBlowID( 0 ), iCatchCantActionBlowID( 0 ) {};
	};
	vector<S_CATCH_ACTOR_INFO> m_vlCatchedActors;

	set<DWORD> m_setCatchCheckedActorIDs;
#endif //#ifdef PRE_ADD_MONSTER_CATCH

	std::map<int,int> m_mOrderCount;

#if defined( PRE_FIX_MOVEBACK )
	bool	m_bNearMoveBack;				// 액션 실행시 처리
	bool	m_bPrevMoveBack;				// 강제로 Move_Back 실행
#endif

	int		m_nAutoRecallRange;
	bool m_bChangeAxisOnFinishAction;

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	int m_nSwapActorID;
#endif

protected:
	void GenerationDropItem();

	// Movement Message
	virtual void OnDrop( float fCurVelocity );
	virtual void OnStop( EtVector3 &vPosition );
	virtual void OnBeginNaviMode();

	void DropItems();

	virtual void ProcessDie( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor, HitStruct *pStruct );
	// 한번의 HitSignal 체크가 끝나고 불려진다. 때리는 놈 입장에서 콜
	virtual void OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct );

	void CalcMonsterWeightIndex();

	void CalcItemDropTableID();
	std::string GetAIFileName();

	bool CheckPositionRevision();
	bool CheckSignalPositionRevision( float fDelta );
	void ResetPositionRevision();
	void ProcessPositionRevision( float fDelta );
	void ProcessPartyCombo( LOCAL_TIME LocalTime, float fDelta );
	void OnPartyCombo( DnActorHandle hHitter, int nComboDelay );
	void Process_AutoRecallRange();

private:

public:

	void SendProjectile( CDnProjectile *pProjectile, ProjectileStruct* pStruct, MatrixEx& LocalCross, int iSignalIndex );
	
	// #15557 이슈 관련. 몬스터가 발사체에서 발사체를 쏘는 경우.
	// 기본적으로 클라이언트의 LocalPlayerActor 가 보내는 내용과 같다.
	// 클라이언트 측에선 파티원이 쏜 발사체와 역시 동일하게 패킷을 처리한다.
	// 따라서 반드시 CDnLocalPlayerActor::OnProjectile() 쪽도 같이 수정해야 함. 
	void SendProjectileFromProjectile( CDnProjectile* pProjectile, int nSignalIndex );
	
	void SetGenerationArea( SOBB &Box );
	SOBB *GetGenerationArea();
	void SetBirthAreaHandle( int nIndex ) { m_nBirthAreaHandle = nIndex; }
	int GetBirthAreaHandle() { return m_nBirthAreaHandle; }

	virtual void SetTeam( int nValue );
	void SetAIDifficult( TDUNGEONDIFFICULTY Level ) { m_AIDifficult = Level; }

	void SetPropHandle( DnPropHandle hProp ){ m_hProp = hProp; }
	void SetMonsterClassID( int nValue ) { m_nMonsterClassID = nValue; }
	int GetMonsterClassID() { return m_nMonsterClassID; }

	int GetMonsterWeightTableID() { return m_nMonsterWeightTableID; }
	float	GetThreatRange();
	float	GetCognizanceThreatRange();
	float	GetCognizanceThreatRangeSq();

	void EnableNoDamage( bool bEnable ) { m_bNoDamage = bEnable; }
	bool IsEnableNoDamage() { return m_bNoDamage; }

	void SetScale( float fValue );
	float GetScale() { return m_fScale; }
	virtual float GetWeight();

	virtual float	GetRotateAngleSpeed();
	virtual bool	IsMonsterActor(){ return true; }
	virtual bool    IsSummonedMonster(){return m_bSummoned;}
	virtual bool	IsCannonMonsterActor(){ return false; }

	DnPropHandle	GetPropHandle(){ return m_hProp; }

	void SetTimeMonster(bool bEnable, LOCAL_TIME nTime) { m_bTimeMonster = bEnable; m_nDestroyTime = nTime; }
	LOCAL_TIME GetRemainDestroyTime( void ) { return m_nDestroyTime; };
	
	bool	_bIsCheckVaildPosition( EtVector3& vTargetPos );
	void	_SendCmdMoveNavi( EtVector3& vTargetPos, float fMinDistance, const char* szActionName, int nLoopCount );
	int		GetUseSkillCount( int iSkillID );

	// Actor Command
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdMove( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdMoveNavi( EtVector3& vTargetPos, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f );
	virtual void CmdLook( EtVector2 &vVec, bool bForce = false );
	virtual void CmdLook( DnActorHandle hActor, bool bLock = true );
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, bool bFromStateBlow = false, bool bSkillChain = false );
	virtual int  CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit = false, bool bCheckCanBegin = true , bool bEternity = false );
	virtual void CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bRemoveFromServerToo = true );
	virtual void SendRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	virtual void CmdSuicide( bool bDropItem, bool bDropExp );
	virtual void CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, float fFrame, float fBlendFrame = 3.f );
	virtual void CmdWarp( EtVector3 &vPos, EtVector2 &vLook, CDNUserSession* pGameSession=NULL, bool bCheckPlayerFollowSummonedMonster=false );

	virtual void SyncClassTime( LOCAL_TIME LocalTime );
	virtual bool Initialize();
	virtual void ResetActor();
	virtual bool IsHittable( DnActorHandle hHitter, LOCAL_TIME LocalTime, HitStruct *pHitSignal = NULL, int iHitUniqueID = -1 );

	// Actor Message
	virtual void ProcessAI(LOCAL_TIME LocalTime, float fDelta);
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessLook( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );
	virtual void OnDie( DnActorHandle hHitter );
	virtual void OnBeginStateBlow( DnBlowHandle hBlow );
	
	virtual void ReTransmitSlaveMsg(CDNUserSession* pBreakIntoSession); // 동기맞추기위해 난입 유저에게 보내줄때 사용.
	virtual void SlaveOf( DWORD dwSummonerActorUniqueID, bool bSummoned = false, bool bSuicideWhenSummonerDie = false, bool bFollowSummonerStage = false, bool bReCreateFollowStageMonster = false );
	virtual void SlaveRelease( void );

	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );

	virtual void RequestDamageFromStateBlow( DnBlowHandle hFromBlow, int iDamage );

	// MASkillUser
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	virtual bool AddSkill( int nSkillTableID, int nLevel = 1, int iSkillLevelApplyType = CDnSkill::PVE );
	virtual void OnAddSkill( DnSkillHandle hSkill, bool isInitialize = false );
#endif // #ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta );
	virtual CDnSkill::UsingResult UseSkill( int nSkillTableID, bool bCheckValid = true, bool bAutoUseFromServer = false, int nLuaSkillIndex=-1 );

	// TransAction Message
	virtual void OnChangeAction( const char *szPrevAction );
	virtual void OnFinishAction(const char* szPrevAction, LOCAL_TIME time);
	virtual bool OnPreAiProcess(const char* szPrevAction, LOCAL_TIME time) { return false; }
	void _CheckActionWithProcessPassiveActionSkill( const char* szPrevAction );

	// AI
	virtual bool bIsAILook(){ return (m_pAi) ? m_pAi->bIsAILook() : false; }
	virtual bool bIsTurnOnAILook(){ return true; }
	virtual bool bIsProjectileTargetSignal(){ return m_pAi ? m_pAi->bIsProjectileTargetSignal() : false; }
	virtual bool OnAINonTarget();
	virtual void OnBumpWall();

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	// 플레이어가 소환하는 렐릭 몬스터 타입(CDnClericRelicMonster)이면 true
	virtual bool IsClericRelicMonster( void ) { return false; };

	// 로그관련 임시코드 090226
	//void QueryLogStage(int nRoomID, int nMapIndex);

	ElementEnum GetElementType( void ) { return m_eElementType; };
	void SetMonsterElementType( ElementEnum eType ) { m_eElementType = eType; }

	// AI
	void	SetNaviDestination( SOBB* pOBB, UINT uiMoveFrontRate );
	int		GetWaitOrderCount( int iSkillID );
	void	AddWaitOrderCount( int iSkillID );
	void	DelWaitOrderCount( int iSkillID );

	// Trigger
	void	SetTriggerMonster( const int iRandomSeed ){ m_bIsTriggerMonster = true; m_iTriggerRandomSeed = iRandomSeed; }
	bool	bIsTriggerMonster(){ return m_bIsTriggerMonster; }
	int		GetTriggerRandomSeed(){ return m_iTriggerRandomSeed; }
#ifdef PRE_MOD_DARKLAIR_RECONNECT
	void	SetEventAreaUniqueID( int nEventAreaUniqueID ) { m_nEventAreaUniqueID = nEventAreaUniqueID; }
	int		GetEventAreaUniqueID() { return m_nEventAreaUniqueID; }
#endif // PRE_MOD_DARKLAIR_RECONNECT

	void EnableDropItem( bool bEnable ) { m_bEnableDropItem = bEnable; }

	// 소환 몬스터에게 능력치 복사할 때 따로 백업했다가 복구해줄 때만 쓰임.
	void SetRevisionWeight( float fRevisionWeight ) { m_fRevisionWeight = fRevisionWeight; };
	DnActorHandle GetSummonerPlayerActor( void ) { return m_hSummonerPlayerActor; };

	// 몬스터 어그로 리셋 (#21673)
	void ResetAggro( void );
	void ResetAggro( DnActorHandle hActor );

#ifdef PRE_ADD_MONSTER_CATCH
	void ProcessCatchActor( LOCAL_TIME LocalTime, float fDelta );
	void CatchActor( CatchActorStruct* pCatchActor, int nSignalIndex );
	void _CatchThisActor( DnActorHandle hResultActor, int nSignalIndex );
	void ReleaseAllActor( void );

	void ReleaseThisActor( S_CATCH_ACTOR_INFO& CatchActorInfo );
	void ReleaseThisActor( DnActorHandle hActor );
#endif // #ifdef PRE_ADD_MONSTER_CATCH

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	virtual void SwapActor( int nActorIndex ) {}
#endif

	bool IsFollowSummonerStage( void ) { return m_bFollowSummonerStage; };
	void SetLimitSummonerDistance( float fLimitSummonerDistance ) { m_fLimitSummonerDistanceSQ = fLimitSummonerDistance*fLimitSummonerDistance; };
	void SetSummonGroupID( int iSummonGroupID ) { m_iSummonGroupID = iSummonGroupID; };
	int GetSummonGroupID( void ) { return m_iSummonGroupID; };

	void SetAutoRecallRange( int nAutoRecallRange ) { m_nAutoRecallRange = nAutoRecallRange; }

protected:
	LOCAL_TIME m_RequestSummonTime;	//Summon몬스터 생성 시간
public:
	void SetRequestSummonTime(LOCAL_TIME localTime) { m_RequestSummonTime = localTime; }
	LOCAL_TIME GetRequestSummonTime() { return m_RequestSummonTime; }

#if defined( PRE_FIX_MOVEBACK )
	void SetPrevMoveBack( bool bMoveBack ) { m_bPrevMoveBack = bMoveBack; }
	void SetNearMoveBack() { m_bNearMoveBack = m_bPrevMoveBack; }
	bool IsNearMoveBack() { return m_bNearMoveBack; }
	bool IsPrevMoveBack() { return m_bPrevMoveBack; }
#endif

#if defined(PRE_FIX_51048)
protected:
	struct PassiveStateEffectInfo
	{
		const CDnSkill::SkillInfo* pParentSkill;
		STATE_BLOW::emBLOW_INDEX emBlowIndex;
		int nDurationTime;
		std::string szParam;

		PassiveStateEffectInfo()
		{
			pParentSkill = NULL;
			emBlowIndex = STATE_BLOW::BLOW_NONE;
			nDurationTime = 0;
		}
	};

	std::list<PassiveStateEffectInfo> m_PassiveStateEffectInfoList;
	//몬스터 생성시점에 이 플래그가 켜저 있으면 리스트에 담았다가 SendGameGenerationMonsterMsg 호출후에 ApplyPassiveStateEffect함수 호출로 상태효과 적용 시킨다.
	//플래그가 설정 되어 있지 않으면 기존과 동일하게..
	bool m_EnablePassiveStateEffect;
public:
	//패시브 상태효과를 리스트에 담아서 나중에 처리 할지 여부를 설정한다.
	void SetEnablePassiveStateEffectList(bool bSet) { m_EnablePassiveStateEffect = bSet; }
	bool GetEnablePassiveStateEffectList() { return m_EnablePassiveStateEffect; }

	void AddPassiveStateEffectInfo(PassiveStateEffectInfo& stateEffectInfo);	//ApplyPassiveSkill에서
	void InitPassiveStateEffectInfo();											//리스트 초기화
	void ApplyPassiveStateEffect();												//패시브 스킬의 상태효과 적용.

	virtual bool ApplyPassiveSkill( DnSkillHandle hSkill, bool isInitialize = false );
#endif // PRE_FIX_51048

#if defined(PRE_FIX_61382)
	// 상태효과 측에서 데미지를 가하는 경우. 우선 데미지 반사에서만 쓰임. 데미지 값은 양수로 받는다. #23104
	virtual void RequestDamageFromStateBlow( DnBlowHandle hFromBlow, int iDamage, CDnDamageBase::SHitParam* pHitParam = NULL );
#endif // PRE_FIX_61382

protected:
	bool m_isPuppetSummonMonster;
public:
	void SetPuppetSummonMonster(bool bFlag) { m_isPuppetSummonMonster = bFlag; }
	bool IsPuppetSummonMonster() { return m_isPuppetSummonMonster; }

#if defined(PRE_FIX_64312)
public:
	void SendApplySummonMonsterExSkill(int nBaseSkillID, int nLevel, int nSelectedType, DWORD dwMasterUniqueID, int nMasterExSkillID);
#endif // PRE_FIX_64312
};

template <class RenderType, class DamageType>
class TDnMonsterActor : public CDnMonsterActor, virtual public RenderType, virtual public DamageType
{
public:
	TDnMonsterActor( CMultiRoom *pRoom, int nClassID ) : CDnMonsterActor( pRoom, nClassID )
	{
	}
	virtual ~TDnMonsterActor() {}

protected:

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {
		CDnMonsterActor::Process( LocalTime, fDelta );

		RenderType::PreProcess( LocalTime, fDelta );

		// #39347 PreProcess 호출되고 나서 AniDistance 값이 다시 생김.
		if( 0 < GetCantXZMoveSEReferenceCount() )
			m_vAniDistance.x = m_vAniDistance.z = 0.0f;

		m_pMovement->Process( LocalTime, fDelta );
		RenderType::Process( m_Cross, LocalTime, fDelta );
	}

	virtual void Look( EtVector2 &vVec, bool bForceRotate=true )
	{
		MAMovementInterface::Look( vVec, bForceRotate );

		if( bForceRotate )
		{
			if( strstr( m_hActor->GetCurrentAction(), "Turn" ) )
			{
				CmdAction( "Stand" );
			}
		}
	}
};

class IBoostPoolDnMonsterActor : public CDnMonsterActor, public TBoostMemoryPool< IBoostPoolDnMonsterActor >
{
public:
	IBoostPoolDnMonsterActor( CMultiRoom *pRoom, int nClassID ):CDnMonsterActor( pRoom, nClassID ){}
	virtual ~IBoostPoolDnMonsterActor(){}
};
