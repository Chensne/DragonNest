#pragma once
#include "afxcmn.h"



class CUIToolTemplate;

// CTemplatePaneView form view

class CTemplatePaneView : public CFormView
{
	DECLARE_DYNCREATE(CTemplatePaneView)

protected:
	CTemplatePaneView();           // protected constructor used by dynamic creation
	virtual ~CTemplatePaneView();

public:
	enum { IDD = IDD_TEMPLATEPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	HTREEITEM m_hRoot;
	CImageList m_ImageList;
	bool m_bActivate;
	CMenu *m_pContextMenu;
	std::vector< CUIToolTemplate * > m_vecUIToolTemplate;

//#define SW_ADD_SEPARATE_STATIC_20091014_hayannal2009
#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)

public:
	enum
	{
		MAX_SEPARATE_STATIC = 7,
	};
protected:
	// Static은 너무 많이 늘어서 이렇게 별도로 관리한다.
	std::vector< std::string > m_vecSeparateStatic[MAX_SEPARATE_STATIC];
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
	void LoadSeparateStatic();
	void SaveSeparateStatic();
#endif

	void LoadExistUITemplate();
	void CheckExternControl();
	int GetTreeDepth( HTREEITEM hItem );
	int GetCurrentTreeDepth();
	// Note : 현재 선택된 템플릿의 엘리먼트의 수를 얻는다.
	int GetElementCount();
	bool CheckDuplicateItem( CString &szItemName );
	bool CheckDuplicateTemplate( CString &szTemplateName );
	bool CheckDuplicateElement( CString &szElementName );
	void DeleteAllTreeItemData();
	void InsertTemplate( CUIToolTemplate *pTemplate );
	void NewTemplate( CString &szTemplateName );
	void NewTemplate( CString &szTemplateName ,UI_CONTROL_TYPE UIType);
	void InsertElement( CString &szElementName, HTREEITEM hInsertItem );
	int GetTreeItemIndex( HTREEITEM hItem );
	void ProcessMultiSelect();
	void SaveAllTemplate();

	CUIToolTemplate *FindTemplate( const char *pszName );

	CXTTreeCtrl m_TemplateTree;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnTemplateAddtemplate();
	afx_msg void OnTvnSelchangedTemplateTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTemplateAddelement();
	afx_msg void OnNMClickTemplateTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnElementDelelement();
	afx_msg void OnTemplateAddtemplates();
	afx_msg void OnElementCopytemplate();
	afx_msg void OnElementSaveTemplateTexture();
	afx_msg void OnElementSaveuiUsedtemplate();
};


