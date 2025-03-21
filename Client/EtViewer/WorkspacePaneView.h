#pragma once
#include "afxcmn.h"



// CWorkspacePaneView 폼 뷰입니다.

#include "TreeCtrlEx.h"
class CWorkspacePaneView : public CFormView
{
	DECLARE_DYNCREATE(CWorkspacePaneView)

protected:
	CWorkspacePaneView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CWorkspacePaneView();

public:
	enum { IDD = IDD_WORKSPACEPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	bool m_bActiavte;
	CTreeCtrlEx m_TreeCtrl;
	CImageList m_ImageList;
	CMenu *m_pContextMenu;
	CTypedPtrList<CPtrList, HTREEITEM> m_SelectItemList;

	void CalcSelectItemList();
	void RefreshSelectInfo();
	bool CheckSameTypeObject( int nType );

public:
	virtual void OnInitialUpdate();
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAdd( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnRemove( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCheckItem( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetCheckItem( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetTreeCtrl( WPARAM wParam, LPARAM lParam );
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSceneAdd();
	afx_msg void OnSceneRemove();
	afx_msg void OnSkinAdd();
	afx_msg void OnSkinRemove();
	afx_msg void OnSkinSave();
	afx_msg void OnSkinSaveAs();
	afx_msg void OnAnimationAdd();
	afx_msg void OnAnimationRemove();
	afx_msg void OnAnimationMerge();
	afx_msg void OnCommonRemove();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnLightAddDir();
	afx_msg void OnLightAddPoint();
	afx_msg void OnLightAddSpot();
	afx_msg void OnLightRemove();
};


