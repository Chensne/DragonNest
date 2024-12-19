#pragma once

#include "UnionValueProperty.h"
#include "PropertyGridCreator.h"
#include "StringSelect.h"

struct SUIDialogInfo;
struct SUIControlProperty;
// CControlPropertyPaneView form view

class CControlPropertyPaneView : public CFormView, public CPropertyGridImp
{
	DECLARE_DYNCREATE(CControlPropertyPaneView)

protected:
	CControlPropertyPaneView();           // protected constructor used by dynamic creation
	virtual ~CControlPropertyPaneView();

public:
	enum { IDD = IDD_CONTROLPROPERTYPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	bool				m_bActivate;
	SUIDialogInfo		*m_pDlgInfo;
	SUIControlProperty	*m_pCurControlProperty;
	int					m_nRefreshMode;
	CStringSelect		m_StringSelectDlg;

	// 텍스처컨트롤에서 텍스처 설정때 임시로 보여주려 했는데, 완전 임시코드인게,
	// 이렇게 하나만 가지고 있으니 여러 컨트롤에다 설정하면, 제대로 처리도 안된다.
	// 즉, 텍스처컨트롤 자체가 프로그램에서 직접 텍스처설정하려고 만든거라 제대로 툴에서 처리하지 못한 것.
	// 그래서 이런 이상한 예외처리는 다 빼고 제대로 넣기로 하겠다.
	//EtTextureHandle		m_hTextureControl;

public:
	void RefreshDialogProperty();
	void RefreshControlProperty( SUIControlProperty *pProperty );
	void SetDlgInfo( SUIDialogInfo *pInfo ) { m_pDlgInfo = pInfo; }

	void OnSetDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	void OnSetControlValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	void OnChangeDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	void OnChangeControlValue( CUnionValueProperty *pVariable, DWORD dwIndex, bool bNotifyChangeToLayoutView = true, SUIControlProperty *pProperty = NULL );

	virtual void OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnCustomDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);
	afx_msg LRESULT OnCustomDialogGrid( WPARAM wParam, LPARAM lParam );
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


