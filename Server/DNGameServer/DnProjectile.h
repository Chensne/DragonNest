#pragma once

#include "DnUnknownRenderObject.h"
#include "EtMatrixEx.h"
#include "DnActionBase.h"
#include "DnActor.h"
#include "Timer.h"
#include "DnWeapon.h"
#include "DnCalcCatmullrom.h"
#include "DnSkill.h"

class CDnProjectile;
class CDnWeapon;
class IDnOrbitProcessor;

class CDnProjectile : public CDnWeapon, public TBoostMemoryPool<CDnProjectile>
{
public:
	CDnProjectile( CMultiRoom *pRoom, DnActorHandle hActor, bool bProcess = true, bool bIncreaseUniqueID = true );
	virtual ~CDnProjectile();

	// 궤적 타입
	enum OrbitTypeEnum {
		Linear,        // 등속도 SetSpeed
		Acceleration,  // 가속도 SetSpeed, SetResistance
		Homing,		   // 유도 SetSpeed
		TerrainLinear, // 바닥 타고 등속도 SetSpeed
		Projectile,    // 포물선 SetSpeed
		TerrainHoming, // 바닥 타고 등속도 호밍 SetSpeed
		OrbitTypeCount,
	};

	// 소멸시 타입
	enum DestroyOrbitTypeEnum {
		Instantly,	       // 즉시 사라짐
		FallGravity,       // 중력값 받으며 떨어짐
		RangeFallGravity,  // 기본적으로는 FallGravity 와 같으나 사거리 다 되면 바로 Destroy. (클라이언트에서는 Destroy 액션 실행)
	};

	// 발사체가 대미지를 줄 수 없게 되는 타입
	enum ValidTypeEnum {
		WeaponLength = 0x01, // 무기 길이 이상 날아갔을 경우
		Time = 0x02,		 // 일정 시간 이후
		Stick = 0x04,		 // 땅이나 Prop 에 박히는 경우
	};

	// 타겟 타입
	enum TargetTypeEnum
	{
		CrossHair = 0,  // 크로스 헤어
		Direction,	    // Actor 의 View 방향
		TargetPosition, // 타겟의 위치
		Target,         // 타겟
		DestPosition,   // 액션툴에서 박아놓은 목적지
		Shooter,		// 타겟 타입은 Target 으로 지정되며 발사체를 발사한 액터로 됨. 우선 몬스터에서만 사용하기 때문에 거기까지만 작업됨.
		TargetTypeCount,
	};

	// 가속도/등속도 타입. Orbit 타입이 Projectile 이거나 Homing 인 경우에만 유효.
	enum VelocityTypeEnum
	{
		Constant,	// 디폴트. 등속도로 프로젝타일 날아감.
		Accell,		// 프로젝타일 가속도 먹음.
		VelocityTypeCount,
	};


protected:
	IDnOrbitProcessor* m_pOrbitProcessor;

	static int s_iHitUniqueID;

	int m_iHitUniqueID;

	DnActorHandle m_hShooter;
	MatrixEx m_Cross;
	MatrixEx m_OffsetCross;
	EtVector3 m_vPrevPos;
	EtVector3 m_vStartPos;
	EtVector3 m_vHitPos;
	LOCAL_TIME m_InvalidLocalTime;
	LOCAL_TIME m_CreateLocalTime;
	
	OrbitTypeEnum m_OrbitType;
	DestroyOrbitTypeEnum m_DestroyOrbitType;
	TargetTypeEnum m_TargetType; 
	ValidTypeEnum m_ValidType;

	CDnDamageBase::SHitParam m_HitParam;

	bool m_bValidDamage;
	bool m_bFallGravity;

	bool m_bStick;
	float m_fGravity;

	float m_fSpeed;
	float m_fResistance;
	int m_nValidTime;
	int m_nDestroyOrbitTimeGap;
	bool m_bFirstProcess;

	DnSkillHandle m_hParentSkill;

	std::vector<CDnSkill::StateEffectStruct> m_VecStateEffectList;
	std::vector<CDnSkill::StateEffectStruct> m_ComboLimitStateEffectList;

	HitStruct m_HitStruct;
	DnActorHandle m_hTargetActor;
	EtVector3 m_vTargetPosition;

	CDnCalcCatmullRom m_CatmullRom;
	CDnHoming m_Homing;

	bool m_bOnCollisionCalled;

	bool m_bPierce;

	LOCAL_TIME			m_LastHitSignalEndTime;

	// 폭발형 프로젝타일인 경우 한 번 hit 된 녀석에겐 폭발 데미지를 주지 않도록 한다..
	vector<DnActorHandle> m_VecHittedActor;

