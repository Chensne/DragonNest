#include "StdAfx.h"
#include "DnChannelChatTask.h"
#include "ClientSessionManager.h"
#include "DnCommunityDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnPrivateChannelDlg.h"
#include "DnPVPLobbyVillageTask.h"
#include "TaskManager.h"
#include "DnChatTabDlg.h"
#include "DnChatOption.h"


#ifdef PRE_PRIVATECHAT_CHANNEL

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnChannelChatTask::CDnChannelChatTask()
: CTaskListener( false )
, m_bJoinChannel( false )
, m_bChannelMaster( false )
{
}

CDnChannelChatTask::~CDnChannelChatTask()
{
	m_vecChannelMemberInfo.clear();
}

bool CDnChannelChatTask::Initialize()
{
	return true;
}

void CDnChannelChatTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( nMainCmd == SC_PRIVATECHAT_CHANNEL )
	{
		switch( nSubCmd )
		{
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_INFO:			OnRecvChannelInfo( (PrivateChatChannel::SCPrivateChannleInfo*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_JOINRESULT:	OnRecvChannelJoinResult( (PrivateChatChannel::SCPrivateChatChannleJoinResult*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_OUTRESULT:		OnRecvChannelOutResult( (PrivateChatChannel::SCPrivateChatChannleOutResult*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_ADD:			OnRecvChannelAdd( (PrivateChatChannel::SCPrivateChatChannleAdd*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_DEL:			OnRecvChannelDel( (PrivateChatChannel::SCPrivateChatChannleDel*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_MOD:			OnRecvChannelMod( (PrivateChatChannel::SCPrivateChatChannleMod*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_KICK:			OnRecvChannelKick( (PrivateChatChannel::SCPrivateChatChannleKick*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_RESULT:		OnRecvChannelResult( (PrivateChatChannel::SCPrivateChatChannleResult*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_INVITERESULT:	OnRecvChannelInviteResult( (PrivateChatChannel::SCPrivateChatChannleInviteResult*)pData ); break;
			case ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_KICKRESULT:	OnRecvChannelKickResult( (PrivateChatChannel::CSPrivateChatChannleKickResult*)pData ); break;
		}
	}
}

void CDnChannelChatTask::OnRecvChannelInfo( PrivateChatChannel::SCPrivateChannleInfo* pPacket )
{
	if( pPacket == NULL || !m_bJoinChannel )
		return;

	std::wstring szMyCharacterName;
	CDnPVPLobbyVillageTask* pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	{
		if( CDnActor::s_hLocalActor )
		{
			szMyCharacterName = CDnActor::s_hLocalActor->GetName();
		}
		else
		{
			CDnPVPLobbyVillageTask* pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
			if( pPVPLobbyTask )
				 szMyCharacterName = pPVPLobbyTask->GetUserInfo().wszCharacterName;
		}
	}

	if( m_strChannelName.empty() )
	{
		m_strChannelName = pPacket->wszChannelName;
		std::wstring strSystemMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7809 ), m_strChannelName.c_str() );
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), strSystemMsg.c_str(), false );
#else // PRE_ADD_PRIVATECHAT_CHANNEL
		GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), strSystemMsg.c_str(), false );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
	}

	m_vecChannelMemberInfo.clear();
	for( int i=0; i<pPacket->nCount; i++ )
	{
		m_vecChannelMemberInfo.push_back( pPacket->Members[i] );
		if( pPacket->Members[i].bMaster && __wcsicmp_l( pPacket->Members[i].wszCharacterName, szMyCharacterName.c_str() ) == 0 ) 
		{
			m_bChannelMaster = true;
		}
	}

	SortChannelMemberInfo();

	CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if( pCommunityDlg )
	{
		CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
		if( pPrivateChannelDlg && pPrivateChannelDlg->IsShow() )
			pPrivateChannelDlg->ShowChannelJoinMode();
	}
}

void CDnChannelChatTask::OnRecvChannelJoinResult( PrivateChatChannel::SCPrivateChatChannleJoinResult* pPacket )
{
	if( pPacket == NULL )
		return;

	// 채널에 비밀번호 걸려있는 경우
	if( pPacket->nRet == PrivateChatChannel::Common::NeedPassWord )
	{
		CDnPrivateChannelPasswordDlg* pDnPrivateChannelPasswordDlg = GetInterface().GetPrivateChannelPasswordDlg();
		if( pDnPrivateChannelPasswordDlg )
		{
			pDnPrivateChannelPasswordDlg->SetChannelName( pPacket->wszChannelName );
#ifndef PRE_ADD_PRIVATECHAT_CHANNEL
			pDnPrivateChannelPasswordDlg->SetChannelPasswordType( CDnPrivateChannelPasswordDlg::TYPE_PASSWORD_JOIN );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
			pDnPrivateChannelPasswordDlg->Show( true );
		}
		return;
	}

	int nErrorMsgId = 0;
	switch( pPacket->nRet )
	{
	case ERROR_NONE:
		m_bJoinChannel = true;
		break;
	case ERROR_NOTEXIST_CHANNELNAME:
		nErrorMsgId = 7815;
		break;
	case ERROR_INVALID_CHANNELNAME:
		nErrorMsgId = 7811;
		break;
	case ERROR_DUPLICATE_CHANNELJOIN:
		nErrorMsgId = 7813;
		break;
	case ERROR_MAX_CHANNELMEMBER:
		nErrorMsgId = 7828;
		break;
	}

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	if( m_bJoinChannel )
	{
		if( GetInterface().GetChatDialog() )
			GetInterface().GetChatDialog()->SetPrivateChannelNotify( true );

		CDnPrivateChannelPasswordDlg* pDnPrivateChannelPasswordDlg = GetInterface().GetPrivateChannelPasswordDlg();
		if( pDnPrivateChannelPasswordDlg )
			pDnPrivateChannelPasswordDlg->Show( false );
	}
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	if( nErrorMsgId > 0 )
	{
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorMsgId ), false );
	}
}

void CDnChannelChatTask::OnRecvChannelOutResult( PrivateChatChannel::SCPrivateChatChannleOutResult* pPacket )
{
	if( pPacket == NULL || !m_bJoinChannel )
		return;

	if( pPacket->nRet == ERROR_NONE )
	{
		m_bChannelMaster = false;
		m_bJoinChannel = false;
		m_strChannelName.clear();
		m_vecChannelMemberInfo.clear();

		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if( pCommunityDlg )
		{
			CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
			if( pPrivateChannelDlg && pPrivateChannelDlg->IsShow() )
				pPrivateChannelDlg->ShowChannelCreateMode();
		}

		GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7826 ), false );

		if( GetInterface().GetChatDialog()->GetChatMode() == CHAT_PRIVATE_CHANNEL )
			GetInterface().GetChatDialog()->SetChatMode( CHAT_NORMAL );
	}
}

void CDnChannelChatTask::OnRecvChannelAdd( PrivateChatChannel::SCPrivateChatChannleAdd* pPacket )
{
	if( pPacket == NULL || !m_bJoinChannel )
		return;

	if( pPacket->nRet == ERROR_NONE )
	{
		bool bIsExistMember = false;	// 채널 참가자가 이름 바꿀 경우 이름만 갱신 함
		for( int i=0; i<static_cast<int>( m_vecChannelMemberInfo.size() ); i++ )
		{
			if( m_vecChannelMemberInfo[i].biCharacterDBID == pPacket->Member.biCharacterDBID )
			{
				bIsExistMember = true;
				_wcscpy( m_vecChannelMemberInfo[i].wszCharacterName, _countof(m_vecChannelMemberInfo[i].wszCharacterName), pPacket->Member.wszCharacterName, (int)wcslen(pPacket->Member.wszCharacterName) );
				break;
			}
		}

		if( !bIsExistMember )
		{
			std::wstring strSystemMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7814 ), pPacket->Member.wszCharacterName );
			GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), strSystemMsg.c_str(), false );

			m_vecChannelMemberInfo.push_back( pPacket->Member );
		}

		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if( pCommunityDlg )
		{
			CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
			if( pPrivateChannelDlg && pPrivateChannelDlg->IsShow() )
				pPrivateChannelDlg->ShowChannelJoinMode();
		}
	}
}

void CDnChannelChatTask::OnRecvChannelDel( PrivateChatChannel::SCPrivateChatChannleDel* pPacket )
{
	if( pPacket == NULL || !m_bJoinChannel )
		return;

	if( pPacket->nRet == ERROR_NONE )
	{
		std::wstring strSystemMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7827 ), pPacket->wszCharacterName );
		GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), strSystemMsg.c_str(), false );

		std::vector<PrivateChatChannel::TMemberInfo>::iterator iter = m_vecChannelMemberInfo.begin();
		for( ; iter != m_vecChannelMemberInfo.end(); iter++ )
		{
			if( (*iter).biCharacterDBID == pPacket->biCharacterDBID )
			{
				m_vecChannelMemberInfo.erase( iter );
				break;
			}
		}

		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if( pCommunityDlg )
		{
			CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
			if( pPrivateChannelDlg && pPrivateChannelDlg->IsShow() )
				pPrivateChannelDlg->ShowChannelJoinMode();
		}
	}
}

