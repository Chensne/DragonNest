#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"


// OribtType:TerrainLinear 타입의 궤적 처리를 담당하는 클래스
class CDnTerrainLinearOrbit : public IDnOrbitProcessor
{
private:
	bool m_bFirstProcess;
	float m_fSpeed;


public:
	CDnTerrainLinearOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnTerrainLinearOrbit( void );

	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );
};