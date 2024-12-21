// MainFrm.h : CMainFrame Ŭ������ �������̽�
//
#pragma once

#include "MainSplit.h"
#include "LogSplit.h"
#include "StateSplit.h"

class CFileLogView;

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();

public:
	// command
	afx_msg void OnMenuPatchUrl();
	afx_msg void OnMenuLog();
	afx_msg void OnMenuExceptionReportToEMail();
	afx_msg void OnMenuExceptionReportToClipboard();
	afx_msg void OnMenuCommand();
	afx_msg void OnMenuLoadConfig();

	// view > server view
	afx_msg void OnMenuWorldView();
	afx_msg void OnMenuLauncherView();

public:
	void BuildView();
	void RefreshView();
	void RefreshStateView();

public:
	CMainSplit& GetMainSplit() { return m_MainSplit; }
	CLogSplit& GetLogSplit() { return m_LogSplit; }
	CStateSplit& GetStateSplit() { return m_StateSplit; }

private:
	CLogSplit m_LogSplit;
	CMainSplit m_MainSplit;
	CStateSplit m_StateSplit;
	CFileLogView* m_pFileLogView;
};
