// ActionPaneView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "ActionPaneView.h"
#include "UserMessage.h"
#include "ActionBase.h"
#include "GlobalValue.h"
#include "AddActionDlg.h"
#include "resource.h"
#include "MainFrm.h"
#include "PaneDefine.h"
#include "SignalManager.h"
#include "ActionElement.h"
#include "ActionSignal.h"
#include "../Common/Utility/CSVWriter.h"
#include "SignalListViewDlg.h"



// CActionPaneView

IMPLEMENT_DYNCREATE(CActionPaneView, CFormView)

CActionPaneView::CActionPaneView()
	: CFormView(CActionPaneView::IDD)
{
	m_bActivate = false;

	m_pContextMenu = new CMenu;
	m_pContextMenu->LoadMenu( IDR_CONTEXT );

	m_hRootItem = 0;
	/*
	m_pDragImage = NULL;
	m_bDragging = false;
	m_hDrag = 0;
	m_hDrop = 0;
	*/
}

CActionPaneView::~CActionPaneView()
{
	SAFE_DELETE( m_pContextMenu );
}

void CActionPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}

BEGIN_MESSAGE_MAP(CActionPaneView, CFormView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( UM_REFRESH_PANE, OnRefresh )
	ON_MESSAGE( UM_ACTION_PANE_ADD, OnAdd )
	ON_WM_CREATE()
	ON_COMMAND(ID_ACTION_REMOVE_ELEMENT, &CActionPaneView::OnActionRemoveElement)
	ON_COMMAND(ID_ACTION_ADD_ELEMENT, &CActionPaneView::OnActionAddElement)
	ON_COMMAND(ID_ACTION_CLONE_ELEMENT, &CActionPaneView::OnActionCloneElement)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ADD_ELEMENT, &CActionPaneView::OnUpdateActionAddElement)
	ON_UPDATE_COMMAND_UI(ID_ACTION_REMOVE_ELEMENT, &CActionPaneView::OnUpdateActionRemoveElement)
	ON_UPDATE_COMMAND_UI(ID_ACTION_CLONE_ELEMENT, &CActionPaneView::OnUpdateActionCloneElement)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ACTION_ADD_DLG, &CActionPaneView::OnActionAddDlg)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ADD_DLG, &CActionPaneView::OnUpdateActionAddDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CActionPaneView::OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE1, &CActionPaneView::OnTvnBeginlabeleditTree1)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE1, &CActionPaneView::OnTvnEndlabeleditTree1)
	/*
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE1, &CActionPaneView::OnTvnBegindragTree1)
	*/
	ON_COMMAND(ID_ACTIONPANECONTEXT_EXPORTEXCEL, &CActionPaneView::OnActionpanecontextExportexcel)
END_MESSAGE_MAP()


// CActionPaneView 진단입니다.

#ifdef _DEBUG
void CActionPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CActionPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CActionPaneView 메시지 처리기입니다.

void CActionPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	m_TreeCtrl.EnableMultiSelect();

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_ACTIONICON );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 4, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );

	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );

}

void CActionPaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int nTop = 0;
	if( m_wndToolBar.GetSafeHwnd() )
	{
		CSize sz = m_wndToolBar.CalcDockingLayout(cx, LM_HORZDOCK|LM_HORZ | LM_COMMIT);

		m_wndToolBar.MoveWindow(0, 0, cx, sz.cy);
		m_wndToolBar.Invalidate(FALSE);
		nTop += sz.cy;
	}

	if( m_TreeCtrl ) {
		m_TreeCtrl.MoveWindow( 0, nTop, cx, cy - nTop );
	}
}

BOOL CActionPaneView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return FALSE;
//	return CFormView::OnEraseBkgnd(pDC);
}

LRESULT CActionPaneView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	CActionBase *pBase = (CActionBase *)wParam;

	if( pBase ) {
		SendMessage( UM_ACTION_PANE_ADD, (WPARAM)pBase );

		for( DWORD i=0; i<pBase->GetChildCount(); i++ ) {
			CActionBase *pChild = pBase->GetChild(i);
			SendMessage( UM_ACTION_PANE_ADD, (WPARAM)pChild );
		}
//		m_TreeCtrl.SortChildren( pBase->GetTreeItemID() );
	}
	else {
		m_TreeCtrl.DeleteAllItems();
		m_hRootItem = 0;
	}

	return S_OK;
}

