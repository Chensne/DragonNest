#pragma once

class ItemData;
class CStateView;
class CPatchDlg;

class CServerList : public CTreeCtrl
{
	DECLARE_DYNAMIC(CServerList)

public:
	CServerList(CStateView* pStateView);
	virtual ~CServerList();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCommandStart();
	afx_msg void OnCommandStartLauncher();
	afx_msg void OnCommandStop();
	afx_msg void OnCommandStopDB();
	afx_msg void OnCommandPatch();
	afx_msg void OnCommandStopLauncher();
	afx_msg void OnCommandPatchLauncher();
	afx_msg void OnCommandWorldMaxUser();

public:
	HTREEITEM GetWorldItem(int id) const;
	HTREEITEM GetLauncherItem(int id) const;
	HTREEITEM GetServerItem(HTREEITEM hParent, int id) const;

public:
	void OnPatchStart();
	void OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax);
	void OnPatchEnd(bool succeeded);
	void OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax);
	void OnPatchFail(int id, const wchar_t* msg);
	void OnPatchCompleted(int id);

	void OnEachLauncherStart();
	void OnEachLauncherStop();
private:
	CStateView* m_pStateView;
	CPatchDlg* m_pPatchDlg;
	ItemData* m_pItemData;
};