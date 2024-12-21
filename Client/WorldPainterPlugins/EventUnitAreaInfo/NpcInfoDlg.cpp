// NpcInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NpcInfoDlg.h"
#include "EtWorldEventArea.h"
#include "DNTableFile.h"
#include "TEtWorldEventArea.h"
#include "EtUIXML.h"
#include "StringUtil.h"

// CNpcInfoDlg dialog

extern DNTableFileFormat *s_pSox[3];

IMPLEMENT_DYNAMIC(CNpcInfoDlg, CDialog)

CNpcInfoDlg::CNpcInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcInfoDlg::IDD, pParent)
{
	m_pEventArea = NULL;
}

CNpcInfoDlg::~CNpcInfoDlg()
{
}

void CNpcInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szName );
}


BEGIN_MESSAGE_MAP(CNpcInfoDlg, CDialog)
END_MESSAGE_MAP()


// CNpcInfoDlg message handlers

BOOL CNpcInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if( m_pEventArea == NULL ) return FALSE;

	CString szAreaName = m_pEventArea->GetName();
	char szHeadStr[256] = { 0, };
	int nMonsterID = -1;
	sscanf_s( szAreaName.GetBuffer(), "%s %d", szHeadStr, 256, &nMonsterID );
	_strlwr_s( szHeadStr );
	if( nMonsterID == -1 ) return FALSE;

	int nNameID = s_pSox[2]->GetFieldFromLablePtr( nMonsterID, "_NameID" )->GetInteger();
	std::string szName;
	ToMultiString( std::wstring(CEtUIXML::GetInstance().GetUIString( CEtUIXML::idCategory1, nNameID )), szName );
	m_szName = szName.c_str();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
