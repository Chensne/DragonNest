#include "StdAfx.h"
#include "DnChatRoomTask.h"
#include "DnInterface.h"
#include "ChatRoomSendPacket.h"
#include "DnPlayerActor.h"
#include "DnChatRoomDlg.h"
#include "DnChatOption.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnChatRoomTask::CDnChatRoomTask()
: CTaskListener( false )
, m_bRequestWait( false )
, m_nChatRoomAllow( 0 )
, m_nRequestRoomID( 0 )
{
}

CDnChatRoomTask::~CDnChatRoomTask()
{
	Finalize();
}

void CDnChatRoomTask::OnDisconnectTcp( bool bValidDisconnect )
{
}

void CDnChatRoomTask::OnDisconnectUdp()
{
}

void CDnChatRoomTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch( nMainCmd ) {
		case SC_CHATROOM: OnRecvChatRoomMessage( nSubCmd, pData, nSize ); break;
		case SC_ROOM: OnRecvRoomMessage( nSubCmd, pData, nSize ); break;
	}
}

bool CDnChatRoomTask::Initialize()
{
	return true;
}

void CDnChatRoomTask::Finalize()
{
}

void CDnChatRoomTask::OnRecvChatRoomMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eChatRoom::SC_CREATECHATROOM:		OnRecvChatRoomCreateRoom( (SCCreateChatRoom*)pData ); break;
		case eChatRoom::SC_ENTERCHATROOM:		OnRecvChatRoomEnterRoom( (SCChatRoomEnterRoom*)pData ); break;
		case eChatRoom::SC_ENTERUSERCHATROOM:	OnRecvChatRoomEnterUser( (SCChatRoomEnterUser*)pData ); break;
		case eChatRoom::SC_CHANGEROOMOPTION:	OnRecvChatRoomChangeRoomOption( (SCChatRoomChangeRoomOption*)pData ); break;
		case eChatRoom::SC_KICKUSER:			OnRecvChatRoomKickUser( (SCChatRoomKickUser*)pData ); break;
		case eChatRoom::SC_LEAVEUSER:			OnRecvChatRoomLeave( (SCChatRoomLeaveUser*)pData ); break;

		case eChatRoom::SC_CHATROOMVIEW:		OnRecvChatRoomView( (SCChatRoomView*)pData ); break;
	}
}

void CDnChatRoomTask::OnRecvRoomMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eRoom::SC_SYNC_CHATROOMINFO:		OnRecvRoomSyncChatRoomInfo( (SCROOM_SYNC_CHATROOMINFO*)pData ); break;
	}
}

