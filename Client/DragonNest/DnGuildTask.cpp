#include "StdAfx.h"
#include "DnGuildTask.h"
#include "GuildSendPacket.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnCommunityDlg.h"
#include "DnGuildDlg.h"
#include "DnGuildCreateDlg.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnLocalPlayerActor.h"
#include "DnChatRoomDlg.h"
#include "DnItemTask.h"
#include "DnPGStorageTabDlg.h"
#include "DnGuildStorageDlgNew.h"
#include "DnPVPLobbyChatTabDlg.h"
#include "DnPVPLobbyVillageTask.h"
#include "EtDrawQuad.h"
#include "DnGuildMarkCreateDlg.h"
#include "GuildRecruitSendPacket.h"
#include "DnNPCActor.h"

#ifdef PRE_ADD_GUILD_EASYSYSTEM
#include "DnGuildWantedListTabDlg.h"
#else
#include "DnGuildWantedListDlg.h"
#endif

#include "DnGuildInfoDlg.h"
#include "DnUIString.h"
#include "DnCharStatusDlg.h"
#include "DnNpcDlg.h"
#include "DNGestureTask.h"

#include "DnPetTask.h"
#include "DnChatOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGuildTask::CDnGuildTask(void)
	: CTaskListener(true)
{
	m_fAutoRequestDelayTime = AUTO_REQUEST_GUILDINFO_FIRST;
	m_fDialogShowDelayTime = 0;

	m_cCurrentGuildWarEventStep = 0;
	m_wCurrentGuldWarScheduleID = 0;

	m_nMarkGenerateDelayFrameCount = 0;
	 m_bIsHaveGuildInfo =false;

	 memset(&m_GuildRewardItem, 0, sizeof(m_GuildRewardItem));
#ifdef PRE_ADD_BEGINNERGUILD
	 m_bMessageBoxForGraduateBeginnerGuild = false;
	 m_bChangedGuildChat = false;
#endif
}

CDnGuildTask::~CDnGuildTask(void)
{
	for( int i = 0; i < (int)m_vecGuildMarkInfo.size(); ++i )
		SAFE_RELEASE_SPTR( m_vecGuildMarkInfo[i].hTexture );
	m_vecGuildMarkInfo.clear();
}

bool CDnGuildTask::Initialize()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUILDLEVEL );
	if( !pSox ) return false;

	for( int iItemID = 0; iItemID < pSox->GetItemCount(); ++iItemID ) {
		int nTableID = pSox->GetItemID( iItemID );
		bool bApplicable = pSox->GetFieldFromLablePtr( nTableID, "_IsApplicable" )->GetInteger() ? true : false;
		if( !bApplicable ) continue;

		int nGuildLevel = pSox->GetFieldFromLablePtr( nTableID, "_GuildLevel" )->GetInteger();

		if( nGuildLevel == 1 ) {
			m_nGuildCreateTax = pSox->GetFieldFromLablePtr( nTableID, "_ReqGold" )->GetInteger();
		}

		SGuildLevelInfo Info;
		Info.nLevel = nGuildLevel;
		Info.nReqGuildPoint = pSox->GetFieldFromLablePtr( nTableID, "_ReqGuildPoint" )->GetInteger();
		Info.nDailyGuildPointLimit = pSox->GetFieldFromLablePtr( nTableID, "_DailyGuildPointLimit" )->GetInteger();
		Info.nDailyMissionPointLimit = pSox->GetFieldFromLablePtr( nTableID, "_DailyMissionPointLimit" )->GetInteger();
		m_vecGuildLevelInfo.push_back(Info);
	}

	DNTableFileFormat* pSoxWareHouse = GetDNTable(CDnTableDB::TGUILDWAREHOUSE);
	if( !pSoxWareHouse ) return false;

	// UI상에서 순차적으로 for루프 돌면서 부르기때문에, 인덱스 데이터가 순차적으로 들어있어야 제대로 보일 것이다.
	for( int iItemID = 0; iItemID < pSoxWareHouse->GetItemCount(); ++iItemID ) {
		int nTableID = pSoxWareHouse->GetItemID(iItemID);
		int nType = pSoxWareHouse->GetFieldFromLablePtr( nTableID, "_WareHouseType" )->GetInteger();
		int nIndex = pSoxWareHouse->GetFieldFromLablePtr( nTableID, "_Limit_Index" )->GetInteger();
		int nValue = pSoxWareHouse->GetFieldFromLablePtr( nTableID, "_Limit_Value" )->GetInteger();
		if( nType == 0 ) {
			if( m_mapMaxTakeItemInfo.find(nIndex) == m_mapMaxTakeItemInfo.end() )
				m_mapMaxTakeItemInfo[nIndex] = nValue;
			else
				ASSERT(0&&"GuildWareHouse테이블에 중복된 인덱스가 있습니다!");
		}
		else if( nType == 1 ) {
			if( m_mapMaxWithDrawInfo.find(nIndex) == m_mapMaxWithDrawInfo.end() )
				m_mapMaxWithDrawInfo[nIndex] = nValue;
			else
				ASSERT(0&&"GuildWareHouse테이블에 중복된 인덱스가 있습니다!");
		}
	}

	return true;
}

void CDnGuildTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	bool bProcessDispatch = false;
	switch( nMainCmd ) 
	{
	case SC_GUILD: OnRecvGuildMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
	case SC_GUILDRECRUIT: OnRecvGuildRecruitMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
	}
	if( bProcessDispatch ) {
		WriteLog( 1, ", Info, CDnGuildTask::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}
}

void CDnGuildTask::OnRecvGuildMessage(int nSubCmd, char *pData, int nSize)
{
	switch( nSubCmd ) // 타입에따른 Npc의 사운드 재생을 위한 패킷 구분.
	{
	case eGuild::SC_OPEN_GUILDWARE:
		{
			DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
			DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);

			if ( hNpc ) {
				CDnNPCActor* pActor = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());
				if ( pActor ) {
					pActor->OnSoundPlay( CDnNPCActor::OpenShop );
				}
			}
		}
	}

	switch (nSubCmd)
	{
	case eGuild::SC_CREATEGUILD:			OnRecvCreateGuild( (SCCreateGuild*)pData );					break;
	case eGuild::SC_CHANGEGUILDSELFVIEW:	OnRecvChangeGuildSelfView( (SCChangeGuildSelfView*)pData );	break;
	case eGuild::SC_GETGUILDINFO:			OnRecvGetGuildInfo( (SCGetGuildInfo*)pData );				break;
	case eGuild::SC_DISMISSGUILD:			OnRecvDismissGuild( (SCDismissGuild*)pData );				break;
	case eGuild::SC_INVITEGUILDMEMBREQ:		OnRecvInviteGuildMemberReq( (SCInviteGuildMemberReq*)pData );	break;
	case eGuild::SC_INVITEGUILDMEMBACK:		OnRecvInviteGuildMemberAck( (SCInviteGuildMemberAck*)pData );	break;
	case eGuild::SC_LEAVEGUILDMEMB:			OnRecvLeaveGuild( (SCLeaveGuildMember*)pData );				break;
	case eGuild::SC_EXILEGUILDMEMB:			OnRecvExileGuild( (SCExileGuildMember*)pData );				break;
	case eGuild::SC_CHANGEGUILDINFO:		OnRecvChangeGuildInfo( (SCChangeGuildInfo*)pData );			break;
	case eGuild::SC_CHANGEGUILDMEMBINFO:	OnRecvChangeGuildMemberInfo( (SCChangeGuildMemberInfo*)pData );break;
	case eGuild::SC_GETGUILDHISTORYLIST:	OnRecvGetGuildHistoryList( (SCGetGuildHistoryList*)pData );	break;
	case eGuild::SC_GUILDMEMBLOGINLIST:		OnRecvGuildMemberLoginList( (SCGuildMemberLoginList*)pData );	break;
	case eGuild::SC_OPEN_GUILDWARE:			OnRecvOpenGuildWare( (SCOpenGuildWare*)pData );				break;
	case eGuild::SC_GET_GUILDWARE_HISTORY:	OnRecvGetGuildWareHistoryList( (SCGetGuildWareHistory*)pData );break;
	case eGuild::SC_CHANGEGUILDNAME :		OnRecvChangeGuildName( (SCChangeGuildName*)pData);			break;
	case eGuild::SC_EXTEND_GUILDWARESIZE:	OnRecvExtendGuildWare( (SCExtendGuildWare*)pData );			break;
	case eGuild::SC_ENROLL_GUILDWAR:		OnRecvEnrollGuildWar( (SCEnrollGuildWar*)pData );			break;
	case eGuild::SC_CHANGE_WAREVENT:		OnRecvChangeGuildWarEvent( (SCGuildWarEvent*)pData );			break;
	case eGuild::SC_GUILDWAR_TOURNAMENT_WIN:	OnRecvGuildWarTournamentWin( (SCGuildWarTournamentWin*)pData );	break;
	case eGuild::SC_CHANGEGUILDMARK:		OnRecvChangeGuildMark( (SCChangeGuildMark*)pData );			break;
	case eGuild::SC_UPDATEGUILDEXP:         OnRecvUpdateGuildExp( (SCUpdateGuildExp*)pData ); break;
	case eGuild::SC_GUILDLEVELUP:			OnRecvGuildLevelUp( (SCGuildLevelUp*)pData ); break;

	case eGuild::SC_GUILD_GET_REWARDITEM:	OnRecvGuildGetRewardItem((SCGetGuildRewardItem*)pData);	break;
	case eGuild::SC_GUILD_BUY_REWARDITEM:	OnRecvGuildBuyRewardItem((SCBuyGuildRewardItem*)pData); break;
	case eGuild::SC_GUILD_ADD_REWARDITEM:	OnRecvGuildAddRewardItem((SCAddGUildRewardItem*)pData); break;
	case eGuild::SC_GUILD_EXTEND_GUILDSIZE:	OnRecvGuildExtendGuildSize((SCExtendGuildSize*)pData); break;
	case eGuild::SC_PLAYER_REQUEST_GUILDINFO: OnRecvPlayerGuildInfo((SCPlayerGuildInfo*)pData); break;
	case eGuild::SC_GET_GUILDMEMBER:		OnRecvGetGuildMember((SCGetGuildMember*)pData); break;
#ifdef PRE_ADD_GUILD_CONTRIBUTION
	case eGuild::SC_GUILD_CONTRIBUTION_POINT: OnRecvGuildContributionPoint((GuildContribution::SCGuildContributionPoint*)pData ); break;
	case eGuild::SC_GUILD_CONTRIBUTION_RANK: OnRecvGuildContributionRankList((GuildContribution::SCGuildContributionRank*)pData ); break;
#endif 

	default: break;
	}
}

void CDnGuildTask::OnRecvGuildRecruitMessage(int nSubCmd, char *pData, int nSize)
{
	switch (nSubCmd)
	{
	case eGuildRecruit::SC_GUILDRECRUIT_REGISTERINFO:	OnRecvGuildRecruitRegisterInfo( (GuildRecruitSystem::SCGuildRecruitRegisterInfo*)pData );	break;
	case eGuildRecruit::SC_GUILDRECRUIT_REGISTER:	OnRecvGuildRecruitRegister( (GuildRecruitSystem::SCGuildRecruitRegister*)pData );	break;
	case eGuildRecruit::SC_GUILDRECRUIT_LIST:	OnRecvGuildRecruitList( (GuildRecruitSystem::SCGuildRecruitList*)pData );	break;
	case eGuildRecruit::SC_GUILDRECRUIT_REQUESTCOUNT:	OnRecvGuildRecruitRequestCount( (GuildRecruitSystem::SCGuildRecruitRequestCount*)pData );	break;
	case eGuildRecruit::SC_GUILDRECRUIT_MYLIST:	OnRecvGuildRecruitMyList( (GuildRecruitSystem::SCMyGuildRecruitList*)pData );	break;
	case eGuildRecruit::SC_GUILDREQUEST_CHARACTER:	OnRecvGuildRecruitCharacter( (GuildRecruitSystem::SCGuildRecruitCharacterList*)pData );	break;
	case eGuildRecruit::SC_GUILDRECRUIT_REQUEST:	OnRecvGuildRecruitRequest( (GuildRecruitSystem::SCGuildRecruitRequest*)pData );	break;
	case eGuildRecruit::SC_GUILDRECRUIT_ACCEPRESULT:	OnRecvGuildRecruitAcceptResult( (GuildRecruitSystem::SCGuildRecruitAccept*)pData );	break;
	case eGuildRecruit::SC_GUILDRECRUIT_MEMBERRESULT:	OnRecvGuildRecruitMemberResult( (GuildRecruitSystem::SCGuildRecruitMemberResult*)pData );	break;
	default: break;
	}
}

void CDnGuildTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !CDnActor::s_hLocalActor ) return;
	if( !CDnInterface::IsActive() ) return;

	if( m_nMarkGenerateDelayFrameCount == 0 )
	{
		for( int i = 0; i < (int)m_vecGuildMarkInfo.size(); ++i )
		{
			if( !m_vecGuildMarkInfo[i].hTexture )
				m_vecGuildMarkInfo[i].hTexture = GenerateGuildMarkTexture( m_vecGuildMarkInfo[i].nIconIndex[0], m_vecGuildMarkInfo[i].nIconIndex[1], m_vecGuildMarkInfo[i].nIconIndex[2] );
		}
	}
	else m_nMarkGenerateDelayFrameCount -= 1;

	if( m_fAutoRequestDelayTime > 0.0f ) {
		m_fAutoRequestDelayTime -= fDelta;
	}
	else {
		CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if( !pCommunityDlg ) return;

		CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
		if( !pGuildDlg ) return;

		if( pGuildDlg->IsShow() ) {
			RequestGetGuildInfo( true );
			m_fAutoRequestDelayTime = AUTO_REQUEST_GUILDINFO_DIALOG_SHOW_TRUE;
		}
		else {
			RequestGetGuildInfo( false );
			m_fAutoRequestDelayTime = AUTO_REQUEST_GUILDINFO_DIALOG_SHOW_FALSE;
		}
	}

	if( m_fDialogShowDelayTime > 0.0f ) {
		m_fDialogShowDelayTime -= fDelta;
	}


	bool bShow = m_Guild.IsSet();
	bool isMarked = GetInterface().IsMarkedGuildReward();

	if (isMarked != bShow)
		GetInterface().RefreshGuildRewardMark(bShow, true);
}


void CDnGuildTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) 
	{
	case 0:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
				{
					RequestInviteGuildMemberAck( true );
				}	
				else if( (strcmp( pControl->GetControlName(), "ID_CANCEL" ) == 0) )
				{
					RequestInviteGuildMemberAck( false );
				}

				GetInterface().CloseGuildInviteReqDlg();
			}
		}
		break;
#ifdef PRE_ADD_BEGINNERGUILD
	case 1:
		{
			if( GetInterface().IsOpenBlind() )
			{
				if( GetInterface().GetNpcDialog() && GetInterface().GetNpcDialog()->IsShow() )
					GetInterface().CloseNpcDialog();

				GetInterface().CloseAllMainMenuDialog();
				GetInterface().CloseBlind();

				GetInterface().GetBlindDialog()->AddBlindCallBack( this	);
			}
			else
			{
				// 직접 호출하자.
				OnBlindClosed();
			}
		}
		break;
#endif
	}
}

#ifdef PRE_ADD_BEGINNERGUILD
void CDnGuildTask::OnBlindClosed()
{
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if( pCommunityDlg )
	{
		if( !pCommunityDlg->IsShow() )
		{
			GetInterface().GetMainMenuDialog()->ToggleShowDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		}
		CEtUIRadioButton *pRBT = pCommunityDlg->GetControl<CEtUIRadioButton>("ID_TAB_GUILD");
		if( pRBT )
		{
			pCommunityDlg->SetCheckedTab( pRBT->GetTabID() );
		}
#ifdef PRE_ADD_GUILD_EASYSYSTEM
		CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
		CDnGuildWantedListTabDlg *pGuildWantedListTabDlg = pCommunityDlg->GetGuildWantedListTabDialog();
		if( pGuildDlg && pGuildWantedListTabDlg )
		{
			pGuildDlg->Show( false );
			pGuildWantedListTabDlg->Show( true );
			if( !pGuildWantedListTabDlg->GetHWnd() )
			{
				RequestGetGuildRecruitList( 0, GuildRecruitSystem::PuposeCodeType::AllCode, L"", GuildRecruitSystem::SortType::GuildLevelDesc );
				RequestGetGuildRecruitRequestCount();
			}
		}
#endif
	}
}
#endif

void CDnGuildTask::RequestCreateGuild( const WCHAR *wszGuildName )
{
	// 길드태스크는 길드정보를 접속 직후 받는게 아니기때문에, 플레이어정보에서 검사하는 것이 확실하다.
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pPlayer->IsJoinGuild() ) {
		GetInterface().MessageBox( 3780 );	// 들어올 가능성은 없지만,
		return;
	}

	SendCreateGuild(wszGuildName);
}

void CDnGuildTask::RequestGetGuildInfo( bool bNeedMemeberList )
{
	// pvp로비에선 s_hLocalActor NULL일 수 있다.
	if( CDnActor::s_hLocalActor ) {
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if( !pPlayer->IsJoinGuild() ) return;
	}

	// 다이얼로그 요청 딜레이가 아직 남았다면 다시 보낼 수 없다.
	if( m_fDialogShowDelayTime > 0.0f ) return;

	SendGetGuildInfo( bNeedMemeberList );
	m_fDialogShowDelayTime = REQUEST_GUILDINFO_DELAY;

	// 요청을 했다면 자동요청 딜레이를 재설정한다.
	m_fAutoRequestDelayTime = AUTO_REQUEST_GUILDINFO_DIALOG_SHOW_TRUE;
}

void CDnGuildTask::RequestDismissGuild()
{
	// 길드태스크는 길드정보를 접속 직후 받는게 아니기때문에, 플레이어정보에서 검사하는 것이 확실하다.
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( !pPlayer->IsJoinGuild() ) {
		GetInterface().MessageBox( 3908 );	// 들어올 가능성은 없지만,
		return;
	}
	if( !pPlayer->IsGuildMaster() ) {
		GetInterface().MessageBox( 3914 );	// 들어올 가능성은 없지만,
		return;
	}

	// 길드정보가 세팅되어있다면 유효성검사를 실시한다.
	if( m_Guild.IsSet() ) {

		// 길드장 혼자 남았는지
		if( (int)m_vecGuildMember.size() > 1 ) {
			GetInterface().MessageBox( 3778 );
			return;
		}

		// 길드돈(추후 창고 검사)
		if( m_Guild.ui64GuildMoney > 0 ) {
			GetInterface().MessageBox( 3779 );
			return;
		}
	}

	SendDismissGuild();
}

void CDnGuildTask::RequestInviteGuildMember( const WCHAR *wszPlayerName )
{
	// 길드에 가입 되어있나. 예외처리.
	if( !m_Guild.IsSet() )
		return;

	// 길드에 가입 되어있나. 예외처리.
	if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_INVITE ) == false )
		return;

	// 더이상 받을 공간이 없다면,
	if( GetGuildMemberCount() >= m_Guild.wGuildSize) {
		GetInterface().MessageBox( 3781 );
		return;
	}

	// 자신이라면,
	if( !CDnActor::s_hLocalActor ) return;
	if( __wcsicmp_l(wszPlayerName, CDnActor::s_hLocalActor->GetName()) == 0 ) {
		GetInterface().MessageBox( 3900 );
		return;
	}

	// 혹시 자신의 길드멤버라면,
	TGuildMember *pMember = GetGuildMemberFromName( wszPlayerName );
	if( pMember ) {
		GetInterface().MessageBox( 3901 );
		return;
	}

	// 혹시 주변에서 해당플레이어를 찾아 있다면,
	DnActorHandle hActor = CDnActor::FindActorFromName( (TCHAR *)wszPlayerName );
	if( hActor ) {
		CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
		if( pActor->IsJoinGuild() ) {
			GetInterface().MessageBox( 3780 );
			return;
		}
	}

	SendInviteGuildMemberReq( wszPlayerName );
}

void CDnGuildTask::RequestInviteGuildMemberAck( bool bAccept )
{
	if( m_InviteGuildUID.IsSet() && m_InviteSESID > 0 ) {
		SendInviteGuildMemberAck( m_InviteGuildUID, m_InviteACCID, m_InviteSESID, bAccept );
		m_InviteGuildUID.Reset();
		m_InviteSESID = 0;
		m_InviteACCID = 0;
	}
}

void CDnGuildTask::RequestLeaveGuild()
{
	// 길드태스크는 길드정보를 접속 직후 받는게 아니기때문에, 플레이어정보에서 검사하는 것이 확실하다.
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( !pPlayer->IsJoinGuild() ) {
		GetInterface().MessageBox( 3908 );	// 들어올 가능성은 없지만,
		return;
	}
	if( pPlayer->IsGuildMaster() ) {
		GetInterface().MessageBox( 3913 );	// 들어올 가능성은 없지만,
		return;
	}

	SendLeaveGuild();
}

void CDnGuildTask::RequestExileGuild( UINT nAccountDBID, INT64 nCharacterDBID )
{
	// 길드태스크는 길드정보를 접속 직후 받는게 아니기때문에, 플레이어정보에서 검사하는 것이 확실하다.
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( !pPlayer->IsJoinGuild() ) {
		GetInterface().MessageBox( 3908 );	// 들어올 가능성은 없지만,
		return;
	}

	if( !pPlayer->IsGuildMaster() ) {
		if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_EXILE ) == false ) {
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3914 ), false );
			return;
		}
	}

	SendExileGuild( nAccountDBID, nCharacterDBID );
}

void CDnGuildTask::RequestChangeGuildInfo(BYTE btGuildUpdate, int iInt1, int iInt2, INT64 biInt64, LPCWSTR pText)
{
	SendChangeGuildInfo(btGuildUpdate, iInt1, iInt2, biInt64, pText);
}


void CDnGuildTask::RequestChangeGuildInfoEx(BYTE btGuildUpdate, int iInt1, int iInt2, int iInt3, int iInt4, INT64 biInt64, LPCWSTR pText)
{
	SendChangeGuildInfoEx(btGuildUpdate, iInt1, iInt2, iInt3, iInt4, biInt64, pText);
}

void CDnGuildTask::RequestChangeGuildMemberInfo(BYTE btGuildMembUpdate, int iInt1, int iInt2, INT64 biInt64, LPCWSTR pText, UINT nChgAccountDBID, INT64 nChgCharacterDBID)
{
	SendChangeGuildMemberInfo(btGuildMembUpdate, iInt1, iInt2, biInt64, pText, nChgAccountDBID, nChgCharacterDBID);
}

void CDnGuildTask::RequestGetGuildHistoryList(INT64 biIndex, bool bDirection)
{
	SendGetGuildHistoryList(biIndex, bDirection);
}

void CDnGuildTask::RequestGetGuildStorageHistoryList(int nPage)
{
	SendGetGuildStorageHistoryList(nPage);
}

void CDnGuildTask::RequestEnrollGuildWar()
{
	SendEnrollGuildWar();
}

void CDnGuildTask::RequestGetGuildRecruitRegisterInfo()
{
	SendGetGuildRecruitRegisterInfo();
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
void CDnGuildTask::RequestRegisterGuildRecruit( bool bModify, BYTE *pClassGrade, int nMinLevel, int nMaxLevel, const WCHAR *pGuildIntroduction, BYTE cPurposeCode, bool bCheckHomepage )
{
	SendRegisterGuildRecruit( bModify, pClassGrade, nMinLevel, nMaxLevel, pGuildIntroduction, cPurposeCode, bCheckHomepage );
}
#else
void CDnGuildTask::RequestRegisterGuildRecruit( bool bModify, BYTE *pClassGrade, int nMinLevel, int nMaxLevel, const WCHAR *pGuildIntroduction )
{
	SendRegisterGuildRecruit( bModify, pClassGrade, nMinLevel, nMaxLevel, pGuildIntroduction );
}
#endif

void CDnGuildTask::RequestCancelGuildRecruit()
{
	SendCancelGuildRecruit();
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
void CDnGuildTask::RequestGetGuildRecruitList( int nPage, BYTE cPurposeCode, const WCHAR *pGuildName, BYTE cSortType )
{
	SendGetGuildRecruitList( nPage, cPurposeCode, pGuildName, cSortType );
}
#else
void CDnGuildTask::RequestGetGuildRecruitList( int nPage )
{
	SendGetGuildRecruitList( nPage );
}
#endif

void CDnGuildTask::RequestGetGuildRecruitRequestCount()
{
	SendGetGuildRecruitRequestCount();
}

void CDnGuildTask::RequestGetGuildRecruitMyList()
{
	SendGetGuildRecruitMyList();
}

void CDnGuildTask::RequestGetGuildRecruitCharacter()
{
	SendGetGuildRecruitCharacter();
}

void CDnGuildTask::RequestJoinGuild( TGuildUID GuildUID, bool bCancel )
{
	SendRequestJoinGuild( GuildUID, bCancel );
}

void CDnGuildTask::RequestAcceptJoinGuild( bool bAccept, INT64 nCharacterDBID, const WCHAR *pToCharacterName )
{
	if( !m_Guild.IsSet() ) return;
	SendRequestAcceptJoinGuild( bAccept, m_Guild.GuildView.GuildUID, nCharacterDBID, pToCharacterName );
}

void CDnGuildTask::OnRecvCreateGuild( SCCreateGuild *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo, MB_OK, 0, GetInterface().GetGuildCreateDlg() );
		return;
	}

	SetGuildInfo( pPacket->Info );

	// 창설창 닫고,
	GetInterface().ShowGuildCreateDlg( false );

	// 자신을 길드멤버 리스트에 추가시킨다.
	WCHAR wszCharacterName[NAMELENMAX] = {0,};
	if( CDnActor::s_hLocalActor ) _wcscpy(wszCharacterName, _countof(wszCharacterName), CDnActor::s_hLocalActor->GetName(), (int)wcslen(CDnActor::s_hLocalActor->GetName()));

	TGuildMember MemberInfo;
	MemberInfo.Set( pPacket->nAccountDBID, pPacket->nCharacterDBID, wszCharacterName, 
		pPacket->nJob, pPacket->cLevel, GUILDROLE_TYPE_JUNIOR,
		0, 0, 0, NULL, 0, pPacket->Location.cServerLocation, pPacket->Location.nChannelID, pPacket->Location.nMapIdx );
	AddGuildMember( &MemberInfo );

	// 창설되었습니다. 메세지 처리
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3954 ), false );
	GetPetTask().DoPetChat( PET_CHAT_USER_CREATE_GUILD );
}

