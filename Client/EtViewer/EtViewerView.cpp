// EtViewerView.cpp : CEtViewerView 클래스의 구현
//

#include "stdafx.h"
#include "EtViewer.h"

#include "EtViewerDoc.h"
#include "EtViewerView.h"
#include "GlobalValue.h"
#include "RenderBase.h"

#include "ObjectBase.h"
#include "MainFrm.h"
#include "PaneDefine.h"
#include "UserMessage.h"

#include "PropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtViewerView

IMPLEMENT_DYNCREATE(CEtViewerView, CView)

BEGIN_MESSAGE_MAP(CEtViewerView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_PLAY_PLAYPAUSE, &CEtViewerView::OnPlayPlaypause)
	ON_COMMAND(ID_PLAY_REWIND, &CEtViewerView::OnPlayRewind)
	ON_COMMAND(ID_PLAY_FORWARD, &CEtViewerView::OnPlayForward)
	ON_COMMAND(ID_PLAY_PREVF, &CEtViewerView::OnPlayPrev)
	ON_COMMAND(ID_PLAY_NEXTF, &CEtViewerView::OnPlayNext)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CEtViewerView 생성/소멸

CEtViewerView::CEtViewerView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

	m_bIdle = false;
	m_bActivate = false;
}

CEtViewerView::~CEtViewerView()
{
}

BOOL CEtViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CEtViewerView 그리기

void CEtViewerView::OnDraw(CDC* /*pDC*/)
{
	CEtViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CEtViewerView 인쇄

BOOL CEtViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CEtViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CEtViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CEtViewerView 진단

#ifdef _DEBUG
void CEtViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CEtViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEtViewerDoc* CEtViewerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEtViewerDoc)));
	return (CEtViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CEtViewerView 메시지 처리기

void CEtViewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CGlobalValue::GetInstance().SetView( this );
	if( m_bActivate == true ) return;
	m_bActivate = true;

	CRenderBase::GetInstance().Initialize( m_hWnd );

	char szStr[2048] = { 0, };
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", szStr, 2048 );
	if( strlen( szStr ) == 0 ) {
		CPropertiesDlg Dlg;
		Dlg.m_bDisableCancel = true;
		Dlg.DoModal();
	}


	// 기본 ObjectBase 생성
	CGlobalValue::GetInstance().ResetObject();
}

void CEtViewerView::Refresh()
{
	Invalidate();
	Sleep(0);
}

BOOL CEtViewerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return FALSE;
}

void CEtViewerView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.

	m_bIdle = !CRenderBase::GetInstance().Render();
}

void CEtViewerView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGlobalValue::GetInstance().Finalize();
	CRenderBase::GetInstance().Finalize();
}

void CEtViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRenderBase::GetInstance().Reconstruction();
}

void CEtViewerView::OnPlayPlaypause()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWnd *pWnd = GetPaneWnd( ANI_PANE );
	if( pWnd ) pWnd->SendMessage( UM_ANICONTROL_ACCEL_KEY, 0 );
}

void CEtViewerView::OnPlayRewind()
{
	CWnd *pWnd = GetPaneWnd( ANI_PANE );
	if( pWnd ) pWnd->SendMessage( UM_ANICONTROL_ACCEL_KEY, 1 );
}

void CEtViewerView::OnPlayForward()
{
	CWnd *pWnd = GetPaneWnd( ANI_PANE );
	if( pWnd ) pWnd->SendMessage( UM_ANICONTROL_ACCEL_KEY, 2 );
}

void CEtViewerView::OnPlayPrev()
{
	CWnd *pWnd = GetPaneWnd( ANI_PANE );
	if( pWnd ) pWnd->SendMessage( UM_ANICONTROL_ACCEL_KEY, 3 );
}

void CEtViewerView::OnPlayNext()
{
	CWnd *pWnd = GetPaneWnd( ANI_PANE );
	if( pWnd ) pWnd->SendMessage( UM_ANICONTROL_ACCEL_KEY, 4 );
}

void CEtViewerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nChar == VK_ESCAPE ) {
		if( CGlobalValue::GetInstance().IsFullScreen() ) 
			((CMainFrame*)AfxGetMainWnd())->OnFullscreen();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
