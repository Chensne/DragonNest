#pragma once
#include "IDnOrbitProcessor.h"
#include "DnCalcCatmullrom.h"
#include "SignalHeader.h"

// OrbitType:Homing, VelocityType:Constant 타입의 호밍 발사체를 처리하는 클래스
class CDnHomingOrbit : public IDnOrbitProcessor
{
private:
	float m_fSpeed;
	DnActorHandle m_hTargetActor;
	EtVector3 m_vTargetPosition;
	CDnHoming m_Homing;
	bool m_bFallGravity;

public:
	CDnHomingOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnHomingOrbit( void );

	void SetTargetActor( DnActorHandle hActor ) { m_hTargetActor = hActor; };
	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );
	
	// #29199 이슈 때문에 invalid 되어 fallgravity 처리되는 발사체는 발사체의 월드 변환 행렬을 진행 방향으로 맞춰준다.
	void ProcessFallGravityOrbit( MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta, float& fGravity, float fFallGravityAccell );
};