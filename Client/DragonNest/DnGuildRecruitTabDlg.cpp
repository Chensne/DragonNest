#include "StdAfx.h"
#include "DnGuildRecruitTabDlg.h"
#include "DnGuildRecruitDlg.h"
#include "DnGuildRequestDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildRecruitTabDlg::CDnGuildRecruitTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback )
, m_pWantedTabButton(NULL)
, m_pRequestTabButton(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
, m_pGuildRecruitDlg(NULL)
, m_pGuildRequestDlg(NULL)
{
}

CDnGuildRecruitTabDlg::~CDnGuildRecruitTabDlg(void)
{
}

void CDnGuildRecruitTabDlg::Initialize( bool bShow )
{
#ifdef _ADD_RENEWED_GUILDUI
	CEtUITabDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("Wanted_TabDlg.ui").c_str(), bShow);
#else
	CEtUITabDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWantedTabDlg.ui" ).c_str(), bShow );
#endif
}

void CDnGuildRecruitTabDlg::InitialUpdate()
{
	m_pWantedTabButton = GetControl<CEtUIRadioButton>("ID_RBT_WANTED");
	m_pGuildRecruitDlg = new CDnGuildRecruitDlg( UI_TYPE_CHILD, this );
	m_pGuildRecruitDlg->Initialize( false );
	AddTabDialog( m_pWantedTabButton, m_pGuildRecruitDlg );

	m_pRequestTabButton = GetControl<CEtUIRadioButton>("ID_RBT_REQUEST");
	m_pGuildRequestDlg = new CDnGuildRequestDlg( UI_TYPE_CHILD, this );
	m_pGuildRequestDlg->Initialize( false );
	AddTabDialog( m_pRequestTabButton, m_pGuildRequestDlg );

	SetCheckedTab( m_pWantedTabButton->GetTabID() );
}

void CDnGuildRecruitTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
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

void CDnGuildRecruitTabDlg::OnRecvGuildRecruitRegisterInfo( GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket )
{
	if( !m_pGuildRecruitDlg ) return;
	m_pGuildRecruitDlg->OnRecvGuildRecruitRegisterInfo( pPacket );
}

void CDnGuildRecruitTabDlg::OnRecvGuildRecruitCharacter( GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket )
{
	if( !m_pGuildRequestDlg ) return;
	m_pGuildRequestDlg->OnRecvGuildRecruitCharacter( pPacket );
}

void CDnGuildRecruitTabDlg::OnRecvGuildRecruitAcceptResult( GuildRecruitSystem::SCGuildRecruitAccept *pPacket )
{
	if( !m_pGuildRequestDlg ) return;
	m_pGuildRequestDlg->OnRecvGuildRecruitAcceptResult( pPacket );
}