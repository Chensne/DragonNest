// TreeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "TreeCtrlEx.h"
#include "resource.h"
#include "UserMessage.h"


// CTreeCtrlEx

IMPLEMENT_DYNAMIC(CTreeCtrlEx, CXTTreeCtrl)
CTreeCtrlEx::CTreeCtrlEx()
{
	m_imageState.Create( IDB_CHECKIMAGE, 16, 1, RGB( 255, 255, 255 ) );
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CXTTreeCtrl)
	ON_WM_DRAWITEM()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CTreeCtrlEx message handlers
void CTreeCtrlEx::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CXTTreeCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

int CTreeCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CTreeCtrlEx::Activate()
{
	SetImageList( &m_imageState, TVSIL_STATE );
}

void CTreeCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_SPACE )
	{
		HTREEITEM hti = GetSelectedItem();
		int iImage = GetItemState( hti, TVIS_STATEIMAGEMASK )>>12;
		if( iImage == 0 ) return;
		SetItemState( hti, INDEXTOSTATEIMAGEMASK(iImage == 1 ? 2 : 1),
			TVIS_STATEIMAGEMASK );
		BOOL bCheck = ( iImage == 1 ) ? TRUE : FALSE;

		GetParent()->SendMessage( UM_TREEEX_MODIFY_CHECKED, (WPARAM)hti, (LPARAM)bCheck );
		return;
	}


	CXTTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTreeCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	UINT uFlags=0;
	HTREEITEM hti = HitTest(point,&uFlags);

	if( uFlags & TVHT_ONITEMSTATEICON )
	{
		int iImage = GetItemState( hti, TVIS_STATEIMAGEMASK )>>12;
		if( iImage == 0 ) return;
		SetItemState( hti, INDEXTOSTATEIMAGEMASK(iImage == 1 ? 2 : 1), 
			TVIS_STATEIMAGEMASK );

		BOOL bCheck = ( iImage == 1 ) ? TRUE : FALSE;

		GetParent()->SendMessage( UM_TREEEX_MODIFY_CHECKED, (WPARAM)hti, (LPARAM)bCheck );
		return;
	}

	CXTTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CTreeCtrlEx::IsItemChecked(HTREEITEM hItem)
{
	return GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12 == 2;
}

