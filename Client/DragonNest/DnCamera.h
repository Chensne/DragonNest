#pragma once

#include "EtMatrixEx.h"
#include "SmartPtr.h"
#include "Timer.h"

class CDnWorldEnvironment;
class CDnCameraEffectBase;

#define CAMERA_ZOOM_MIN 100.f
#define CAMERA_ZOOM_MAX 430.f
#define CAMERA_VEHICLE_ZOOM_MAX 600.f
#define CAMERA_VEHICLE_ZOOM_MIN 200.f

class CDnCamera : public CSmartPtrBase<CDnCamera> {
public:
	CDnCamera();
	virtual ~CDnCamera();

	enum CameraTypeEnum {
		Unknown = -1,
		PlayerCamera = 0,
		FreeCamera,
		InterfaceCamera,
		NpcTalkCamera,
		TriggerControl1Camera,
		TriggerControl2Camera,
		CannonCamera,
	};

protected:
	EtCameraHandle m_hCamera;
	SCameraInfo m_CameraInfo;
	MatrixEx m_matExWorld;
	LOCAL_TIME m_LocalTime;
	std::map<int, float> m_nMapFreezeList;
	CameraTypeEnum m_CameraType;
	bool m_bDestroy;

	std::vector<CDnCameraEffectBase *> m_pVecCameraEffect;

	static DnCameraHandle s_hActiveCamera;
	static DnCameraHandle s_hActiveCameraQueue;
	static int s_nCameraEffectCount;

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual void PostProcess( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual bool Activate() { return true; }
	virtual bool DeActivate();
	virtual void ProcessEffect( LOCAL_TIME LocalTime, float fDelta );

	MatrixEx *GetMatEx() { return &m_matExWorld; }
	SCameraInfo *GetCameraInfo() { return &m_CameraInfo; }
	void SetCameraInfo( SCameraInfo &Info ) { m_CameraInfo = Info;	m_hCamera->Initialize( &m_CameraInfo ); }
	EtCameraHandle GetCameraHandle() { return m_hCamera; }
	LOCAL_TIME GetLocalTime() { return m_LocalTime; }

	static void ProcessClass( LOCAL_TIME LocalTime, float fDelta );
	static void ReleaseClass();
	static void SetActiveCamera( DnCameraHandle hCamera, bool bForce = true );
	static DnCameraHandle GetActiveCamera() { return s_hActiveCamera; }
	static DnCameraHandle FindCamera( CDnCamera::CameraTypeEnum Type );

	void SetFreeze( bool bFlag, int nIndex = 0 );
	void SetFreeze( float fResistanceRatio, int nIndex = 0 );
	bool IsFreeze( int nIndex = -1 );
	float GetFreezeResistanceRatio( int nIndex = -1 );

	int CreateCameraBlurFilter();
	int SetCameraBlur( LOCAL_TIME StartTime, DWORD dwDelay, float fBlendFactor );

	virtual void ResetCamera();
	virtual void ForceRefreshEngineCamera();
	// Camera Effect
	CDnCameraEffectBase *FindCameraEffect( int nIndex );
	void RemoveCameraEffect( int nIndex );
	void RemoveAllCameraEffect();
	void ResetRadialBlurEffect();
	void ResetQuakeEffect();

	virtual int Quake( DWORD dwDelay, float fBeginRatio, float fEndRatio, float fValue, bool bDefaultEffect = false );
	virtual int DepthOfField( DWORD dwDelay, float fFocusDist, float fNearStart, float fNearEnd, float fFarStart, float fFarEnd, float fNearBlur, float fFarBlur, bool bDefaultEffect = false );
	virtual int RadialBlur( DWORD dwDelay, EtVector2 vBlurCenter, float fBlurSize, float fBeginRatio, float fEndRatio, bool bDefaultEffect = false );
	virtual int Swing( DWORD dwDelay, float fAngle, float fSpeed, BOOL bSmooth, bool bDefaultEffect = false );
	virtual int Movement( DWORD dwDelay, EtVector2 vDir, float fSpeedBegin, float fSpeedEnd, bool bDefaultEffect = false );
	virtual int KeyFrame( char * pStrCamFile, bool bDefaultEffect = false );

	CameraTypeEnum GetCameraType() { return m_CameraType; }
	void SetDestroy();
	bool IsDestroy() { return m_bDestroy; }

	/*
	bool IsEnableDOFFilter();
	void EnableDOFFilter( bool bEnable );
	void SetDOFNearValue( float fStart, float fEnd );
	void SetDOFFarValue( float fStart, float fEnd );
	void SetDOFBlurSize( float fSize );
	void SetDOFFocusDistance( float fDist );
	void CreateDOFFilter();
	*/

	void SetFOV( float fov );
};

class CDnCameraEffectBase {
public:
	CDnCameraEffectBase( DnCameraHandle hCamera, bool bDefaultEffect = false ) { m_hCamera = hCamera; m_nIndex = -1; m_bDefaultEffect = bDefaultEffect; }
	virtual ~CDnCameraEffectBase() {}

	void SetIndex( int nIndex ) { m_nIndex = nIndex; }
	int GetIndex() { return m_nIndex; }

	bool IsDefaultEffect() { return m_bDefaultEffect; }

	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual bool IsDestroy() { return false; }

protected:
	DnCameraHandle m_hCamera;
	int m_nIndex;
	bool m_bDefaultEffect;
};