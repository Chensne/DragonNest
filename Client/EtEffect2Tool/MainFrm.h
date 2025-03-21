// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

class CMainFrame : public CXTPFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	CXTPDockingPaneManager m_paneManager;

	CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;

	int m_nCount;

	BOOL m_bContextMenu;

	BOOL m_bForbidEditClosing;
	BOOL m_bForbidFormClosing;
	BOOL m_bForbidListClosing;

	std::vector<CXTPDockingPane*> m_vecPanes;


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

	void CreatePanes();	
	// Generated message map functions
	CXTPDockingPane* CreatePane(CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour = NULL, int width = 300, int height = 160) ;
public:
	bool IsPaneSelected( int nID );
	void ShowPane( int nIndex );


// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


