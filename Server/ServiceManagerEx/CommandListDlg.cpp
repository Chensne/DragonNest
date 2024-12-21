// CommandListDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "CommandListDlg.h"
#include "ServiceManager.h"

extern CServiceManager* g_pServiceManager;

IMPLEMENT_DYNAMIC(CCommandListDlg, CDialog)

CCommandListDlg::CCommandListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommandListDlg::IDD, pParent)
{

}

CCommandListDlg::~CCommandListDlg()
{
}

void CCommandListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCommandListDlg, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_COMMAND_LIST, &CCommandListDlg::OnLbnDblclkListCommandList)
	ON_BN_CLICKED(IDOK, &CCommandListDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCommandListDlg 메시지 처리기입니다.

BOOL CCommandListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	const map<const wstring, int>& commands = g_pServiceManager->GetCommandList();

	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_COMMAND_LIST);
	ASSERT(pList);
	for each (map<const wstring, int>::value_type v in commands)
	{
		pList->AddString(v.first.c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCommandListDlg::OnLbnDblclkListCommandList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetCommand();	
	OnOK();
}

void CCommandListDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetCommand();
	if (m_Command.empty())
	{
		::AfxMessageBox(L"Command not selected.", MB_ICONASTERISK);
		return;
	}

	OnOK();
}

void CCommandListDlg::SetCommand()
{
	m_Command.clear();

	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_COMMAND_LIST);
	ASSERT(pList);
	int sel = pList->GetCurSel();
	if (sel < 0)
		return;

	wchar_t command[256];
	pList->GetText(sel, command);
	m_Command = command;
}