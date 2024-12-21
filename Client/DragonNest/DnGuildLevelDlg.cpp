#include "StdAfx.h"
#include "DnGuildLevelDlg.h"
#include "DnGuildTask.h"
#include "DnInterfaceString.h"
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#include "DnInterface.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildLevelDlg::CDnGuildLevelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticLevel(NULL)
, m_pStaticPercent(NULL)
, m_pProgressBar(NULL)
, m_pStaticPoint(NULL)
, m_pStaticStage(NULL)
, m_pStaticGuildMission(NULL)
, m_pStaticGuildWar(NULL)
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
, m_pEditBoxHomepage(NULL)
, m_pStaticHomepage(NULL)
, m_pButtonHomepage(NULL)
#endif
#endif

#ifdef PRE_ADD_GUILD_CONTRIBUTION
, m_nToTalContributionPoint(0)
, m_nWeeklyContributionPoint(0)
#endif 
{
}

CDnGuildLevelDlg::~CDnGuildLevelDlg(void)
{
}

void CDnGuildLevelDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildLevelDlg.ui" ).c_str(), bShow );
}

void CDnGuildLevelDlg::InitialUpdate()
{
	m_pStaticLevel = GetControl<CEtUIStatic>("ID_TEXT_LEVEL");
	m_pStaticPercent = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	m_pProgressBar = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR3");
	m_pStaticPoint = GetControl<CEtUIStatic>("ID_TEXT_GUILDEXP");
	m_pStaticStage = GetControl<CEtUIStatic>("ID_TEXT_STAGEGUILDEXP");
	m_pStaticGuildMission = GetControl<CEtUIStatic>("ID_TEXT_MISSIONGUILDEXP");
	m_pStaticGuildWar = GetControl<CEtUIStatic>("ID_TEXT_BONUSGUILDEXP");
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	m_pEditBoxHomepage = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_HOMEPAGE");
	m_pStaticHomepage = GetControl<CEtUIStatic>("ID_TEXT_HOMEPAGE");
	m_pButtonHomepage = GetControl<CEtUIButton>("ID_BT_OK");
#endif
#endif

#ifdef PRE_ADD_GUILD_CONTRIBUTION
	m_pStaticWeeklyContribution = GetControl<CEtUIStatic>("ID_TEXT6");
	m_pStaticCurrentContribution = GetControl<CEtUIStatic>("ID_TEXT7");
	m_pStaticWeeklyCtriText = GetControl<CEtUIStatic>("ID_TEXT_WEEKCONTRIBUTE");
	m_pStaticCurrentCtriText = GetControl<CEtUIStatic>("ID_TEXT_ALLCONTRIBUTE");
#endif 
}

void CDnGuildLevelDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RefreshGuildLevel();
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
		RefreshGuildInfo();
#endif
#endif
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildLevelDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_CLOSE") )
		{
			if( m_pParentDialog ) m_pParentDialog->Show( false );
		}
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
		else if( IsCmdControl("ID_BT_OK") )
		{
			std::wstring szStr = m_pEditBoxHomepage->GetText();
#if defined(PRE_ADD_ENGLISH_STRING)
			std::vector<std::wstring> &vecWhiteList = GetEtUIXML().GetWhiteList();
			WCHAR *wszTempWhiteList[6] = { L"http", L"http:", L"http://", L"www", L".net", L".com" };
			for( int i=0; i<6; i++ )
				vecWhiteList.push_back( wszTempWhiteList[i] );
#endif
			if( DN_INTERFACE::UTIL::CheckChat( szStr ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}
#if defined(PRE_ADD_ENGLISH_STRING)
			for( int i=0; i<6; i++ )
				vecWhiteList.pop_back();
#endif

			if( m_pEditBoxHomepage->GetTextLength() >= 1 && m_pEditBoxHomepage->GetTextLength() <= 7 )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3871 ), MB_OK, 0, this );
				return;
			}

			if( m_pEditBoxHomepage->GetTextLength() > 7 )
			{
				bool bCheckHttp = true;
				if( szStr[0] != 'h' && szStr[0] != 'H' ) bCheckHttp = false;
				if( szStr[1] != 't' && szStr[1] != 'T' ) bCheckHttp = false;
				if( szStr[2] != 't' && szStr[2] != 'T' ) bCheckHttp = false;
				if( szStr[3] != 'p' && szStr[3] != 'P' ) bCheckHttp = false;
				if( szStr[4] != ':' ) bCheckHttp = false;
				if( szStr[5] != '/' ) bCheckHttp = false;
				if( szStr[6] != '/' ) bCheckHttp = false;
				if( bCheckHttp == false )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3871 ), MB_OK, 0, this );
					return;
				}
			}

			if( wcscmp( m_pEditBoxHomepage->GetText(), GetGuildTask().GetGuildInfo()->wszGuildHomePage ) != 0 )
				GetGuildTask().RequestChangeGuildInfo( GUILDUPDATE_TYPE_HOMEPAGE, 0, 0, 0, m_pEditBoxHomepage->GetText() );
		}
#endif
#endif
#ifdef PRE_ADD_GUILD_CONTRIBUTION
		else if( IsCmdControl("ID_BT_RANK") )
		{
			GetGuildTask().RequsetGuildRankList(); 		
		}
#endif 
	}
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	else if( nCommand == EVENT_EDITBOX_FOCUS )
	{
		m_pStaticHomepage->Show( false );
		if( m_pEditBoxHomepage->GetTextLength() == 0 )
			m_pEditBoxHomepage->SetText( L"http://" );
	}
	else if( nCommand == EVENT_EDITBOX_RELEASEFOCUS )
	{
		std::wstring wszHomepage = m_pEditBoxHomepage->GetText();
		if( wszHomepage == L"http://" )
			m_pEditBoxHomepage->ClearText();
		if( m_pEditBoxHomepage->GetTextLength() == 0 )
			m_pStaticHomepage->Show( true );
	}
