#pragma once


// CDnAboutDlg 대화 상자입니다.

class CDnAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnAboutDlg)

public:
	CDnAboutDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDnAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_HOWTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
