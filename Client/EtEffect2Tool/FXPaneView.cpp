// FXPaneView.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "EtEffect2Tool.h"
#include "FXPaneView.h"
#include "GlobalValue.h"
#include "ParticlePaneView.h"
#include "PropertyPaneView.h"
#include "KeyframePropPaneView.h"
#include "KeyControlPaneView.h"
#include "AxisRenderObject.h"
// CFXPaneView

IMPLEMENT_DYNCREATE(CFXPaneView, CFormView)

CFXPaneView::CFXPaneView()
	: CFormView(CFXPaneView::IDD)
{
	m_bActivate = false;
	m_pContextMenu = new CMenu;
	m_pContextMenu->LoadMenu( IDR_CONTEXT );
	m_szSelectName[0] = '\0';
}

CFXPaneView::~CFXPaneView()
{
	SAFE_DELETE( m_pContextMenu );
}

void CFXPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}

BEGIN_MESSAGE_MAP(CFXPaneView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFXPaneView::OnTvnSelchangedTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CFXPaneView::OnNMClickTree1)
	ON_COMMAND(ID_FX_UP, &CFXPaneView::OnFxUp)
	ON_COMMAND(ID_FX_DOWN, &CFXPaneView::OnFxDown)
	ON_COMMAND(ID_FX_COPY, &CFXPaneView::OnFxCopy)
	ON_COMMAND(ID_FX_PASTE, &CFXPaneView::OnFxPaste)
END_MESSAGE_MAP()


// CFXPaneView diagnostics

#ifdef _DEBUG
void CFXPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFXPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFXPaneView message handlers

void CFXPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if( m_bActivate == true ) return;
	m_bActivate = true;

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_WORKSPACEICON );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 14, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );

	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_TreeCtrl.Activate();

	m_TreeCtrl.DeleteAllItems();
	m_rootItem = m_TreeCtrl.InsertItem( "FX", 0, 1, 0 );
	CGlobalValue::GetInstance().SetFXPaneView( this );

	// TODO: Add your specialized code here and/or call the base class
}

HTREEITEM CFXPaneView::GetCurrentItem()
{
	HTREEITEM hti = m_TreeCtrl.GetSelectedItem();
	return hti;
}

int CFXPaneView::GetCurrentDepth()
{
	HTREEITEM hti = GetCurrentItem();
	int depth = -1;
	while( hti != 0 ) {
		hti = m_TreeCtrl.GetParentItem( hti );
		depth++;
	}
	return depth;
}

int CFXPaneView::GetCurrentPeerOrder()
{
	HTREEITEM hti = GetCurrentItem();
	int depth = 0;
	while( hti != NULL ) {
		hti = m_TreeCtrl.GetPrevSiblingItem( hti );
		depth++;
	}
	return depth-1;
}

