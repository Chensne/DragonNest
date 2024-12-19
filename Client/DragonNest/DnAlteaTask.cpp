#include "stdafx.h"
#include "DnAlteaTask.h"
#include "AlteaSendPacket.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnAlteaTabDlg.h"
#include "DnAlteaCheckDlg.h"
#include "DnAlteaRankDlg.h"
#include "DnAlteaInviteDlg.h"
#include "DnAlteaBoardDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ALTEIAWORLD_EXPLORE )

CDnAlteaTask::CDnAlteaTask()
: CTaskListener( false )
, m_bRequestWait( false )
, m_nDailyClearCount( 0 )
, m_nWeeklyClearCount( 0 )
, m_nTicketCount( 0 )
, m_nMyBestGoldKeyCount( 0 )
, m_nPlaySec( 0 )
, m_nSendTicketCount( 0 )
{

}

CDnAlteaTask::~CDnAlteaTask()
{
	Finalize();
}

bool CDnAlteaTask::Initialize()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TALTEIAWORLDMAP );
	if( NULL == pSox )
		return false;

	int nCount = pSox->GetItemCount();
	for( int itr = 0; itr < nCount; ++itr )
	{
		int nItemID = pSox->GetItemID( itr );
		int nMapType = pSox->GetFieldFromLablePtr( nItemID, "_MapType" )->GetInteger();

		if( E_MAPTYPE_WORLDMAP != nMapType )
			continue;

		int nMapID = pSox->GetFieldFromLablePtr( nItemID, "_MapID" )->GetInteger();
		m_vAlteaWorldMap.push_back( nMapID );
	}

	return true;
}

void CDnAlteaTask::Finalize()
{

}

void CDnAlteaTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch( nMainCmd )
	{
		case SC_ALTEIAWORLD: OnRecvAlteaMessage( nSubCmd, pData, nSize ); break;
	}
}

