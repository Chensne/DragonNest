#include "stdafx.h"
#include "DnPVPTournamentFinalResultListDlg.h"
#include "DnGuildTask.h"
#include "DnInterface.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

#ifdef PRE_ADD_PVP_TOURNAMENT

CDnPVPTournamentFinalResultListDlg::CDnPVPTournamentFinalResultListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	:CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pRank = NULL;
	m_pJobIcon = NULL;
	m_pGuildIcon = NULL;
	m_pGuildName = NULL;
	m_pPVPRankIcon = NULL;
	m_pName = NULL;
	m_pKillCount = NULL;
	m_pDeathCount = NULL;
	m_pScore = NULL;
	m_pPVPXP = NULL;
	m_pPCCafe = NULL;
	m_pMedalIcon = NULL;
	m_pMedal = NULL;
}

CDnPVPTournamentFinalResultListDlg::~CDnPVPTournamentFinalResultListDlg(void)
{
	SAFE_RELEASE_SPTR(m_hPVPMedalIconImage);
}

void CDnPVPTournamentFinalResultListDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpTournamentResultListDlg.ui").c_str(), bShow);
}

void CDnPVPTournamentFinalResultListDlg::InitialUpdate()
{
	SAFE_RELEASE_SPTR(m_hPVPMedalIconImage);
	m_hPVPMedalIconImage = LoadResource(CEtResourceMng::GetInstance().GetFullName("Pvp_Medal.dds").c_str(), RT_TEXTURE);

	m_pRank = GetControl<CEtUIStatic>("ID_TEXT_RANK");
	m_pJobIcon = GetControl<CDnJobIconStatic>("ID_SCORE_CLASS");
	m_pGuildIcon = GetControl<CEtUITextureControl>("ID_TEXTURE_MARK");
	m_pGuildName = GetControl<CEtUIStatic>("ID_TEXT_GUILDNAME");
	m_pPVPRankIcon = GetControl<CEtUITextureControl>("ID_TEXTURE_RANK");
	m_pName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pKillCount = GetControl<CEtUIStatic>("ID_TEXT_KILLCOUND");
	m_pDeathCount = GetControl<CEtUIStatic>("ID_TEXT_DEATH");
	m_pScore = GetControl<CEtUIStatic>("ID_TEXT_SCORE");
	m_pPVPXP = GetControl<CEtUIStatic>("ID_TEXT_PVPXP");
	m_pPCCafe = GetControl<CEtUIStatic>("ID_TEXT_PC");
	m_pMedalIcon = GetControl<CEtUITextureControl>("ID_TEXTURE_MEDALICON");
	m_pMedal = GetControl<CEtUIStatic>("ID_TEXT_MEDAL");
}

void CDnPVPTournamentFinalResultListDlg::SetInfo(const SFinalReportUserInfo& info, int rank)
{
	std::wstring msg = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 840), rank); // UISTRING : %d위
	m_pRank->SetText(msg.c_str());

	m_pJobIcon->SetIconID(info.cJobClassID);

	if (info.GuildSelfView.IsSet())
	{
		// 길드명
		m_pGuildName->SetText(info.GuildSelfView.wszGuildName);

		const TGuildView &GuildView = info.GuildSelfView;
		if (CDnGuildTask::IsActive() && GetGuildTask().IsShowGuildMark(GuildView))
		{
			EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture(GuildView);
			m_pGuildIcon->SetTexture(hGuildMark);
			m_pGuildIcon->Show(true);
		}
	}

	int iIconW,iIconH;
	int iU,iV;
	iIconW = GetInterface().GeticonWidth();
	iIconH = GetInterface().GeticonHeight();
	if (GetInterface().ConvertPVPGradeToUV(info.cPVPlevel, iU, iV))
	{
		m_pPVPRankIcon->SetTexture(GetInterface().GetPVPIconTex(), iU, iV, iIconW, iIconH);
		m_pPVPRankIcon->Show(true);
	}
	
	m_pName->SetText(info.wszUserName.c_str());
	m_pKillCount->SetInt64ToText(info.uiKOCount);
	m_pDeathCount->SetInt64ToText(info.uiKObyCount);
	m_pScore->SetInt64ToText(info.uiTotalScore);
	m_pPVPXP->SetInt64ToText(info.uiXP);
	m_pPCCafe->ClearText();
	m_pMedalIcon->SetTexture(m_hPVPMedalIconImage, 0, 0, PVP_MEDAL_ICON_XSIZE, PVP_MEDAL_ICON_YSIZE);

	msg = FormatW(L"X %d", info.uiMedal);
	m_pMedal->SetText(msg.c_str());
}

