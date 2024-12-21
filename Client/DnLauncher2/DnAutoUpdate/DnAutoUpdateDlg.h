// DnAutoUpdateDlg.h : 헤더 파일
//

#pragma once

#define WM_NOTIFYICON_MSG	WM_USER+1
#define WM_NOTIFYICON_HIDE	WM_USER+2

// CDnAutoUpdateDlg 대화 상자
class CDnAutoUpdateDlg : public CDialog
{
	// 생성입니다.
public:
	CDnAutoUpdateDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DNAUTOUPDATE_DIALOG };
	BOOL			m_bShowWindow;
	BOOL			m_bShowTrayTooltip;
	CMenu			m_TrayMenu;
	NOTIFYICONDATA	m_stNID;

	CWinThread*		m_pThread;
	CProgressCtrl	m_progressCtrl;
	BOOL			m_bDownloadEnd;
	DWORD			m_dwExitTick;

public:
	BOOL ShowWindowEx( int nCmdShow );
	void SetDownloadInfo( float fRate, float fPercent );
	void DownloadComplete( int nRtn );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExit();
	afx_msg LRESULT OnTrayNotifyMsg( WPARAM wParam,LPARAM lParam );
	afx_msg void OnWindowPosChanging( WINDOWPOS FAR* lpwndpos );
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nIDEvent );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonResume();
	afx_msg void OnBnClickedButtonExit();
};
