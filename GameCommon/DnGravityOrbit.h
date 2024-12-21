#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"



// OrbitType:Projectile VelocityType:Accell 타입의 중력 가속도를 먹는 발사체 궤적 처리를 담당하는 클래스
class CDnGravityOrbit : public IDnOrbitProcessor
{
private:
	float m_fProjectileOrbitRotateZDegree;
	
	EtVector3 m_vStartPos;
	EtVector3 m_vDirectionWhenCreate;
	
	EtVector3 m_vTargetPosition;
	EtVector3 m_vInitialSpeed;
	
	bool m_bFirstProcess;

	float m_fElapsedTime;

public:
	CDnGravityOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnGravityOrbit( void );
	
	void Initialize( const MatrixEx& Cross, const MatrixEx& OffsetCross );
	void SetTargetPosition( EtVector3 vTarget ) { m_vTargetPosition = vTarget; };
	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );
};

// 2차곡선 관련 계산 편하게 하기위한 헬퍼.. 현재 CDnCannonMonsterActor에서 대포 궤적 계산시 쓰임.
class CDnGravityOrbitCalculator
{
public:
	static const EtVector3 GRAVITY;

	// 초기속도, 발사방향이 주어졌을 때 낙하위치 및 시간 계산.. 
	// 지형에 따라 어느곳에 떨어질줄 모르기 때문에 20분의 1초만큼 delta 값을 줘서 직접 지형과 충돌이 일어나는지 계산해야한다..
	// 외부에서는 조건값이 바뀌었을 때 한번만 호출해주도록 한다.
	// vInitialDir 은 로컬 좌표계 기준 정규화된 방향벡터임. (x 값은 0)
#ifndef _GAMESERVER
	static bool CalcHitGroundPos( /*IN*/ MatrixEx Cross, /*IN*/ const EtVector3& vShootPos, /*IN*/ const float fInitialSpeed, 
								  /*IN*/ const EtVector3& vInitialDir, /*OUT*/ EtVector3& vGroundHitPosition );
#endif
};