#ifdef PRE_PVP_GAMBLEROOM

CDnPVPTournamentGambleFinalResultListDlg::CDnPVPTournamentGambleFinalResultListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pRank = NULL;
	m_pLevel = NULL;
	m_pJobIcon = NULL;
	m_pGuildName = NULL;
	m_pPVPRankIcon = NULL;
	m_pName = NULL;
	m_pKillCount = NULL;
	m_pDeathCount = NULL;
	m_pPVPXP = NULL;
	m_pMedalIcon = NULL;
	m_pMedal = NULL;
}

CDnPVPTournamentGambleFinalResultListDlg::~CDnPVPTournamentGambleFinalResultListDlg( void )
{
	SAFE_RELEASE_SPTR( m_hPVPMedalIconImage );
}

void CDnPVPTournamentGambleFinalResultListDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpTournamentResultList_GDlg.ui" ).c_str(), bShow );
}

void CDnPVPTournamentGambleFinalResultListDlg::InitialUpdate()
{
	SAFE_RELEASE_SPTR( m_hPVPMedalIconImage );
	m_hPVPMedalIconImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Pvp_Medal.dds" ).c_str(), RT_TEXTURE );

	m_pRank = GetControl<CEtUIStatic>( "ID_TEXT_RANK" );
	m_pLevel = GetControl<CEtUIStatic>( "ID_TEXT_LEVEL" );
	m_pJobIcon = GetControl<CDnJobIconStatic>( "ID_SCORE_CLASS" );
	m_pGuildName = GetControl<CEtUIStatic>( "ID_TEXT_GUILDNAME" );
	m_pPVPRankIcon = GetControl<CEtUITextureControl>( "ID_TEXTURE_RANK" );
	m_pName = GetControl<CEtUIStatic>( "ID_TEXT_NAME" );
	m_pKillCount = GetControl<CEtUIStatic>( "ID_TEXT_KILLCOUND" );
	m_pDeathCount = GetControl<CEtUIStatic>( "ID_TEXT_DEATH" );
	m_pPVPXP = GetControl<CEtUIStatic>( "ID_TEXT_PVPXP" );
	m_pMedalIcon = GetControl<CEtUITextureControl>( "ID_TEXTURE_MEDALICON" );
	m_pMedal = GetControl<CEtUIStatic>( "ID_TEXT_MEDAL" );
}

void CDnPVPTournamentGambleFinalResultListDlg::SetInfo( const SFinalReportUserInfo& info, int rank )
{
	std::wstring msg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 840 ), rank ); // UISTRING : %d위
	m_pRank->SetText( msg.c_str() );

	m_pJobIcon->SetIconID( info.cJobClassID );

	if( info.GuildSelfView.IsSet() )
	{
		m_pGuildName->SetText( info.GuildSelfView.wszGuildName );
	}

	int iIconW,iIconH;
	int iU,iV;
	iIconW = GetInterface().GeticonWidth();
	iIconH = GetInterface().GeticonHeight();
	if( GetInterface().ConvertPVPGradeToUV( info.cPVPlevel, iU, iV ) )
	{
		m_pPVPRankIcon->SetTexture( GetInterface().GetPVPIconTex(), iU, iV, iIconW, iIconH );
		m_pPVPRankIcon->Show( true );
	}

	std::wstring temp = FormatW( L"LV %d", info.nLevel );
	m_pLevel->SetText( temp.c_str() );
	m_pName->SetText( info.wszUserName.c_str() );
	m_pKillCount->SetInt64ToText( info.uiKOCount );
	m_pDeathCount->SetInt64ToText( info.uiKObyCount );
	m_pPVPXP->SetInt64ToText( info.uiXP );
	m_pMedalIcon->SetTexture( m_hPVPMedalIconImage, 0, 0, PVP_MEDAL_ICON_XSIZE, PVP_MEDAL_ICON_YSIZE );

	msg = FormatW( L"X %d", info.uiMedal );
	m_pMedal->SetText( msg.c_str() );
}

#endif // PRE_PVP_GAMBLEROOM

#endif // PRE_ADD_PVP_TOURNAMENT