void CDnGuildTask::OnRecvChangeGuildSelfView( SCChangeGuildSelfView *pPacket )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	// 자신인지 확인
	if( pPacket->nSessionID == pPlayer->GetUniqueID() ) {
		pPlayer->SetGuildSelfView( pPacket->GuildSelfView );
		RefreshGuildDlg( true );
		return;
	}

	// 아니라면, 주변에 SessionID가진 녀석 찾아 있다면 적용.
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	if( !hActor ) return;

	CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
	pActor->SetGuildSelfView( pPacket->GuildSelfView );
}

void CDnGuildTask::OnRecvGetGuildInfo( SCGetGuildInfo *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}

#ifdef PRE_ADD_BEGINNERGUILD
	if( m_bChangedGuildChat == false ) {
		if( !m_Guild.IsSet() && pPacket->Info.IsSet() ) {
			if( pPacket->Info.cGuildType == BeginnerGuild::Type::Beginner ) {
				GetInterface().GetChatDialog()->SetChatMode( CHAT_GUILD );
				m_bChangedGuildChat = true;
			}
		}
	}
#endif

	SetGuildInfo( pPacket->Info );

	m_Guild.nGuildWarFinalCount = pPacket->Info.nGuildWarFinalCount;
	m_Guild.nGuildWarFinalWinCount = pPacket->Info.nGuildWarFinalWinCount;

	m_bNotRecruitMember = pPacket->bNotRecruitMember;

	// 아주 가끔 자신이 로그아웃으로 나온다고 한다. 서버 동기화가 조금 늦어진건데, 고쳐달라 하니...
	// 만약 그렇다면 이동중으로 강제 설정한다.
	TGuildMember *pMember = GetMyGuildMemberInfo();
	if( pMember && pMember->Location.cServerLocation == _LOCATION_NONE )
		pMember->Location.cServerLocation = _LOCATION_MOVE;
}
void CDnGuildTask::OnRecvGetGuildMember( SCGetGuildMember *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}
	SetGuildMemberInfo( pPacket->nCount, pPacket->nPage, pPacket->MemberList );

	// 아주 가끔 자신이 로그아웃으로 나온다고 한다. 서버 동기화가 조금 늦어진건데, 고쳐달라 하니...
	// 만약 그렇다면 이동중으로 강제 설정한다.
	TGuildMember *pMember = GetMyGuildMemberInfo();
	if( pMember && pMember->Location.cServerLocation == _LOCATION_NONE )
		pMember->Location.cServerLocation = _LOCATION_MOVE;
	if( pPacket->bEndMember == true )
	{
		m_bIsHaveGuildInfo = true;
		RefreshGuildDlg();
	}	
}

void CDnGuildTask::OnRecvDismissGuild( SCDismissGuild *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}

	m_Guild.Reset();
	m_vecGuildMember.clear();

	RefreshGuildDlg();
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3904 ), false );
}

void CDnGuildTask::OnRecvInviteGuildMemberReq( SCInviteGuildMemberReq *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}

	// 혹시 다른 길드초대 메세지를 받아 처리중이라면, 그냥 패스.
	if( GetInterface().IsShowGuildInviteReqDlg() )
	{
		SendInviteGuildMemberAck( pPacket->GuildUID, pPacket->nFromAccountDBID, pPacket->nFromSessionID, false );
		return;
	}

	// 먼저 기억해두고,
	m_InviteGuildUID = pPacket->GuildUID;
	m_InviteSESID = pPacket->nFromSessionID;
	m_InviteACCID = pPacket->nFromAccountDBID;

	// 블라인드 중이라면,
	if( GetInterface().IsOpenBlind() )
	{
		RequestInviteGuildMemberAck( false );
		return;
	}

	if( GetInterface().GetChatRoomDlg()->IsShow() )
	{
		// 채팅룸이 보이는 상태라면,
		// 현재 채팅룸에 속한 사람인지 판단해서 확인한다.
		if( !GetInterface().GetChatRoomDlg()->IsMyChatRoomMember( m_InviteSESID ) )
		{
			RequestInviteGuildMemberAck( false );
			return;
		}
	}
	else
	if( CDnLocalPlayerActor::IsLockInput() )
	{
		RequestInviteGuildMemberAck( false );
		return;
	}
	
	// #35802 직업변경아이템 창이 떠 있으면 길드 초대 메시지 거절로 자동으로 패킷 보냄.
	if( GetInterface().IsShowChangeJobDialog() )
	{
		RequestInviteGuildMemberAck( false );
		return;
	}

	if( GetInterface().IsShowMapMoveCashItemDlg() )
	{
		RequestInviteGuildMemberAck( false );
		return;
	}

	// 없다면,
	float fTime = _REQUEST_ACCEPT_TOTALTIME;
	// 강제로 자식 링크 거는거기때문에, Process가 2회 호출된다. 그래서 시간 강제로 2배 해준다.
	if( GetInterface().GetChatRoomDlg()->IsShow() )
		fTime *= 2.0f;
	GetInterface().OpenGuildInviteReqDlg( pPacket->wszGuildName, pPacket->wszFromCharacterName, fTime, 0, this );
}

void CDnGuildTask::OnRecvInviteGuildMemberAck( SCInviteGuildMemberAck *pPacket )
{
	// 현재 Ack에 대한 처리는 하지 않는다.
	// 요청 수락하면 그냥 길드원으로 바뀌어서 길드셀프정보 가는거고,
	// 요청 거절하면, 그냥 거절한 걸로 끝이다.
	//
	// 나중에 거절 메세지 추가해달라고 와서 추가함.
	WCHAR wszTemp[128] = { 0, };
	if( pPacket->iErrNo != NOERROR ) {
		if( pPacket->iErrNo == ERROR_GUILD_REFUESED_GUILDINVITATION ) {
			wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3925 ), pPacket->wszToCharacterName );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, true );
		}
		return;
	}

	// 제대로 응답이 왔다면 길드에 가입한 것이다.
	TGuildMember MemberInfo;
	MemberInfo.Set( pPacket->nToAccountDBID, pPacket->nToCharacterDBID, pPacket->wszToCharacterName, 
		pPacket->nJob, pPacket->cLevel, GUILDROLE_TYPE_JUNIOR,
		0, 0, 0, NULL, 0, pPacket->Location.cServerLocation, pPacket->Location.nChannelID, pPacket->Location.nMapIdx );
	AddGuildMember( &MemberInfo );

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if( NULL == pCommunityDlg )
	{
		return;
	}

	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CDnGuildWantedListTabDlg *pGuildWantedListTabDlg = pCommunityDlg->GetGuildWantedListTabDialog();
	if( pGuildDlg && pGuildWantedListTabDlg && pGuildWantedListTabDlg->IsShow() )
	{
		pGuildDlg->Show( true );
		pGuildWantedListTabDlg->Show( false );
	}
#else
	CDnGuildWantedListDlg *pGuildWantedListDlg = pCommunityDlg->GetGuildWantedListDialog();
	if( pGuildDlg && pGuildWantedListDlg && pGuildWantedListDlg->IsShow() )
	{
		pGuildDlg->Show( true );
		pGuildWantedListDlg->Show( false );
	}
#endif

	RefreshGuildDlg();

	wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3905 ), pPacket->wszToCharacterName );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );

	// 자신이 길드에 가입한거라면, 멤버리스트를 받아둔다.
	if( CDnActor::s_hLocalActor && __wcsicmp_l(pPacket->wszToCharacterName, CDnActor::s_hLocalActor->GetName()) == 0 ) 
	{
		m_fDialogShowDelayTime = 0.0f;
		RequestGetGuildInfo( true );
		GetPetTask().DoPetChat( PET_CHAT_USER_JOIN_GUILD );
	}
}

void CDnGuildTask::OnRecvLeaveGuild( SCLeaveGuildMember *pPacket )
{
	if( !CDnActor::s_hLocalActor ) return;

	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}

	// 자신이 길드를 탈퇴하는 경우엔 길드정보를 초기화 하고,
	TGuildMember *pMember = GetGuildMemberFromCharacterDBID( pPacket->nCharacterDBID );
	if( pMember )
	{
		WCHAR wszTemp[128] = { 0, };
		wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3906 ), pMember->wszCharacterName );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );

		if( __wcsicmp_l(pMember->wszCharacterName, CDnActor::s_hLocalActor->GetName()) == 0 )
		{
			m_Guild.Reset();
			m_vecGuildMember.clear();

#ifdef PRE_ADD_BEGINNERGUILD
			if( pPacket->bGraduateBeginnerGuild )
			{
				if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage )
					MessageBoxForGraduateBeginnerGuild();
				else
					m_bMessageBoxForGraduateBeginnerGuild = true;
			}
#endif
		}
		else
		{
			// 다른 캐릭터라면, 캐릭터DBID를 보고 리스트에서 제외시킨다.
			DelGuildMember( pPacket->nCharacterDBID );
		}

		RefreshGuildDlg();
	}
}

void CDnGuildTask::OnRecvExileGuild( SCExileGuildMember *pPacket )
{
	if( !CDnActor::s_hLocalActor ) return;

	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}

	// 자신이 길드에서 추방된 경우엔 길드정보를 초기화 하고,
	TGuildMember *pMember = GetGuildMemberFromCharacterDBID( pPacket->nCharacterDBID );
	if( pMember )
	{
		WCHAR wszTemp[128] = { 0, };
		wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3907 ), pMember->wszCharacterName );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );

		if( __wcsicmp_l(pMember->wszCharacterName, CDnActor::s_hLocalActor->GetName()) == 0 )
		{
			m_Guild.Reset();
			m_vecGuildMember.clear();
		}
		else
		{
			// 다른 캐릭터라면, 캐릭터DBID를 보고 리스트에서 제외시킨다.
			DelGuildMember( pPacket->nCharacterDBID );
		}

		RefreshGuildDlg();
	}
}

void CDnGuildTask::OnRecvChangeGuildInfo( SCChangeGuildInfo *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}
	if( !m_Guild.IsSet() ) {
		_ASSERT(0&&"길드 세팅도 안되어있는데 길드정보 변경이 오나.");
		return;
	}

	// 일부 타입에 대해선 변경 즉시 오는 것이 아니다. 이것들중엔 아예 이 OnRecvChange...에 오지 않는 것도 있다.
	// 부하를 줄이기 위해 GetGuildInfo(주기적 요청 타임)때 길드 정보안에 들어있다는 것이다.
	// 그렇지만, 어쨌든 처리는 해둔다. 혹시 나중에 즉시 응답 오는 것으로 바뀔 수 있으니.

	WCHAR wszTemp[128] = { 0, };
	switch(pPacket->btGuildUpdate)
	{
	case GUILDUPDATE_TYPE_NOTICE:
		{
			//swprintf_s( m_Guild.wszGuildNotice, _countof(m_Guild.wszGuildNotice), pPacket->Text );
			_wcscpy( m_Guild.wszGuildNotice, _countof(m_Guild.wszGuildNotice), pPacket->Text, (int)wcslen(pPacket->Text) );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3902 ), false );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", m_Guild.wszGuildNotice, false );
		}
		break;
	case GUILDUPDATE_TYPE_ROLENAME:
		break;
	case GUILDUPDATE_TYPE_ROLEAUTH:
		{
			m_Guild.wGuildRoleAuth[pPacket->Int1] = pPacket->Int2;
			m_Guild.nRoleMaxItem[pPacket->Int1] = pPacket->Int3;
			m_Guild.nRoleMaxCoin[pPacket->Int1] = pPacket->Int4;
		}
		break;
	case GUILDUPDATE_TYPE_GUILDWAR :
		{
			m_Guild.nGuildWarFinalCount += pPacket->Int1;
			m_Guild.nGuildWarFinalWinCount += pPacket->Int2;
		}
		break;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	case GUILDUPDATE_TYPE_HOMEPAGE:
		{
			_wcscpy( m_Guild.wszGuildHomePage, _countof(m_Guild.wszGuildHomePage), pPacket->Text, (int)wcslen(pPacket->Text) );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100060 ) );
		}
		break;
