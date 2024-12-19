// NumMiniSlider.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NumMiniSliderRange.h"


// CNumMiniSliderRange

IMPLEMENT_DYNAMIC(CNumMiniSliderRange, CWnd)

CNumMiniSliderRange::CNumMiniSliderRange()
{
	m_bForceUpdate = false;
}

CNumMiniSliderRange::~CNumMiniSliderRange()
{
	m_Static.DestroyWindow();
	m_Edit.DestroyWindow();
	m_Edit2.DestroyWindow();
	m_Slider.DestroyWindow();
	m_Spin.DestroyWindow();
	m_Spin2.DestroyWindow();
}


BEGIN_MESSAGE_MAP(CNumMiniSliderRange, CWnd)
	ON_WM_CREATE()
	ON_MESSAGE( UM_MINISLIDER_UPDATE_POS, OnUpdatePosSliderEx )
	ON_EN_UPDATE(1000, OnEnUpdateEdit)
	ON_EN_UPDATE(1003, OnEnUpdateEdit2)
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CNumMiniSliderRange 메시지 처리기입니다.



int CNumMiniSliderRange::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	
	// Edit1
	int nEditWidth = 47;
	rcTemp = rcRect;
	rcTemp.bottom = rcTemp.top + 19;
	rcTemp.left = rcTemp.right - ( (nEditWidth * 2) + 30 );
	rcTemp.right -= ( nEditWidth + 30 );
	GetWindowText( szStr );
	m_Edit.CreateEx( WS_EX_CLIENTEDGE, _T("EDIT"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_RIGHT | ES_AUTOHSCROLL, rcTemp, this, 1000 );
	m_Edit.SetFont( pFont );

	// Spin1
	rcTemp = rcRect;
	rcTemp.bottom = rcTemp.top + 17;
	rcTemp.left = rcTemp.right - ( nEditWidth + 31 );
	m_Spin.Create( UDS_ARROWKEYS | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_HOTTRACK | UDS_NOTHOUSANDS | WS_VISIBLE | WS_CHILD, rcTemp, this, 1001 );
	m_Spin.SetFont( pFont );

	// Edit2
	rcTemp = rcRect;
	rcTemp.bottom = rcTemp.top + 19;
	rcTemp.left = rcTemp.right - ( nEditWidth + 15 );
	rcTemp.right -= 15;
	GetWindowText( szStr );
	m_Edit2.CreateEx( WS_EX_CLIENTEDGE, _T("EDIT"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_RIGHT | ES_AUTOHSCROLL, rcTemp, this, 1003 );
	m_Edit2.SetFont( pFont );

	// Spin2
	rcTemp = rcRect;
	rcTemp.bottom = rcTemp.top + 17;
	rcTemp.left = rcTemp.right - 16;
	m_Spin2.Create( UDS_ARROWKEYS | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_HOTTRACK | UDS_NOTHOUSANDS | WS_VISIBLE | WS_CHILD, rcTemp, this, 1004 );
	m_Spin2.SetFont( pFont );

	// Slider
	rcTemp = rcRect;
	rcTemp.top = rcTemp.bottom - 10;
	m_Slider.Create( NULL, NULL, WS_VISIBLE | WS_CHILD, rcTemp, this, 1002 );
	m_Slider.SetFont( pFont );
	m_Slider.SetSliderType( CMiniSliderEx::ST_RANGE );


	SetRange( GetRangeMin(), GetRangeMax() );
	Invalidate();

	return 0;
}


void CNumMiniSliderRange::SetRange( int nMin, int nMax )
{
	m_Slider.SetRange( nMin, nMax );
	m_Spin.SetRange32( nMin, nMax );
	m_Spin2.SetRange32( nMin, nMax );
}

void CNumMiniSliderRange::SetRangeMin( int nValue )
{
	m_Slider.SetRangeMin( nValue );
	m_Spin.SetRange32( nValue, GetRangeMax() );
	m_Spin2.SetRange32( nValue, GetRangeMax() );
}

void CNumMiniSliderRange::SetRangeMax( int nValue )
{
	m_Slider.SetRangeMax( nValue );
	m_Spin.SetRange32( GetRangeMin(), nValue );
	m_Spin2.SetRange32( GetRangeMin(), nValue );
}

irange CNumMiniSliderRange::GetPos()
{
	return irange( m_Slider.GetPos(), m_Slider.GetEndPos() );
}

void CNumMiniSliderRange::SetPos( irange range )
{
	m_Slider.SetPos( range.nMin );
	m_Slider.SetEndPos( range.nMax );

	CString szStr;

	m_bForceUpdate = true;

	szStr.Format( "%d", range.nMin );
	m_Edit.SetWindowText( szStr );

	szStr.Format( "%d", range.nMax );
	m_Edit2.SetWindowText( szStr );

	m_bForceUpdate = false;
}


LRESULT CNumMiniSliderRange::OnUpdatePosSliderEx( WPARAM wParam, LPARAM lParam )
{
	irange range = *(irange*)lParam;

	CString szStr;
	szStr.Format( "%d", range.nMin );
	m_Edit.SetWindowText( szStr );

	szStr.Format( "%d", range.nMax );
	m_Edit2.SetWindowText( szStr );


	if( GetParent() )
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)lParam );
	return S_OK;
}


void CNumMiniSliderRange::OnEnUpdateEdit()
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

	if( GetParent() ) {
		irange range( nValue, m_Slider.GetEndPos() );
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)&range );
	}
}

void CNumMiniSliderRange::OnEnUpdateEdit2()
{
	CString szStr;
	int nValue;
	bool bFlag = false;
	if( !m_Slider ) return;

	if( m_bForceUpdate == true ) return;
	m_Edit2.GetWindowText( szStr );
	nValue = atoi( szStr );
	if( nValue < m_Slider.GetRangeMin() ) {
		nValue = m_Slider.GetRangeMin();
		bFlag = true;
	}
	else if( nValue > m_Slider.GetRangeMax() ) {
		nValue = m_Slider.GetRangeMax();
		bFlag = true;
	}

	m_Slider.SetEndPos( nValue );
	if( bFlag ) {
		szStr.Format( "%d", nValue );
		m_Edit2.SetWindowText( szStr );
	}

	if( GetParent() ) {
		irange range( m_Slider.GetPos(), nValue );
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)&range );
	}
}
void CNumMiniSliderRange::OnEnable(BOOL bEnable)
{
	m_Edit.EnableWindow( bEnable );
	m_Edit2.EnableWindow( bEnable );
	m_Static.EnableWindow( bEnable );
	m_Slider.EnableWindow( bEnable );
	m_Spin.EnableWindow( bEnable );
	m_Spin2.EnableWindow( bEnable );
	CWnd::OnEnable( bEnable );
}