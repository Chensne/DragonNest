// PropertiesPaneView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "PropertiesPaneView.h"
#include "EventPaneView.h"
#include "ActionBase.h"
#include "UserMessage.h"
#include "ActionSignal.h"
#include "SignalCustomRender.h"
#include "GlobalValue.h"
#ifdef _CHECK_CUSTOMFLY
#include "MainFrm.h"
#include "PaneDefine.h"
#endif


// CPropertiesPaneView

IMPLEMENT_DYNCREATE(CPropertiesPaneView, CFormView)

CPropertiesPaneView::CPropertiesPaneView()
	: CFormView(CPropertiesPaneView::IDD)
{
	m_bActivate = false;
	m_pObject = NULL;
#ifdef _CHECK_CUSTOMFLY
	m_bNoAccessRefreshState = false;
#endif
}

CPropertiesPaneView::~CPropertiesPaneView()
{
}

void CPropertiesPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPropertiesPaneView, CFormView)
	ON_WM_SIZE()
	ON_MESSAGE( UM_REFRESH_PANE, OnRefresh )
	ON_MESSAGE( XTPWM_PROPERTYGRID_NOTIFY, OnNotifyGrid )
	ON_MESSAGE( UM_PROPERTY_PANE_SET_READONLY, OnSetReadOnly )
	ON_MESSAGE( UM_PROPERTY_PANE_SET_MODIFY, OnSetModify )
	ON_MESSAGE( UM_PROPERTY_PANE_CHECK_STATE, OnCheckState )
END_MESSAGE_MAP()


// CPropertiesPaneView 진단입니다.

#ifdef _DEBUG
void CPropertiesPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPropertiesPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPropertiesPaneView 메시지 처리기입니다.

void CPropertiesPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	CRect rcRect;
	GetClientRect( &rcRect );

	m_PropertyGrid.Create( rcRect, this, 0 );
	m_PropertyGrid.SetCustomColors( RGB(184, 177, 205), 0, RGB(182, 210, 189), RGB(247, 243, 233), 0);
}

void CPropertiesPaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_PropertyGrid ) {
		CRect rcRect;
		GetClientRect( &rcRect );
		m_PropertyGrid.MoveWindow( &rcRect );
	}
}

void CPropertiesPaneView::EnableControl( bool bEnable )
{
	m_PropertyGrid.EnableWindow( bEnable );
	ResetPropertyGrid();
}

LRESULT CPropertiesPaneView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	ResetPropertyGrid();

	m_pObject = (CActionBase *)lParam;

	if( m_pObject == NULL ) {
		m_pObject = CGlobalValue::GetInstance().GetGlobalObject();
//		EnableControl( false );
//		return S_OK;
	}

	EnableControl( TRUE );

	std::vector<CUnionValueProperty *> *pVecList = m_pObject->GetPropertyList();
	RefreshPropertyGrid( pVecList );
	
	char szStr[2048] = { 0, };
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ResourceFolder", szStr, 2048 );
	SetInitDirectory( szStr );
	
	return S_OK;
}

#ifdef _CHECK_CUSTOMFLY
bool CPropertiesPaneView::CheckCustomSignal_Fly() const
{
	const std::vector<CUnionValueProperty*>* pPropertyList = m_pObject->GetPropertyList();
	if (pPropertyList == NULL)
		return true;
	
	bool bCustomFly2 = false, bCustomFly = false;
	std::vector<CUnionValueProperty*>::const_iterator propertyIter = pPropertyList->begin();
	for (; propertyIter != pPropertyList->end(); ++propertyIter)
	{
		const CUnionValueProperty* pPropertyVar = (*propertyIter);
		if (pPropertyVar)
		{
			const char* pCurrentCategory = pPropertyVar->GetCategory();
			if (strcmp(pCurrentCategory, "CustomState") == 0)
			{
				const CString& valueString = pPropertyVar->GetBindStr();
				if (valueString.CompareNoCase("Custom_Fly2") == 0)
					bCustomFly2 = true;
				else if (valueString.CompareNoCase("Custom_Fly") == 0)
					bCustomFly = true;

				if (bCustomFly2 && bCustomFly)
					return true;
			}
		}
	}

	if (bCustomFly2 && (bCustomFly == false))
	{
		int nID = ::MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, "State에 Custom_Fly 값을 설정해 주세요!\n\n(Custom_Fly2 값이 설정되면 Custom_Fly 값이 반드시 있어야합니다.)", "에러", MB_OK );
		CWnd* pWnd = GetPaneWnd( EVENT_PANE );
		if( pWnd ) pWnd->SendMessage( WM_LBUTTONUP );

		return false;
	}

	return true;
}
#endif

LRESULT CPropertiesPaneView::OnNotifyGrid( WPARAM wParam, LPARAM lParam )
{
	return ProcessNotifyGrid( wParam, lParam );
}

void CPropertiesPaneView::OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( m_pObject ) m_pObject->OnSetPropertyValue( dwIndex, pVariable );
}

void CPropertiesPaneView::OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( m_pObject )
		m_pObject->OnChangePropertyValue( dwIndex, pVariable );

	CWnd * pWnd = GetPaneWnd( EVENT_PANE );
	CEventPaneView * pEventPaneView = dynamic_cast<CEventPaneView *>(pWnd);
	if( pWnd )
	{
		pEventPaneView->ChangeValueSelectedArraySignal( pVariable, dwIndex );

		// ---- 시그널 길이변경은 하지않도록 한다. 
		// 이유는 시그널이 종료된 이후에도 카메라는 keyframe 만큼 계속 적용될 것이고,
		// 카메라 Keyframe 이 Element의 최대길이를 넘어가는 경우가 많기 때문이다.
		// ----
		// .cam 데이타를 읽어들여 카메라KeyFrame길이만큼 Signal의 EndFrame 을 설정한다.
		//pEventPaneView->ChangeValueSelectedSignal( pVariable, dwIndex ); // bintitle.
	}
}

void CPropertiesPaneView::OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( m_pObject ) m_pObject->OnSelectChangePropertyValue( dwIndex, pVariable );
}

LRESULT CPropertiesPaneView::OnSetReadOnly( WPARAM wParam, LPARAM lParam )
{
	SetReadOnly( (DWORD)wParam, lParam == 0 ? false : true );
	return S_OK;
}

LRESULT CPropertiesPaneView::OnSetModify( WPARAM wParam, LPARAM lParam )
{
	CUnionValueProperty *pVariable = (CUnionValueProperty *)lParam;
	if( lParam == NULL ) 
		ModifyItem( wParam, (LPARAM)m_pVecVariable[wParam] );
	else ModifyItem( wParam, (LPARAM)lParam );
	return S_OK;
}

LRESULT CPropertiesPaneView::OnCheckState( WPARAM wParam, LPARAM lParam )
{
#ifdef _CHECK_CUSTOMFLY
	if (CheckCustomSignal_Fly() == false)
		return S_FALSE;
#endif
	return S_OK;
}