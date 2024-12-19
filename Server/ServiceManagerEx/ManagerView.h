#pragma once

#include "ServerViewType.h"
#include "ItemData.h"

struct AllocData
{
	wstring name;
	int id;
	ItemData::Type type;
};

class CServerList;
class CLauncherSession;
class CServiceSession;
class CMonitorSession;
class CPatcherSession;
class CSessionWatcher;

typedef map<int, ItemData*> MAP_ITEM_DATA;

class CManagerView : public CScrollView
{
private:
	DECLARE_DYNCREATE(CManagerView)

protected:
	CManagerView();
	virtual ~CManagerView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual void OnInitialUpdate();     // ������ �� ó���Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	void BuildView();
	void RefreshView();
	void SetViewType(ViewType type);
	void ExpandServerList(HTREEITEM hRoot, bool expand);
	void Update();
	
public:
	void OnPatchStart();
	void OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax);
	void OnPatchEnd(bool succeede);
	void OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax);
	void OnWorldMaxUser(int id, int maxUser);
	void OnPatchFail(int id, const wchar_t* msg);
	void OnPatchCompleted(int id);
	void OnCommandPatch();

private:
	void ExpandLeafItem(HTREEITEM hRoot, UINT expand);
	void BuildLogicalView(const ItemData* pRootItem, CImageList* pImageList);
	void BuildPhysicalView(const ItemData* pRootItem, CImageList* pImageList);
	void RefreshLogicalView();
	void RefreshPhysicalView();
	void GetWorldIDString(ItemData* pItemData, const sServiceState* pState);

private:
	ItemData* AllocItemData(const AllocData& data);
	ItemData* AllocItemData(const wstring& name, int id, ItemData::Type type);
	ItemData* GetWorldItem(int id);
	ItemData* GetLauncherItem(int id);
	ItemData* GetServerItem(int id);
	void ClearItemData();

private:
	CServerList* m_pServerList;
	ViewType m_ViewType;
	vector<ItemData*> m_ItemDatas;
	MAP_ITEM_DATA m_WorldItems;
	MAP_ITEM_DATA m_LauncherItems;
	MAP_ITEM_DATA m_ServerItems;
	CImageList* m_pImageList;
};