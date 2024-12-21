// KeyControlPaneView.cpp : implementation file
//

#include "stdafx.h"
#include "EtEffect2Tool.h"
#include "EtEffectDataContainer.h"
#include "KeyControlPaneView.h"
#include "GlobalValue.h"
#include "AxisRenderObject.h"
#include "PropertyPaneView.h"

// CKeyControlPaneView

IMPLEMENT_DYNCREATE(CKeyControlPaneView, CScrollView)

CKeyControlPaneView::CKeyControlPaneView()
	//: CScrollView(CKeyControlPaneView::IDD)
{
	m_pSliderCtrl = NULL;
	m_dwGridWidth = 10;
	m_dwGridHeight = 16;
	m_dwGridHeightCount = 1;
	m_dwScrollHeight = 25;
	m_pMemDC = NULL;
	m_pdcBitmap = NULL;	
	m_bActivate = false;
	m_overIndex = -1;
}

CKeyControlPaneView::~CKeyControlPaneView()
{
	SAFE_DELETE( m_pSliderCtrl );
	DeleteMemDC();
}

void CKeyControlPaneView::DoDataExchange(CDataExchange* pDX)
{
	CScrollView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKeyControlPaneView, CScrollView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()


// CKeyControlPaneView diagnostics

#ifdef _DEBUG
void CKeyControlPaneView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CKeyControlPaneView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG

void CKeyControlPaneView::Reset()
{
	m_overIndex = -1;
}

void CKeyControlPaneView::CreateSlider()
{
	SAFE_DELETE( m_pSliderCtrl );
	// Create Slider Ctrl
	DWORD dwStyle = TBS_FIXEDLENGTH | TBS_HORZ | TBS_AUTOTICKS | TBS_BOTTOM | WS_CHILD | WS_VISIBLE;

	m_pSliderCtrl = new CSliderCtrl;
	m_pSliderCtrl->Create( dwStyle, CRect( 0, 0, 500, 30 ), this, 10000 );
	m_pSliderCtrl->SetRange( 0, 2 );
}

void CKeyControlPaneView::SetRangeMax( int nMax )
{
	m_pSliderCtrl->SetRange(0, nMax);
	RefreshScroll();
	RefreshSlider();
}

void CKeyControlPaneView::RefreshSlider()
{
	//	CPoint ScrollPos( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ) );
	int nScrollPos = GetScrollPos( SB_HORZ );

	if(!m_pSliderCtrl) {
		return;
	}

	CRect rcRect, rcResize;
	//DWORD dwPrevRight = 0;
	GetClientRect( &rcRect );
	rcRect.bottom = m_dwScrollHeight;
	//		if( pCtrl->GetRangeMin() == 0 && pCtrl->GetRangeMax() == 0 ) continue;

	rcResize = rcRect;
	rcResize.bottom = m_dwScrollHeight;
	rcResize.left = 0;
	rcResize.right = rcResize.left + ( m_dwGridWidth *m_pSliderCtrl->GetRangeMax() ) + 29;
	//dwPrevRight = rcResize.right + 5;

	rcResize.left -= nScrollPos;
	rcResize.right -= nScrollPos;
	m_pSliderCtrl->MoveWindow( rcResize );

	//m_dwLastSliderRight = dwPrevRight - 5;
}


void CKeyControlPaneView::RefreshScroll()
{
	DWORD dwTotalWidth = 0;
	CRect rcRect;

	if( m_pSliderCtrl ) {
		dwTotalWidth += ( m_dwGridWidth * m_pSliderCtrl->GetRangeMax() ) + 5 + 29;
	}

	// Scroll Resize
	CSize TotalSize;
	TotalSize.cx = dwTotalWidth;
	TotalSize.cy = m_dwScrollHeight + ( m_dwGridHeightCount * m_dwGridHeight ) + 1;

	SetScrollSizes( MM_TEXT, TotalSize );
}

void CKeyControlPaneView::OnInitialUpdate()
{
	if(m_bActivate) {
		return;
	}
	m_bActivate = true;
	CScrollView::OnInitialUpdate();
	CGlobalValue::GetInstance().SetKeyControlView( this );

	CreateSlider();
	RefreshScroll();
	RefreshSlider();

	CGlobalValue::GetInstance().InitAxisRender();
	
}

void CKeyControlPaneView::CreateMemDC()
{
	//if( m_pdcBitmap || m_pMemDC ) return;
	DeleteMemDC();

	m_pMemDC = new CDC;
	m_pdcBitmap = new CBitmap;

	CRect rcRect;
	GetClientRect( &rcRect );

	CDC *pDC = GetDC();
	m_pMemDC->CreateCompatibleDC( pDC );
	m_pdcBitmap->CreateCompatibleBitmap( pDC, rcRect.Width(), rcRect.Height() );
	ReleaseDC( pDC );
}

void CKeyControlPaneView::DeleteMemDC()
{
	if( m_pdcBitmap ) {
		m_pdcBitmap->DeleteObject();
		SAFE_DELETE( m_pdcBitmap );
	}
	if( m_pMemDC ) {
		m_pMemDC->DeleteDC();
		SAFE_DELETE( m_pMemDC );
	}
}

void CKeyControlPaneView::DrawGrid()
{
	CRect rcSliderRect;
	CPoint Point[2];

	m_pSliderCtrl->GetWindowRect( &rcSliderRect );
	ScreenToClient( &rcSliderRect );
	if( m_pSliderCtrl->GetRangeMin() == 0 && m_pSliderCtrl->GetRangeMax() == 0 ) return;

	COLORREF Col;	
	Col = RGB( 64, 64, 64 );
	CPen Pen( PS_DOT, 1, Col );

	CPen PenBold( PS_SOLID, 2, RGB( GetRValue(Col) * 0.8f, GetGValue(Col) * 0.8f, GetBValue(Col) * 0.8f ) );

	CPen PenTick( PS_SOLID, 2, Col );
	CPen PenCurPos( PS_SOLID, 2, Col );

	//CPoint ScrollPos( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ) );
	//rcSliderRect.left -= GetScrollPos( SB_HORZ );
	//rcSliderRect.right -= GetScrollPos( SB_HORZ );
	rcSliderRect.top -= GetScrollPos( SB_VERT );
	rcSliderRect.bottom -= GetScrollPos( SB_VERT );
	// 세로선 그려주구~
	int nCount = m_pSliderCtrl->GetRangeMax() - m_pSliderCtrl->GetRangeMin();
	int nFrameCount = nCount;
	// 현제 프레임 라인 그려주구..
	int nCurPos = m_pSliderCtrl->GetPos();
	int m_dwTickCount = 10;
	for( int i=-1; i<nCount; i++ ) {
		if( i == -1 ) {
			Point[0] = CPoint( rcSliderRect.left + 13, rcSliderRect.top );
			Point[1] = CPoint( rcSliderRect.left + 13, rcSliderRect.top + ( (m_dwGridHeightCount) * m_dwGridHeight ) );
			m_pMemDC->SelectObject( &PenBold );
		}
		else if( i == nCount - 1 ) {
			Point[0] = CPoint( rcSliderRect.right - 14, rcSliderRect.top );
			Point[1] = CPoint( rcSliderRect.right - 14, rcSliderRect.top + ( (m_dwGridHeightCount) * m_dwGridHeight ) );
			m_pMemDC->SelectObject( &PenBold );
		}
		else {
			Point[0] = CPoint( rcSliderRect.left + m_pSliderCtrl->GetTicPos(i), rcSliderRect.top );
			Point[1] = CPoint( rcSliderRect.left + m_pSliderCtrl->GetTicPos(i), rcSliderRect.top + ( (m_dwGridHeightCount) * m_dwGridHeight ) );
			if( i != 0 && (i+1) % m_dwTickCount == 0 )
				m_pMemDC->SelectObject( &PenTick );
			else m_pMemDC->SelectObject( &Pen );
		}
		if( nCurPos - 1 == i ) {
			//m_pMemDC->SelectObject( &PenCurPos );
		}
		//		Point[0] -= ScrollPos;
		//		Point[1] -= ScrollPos;
		m_pMemDC->Polyline( Point, 2 );
	}

	// 가로선 그려주구~
	nCount = (m_dwGridHeightCount);
	for( int i=0; i<nCount+1; i++ ) {
		if( i == 0 || i == nCount ) m_pMemDC->SelectObject( &PenBold );
		else m_pMemDC->SelectObject( &Pen );
		Point[0] = CPoint( rcSliderRect.left + 13, rcSliderRect.top + ( i * m_dwGridHeight ) );
		Point[1] = CPoint( rcSliderRect.right - 14, rcSliderRect.top + ( i * m_dwGridHeight ) );
		//		Point[0] -= ScrollPos;
		//		Point[1] -= ScrollPos;
		m_pMemDC->Polyline( Point, 2 );
	}
}

void CKeyControlPaneView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CScrollView::OnPaint() for painting messages
	CreateMemDC();

	CRect rcRect;
	CBitmap *pOldBitmap;
	GetClientRect( &rcRect );

	pOldBitmap = m_pMemDC->SelectObject( m_pdcBitmap );
	m_pMemDC->FillSolidRect( &rcRect, RGB(255,255,255) );
	// Draw memDC
	DrawGrid();	
	DrawItems();
	ReleaseDC( m_pMemDC );
	dc.BitBlt(rcRect.left, rcRect.top + m_dwScrollHeight, rcRect.Width(), rcRect.Height() ,
		m_pMemDC, rcRect.left, rcRect.top, SRCCOPY);
	m_pMemDC->SelectObject(pOldBitmap);
	

	//TRACE("OnPaint()\n");
}

