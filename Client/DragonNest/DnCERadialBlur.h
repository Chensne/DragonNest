#pragma once

#include "DnCamera.h"
#include "Timer.h"

class CEtRadialBlurFilter;
class CDnCERadialBlur : public CDnCameraEffectBase {
public:
	CDnCERadialBlur( DnCameraHandle hCamera, DWORD dwFrame, EtVector2 vBlurCenter, float fBlurSize, float fBeginRatio, float fEndRatio, bool bDefaultEffect = false );
	virtual ~CDnCERadialBlur();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy();

protected:
	LOCAL_TIME m_StartTime;
	DWORD m_dwFrame;
	EtVector2 m_vBlurCenter;
	float m_fBlurSize;
	CEtRadialBlurFilter *m_pRadialBlur;
	float m_fBeginRatio;
	float m_fEndRatio;

	bool m_bDestroy;
};