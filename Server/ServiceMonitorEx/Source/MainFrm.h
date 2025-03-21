// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "ConnectionDialog.h"
#include "MainTaskbarIcon.h"

class CNetConnection;

class CMainFrame : public CFrameWnd
{

public:
	enum EF_STATUSBUTTONTYPE	// 상태 버튼 타입
	{
		EV_SBT_CONNECT,		// CONNECT
		EV_SBT_ERRORLOG,	// ERRORLOG
		EV_SBT_CNT,
	};

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpCreateStruct, CCreateContext* pContext);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void ToggleConnectionDialog();
	void ShowConnectionDialog(int nCmdShow);
	void ShowControlDialog(int nCmdShow);


	CConnectionDialog& GetConnectionDialog() { return m_ConnectionDialog; }

	int GetActivePageIndex() const;
	void SetPageImageIndex(int pPageIndex, int pImageIndex);
	void ForceAllTabViewActivate();

	void SetAlert(BOOL pDoAlert) { m_DoAlert = pDoAlert; }
	BOOL IsAlert() const { return m_DoAlert; }

	CBasePartitionView* AddPartition (CNetConnection* pConnection, ULONG uIpAddress);
	void RemovePartition (CBasePartitionView* pPartition);

private:
	DWORD GetTickTerm(DWORD dwOldTick, DWORD dwCurTick);

private:

	int m_TabIconTick[EV_SBT_CNT];

	int m_TaskbarIconTick;

	BOOL m_DoAlert;
	COX3DTabViewContainer m_PartitionTabView;
	CConnectionDialog m_ConnectionDialog;
	

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPopupRestore();
	afx_msg void OnPopupMinimize();
	afx_msg void OnPopupExit();
	afx_msg void OnPopupAbout();
	afx_msg void OnClose();
	afx_msg void OnViewTotalStatusBar();
	afx_msg void OnUpdateViewTotalStatusBar(CCmdUI *pCmdUI);
	afx_msg void OnSettingConnection();
	afx_msg void OnConnectionFromFile();
	afx_msg void OnUpdateSettingConnection(CCmdUI *pCmdUI);
	afx_msg void OnSettingConfig();
	afx_msg void OnUpdateSettingConfig(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnButtonErrorLog();
	afx_msg void OnUpdateButtonErrorLog(CCmdUI *pCmdUI);
	afx_msg void OnButtonRefreshWorldInfo();
	afx_msg void OnUpdateButtonRefreshWorldInfo(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
