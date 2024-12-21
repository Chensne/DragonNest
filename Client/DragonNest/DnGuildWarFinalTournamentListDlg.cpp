#include "StdAfx.h"
#include "DnGuildWarFinalTournamentListDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnGuildWarTask.h"
#include "TimeSet.h"
#include "DnGuildTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//////////////////////////////////////////////////////////////////////////
// CDnGuildWarFinalTournamentListDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarFinalTournamentListDlg::CDnGuildWarFinalTournamentListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticTournamentInfo( NULL )
{
	memset( m_pStaticGuildName, 0, sizeof( m_pStaticGuildName ) );
	memset( m_pStaticListOn, 0, sizeof( m_pStaticListOn ) );
	memset( m_pStaticLeftRoundResultLine, 0, sizeof( m_pStaticLeftRoundResultLine ) );
	memset( m_pStaticRightRoundResultLine, 0, sizeof( m_pStaticRightRoundResultLine ) );
	memset( m_pButtonVote, 0, sizeof( m_pButtonVote ) );
	memset( m_pGuildMark, 0, sizeof( m_pGuildMark ) );
}

CDnGuildWarFinalTournamentListDlg::~CDnGuildWarFinalTournamentListDlg()
{
}

void CDnGuildWarFinalTournamentListDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	char szStr[64];
	for( int i=0; i<MAX_TOURNAMENT_COUNT; i++ )
	{
		sprintf_s( szStr, "ID_TEXT_GUILDNAME%d", i );
		m_pStaticGuildName[i] = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_STATIC_LISTON%d", i );
		m_pStaticListOn[i] = GetControl<CEtUIStatic>( szStr );

		sprintf_s( szStr, "ID_BT_VOTE%d", i );
		m_pButtonVote[i] = GetControl<CEtUIButton>( szStr );
		m_pButtonVote[i]->Enable( false );

		sprintf_s( szStr, "ID_TEXTUREL_EMBLEM%d", i );
		m_pGuildMark[i] = GetControl<CEtUITextureControl>( szStr );
	}

	for( int i=0; i<RESULT_LINE_COUNT/2; i++ )
	{
		sprintf_s( szStr, "ID_LINE_L_%d", i+1 );
		m_pStaticLeftRoundResultLine[i] = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_LINE_R_%d", i+1 );
		m_pStaticRightRoundResultLine[i] = GetControl<CEtUIStatic>( szStr );
	}

	m_pStaticTournamentInfo = GetControl<CEtUIStatic>( "ID_TEXT_NOTICE0" );
}

void CDnGuildWarFinalTournamentListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarMatchListDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarFinalTournamentListDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
		
		if( hNpc )
			hNpc->SetActionQueue( "Close" );

		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarFinalTournamentListDlg::SetTournamentUI()
{
	ClearTournamentUI();

	int nWinnerIndex = -1;
	for( int i=0; i<MAX_TOURNAMENT_COUNT; i++ )
	{
		m_pStaticGuildName[i]->SetText( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_strGuildName );

		const TGuildView &GuildView = GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_tGuildSelfView;
		if( GetGuildTask().IsShowGuildMark( GuildView ) )
		{
			EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture( GuildView );
			m_pGuildMark[i]->SetTexture( hGuildMark );
			m_pGuildMark[i]->Show( true );
		}

		if( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_strGuildName.length() == 0 )	// 16강이 미달일 경우 투표버튼 비활성화
		{
			m_pButtonVote[i]->Enable( false );
			m_pStaticListOn[i]->Show( false );
			continue;
		}
		else
			m_pButtonVote[i]->Enable( true );

		// Show Tournament Line
		if( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_cMatchTypeCode == eGuildFinalPart::GUILDWAR_FINALPART_NONE )	// 16강 결과전일 경우 전부 On상태 표시
			m_pStaticListOn[i]->Show( true );
		else if( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_cMatchTypeCode <= eGuildFinalPart::GUILDWAR_FINALPART_16 )
		{
			// 우승자 저장
			if( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_bWin && GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_cMatchTypeCode == eGuildFinalPart::GUILDWAR_FINALPART_FINAL )
				nWinnerIndex = i;

			if( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_bWin )
				m_pStaticListOn[i]->Show( true );
			else
				m_pStaticListOn[i]->Show( false );

			int nNum = i % ( MAX_TOURNAMENT_COUNT / 2 );
			int nAddNum =  MAX_TOURNAMENT_COUNT;

			int nWinCount = 0;
			bool bFinal = false;
			switch( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_cMatchTypeCode )
			{
				case eGuildFinalPart::GUILDWAR_FINALPART_16:
					nWinCount = 1;
					break;
				case eGuildFinalPart::GUILDWAR_FINALPART_8:
					nWinCount = 2;
					break;
				case eGuildFinalPart::GUILDWAR_FINALPART_4:
					nWinCount = 3;
					break;
				case eGuildFinalPart::GUILDWAR_FINALPART_FINAL:
					{
						bFinal = true;
						nWinCount = 3;
					}
					break;
			}

			if( !bFinal && GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_bWin == false )
				nWinCount--;

			for( int j=0; j<nWinCount; j++ )
			{
				if( i < MAX_TOURNAMENT_COUNT / 2 )
				{
					m_pStaticLeftRoundResultLine[nNum]->Show( true );
					if( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_bWin )
						m_pStaticLeftRoundResultLine[nNum]->Enable( true );
					else
						m_pStaticLeftRoundResultLine[nNum]->Enable( false );
				}
				else
				{
					m_pStaticRightRoundResultLine[nNum]->Show( true );
					if( GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[i].m_bWin )
						m_pStaticRightRoundResultLine[nNum]->Enable( true );
					else
						m_pStaticRightRoundResultLine[nNum]->Enable( false );
				}
				
				switch( nNum )
				{
					case 0:
					case 1:
						nNum = 8;
						break;
					case 2:
					case 3:
						nNum = 9;
						break;
					case 4:
					case 5:
						nNum = 10;
						break;
					case 6:
					case 7:
						nNum = 11;
						break;
					case 8:
					case 9:
						nNum = 12;
						break;
					case 10:
					case 11:
						nNum = 13;
						break;
				}
			}
		}
	}

	if( !GetGuildWarTask().GetGuildTournamentInfo().m_bPopularityVote )
	{
		for( int i=0; i<MAX_TOURNAMENT_COUNT; i++ )
			m_pButtonVote[i]->Enable( false );
	}

	if( GetGuildWarTask().GetGuildTournamentInfo().m_cMatchTypeCode == eGuildFinalPart::GUILDWAR_FINALPART_NONE )	// 경기 종료
	{
		if( nWinnerIndex > -1 )
		{
			WCHAR wszStr[256];
			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126241 ), GetGuildWarTask().GetGuildTournamentInfo().m_TournamentGuild[nWinnerIndex].m_strGuildName.c_str() );
			m_pStaticTournamentInfo->SetText( wszStr );
		}
	}
	else if( GetGuildWarTask().GetGuildTournamentInfo().m_cMatchTypeCode != eGuildFinalPart::GUILDWAR_FINALPART_FINAL )	// 16,8,4강 전
	{
		int nCount = 2;
		for( int i=1; i<GetGuildWarTask().GetGuildTournamentInfo().m_cMatchTypeCode; i++ )
			nCount *= 2;

		WCHAR wszStr[256];
		if( GetGuildWarTask().GetGuildTournamentInfo().m_tStartTime == 0 )	// n강 경기가 진행 중
		{
			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126118 ), nCount );
			m_pStaticTournamentInfo->SetText( wszStr );
		}
		else	// n강 경기까지 남은 시간
		{
			DBTIMESTAMP DbTime;
			CTimeSet::ConvertTimeT64ToDbTimeStamp_LC( GetGuildWarTask().GetGuildTournamentInfo().m_tStartTime, &DbTime );

			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126117 ), nCount, DbTime.day, DbTime.hour, DbTime.minute );
			m_pStaticTournamentInfo->SetText( wszStr );
		}
	}
	else
	{
		if( GetGuildWarTask().GetGuildTournamentInfo().m_tStartTime == 0 )	// 결승 경기가 진행 중
		{
			m_pStaticTournamentInfo->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126120 ) );
		}
		else	// 결승 경기까지 남은 시간
		{
			DBTIMESTAMP DbTime;
			CTimeSet::ConvertTimeT64ToDbTimeStamp_LC( GetGuildWarTask().GetGuildTournamentInfo().m_tStartTime, &DbTime );

			WCHAR wszStr[256];
			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126119 ), DbTime.day, DbTime.hour, DbTime.minute );
			m_pStaticTournamentInfo->SetText( wszStr );
		}
	}
}

void CDnGuildWarFinalTournamentListDlg::ClearTournamentUI()
{
	for( int i=0; i<MAX_TOURNAMENT_COUNT; i++ )
	{
		m_pStaticGuildName[i]->SetText( L"" );
		m_pStaticListOn[i]->Show( true );
		m_pButtonVote[i]->Enable( false );
		m_pGuildMark[i]->Show( false );
	}

	for( int i=0; i<RESULT_LINE_COUNT/2; i++ )
	{
		m_pStaticLeftRoundResultLine[i]->Show( false );
		m_pStaticRightRoundResultLine[i]->Show( false );
	}

	m_pStaticTournamentInfo->SetText( L"" );
}

void CDnGuildWarFinalTournamentListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			Show( false );
		}
		else if( strstr( GetCmdControlName().c_str(), "ID_BT_VOTE" ) )
		{
			std::string strControlName = GetCmdControlName();
			strControlName.erase( 0, 10 );

			GetGuildWarTask().SendGuildWarVote( atoi( strControlName.c_str() ) );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