	CDnSkill::SkillInfo m_ParentSkillInfo;

	bool m_bHasHitAction;		// hit 액션(폭발)을 갖고 있는 프로젝타일인 
	bool m_bHasHitSignalInHitAction;	// Hit 액션에서 Hit 시그널을 갖고 있는가.
	bool m_bHasHitSignalInShootAction;	// Shoot 액션에서 Hit 시그널을 갖고 있는가.
	bool m_bHitSignalProcessed;
	bool m_bBombHitSignalStarted;	// 2009.7.29 한기 - 폭발시 히트 시그널이 뒤에 있는 경우가 있으므로 뒤에 있는 케이스가 있어서 히트 시그널 시작 시에 플래그 켜줌.
	bool m_bHitActionStarted;
	int m_iShootActionIndex;
	int m_iHitActionIndex;
	int m_iDestroyActionIndex;
	
	bool m_bHasDestroyAction;

	VelocityTypeEnum m_VelocityType;

	LOCAL_TIME m_nLastHitSignalIndex;
	
	float m_fElapsedTime;
	int m_nMaxHitCount;		// 관통형(bPierce == true) 인 경우에만 유효한 최대 히트 값.
	map<CDnActor*, int> m_mapMaxHitCount;

	// 생성 정보의 포인터를 물고 있는다.
	int m_iShooterShootActionIndex;	// 이 프로젝타일을 발사한 액션 인덱스
	int m_iSignalArrayIndex;	// 이 프로젝타일 시그널의 array index
	boost::shared_ptr<ProjectileStruct> m_pProjectileSignal;
	boost::shared_ptr<CDnState> m_pShooterState;
#ifdef PRE_ADD_PROJECTILE_SE_INFO
	boost::shared_ptr<CDnStateBlow> m_pShooterStateBlow;
#endif

	// 체인 공격 상태효과에서만 쓰는 실제 상태효과 걸려서 이 프로젝타일을 쏜 액터
	DnActorHandle m_hPrevAttacker;
	// 체인 공격 상태효과에서 쓰고 있는 스킬 테이블에 정의되어있는 hit prob 값.
	float m_fHitApplyPercent;
	// 

	// 현재 폭발하는 발사체가 영역 hit 시그널을 사용하고 있는지. (ex) summon blackhole
	bool m_bProcessingBombHitSignal;

	// 프랍에서 쏜 발사체라면 기억해뒀다가 충돌 체크에서 뺀다.
	DnPropHandle m_hShooterProp;

	bool m_bFromCharger;

	float m_fProjectileOrbitRotateZDegree;

	// 패킷 달라고 할 때 까지 패킷 스트림 객체는 생성 안됨.
	char* m_pPacketBuffer;
	CPacketCompressStream* m_pPacketStream;

	// hit 된 대상을 따라가는 기능. (#22666)
	bool m_bTraceHitTarget;
	bool m_bTraceHitActorHittable;
	DnActorHandle m_hTraceActor;
	//////////////////////////////////////////////////////////////////////////

	// 외부에서 강제로 지정해준 방향. 현재 대포에서 쓰임. (몬스터의 발사체 패킷에서만 쓰임..)
	EtVector3 m_vForceDir;
	bool m_bUseForceDir;

	char m_cShooterType;
	DWORD m_dwShooterUniqueID;
	INT64 m_nShooterSerialID;
	int m_nShooterActionIndex;
	int m_nShooterSignalIndex;

	ElementEnum m_eForceHitElement;

	// #41382 몬스터 소환 발사체인 경우 167번 상태효과로 소환된 몬스터의 스킬레벨 강제 지정 값이 있다면 적용한다.
	// 이미 발사체가 땅에 떨어져 소환되는 시점에서는 167번 상태효과가 제거된 상황일 수 있다. 
	// (이동하거나 맞거나하서 스킬 액션이 종료되어 자기 자신에게 부여한 상태효과가 제거된 경우)
	int m_iSummonMonsterForceSkillLevel;

protected:
	void MakeHitParam( CDnDamageBase::SHitParam &HitParam );

	void ProcessOrbit( LOCAL_TIME LocalTime, float fDelta );
	void ProcessValid( LOCAL_TIME LocalTime, float fDelta );
	bool ProcessDamage( LOCAL_TIME LocalTime, float fDelta, 
						LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, 
						int nSignalIndex, BOOL bUseHitSignalArea );

	DnPropHandle _CheckPropCollision( EtVector3& vCenter, float fLength, float& fPropContactDistance, float& fPropContactTime );
	void ProcessDestroyOrbit( LOCAL_TIME LocalTime, float fDelta );

