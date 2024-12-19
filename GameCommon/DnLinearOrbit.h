#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"


// OribtType:Linear 타입의 궤적 처리를 담당하는 클래스
class CDnLinearOrbit : public IDnOrbitProcessor
{
private:
	float m_fSpeed;
	EtVector3 m_vFirstZVector;		// 실제 발사체 이동시킬 때 사용하는 방향 벡터. 기다란 오브젝트를 위한 변환행렬로 Cross.ZVector 는 실시간으로 업데이트 해준다.
	bool m_bFirstProcess;

public:
	CDnLinearOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnLinearOrbit( void );

	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );
};