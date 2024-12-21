#pragma once

class CPreparePatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreparePatchDlg)

public:
	CPreparePatchDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPreparePatchDlg();

// 대화 상자 데이터입니다.
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
