#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"

// CHATROOM -----------------------------------------------------------------------------

inline void SendCreateChatRoom(const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3)
{
	CSCreateChatRoom packet;
	memset(&packet, 0, sizeof(CSCreateChatRoom));

	_wcscpy(packet.wszName, _countof(packet.wszName), pRoomName, (int)wcslen(pRoomName));
	_wcscpy(packet.wszPassword, _countof(packet.wszPassword), pPassword, (int)wcslen(pPassword));
	packet.nRoomType = (BYTE)nRoomType;
	packet.nRoomAllow = (USHORT)nRoomAllow;
	_wcscpy(packet.PRLine1, _countof(packet.PRLine1), pPR1, (int)wcslen(pPR1));
	_wcscpy(packet.PRLine2, _countof(packet.PRLine2), pPR2, (int)wcslen(pPR2));
	_wcscpy(packet.PRLine3, _countof(packet.PRLine3), pPR3, (int)wcslen(pPR3));

	CClientSessionManager::GetInstance().SendPacket(CS_CHATROOM, eChatRoom::CS_CREATECHATROOM, (char*)&packet, sizeof(CSCreateChatRoom));
}

inline void SendLeaveChatRoom()
{
	CClientSessionManager::GetInstance().SendPacket(CS_CHATROOM, eChatRoom::CS_LEAVEROOM, NULL, 0);
}

inline void SendChatRoomMsg( LPCWSTR wszChatMsg )
{
	CSChatChatRoom ChatRoomMsg;
	memset(&ChatRoomMsg, 0, sizeof(CSChatChatRoom));

	ChatRoomMsg.nLen = (short)wcslen( wszChatMsg );
	_wcscpy( ChatRoomMsg.wszChatMsg, _countof(ChatRoomMsg.wszChatMsg), wszChatMsg, (int)wcslen(wszChatMsg) );

	CClientSessionManager::GetInstance().SendPacket(CS_CHAT, eChat::CS_CHAT_CHATROOM, (char*)&ChatRoomMsg, sizeof(CSChatChatRoom) - sizeof(ChatRoomMsg.wszChatMsg) + (ChatRoomMsg.nLen * 2));
}

inline void SendEnterChatRoom( int nRoomID, const WCHAR *pPassword )
{
	CSChatRoomEnterRoom EnterRoom;
	memset(&EnterRoom, 0, sizeof(CSChatRoomEnterRoom));

	EnterRoom.nChatRoomID = nRoomID;
	_wcscpy(EnterRoom.wszPassword, _countof(EnterRoom.wszPassword), pPassword, (int)wcslen(pPassword));

	CClientSessionManager::GetInstance().SendPacket(CS_CHATROOM, eChatRoom::CS_ENTERCHATROOM, (char*)&EnterRoom, sizeof(CSChatRoomEnterRoom));
}

inline void SendChangeRoomOption(const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3)
{
	CSChatRoomChangeRoomOption packet;
	memset(&packet, 0, sizeof(CSChatRoomChangeRoomOption));

	packet.nRoomAllow = nRoomAllow;
	_wcscpy(packet.wszPassword, _countof(packet.wszPassword), pPassword, (int)wcslen(pPassword));
	packet.ChatRoomView.Set( 0, pRoomName, nRoomType, false, pPR1, pPR2, pPR3 );

	CClientSessionManager::GetInstance().SendPacket(CS_CHATROOM, eChatRoom::CS_CHANGEROOMOPTION, (char*)&packet, sizeof(CSChatRoomChangeRoomOption));
}

inline void SendChatRoomKickUser( int nKickUserSessionID, int nType )
{
	CSChatRoomKickUser packet;
	memset(&packet, 0, sizeof(CSChatRoomKickUser));

	packet.nKickUserSessionID = nKickUserSessionID;
	packet.nKickReason = nType;

	CClientSessionManager::GetInstance().SendPacket(CS_CHATROOM, eChatRoom::CS_KICKUSER, (char*)&packet, sizeof(CSChatRoomKickUser));
};