LRESULT CActionPaneView::OnAdd( WPARAM wParam, LPARAM lParam )
{
	CActionBase *pBase = (CActionBase *)wParam;
	int nIconID = 0;
	switch( pBase->GetType() ) {
		case CActionBase::OBJECT:
			m_hRootItem = 0;
			m_TreeCtrl.DeleteAllItems();
			nIconID = 0;
			break;
		case CActionBase::ELEMENT:
			nIconID = 2;
			break;
	}
	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	HTREEITEM hParent = ( pBase->GetParent() ) ? pBase->GetParent()->GetTreeItemID() : NULL ;
	HTREEITEM hti = m_TreeCtrl.InsertItem( pBase->GetName(), nIconID, nIconID+1, hParent ? hParent : 0 );
	if( hParent == NULL ) m_hRootItem = hti;
	pBase->SetTreeCtrl( &m_TreeCtrl );
	pBase->SetTreeItemID( hti );

	m_TreeCtrl.Expand( hParent, TVE_EXPAND );
	m_TreeCtrl.SelectItems( hti, hti, TRUE );
	m_TreeCtrl.FocusItem( hti );

	m_TreeCtrl.SortChildren( hParent );
	return S_OK;
}

void CActionPaneView::CalcSelectItemList()
{
	m_SelectItemList.RemoveAll();
	m_TreeCtrl.GetSelectedList( m_SelectItemList );
}

int CActionPaneView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_wndToolBar.CreateToolBar(WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this);
	m_wndToolBar.LoadToolBar(IDR_ACTION_TOOLBAR);

	return 0;
}

void CActionPaneView::OnActionRemoveElement()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CalcSelectItemList();

	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext(p);
		CGlobalValue::GetInstance().RemoveElement( hti );
		m_TreeCtrl.DeleteItem( hti );

	}
}

void CActionPaneView::OnActionAddElement()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szName = CGlobalValue::GetInstance().GetUniqueName( CActionBase::ELEMENT );

	CGlobalValue::GetInstance().AddElement( szName );
}

void CActionPaneView::OnActionCloneElement()
{
	CalcSelectItemList();
	if( m_SelectItemList.GetCount() != 1 ) return;

	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	CString szCloneName;

	CActionBase *pRoot = CGlobalValue::GetInstance().GetRootObject();
	bool bFirst = true;
	while(p) {
		hti = m_SelectItemList.GetNext(p);
		CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti );
		if( pBase == NULL ) continue;

		int nCount = 0;
		while(1) {
			nCount++;
			szCloneName.Format( "%s_Clone[%d]", pBase->GetName(), nCount );

			if( CGlobalValue::GetInstance().IsSameName( pRoot, szCloneName ) == FALSE ) break;
		}

		CGlobalValue::GetInstance().AddElement( szCloneName );
		CActionBase *pClone = CGlobalValue::GetInstance().GetObjectFromName( pRoot, szCloneName );

		*(CActionElement*)pClone = *(CActionElement*)pBase;

		if( bFirst ) {
			m_TreeCtrl.SelectItems( pClone->GetTreeItemID(), pClone->GetTreeItemID(), TRUE );
			m_TreeCtrl.FocusItem( hti );
			bFirst = false;
		}
	}
}

void CActionPaneView::OnActionAddDlg()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CAddActionDlg Dlg;
	if( Dlg.DoModal() != IDOK ) return;

	switch( Dlg.m_nType ) {
		case 0:	// Define String
			CGlobalValue::GetInstance().AddElement( Dlg.GetResultString() );
			break;
		case 1:	// Package
			{
				int nIndex = CSignalManager::GetInstance().GetDefineCategoryIndex( Dlg.GetResultString() );
				if( nIndex == -1 ) break;

				for( DWORD i=0; i<CSignalManager::GetInstance().GetDefineStringCount( (DWORD)nIndex ); i++ ) {
					CString szStr = CSignalManager::GetInstance().GetDefineString( (DWORD)nIndex, i );
					CGlobalValue::GetInstance().AddElement( szStr );
				}
			}
			break;
		case 2: // Custom
			CGlobalValue::GetInstance().AddElement( Dlg.GetResultString() );
			break;
	}

}

