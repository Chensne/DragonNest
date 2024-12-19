#include "StdAfx.h"
#include "DnGuildInfoDlg.h"
#include "DnGuildTask.h"
#include "DnInterface.h"
#include "DnGuildHistoryDlg.h"
#include "DnGuildLevelDlg.h"
#include "DnGuildRewardDlg.h"

#ifdef PRE_ADD_GUILD_CONTRIBUTION
#include "DnGuildContributionRankDlg.h"
#endif 

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildInfoDlg::CDnGuildInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback )
, m_pHistoryTabButton(NULL)
, m_pLevelTabButton(NULL)
, m_pRewardTabButton(NULL)
, m_pGuildHistoryDlg(NULL)
, m_pGuildLevelDlg(NULL)
, m_pGuildRewardDlg(NULL)
#ifdef PRE_ADD_GUILD_CONTRIBUTION
, m_pGuildRankDlg(NULL)
#endif 
{
}

CDnGuildInfoDlg::~CDnGuildInfoDlg(void)
{
#ifdef PRE_ADD_GUILD_CONTRIBUTION
	SAFE_DELETE( m_pGuildRankDlg );
#endif 
}

void CDnGuildInfoDlg::Initialize( bool bShow )
{
	CEtUITabDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildDescDlg.ui" ).c_str(), bShow );
}

void CDnGuildInfoDlg::InitialUpdate()
{
	m_pHistoryTabButton = GetControl<CEtUIRadioButton>("ID_RBT0");
	m_pGuildHistoryDlg = new CDnGuildHistoryDlg( UI_TYPE_CHILD, this );
	m_pGuildHistoryDlg->Initialize( false );
	AddTabDialog( m_pHistoryTabButton, m_pGuildHistoryDlg );

	m_pLevelTabButton = GetControl<CEtUIRadioButton>("ID_RBT1");
	m_pGuildLevelDlg = new CDnGuildLevelDlg( UI_TYPE_CHILD, this );
	m_pGuildLevelDlg->Initialize( false );
	AddTabDialog( m_pLevelTabButton, m_pGuildLevelDlg );

	m_pRewardTabButton = GetControl<CEtUIRadioButton>("ID_RBT2");
	m_pGuildRewardDlg = new CDnGuildRewardDlg( UI_TYPE_CHILD, this );
	m_pGuildRewardDlg->Initialize( false );
	AddTabDialog( m_pRewardTabButton, m_pGuildRewardDlg );

#ifdef PRE_ADD_GUILD_CONTRIBUTION
	m_pGuildRankDlg = new CDnGuildContributionRankDlg( UI_TYPE_MODAL );
	m_pGuildRankDlg->Initialize( false );
#endif 
	SetCheckedTab( m_pLevelTabButton->GetTabID() );
}

void CDnGuildInfoDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}

	CEtUITabDialog::Show( bShow );
}

void CDnGuildInfoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show( false );
			return;
		}
	}


	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildInfoDlg::OnRecvGetGuildHistoryList( SCGetGuildHistoryList *pPacket )
{
	if( !m_pGuildHistoryDlg ) return;
	m_pGuildHistoryDlg->OnRecvGetGuildHistoryList( pPacket );
}

void CDnGuildInfoDlg::UpdateGuildRewardInfo()
{
	if (m_pGuildRewardDlg)
		m_pGuildRewardDlg->ShowRewardList();
}

void CDnGuildInfoDlg::CloseBuyGuildRewardConfirmDlg()
{
	if (m_pGuildRewardDlg)
		m_pGuildRewardDlg->CloseBuyGuildRewardConfirmDlg();
}

#ifdef PRE_ADD_GUILD_CONTRIBUTION
void CDnGuildInfoDlg::OnRecvGetGuildContributionPoint( GuildContribution::SCGuildContributionPoint* pPacket )
{
	if( ! m_pGuildLevelDlg ) return;
	m_pGuildLevelDlg->SetGuildContributinPoint( pPacket->nTotalContributionPoint, pPacket->nWeeklyContributionPoint );
}

void CDnGuildInfoDlg::OnRecvGetGuildContributionRankList( GuildContribution::SCGuildContributionRank* pPacket )
{
	if( ! m_pGuildRankDlg ) return;
	m_pGuildRankDlg->SetGuildRankList( pPacket->Data, pPacket->nCount );
	m_pGuildRankDlg->Show( pPacket->nCount > 0 );

}	

#endif 