// AniListPaneView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtViewer.h"
#include "AniListPaneView.h"
#include "UserMessage.h"
#include "ObjectBase.h"


// CAniListPaneView

IMPLEMENT_DYNCREATE(CAniListPaneView, CFormView)

CAniListPaneView::CAniListPaneView()
	: CFormView(CAniListPaneView::IDD)
	, m_nEditFPS((int)CObjectSkin::s_fFramePerSec)
{
	m_bActivate = false;
	m_pObject = NULL;
}

CAniListPaneView::~CAniListPaneView()
{
}

void CAniListPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_Rewind);
	DDX_Control(pDX, IDC_BUTTON2, m_Prev);
	DDX_Control(pDX, IDC_BUTTON3, m_Play);
	DDX_Control(pDX, IDC_BUTTON4, m_Next);
	DDX_Control(pDX, IDC_BUTTON5, m_Forward);
	DDX_Control(pDX, IDC_BUTTON6, m_Loop);
	DDX_Control(pDX, IDC_BUTTON7, m_NoLoop);
	DDX_Control(pDX, IDC_BUTTON8, m_Pause);
	DDX_Control(pDX, IDC_STATIC_PROGRESS, m_StaticFrame);
	DDX_Control(pDX, IDC_EDIT1, m_EditFps);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	DDX_Text(pDX, IDC_EDIT1, m_nEditFPS);
}

BEGIN_MESSAGE_MAP(CAniListPaneView, CFormView)
	ON_MESSAGE( UM_ANICONTROL_REFRESH, OnRefresh )
	ON_MESSAGE( UM_ANICONTROL_UPDATE_FRAME, OnUpdateFrame )
	ON_MESSAGE( UM_ANICONTROL_ACCEL_KEY, OnAccelKey )
	ON_BN_CLICKED(IDC_BUTTON1, &CAniListPaneView::OnBnClickedButtonRewind)
	ON_BN_CLICKED(IDC_BUTTON2, &CAniListPaneView::OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON3, &CAniListPaneView::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON4, &CAniListPaneView::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON5, &CAniListPaneView::OnBnClickedButtonForward)
	ON_BN_CLICKED(IDC_BUTTON6, &CAniListPaneView::OnBnClickedButtonLoop)
	ON_BN_CLICKED(IDC_BUTTON7, &CAniListPaneView::OnBnClickedButtonNoLoop)
	ON_BN_CLICKED(IDC_BUTTON8, &CAniListPaneView::OnBnClickedButtonPause)

	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT1, &CAniListPaneView::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CAniListPaneView 진단입니다.

#ifdef _DEBUG
void CAniListPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAniListPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAniListPaneView 메시지 처리기입니다.
float fround( float Value, int n = 1 );
float fround( float Value, int n )
{
	Value = Value * pow(10.f,n);
	if( Value >= 0 ) {
		if( Value - (int)Value >= 0.5 ) Value++;
	}
	if( Value < 0 ) {
		if( Value - (int)Value < -0.5 ) Value--;
	}
	Value = (int)Value/pow(10.f,n);
	return Value;

}

void CAniListPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_PLAY );

	CImageList ImageList;
	ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 7, 1 );
	ImageList.Add( &bitmap, RGB(0,255,0) );

	m_Rewind.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(0) );
	m_Prev.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(1) );
	m_Play.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(2) );
	m_Next.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(3) );
	m_Forward.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(4) );
	m_Pause.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(5) );
	m_NoLoop.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(6) );
	m_Loop.SetIcon( CSize( 16, 16 ), ImageList.ExtractIcon(7) );

	m_Pause.ShowWindow( SW_HIDE );
	m_Loop.ShowWindow( SW_HIDE );

	EnableControl( false );
}	

LRESULT CAniListPaneView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	if( m_pObject ) {
		m_pObject->Stop();
		m_pObject->LinkAniInfo( false );
	}
	m_pObject = (CObjectAnimation*)wParam;


	if( m_pObject == NULL ) {
		EnableControl( FALSE );
		return S_OK;
	}

	// 애니 클릭할때 자동재생하는 기능 삭제.
	//m_pObject->Stop();
	//m_pObject->Resume();
	//m_pObject->Play( m_pObject->GetCurAniIndex() );
	m_pObject->LinkAniInfo( true );

	EnableControl( TRUE );
	m_Loop.ShowWindow( SW_SHOW );
	m_NoLoop.ShowWindow( SW_HIDE );

	m_Pause.ShowWindow( SW_SHOW );
	m_Play.ShowWindow( SW_HIDE );

	CString szStr;
	szStr.Format( "%3d/%3d", (int)fround( m_pObject->GetCurFrame() ), m_pObject->GetTotalFrame() - 1 );
	m_StaticFrame.SetWindowText( szStr );
	m_StaticFrame.UpdateData( TRUE );

	m_Slider.SetRange( 0, m_pObject->GetTotalFrame() - 1, TRUE );
	m_Slider.SetPos( m_pObject->GetCurFrame() );

	return S_OK;
}

void CAniListPaneView::EnableControl( bool bEnable )
{
	m_Rewind.EnableWindow( bEnable );
	m_Prev.EnableWindow( bEnable );
	m_Play.EnableWindow( bEnable );
	m_Pause.EnableWindow( bEnable );
	m_Next.EnableWindow( bEnable );
	m_Forward.EnableWindow( bEnable );
	m_Loop.EnableWindow( bEnable );
	m_NoLoop.EnableWindow( bEnable );

	m_StaticFrame.EnableWindow( bEnable );
	m_EditFps.EnableWindow( bEnable );
	m_Slider.EnableWindow( bEnable );
}
void CAniListPaneView::OnBnClickedButtonRewind()
{
	if( !m_pObject ) return;

	int nCurAni = m_pObject->GetCurAniIndex() - 1;
	if( nCurAni <= 0 ) nCurAni = 0;

	m_pObject->Play( nCurAni, GetLoopCount() );
	OnRefresh( (WPARAM)m_pObject, 0 );
}

