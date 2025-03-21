// ServerMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "MainFrm.h"
#include "ManagerView.h"
#include "Log.h"
#include "ServerList.h"
#include "ServiceManager.h"

extern CServiceManager* g_pServiceManager;

IMPLEMENT_DYNCREATE(CManagerView, CScrollView)

CManagerView::CManagerView()
	: m_pServerList(NULL), m_ViewType(Logical), m_pImageList(NULL)
{
}

CManagerView::~CManagerView()
{
	for each (const ItemData* pData in m_ItemDatas)
	{
		delete pData;
	}
	SAFE_DELETE(m_pImageList);
	delete m_pServerList;
}

BEGIN_MESSAGE_MAP(CManagerView, CScrollView)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CManagerView 그리기입니다.

void CManagerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	RECT rect;
	GetClientRect(&rect);

	m_pServerList = new CServerList(((CMainFrame*)::AfxGetMainWnd())->GetStateSplit().GetStateView());
	m_pServerList->Create(WS_CHILD | WS_BORDER | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_TRACKSELECT, CRect(0, 0, rect.right, rect.bottom), this, IDC_TREE_SERVER_LIST);

	const ViewConfig* pViewConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetViewConfig(L"Manager");
	if (pViewConfig)
		m_pServerList->SetBkColor(pViewConfig->GetBgColor());
	else
		m_pServerList->SetBkColor(RGB(0, 0, 0));
}

void CManagerView::OnDraw(CDC* pDC)
{
}

void CManagerView::BuildView()
{
	if (!m_pServerList)
		return;

	// clear
	ClearItemData();

	// insert partition(==root)
	vector<const TPartitionInfo*> partitions;
	g_pServiceManager->GetPartitionList(partitions);

	AllocData alloc;
	if (partitions.empty())
	{
		alloc.name = L"Partition";
		alloc.id = 1;
	}
	else
	{
		// 파티션당 SM이므로 하위는 무시.
		const TPartitionInfo* pInfo = partitions[0];
		alloc.name = pInfo->szPartitionName;
		alloc.id = pInfo->nPID;
	}
	alloc.type = ItemData::Partition;

	ItemData* pRootItem = AllocItemData(alloc);
	pRootItem->state = ItemData::Deactivity;

	wchar_t text[256];
	::wsprintf(text, L"[%s] PID : %d", alloc.name.c_str(), alloc.id);
	m_pImageList = new CImageList();
	m_pImageList->Create(IDB_BITMAP_TREE, 16, 16, RGB(255, 255, 255));

	TVINSERTSTRUCT tvi;
	tvi.hParent = TVI_ROOT;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
	tvi.item.pszText = text;
	tvi.item.iImage = ICON_PARTITION_DEACTIVITY;
	tvi.item.iSelectedImage = ICON_PARTITION_DEACTIVITY;
	tvi.item.lParam = (LPARAM)pRootItem;
	ASSERT(m_pServerList);
	pRootItem->item = m_pServerList->InsertItem(&tvi);
#if defined (USE_FONT_BOLD)
	m_pServerList->SetItemState(pRootItem->item, TVIS_BOLD, TVIS_BOLD);
#endif //#if defined (USE_FONT_BOLD)
	m_pServerList->SetImageList(m_pImageList, TVSIL_NORMAL);
	m_pImageList->Detach();

	switch (m_ViewType)
	{
	case Logical:
		BuildLogicalView(pRootItem, m_pImageList);
		break;
		
	case Physical:
		BuildPhysicalView(pRootItem, m_pImageList);
		break;

	default:
		ASSERT(0);
		break;
	}
}

