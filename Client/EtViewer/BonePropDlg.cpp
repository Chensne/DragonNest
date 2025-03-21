// BonePropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtViewer.h"
#include "BonePropDlg.h"
#include "ObjectBase.h"
#include "GlobalValue.h"


// CBonePropDlg dialog

IMPLEMENT_DYNAMIC(CBonePropDlg, CXTResizeDialog)

CBonePropDlg::CBonePropDlg(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CBonePropDlg::IDD, pParent)
{

}

CBonePropDlg::~CBonePropDlg()
{
}

void CBonePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
}


BEGIN_MESSAGE_MAP(CBonePropDlg, CXTResizeDialog)
	ON_BN_CLICKED(IDOK, &CBonePropDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBonePropDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBonePropDlg message handlers

BOOL CBonePropDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize( IDC_TREE1, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT );
	SetResize( IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );

	LoadPlacement(_T("CBonePropDlg"));

	m_Tree.EnableMultiSelect();

	CObjectBase *pRoot = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
	for( DWORD i=0; i<pRoot->GetChildCount(); i++ ) {
		CObjectSkin *pSkin = dynamic_cast<CObjectSkin *>(pRoot->GetChild(i));
		if( !pSkin ) continue;

		HTREEITEM hRoot;
		hRoot = m_Tree.InsertItem( pSkin->GetName() );

		EtAniObjectHandle hHandle = pSkin->GetObjectHandle();
		if( !hHandle ) continue;
		EtAniHandle hAni = hHandle->GetAniHandle();
		if( !hAni ) continue;
		if( hAni->GetBoneCount() < 1 ) continue;

		for( int j=0; j<hAni->GetBoneCount(); j++ ) {
			CEtBone *pBone = hAni->GetBone(j);
			if( pBone->IsRootBone() ) {
				InsertTree( hRoot, pBone );
			}
		}
	}

	return TRUE;
}

void CBonePropDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}

void CBonePropDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CXTResizeDialog::OnCancel();
}

void CBonePropDlg::InsertTree( HTREEITEM hParent, CEtBone *pBone )
{
	HTREEITEM hTree = m_Tree.InsertItem( pBone->GetName(), hParent );
	m_Tree.Expand( hParent, TVE_EXPAND );
	for( int i=0; i<pBone->GetChildBoneCount(); i++ ) {
		InsertTree( hTree, pBone->GetChild(i) );
	}
}

void CBonePropDlg::OnDestroy()
{
	CXTResizeDialog::OnDestroy();

	// TODO: Add your message handler code here
	SavePlacement(_T("CBonePropDlg"));
}
