// NumMiniSlider.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FloatMiniSliderRange.h"


// CFloatMiniSliderRange

IMPLEMENT_DYNAMIC(CFloatMiniSliderRange, CWnd)

CFloatMiniSliderRange::CFloatMiniSliderRange()
{
	m_bForceUpdate = false;
}

CFloatMiniSliderRange::~CFloatMiniSliderRange()
{
	m_Static.DestroyWindow();
	m_Edit.DestroyWindow();
	m_Edit2.DestroyWindow();
	m_Slider.DestroyWindow();
	m_Spin.DestroyWindow();
	m_Spin2.DestroyWindow();
}


BEGIN_MESSAGE_MAP(CFloatMiniSliderRange, CWnd)
	ON_WM_CREATE()
	ON_MESSAGE( UM_MINISLIDER_UPDATE_POS, OnUpdatePosSliderEx )
	ON_EN_UPDATE(1000, OnEnUpdateEdit)
	ON_EN_UPDATE(1003, OnEnUpdateEdit2)
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CFloatMiniSliderRange 메시지 처리기입니다.



int CFloatMiniSliderRange::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	m_Slider.SetUnitType( CMiniSliderEx::UT_FLOAT );
	m_Slider.SetSliderType( CMiniSliderEx::ST_RANGE );


	SetRange( GetRangeMin(), GetRangeMax() );
	Invalidate();

	return 0;
}


void CFloatMiniSliderRange::SetRange( float fMin, float fMax )
{
	m_Slider.SetRange( fMin, fMax );
	m_Spin.SetRange32( (int)fMin, (int)fMax );
	m_Spin2.SetRange32( (int)fMin, (int)fMax );
}

void CFloatMiniSliderRange::SetRangeMin( float fValue )
{
	m_Slider.SetRangeMin( fValue );
	m_Spin.SetRange32( (int)fValue, (int)GetRangeMax() );
	m_Spin2.SetRange32( (int)fValue, (int)GetRangeMax() );
}

void CFloatMiniSliderRange::SetRangeMax( float fValue )
{
	m_Slider.SetRangeMax( fValue );
	m_Spin.SetRange32( (int)GetRangeMin(), (int)fValue );
	m_Spin2.SetRange32( (int)GetRangeMin(), (int)fValue );
}

frange CFloatMiniSliderRange::GetPos()
{
	return frange( m_Slider.GetPosFloat(), m_Slider.GetEndPosFloat() );
}

void CFloatMiniSliderRange::SetPos( frange range )
{
	m_Slider.SetPos( range.fMin );
	m_Slider.SetEndPos( range.fMax );

	CString szStr;

	m_bForceUpdate = true;

	szStr.Format( "%.2f", range.fMin );
	m_Edit.SetWindowText( szStr );

	szStr.Format( "%.2f", range.fMax );
	m_Edit2.SetWindowText( szStr );

	m_bForceUpdate = false;
}


LRESULT CFloatMiniSliderRange::OnUpdatePosSliderEx( WPARAM wParam, LPARAM lParam )
{
	frange range = *(frange*)lParam;
	CString szStr;
	szStr.Format( "%.2f", range.fMin );
	m_Edit.SetWindowText( szStr );

	szStr.Format( "%.2f", range.fMax );
	m_Edit2.SetWindowText( szStr );


	if( GetParent() )
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)lParam );
	return S_OK;
}


void CFloatMiniSliderRange::OnEnUpdateEdit()
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

	if( GetParent() ) {
		frange range( fValue, m_Slider.GetEndPosFloat() );
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)&range );
	}
}

void CFloatMiniSliderRange::OnEnUpdateEdit2()
{
	CString szStr;
	float fValue;
	bool bFlag = false;
	if( !m_Slider ) return;

	if( m_bForceUpdate == true ) return;
	m_Edit2.GetWindowText( szStr );
	fValue = (float)atof( szStr );
	if( fValue < m_Slider.GetRangeMinFloat() ) {
		fValue = m_Slider.GetRangeMinFloat();
		bFlag = true;
	}
	else if( fValue > m_Slider.GetRangeMaxFloat() ) {
		fValue = m_Slider.GetRangeMaxFloat();
		bFlag = true;
	}

	m_Slider.SetEndPos( fValue );
	if( bFlag ) {
		szStr.Format( "%.2f", fValue );
		m_Edit2.SetWindowText( szStr );
	}

	if( GetParent() ) {
		frange range( m_Slider.GetPosFloat(), fValue );
		GetParent()->SendMessage( UM_NUMMINISLIDER_UPDATE, (WPARAM)m_hWnd, (LPARAM)&range );
	}
}
void CFloatMiniSliderRange::OnEnable(BOOL bEnable)
{
	m_Edit.EnableWindow( bEnable );
	m_Edit2.EnableWindow( bEnable );
	m_Static.EnableWindow( bEnable );
	m_Slider.EnableWindow( bEnable );
	m_Spin.EnableWindow( bEnable );
	m_Spin2.EnableWindow( bEnable );
	CWnd::OnEnable( bEnable );
}