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
	void Finalize();
	void Reconstruction( int cx = 0, int cy = 0 );
	void Render();
	void Process();

	void ChangeShaderFolder( const char *szFullPath );
	CString GetShaderFolder() { return m_szShaderFolder; }

	void ChangeResourceFolder( const char *szFullPath );
	CString GetResourceFolder() { return m_szResourceFolder; }	

	EtCameraHandle GetCameraHandle() { return m_CameraHandle; }
	LOCAL_TIME GetLocalTime() { return m_FrameSync.GetMSTime(); }

	void UpdateCamera( MatrixEx &Cross );
	MatrixEx *GetLastUpdateCamera() { return &m_Cross; }

	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime = -1 );

	enum MouseFlag {
		NONE = 0x00,
		LB_DOWN = 0x01,
		RB_DOWN = 0x02,
		WB_DOWN = 0x04,
	};
	bool IsInMouseRect( CWnd *pWnd );

	void UpdateLightGrid( SLightInfo *pInfo );
	void SetBackgroundColor( COLORREF dwColor ) { m_BgColor = dwColor; }
	COLORREF GetBackgroundColor() { return m_BgColor; }
	bool	IsShowGrid() {return m_bShowGrid;}
	void SetShowGrid(bool bShow) {m_bShowGrid = bShow;}
	void SetEnvironmentTexture( const char *szFileName );
	const char *GetEnvironmentTexture() { return m_szEnviTextureFile.GetBuffer(); }

protected:
	bool m_bActivate;
	HWND m_hWnd;
	CFrameSync m_FrameSync;
	EtCameraHandle m_CameraHandle;
	CString m_szShaderFolder;
	CString m_szResourceFolder;
	CString m_szWorkingFolder;
	LOCAL_TIME m_PrevLocalTime;

	MatrixEx m_Cross;
	EtVector3 m_CameraAt;
	COLORREF m_BgColor;

	bool m_bShowGrid;

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

	CString m_szEnviTextureFile;
	EtTextureHandle m_hEnviTexture;

	

protected:
	void RenderReducedScale();
	void CalcRenderRect( CRect &rcRect );

	void DrawGrid();
	void DrawAxis();
	void DrawLight( SLightInfo *pInfo );
	void DrawSpotFunc1( float fAngle, SLightInfo *pInfo, bool bYawPitch, DWORD dwColor );
};