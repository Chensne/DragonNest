#include "stdafx.h"
#include "DnGuildWarResultListDlg.h"
#include "DnGuildWarResultDlg.h"
#include "DnInterface.h"
#include "DnGuildTask.h"
#include "TaskManager.h"
#include "DnRevengeTask.h"

CDnGuildWarResultListDlg::CDnGuildWarResultListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
:CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pStaticRedBar( NULL )
, m_pStaticBlueBar( NULL )
{

}

CDnGuildWarResultListDlg::~CDnGuildWarResultListDlg(void)
{

}

void CDnGuildWarResultListDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarScoreListDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarResultListDlg::InitialUpdate()
{
	m_UIDataSet[eTeam_My].m_pStaticLevel = GetControl<CEtUIStatic>( "ID_SCORE_LEVEL" );
	m_UIDataSet[eTeam_My].m_pStaticGuildName = GetControl<CEtUIStatic>( "ID_SCORE_GUILDNAME" );
	m_UIDataSet[eTeam_My].m_pStaticPlayerName = GetControl<CEtUIStatic>( "ID_SCORE_NAME" );
	m_UIDataSet[eTeam_My].m_pStaticKillCount = GetControl<CEtUIStatic>( "ID_SCORE_KILLCOUND" );
	m_UIDataSet[eTeam_My].m_pStaticDeathCount = GetControl<CEtUIStatic>( "ID_SCORE_DEATH" );
	m_UIDataSet[eTeam_My].m_pStaticAssistCount = GetControl<CEtUIStatic>( "ID_SCORE_ASSIST" );
	m_UIDataSet[eTeam_My].m_pStaticScore = GetControl<CEtUIStatic>( "ID_SCORE_SCORE" );
	m_UIDataSet[eTeam_My].m_pStaticBasePoint = GetControl<CEtUIStatic>( "ID_SCORE_BASEPOINT" );
	m_UIDataSet[eTeam_My].m_pStaticMedal = GetControl<CEtUIStatic>( "ID_SCORE_MEDAL" );
	m_UIDataSet[eTeam_My].m_pStaticJob = GetControl<CDnJobIconStatic>( "ID_SCORE_CLASS" );
	m_UIDataSet[eTeam_My].m_pTextureRank = GetControl<CEtUITextureControl>( "ID_TEXTURE_RANK" );
	m_UIDataSet[eTeam_My].m_pTextureMark = GetControl<CEtUITextureControl>( "ID_TEXTURE_MARK0" );

	m_UIDataSet[eTeam_Blue].m_pStaticLevel = GetControl<CEtUIStatic>( "ID_SCORE_BLUELEVEL" );
	m_UIDataSet[eTeam_Blue].m_pStaticGuildName = GetControl<CEtUIStatic>( "ID_SCORE_BLUEGUILDNAME" );
	m_UIDataSet[eTeam_Blue].m_pStaticPlayerName = GetControl<CEtUIStatic>( "ID_SCORE_BLUENAME" );
	m_UIDataSet[eTeam_Blue].m_pStaticKillCount = GetControl<CEtUIStatic>( "ID_SCORE_BLUEKILLCOUND" );
	m_UIDataSet[eTeam_Blue].m_pStaticDeathCount = GetControl<CEtUIStatic>( "ID_SCORE_BLUEDEATH" );
	m_UIDataSet[eTeam_Blue].m_pStaticAssistCount = GetControl<CEtUIStatic>( "ID_SCORE_BLUEASSIST" );
	m_UIDataSet[eTeam_Blue].m_pStaticScore = GetControl<CEtUIStatic>( "ID_SCORE_BLUESCORE" );
	m_UIDataSet[eTeam_Blue].m_pStaticBasePoint = GetControl<CEtUIStatic>( "ID_SCORE_BLUEBASEPOINT" );
	m_UIDataSet[eTeam_Blue].m_pStaticMedal = GetControl<CEtUIStatic>( "ID_SCORE_BLUEMEDAL" );
	m_UIDataSet[eTeam_Blue].m_pStaticJob = GetControl<CDnJobIconStatic>( "ID_SCORE_BLUECLASS" );
	m_UIDataSet[eTeam_Blue].m_pTextureRank = GetControl<CEtUITextureControl>( "ID_TEXTURE_BLUERANK" );
	m_UIDataSet[eTeam_Blue].m_pTextureMark = GetControl<CEtUITextureControl>( "ID_TEXTURE_BLUEMARK" );

	m_UIDataSet[eTeam_Red].m_pStaticLevel = GetControl<CEtUIStatic>( "ID_SCORE_REDLEVEL" );
	m_UIDataSet[eTeam_Red].m_pStaticGuildName = GetControl<CEtUIStatic>( "ID_SCORE_REDGUILDNAME" );
	m_UIDataSet[eTeam_Red].m_pStaticPlayerName = GetControl<CEtUIStatic>( "ID_SCORE_REDNAME" );
	m_UIDataSet[eTeam_Red].m_pStaticKillCount = GetControl<CEtUIStatic>( "ID_SCORE_REDKILLCOUND" );
	m_UIDataSet[eTeam_Red].m_pStaticDeathCount = GetControl<CEtUIStatic>( "ID_SCORE_REDDEATH" );
	m_UIDataSet[eTeam_Red].m_pStaticAssistCount = GetControl<CEtUIStatic>( "ID_SCORE_REDASSIST" );
	m_UIDataSet[eTeam_Red].m_pStaticScore = GetControl<CEtUIStatic>( "ID_SCORE_REDSCORE" );
	m_UIDataSet[eTeam_Red].m_pStaticBasePoint = GetControl<CEtUIStatic>( "ID_SCORE_REDBASEPOINT" );
	m_UIDataSet[eTeam_Red].m_pStaticMedal = GetControl<CEtUIStatic>( "ID_SCORE_REDMEDAL" );
	m_UIDataSet[eTeam_Red].m_pStaticJob = GetControl<CDnJobIconStatic>( "ID_SCORE_REDCLASS" );
	m_UIDataSet[eTeam_Red].m_pTextureRank = GetControl<CEtUITextureControl>( "ID_TEXTURE_REDRANK" );
	m_UIDataSet[eTeam_Red].m_pTextureMark = GetControl<CEtUITextureControl>( "ID_TEXTURE_REDMARK" );

	m_pStaticRedBar = GetControl<CEtUIStatic>( "ID_STATIC_RED" );;
	m_pStaticBlueBar = GetControl<CEtUIStatic>( "ID_STATIC_BLUE" );;
}

