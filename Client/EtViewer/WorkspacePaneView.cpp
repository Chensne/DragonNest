// MeshPaneView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <shlwapi.h>
#include "EtViewer.h"
#include "WorkspacePaneView.h"
#include "UserMessage.h"
#include "RenderBase.h"

#include "MainFrm.h"

#include "PaneDefine.h"


// CWorkspacePaneView

IMPLEMENT_DYNCREATE(CWorkspacePaneView, CFormView)

CWorkspacePaneView::CWorkspacePaneView()
	: CFormView(CWorkspacePaneView::IDD)
{
	m_bActiavte = false;
	m_pContextMenu = new CMenu;
	m_pContextMenu->LoadMenu( IDR_CONTEXT );
}

CWorkspacePaneView::~CWorkspacePaneView()
{
	SAFE_DELETE( m_pContextMenu );
}

void CWorkspacePaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}

BEGIN_MESSAGE_MAP(CWorkspacePaneView, CFormView)
	ON_WM_SIZE()
	ON_MESSAGE( UM_WORKSPACEPANE_REFRESH, OnRefresh )
	ON_MESSAGE( UM_WORKSPACEPANE_ADD, OnAdd )
	ON_MESSAGE( UM_WORKSPACEPANE_REMOVE, OnRemove )
	ON_MESSAGE( UM_TREEEX_UPDATECHECK, OnCheckItem )
	ON_MESSAGE( UM_WORKSPACEPANE_SETSHOWFLAG, OnSetCheckItem )
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SCENE_ADD, &CWorkspacePaneView::OnSceneAdd)
	ON_COMMAND(ID_SCENE_REMOVE, &CWorkspacePaneView::OnSceneRemove)
	ON_COMMAND(ID_SKIN_ADD, &CWorkspacePaneView::OnSkinAdd)
	ON_COMMAND(ID_SKIN_REMOVE, &CWorkspacePaneView::OnSkinRemove)
	ON_COMMAND(ID_SKIN_SAVE, &CWorkspacePaneView::OnSkinSave)
	ON_COMMAND(ID_SKIN_SAVEAS, &CWorkspacePaneView::OnSkinSaveAs)
	ON_COMMAND(ID_ANIMATION_ADD, &CWorkspacePaneView::OnAnimationAdd)
	ON_COMMAND(ID_ANIMATION_REMOVE, &CWorkspacePaneView::OnAnimationRemove)
	ON_COMMAND( ID_ANIMATION_MERGE, &CWorkspacePaneView::OnAnimationMerge)
	ON_COMMAND(ID_COMMON_REMOVE, &CWorkspacePaneView::OnCommonRemove)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CWorkspacePaneView::OnTvnSelchangedTree1)
	ON_COMMAND(ID_LIGHT_ADD_DIR, &CWorkspacePaneView::OnLightAddDir)
	ON_COMMAND(ID_LIGHT_ADD_POINT, &CWorkspacePaneView::OnLightAddPoint)
	ON_COMMAND(ID_LIGHT_ADD_SPOT, &CWorkspacePaneView::OnLightAddSpot)
	ON_COMMAND(ID_LIGHT_REMOVE, &CWorkspacePaneView::OnLightRemove)
END_MESSAGE_MAP()


// CWorkspacePaneView 진단입니다.

#ifdef _DEBUG
void CWorkspacePaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWorkspacePaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWorkspacePaneView 메시지 처리기입니다.

void CWorkspacePaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActiavte == true ) return;
	m_bActiavte = true;

	m_TreeCtrl.EnableMultiSelect();

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_WORKSPACEICON );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 14, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );

	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_TreeCtrl.Activate();

//	m_TreeCtrl.EnableDrag( TRUE );
//	m_TreeCtrl.SetDragRect( TRUE );

}

void CWorkspacePaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_TreeCtrl ) {
		CRect rcRect;
		GetClientRect( &rcRect );
		m_TreeCtrl.MoveWindow( &rcRect );
	}
}

void CWorkspacePaneView::CalcSelectItemList()
{
	m_SelectItemList.RemoveAll();
	m_TreeCtrl.GetSelectedList( m_SelectItemList );
}


