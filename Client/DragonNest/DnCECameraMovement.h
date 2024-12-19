#pragma once


// Camera 이동.
class CDnCEMovement : public CDnCameraEffectBase
{

protected:

	// Signal Data.
	EtVector2 m_vDir; // 이동방향.
	float m_fSpeedBegin;
	float m_fSpeedEnd;


	//
	LOCAL_TIME m_StartTime; // 카메라이펙트 시작시작.
	LOCAL_TIME m_HalfTime;  // 중간지점시간.	
	DWORD m_dwFrame;	
	bool m_bDestroy;

	bool m_bInvSign;		
	float m_fSign;			// 이동부호.

	
	//
	float m_fSpeedGap; // m_fSpeedBegin - m_fSpeedEnd.
	float m_fSpeedCrr; // 현재속도.
	EtVector2 m_vPos;
	

public:

	CDnCEMovement( DnCameraHandle hCamera, DWORD dwFrame, EtVector2 & vDir, float fSpeedBegin, float fSpeedEnd, bool bDefaultEffect=false );
	virtual ~CDnCEMovement(){}

	// Override - CDnCameraEffectBase //
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy(){ return m_bDestroy; }

};