void CDnGuildWarResultListDlg::SetList( CDnGuildWarResultDlg::SUserInfo & data )
{
	m_Data = data;

	SUIDataSet * sUIDataSet = NULL;
	if( CDnBridgeTask::GetInstance().GetSessionID() == data.nSessionID )
	{
		sUIDataSet = &m_UIDataSet[eTeam_My];
		m_UIDataSet[eTeam_My].Show( true );
		m_UIDataSet[eTeam_Red].Show( false );
		m_UIDataSet[eTeam_Blue].Show( false );

		if( CDnGuildWarResultDlg::eTeamState::RED_TEAM == data.cTeam )
			m_pStaticRedBar->Show( true );
		else
			m_pStaticBlueBar->Show( true );
	}
	else if( CDnGuildWarResultDlg::eTeamState::RED_TEAM == data.cTeam )
	{
		sUIDataSet = &m_UIDataSet[eTeam_Red];
		m_UIDataSet[eTeam_My].Show( false );
		m_UIDataSet[eTeam_Red].Show( true );
		m_UIDataSet[eTeam_Blue].Show( false );

		m_pStaticRedBar->Show( true );
	}
	else if( CDnGuildWarResultDlg::eTeamState::BLUE_TEAM == data.cTeam )
	{
		sUIDataSet = &m_UIDataSet[eTeam_Blue];
		m_UIDataSet[eTeam_My].Show( false );
		m_UIDataSet[eTeam_Red].Show( false );
		m_UIDataSet[eTeam_Blue].Show( true );

		m_pStaticBlueBar->Show( true );
	}

	if( NULL == sUIDataSet )
		return;

#if defined( PRE_ADD_REVENGE )
	if( CDnBridgeTask::GetInstance().GetSessionID() != data.nSessionID )
	{
		CDnRevengeTask * pRevengeTask = (CDnRevengeTask *)CTaskManager::GetInstance().GetTask( "RevengeTask" );
		if( NULL != pRevengeTask )
			pRevengeTask->GetRevengeUserID( data.nSessionID, data.eRevengeUser );

		if( Revenge::RevengeTarget::eRevengeTarget_Target == data.eRevengeUser )
			sUIDataSet->m_pStaticPlayerName->SetTextColor( EtInterface::textcolor::PVP_REVENGE_TARGET );
		else if( Revenge::RevengeTarget::eRevengeTarget_Me == data.eRevengeUser )
			sUIDataSet->m_pStaticPlayerName->SetTextColor( EtInterface::textcolor::PVP_REVENGE_ME );
		else
			sUIDataSet->m_pStaticPlayerName->SetTextColor( EtInterface::textcolor::WHITE );
	}
#endif	// #if defined( PRE_ADD_REVENGE )

	sUIDataSet->m_pStaticLevel->SetIntToText( data.nLevel );
	sUIDataSet->m_pStaticGuildName->SetText( data.GuildSelfView.wszGuildName );
	sUIDataSet->m_pStaticPlayerName->SetText( data.wszUserName.c_str() );
	sUIDataSet->m_pStaticKillCount->SetIntToText( data.uiKOCount );
	sUIDataSet->m_pStaticDeathCount->SetIntToText( data.uiKObyCount );
	sUIDataSet->m_pStaticAssistCount->SetIntToText( data.uiAssistP );
	sUIDataSet->m_pStaticScore->SetIntToText( data.uiTotalScore );
	sUIDataSet->m_pStaticBasePoint->SetIntToText( data.uiOccupation );
	sUIDataSet->m_pStaticMedal->SetIntToText( data.uiMedal );

	sUIDataSet->m_pStaticJob->SetIconID( data.cJobClassID, true );

	int iIconW,iIconH;
	int iU,iV;
	iIconW = GetInterface().GeticonWidth();
	iIconH = GetInterface().GeticonHeight();
	if( GetInterface().ConvertPVPGradeToUV( data.cPVPlevel, iU, iV ))
	{
		sUIDataSet->m_pTextureRank->SetTexture(GetInterface().GetPVPIconTex(), iU, iV, iIconW, iIconH);
		sUIDataSet->m_pTextureRank->Show(true);
	}

	const TGuildView &GuildView = data.GuildSelfView;
	if( GetGuildTask().IsShowGuildMark( GuildView ) )
	{
		EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture( GuildView );
		sUIDataSet->m_pTextureMark->SetTexture( hGuildMark );
		sUIDataSet->m_pTextureMark->Show( true );
	}
}

void CDnGuildWarResultListDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );

	CDnGuildWarResultDlg * pDialog = GetInterface().GetGuildWarResultDlg();

	if( !pDialog ) return;

	CDnGuildWarResultDlg::SUserInfo sUserInfo = pDialog->GetUserInfo( m_Data.nSessionID );

	if( -1 == sUserInfo.nSessionID ) return;

	SetList( sUserInfo );
}
