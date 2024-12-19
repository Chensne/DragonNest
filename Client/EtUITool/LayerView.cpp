// LayerView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtUITool.h"
#include "LayerView.h"

#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "PaneDefine.h"
#include "LayoutView.h"
#include "DummyView.h"
#include "GlobalValue.h"


// CLayerView

IMPLEMENT_DYNCREATE(CLayerView, CFormView)

CLayerView::CLayerView()
	: CFormView(CLayerView::IDD)
{
	m_bActivate = FALSE;
	m_pContextMenu = new CMenu();
	m_pContextMenu->LoadMenu( IDR_CONTEXT_MENU );
}

CLayerView::~CLayerView()
{
	SAFE_DELETE( m_pContextMenu );
}

void CLayerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ctrlTree);
}

BEGIN_MESSAGE_MAP(CLayerView, CFormView)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_LAYER_CLEAR_ALL, &CLayerView::OnLayerClearAll)
	ON_COMMAND(ID_LAYER_CLEAR, &CLayerView::OnLayerClear)
END_MESSAGE_MAP()


// CLayerView �����Դϴ�.

#ifdef _DEBUG
void CLayerView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLayerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLayerView �޽��� ó�����Դϴ�.

void CLayerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( m_bActivate )
	{
		return;
	}
	m_bActivate = TRUE;

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_LAYER );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 4, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );

	m_ctrlTree.EnableMultiSelect( TRUE );
	m_ctrlTree.DeleteAllItems();
	m_ctrlTree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_hRoot = m_ctrlTree.InsertItem( "Layers", 0, 1 );
}

void CLayerView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if( m_ctrlTree )
	{
		CRect rcRect;
		GetClientRect( &rcRect );
		m_ctrlTree.MoveWindow( &rcRect );
	}
}

void CLayerView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	char szPathName[MAX_PATH] = {0,};
	DragQueryFile( hDropInfo, 0, szPathName, MAX_PATH );

	CString strFileName = szPathName;
	int nLength = strFileName.GetLength();

	if( nLength > 2 )
	{
		bool bUIFile = false;
		if( strFileName[nLength-2] == 'u' && strFileName[nLength-1] == 'i' ) bUIFile = true;

		if( bUIFile )
		{
			CLayoutView *pLayoutView;
			pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
			pLayoutView->AddBackgroundDialog( szPathName );

			m_ctrlTree.InsertItem( szPathName, 2, 3, m_hRoot, TVI_FIRST );
			m_ctrlTree.Expand( m_hRoot, TVE_EXPAND );
		}
		else
		{
			MessageBox( "ui���ϸ� �ε� �����մϴ�.", "���", MB_OK );
		}
	}

	DragFinish( hDropInfo );

	CFormView::OnDropFiles(hDropInfo);
}

void CLayerView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hSelectItem = m_ctrlTree.GetSelectedItem();
	int nTreeDepth = GetTreeDepth( hSelectItem );

	if( nTreeDepth == 1 )
	{
		CMenu *pMenu = m_pContextMenu->GetSubMenu( 2 );
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
	else if( nTreeDepth == 2 )
	{
		CMenu *pMenu = m_pContextMenu->GetSubMenu( 3 );
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
}

int CLayerView::GetTreeDepth( HTREEITEM hItem )
{
	int nDepth;
	nDepth = 1;
	while( 1 )
	{
		hItem = m_ctrlTree.GetParentItem( hItem );
		if( hItem )
		{
			nDepth++;
		}
		else
		{
			break;
		}
	}

	return nDepth;
}

void CLayerView::OnLayerClearAll()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->ClearBackgroundDialog();
	m_ctrlTree.DeleteAllItems();
	m_hRoot = m_ctrlTree.InsertItem( "Layers", 0, 1 );
}

void CLayerView::OnLayerClear()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hti = m_ctrlTree.GetSelectedItem();
	int nPeerOrder = GetCurrentPeerOrder();

	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->DeleteBackgroundDialog( nPeerOrder );
	m_ctrlTree.DeleteItem( hti );
}

int CLayerView::GetCurrentPeerOrder()
{
	HTREEITEM hti = m_ctrlTree.GetSelectedItem();
	int depth = 0;
	while( hti != NULL ) {
		hti = m_ctrlTree.GetPrevSiblingItem( hti );
		depth++;
	}
	return depth-1;
}