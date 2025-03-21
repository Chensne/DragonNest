#pragma once
#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"
#include "MASingleDamage.h"
#include "DnSkill.h"


// 시드래곤네스트에서부터 쓰이는 대포를 위한 몬스터 액터.
// 235792 번 몬스터로 추가되는 신규 대포 몬스터.
// AI 는 사용하지 않는다.
class CDnCannonMonsterActor : public TDnMonsterActor< MASingleBody, MASingleDamage >
{
private:
	// 이 대포 몬스터를 소유하고 있는 액터의 핸들.
	DnActorHandle m_hMasterPlayerActor;
	EtVector3 m_vNowShootDir;				// 대포 기준 로컬 좌표계의 발사체 방향. 액션툴에서 정한 방향과 같은 의미.
	map<int, const ProjectileStruct*> m_mapCannonProjectileSignalBySkillID;
	EtVector3 m_vCannonLookDirection;
	bool m_bHaveMaster; // Rotha 현재 대포에 마스터가 있는가>?

	// 대포 포신이 바라보고 있는 회전 값이 포함된 EtMatrixEx
	MatrixEx m_matExCannonRotated;
	MatrixEx m_matExOriginal;

	// 실제 스킬은 서버에서만 갖고 있다.
	// 클라이언트에서는 쿨타임 표시만을 위해 필요한 데이터만 사용한다.
	// 클라이언트에서는 실제로 UseSkill 이나 스킬 시스템이 돌아가지 않습니다.
	// 현재 강/약 대포 쏘는 두 개가 있음.	

public:
	CDnCannonMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnCannonMonsterActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAStaticMovement();
		return pMovement;
	}

	virtual bool Initialize( void );
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnDie( DnActorHandle hHitter );
	virtual void SetDestroy();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessCannonLook(bool bLocal);
	virtual void Look( EtVector2& vVec, bool bForceRotate=true);
	
	virtual void OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor );
	// from CDnMonsterActor
	virtual void OnSkillUse( int iSkillID, int iLevel );
	void SetMasterPlayerActor( DnActorHandle hMasterPlayerActor );
	void ClearMasterPlayerActor( void );
	bool GetCannonGroundHitPosition( /*IN*/ DnCameraHandle hCamera,  /*OUT*/ EtVector3& vCannonGroundHitPos );
	void OnMasterPlayerActorDie( void );
	DnActorHandle GetMasterPlayerActor( void ) { return m_hMasterPlayerActor; };

	const EtVector3& GetNowShootDir( void ) { return m_vNowShootDir; };

	void SetCannonLookDirection(EtVector3 *Direction) { m_vCannonLookDirection = *Direction; }
	EtVector3 GetCannonLookDricetion(){return m_vCannonLookDirection;}
	bool IsHaveMaster(){return m_bHaveMaster;}

	virtual void OnCreateProjectileBegin( /*IN OUT*/ MatrixEx& LocalCross );
	virtual void OnCreateProjectileEnd( void ); 
	virtual void OnComboCount( int nValue, int nDelay );
	
};
