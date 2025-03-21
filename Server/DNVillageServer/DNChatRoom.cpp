#include "StdAfx.h"
#include "DNChatRoom.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNFriend.h"

CDNChatRoom::CDNChatRoom(UINT nChatRoomID, WCHAR * pwzRoomName, BYTE nRoomType, USHORT uRoomAllow, WCHAR * pwzPassword,
						 WCHAR * pwzPRLine1, WCHAR * pwzPRLine2, WCHAR * pwzPRLine3 )
{
	memset(m_nChatRoomMemberArray, 0, sizeof(m_nChatRoomMemberArray));
	m_nLeaderAccountDBID = 0;

	m_nChatRoomID		= nChatRoomID;

	m_nChatRoomType		= nRoomType;
	m_nChatRoomAllow	= uRoomAllow;

	m_wstrChatRoomName	= pwzRoomName;
	m_wstrChatRoomPW	= pwzPassword;

	m_wstrChatRoomPRLine1	= pwzPRLine1;
	m_wstrChatRoomPRLine2	= pwzPRLine2;
	m_wstrChatRoomPRLine3	= pwzPRLine3;

	m_VecKickedList.clear();
}

CDNChatRoom::~CDNChatRoom()
{
	m_VecKickedList.clear();
}

UINT CDNChatRoom::GetMemberAIDFromIndex( int nIndex )
{
	if( nIndex < 0 || nIndex >= CHATROOMMAX )	return 0;

	return m_nChatRoomMemberArray[ nIndex ];
}

int CDNChatRoom::GetUserCount()
{
	int nUserCount = 0;
	for( int i=0; i<CHATROOMMAX; i++ )
	{
		if( m_nChatRoomMemberArray[i] <= 0 )	continue;

		nUserCount++;
	}

	return nUserCount;
}

bool CDNChatRoom::IsLeader( UINT nUserAID )
{
	if( m_nLeaderAccountDBID <= 0 )			return false;
	if( nUserAID != m_nLeaderAccountDBID )	return false;

	return true;
}

int CDNChatRoom::LeaveUser( UINT nUserAID, int nReason )
{
	int idx = _FindUserFromAID( nUserAID );
	if (idx < 0 )
		return ERROR_CHATROOM_UNKNOWNUSER;		// 방에 없는 유저이다.

	if( nReason == CHATROOMLEAVE_FOREVERKICKED )
	{
		m_VecKickedList.push_back( nUserAID );
	}

	m_nChatRoomMemberArray[idx] = 0;

	// 인원이 빠졌으므로 다시 정렬
	for( int i=idx; i < CHATROOMMAX-1; i++ )
	{
		m_nChatRoomMemberArray[i] = m_nChatRoomMemberArray[i+1];
	}
	m_nChatRoomMemberArray[CHATROOMMAX-1] = 0;

	// 유저의 채팅방ID를 지운다 - 끊어진 유저의 경우에는 포인터가 없을 수도 있다.
	CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID( nUserAID );
	if( pUser != NULL )
	{
		pUser->m_nChatRoomID = 0;
	}

	return ERROR_NONE;
}

int CDNChatRoom::EnterRoom( UINT nUserAID, WCHAR * pwzPassword, bool bLeader )
{
	CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID( nUserAID );
	if( pUser == NULL )					return ERROR_GENERIC_USER_NOT_FOUND;

	int nCanEnter = 0;
	if( (nCanEnter = _CanEnterRoom( nUserAID, pwzPassword, bLeader )) != ERROR_NONE )		return nCanEnter;

	int nEmptyIndex = _FindEmptyIndex();
	if( nEmptyIndex < 0 )				return ERROR_CHATROOM_NOMORESPACE;		// 방이 다 찼음

	m_nChatRoomMemberArray[nEmptyIndex] = nUserAID;
	if( bLeader )
		m_nLeaderAccountDBID = nUserAID;

	// 유저의 채팅방ID를 설정한다.
	pUser->m_nChatRoomID = m_nChatRoomID;

	return ERROR_NONE;
}

