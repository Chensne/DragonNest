#pragma once


// Camera �̵�.
class CDnCEMovement : public CDnCameraEffectBase
{

protected:

	// Signal Data.
	EtVector2 m_vDir; // �̵�����.
	float m_fSpeedBegin;
	float m_fSpeedEnd;


	//
	LOCAL_TIME m_StartTime; // ī�޶�����Ʈ ���۽���.
	LOCAL_TIME m_HalfTime;  // �߰������ð�.	
	DWORD m_dwFrame;	
	bool m_bDestroy;

	bool m_bInvSign;		
	float m_fSign;			// �̵���ȣ.

	
	//
	float m_fSpeedGap; // m_fSpeedBegin - m_fSpeedEnd.
	float m_fSpeedCrr; // ����ӵ�.
	EtVector2 m_vPos;
	

public:

	CDnCEMovement( DnCameraHandle hCamera, DWORD dwFrame, EtVector2 & vDir, float fSpeedBegin, float fSpeedEnd, bool bDefaultEffect=false );
	virtual ~CDnCEMovement(){}

	// Override - CDnCameraEffectBase //
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy(){ return m_bDestroy; }

};
