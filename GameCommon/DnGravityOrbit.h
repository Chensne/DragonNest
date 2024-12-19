#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"



// OrbitType:Projectile VelocityType:Accell Ÿ���� �߷� ���ӵ��� �Դ� �߻�ü ���� ó���� ����ϴ� Ŭ����
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

// 2��� ���� ��� ���ϰ� �ϱ����� ����.. ���� CDnCannonMonsterActor���� ���� ���� ���� ����.
class CDnGravityOrbitCalculator
{
public:
	static const EtVector3 GRAVITY;

	// �ʱ�ӵ�, �߻������ �־����� �� ������ġ �� �ð� ���.. 
	// ������ ���� ������� �������� �𸣱� ������ 20���� 1�ʸ�ŭ delta ���� �༭ ���� ������ �浹�� �Ͼ���� ����ؾ��Ѵ�..
	// �ܺο����� ���ǰ��� �ٲ���� �� �ѹ��� ȣ�����ֵ��� �Ѵ�.
	// vInitialDir �� ���� ��ǥ�� ���� ����ȭ�� ���⺤����. (x ���� 0)
#ifndef _GAMESERVER
	static bool CalcHitGroundPos( /*IN*/ MatrixEx Cross, /*IN*/ const EtVector3& vShootPos, /*IN*/ const float fInitialSpeed, 
								  /*IN*/ const EtVector3& vInitialDir, /*OUT*/ EtVector3& vGroundHitPosition );
#endif
};
