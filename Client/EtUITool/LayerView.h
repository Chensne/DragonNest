#pragma once
#include "controls\xttreectrlview.h"



// CLayerView 폼 뷰입니다.

class CLayerView : public CFormView
{
	DECLARE_DYNCREATE(CLayerView)

protected:
	CLayerView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CLayerView();

	HTREEITEM m_hRoot;
	CImageList m_ImageList;
	bool m_bActivate;
	CMenu *m_pContextMenu;

	int GetTreeDepth( HTREEITEM hItem );
	int GetCurrentPeerOrder();

public:
	enum { IDD = IDD_LAYERVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CXTTreeCtrl m_ctrlTree;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual void OnInitialUpdate();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLayerClearAll();
	afx_msg void OnLayerClear();
};