void CDnAlteaTask::OnRecvAlteaMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd )
	{
		case eAlteiaWorld::SC_ALTEIAWORLD_JOIN_INFO:					OnRecvJoinInfo( (AlteiaWorld::SCAlteiaWorldJoinInfo *)pData ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_PRIVATEGOLDKEYRANKING_INFO :	OnRecvPrivateGoldKeyRankingInfo( (AlteiaWorld::SCAlteiaWorldPrivateGoldKeyRankingInfo *)pData  ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_PRIVATEPLAYTIMERANKING_INFO : OnRecvPrivatePlayTimeRankingInfo( (AlteiaWorld::SCAlteiaWorldPrivatePlayTimeRankingInfo *)pData ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_GUILDGOLDKEYRANKING_INFO :	OnRecvGuildGoldKeyRankingInfo( (AlteiaWorld::SCAlteiaWorldGuildGoldKeyRankingInfo *)pData ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_SENDTICKET_INFO :				OnRecvSendTicketInfo( (AlteiaWorld::SCAlteiaWorldSendTicketInfo *)pData ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_JOIN_RESULT :					OnRecvJoinResult( (AlteiaWorld::SCAlteiaWorldJoinResult *)pData ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_SENDTICKET :					OnRecvSendTicket( (AlteiaWorld::SCAlteiaWorldSendTicket *)pData ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_SENDTICKET_RESULT :			OnRecvSendTicketResult( (AlteiaWorld::SCAlteiaWorldSendTicketResult *)pData ); break;
		case eAlteiaWorld::SC_ALTEIAWORLD_DICE_RESULT :					OnRecvDiceResult( (AlteiaWorld::SCAlteiaWorldDiceResult *)pData ); break;
	}
}

/////////////////////////////////////////////////////////////
//	Recv

void CDnAlteaTask::OnRecvJoinInfo( AlteiaWorld::SCAlteiaWorldJoinInfo * pPacket )
{
	if( NULL == pPacket )
		return;

	m_nDailyClearCount = pPacket->cDailyClearCount;
	m_nWeeklyClearCount = pPacket->cWeeklyClearCount;
	m_nTicketCount = pPacket->cTicketCount;

	RefreshTabDlg();
	RefreshCheckDlg();
}

void CDnAlteaTask::OnRecvPrivateGoldKeyRankingInfo( AlteiaWorld::SCAlteiaWorldPrivateGoldKeyRankingInfo * pPacket )
{
	if( NULL == pPacket )
		return;

	m_nMyBestGoldKeyCount = pPacket->nMyBestGoldKeyCount;

	m_vPersonalGoldKey.clear();
	for( int itr = 0; itr < pPacket->nCount; ++itr )
		m_vPersonalGoldKey.push_back( pPacket->MemberInfo[itr] );
}

void CDnAlteaTask::OnRecvPrivatePlayTimeRankingInfo( AlteiaWorld::SCAlteiaWorldPrivatePlayTimeRankingInfo * pPacket )
{
	if( NULL == pPacket )
		return;

	m_nPlaySec = pPacket->nPlaySec;

	m_vPersonalPlayTime.clear();
	for( int itr = 0; itr < pPacket->nCount; ++itr )
		m_vPersonalPlayTime.push_back( pPacket->MemberInfo[itr] );
}

void CDnAlteaTask::OnRecvGuildGoldKeyRankingInfo( AlteiaWorld::SCAlteiaWorldGuildGoldKeyRankingInfo * pPacket )
{
	if( NULL == pPacket )
		return;

	m_nMyBestGoldKeyCount = pPacket->nMyBestGoldKeyCount;

	m_vGuildGoldKey.clear();
	for( int itr = 0; itr < pPacket->nCount; ++itr )
		m_vGuildGoldKey.push_back( pPacket->GuildRankInfo[itr] );
}

void CDnAlteaTask::OnRecvSendTicketInfo( AlteiaWorld::SCAlteiaWorldSendTicketInfo * pPacket )
{
	if( NULL == pPacket )
		return;

	m_nSendTicketCount = pPacket->cSendTicketCount;

	m_vSendUser.clear();
	for( int itr = 0; itr < AlteiaWorld::Common::MaxSendCount; ++itr )
	{
		if( 0 >= pPacket->biCharacterDBID[itr] )
			break;

		m_vSendUser.push_back( pPacket->biCharacterDBID[itr] );
	}
}

void CDnAlteaTask::OnRecvJoinResult( AlteiaWorld::SCAlteiaWorldJoinResult * pPacket )
{
	if( NULL == pPacket )
		return;

	if( ERROR_NONE != pPacket->nRet )
		GetInterface().ServerMessageBox( pPacket->nRet );
}

void CDnAlteaTask::OnRecvSendTicket( AlteiaWorld::SCAlteiaWorldSendTicket * pPacket )
{
	if( NULL == pPacket )
		return;

	m_nSendTicketCount = pPacket->nTicketCount;

	WCHAR wszString[256] = {0,};
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7971), pPacket->wszCharacterName );		// UISTRING : %s님이 알테이아대륙탐험 입장권을 보냈습니다.
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString );

	RefreshInviteDlg();
}

void CDnAlteaTask::OnRecvSendTicketResult( AlteiaWorld::SCAlteiaWorldSendTicketResult * pPacket )
{
	if( NULL == pPacket )
		return;

	m_nSendTicketCount = pPacket->nSendTicketCount;
	RefreshInviteDlg();

	if( ERROR_NONE != pPacket->nRet )
	{
		if( ERROR_ALTEIAWORLD_NOTEXISTUSER == pPacket->nRet )
			GetInterface().MessageBox( 8332 );		// UISTRING : 대상을 찾을 수 없습니다.
		else
			GetInterface().ServerMessageBox( pPacket->nRet );
		return;
	}
	else
	{
		WCHAR wszString[256] = {0,};
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7969), m_wszSendUserName.c_str() );		// UISTRING : %s님에게 찢어진 지도 조각 1개를 보냈습니다.
		GetInterface().MessageBox( wszString );
	}
}

void CDnAlteaTask::OnRecvDiceResult( AlteiaWorld::SCAlteiaWorldDiceResult * pPacket )
{
	if( NULL == pPacket )
		return;

	if( ERROR_NONE != pPacket->nRet )
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
		return;
	}

	CDnAlteaBoardDlg * pAlteaBoardDlg = GetInterface().GetAlteaBoard();
	if( NULL == pAlteaBoardDlg )
		return;

	pAlteaBoardDlg->SetDiceResult( pPacket->nNumber );
}

/////////////////////////////////////////////////////////////
//	Request

void CDnAlteaTask::RequestAlteaInfo( const AlteiaWorld::Info::eInfoType eType )
{
	SendAlteaInfo( eType );
}

void CDnAlteaTask::RequestAlteaJoin()
{
	SendAlteaJoin();
}

