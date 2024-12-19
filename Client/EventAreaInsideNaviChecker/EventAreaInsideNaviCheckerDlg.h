// EventAreaInsideNaviCheckerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


class CEtWorld;
// CEventAreaInsideNaviCheckerDlg dialog
class CEventAreaInsideNaviCheckerDlg : public CDialog
{
// Construction
public:
	CEventAreaInsideNaviCheckerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EVENTAREAINSIDENAVICHECKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


public:
	CString m_szFolder;
	BOOL m_bCheckEventAreaInsideNavi;
	BOOL m_bCheckOptionMonster;
	BOOL m_bCheckOptionMonsterGroup;
	BOOL m_bCheckOptionNpc;
	BOOL m_bCheckOptionEtc;
	bool m_bWorking;

protected:
	HANDLE m_hThreadHandle;
	unsigned  m_dwThreadID;
	CImageList m_ImageList;

protected:
	static UINT __stdcall CheckStart( void *pParam );
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

	void EnableControl( bool bEnable );
	bool IsValidEventAreaInsideNaviMesh( CEtWorld *pWorld, std::vector<std::string> &szVecResult );
	bool IsValidUnitAreaTableID( CEtWorld *pWorld, std::vector<std::string> &szVecResult );
	CXTTreeCtrl m_TreeCtrl;
};
