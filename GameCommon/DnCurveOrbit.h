#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"
#include "DnCalcCatmullrom.h"

// OrbitType:Projectile, VelocityType:Constant 타입의 곡선 발사체의 궤적을 처리하는 클래스
class CDnCurveOrbit : public IDnOrbitProcessor
{
private:
	EtVector3 m_vTargetPosition;
	CDnCalcCatmullRom m_CatmullRom;
	float m_fSpeed;
	bool m_bFirstProcess;

public:
	CDnCurveOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnCurveOrbit( void );

	void SetTargetPosition( EtVector3 vTarget ) { m_vTargetPosition = vTarget; };
	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );
};