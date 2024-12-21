#include "StdAfx.h"


#ifdef PRE_ADD_DONATION


#include <stdlib.h>


#include "DnItemTask.h"
#include "DnContributionTask.h"

#include "DnItemSlotButton.h"
#include "DnJobIconStatic.h"

#include "DnContributionRankListItemDlg.h"

CDnContributionRankListItemDlg::CDnContributionRankListItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pStaticRank( NULL )
, m_pStaticGuild( NULL )
, m_pStaticID( NULL )
, m_pStaticGold( NULL )
, m_pItemSlotBtn( NULL )
, m_pStaticJob( NULL )
{
}

void CDnContributionRankListItemDlg::ReleaseDlg()
{

}


void CDnContributionRankListItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("DonationListDlg.ui").c_str(), bShow );
}


void CDnContributionRankListItemDlg::InitialUpdate()
{
	m_pStaticRank = GetControl<CEtUIStatic>("ID_TEXT_RANK");
	m_pStaticJob = GetControl<CDnJobIconStatic>("ID_STATIC_CLASS");
	m_pStaticGuild = GetControl<CEtUIStatic>("ID_TEXT_GUILDNAME");
	m_pStaticID = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD1");
	/*m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER1");
	m_pStaticCopper = GetControl<CEtUIStatic>("ID_BRONZE1");*/
	m_pItemSlotBtn = GetControl<CDnItemSlotButton>("ID_BT_ITEM1");	
}


void CDnContributionRankListItemDlg::SetData( const SContributionRanker * aRanker, int nItemID )
{
	wchar_t buf[64];
	swprintf_s( buf, 64, GetEtUIXML().GetUIString(CEtUIXML::idCategory1,809), aRanker->nRank );	
	m_pStaticRank->SetText( std::wstring(buf) );
	m_pStaticGuild->SetText( aRanker->strGuildName ); 
	m_pStaticID->SetText( aRanker->strID );
	m_pStaticGold->SetText( GoldToMoneyString( aRanker->nGold ) );	
	/*m_pStaticSilver->SetText( std::wstring(::_i64tow(aRanker->nGold%10000/100,buf,10)) );
	m_pStaticCopper->SetText( std::wstring(::_i64tow(aRanker->nGold%100,buf,10)) );*/
	m_pStaticJob->SetIconID( aRanker->nJob );	

	// 보상아이템.
	TItemInfo info;
	if( CDnItem::MakeItemInfo( nItemID, 1, info ) )
	{
		CDnItem * pItem = CDnItemTask::GetInstance().CreateItem( info );
		if( pItem )
			m_pItemSlotBtn->SetItem( pItem, info.Item.wCount );
	}

}

#endif