// CSequenceView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SequenceView.h"
#include "GlobalValue.h"
#include "PropertyPaneView.h"
#include "FXPaneView.h"


unsigned long GetHash( const char* pString )
{
	unsigned long i,len;
	unsigned long ch;
	unsigned long result;

	len     = (int)strlen( pString );
	result = 5381;

	for( i=0; i<len; i++ )
	{
		ch = (unsigned long)pString[i];
		result = ((result<< 5) + result) + (ch^123);
	}
	return result;
}


// CSequenceView

IMPLEMENT_DYNCREATE(CSequenceView, CScrollView)

int CSequenceView::s_nMargin = 14;
CSequenceView::CSequenceView()
{
	m_bActivate = false;
	m_pDC = NULL;
	m_pBitmap = NULL;
	m_pSlider = NULL;

	m_GridSize = CPoint(12, 16);
	m_nGridHeightCount = 5;
	m_nScrollHeight = 25;

	m_dwAdditionalGridHeightCount = 0;
	m_bUpdateAdditionalGridHeightCount = true;

	memset( m_Pens, 0, sizeof(m_Pens));
	m_bEnable = true;

	m_bSelectEmpty = false;
	m_SelectEmptyMousePos = CPoint(0,0);

	memset( m_bButton, 0, sizeof(m_bButton) );

	m_pCursor = IDC_ARROW;

	m_nCurSingalMaxRange = 0;
}

CSequenceView::~CSequenceView()
{
	DeleteDC();
	SAFE_DELETE(m_pSlider);
}


BEGIN_MESSAGE_MAP(CSequenceView, CScrollView)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// CSequenceView drawing

void CSequenceView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CGlobalValue::GetInstance().SetSequenceView( this );
	if( m_bActivate == true ) return;
	m_bActivate = true;

	// Create Slider
	m_pSlider = new CSliderCtrl();
	m_pSlider->Create( TBS_FIXEDLENGTH | TBS_HORZ | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_BOTTOM | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 12345 );
	m_pSlider->SetRange(0, 0);

	// Create Pens
	COLORREF color[2] = { RGB(100, 100, 100), RGB(230, 230, 230) };
	for( int i=0; i<2; i++ ) 
	{
		m_Pens[i][0] = new CPen( PS_DOT, 1, color[i] );
		m_Pens[i][1] = new CPen( PS_SOLID, 2, RGB( GetRValue(color[i]) * 0.7f, GetGValue(color[i]) * 0.7f, GetBValue(color[i]) * 0.7f ) );
		m_Pens[i][2] = new CPen( PS_SOLID, 2, color[i] );
		m_Pens[i][3] = new CPen( PS_SOLID, 2, RGB(255,100,100) );
	}
}

void CSequenceView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CSequenceView diagnostics

#ifdef _DEBUG
void CSequenceView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CSequenceView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG

void CSequenceView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not all CScrollView::OnPaint() for painting messages
	CreateDC();

	CRect rcRect;
	CBitmap *pOldBitmap;
	GetClientRect( &rcRect );

	pOldBitmap = m_pDC->SelectObject( m_pBitmap );
	m_pDC->FillSolidRect( &rcRect, RGB(255,255,255) );
	dc.FillSolidRect( &CRect(m_rcSliderRect.right, 0, rcRect.right, rcRect.top + m_nScrollHeight), RGB(212,208,200));

	DrawSequence();
	DrawSignals();
	// Blt
	dc.BitBlt(rcRect.left, rcRect.top + m_nScrollHeight, rcRect.Width(), rcRect.Height(), m_pDC, rcRect.left, rcRect.top, SRCCOPY);
	m_pDC->SelectObject(pOldBitmap);

}

BOOL CSequenceView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void CSequenceView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	RefreshScroll();
	RefreshSlider();
	// TODO: Add your message handler code here
	DeleteDC();
	CreateDC();
}

void CSequenceView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
	if( pScrollBar ) Invalidate();
	else {
		RefreshSlider();
		Invalidate();
	}
}

void CSequenceView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
	if( pScrollBar ) Invalidate();
	else {
		RefreshSlider();
		Invalidate();
	}
}

BOOL CSequenceView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CScrollView::OnMouseWheel(nFlags, zDelta, pt);

	RefreshSlider();
	RefreshScroll();
	Invalidate();

	return TRUE;
}

bool CSequenceView::ModifySignalStartFrame( CPoint point, int nCurSelectedSignalIndex )
{
	bool bModify = false;
	if( (int)m_GridSize.x < abs( m_PrevMousePosition.x - point.x ) ) 
	{
		int nValue = ( m_PrevMousePosition.x - point.x ) / (int)m_GridSize.x;
		CSequenceSignalBase *pSignal = m_pVecSignalList[nCurSelectedSignalIndex];

		if( pSignal->bFreeze == false && IsMoveSignal( pSignal->nStartFrame - nValue, pSignal->nEndFrame, pSignal->nYOrder ) == true ) 
		{
			pSignal->nStartFrame -= nValue;
			if( pSignal->nEndFrame - pSignal->nStartFrame < m_nCurSingalMaxRange ) {
				pSignal->nStartFrame = pSignal->nEndFrame - m_nCurSingalMaxRange;
			}
			OnModifySignal( pSignal );

			Invalidate();
		}

		bModify = true;
		m_PrevMousePosition.x = point.x + ( ( m_PrevMousePosition.x - point.x ) % (int)m_GridSize.x );
	}
	return bModify;
}

