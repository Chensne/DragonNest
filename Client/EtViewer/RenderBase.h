#pragma once

#include "Singleton.h"
#include "FrameSync.h"
#include "GlobalValue.h"
#include "EtMatrixEx.h"
#include "InputReceiver.h"

class CRenderBase : public CSingleton<CRenderBase>, public CInputReceiver {
public:
	CRenderBase();
	~CRenderBase();

public:
	void Initialize( HWND hWnd );
	void Reconstruction();
	void Finalize();
	void Resize();
	bool Render();
	float GetFPS() { return m_FrameSync.GetFps();}

	EtCameraHandle GetCameraHandle() { return m_CameraHandle; }
	MatrixEx *GetCameraCross() { return &m_CameraCross; }
	void SetCameraCross( MatrixEx *pCross ) { m_CameraCross = *pCross; }

	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime );

	void UpdateLightGrid( SLightInfo *pInfo );
	void SetBackgroundColor( DWORD dwColor ) { m_dwBgColor = dwColor; }
	DWORD GetBackgroundColor() { return m_dwBgColor; }
	void ShowGrid( bool bShow ) { m_bShowGrid = bShow; }
	bool IsShowGrid() { return m_bShowGrid; }
	void ShowAxis( bool bShow ) { m_bShowAxis = bShow; }
	bool IsShowAxis() { return m_bShowAxis; }

	void SetEnvironmentTexture( const char *szFileName );
	const char *GetEnvironmentTexture() { return m_szEnviTextureFile.GetBuffer(); }

	enum MouseFlag {
		NONE = 0x00,
		LB_DOWN = 0x01,
		RB_DOWN = 0x02,
		WB_DOWN = 0x04,
	};
	bool IsInMouseRect( CWnd *pWnd );
	CString GetShaderFolder() { return m_szShaderFolder; }
	void ChangeShaderFolder( const char *szFullPath );

	void ChangePriority( bool bLessPriority );
	bool IsLessPriority() { return m_bLessPriority; }

protected:
	bool m_bActivate;
	HWND m_hWnd;
	CFrameSync m_FrameSync;
	bool m_bLessPriority;

	EtVector3 m_CameraAt;
	MatrixEx m_CameraCross;
	EtCameraHandle m_CameraHandle;
	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_PrevLocalTime;
	DWORD m_dwBgColor;
	bool m_bShowGrid;
	bool m_bShowAxis;

	int m_nMouseFlag;
	D3DXVECTOR2 m_StartPos;
	D3DXVECTOR2 m_CurPos;

	EtVector3 m_vAxis[6];
	EtVector3 m_vAlphabetX[4];
	EtVector3 m_vAlphabetY[4];
	EtVector3 m_vAlphabetZ[6];

	EtVector3 m_vDirection[18];
	EtVector3 m_vPoint[30];
	std::vector<SLightInfo> m_VecLightInfo;

	CString m_szShaderFolder;
	CString m_szEnviTextureFile;
	EtTextureHandle m_hEnviTexture;

protected:
	void RenderReducedScale();
	void DrawGrid();
	void DrawAxis();
	void DrawLight( SLightInfo *pInfo );
	void DrawSpotFunc1( float fAngle, SLightInfo *pInfo, bool bYawPitch, DWORD dwColor );
};