#endif
	default:
		{
			_ASSERT(0&&"길드정보 변경 타입이상");
		}
		break;
	}

	RefreshGuildDlg();
}

void CDnGuildTask::OnRecvChangeGuildMemberInfo( SCChangeGuildMemberInfo *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}

	if( !m_Guild.IsSet() ) {
		_ASSERT(0&&"길드 세팅도 안되어있는데 길드멤버정보 변경이 오나.");
		return;
	}

	TGuildMember *pMember = GetGuildMemberFromCharacterDBID( pPacket->nChgCharacterDBID );
	if( !pMember ) return;

	TGuildMember *pGulidMaster;

	// 일부 타입에 대해선 변경 즉시 오는 것이 아니다. 이것들중엔 아예 이 OnRecvChange...에 오지 않는 것도 있다.
	// 부하를 줄이기 위해 GetGuildInfo(주기적 요청 타임)때 길드멤버 리스트안에 들어있다는 것이다.
	// 그렇지만, 어쨌든 처리는 해둔다. 혹시 나중에 즉시 응답 오는 것으로 바뀔 수 있으니.

	WCHAR wszRole[32] = { 0, };
	WCHAR wszTemp[128] = { 0, };
	switch(pPacket->btGuildMemberUpdate)
	{
	case GUILDMEMBUPDATE_TYPE_INTRODUCE:
		{
			//swprintf_s( pMember->wszGuildMemberIntroduce, _countof(pMember->wszGuildMemberIntroduce), pPacket->Text );
			_wcscpy( pMember->wszGuildMemberIntroduce, _countof(pMember->wszGuildMemberIntroduce), pPacket->Text, (int)wcslen(pPacket->Text) );
		}
		break;
	case GUILDMEMBUPDATE_TYPE_ROLE:
		{
			pMember->btGuildRole = pPacket->Int1;
			switch(pMember->btGuildRole)
			{
			case GUILDROLE_TYPE_MASTER:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3733 ) );	break;
			case GUILDROLE_TYPE_SUBMASTER:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ) );	break;
			case GUILDROLE_TYPE_SENIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ) );	break;
			case GUILDROLE_TYPE_REGULAR:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ) );	break;
			case GUILDROLE_TYPE_JUNIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ) );	break;
			}
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3903 ), pMember->wszCharacterName, wszRole );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
		}
		break;
	case GUILDMEMBUPDATE_TYPE_GUILDMASTER:
		{
			pMember->btGuildRole = GUILDROLE_TYPE_MASTER;
			swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3733 ) );
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3903 ), pMember->wszCharacterName, wszRole );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );

			pGulidMaster = GetGuildMemberFromCharacterDBID( pPacket->nReqCharacterDBID );
			if( pGulidMaster ) {
				pGulidMaster->btGuildRole = GUILDROLE_TYPE_SUBMASTER;
				swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ) );
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3903 ), pGulidMaster->wszCharacterName, wszRole );
				GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
			}
		}
		break;
	case GUILDMEMBUPDATE_TYPE_CMMPOINT:
		{
			pMember->iUseGuildCommonPoint = pPacket->Int1;
			pMember->iTotGuildCommonPoint = pPacket->Int2;
		}
		break;
	case GUILDMEMBUPDATE_TYPE_LOGINOUT:
		{
			if( pMember != GetMyGuildMemberInfo() )
			{
				if(pPacket->Int1 == _LOCATION_MOVE)
				{
					WCHAR wszTemp[128] = { 0, };
					pMember->Location.cServerLocation = _LOCATION_MOVE;				
					swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3783 ), pMember->wszCharacterName );
					GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
				}
				else if(pPacket->Int1 == _LOCATION_NONE)
					pMember->Location.Reset();
			}
		}
		break;
	default:
		{
			_ASSERT(0&&"길드멤버정보 변경 타입이상");
		}
		break;
	}

	RefreshGuildDlg();
}

void CDnGuildTask::OnRecvGetGuildHistoryList( SCGetGuildHistoryList *pPacket )
{
	if( pPacket->iErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrNo );
		return;
	}

	// 길드다이얼로그가 열려있다면 리프레쉬 호출.
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
	if( !pGuildDlg ) return;
	pGuildDlg->OnRecvGetGuildHistoryList( pPacket );
}

void CDnGuildTask::OnRecvGetGuildWareHistoryList( SCGetGuildWareHistory *pPacket )
{
	if( pPacket->nErrNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->nErrNo );
		return;
	}

#ifdef PRE_ADD_SERVER_WAREHOUSE
	CDnPGStorageTabDlg* pPGStorageDlg = (CDnPGStorageTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG );
	CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageDlg->GetStorageDlg( StorageUIDef::eTAB_GUILD ));
#else // PRE_ADD_ACCOUNT_STORAGE
	CDnPGStorageTabDlg* pPGStorageDlg = (CDnPGStorageTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG );
	CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageDlg->GetStorageDlg( CDnPGStorageTabDlg::GUILD_STORAGE_TAB ));
#endif // PRE_ADD_ACCOUNT_STORAGE
	if( !pGuildStorageDlg ) return;
	pGuildStorageDlg->OnRecvGetGuildStorageHistoryList( pPacket );
}

void CDnGuildTask::OnRecvGuildMemberLoginList( SCGuildMemberLoginList *pPacket )
{
	// 로컬액터가 없거나 pvp태스크가 아닐때 로그인 리스트가 올때가 있다.(빌리지,게임서버 엔터되기 직전)
	// 이땐 자신인지 판단할 수 있는 방법이 없기때문에 자신이름 역시 입장리스트에 뜨게된다.(한번은 길드멤버리스트를 받은 상태)
	// 그래서 이런 상황에선 로그인 리스트를 안찍어주기로 하겠다.
	//
	// 한가지 해결책으로 멤버리스트에 자신임을 체크해두는 방법도 있겠지만, 어차피 중요한 데이터도 아니기때문에 그냥 이렇게 넘기기로 한다.
	if( !CDnActor::s_hLocalActor && !CTaskManager::GetInstance().GetTask( "PVPLobbyTask" ) )
		return;

	WCHAR wszTemp[128] = { 0, };
	for( int i = 0; i < pPacket->nCount; ++i ) {
		TGuildMember *pMember = GetGuildMemberFromCharacterDBID( pPacket->List[i] );
		if( pMember ) {
			if( pMember == GetMyGuildMemberInfo() ) continue;
			if( pMember->Location.cServerLocation != _LOCATION_NONE ) continue;
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3783 ), pMember->wszCharacterName );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
		}
	}
}

void CDnGuildTask::OnRecvOpenGuildWare( SCOpenGuildWare *pPacket )
{
	if( pPacket->nError != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->nError );
		return;
	}

	if( GetCurrentGuildStorageSlotCount() != pPacket->wWareSize )
	{
		m_Guild.wGuildWareSize = pPacket->wWareSize;
		OnChangeGuildWareSize();
	}

	GetItemTask().OnRecvItemOpenGuildWare( pPacket );
}

void CDnGuildTask::OnRecvExtendGuildWare( SCExtendGuildWare *pPacket )
{
	if( GetCurrentGuildStorageSlotCount() != pPacket->wTotalSize )
	{
		m_Guild.wGuildWareSize = pPacket->wTotalSize;
		OnChangeGuildWareSize();
	}
}

void CDnGuildTask::OnRecvChangeGuildName( SCChangeGuildName *pPacket )
{	
	//메세지 처리		
	GetInterface().AddChatMessage( CHATTYPE_GUILD, L"", FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3970 ),
		m_Guild.GuildView.wszGuildName, pPacket->wszGuildName).c_str()); // UISTRING : 길드 이름이 %에서 %s로 변경 되었습니다.

	CDnPVPLobbyVillageTask *pPVPLobbyVillageTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if( pPVPLobbyVillageTask )
	{
		SCEnter Temp = pPVPLobbyVillageTask->GetUserInfo();
		_wcscpy(Temp.GuildSelfView.wszGuildName, _countof(Temp.GuildSelfView.wszGuildName), pPacket->wszGuildName, (int)wcslen(pPacket->wszGuildName));
		pPVPLobbyVillageTask->SetUserInfo(Temp); // 길드정보 동기화

		if(GetInterface().GetPVPLobbyChatTabDlg())
			GetInterface().GetPVPLobbyChatTabDlg()->RefreshMyPVPInfo();
	}

	_wcscpy(m_Guild.GuildView.wszGuildName, _countof(m_Guild.GuildView.wszGuildName), pPacket->wszGuildName, (int)wcslen(pPacket->wszGuildName));
	SetGuildInfo( m_Guild );
	RefreshGuildDlg();
}

void CDnGuildTask::OnRecvEnrollGuildWar( SCEnrollGuildWar *pPacket )
{
	if( pPacket->iErrorNo != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iErrorNo );
		return;
	}

	if( CDnActor::s_hLocalActor ) {
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		TGuildSelfView GuildSelfView;
		GuildSelfView.Set( pPlayer->GetGuildSelfView() );
		GuildSelfView.cTeamColorCode = pPacket->cTeamColorCode;
		GuildSelfView.wWarSchduleID = pPacket->wScheduleID;
		pPlayer->SetGuildSelfView( GuildSelfView ); 
	}

	if( pPacket->cTeamColorCode != 0 )
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126024 ), false );
}

void CDnGuildTask::OnRecvChangeGuildWarEvent( SCGuildWarEvent *pPacket )
{
	m_wCurrentGuldWarScheduleID = pPacket->wScheduleID;
	m_cCurrentGuildWarEventStep = pPacket->cEventStep;

	if( pPacket->cEventType == GUILDWAR_EVENT_START ) {

	}
	else if( pPacket->cEventType == GUILDWAR_EVENT_END ) {
		if( pPacket->cEventStep == GUILDWAR_STEP_REWARD ) {
			m_wCurrentGuldWarScheduleID = 0;
			m_cCurrentGuildWarEventStep = GUILDWAR_STEP_NONE;
		}
	}

	if( pPacket->cEventType == GUILDWAR_EVENT_START )
	{
		int nNoticeStringIndex = 0;
		if( m_cCurrentGuildWarEventStep == GUILDWAR_STEP_PREPARATION )
			nNoticeStringIndex = 126294;
		else if( m_cCurrentGuildWarEventStep == GUILDWAR_STEP_TRIAL )
			nNoticeStringIndex = 126295;
		else if( m_cCurrentGuildWarEventStep == GUILDWAR_STEP_REWARD )
			nNoticeStringIndex = 126296;

		if( nNoticeStringIndex != 0 )
		{
			GetInterface().ShowCaptionDialog( CDnInterface::typeCaption1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nNoticeStringIndex ), textcolor::HOTPINK );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nNoticeStringIndex ), false );
		}
	}
}

void CDnGuildTask::OnRecvGuildWarTournamentWin( SCGuildWarTournamentWin* pPacket )
{
	if( pPacket->cMatchTypeCode == GUILDWAR_FINALPART_FINAL )
	{
		std::wstring wszString;
		wszString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126301 ), pPacket->wszGuildName );

		if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeDungeon )
			GetInterface().ShowGuildWarFinalWinNoticeDlg( true, wszString );

		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString.c_str(), false );
	}
	else
	{
		int nNoticeStringIndex = 0;
		if( pPacket->cMatchTypeCode == GUILDWAR_FINALPART_16 )
			nNoticeStringIndex = 126298;
		else if( pPacket->cMatchTypeCode == GUILDWAR_FINALPART_8 )
			nNoticeStringIndex = 126299;
		else if( pPacket->cMatchTypeCode == GUILDWAR_FINALPART_4 )
			nNoticeStringIndex = 126300;

		if( nNoticeStringIndex != 0 )
		{
			std::wstring wszString;
			wszString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nNoticeStringIndex ), pPacket->wszGuildName );
			GetInterface().ShowCaptionDialog( CDnInterface::typeCaption1, wszString.c_str(), textcolor::HOTPINK );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString.c_str(), false );
		}
	}
}

