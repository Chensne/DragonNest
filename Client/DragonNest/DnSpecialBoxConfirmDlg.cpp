#include "StdAfx.h"
#include "DnSpecialBoxConfirmDlg.h"
#include "DnInterfaceString.h"
#include "TradeSendPacket.h"
#include "DnPlayerActor.h"

using namespace DN_INTERFACE::STRING;

#ifdef PRE_SPECIALBOX

CDnSpecialBoxConfirmDlg::CDnSpecialBoxConfirmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pItemSlotButton = NULL;
	m_pStaticItemName = NULL;
	m_pStaticAsk = NULL;

	m_nRewardID = 0;
}

CDnSpecialBoxConfirmDlg::~CDnSpecialBoxConfirmDlg(void)
{
}

void CDnSpecialBoxConfirmDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "KeepGiftBoxMessageBoxDlg.ui" ).c_str(), bShow );
}

void CDnSpecialBoxConfirmDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_BT_ITEM");
	m_pItemSlotButton->SetSlotType( ST_SPECIALBOX_REWARD );

	m_pStaticItemName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pStaticAsk = GetControl<CEtUIStatic>("ID_TEXT_ASK0");
}

void CDnSpecialBoxConfirmDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
}


void CDnSpecialBoxConfirmDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if( IsCmdControl("ID_BT_CLOSE") )
		{
			Show( false );
		}
		if( IsCmdControl("ID_CANCEL") )
		{
			Show( false );
		}
		if( IsCmdControl("ID_OK") )
		{
			if( m_pItemSlotButton && m_pItemSlotButton->GetItem() )
			{
				if (m_pItemSlotButton->GetItem()->IsCashItem())
					SendReceiveSpecialBoxItem( m_nRewardID , m_pItemSlotButton->GetItem()->GetCashItemSN() );
				else
					SendReceiveSpecialBoxItem( m_nRewardID , m_pItemSlotButton->GetItem()->GetClassID() );
			}
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSpecialBoxConfirmDlg::Show( bool bShow )
{ 
	CDnCustomDlg::Show( bShow );
}

void CDnSpecialBoxConfirmDlg::SetItemInfo( int nRewardID, CDnItem *pItem )
{
	m_nRewardID = nRewardID;

	m_pItemSlotButton->ResetSlot();
	if( pItem )
	{
		m_pItemSlotButton->SetItem( pItem , CDnSlotButton::ITEM_ORIGINAL_COUNT );
		m_pStaticItemName->SetText( pItem->GetName() );
		m_pStaticItemName->SetTextColor( ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
		m_pStaticAsk->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8214 ) );

		if(  pItem->IsNeedJob() == true && CDnActor::s_hLocalActor )
		{
			std::vector<int> nVecJobList;
			((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
			if( !pItem->IsPermitPlayer( nVecJobList ) )
			{
				m_pStaticAsk->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8218 ) );
			}
		}
	}
}
	

#endif // PRE_SPECIALBOX