void CManagerView::BuildLogicalView(const ItemData* pRootItem, CImageList* pImageList)
{
	// insert world
	vector<const TServerInfo*> worlds;
	g_pServiceManager->GetWorldList(worlds);

	vector<HTREEITEM> expandList;
	wchar_t text[256];
	TVINSERTSTRUCT tvi;

	for each (const TServerInfo* pWorld in worlds)
	{
		::wsprintf(text, L"[%s] WID : %d MaxUser : %d", pWorld->wszWorldName, pWorld->cWorldID, pWorld->nWorldMaxUser);
		ItemData* pWorldItem = AllocItemData(wstring(pWorld->wszWorldName), pWorld->cWorldID, ItemData::World);
		pWorldItem->state = ItemData::Deactivity;

		tvi.hParent = pRootItem->item;
		tvi.hInsertAfter = TVI_LAST;
		tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
		tvi.item.pszText = text;
		tvi.item.iImage = ICON_WORLD_DEACTIVITY;
		tvi.item.iSelectedImage = ICON_WORLD_DEACTIVITY;
		tvi.item.lParam = (LPARAM)pWorldItem;
		pWorldItem->item = m_pServerList->InsertItem(&tvi);
#if defined (USE_FONT_BOLD)
		m_pServerList->SetItemState(pWorldItem->item, TVIS_BOLD, TVIS_BOLD);
#endif //#if defined (USE_FONT_BOLD)
	}

	// insert server
	vector<const sServiceState*> servers;
	g_pServiceManager->GetServiceServerList(servers);

	for each (const sServiceState* pServer in servers)
	{
		const TNetLauncher* pLauncher = g_pServiceManager->GetLauncherInfo(pServer->nManagedLauncherID);
		if (!pLauncher)
		{
			wchar_t msg[256];
			wsprintf(msg, L"Build view failed. Not found NetLauncher[%d]", pServer->nManagedLauncherID);
			AfxMessageBox(msg, MB_ICONERROR);
			continue;
		}

		::wsprintf(text, L"[%s] SID : %d IP : %s", pServer->szType, pServer->nSID, pLauncher->szIP);
		ItemData* pServerItem = AllocItemData(wstring(pServer->szType), pServer->nSID, ItemData::Server);

		HTREEITEM hWorld = m_pServerList->GetWorldItem(pServer->cWorldIDs[0]);
		if (hWorld)
		{
			tvi.hParent = hWorld;
		}
		else
		{
			//이 경우는 Login, Log등 특정 월드에 속하지 않는 서버.
			tvi.hParent = pRootItem->item;
		}

		HTREEITEM hSecondWorld = m_pServerList->GetWorldItem(pServer->cWorldIDs[1]);
		if (hSecondWorld)
			tvi.hParent = pRootItem->item;	// 월드 Game, DB 서버

		tvi.hInsertAfter = TVI_LAST;
		tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
		tvi.item.pszText = text;
		tvi.item.iImage = ICON_SERVER_OFF;
		tvi.item.iSelectedImage = ICON_SERVER_OFF;
		tvi.item.lParam = (LPARAM)pServerItem;
		pServerItem->item = m_pServerList->InsertItem(&tvi);
#if defined (USE_FONT_BOLD)
		m_pServerList->SetItemState(pServerItem->item, TVIS_BOLD, TVIS_BOLD);
#endif //#if defined (USE_FONT_BOLD)
	}

	// expand
	for each (HTREEITEM item in expandList)
	{
		m_pServerList->Expand(item, TVE_EXPAND);
	}
	m_pServerList->Expand(pRootItem->item, TVE_EXPAND);
}

