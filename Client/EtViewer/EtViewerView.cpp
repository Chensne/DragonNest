// EtViewerView.cpp : CEtViewerView Ŭ������ ����
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
	// ǥ�� �μ� ����Դϴ�.
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

// CEtViewerView ����/�Ҹ�

CEtViewerView::CEtViewerView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

	m_bIdle = false;
	m_bActivate = false;
}

CEtViewerView::~CEtViewerView()
{
}

BOOL CEtViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CEtViewerView �׸���

void CEtViewerView::OnDraw(CDC* /*pDC*/)
{
	CEtViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CEtViewerView �μ�

BOOL CEtViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CEtViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CEtViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CEtViewerView ����

#ifdef _DEBUG
void CEtViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CEtViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEtViewerDoc* CEtViewerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEtViewerDoc)));
	return (CEtViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CEtViewerView �޽��� ó����

void CEtViewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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


	// �⺻ ObjectBase ����
	CGlobalValue::GetInstance().ResetObject();
}

void CEtViewerView::Refresh()
{
	Invalidate();
	Sleep(0);
}

BOOL CEtViewerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

//	return CView::OnEraseBkgnd(pDC);
	return FALSE;
}

void CEtViewerView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CView::OnPaint()��(��) ȣ������ ���ʽÿ�.

	m_bIdle = !CRenderBase::GetInstance().Render();
}

void CEtViewerView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CGlobalValue::GetInstance().Finalize();
	CRenderBase::GetInstance().Finalize();
}

void CEtViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CRenderBase::GetInstance().Reconstruction();
}

void CEtViewerView::OnPlayPlaypause()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( nChar == VK_ESCAPE ) {
		if( CGlobalValue::GetInstance().IsFullScreen() ) 
			((CMainFrame*)AfxGetMainWnd())->OnFullscreen();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
