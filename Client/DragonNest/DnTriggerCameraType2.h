#pragma once

#include "DnCamera.h"

/*
트리거에서 간단하게 쓰는 워킹 카메라.
A 프랍위치에서 B 프랍을 기준으로 회전합니다.
*/
class CDnTriggerCameraType2 : public CDnCamera {
public:
	CDnTriggerCameraType2( DnCameraHandle hCurCamera, MatrixEx &BeginCross, MatrixEx &OffsetCross, int nDelay, int nTotalAngle, bool bStartVel = true, bool bEndVel = true );
	virtual ~CDnTriggerCameraType2();

protected:
	int m_nDelay;
	int m_nTotalAngle;
	LOCAL_TIME m_StartTime;
	DnCameraHandle m_hSourceCamera;
	MatrixEx m_CrossOffset[2];
	float m_fCameraDistance;

	bool m_bBlindControl;
	bool m_bStartVel;
	bool m_bEndVel;

public:
	virtual bool Activate();
	virtual bool DeActivate();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	DnCameraHandle GetSourceCamera() { return m_hSourceCamera; }
};