void CDnChannelChatTask::OnRecvChannelMod( PrivateChatChannel::SCPrivateChatChannleMod* pPacket )
{
	if( pPacket == NULL || !m_bJoinChannel )
		return;

	std::wstring szMyCharacterName;
	CDnPVPLobbyVillageTask* pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	{
		if( CDnActor::s_hLocalActor )
		{
			szMyCharacterName = CDnActor::s_hLocalActor->GetName();
		}
		else
		{
			CDnPVPLobbyVillageTask* pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
			if( pPVPLobbyTask )
				szMyCharacterName = pPVPLobbyTask->GetUserInfo().wszCharacterName;
		}
	}

	if( pPacket->nModType == PrivateChatChannel::Common::ChangePassWord )
	{
		if( pPacket->nRet == ERROR_NONE )
		{
			std::wstring strSystemMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7818 ), pPacket->wszCharacterName );
			GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), strSystemMsg.c_str(), false );
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
			CDnPrivateChannelPasswordChangeDlg* pDnPrivateChannelPasswordChangeDlg = GetInterface().GetPrivateChannelPasswordChangeDlg();
			if( pDnPrivateChannelPasswordChangeDlg )
			{
				pDnPrivateChannelPasswordChangeDlg->Show( false );
			}
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
		}
	}
	else if( pPacket->nModType == PrivateChatChannel::Common::ChangeMaster )
	{
		std::vector<PrivateChatChannel::TMemberInfo>::iterator iter = m_vecChannelMemberInfo.begin();
		for( ; iter != m_vecChannelMemberInfo.end(); iter++ )
		{
			if( (*iter).biCharacterDBID == pPacket->biCharacterDBID )
			{
				(*iter).bMaster = true;
				if( __wcsicmp_l( (*iter).wszCharacterName, szMyCharacterName.c_str() ) == 0 )
				{
					m_bChannelMaster = true;
				}
				else
				{
					m_bChannelMaster = false;
				}
			}
			else
			{
				(*iter).bMaster = false;
			}
		}

		SortChannelMemberInfo();

		std::wstring strSystemMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7816 ), pPacket->wszCharacterName );
		GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), strSystemMsg.c_str(), false );

		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if( pCommunityDlg )
		{
			CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
			if( pPrivateChannelDlg && pPrivateChannelDlg->IsShow() )
				pPrivateChannelDlg->ShowChannelJoinMode();
		}
	}
}