void CDnGuildTask::OnRecvChangeGuildMark( SCChangeGuildMark *pPacket )
{
	m_Guild.GuildView.wGuildMark = pPacket->wMark;
	m_Guild.GuildView.wGuildMarkBG = pPacket->wMarkBG;
	m_Guild.GuildView.wGuildMarkBorder = pPacket->wMarkBorder;

	if( CDnActor::s_hLocalActor ) {
		CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if( pPlayer->GetGuildSelfView().wGuildMarkBG == 0 && pPlayer->GetGuildSelfView().wGuildMarkBorder == 0 && pPlayer->GetGuildSelfView().wGuildMark == 0 )
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3979 ), false );
		else
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3980 ), false );
		pPlayer->GetGuildSelfView().wGuildMark = pPacket->wMark;
		pPlayer->GetGuildSelfView().wGuildMarkBG = pPacket->wMarkBG;
		pPlayer->GetGuildSelfView().wGuildMarkBorder = pPacket->wMarkBorder;
	}
}

void CDnGuildTask::OnRecvGuildRecruitRegisterInfo( GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket )
{
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if(pCommunityDlg)
	{
		CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
		if( !pGuildDlg ) return;
		pGuildDlg->OnRecvGuildRecruitRegisterInfo( pPacket );
	}
}

void CDnGuildTask::OnRecvGuildRecruitRegister( GuildRecruitSystem::SCGuildRecruitRegister *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterOn || pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterMod )
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3994 ), false );
	else if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterOff )
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3995 ), false );
}

void CDnGuildTask::OnRecvGuildRecruitList( GuildRecruitSystem::SCGuildRecruitList *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CDnGuildWantedListTabDlg *pGuildWantedListTabDlg = pCommunityDlg->GetGuildWantedListTabDialog();
	if( !pGuildWantedListTabDlg ) return;
	pGuildWantedListTabDlg->OnRecvGetGuildRecruitList( pPacket );
#else
	CDnGuildWantedListDlg *pGuildWantedListDlg = pCommunityDlg->GetGuildWantedListDialog();
	if( !pGuildWantedListDlg ) return;
	pGuildWantedListDlg->OnRecvGetGuildRecruitList( pPacket );
#endif
}

void CDnGuildTask::OnRecvGuildRecruitRequestCount( GuildRecruitSystem::SCGuildRecruitRequestCount *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CDnGuildWantedListTabDlg *pGuildWantedListTabDlg = pCommunityDlg->GetGuildWantedListTabDialog();
	if( !pGuildWantedListTabDlg ) return;
	pGuildWantedListTabDlg->OnRecvGetGuildRecruitRequestCount( pPacket );
#else
	CDnGuildWantedListDlg *pGuildWantedListDlg = pCommunityDlg->GetGuildWantedListDialog();
	if( !pGuildWantedListDlg ) return;
	pGuildWantedListDlg->OnRecvGetGuildRecruitRequestCount( pPacket );
#endif
}

void CDnGuildTask::OnRecvGuildRecruitMyList( GuildRecruitSystem::SCMyGuildRecruitList *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CDnGuildWantedListTabDlg *pGuildWantedListTabDlg = pCommunityDlg->GetGuildWantedListTabDialog();
	if( !pGuildWantedListTabDlg ) return;
	pGuildWantedListTabDlg->OnRecvGetGuildRecruitMyList( pPacket );
#else
	CDnGuildWantedListDlg *pGuildWantedListDlg = pCommunityDlg->GetGuildWantedListDialog();
	if( !pGuildWantedListDlg ) return;
	pGuildWantedListDlg->OnRecvGetGuildRecruitMyList( pPacket );
#endif
}

void CDnGuildTask::OnRecvGuildRecruitCharacter( GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
	if( !pGuildDlg ) return;
	pGuildDlg->OnRecvGuildRecruitCharacter( pPacket );
}

void CDnGuildTask::OnRecvGuildRecruitRequest( GuildRecruitSystem::SCGuildRecruitRequest *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	if( pPacket->cRequestType == GuildRecruitSystem::RequestType::RequestOn )
	{
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3992 ), false );

		CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
		CDnGuildWantedListTabDlg *pGuildWantedListTabDlg = pCommunityDlg->GetGuildWantedListTabDialog();
		if( pGuildWantedListTabDlg )
			pGuildWantedListTabDlg->OnRecvGuildRecruitRequest( pPacket );
#else
		CDnGuildWantedListDlg *pGuildWantedListDlg = pCommunityDlg->GetGuildWantedListDialog();
		if( pGuildWantedListDlg )
			pGuildWantedListDlg->OnRecvGuildRecruitRequest( pPacket );
#endif
	}
	else if( pPacket->cRequestType == GuildRecruitSystem::RequestType::RequestOff )
	{
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3999 ), false );
		RequestGetGuildRecruitMyList();
	}

	RequestGetGuildRecruitRequestCount();
}

void CDnGuildTask::OnRecvGuildRecruitAcceptResult( GuildRecruitSystem::SCGuildRecruitAccept *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
	if( !pGuildDlg ) return;
	pGuildDlg->OnRecvGuildRecruitAcceptResult( pPacket );

	if( pPacket->bDelGuildRecruit )
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1592 ), true );
}

void CDnGuildTask::OnRecvGuildRecruitMemberResult( GuildRecruitSystem::SCGuildRecruitMemberResult *pPacket )
{
	if( pPacket->iRet != NOERROR ) {
		GetInterface().ServerMessageBox( pPacket->iRet );
		return;
	}

	int nMsgIndex = 0;
	if( pPacket->cAcceptType == GuildRecruitSystem::AcceptType::AcceptOn ) nMsgIndex = 1581;
	else if( pPacket->cAcceptType == GuildRecruitSystem::AcceptType::AcceptOff ) nMsgIndex = 1582;

	WCHAR wszTemp[128] = {0,};
	swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMsgIndex ), pPacket->wszGuildName );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );

	m_fDialogShowDelayTime = 0.0f;
	RequestGetGuildInfo( true );

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if( !pCommunityDlg ) return;
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CDnGuildWantedListTabDlg *pGuildWantedListTabDlg = pCommunityDlg->GetGuildWantedListTabDialog();
	if( !pGuildDlg || !pGuildWantedListTabDlg ) return;
	pGuildDlg->Show( true );
	pGuildWantedListTabDlg->Show( false );
#else
	CDnGuildWantedListDlg *pGuildWantedListDlg = pCommunityDlg->GetGuildWantedListDialog();
	if( !pGuildDlg || !pGuildWantedListDlg ) return;
	pGuildDlg->Show( true );
	pGuildWantedListDlg->Show( false );
#endif

	if( pPacket->cAcceptType == GuildRecruitSystem::AcceptType::AcceptOn )
		GetPetTask().DoPetChat( PET_CHAT_USER_JOIN_GUILD );
}

void CDnGuildTask::OnRecvGuildLevelUp( SCGuildLevelUp *pPacket )
{
	m_Guild.wGuildLevel = pPacket->nLevel;

	WCHAR wszTemp[128] = {0,};
	swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3765 ), m_Guild.wGuildLevel );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );

	// 길드레벨업 알리미 다이얼로그
	GetInterface().ShowGuildLevelUpAlarmDlg( true, m_Guild.wGuildLevel );
}

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
void CDnGuildTask::OnRecvGuildChat( LPCWSTR wszMsg, INT64 nDBID, bool bIsDoorsMobile )
#else
void CDnGuildTask::OnRecvGuildChat( LPCWSTR wszMsg, INT64 nDBID )
#endif
{
	// 길드챗이 왔다는건 길드에 가입되어있다는 이야기다.
	// 그런데 길드원 정보가 없으면,
	if( !GetGuildMemberCount() ) {
		// 서버 접속하자마자 창을 열면 길드에 가입은 되어있으나, 정보를 얻지 못한 상태일 수 있으므로, 이렇게 처리해야한다.
		GetGuildTask().RequestGetGuildInfo( true );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3963 ), false );
		return;
	}

	TGuildMember *pGuildMember = GetGuildMemberFromCharacterDBID( nDBID );
	if( !pGuildMember ) return;

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	eChatType chatType = CHATTYPE_GUILD;
	if (bIsDoorsMobile)
		chatType = CHATTYPE_GUILD_MOBILE;
	GetInterface().AddChatMessage( chatType, pGuildMember->wszCharacterName, wszMsg );
#else
	GetInterface().AddChatMessage( CHATTYPE_GUILD, pGuildMember->wszCharacterName, wszMsg );
#endif
}

void CDnGuildTask::OnChangeGuildWareSize()
{
	if( !CDnInterface::IsActive() ) return;
	if( !GetInterface().GetMainMenuDialog() ) return;
	CDnPGStorageTabDlg* pPGStorageDlg = static_cast<CDnPGStorageTabDlg*>(GetInterface().GetMainMenuDialog( CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG ));
#ifdef PRE_ADD_SERVER_WAREHOUSE
	CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageDlg->GetStorageDlg(StorageUIDef::eTAB_GUILD));
#else
	CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageDlg->GetStorageDlg(CDnPGStorageTabDlg::GUILD_STORAGE_TAB));
#endif
	if( !pGuildStorageDlg ) return;
	
	//길드 창고 크기
	int nWareSize = m_Guild.wGuildWareSize;
	nWareSize = min(nWareSize, GUILD_WAREHOUSE_MAX);

	pGuildStorageDlg->SetUseItemCnt( nWareSize );
}

int CDnGuildTask::GetCurrentGuildStorageSlotCount()
{
	if( !CDnInterface::IsActive() ) return 0;
	if( !GetInterface().GetMainMenuDialog() ) return 0;
	CDnPGStorageTabDlg* pPGStorageDlg = static_cast<CDnPGStorageTabDlg*>(GetInterface().GetMainMenuDialog( CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG ));
#ifdef PRE_ADD_SERVER_WAREHOUSE
	CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageDlg->GetStorageDlg(StorageUIDef::eTAB_GUILD));
#else
	CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageDlg->GetStorageDlg(CDnPGStorageTabDlg::GUILD_STORAGE_TAB));
#endif

	if( !pGuildStorageDlg ) return 0;
	return pGuildStorageDlg->GetUseItemCnt();
}

void CDnGuildTask::SetGuildInfo( const TGuild &Guild )
{
	if( CDnActor::s_hLocalActor ) {
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		pPlayer->SetGuildView( Guild.GuildView );
	}

	bool bWareSizeChanged = false;
	if( GetCurrentGuildStorageSlotCount() != Guild.wGuildWareSize ) bWareSizeChanged = true;
	m_Guild = Guild;
	if( bWareSizeChanged ) OnChangeGuildWareSize();
}


void CDnGuildTask::SetGuildMemberInfo( int nMemberCount, int nPage, const TGuildMember *pMemberList )
{
	// 어떤 상황에서도 멤버가 0명 이하일땐 없다.
	if( nMemberCount <= 0 ) return;

	if(nPage == 0)
		m_vecGuildMember.clear();
	for( int i = 0; i < nMemberCount; ++i ) {
		m_vecGuildMember.push_back( pMemberList[i] );
	}
}

void CDnGuildTask::AddGuildMember( TGuildMember *pMember )
{
	TGuildMember *pGuildMember = GetGuildMemberFromCharacterDBID( pMember->nCharacterDBID );
	if( pGuildMember )
	{
		_ASSERT(0&&"이미 길드원인데 왜 또 Add하나.");
	}
	else
	{
		m_vecGuildMember.push_back( *pMember );
	}
}

void CDnGuildTask::DelGuildMember( INT64 nDBID )
{
	for( std::vector<TGuildMember>::iterator it = m_vecGuildMember.begin(); it != m_vecGuildMember.end(); ++it ) {
		if( it->nCharacterDBID == nDBID ) {
			m_vecGuildMember.erase( it );
			break;
		}
	}
}

TGuildMember *CDnGuildTask::GetGuildMemberFromArrayIndex( int nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_vecGuildMember.size() ) return NULL;
	return &m_vecGuildMember[nArrayIndex];
}

TGuildMember *CDnGuildTask::GetGuildMemberFromCharacterDBID( INT64 nDBID )
{
	for( int i = 0; i < (int)m_vecGuildMember.size(); ++i ) {
		if( m_vecGuildMember[i].nCharacterDBID == nDBID )
			return &m_vecGuildMember[i];
	}
	return NULL;
}

TGuildMember *CDnGuildTask::GetGuildMemberFromName( LPCWSTR wszName )
{
	for( int i = 0; i < (int)m_vecGuildMember.size(); ++i ) {
		if( __wcsicmp_l( m_vecGuildMember[i].wszCharacterName, wszName ) == 0 )
			return &m_vecGuildMember[i];
	}
	return NULL;
}

