// MainFrm.h : CMainFrame Ŭ������ �������̽�
//


#pragma once

class CMainFrame : public CXTPFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:
	CXTPDockingPaneManager m_paneManager;

	CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;

	int m_nCount;

	BOOL m_bContextMenu;

	BOOL m_bForbidEditClosing;
	BOOL m_bForbidFormClosing;
	BOOL m_bForbidListClosing;

	std::vector<CXTPDockingPane*> m_vecPanes;


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

	void CreatePanes();	
	// Generated message map functions
	CXTPDockingPane* CreatePane(CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour = NULL, int width = 300, int height = 160) ;
public:
	bool IsPaneSelected( int nID );
	void ShowPane( int nIndex );


// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