void CDnAlteaTask::RequestAlteaSendTicket( const WCHAR * wszCharacterName )
{
	m_wszSendUserName = std::wstring( wszCharacterName );

	SendAlteaSendTicket( wszCharacterName );
}

void CDnAlteaTask::RequestAlteaDice()
{
	SendAlteaDice();
}

void CDnAlteaTask::RequestAlteaQuitInfo()
{
	SendAlteaQuitInfo();
}

void CDnAlteaTask::RequestAlteaNextMap()
{
	SendAlteaNextMap();
}

/////////////////////////////////////////////////////////////
//	Refresh Dlg

void CDnAlteaTask::RefreshTabDlg()
{
	CDnAlteaTabDlg * pAlteaTabDlg = (CDnAlteaTabDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ALTEA_TAB_DIALOG);
	if( NULL == pAlteaTabDlg )
		return;

	pAlteaTabDlg->SetCount( m_nDailyClearCount, m_nTicketCount );
}

void CDnAlteaTask::RefreshCheckDlg()
{
	CDnAlteaTabDlg * pAlteaTabDlg = (CDnAlteaTabDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ALTEA_TAB_DIALOG);
	if( NULL == pAlteaTabDlg )
		return;

	CDnAlteaCheckDlg * pAlteaCheckDlg = pAlteaTabDlg->GetAlteaCheckDlg();
	if( NULL == pAlteaCheckDlg )
		return;

	pAlteaCheckDlg->SetWeeklyClearCount( m_nWeeklyClearCount );
}

void CDnAlteaTask::RefreshPersonalGoldKeyRankDlg()
{
	CDnAlteaTabDlg * pAlteaTabDlg = (CDnAlteaTabDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ALTEA_TAB_DIALOG);
	if( NULL == pAlteaTabDlg )
		return;

	CDnAlteaRankDlg * pAlteaRankDlg = pAlteaTabDlg->GetPersonalRankDlg();
	if( NULL == pAlteaRankDlg )
		return;

	pAlteaRankDlg->SetMyGoldKeyCount( m_nMyBestGoldKeyCount );
	pAlteaRankDlg->SetPersonalGoldKeyRank( m_vPersonalGoldKey );
}

void CDnAlteaTask::RefreshPersonalPlayTimeRankDlg()
{
	CDnAlteaTabDlg * pAlteaTabDlg = (CDnAlteaTabDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ALTEA_TAB_DIALOG);
	if( NULL == pAlteaTabDlg )
		return;

	CDnAlteaRankDlg * pAlteaRankDlg = pAlteaTabDlg->GetPersonalRankDlg();
	if( NULL == pAlteaRankDlg )
		return;

	pAlteaRankDlg->SetMyPlayTime( m_nPlaySec );
	pAlteaRankDlg->SetPersonalPlayTimeRank( m_vPersonalPlayTime );
}

void CDnAlteaTask::RefreshGuildGoldKeyRankDlg()
{
	CDnAlteaTabDlg * pAlteaTabDlg = (CDnAlteaTabDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ALTEA_TAB_DIALOG);
	if( NULL == pAlteaTabDlg )
		return;

	CDnAlteaRankDlg * pAlteaRankDlg = pAlteaTabDlg->GetGuildRankDlg();
	if( NULL == pAlteaRankDlg )
		return;

	pAlteaRankDlg->SetMyGoldKeyCount( m_nMyBestGoldKeyCount );
	pAlteaRankDlg->SetGuildGoldKeyRank( m_vGuildGoldKey );
}

void CDnAlteaTask::RefreshInviteDlg()
{
	CDnAlteaTabDlg * pAlteaTabDlg = (CDnAlteaTabDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ALTEA_TAB_DIALOG);
	if( NULL == pAlteaTabDlg )
		return;

	CDnAlteaInviteDlg * pAlteaInviteDlg = pAlteaTabDlg->GetInviteDlg();
	if( NULL == pAlteaInviteDlg )
		return;

	pAlteaInviteDlg->SetSendTicketCount( m_nSendTicketCount );
	pAlteaInviteDlg->SetSendUser( m_vSendUser );
}

bool CDnAlteaTask::IsAlteaWorldMap( const int nMapIndex )
{
	for( DWORD itr = 0; itr < m_vAlteaWorldMap.size(); ++itr )
	{
		if( nMapIndex == m_vAlteaWorldMap[itr] )
			return true;
	}

	return false;
}

#endif	//	#if defined( PRE_ALTEIAWORLD_EXPLORE )