void CDNChatRoom::ChangeRoomOption( WCHAR * pwzRoomName, BYTE nRoomType, USHORT nRoomAllow, WCHAR * pwzPassword,
									WCHAR * pwzPRLine1, WCHAR * pwzPRLine2, WCHAR * pwzPRLine3 )
{
	m_wstrChatRoomName = pwzRoomName;
	m_nChatRoomType = nRoomType;
	m_nChatRoomAllow = nRoomAllow;
	m_wstrChatRoomPW = pwzPassword;
	m_wstrChatRoomPRLine1 = pwzPRLine1;
	m_wstrChatRoomPRLine2 = pwzPRLine2;
	m_wstrChatRoomPRLine3 = pwzPRLine3;
}

void CDNChatRoom::SendChatMessageToAll( int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg )
{
	for( int i=0; i<CHATROOMMAX; i++ )
	{
		if( m_nChatRoomMemberArray[i] <= 0 )		continue;

		CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID( m_nChatRoomMemberArray[i] );
		if( pUser == NULL )							continue;

		pUser->SendChat( CHATTYPE_CHATROOM, cLen, pwszCharacterName, pwszChatMsg );
	}
}

void CDNChatRoom::SendProfileToAll( int nSessionID, TProfile & profile )
{
	for( int i=0; i<CHATROOMMAX; i++ )
	{
		if( m_nChatRoomMemberArray[i] <= 0 )		continue;

		CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID( m_nChatRoomMemberArray[i] );
		if( pUser == NULL )							continue;

		pUser->SendDisplayProfile( nSessionID, profile );
	}
}

int CDNChatRoom::_CanEnterRoom( UINT nUserAID, WCHAR * pwzPassword, bool bLeader )
{
	CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(nUserAID);
	if( pUser == NULL )					return ERROR_CHATROOM_CANNOTENTER;


	// 1. 일반적인 체크
	// 1.1 빈공간 체크
	int nEmptyIndex = _FindEmptyIndex();
	if( nEmptyIndex < 0 )			return ERROR_CHATROOM_NOMORESPACE;		// 방이 다 찼음
	
	// 1.2 방에 참여중인가?
	int nMyIndex = _FindUserFromAID( nUserAID );
	if( nMyIndex >= 0 )				return ERROR_CHATROOM_ALREADYINROOM;	// 이미 방에 참여중이다.

	// 1.3 영구킥리스트에 존재하는가?
	for( UINT i=0; i<m_VecKickedList.size(); i++)
	{
		if( m_VecKickedList[i] == nUserAID )
			return ERROR_CHATROOM_KICKEDFOREVER;
	}

	// 2. 입장조건 체크
	bool bCanEnterRoom = true;

	// 2.1 패스워드
	if( m_wstrChatRoomPW.length() > 0 && m_wstrChatRoomPW != pwzPassword )	return ERROR_CHATROOM_WRONGPASSWORD;

	// 2.2 입장 제한 체크
	if( bLeader == false )	// 리더는 입장 제한에 걸리지 않는다.
	{
		CDNUserSession * pLeader = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nLeaderAccountDBID);
		if( pLeader == NULL )		return ERROR_CHATROOM_UNKNOWNUSER;

		bool bFriend = pLeader->GetFriend()->HasFriend( pUser->GetCharacterDBID() );
		bool bSameGuild = pLeader->GetGuildUID().IsSet() && pUser->GetGuildUID().IsSet() && pLeader->GetGuildUID() == pUser->GetGuildUID();

		switch( m_nChatRoomAllow )
		{
		case CHATROOMALLOW_ALL:
			break;
		case CHATROOMALLOW_FRIEND:
			if( bFriend == false )						return ERROR_CHATROOM_CANNOTENTER;
			break;
		case CHATROOMALLOW_GUILD:
			if( bSameGuild == false )					return ERROR_CHATROOM_CANNOTENTER;
			break;
		case CHATROOMALLOW_FRIENDANDGUILD:
			if( (bFriend || bSameGuild) == false )		return ERROR_CHATROOM_CANNOTENTER;
			break;
		}
	}

	return ERROR_NONE;
}

int CDNChatRoom::_FindUserFromAID( UINT nUserAID )
{
	for( int i=0; i < CHATROOMMAX; i++ )
	{
		if( m_nChatRoomMemberArray[i] == nUserAID )
		{
			return i;
		}
	}

	return -1;
}

int CDNChatRoom::_FindEmptyIndex()
{
	for( int i=0; i < CHATROOMMAX; i++ )
	{
		if( m_nChatRoomMemberArray[i] <= 0 )
			return i;
	}

	return -1;
}