	void CheckAndApplyDamage( DnActorHandle hActor, bool bValidDamageGoOn = true, int* nHitCount = NULL );

	bool SetTraceActor( DnActorHandle hActor, bool bValidDamageGoOn );

public:
	OrbitTypeEnum GetOrbitType() { return m_OrbitType; }
	DestroyOrbitTypeEnum GetDestroyOrbitType() { return m_DestroyOrbitType; } 
	TargetTypeEnum GetTargetType() { return m_TargetType; } 

	MatrixEx *GetMatEx() { return &m_Cross; }
	
	void SetSpeed( float fValue ) { m_fSpeed = fValue; }
	float GetSpeed() { return m_fSpeed; }
	void SetResistance( float fValue ) { m_fResistance = fValue; }
	float GetResistance() { return m_fResistance; }
	void SetValidTime( int nValidTime ) 
	{ 
		m_nValidTime = nValidTime; 
		if( 0 < nValidTime )
			m_ValidType = (ValidTypeEnum)( m_ValidType | Time ); 
	};
	int GetValidTime( void ) { return m_nValidTime; };

	EtVector3 *GetTargetPosition() { return &m_vTargetPosition; }
	DnActorHandle GetTargetActor() { return m_hTargetActor; }

	void SetTargetActor( DnActorHandle hActor, bool bUpdateTargetPartsIndex = true );// { m_hTargetActor = hActor; };
	void SetTargetPosition( EtVector3 &vPos ) { m_vTargetPosition = vPos; }

	void SetValidType(ValidTypeEnum ValidType) { m_ValidType = ValidType; }
	void SetDestroyOrbitTime( int nValue ) { m_nDestroyOrbitTimeGap = nValue; }

	bool Initialize( MatrixEx &Offset, OrbitTypeEnum OrbitType, DestroyOrbitTypeEnum DestroyType, TargetTypeEnum TargetType );
	// 정적 멤버 함수인 CreateProjectile... 함수를 통해 생성한 것이 아니라 직접 new 로 생성해서 실제로 발사되어 날아가는 경우,
	// Initialize 함수와 마찬가지로 발사체 생성 후 설정 끝나고 마지막에 이 함수를 꼭 호출해주어야 한다.
	bool PostInitialize( void );		

	void OnDamageSuccess( DnActorHandle hDamagedActor, CDnDamageBase::SHitParam &HitParam );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	void SetParentSkill( DnSkillHandle hParentSkill ) 
	{ 
		m_hParentSkill = hParentSkill;
		if (hParentSkill)
			m_ParentSkillInfo = *(hParentSkill->GetInfo());
		m_ParentSkillInfo.hSkillUser = m_hShooter; 
	};
	void SetParentSkillInfo( CDnSkill::SkillInfo& SkillInfo ) 
	{ 
		m_ParentSkillInfo = SkillInfo; 
		m_ParentSkillInfo.hSkillUser = m_hShooter; 
		m_iShooterShootActionIndex = m_ParentSkillInfo.iProjectileShootActionIndex;
		m_iSignalArrayIndex = m_ParentSkillInfo.iProjectileSignalArrayIndex;
		m_hPrevAttacker = m_ParentSkillInfo.hPrevAttacker;
	}

	void AddStateEffect( CDnSkill::StateEffectStruct &Struct );

	void OnCollisionWithActor( void );
	void OnCollisionWithGround( void );
	void OnCollisionWithProp( void );
	void OnChangedNextActionToHit( void );			// 액션 툴에서 shoot 액션에서 next 액션을 hit 로 설정해놓아서 자동으로 폭발하는 발사체인 경우엔 액션이 바뀐 시점에서 처리할 것들 처리해준다.

	void SetPierce( bool bPierce ) { m_bPierce = bPierce; };
	bool GetPierce( void ) { return m_bPierce; };

	void FromSkill( bool bFromSkill ) { m_HitParam.bFromProjectileSkill = bFromSkill; };
	bool IsFromSkill() { return m_HitParam.bFromProjectileSkill; }

	void SetVelocityType( VelocityTypeEnum VelocityType ) { m_VelocityType = VelocityType; };
	VelocityTypeEnum GetVelocityType( void ) { return m_VelocityType; };

	void SetMaxHitCount( int iMaxHitCount ) { m_nMaxHitCount = iMaxHitCount; };
	int GetMaxHitCount( void ) { return m_nMaxHitCount; };

	virtual void OnChangeAction( const char *szPrevAction );
	virtual void OnFinishAction( const char* szPrevAction, LOCAL_TIME time );

