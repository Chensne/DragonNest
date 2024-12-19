#pragma once

#include "DnActor.h"
#include "DnMonsterState.h"
#include "MAAiBase.h"
#include "MAAiReceiver.h"
#include "DnDropItem.h"

class CDnMonsterActor : public CDnActor, public MAAiReceiver, public CDnMonsterState
{
public:
	CDnMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnMonsterActor();

	virtual bool	IsMonsterActor(){ return true; }

	enum AIDifficult {
		Easy = 0,
		Normal,
		Hard,
		VeryHard,
		Nightmare,
	};

protected:
	SOBB m_GenerationArea;
	std::vector<CDnItem::DropItemStruct> m_VecDropItemList;
	int m_nMonsterClassID;
	bool m_bBoss;
	float m_fScale;
	float m_fRevisionWeight;
	int m_nMonsterWeightTableID;
	AIDifficult m_AIDifficult;

	float m_fRotateResistance;
	DnBlowHandle m_hAllowBlowEffect;

	bool m_bLockTarget;
#if defined( PRE_MOD_LOCK_TARGET_LOOK )
	int m_nLockLookEventArea;
#endif	// #if defined( PRE_MOD_LOCK_TARGET_LOOK )
	char*		m_pszCanBumpActionName;
	
	DWORD m_dwSummonerActorID;		// 이 몬스터를 소환하거나 대포인 경우 대포를 잡고 있는 플레이어 액터의 유니크 아이디
	DnActorHandle m_hSummonerPlayerActor;
	bool m_bFollowSummonerStage;

	DnPropHandle m_hParentProp;
	bool m_bAlwaysShowExposureInfo;

	bool	m_bSignalBallon;

#ifdef PRE_ADD_MONSTER_CATCH
	// 몬스터가 플레이어 잡은 상태.
	string m_strCatchBoneName;
	string m_strTargetActorCatchBoneName;
	string m_strCatchedActorAction;
	
	//DnActorHandle m_hCatchedActor;
	struct S_CATCH_ACTOR_INFO
	{
		DnActorHandle hCatchedActor;
	};
	vector<S_CATCH_ACTOR_INFO> m_vlCatchedActors;

	LOCAL_TIME m_CatchSignalTimeStamp;
	LOCAL_TIME m_ReleaseSignalTimeStamp;

	EtVector3 m_vPrevCatchPos;
#endif // #ifdef PRE_ADD_MONSTER_CATCH

#if defined( PRE_FIX_MOVEBACK )
	bool m_bPrevMoveBack;
	bool m_bNearMoveBack;	
#endif

	bool m_bChangeAxisOnFinishAction;

#ifdef PRE_ADD_CHANGE_MONSTER_SKIN
	bool m_bChangeSkin;
#endif

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	int m_nSwapActorID;
#endif

#ifdef PRE_ADD_MONSTER_CHEAT
	int m_nSkillTableID;
	std::string m_szAiFileName;
#endif 
	
protected:
	void CalcMonsterWeightIndex();

	void GenerationDropItem();

	// Movement Message
	virtual void OnDrop( float fCurVelocity );
	virtual void OnStop( EtVector3 &vPosition );
	virtual void OnBeginNaviMode();
	virtual void OnMoveNavi( EtVector3 &vPosition );

	virtual void OnRestorePlaySpeed( float fPrevFPS );

	void DropItems();
	bool IsBalloon( DialogueStruct * pStruct );
	
private:

	void	_CreateProjectile( BYTE* pPacket );
	void	_CreateProjectileFromProjectile( BYTE* pPacket );

public:
	virtual void RenderCustom( float fElapsedTime );
	void SetGenerationArea( SOBB &Box );
	SOBB *GetGenerationArea();

	void SetMonsterClassID( int nValue ) { m_nMonsterClassID = nValue; }
	int GetMonsterClassID();
	int GetMonsterWeightTableID() { return m_nMonsterWeightTableID; }
	void SetAIDifficult( AIDifficult Level ) { m_AIDifficult = Level; }

//	void SetBoss( bool bValue ) { m_bBoss = bValue; }
//	bool IsBoss() { return m_bBoss; }

	void SetScale( float fValue );
	float GetScale() { return m_fScale; }
	virtual float GetWeight();

	virtual float GetRotateAngleSpeed();
	