bool CSequenceView::ModifySignalEndFrame( CPoint point, int nCurSelectedSignalIndex )
{
	bool bModify = false;
	if( (int)m_GridSize.x < abs( m_PrevMousePosition.x - point.x ) ) {
		int nValue = ( m_PrevMousePosition.x - point.x ) / (int)m_GridSize.x;
		CSequenceSignalBase *pSignal = m_pVecSignalList[nCurSelectedSignalIndex];
		if( pSignal->bFreeze == false && IsMoveSignal( pSignal->nStartFrame, pSignal->nEndFrame - nValue, pSignal->nYOrder ) == true ) {
			pSignal->nEndFrame -= nValue;
			if( pSignal->nEndFrame - pSignal->nStartFrame < m_nCurSingalMaxRange ) {
				pSignal->nEndFrame = pSignal->nStartFrame + m_nCurSingalMaxRange;
			}
			OnModifySignal( pSignal );
			Invalidate();
		}
		bModify = true;
		m_PrevMousePosition.x = point.x + ( ( m_PrevMousePosition.x - point.x ) % (int)m_GridSize.x );
	}
	return bModify;
}

bool CSequenceView::ModifySignalMove( CPoint point, int nCurSelectedSignalIndex )
{
	bool bModify = false;
	if( (int)m_GridSize.x < abs( m_PrevMousePosition.x - point.x ) ) 
	{
		int nValue = ( m_PrevMousePosition.x - point.x ) / (int)m_GridSize.x;
		CSequenceSignalBase *pSignal = m_pVecSignalList[nCurSelectedSignalIndex];

		if( pSignal->bFreeze == false && IsMoveSignal( pSignal->nStartFrame - nValue, pSignal->nEndFrame - nValue, pSignal->nYOrder ) == true ) {
			pSignal->nStartFrame -= nValue;
			pSignal->nEndFrame -= nValue;
			OnModifySignal( pSignal );
			Invalidate();
		}
		bModify = true;
		m_PrevMousePosition.x = point.x + ( ( m_PrevMousePosition.x - point.x ) % (int)m_GridSize.x );
	}
	if( (int)m_GridSize.y < abs( m_PrevMousePosition.y - point.y ) ) {
		int nValue = ( m_PrevMousePosition.y - point.y ) / (int)m_GridSize.y;
		CSequenceSignalBase *pSignal = m_pVecSignalList[nCurSelectedSignalIndex];

		if( pSignal->bFreeze == false && IsMoveSignal( pSignal->nStartFrame, pSignal->nEndFrame, pSignal->nYOrder - nValue ) == true ) {
			pSignal->nYOrder -= nValue;
			OnModifySignal( pSignal );
			Invalidate();
		}
		bModify = true;
		m_PrevMousePosition.y = point.y + ( ( m_PrevMousePosition.y - point.y ) % (int)m_GridSize.y );
	}
	return bModify;
}


void CSequenceView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CScrollView::OnMouseMove(nFlags, point);

	if( m_bEnable == false ) return;
	if( m_bButton[0] == true ) 
	{
#ifdef _SELECT_MULTIPLE_SIGNAL
		if (m_CurSelectedItems.size() == 1 && GetCapture() == NULL) {
			std::vector<DWORD>::const_iterator selectedItemIter = m_CurSelectedItems.begin();
			DWORD curSelectedItemIndex = (*selectedItemIter);
			if (curSelectedItemIndex < 0 || curSelectedItemIndex >= m_pVecSignalList.size())
			{
				return;
			}
#else
		if( m_nFocusSignalIndex != -1 ) {
#endif
			if( m_PrevMousePosition != point ) 
			{
				bool bModifySignal = false;
				switch( m_cMouseEventFlag ) 
				{
					case 0:
#ifdef _SELECT_MULTIPLE_SIGNAL
						bModifySignal = ModifySignalStartFrame( point, curSelectedItemIndex );
#else
						bModifySignal = ModifySignalStartFrame( point, m_nFocusSignalIndex );
#endif
						break;
					case 1:
#ifdef _SELECT_MULTIPLE_SIGNAL
						bModifySignal = ModifySignalMove( point, curSelectedItemIndex );
#else
						bModifySignal = ModifySignalMove( point, m_nFocusSignalIndex );
#endif
						break;
					case 2:
#ifdef _SELECT_MULTIPLE_SIGNAL
						bModifySignal = ModifySignalEndFrame( point, curSelectedItemIndex );
#else
						bModifySignal = ModifySignalEndFrame( point, m_nFocusSignalIndex );
#endif
						break;
				}
				if( bModifySignal ) 
				{
					CRect rcRect;
					GetClientRect( &rcRect );
					ClientToScreen( &rcRect );
					SetCursorPos( rcRect.left + m_PrevMousePosition.x, rcRect.top + m_PrevMousePosition.y );
				}

			}
		}

#ifdef _SELECT_MULTIPLE_SIGNAL
		if (GetCapture() != NULL)
		{
			CClientDC dc(this);
			CRect drawRect(min(m_PrevMousePosition.x, m_DragEndPoint.x), min(m_PrevMousePosition.y, m_DragEndPoint.y),
				max(m_PrevMousePosition.x, m_DragEndPoint.x), max(m_PrevMousePosition.y, m_DragEndPoint.y));
			dc.DrawFocusRect(drawRect); // rubbing

			m_DragEndPoint = point;
			drawRect.SetRect(min(m_PrevMousePosition.x, m_DragEndPoint.x), min(m_PrevMousePosition.y, m_DragEndPoint.y),
				max(m_PrevMousePosition.x, m_DragEndPoint.x), max(m_PrevMousePosition.y, m_DragEndPoint.y));
			dc.DrawFocusRect(drawRect);
		}
#endif
	}
	else 
	{
		m_pCursor = IDC_ARROW;

		for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) 
		{
			if( m_pVecSignalList[i]->bFreeze == true ) continue;
			char cCursorMode = CheckSignalRect( i, point );
			if( cCursorMode == -1 ) continue;

			switch( cCursorMode ) 
			{
				case 1: m_pCursor = IDC_SIZEALL; break;

				case 2:
				case 0: m_pCursor = IDC_SIZEWE; break;
			}
			break;
		}

		SetCursor( LoadCursor( NULL, m_pCursor ) );
	}
}