void CManagerView::BuildPhysicalView(const ItemData* pRootItem, CImageList* pImageList)
{
	// insert netlauncher
	vector<const TNetLauncher*> launchers;
	g_pServiceManager->GetLauncherList(launchers);

	vector<HTREEITEM> expandList;
	wchar_t text[256];
	TVINSERTSTRUCT tvi;

	for each (const TNetLauncher* pLauncher in launchers)
	{
		::wsprintf(text, L"[%s] NID : %d", pLauncher->szIP, pLauncher->nID);
		ItemData* pLauncherItem = AllocItemData(wstring(pLauncher->szIP), pLauncher->nID, ItemData::Launcher);

		tvi.hParent = pRootItem->item;
		tvi.hInsertAfter = TVI_LAST;
		tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
		tvi.item.pszText = text;
		tvi.item.iImage = ICON_LAUNCHER_OFF;
		tvi.item.iSelectedImage = ICON_LAUNCHER_OFF;
		tvi.item.lParam = (LPARAM)pLauncherItem;
		pLauncherItem->item = m_pServerList->InsertItem(&tvi);
#if defined (USE_FONT_BOLD)
		m_pServerList->SetItemState(pLauncherItem->item, TVIS_BOLD, TVIS_BOLD);
#endif //#if defined (USE_FONT_BOLD)

		// insert server
		for each (const TServerExcuteData* pServer in pLauncher->ExcuteList)
		{
			::wsprintf(text, L"[%s] SID : %d IP : %s", pServer->wszType, pServer->nSID, pLauncher->szIP);
			ItemData* pServerItem = AllocItemData(wstring(pServer->wszType), pServer->nSID, ItemData::Server);

			tvi.hParent = pLauncherItem->item;
			tvi.hInsertAfter = TVI_LAST;
			tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
			tvi.item.pszText = text;
			tvi.item.iImage = ICON_SERVER_OFF;
			tvi.item.iSelectedImage = ICON_SERVER_OFF;
			tvi.item.lParam = (LPARAM)pServerItem;
			pServerItem->item = m_pServerList->InsertItem(&tvi);
#if defined (USE_FONT_BOLD)
			m_pServerList->SetItemState(pServerItem->item, TVIS_BOLD, TVIS_BOLD);
#endif //#if defined (USE_FONT_BOLD)
		}
	}

	// expand
	for each (HTREEITEM item in expandList)
	{
		m_pServerList->Expand(item, TVE_EXPAND);
	}
	m_pServerList->Expand(pRootItem->item, TVE_EXPAND);
}

void CManagerView::RefreshView()
{
	if (!m_pServerList)
		return;

	switch (m_ViewType)
	{
	case Logical:
		RefreshLogicalView();
		break;

	case Physical:
		RefreshPhysicalView();
		break;

	default:
		ASSERT(0);
		break;
	}
}

void CManagerView::GetWorldIDString(ItemData* pItemData, const sServiceState* pState)
{
	pItemData->world.clear();
	for (int i=0; i<WORLDCOUNTMAX; i++)
	{
		if (pState->cWorldIDs[i] > 0)
		{		
			if (!pItemData->world.empty())
				pItemData->world.append(L", ");

			wchar_t str[24] = {0,};
			::swprintf_s(str, L"%d", pState->cWorldIDs[i]);
			pItemData->world.append(str);	
		}
	}
}

