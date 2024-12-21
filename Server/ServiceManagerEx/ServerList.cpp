// ServerList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "ServerList.h"
#include "StateView.h"
#include "ItemData.h"
#include "ServiceManager.h"
#include "SMConfig.h"
#include "PatchDlg.h"
#include "WorldMaxUserDlg.h"

extern CServiceManager* g_pServiceManager;

IMPLEMENT_DYNAMIC(CServerList, CTreeCtrl)

CServerList::CServerList(CStateView* pStateView)
	: m_pStateView(pStateView), m_pItemData(NULL)
{
	ASSERT(m_pStateView);
	m_pPatchDlg = new CPatchDlg(this);
}

CServerList::~CServerList()
{
	delete m_pPatchDlg;
}

BEGIN_MESSAGE_MAP(CServerList, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CServerList::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CServerList::OnNMRclick)
	ON_COMMAND(ID_COMMAND_START, &CServerList::OnCommandStart)
	ON_COMMAND(ID_COMMAND_START_LAUNCHER, &CServerList::OnCommandStartLauncher)
	ON_COMMAND(ID_COMMAND_STOP, &CServerList::OnCommandStop)
	ON_COMMAND(ID_COMMAND_STOP_DB, &CServerList::OnCommandStopDB)
	ON_COMMAND(ID_COMMAND_PATCH, &CServerList::OnCommandPatch)
	ON_COMMAND(ID_COMMAND_STOP_LAUNCHER, &CServerList::OnCommandStopLauncher)
	ON_COMMAND(ID_COMMAND_PATCH_LAUNCHER, &CServerList::OnCommandPatchLauncher)
	ON_COMMAND(ID_COMMAND_WORLD_MAX_USER, &CServerList::OnCommandWorldMaxUser)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CServerList::OnNMCustomdraw)
END_MESSAGE_MAP()

void CServerList::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ItemData* pData = (ItemData*)pNMTreeView->itemNew.lParam;
	m_pStateView->OnSelectedServerItem(pData);

	*pResult = 0;
}

void CServerList::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pItemData = NULL;

	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);

	HTREEITEM hItem = HitTest(point);
	if (hItem == NULL)
		return;

	CRect rect;
	GetItemRect(hItem, &rect, TRUE);
	if (!rect.PtInRect(point))
		return;

	SelectItem(hItem);
	m_pItemData = (ItemData*)GetItemData(hItem);

	::GetCursorPos(&point);
	CMenu menu;
	menu.LoadMenu(IDR_MENU_SERVER_COMMAND);
	CMenu* pPopup = menu.GetSubMenu(0);

	switch (m_pItemData->type)
	{
	case ItemData::Partition:
		{
			pPopup->DeleteMenu(9, MF_BYPOSITION); // separator
			pPopup->DeleteMenu(ID_COMMAND_WORLD_MAX_USER, MF_BYCOMMAND);
		}
		break;

	case ItemData::World:
		{
			if (m_pItemData->state == ItemData::Off)
				pPopup->EnableMenuItem(ID_COMMAND_WORLD_MAX_USER, MF_DISABLED);

			pPopup->DeleteMenu(6, MF_BYPOSITION); // separator
			pPopup->DeleteMenu(ID_COMMAND_START_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_STOP_DB, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_STOP_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_PATCH, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_PATCH_LAUNCHER, MF_BYCOMMAND);
		}
		break;

	case ItemData::Launcher:
		{
			if (m_pItemData->state == ItemData::Off)
				pPopup->EnableMenuItem(ID_COMMAND_SERVER_PATCH, MF_DISABLED);
			
			pPopup->DeleteMenu(6, MF_BYPOSITION); // separator
			pPopup->DeleteMenu(8, MF_BYPOSITION); // separator
//			pPopup->DeleteMenu(ID_COMMAND_START_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_STOP_DB, MF_BYCOMMAND);
//			pPopup->DeleteMenu(ID_COMMAND_STOP_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_PATCH, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_PATCH_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_WORLD_MAX_USER, MF_BYCOMMAND);
		}
		break;

	case ItemData::Server:
		{
			pPopup->DeleteMenu(6, MF_BYPOSITION); // separator
			pPopup->DeleteMenu(8, MF_BYPOSITION); // separator
			pPopup->DeleteMenu(ID_COMMAND_START_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_STOP_DB, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_STOP_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_PATCH, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_PATCH_LAUNCHER, MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_COMMAND_WORLD_MAX_USER, MF_BYCOMMAND);

		}
		break;

	default:
		ASSERT(0);
		break;
	}

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	menu.DestroyMenu();

	*pResult = 0;
}

