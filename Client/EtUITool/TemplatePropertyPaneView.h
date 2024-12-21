#pragma once

#include "UnionValueProperty.h"
#include "PropertyGridCreator.h"

class CUIToolTemplate;

// CTemplatePropertyPaneView form view

class CTemplatePropertyPaneView : public CFormView, public CPropertyGridImp
{
	DECLARE_DYNCREATE(CTemplatePropertyPaneView)

protected:
	CTemplatePropertyPaneView();           // protected constructor used by dynamic creation
	virtual ~CTemplatePropertyPaneView();

public:
	enum { IDD = IDD_TEMPLATEPROPERTYPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	bool m_bActivate;
	CUIToolTemplate *m_pCurTemplate;
	std::vector< int > m_vecUIElementIndex;

public:
	virtual void OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnCustomDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void RefreshProperty( CUIToolTemplate *pTemplate, std::vector< int > &vecItemIndex );
	void ApplyTemplateChange( bool bChangeElementTexture = false );

	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);
	afx_msg LRESULT OnCustomDialogGrid( WPARAM wParam, LPARAM lParam );
};