TGuildMember *CDnGuildTask::GetMyGuildMemberInfo()
{
	if( CDnActor::s_hLocalActor )
	{
		for( int i = 0; i < (int)m_vecGuildMember.size(); ++i ) {
			if( __wcsicmp_l( m_vecGuildMember[i].wszCharacterName, CDnActor::s_hLocalActor->GetName() ) == 0 )
				return &m_vecGuildMember[i];
		}
	}
	else
	{
		if( CTaskManager::GetInstance().GetTask( "PVPLobbyTask" ) )
		{
			// PVP로비에서는 캐릭터 처리가 필요없어서 s_hLocalActor가 없을 수 있다.
			// 이땐 파티태스크 로컬 데이터에서 얻어와서 비교한다.
			for( int i = 0; i < (int)m_vecGuildMember.size(); ++i ) {
				if( __wcsicmp_l( m_vecGuildMember[i].wszCharacterName, GetPartyTask().GetLocalData()->wszCharacterName ) == 0 )
					return &m_vecGuildMember[i];
			}
		}	
	}
	return NULL;
}

bool CDnGuildTask::GetAuth( eGuildRoleType pGuildRoleType, eGuildAuthType pGuildAuthType )
{
	if (!CHECK_LIMIT( pGuildRoleType, GUILDROLE_TYPE_CNT )) return false;
	return m_Guild.CheckAuth( pGuildRoleType, pGuildAuthType );
}

int CDnGuildTask::GetGuildMemberLogonCount()
{
	int nLogonCount = 0;
	for( int i = 0; i < (int)m_vecGuildMember.size(); ++i ) {
		if( m_vecGuildMember[i].Location.cServerLocation == _LOCATION_NONE ) continue;
		++nLogonCount;
	}
	return nLogonCount;
}

bool CDnGuildTask::IsMaster()
{
	if( !CDnActor::s_hLocalActor ) return false;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	return pPlayer->IsGuildMaster();
}

BYTE CDnGuildTask::GetMyRole()
{
	if( !CDnActor::s_hLocalActor ) return false;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	return pPlayer->GetGuildRole();
}

CDnGuildTask::SGuildLevelInfo *CDnGuildTask::GetGuildLevelInfo( int nGuildLevel )
{
	for( int i = 0; i < (int)m_vecGuildLevelInfo.size(); ++i ) {
		if( m_vecGuildLevelInfo[i].nLevel == nGuildLevel )
			return &m_vecGuildLevelInfo[i];
	}
	return NULL;
}

CDnGuildTask::SGuildLevelInfo *CDnGuildTask::GetGuildLevelInfoFromGuildExp( int nGuildExp )
{
	int nIndex = -1;
	for( int i = 0; i < (int)m_vecGuildLevelInfo.size(); ++i ) {
		if( nGuildExp < m_vecGuildLevelInfo[i].nReqGuildPoint ) {
			nIndex = i-1;
			break;
		}
	}
	if( nIndex >= 0 && nIndex < (int)m_vecGuildLevelInfo.size() ) return &m_vecGuildLevelInfo[nIndex];
	if( nIndex == -1 ) {	// 못얻었으면 최대치 넘은거니 최대레벨 정보 돌려준다.
		if( !m_vecGuildLevelInfo.empty() )
			return &m_vecGuildLevelInfo[m_vecGuildLevelInfo.size()-1];
	}
	return NULL;
}

int CDnGuildTask::GetMaxTakeItemByRole( eGuildRoleType eRole )
{
	if( !m_Guild.IsSet() )
		return -1;

	return m_Guild.nRoleMaxItem[eRole];
}

int CDnGuildTask::GetMaxTakeItemData( int nIndex )
{
	// 만약 인덱스 데이터가 순차적으로 들어있지 않다면 iter로 돌면서 구해줘야한다.
	if( m_mapMaxTakeItemInfo.find(nIndex) != m_mapMaxTakeItemInfo.end() )
		return m_mapMaxTakeItemInfo[nIndex];
	return -1;
}

int CDnGuildTask::GetMaxTakeItemIndexCount()
{
	return (int)m_mapMaxTakeItemInfo.size();
}

int CDnGuildTask::GetMaxWithDrawByRole( eGuildRoleType eRole )
{
	if( !m_Guild.IsSet() )
		return -1;

	return m_Guild.nRoleMaxCoin[eRole];
}

int CDnGuildTask::GetMaxWithDrawData( int nIndex )
{
	// 만약 인덱스 데이터가 순차적으로 들어있지 않다면 iter로 돌면서 구해줘야한다. 우선은 순차적이라 가정한다.
	if( m_mapMaxWithDrawInfo.find(nIndex) != m_mapMaxWithDrawInfo.end() )
		return m_mapMaxWithDrawInfo[nIndex];
	return -1;
}

int CDnGuildTask::GetMaxWithDrawIndexCount()
{
	return (int)m_mapMaxWithDrawInfo.size();
}

void CDnGuildTask::RefreshGuildDlg( bool bRefreshShowing )
{
	// 길드다이얼로그가 열려있다면 리프레쉬 호출.
	if( !CDnInterface::IsActive() ) return;
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if( !pCommunityDlg ) return;
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
	if( !pGuildDlg ) return;

	if( (bRefreshShowing && pGuildDlg->IsShow()) || (bRefreshShowing == false) )
		pGuildDlg->RefreshGuildDlg();
}

static bool CompareGuildMemberName( TGuildMember s1, TGuildMember s2 )
{
	if( __wcsicmp_l( s1.wszCharacterName, s2.wszCharacterName ) > 0 ) return true;
	else if( __wcsicmp_l( s1.wszCharacterName, s2.wszCharacterName ) < 0 ) return false;
	return false;
}
static bool CompareGuildMemberLevel( TGuildMember s1, TGuildMember s2 )
{
	// 고렙이 위로,
	if( s1.cLevel > s2.cLevel ) return true;
	else if( s1.cLevel < s2.cLevel ) return false;
	return false;
}
static bool CompareGuildMemberJob( TGuildMember s1, TGuildMember s2 )
{
	// 워리어가 위로
	if( s1.nJob < s2.nJob ) return true;
	else if( s1.nJob > s2.nJob ) return false;
	return false;
}
static bool CompareGuildMemberRole( TGuildMember s1, TGuildMember s2 )
{
	// 길드장이 위로
	if( s1.btGuildRole < s2.btGuildRole ) return true;
	else if( s1.btGuildRole > s2.btGuildRole ) return false;
	return false;
}
static bool CompareGuildMemberLogoutDate( TGuildMember s1, TGuildMember s2 )
{
	bool bIsSets1 = true;
	bool bIsSets2 = true;
	if( s1.LastLogoutDate == 0 || s1.LastLogoutDate == -1 ) bIsSets1 = false;
	if( s2.LastLogoutDate == 0 || s2.LastLogoutDate == -1 ) bIsSets2 = false;

	// 둘다 설정이 안되어있을 경우엔
	if( bIsSets1 == false && bIsSets2 == false ) return false;
	// s1만 설정되어있을 경우 s1을 위로,
	else if( bIsSets1 == true && bIsSets2 == false ) return true;
	// s2만 설정되어있을 경우 s2를 위로
	else if( bIsSets1 == false && bIsSets2 == true ) return false;

	// 현재에서 가까운게 위로
	if( s1.LastLogoutDate > s2.LastLogoutDate ) return true;
	else if( s1.LastLogoutDate < s2.LastLogoutDate ) return false;
	return false;
}

bool CDnGuildTask::SortGuildMember( eGuildMemberSortType Type, bool bReverse )
{
	static bool (*fp[GuildMemberSort_Amount])( TGuildMember s1, TGuildMember s2 ) = {
		CompareGuildMemberName,
		CompareGuildMemberLevel,
		CompareGuildMemberJob,
		CompareGuildMemberRole,
		CompareGuildMemberLogoutDate
	};

	if( Type >= GuildMemberSort_Name && Type <= GuildMemberSort_LastLogoutDate ) {
		std::sort( m_vecGuildMember.begin(), m_vecGuildMember.end(), fp[Type] );
		if( bReverse )
			std::reverse( m_vecGuildMember.begin(), m_vecGuildMember.end() );
		return true;
	}

	return false;
}

EtTextureHandle CDnGuildTask::GenerateGuildMarkTexture( int nIndex1, int nIndex2, int nIndex3 )
{
	ScopeLock<CSyncLock> Lock( g_pEtRenderLock );

	EtTextureHandle hTexture;

	bool bLoading = false;
	if( CDnLoadingTask::IsActive() )
	{
		// 진짜 로딩화면중이라면 텍스처 생성하지 않는다.
		if( CDnLoadingTask::GetInstance().IsEnableBackgroundLoading() == false )
			bLoading = true;
	}
	if( !bLoading && CDnInterface::IsActive() )
	{
		hTexture = CEtTexture::CreateRenderTargetTexture( 128, 128, FMT_A8R8G8B8 );
		if( hTexture )
		{
			bool bEnableZ = GetEtDevice()->EnableZ( false );	
			bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
			GetEtDevice()->SetDepthStencilSurface( NULL );
			GetEtDevice()->SetVertexShader( NULL );

			GetEtDevice()->SetRenderTarget( hTexture->GetSurfaceLevel() );
			GetEtDevice()->ClearBuffer( 0x0 );

			EtTextureHandle hIconTexture;
			int nTexIndex, nIconIndex;
			SUICoord Coord;

			nTexIndex = nIndex1 / GUILDMARK_TEXTURE_ICON_COUNT;
			hIconTexture = GetInterface().GetGuildMarkIconTex( nTexIndex );
			if( hIconTexture )
			{
				nIconIndex = nIndex1 % GUILDMARK_TEXTURE_ICON_COUNT;
				CalcButtonUV( nIconIndex, hIconTexture, Coord, GUILDMARK_ICON_XSIZE, GUILDMARK_ICON_YSIZE );
				DrawQuadWithTex( &EtVector2(0, 0), &EtVector2(1, 1), &EtVector2(Coord.fX, Coord.fY), &EtVector2(Coord.fX+Coord.fWidth, Coord.fY+Coord.fHeight), hIconTexture->GetTexturePtr() );
			}

			nTexIndex = nIndex2 / GUILDMARK_TEXTURE_ICON_COUNT;
			hIconTexture = GetInterface().GetGuildMarkIconTex( nTexIndex );
			if( hIconTexture )
			{
				nIconIndex = nIndex2 % GUILDMARK_TEXTURE_ICON_COUNT;
				CalcButtonUV( nIconIndex, hIconTexture, Coord, GUILDMARK_ICON_XSIZE, GUILDMARK_ICON_YSIZE );
				DrawQuadWithTex( &EtVector2(0, 0), &EtVector2(1, 1), &EtVector2(Coord.fX, Coord.fY), &EtVector2(Coord.fX+Coord.fWidth, Coord.fY+Coord.fHeight), hIconTexture->GetTexturePtr() );
			}

			nTexIndex = nIndex3 / GUILDMARK_TEXTURE_ICON_COUNT;
			hIconTexture = GetInterface().GetGuildMarkIconTex( nTexIndex );
			if( hIconTexture )
			{
				nIconIndex = nIndex3 % GUILDMARK_TEXTURE_ICON_COUNT;
				CalcButtonUV( nIconIndex, hIconTexture, Coord, GUILDMARK_ICON_XSIZE, GUILDMARK_ICON_YSIZE );
				DrawQuadWithTex( &EtVector2(0, 0), &EtVector2(1, 1), &EtVector2(Coord.fX, Coord.fY), &EtVector2(Coord.fX+Coord.fWidth, Coord.fY+Coord.fHeight), hIconTexture->GetTexturePtr() );
			}

			GetEtDevice()->RestoreRenderTarget();
			GetEtDevice()->RestoreDepthStencil();
			GetEtDevice()->EnableZ( bEnableZ );
			GetEtDevice()->EnableAlphaBlend( bAlphaEnable );
		}
	}

	return hTexture;
}

EtTextureHandle CDnGuildTask::GetGuildMarkTexture( const TGuildView &View )
{
	int nIndex1 = 0;
	int nIndex2 = 0;
	int nIndex3 = 0;

	if( CDnInterface::IsActive() && GetInterface().GetGuildMarkCreateDlg() )
	{
		nIndex1 = GetInterface().GetGuildMarkCreateDlg()->GetMarkInfo( View.wGuildMarkBG ).nIconIndex;
		nIndex2 = GetInterface().GetGuildMarkCreateDlg()->GetMarkInfo( View.wGuildMarkBorder ).nIconIndex;
		nIndex3 = GetInterface().GetGuildMarkCreateDlg()->GetMarkInfo( View.wGuildMark ).nIconIndex;
	}

	for( int i = 0; i < (int)m_vecGuildMarkInfo.size(); ++i )
	{
		if( m_vecGuildMarkInfo[i].nIconIndex[0] == nIndex1 &&
			m_vecGuildMarkInfo[i].nIconIndex[1] == nIndex2 &&
			m_vecGuildMarkInfo[i].nIconIndex[2] == nIndex3 )
		{
			return m_vecGuildMarkInfo[i].hTexture;
		}
	}

	SGuildMarkInfo Info;
	Info.nIconIndex[0] = nIndex1;
	Info.nIconIndex[1] = nIndex2;
	Info.nIconIndex[2] = nIndex3;
	Info.hTexture = GenerateGuildMarkTexture( nIndex1, nIndex2, nIndex3 );
	m_vecGuildMarkInfo.push_back( Info );
	return Info.hTexture;
}