void CServerList::OnCommandStart()
{
	vector<string> commands;

	char cmd[256];
	wchar_t msg[256];

	ASSERT(m_pItemData);
	switch (m_pItemData->type)
	{
	case ItemData::Partition:
		{
			::sprintf_s(cmd, "start");
			commands.push_back(cmd);
			::wsprintf(msg, L"Are you sure to start partition?");
		}
		break;

	case ItemData::World:
		{
			HTREEITEM serverItem = GetChildItem(m_pItemData->item);
			while (serverItem)
			{
				const ItemData* pData = (ItemData*)GetItemData(serverItem);
				if (pData->type != ItemData::Server)
					continue;

				::sprintf_s(cmd, "starteach|%d", pData->id);
				commands.push_back(cmd);
				serverItem = GetNextSiblingItem(serverItem);
			}

			::wsprintf(msg, L"Are you sure to start [%d] %s world?", m_pItemData->id, m_pItemData->name.c_str());
		}
		break;

	case ItemData::Launcher:
		{
			::sprintf_s(cmd, "starteachbylauncher|%d", m_pItemData->id);
			commands.push_back(cmd);
			::wsprintf(msg, L"Are you sure to start [%d] %s launcher?", m_pItemData->id, m_pItemData->name.c_str());
		}
		break;

	case ItemData::Server:
		{
			::sprintf_s(cmd, "starteach|%d", m_pItemData->id);
			commands.push_back(cmd);
			::wsprintf(msg, L"Are you sure to start [%d] %s server?", m_pItemData->id, m_pItemData->name.c_str());
		}
		break;

	default:
		ASSERT(0);
		return;
	}

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	try
	{
		for each (string c in commands)
		{
			g_pServiceManager->ParseCommand(c.c_str());
		}
	}
	catch (...)
	{
		::AfxMessageBox(L"Start failed.");
		return;
	}
}
void CServerList::OnEachLauncherStart()
{
	vector<string> commands;

	char cmd[256];
	wchar_t msg[256];

	ASSERT(m_pItemData);
	::sprintf_s(cmd, "starteachlauncher|%d", m_pItemData->id);
	commands.push_back(cmd);
	::wsprintf(msg, L"Are you sure to start [%d] %s launcher?", m_pItemData->id, m_pItemData->name.c_str());

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	try
	{
		for each (string c in commands)
		{
			g_pServiceManager->ParseCommand(c.c_str());
		}
	}
	catch (...)
	{
		::AfxMessageBox(L"Launcher Start failed.");
		return;
	}
}

void CServerList::OnEachLauncherStop()
{
	vector<string> commands;

	char cmd[256];
	wchar_t msg[256];

	ASSERT(m_pItemData);
	::sprintf_s(cmd, "stopeachlauncher|%d", m_pItemData->id);
	commands.push_back(cmd);
	::wsprintf(msg, L"Are you sure to stop [%d] %s launcher?", m_pItemData->id, m_pItemData->name.c_str());

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	try
	{
		for each (string c in commands)
		{
			g_pServiceManager->ParseCommand(c.c_str());
		}
	}
	catch (...)
	{
		::AfxMessageBox(L"Launcher Stop failed.");
		return;
	}
}
void CServerList::OnCommandStartLauncher()
{
	ASSERT(m_pItemData);
	if (m_pItemData->type != ItemData::Partition)
	{
		if(m_pItemData->type == ItemData::Launcher)
		{
			OnEachLauncherStart();
			return;
		}
		ASSERT(0);
		return;
	}

	wchar_t msg[256];
	::wsprintf(msg, L"Are you sure to start all launcher?");

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	char cmd[256];
	::sprintf_s(cmd, "startlauncher");

	try
	{
		g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		::AfxMessageBox(L"Start launcher failed.");
		return;
	}
}

void CServerList::OnCommandStop()
{
	vector<string> commands;

	char cmd[256];
	wchar_t msg[256];

	ASSERT(m_pItemData);
	switch (m_pItemData->type)
	{
	case ItemData::Partition:
		{
			::sprintf_s(cmd, "stopall");
			commands.push_back(cmd);
			::wsprintf(msg, L"Are you sure to stop partition?");
		}
		break;

	case ItemData::World:
		{
			HTREEITEM serverItem = GetChildItem(m_pItemData->item);
			while (serverItem)
			{
				const ItemData* pData = (ItemData*)GetItemData(serverItem);
				if (pData->type != ItemData::Server)
					continue;

				::sprintf_s(cmd, "stopeach|%d", pData->id);
				commands.push_back(cmd);
				serverItem = GetNextSiblingItem(serverItem);
			}

			::wsprintf(msg, L"Are you sure to stop [%d] %s world?", m_pItemData->id, m_pItemData->name.c_str());
		}
		break;

	case ItemData::Launcher:
		{
			::sprintf_s(cmd, "stopeachbylauncher|%d", m_pItemData->id);
			commands.push_back(cmd);
			::wsprintf(msg, L"Are you sure to stop [%d] %s launcher?", m_pItemData->id, m_pItemData->name.c_str());
		}
		break;

	case ItemData::Server:
		{
			::sprintf_s(cmd, "stopeach|%d", m_pItemData->id);
			commands.push_back(cmd);
			::wsprintf(msg, L"Are you sure to stop [%d] server?", m_pItemData->id, m_pItemData->name.c_str());
		}
		break;

	default:
		ASSERT(0);
		return;
	}

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	try
	{
		for each (string c in commands)
		{
			g_pServiceManager->ParseCommand(c.c_str());
		}
	}
	catch (...)
	{
		::AfxMessageBox(L"Stop failed.");
		return;
	}
}

