#pragma once
#include "IDnOrbitProcessor.h"
#include "DnCalcCatmullrom.h"
#include "SignalHeader.h"

// OrbitType:Homing, VelocityType:Constant Ÿ���� ȣ�� �߻�ü�� ó���ϴ� Ŭ����
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
	
	// #29199 �̽� ������ invalid �Ǿ� fallgravity ó���Ǵ� �߻�ü�� �߻�ü�� ���� ��ȯ ����� ���� �������� �����ش�.
	void ProcessFallGravityOrbit( MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta, float& fGravity, float fFallGravityAccell );
};