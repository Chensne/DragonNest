#pragma once

#include "DnCamera.h"
#include "InputReceiver.h"
#include "CameraData.h"

class CDnAniCamera : public CDnCamera, public CInputReceiver {
public:
	CDnAniCamera();
	virtual ~CDnAniCamera();

public:
	struct AniCameraStruct {
		SCameraFileHeader Header;
		std::string szName;
		std::vector<SCameraFOVKey> VecFOV;
		std::vector<SCameraPositionKey> VecPosition;
		std::vector<SCameraRotationKey> VecRotation;

		void CalcPosition( EtVector3 &Position, float fFrame );
		void CalcRotation( EtQuat &Rotation, float fFrame );
		void CalcFov( float &fFov, float fFrame );
	};
protected:
	std::vector<AniCameraStruct *> m_pVecAniList;
	
	LOCAL_TIME m_StartTime;
	float m_fTotalFrame;
	float m_fFrame;

	std::string m_szAction;
	int m_nCurrentActionIndex;
	std::string m_szNextAction;
	int m_nNextActionIndex;

protected:
	AniCameraStruct *LoadCameraData( const char *szFileName );
	int GetCameraIndex( const char *szName );

public:
	virtual bool Activate();
	virtual bool DeActivate();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	bool AddCameraData( const char *szFileName );
	void PlayCamera( const char *szName, const char *szNextName, bool bLoop = true );
	const char *GetCurrentAction() { return m_szAction.c_str(); }
	float GetCurFrame() { return m_fFrame; }

	AniCameraStruct* GetCameraStruct( int nIndex ) { return m_pVecAniList[nIndex]; }
	float GetTotalFrame( const char *szName );

	void ResetAni();
};