void CServerList::OnCommandStopDB()
{
	ASSERT(m_pItemData);
	if (m_pItemData->type != ItemData::Partition)
	{
		ASSERT(0);
		return;
	}

	wchar_t msg[256];
	::wsprintf(msg, L"Are you sure to stop DB?");

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	char cmd[256];
	::sprintf_s(cmd, "stopdb");

	try
	{
		g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		::AfxMessageBox(L"StopDB failed.");
		return;
	}
}

void CServerList::OnCommandPatch()
{
	if(!g_pServiceManager->IsRunLauncher())
	{
		AfxMessageBox(L"Launcher Not Found!");
		return;
	}
	m_pPatchDlg->Open();
}

void CServerList::OnCommandStopLauncher()
{
	ASSERT(m_pItemData);
	if (m_pItemData->type != ItemData::Partition)
	{
		if(m_pItemData->type == ItemData::Launcher)
		{
			OnEachLauncherStop();
			return;
		}
		ASSERT(0);
		return;
	}

	wchar_t msg[256];
	::wsprintf(msg, L"Are you sure to stop all launcher?");

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	char cmd[256];
	::sprintf_s(cmd, "stoplauncher");

	try
	{
		g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		::AfxMessageBox(L"Stop launcher failed.");
		return;
	}
}

void CServerList::OnCommandPatchLauncher()
{
	ASSERT(m_pItemData);
	if (m_pItemData->type != ItemData::Partition)
	{
		ASSERT(0);
		return;
	}

	wchar_t msg[256];
	::wsprintf(msg, L"Are you sure to patch launcher?");

	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	char cmd[256];
	::sprintf_s(cmd, "patchlauncher");

	try
	{
		g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		::AfxMessageBox(L"Patch launcher failed.");
		return;
	}
}

void CServerList::OnCommandWorldMaxUser()
{
	ASSERT(m_pItemData);
	if (m_pItemData->type != ItemData::World)
	{
		ASSERT(0);
		return;
	}

	CWorldMaxUserDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;

	UINT maxUser = dlg.GetMaxUser();
	int masterID = g_pServiceManager->GetMasterSIDByChannelID(m_pItemData->id, 0);

	char cmd[256];
	::sprintf_s(cmd, "worldmaxuser|%d|%d", masterID, maxUser);

	try
	{
		g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		::AfxMessageBox(L"World max user setting failed.");
		return;
	}
}

void CServerList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pTVCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	*pResult = 0;

	if (pTVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pTVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		const ViewConfig* pViewConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetViewConfig(L"Manager");
		if (pViewConfig)
		{
			pTVCD->clrText = pViewConfig->GetDefaultFontColor();
			pTVCD->clrTextBk = pViewConfig->GetBgColor();
		}
		else
		{
			pTVCD->clrText = RGB(192, 192, 192);
			pTVCD->clrTextBk = RGB(0, 0, 0);
		}

		*pResult = CDRF_DODEFAULT;
	}
}

HTREEITEM CServerList::GetWorldItem(int id) const
{
	HTREEITEM hWorld = GetChildItem(GetRootItem());
	while (hWorld)
	{
		const ItemData* pData = (ItemData*)GetItemData(hWorld);
		if (pData->type == ItemData::World && pData->id == id)
			return hWorld;

		hWorld = GetNextSiblingItem(hWorld);
	}

	return NULL;
}

HTREEITEM CServerList::GetLauncherItem(int id) const
{
	HTREEITEM hLauncher = GetChildItem(GetRootItem());
	while (hLauncher)
	{
		const ItemData* pData = (ItemData*)GetItemData(hLauncher);
		if (pData->type == ItemData::Launcher && pData->id == id)
			return hLauncher;

		hLauncher = GetNextSiblingItem(hLauncher);
	}

	return NULL;
}

HTREEITEM CServerList::GetServerItem(HTREEITEM hParent, int id) const
{
	if (!hParent)
	{
		ASSERT(0);
		return NULL;
	}

	HTREEITEM hServer = GetChildItem(hParent);
	while (hServer)
	{
		const ItemData* pData = (ItemData*)GetItemData(hServer);
		if (pData->type == ItemData::Server && pData->id == id)
			return hServer;

		hServer = GetNextSiblingItem(hServer);
	}

	return NULL;
}

void CServerList::OnPatchStart()
{
	m_pPatchDlg->OnPatchStart();
}

void CServerList::OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax)
{
	m_pPatchDlg->OnPatchProgress(id, key, progress, progressMax);
}

void CServerList::OnPatchEnd(bool succeeded)
{
	m_pPatchDlg->OnPatchEnd(succeeded);
}

void CServerList::OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax)
{
	m_pPatchDlg->OnUnzipProgress(id, filename, progress, progressMax);
}

void CServerList::OnPatchFail(int id, const wchar_t* msg)
{
	m_pPatchDlg->OnPatchFail(id, msg);
}

void CServerList::OnPatchCompleted(int id)
{
	m_pPatchDlg->OnPatchCompleted(id);
}