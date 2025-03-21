#pragma once
#include "DnCamera.h"
#include "InputReceiver.h"


class CDnCannonCamera : public CDnCamera, public CInputReceiver
{
private:
	DnActorHandle m_hAttachActor;
	
	// 대포로 사용되는 몬스터. 실제로 이 녀석의 위치값이 카메라 좌우 회전의 축이 된다.
	// 마우스를 좌우로 움직이면 해당 값을 기준으로 카메라를 회전시키고 붙은 액터도 회전시킨다.
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

	float m_fPlayerActorZDistance;			// 대포를 잡은 캐릭터와의 로컬 Z 축 거리.

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

	//// 우선 프로토타입으로 바로 방향벡터 설정하도록.
	//void ViewActorDir( void );
};