void CKeyControlPaneView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
}

void CKeyControlPaneView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	RefreshScroll();
	RefreshSlider();
	//DeleteMemDC();
	//CreateMemDC();
	// TODO: Add your message handler code here
}

void CKeyControlPaneView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mousePoint = point;
	Invalidate(false);
	CScrollView::OnMouseMove(nFlags, point);
}

void CKeyControlPaneView::OnFXSelected( CString str )
{
	SetRangeMax( CGlobalValue::GetInstance().FindItem( str )->dwEndFrame -
						CGlobalValue::GetInstance().FindItem( str )->dwStartFrame );

	m_ppTable = CGlobalValue::GetInstance().GetFXData( str );

	Invalidate();
}

RECT CKeyControlPaneView::GetKeyframeRect( int index )
{
	RECT rcSliderRect;
	m_pSliderCtrl->GetWindowRect( &rcSliderRect );
	ScreenToClient( &rcSliderRect );
	rcSliderRect.top -= GetScrollPos( SB_VERT );
	rcSliderRect.bottom -= GetScrollPos( SB_VERT );

	RECT rcSelect;
	int p1 = (index == 0) ? rcSliderRect.left + 13 : rcSliderRect.left + m_pSliderCtrl->GetTicPos(index-1);
	rcSelect.left = p1;
	rcSelect.top = rcSliderRect.top;
	rcSelect.right = rcSelect.left + m_dwGridWidth;
	rcSelect.bottom = rcSelect.top + m_dwGridHeight;
	return rcSelect;
}

