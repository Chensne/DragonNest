#pragma once

class CPreparePatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreparePatchDlg)

public:
	CPreparePatchDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPreparePatchDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PREPARE_PATCH };

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkListPatchList();

private:
	void InitPatchList();
	bool PatchURL();
};
