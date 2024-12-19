#pragma once


// CChangePasswordDialog ��ȭ �����Դϴ�.
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
	CChangePasswordDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CChangePasswordDialog();

	void SetSession(CNetSession* pSession);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHGPWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
