// ScrollBarSelect.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "ScrollBarSelect.h"

#include "UIToolTemplate.h"

extern std::vector< CUIToolTemplate * > g_vecScrollBar;

// CScrollBarSelect dialog

IMPLEMENT_DYNAMIC(CScrollBarSelect, CDialog)

CScrollBarSelect::CScrollBarSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CScrollBarSelect::IDD, pParent)
{

}

CScrollBarSelect::~CScrollBarSelect()
{
}

void CScrollBarSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ScrollBarName);
}


BEGIN_MESSAGE_MAP(CScrollBarSelect, CDialog)
	ON_BN_CLICKED(IDOK, &CScrollBarSelect::OnBnClickedOk)
END_MESSAGE_MAP()


// CScrollBarSelect message handlers

BOOL CScrollBarSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ScrollBarName.InsertString( 0, "None" );

	for( int i = 0; i < ( int )g_vecScrollBar.size(); i++ )
	{
		m_ScrollBarName.InsertString( i+1, g_vecScrollBar[i]->m_Template.m_szTemplateName.c_str() );
	}

	m_ScrollBarName.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScrollBarSelect::OnBnClickedOk()
{
	m_nSelectIndex = m_ScrollBarName.GetCurSel()-1;
	OnOK();
}