void CSequenceView::OnModifySignal( CSequenceSignalBase *pSignal )
{
	Item *globalItem = CGlobalValue::GetInstance().FindItem(pSignal->szDesc);
	if( globalItem )  {
		globalItem->dwStartFrame = pSignal->nStartFrame;
		globalItem->dwEndFrame = pSignal->nEndFrame;
		CGlobalValue::GetInstance().GetPropertyPaneView()->Refresh();
	}
}

void CSequenceView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CScrollView::OnLButtonDown(nFlags, point);

	if (IsValidPropertyPaneState() == false) return;

	// TODO: Add your message handler code here and/or call default
	m_bSelectEmpty = FALSE;
	m_bButton[0] = true;
	m_PrevMousePosition = point;

#ifdef _SELECT_MULTIPLE_SIGNAL
	if( false == IsTabKey() )
		ClearSelectedSignals();

	for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) {
		m_cMouseEventFlag = CheckSignalRect( i, point );
		if( m_cMouseEventFlag == -1 ) continue;

		CSequenceSignalBase* pSignal = m_pVecSignalList[i];
		PushBackSelectedSignalIndex(i);
		OnSelectSignal( m_pVecSignalList[i] );
		Invalidate();
		return;
	}

	CClientDC dc(this);
	m_DragEndPoint = point;
	CRect dragArea(point, m_DragEndPoint);
	dc.DrawFocusRect(dragArea);
	SetCapture();
#else
	m_nFocusSignalIndex = -1;
	for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) 
	{
		m_cMouseEventFlag = CheckSignalRect( i, point );
		if( m_cMouseEventFlag == -1 ) continue;

		CEtEffectDataContainer**ppTable = CGlobalValue::GetInstance().GetFXData( m_pVecSignalList[i]->szDesc );
		m_nCurSingalMaxRange = (int)ppTable[ERT_POSITION_TABLE]->GetMaximumKeyValue()+1;
		m_nCurSingalMaxRange = __max(m_nCurSingalMaxRange, (int)ppTable[ERT_ROTATE_TABLE]->GetMaximumKeyValue()+1);

		m_nFocusSignalIndex = (int)i;
		OnSelectSignal( m_pVecSignalList[m_nFocusSignalIndex] );
		Invalidate();
		return;
	}
#endif
	OnSelectSignal( NULL );
	Invalidate();
}

void CSequenceView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CScrollView::OnLButtonDblClk(nFlags, point);

	if( !CGlobalValue::GetInstance().GetFXPaneView()->IsCanAddParticle() ) {
		return;
	}

	if( m_bEnable == false ) return;
	// TODO: Add your message handler code here and/or call default 
	CPoint GridPos;

	MousePosToGridPos( point, GridPos );
	if( IsMoveSignal( GridPos.x, GridPos.x + 1, GridPos.y, FALSE ) == FALSE ) return;

	CSequenceSignalBase *pSignal = CreateSignal();
	if( pSignal == NULL ) return;
	pSignal->nStartFrame = GridPos.x;
	pSignal->nEndFrame = GridPos.x + 1;
	pSignal->nYOrder = GridPos.y;
	char *pName = CGlobalValue::GetInstance().GetFXPaneView()->GetCurrentSelectName();
	pSignal->szString = pName;	
	DWORD dwHash = GetHash( pName );
	pSignal->Color = RGB( GetRValue(dwHash), GetGValue(dwHash), GetBValue(dwHash) );
	pSignal->InitialColor = RGB( 255-GetGValue(dwHash), 255-GetBValue(dwHash), 255-GetRValue(dwHash) );

	if( InsertSignal( pSignal ) == FALSE ) {
		delete pSignal;
		//		Invalidate();
		return;
	}
	char *pDescName = CGlobalValue::GetInstance().GetFXPaneView()->AddSelectedParticleToGrid();
	pSignal->szDesc = pDescName;
	CGlobalValue::GetInstance().AddItemOnGridView( pName, pDescName, pSignal->nYOrder );

	m_pVecSignalList.push_back( pSignal );

#ifdef _SELECT_MULTIPLE_SIGNAL
	ClearSelectedSignals();
#endif

	Invalidate();
}

void CSequenceView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CScrollView::OnRButtonDown(nFlags, point);
	// TODO: Add your message handler code here and/or call default
	m_PrevMousePosition = point;
	m_bButton[1] = true;