void CActionPaneView::OnUpdateActionAddElement(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->Enable( CGlobalValue::GetInstance().IsOpenAction() );
}

void CActionPaneView::OnUpdateActionRemoveElement(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	BOOL bEnable = FALSE;
	if( CGlobalValue::GetInstance().IsOpenAction() ) bEnable = TRUE;

	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	CActionBase *pRoot = CGlobalValue::GetInstance().GetRootObject();
	while(p) {
		hti = m_SelectItemList.GetNext(p);
		if( !CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti ) || 
			 CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti )->GetType() != CActionBase::ELEMENT ) {
			bEnable = FALSE;
			break;
		}
	}

	pCmdUI->Enable( bEnable );
}


void CActionPaneView::OnUpdateActionCloneElement(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->Enable( ( m_SelectItemList.GetCount() == 1 ) ? TRUE : FALSE );
}

void CActionPaneView::OnUpdateActionAddDlg(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->Enable( CGlobalValue::GetInstance().IsOpenAction() );
}


void CActionPaneView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CMenu *pMenu = m_pContextMenu->GetSubMenu(0);

	UINT nEnable;
	if( CGlobalValue::GetInstance().IsOpenAction() ) 
		nEnable = MF_BYCOMMAND | MF_ENABLED;
	else nEnable = MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;

	pMenu->EnableMenuItem( ID_ACTION_ADD_ELEMENT, nEnable );
	pMenu->EnableMenuItem( ID_ACTION_REMOVE_ELEMENT, nEnable );
	pMenu->EnableMenuItem( ID_ACTION_ADD_DLG, nEnable );

	if( m_SelectItemList.GetCount() == 1 )
		pMenu->EnableMenuItem( ID_ACTION_CLONE_ELEMENT, MF_BYCOMMAND | MF_ENABLED );
	else 
		pMenu->EnableMenuItem( ID_ACTION_CLONE_ELEMENT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

	pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}
void CActionPaneView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	/* 내가 작업 하기 이전의 소스들 */

	CActionBase *pRoot = CGlobalValue::GetInstance().GetRootObject();

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
		pBase->InitPropertyInfo();

		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase );

		pWnd = GetPaneWnd( EVENT_PANE );
		switch( pBase->GetType() ) {
			case CActionBase::OBJECT:
				if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, NULL );
				break;
			case CActionBase::ELEMENT:
				if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, (WPARAM)pBase );
				break;
		}
	}

	//CFile pStream;

	//TCHAR szFileter[] = _T("Excel File (*.csv) | *.csv|All File (*.*)|*.*||");
	//CFileDialog dlg( FALSE, _T("csv"), _T("*.csv"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING, szFileter, NULL );

	//CString str;

	//if( dlg.DoModal() == IDOK )
	//{
	//	MessageBox("Test", "Success", MB_OK);
	//	str = dlg.GetPathName();
	//}
	//else
	//{
	//	MessageBox("Test", "Failed", MB_OK);
	//}	

	//if(! pStream.Open( str.GetBuffer(), CFile::modeReadWrite ) )
	//{
	//	AfxMessageBox(_T("could not open file"));
	//	return;
	//}

	//int fsize = (int)pStream.GetLength();

	//pStream.Read( str.GetBuffer(fsize), fsize);

	//MessageBox( str.GetBuffer(), "읽은녀석들", MB_OK);

	//pStream.Close();
}

void CActionPaneView::OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	m_szPrevLabelString = pTVDispInfo->item.pszText;
}

