// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "RenderBase.h"

class CEtResourceMng;
class CMainFrame : public CXTPFrameWnd
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

	CWnd *GetPaneFrame( int nID );
	int GetFocusPaneID();
	CXTPDockingPaneManager* XTPDockingPaneManager() {return &m_paneManager;}
	void SetFocus( int nID );
	void UpdateProgress( int nPer );

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
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

// 생성된 메시지 맵 함수
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