#ifdef _SELECT_MULTIPLE_SIGNAL
	if (m_CurSelectedItems.empty() == false)
	{
		if (IsSelectedSignal(point))
		{
			std::vector<CSequenceSignalBase*> pSelectedItemArray;
			std::vector<DWORD>::const_iterator iter = m_CurSelectedItems.begin();
			for (; iter != m_CurSelectedItems.end(); ++iter)
			{
				const DWORD& selItemIdx = (*iter);
				if (selItemIdx >= (DWORD)m_pVecSignalList.size())
					continue;
				pSelectedItemArray.push_back(m_pVecSignalList[selItemIdx]);
			}

			OnSelectSignalArray(pSelectedItemArray);
			return;
		}
	}
	else
	{
		for (DWORD i=0; i<m_pVecSignalList.size(); i++)
		{
			m_cMouseEventFlag = CheckSignalRect( i, point );
			if( m_cMouseEventFlag == -1 ) continue;

			PushBackSelectedSignalIndex(i);
			OnSelectSignal( m_pVecSignalList[i] );
			Invalidate();
			return;
		}
	}

	ClearSelectedSignals();
#else
	m_nFocusSignalIndex = -1;

	for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) {
		m_cMouseEventFlag = CheckSignalRect( i, point );
		if( m_cMouseEventFlag == -1 ) continue;

		m_nFocusSignalIndex = (int)i;
		OnSelectSignal( m_pVecSignalList[m_nFocusSignalIndex] );
		Invalidate();
		return;
		//		break;
	}
#endif

	OnSelectSignal( NULL );
	Invalidate();
}

void CSequenceView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
#ifdef _SELECT_MULTIPLE_SIGNAL
	ClearSelectedSignals();
#else
	m_nFocusSignalIndex = -1;
#endif
	// TODO: Add your message handler code here and/or call default
	for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) {
		if( m_pVecSignalList[i]->bDestroy == false ) continue;
		m_cMouseEventFlag = CheckSignalRect( i, point );
		if( m_cMouseEventFlag == -1 ) continue;

		if( OnRemoveSignal( m_pVecSignalList[i] ) == false ) continue;

		CGlobalValue::GetInstance().RemoveItemOnGridView( m_pVecSignalList[i]->szDesc.GetBuffer() );

		delete m_pVecSignalList[i];
		m_pVecSignalList.erase( m_pVecSignalList.begin() + i );
		Invalidate();
		break;
	}
	CScrollView::OnRButtonDblClk(nFlags, point);
}

void CSequenceView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bButton[0] = false;

#ifdef _SELECT_MULTIPLE_SIGNAL
	if (GetCapture() != NULL)
	{
		OnDragEnd(m_PrevMousePosition, point);
		m_DragEndPoint.SetPoint(0, 0);
	}

	if( m_CurSelectedItems.empty() )
	{
		m_bSelectEmpty = TRUE;
		m_SelectEmptyMousePos = point;
	}
#endif

	CScrollView::OnLButtonUp(nFlags, point);
}

void CSequenceView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bButton[1] = false;

	CScrollView::OnRButtonUp(nFlags, point);
}

BOOL CSequenceView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	SetCursor( LoadCursor( NULL, m_pCursor ) );

	return TRUE;
}


// SequenceItemBase
CSequenceView::CSequenceSignalBase::CSequenceSignalBase()
{
	szString = "Item";

	nStartFrame = nEndFrame = nYOrder = 0;

	InitialColor = RGB( rand()%256, rand()%256, rand()%256 );
	Color = RGB(rand()%256, rand()%256, rand()%256);

	bFreeze = false;
	bDestroy = true;
	bModifyLength = true;
}

CSequenceView::CSequenceSignalBase::~CSequenceSignalBase()
{
}



// CSequenceView message handlers
void CSequenceView::SetEnable( bool bEnable )
{
	m_bEnable = bEnable;
	m_pSlider->EnableWindow(bEnable);
}

void CSequenceView::CreateDC()
{
	if( m_pBitmap || m_pDC ) return;

	CRect rcRect;

	m_pBitmap = new CBitmap();
	m_pDC = new CDC();

	GetClientRect( &rcRect );

	CDC *pDC = GetDC();

	m_pDC->CreateCompatibleDC(pDC);
	m_pBitmap->CreateCompatibleBitmap(pDC, rcRect.Width(), rcRect.Height());

	ReleaseDC( pDC );
}

void CSequenceView::DeleteDC()
{
	if( m_pBitmap ) {
		m_pBitmap->DeleteObject();
		m_pBitmap = NULL;
	}

	if( m_pDC ) {
		m_pDC->DeleteDC();
		m_pDC = NULL;
	}
}

void CSequenceView::RefreshSlider()
{
	if( m_pSlider == NULL ) return;
	int nScrollPos = GetScrollPos( SB_HORZ );

	GetClientRect( &m_rcSliderRect );
	m_rcSliderRect.left = 0;
	m_rcSliderRect.bottom = m_nScrollHeight;
	m_rcSliderRect.right = m_rcSliderRect.left + ( m_GridSize.x * m_pSlider->GetRangeMax() ) + 29;

	m_rcSliderRect.left -= nScrollPos;
	m_rcSliderRect.right -= nScrollPos;
	m_pSlider->MoveWindow( &m_rcSliderRect );
}

void CSequenceView::RefreshScroll()
{
	if( m_pSlider == NULL ) return;

	CRect rcRect;
	int nSize = 34 + (m_GridSize.x * m_pSlider->GetRangeMax());

	// Scroll Resize
	CSize TotalSize;
	TotalSize.cx = nSize;
	TotalSize.cy = m_nScrollHeight + ( (m_nGridHeightCount + m_dwAdditionalGridHeightCount) * m_GridSize.y ) + 1;

	SetScrollSizes( MM_TEXT, TotalSize );
}

