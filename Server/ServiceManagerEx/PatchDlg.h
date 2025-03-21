#pragma once

//#include "patchserverlist.h"
#include "PatchInfo.h"
#include "TreeCtrlEx.h"

class ItemData;

class CPatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CPatchDlg)

public:
	CPatchDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPatchDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PATCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();

protected:
	virtual void OnCancel();

public:
	void Open();
	void OnPatchStart();
	void OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax);
	void OnPatchEnd(bool succeeded);
	void OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax);
	void OnPatchFail(int id, const wchar_t* msg);
	void OnPatchCompleted(int id);

private:
	void BuildView();
	void RefreshView();
	void Patch(PatchTarget target);
	void ClearLauncherData();

private:
	CTreeCtrlEx m_Tree;
	map<int, ItemData*> m_LauncherList;
};
