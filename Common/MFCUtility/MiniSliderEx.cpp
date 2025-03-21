#include "stdafx.h"
#include "MiniSliderEx.h"


// CMiniSliderEx

IMPLEMENT_DYNAMIC(CMiniSliderEx, CWnd)

CMiniSliderEx::CMiniSliderEx( SLIDER_TYPE Type, UNIT_TYPE UnitType )
{
	m_Type = Type;
	m_UnitType = UnitType;
	m_nPos = m_nEndPos = 0;
	m_nRangeMin = 0;
	m_nRangeMax = 100;
	m_bLButtonDown = false;
	m_nSendRootParent = 0;
}

CMiniSliderEx::~CMiniSliderEx()
{
}


BEGIN_MESSAGE_MAP(CMiniSliderEx, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


LRESULT CMiniSliderEx::OnSetPos( WPARAM wParam, LPARAM lParam )
{
	SetPos( (int)wParam );
	return S_OK;
}

// CMiniSliderEx 메시지 처리기입니다.
void CMiniSliderEx::SetPos( int nValue )
{
	m_nPos = nValue;
	if( m_nPos < m_nRangeMin ) m_nPos = m_nRangeMin;
	if( m_nPos > m_nRangeMax) m_nPos = m_nRangeMax;

	Invalidate();
}

void CMiniSliderEx::SetEndPos( int nValue )
{
	m_nEndPos = nValue;

	if( m_nEndPos < m_nRangeMin ) m_nEndPos = m_nRangeMin;
	if( m_nEndPos > m_nRangeMax) m_nEndPos = m_nRangeMax;

	Invalidate();
}

void CMiniSliderEx::SetPos( float fValue )
{
	m_fPos = fValue;
	if( m_fPos < m_fRangeMin ) m_fPos = m_fRangeMin;
	if( m_fPos > m_fRangeMax) m_fPos = m_fRangeMax;

	Invalidate();
}

void CMiniSliderEx::SetEndPos( float fValue )
{
	m_fEndPos = fValue;

	if( m_fEndPos < m_fRangeMin ) m_fEndPos = m_fRangeMin;
	if( m_fEndPos > m_fRangeMax) m_fEndPos = m_fRangeMax;

	Invalidate();
}


void CMiniSliderEx::OnPaint()
{
	CPaintDC dc(this);

	CRect rcRect;
	GetClientRect( &rcRect );

	dc.Draw3dRect( &rcRect, RGB(0,0,0), RGB(255,255,255) );

	rcRect.DeflateRect( 1, 1, 1, 1 );
	dc.FillSolidRect( &rcRect, RGB( 255, 255, 255 ) );

	rcRect.DeflateRect( 1, 1, 1, 1 );

	switch( m_Type ) {
		case ST_NORMAL:
			{
				float fLength = (float)rcRect.Width();
				float fRange;
				switch( m_UnitType ) {
					case UT_INT:
						fRange = (float)( m_nRangeMax - m_nRangeMin );
						rcRect.right = rcRect.left + (int)( ( fLength / fRange ) * ( m_nPos - m_nRangeMin ) );
						break;
					case UT_FLOAT:
						fRange = (float)( m_fRangeMax - m_fRangeMin );
						rcRect.right = rcRect.left + (int)( ( fLength / fRange ) * ( m_fPos - m_fRangeMin ) );
						break;
				}				

				DWORD dwColor = ( IsWindowEnabled() == TRUE ) ? RGB( 80, 80, 80 ) : RGB( 180, 180, 180 );
				dc.FillSolidRect( &rcRect, dwColor );
			}
			break;
		case ST_RANGE:
			{
				float fLength = (float)rcRect.Width();
				float fRange;

				rcRect.right = rcRect.left;
				switch( m_UnitType ) {
					case UT_INT:
						fRange = (float)( m_nRangeMax - m_nRangeMin );
						rcRect.left += (int)( ( fLength / fRange ) * ( m_nPos - m_nRangeMin ) );
						rcRect.right += (int)( ( fLength / fRange ) * ( m_nEndPos - m_nRangeMin ) );
						break;
					case UT_FLOAT:
						fRange = (float)( m_fRangeMax - m_fRangeMin );
						rcRect.left += (int)( ( fLength / fRange ) * ( m_fPos - m_fRangeMin ) );
						rcRect.right += (int)( ( fLength / fRange ) * ( m_fEndPos - m_fRangeMin ) );
						break;
				}


				DWORD dwColor = ( IsWindowEnabled() == TRUE ) ? RGB( 80, 80, 80 ) : RGB( 180, 180, 180 );
				dc.FillSolidRect( &rcRect, dwColor );
			}
			break;
	}
}

void CMiniSliderEx::OnMouseMove(UINT nFlags, CPoint point)
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
		else if( nPos >= rcRect.Width() - 2 ) nPos = rcRect.Width() - 2;
		else nPos -= 2;

		float fPos = ( ( 100.f / (float)nWidth ) * nPos ) / 100.f;

		CWnd *pWnd = GetParent();
		for( int i=0; i<m_nSendRootParent; i++ ) {
			if( pWnd ) pWnd = pWnd->GetParent();
		}
		if( pWnd ) {
			switch( m_Type ) {
				case ST_NORMAL:
					switch( m_UnitType ) {
						case UT_INT:
							{
								int nCurPos = m_nRangeMin + (int)( ( m_nRangeMax - m_nRangeMin ) * fPos );
								SetPos( nCurPos );
								pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)m_nPos );
							}
							break;
						case UT_FLOAT:
							{
								float fCurPos = m_fRangeMin + ( ( m_fRangeMax - m_fRangeMin ) * fPos );
								SetPos( fCurPos );
								pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)&m_fPos );
							}
							break;
					}
					break;
				case ST_RANGE:
					switch( m_UnitType ) {
						case UT_INT:
							{
								int nCurPos = m_nRangeMin + (int)( ( m_nRangeMax - m_nRangeMin ) * fPos );
								if( nCurPos > m_nRangeMax ) nCurPos = m_nRangeMax;
								m_nEndPos = nCurPos;
								irange range( m_nPos, m_nEndPos );
								if( range.nMin > range.nMax ) {
									_Swap( m_nPos, m_nEndPos );
									_Swap( range.nMin, range.nMax );
								}
								SetEndPos( m_nEndPos );

								pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)&range );
							}
							break;
						case UT_FLOAT:
							{
								float fCurPos = m_fRangeMin + ( ( m_fRangeMax - m_fRangeMin ) * fPos );
								if( fCurPos > m_fRangeMax ) fCurPos = m_fRangeMax;
								m_fEndPos = fCurPos;
								frange range( m_fPos, m_fEndPos );
								if( range.fMin > range.fMax ) {
									_Swap( m_fPos, m_fEndPos );
									_Swap( range.fMin, range.fMax );
								}
								SetEndPos( m_fEndPos );
								pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)&range );
							}
							break;
					}

					break;
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);

}