void CSequenceView::DrawSequence()
{
	if( m_pSlider == NULL ) return;
	if( m_pSlider->GetRangeMin() == 0 && m_pSlider->GetRangeMax() == 0 ) return;

	CRect rcSequenceRect;

	m_pSlider->GetWindowRect( &rcSequenceRect );
	ScreenToClient( &rcSequenceRect );

	rcSequenceRect.top -= GetScrollPos( SB_VERT );
	rcSequenceRect.bottom -= GetScrollPos( SB_VERT );

	int nWidthCount = m_pSlider->GetRangeMax() - m_pSlider->GetRangeMin();
	int nHeightCount = m_nGridHeightCount + m_dwAdditionalGridHeightCount;

	CPoint p[2];

	int nEnableIndex = 0;
	int nPanIndex = 0;
	if( !m_bEnable ) nEnableIndex = 1;

	int nCurFrame = m_pSlider->GetPos();
	// Vertical
	for( int i=-1; i<nWidthCount; i++ )
	{
		nPanIndex = 0;
		int nHeightSize = (m_nGridHeightCount + m_dwAdditionalGridHeightCount) * m_GridSize.y;
		int nRealFrame = i + 1;
		bool bTick = false;
		if( -1 == i )
		{
			nPanIndex = 1;
			p[0] = CPoint( rcSequenceRect.left + s_nMargin - 1, rcSequenceRect.top );
			p[1] = CPoint( rcSequenceRect.left + s_nMargin - 1, rcSequenceRect.top + nHeightSize );
		}
		else if( nWidthCount - 1 == i )
		{
			nPanIndex = 1;
			p[0] = CPoint( rcSequenceRect.right - s_nMargin, rcSequenceRect.top );
			p[1] = CPoint( rcSequenceRect.right - s_nMargin, rcSequenceRect.top + nHeightSize );
		}
		else {
			if( nRealFrame % 10 == 0 && i > 0 ) {
				nPanIndex = 2;
				bTick = true;
			}
			p[0] = CPoint( rcSequenceRect.left + m_pSlider->GetTicPos(i), rcSequenceRect.top );
			p[1] = CPoint( rcSequenceRect.left + m_pSlider->GetTicPos(i), rcSequenceRect.top + nHeightSize );
		}
		if( nRealFrame == nCurFrame ) nPanIndex = 3;
		if( bTick ) 
		{
			m_pDC->SetBkMode( TRANSPARENT );
			m_pDC->SelectStockObject( DEFAULT_GUI_FONT );
			m_pDC->SetTextColor( RGB( 80, 80, 180 ) );

			RECT rcRect;
			rcRect.left = p[1].x - 20;
			rcRect.top = p[1].y - 10;
			rcRect.right = p[1].x + 20;
			rcRect.bottom = p[1].y + 10;
			char szTemp[32] = {0,};
			sprintf_s( szTemp, _countof(szTemp), "%d", i+1 );
			m_pDC->DrawText( szTemp, &rcRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

			rcRect.left -= 1; rcRect.right -= 1;
			m_pDC->DrawText( szTemp, &rcRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
		}

		m_pDC->SelectObject(m_Pens[nEnableIndex][nPanIndex]);
		m_pDC->Polyline(p, 2);
	}

	// Horizontal
	for( int i=0; i<nHeightCount + 1; i++ )
	{
		nPanIndex = 0;
		if( i == 0 || i == nHeightCount ) nPanIndex = 1;

		p[0] = CPoint( rcSequenceRect.left + s_nMargin, rcSequenceRect.top + ( i * m_GridSize.y ) );
		p[1] = CPoint( rcSequenceRect.right - s_nMargin, rcSequenceRect.top + ( i * m_GridSize.y ) );

		m_pDC->SelectObject(m_Pens[nEnableIndex][nPanIndex]);
		m_pDC->Polyline(p, 2);
	}
}

void CSequenceView::DrawSignals()
{
	for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) 
	{
		DrawSignal(i);
	}
}

void CSequenceView::DrawSignal(int nIndex)
{
	CRect rcRect;

	CalcSignalRect( nIndex, rcRect );

	rcRect.DeflateRect( 1, 1, 0, 0 );

	CSequenceSignalBase *pSignal = m_pVecSignalList[nIndex];

#ifdef _SELECT_MULTIPLE_SIGNAL
	if (IsSelectedSignal_Valid(nIndex))
	{
		if (pSignal)
		{
			CRect rcFocus;
			CalcSignalRect(nIndex, rcFocus);

			rcFocus.InflateRect( 2, 2, 2, 2 );
			m_pDC->Draw3dRect( rcFocus, RGB( 0, 0, 0 ), RGB( 128, 128, 128 ) );

			rcFocus.DeflateRect( 1, 1, 1, 1 );
			m_pDC->Draw3dRect( rcFocus, RGB( 0, 0, 0 ), RGB( 128, 128, 128 ) );
		}
	}
#else
	if(m_nFocusSignalIndex == nIndex)
	{
		// Border
		COLORREF colorTop = RGB(0, 0, 0);
		COLORREF colorBottom = RGB(120, 120, 120);
		CRect rcBorder = rcRect;

		rcBorder.InflateRect(2, 2, 2, 2);
		m_pDC->Draw3dRect(rcBorder, colorTop, colorBottom);

		rcBorder.DeflateRect(1, 1, 1, 1);
		m_pDC->Draw3dRect(rcBorder, colorTop, colorBottom);
	}
#endif

	m_pDC->Draw3dRect(rcRect, RGB(220, 220, 220), RGB(0, 0, 0));

	rcRect.DeflateRect(1, 1, 1, 1);
	m_pDC->FillSolidRect(rcRect, pSignal->Color);

	m_pDC->SetBkMode( TRANSPARENT );
	m_pDC->SelectStockObject( DEFAULT_GUI_FONT );

	m_pDC->SetTextColor( RGB( 0, 0, 0 ) );
	m_pDC->DrawText( pSignal->szDesc, &rcRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	m_pDC->SetTextColor( pSignal->InitialColor );
	rcRect -= CPoint( 1, 1 );
	m_pDC->DrawText( pSignal->szDesc, &rcRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	COLORREF colorTemp = RGB(255,255,255);
	m_pDC->SetPixel( rcRect.left, rcRect.top, colorTemp );
	m_pDC->SetPixel( rcRect.left+1, rcRect.top, colorTemp );
	m_pDC->SetPixel( rcRect.left+2, rcRect.top, colorTemp );

	m_pDC->SetPixel( rcRect.left, rcRect.top+1, colorTemp );
	m_pDC->SetPixel( rcRect.left+1, rcRect.top+1, colorTemp );

	m_pDC->SetPixel( rcRect.left, rcRect.top+2, colorTemp );

}

void CSequenceView::CalcSignalRect( int nIndex, CRect &rcRect )
{
#ifdef _SELECT_MULTIPLE_SIGNAL
	if (nIndex < 0 || nIndex >= (int)m_pVecSignalList.size())
		return;
#endif
	CSequenceSignalBase *pSignal = m_pVecSignalList[nIndex];

#ifdef _SELECT_MULTIPLE_SIGNAL
	if (pSignal == NULL)
		return;
#endif

	CRect rcSliderRect;
	m_pSlider->GetWindowRect( &rcSliderRect );
	ScreenToClient( &rcSliderRect );

	rcSliderRect.top -= GetScrollPos( SB_VERT );
	rcSliderRect.bottom -= GetScrollPos( SB_VERT );

	rcRect.bottom = rcSliderRect.top + (m_GridSize.y * ( pSignal->nYOrder + 1 ));
	rcRect.top = rcSliderRect.top + (m_GridSize.y * pSignal->nYOrder);

	if( pSignal->nStartFrame == 0 ) rcRect.left = rcSliderRect.left + s_nMargin;
	else rcRect.left = rcSliderRect.left + m_pSlider->GetTicPos(pSignal->nStartFrame - 1);

	if( pSignal->nEndFrame == m_pSlider->GetRangeMax() ) rcRect.right = rcSliderRect.right - (s_nMargin+1);
	else rcRect.right = rcSliderRect.left + m_pSlider->GetTicPos(pSignal->nEndFrame - 1);
}

#ifdef _SELECT_MULTIPLE_SIGNAL
void CSequenceView::OnDragging(const CRect& dragArea)
{
	std::vector<CSequenceSignalBase*> inDragItems;
	for (DWORD i=0; i<m_pVecSignalList.size(); i++)
	{
		CSequenceSignalBase* pCurItem = m_pVecSignalList[i];
		if (pCurItem)
		{
			if (pCurItem->bFreeze == true)
				continue;

			if (IsSignalInDragginArea(i, dragArea))
			{
				PushBackSelectedSignalIndex(i);
				inDragItems.push_back(pCurItem);
			}
			else
			{
				if (IsSelectedSignal(i))
					RemoveSelectedSignal(i);
			}
		}
	}

	OnSelectSignalArray(inDragItems);
}

void CSequenceView::OnDragEnd(const CPoint& startPoint, const CPoint& endPoint)
{
	CRect dragRect(min(startPoint.x, endPoint.x), min(startPoint.y, endPoint.y),
		max(startPoint.x, endPoint.x), max(startPoint.y, endPoint.y));

	OnDragging(dragRect);

	CClientDC dc(this);
	dc.DrawFocusRect(dragRect);
	ReleaseCapture();
	Invalidate();
	//InvalidateRect(&dragRect);
}

BOOL CSequenceView::IsSignalInDragginArea(DWORD dwIndex, const CRect& dragArea)
{
	CRect rcRect;
	CalcSignalRect( dwIndex, rcRect );
	rcRect.top += m_nScrollHeight;
	rcRect.bottom += m_nScrollHeight;

	if (dwIndex < 0 || dwIndex >= m_pVecSignalList.size())
	{
		_ASSERT(0);
		return FALSE;
	}

	return (CRect().IntersectRect(rcRect, dragArea));
}

BOOL CSequenceView::IsSelectedSignal_Valid(DWORD dwIndex)
{
	std::vector<DWORD>::const_iterator iter = std::find(m_CurSelectedItems.begin(), m_CurSelectedItems.end(), dwIndex);
	if (iter != m_CurSelectedItems.end())
	{
		const DWORD& curIndex = *iter;
		return (curIndex >= 0 && curIndex < (DWORD)m_pVecSignalList.size());
	}
	return FALSE;
}

BOOL CSequenceView::IsSelectedSignal(DWORD dwIndex)
{
	std::vector<DWORD>::const_iterator iter = std::find(m_CurSelectedItems.begin(), m_CurSelectedItems.end(), dwIndex);
	return (iter != m_CurSelectedItems.end());
}

void CSequenceView::ClearSelectedSignals()
{
	m_CurSelectedItems.clear();

	OnClearSelectedSignals();
}

BOOL CSequenceView::IsTabKey()
{
	if( GetKeyState( VK_TAB ) & 0x80 )
		return TRUE;

	return FALSE;
}

void CSequenceView::PushBackSelectedSignalIndex(DWORD dwIndex)
{
	std::vector<DWORD>::const_iterator iter = std::find(m_CurSelectedItems.begin(), m_CurSelectedItems.end(), dwIndex);
	if (iter == m_CurSelectedItems.end())
		m_CurSelectedItems.push_back(dwIndex);
}

void CSequenceView::RemoveSelectedSignal(DWORD dwIndex)
{
	std::vector<DWORD>::iterator iter = std::find(m_CurSelectedItems.begin(), m_CurSelectedItems.end(), dwIndex);
	if (iter != m_CurSelectedItems.end())
		m_CurSelectedItems.erase(iter);
}

BOOL CSequenceView::IsSelectedSignal(CPoint& pointInsideItem)
{
	std::vector<DWORD>::const_iterator iter = m_CurSelectedItems.begin();
	for (; iter != m_CurSelectedItems.end(); ++iter)
	{
		const DWORD& selected = (*iter);
		char checkGridFlag = CheckSignalRect((int)selected, pointInsideItem);
		if (checkGridFlag != -1)
			return TRUE;
	}

	return FALSE;
}
#endif // _SELECT_MULTIPLE_SIGNAL

bool CSequenceView::IsMoveSignal(int nStartFrame, int nEndFrame, int nYOrder, bool bCheckFocus)
{
	DWORD dwPrevAdditionalGridHeightCount = m_dwAdditionalGridHeightCount;
	bool bAutoSortOrder = true;

	if( nStartFrame < 0 ) return false;
	if( nStartFrame >= m_pSlider->GetRangeMax() ) return false;
	if( nEndFrame < 1 ) return FALSE;
	if( nEndFrame > m_pSlider->GetRangeMax() ) return false;
	if( nYOrder < 0 ) return FALSE;

	if( m_bUpdateAdditionalGridHeightCount ) 
	{
#ifdef _SELECT_MULTIPLE_SIGNAL
		if (ExtendGridHeightCount(nYOrder) == TRUE)
			bAutoSortOrder = false;
#else
		if( nYOrder >= (int)(m_nGridHeightCount + m_dwAdditionalGridHeightCount) - 2 ) {
			m_dwAdditionalGridHeightCount = 2 + ( nYOrder - m_nGridHeightCount );
			bAutoSortOrder = false;
		}
#endif
	}

	if( nStartFrame >= nEndFrame ) return false;

	int nMinYOrder = nYOrder;
	for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) 
	{
		if( bCheckFocus ) 
		{
#ifdef _SELECT_MULTIPLE_SIGNAL
			if (IsSelectedSignal_Valid(i)) continue;
#else
			if( i == m_nFocusSignalIndex ) continue;
#endif
		}

		CSequenceSignalBase *pSignal = m_pVecSignalList[i];

		if( pSignal->nStartFrame >= nStartFrame && pSignal->nStartFrame <= nEndFrame - 1 && nYOrder == pSignal->nYOrder ) return false;
		if( nStartFrame >= pSignal->nStartFrame && nStartFrame <= pSignal->nEndFrame - 1 && nYOrder == pSignal->nYOrder ) return false;
		if( nMinYOrder < pSignal->nYOrder ) nMinYOrder = pSignal->nYOrder;
	}
	if( m_bUpdateAdditionalGridHeightCount ) {
		if( bAutoSortOrder ) {
			if( nMinYOrder < m_nGridHeightCount ) m_dwAdditionalGridHeightCount = 0;
			else {
				m_dwAdditionalGridHeightCount = 2 + ( nMinYOrder - m_nGridHeightCount );
			}
		}
	}

	if( dwPrevAdditionalGridHeightCount != m_dwAdditionalGridHeightCount ) RefreshScroll();
	return TRUE;
}

char CSequenceView::CheckSignalRect( int nIndex, const CPoint &point )
{
	CRect rcRect;
	CalcSignalRect( nIndex, rcRect );

	rcRect.top += m_nScrollHeight;
	rcRect.bottom += m_nScrollHeight;

#ifdef _SELECT_MULTIPLE_SIGNAL
	if (nIndex < 0 || nIndex >= (int)m_pVecSignalList.size())
	{
		_ASSERT(0);
		return -1;
	}
#endif

	CSequenceSignalBase *pSignal = m_pVecSignalList[nIndex];
	if( pSignal->bModifyLength ) {
		if( point.x >= rcRect.left && point.x <= rcRect.left + 2 &&
			point.y >= rcRect.top && point.y <= rcRect.bottom ) return 0;
		if( point.x >= rcRect.right - 2 && point.x <= rcRect.right &&
			point.y >= rcRect.top && point.y <= rcRect.bottom ) return 2;
	}
	if( point.x >= rcRect.left && point.x <= rcRect.right &&
		point.y >= rcRect.top && point.y <= rcRect.bottom ) return 1;

	return -1;
}

void CSequenceView::MousePosToGridPos( CPoint &MousePos, CPoint &ResultPos )
{
	CPoint p = MousePos;
	p += CPoint( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ) );

	CRect rcRect;

	m_pSlider->GetWindowRect( &rcRect );
	ScreenToClient( &rcRect );
	rcRect += CPoint( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ) );

	if( p.x < rcRect.left + (s_nMargin-1) || p.x > rcRect.right - s_nMargin ) return;
	if( p.y < (int)m_nScrollHeight || p.y > (int)( m_nScrollHeight + ( m_GridSize.y * (m_nGridHeightCount + m_dwAdditionalGridHeightCount) ) ) ) return;

	CPoint TempPos;
	int nCount = m_pSlider->GetRangeMax() - m_pSlider->GetRangeMin();

	ResultPos.y = ( p.y - m_nScrollHeight ) / m_GridSize.y;
	for( int j=0; j<nCount; j++ ) 
	{
		if( p.x < rcRect.left + m_pSlider->GetTicPos(j) ) 
		{
			ResultPos.x = j;
			return;
		}
	}
	ResultPos.x = m_pSlider->GetRangeMax() - 1;
}