bool CDnGuildTask::IsShowGuildMark( const TGuildView &View )
{
	if( !View.IsSet() ) return false;
	if( View.wGuildMarkBG == 0 && View.wGuildMarkBorder == 0 && View.wGuildMark == 0 ) return false;

	if( GetGuildMarkTexture( View ) )
		return true;

	return false;
}

// 매니저 형태로 핸들 돌려주는거니 지우지 않고 그냥 사용하시면 됩니다.
EtTextureHandle CDnGuildTask::GetGuildMarkTexture( const TGuildSelfView &View )
{
	int nIndex1 = 0;
	int nIndex2 = 0;
	int nIndex3 = 0;

	if( CDnInterface::IsActive() && GetInterface().GetGuildMarkCreateDlg() )
	{
		nIndex1 = GetInterface().GetGuildMarkCreateDlg()->GetMarkInfo( View.wGuildMarkBG ).nIconIndex;
		nIndex2 = GetInterface().GetGuildMarkCreateDlg()->GetMarkInfo( View.wGuildMarkBorder ).nIconIndex;
		nIndex3 = GetInterface().GetGuildMarkCreateDlg()->GetMarkInfo( View.wGuildMark ).nIconIndex;
	}

	for( int i = 0; i < (int)m_vecGuildMarkInfo.size(); ++i )
	{
		if( m_vecGuildMarkInfo[i].nIconIndex[0] == nIndex1 &&
			m_vecGuildMarkInfo[i].nIconIndex[1] == nIndex2 &&
			m_vecGuildMarkInfo[i].nIconIndex[2] == nIndex3 )
		{
			return m_vecGuildMarkInfo[i].hTexture;
		}
	}

	SGuildMarkInfo Info;
	Info.nIconIndex[0] = nIndex1;
	Info.nIconIndex[1] = nIndex2;
	Info.nIconIndex[2] = nIndex3;
	Info.hTexture = GenerateGuildMarkTexture( nIndex1, nIndex2, nIndex3 );
	m_vecGuildMarkInfo.push_back( Info );
	return Info.hTexture;
}

bool CDnGuildTask::IsShowGuildMark( const TGuildSelfView &View )
{
	if( !View.IsSet() ) return false;
	if( View.wGuildMarkBG == 0 && View.wGuildMarkBorder == 0 && View.wGuildMark == 0 ) return false;

	if( GetGuildMarkTexture( View ) )
		return true;

	return false;
}

void CDnGuildTask::OnLostDevice()
{
	for( int i = 0; i < (int)m_vecGuildMarkInfo.size(); ++i )
		SAFE_RELEASE_SPTR( m_vecGuildMarkInfo[i].hTexture );
}

void CDnGuildTask::OnResetDevice()
{
	// OnReset하자마자 다음번 프로세스에서 바로 Generate하니 엔진단에서 뭐가 잘못되었는지 안된다. 그래서 1프레임 건너뛰어 한다.
	m_nMarkGenerateDelayFrameCount = 1;
}

tstring CDnGuildTask::GetGuildMasterName()
{
	tstring strGuildMasterName;

	for( int i = 0; i < (int)m_vecGuildMember.size(); i++ )
	{
		if( m_vecGuildMember[i].btGuildRole == GUILDROLE_TYPE_MASTER )
		{
			strGuildMasterName = m_vecGuildMember[i].wszCharacterName;
			break;
		}
	}

	return strGuildMasterName;
}


void CDnGuildTask::OnRecvUpdateGuildExp( SCUpdateGuildExp *pPacket )
{
	if( pPacket->nError == ERROR_NONE )
	{
		m_Guild.iTotalGuildExp = pPacket->nGuildExp;

		switch( pPacket->cPointType )
		{
		case eGuildPointType::GUILDPOINTTYPE_STAGE:
			{
				TGuildMember *pCurMember = GetGuildTask().GetGuildMemberFromCharacterDBID( pPacket->biCharacterDBID );
				if( !pCurMember ) 
					break;

				WCHAR wszMessage[256] = L"";
				wsprintf( wszMessage, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3361), pPacket->nPointValue);
				CDnInterface::GetInstance().AddChatMessage(CHATTYPE_SYSTEM, L"", wszMessage , false);
			}
			break;
		case eGuildPointType::GUILDPOINTTYPE_MISSION:
			{
				TGuildMember *pCurMember = GetGuildTask().GetGuildMemberFromCharacterDBID( pPacket->biCharacterDBID );
				if( !pCurMember ) 
					break;

				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDAILYMISSION );
				if( !pSox->IsExistItem( pPacket->nMissionID ) ) 
					break;

				WCHAR wszMessage[256] = L"";

				wsprintf( wszMessage , 
					GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3349) , 
					pCurMember->wszCharacterName,
					GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( pPacket->nMissionID , "_TitleNameID" )->GetInteger() ) ,
					pPacket->nPointValue
					);

				CDnInterface::GetInstance().AddChatMessage(CHATTYPE_SYSTEM, L"", wszMessage , false);
			}
			break;
		case eGuildPointType::GUILDPOINTTYPE_WAR:
			{
				WCHAR wszTemp[256] = {0,};
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3363 ), pPacket->nPointValue );
				GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
			}
			break;
		}
	}
	else
	{
		if( pPacket->cPointType == eGuildPointType::GUILDPOINTTYPE_STAGE )
			CDnInterface::GetInstance().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3455) , false);
		else if( pPacket->cPointType == eGuildPointType::GUILDPOINTTYPE_MISSION )
			CDnInterface::GetInstance().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3362) , false);
	}
}


//길드 보상중 기간설정이 없는 아이템중 선행 조건 설정이 있고, 현재 구입한 아이템 보다 낮은 등급 아이템 걸러내기 위한 조건 검사 함수.
bool CDnGuildTask::IsPrerequisite(const GuildReward::GuildRewardInfo &info)
{
	CGuildRewardTable* pGuildRewardTable = CGuildRewardTable::GetInstancePtr();
	if (pGuildRewardTable == NULL)
		return false;

	//기간제인 경우는 조건 체크 하지 않는다.
// 	if (info._Period != 0)
// 		return true;

	//길드 마크인경우
	switch(info._Type)
	{
	case GuildReward::MAKE_GUILDMARK:
		{
			if (info._TypeParam2 == 1)	//신규 제작
			{
				//신규 제작인 경우 마크가 없으면 추가 하고, 마크가 있으면 추가 하지 않도록..
				return (m_Guild.GuildView.wGuildMark == 0);
			}
			else if (info._TypeParam2 == 2)	//마크 변경
			{
				//마크 변경일 경우 마크가 있으면 추가 하고, 마크가 없으면 추가 하지 않도록..
				return (m_Guild.GuildView.wGuildMark != 0);
			}
			else
				return false;
		}
		break;
	case GuildReward::GUILDITEM:
		return true;
	default:
		{
			//같은 타입의 보상을 이미 구입 했는지 확인..
			bool isPurchased = m_GuildRewardItem[info._Type].nItemID != 0;
			if (isPurchased)
			{
				//구입한 길드 보상이 선행 아이템 조건이 설정 되어 있고,
				//선행 아이템 보다 큰 녀석만 표시 하도록 한다..
				GuildReward::GuildRewardInfo* pPurchasedItem = pGuildRewardTable->GetGuildRewardInfo(m_GuildRewardItem[info._Type].nItemID);
				if (pPurchasedItem == NULL)
					return false;

				switch(pPurchasedItem->_PrerequisiteType)
				{
				case GuildReward::GuildRewardPrerequisiteType::PrerequisiteType_REWARDID:	//선행 아이디 확인..
					{
						return (pPurchasedItem->ID == info.ID || pPurchasedItem->_PrerequisiteID < info.ID);
					}
					break;
				case GuildReward::GuildRewardPrerequisiteType::PrerequisiteType_GUILDMARK:	//길드 마크 유무 확인.
					{
						return m_Guild.GuildView.wGuildMark != 0;
					}
					break;
				default:
					return true;
				}
			}
			else
				return true;
		}
		break;
	}
}

bool CDnGuildTask::IsAppliedRewardByBetterThan(const GuildReward::GuildRewardInfo &info)
{
	bool isPurchased = m_GuildRewardItem[info._Type].nItemID != 0;
	if (isPurchased)
	{
		return m_GuildRewardItem[info._Type].nEffectValue >= info._TypeParam1;
	}
	else
		return false;
}

bool CDnGuildTask::IsPurchased(const GuildReward::GuildRewardInfo &info)
{
	//구입했으면 보상 아이디가 설정 되어 있음??
	return m_GuildRewardItem[info._Type].nItemID == info.ID;
}

int CDnGuildTask::GetPurchasedItemID(const GuildReward::GuildRewardInfo &info)
{
	return m_GuildRewardItem[info._Type].nItemID;
}

bool CDnGuildTask::IsAvailableReward(const GuildReward::GuildRewardInfo &info)
{
	bool isAvailable = true;

	CGuildRewardTable* pGuildRewardTable = CGuildRewardTable::GetInstancePtr();
	if (pGuildRewardTable == NULL)
		return isAvailable;

	//길드 레벨 제한 확인..
	if (m_Guild.wGuildLevel < info._NeedGuildLevel)
		return false;

	//길드 마스터 제한 확인..
	if (info._GuildMasterLimit && !IsMaster())
		return false;

	//길드 마크인경우
	if (info._Type == GuildReward::MAKE_GUILDMARK)
	{
		if (info._TypeParam2 == 1)	//신규 제작
		{
			if (m_Guild.GuildView.wGuildMark != 0)	//이미 길드 마크가 있다면
				return false;
		}
		else if (info._TypeParam2 == 2)	//마크 변경
		{
			if (m_Guild.GuildView.wGuildMark == 0)	//아직 길드 마크가 없다면
				return false;
		}
	}
	else
	{
		//같은 타입의 보상을 이미 구입 했는지 확인..
		bool isPurchased = m_GuildRewardItem[info._Type].nItemID != 0;
		
		//실제 구입한 길드 보상 정보를 얻는다.
		GuildReward::GuildRewardInfo* pPurchasedItem = pGuildRewardTable->GetGuildRewardInfo(m_GuildRewardItem[info._Type].nItemID);
		if (isPurchased && pPurchasedItem != NULL)
		{
			switch(pPurchasedItem->_PrerequisiteType)
			{
			case GuildReward::GuildRewardPrerequisiteType::PrerequisiteType_REWARDID:	//선행 아이디 확인..
				{
					isAvailable = pPurchasedItem->ID == info._PrerequisiteID;
				}
				break;
			case GuildReward::GuildRewardPrerequisiteType::PrerequisiteType_GUILDMARK:	//길드 마크 유무 확인.
				{
					isAvailable = m_Guild.GuildView.wGuildMark != 0;
				}
				break;
			default:
				{
					//이미 구입한 창고갯수/인원수 증가 아이템인 경우 선행조건이 없으면 사용 불가
					switch(info._Type)
					{
					case GuildReward::MAX_GUILDWARE:
					case GuildReward::MAX_MEMBER:
						{
							//이미 구입한 아이템인 경우 구입 불가..
							if (pPurchasedItem->ID == info.ID)
								isAvailable = false;
						}
						break;
					}
				}
			}
		}
	}

	return isAvailable;
}

__time64_t CDnGuildTask::GetRemainPeriod(const GuildReward::GuildRewardInfo &info)
{
	__time64_t nRemainPeriod = {0, };

	//구입 
	if (m_GuildRewardItem[info._Type].nItemID != 0)
		return m_GuildRewardItem[info._Type].m_tExpireDate;
	else
		return nRemainPeriod;
}

// bool CDnGuildTask::IsAppliedGuildRewardType(int rewardType)
// {
// 	m_GuildRewardItem[rewardType].nItemID
// 
// 	return false;
// }
// 
// bool CDnGuildTask::IsAppliedGuildRewardType(int rewardType, int rewardID)
// {
// 	GUILDREWARD_MAP::iterator findIter = m_GuildRewardList.find(rewardType);
// 	if (findIter != m_GuildRewardList.end())
// 	{
// 		GUILDREWARDINFO_LIST& list = findIter->second;
// 		GUILDREWARDINFO_LIST::iterator iter = list.begin();
// 		GUILDREWARDINFO_LIST::iterator endIter = list.end();
// 		for (; iter != endIter; ++iter)
// 		{
// 			if (rewardID == iter->ID)
// 				return true;
// 		}
// 
// 		return false;
// 	}
// 	else
// 		return false;
// }