LRESULT CWorkspacePaneView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	m_TreeCtrl.DeleteAllItems();
	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );	

	return S_OK;
}

LRESULT CWorkspacePaneView::OnAdd( WPARAM wParam, LPARAM lParam )
{
	CObjectBase *pBase = (CObjectBase *)wParam;
	int nIconID = 0;
	bool bShowHide = false;
	switch( pBase->GetType() ) {
		case CObjectBase::SCENE:
			nIconID = 0;
			bShowHide = true;
			break;
		case CObjectBase::SKIN:
			nIconID = 2;
			bShowHide = true;
			break;
		case CObjectBase::SUBMESH:
			nIconID = 4;
			bShowHide = true;
			break;
		case CObjectBase::ANIMATION:
			nIconID = 8;
			break;
		case CObjectBase::LIGHTDIR:
		case CObjectBase::LIGHTPOINT:
		case CObjectBase::LIGHTSPOT:
			nIconID = 10;
			bShowHide = true;
			break;
		case CObjectBase::LIGHTFOLDER:
			nIconID = 12;
			break;
	}
	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	HTREEITEM hParent = pBase->GetParent()->GetTreeItemID();
	HTREEITEM hti = m_TreeCtrl.InsertItem( pBase->GetName(), nIconID, nIconID+1, hParent ? hParent : 0 );
	pBase->SetTreeCtrl( &m_TreeCtrl );
	pBase->SetTreeItemID( hti );

	m_TreeCtrl.Expand( hParent, TVE_EXPAND );
	if( bShowHide == true )
		m_TreeCtrl.SetItemState( hti, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );	
	else m_TreeCtrl.SetItemState( hti, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK );	

	return S_OK;
}

LRESULT CWorkspacePaneView::OnRemove( WPARAM wParam, LPARAM lParam )
{
	CObjectBase *pBase = (CObjectBase *)wParam;
	m_TreeCtrl.DeleteItem( pBase->GetTreeItemID() );
	return S_OK;
}

LRESULT CWorkspacePaneView::OnSetCheckItem( WPARAM wParam, LPARAM lParam )
{
	HTREEITEM hti = (HTREEITEM)wParam;
	m_TreeCtrl.SetItemState( hti, INDEXTOSTATEIMAGEMASK(1 + (int)lParam), TVIS_STATEIMAGEMASK );
	return S_OK;
}

bool CWorkspacePaneView::CheckSameTypeObject( int nType )
{
	bool bFlag = true;
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		if( CGlobalValue::GetInstance().GetObjectTypeFromDescription( m_TreeCtrl.GetItemText( hti ) ) != (CObjectBase::OBJECT_TYPE)nType ) {
			bFlag = false;
			break;
		}
	}
	return bFlag;

}

