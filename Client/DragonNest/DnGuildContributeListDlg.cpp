#include "Stdafx.h"

#ifdef PRE_ADD_GUILD_CONTRIBUTION
#include "DnGuildContributeListDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGuildContributionListDlg::CDnGuildContributionListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
	m_pStaticRank = NULL;
	m_pStaticJobIcon = NULL;
	m_pStaticName = NULL;
	m_pStaticPoint = NULL;
}

CDnGuildContributionListDlg::~CDnGuildContributionListDlg()
{

}

void CDnGuildContributionListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildContributeListDlg.ui" ).c_str(), bShow );
}

void CDnGuildContributionListDlg::InitialUpdate()
{
	m_pStaticRank = GetControl<CEtUIStatic>("ID_TEXT_RANK");
	m_pStaticName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pStaticPoint = GetControl<CEtUIStatic>("ID_TEXT_EXP");
	m_pStaticJobIcon = GetControl<CDnJobIconStatic>("ID_STATIC_CLASS");
}

void CDnGuildContributionListDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;
	
	CEtUIDialog::Show( bShow );
}

void CDnGuildContributionListDlg::SetInfo( int nRank, int nJobIcon, WCHAR* wszName, int nPoint )
{
	m_pStaticName->SetText( wszName );
	m_pStaticRank->SetIntToText( nRank );
	m_pStaticPoint->SetIntToText( nPoint );

	m_pStaticJobIcon->SetIconID( nJobIcon, true);
	m_pStaticJobIcon->Show( true );
}	

#endif