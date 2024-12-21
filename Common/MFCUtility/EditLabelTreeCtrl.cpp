// EditLabelTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EditLabelTreeCtrl.h"


// CEditLabelTreeCtrl

IMPLEMENT_DYNAMIC(CEditLabelTreeCtrl, CXTTreeCtrl)

CEditLabelTreeCtrl::CEditLabelTreeCtrl()
{

}

CEditLabelTreeCtrl::~CEditLabelTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditLabelTreeCtrl, CXTTreeCtrl)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CEditLabelTreeCtrl message handlers



void CEditLabelTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch( nChar ) {
		case VK_F2:
			{
				CTypedPtrList<CPtrList, HTREEITEM> SelectItemList;
				GetSelectedList( SelectItemList );

				if( SelectItemList.GetCount() == 1 ) {
					HTREEITEM hti = SelectItemList.GetHead();
					EditLabel( hti );
				}
			}
			break;
	}

	CXTTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
