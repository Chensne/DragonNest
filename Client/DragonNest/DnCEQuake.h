#pragma once

#include "DnCamera.h"
#include "Timer.h"
class CDnCEQuake : public CDnCameraEffectBase {
public:
	CDnCEQuake( DnCameraHandle hCamera, DWORD dwFrame, float fStartRatio, float fEndRatio, float fDelta, bool bDefaultEffect = false );
	virtual ~CDnCEQuake();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy();

protected:
	LOCAL_TIME m_StartTime;
	DWORD m_dwFrame;
	float m_fStartRatio;
	float m_fEndRatio;
	float m_fDelta;
	bool m_bDestroy;
};