void CDnChannelChatTask::OnRecvChannelKick( PrivateChatChannel::SCPrivateChatChannleKick* pPacket )
{
	if( pPacket == NULL )
		return;

	if( pPacket->nRet == ERROR_NONE )
	{
		std::vector<PrivateChatChannel::TMemberInfo>::iterator iter = m_vecChannelMemberInfo.begin();
		for( ; iter != m_vecChannelMemberInfo.end(); iter++ )
		{
			if( (*iter).biCharacterDBID == pPacket->biCharacterDBID )
			{
				std::wstring strSystemMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7822 ), (*iter).wszCharacterName );
				GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), strSystemMsg.c_str(), false );

				m_vecChannelMemberInfo.erase( iter );

				CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
				if( pCommunityDlg )
				{
					CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
					if( pPrivateChannelDlg && pPrivateChannelDlg->IsShow() )
						pPrivateChannelDlg->ShowChannelJoinMode();
				}
				break;
			}
		}
	}
}

void CDnChannelChatTask::OnRecvChannelResult( PrivateChatChannel::SCPrivateChatChannleResult* pPacket )
{
	if( pPacket == NULL )
		return;

	int nErrorMsgId = 0;
	switch( pPacket->nRet )
	{
		case ERROR_NONE:
			m_bJoinChannel = true;
			break;
		case ERROR_ALREADY_CHANNELNAME:
			nErrorMsgId = 7810;
			break;
		case ERROR_INVALID_CHANNELNAME:
			nErrorMsgId = 7811;
			break;
		case ERROR_INVALID_CHANNELNAMELENGTH:
			nErrorMsgId = 7812;
			break;
		case ERROR_DUPLICATE_CHANNELJOIN:
			nErrorMsgId = 7813;
			break;
		case ERROR_NOTEXIST_CHANNELNAME:
			nErrorMsgId = 7815;
			break;
		case ERROR_INVALID_PRIVATECHATCHANNELPASSWORD:
			nErrorMsgId = 7819;
			break;
		case ERROR_INVALID_PIRVATECHATMEMBERNAME:
			nErrorMsgId = 7824;
			break;
		case ERROR_MAX_CHANNELMEMBER:
			nErrorMsgId = 7828;
			break;
		case ERROR_NOTEXIST_CHANNELMEMBER:
			nErrorMsgId = 7825;
			break;
	}

	if( nErrorMsgId > 0 )
	{
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
		bool bShowWindow = false;
		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if( pCommunityDlg )
		{
			CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
			if( pPrivateChannelDlg && pPrivateChannelDlg->IsShowChannelInputDlg() )
				bShowWindow = true;
		}
		if( GetInterface().GetPrivateChannelPasswordDlg() && GetInterface().GetPrivateChannelPasswordDlg()->IsShow() )
			bShowWindow = true;

		if( bShowWindow )
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorMsgId ), MB_OK );
		else
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorMsgId ), false );
#else // PRE_ADD_PRIVATECHAT_CHANNEL
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorMsgId ), false );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
	}
}

