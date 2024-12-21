// MainView.h : interface of the CMainView class
//


#pragma once


class CMainView : public CFormView
{
protected: // create from serialization only
	CMainView();
	DECLARE_DYNCREATE(CMainView)

public:
	enum{ IDD = IDD_MAINFRM };

// Attributes
public:
	CMainDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CMainView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MainView.cpp
inline CMainDoc* CMainView::GetDocument() const
   { return reinterpret_cast<CMainDoc*>(m_pDocument); }
#endif

