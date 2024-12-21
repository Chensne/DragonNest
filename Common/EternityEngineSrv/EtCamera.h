#pragma once

#include "EtType.h"
#include "EtObject.h"

class CEtCamera;
typedef CMultiSmartPtr< CEtCamera >	EtCameraHandle;

struct SRenderSkin
{
	std::string szSkinName;
	EtSkinHandle hRenderSkin;
	EtMatrix SkinRenderMat;
};

class CEtCamera : public CMultiSmartPtrBase< CEtCamera, MAX_SESSION_COUNT >
{
public:
	CEtCamera( CMultiRoom *pRoom );
	virtual ~CEtCamera();

protected:
	SCameraInfo m_CameraInfo;
	float m_fAspectRatio;

	EtMatrix m_ViewMat;
	EtMatrix m_InvViewMat;
	EtMatrix m_ProjMat;

	STATIC_DECL( EtCameraHandle s_hActiveCamera );

public:
	void Initialize( SCameraInfo *pCamera );
	void Reset();
	void LookAt( EtVector3 &Eye, EtVector3 &At, EtVector3 &Up );
	void Update( EtMatrix *pCameraMat ) { EtMatrixInverse( &m_ViewMat, NULL, pCameraMat ); }
	EtVector3 *GetPosition() { return ( EtVector3 * )&m_InvViewMat._41; }
	EtVector3 *GetDirection() { return ( EtVector3 * )&m_InvViewMat._31; }
	EtMatrix *GetViewMat() { return &m_ViewMat; }
	EtMatrix *GetInvViewMat() { return &m_InvViewMat; }
	EtMatrix *GetProjMat() { return &m_ProjMat; }
	void SetProjMat( EtMatrix &ProjMat ) { m_ProjMat = ProjMat; }
	EtMatrix *GetViewProjMat();
	void CalcInvViewMat() { EtMatrixInverse( &m_InvViewMat, NULL, &m_ViewMat ); }
	void *GetViewport() { return NULL; }
	CameraTarget GetCameraTargetType() { return m_CameraInfo.Target; }

	float GetCameraFar() { return m_CameraInfo.fFar; }
	float GetCameraNear() { return m_CameraInfo.fNear; }
	float GetAspectRatio() { return m_fAspectRatio; }
	float GetFOV() { return m_CameraInfo.fFOV; }

	float GetFogNear() { return m_CameraInfo.fFogNear; }
	float GetFogFar() { return m_CameraInfo.fFogFar; }
	EtColor *GetFogColor() { return &m_CameraInfo.FogColor; }

	void CalcPositionAndDir( int nX, int nY, EtVector3 &Position, EtVector3 &Dir );

	EtCameraHandle Activate();

	static EtCameraHandle SetActiveCamera( CMultiRoom *pRoom, int nItemIndex );
	static EtCameraHandle GetActiveCamera( CMultiRoom *pRoom ) { return STATIC_INSTANCE_(s_hActiveCamera); }
};