void CDnChannelChatTask::OnRecvChannelInviteResult( PrivateChatChannel::SCPrivateChatChannleInviteResult* pPacket )
{
	if( pPacket == NULL )
		return;

	if( pPacket->nRet != ERROR_NONE )
	{
		int nErrorMsgId = 0;
		switch( pPacket->nRet )
		{
			case ERROR_NOTEXIST_INVITEUSER:
				nErrorMsgId = 7820;
				break;
			case ERROR_ALREADY_JOINCHANNEL:
				nErrorMsgId = 7821;
				break;
		}

		if( nErrorMsgId > 0 )
		{
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorMsgId ), false );
		}
	}
}

void CDnChannelChatTask::OnRecvChannelKickResult( PrivateChatChannel::CSPrivateChatChannleKickResult* pPacket )
{
	if( pPacket == NULL )
		return;

	if( pPacket->nRet == ERROR_NONE )
	{
		if( pPacket->bMe )
		{
			m_bChannelMaster = false;
			m_bJoinChannel = false;
			m_strChannelName.clear();
			m_vecChannelMemberInfo.clear();

			CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
			if( pCommunityDlg )
			{
				CDnPrivateChannelDlg* pPrivateChannelDlg = pCommunityDlg->GetPrivateChannelDlg();
				if( pPrivateChannelDlg && pPrivateChannelDlg->IsShow() )
					pPrivateChannelDlg->ShowChannelCreateMode();
			}

			GetInterface().AddChatMessage( CHATTYPE_PRIVATECHANNEL, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7823 ), false );

			if( GetInterface().GetChatDialog()->GetChatMode() == CHAT_PRIVATE_CHANNEL )
				GetInterface().GetChatDialog()->SetChatMode( CHAT_NORMAL );
		}
	}
}

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
void CDnChannelChatTask::SendChannelAdd( const WCHAR* pChannelName, int nChannelPassword )
#else // PRE_ADD_PRIVATECHAT_CHANNEL
void CDnChannelChatTask::SendChannelAdd( const WCHAR* pChannelName )
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
{
	PrivateChatChannel::CSPrivateChatChannleAdd PrivateChatChannelAdd;
	memset( &PrivateChatChannelAdd, 0, sizeof( PrivateChatChannelAdd ) );
	_wcscpy( PrivateChatChannelAdd.wszChannelName, _countof(PrivateChatChannelAdd.wszChannelName), pChannelName, (int)wcslen(pChannelName) );
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	PrivateChatChannelAdd.nPassWord = nChannelPassword;
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	CClientSessionManager::GetInstance().SendPacket( CS_PRIVATECHAT_CHANNEL, ePrivateChatChannel::CS_PRIVATECHAT_CHANNEL_ADD, (char*)&PrivateChatChannelAdd, int(sizeof(PrivateChatChannelAdd)) );
}

