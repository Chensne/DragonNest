#include "StdAfx.h"
#include "DnGuildBaseDlg.h"
#include "DnPlayerActor.h"
#include "DnGuildDlg.h"
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#include "DnGuildWantedListTabDlg.h"
#else
#include "DnGuildWantedListDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_GUILD_EASYSYSTEM

CDnGuildBaseDlg::CDnGuildBaseDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	m_pGuildDlg = NULL;
	m_pGuildWantedListTabDlg = NULL;
}

CDnGuildBaseDlg::~CDnGuildBaseDlg(void)
{
	SAFE_DELETE( m_pGuildDlg );
	SAFE_DELETE( m_pGuildWantedListTabDlg );
}

void CDnGuildBaseDlg::InitialUpdate()
{
	m_pGuildDlg = new CDnGuildDlg( UI_TYPE_CHILD, this );
	m_pGuildDlg->Initialize( false );

	m_pGuildWantedListTabDlg = new CDnGuildWantedListTabDlg( UI_TYPE_CHILD, this );
	m_pGuildWantedListTabDlg->Initialize( false );
}

void CDnGuildBaseDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		bool bJoinGuild = pPlayer->IsJoinGuild();
		m_pGuildDlg->Show( bJoinGuild );
		m_pGuildWantedListTabDlg->Show( !bJoinGuild );
	}
	else
	{
		m_pGuildDlg->Show( false );
		m_pGuildWantedListTabDlg->Show( false );
	}

	CEtUIDialog::Show( bShow );
}

#else

CDnGuildBaseDlg::CDnGuildBaseDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	m_pGuildDlg = NULL;
	m_pGuildWantedListDlg = NULL;
}

CDnGuildBaseDlg::~CDnGuildBaseDlg(void)
{
	SAFE_DELETE( m_pGuildDlg );
	SAFE_DELETE( m_pGuildWantedListDlg );
}

void CDnGuildBaseDlg::InitialUpdate()
{
	m_pGuildDlg = new CDnGuildDlg( UI_TYPE_CHILD, this );
	m_pGuildDlg->Initialize( false );

	m_pGuildWantedListDlg = new CDnGuildWantedListDlg( UI_TYPE_CHILD, this );
	m_pGuildWantedListDlg->Initialize( false );
}

void CDnGuildBaseDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		bool bJoinGuild = pPlayer->IsJoinGuild();
		m_pGuildDlg->Show( bJoinGuild );
		m_pGuildWantedListDlg->Show( !bJoinGuild );
	}
	else
	{
		m_pGuildDlg->Show( false );
		m_pGuildWantedListDlg->Show( false );
	}

	CEtUIDialog::Show( bShow );
}

#endif