CSequenceView::CSequenceSignalBase *CSequenceView::AddSignal(int nStartFrame, int nEndFrame, int nYOrder, char *szDescription, char *szStr)
{
	CSequenceSignalBase *pSignal = CreateSignal();
	pSignal->nStartFrame = nStartFrame;
	pSignal->nEndFrame = nEndFrame;
	pSignal->nYOrder = nYOrder;
	pSignal->szString = szStr;
	pSignal->szDesc = szDescription;

	DWORD dwHash = GetHash(szStr);
	pSignal->InitialColor = RGB( 255-GetGValue(dwHash), 255-GetBValue(dwHash), 255-GetRValue(dwHash) );
	pSignal->Color = RGB( GetRValue(dwHash), GetGValue(dwHash), GetBValue(dwHash) );
	pSignal->bFreeze = false;
	pSignal->bDestroy = true;

	if( pSignal->nYOrder >= m_nGridHeightCount  ) 
	{
		m_dwAdditionalGridHeightCount = (pSignal->nYOrder - m_nGridHeightCount) + 1;
	}

	m_pVecSignalList.push_back( pSignal );
	return pSignal;
}

bool CSequenceView::CanAddSignal(int nStartFrame, int nEndFrame, int nYOrder)
{
	if( IsMoveSignal( nStartFrame, nEndFrame, nYOrder, FALSE ) == true ) return true;

	return false;
}