void CManagerView::RefreshLogicalView()
{
	// server
	vector<const sServiceState*> servers;
	g_pServiceManager->GetServiceServerList(servers);

	for each (const sServiceState* pState in servers)
	{
		ItemData* pServerItem = GetServerItem(pState->nSID);
		if (!pServerItem)
		{
			g_Log.Log(LogType::_ERROR, L"[RefreshLogicalView] not found service server[%d]\n", pState->nSID);
			continue;
		}

		GetWorldIDString(pServerItem, pState);

		switch (pState->nState)
		{
		case _SERVICE_STATE_ONLINE:
			if (pServerItem->state != ItemData::Activity)
			{
				pServerItem->state = ItemData::Activity;
				m_pServerList->SetItemImage(pServerItem->item, ICON_SERVER_ACTIVITY, ICON_SERVER_ACTIVITY);
				ToWideString((CHAR*)pState->szVersion, pServerItem->version);
				pServerItem->final = pState->bFinalBuild;
			}
			break;

		case _SERVICE_STATE_OFFLINE:
			if (pServerItem->state != ItemData::Off)
			{
				pServerItem->state = ItemData::Off;
				m_pServerList->SetItemImage(pServerItem->item, ICON_SERVER_OFF, ICON_SERVER_OFF);
			}
			break;

		default:
			continue;
		}
	}

	// world
	for each (MAP_ITEM_DATA::value_type v in m_WorldItems)
	{
		bool activated = false;
		ItemData* pWorldItem = v.second;
		HTREEITEM hServer = m_pServerList->GetChildItem(pWorldItem->item);
		if (!hServer)
		{
			g_Log.Log(LogType::_ERROR, L"[RefreshLogicalView] not found world item[%d]\n", pWorldItem->id);
			continue;
		}

		while (hServer)
		{
			const ItemData* pServerItem = (ItemData*)m_pServerList->GetItemData(hServer);
			if (pServerItem->state != ItemData::Activity)
			{
				if (pWorldItem->state != ItemData::Deactivity)
				{
					pWorldItem->state = ItemData::Deactivity;
					m_pServerList->SetItemImage(pWorldItem->item, ICON_WORLD_DEACTIVITY, ICON_WORLD_DEACTIVITY);
				}

				activated = false;
				break;
			}

			activated = true;
			hServer = m_pServerList->GetNextSiblingItem(hServer);
		}

		// 깜빡임 때문에 이렇게 해야 하다니...
		if (activated)
		{
			if (pWorldItem->state != ItemData::Activity)
			{
				pWorldItem->state = ItemData::Activity;
				m_pServerList->SetItemImage(pWorldItem->item, ICON_WORLD_ACTIVITY, ICON_WORLD_ACTIVITY);
			}
		}
	}

	// partition
	bool activated = false;
	ItemData* pPartitionItem = (ItemData*)m_pServerList->GetItemData(m_pServerList->GetRootItem());
	HTREEITEM hWorld = m_pServerList->GetChildItem(m_pServerList->GetRootItem());
	while (hWorld)
	{
		const ItemData* pWorldItem = (ItemData*)m_pServerList->GetItemData(hWorld);
		if (pWorldItem->state != ItemData::Activity)
		{
			if (pPartitionItem->state != ItemData::Deactivity)
			{
				pPartitionItem->state = ItemData::Deactivity;
				m_pServerList->SetItemImage(pPartitionItem->item, ICON_PARTITION_DEACTIVITY, ICON_PARTITION_DEACTIVITY);
			}

			activated = false;
			break;
		}

		activated = true;
		hWorld = m_pServerList->GetNextSiblingItem(hWorld);
	}

	// 깜빡임 때문에 이렇게 해야 하다니...
	if (activated)
	{
		if (pPartitionItem->state != ItemData::Activity)
		{
			pPartitionItem->state = ItemData::Activity;
			m_pServerList->SetItemImage(pPartitionItem->item, ICON_PARTITION_ACTIVITY, ICON_PARTITION_ACTIVITY);
		}
	}
}

