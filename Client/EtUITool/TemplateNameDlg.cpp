// TemplateNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "TemplateNameDlg.h"


// CTemplateNameDlg dialog

IMPLEMENT_DYNAMIC(CTemplateNameDlg, CDialog)

CTemplateNameDlg::CTemplateNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateNameDlg::IDD, pParent)
	, m_szTemplateName(_T(""))
{

}

CTemplateNameDlg::~CTemplateNameDlg()
{
}

void CTemplateNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_szTemplateName);
}


BEGIN_MESSAGE_MAP(CTemplateNameDlg, CDialog)
END_MESSAGE_MAP()


// CTemplateNameDlg message handlers