void CKeyControlPaneView::DrawItems()
{
	if( CGlobalValue::GetInstance().GetPosOrRot() == 0 ) {
		CEtEffectDataContainer *pTable = m_ppTable[ERT_POSITION_TABLE];
		for( DWORD i = 0; i < pTable->GetDataCount(); i++) {
			CEtEffectPositionDataValue *pPosTable = (CEtEffectPositionDataValue*)pTable->GetValueFromIndex(i);
			RECT rcSelect = GetKeyframeRect( (int)pPosTable->GetKey() );
			m_pMemDC->FillSolidRect( &rcSelect, m_overIndex == pPosTable->GetKey() ?
																RGB( 255, 64, 64) : RGB( 128, 128, 255)  );
			m_pMemDC->DrawText( "P", &rcSelect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
		}
	}
	else if( CGlobalValue::GetInstance().GetPosOrRot() == 1 ) {
		CEtEffectDataContainer *pTable = m_ppTable[ERT_ROTATE_TABLE];	
		for( DWORD i = 0; i < pTable->GetDataCount(); i++) {
			CEtEffectRotationDataValue *pRotTable = (CEtEffectRotationDataValue*)pTable->GetValueFromIndex(i);
			RECT rcSelect = GetKeyframeRect( (int)pRotTable->GetKey() );
			m_pMemDC->FillSolidRect( &rcSelect, m_overIndex == pRotTable->GetKey() ?
																RGB( 255, 64, 64) : RGB(128, 255, 128) );
			m_pMemDC->DrawText( "R", &rcSelect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
		}
	}
}

int CKeyControlPaneView::GetMouseOverIndex() 
{
	RECT rcSliderRect;
	m_pSliderCtrl->GetWindowRect( &rcSliderRect );
	ScreenToClient( &rcSliderRect );
	rcSliderRect.top -= GetScrollPos( SB_VERT );
	rcSliderRect.bottom -= GetScrollPos( SB_VERT );
	int nFrameCount = m_pSliderCtrl->GetRangeMax() - m_pSliderCtrl->GetRangeMin();
	int selectIndex = -1;

	if( m_mousePoint.y > 25 && m_mousePoint.y <= 25 + (int)m_dwGridHeight &&
		m_mousePoint.x > rcSliderRect.left + 13 &&
		m_mousePoint.x < rcSliderRect.right - 14 ) {
		for( int i = 0; i < nFrameCount; i++) {
			int p1 = (i == 0) ? rcSliderRect.left + 13 : rcSliderRect.left + m_pSliderCtrl->GetTicPos(i-1);
			int p2 = (i == nFrameCount-1) ? rcSliderRect.right - 14 : rcSliderRect.left + m_pSliderCtrl->GetTicPos(i);
			if( m_mousePoint.x >= p1 && m_mousePoint.x < p2 ) {
				selectIndex = i;
				break;
			}
		}
	}
	return selectIndex;
}

// 이미 있으면 스킵, 없으면 추가
void CKeyControlPaneView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default	
	m_overIndex = GetMouseOverIndex();
	if( m_overIndex == -1) {
		CGlobalValue::GetInstance().SetPositionPtr( NULL );
		CGlobalValue::GetInstance().SetRotationPtr( NULL );
	}

	if( m_overIndex != -1 ) {		
		if( CGlobalValue::GetInstance().GetPosOrRot() == 0 ) {
			CEtEffectDataContainer *pTable = m_ppTable[ERT_POSITION_TABLE];
			DWORD i;
			for( i = 0; i < pTable->GetDataCount(); i++) {
				if( pTable->GetValueFromIndex(i)->GetKey() == m_overIndex ) {
					break;
				}
			}
			if( i == pTable->GetDataCount() ) {
				CEtEffectPositionDataValue *pPosTableValue = new CEtEffectPositionDataValue;
				pPosTableValue->SetKey( (float)m_overIndex );
				pPosTableValue->SetPosition( CGlobalValue::GetInstance().GetAxisRenderObject()->GetPosition() );

				CGlobalValue::GetInstance().SetPositionPtr( pPosTableValue->GetPosition() );
				pTable->AddData( pPosTableValue, true );
			}
		}
		else if ( CGlobalValue::GetInstance().GetPosOrRot() == 1 ) {
			CEtEffectDataContainer *pTable = m_ppTable[ERT_ROTATE_TABLE];
			DWORD i;
			for( i = 0; i < pTable->GetDataCount(); i++) {
				if( pTable->GetValueFromIndex(i)->GetKey() == m_overIndex ) {
					break;
				}
			}
			if( i == pTable->GetDataCount() ) {
				CEtEffectRotationDataValue *pRotTableValue = new CEtEffectRotationDataValue;
				pRotTableValue->SetKey( (float)m_overIndex );
				pRotTableValue->SetRotation( CGlobalValue::GetInstance().GetAxisRenderObject()->GetRotation() );
				pTable->AddData( pRotTableValue, true );
			}
		}
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
	Invalidate(FALSE);
}

void CKeyControlPaneView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_overIndex = GetMouseOverIndex();
	if( m_overIndex == -1) {
		CGlobalValue::GetInstance().SetPositionPtr( NULL );
		CGlobalValue::GetInstance().SetRotationPtr( NULL );
	}

	if( CGlobalValue::GetInstance().GetPosOrRot() == 0 )  {
		CEtEffectDataContainer *pTable = m_ppTable[ERT_POSITION_TABLE];
		for( DWORD i = 0; i < pTable->GetDataCount(); i++) {
			CEtEffectPositionDataValue *pPosTable = (CEtEffectPositionDataValue*)pTable->GetValueFromIndex(i);
			if( (int)pPosTable->GetKey() == m_overIndex ) {
				CGlobalValue::GetInstance().GetAxisRenderObject()->SetPosition( *pPosTable->GetPosition() );
				CGlobalValue::GetInstance().SetPositionPtr( pPosTable->GetPosition() );
				CGlobalValue::GetInstance().GetPropertyPaneView()->OnPositionSelected( *pPosTable->GetPosition() );
				break;
			}
		}
	}
	else if( CGlobalValue::GetInstance().GetPosOrRot() == 1 )  {
		CEtEffectDataContainer *pTable = m_ppTable[ERT_ROTATE_TABLE];
		for( DWORD i = 0; i < pTable->GetDataCount(); i++) {
			CEtEffectRotationDataValue *pRotTable = (CEtEffectRotationDataValue*)pTable->GetValueFromIndex(i);
			if( (int)pRotTable->GetKey() == m_overIndex ) {
				CGlobalValue::GetInstance().GetAxisRenderObject()->SetRotation( *pRotTable->GetRotation() );
				CGlobalValue::GetInstance().SetRotationPtr( pRotTable->GetRotation() );
				CGlobalValue::GetInstance().GetPropertyPaneView()->OnRotationSelected( *pRotTable->GetRotation() );
				break;
			}
		}
	}
	Invalidate();
	CScrollView::OnLButtonDown(nFlags, point);
}

// 키프레임 제거
void CKeyControlPaneView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int selectIndex = GetMouseOverIndex();
	if( selectIndex != -1 ) {
		if( CGlobalValue::GetInstance().GetPosOrRot() == 0 ) {
			CEtEffectDataContainer *pTable = m_ppTable[ERT_POSITION_TABLE];
			DWORD i;
			for( i = 0; i < pTable->GetDataCount(); i++) {
				if( pTable->GetValueFromIndex( i )->GetKey() == selectIndex ) {
					pTable->RemoveData( i );
					break;
				}
			}
		}
		else if ( CGlobalValue::GetInstance().GetPosOrRot() == 1 ) {
			CEtEffectDataContainer *pTable = m_ppTable[ERT_ROTATE_TABLE];
			DWORD i;
			for( i = 0; i < pTable->GetDataCount(); i++) {
				if( pTable->GetValueFromIndex( i )->GetKey() == selectIndex ) {
					pTable->RemoveData( i );
					break;
				}
			}
		}
	}
	Invalidate(FALSE);
	CScrollView::OnRButtonDblClk(nFlags, point);
}
