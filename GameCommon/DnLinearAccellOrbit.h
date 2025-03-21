#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"


// OrbitType:Accell 타입의 궤적 처리를 담당하는 클래스
class CDnLinearAccellOrbit : public IDnOrbitProcessor
{
private:
	float m_fSpeed;
	float m_fResistance;

public:
	CDnLinearAccellOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnLinearAccellOrbit( void );

	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );
};