void CManagerView::RefreshPhysicalView()
{
	// server
	vector<const sServiceState*> servers;
	g_pServiceManager->GetServiceServerList(servers);

	for each (const sServiceState* pState in servers)
	{
		ItemData* pServerItem = GetServerItem(pState->nSID);
		if (!pServerItem)
		{
			g_Log.Log(LogType::_ERROR, L"[RefreshPhysicalView] not found service server[%d]\n", pState->nSID);
			continue;
		}

		GetWorldIDString(pServerItem, pState);

		switch (pState->nState)
		{
		case _SERVICE_STATE_ONLINE:
			if (pServerItem->state != ItemData::Activity)
			{
				pServerItem->state = ItemData::Activity;
				m_pServerList->SetItemImage(pServerItem->item, ICON_SERVER_ACTIVITY, ICON_SERVER_ACTIVITY);
				ToWideString((CHAR*)pState->szVersion, pServerItem->version);
				pServerItem->final = pState->bFinalBuild;
			}
			break;

		case _SERVICE_STATE_OFFLINE:
			if (pServerItem->state != ItemData::Off)
			{
				pServerItem->state = ItemData::Off;
				m_pServerList->SetItemImage(pServerItem->item, ICON_SERVER_OFF, ICON_SERVER_OFF);
			}
			break;

		default:
			continue;
		}
	}

	for each (MAP_ITEM_DATA::value_type v in m_LauncherItems)
	{
		ItemData* pLauncherItem = v.second;
		if (g_pServiceManager->IsLauncherConnected(pLauncherItem->id))
		{
			bool activated = false;
			HTREEITEM hServer = m_pServerList->GetChildItem(pLauncherItem->item);
			while (hServer)
			{
				const ItemData* pServerItem = (ItemData*)m_pServerList->GetItemData(hServer);
				if (pServerItem->state != ItemData::Activity)
				{
					if (pLauncherItem->state != ItemData::Deactivity)
					{
						pLauncherItem->state = ItemData::Deactivity;
						m_pServerList->SetItemImage(pLauncherItem->item, ICON_LAUNCHER_DEACTIVITY, ICON_LAUNCHER_DEACTIVITY);
					}

					activated = false;
					break;
				}

				activated = true;
				hServer = m_pServerList->GetNextSiblingItem(hServer);
			}

			// 깜빡임 때문에 이렇게 해야 하다니...
			if (activated)
			{
				if (pLauncherItem->state != ItemData::Activity)
				{
					pLauncherItem->state = ItemData::Activity;
					m_pServerList->SetItemImage(pLauncherItem->item, ICON_LAUNCHER_ACTIVITY, ICON_LAUNCHER_ACTIVITY);
				}
			}
		}
		else
		{
			if (pLauncherItem->state != ItemData::Off)
			{
				pLauncherItem->state = ItemData::Off;
				m_pServerList->SetItemImage(pLauncherItem->item, ICON_LAUNCHER_OFF, ICON_LAUNCHER_OFF);
			}
		}
	}

	// partition
	ItemData* pPartitionItem = (ItemData*)m_pServerList->GetItemData(m_pServerList->GetRootItem());
	HTREEITEM hWorld = m_pServerList->GetChildItem(m_pServerList->GetRootItem());
	bool activated = false;
	while (hWorld)
	{
		const ItemData* pWorldItem = (ItemData*)m_pServerList->GetItemData(hWorld);
		if (pWorldItem->state != ItemData::Activity)
		{
			if (pPartitionItem->state != ItemData::Deactivity)
			{
				pPartitionItem->state = ItemData::Deactivity;
				m_pServerList->SetItemImage(pPartitionItem->item, ICON_PARTITION_DEACTIVITY, ICON_PARTITION_DEACTIVITY);
			}

			activated = false;
			break;
		}

		activated = true;
		hWorld = m_pServerList->GetNextSiblingItem(hWorld);
	}

	// 깜빡임 때문에 이렇게 해야 하다니...
	if (activated)
	{
		if (pPartitionItem->state != ItemData::Activity)
		{
			pPartitionItem->state = ItemData::Activity;
			m_pServerList->SetItemImage(pPartitionItem->item, ICON_PARTITION_ACTIVITY, ICON_PARTITION_ACTIVITY);
		}
	}
}

void CManagerView::SetViewType(ViewType type)
{
	ASSERT(type == Logical || type == Physical);
	if (m_ViewType == type)
		return;

	m_ViewType = type;
	BuildView();
	RefreshView();
}

ItemData* CManagerView::AllocItemData(const AllocData& data)
{
	return AllocItemData(data.name, data.id, data.type);
}

ItemData* CManagerView::AllocItemData(const wstring& name, int id, ItemData::Type type)
{
	ItemData* pData = new ItemData(name, id, type, ItemData::Off);
	m_ItemDatas.push_back(pData);

	switch (type)
	{
	case ItemData::Partition:
		break;

	case ItemData::World:
		m_WorldItems.insert(make_pair(id, pData));
		break;

	case ItemData::Launcher:
		m_LauncherItems.insert(make_pair(id, pData));
		break;

	case ItemData::Server:
		m_ServerItems.insert(make_pair(id, pData));
		break;

	default:
		ASSERT(0);
		break;
	}

	return pData;
}

