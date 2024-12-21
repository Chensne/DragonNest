
#pragma once

#include "resource.h"
#include "ChangePasswordDialog.h"


class CNetSession;

/////////////////////////////////////////////////////////////////////////////
// CChildDlg dialog

struct ComboBoxProcess
{
	int nSelect;
	int nWorldID;
};

class CPartitionDlg : public CDialog
{
	// Construction
public:
	CPartitionDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(const RECT &rect, CWnd *pParentWnd);

	enum { IDD = IDD_PARDLG };

	virtual void OnOK();
	virtual void OnCancel();

	void AddProcessType(int nWorldID);
	void SetSession(CNetSession* pSession);

	CChangePasswordDialog& GetChangePasswordDialog() { return m_ChangePasswordDialog; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMdbConnectbutton();
	afx_msg void OnBnClickedMdbExit();
	afx_msg void OnBnClickedMdbErrorlogbutton();
	afx_msg void OnBnClickedMdbRefreshworldinfo();

private:
	CWnd* m_pParentWnd;
	CNetSession* m_pSession;
	std::vector<ComboBoxProcess> m_ProcessType;
	CEdit m_LogDays;
	CChangePasswordDialog m_ChangePasswordDialog;
	
public:
	afx_msg void OnBnClickedStartProcess();
	afx_msg void OnBnClickedStopProcess();
	afx_msg void OnBnClickedStopForceProcess();
	afx_msg void OnBnClickedExceptReport();
	afx_msg void OnBnClickedMdbResetdelaycount();
	afx_msg void OnBnClickedMdbChangepw();
};

