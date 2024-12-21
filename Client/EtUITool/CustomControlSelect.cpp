// CustomControlSelect.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "CustomControlSelect.h"
#include "RenderBase.h"

extern std::vector< std::string > g_vecCustomControlName;


// CCustomControlSelect dialog

IMPLEMENT_DYNAMIC(CCustomControlSelect, CDialog)

CCustomControlSelect::CCustomControlSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomControlSelect::IDD, pParent)
{
	m_nCurSel = -1;
}

CCustomControlSelect::~CCustomControlSelect()
{
}

void CCustomControlSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_COMBO, m_CustomSelect);
}


BEGIN_MESSAGE_MAP(CCustomControlSelect, CDialog)
	ON_BN_CLICKED(IDOK, &CCustomControlSelect::OnBnClickedOk)
END_MESSAGE_MAP()


// CCustomControlSelect message handlers

BOOL CCustomControlSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;

	for( i = 0; i < ( int )g_vecCustomControlName.size(); i++ )
	{
		m_CustomSelect.InsertString( i, g_vecCustomControlName[i].c_str() );
	}
	m_CustomSelect.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomControlSelect::OnBnClickedOk()
{
	m_nCurSel = m_CustomSelect.GetCurSel();
	OnOK();
}