void CWorkspacePaneView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CMenu *pMenu = m_pContextMenu->GetSubMenu(0);

	std::vector<int> VecEnableMenuID;
	pMenu->EnableMenuItem( ID_COMMON_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

	pMenu->EnableMenuItem( ID_SCENE_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_SCENE_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

	pMenu->EnableMenuItem( ID_SKIN_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_SKIN_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_SKIN_SAVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_SKIN_SAVEAS, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

	pMenu->EnableMenuItem( ID_ANIMATION_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_ANIMATION_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_ANIMATION_MERGE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

	pMenu->EnableMenuItem( ID_LIGHT_ADD_DIR, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_LIGHT_ADD_POINT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_LIGHT_ADD_SPOT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_LIGHT_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );


	CalcSelectItemList();
	int nSelectCount = (int)m_SelectItemList.GetCount();
	if( nSelectCount == 0 ) {
		VecEnableMenuID.push_back( ID_SCENE_ADD );
	}
	else if( nSelectCount > 1 ) {
		VecEnableMenuID.push_back( ID_COMMON_REMOVE );


		// 같은 종류끼리만 골라졌을때 체크
		if( CheckSameTypeObject( (int)CObjectBase::ANIMATION ) == true ) {
			VecEnableMenuID.push_back( ID_ANIMATION_MERGE );
			VecEnableMenuID.push_back( ID_ANIMATION_REMOVE );
		}
		if( CheckSameTypeObject( (int)CObjectBase::SKIN ) == true ) {
			VecEnableMenuID.push_back( ID_SKIN_REMOVE );

			POSITION p = m_SelectItemList.GetHeadPosition();
			HTREEITEM hti;
			while(p) {
				hti = m_SelectItemList.GetNext( p );
				CObjectBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
				if( pBase->IsChildModify() == true ) {
					VecEnableMenuID.push_back( ID_SKIN_SAVE );
					break;
				}
			}

		}
		if( CheckSameTypeObject( (int)CObjectBase::SCENE ) == true ) {
			VecEnableMenuID.push_back( ID_SCENE_REMOVE );
		}
	}
	else {
		CString szName = m_TreeCtrl.GetItemText( m_SelectItemList.GetHead() );
		HTREEITEM hti = m_SelectItemList.GetHead();
		CObjectBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
		if( pBase == NULL ) return;
		switch( pBase->GetType() ) {
			case CObjectBase::SCENE:
				VecEnableMenuID.push_back( ID_SCENE_ADD );
				if( CGlobalValue::GetInstance().GetRootObject() && 
					CGlobalValue::GetInstance().GetRootObject()->GetChild(0) && 
					CGlobalValue::GetInstance().GetRootObject()->GetChild(0) != pBase ) {
					VecEnableMenuID.push_back( ID_SCENE_REMOVE );
					VecEnableMenuID.push_back( ID_COMMON_REMOVE );
				}
				VecEnableMenuID.push_back( ID_ANIMATION_ADD );
				VecEnableMenuID.push_back( ID_SKIN_ADD );
				break;
			case CObjectBase::SKIN:
				VecEnableMenuID.push_back( ID_SKIN_REMOVE );
				VecEnableMenuID.push_back( ID_COMMON_REMOVE );
				VecEnableMenuID.push_back( ID_ANIMATION_ADD );
				if( pBase->IsChildModify() == true ) {
					VecEnableMenuID.push_back( ID_SKIN_SAVE );
				}
				VecEnableMenuID.push_back( ID_SKIN_SAVEAS );
				break;
			case CObjectBase::SUBMESH:
				break;
			case CObjectBase::ANIMATION:
				VecEnableMenuID.push_back( ID_COMMON_REMOVE );
				VecEnableMenuID.push_back( ID_ANIMATION_REMOVE );
				break;
			case CObjectBase::LIGHTFOLDER:
				VecEnableMenuID.push_back( ID_LIGHT_ADD_DIR );
				VecEnableMenuID.push_back( ID_LIGHT_ADD_POINT );
				VecEnableMenuID.push_back( ID_LIGHT_ADD_SPOT );
				break;
			case CObjectBase::LIGHTDIR:
			case CObjectBase::LIGHTPOINT:
			case CObjectBase::LIGHTSPOT:
				VecEnableMenuID.push_back( ID_LIGHT_REMOVE );
				VecEnableMenuID.push_back( ID_COMMON_REMOVE );
				break;
		}
	}
	for( DWORD i=0; i<VecEnableMenuID.size(); i++ ) {
		pMenu->EnableMenuItem( VecEnableMenuID[i], MF_BYCOMMAND | MF_ENABLED );
	}

	pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}

void CWorkspacePaneView::OnSceneAdd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CObjectBase *pBase = new CObjectScene;
	CGlobalValue::GetInstance().AddObject( CString(), pBase );

}

void CWorkspacePaneView::OnSceneRemove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( hti );
	}
}

