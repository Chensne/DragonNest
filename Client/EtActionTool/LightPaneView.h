#pragma once
#include "afxcmn.h"



// CLightPaneView 폼 뷰입니다.

#include "TreeCtrlEx.h"
class CLightPaneView : public CFormView
{
	DECLARE_DYNCREATE(CLightPaneView)

protected:
	CLightPaneView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CLightPaneView();

public:
	enum { IDD = IDD_LIGHTPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CTreeCtrlEx m_TreeCtrl;
	bool m_bActivate;
	CImageList m_ImageList;
	CMenu *m_pContextMenu;
	CTypedPtrList<CPtrList, HTREEITEM> m_SelectItemList;

	void CalcSelectItemList();
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAdd( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCheckItem( WPARAM wParam, LPARAM lParam );
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLightpaneAdddirectional();
	afx_msg void OnLightpaneAddpoint();
	afx_msg void OnLightpaneAddspot();
	afx_msg void OnLightpanecontextDelete();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
};


