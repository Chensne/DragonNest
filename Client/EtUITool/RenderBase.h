#pragma once

#include "Singleton.h"
#include "FrameSync.h"
#include "EtMatrixEx.h"

struct SSelectUIBox
{
	RECT BoxRect;
	DWORD dwColor;
};

struct SDrawTabOrder
{
	int nTabOrder;
	float fX;
	float fY;
	DWORD dwColor;
};

struct SDrawInitState
{
	int nInitState;
	RECT BoxRect;
	float fX;
	float fY;
};

class CEtUIToolView;
class CEnviElement;
class CEtSoundEngine;

class CRenderBase : public CSingleton< CRenderBase > 
{
public:
	CRenderBase();
	~CRenderBase();

protected:
	bool m_bActivate;
	HWND m_hWnd;
	CFrameSync m_FrameSync;
	EtCameraHandle m_CameraHandle;
	MatrixEx m_MatrixEx;

	LOCAL_TIME m_PrevLocalTime;

	int m_nWidth;
	int m_nHeight;

	CEtUIToolView *m_pView;

	RECT m_DragRect;
	DWORD m_dwDragColor;
	bool m_bDragSolid;

	bool m_bDrawCrosshair;
	RECT m_rcDialog;

	std::vector< SSelectUIBox > m_vecSelectBox;
	std::vector< SDrawTabOrder > m_vecTabOrder;
	std::vector< SDrawInitState > m_vecInitState;

protected:
	void RenderReducedScale();
	void RenderDialogCrosshair();

public:
	void Initialize( HWND hWnd, int nWidth, int nHeight );
	void Reinitialize( int nWidth, int nHeight );
	void Finalize();
	void Render();
	void Process();

	EtCameraHandle GetCameraHandle() { return m_CameraHandle; }
	LOCAL_TIME GetLocalTime() { return m_FrameSync.GetMSTime(); }

	void UpdateCamera( MatrixEx &Cross );
	MatrixEx *GetLastUpdateCamera() { return &m_MatrixEx; }
	void InitializeCamera();

	void CalcRenderRect( CRect &rcRect );

	void ClearTabOrder() { m_vecTabOrder.clear(); }
	void AddTabOrder( int nOrder, float fX, float fY, DWORD dwColor );
	void ClearSelectBox() { m_vecSelectBox.clear(); }
	void AddSelectBox( RECT &BoxRect, DWORD dwColor );
	void SetDragRect( RECT &Rect, DWORD dwColor = 0, bool bSolid = false );
	void ClearInitState() { m_vecInitState.clear(); }
	void AddInitState( int nInitState, RECT &BoxRect, float fX, float fY );
	void DrawBoxes();
	void DrawBox( RECT &Rect, DWORD dwColor, bool bDrawSolid );

	void SetCrosshair( bool bDraw, RECT &rcDialog );
};