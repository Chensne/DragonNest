#include "StdAfx.h"
#include "DnEventReceiverTabDlg.h"
#include "DnSpecialBoxTabItemDlg.h"
#include "DnSpecialBoxListDlg.h"
#include "TaskManager.h"
#include "DnCommonTask.h"
#include "DnInterface.h"
#include "DnMainDlg.h"

#ifdef PRE_SPECIALBOX

float CDnEventReceiverTabDlg::s_fRequestDelay = 0.f;

CDnEventReceiverTabDlg::CDnEventReceiverTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
	m_pSpecialBoxListDlg = NULL;

	m_pListBoxExSpecialBox = NULL;
	m_pRadioButtonSpecialBox = NULL;
	m_pStaticItemCount = NULL;

	m_vecSpecialBoxInfo.clear();
}

CDnEventReceiverTabDlg::~CDnEventReceiverTabDlg(void)
{
	SAFE_DELETE( m_pSpecialBoxListDlg );
}

void CDnEventReceiverTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "KeepBoxTabDlg.ui" ).c_str(), bShow );
	m_pSpecialBoxListDlg = new CDnSpecialBoxListDlg( UI_TYPE_CHILD_MODAL, this );
	m_pSpecialBoxListDlg->Initialize( false );
}

void CDnEventReceiverTabDlg::InitialUpdate()
{
	m_pStaticItemCount = GetControl<CEtUIStatic>("ID_KEEPBOX_COUNT");
	m_pListBoxExSpecialBox = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");
	m_pRadioButtonSpecialBox = GetControl<CEtUIRadioButton>("ID_RBT0");
	m_pRadioButtonSpecialBox->SetChecked( true );
}

void CDnEventReceiverTabDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
	
	if( s_fRequestDelay > -1 )
	{
		s_fRequestDelay -= fElapsedTime;
		if( s_fRequestDelay < 0)
			s_fRequestDelay = -1;
	}
}

void CDnEventReceiverTabDlg::Show( bool bShow )
{ 
	if( m_bShow != bShow )
	{
		CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
		if( pTask )	
		{
			pTask->SendWindowState( bShow == true ? WINDOW_BLIND : WINDOW_NONE );
		}
	}

	if( bShow == true )
	{
		RefreshSpecialList();
	}

	CDnCustomDlg::Show( bShow );
	
}

void CDnEventReceiverTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CLOSE") )
		{
			Show( false );
		}
		if( IsCmdControl("ID_BT_CANCEL") )
		{
			Show( false );
		}
	}

	if( nCommand == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if( IsCmdControl("ID_LISTBOXEX_LIST") )
		{
			SListBoxItem *pListItem = m_pListBoxExSpecialBox->GetSelectedItem();
			if( pListItem )
			{
				CDnSpecialBoxTabItemDlg *pItem = (CDnSpecialBoxTabItemDlg *)pListItem->pData;
				if( pItem )
				{
					pItem->RequestSpecialBoxItemList();
				}
			}
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}



void CDnEventReceiverTabDlg::ClearSpecialBoxInfo()
{
	m_vecSpecialBoxInfo.clear();
}

void CDnEventReceiverTabDlg::AddSpecialBoxInfo( TSpecialBoxInfo *pBoxInfo )
{
	m_vecSpecialBoxInfo.push_back( *pBoxInfo );
}

void CDnEventReceiverTabDlg::RemoveSpecialBoxInfoFromRewardID( int nEventRewardID )
{
	std::vector<TSpecialBoxInfo>::iterator itor = m_vecSpecialBoxInfo.begin();
	for( itor = m_vecSpecialBoxInfo.begin(); itor != m_vecSpecialBoxInfo.end(); itor++ )
	{
		if( itor->nEventRewardID == nEventRewardID )
		{
			itor = m_vecSpecialBoxInfo.erase( itor );
			return;
		}
	}
}

void CDnEventReceiverTabDlg::RefreshSpecialList()
{
	m_pListBoxExSpecialBox->RemoveAllItems();
	for( DWORD i=0; i<m_vecSpecialBoxInfo.size(); i++ )
	{
		CDnSpecialBoxTabItemDlg *pItem = m_pListBoxExSpecialBox->InsertItem<CDnSpecialBoxTabItemDlg>((int)m_pListBoxExSpecialBox->GetSize());
		pItem->SetSpecialBoxInfo( &m_vecSpecialBoxInfo[i] );
	}

	m_pStaticItemCount->SetText( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8223 ),m_pListBoxExSpecialBox->GetSize() ) );

	if( m_pListBoxExSpecialBox->GetSize() == 0 )
	{
		if( GetInterface().GetMainBarDialog() ) 
		{
			GetInterface().GetMainBarDialog()->CloseEventReceiverAlarm();
		}
	}
}

CDnSpecialBoxListDlg *CDnEventReceiverTabDlg::GetSpecialListDlg()
{ 
	return m_pSpecialBoxListDlg; 
}

void CDnEventReceiverTabDlg::RequestDealy()
{
	s_fRequestDelay = SpecialBox::Common::eRequestDelay::DelayTime;
}

void CDnEventReceiverTabDlg::ResetDelayTime()
{
	s_fRequestDelay = -1;
}

bool CDnEventReceiverTabDlg::CanRequestSpecialBoxInfo()
{
	if( s_fRequestDelay == -1 )
	{
		return true;
	}

	return false;
}

#endif // PRE_SPECIALBOX