void CWorkspacePaneView::OnSkinAdd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFileList;
	TCHAR szFilter[] = _T( "Eternity Skin File (*.skn)|*.skn|Eternity Animation File (*.ani)|*.ani|Eternity Support File (*.skn;*.ani)|*.skn;*.ani|All Files (*.*)|*.*||" );
	CFileDialog dlg( TRUE, _T("skn"), _T("*.skn"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT| OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );
	dlg.GetOFN().lpstrFile = szFileList.GetBuffer(100000);
	dlg.GetOFN().nMaxFile = 100000;

	dlg.m_ofn.lpstrTitle = "Open Eternity Engine File";

	if( IDOK != dlg.DoModal() ) {
		szFileList.ReleaseBuffer();
		return;
	}

	std::vector<CString> szVecSkinList;
	std::vector<CString> szVecAniList;
	CGlobalValue::GetInstance().ParseFileList( szFileList, "skn", szVecSkinList );
	CGlobalValue::GetInstance().ParseFileList( szFileList, "ani", szVecAniList );
	szFileList.ReleaseBuffer();

	if( szVecSkinList.size() == 0 ) return;
	else if( szVecSkinList.size() == 1 ) {
		float fPer = 100 / (float)szVecAniList.size() + 1.f;
		CGlobalValue::GetInstance().UpdateProgress( 0 );

		char szTemp[512];
		_GetFullFileName( szTemp, _countof(szTemp), szVecSkinList[0] );
		CObjectBase *pBase = new CObjectSkin;
		pBase->SetName( CString(szTemp) );
		((CObjectSkin*)pBase)->LoadSkin( szVecSkinList[0], true );

		if( strstr( szVecSkinList[0], "Weapon" ) )
		{
			if( StrStrI( szVecSkinList[0], "Gauntlet" ) == 0 &&
				StrStrI( szVecSkinList[0], "Academicglove" ) == 0 &&
				StrStrI( szVecSkinList[0], "Charm" ) == 0 )
				((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_WEAPON );
		}
		else if( strstr( szVecSkinList[0], "Cash_Item" ) )
		{
			if( StrStrI( szVecSkinList[0], "EarRing" ) )
				((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_EARRING );
			else if( StrStrI( szVecSkinList[0], "Wing" ) )
				((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_WING );
			else if( StrStrI( szVecSkinList[0], "Tail" ) )
				((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_TAIL );
		}

		CGlobalValue::GetInstance().AddObject( m_SelectItemList.GetHead(), pBase );

		CGlobalValue::GetInstance().UpdateProgress( (int)(fPer*1.f) );
		// 에뉘 로딩
		for( DWORD i=0; i<szVecAniList.size(); i++ ) {
			char szTemp[512];
			_GetFullFileName( szTemp, _countof(szTemp), szVecAniList[i] );
			CObjectBase *pBaseAni = new CObjectAnimation;

			if( ((CObjectAnimation*)pBaseAni)->LoadAnimation( szVecAniList[i] ) == false ) {
				SAFE_DELETE( pBaseAni );
				continue;
			}

			CGlobalValue::GetInstance().AddObject( pBase->GetTreeItemID(), pBaseAni );

			CGlobalValue::GetInstance().UpdateProgress( (int)(fPer*(i+2)) );
		}
		CGlobalValue::GetInstance().UpdateProgress( 0 );
	}
	else if( szVecSkinList.size() > 1 ) {
		if( szVecAniList.size() > 0 ) return;

		float fPer = 100 / (float)szVecSkinList.size() + 1.f;
		CGlobalValue::GetInstance().UpdateProgress( 0 );
		for( DWORD i=0; i<szVecSkinList.size(); i++ ) {
			char szTemp[512];
			_GetFullFileName( szTemp, _countof(szTemp), szVecSkinList[i] );
			CObjectBase *pBase = new CObjectSkin;
			pBase->SetName( CString(szTemp) );
			((CObjectSkin*)pBase)->LoadSkin( szVecSkinList[i], true );

			if( strstr( szVecSkinList[i], "Weapon" ) )
			{
				if( StrStrI( szVecSkinList[i], "Gauntlet" ) == 0 &&
					StrStrI( szVecSkinList[i], "Academicglove" ) == 0 &&
					StrStrI( szVecSkinList[i], "Charm" ) == 0 )
					((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_WEAPON );
			}
			else if( strstr( szVecSkinList[i], "Cash_Item" ) )
			{
				if( StrStrI( szVecSkinList[i], "EarRing" ) )
					((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_EARRING );
				else if( StrStrI( szVecSkinList[i], "Wing" ) )
					((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_WING );
				else if( StrStrI( szVecSkinList[i], "Tail" ) )
					((CObjectSkin*)pBase)->SetSkinType( CObjectSkin::SKIN_TAIL );
			}

			CGlobalValue::GetInstance().AddObject( m_SelectItemList.GetHead(), pBase );

			CGlobalValue::GetInstance().UpdateProgress( (int)(fPer*(i+1)) );
		}
		CGlobalValue::GetInstance().UpdateProgress( 0 );
	}
}

void CWorkspacePaneView::OnSkinRemove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( hti );
	}
}

void CWorkspacePaneView::OnSkinSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CObjectSkin *pBase = (CObjectSkin *)CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
		if( !pBase ) continue;
		if( pBase->IsChildModify() == false ) continue;
		pBase->SaveSkin( CString("") );
	}
}

void CWorkspacePaneView::OnSkinSaveAs()
{
	CString szFileList;
	TCHAR szFilter[] = _T( "Eternity Skin File (*.skn)|*.skn|Eternity Animation File (*.ani)|*.ani|Eternity Support File (*.skn;*.ani)|*.skn;*.ani|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("skn"), _T("*.skn"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST| OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	dlg.m_ofn.lpstrTitle = "Save As Skin File";

	if( IDOK != dlg.DoModal() ) {
		return;
	}
	
	HTREEITEM hti = m_SelectItemList.GetHead();
	CObjectSkin *pBase = (CObjectSkin *)CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
	if( !pBase ) return;
	pBase->SaveSkin( dlg.GetPathName() );

	// 기존거 빼버리구 다시 읽어준다.
	HTREEITEM hParent = pBase->GetParent()->GetTreeItemID();
	CGlobalValue::GetInstance().RemoveObjectFromTreeID( hti );

	char szTemp[512];
	_GetFullFileName( szTemp, _countof(szTemp), dlg.GetPathName() );
	pBase = new CObjectSkin;
	pBase->SetName( CString(szTemp) );
	((CObjectSkin*)pBase)->LoadSkin( dlg.GetPathName(), true );

	CGlobalValue::GetInstance().AddObject( hParent, pBase );

}

void CWorkspacePaneView::OnAnimationAdd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFileList;
	TCHAR szFilter[] = _T( "Eternity Animation File (*.ani)|*.ani|Eternity Skin File (*.skn)|*.skn|Eternity Support File (*.skn;*.ani)|*.skn;*.ani|All Files (*.*)|*.*||" );
	CFileDialog dlg( TRUE, _T("ani"), _T("*.ani"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT| OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );
	dlg.GetOFN().lpstrFile = szFileList.GetBuffer(100000);
	dlg.GetOFN().nMaxFile = 100000;

	dlg.m_ofn.lpstrTitle = "Open Eternity Engine File";

	if( IDOK != dlg.DoModal() ) {
		szFileList.ReleaseBuffer();
		return;
	}

	std::vector<CString> szVecAniList;
	CGlobalValue::GetInstance().ParseFileList( szFileList, "ani", szVecAniList );
	szFileList.ReleaseBuffer();

	if( szVecAniList.size() == 0 ) return;
	float fPer = 100 / (float)szVecAniList.size();
	CGlobalValue::GetInstance().UpdateProgress( 0 );
	for( DWORD i=0; i<szVecAniList.size(); i++ ) {
		char szTemp[512];
		_GetFullFileName( szTemp, _countof(szTemp), szVecAniList[i] );
		CObjectBase *pBase = new CObjectAnimation;

		if( ((CObjectAnimation*)pBase)->LoadAnimation( szVecAniList[i] ) == false ) {
			SAFE_DELETE( pBase );
		}
		else
			CGlobalValue::GetInstance().AddObject( m_SelectItemList.GetHead(), pBase );
		CGlobalValue::GetInstance().UpdateProgress( (int)(fPer * (i+1)) );
	}
	CGlobalValue::GetInstance().UpdateProgress( 0 );
}

void CWorkspacePaneView::OnAnimationRemove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( hti );
	}
}

void CWorkspacePaneView::OnAnimationMerge()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	std::vector<CString> szVecFileList;
	std::vector<HTREEITEM> hVecItemList;
	std::vector<CString> szVecParentList;
	std::vector<HTREEITEM> hVecParentItemList;
	while(p) {
		hti = m_SelectItemList.GetNext( p );

		CObjectAnimation *pBase = (CObjectAnimation*)CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
		if( pBase && pBase->GetParent() ) {
			bool bFlag = true;
			for( DWORD i=0; i<szVecParentList.size(); i++ ) {
				if( strcmp( szVecParentList[i], pBase->GetParent()->GetName() ) == NULL ) {
					bFlag = false;
					break;
				}
			}
			if( bFlag == true ) {
				szVecParentList.push_back( pBase->GetParent()->GetName() );
				hVecParentItemList.push_back( pBase->GetParent()->GetTreeItemID() );
			}
		}

		szVecFileList.push_back( pBase->GetFileName() );
		hVecItemList.push_back( hti );

	}

	if( szVecFileList.size() <= 1 ) return;

	TCHAR szFilter[] = _T( "Eternity Animation File (*.ani)|*.ani|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("ani"), _T("*.ani"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST| OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	dlg.m_ofn.lpstrTitle = "Save Merge Animation";

	if( IDOK != dlg.DoModal() ) {
		return;
	}

	CWnd *pWnd = GetPaneWnd( ANI_PANE );
	pWnd->SendMessage( UM_ANICONTROL_REFRESH );

	// 머지해서 저장
	CGlobalValue::GetInstance().UpdateProgress( 0 );
	float fPer = 100 / (float)szVecFileList.size();

	CEtResource::FlushWaitDelete();
	EtAniHandle MergeHandle, DummyHandle;
	MergeHandle = EternityEngine::LoadAni( szVecFileList[0] );
	for( DWORD i=1; i<szVecFileList.size(); i++ ) {
		DummyHandle = EternityEngine::LoadAni( szVecFileList[i] );
		if( !DummyHandle ) {
			assert(0);
			continue;
		}
		EternityEngine::MergeAni( MergeHandle, DummyHandle );
		SAFE_RELEASE_SPTR( DummyHandle );
		CGlobalValue::GetInstance().UpdateProgress( (int)(fPer * (i+1)) );
	}

	EternityEngine::SaveAni( MergeHandle, dlg.GetPathName() );
	SAFE_RELEASE_SPTR( MergeHandle );

	// 기존에 있던것들 전부 날리구 머지된것 로딩	
	fPer = 100 / (float)szVecFileList.size();
	for( DWORD i=0; i<hVecItemList.size(); i++ ) {
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( hVecItemList[i] );
		CGlobalValue::GetInstance().UpdateProgress( (int)(fPer * (i+1)) );
	}

	// 부모가 많았을 경우엔 머지시킨 것들을 전부 가치 넣어준다.
	fPer = 100 / (float)szVecParentList.size();
	for( DWORD i=0; i<szVecParentList.size(); i++ ) {
		char szTemp[512];
		_GetFullFileName( szTemp, _countof(szTemp), dlg.GetPathName() );
		CObjectBase *pBase = new CObjectAnimation;

		if( ((CObjectAnimation*)pBase)->LoadAnimation( dlg.GetPathName() ) == false ) {
			SAFE_DELETE( pBase );
		}
		else 
			CGlobalValue::GetInstance().AddObject( hVecParentItemList[i], pBase );

		CGlobalValue::GetInstance().UpdateProgress( (int)(fPer * (i+1)) );
	}
	CGlobalValue::GetInstance().UpdateProgress( 0 );
	CEtResource::FlushWaitDelete();
}

void CWorkspacePaneView::OnCommonRemove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( hti );
	}
}