void CDnChatRoomTask::OnRecvChatRoomCreateRoom( SCCreateChatRoom *pPacket )
{
	m_bRequestWait = false;

	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	if( pPacket->nRet == ERROR_NONE )
	{
		// 정보들 설정 후
		m_ChatRoomView.Set( pPacket->ChatRoomView );
		GetInterface().GetChatRoomDlg()->SetRoomInfo( pPacket->ChatRoomView.m_nChatRoomID, pPlayer->GetUniqueID(), pPacket->ChatRoomView.m_wszChatRoomName, true );
		GetInterface().ShowChatRoomDialog( true );
		OnSetChatRoomView( pPlayer, &pPacket->ChatRoomView );

		bool bResetAction = true;
		if( strcmp( pPlayer->GetCurrentAction(), "Normal_Stand" ) == 0 )
			bResetAction = false;
		if( strcmp( pPlayer->GetCurrentAction(), "PutIn_Weapon" ) == 0 )
			bResetAction = false;
		if( bResetAction )
			pPlayer->CmdStop( "Normal_Stand", 0, 15.f, 0.f, true );
	}
	else
	{
		// 생성창 굳이 닫아줄 필요 있을까..
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnChatRoomTask::OnRecvChatRoomView( SCChatRoomView *pPacket )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	// 자신인지 확인
	if( pPacket->nSessionID == pPlayer->GetUniqueID() ) {
		// 자기가 방생성 한건 자신에게 안온다고 들었다.
		return;
	}

	// 리스트에 있는지부터 판단 후
	TChatRoomView *pView = FindChatRoomView( pPacket->nSessionID );
	if( pView )
	{
		pView->Set( pPacket->ChatRoomView );
	}
	else
	{
		m_listSCChatRoomView.push_back( *pPacket );
	}

	// 주변에 SessionID가진 녀석 찾아 있다면 적용.
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	if( !hActor ) return;

	CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
	if( !pActor ) return;

	OnSetChatRoomView( pActor, &pPacket->ChatRoomView );

	// 채팅방 참여중에 방장이 방이름을 바꿨다면, 여기서 판단할 수 있다.
	if( pPacket->ChatRoomView.m_nChatRoomID == GetInterface().GetChatRoomDlg()->GetRoomID() )
	{
		GetInterface().GetChatRoomDlg()->SetRoomInfo( pPacket->ChatRoomView.m_nChatRoomID, pPacket->nSessionID, pPacket->ChatRoomView.m_wszChatRoomName );
	}
}

void CDnChatRoomTask::OnRecvChatRoomLeave( SCChatRoomLeaveUser *pPacket )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	// 자신인지 확인
	if( pPacket->nUserSessionID == pPlayer->GetUniqueID() ) {
		bool bMaster = GetInterface().GetChatRoomDlg()->IsMaster();
		if( bMaster ) {
			m_ChatRoomView.Reset();
			OnSetChatRoomView( pPlayer, &m_ChatRoomView );
		}
		else {
			OnNotifyRoomEnter( pPlayer, false );
		}
		bool bShow = GetInterface().GetChatRoomDlg()->IsShow();
		GetInterface().ShowChatRoomDialog( false );

		if( bMaster || !bShow )
			return;

		int nMsgIndex = 0;
		switch( pPacket->nLeaveReason )
		{
		case CHATROOMLEAVE_KICKED:			nMsgIndex = 121059; break;
		case CHATROOMLEAVE_FOREVERKICKED:	nMsgIndex = 121060; break;
		case CHATROOMLEAVE_DESTROYROOM:		nMsgIndex = 8138; break;
		}
		if( nMsgIndex )
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMsgIndex ), false );

		return;
	}

	// 다른 사람이면..
	// 주변에 SessionID가진 녀석 찾아 있다면 적용.
	if( GetInterface().GetChatRoomDlg()->IsMyChatRoomMember( pPacket->nUserSessionID ) )
	{
		GetInterface().GetChatRoomDlg()->DelUser( pPacket->nUserSessionID, pPacket->nLeaveReason );
	}

	// 다른 방에서 나간 듯. 주변에서 찾아서 말풍선 풀어줘야한다.
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nUserSessionID );
	if( !hActor ) return;

	CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
	if( !pActor ) return;

	OnNotifyRoomEnter( pActor, false );
}

