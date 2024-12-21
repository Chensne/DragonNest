#pragma once

#include "DnCamera.h"
#include "DnActor.h"
#include "InputReceiver.h"

#ifdef PRE_ADD_JAPAN_AUTO_AIMING
const int LOCKON_TEXTURE_COUNT = 2;
#endif // PRE_ADD_JAPAN_AUTO_AIMING

class CDnAniCamera;
class CDnPlayerCamera : public CDnCamera, public CInputReceiver
{
public:
	CDnPlayerCamera();
	virtual ~CDnPlayerCamera();

	static float s_fCenterPushWidth;
	static float s_fCenterPushHeight;
	static float s_fLastDistance;
protected:
	DnActorHandle m_hAttachActor;
	float m_fDistance;
	float m_fDistanceAccel;

	float m_fMaxZoom;
	float m_fMinZoom;

	float m_fChangeViewYaw;
	float m_fChangeViewPrevYaw;
	float m_fChangeViewCurDelta;
	float m_fChangeViewVectorDelta;

	float m_fPitchValue;
	float m_fYawValue;
	float m_fForceYawValue;

	float m_fPitchSmoothValue;
	float m_fYawSmoothValue;
	float m_fDistanceBeforeSmooth;

	float m_fPrevPropDistance;
	float m_fCurrentDistance;
	float m_fTargetDistance;

	bool m_bLockFreeYaw;
	bool m_bFollowCam;
	bool m_bFirstProcess;

	float	m_fSmoothAnglePower;
	float	m_fSmoothZoomPower;

	CDnAniCamera	*m_PitchCamera;
	bool	m_bUsePitchCamera;
	bool	m_bMouseInvert;
	bool    m_bResetLookWhenActivated;
	
#ifdef PRE_ADD_JAPAN_AUTO_AIMING
	bool				m_bPrevAutoAmingBtnDown;
	DnActorHandle		m_hLastAutoAmingActor;

	EtTextureHandle		m_ahLockOnTexture[ LOCKON_TEXTURE_COUNT ];
	LOCAL_TIME			m_LockOnFrameChangeTime;
	int					m_iNowLockOnTextureIndex;
#endif // PRE_ADD_JAPAN_AUTO_AIMING
	bool				m_bPrevTurnBtnDown;

protected:
	void RefreshPlayerView( float fDelta, bool bForce );

public:
	virtual bool Activate();
	virtual bool DeActivate();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void PostProcess( LOCAL_TIME LocalTime, float fDelta );

	void GetFocusCross();
	void AttachActor( DnActorHandle hHandle );
	void DetachActor();
	void ChangeViewVector( float fDelta );
	DnActorHandle GetAttachActor() { return m_hAttachActor; }

	float GetDistance() { return m_fDistance - m_fCurrentDistance; }

	float GetDistanceOrg(){ return m_fDistance; }

	virtual void ResetCamera();
	virtual void ResetLook();
	virtual void ForceRefreshEngineCamera();
	void SyncActorView();

	void ResetLookWhenActivated(){ m_bResetLookWhenActivated = true; }
	void LockFreeYaw( bool bLock );
	bool IsFreeYaw() { return m_bLockFreeYaw; }
	void FollowCam( bool bFollow );
	bool IsFollowCam() { return m_bFollowCam; }
	
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime = -1 );
	float GetCurrentDistance() { return m_fCurrentDistance; }

	void SetSmoothAnglePower( float fPower) { m_fSmoothAnglePower = fPower; }
	void SetSmoothZoomPower( float fPower) { m_fSmoothZoomPower = fPower; }
	
	void ForceYawCamera(float fPower){m_fForceYawValue = fPower;}
	void SetZoomDistance(float fMin,float fMax);
	void ResetZoomDistance();
	EtVector2 GetZoomDistance(){EtVector2 eZoom; eZoom.x = m_fMinZoom; eZoom.y = m_fMaxZoom; return eZoom;}

	void EnablePitchCamera( bool bEnable ) {m_bUsePitchCamera = bEnable ;}

	void SetMouseInvert( bool bInvert )	{ m_bMouseInvert = bInvert; }
};
