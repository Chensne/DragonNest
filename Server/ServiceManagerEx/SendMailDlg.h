#pragma once

// CSendMailDlg ��ȭ �����Դϴ�.

class CSendMailDlg : public CDialog
{
	DECLARE_DYNAMIC(CSendMailDlg)

public:
	CSendMailDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSendMailDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SEND_MAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
};