void CDnChannelChatTask::SendChannelJoin( const WCHAR* pChannelName, int nChannelPassword )
{
	PrivateChatChannel::CSPrivateChatChannleJoin PrivateChatChannelJoin;
	memset( &PrivateChatChannelJoin, 0, sizeof( PrivateChatChannelJoin ) );
	_wcscpy( PrivateChatChannelJoin.wszChannelName, _countof(PrivateChatChannelJoin.wszChannelName), pChannelName, (int)wcslen(pChannelName) );
	PrivateChatChannelJoin.nPassWord = nChannelPassword;

	CClientSessionManager::GetInstance().SendPacket( CS_PRIVATECHAT_CHANNEL, ePrivateChatChannel::CS_PRIVATECHAT_CHANNEL_JOIN, (char*)&PrivateChatChannelJoin, int(sizeof(PrivateChatChannelJoin)) );
}

void CDnChannelChatTask::SendChannelOut()
{
	PrivateChatChannel::CSPrivateChatChannleOut PrivateChatChannelOut;
	memset( &PrivateChatChannelOut, 0, sizeof( PrivateChatChannelOut ) );

	CClientSessionManager::GetInstance().SendPacket( CS_PRIVATECHAT_CHANNEL, ePrivateChatChannel::CS_PRIVATECHAT_CHANNEL_OUT, (char*)&PrivateChatChannelOut, int(sizeof(PrivateChatChannelOut)) );
}

void CDnChannelChatTask::SendChannelMod( int nModType, int nPassword, INT64 biCharacterDBID )
{
	PrivateChatChannel::CSPrivateChatChannleMod PrivateChatChannelMod;
	memset( &PrivateChatChannelMod, 0, sizeof( PrivateChatChannelMod ) );

	PrivateChatChannelMod.nModType = nModType;
	PrivateChatChannelMod.nPassWord = nPassword;
	PrivateChatChannelMod.biCharacterDBID = biCharacterDBID;

	CClientSessionManager::GetInstance().SendPacket( CS_PRIVATECHAT_CHANNEL, ePrivateChatChannel::CS_PRIVATECHAT_CHANNEL_MOD, (char*)&PrivateChatChannelMod, int(sizeof(PrivateChatChannelMod)) );
}

