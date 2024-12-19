#pragma once
#include "DnCamera.h"
#include "InputReceiver.h"


class CDnCannonCamera : public CDnCamera, public CInputReceiver
{
private:
	DnActorHandle m_hAttachActor;
	
	// ������ ���Ǵ� ����. ������ �� �༮�� ��ġ���� ī�޶� �¿� ȸ���� ���� �ȴ�.
	// ���콺�� �¿�� �����̸� �ش� ���� �������� ī�޶� ȸ����Ű�� ���� ���͵� ȸ����Ų��.
	DnActorHandle m_hAttachCannonMonster;

	float m_fPitchInputValue;
	float m_fYawInputValue;
	float m_fZDistanceAccel;
	float m_fNowZDistanceValue;

	float m_fNowPitchValue;
	float m_fNowYawValue;

	float m_fSmoothAnglePower;
	float m_fSmoothZoomPower;

	bool m_bFirstProcess;
	float m_fTargetDistance;
	float m_fCurrentDistance;

	float m_fMinZoom;
	float m_fMaxZoom;

	float m_fZDistanceBeforeSmooth;

	static float s_fLastZDistance;

	float m_fPlayerActorZDistance;			// ������ ���� ĳ���Ϳ��� ���� Z �� �Ÿ�.

public:
	CDnCannonCamera(void);
	virtual ~CDnCannonCamera(void);

	virtual bool Activate();
	virtual bool DeActivate();

	void AttachActor( DnActorHandle hActor );
	void DetachActor( void ) { m_hAttachActor.Identity(); };

	void AttachCannonMonster( DnActorHandle hActor );
	void DetachCannonMonster( void ) { m_hAttachCannonMonster.Identity(); };

	void RotateActorToCamZDirection( void );

	virtual void ResetCamera( void );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void PostProcess( LOCAL_TIME LocalTime, float fDelta );

	float GetDistance() { return m_fNowZDistanceValue - m_fCurrentDistance; }
	float GetYawValue() { return m_fNowYawValue; }

	//// �켱 ������Ÿ������ �ٷ� ���⺤�� �����ϵ���.
	//void ViewActorDir( void );
};
