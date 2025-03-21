#pragma once

#include "ServerViewType.h"

class CManagerView;
class CLauncherSession;
class CServiceSession;
class CMonitorSession;
class CPatcherSession;
class CSessionWatcher;

class CMainSplit : public CSplitterWnd
{
	DECLARE_DYNAMIC(CMainSplit)

public:
	CMainSplit();
	virtual ~CMainSplit();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void CreateSplit(CWnd* pParent, int paneId, const RECT& rect, CCreateContext* pContext);
	void ShowWnd(bool show);
	void BuildView();
	void RefreshView();
	void SetManagerViewType(ViewType type);
	void ResizeView(const RECT& rect);
	void Update();

public:
	void OnPatchStart();
	void OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax);
	void OnPatchEnd(bool succeeded);

	void OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax);

public:
	void OnWorldMaxUser(int id, int maxUser);
	void OnPatchFail(int id, const wchar_t* msg);
	void OnPatchCompleted(int id);
	void OnCommandPatch();

private:
	CManagerView* m_pManagerView;
};