	void SetProjectileSignal( boost::shared_ptr<ProjectileStruct>& pProjectileStruct ) { m_pProjectileSignal = std::move(pProjectileStruct); };
	boost::shared_ptr<ProjectileStruct>& GetProjectileSignal( void ) { return m_pProjectileSignal; };
	
	void SetShootActionIndex( int iActionIndex ) { m_iShooterShootActionIndex = iActionIndex; };
	void SetSignalArrayIndex( int iSignalArrayIndex ) { m_iSignalArrayIndex = iSignalArrayIndex; };

	// 스킬로 발사되는 경우에만 저장해 둔다.
	void SetShooterStateSnapshot( boost::shared_ptr<CDnState>& pShooterState ) { m_pShooterState = std::move(pShooterState); };
	boost::shared_ptr<CDnState>& GetShooterStateSnapshot( void ) { return m_pShooterState; };
#ifdef PRE_ADD_PROJECTILE_SE_INFO
	void SetShooterStateBlow( boost::shared_ptr<CDnStateBlow>& pShooterStateBlow ) { 
        m_pShooterStateBlow = std::move(pShooterStateBlow);};
	boost::shared_ptr<CDnStateBlow>& GetShooterStateBlow( void ) { return m_pShooterStateBlow; };
#endif

	void SetHitApplyPercent( float fHitApplyPercent ) { m_fHitApplyPercent = fHitApplyPercent; };
	
	DnActorHandle GetShooterActor( void ) { return m_hShooter; };
	DnSkillHandle GetParentSkill( void ) { return m_hParentSkill; };

	void SetShooterProp( DnPropHandle hProp ) { m_hShooterProp = hProp; };

	void FromCharger( void ) { m_bFromCharger = true; };
	bool IsFromCharger( void ) { return m_bFromCharger; };

	void SetProjectileOrbitRotateZ( float fProjectileOrbitRotateZ ) { m_fProjectileOrbitRotateZDegree = fProjectileOrbitRotateZ; };
	float GetProjectileOrbitRotateZ( void ) { return m_fProjectileOrbitRotateZDegree; };

	void SetTraceHitTarget( bool bTraceHitTarget, bool bTraceHitActorHittable ) { m_bTraceHitTarget = bTraceHitTarget; m_bTraceHitActorHittable = bTraceHitActorHittable; };
	bool IsTraceHitTarget( void ) { return m_bTraceHitTarget; };
	bool IsTraceHitActorHittable( void ) { return m_bTraceHitActorHittable; };

	void SetForceDir( const EtVector3& vForceDir ) { m_vForceDir = vForceDir; m_bUseForceDir = true; };
	bool IsUsedForceDir( void ) { return m_bUseForceDir; };
	const EtVector3& GetForceDir( void ) { return m_vForceDir; };

	void SetShooterType( DnActorHandle hActor, int nActionIndex, int nSignalIndex );
	void SetShooterType( DnWeaponHandle hWeapon, int nActionIndex, int nSignalIndex );
	// 발사체 생성, 발사체 객체로부터 전송 패킷 만들기.. 등등..
	// 발사체 시그널에 내용이 추가되면 같이 수정해주어야 합니다.
	static CDnProjectile *CreateProjectile( CMultiRoom *pRoom, DnActorHandle hShooter, MatrixEx &LocalCross, ProjectileStruct *pStruct, EtVector3* pForceTargetPos=NULL, 
											DnActorHandle hForceTarget=CDnActor::Identity(), EtVector3* pForceDir = NULL );
	static CDnProjectile* CreatePlayerProjectileFromClientPacket( DnActorHandle hShooter, const BYTE* pPacket );
	CPacketCompressStream* GetPacketStream( void );

	const std::vector<CDnSkill::StateEffectStruct>& GetStateEffectList() { return m_VecStateEffectList; }
	void ApplyParentProjectile(CDnProjectile* pParentProjectile);

protected:
	int m_nTargetPartsIndex;		//Parts몬스터일경우 선택된 parts인덱스
	int m_nTargetPartsBoneIndex;	//선택된 Parts에서 실제 Bone정보를 가진 Index..(MonsterParts를 Vector로 가지고, 다시 _Info가 _PartsInfo를 vector로 가짐)
public:
	void SetTargetPartsIndex(int nPartsIndex, int nBoneIndex) { m_nTargetPartsIndex = nPartsIndex; m_nTargetPartsBoneIndex = nBoneIndex; }
	int GetTargetPartsIndex() { return m_nTargetPartsIndex; }
	int GetTargetPartsBoneIndex() { return m_nTargetPartsBoneIndex; }

	void UpdateTargetPartsIndex();

