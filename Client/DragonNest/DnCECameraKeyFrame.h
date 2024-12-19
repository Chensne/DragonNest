#pragma once

// #56274 [데저트 드래곤] 흔들리는 CameraEffect 기능 개발.
// Camera 이동.
class CDnCECameraKeyFrame : public CDnCameraEffectBase
{

protected:

	// Signal Data.
	std::string m_strCamFile;
		
	//
	LOCAL_TIME m_StartTime; // 카메라이펙트 시작시작.
	DWORD m_dwFrame;
	bool m_bDestroy;

	class CDnPlayerCamera * m_pPlayerCam;
	class CCameraData * m_pCamData; // 카메라이펙트 카메라모션정보.

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
