#pragma once


// CChangePasswordDialog 대화 상자입니다.
#include "resource.h"

class CNetSession;
struct ComboBoxMonitorUser
{
	int nSelect;
	char szUserName[NAMELENMAX];
	int nMonitorLevel;
};

class CChangePasswordDialog : public CDialog
{
	DECLARE_DYNAMIC(CChangePasswordDialog)

public:
	CChangePasswordDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CChangePasswordDialog();

	void SetSession(CNetSession* pSession);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHGPWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSummit();

private:
	std::vector<ComboBoxMonitorUser> m_MonitorUser;
	CEdit m_Password;
	CNetSession* m_pSession;
};
