#include "StdAfx.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"
#include "DnSpecialBoxListItemDlg.h"
#include "DnInterfaceString.h"

using namespace DN_INTERFACE::STRING;

#ifdef PRE_SPECIALBOX

CDnSpecialBoxListItemDlg::CDnSpecialBoxListItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pItemSlotButton = NULL;
	m_pStaticItemName = NULL;
	m_pItem = NULL;
}

CDnSpecialBoxListItemDlg::~CDnSpecialBoxListItemDlg(void)
{
	SAFE_DELETE(m_pItem);
}

void CDnSpecialBoxListItemDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "KeepGiftBoxListDlg.ui" ).c_str(), bShow );
}

void CDnSpecialBoxListItemDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM");
	m_pItemSlotButton->SetSlotType( ST_SPECIALBOX_REWARD );
	m_pStaticItemName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
}

void CDnSpecialBoxListItemDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
}

void CDnSpecialBoxListItemDlg::Show( bool bShow )
{ 
	CDnCustomDlg::Show( bShow );
}

void CDnSpecialBoxListItemDlg::SetRewardItemInfo( TSpecialBoxItemInfo *pInfo )
{
	if( pInfo )
	{
		m_pItemSlotButton->ResetSlot();
		SAFE_DELETE(m_pItem);

		if( pInfo->bCashItem == false )
		{
			m_pItem = GetItemTask().CreateItem( pInfo->RewardItem );
			if( m_pItem )
			{
				m_pItemSlotButton->SetItem( m_pItem , CDnSlotButton::ITEM_ORIGINAL_COUNT );
				m_pStaticItemName->SetText( m_pItem->GetName() );
				m_pStaticItemName->SetTextColor( ITEM::RANK_2_COLOR( m_pItem->GetItemRank() ) );
			}
			else
			{
				SAFE_DELETE( m_pItem );
			}
		}
		else if( pInfo->bCashItem == true )
		{
			const SCashShopItemInfo* pCashInfo = GetCashShopTask().GetItemInfo( pInfo->RewardItem.nItemID );
			if( pCashInfo ) 
			{
				TItemInfo Info;
				if( CDnItem::MakeItemInfo( pCashInfo->presentItemId, pCashInfo->count, Info ) ) 
				{
					m_pItem = CDnItemTask::GetInstance().CreateItem( Info );
					m_pItem->SetCashItemSN( pCashInfo->sn );

					if( m_pItem )
					{
						std::wstring wszResultName = pCashInfo->nameString;
						std::wstring wszRemainData;
						if( pCashInfo->period > 0 )
						{
							wszRemainData += FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4613 ), pCashInfo->period );
							wszResultName += L" ";
							wszResultName += wszRemainData;
						}
						
				
						m_pItemSlotButton->SetItem( m_pItem , CDnSlotButton::ITEM_ORIGINAL_COUNT );
						m_pStaticItemName->SetText( wszResultName );
						m_pStaticItemName->SetTextColor( ITEM::RANK_2_COLOR( m_pItem->GetItemRank() ) );
						m_pStaticItemName->SetPartColorText( wszRemainData , textcolor::WHITE , D3DCOLOR_ARGB(0, 255, 255, 255) );
					}
					else
					{
						SAFE_DELETE( m_pItem );
					}
				}
			}
		}
	}
}

CDnItem *CDnSpecialBoxListItemDlg::GetItem()
{
	return m_pItem; 
}

#endif // PRE_SPECIALBOX