void CFXPaneView::OnSelected()
{
	int depth = GetCurrentDepth();
	HTREEITEM hti = GetCurrentItem();
	hti = m_TreeCtrl.GetParentItem( hti );
	CString str = m_TreeCtrl.GetItemText( hti );
	CGlobalValue::GetInstance().SetPosOrRot( -1 );
	if( depth == 0 ) {
		CGlobalValue::GetInstance().GetPropertyPaneView()->OnGlobalFXSelected();
	}
	else if( depth == 1 ) {
	}
	else if( depth == 2 ) {
		int peerOrder = GetCurrentPeerOrder();
		if( peerOrder == 0 ) {	// lifetime
			CGlobalValue::GetInstance().GetPropertyPaneView()->OnFXSelected( str );
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 0 );
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 3 );
		}
		else if( peerOrder == 1 ) {	// color, alpha, scale
			CGlobalValue::GetInstance().GetPropertyPaneView()->OnEmpty();
			CGlobalValue::GetInstance().GetKeyframePropPaneView()->OnFXSelected( str );
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 0 );
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 4 );
		}
		else if( peerOrder == 2 ) {	// position
			CGlobalValue::GetInstance().GetPropertyPaneView()->OnFXSelected( str );
			CGlobalValue::GetInstance().GetPropertyPaneView()->OnEmpty();
			CGlobalValue::GetInstance().GetKeyControlPaneView()->Reset();
			CGlobalValue::GetInstance().GetKeyControlPaneView()->OnFXSelected( str );
			CGlobalValue::GetInstance().GetKeyControlPaneView()->Invalidate();
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 3 );
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 5 );
			CGlobalValue::GetInstance().SetPosOrRot( 0 );
			CGlobalValue::GetInstance().GetAxisRenderObject()->SetRotation(EtVector3(0,0,0));
		}
		else if( peerOrder == 3 ) {	// rotation
			CGlobalValue::GetInstance().GetPropertyPaneView()->OnEmpty();
			CGlobalValue::GetInstance().GetKeyControlPaneView()->Reset();
			CGlobalValue::GetInstance().GetKeyControlPaneView()->OnFXSelected( str );
			CGlobalValue::GetInstance().GetKeyControlPaneView()->Invalidate();
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 3 );
			((CMainFrame*)AfxGetMainWnd())->ShowPane( 5 );
			CGlobalValue::GetInstance().SetPosOrRot( 1 );
			CGlobalValue::GetInstance().GetAxisRenderObject()->SetPosition(EtVector3(0,0,0));
		}
	}
}

void CFXPaneView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int depth = GetCurrentDepth();
	CMenu *pSubMenu = NULL;
	int nMenuIndex = 0;
	if( depth == 1 ) nMenuIndex = 6;
	else if( depth == 2 ) nMenuIndex = 5;
	if( depth == 1 || depth == 2 )
	{
		pSubMenu = m_pContextMenu->GetSubMenu( nMenuIndex );
		pSubMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
}

void CFXPaneView::AddItem( char *pDescription )
{
	HTREEITEM hti = m_TreeCtrl.InsertItem( pDescription, 6, 7, m_rootItem );
	m_TreeCtrl.SetItemState(hti, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
	m_TreeCtrl.InsertItem( "Setting", 0, 1, hti );
	m_TreeCtrl.InsertItem( "Color Alpha Scale", 0, 1, hti );
	m_TreeCtrl.InsertItem( "Position", 0, 1, hti );
	m_TreeCtrl.InsertItem( "Rotation", 0, 1, hti );
	m_TreeCtrl.Expand( m_rootItem, TVE_EXPAND );
}

bool CFXPaneView::IsVisibleItem( char *pDesc ) 
{
	HTREEITEM hti = m_TreeCtrl.GetChildItem( m_rootItem );

	while( hti != NULL ) {
		CString str = m_TreeCtrl.GetItemText( hti );
		if( strcmp(str.GetBuffer(), pDesc) == 0 ) {
			DWORD dwState = m_TreeCtrl.GetItemState( hti, TVIS_STATEIMAGEMASK) >> 12;
			return (dwState==2);			
		}
		hti = m_TreeCtrl.GetNextSiblingItem( hti );
	}
	return false;
}

void CFXPaneView::RemoveItem( char *pDesc ) 
{
	HTREEITEM hti = m_TreeCtrl.GetChildItem( m_rootItem );

	while( hti != NULL ) {
		CString str = m_TreeCtrl.GetItemText( hti );
		if( strcmp(str.GetBuffer(), pDesc) == 0 ) {
			m_TreeCtrl.DeleteItem( hti );
			break;
		}
		hti = m_TreeCtrl.GetNextSiblingItem( hti );
	}
}

void CFXPaneView::SetLayer( char *pDesc, bool bUp )
{
	HTREEITEM htiSub;
	HTREEITEM hti = m_TreeCtrl.GetChildItem( m_rootItem );
	CString str, strSub;

	while( hti != NULL ) {
		str = m_TreeCtrl.GetItemText( hti );
		if( strcmp(str.GetBuffer(), pDesc) == 0 ) {
			if( bUp ) {
				strSub = m_TreeCtrl.GetItemText( htiSub );
				m_TreeCtrl.SetItemText( hti, strSub.GetBuffer() );
				m_TreeCtrl.SetItemText( htiSub, str.GetBuffer() );
			}
			else {
				htiSub = m_TreeCtrl.GetNextSiblingItem( hti );
				strSub = m_TreeCtrl.GetItemText( htiSub );
				m_TreeCtrl.SetItemText( hti, strSub.GetBuffer() );
				m_TreeCtrl.SetItemText( htiSub, str.GetBuffer() );
			}
			break;
		}
		htiSub = hti;
		hti = m_TreeCtrl.GetNextSiblingItem( hti );
	}
}

char* CFXPaneView::GetUniqueName( char *name)
{
	static char szResult[ 255 ] = {0,};

	bool bBreak = true;
	do {
		m_uniqueStringCounter[name]++;
		sprintf(szResult, "%s - %d", name, m_uniqueStringCounter[name]);

		HTREEITEM hti = m_TreeCtrl.GetChildItem( m_rootItem );
		bBreak = true;
		while( hti != NULL ) {
			CString str = m_TreeCtrl.GetItemText( hti );
			if( strcmp(szResult, str.GetBuffer()) == 0 ) {
				bBreak = false;
				break;
			}
			hti = m_TreeCtrl.GetNextSiblingItem( hti );
		}
	} while( !bBreak );

	return szResult;
}

bool CFXPaneView::IsCanAddParticle()
{
	if( 	CGlobalValue::GetInstance().GetParticlePaneView()->GetCurrentDepth() == 1 ) {
		return true;
	}	
	return false;
}

char *CFXPaneView::AddSelectedParticleToGrid()
{
	char *uniqueName = GetUniqueName(m_szSelectName);
	AddItem( uniqueName );	
	return uniqueName;
}

void CFXPaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( m_TreeCtrl ) {
		CRect rcRect;
		GetClientRect( &rcRect );
		m_TreeCtrl.MoveWindow( &rcRect );
	}
}

