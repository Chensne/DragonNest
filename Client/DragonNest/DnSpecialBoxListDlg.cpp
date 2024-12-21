#include "StdAfx.h"
#include "DnSpecialBoxListDlg.h"
#include "DnSpecialBoxListItemDlg.h"
#include "DnSpecialBoxConfirmDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "TradeSendPacket.h"
#include "DnEventReceiverTabDlg.h"
#include "DnPlayerActor.h"

#ifdef PRE_SPECIALBOX

CDnSpecialBoxListDlg::CDnSpecialBoxListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pStaticInfo = NULL;
	m_pStaticType = NULL;
	m_pSpecialBoxConfirmDlg = NULL;
	m_pSpecialBoxList = NULL;

	m_nReceiveType = SpecialBox::ReceiveTypeCode::All;
	m_nRewardID = 0;
}

CDnSpecialBoxListDlg::~CDnSpecialBoxListDlg(void)
{
	SAFE_DELETE( m_pSpecialBoxConfirmDlg );
}

void CDnSpecialBoxListDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "KeepGiftBoxDlg.ui" ).c_str(), bShow );
	m_pSpecialBoxConfirmDlg = new CDnSpecialBoxConfirmDlg( UI_TYPE_CHILD_MODAL , this );
	m_pSpecialBoxConfirmDlg->Initialize( false );
}

void CDnSpecialBoxListDlg::InitialUpdate()
{
	m_pSpecialBoxList = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");

	m_pStaticInfo = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	m_pStaticInfo->SetTextColor( textcolor::YELLOW );

	m_pStaticType = GetControl<CEtUIStatic>("ID_TEXT_ASK2");
	

	m_sUIGoldSet.pStatic[sUIGoldControl::Text] = GetControl<CEtUIStatic>("ID_TEXT_GOLD");
	m_sUIGoldSet.pStatic[sUIGoldControl::Cover] = GetControl<CEtUIStatic>("ID_STATIC1");
	m_sUIGoldSet.pStatic[sUIGoldControl::Gold] = GetControl<CEtUIStatic>("ID_GOLD");
	m_sUIGoldSet.pStatic[sUIGoldControl::Silver] = GetControl<CEtUIStatic>("ID_SILVER");
	m_sUIGoldSet.pStatic[sUIGoldControl::Bronze]  = GetControl<CEtUIStatic>("ID_BRONZE");
}

void CDnSpecialBoxListDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
}

void CDnSpecialBoxListDlg::Show( bool bShow )
{ 
	CDnCustomDlg::Show( bShow );
}

void CDnSpecialBoxListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if( IsCmdControl("ID_BT_CLOSE") )
		{
			Show( false );
		}
		if( IsCmdControl("ID_BT_CANCEL") )
		{
			Show( false );
		}
		if( IsCmdControl("ID_BT_OK") )
		{
			OpenItemConfirmDlg();
		}
	}

	if( nCommand == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if( IsCmdControl("ID_LISTBOXEX_LIST") )
		{
			OpenItemConfirmDlg();
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSpecialBoxListDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg  )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( nID == MESSAGEBOX_REQEUST_SPECIALBOX )
		{
			if( IsCmdControl("ID_YES" ) )
			{
				SendReceiveSpecialBoxItem( m_nRewardID , 0 );
			}
		}
	}
}

void CDnSpecialBoxListDlg::SetSpecialBoxListItem( SCSpecialBoxItemList *pData )
{
	m_pSpecialBoxList->RemoveAllItems();

	if( pData && pData->nRet == ERROR_NONE )
	{
		for(int i=0; i<pData->cCount; i++ )
		{
			CDnSpecialBoxListItemDlg *pItem = m_pSpecialBoxList->InsertItem<CDnSpecialBoxListItemDlg>((int)m_pSpecialBoxList->GetSize());
			pItem->SetRewardItemInfo( &pData->BoxItem[i] );
		}

		INT64 nGold = pData->biRewardCoin/10000;
		INT64 nSilver = (pData->biRewardCoin%10000)/100;
		INT64 nBronze = pData->biRewardCoin%100;
		m_sUIGoldSet.pStatic[sUIGoldControl::Gold]->SetInt64ToText( nGold );
		m_sUIGoldSet.pStatic[sUIGoldControl::Silver]->SetInt64ToText( nSilver );
		m_sUIGoldSet.pStatic[sUIGoldControl::Bronze]->SetInt64ToText( nBronze );
		m_sUIGoldSet.Enable( pData->biRewardCoin > 0 );
	}
}

