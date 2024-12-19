// MainView.cpp : implementation of the CMainView class
//

#include "stdafx.h"
#include "ServiceMonitor.h"

#include "MainDoc.h"
#include "MainView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainView

IMPLEMENT_DYNCREATE(CMainView, CFormView)

BEGIN_MESSAGE_MAP(CMainView, CFormView)
END_MESSAGE_MAP()

// CMainView construction/destruction

CMainView::CMainView()
	: CFormView(CMainView::IDD)
{

}

CMainView::~CMainView()
{
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~(WS_HSCROLL | WS_VSCROLL);

	return CFormView::PreCreateWindow(cs);
}

void CMainView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CMainView diagnostics

#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMainView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CMainDoc* CMainView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMainDoc)));
	return (CMainDoc*)m_pDocument;
}
#endif //_DEBUG


// CMainView message handlers

