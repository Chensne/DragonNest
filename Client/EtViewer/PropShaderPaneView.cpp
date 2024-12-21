// PropShaderPaneView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtViewer.h"
#include "PropShaderPaneView.h"
#include "UserMessage.h"
#include "ObjectBase.h"
#include "../MFCUtility/Common.h"


// CPropShaderPaneView

IMPLEMENT_DYNCREATE(CPropShaderPaneView, CFormView)

CPropShaderPaneView::CPropShaderPaneView()
	: CFormView(CPropShaderPaneView::IDD)
{
	m_bActivate = false;
}

CPropShaderPaneView::~CPropShaderPaneView()
{
}

void CPropShaderPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPropShaderPaneView, CFormView)
	ON_WM_SIZE()
	ON_MESSAGE( UM_SHADERPROP_REFRESH, OnRefresh )
	ON_MESSAGE( XTPWM_PROPERTYGRID_NOTIFY, OnNotifyGrid )
	ON_MESSAGE( UM_SHADERPROP_MODIFY_ITEM, OnModifyItem )
	ON_MESSAGE( UM_PROPERTYGRID_ONCUSTOM_DIALOG, OnCustomDialogGrid )
END_MESSAGE_MAP()


// CPropShaderPaneView 진단입니다.

#ifdef _DEBUG
void CPropShaderPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPropShaderPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPropShaderPaneView 메시지 처리기입니다.

void CPropShaderPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	CRect rcRect;
	GetClientRect( &rcRect );

	m_PropertyGrid.Create( rcRect, this, 0 );
	m_PropertyGrid.SetCustomColors( RGB(200, 200, 200), 0, RGB(182, 210, 189), RGB(247, 243, 233), 0);
}

void CPropShaderPaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_PropertyGrid ) {
		CRect rcRect;
		GetClientRect( &rcRect );
		m_PropertyGrid.MoveWindow( &rcRect );
	}
}

void CPropShaderPaneView::EnableControl( bool bEnable )
{
	m_PropertyGrid.EnableWindow( bEnable );
	ResetPropertyGrid();
}

LRESULT CPropShaderPaneView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	CObjectBase *pTemp = m_pObject;
	m_pObject = (CObjectBase *)wParam;

	if( m_pObject == NULL ) {
		EnableControl( false );
		return S_OK;
	}

	EnableControl( TRUE );

	std::vector<CUnionValueProperty *> *pVecList = m_pObject->GetPropertyList();
	RefreshPropertyGrid( pVecList );

	return S_OK;
}

LRESULT CPropShaderPaneView::OnNotifyGrid( WPARAM wParam, LPARAM lParam )
{
	return ProcessNotifyGrid( wParam, lParam );
}

void CPropShaderPaneView::OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pObject ) return;
	m_pObject->OnSetPropertyValue( dwIndex, pVariable );
}

void CPropShaderPaneView::OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pObject ) return;
	m_pObject->OnChangePropertyValue( dwIndex, pVariable );
}

void CPropShaderPaneView::OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pObject ) return;
	m_pObject->OnSelectChangePropertyValue( dwIndex, pVariable );
}

void CPropShaderPaneView::OnCustomDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pObject ) return;
}

LRESULT CPropShaderPaneView::OnModifyItem( WPARAM wParam, LPARAM lParam )
{
	ModifyItem( wParam, lParam );
	return S_OK;
}


LRESULT CPropShaderPaneView::OnCustomDialogGrid( WPARAM wParam, LPARAM lParam )
{
	return ProcessCustomDialogGrid( wParam, lParam );
}