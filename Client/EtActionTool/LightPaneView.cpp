// LightPaneView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "LightPaneView.h"
#include "ActionBase.h"
#include "GlobalValue.h"
#include "UserMessage.h"
#include "ObjectLightFolder.h"
#include "ObjectLightDir.h"
#include "ObjectLightSpot.h"
#include "ObjectLightPoint.h"
#include "resource.h"
#include "PaneDefine.h"
#include "MainFrm.h"


// CLightPaneView

IMPLEMENT_DYNCREATE(CLightPaneView, CFormView)

CLightPaneView::CLightPaneView()
	: CFormView(CLightPaneView::IDD)
{
	m_bActivate = false;
	m_pContextMenu = new CMenu;
	m_pContextMenu->LoadMenu( IDR_CONTEXT );
}

CLightPaneView::~CLightPaneView()
{
	SAFE_DELETE( m_pContextMenu );
}

void CLightPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}

BEGIN_MESSAGE_MAP(CLightPaneView, CFormView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( UM_LIGHT_PANE_ADD, OnAdd )
	ON_MESSAGE( UM_REFRESH_PANE, OnRefresh )
	ON_MESSAGE( UM_TREEEX_MODIFY_CHECKED, OnCheckItem )
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_LIGHTPANE_ADDDIRECTIONAL, &CLightPaneView::OnLightpaneAdddirectional)
	ON_COMMAND(ID_LIGHTPANE_ADDPOINT, &CLightPaneView::OnLightpaneAddpoint)
	ON_COMMAND(ID_LIGHTPANE_ADDSPOT, &CLightPaneView::OnLightpaneAddspot)
	ON_COMMAND(ID_LIGHTPANECONTEXT_DELETE, &CLightPaneView::OnLightpanecontextDelete)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CLightPaneView::OnTvnSelchangedTree1)
END_MESSAGE_MAP()


// CLightPaneView �����Դϴ�.

#ifdef _DEBUG
void CLightPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLightPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLightPaneView �޽��� ó�����Դϴ�.

void CLightPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	m_TreeCtrl.EnableMultiSelect();

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_LIGHTICON );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 4, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );

	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_TreeCtrl.Activate();

	SendMessage( UM_REFRESH_PANE );
}

void CLightPaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if( m_TreeCtrl ) {
		m_TreeCtrl.MoveWindow( 0, 0, cx, cy );
	}
}

BOOL CLightPaneView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	return TRUE;
//	return CFormView::OnEraseBkgnd(pDC);
}

void CLightPaneView::CalcSelectItemList()
{
	m_SelectItemList.RemoveAll();
	m_TreeCtrl.GetSelectedList( m_SelectItemList );
}


LRESULT CLightPaneView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	m_TreeCtrl.DeleteAllItems();
	if( CGlobalValue::GetInstance().GetRootLightObject() == NULL ) return S_OK;

	CObjectLightFolder *pFolder = (CObjectLightFolder *)CGlobalValue::GetInstance().GetRootLightObject();
	SendMessage( UM_LIGHT_PANE_ADD, (WPARAM)pFolder );

	for( DWORD i=0; i<pFolder->GetChildCount(); i++ ) {
		SendMessage( UM_LIGHT_PANE_ADD, (WPARAM)pFolder->GetChild(i) );
	}

	return S_OK;
}

LRESULT CLightPaneView::OnAdd( WPARAM wParam, LPARAM lParam )
{
	CActionBase *pBase = (CActionBase *)wParam;

	int nIconID = 0;
	switch( pBase->GetType() ){
		case CActionBase::LIGHTFOLDER:
			nIconID = 0;
			break;
		case CActionBase::LIGHTDIR:
		case CActionBase::LIGHTSPOT:
		case CActionBase::LIGHTPOINT:
			nIconID = 2;
			break;
	}

	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	HTREEITEM hParent = ( pBase->GetParent() ) ? pBase->GetParent()->GetTreeItemID() : NULL ;
	HTREEITEM hti = m_TreeCtrl.InsertItem( pBase->GetName(), nIconID, nIconID+1, hParent ? hParent : 0 );
	pBase->SetTreeCtrl( &m_TreeCtrl );
	pBase->SetTreeItemID( hti );

	m_TreeCtrl.Expand( hParent, TVE_EXPAND );
	if( nIconID == 2 )
		m_TreeCtrl.SetItemState( hti, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );	
	else m_TreeCtrl.SetItemState( hti, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK );	

	return S_OK;
}

