// EventEditView_SelectSignal.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "EventEditView_SelectSignal.h"
#include "SignalManager.h"
#include "SignalItem.h"
#include "EventEditView_CreateModifySignal.h"
#include "UnionValueProperty.h"


// CEventEditView_SelectSignal

IMPLEMENT_DYNCREATE(CEventEditView_SelectSignal, CFormView)

CEventEditView_SelectSignal::CEventEditView_SelectSignal()
	: CFormView(CEventEditView_SelectSignal::IDD)
{
	m_bActivate = false;
}

CEventEditView_SelectSignal::~CEventEditView_SelectSignal()
{
}

void CEventEditView_SelectSignal::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CEventEditView_SelectSignal, CFormView)
END_MESSAGE_MAP()


// CEventEditView_SelectSignal �����Դϴ�.

#ifdef _DEBUG
void CEventEditView_SelectSignal::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEventEditView_SelectSignal::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEventEditView_SelectSignal �޽��� ó�����Դϴ�.

void CEventEditView_SelectSignal::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	m_ListCtrl.AddColumn( "Name", 255 );
	m_ListCtrl.AddColumn( "Initials", 50, LVCFMT_CENTER );
	m_ListCtrl.AddColumn( "Has Length", 70, LVCFMT_CENTER );
	m_ListCtrl.AddColumn( "Parameter Count", 100, LVCFMT_RIGHT );

	m_ListCtrl.SetExtendedStyle( LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_FLATSB );
}

bool SignalCompareProc( CSignalItem *pItem1, CSignalItem *pItem2 )
{
	return ( strcmp( pItem1->GetName(), pItem2->GetName() ) > 0 ) ? false : true;
}

void CEventEditView_SelectSignal::PreProcess()
{
	m_ListCtrl.DeleteAllItems();

	std::vector<CSignalItem *> pVecList;
	for( DWORD i=0; i<CSignalManager::GetInstance().GetSignalItemCount(); i++ ) {
		pVecList.push_back( CSignalManager::GetInstance().GetSignalItem(i) );
	}
	std::sort( pVecList.begin(), pVecList.end(), SignalCompareProc );

	CString szStr;
	for( DWORD i=0; i<pVecList.size(); i++ ) {
		CSignalItem *pItem = pVecList[i];
		m_ListCtrl.InsertItem( i, pItem->GetName() );

		szStr.Format( "%c|%d|%d", pItem->GetName()[0], pItem->GetInitialsColor(), pItem->GetBackgroundColor() );
		m_ListCtrl.SetItemText( i, 1, szStr );

		szStr.Format( "%s", ( pItem->IsHasLength() == true ) ? "True" : "False" );
		m_ListCtrl.SetItemText( i, 2, szStr );

		szStr.Format( "%d", pItem->GetParameterCount() );
		m_ListCtrl.SetItemText( i, 3, szStr );
	}
}

bool CEventEditView_SelectSignal::PostProcess()
{
	POSITION p = m_ListCtrl.GetFirstSelectedItemPosition();
	std::vector<CString> szVecSelectItemList;
	while(p) {
		int nIndex = m_ListCtrl.GetNextSelectedItem(p);
		szVecSelectItemList.push_back( m_ListCtrl.GetItemText( nIndex, 0 ) );
	}
	if( szVecSelectItemList.size() == 0 ) {
		MessageBox( "��ĥ�� ��������~", "����", MB_OK );
		return false;
	}

	// �����ؼ� �־��ش�.
	switch( s_nStartType ) {
		case 1:	// Modify
			{
				// ��Ұ� ���� �� �����Ƿ� �����ؼ� �־��ش�.
				CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromName( szVecSelectItemList[0] );
				if( pItem == NULL ) assert(0);
				s_szCurrentSignalName = pItem->GetName();

				s_pCurrentSignalItem->SetName( pItem->GetName() );
				s_pCurrentSignalItem->SetInitialsColor( pItem->GetInitialsColor() );
				s_pCurrentSignalItem->SetBackgroundColor( pItem->GetBackgroundColor() );
				s_pCurrentSignalItem->SetHasLength( pItem->IsHasLength() );
			}
			break;
		case 2:	// Delete
			{
				CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromName( szVecSelectItemList[0] );
				if( pItem == NULL ) assert(0);

				s_szCurrentSignalName = pItem->GetName();
			}
			break;
		case 3:	// Create Parameter
		case 4:	// Modify Parameter
			{
				CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromName( szVecSelectItemList[0] );
				s_pCurrentSignalItem = pItem;

				s_pCurrentPropertyVariable = new CUnionValueProperty( CUnionValueProperty::Unknown );
			}
			break;
		case 5:	// Delete Parameter
			{
				CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromName( szVecSelectItemList[0] );
				s_pCurrentSignalItem = pItem;
			}
			break;
	}

	return true;
}

CWizardCommon::VIEW_TYPE CEventEditView_SelectSignal::GetNextView() 
{ 
	switch( s_nStartType ) {
		case 1: // Modify Signal
			return CREATE_SIGNAL;
		case 2: // Delete Signal
			return FINISH;
		case 3:	// Create Parameter
			return CREATE_PARAMETER;
		case 4:	// Modify Parameter
		case 5: // Delete Parameter
			return SELECT_PARAMETER;
		default:
			assert(0);
	}

	return UNKNOWN_TYPE;
}