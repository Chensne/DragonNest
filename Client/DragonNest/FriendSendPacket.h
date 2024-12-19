
#pragma once

#include "GameClientSession.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DNPacket.h"
#include "DnCommonUtil.h"

inline void SendReqFriendList()
{
	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_REQ_LIST, NULL, 0);
}

inline void SendReqFriendLocationList()
{
	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_REQ_LOCATION, NULL, 0);
}

inline void SendAddGroup(const WCHAR * pGroupName)	
{
	CSFriendAddGroup packet;
	memset(&packet, 0, sizeof(packet));

	packet.cNameLen = (BYTE)wcslen(pGroupName);
	if (CommonUtil::IsValidFriendGroupNameLen(packet.cNameLen) == false)
		return;

	_wcscpy(packet.wszBuf, _countof(packet.wszBuf), pGroupName, (int)wcslen(pGroupName));

	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_GROUP_ADD, (char*)&packet, sizeof(packet) - \
		sizeof(packet.wszBuf) + (packet.cNameLen * sizeof(WCHAR)));
}

inline void SendDelGroup(UINT nGroupDBID)
{
	CSFriendDelGourp packet;
	packet.nGroupDBID = nGroupDBID;

	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_GROUP_DELETE, (char*)&packet, sizeof(packet));
}

inline void SendGroupUpdate(UINT nGroupDBID, const WCHAR * pGroupName)
{
	int nSize = 0;
	CSFriendGroupUpdate packet;
	memset(&packet, 0, sizeof(packet));

	packet.nGroupDBID = nGroupDBID;
	if (pGroupName != NULL && pGroupName[0] != '\0')
	{
		packet.cNameLen = (BYTE)wcslen(pGroupName);
		if (CommonUtil::IsValidFriendGroupNameLen(packet.cNameLen) == false)
			return;

		_wcscpy(packet.wszBuf, _countof(packet.wszBuf), pGroupName, (int)wcslen(pGroupName));
		nSize = packet.cNameLen * sizeof(WCHAR);
	}

	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_GROUP_STATEUPDATE, (char*)&packet, sizeof(packet) - \
		sizeof(packet.wszBuf) + nSize);
}

inline void SendAddFriend(UINT nGroupDBID, const WCHAR * pCharacterName)
{
	int nSize = 0;
	CSFriendAdd packet;
	memset(&packet, 0, sizeof(packet));

	packet.cNameLen = (BYTE)wcslen(pCharacterName);
	if (CommonUtil::IsValidCharacterNameLen(packet.cNameLen) == false)
		return;

	_wcscpy(packet.wszBuf, _countof(packet.wszBuf), pCharacterName, (int)wcslen(pCharacterName));
	packet.nGroupDBID = nGroupDBID;
	nSize = packet.cNameLen * sizeof(WCHAR);

	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_ADD, (char*)&packet, sizeof(packet) - \
		sizeof(packet.wszBuf) + nSize);
}

inline void SendDeleteFriend(INT64 biFriendCharDBID)
{
	CSFriendDelete packet;
	packet.biFriendCharacterDBID = biFriendCharDBID;

	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_DELETE, (char*)&packet, sizeof(packet));
}

inline void SendFriendUpdate(INT64 biFriendDBID, UINT nGroupDBID)
{
	CSFriendUpdate packet;
	memset(&packet, 0, sizeof(packet));

	packet.biFriendCharacterDBID = biFriendDBID;
	packet.nGroupDBID = nGroupDBID;

// 	if (pMemo != NULL && pMemo[0] != '\0')
// 	{
// 		packet.cMemoLen = (BYTE)wcslen(pMemo);
// 		wcscpy_s(packet.wszBuf, FRIEND_MEMO_LEN_MAX, pMemo);
// 	}

	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_STATEUPDATE, (char*)&packet, sizeof(packet));
}

inline void SendReqDetailInfo(INT64 biFriendCharDBID)
{
	CSFriendDetailInfo packet;
	packet.biFriendCharacterDBID = biFriendCharDBID;
	
	CClientSessionManager::GetInstance().SendPacket(CS_FRIEND, eFriend::CS_FRIEND_INFO, (char*)&packet, sizeof(packet));
}