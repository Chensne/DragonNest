#pragma once

// CSendMailDlg 대화 상자입니다.

class CSendMailDlg : public CDialog
{
	DECLARE_DYNAMIC(CSendMailDlg)

public:
	CSendMailDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSendMailDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SEND_MAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
};
