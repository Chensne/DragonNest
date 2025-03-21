#pragma once

#include "DnCamera.h"
#include "Timer.h"

class CEtCameraBlurFilter;
class CDnCECameraBlur : public CDnCameraEffectBase {
public:
	CDnCECameraBlur( DnCameraHandle hCamera );
	virtual ~CDnCECameraBlur();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy();

	void SetBlur( LOCAL_TIME StartTime, DWORD dwFrame, float fBlendFactor );

protected:
	LOCAL_TIME m_StartTime;
	DWORD m_dwFrame;
	float m_fBlendFactor;
	CEtCameraBlurFilter *m_pCameraBlur;
	float m_fBeginRatio;
	float m_fEndRatio;

	bool m_bDestroy;
};