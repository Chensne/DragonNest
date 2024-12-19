// DialogList.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "DialogList.h"
#include "LayoutView.h"

extern std::vector<std::string> g_vecFileNameforCustomSave;

// CDialogList dialog

IMPLEMENT_DYNAMIC(CDialogList, CDialog)

CDialogList::CDialogList(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogList::IDD, pParent)
{

}

CDialogList::~CDialogList()
{
}

void CDialogList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}


BEGIN_MESSAGE_MAP(CDialogList, CDialog)
END_MESSAGE_MAP()


BOOL CDialogList::OnInitDialog()
{
	CDialog::OnInitDialog();

	for( int i = 0; i < (int)g_vecFileNameforCustomSave.size(); ++i )
		m_ListBox.AddString(g_vecFileNameforCustomSave[i].c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CDialogList message handlers
void CDialogList::AddDialog( const char *szDialogName )
{
	
}