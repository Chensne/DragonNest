#pragma once

#include "DnCamera.h"
#include "Timer.h"

class CEtDOFFilter;
class CDnCEDof : public CDnCameraEffectBase {
public:
	CDnCEDof( DnCameraHandle hCamera, DWORD dwFrame, float fFocusDist, float fNearStart, float fNearEnd, float fFarStart, float fFarEnd, float fNearBlur, float fFarBlur, bool bDefaultEffect = false );
	virtual ~CDnCEDof();

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy();

	void SetNear( float fStart, float fEnd );
	void SetFar( float fStart, float fEnd );
	void SetFocusDistance( float fValue );
	void SetBlurSize( float fNear, float fFar );
	void GetBlurSize( float *fNear, float *fFar ) { *fNear = m_fNearBlur; *fFar = m_fFarBlur; }

	CEtDOFFilter* GetFilter( void ) { return m_pDOFFilter; };

protected:
	LOCAL_TIME m_StartTime;
	DWORD m_dwFrame;
	EtVector2 m_vNear;
	EtVector2 m_vFar;
	float m_fFocusDist;
	float m_fNearBlur;
	float m_fFarBlur;
	CEtDOFFilter *m_pDOFFilter;

	bool m_bDestroy;
};