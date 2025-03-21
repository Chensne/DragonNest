// EtActionToolView.cpp : CEtActionToolView 클래스의 구현
//

#include "stdafx.h"
#include "EtActionTool.h"

#include "EtActionToolDoc.h"
#include "EtActionToolView.h"

#include "RenderBase.h"
#include "UserMessage.h"
#include "PropertiesDlg.h"
#include "ObjectLightFolder.h"
#include "SignalCustomRender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtActionToolView

IMPLEMENT_DYNCREATE(CEtActionToolView, CView)

BEGIN_MESSAGE_MAP(CEtActionToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( UM_REFRESH_VIEW, OnRefresh )
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CEtActionToolView 생성/소멸

CEtActionToolView::CEtActionToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_bActivate = false;
	m_bIdle = false;

	CSignalCustomRender::RegisterClass();
}

CEtActionToolView::~CEtActionToolView()
{
	CSignalCustomRender::UnRegisterClass();
}

BOOL CEtActionToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CEtActionToolView 그리기

void CEtActionToolView::OnDraw(CDC* /*pDC*/)
{
	CEtActionToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
//	CRenderBase::GetInstance().Render();
}


// CEtActionToolView 인쇄

BOOL CEtActionToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CEtActionToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CEtActionToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CEtActionToolView 진단

#ifdef _DEBUG
void CEtActionToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEtActionToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEtActionToolDoc* CEtActionToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEtActionToolDoc)));
	return (CEtActionToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CEtActionToolView 메시지 처리기

void CEtActionToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CGlobalValue::GetInstance().SetView( this );
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	static CRenderBase g_RenderBase;
	CRenderBase::GetInstance().Initialize( GetSafeHwnd() );

	char szStr1[2048] = { 0, };
	char szStr2[2048] = { 0, };
	char szStr3[2048] = { 0, };
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "WorkingFolder", szStr1, 2048 );
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", szStr2, 2048 );
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ResourceFolder", szStr3, 2048 );
	if( strlen( szStr1 ) == 0 || strlen( szStr2 ) == 0 || strlen( szStr3 ) == 0 ) {
		CPropertiesDlg Dlg;
		Dlg.m_bDisableCancel = true;
		Dlg.DoModal();
	}

	((CObjectLightFolder*)CGlobalValue::GetInstance().GetRootLightObject())->LoadLightSetting( CString("") );

	CGlobalValue::GetInstance().Reset();
}

void CEtActionToolView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( CGlobalValue::IsActive() )
		CGlobalValue::GetInstance().Finalize();
	if( CRenderBase::IsActive() )
		CRenderBase::GetInstance().Finalize();
}

void CEtActionToolView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( CRenderBase::IsActive() )
		CRenderBase::GetInstance().Reconstruction( cx, cy );
}


BOOL CEtActionToolView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

LRESULT CEtActionToolView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	CRenderBase::GetInstance().Process();
	Invalidate();

	return S_OK;
}
void CEtActionToolView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.
	m_bIdle = !CRenderBase::GetInstance().Render();
}