void CLightPaneView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CMenu *pMenu = m_pContextMenu->GetSubMenu(1);

	CalcSelectItemList();
	int nSelectCount = (int)m_SelectItemList.GetCount();

	pMenu->EnableMenuItem( ID_LIGHTPANECONTEXT_DELETE, MF_BYCOMMAND | MF_ENABLED );
	if( nSelectCount == 0 ) {
		pMenu->EnableMenuItem( ID_LIGHTPANECONTEXT_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
	else if( nSelectCount == 1 ) {
		CActionBase *pRoot = CGlobalValue::GetInstance().GetRootLightObject();
		CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, m_SelectItemList.GetHead() );
		if( !pBase || pBase->GetType() == CActionBase::LIGHTFOLDER )
			pMenu->EnableMenuItem( ID_LIGHTPANECONTEXT_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}

	pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}

void CLightPaneView::OnLightpaneAdddirectional()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CObjectLightDir *pLight = new CObjectLightDir;
	pLight->SetName( CGlobalValue::GetInstance().GetUniqueName( CActionBase::LIGHTDIR ) );
	CGlobalValue::GetInstance().AddLight( pLight );
}

void CLightPaneView::OnLightpaneAddpoint()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CObjectLightPoint *pLight = new CObjectLightPoint;
	pLight->SetName( CGlobalValue::GetInstance().GetUniqueName( CActionBase::LIGHTPOINT ) );
	CGlobalValue::GetInstance().AddLight( pLight );
}

void CLightPaneView::OnLightpaneAddspot()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CObjectLightSpot *pLight = new CObjectLightSpot;
	pLight->SetName( CGlobalValue::GetInstance().GetUniqueName( CActionBase::LIGHTSPOT ) );
	CGlobalValue::GetInstance().AddLight( pLight );
}

void CLightPaneView::OnLightpanecontextDelete()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CalcSelectItemList();

	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext(p);
		CGlobalValue::GetInstance().RemoveLightFromTreeID( hti );
		m_TreeCtrl.DeleteItem( hti );

	}
}

void CLightPaneView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	CActionBase *pRoot = CGlobalValue::GetInstance().GetRootLightObject();

	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti );
		if( pBase ) pBase->SetFocus( false );
	}
	CGlobalValue::GetInstance().SetControlObject( NULL );

	CalcSelectItemList();

	p = m_SelectItemList.GetHeadPosition();
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti );
		if( pBase ) pBase->SetFocus( true );
	}

	int nCount = (int)m_SelectItemList.GetCount();
	if( nCount == 0 ) {
		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, NULL );
		pWnd = GetPaneWnd( EVENT_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, NULL );
	}
	else if( nCount > 1 ) {
		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );
		pWnd = GetPaneWnd( EVENT_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );
	}
	else {
		HTREEITEM hti = m_SelectItemList.GetHead();
		CString szName = m_TreeCtrl.GetItemText( m_SelectItemList.GetHead() );
		CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti );

		CGlobalValue::GetInstance().SetControlObject( pBase );

		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase );

		pWnd = GetPaneWnd( EVENT_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );
	}
}

LRESULT CLightPaneView::OnCheckItem( WPARAM wParam, LPARAM lParam )
{
	HTREEITEM hti = (HTREEITEM)wParam;

	CActionBase *pRoot = CGlobalValue::GetInstance().GetRootLightObject();
	CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti );
	if( pBase ) {
		pBase->Show( lParam == 1 );
	}
	return S_OK;
}
