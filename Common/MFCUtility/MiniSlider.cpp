// EditSlider.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MiniSlider.h"


// CMiniSlider

IMPLEMENT_DYNAMIC(CMiniSlider, CWnd)

CMiniSlider::CMiniSlider()
{
	m_fPos = 25.f;
	m_bLButtonDown = false;
}

CMiniSlider::~CMiniSlider()
{
}


BEGIN_MESSAGE_MAP(CMiniSlider, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


LRESULT CMiniSlider::OnSetPos( WPARAM wParam, LPARAM lParam )
{
	float *pPos = (float *)wParam;
	SetPos( *pPos );
	return S_OK;
}

// CMiniSlider 메시지 처리기입니다.
void CMiniSlider::SetPos( float fValue )
{
	m_fPos = fValue;
	if( m_fPos <= 0.f ) m_fPos = 0.f;
	if( m_fPos >= 100.f) m_fPos = 100.f;
	CWnd *pWnd = GetParent();
	if( pWnd ) pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)&m_fPos );
	Invalidate();
}

void CMiniSlider::OnPaint()
{
	CPaintDC dc(this);

	CRect rcRect;
	GetClientRect( &rcRect );

	dc.Draw3dRect( &rcRect, RGB(0,0,0), RGB(255,255,255) );

	rcRect.DeflateRect( 1, 1, 1, 1 );
	dc.FillSolidRect( &rcRect, RGB( 255, 255, 255 ) );

	rcRect.DeflateRect( 1, 1, 1, 1 );

	float fLength = (float)rcRect.Width();
	rcRect.right = rcRect.left + (int)( ( fLength / 100.f ) * m_fPos );

	DWORD dwColor = ( IsWindowEnabled() == TRUE ) ? RGB( 80, 80, 80 ) : RGB( 180, 180, 180 );
	dc.FillSolidRect( &rcRect, dwColor );
}

void CMiniSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_bLButtonDown == true ) {
		CRect rcRect;
		GetClientRect( &rcRect );
		ClientToScreen( &rcRect );
		ClipCursor( &rcRect );

		GetClientRect( &rcRect );

		int nPos = point.x;
		int nWidth = rcRect.Width() - 4;
		if( nPos <= 2 ) nPos = 0;
		if( nPos >= rcRect.Width() - 2 ) nPos = rcRect.Width() - 2;

		float fPos = 100.f / (float)nWidth * nPos;
		SetPos( fPos );
	}

	CWnd::OnMouseMove(nFlags, point);

}

void CMiniSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bLButtonDown = true;

	CRect rcRect;
	GetClientRect( &rcRect );
	ClientToScreen( &rcRect );
	ClipCursor( &rcRect );

	GetClientRect( &rcRect );
	int nPos = point.x;
	int nWidth = rcRect.Width() - 4;
	if( nPos <= 2 ) nPos = 0;
	if( nPos >= rcRect.Width() - 2 ) nPos = rcRect.Width() - 2;

	float fPos = 100.f / (float)nWidth * nPos;
	SetPos( fPos );

	CWnd::OnLButtonDown(nFlags, point);
}

void CMiniSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bLButtonDown = false;
	ClipCursor( NULL );

	CWnd::OnLButtonUp(nFlags, point);
}

