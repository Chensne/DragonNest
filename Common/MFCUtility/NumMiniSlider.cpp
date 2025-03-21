// NumMiniSlider.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NumMiniSlider.h"


// CNumMiniSlider

IMPLEMENT_DYNAMIC(CNumMiniSlider, CWnd)

CNumMiniSlider::CNumMiniSlider()
{
	m_bForceUpdate = false;
}

CNumMiniSlider::~CNumMiniSlider()
{
	m_Static.DestroyWindow();
	m_Edit.DestroyWindow();
	m_Slider.DestroyWindow();
	m_Spin.DestroyWindow();
}


BEGIN_MESSAGE_MAP(CNumMiniSlider, CWnd)
	ON_WM_CREATE()
	ON_MESSAGE( UM_MINISLIDER_UPDATE_POS, OnUpdatePosSliderEx )
	ON_EN_UPDATE(1000, OnEnUpdateEdit)
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CNumMiniSlider 메시지 처리기입니다.



int CNumMiniSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CRect rcRect, rcTemp;
	GetClientRect( &rcRect );

	CFont *pFont = GetParent()->GetFont();
	SetFont( pFont );

	// Static
	CString szStr;
	rcTemp = rcRect;
	rcTemp.top += 2;
	rcTemp.bottom = rcTemp.top + 15;
	rcTemp.right -= 60;
	GetWindowText( szStr );
	m_Static.Create( szStr, WS_VISIBLE | WS_CHILD, rcTemp, this );
	m_Static.SetFont( pFont );
	
	// Edit
	int nEditSize = 50;
	rcTemp = rcRect;
	rcTemp.bottom = rcTemp.top + 19;
	rcTemp.left = rcTemp.right - ( nEditSize + 15 );
	rcTemp.right -= 15;
	GetWindowText( szStr );
	m_Edit.CreateEx( WS_EX_CLIENTEDGE, _T("EDIT"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_RIGHT | ES_AUTOHSCROLL, rcTemp, this, 1000 );
	m_Edit.SetFont( pFont );
	szStr.Format( "%d", m_Slider.GetPos() );
	m_Edit.SetWindowText( szStr );
	m_Edit.EnableWindow( TRUE );

	// Spin
	rcTemp = rcRect;
	rcTemp.bottom = rcTemp.top + 17;
	rcTemp.left = rcTemp.right - 16;
	m_Spin.Create( UDS_ARROWKEYS | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_HOTTRACK | UDS_NOTHOUSANDS | WS_VISIBLE | WS_CHILD, rcTemp, this, 1001 );
	m_Spin.SetFont( pFont );

	// Slider
	rcTemp = rcRect;
	rcTemp.top = rcTemp.bottom - 10;
	m_Slider.Create( NULL, NULL, WS_VISIBLE | WS_CHILD, rcTemp, this, 1002 );
	m_Slider.SetFont( pFont );


	SetRange( GetRangeMin(), GetRangeMax() );
	Invalidate();

	return 0;
}

void CNumMiniSlider::SetRange( int nMin, int nMax )
{
	m_Slider.SetRange( nMin, nMax );
	m_Spin.SetRange32( nMin, nMax );
}

void CNumMiniSlider::SetRangeMin( int nValue )
{
	m_Slider.SetRangeMin( nValue );
	m_Spin.SetRange32( nValue, GetRangeMax() );
}

void CNumMiniSlider::SetRangeMax( int nValue )
{
	m_Slider.SetRangeMax( nValue );
	m_Spin.SetRange32( GetRangeMin(), nValue );
}


void CNumMiniSlider::SetPos( int nValue )
{
	m_Slider.SetPos( nValue );

	CString szStr;
	szStr.Format( "%d", nValue );

	m_bForceUpdate = true;
	m_Edit.SetWindowText( szStr );
	m_bForceUpdate = false;
}


LRESULT CNumMiniSlider::OnUpdatePosSliderEx( WPARAM wParam, LPARAM lParam )
{
	int nPos = (int)lParam;
	CString szStr;
	szStr.Format( "%d", nPos );
	m_Edit.SetWindowText( szStr );

	if( GetParent() )
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)lParam );
	return S_OK;
}


void CNumMiniSlider::OnEnUpdateEdit()
{
	CString szStr;
	int nValue;
	bool bFlag = false;
	if( !m_Slider ) return;

	if( m_bForceUpdate == true ) return;
	m_Edit.GetWindowText( szStr );
	nValue = atoi( szStr );
	if( nValue < m_Slider.GetRangeMin() ) {
		nValue = m_Slider.GetRangeMin();
		bFlag = true;
	}
	else if( nValue > m_Slider.GetRangeMax() ) {
		nValue = m_Slider.GetRangeMax();
		bFlag = true;
	}

	m_Slider.SetPos( nValue );
	if( bFlag ) {
		szStr.Format( "%d", nValue );
		m_Edit.SetWindowText( szStr );
	}

	if( GetParent() )
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)nValue );
}

void CNumMiniSlider::OnEnable(BOOL bEnable)
{
	m_Edit.EnableWindow( bEnable );
	m_Static.EnableWindow( bEnable );
	m_Slider.EnableWindow( bEnable );
	m_Spin.EnableWindow( bEnable );
	CWnd::OnEnable( bEnable );
}