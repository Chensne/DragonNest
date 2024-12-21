#include "StdAfx.h"
#include "DnGuildWantedListTabDlg.h"
#include "DnGuildWantedListDlg.h"
#include "DnGuildWantedMyListDlg.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildWantedListTabDlg::CDnGuildWantedListTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback )
, m_pWantedTabButton(NULL)
, m_pJoinTabButton(NULL)
, m_pGuildWantedListDlg(NULL)
, m_pGuildWantedMyListDlg(NULL)
{
}

CDnGuildWantedListTabDlg::~CDnGuildWantedListTabDlg(void)
{
}

void CDnGuildWantedListTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWantedListTabDlg.ui" ).c_str(), bShow );
}

void CDnGuildWantedListTabDlg::InitialUpdate()
{
	m_pWantedTabButton = GetControl<CEtUIRadioButton>("ID_RBT_WANTEDLIST");
	m_pGuildWantedListDlg = new CDnGuildWantedListDlg( UI_TYPE_CHILD, this );
	m_pGuildWantedListDlg->Initialize( false );
	AddTabDialog( m_pWantedTabButton, m_pGuildWantedListDlg );

	m_pJoinTabButton = GetControl<CEtUIRadioButton>("ID_RBT_JOINLIST");
	m_pGuildWantedMyListDlg = new CDnGuildWantedMyListDlg( UI_TYPE_CHILD, this );
	m_pGuildWantedMyListDlg->Initialize( false );
	AddTabDialog( m_pJoinTabButton, m_pGuildWantedMyListDlg );

	SetCheckedTab( m_pWantedTabButton->GetTabID() );
}

void CDnGuildWantedListTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		if(IsCmdControl("ID_BT_SMALLHELP"))
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_GUILD);
		}
#endif
	}
	CEtUITabDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnGuildWantedListTabDlg::OnRecvGetGuildRecruitList( GuildRecruitSystem::SCGuildRecruitList *pPacket )
{
	if( m_pGuildWantedListDlg )
		m_pGuildWantedListDlg->OnRecvGetGuildRecruitList( pPacket );
}

void CDnGuildWantedListTabDlg::OnRecvGetGuildRecruitRequestCount( GuildRecruitSystem::SCGuildRecruitRequestCount *pPacket )
{
	if( m_pGuildWantedListDlg )
		m_pGuildWantedListDlg->OnRecvGetGuildRecruitRequestCount( pPacket );
	if( m_pGuildWantedMyListDlg )
		m_pGuildWantedMyListDlg->OnRecvGetGuildRecruitRequestCount( pPacket );
}

void CDnGuildWantedListTabDlg::OnRecvGetGuildRecruitMyList( GuildRecruitSystem::SCMyGuildRecruitList *pPacket )
{
	if( m_pGuildWantedListDlg )
		m_pGuildWantedListDlg->OnRecvGetGuildRecruitMyList( pPacket );
	if( m_pGuildWantedMyListDlg )
		m_pGuildWantedMyListDlg->OnRecvGetGuildRecruitMyList( pPacket );
}

void CDnGuildWantedListTabDlg::OnRecvGuildRecruitRequest( GuildRecruitSystem::SCGuildRecruitRequest *pPacket )
{
	if( m_pGuildWantedListDlg )
		m_pGuildWantedListDlg->OnRecvGuildRecruitRequest( pPacket );
}
