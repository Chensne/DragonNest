// DummyView.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "DummyView.h"

#include "LayoutView.h"
#include "TemplateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDummyView

IMPLEMENT_DYNCREATE(CDummyView, CFormView)

CDummyView::CDummyView()
	: CFormView(CDummyView::IDD)
	, m_pTabViewItem(NULL)
	, m_bTemplateOnLayoutView(false)
	, m_TemplateDlgAlignHori(AT_HORI_LEFT)
	, m_TemplateDlgAlignVert(AT_VERT_TOP)
{
}

CDummyView::~CDummyView()
{
	SAFE_DELETE( m_pTabViewItem );
}

void CDummyView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDummyView, CFormView)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CDummyView diagnostics

#ifdef _DEBUG
void CDummyView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDummyView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDummyView message handlers


void CDummyView::InitView( CXTPTabControl *pControl )
{
	CRect rcRect;
	GetClientRect( &rcRect );

	CCreateContext cc;
	ZeroMemory(&cc, sizeof(cc));
	cc.m_pCurrentDoc = GetDocument();

	struct TabViewInitStruct {
		int nTabIndex;
		char *szString;
		CRuntimeClass *pRuntimeClass;
		CWnd *pWnd;
	};
	TabViewInitStruct TabViewInitList[] = {
		{ LAYOUT_VIEW, "Layout", RUNTIME_CLASS( CLayoutView ), NULL },
		{ TEMPLATE_VIEW, "Template", RUNTIME_CLASS( CTemplateView ), NULL },
	};

	int nCount = sizeof(TabViewInitList) / sizeof(TabViewInitStruct);
	TabViewStruct Struct;
	m_pTabViewItem = new TabViewStruct[nCount];
	for( int i=0; i<nCount; i++ ) {
		if( TabViewInitList[i].pRuntimeClass ) {
			TabViewInitList[i].pWnd = (CWnd*)TabViewInitList[i].pRuntimeClass->CreateObject();
			TabViewInitList[i].pWnd->Create( NULL, NULL, WS_CHILD, rcRect, pControl, 2000 + i, &cc );
		}

		Struct.pItem = pControl->InsertItem(TabViewInitList[i].nTabIndex, 
											TabViewInitList[i].szString, 
											( TabViewInitList[i].pWnd == NULL ) ? NULL : TabViewInitList[i].pWnd->m_hWnd );
		Struct.pView = TabViewInitList[i].pWnd;
		m_pTabViewItem[ TabViewInitList[i].nTabIndex ] = Struct;
	}

	pControl->SetCurSel(LAYOUT_VIEW);
}

BOOL CDummyView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CDummyView::SetSelectChangeView( EM_VIEWTYPE emActiveView )
{
	CTemplateView *pTemplateView(NULL);
	CLayoutView *pLayoutView(NULL);
	pLayoutView = (CLayoutView*)m_pTabViewItem[LAYOUT_VIEW].pView;
	ASSERT(pLayoutView&&"CDummyView::SetSelectChangeView, pLayoutView is NULL!");
	pTemplateView = (CTemplateView*)m_pTabViewItem[TEMPLATE_VIEW].pView;
	ASSERT(pTemplateView&&"CDummyView::SetSelectChangeView, pTemplate is NULL!");

	switch( emActiveView )
	{
	case LAYOUT_VIEW:
		{
			pLayoutView->SendMessage( UM_SELECT_CHANGE_OPEN_VIEW );
			if( m_bTemplateOnLayoutView )
			{
				pTemplateView->SendMessage( UM_SELECT_CHANGE_OPEN_VIEW );
				pTemplateView->AlignDialog( m_TemplateDlgAlignHori, m_TemplateDlgAlignVert );
			}
			else
			{
				pTemplateView->SendMessage( UM_SELECT_CHANGE_CLOSE_VIEW );
			}
		}
		break;
	case TEMPLATE_VIEW:
		{
			pLayoutView->SendMessage( UM_SELECT_CHANGE_CLOSE_VIEW );
			pTemplateView->SendMessage( UM_SELECT_CHANGE_OPEN_VIEW );
			pTemplateView->AlignDialog( AT_HORI_CENTER, AT_VERT_CENTER );
		}
		break;
	}
}

void CDummyView::EnableTemplateOnLayoutView( bool bEnable )
{
	m_bTemplateOnLayoutView = bEnable;

	CTemplateView *pTemplateView(NULL);
	pTemplateView = (CTemplateView*)m_pTabViewItem[TEMPLATE_VIEW].pView;
	ASSERT(pTemplateView&&"CDummyView::EnableTemplateOnLayoutView, pTemplate is NULL!");

	if( m_bTemplateOnLayoutView )
	{
		pTemplateView->SendMessage( UM_SELECT_CHANGE_OPEN_VIEW );
		pTemplateView->AlignDialog( m_TemplateDlgAlignHori, m_TemplateDlgAlignVert );
	}
	else
	{
		pTemplateView->SendMessage( UM_SELECT_CHANGE_CLOSE_VIEW );
	}
}

void CDummyView::AlignTemplateDialog( UIAllignHoriType alignHori, UIAllignVertType alignVert )
{
	if( m_bTemplateOnLayoutView )
	{
		m_TemplateDlgAlignHori = alignHori;
		m_TemplateDlgAlignVert = alignVert;

		CTemplateView *pTemplateView(NULL);
		pTemplateView = (CTemplateView*)m_pTabViewItem[TEMPLATE_VIEW].pView;
		ASSERT( pTemplateView&&"CDummyView::AlignTemplateDialog, pTemplateView is NULL!" );
		pTemplateView->AlignDialog( m_TemplateDlgAlignHori, m_TemplateDlgAlignVert );
	}
}