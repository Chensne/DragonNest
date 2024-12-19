// MainFrm.h : CMainFrame Ŭ������ �������̽�
//


#pragma once

#include "RenderBase.h"

class CEtResourceMng;
class CMainFrame : public CXTPFrameWnd
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

	CWnd *GetPaneFrame( int nID );
	int GetFocusPaneID();
	CXTPDockingPaneManager* XTPDockingPaneManager() {return &m_paneManager;}
	void SetFocus( int nID );
	void UpdateProgress( int nPer );

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	DWORD m_dwStyle;
	DWORD m_dwExStyle;
	CRect m_rcPrevRect;

	CXTPStatusBar m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CProgressCtrl m_wndProgCtrl;

	CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;
	CXTPDockingPaneManager m_paneManager;

	CEtResourceMng *m_pResMng;

	CRenderBase m_RenderBase;

	CXTPDockingPane* CreatePane( int nID, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour = NULL, CRect *pRect = NULL ) ;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnUpdatePaneWorkspacepane(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePanePropertiespane(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePaneAnimationlistpane(CCmdUI *pCmdUI);
	afx_msg void OnPaneWorkspacepane();
	afx_msg void OnPanePropertiespane();
	afx_msg void OnPaneAnimationlistpane();
	afx_msg void OnFullscreen();
	afx_msg void OnUpdateFullscreen(CCmdUI *pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShaderfolder();
	afx_msg void OnBoneProperties();
	afx_msg void OnUpdateBoneProperties(CCmdUI *pCmdUI);
	afx_msg void OnToggleGrid();
	afx_msg void OnLesspriority();
	afx_msg void OnUpdateLesspriority(CCmdUI *pCmdUI);
	afx_msg void OnLinkObject();
	afx_msg void OnUpdateLinkObject(CCmdUI *pCmdUI);
	afx_msg void OnUnlinkObject();
	afx_msg void OnUpdateUnlinkObject(CCmdUI *pCmdUI);
	afx_msg void OnCapture();
};


