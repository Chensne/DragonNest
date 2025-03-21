// MonsterInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MonsterInfoDlg.h"
#include "EtWorldEventArea.h"
#include "DNTableFile.h"
#include "TEtWorldEventArea.h"
#include "EtUIXML.h"
#include "StringUtil.h"


// CMonsterInfoDlg dialog

extern DNTableFileFormat *s_pSox[3];

IMPLEMENT_DYNAMIC(CMonsterInfoDlg, CDialog)

CMonsterInfoDlg::CMonsterInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMonsterInfoDlg::IDD, pParent)
	, m_nLevel(0)
{
	m_pEventArea = NULL;
}

CMonsterInfoDlg::~CMonsterInfoDlg()
{
}

void CMonsterInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szName );
	DDX_Text(pDX, IDC_EDIT5, m_nLevel );
}


BEGIN_MESSAGE_MAP(CMonsterInfoDlg, CDialog)
END_MESSAGE_MAP()


// CMonsterInfoDlg message handlers

BOOL CMonsterInfoDlg::OnInitDialog()
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

	int nNameID = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_NameID" )->GetInteger();
	std::string szName;
	ToMultiString( std::wstring(CEtUIXML::GetInstance().GetUIString( CEtUIXML::idCategory1, nNameID )), szName );
	m_szName = szName.c_str();

	m_nLevel = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_Level" )->GetInteger();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