void CDnChatRoomTask::OnRecvChatRoomEnterRoom( SCChatRoomEnterRoom *pPacket )
{
	if( pPacket->nRet == ERROR_NONE )
	{
		int nChatRoomID = 0;
		int nChatRoomType = 0;
		std::wstring wszRoomName;
		TChatRoomView *pView = FindChatRoomView( pPacket->LeaderSID );
		if( pView ) {
			nChatRoomID = pView->m_nChatRoomID;
			nChatRoomType = pView->m_nChatRoomType;
			wszRoomName = pView->m_wszChatRoomName;
		}
		GetInterface().GetChatRoomDlg()->SetRoomInfo( nChatRoomID, pPacket->LeaderSID, wszRoomName.c_str() );
		for( int i = 0; i < CHATROOMMAX; ++i )
		{
			if( pPacket->UserSessionIDs[i] == 0 ) continue;
			if( pPacket->UserSessionIDs[i] == pPacket->LeaderSID ) continue;
			GetInterface().GetChatRoomDlg()->AddUser( pPacket->UserSessionIDs[i] );
		}
		GetInterface().ShowChatRoomDialog( true );

		// 자신의 참여 말풍선 표시.
		if( !CDnActor::s_hLocalActor ) return;
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		OnNotifyRoomEnter( pPlayer, true, nChatRoomType );

		// 그리고 앉기.
		pPlayer->CmdAction( "Social_Sit01" );
	}
	else if( pPacket->nRet == ERROR_CHATROOM_WRONGPASSWORD )
	{
		GetInterface().MessageBox( 120113 );
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnChatRoomTask::OnRecvChatRoomEnterUser( SCChatRoomEnterUser *pPacket )
{
	// 주변을 찾아 있으면 처리후 끝내고,
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nUserSessionID );
	if( hActor )
	{
		// 플레이어 변환이 실패하면 진짜 이상한 경우인데, 그땐 패스.
		CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
		if( pActor )
		{
			if( GetInterface().GetChatRoomDlg()->GetRoomID() == pPacket->nChatRoomID )
			{
				GetInterface().GetChatRoomDlg()->AddUser( pPacket->nUserSessionID, true );
			}

			// 여기서 참여 말풍선 표시.
			OnNotifyRoomEnter( pActor, true, pPacket->cChatRoomType );
		}
	}
	else
	{
		// 없으면 로딩중일 가능성이 높으니, 리스트에 넣어뒀다가 다음에 처리한다.
		m_listSCChatRoomEnterUser.push_back( *pPacket );
	}
}

void CDnChatRoomTask::OnRecvChatRoomChangeRoomOption( SCChatRoomChangeRoomOption *pPacket )
{
	if( pPacket->nRet == ERROR_NONE )
	{
		m_ChatRoomView.Set( pPacket->ChatRoomView );

		if( !CDnActor::s_hLocalActor ) return;
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		OnSetChatRoomView( pPlayer, &m_ChatRoomView );
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnChatRoomTask::OnRecvChatRoomKickUser( SCChatRoomKickUser *pPacket )
{
	if( pPacket->nRet == ERROR_NONE )
	{
		// 에러가 NONE이라면 LeaveUser로 오는거 아닌가?
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnChatRoomTask::RequestCreateChatRoom( const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3 )
{
	m_wszPassword = pPassword;
	m_nChatRoomAllow = nRoomAllow;
	SendCreateChatRoom( pRoomName, pPassword, nRoomType, nRoomAllow, pPR1, pPR2, pPR3 );

	m_bRequestWait = true;
}

void CDnChatRoomTask::RequestConfigChatRoom( const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3 )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	m_wszPassword = pPassword;
	m_nChatRoomAllow = nRoomAllow;
	SendChangeRoomOption( pRoomName, pPassword, nRoomType, nRoomAllow, pPR1, pPR2, pPR3 );
	GetInterface().GetChatRoomDlg()->SetRoomInfo( m_ChatRoomView.m_nChatRoomID, pPlayer->GetUniqueID(), pRoomName, true );
}

bool CDnChatRoomTask::IsEnableChatRoomEnter( UINT nSessionID )
{
	// 아니라면, 주변에 SessionID가진 녀석 찾아 있다면 적용.
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return false;

	TChatRoomView *pView = FindChatRoomView( nSessionID );
	if( !pView ) return false;

	if( pView->m_nChatRoomID == 0 ) return false;
	return true;
}

void CDnChatRoomTask::RequestEnterChatRoom( UINT nSessionID )
{
	// 아니라면, 주변에 SessionID가진 녀석 찾아 있다면 적용.
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return;

	TChatRoomView *pView = FindChatRoomView( nSessionID );
	if( !pView ) return;

	if( pView->m_nChatRoomID == 0 ) return;

	if( pView->m_bHasPassword )
	{
		// 비번 입력 창 보여주고 Callback으로 처리한다.
		m_nRequestRoomID = pView->m_nChatRoomID;
		GetInterface().ShowChatRoomPassWordDlg( true, CHATROOM_PASSWORD_DIALOG, this );
	}
	else
	{
		SendEnterChatRoom( pView->m_nChatRoomID, L"" );
	}
}

void CDnChatRoomTask::RequestKickUser( LPCWSTR pwszName, bool bForever )
{
	DnActorHandle hActor = CDnActor::FindActorFromName( (TCHAR *)pwszName );
	if( !hActor ) return;

	SendChatRoomKickUser( hActor->GetUniqueID(), bForever ? CHATROOMLEAVE_FOREVERKICKED : CHATROOMLEAVE_KICKED );
}

void CDnChatRoomTask::OnSetChatRoomView( CDnPlayerActor *pActor, TChatRoomView *pView )
{
	if( pView->m_nChatRoomID > 0 )
	{
		pActor->GetChatBalloon().ForceShowState( true );
		pActor->GetChatBalloon().StopChatBalloonCustom();
		pActor->GetChatBalloon().ClearCustomChatBalloonText();

		if( pView->m_nChatRoomType == CHATROOMTYPE_NORMAL )
		{
			pActor->GetChatBalloon().SetChatBalloonCustom( "ChatNormalDlg.ui", pView->m_wszChatRoomName, chatroomcolor::NORMAL, GetTickCount(), 1 );
		}
		else if( pView->m_nChatRoomType == CHATROOMTYPE_TRADE )
		{
			pActor->GetChatBalloon().SetChatBalloonCustom( "ChatDealDlg.ui", pView->m_wszChatRoomName, chatroomcolor::TRADE, GetTickCount(), 1 );
		}
		else if( pView->m_nChatRoomType == CHATROOMTYPE_PR )
		{
			pActor->GetChatBalloon().SetChatBalloonCustom( "ChatProfileDlg.ui", pView->m_wszChatRoomName, chatroomcolor::PROFILE, GetTickCount(), 1 );

			std::wstring wszPR[3];
			wszPR[0] = pView->m_wszChatRoomPRLine1;
			wszPR[1] = pView->m_wszChatRoomPRLine2;
			wszPR[2] = pView->m_wszChatRoomPRLine3;
			for( int i = 0; i < 3; ++i ) wszPR[i] = boost::algorithm::trim_copy( wszPR[i] );

			// 스페이스만 있는 라인이거나 아예 적은 안적은 라인 빼고 나머지 넣는다. 넣을땐 원본 그대로 넣는다.
			// 만약 하나도 설정하지 않았다면, AddCustom으로 리스트안에 넣는게 없기때문에 다른 방처럼 방이름만 계속 나올 것이다.
			if( wszPR[0].size() ) pActor->GetChatBalloon().AddCustomChatBalloonText( pView->m_wszChatRoomPRLine1 );
			if( wszPR[1].size() ) pActor->GetChatBalloon().AddCustomChatBalloonText( pView->m_wszChatRoomPRLine2 );
			if( wszPR[2].size() ) pActor->GetChatBalloon().AddCustomChatBalloonText( pView->m_wszChatRoomPRLine3 );
		}
	}
	else
	{
		pActor->GetChatBalloon().ForceShowState( false );
		pActor->GetChatBalloon().StopChatBalloonCustom();
		pActor->GetChatBalloon().ClearCustomChatBalloonText();
	}
}

void CDnChatRoomTask::OnNotifyRoomEnter( CDnPlayerActor *pActor, bool bEnter, int nRoomType )
{
	int nChatType = 0;
	switch( nRoomType )
	{
	case CHATROOMTYPE_NORMAL:	nChatType = CHAT_CHATROOM1; break;
	case CHATROOMTYPE_TRADE:	nChatType = CHAT_CHATROOM2; break;
	case CHATROOMTYPE_PR:		nChatType = CHAT_CHATROOM3; break;
	}

	if( bEnter )
	{
		pActor->GetChatBalloon().ForceShowState( true );
		pActor->GetChatBalloon().SetChatBalloonText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8133 ), 0, nChatType );
	}
	else
	{
		pActor->GetChatBalloon().ForceShowState( false );
	}
}

void CDnChatRoomTask::OnCharUserEntered( DnActorHandle hActor )
{
	// 먼저 OnRecvChatRoomEnterUser때 처리 못한 유저인지를 판단해서 처리.
	if( !m_listSCChatRoomEnterUser.empty() )
	{
		std::list<SCChatRoomEnterUser>::iterator iter = m_listSCChatRoomEnterUser.begin();
		for( ; iter != m_listSCChatRoomEnterUser.end(); )
		{
			if( iter->nUserSessionID == hActor->GetUniqueID() )
			{
				// 플레이어 변환이 실패하면 진짜 이상한 경우인데, 그땐 패스.
				CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
				if( pActor )
				{
					// 거의 아래 AddUser는 호출될 가능성이 없기 때문에 빼도 되지만, 로딩이 길어질 경우를 대비해서 넣어둔다.
					if( GetInterface().GetChatRoomDlg()->GetRoomID() == iter->nChatRoomID )
					{
						GetInterface().GetChatRoomDlg()->AddUser( hActor, true );
					}

					// 여기서 참여 말풍선 표시.
					OnNotifyRoomEnter( pActor, true, iter->cChatRoomType );
				}
				m_listSCChatRoomEnterUser.erase( iter );
				break;
			}
			++iter;
		}
	}

	// 이 후 룸뷰 리스트에 쌓인 것들 원래대로 처리.
	if( m_listSCChatRoomView.empty() ) return;

	CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
	if( !pActor ) return;

	TChatRoomView *pView = FindChatRoomView( pActor->GetUniqueID() );
	if( !pView ) return;

	OnSetChatRoomView( pActor, pView );
}

void CDnChatRoomTask::OnCharUserLeaved( DnActorHandle hActor )
{
	if( m_listSCChatRoomView.empty() ) return;
	if( !hActor ) return;

	std::list<SCChatRoomView>::iterator iter = m_listSCChatRoomView.begin();
	for( ; iter != m_listSCChatRoomView.end(); )
	{
		if( iter->nSessionID == hActor->GetUniqueID() )
		{
			m_listSCChatRoomView.erase( iter );
			return;
		}
		++iter;
	}
}

TChatRoomView *CDnChatRoomTask::FindChatRoomView( UINT nSessionID )
{
	std::list<SCChatRoomView>::iterator iter = m_listSCChatRoomView.begin();
	for( ; iter != m_listSCChatRoomView.end(); )
	{
		if( iter->nSessionID == nSessionID )
		{
			return &iter->ChatRoomView;
		}
		++iter;
	}
	return NULL;
}

void CDnChatRoomTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) 
	{
	case CHATROOM_PASSWORD_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
				{
					LPCWSTR pwszPassword = GetInterface().GetChatRoomPassWord();
					SendEnterChatRoom( m_nRequestRoomID, (pwszPassword == NULL) ? L"" : pwszPassword );
				}
				else
				{

					GetInterface().ShowChatRoomPassWordDlg( false );
				}
			}
		}
		break;
	}
}

