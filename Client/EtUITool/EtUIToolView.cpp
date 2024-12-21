// EtUIToolView.cpp : implementation of the CEtUIToolView class
//

#include "stdafx.h"
#include "EtUI.h"
#include "EtUITool.h"
#include "EtUIType.h"
#include "EtUIMan.h"
#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "MainFrm.h"
#include "PaneDefine.h"
#include "RenderBase.h"
#include "DummyView.h"
#include "GlobalValue.h"
#include "UIToolTemplate.h"
#include "LayoutView.h"
#include "TemplatePaneView.h"
#include "ControlPropertyPaneView.h"
#include "GenTexture.h"
#include "EtSoundEngine.h"
#include "../../Common/EtResManager/EtResourceMng.h"
#include "EtUISound.h"
#include "DebugSet.h"
#include "../../Common/EtStringManager/EtUIXML.h"
#include "PropertiesDlg.h"
#include "EtFontMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEtUIToolView

IMPLEMENT_DYNCREATE(CEtUIToolView, CView)

BEGIN_MESSAGE_MAP(CEtUIToolView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CEtUIToolView construction/destruction

CEtUIToolView::CEtUIToolView()
{
	// TODO: add construction code here
	m_pFlatTabCtrl = NULL;
	m_pDummyView = NULL;
	m_nActiveView = 0;
	m_pSoundEngine = NULL;
	m_pUISound = NULL;
	m_pUIXML = NULL;

	EtInterface::g_bEtUIInitTool = true;
}

CEtUIToolView::~CEtUIToolView()
{
	CEtFontMng::GetInstance().Finalize();
	EtInterface::Finalize();
	CRenderBase::GetInstance().Finalize();

	SAFE_DELETE( m_pFlatTabCtrl );
	SAFE_DELETE( m_pUIXML );

	if( m_pUISound )
	{
		m_pUISound->Finalize();
		SAFE_DELETE( m_pUISound );
	}

	if( m_pSoundEngine ) 
	{
		m_pSoundEngine->Finalize();
		SAFE_DELETE( m_pSoundEngine );
	}
}

BOOL CEtUIToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CEtUIToolView::PostNcDestroy()
{
	//KillTimer( m_nTimer );
	CView::PostNcDestroy();
}

void CEtUIToolView::OnTimer( UINT_PTR nIDEvent )
{
	OnPaint();
	CView::OnTimer(nIDEvent);
}

// CEtUIToolView drawing

void CEtUIToolView::OnDraw(CDC* pDC)
{
	CEtUIToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	//CPaintDC dc(this); // device context for painting

	CRenderBase::GetInstance().Process();
	CRenderBase::GetInstance().Render();

	CRect ViewRectB, ViewRectR, ScreenViewRect, ScreenPaneViewRect;
	CDummyView::TabViewStruct *pStruct;

	pStruct = m_pDummyView->GetTabViewStruct( GetActiveView() );

	GetClientRect( &ScreenViewRect );
	ClientToScreen( &ScreenViewRect );
	pStruct->pView->GetClientRect( &ScreenPaneViewRect );
	pStruct->pView->ClientToScreen( &ScreenPaneViewRect );

	GetClientRect( &ViewRectB );
	ViewRectB.top = ( ScreenPaneViewRect.top - ScreenViewRect.top ) + GetEtDevice()->Height();
	ViewRectB.bottom -= ScreenViewRect.bottom - ScreenPaneViewRect.bottom;
	ViewRectB.left = ScreenPaneViewRect.left - ScreenViewRect.left;
	ViewRectB.right -= ScreenViewRect.right - ScreenPaneViewRect.right;
	pDC->FillSolidRect( &ViewRectB, RGB( 128, 128, 128 ) );

	GetClientRect( &ViewRectR );
	ViewRectR.top = ScreenPaneViewRect.top - ScreenViewRect.top;
	ViewRectR.bottom -= ScreenViewRect.bottom - ScreenPaneViewRect.bottom;
	ViewRectR.left = ( ScreenPaneViewRect.left - ScreenViewRect.left ) + GetEtDevice()->Width();
	ViewRectR.right -= ScreenViewRect.right - ScreenPaneViewRect.right;
	pDC->FillSolidRect( &ViewRectR, RGB( 128, 128, 128 ) );

	CPen pen;
	pen.CreatePen(PS_DASH, 1, RGB(255, 255, 255));
	CPen* pOldPen = pDC->SelectObject(&pen);

	POINT pt;
	pt.x = ViewRectB.left;
	pt.y = ViewRectB.top;
	pDC->MoveTo( pt );
	pt.x = ViewRectB.right;
	pt.y = ViewRectB.top;
	pDC->LineTo( pt );

	pt.x = ViewRectR.left;
	pt.y = ViewRectR.top;
	pDC->MoveTo( pt );
	pt.x = ViewRectR.left;
	pt.y = ViewRectR.bottom;
	pDC->LineTo( pt );

	// put back the old objects
	pDC->SelectObject(pOldPen);

	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( GetTabView( CDummyView::LAYOUT_VIEW ) );
	pLayoutView->DrawControlGroupInfo( pDC, ViewRectR.left, ViewRectR.top );
}


// CEtUIToolView printing

BOOL CEtUIToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEtUIToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEtUIToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CEtUIToolView diagnostics

#ifdef _DEBUG
void CEtUIToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEtUIToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEtUIToolDoc* CEtUIToolView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEtUIToolDoc)));
	return (CEtUIToolDoc*)m_pDocument;
}
#endif //_DEBUG

