// PatchDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "PreparePatchDlg.h"
#include "ServiceManager.h"

extern CServiceManager* g_pServiceManager;
extern TServiceManagerConfig g_Config;

IMPLEMENT_DYNAMIC(CPreparePatchDlg, CDialog)

CPreparePatchDlg::CPreparePatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreparePatchDlg::IDD, pParent)
{
}

CPreparePatchDlg::~CPreparePatchDlg()
{
}

BEGIN_MESSAGE_MAP(CPreparePatchDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPreparePatchDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST_PREPARE_PATCH_LIST, &CPreparePatchDlg::OnLbnDblclkListPatchList)
END_MESSAGE_MAP()

BOOL CPreparePatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	InitPatchList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CPreparePatchDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (!PatchURL())
		return;

	OnOK();
}

void CPreparePatchDlg::OnLbnDblclkListPatchList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (!PatchURL())
		return;

	OnOK();
}

void CPreparePatchDlg::InitPatchList()
{
	WIN32_FIND_DATA wfd;
	HANDLE handle;

	wchar_t dir[MAX_PATH];
	::wsprintf(dir, L"%s\\*.*", g_Config.wszPatchDir);

	handle = ::FindFirstFile(dir, &wfd);
	if (handle == INVALID_HANDLE_VALUE)
	{
		wchar_t msg[256];
		::wsprintf(msg, L"%s is not found.", g_Config.wszPatchDir);
		AfxMessageBox(msg, MB_ICONERROR);
		OnCancel();
		return;
	}

	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_PREPARE_PATCH_LIST);
	ASSERT(pList);

	do
	{
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (lstrlen(wfd.cFileName) > 2)
			{
				pList->AddString(wfd.cFileName);
			}
		}
	}
	while (::FindNextFile(handle, &wfd));

	::FindClose(handle);
}

bool CPreparePatchDlg::PatchURL()
{
	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_PREPARE_PATCH_LIST);
	ASSERT(pList);
	int sel = pList->GetCurSel();
	if (sel < 0)
	{
		AfxMessageBox(L"Please select patch folder", MB_ICONASTERISK);
		return false;
	}

	CString src;
	pList->GetText(sel, src);

	char url[256];
	::WideCharToMultiByte(CP_ACP, 0, src.GetBuffer(), src.GetLength() + 1, url, 256, NULL, NULL);

	char cmd[256];
	::sprintf_s(cmd, "patchurl|%s", url);

	try
	{
		g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		::AfxMessageBox(L"PatchURL failed.");
		return false;
	}

	return true;
}
