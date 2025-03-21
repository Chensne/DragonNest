#include "StdAfx.h"
#include "EtUI.h"
#include "RenderBase.h"
#include "MainFrm.h"
#include "EtUITool.h"
#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "DummyView.h"
#include "GlobalValue.h"
#include "../../Common/EtResManager/EtResourceMng.h"
#include "EtUIMan.h"
#include "EtPrimitiveDraw.h"
#include "../../Common/EternityEngine/D3DDevice9/EtStateManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRenderBase g_RenderBase;

CRenderBase::CRenderBase()
{
	m_bActivate = false;
	m_PrevLocalTime = 0;

	m_nWidth = 0;
	m_nHeight = 0;

	SetRect( &m_DragRect, 0, 0, 0, 0 );
	m_dwDragColor = 0;
	m_bDragSolid = false;
}

CRenderBase::~CRenderBase()
{
	Finalize();
}

void CRenderBase::Finalize()
{
	if( m_bActivate == false ) return;
	SAFE_RELEASE_SPTR( m_CameraHandle );
	m_FrameSync.End();

	EternityEngine::FinalizeEngine();
	m_bActivate = false;
}

void CRenderBase::Initialize( HWND hWnd, int nWidth, int nHeight )
{
	m_hWnd = hWnd;

	EternityEngine::InitializeEngine( m_hWnd, nWidth, nHeight, true, true );
#ifdef PRE_MODIFY_RESOURCE_MANAGE
#else
	CEtResource::SetWaitDeleteMode( false );
#endif
	GetEtStateManager()->SetEnable( false );

	InitializeCamera();

	m_FrameSync.Begin( -1 );
	m_bActivate = true;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

void CRenderBase::Reinitialize( int nWidth, int nHeight )
{
	//EternityEngine::ReinitializeEngine( nWidth, nHeight );
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	GetEtDevice()->SetSize( m_nWidth, m_nHeight );
	CEtUIDialog::SetDialogSize( 2, m_nWidth, m_nHeight );
	CEtUIDialog::ProcessChangeResolution();
	EternityEngine::ReinitializeEngine( m_nWidth, m_nHeight );
}

void CRenderBase::InitializeCamera()
{
	SAFE_RELEASE_SPTR( m_CameraHandle );

	m_CameraHandle = EternityEngine::CreateCamera();
	
	EternityEngine::SetGlobalAmbient( &EtColor( 1.0f, 1.0f, 1.0f, 1.f ) );
	UpdateCamera( m_MatrixEx );
}

void CRenderBase::Render()
{
	if( m_bActivate == false ) 
		return;

	if( m_FrameSync.CheckSync() )
	{
		LOCAL_TIME LocalTime = m_FrameSync.GetMSTime();
		CString szFrame;
		szFrame.Format( "FPS : %.2f", m_FrameSync.GetFps() );
		EternityEngine::DrawText2D( EtVector2( 0.f, 0.f ), szFrame );

		CRect rcRect;
		CalcRenderRect( rcRect );

		EternityEngine::ClearScreen( D3DCOLOR_ARGB( 255, 128, 128, 128 ) );
		
		float fDelta = ( LocalTime - m_PrevLocalTime ) * 0.001f;

		EtInterface::Process( fDelta );
		EternityEngine::RenderFrame( fDelta );

		EtInterface::Render( fDelta );
		if( ( ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView )->GetActiveView() == 0 )
		{
			DrawBoxes();
			RenderDialogCrosshair();
		}
		EternityEngine::ShowFrame( &rcRect );
		m_PrevLocalTime = LocalTime;
	}
}

void CRenderBase::Process()
{
	if( m_bActivate == false )
		return;

	m_FrameSync.UpdateTime();
	LOCAL_TIME LocalTime = m_FrameSync.GetMSTime();
}

void CRenderBase::UpdateCamera( MatrixEx &Cross )
{
	m_MatrixEx = Cross;
	if( !m_CameraHandle ) return;
	m_CameraHandle->Update( Cross );
}

void CRenderBase::CalcRenderRect( CRect &rcRect )
{
	CEtUIToolView *pView;

	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	GetClientRect( m_hWnd, &rcRect );
	if( pView )
	{
		CRect rcTabRect;
		CDummyView *pDummyView;
		CDummyView::TabViewStruct *pStruct;

		pDummyView = pView->GetDummyView();
		pStruct = pDummyView->GetTabViewStruct( pView->GetActiveView() );
		if( pStruct->pView == NULL ) return;
		pStruct->pView->GetClientRect( &rcTabRect );

		pDummyView->ClientToScreen( &rcRect );
		pStruct->pView->ClientToScreen( &rcTabRect );

		int nLeft = rcRect.left - rcTabRect.left;
		int nTop = rcRect.top - rcTabRect.top;
		int nRight = rcRect.right - rcTabRect.right;
		int nBottom = rcRect.bottom - rcTabRect.bottom;

		GetClientRect( m_hWnd, &rcRect );
		rcRect += CRect( nLeft, nTop, -nRight, -nBottom );

		rcRect.right = min( rcRect.right, rcRect.left + m_nWidth );
		rcRect.bottom = min( rcRect.bottom, rcRect.top + m_nHeight );

		rcRect.right = rcRect.left + m_nWidth;
		rcRect.bottom = rcRect.top + m_nHeight;
	}
}

void CRenderBase::AddTabOrder( int nOrder, float fX, float fY, DWORD dwColor )
{
	SDrawTabOrder Order;

	Order.nTabOrder = nOrder;
	Order.fX = fX;
	Order.fY = fY;
	Order.dwColor = dwColor;
	m_vecTabOrder.push_back( Order );
}

void CRenderBase::AddSelectBox( RECT &BoxRect, DWORD dwColor )
{
	SSelectUIBox SelectBox;

	SelectBox.BoxRect = BoxRect;
	SelectBox.dwColor = dwColor;
	m_vecSelectBox.push_back( SelectBox );
}

void CRenderBase::AddInitState( int nInitState, RECT &BoxRect, float fX, float fY )
{
	SDrawInitState InitState;

	InitState.nInitState = nInitState;
	InitState.BoxRect = BoxRect;
	InitState.fX = fX;
	InitState.fY = fY;
	m_vecInitState.push_back( InitState );
}

void CRenderBase::DrawBoxes()
{
	// Note : 마우스를 드래그 할때 박스를 그려준다.
	//
	if( ( m_DragRect.left != m_DragRect.right ) && ( m_DragRect.top != m_DragRect.bottom ) )
	{
		DrawBox( m_DragRect, m_dwDragColor, m_bDragSolid );
	}

	for( int i = 0; i < ( int )m_vecSelectBox.size(); i++ )
	{
		DrawBox( m_vecSelectBox[ i ].BoxRect, m_vecSelectBox[ i ].dwColor, false );
	}

	// Note : 탭 표시 상자를 그린다.
	//
	for( int i = 0; i < ( int )m_vecTabOrder.size(); i++ )
	{
		RECT Rect;
		EtVector2 Position;
		char szString[ 100 ];
		SetRect( &Rect, ( int )( m_vecTabOrder[ i ].fX * GetEtDevice()->Width() - 16 ) , ( int )( m_vecTabOrder[ i ].fY * GetEtDevice()->Height() - 16 ), 
			( int )( m_vecTabOrder[ i ].fX * GetEtDevice()->Width() + 16 ), ( int )( m_vecTabOrder[ i ].fY * GetEtDevice()->Height() + 16 ) );
		DrawBox( Rect, m_vecTabOrder[ i ].dwColor, true );
		Position.x = m_vecTabOrder[ i ].fX - 0.01f;
		Position.y = m_vecTabOrder[ i ].fY - 0.015f;
		sprintf_s( szString, 100, "%d", m_vecTabOrder[ i ].nTabOrder );
		EternityEngine::DrawText2D( Position, szString, 0xffffffff );
	}

	// 하이드랑 Disable 표시
	for( int i = 0; i < ( int )m_vecInitState.size(); i++ )
	{
		EtVector2 Position;
		Position.x = m_vecInitState[ i ].fX;// - 0.01f;
		Position.y = m_vecInitState[ i ].fY;// - 0.015f;

		char szString[ 16 ];
		if( m_vecInitState[ i ].nInitState == 1 )
		{
			DrawBox( m_vecInitState[ i ].BoxRect, 0x77ffffff, false );
			sprintf_s( szString, 16, "%s", "[Hide]" );
			EternityEngine::DrawText2D( Position, szString, 0x77ffffff );
		}
		else if( m_vecInitState[ i ].nInitState == 2 )
		{
			DrawBox( m_vecInitState[ i ].BoxRect, 0x77ffffff, false );
			sprintf_s( szString, 16, "%s", "[Disable]" );
			EternityEngine::DrawText2D( Position, szString, 0x77ffffff );
		}
	}

	CEtPrimitiveDraw::GetInstance().Render( CEtCamera::GetActiveCamera() );
}

void CRenderBase::DrawBox( RECT &Rect, DWORD dwColor, bool bDrawSolid )
{
	float fX1, fX2, fY1, fY2;
	EtVector2 Start, End;

	fX1 = Rect.left / ( float )GetEtDevice()->Width();
	fY1 = Rect.top / ( float )GetEtDevice()->Height();
	fX2 = Rect.right / ( float )GetEtDevice()->Width();
	fY2 = Rect.bottom / ( float )GetEtDevice()->Height();

	EternityEngine::DrawLine2D( EtVector2( fX1, fY1 ), EtVector2( fX2, fY1 ), dwColor );
	EternityEngine::DrawLine2D( EtVector2( fX2, fY1 ), EtVector2( fX2, fY2 ), dwColor );
	EternityEngine::DrawLine2D( EtVector2( fX2, fY2 ), EtVector2( fX1, fY2 ), dwColor );
	EternityEngine::DrawLine2D( EtVector2( fX1, fY2 ), EtVector2( fX1, fY1 ), dwColor );

	if( bDrawSolid )
	{
		dwColor &= 0x00ffffff;
		dwColor |= 0x40000000;
		EternityEngine::DrawTriangle2D( EtVector2( fX1, fY1 ), EtVector2( fX2, fY1 ), EtVector2( fX2, fY2 ), dwColor );
		EternityEngine::DrawTriangle2D( EtVector2( fX1, fY1 ), EtVector2( fX2, fY2 ), EtVector2( fX1, fY2 ), dwColor );
	}
}

void CRenderBase::SetDragRect( RECT &Rect, DWORD dwColor, bool bSolid )
{ 
	m_DragRect = Rect;
	m_dwDragColor = dwColor;
	m_bDragSolid = bSolid;
}

void CRenderBase::SetCrosshair( bool bDraw, RECT &rcDialog )
{
	m_bDrawCrosshair = bDraw;
	m_rcDialog = rcDialog;
}

void CRenderBase::RenderDialogCrosshair()
{
	if( !m_bDrawCrosshair ) return;

	float fX1, fX2, fY1, fY2;
	fX1 = m_rcDialog.left / ( float )GetEtDevice()->Width();
	fY1 = m_rcDialog.top / ( float )GetEtDevice()->Height();
	fX2 = m_rcDialog.right / ( float )GetEtDevice()->Width();
	fY2 = m_rcDialog.bottom / ( float )GetEtDevice()->Height();

	float fXMid = (m_rcDialog.left + m_rcDialog.right)/2 / ( float )GetEtDevice()->Width();
	float fYMid = (m_rcDialog.top + m_rcDialog.bottom)/2 / ( float )GetEtDevice()->Height();

	EternityEngine::DrawLine2D( EtVector2( fXMid, fY1 ), EtVector2( fXMid, fY2 ), 0xFF0077FF );
	EternityEngine::DrawLine2D( EtVector2( fX1, fYMid ), EtVector2( fX2, fYMid ), 0xFF0077FF );
}