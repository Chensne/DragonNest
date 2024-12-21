// MainFrm.h : interface of the CMainFrame class
//
#pragma once
#include "EtUIType.h"

class CEtResourceMng;

class CMainFrame : public CXTPFrameWnd
{
	
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
	CXTPDockingPaneManager* XTPDockingPaneManager() {return &m_paneManager;}

	CWnd *GetPaneFrame( int nID );
	void SetPaneText( int nID, CString &szText );

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CXTPStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CMap< UINT, UINT, CWnd* , CWnd * > m_mapPanes;
	CXTPDockingPaneManager m_paneManager;

	CEtResourceMng *m_pResMng;
	int m_nCreateINIFile;
	int m_nTemplateOnLayoutView;

	// Directory
	static CString m_szShaderFolder;
	static CString m_szResourceFolder;
	static CString m_szUITemplateFolder;

protected:
	CXTPDockingPane* CreatePane( int nID, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour = NULL, CRect *pRect = NULL );
	const char* CONTROL_TYPE_2_STRING( UI_CONTROL_TYPE typeControl );

public:
	BOOL CheckSelectedCount();
	BOOL IsPaneClosed( int nID );

	static void ChangeShaderFolder( const char *szFullPath );
	static CString GetShaderFolder() { return m_szShaderFolder; }

	static void ChangeResourceFolder( const char *szFullPath );
	static CString GetResourceFolder() { return m_szResourceFolder; }

	static void ChangeUITemplateFolder( const char *szFullPath );
	static CString GetUITemplateFolder() { return m_szUITemplateFolder; }

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnUpdateFileSavealltemplate(CCmdUI *pCmdUI);
	afx_msg void OnFileSavealltemplate();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnFileSetpropertyfolder();
	afx_msg void OnFileLoadXml();
	afx_msg void OnFileCreateIniFile();
	afx_msg void OnUpdateFileCreateIniFile(CCmdUI *pCmdUI);
	afx_msg void OnViewTemplateOnLayoutview();
	afx_msg void OnUpdateViewTemplateOnLayoutview(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveDialogInfo();
	afx_msg void OnFormatSetfont();
	afx_msg void OnSetBackDlg();
	afx_msg void OnDelBackDlg();
	afx_msg void OnSaveCustomui();
	afx_msg void OnSaveAllui();
};


