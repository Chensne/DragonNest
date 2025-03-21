#pragma once

#include "DnCamera.h"

/*
	트리거에서 간단하게 쓰는 워킹 카메라.
	A 프랍에서 B 프랍으로 이동입니다.
*/
class CDnTriggerCameraType1 : public CDnCamera {
public:
	CDnTriggerCameraType1( DnCameraHandle hCurCamera, MatrixEx &BeginCross, MatrixEx &EndCross, int nDelay, bool bStartVel = true, bool bEndVel = true );
	virtual ~CDnTriggerCameraType1();

protected:
	int m_nDelay;
	LOCAL_TIME m_StartTime;
	DnCameraHandle m_hSourceCamera;
	MatrixEx m_CrossOffset[2];

	bool m_bBlindControl;
	bool m_bStartVel;
	bool m_bEndVel;

public:
	virtual bool Activate();
	virtual bool DeActivate();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	DnCameraHandle GetSourceCamera() { return m_hSourceCamera; }
};
