#pragma once
#include "IDnOrbitProcessor.h"
#include "SignalHeader.h"


// OribtType:Linear Ÿ���� ���� ó���� ����ϴ� Ŭ����
class CDnLinearOrbit : public IDnOrbitProcessor
{
private:
	float m_fSpeed;
	EtVector3 m_vFirstZVector;		// ���� �߻�ü �̵���ų �� ����ϴ� ���� ����. ��ٶ� ������Ʈ�� ���� ��ȯ��ķ� Cross.ZVector �� �ǽð����� ������Ʈ ���ش�.
	bool m_bFirstProcess;

public:
	CDnLinearOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo );
	virtual ~CDnLinearOrbit( void );

	void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta );
};