#endif
#endif

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
void CDnGuildLevelDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
			RequestFocus( m_pEditBoxHomepage );
#endif
		}
	}
}
#endif

void CDnGuildLevelDlg::RefreshGuildLevel()
{
	if( !CDnGuildTask::IsActive() ) return;
	TGuild *pGuild = GetGuildTask().GetGuildInfo();
	if( !pGuild->IsSet() ) return;

	CDnGuildTask::SGuildLevelInfo *pLevelInfo = GetGuildTask().GetGuildLevelInfoFromGuildExp( pGuild->iTotalGuildExp );
	if( !pLevelInfo ) return;

	WCHAR wszTemp[256] = {0,};
	swprintf_s( wszTemp, _countof(wszTemp), L"%d", pGuild->wGuildLevel );
	m_pStaticLevel->SetText( wszTemp );

	bool bMaxLevel = false;
	CDnGuildTask::SGuildLevelInfo *pNextLevelInfo = GetGuildTask().GetGuildLevelInfo( pLevelInfo->nLevel + 1 );
	if( !pNextLevelInfo ) bMaxLevel = true;

	if( bMaxLevel )
	{
		m_pProgressBar->SetProgress( 100.0f );
		m_pProgressBar->SetTooltipText( L"" );
		m_pStaticPercent->SetTooltipText( L"" );
		m_pStaticPercent->SetText( L"Max" );
		swprintf_s( wszTemp, _countof(wszTemp), L"%d", 0 );
		m_pStaticPoint->SetText( wszTemp );
	}
	else
	{
		float fRate = (float)(pGuild->iTotalGuildExp - pLevelInfo->nReqGuildPoint) / (float)(pNextLevelInfo->nReqGuildPoint - pLevelInfo->nReqGuildPoint) * 100.0f;
		m_pProgressBar->SetProgress( fRate );
		swprintf_s( wszTemp, _countof(wszTemp), L"%s / %s", DN_INTERFACE::UTIL::GetAddCommaString( pGuild->iTotalGuildExp - pLevelInfo->nReqGuildPoint ).c_str(), DN_INTERFACE::UTIL::GetAddCommaString( pNextLevelInfo->nReqGuildPoint - pLevelInfo->nReqGuildPoint ).c_str() );
		m_pProgressBar->SetTooltipText( wszTemp );
		m_pStaticPercent->SetTooltipText( wszTemp );
		swprintf_s( wszTemp, _countof(wszTemp), L"%.2f%%", fRate );
		m_pStaticPercent->SetText( wszTemp );
		swprintf_s( wszTemp, _countof(wszTemp), L"%s", DN_INTERFACE::UTIL::GetAddCommaString( pNextLevelInfo->nReqGuildPoint - pGuild->iTotalGuildExp ).c_str() );
		m_pStaticPoint->SetText( wszTemp );
	}

	swprintf_s( wszTemp, _countof(wszTemp), L"%s / %s", DN_INTERFACE::UTIL::GetAddCommaString( pGuild->iDailyStagePoint ).c_str(), DN_INTERFACE::UTIL::GetAddCommaString( pLevelInfo->nDailyGuildPointLimit ).c_str() );
	m_pStaticStage->SetText( wszTemp );
	swprintf_s( wszTemp, _countof(wszTemp), L"%s / %s", DN_INTERFACE::UTIL::GetAddCommaString( pGuild->iDailyMissionPoint ).c_str(), DN_INTERFACE::UTIL::GetAddCommaString( pLevelInfo->nDailyMissionPointLimit ).c_str() );
	m_pStaticGuildMission->SetText( wszTemp );
	swprintf_s( wszTemp, _countof(wszTemp), L"%s", DN_INTERFACE::UTIL::GetAddCommaString( pGuild->iWarPoint ).c_str() );
	m_pStaticGuildWar->SetText( wszTemp );

#ifdef PRE_ADD_GUILD_CONTRIBUTION
	swprintf_s( wszTemp, _countof( wszTemp ), L"%d", m_nWeeklyContributionPoint );
	m_pStaticWeeklyCtriText->SetText( wszTemp );
	swprintf_s( wszTemp, _countof( wszTemp ), L"%d", m_nToTalContributionPoint );
	m_pStaticCurrentCtriText->SetText( wszTemp );
#endif 
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
void CDnGuildLevelDlg::RefreshGuildInfo()
{
	if( !CDnGuildTask::IsActive() ) return;
	TGuild *pGuild = GetGuildTask().GetGuildInfo();
	if( !pGuild->IsSet() ) return;

	if( wcslen( pGuild->wszGuildHomePage ) > 0 )
	{
		m_pEditBoxHomepage->SetText( pGuild->wszGuildHomePage );
		m_pStaticHomepage->Show( false );
	}
	else
	{
		m_pEditBoxHomepage->ClearText();
		m_pStaticHomepage->Show( true );
	}

	m_pButtonHomepage->Enable( GetGuildTask().IsMaster() );
	m_pEditBoxHomepage->Enable( GetGuildTask().IsMaster() );
}
#endif
#endif

#ifdef PRE_ADD_GUILD_CONTRIBUTION
void CDnGuildLevelDlg::SetGuildContributinPoint( int nTotalContribution, int nWeeklyContribution )
{
	m_nToTalContributionPoint = nTotalContribution;
	m_nWeeklyContributionPoint = nWeeklyContribution;
}
#endif