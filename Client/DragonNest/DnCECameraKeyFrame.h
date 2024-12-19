#pragma once

// #56274 [����Ʈ �巡��] ��鸮�� CameraEffect ��� ����.
// Camera �̵�.
class CDnCECameraKeyFrame : public CDnCameraEffectBase
{

protected:

	// Signal Data.
	std::string m_strCamFile;
		
	//
	LOCAL_TIME m_StartTime; // ī�޶�����Ʈ ���۽���.
	DWORD m_dwFrame;
	bool m_bDestroy;

	class CDnPlayerCamera * m_pPlayerCam;
	class CCameraData * m_pCamData; // ī�޶�����Ʈ ī�޶�������.

	EtQuat m_PreQuat;

public:

	CDnCECameraKeyFrame( DnCameraHandle hCamera, char * pStrCamFile, bool bDefaultEffect=false );
	virtual ~CDnCECameraKeyFrame(){}

	// Override - CDnCameraEffectBase //
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsDestroy(){ return m_bDestroy; }


private:

	void LoadCamFile( class  CDnGameTask *pTask, std::string & strCamFile );

	void CalcPosition( EtVector3 &Position, float fFrame );
	void CalcRotation( EtQuat &Rotation, float fFrame );
	void CalcFov( float &fFov, float fFrame );

};
