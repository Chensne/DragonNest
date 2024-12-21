
#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"

inline void SendCreateGuild(const WCHAR *pGuildName)
{
	CSCreateGuild Packet;
	memset(&Packet, 0, sizeof(Packet));

	_wcscpy(Packet.wszGuildName, _countof(Packet.wszGuildName), pGuildName, (int)wcslen(pGuildName));

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_CREATEGUILD, (char*)&Packet, sizeof(Packet));
}

inline void SendGetGuildInfo(bool bNeedMembList)
{
	CSGetGuildInfo Packet;
	memset(&Packet, 0, sizeof(CSGetGuildInfo));

	Packet.bNeedMembList = bNeedMembList;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_GETGUILDINFO, (char*)&Packet, sizeof(Packet));
}

inline void SendDismissGuild()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_DISMISSGUILD, NULL, 0);
}

inline void SendInviteGuildMemberReq(const WCHAR *pPlayerName)
{
	CSInviteGuildMemberReq Packet;
	memset(&Packet, 0, sizeof(Packet));

	_wcscpy(Packet.wszToCharacterName, _countof(Packet.wszToCharacterName), pPlayerName, (int)wcslen(pPlayerName));

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_INVITEGUILDMEMBREQ, (char*)&Packet, sizeof(Packet));
}

inline void SendInviteGuildMemberAck(const TGuildUID pGuildUID, UINT nFromAccountID, UINT nFromSessionID, bool bAccept)
{
	CSInviteGuildMemberAck Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.GuildUID = pGuildUID;
	Packet.nFromSessionID = nFromSessionID;
	Packet.bAccept = bAccept;
	Packet.nFromAccountID = nFromAccountID;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_INVITEGUILDMEMBACK, (char*)&Packet, sizeof(Packet));
}

inline void SendLeaveGuild()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_LEAVEGUILDMEMB, NULL, 0);
}

inline void SendExileGuild(UINT nAccountDBID, INT64 nCharacterDBID)
{
	CSExileGuildMember Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nAccountDBID = nAccountDBID;
	Packet.nCharacterDBID = nCharacterDBID;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_EXILEGUILDMEMB, (char*)&Packet, sizeof(Packet));
}

inline void SendChangeGuildInfo(BYTE btGuildUpdate, int iInt1 = 0, int iInt2 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL)
{
	CSChangeGuildInfo Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.btGuildUpdate = btGuildUpdate;
	Packet.Int1 = iInt1;
	Packet.Int2 = iInt2;
	Packet.Int64 = biInt64;
	if (pText) {
		_wcscpy(Packet.Text, _countof(Packet.Text), pText, (int)wcslen(pText));
	}

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_CHANGEGUILDINFO, (char*)&Packet, sizeof(Packet));
}

inline void SendChangeGuildInfoEx(BYTE btGuildUpdate, int iInt1 = 0, int iInt2 = 0, int iInt3 = 0, int iInt4 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL)
{
	CSChangeGuildInfo Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.btGuildUpdate = btGuildUpdate;
	Packet.Int1 = iInt1;
	Packet.Int2 = iInt2;
	Packet.Int3 = iInt3;
	Packet.Int4 = iInt4;
	Packet.Int64 = biInt64;
	if (pText) {
		_wcscpy(Packet.Text, _countof(Packet.Text), pText, (int)wcslen(pText));
	}

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_CHANGEGUILDINFO, (char*)&Packet, sizeof(Packet));
}

inline void SendChangeGuildMemberInfo(BYTE btGuildMemberUpdate, int iInt1 = 0, int iInt2 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL, UINT nChgAccountDBID = 0, INT64 nChgCharacterDBID = 0)
{
	CSChangeGuildMemberInfo Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.btGuildMemberUpdate = btGuildMemberUpdate;
	Packet.nChgAccountDBID = nChgAccountDBID;
	Packet.nChgCharacterDBID = nChgCharacterDBID;
	Packet.Int1 = iInt1;
	Packet.Int2 = iInt2;
	Packet.Int64 = biInt64;
	if (pText) {
		_wcscpy(Packet.Text, _countof(Packet.Text), pText, (int)wcslen(pText));
	}

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_CHANGEGUILDMEMBINFO, (char*)&Packet, sizeof(Packet));
}

inline void SendGetGuildHistoryList(INT64 biIndex, bool bDirection)
{
	CSGetGuildHistoryList Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.biIndex = biIndex;
	Packet.bDirection = bDirection;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_GETGUILDHISTORYLIST, (char*)&Packet, sizeof(Packet));
}

inline void SendGetGuildStorageHistoryList(int nPage)
{
	CSGetGuildWareHistory Packet;
	Packet.nIndex = nPage;
	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_GET_GUILDWARE_HISTORY, (char*)&Packet, sizeof(Packet));
}

inline void SendCloseGuildWare()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_CLOSE_GUILDWARE, NULL, 0);
}

inline void SendEnrollGuildWar()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_ENROLL_GUILDWAR, NULL, 0);
}

inline void SendGuildWarWinSkill(int nSkillID)
{
	CSGuildWarWinSkill Packet;
	Packet.nSkillID = nSkillID;
	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_GUILDWAR_WIN_SKILL, (char*)&Packet, sizeof(Packet));
}

inline void SendBuyGuildRewardItem(int nItemID)
{
	CSBuyGuildRewardItem Packet;
	Packet.nItemID = nItemID;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_GUILD_BUY_REWARDITEM, (char*)&Packet, sizeof(Packet));
}

inline void SendRequestPlayerGuildInfo(int nSessionID)
{
	CSPlayerGuildInfo requestGuildInfo;
	requestGuildInfo.nSessionID = nSessionID;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILD, eGuild::CS_PLAYER_REQUEST_GUILDINFO, (char*)&requestGuildInfo, sizeof(requestGuildInfo));
}

#ifdef PRE_ADD_GUILD_CONTRIBUTION
inline void SendRequestGuildContributionRank()
{
	CClientSessionManager::GetInstance().SendPacket( CS_GUILD, eGuild::CS_GUILD_CONTRIBUTION_RANK, NULL, 0 ); 
}
#endif