	// Actor Command
	virtual void SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop = false );
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdMove( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdMoveNavi( EtVector3& vTargetPos, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bResetStop = false, bool bForce = false );
	virtual void CmdLook( EtVector2 &vVec, bool bForce = false );
	virtual void CmdLook( DnActorHandle hActor, bool bLock = true );
	//virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, bool bFromStateBlow = false );
	virtual void CmdSuicide( bool bDropItem, bool bDropExp );
	virtual void CmdShowExposureInfo( bool bShow );

	virtual void SyncClassTime( LOCAL_TIME LocalTime );
	virtual bool Initialize();

	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );

	bool IsAlwaysShowExposureInfo() { return m_bAlwaysShowExposureInfo; }
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessLook( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
	virtual void OnDie( DnActorHandle hHitter );
	virtual DWORD GetSummonerUniqueID( void ) { return m_dwSummonerActorID; };

	// ActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnChangeAction( const char* szPrevAction );
	virtual void OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time);
	virtual bool SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bCheck = true, bool bCheckStateEffect = true );

	// MAActorRenderBase
	virtual void PreProcess( LOCAL_TIME LocalTime, float fDelta );

	// TransAction Message
	virtual void OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket );

	// AI
	virtual bool bIsAILook(){ return (m_pAi) ? m_pAi->bIsAILook() : false; }
	virtual bool bIsProjectileTargetSignal(){ return m_pAi ? m_pAi->bIsProjectileTargetSignal() : false; }
	virtual void OnBumpWall();

	virtual void PreThreadRelease();

	virtual void OnSkillUse( int iSkillID, int iLevel ) {};

	void SetPropHandle( DnPropHandle hProp ) { m_hParentProp = hProp; }
	DnPropHandle GetPropHandle( void ) { return m_hParentProp; };

	DnActorHandle GetSummonerPlayerActor( void ) { return m_hSummonerPlayerActor; };

	virtual void OnCreateProjectileBegin( /*IN OUT*/ MatrixEx& LocalCross ) {};
	virtual void OnCreateProjectileEnd( void ) {};

	bool IsFollowSummonerStage( void ) { return m_bFollowSummonerStage; };

#ifdef PRE_ADD_MONSTER_CATCH
	void CatchActorFromServer( BYTE* pPacket );
	void CatchActorFailedFromServer( BYTE* pPacket );
	//void CatchActorFromSignal( CatchActorStruct* pCatchActor, int nSignalIndex );
	void CatchActor( int nCatchActionIndex, int nCatchSignalArrayIndex, DWORD dwCatchedActorID );
	void ReleaseActorFromServer( BYTE* pPacket );
	void ReleaseThisActor( DnActorHandle hCatchedActor, bool bEraseInVector );
	void ProcessCatchActor( LOCAL_TIME LocalTime, float fDelta );
#endif // #ifdef PRE_ADD_MONSTER_CATCH

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	virtual void SwapActor( int nActorIndex ) {}
#endif

#if defined( PRE_FIX_MOVEBACK )
	void SetPrevMoveBack( bool bMoveBack ) { m_bPrevMoveBack = bMoveBack; }
	void SetNearMoveBack() { m_bNearMoveBack = m_bPrevMoveBack; }
	bool IsNearMoveBack() { return m_bNearMoveBack; }
#endif

#if defined(PRE_FIX_64312)
	void ApplySummonMonsterExSkill(BYTE* pPacket);
#endif // PRE_FIX_64312

protected:
	bool m_isPuppetSummonMonster;
public:
	void SetPuppetSummonMonster(bool bFlag) { m_isPuppetSummonMonster = bFlag; }
	bool IsPuppetSummonMonster() { return m_isPuppetSummonMonster; }

#ifdef PRE_ADD_CHANGE_MONSTER_SKIN
	void SwapSkin( EtAniObjectHandle hSwapObject );
#endif 

#ifdef PRE_ADD_MONSTER_CHEAT
	public:
	void SetSkillTableID( const int nTableID ) { m_nSkillTableID = nTableID; }
	int GetSkillTableID() { return m_nSkillTableID; }
	void SetAIFileName( std::string szFileName ) { m_szAiFileName = szFileName; }
	std::string GetAIFildName() const { return m_szAiFileName; }
#endif 
};

template <class RenderType, class DamageType>
class TDnMonsterActor : public CDnMonsterActor, virtual public RenderType, virtual public DamageType
{
public:
	TDnMonsterActor( int nClassID, bool bProcess = true ) : CDnMonsterActor( nClassID, bProcess ) 
	{
	}
	virtual ~TDnMonsterActor() {}

protected:

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {
		CDnMonsterActor::Process( LocalTime, fDelta );
		// 원래 RenderType::PreProcess( Process ) 가 맞는 구조이지만
		// 불가피하게 PreProcess 이후에 뭔가를 해줘야 하는것들이 생겨서 이케 한다.
		// 나중에 뭔가 맞지않는다 싶으면 걍 버쳘함수 하나 추가해서 PreProcess 와 MoveType::Process 사이에 껴넣어주고
		// 호출하는 식으로 바꿔줘도 된다.
		// ( 원래 Method Class 들의 virtual 함수들을 상속받지 않게 하려 했으므로.. )
		((MAActorRenderBase*)this)->PreProcess( LocalTime, fDelta );

		// #39347 PreProcess 호출되고 나서 AniDistance 값이 다시 생김.
		if( 0 < GetCantXZMoveSEReferenceCount() )
			m_vAniDistance.x = m_vAniDistance.z = 0.0f;

		m_pMovement->Process( LocalTime, fDelta );
		((MAActorRenderBase*)this)->Process( m_matexWorld, LocalTime, fDelta );
	}

	virtual void Look( EtVector2& vVec, bool bForceRotate=true )
	{
		MAMovementInterface::Look( vVec, bForceRotate );

		if( bForceRotate )
		{
			if( strstr( m_hActor->GetCurrentAction(), "Turn" ) )
				CmdStop( "Stand" );
		}		
	}
};

