// EmblemTestView.cpp : implementation of the CEmblemTestView class
//

#include "stdafx.h"
#include "EmblemTest.h"

#include "EmblemTestDoc.h"
#include "EmblemTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEmblemTestView

IMPLEMENT_DYNCREATE(CEmblemTestView, CView)

BEGIN_MESSAGE_MAP(CEmblemTestView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CEmblemTestView construction/destruction

CEmblemTestView::CEmblemTestView()
{
	// TODO: add construction code here

}

CEmblemTestView::~CEmblemTestView()
{
}

BOOL CEmblemTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CEmblemTestView drawing

void CEmblemTestView::OnDraw(CDC* /*pDC*/)
{
	CEmblemTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CEmblemTestView printing

BOOL CEmblemTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEmblemTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEmblemTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CEmblemTestView diagnostics

#ifdef _DEBUG
void CEmblemTestView::AssertValid() const
{
	CView::AssertValid();
}

void CEmblemTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEmblemTestDoc* CEmblemTestView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEmblemTestDoc)));
	return (CEmblemTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CEmblemTestView message handlers
