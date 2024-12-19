#pragma once

#include "DnCamera.h"

// #56274
// Camera Roll 회전 - fAngle 만큼 좌에서 우로.
class CDnCESwing : public CDnCameraEffectBase
{

protected:

	
	// Signal Data.
	float m_fSpeed;
	float m_fAngle;
	BOOL m_bSmooth;

	//
	LOCAL_TIME m_StartTime; // 카메라이펙트 시작시간.
	DWORD m_dwFrame;	
	bool m_bDestroy;

	float m_fCrrAngle; // 현재까지 증가된 회전각.
	float m_Sign;      // 회전각 부호.

	float m_fHalfAngle;

public:

	CDnCESwing( DnCameraHandle hCamera, DWORD dwFrame, float fAngle, float fSpeed, BOOL bSmooth, bool bDefaultEffect=false );
	virtual ~CDnCESwing(){}

	// Override - CDnCameraEffectBase //
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy() { return m_bDestroy; }

};
