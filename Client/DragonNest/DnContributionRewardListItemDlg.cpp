#include "Stdafx.h"


#ifdef PRE_ADD_DONATION


#include "DnItemTask.h"
#include "DnContributionTask.h"

#include "DnItemSlotButton.h"

#include "DnContributionRewardListItemDlg.h"

namespace NS_Contribution
{
	int g_nItemSlotSize = 5;
}

CDnContributionRewardListItemDlg::CDnContributionRewardListItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pRadioDay( NULL )
, m_pRadioMonth( NULL )
, m_pStaticRank( NULL )
{
}

void CDnContributionRewardListItemDlg::ReleaseDlg()
{
	m_vItemSlots.clear();
}

// Override - CEtUIDialog //	
void CDnContributionRewardListItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DonationRewardListDlg.ui" ).c_str(), bShow );
}


void CDnContributionRewardListItemDlg::InitialUpdate()
{
	// Static 순위.
	m_pStaticRank = GetControl<CEtUIStatic>( "ID_TEXT_RANK" );
	
	// ItemSlot 보상템.
	char str[32] = {0,};
	m_vItemSlots.reserve( NS_Contribution::g_nItemSlotSize );
	for( int i=0; i<NS_Contribution::g_nItemSlotSize; ++i )
	{
		sprintf_s( str, 32, "ID_BT_ITEM%d", i );
		m_vItemSlots.push_back( GetControl<CDnItemSlotButton>( str ) );
	}
}


void CDnContributionRewardListItemDlg::SetData( const struct SContributionRewardItems & aItems )
{
	wchar_t buf[64];
	const std::wstring & strRank = m_pStaticRank->GetText();
	swprintf_s( buf, 64, strRank.c_str(), aItems.nRank );
	m_pStaticRank->SetText( std::wstring(buf) );

	// 보상아이템.
	TItemInfo info;
	for( int i=0; i<NS_Contribution::g_nItemSlotSize; ++i )
	{
		if( aItems.nItemCnt[ i ] <= 0 )
			continue;

		if( CDnItem::MakeItemInfo( aItems.nItemID[ i ], aItems.nItemCnt[ i ], info ) )
		{
			CDnItem * pItem = CDnItemTask::GetInstance().CreateItem( info );
			if( pItem )
				m_vItemSlots[ i ]->SetItem( pItem, info.Item.wCount );
		}
	}
}


#endif