void CDnChannelChatTask::SendChannelInvite( const WCHAR* pInvitePlayerName )
{
	PrivateChatChannel::CSPrivateChatChannleInvite PrivateChatChannelInvite;
	memset( &PrivateChatChannelInvite, 0, sizeof( PrivateChatChannelInvite ) );
	_wcscpy( PrivateChatChannelInvite.wszCharacterName, _countof(PrivateChatChannelInvite.wszCharacterName), pInvitePlayerName, (int)wcslen(pInvitePlayerName) );

	CClientSessionManager::GetInstance().SendPacket( CS_PRIVATECHAT_CHANNEL, ePrivateChatChannel::CS_PRIVATECHAT_CHANNEL_INVITE, (char*)&PrivateChatChannelInvite, int(sizeof(PrivateChatChannelInvite)) );
}

void CDnChannelChatTask::SendChannelKick( const WCHAR* pKickPlayerName )
{
	if( !m_bJoinChannel || !m_bChannelMaster )
		return;

	bool bExistMember = false;
	std::vector<PrivateChatChannel::TMemberInfo>::iterator iter = m_vecChannelMemberInfo.begin();
	for( ; iter != m_vecChannelMemberInfo.end(); iter++ )
	{
		if( wcscmp( (*iter).wszCharacterName, pKickPlayerName ) == 0 )
		{
			bExistMember = true;
			break;
		}
	}

	if( !bExistMember )
		return;

	PrivateChatChannel::CSPrivateChatChannleKick PrivateChatChannelKick;
	memset( &PrivateChatChannelKick, 0, sizeof( PrivateChatChannelKick ) );
	_wcscpy( PrivateChatChannelKick.wszCharacterName, _countof(PrivateChatChannelKick.wszCharacterName), pKickPlayerName, (int)wcslen(pKickPlayerName) );

	CClientSessionManager::GetInstance().SendPacket( CS_PRIVATECHAT_CHANNEL, ePrivateChatChannel::CS_PRIVATECHAT_CHANNEL_KICK, (char*)&PrivateChatChannelKick, int(sizeof(PrivateChatChannelKick)) );
}

PrivateChatChannel::TMemberInfo* CDnChannelChatTask::GetChannelMemberInfoByDBID( INT64 nCharacterDBID )
{
	PrivateChatChannel::TMemberInfo* pMemberInfo = NULL;
	std::vector<PrivateChatChannel::TMemberInfo>::iterator iter = m_vecChannelMemberInfo.begin();
	for( ; iter != m_vecChannelMemberInfo.end(); iter++ )
	{
		if( (*iter).biCharacterDBID == nCharacterDBID )
		{
			pMemberInfo = &(*iter);
			break;
		}
	}

	return pMemberInfo;
}

PrivateChatChannel::TMemberInfo* CDnChannelChatTask::GetChannelMemberInfoByName( const WCHAR* pstrCharacterName )
{
	PrivateChatChannel::TMemberInfo* pMemberInfo = NULL;
	std::vector<PrivateChatChannel::TMemberInfo>::iterator iter = m_vecChannelMemberInfo.begin();
	for( ; iter != m_vecChannelMemberInfo.end(); iter++ )
	{
		if( wcscmp( (*iter).wszCharacterName, pstrCharacterName ) == 0 )
		{
			pMemberInfo = &(*iter);
			break;
		}
	}

	return pMemberInfo;
}

void CDnChannelChatTask::SortChannelMemberInfo()
{
	PrivateChatChannel::TMemberInfo MasterMemberInfo;
	std::vector<PrivateChatChannel::TMemberInfo>::iterator iter = m_vecChannelMemberInfo.begin();
	for( ; iter != m_vecChannelMemberInfo.end(); iter++ )
	{
		if( (*iter).bMaster )
		{
			MasterMemberInfo = (*iter);
			m_vecChannelMemberInfo.erase( iter );
			break;
		}
	}

	m_vecChannelMemberInfo.insert( m_vecChannelMemberInfo.begin(), MasterMemberInfo );
}

#endif // PRE_PRIVATECHAT_CHANNEL