	void SetForceHitElement( ElementEnum eElement ) { m_eForceHitElement = eElement; };

private:
	// 상태효과 설정값을 확인한다.
	void GetChainAttackInfo(const CDnSkill::StateEffectStruct& stateEffectStruct, float& fRange, int& nMaxCount);
	//다음 상대를 검색
	DnActorHandle FindNextChainActor(int iRootAttackerTeam, DnActorHandle hActor, DnActorHandle hPrevActor, float fRange);
	void CreateChainAttackProjectile(DnActorHandle hRootAttacker, DnActorHandle hActor, DnActorHandle hActorToAttack, ProjectileStruct* pProjectileSignalInfo, CDnSkill::SkillInfo& parentSkillInfo);

public:
	void SetSummonMonsterForceSkillLevel( int iLevel ) { m_iSummonMonsterForceSkillLevel = iLevel; };

#if defined(PRE_FIX_52329)
protected:
	int m_nIgnoreHitType;	//

public:
	void SetIgnoreHitType(int nType) { m_nIgnoreHitType = nType; }
	int GetIgnoreHitType() { return m_nIgnoreHitType; }
#endif // PRE_FIX_52329

#if defined(PRE_ADD_55295)
protected:
	bool m_bHitActionVectorInit;

public:
	//발사체의 방향은 유지한체 Y벡터 초기화 시킨다.
	void ChangeProjectileRotation();

	void SetHitActionVectorInit(bool bHitActionVectorInit) { m_bHitActionVectorInit = bHitActionVectorInit; }
	bool IsHitActionVectorInit() { return m_bHitActionVectorInit; }
#endif // PRE_ADD_55295

//////////////////////////////////////////////////////////////////////////
//#56880
//히트 수 제한 관련 발사체 스킬의 시작 시간을 설정 하도록 한다.
protected:
	LOCAL_TIME m_SkillStartTime;

public:
	void SetSkillStartTime(LOCAL_TIME startTime) { m_SkillStartTime = startTime; }
	LOCAL_TIME GetSkillStartTime() { return m_SkillStartTime; }
//////////////////////////////////////////////////////////////////////////

#if defined(PRE_FIX_59238)
//서먼페펫과 주인 액터가 동시에 히트 되지 않도록 하기 위한 작업
//발사체에서 히트 된 액터를 리스트에 담아 놓는다.
protected:
	std::map<DWORD, DnActorHandle> m_HittedActorList;

public:
	void AddHittedActor(DnActorHandle hHittedActor);
	bool IsHittable(DnActorHandle hActor);
#endif // PRE_FIX_59238

#if defined(PRE_FIX_59336)
	void ApplyComboLimitStateEffect( DnActorHandle hActor );
#endif // PRE_FIX_59336

	void ApplySkillStateEffect(DnActorHandle hActor);

#if defined(PRE_FIX_65287)
	//스킬에서 50번 상태효과를 스킬 시전자 자신에게 적용 해 놓은 상태에서 발사체가 발사되고,
	//발사체유지 되는 시간이 스킬 유지 시간 보다 길어 질 경우, 스킬 시전자의 상태효과는 사라지고,
	//발사체의 데미지 계산에 더 이상 영향을 모 주는 경우가 발생된다.
	//발사체 생성해서 발사체 발사 액터의 스텟 정보를 저장 하는 시점에서, 50번 상태효과가 있는 경우
	//상태효과 설정값을 합산해서 발사체에 설정 해 놓고, 발사체 Hit처리 시 이 정보를 이용하도록 한다.
protected:
	float m_fShooterFinalDamageRate;

public:
	void SetShooterFinalDamageRate(float fRate) { m_fShooterFinalDamageRate = fRate; }
	float GetShooterFinalDamageRate() { return m_fShooterFinalDamageRate; }

#endif // PRE_FIX_65287
};


// 궤적을 결정하는데 필요한 정보들.
struct S_PROJECTILE_PROPERTY
{
	CDnProjectile::OrbitTypeEnum eOrbitType;
	CDnProjectile::TargetTypeEnum eTargetType;
	CDnProjectile::VelocityTypeEnum eVelocityType;
	float fSpeed;
	float fResistance;
	float fProjectileOrbitRotateZ;

	S_PROJECTILE_PROPERTY( void ) : eOrbitType( CDnProjectile::OrbitTypeCount ),
									eTargetType( CDnProjectile::TargetTypeCount ), 
									eVelocityType( CDnProjectile::VelocityTypeCount ),
									fSpeed( 0.0f ),
									fResistance( 0.0f ),
									fProjectileOrbitRotateZ( 0.0f ) {}
};