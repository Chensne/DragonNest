#pragma once

#include "DnCamera.h"
#include "InputReceiver.h"

class CDnFreeCamera : public CDnCamera, public CInputReceiver {
public:
	CDnFreeCamera();

protected:
	float m_fSensitivity;
	float m_fSensitivityScale;

	LOCAL_TIME m_PrevLocalTime;
	LARGE_INTEGER m_qpFreq;
	LARGE_INTEGER m_qpBaseTime;

	float m_fShowSensitivity;
	int m_nMoveFlag;
	float m_fDelta;

	float m_fSlowScale;

	int m_nMouseRBDownPosX;
	int m_nMouseRBDownPosY;

public:
	virtual bool Activate();
	virtual bool DeActivate();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// CInputReceiver
	void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime = -1 );
};