void CDnSpecialBoxListDlg::SetParentsInfo( TSpecialBoxInfo m_SpecialBoxInfo )
{
	m_nReceiveType = m_SpecialBoxInfo.cReceiveTypeCode;
	m_nRewardID = m_SpecialBoxInfo.nEventRewardID;

	if( m_nReceiveType == SpecialBox::ReceiveTypeCode::Select )
	{
		m_pStaticInfo->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8215) );
		m_pSpecialBoxList->SetRenderSelectBar( true );
	}
	else if( m_nReceiveType == SpecialBox::ReceiveTypeCode::All )
	{
		m_pStaticInfo->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8213) );
		m_pSpecialBoxList->SetRenderSelectBar( false );
	}

	if( m_SpecialBoxInfo.cTargetTypeCode == SpecialBox::TargetTypeCode::AccountAll || m_SpecialBoxInfo.cTargetTypeCode == SpecialBox::TargetTypeCode::AccountSelect )
	{
		m_pStaticType->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8224) );
	}
	else if(  m_SpecialBoxInfo.cTargetTypeCode == SpecialBox::TargetTypeCode::CharacterAll || m_SpecialBoxInfo.cTargetTypeCode == SpecialBox::TargetTypeCode::CharacterSelect )
	{
		if( CDnActor::s_hLocalActor )
		{
			m_pStaticType->SetText( FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8225) , CDnActor::s_hLocalActor->GetName() ) );
		}
	}
}

void CDnSpecialBoxListDlg::OpenItemConfirmDlg()
{
	int nErrorMsg = RequestItemErrorHandler();
	if( ( nErrorMsg == ERROR_NONE || nErrorMsg == ERROR_ITEM_INSUFFICIENTY_JOBID ) == false )
	{
		return;
	}

	if( m_nReceiveType == SpecialBox::ReceiveTypeCode::All )
	{
		if( nErrorMsg == ERROR_ITEM_INSUFFICIENTY_JOBID )
		{
			GetInterface().MessageBox( 8212, MB_YESNO, MESSAGEBOX_REQEUST_SPECIALBOX, this ); 
		}
		else
		{
			GetInterface().MessageBox( 8213, MB_YESNO, MESSAGEBOX_REQEUST_SPECIALBOX, this ); 
		}
	}
	else if( m_nReceiveType = SpecialBox::ReceiveTypeCode::Select )
	{
		SListBoxItem *pSelectedItem = m_pSpecialBoxList->GetSelectedItem();
		if( pSelectedItem ) 
		{
			CDnSpecialBoxListItemDlg *pItemDlg = (CDnSpecialBoxListItemDlg*)pSelectedItem->pData;
			if( pItemDlg && pItemDlg->GetItem() )
			{
				m_pSpecialBoxConfirmDlg->SetItemInfo( m_nRewardID , pItemDlg->GetItem() );
				m_pSpecialBoxConfirmDlg->Show( true );
			}
		}
	}
}

int CDnSpecialBoxListDlg::RequestItemErrorHandler()
{
	CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	if( pInvenDlg ) 
	{
		bool bOtherJobItemExist = false;
		int nCurEmptySlotCount = pInvenDlg->GetEmptySlotCount();
		int nItemCount = 1;
		
		if( m_nReceiveType == SpecialBox::ReceiveTypeCode::All )
		{
			nItemCount = 0;

			for( int i=0; i<m_pSpecialBoxList->GetSize(); i++ )
			{
				CDnSpecialBoxListItemDlg *pItemDlg = m_pSpecialBoxList->GetItem<CDnSpecialBoxListItemDlg>(i);
				if( pItemDlg && pItemDlg->GetItem() )
				{
					if( pItemDlg->GetItem()->IsCashItem() == false )
					{
						nItemCount++;
					}

					if( bOtherJobItemExist == false && pItemDlg->GetItem()->IsNeedJob() == true && CDnActor::s_hLocalActor )
					{
						std::vector<int> nVecJobList;
						((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
						if( !pItemDlg->GetItem()->IsPermitPlayer( nVecJobList ) )
						{
							bOtherJobItemExist = true;
						}
					}
				}
			}
		}
		else if( m_nReceiveType = SpecialBox::ReceiveTypeCode::Select )
		{
			SListBoxItem *pSelectedItem = m_pSpecialBoxList->GetSelectedItem();
			if( pSelectedItem ) 
			{
				CDnSpecialBoxListItemDlg *pItemDlg = (CDnSpecialBoxListItemDlg*)pSelectedItem->pData;
				if( pItemDlg && pItemDlg->GetItem() )
				{
					if( pItemDlg->GetItem()->IsCashItem() == true )
						nItemCount = 0;
				}
			}
		}
		 
		if( nCurEmptySlotCount < nItemCount )
		{
			GetInterface().MessageBox( 1925, MB_OK );
			return ERROR_ITEM_INVENTORY_NOTENOUGH;
		}

		if( bOtherJobItemExist == true )
		{
			return ERROR_ITEM_INSUFFICIENTY_JOBID;
		}
	}

	return ERROR_NONE;
}

void CDnSpecialBoxListDlg::OnCompleteReceiveItem()
{
	CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
	if( pMainMenuDlg )
	{
		CDnEventReceiverTabDlg *pEventReceiverTabDlg = static_cast<CDnEventReceiverTabDlg*>( pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::EVENT_RECEIVER_DIALOG ) );
		if( pEventReceiverTabDlg )
		{
			pEventReceiverTabDlg->RemoveSpecialBoxInfoFromRewardID(m_nRewardID);
			pEventReceiverTabDlg->RefreshSpecialList();
			
			Show( false );
			m_pSpecialBoxConfirmDlg->Show( false );

			CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption2, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3548), textcolor::YELLOW );
		}
	}
}

#endif // PRE_SPECIALBOX