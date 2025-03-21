#pragma once



// CPropertiesPaneView 폼 뷰입니다.
#include "PropertyGridCreator.h"

#define _CHECK_CUSTOMFLY

class CActionBase;
class CPropertiesPaneView : public CFormView, public CPropertyGridImp
{
	DECLARE_DYNCREATE(CPropertiesPaneView)

protected:
	CPropertiesPaneView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CPropertiesPaneView();

public:
	enum { IDD = IDD_PROPERTIESPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	bool m_bActivate;

	CActionBase *m_pObject;
#ifdef _CHECK_CUSTOMFLY
	bool m_bNoAccessRefreshState;
#endif

public:
	void EnableControl( bool bEnable );

	virtual void OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	virtual void OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	LRESULT OnNotifyGrid( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetReadOnly( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetModify( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCheckState( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()

private:
#ifdef _CHECK_CUSTOMFLY
	bool CheckCustomSignal_Fly() const;
#endif
};