void CActionPaneView::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if( pTVDispInfo->item.pszText == NULL ) return;

	CActionBase *pRoot = CGlobalValue::GetInstance().GetRootObject();
	if( CGlobalValue::GetInstance().GetObjectFromName( pRoot, CString( pTVDispInfo->item.pszText ) ) ) return;

	CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, pTVDispInfo->item.hItem );
	if( pBase == NULL ) return;
	if( pBase->GetType() != CActionBase::ELEMENT ) return;

	pBase->SetName( CString( pTVDispInfo->item.pszText ) );
	m_TreeCtrl.SetItemText( pTVDispInfo->item.hItem, pTVDispInfo->item.pszText );

	CActionBase *pParent = pBase->GetParent();
	for( DWORD i=0; i<pParent->GetChildCount(); i++ ) {
		((CActionElement*)pParent->GetChild(i))->ChangeNextActionName( m_szPrevLabelString, pTVDispInfo->item.pszText );
	}
	pBase->InitPropertyInfo();
	CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase );

	m_TreeCtrl.SortChildren( pRoot->GetTreeItemID() );
}

/*
void CActionPaneView::OnTvnBegindragTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// TODO: Add your control notification handler code here
	if( m_TreeCtrl.GetParentItem(pNMTreeView->itemNew.hItem) == NULL ) return; 

	m_hDrag = pNMTreeView->itemNew.hItem;

	m_pDragImage = m_TreeCtrl.CreateDragImage( m_hDrag );

	m_bDragging = true;
	m_pDragImage->BeginDrag( 0, CPoint(-15,0) );

	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );

	if( m_wndToolBar.GetSafeHwnd() ) {
		RECT wndRect;
		GetWindowRect( &wndRect );
		CSize sz = m_wndToolBar.CalcDockingLayout(wndRect.right - wndRect.left, LM_HORZDOCK|LM_HORZ | LM_COMMIT);
		pt.y += sz.cy;
	}

	m_pDragImage->DragEnter( NULL, pt );
	SetCapture();

	*pResult = 0;
}

void CActionPaneView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM	hitem;
	UINT		flags;

	if( m_bDragging ) {
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);

		pt = point;
		if( m_wndToolBar.GetSafeHwnd() ) {
			RECT wndRect;
			GetWindowRect( &wndRect );
			CSize sz = m_wndToolBar.CalcDockingLayout(wndRect.right - wndRect.left, LM_HORZDOCK|LM_HORZ | LM_COMMIT);
			pt.y -= sz.cy;
		}

		if( (hitem = m_TreeCtrl.HitTest(pt, &flags) ) != NULL ) {
			CImageList::DragShowNolock(FALSE);

			if( m_hRootItem == hitem ) {
				m_TreeCtrl.SelectDropTarget(hitem);
				m_hDrop = hitem;
			}
			else {
				m_TreeCtrl.SelectDropTarget(NULL);
				m_hDrop = NULL;
			}

			CImageList::DragShowNolock( TRUE );
		}
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CActionPaneView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bDragging ) {
		m_bDragging = false;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		if( m_pDragImage != NULL ) { 
			delete m_pDragImage; 
			m_pDragImage = NULL; 
		} 

		m_TreeCtrl.SelectDropTarget(NULL);

		if( m_hDrag != m_hDrop ) {
			HTREEITEM hitem;
			POINT pt2 = point;
			if( m_wndToolBar.GetSafeHwnd() ) {
				RECT wndRect;
				GetWindowRect( &wndRect );
				CSize sz = m_wndToolBar.CalcDockingLayout(wndRect.right - wndRect.left, LM_HORZDOCK|LM_HORZ | LM_COMMIT);
				pt2.y -= sz.cy;
			}

			if( ((hitem = m_TreeCtrl.HitTest(pt2, &nFlags)) == NULL)  ) return;
		}
	}

	CFormView::OnLButtonUp(nFlags, point);
}

BOOL CActionPaneView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	return CFormView::PreTranslateMessage(pMsg);
}
*/