void CMiniSliderEx::OnLButtonDown(UINT nFlags, CPoint point)
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
	else if( nPos >= rcRect.Width() - 2 ) nPos = rcRect.Width() - 2;
	else nPos -= 2;

	float fPos = ( ( 100.f / (float)nWidth ) * nPos ) / 100.f;


	CWnd *pWnd = GetParent();
	for( int i=0; i<m_nSendRootParent; i++ ) {
		if( pWnd ) pWnd = pWnd->GetParent();
	}
	if( pWnd ) {
		switch( m_Type ) {
			case ST_NORMAL:
				switch( m_UnitType ) {
					case UT_INT:
						{
							int nCurPos = m_nRangeMin + (int)( ( m_nRangeMax - m_nRangeMin ) * fPos );
							SetPos( nCurPos );
							pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)m_nPos );
						}
						break;
					case UT_FLOAT:
						{
							float fCurPos = m_fRangeMin + ( ( m_fRangeMax - m_fRangeMin ) * fPos );
							SetPos( fCurPos );
							pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)&m_fPos );
						}
						break;
				}
				break;

			case ST_RANGE:
				switch( m_UnitType ) {
					case UT_INT:
						{
							int nCurPos = m_nRangeMin + (int)( ( m_nRangeMax - m_nRangeMin ) * fPos );
							SetPos( nCurPos );
							SetEndPos( nCurPos );
							irange range( m_nPos, m_nEndPos );
							if( range.nMin > range.nMax ) _Swap( range.nMin, range.nMax );
							pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)&range );
						}
						break;
					case UT_FLOAT:
						{
							float fCurPos = m_fRangeMin + ( ( m_fRangeMax - m_fRangeMin ) * fPos );
							SetPos( fCurPos );
							SetEndPos( fCurPos );
							frange range( m_fPos, m_fEndPos );
							if( range.fMin > range.fMax ) _Swap( range.fMin, range.fMax );
							pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)m_hWnd, (LPARAM)&range );
						}
						break;
				}
				break;
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CMiniSliderEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bLButtonDown = false;
	ClipCursor( NULL );

	CWnd::OnLButtonUp(nFlags, point);
}


int CMiniSliderEx::GetTicPos( int nIndex )
{
	CRect rcRect;
	GetClientRect( &rcRect );

	float fLength = (float)rcRect.Width();
	float fRange;
	switch( m_UnitType ) {
		case UT_INT:
			fRange = (float)( m_nRangeMax - m_nRangeMin );
			rcRect.right = rcRect.left + (int)( ( fLength / fRange ) * ( nIndex - m_nRangeMin ) );
			break;
		case UT_FLOAT:
			fRange = (float)( m_fRangeMax - m_fRangeMin );
			rcRect.right = rcRect.left + (int)( ( fLength / fRange ) * ( (float)nIndex - m_fRangeMin ) );
			break;
	}
	return rcRect.right;
}
