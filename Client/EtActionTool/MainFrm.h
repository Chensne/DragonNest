// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "MiniSliderEx.h"

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
	CWnd *GetPaneFrame( int nID );
	int GetFocusPaneID();
	CXTPDockingPaneManager* XTPDockingPaneManager() {return &m_paneManager;}
	void SetFocus( int nID );


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
	CXTPStatusBar m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;
	CXTPDockingPaneManager m_paneManager;

	CMiniSliderEx m_PlaySlider;

	CXTPDockingPane* CreatePane( int nID, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour = NULL, CRect *pRect = NULL ) ;
	bool m_bLoopFlag;

	CEtResourceMng *m_pResMng;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPlayRewind();
	afx_msg void OnPlayPrev();
	afx_msg void OnPlayPlay();
	afx_msg void OnPlayNext();
	afx_msg void OnPlayFoward();
	afx_msg void OnPlayFrame();
	afx_msg void OnPlayLoop();
	afx_msg LRESULT OnPlaySliderUpdatePos( WPARAM wParam, LPARAM lParam );
	

	afx_msg void OnUpdatePlayRewind(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayPrev(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayPlay(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayNext(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayFoward(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayFrame(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlaySlider(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayLoop(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnSignalEdit();
	afx_msg void OnProperties();
	afx_msg void OnToggleGrid();
	afx_msg void OnLesspriority();
	afx_msg void OnUpdateLesspriority(CCmdUI *pCmdUI);
	afx_msg void OnPlayReload();
	afx_msg void OnUpdatePlayReload(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditRemove();
	afx_msg void OnUpdateEditRemove(CCmdUI *pCmdUI);
	afx_msg void OnConfigSelectWeapon();
	afx_msg void OnUpdateConfigSelectWeapon(CCmdUI *pCmdUI);
	afx_msg void OnCapture();
	afx_msg void OnHitSignalShow();
	afx_msg void OnUpdateHitSignalShow(CCmdUI *pCmdUI);
	afx_msg void OnCopyPasteSignal();
	afx_msg void OnUpdateCopyPasteSignal(CCmdUI *pCmdUI);
};