bool CSequenceView::CanAddSignal(int nX, int nY)
{
	CPoint GridPos;
	MousePosToGridPos( CPoint(nX, nY), GridPos );

	return CanAddSignal( GridPos.x, GridPos.x + 1, GridPos.y );
}

bool CSequenceView::CalcSignalPos( int nX, int nY, int &nPos, int &nYOrder )
{
	CPoint GridPos;
	MousePosToGridPos( CPoint(nX,nY), GridPos );

	nPos = GridPos.x;
	nYOrder = GridPos.y;

	if( !CanAddSignal(nX, nY) ) return false;

	return true;
}

#ifdef _SELECT_MULTIPLE_SIGNAL
int CSequenceView::IsSignalEndOverRangeMax(const int& nGridEnd)
{
	if (m_pSlider)
	{
		if (nGridEnd > m_pSlider->GetRangeMax())
			return abs(nGridEnd - m_pSlider->GetRangeMax());
	}
	return -1;
}

BOOL CSequenceView::IsSignalInArea(int nStart, int nEnd, int nStartYOrder, int nEndYOrder) const
{
	CRect areaRect(nStart, 0, nEnd, 1);
	std::vector<CSequenceSignalBase *>::const_iterator iter = m_pVecSignalList.begin();
	for (; iter != m_pVecSignalList.end(); ++iter)
	{
		CSequenceSignalBase* pCurrentItem = (*iter);
		if (pCurrentItem != NULL)
		{
			int curSignalEnd = pCurrentItem->nEndFrame - 1;
			if (pCurrentItem->nYOrder >= nStartYOrder && pCurrentItem->nYOrder <= nEndYOrder)
			{
				CRect currentItemRect(pCurrentItem->nStartFrame, 0, pCurrentItem->nEndFrame, 1);
				if (CRect().IntersectRect(currentItemRect, areaRect))
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CSequenceView::IsSignalInArea(const CRect& area) const
{
	return IsSignalInArea(area.left, area.right, area.top, area.bottom);
}

int CSequenceView::GetLastSignalYOrder(const CRect& gridArea) const
{
	int lastYOrder = INT_MIN;
	std::vector<CSequenceSignalBase *>::const_iterator iter = m_pVecSignalList.begin();
	for (; iter != m_pVecSignalList.end(); ++iter)
	{
		CSequenceSignalBase* pCurrentItem = (*iter);
		if (pCurrentItem != NULL)
		{
			if (CommonUtil::IsIntersect(pCurrentItem->nStartFrame, pCurrentItem->nEndFrame, gridArea.left, gridArea.right) && lastYOrder < pCurrentItem->nYOrder)
				lastYOrder = pCurrentItem->nYOrder;
		}
	}

	return lastYOrder;
}
BOOL CSequenceView::ExtendGridHeightCount(int nYOrder)
{
	if (nYOrder >= (int)(m_nGridHeightCount+m_dwAdditionalGridHeightCount) - 2)
	{
		m_dwAdditionalGridHeightCount = 2 + ( nYOrder - m_nGridHeightCount );
		return TRUE;
	}

	return FALSE;
};
#endif

bool CSequenceView::IsValidPropertyPaneState() const
{
	return true;
}

void CSequenceView::Reset()
{
	SAFE_DELETE_PVEC( m_pVecSignalList );
	m_dwAdditionalGridHeightCount = 0;
	Invalidate();
}

void CSequenceView::SetFrame( CString str, int nStart, int nEnd )
{
	int i, nSize;
	nSize = (int)m_pVecSignalList.size();
	for( i = 0; i < nSize; i++) {
		if( str == m_pVecSignalList[i]->szDesc ) {
			m_pVecSignalList[i]->nStartFrame = nStart;
			m_pVecSignalList[i]->nEndFrame = nEnd;
		}
	}
	Invalidate();
}