ItemData* CManagerView::GetWorldItem(int id)
{
	MAP_ITEM_DATA::iterator it = m_WorldItems.find(id);
	if (it == m_WorldItems.end())
		return NULL;

	return it->second;
}

ItemData* CManagerView::GetLauncherItem(int id)
{
	MAP_ITEM_DATA::iterator it = m_LauncherItems.find(id);
	if (it == m_LauncherItems.end())
		return NULL;

	return it->second;
}

ItemData* CManagerView::GetServerItem(int id)
{
	MAP_ITEM_DATA::iterator it = m_ServerItems.find(id);
	if (it == m_ServerItems.end())
		return NULL;

	return it->second;
}

void CManagerView::ClearItemData()
{
	for each (const ItemData* pData in m_ItemDatas)
	{
		delete pData;
	}
	m_ItemDatas.clear();
	m_WorldItems.clear();
	m_LauncherItems.clear();
	m_ServerItems.clear();
	m_pServerList->DeleteAllItems();
	SAFE_DELETE(m_pImageList);
}

void CManagerView::ExpandServerList(HTREEITEM hRoot, bool expand)
{
	if (!m_pServerList || !hRoot)
	{
		ASSERT(0);
		return;
	}

	UINT cmd = (expand) ? TVE_EXPAND : TVE_COLLAPSE;
	ExpandLeafItem(hRoot, cmd);
}

void CManagerView::ExpandLeafItem(HTREEITEM hRoot, UINT cmd)
{
	if (!hRoot)
	{
		ASSERT(0);
		return;
	}

	m_pServerList->Expand(hRoot, cmd);
	HTREEITEM hLeaf = m_pServerList->GetChildItem(hRoot);
	while (hLeaf)
	{
		ExpandLeafItem(hLeaf, cmd);
		hLeaf = m_pServerList->GetNextSiblingItem(hLeaf);
	}
}

void CManagerView::Update()
{
	RefreshView();
}

// CManagerView 진단입니다.

#ifdef _DEBUG
void CManagerView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CManagerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG

// CManagerView 메시지 처리기입니다.

void CManagerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pServerList)
		m_pServerList->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE);
}

void CManagerView::OnPatchStart()
{
	if (!m_pServerList)
	{
		ASSERT(0);
		return;
	}
	
	m_pServerList->OnPatchStart();
}

void CManagerView::OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax)
{
	if (!m_pServerList)
	{
		ASSERT(0);
		return;
	}
	
	m_pServerList->OnPatchProgress(id, key, progress, progressMax);
}

void CManagerView::OnPatchEnd(bool succeeded)
{
	if (!m_pServerList)
	{
		ASSERT(0);
		return;
	}
	
	m_pServerList->OnPatchEnd(succeeded);
}

void CManagerView::OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax)
{
	if (!m_pServerList)
	{
		ASSERT(0);
		return;
	}

	m_pServerList->OnUnzipProgress(id, filename, progress, progressMax);
}

void CManagerView::OnWorldMaxUser(int id, int maxUser)
{
	if (!m_pServerList)
	{
		ASSERT(0);
		return;
	}

	MAP_ITEM_DATA::iterator it = m_WorldItems.find(id);
	if (it == m_WorldItems.end())
		return;

	const TServerInfo* pWorldInfo = g_pServiceManager->GetWorldInfo(id);

	wchar_t text[256];
	::wsprintf(text, L"[%s] WID : %d MaxUser : %d", pWorldInfo->wszWorldName, id, maxUser);
	m_pServerList->SetItemText(it->second->item, text);
}

void CManagerView::OnPatchFail(int id, const wchar_t* msg)
{
	if (!m_pServerList)
	{
		ASSERT(0);
		return;
	}

	m_pServerList->OnPatchFail(id, msg);
}

void CManagerView::OnPatchCompleted(int id)
{
	if (!m_pServerList)
	{
		ASSERT(0);
		return;
	}

	m_pServerList->OnPatchCompleted(id);
}

void CManagerView::OnCommandPatch()
{
	m_pServerList->OnCommandPatch();
}