// LinkObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtViewer.h"
#include "LinkObjectDlg.h"
#include "ObjectBase.h"
#include "GlobalValue.h"

using namespace std;

// CLinkObjectDlg dialog

IMPLEMENT_DYNAMIC(CLinkObjectDlg, CXTPDialog)

CLinkObjectDlg::CLinkObjectDlg(CWnd* pParent /*=NULL*/)
	: CXTPDialog(CLinkObjectDlg::IDD, pParent)
{
	m_pSelectSkin = NULL;
	m_pTargetSkin = NULL;
}

CLinkObjectDlg::~CLinkObjectDlg()
{
}

void CLinkObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_SelectObject);
	DDX_Control(pDX, IDC_COMBO2, m_LinkTargetBone);
	DDX_Control(pDX, IDC_COMBO3, m_LinkTargetObject);
	DDX_Control(pDX, IDOK, m_OKButton);
}


BEGIN_MESSAGE_MAP(CLinkObjectDlg, CXTPDialog)
	ON_BN_CLICKED(IDOK, &CLinkObjectDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLinkObjectDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CLinkObjectDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CLinkObjectDlg::OnCbnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CLinkObjectDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CLinkObjectDlg message handlers

BOOL CLinkObjectDlg::OnInitDialog()
{
	CXTPDialog::OnInitDialog();

	m_SelectObject.AddString( "None" );
//	m_pVecSkinObjectList.push_back( NULL );

	m_LinkTargetObject.AddString( "None" );
	m_LinkTargetObject.SetCurSel(0);
	m_LinkTargetObject.EnableWindow( FALSE );

	m_LinkTargetBone.AddString( "None" );
	m_LinkTargetBone.SetCurSel(0);
	m_LinkTargetBone.EnableWindow( FALSE );

	CObjectBase *pRoot = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
	if( pRoot == NULL ) return FALSE;
	char szFileName[256];
	for( DWORD i=0; i<pRoot->GetChildCount(); i++ ) {
		CObjectSkin *pSkin = dynamic_cast<CObjectSkin *>(pRoot->GetChild(i));
		if( !pSkin ) continue;

		_GetFileName( szFileName, _countof(szFileName), pSkin->GetFileName() );
		m_SelectObject.AddString( szFileName );
		m_szMapSkinObject.insert( make_pair( szFileName, pSkin ) );
//		m_pVecSkinObjectList.push_back( pSkin );
		/*
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
			}
		}
		*/
	}
	m_SelectObject.SetCurSel(0);

	m_OKButton.EnableWindow( FALSE );

	return TRUE;
}

void CLinkObjectDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if( !m_pSelectSkin || !m_pTargetSkin || m_szTargetBone.IsEmpty() ) return;

	EtAniObjectHandle hHandle = m_pSelectSkin->GetObjectHandle();
	if( !hHandle ) return;
	EtAniObjectHandle hTargetHandle = m_pTargetSkin->GetObjectHandle();
	if( !hTargetHandle ) return;
	if( !hTargetHandle->GetAniHandle() ) return;

	int nBoneIndex = -1;
	if( strcmp( m_szTargetBone.GetBuffer(), "None" ) ) {
		nBoneIndex = hTargetHandle->GetAniHandle()->GetBoneIndex( m_szTargetBone.GetBuffer() );
	}
	
	hHandle->SetParent( hTargetHandle, nBoneIndex );

	CXTPDialog::OnOK();
}

void CLinkObjectDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CXTPDialog::OnCancel();
}

void CLinkObjectDlg::OnDestroy()
{
	CXTPDialog::OnDestroy();
}

void CLinkObjectDlg::OnCbnSelchangeCombo1()
{
	m_LinkTargetObject.ResetContent();
	m_LinkTargetObject.AddString( "None" );
	m_pSelectSkin = NULL;

	int nCurSel = m_SelectObject.GetCurSel();
	if( nCurSel == 0 ) {
		m_LinkTargetObject.EnableWindow( FALSE );
		m_LinkTargetObject.SetCurSel(0);
		m_LinkTargetBone.EnableWindow( FALSE );
		m_LinkTargetBone.SetCurSel(0);
		m_OKButton.EnableWindow( FALSE );
	}
	else {
		CString szSelectStr;
		m_SelectObject.GetLBText( nCurSel, szSelectStr );
		std::map<std::string, CObjectSkin *>::iterator it = m_szMapSkinObject.find( szSelectStr.GetBuffer() );
		if( it == m_szMapSkinObject.end() ) return;
		m_pSelectSkin = it->second;

		CObjectBase *pRoot = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
		char szFileName[256];
		for( DWORD i=0; i<pRoot->GetChildCount(); i++ ) {
			CObjectSkin *pSkin = dynamic_cast<CObjectSkin *>(pRoot->GetChild(i));
			if( !pSkin ) continue;
			if( m_pSelectSkin == pSkin ) continue; 

			_GetFileName( szFileName, _countof(szFileName), pSkin->GetFileName() );
			m_LinkTargetObject.AddString( szFileName );
		}
		m_LinkTargetObject.EnableWindow( TRUE );
		m_LinkTargetObject.SetCurSel(0);
		m_LinkTargetBone.EnableWindow( FALSE );
		m_LinkTargetBone.SetCurSel(0);
	}
}

void CLinkObjectDlg::OnCbnSelchangeCombo3()
{
	// TODO: Add your control notification handler code here
	m_LinkTargetBone.ResetContent();
	m_LinkTargetBone.AddString( "None" );

	int nCurSel = m_LinkTargetObject.GetCurSel();
	if( nCurSel == 0 ) {
		m_LinkTargetBone.EnableWindow( FALSE );
		m_LinkTargetBone.SetCurSel(0);
		m_OKButton.EnableWindow( FALSE );
	}
	else {
		CString szSelectStr;
		m_LinkTargetObject.GetLBText( nCurSel, szSelectStr );
		std::map<std::string, CObjectSkin *>::iterator it = m_szMapSkinObject.find( szSelectStr.GetBuffer() );
		if( it == m_szMapSkinObject.end() ) return;
		m_pTargetSkin = it->second;

		EtAniObjectHandle hHandle = m_pTargetSkin->GetObjectHandle();
		if( !hHandle ) return;
		EtAniHandle hAni = hHandle->GetAniHandle();
		if( !hAni ) return;
		if( hAni->GetBoneCount() < 1 ) return;
		for( int j=0; j<hAni->GetBoneCount(); j++ ) {
			CEtBone *pBone = hAni->GetBone(j);
			m_LinkTargetBone.AddString( pBone->GetName() );
		}
		m_LinkTargetBone.EnableWindow( TRUE );
		m_LinkTargetBone.SetCurSel(0);
		m_OKButton.EnableWindow( TRUE );
	}
}

void CLinkObjectDlg::OnCbnSelchangeCombo2()
{
	m_OKButton.EnableWindow( TRUE );

	int nCurSel = m_LinkTargetBone.GetCurSel();
	m_LinkTargetBone.GetLBText( nCurSel, m_szTargetBone );
}