int CDnGuildTask::CollectGuildRewardInfo(GUILDREWARDINFO_LIST &guildRewardList, TGuildRewardItem* pGuildReward)
{
// 	GUILDREWARD_MAP::iterator mapIter = m_GuildRewardList.begin();
// 	GUILDREWARD_MAP::iterator mapEndIter = m_GuildRewardList.end();
// 
// 	for (; mapIter != mapEndIter; ++mapIter)
// 	{
// 		GUILDREWARDINFO_LIST &list = mapIter->second;
// 
// 		GUILDREWARDINFO_LIST::iterator listIter = list.begin();
// 		GUILDREWARDINFO_LIST::iterator listEndIter = list.end();
// 
// 		for (; listIter != listEndIter; ++listIter)
// 		{
// 			GuildReward::GuildRewardInfo &info = (*listIter);
// 
// 			guildRewardList.push_back(info);
// 		}
// 	}

	if (pGuildReward == NULL)
		pGuildReward = m_GuildRewardItem;

	CGuildRewardTable* pGuildRewardTable = CGuildRewardTable::GetInstancePtr();
	for (int i = 0; i < GuildReward::MAX_COUNT; ++i)
	{
		//길드 마크 제작은 스킵..길드 아이템도 스킵
		if (i == GuildReward::MAKE_GUILDMARK || i == GuildReward::GUILDITEM)
			continue;

		if (pGuildReward && pGuildReward[i].nItemID != 0)
		{
			GuildReward::GuildRewardInfo* pGuildRewardInfo = pGuildRewardTable ? pGuildRewardTable->GetGuildRewardInfo(pGuildReward[i].nItemID) : NULL;
			if (pGuildRewardInfo)
				guildRewardList.push_back(*pGuildRewardInfo);
		}
	}

	return (int)guildRewardList.size();
}


void CDnGuildTask::OnRecvGuildGetRewardItem(SCGetGuildRewardItem* pPacket)
{
	if (pPacket == NULL)
		return;

	memcpy( m_GuildRewardItem, pPacket->GuildRewardItem, sizeof(pPacket->GuildRewardItem) );

	RefreshGuildRewardInfo();
}

void CDnGuildTask::OnRecvGuildBuyRewardItem(SCBuyGuildRewardItem* pPacket)
{
	if (pPacket == NULL)
		return;

	if (pPacket->iRet == ERROR_NONE)
	{
		//구입 성공..
		GuildReward::GuildRewardInfo* pRewardInfo = CGuildRewardTable::GetInstance().GetGuildRewardInfo(pPacket->nItemID);
		if (pRewardInfo != NULL)
		{
			int nUiStringID = 3371;

			//아이템인 경우 혜택 표시 스킵..
			switch(pRewardInfo->_Type)
			{
			case GuildReward::GUILDITEM:
			case GuildReward::MAKE_GUILDMARK:
				nUiStringID = 3392;
				break;
			default:
				nUiStringID = 3371;
				break;
			}

			//길드 보상 이름.
			std::wstring msg;
			MakeUIStringUseVariableParam( msg, pRewardInfo->_NameID, (char*)pRewardInfo->_NameIDParam.c_str() );

			WCHAR wszTemp[256] = {0,};
			//“%s 혜택이 적용됩니다.” (MID: 3371)를 길드메시지로 출력합니다.
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUiStringID ), msg.c_str() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
		
			RefreshGuildRewardInfo();
		}
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->iRet );
	}

	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
	if( !pGuildDlg ) return;

	CDnGuildInfoDlg* pGuildInfoDlg = pGuildDlg->GetGuildInfoDlg();
	if (pGuildInfoDlg)
		pGuildInfoDlg->CloseBuyGuildRewardConfirmDlg();
}

void CDnGuildTask::OnRecvGuildAddRewardItem(SCAddGUildRewardItem* pPacket)
{
	if (pPacket == NULL)
		return;

	m_GuildRewardItem[pPacket->GuildRewardItem.nItemType] = pPacket->GuildRewardItem;

	RefreshGuildRewardInfo();

	switch(pPacket->GuildRewardItem.nItemType)
	{
	case GuildReward::MAX_GUILDWARE:
		{
			m_Guild.wGuildWareSize = pPacket->GuildRewardItem.nEffectValue;
			OnChangeGuildWareSize();
		}
		break;
	case GuildReward::EXTRA_FESTPOINT:
		{
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if (pCharStatusDlg)
				pCharStatusDlg->RefreshGuildInfo();
		}
		break;
	}

	RefreshGuildDlg(true);
}

void CDnGuildTask::OnRecvGuildExtendGuildSize(SCExtendGuildSize* pPacket)
{
	if (pPacket == NULL)
		return;

	m_Guild.wGuildSize = pPacket->nGuildSize;

	RefreshGuildDlg( true );
}

void CDnGuildTask::RefreshGuildRewardInfo()
{
	// 길드다이얼로그가 열려있다면 리프레쉬 호출.
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	CDnGuildDlg *pGuildDlg = pCommunityDlg ? pCommunityDlg->GetGuildDialog() : NULL;
	if( !pGuildDlg ) return;

	CDnGuildInfoDlg* pGuildInfoDlg = pGuildDlg->GetGuildInfoDlg();
	if (pGuildInfoDlg)
		pGuildInfoDlg->UpdateGuildRewardInfo();

	GetInterface().UpdateGuildRewardInfo();
}

void CDnGuildTask::RequestBuyGuildRewardItem(int nItemID)
{
	// 길드태스크는 길드정보를 접속 직후 받는게 아니기때문에, 플레이어정보에서 검사하는 것이 확실하다.
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pPlayer->IsJoinGuild() == false )
		return;

	SendBuyGuildRewardItem(nItemID);
}

#ifdef PRE_ADD_GUILD_CONTRIBUTION
void CDnGuildTask::OnRecvGuildContributionPoint( GuildContribution::SCGuildContributionPoint* pPacket )
{
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
	if( !pGuildDlg ) return;

	CDnGuildInfoDlg* pGuildInfoDlg = pGuildDlg->GetGuildInfoDlg();
	if( !pGuildInfoDlg ) return;
		pGuildInfoDlg->OnRecvGetGuildContributionPoint( pPacket );
}

void CDnGuildTask::OnRecvGuildContributionRankList( GuildContribution::SCGuildContributionRank* pPacket )
{
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	CDnGuildDlg *pGuildDlg = pCommunityDlg->GetGuildDialog();
	if( !pGuildDlg ) return;

	CDnGuildInfoDlg* pGuildInfoDlg = pGuildDlg->GetGuildInfoDlg();
	if( !pGuildInfoDlg ) return;
	pGuildInfoDlg->OnRecvGetGuildContributionRankList( pPacket );
}

void CDnGuildTask::RequsetGuildRankList()
{
	SendRequestGuildContributionRank();
}
#endif 

int CDnGuildTask::GetGuildRewardValue(GuildReward::GuildRewardType _type)
{
	int value = 0;
	if (m_GuildRewardItem[_type].nItemID != 0)
		value = (int)m_GuildRewardItem[_type].nEffectValue;

	return value;
}

__time64_t CDnGuildTask::GetGuildRewardExpireDate(GuildReward::GuildRewardType _type)
{
	__time64_t expireDate = {0, };
	if (m_GuildRewardItem[_type].nItemID != 0)
		expireDate = (int)m_GuildRewardItem[_type].m_tExpireDate;

	return expireDate;
}

void CDnGuildTask::RequestPlayerGuildInfo(int nSessionID)
{
	SendRequestPlayerGuildInfo(nSessionID);
}

void CDnGuildTask::OnRecvPlayerGuildInfo(SCPlayerGuildInfo* pPacket)
{
	if (pPacket == NULL)
		return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	CDnPlayerActor* pPlayerActor = NULL;
	if (hActor)
		pPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());

	if (pPlayerActor)
		pPlayerActor->SetPlayerGuildInfo(*pPacket);

	GetInterface().ShowPlayerGuildInfo(true);
}

bool CDnGuildTask::IsAvailableRepurchase(const GuildReward::GuildRewardInfo &info)
{
	bool isAvailable = true;

	switch(info._Type)
	{
	case GuildReward::GUILDITEM:			//길드코스튬
	//case GuildReward::GUILD_GESTURE:		//길드 제스처
	//case GuildReward::MAKE_GUILDMARK:		//길드 마크
		{
			if (info._CheckInven == true)
			{
				eItemTypeEnum nItemType = ITEMTYPE_NORMAL;
				int nGesutreID = -1;

				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
				if( pSox ) 
				{
					nItemType = (eItemTypeEnum)pSox->GetFieldFromLablePtr( info._TypeParam1, "_Type" )->GetInteger();
					nGesutreID = pSox->GetFieldFromLablePtr( info._TypeParam1, "_TypeParam1" )->GetInteger();
				}

				if (nItemType == ITEMTYPE_GESTURE)
				{
					CDnGestureTask::SGestureInfo *pGesture = GetGestureTask().GetGestureInfoFromID( nGesutreID );
					isAvailable = (pGesture == NULL || pGesture->bUsable == false);
				}
				else
				{
					CDnItem *pItem = GetItemTask().FindItem(info._TypeParam1, ITEM_SLOT_TYPE::ST_INVENTORY_CASH);

					//일단 캐쉬인벤만 확인해서 아이템이 없다면 구입한게 아님.
					isAvailable = (pItem == NULL);
				}				
			}
		}
		break;
	case GuildReward::MAX_GUILDWARE:	//길드 창고 슬롯 증가
		{
			//구입 하려는 길드 창고 증가 수가 최대치? 를 넘어 서지 않아야 한다..
			//이전 길드창고증가 아이템 수치와 구입하려는 길드창고증가 수치의 차이값을 더해서
			//최대치를 넘어 서는지 확인 해야한다..
			
			//1. 이전 길드창고 증가 아이템 수치
			int nPreGuildWareItemCount = 0;
			int prevGuildWareItemID = m_GuildRewardItem[GuildReward::MAX_GUILDWARE].nItemID;
			GuildReward::GuildRewardInfo* prevRewardInfo = NULL;

			CGuildRewardTable* pGuildRewardTable = CGuildRewardTable::GetInstancePtr();
			if (pGuildRewardTable)
				prevRewardInfo = pGuildRewardTable->GetGuildRewardInfo(prevGuildWareItemID);
			
			if (prevRewardInfo && prevRewardInfo->_Type == GuildReward::MAX_GUILDWARE)
				nPreGuildWareItemCount = prevRewardInfo->_TypeParam1;

			//2. 이전 길드 창고 증가 수치와 구입 하려는 길드창고 증가 수치의 차이값...
			int nIncCount = info._TypeParam1 - nPreGuildWareItemCount;
			
			//3. 실제 증가 수치가 최대치를 넘어 서는지 확인..
			isAvailable = GUILD_WAREHOUSE_MAX >= (m_Guild.wGuildWareSize + nIncCount);
		}
		break;
	default:
		//치장형이 아니고, 기간제인 경우, 
		if (info._Period != 0)
		{
			int nPreGuildWareItemCount = 0;
			int prevGuildWareItemID = m_GuildRewardItem[info._Type].nItemID;
			GuildReward::GuildRewardInfo* prevRewardInfo = NULL;

			CGuildRewardTable* pGuildRewardTable = CGuildRewardTable::GetInstancePtr();
			if (pGuildRewardTable)
				prevRewardInfo = pGuildRewardTable->GetGuildRewardInfo(prevGuildWareItemID);
			
			//이미 구입한 보상이 없으면 구입 가능 하고
			if (prevRewardInfo == NULL)
				isAvailable = true;
			else
			{
				//이미 구입한 보상의 TypeParam1값이 같거나 큰 경우는 구입 가능.
				if (info._TypeParam1 < prevRewardInfo->_TypeParam1)
					isAvailable = false;
			}

		}
		else
			isAvailable = true;
	}

	return isAvailable;
}

#ifdef PRE_ADD_BEGINNERGUILD
bool CDnGuildTask::IsBeginnerGuild() const
{
	if (m_Guild.IsSet())
		return (m_Guild.cGuildType == BeginnerGuild::Type::Beginner);

	return false;
}

void CDnGuildTask::MessageBoxForGraduateBeginnerGuild()
{
	float fGraduateLevel = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::BeginnerGuild_GraduateLevel );
	int nGraduateLevel = (int)fGraduateLevel;
	WCHAR wzStr[1024] = {0, };
	swprintf_s( wzStr, _countof(wzStr), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1387 ), nGraduateLevel );
	GetInterface().MiddleMessageBox( wzStr, MB_OK, 1, this );
	m_bMessageBoxForGraduateBeginnerGuild = false;
}


#endif