void CActionPaneView::GetSelectActionList( std::vector<std::string> &szVecList )
{
	CalcSelectItemList();

	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext(p);
		if( m_hRootItem == hti ) continue;

		CActionBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( CGlobalValue::GetInstance().GetRootObject(), hti );
		if( !pBase ) continue;
		if( pBase->GetType() != CActionBase::ELEMENT ) continue;

		szVecList.push_back( pBase->GetName().GetBuffer() );
	}
}

#ifdef _EXPORT_EXCEL_ACTION_SIGNAL
void CActionPaneView::OnActionpanecontextExportexcel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( ! CGlobalValue::GetInstance().IsOpenAction() ){
		::MessageBox( NULL, "액션파일을 열어주세요", "Error", MB_OK );
		return;
	}

	CActionBase* pRoot = CGlobalValue::GetInstance().GetRootObject();
	CActionBase* pAction = NULL;
	CActionElement* pActionElement = NULL;

	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p){
		hti = m_SelectItemList.GetNext(p);
			
		pAction = CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti );
		
		if( pAction == NULL )
			continue;

		if( pAction->GetType() == CActionBase::ELEMENT ) {
			pActionElement = dynamic_cast<CActionElement*>( CGlobalValue::GetInstance().GetObjectFromTreeID( pRoot, hti ) );
			break;
		}
		else {
			continue;
		}
	}
	
	if( ! pActionElement->IsStop() ) {
		::MessageBox( NULL,"플레이 종료후 사용해 주세요", "경고", MB_OK) ;
		return;
	}

	TCHAR szFileter[] = _T("Excel File (*.csv) | *.csv|All File (*.*)|*.*||");
	CFileDialog dlg( FALSE, _T("csv"), _T("*.csv"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING, szFileter, NULL );

	if( dlg.DoModal() != IDOK )
		return;


	SignalListViewDlg	SignalListdlg;
	
	if( SignalListdlg.DoModal() == IDOK ){

		std::wstring wsFileName;
		ToWideString( (LPSTR) dlg.GetPathName().GetString(), wsFileName);	

		CSVWriter Writer;
		if( ! Writer.Initialize( wsFileName.c_str(), L";") ){
			::MessageBox(NULL, "could not open file", "Error", MB_OK );
			return;
		}

		CActionSignal*		pActionSignal = NULL;
		char				sTemp[_MAX_PATH] = {0, };
		int					nCount = 1;
		int					nSignalIndex = 0, i = 0;
		bool				bFlag = FALSE;

		Writer.StartWrite();	

		ZeroMemory( sTemp, _MAX_PATH );
		fprintf_s( Writer.GetStream(), "액션명\n");
		sprintf_s( sTemp, "%s,", pActionElement->GetName() );
		fprintf_s( Writer.GetStream(), "%s,", sTemp );
		fprintf_s( Writer.GetStream(), "\n\n");

		for( nSignalIndex = 0; nSignalIndex < pActionElement->GetChildCount(); nSignalIndex++ )
		{
			pActionSignal= dynamic_cast<CActionSignal*>( pActionElement->GetChild( nSignalIndex ) );

			if( ! pActionSignal )
				continue;

			if( SignalListdlg.m_strSignalTypeName.Compare( pActionSignal->GetProperty( 0 )->GetCategory() ) != 0) 
				continue;

			if( ! bFlag )		{
				ZeroMemory( sTemp, _MAX_PATH );
				sprintf_s( sTemp, "%s", pActionSignal->GetProperty( 0 )->GetCategory() );
				fprintf_s( Writer.GetStream(), "%s", sTemp );
				fprintf_s( Writer.GetStream(), ",");
 
				for(  i = 0; i < pActionSignal->GetPropertyList()->size(); i++ ){
					ZeroMemory( sTemp, _MAX_PATH );
					sprintf_s( sTemp, "%s", pActionSignal->GetProperty( i )->GetDescription() );
					fprintf_s( Writer.GetStream(), "%s,", sTemp );
				}
				bFlag = TRUE;
			}

			pActionSignal->ExportSignal( Writer.GetStream(), nCount++);
		}
		SignalListdlg.m_strSignalTypeName.Empty();

		Writer.AddField(3);
		Writer.EndWrite();
	}
	else{
		return;
	}



}
#endif 