// CEtUIToolView message handlers

int CEtUIToolView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pFlatTabCtrl = new CXTPTabControl;
	m_pFlatTabCtrl->GetPaintManager()->m_bDisableLunaColors = TRUE;
	m_pFlatTabCtrl->SetAppearance(xtpTabAppearanceExcel);
	m_pFlatTabCtrl->GetPaintManager()->m_bHotTracking = TRUE;

	CCreateContext cc;
	ZeroMemory(&cc, sizeof(cc));
	cc.m_pCurrentDoc = GetDocument();

	CRect rcRect;
	GetClientRect( &rcRect );
	// Create  the flat tab control.
	if (!m_pFlatTabCtrl->Create(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, rcRect, this, 1001))
	{
		TRACE0( "Failed to create flattab control\n" );
		return 0;
	}

	m_pDummyView = (CDummyView*)RUNTIME_CLASS(CDummyView)->CreateObject();
	((CWnd *)m_pDummyView)->Create( NULL, NULL, WS_CHILD|WS_VISIBLE, rcRect, m_pFlatTabCtrl, 1995, &cc );

	m_pDummyView->InitView( m_pFlatTabCtrl );

	return 0;
}

void CEtUIToolView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pFlatTabCtrl && ::IsWindow(m_pFlatTabCtrl->m_hWnd))
	{
		m_pFlatTabCtrl->SetWindowPos( NULL, 0, 0, cx, cy, SWP_FRAMECHANGED );
	}
}

BOOL CEtUIToolView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( ( UINT )wParam == 1001 ) 
	{
		NMHDR *pNMHDR = ( NMHDR * )lParam;

		switch( pNMHDR->code )
		{
			case TCN_SELCHANGING:
				break;

			case TCN_SELCHANGE:
			{
				if( m_pFlatTabCtrl->GetCurSel() == m_nActiveView ) 
				{
					break;
				}
				m_nActiveView = m_pFlatTabCtrl->GetCurSel();
				if( !( ( CMainFrame * )AfxGetMainWnd() ) ) 
				{
					break;
				}

				int i, nCount;

				nCount = sizeof( g_PaneList ) / sizeof( PaneCreateStruct );
				for( i = 0; i < nCount; i++ )
				{
					if( g_PaneList[ i ].bAutoShowHide )
					{
						((CMainFrame*)AfxGetMainWnd())->XTPDockingPaneManager()->ClosePane( g_PaneList[i].nPaneID );
					}
				}

				CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
				CXTPDockingPaneManager *pManager = pFrame->XTPDockingPaneManager();

				CDummyView::EM_VIEWTYPE emActiveView;

				switch( m_nActiveView ) 
				{
					case 0:
						pManager->ShowPane( CONTROL_PROPERTY_PANE );
						emActiveView = CDummyView::LAYOUT_VIEW;
						break;
					case 1:
						pManager->ShowPane( TEMPLATE_PROPERTY_PANE );
						emActiveView = CDummyView::TEMPLATE_VIEW;
						break;
				}

				if( m_pDummyView ) 
				{
					m_pDummyView->SetSelectChangeView( emActiveView );
				}
			}
			break;
		}
	}

	return CView::OnNotify(wParam, lParam, pResult);
}

void CEtUIToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	if( CGlobalValue::GetInstance().m_pCurView ) return;

	CRenderBase::GetInstance().Initialize( m_hWnd, DEFAULT_UI_SCREEN_WIDTH, DEFAULT_UI_SCREEN_HEIGHT );

	CGlobalValue::GetInstance().m_pCurView = this;

	m_pUIXML = new CEtUIXML;
	m_pUIXML->LoadXML( "uistring.xml", CEtUIXML::idCategory1 );
	EtInterface::xml::SetXML( m_pUIXML );
	
	m_pSoundEngine = new CEtSoundEngine;
	m_pSoundEngine->Initialize();
	m_pSoundEngine->CreateChannelGroup( "2D" );

	m_pUISound = new CEtUISound;
	m_pUISound->Initialize();

	EtInterface::Initialize();
	CEtFontMng::GetInstance().Initialize( CEtResourceMng::GetInstance().GetFullName("ui.fontset").c_str() );
	CEtTexture::AddIgnoreMipmapPath( "resource\\ui" );
	CEtTexture::AddIgnoreMipmapPath( "WorkSheet\\UITemplate" );

	m_nTimer = SetTimer( 100, 33, NULL );
}

BOOL CEtUIToolView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

LRESULT CEtUIToolView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CView::WindowProc(message, wParam, lParam);
}

CWnd *CEtUIToolView::GetActiveTabView()
{
	CDummyView::TabViewStruct *pStruct;
	pStruct = m_pDummyView->GetTabViewStruct( m_nActiveView );
	return pStruct->pView;
}

CWnd *CEtUIToolView::GetTabView( int nView )
{
	CDummyView::TabViewStruct *pStruct;
	pStruct = m_pDummyView->GetTabViewStruct( nView );
	return pStruct->pView;
}