void CFXPaneView::Reset()
{
	m_TreeCtrl.DeleteAllItems();	
	m_rootItem = m_TreeCtrl.InsertItem( "FX", 0, 1, 0 );
}

void CFXPaneView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	OnSelected();
	*pResult = 0;
}

void CFXPaneView::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	OnSelected();
	*pResult = 0;
}

void CFXPaneView::OnFxUp()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hti = GetCurrentItem();
	CString str = m_TreeCtrl.GetItemText( hti );
	CGlobalValue::GetInstance().SetFXItemLayer( str.GetBuffer(), true );
}

void CFXPaneView::OnFxDown()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hti = GetCurrentItem();
	CString str = m_TreeCtrl.GetItemText( hti );
	CGlobalValue::GetInstance().SetFXItemLayer( str.GetBuffer(), false );
}

void CFXPaneView::OnFxCopy()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hti = GetCurrentItem();
	m_strCopyFXName = m_TreeCtrl.GetItemText( hti );
}
void CFXPaneView::OnFxPaste()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if( m_strCopyFXName.IsEmpty() ) return;

	int depth = GetCurrentDepth();
	HTREEITEM hti = GetCurrentItem();
	CString str = m_TreeCtrl.GetItemText( hti );
	if( depth == 1 ) {
		// ���� �ٿ��ֱ�
		CGlobalValue::GetInstance().CopyFXItem( m_strCopyFXName.GetBuffer(), str.GetBuffer() );
	}
	else if( depth == 2 ) {
		int nType = -1;
		if( str == "Setting" ) nType = 1;
		else if( str == "Color Alpha Scale" ) nType = 2;
		else if( str == "Position" ) nType = 3;
		else if( str == "Rotation" ) nType = 4;
		if( nType != -1 ) {
			HTREEITEM htiParent = m_TreeCtrl.GetParentItem( hti );
			CString strParent = m_TreeCtrl.GetItemText( htiParent );
			CGlobalValue::GetInstance().CopyFXItem( m_strCopyFXName.GetBuffer(), strParent.GetBuffer(), nType );
		}
	}
}
