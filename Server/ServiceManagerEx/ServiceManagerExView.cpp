// ServiceManagerExView.cpp : CServiceManagerExView 클래스의 구현
//

#include "stdafx.h"
#include "ServiceManagerEx.h"

#include "ServiceManagerExDoc.h"
#include "ServiceManagerExView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServiceManagerExView

IMPLEMENT_DYNCREATE(CServiceManagerExView, CView)

BEGIN_MESSAGE_MAP(CServiceManagerExView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CServiceManagerExView 생성/소멸

CServiceManagerExView::CServiceManagerExView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CServiceManagerExView::~CServiceManagerExView()
{
}

BOOL CServiceManagerExView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CServiceManagerExView 그리기

void CServiceManagerExView::OnDraw(CDC* /*pDC*/)
{
	CServiceManagerExDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CServiceManagerExView 인쇄

BOOL CServiceManagerExView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CServiceManagerExView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CServiceManagerExView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CServiceManagerExView 진단

#ifdef _DEBUG
void CServiceManagerExView::AssertValid() const
{
	CView::AssertValid();
}

void CServiceManagerExView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CServiceManagerExDoc* CServiceManagerExView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServiceManagerExDoc)));
	return (CServiceManagerExDoc*)m_pDocument;
}
#endif //_DEBUG


// CServiceManagerExView 메시지 처리기

void CServiceManagerExView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}