void CAniListPaneView::OnBnClickedButtonPrev()
{
	if( !m_pObject ) return;

	float fCurFrame = m_pObject->GetCurFrame() - 1.f;
	if( fCurFrame <= 0 ) fCurFrame = 0.f;

	m_pObject->SetCurFrame( fCurFrame );
}

void CAniListPaneView::OnBnClickedButtonPlay()
{
	m_Play.ShowWindow( SW_HIDE );
	m_Pause.ShowWindow( SW_SHOW );

	if( !m_pObject ) return;
	if( m_pObject->IsPause() ) {
		m_pObject->Resume();
	}
	else {
		m_pObject->Play( m_pObject->GetCurAniIndex(), GetLoopCount() );
	}
}

void CAniListPaneView::OnBnClickedButtonPause()
{
	m_Play.ShowWindow( SW_SHOW );
	m_Pause.ShowWindow( SW_HIDE );

	if( !m_pObject ) return;
	m_pObject->Pause();
}

void CAniListPaneView::OnBnClickedButtonNext()
{
	if( !m_pObject ) return;

	float fCurFrame = m_pObject->GetCurFrame() + 1.f;
	if( fCurFrame >= (float)m_pObject->GetTotalFrame() - 1 ) fCurFrame = (float)m_pObject->GetTotalFrame() - 1;

	m_pObject->SetCurFrame( fCurFrame );
}

void CAniListPaneView::OnBnClickedButtonForward()
{
	if( !m_pObject ) return;

	int nCurAni = m_pObject->GetCurAniIndex() + 1;
	if( nCurAni >= m_pObject->GetTotalAniIndex() - 1 ) nCurAni = m_pObject->GetTotalAniIndex() - 1;

	m_pObject->Play( nCurAni, GetLoopCount() );
	OnRefresh( (WPARAM)m_pObject, 0 );
}

void CAniListPaneView::OnBnClickedButtonLoop()
{
	m_Loop.ShowWindow( SW_HIDE );
	m_NoLoop.ShowWindow( SW_SHOW );

	if( !m_pObject ) return;
	if( m_pObject->IsPlay() || m_pObject->IsPause() )
		m_pObject->SetLoopCount( 1000000 );
}

void CAniListPaneView::OnBnClickedButtonNoLoop()
{
	m_NoLoop.ShowWindow( SW_HIDE );
	m_Loop.ShowWindow( SW_SHOW );

	if( !m_pObject ) return;
	if( m_pObject->IsPlay() || m_pObject->IsPause() )
		m_pObject->SetLoopCount( 1 );
}

LRESULT CAniListPaneView::OnUpdateFrame( WPARAM wParam, LPARAM lParam )
{
	CString szStr, szTemp;
	szStr.Format( "%3d/%3d", (int)fround( m_pObject->GetCurFrame() ), m_pObject->GetTotalFrame() - 1 );

	m_StaticFrame.GetWindowText( szTemp );
	if( strcmp( szStr, szTemp ) != NULL ) { 
		m_StaticFrame.SetWindowText( szStr );
		m_StaticFrame.UpdateData( TRUE );

		m_Slider.SetPos( (int)m_pObject->GetCurFrame() );
	}

	if( m_pObject->IsPlay() && !m_pObject->IsPause() ) {
		m_Play.ShowWindow( SW_HIDE );
		m_Pause.ShowWindow( SW_SHOW );
	}
	else {
		m_Play.ShowWindow( SW_SHOW );
		m_Pause.ShowWindow( SW_HIDE );
	}
	return S_OK;
}

int CAniListPaneView::GetLoopCount()
{
	int nLoopCount = 1;
	if( m_NoLoop.IsWindowVisible() == TRUE ) nLoopCount = 10000000;
	return nLoopCount;
}

void CAniListPaneView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);

	if( pScrollBar->m_hWnd == m_Slider.m_hWnd ) {
		if( !m_pObject ) return;
		m_pObject->Pause();
		m_pObject->SetCurFrame( (float)m_Slider.GetPos() );

		CString szStr;
		szStr.Format( "%3d/%3d", (int)fround( m_pObject->GetCurFrame() ), m_pObject->GetTotalFrame() - 1 );
		m_StaticFrame.SetWindowText( szStr );
		m_StaticFrame.UpdateData( TRUE );

	}

}

void CAniListPaneView::OnEnChangeEdit1()
{
//	m_EditFps.UpdateData( FALSE );
	if( m_EditFps ) {
		UpdateData();
		CObjectSkin::s_fFramePerSec = (float)m_nEditFPS;
	}
}

LRESULT CAniListPaneView::OnAccelKey( WPARAM wParam, LPARAM lParam )
{
	if( m_pObject == NULL ) return S_OK;
	switch( (int)wParam ) {
		case 0:
			if( m_pObject->IsPlay() && !m_pObject->IsPause() ) OnBnClickedButtonPause();
			else OnBnClickedButtonPlay(); 
			break;
		case 1:
			OnBnClickedButtonRewind();
			break;
		case 2:
			OnBnClickedButtonForward();
			break;
		case 3:
			if( !m_pObject->IsPause() )
				OnBnClickedButtonPause();
			OnBnClickedButtonPrev();
			break;
		case 4:
			if( !m_pObject->IsPause() )
				OnBnClickedButtonPause();
			OnBnClickedButtonNext();
			break;
	}
	return S_OK;
}
