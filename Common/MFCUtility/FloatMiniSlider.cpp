// NumMiniSlider.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FloatMiniSlider.h"


// CFloatMiniSlider

IMPLEMENT_DYNAMIC(CFloatMiniSlider, CWnd)

CFloatMiniSlider::CFloatMiniSlider()
{
	m_bForceUpdate = false;
}

CFloatMiniSlider::~CFloatMiniSlider()
{
	m_Static.DestroyWindow();
	m_Edit.DestroyWindow();
	m_Slider.DestroyWindow();
	m_Spin.DestroyWindow();
}


BEGIN_MESSAGE_MAP(CFloatMiniSlider, CWnd)
	ON_WM_CREATE()
	ON_MESSAGE( UM_MINISLIDER_UPDATE_POS, OnUpdatePosSliderEx )
	ON_EN_UPDATE(1000, OnEnUpdateEdit)
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CFloatMiniSlider �޽��� ó�����Դϴ�.



int CFloatMiniSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
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
	m_Slider.SetUnitType( CMiniSliderEx::UT_FLOAT );


	SetRange( GetRangeMin(), GetRangeMax() );
	Invalidate();

	return 0;
}

void CFloatMiniSlider::SetRange( float fMin, float fMax )
{
	m_Slider.SetRange( fMin, fMax );
	m_Spin.SetRange32( (int)fMin, (int)fMax );
}

void CFloatMiniSlider::SetRangeMin( float fValue )
{
	m_Slider.SetRangeMin( fValue );
	m_Spin.SetRange32( (int)fValue, (int)GetRangeMax() );
}

void CFloatMiniSlider::SetRangeMax( float fValue )
{
	m_Slider.SetRangeMax( fValue );
	m_Spin.SetRange32( (int)GetRangeMin(), (int)fValue );
}


void CFloatMiniSlider::SetPos( float fValue )
{
	m_Slider.SetPos( fValue );

	CString szStr;
	szStr.Format( "%.2f", fValue );

	m_bForceUpdate = true;
	m_Edit.SetWindowText( szStr );
	m_bForceUpdate = false;
}


LRESULT CFloatMiniSlider::OnUpdatePosSliderEx( WPARAM wParam, LPARAM lParam )
{
	float fPos = *(float*)lParam;
	CString szStr;
	szStr.Format( "%.2f", fPos );
	m_Edit.SetWindowText( szStr );

	if( GetParent() )
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)lParam );
	return S_OK;
}


void CFloatMiniSlider::OnEnUpdateEdit()
{
	CString szStr;
	float fValue;
	bool bFlag = false;
	if( !m_Slider ) return;

	if( m_bForceUpdate == true ) return;
	m_Edit.GetWindowText( szStr );
	fValue = (float)atof( szStr );
	if( fValue < m_Slider.GetRangeMinFloat() ) {
		fValue = m_Slider.GetRangeMinFloat();
		bFlag = true;
	}
	else if( fValue > m_Slider.GetRangeMaxFloat() ) {
		fValue = m_Slider.GetRangeMaxFloat();
		bFlag = true;
	}

	m_Slider.SetPos( fValue );
	if( bFlag ) {
		szStr.Format( "%.2f", fValue );
		m_Edit.SetWindowText( szStr );
	}

	if( GetParent() )
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)&fValue );
}

void CFloatMiniSlider::OnEnable(BOOL bEnable)
{
	m_Edit.EnableWindow( bEnable );
	m_Static.EnableWindow( bEnable );
	m_Slider.EnableWindow( bEnable );
	m_Spin.EnableWindow( bEnable );
	CWnd::OnEnable( bEnable );
}