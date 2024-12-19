#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"
#include "DnCalcCatmullrom.h"


// OrbitType:Homing, VelocityType:Accell 타입의 호밍 가속도 발사체의 궤적을 처리하는 클래스.
class CDnHomingAccellOrbit : public IDnOrbitProcessor
{
private:
	float m_fSpeed;
	MatrixEx m_OffsetCross;
	DnActorHandle m_hTargetActor;
	EtVector3 m_vTargetPosition;
	CDnHoming m_Homing;
	bool m_bFirstProcess;
	int* m_piValidTime;

public:
	CDnHomingAccellOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnHomingAccellOrbit( void );

	void Initialize( const MatrixEx& Cross, const MatrixEx& OffsetCross );
	void SetTargetActor( DnActorHandle hActor ) { m_hTargetActor = hActor; };
	void SetTargetPosition( EtVector3 vTarget ) { m_vTargetPosition = vTarget; };		// 호밍 가속도인데 TargetPosition 쓰는 경우가 있어서 추가. #20922
	void SetValidTimePointer( int* piValidTime ) { m_piValidTime = piValidTime; }
	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_FIX_HOMINGTARGET)
	static bool ms_ShowHomingTarget;
#endif // PRE_FIX_HOMINGTARGET
};