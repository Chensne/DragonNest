#pragma once
#include "afxcmn.h"

// CActionPaneView �� ���Դϴ�.
#include "EditLabelTreeCtrl.h"

#include "ActionSignal.h"

class CActionPaneView : public CFormView
{
	DECLARE_DYNCREATE(CActionPaneView)

protected:
	CActionPaneView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CActionPaneView();

public:
	enum { IDD = IDD_ACTIONPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	CEditLabelTreeCtrl	m_TreeCtrl;
	bool				m_bActivate;
	CImageList			m_ImageList;
	CXTPToolBar			m_wndToolBar;
	CMenu *				m_pContextMenu;
	CTypedPtrList<CPtrList, HTREEITEM> m_SelectItemList;
	CString m_szPrevLabelString;

	/*
	CImageList *m_pDragImage;
	bool m_bDragging;
	HTREEITEM m_hDrag;
	HTREEITEM m_hDrop;
	*/
	HTREEITEM m_hRootItem;

	void CalcSelectItemList();

public:
	void GetSelectActionList( std::vector<std::string> &szVecList );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAdd( WPARAM wParam, LPARAM lParam );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActionRemoveElement();
	afx_msg void OnActionAddElement();
	afx_msg void OnActionCloneElement();
	afx_msg void OnUpdateActionAddElement(CCmdUI *pCmdUI);
	afx_msg void OnUpdateActionRemoveElement(CCmdUI *pCmdUI);
	afx_msg void OnUpdateActionCloneElement(CCmdUI *pCmdUI);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnActionAddDlg();
	afx_msg void OnUpdateActionAddDlg(CCmdUI *pCmdUI);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	/*
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTvnBegindragTree1(NMHDR *pNMHDR, LRESULT *pResult);
	*/

#ifdef _EXPORT_EXCEL_ACTION_SIGNAL
	afx_msg void OnActionpanecontextExportexcel();
#endif
};