void CWorkspacePaneView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RefreshSelectInfo();
	*pResult = 0;
}

LRESULT CWorkspacePaneView::OnCheckItem( WPARAM wParam, LPARAM lParam )
{
	HTREEITEM hti = (HTREEITEM)wParam;
	CObjectBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
	pBase->Show( lParam == 1 );
	return S_OK;
}

void CWorkspacePaneView::RefreshSelectInfo()
{
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CObjectBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
		if( pBase ) pBase->SetFocus( false );
	}
	CGlobalValue::GetInstance().SetControlObject( NULL );

	CalcSelectItemList();

	p = m_SelectItemList.GetHeadPosition();
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CObjectBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
		if( pBase ) pBase->SetFocus( true );
	}

	int nCount = (int)m_SelectItemList.GetCount();
	if( nCount == 0 ) {
	}
	else if( nCount > 1 ) {
		// 여러게 골라질 경우 Properties 가능한 경우 체크해서 Enable, Disable 해줘야 하는데.. 음.. 지금은 안뒤야~
		// Submesh - 시멘틱 같은 것들만 모아서 편집 가능하게.. 칼라값정도
		// Light - 같은 종류의 라이트들만 - 칼라값정도
		CWnd *pWnd = GetPaneWnd( PROP_SHADER_PANE );
		pWnd->SendMessage( UM_SHADERPROP_REFRESH );

		pWnd = GetPaneWnd( ANI_PANE );
		pWnd->SendMessage( UM_ANICONTROL_REFRESH );

		((CMainFrame*)AfxGetMainWnd())->SetFocus( PROP_SHADER_PANE );

	}
	else {
		HTREEITEM hti = m_SelectItemList.GetHead();
		CString szName = m_TreeCtrl.GetItemText( m_SelectItemList.GetHead() );
		CObjectBase *pBase = CGlobalValue::GetInstance().GetObjectFromTreeID( hti );
		CGlobalValue::GetInstance().SetControlObject( pBase );

		CWnd *pWnd;
		switch( pBase->GetType() ) {
			case CObjectBase::SKIN:
			case CObjectBase::SUBMESH:
			case CObjectBase::LIGHTFOLDER:
			case CObjectBase::LIGHTDIR:
			case CObjectBase::LIGHTPOINT:
			case CObjectBase::LIGHTSPOT:
			case CObjectBase::SCENE:
				pWnd = GetPaneWnd( PROP_SHADER_PANE );
				pWnd->SendMessage( UM_SHADERPROP_REFRESH, (WPARAM)pBase );

				pWnd = GetPaneWnd( ANI_PANE );
				pWnd->SendMessage( UM_ANICONTROL_REFRESH );

				((CMainFrame*)AfxGetMainWnd())->SetFocus( PROP_SHADER_PANE );
				break;
			case CObjectBase::ANIMATION:
				pWnd = GetPaneWnd( PROP_SHADER_PANE );
				pWnd->SendMessage( UM_SHADERPROP_REFRESH );

				pWnd = GetPaneWnd( ANI_PANE );
				pWnd->SendMessage( UM_ANICONTROL_REFRESH, (WPARAM)pBase );

				((CMainFrame*)AfxGetMainWnd())->SetFocus( ANI_PANE );
				break;
			default:
				pWnd = GetPaneWnd( PROP_SHADER_PANE );
				pWnd->SendMessage( UM_SHADERPROP_REFRESH );

				pWnd = GetPaneWnd( ANI_PANE );
				pWnd->SendMessage( UM_ANICONTROL_REFRESH );

				((CMainFrame*)AfxGetMainWnd())->SetFocus( PROP_SHADER_PANE );
				break;
		}
	}
}

