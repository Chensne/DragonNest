// EtUIToolView.h : interface of the CEtUIToolView class
//
#pragma once
#include "EtUIDialog.h"
#include "EtUISound.h"

class CDummyView;
class CUIToolTemplate;
class CEtSoundEngine;
class CEtUIXML;

class CEtUIToolView : public CView
{
protected: // create from serialization only
	CEtUIToolView();
	DECLARE_DYNCREATE(CEtUIToolView)

// Attributes
public:
	CEtUIToolDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void PostNcDestroy();

// Implementation
public:
	virtual ~CEtUIToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CXTPTabControl *m_pFlatTabCtrl;
	CDummyView *m_pDummyView;
	int m_nActiveView;
	CEtSoundEngine *m_pSoundEngine;
	CEtUISound *m_pUISound;
	CEtUIXML *m_pUIXML;
	UINT_PTR m_nTimer;

public:
	CDummyView *GetDummyView() { return m_pDummyView; }
	int GetActiveView() { return m_nActiveView; }
	CWnd *GetActiveTabView();
	CWnd *GetTabView( int nView );

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer( UINT_PTR nIDEvent );

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in EtUIToolView.cpp
inline CEtUIToolDoc* CEtUIToolView::GetDocument() const
   { return reinterpret_cast<CEtUIToolDoc*>(m_pDocument); }
#endif

