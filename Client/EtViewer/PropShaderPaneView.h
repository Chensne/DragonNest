#pragma once



// CPropShaderPaneView 폼 뷰입니다.
#include "PropertyGridCreator.h"

class CObjectBase;
class CPropShaderPaneView : public CFormView, public CPropertyGridImp
{
	DECLARE_DYNCREATE(CPropShaderPaneView)

protected:
	CPropShaderPaneView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CPropShaderPaneView();

	CObjectBase *m_pObject;

public:
	enum { IDD = IDD_PROPSHADERPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	bool m_bActivate;
	void EnableControl( bool bEnable );
	virtual void OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnCustomDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex );

public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	LRESULT OnNotifyGrid( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCustomDialogGrid( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnModifyItem( WPARAM wParam, LPARAM lParam );
};


