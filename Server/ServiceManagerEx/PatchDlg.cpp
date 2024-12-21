// PatchDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "PatchDlg.h"
#include "ItemData.h"
#include "ServiceManager.h"

extern CServiceManager* g_pServiceManager;

IMPLEMENT_DYNAMIC(CPatchDlg, CDialog)

CPatchDlg::CPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchDlg::IDD, pParent)
{
}

CPatchDlg::~CPatchDlg()
{
	ClearLauncherData();
}

void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PATCH_LIST, m_Tree);
}

BEGIN_MESSAGE_MAP(CPatchDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPatchDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CPatchDlg::Open()
{
	m_LauncherList.clear();
	DoModal();
}

BOOL CPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ClearLauncherData();
	BuildView();
	RefreshView();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPatchDlg::BuildView()
{
	const ViewConfig* pViewConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetViewConfig(L"Manager");
	if (pViewConfig)
		m_Tree.SetBkColor(pViewConfig->GetBgColor());
	else
		m_Tree.SetBkColor(RGB(0, 0, 0));


	m_Tree.SetTextColor(RGB(255,255,255));

	// insert partition(==root)
	wchar_t text[256] = {0,};
	g_pServiceManager->GetPartitionText(text);

	TVINSERTSTRUCT tvi;
	tvi.hParent = TVI_ROOT;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE;
	tvi.item.pszText = text;
	HTREEITEM hRoot = m_Tree.InsertItem(&tvi);
#if defined (USE_FONT_BOLD)
	m_Tree.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
#endif //#if defined (USE_FONT_BOLD)

	// insert netlauncher
	vector<const TNetLauncher*> launchers;
	g_pServiceManager->GetLauncherList(launchers);
	for each (const TNetLauncher* pLauncher in launchers)
	{
		::wsprintf(text, L"[%s] NID : %d", pLauncher->szIP, pLauncher->nID);

		tvi.hParent = hRoot;
		tvi.hInsertAfter = TVI_LAST;
		tvi.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE;
		tvi.item.pszText = text;
		HTREEITEM hLauncher = m_Tree.InsertItem(&tvi);
		ItemData* pItemData = new ItemData(text, 0, ItemData::Launcher, ItemData::Off);
		pItemData->item = hLauncher;
		m_LauncherList.insert(make_pair(pLauncher->nID, pItemData));
#if defined (USE_FONT_BOLD)
		m_Tree.SetItemState(hLauncher, TVIS_BOLD, TVIS_BOLD);
#endif //#if defined (USE_FONT_BOLD)
	}

	//expand
	m_Tree.Expand(hRoot, TVE_EXPAND);
}

void CPatchDlg::RefreshView()
{
}

void CPatchDlg::Patch(PatchTarget target)
{
	if (IDYES != ::AfxMessageBox(L"Are you sure to patch?", MB_YESNO))
		return;

	SetDlgItemText(IDC_STATIC_PATCH_RESULT, L"");
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	char src[32];
	if (target == Exe)
		::sprintf_s(src, "%s", "exe");
	else
		::sprintf_s(src, "%s", "full");
	
	char cmd[256];
	::sprintf_s(cmd, "patch%s", src);

	try
	{
		g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		::AfxMessageBox(L"Patch failed.");
		OnCancel();
		return;
	}
}

void CPatchDlg::ClearLauncherData()
{
	for each (map<int, ItemData*>::value_type v in m_LauncherList)
	{
		delete v.second;
	}
	m_LauncherList.clear();
}

void CPatchDlg::OnBnClickedOk()
{
	PatchTarget target = Full;

	int checkExe = ((CButton*)GetDlgItem(IDC_CHECK_PATCH_EXE))->GetCheck();
	if (checkExe == BST_CHECKED)
		target = Exe;

	Patch(target);
}

void CPatchDlg::OnPatchStart()
{
}

void CPatchDlg::OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax)
{
	map<int, ItemData*>::iterator it = m_LauncherList.find(id);
	if (it == m_LauncherList.end())
		return;

	const ItemData* pItemData = it->second;

	if( pItemData->bFail )
		return;

	wchar_t text[512];
	if (progressMax < 1024)
		::wsprintf(text, L"%s [key : %s] [recv : %d / %d bytes]", pItemData->name.c_str(), key, progress, progressMax);
	else
		::wsprintf(text, L"%s [key : %s] [recv : %d / %d Kbytes]", pItemData->name.c_str(), key, progress / 1024, progressMax / 1024);

	m_Tree.SetItemText(pItemData->item, text);
}

void CPatchDlg::OnPatchEnd(bool succeeded)
{
	if (m_LauncherList.empty())
		return;

	if (succeeded)
	{
		SetDlgItemText(IDC_STATIC_PATCH_RESULT, L"Patch completed.");
		GetDlgItem(IDCANCEL)->SetWindowText(L"Close");
	}
	else
	{
		SetDlgItemText(IDC_STATIC_PATCH_RESULT, L"Patch failed.");
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CPatchDlg::OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax)
{
	map<int, ItemData*>::iterator it = m_LauncherList.find(id);
	if (it == m_LauncherList.end())
		return;
	
	const ItemData* pItemData = it->second;

	if( pItemData->bFail )
		return;

	wchar_t text[512];
	::wsprintf(text, L"%s [count : %d / %d] [%s] ", pItemData->name.c_str(), progress, progressMax, filename);	
	m_Tree.SetItemText(pItemData->item, text);
}

void CPatchDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ClearLauncherData();

	CDialog::OnCancel();
}

void CPatchDlg::OnPatchFail(int id, const wchar_t* msg)
{
	map<int, ItemData*>::iterator it = m_LauncherList.find(id);
	if (it == m_LauncherList.end())
		return;

	ItemData* pItemData = it->second;

	wchar_t text[512];
	::wsprintf(text, L"%s %s", pItemData->name.c_str(), msg);
	pItemData->bFail = true;
	m_Tree.SetItemColor(pItemData->item, RGB(255, 0, 0));	
	m_Tree.SetItemBold(pItemData->item, TRUE);
	m_Tree.SetItemText(pItemData->item, text);
}

void CPatchDlg::OnPatchCompleted(int id)
{
	map<int, ItemData*>::iterator it = m_LauncherList.find(id);
	if (it == m_LauncherList.end())
		return;

	ItemData* pItemData = it->second;

	if( pItemData->bFail )
		return;

	wchar_t text[512];
	::wsprintf(text, L"%s Patch Complete", pItemData->name.c_str());	
	m_Tree.SetItemColor(pItemData->item,RGB(0,250,154));
	m_Tree.SetItemBold(pItemData->item, TRUE);
	m_Tree.SetItemText(pItemData->item, text);
}
