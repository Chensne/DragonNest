// MainFrm.h : CMainFrame 클래스의 인터페이스
//
#pragma once

#include "MainSplit.h"
#include "LogSplit.h"
#include "StateSplit.h"

class CFileLogView;

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 생성된 메시지 맵 함수
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
