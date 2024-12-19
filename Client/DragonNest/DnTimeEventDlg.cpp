#include "StdAfx.h"
#include "DnTimeEventDlg.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"
#include "DnTimeEventListItem.h"

CDnTimeEventDlg::CDnTimeEventDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pListBoxEx = NULL;
}

CDnTimeEventDlg::~CDnTimeEventDlg()
{
	m_pListBoxEx->RemoveAllItems();
}

void CDnTimeEventDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "EventTimeDlg.ui" ).c_str(), bShow );
}

void CDnTimeEventDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");
}

void CDnTimeEventDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( bShow ) {
		RefreshInfo();
	}
	else {
		m_pListBoxEx->RemoveAllItems();
	}

	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
	}
	CDnCustomDlg::Show( bShow );
}

void CDnTimeEventDlg::RefreshInfo()
{
	if( !CDnTimeEventTask::IsActive() ) return;
	CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();

	if( pTask->GetTimeEventCount() == 0 ) return;

	m_pListBoxEx->RemoveAllItems();

	for( DWORD i=0; i<pTask->GetTimeEventCount(); i++ ) {
		CDnTimeEventListItem *pItem = m_pListBoxEx->AddItem<CDnTimeEventListItem>();
		CDnTimeEventTask::TimeEventInfoStruct *pStruct = pTask->GetTimeEventInfo(i);
		pItem->SetInfo( this, pStruct );
		if( pTask->GetTimeEventAlarm() == pStruct->nEventID )
			pItem->SetCheckBox( true );
	}
}

void CDnTimeEventDlg::InvertCheckOthers( CDnTimeEventListItem *pListItem )
{
	int nSize = m_pListBoxEx->GetSize();
	for( int i = 0; i < nSize; i++)  {
		CDnTimeEventListItem *pItem = m_pListBoxEx->GetItem<CDnTimeEventListItem>(i);
		if( pListItem != pItem )  {
			pItem->SetCheckBox( false );
		}
	}
}

void CDnTimeEventDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	if( nCommand == EVENT_BUTTON_CLICKED ) {
		if( IsCmdControl( "ID_CANCEL" ) || IsCmdControl("ID_BT_CLOSE") ) {
			Show( false );
		}
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
