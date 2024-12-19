#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class DnEditLauncherDlg : public CDialog
{
	DECLARE_DYNAMIC(DnEditLauncherDlg)

public:
	DnEditLauncherDlg(CWnd* pParent = NULL);
	virtual ~DnEditLauncherDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DNEDITLAUNCHERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

private:
	DWORD			m_dwWidth;
	DWORD			m_dwHeight;
	HDC				m_dcBkGrnd;
	
	CTreeCtrl		m_TreeSoft;
	CListCtrl		m_ListCtrl;

	bool			m_bLBtnClickedFlag;
	CWnd*			m_pLBtnClickedControl;
	CPoint			m_LBtnClickedPos;
	
	CWnd*			m_pRBtnClickedControl;
	CString			m_szRButtonItem;
	CMenu			m_RBtnPopupMenu;

	HTREEITEM		m_hTreeItem;

	HACCEL			m_hAccel;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
	afx_msg void OnNMRclickTreeControl(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnLoadImage();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnMenuTextmodify();
	
	afx_msg void OnFileMainSave();
	afx_msg void OnFileMainLoad();
	afx_msg void OnFileMainQuit();

	afx_msg void OnAbout();
	afx_msg void OnDestroy();
	afx_msg void OnMenuPosDefault();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMenuScale();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnMenuResourceReset();
	afx_msg void OnCreateBtn();
	afx_msg void OnCreateStatictext();
	afx_msg void OnCreateProgress();
	afx_msg void OnCreateCheckbox();
	afx_msg void OnMenuCtrlmodify();
	afx_msg void OnFileNewfile();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	int		InitBackGround();
	int		InitTreeControl();
	void	UnCheckChildItems(HTREEITEM hItem);
	void	CheckChildItems(HTREEITEM hItem);
	void	SetPossibleMenu(CMenu* pMenu, CString currentItemName);
	
	
	// picture control
	CStatic m_pcWebpage;
	CButton m_btnWebpage;
	
	// button
	CButton m_BtnQuit;
	CButton m_btnStartGame;
	CButton m_btnOption;
	CButton m_btnWinClose;
	CButton m_btnWinMini;

	CButton m_btnStartPatch;
	CButton m_btnSelectPartition;

	// progress bar
	CProgressCtrl	m_prgDownLoad;
	CProgressCtrl	m_prgFileCount;
	CButton			m_btnProgressDownload;
	CButton			m_btnProgressCount;
	
	// text
	CStatic m_stVersion;
	CStatic m_stFileCnt;
	CStatic m_stFileName;
	CStatic m_stTimeLeft;
	CStatic m_stFileSize;
	CStatic m_stDownPercent;
	CStatic m_stDownState;
	CStatic m_stDirectStart;

	CStatic m_stSelectPartition;
	CStatic m_stPartitionName;
	CStatic m_stPartitionGuide;

	// check box
	CButton m_cbDirectStart;
	afx_msg void OnMenuDelete();

	CFont	m_Font;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMenuSetrect();
	afx_msg void OnControlLock();
	afx_msg void OnControlUnlock();
};
