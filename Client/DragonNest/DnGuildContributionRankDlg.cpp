#include "Stdafx.h"

#ifdef PRE_ADD_GUILD_CONTRIBUTION
#include "DnGuildContributionRankDlg.h"
#include "DnGuildContributeListDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGuildContributionRankDlg::CDnGuildContributionRankDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{

}

CDnGuildContributionRankDlg::~CDnGuildContributionRankDlg()
{
	
}

void CDnGuildContributionRankDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildContributeDlg.ui" ).c_str(), bShow );
}

void CDnGuildContributionRankDlg::InitialUpdate()
{
	m_pStaticWeeklyRank = GetControl<CEtUIStatic>("ID_TEXT0");
	m_pListBoxRankList = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");
}

void CDnGuildContributionRankDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnGuildContributionRankDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{
			Show( false );
		}
	}
	
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildContributionRankDlg::SetGuildRankList( GuildContribution::TGuildContributionRankingData* pData, int nSize )
{
	m_pListBoxRankList->RemoveAllItems();
	
	if( nSize == 0 )
	{
		CDnInterface::GetInstance().MessageBox(  GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3885 ) );
		return;
	}

	std::list< GuildContribution::TGuildContributionRankingData* > RankList;
	int nPos = 0;

	for( int i = 0; i < nSize; ++i )	
	{
		GuildContribution::TGuildContributionRankingData* pGuildRankInfo = &pData[i];
		
		if( __wcsicmp_l( CDnActor::s_hLocalActor->GetName(), pGuildRankInfo->wszCharacterName ) == 0 && nSize > 5 )
			RankList.push_front( pGuildRankInfo );
		else 
			RankList.push_back( pGuildRankInfo );
	}

	std::list< GuildContribution::TGuildContributionRankingData* >::iterator iter = RankList.begin();
	for(  ; iter != RankList.end(); ++iter  )
	{
		GuildContribution::TGuildContributionRankingData* pGuildRankInfo = (*iter);
		CDnGuildContributionListDlg* pGuildList = m_pListBoxRankList->AddItem<CDnGuildContributionListDlg>( nPos++ );
		if( pGuildList )
		{
			pGuildList->SetInfo( pGuildRankInfo->nRank, pGuildRankInfo->cJobCode, pGuildRankInfo->wszCharacterName, pGuildRankInfo->nWeeklyGuildContributionPoint );
			pGuildList->Show( true );
		}		
	}
	
}



#endif 