void CDnChatRoomTask::OnRecvRoomSyncChatRoomInfo( SCROOM_SYNC_CHATROOMINFO *pPacket )
{
	// 게임난입시 해당 게임룸에 만들어져있던 채팅룸 정보를 받아 동기화시킨다.
	// 빌리지에서 하던대로 맞춰보려면, 해당 패킷을 SCChatRoomView, SCChatRoomEnterUser로 변환시켜서 사용하면 된다.
	// 참고로 빌리지와 달리 이미 룸안에 있는 유저들 정보 다 받고 액터까지 만든 상태이므로
	// 빌리지에서 유저입장할때의 OnCharUserEntered처리같은건 필요하지 않다.
	if( pPacket->nLeaderID && pPacket->ChatRoomView.m_nChatRoomID )
	{
		SCChatRoomView RoomView;
		RoomView.nSessionID = pPacket->nLeaderID;
		RoomView.ChatRoomView.Set( pPacket->ChatRoomView );
		OnRecvChatRoomView( &RoomView );

		if( pPacket->nCount )
		{
			for( int i = 0; i < pPacket->nCount; ++i )
			{
				if( pPacket->nMembers[i] == 0 ) continue;
				if( pPacket->nMembers[i] == pPacket->nLeaderID ) continue;

				SCChatRoomEnterUser EnterUser;
				EnterUser.nChatRoomID = pPacket->ChatRoomView.m_nChatRoomID;
				EnterUser.cChatRoomType = pPacket->ChatRoomView.m_nChatRoomType;
				EnterUser.nUserSessionID = pPacket->nMembers[i];
				OnRecvChatRoomEnterUser( &EnterUser );
			}
		}
	}
}

void CDnChatRoomTask::ClearChatRoomInfoList()
{
	m_listSCChatRoomView.clear();
	m_listSCChatRoomEnterUser.clear();
}

void CDnChatRoomTask::ForceCloseChatRoom()
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	// 자신인지 확인
	bool bMaster = GetInterface().GetChatRoomDlg()->IsMaster();
	if( bMaster ) {
		m_ChatRoomView.Reset();
		OnSetChatRoomView( pPlayer, &m_ChatRoomView );
	}
	GetInterface().ShowChatRoomDialog( false );
}