LRESULT CWorkspacePaneView::OnGetTreeCtrl( WPARAM wParam, LPARAM lParam )
{
	CTreeCtrl *pCtrl = (CTreeCtrl *)wParam;
	pCtrl = &m_TreeCtrl;
	return S_OK;
}

void CWorkspacePaneView::OnLightAddDir()
{
	HTREEITEM hti = m_SelectItemList.GetHead();

	CObjectBase *pBase = new CObjectLightDir;
	CGlobalValue::GetInstance().AddObject( hti, pBase );
}

void CWorkspacePaneView::OnLightAddPoint()
{
	HTREEITEM hti = m_SelectItemList.GetHead();

	CObjectBase *pBase = new CObjectLightPoint;
	CGlobalValue::GetInstance().AddObject( hti, pBase );
}

void CWorkspacePaneView::OnLightAddSpot()
{
	HTREEITEM hti = m_SelectItemList.GetHead();

	CObjectBase *pBase = new CObjectLightSpot;
	CGlobalValue::GetInstance().AddObject( hti, pBase );
}

void CWorkspacePaneView::OnLightRemove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	POSITION p = m_SelectItemList.GetHeadPosition();
	HTREEITEM hti;
	while(p) {
		hti = m_SelectItemList.GetNext( p );
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( hti );
	}
}
