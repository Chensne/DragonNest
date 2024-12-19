// EtEffect2ToolView.cpp : CEtEffect2ToolView 클래스의 구현
//

#include "stdafx.h"
#include "EtEffect2Tool.h"

#include "EtEffect2ToolDoc.h"
#include "EtEffect2ToolView.h"
#include "UserMessage.h"
#include "RenderBase.h"
#include "SequenceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtEffect2ToolView

IMPLEMENT_DYNCREATE(CEtEffect2ToolView, CView)

BEGIN_MESSAGE_MAP(CEtEffect2ToolView, CView)
	ON_MESSAGE( UM_REFRESH_VIEW, OnRefresh )
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CEtEffect2ToolView 생성/소멸

CEtEffect2ToolView::CEtEffect2ToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_bActivate = false;
}

CEtEffect2ToolView::~CEtEffect2ToolView()
{
}

BOOL CEtEffect2ToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CEtEffect2ToolView 그리기

void CEtEffect2ToolView::OnDraw(CDC* /*pDC*/)
{
	CEtEffect2ToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CEtEffect2ToolView 진단

#ifdef _DEBUG
void CEtEffect2ToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEtEffect2ToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEtEffect2ToolDoc* CEtEffect2ToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEtEffect2ToolDoc)));
	return (CEtEffect2ToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CEtEffect2ToolView 메시지 처리기

void CEtEffect2ToolView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CView::OnPaint() for painting messages
	CRenderBase::GetInstance().Render();
}

void CEtEffect2ToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	if(!m_bActivate) {
		// TODO: Add your specialized code here and/or call the base class	
		CGlobalValue::GetInstance().SetView( this );
		//EnableTabView( -1, false );
		static CRenderBase g_RenderBase;
		
		CRenderBase::GetInstance().Initialize( GetSafeHwnd() );
		CGlobalValue::GetInstance().Reset();
		m_bActivate = true;
	}
}

LRESULT CEtEffect2ToolView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	CRenderBase::GetInstance().Process();	
	Invalidate( FALSE );

	return S_OK;
}

void CEtEffect2ToolView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if( CRenderBase::IsActive() ) {
		CRenderBase::GetInstance().Reconstruction( cx, cy );
	}
	// TODO: Add your message handler code here
}

void CEtEffect2ToolView::OnDestroy()
{
	CView::OnDestroy();
	// 릭방지
	CGlobalValue::GetInstance().Release();	

	// TODO: Add your message handler code here
}
