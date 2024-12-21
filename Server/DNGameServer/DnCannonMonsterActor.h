#pragma once

#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"
#include "MASingleDamage.h"


// 시드래곤네스트에서부터 쓰이는 대포를 위한 몬스터 액터.
// 235792 번 몬스터로 추가되는 신규 대포 몬스터.
// AI 는 사용하지 않는다.

class CDnCannonMonsterActor : public TDnMonsterActor< MASingleBody, MASingleDamage >
{
private:
	// 이 대포 몬스터를 소유하고 있는 액터의 핸들.
	DnActorHandle m_hMasterPlayerActor;
	EtVector3 m_vCannonLookDirection;

	// 클라이언트에서 대포 카메라의 Z 축 벡터. CannonTargeting 메시지로 오기 때문에 실제 발사체 쏠 땐 이걸로 쏴야한다.
	EtVector3 m_vCannonTargetingDir;
	EtVector3 m_vShootDir;
	EtVector3 m_vCannonGroundHitPos;

public:
	CDnCannonMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnCannonMonsterActor();

	virtual MAMovementBase* CreateMovement();

	virtual void OnDie( DnActorHandle hHitter );
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void Look( EtVector2& vVec, bool bForceRotate=true);

	virtual void OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor, HitStruct *pStruct );
	virtual void OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct );

	void SetMasterPlayerActor( DnActorHandle hMasterPlayerActor );
	void ClearMasterPlayerActor( void );
	bool IsPossessed( void ) { return m_hMasterPlayerActor ? true : false; };
	void SetCannonProjectileInfo( const EtVector3& vCannonDir, const EtVector3& vShootDir, const EtVector3& vCannonGroundHitPos )
	{ m_vCannonTargetingDir = vCannonDir; m_vShootDir = vShootDir; m_vCannonGroundHitPos = vCannonGroundHitPos; };
	void OnMasterPlayerActorDie( void );
	DnActorHandle GetMasterPlayerActor( void ) { return m_hMasterPlayerActor; };
	
	void SetCannonLookDirection(EtVector3 *Direction) { m_vCannonLookDirection = *Direction; }
	EtVector3 GetCannonLookDricetion(){return m_vCannonLookDirection;}
	bool IsCannonMonsterActor(){ return true; }
};
