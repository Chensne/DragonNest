#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"

inline void SendGetGuildRecruitRegisterInfo()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_REGISTERINFO, NULL, 0);
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
inline void SendRegisterGuildRecruit(bool bModify, BYTE *pClassGrade, int nMinLevel, int nMaxLevel, const WCHAR *pGuildIntroduction, BYTE cPurposeCode, bool bCheckHomepage)
{
	GuildRecruitSystem::CSGuildRecruitRegister Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.cRegisterType = bModify ? GuildRecruitSystem::RegisterType::RegisterMod : GuildRecruitSystem::RegisterType::RegisterOn;
	for( int i = 0; i < CLASSKINDMAX; ++i ) Packet.cClassGrade[i] =  pClassGrade[i];
	//Packet.cClassGrade[8] =  1; //lencea   //rlkt 2016 fix machina+lencea
	//Packet.cClassGrade[9] =  1; //machina
	Packet.nMinLevel = nMinLevel;
	Packet.nMaxLevel = nMaxLevel;
	_wcscpy(Packet.wszGuildIntroduction, _countof(Packet.wszGuildIntroduction), pGuildIntroduction, (int)wcslen(pGuildIntroduction));
	Packet.cPurposeCode = cPurposeCode;
	Packet.bCheckHomePage = bCheckHomepage;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_REGISTER, (char*)&Packet, sizeof(Packet));
}
#else
inline void SendRegisterGuildRecruit(bool bModify, BYTE *pClassGrade, int nMinLevel, int nMaxLevel, const WCHAR *pGuildIntroduction)
{
	GuildRecruitSystem::CSGuildRecruitRegister Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.cRegisterType = bModify ? GuildRecruitSystem::RegisterType::RegisterMod : GuildRecruitSystem::RegisterType::RegisterOn;
	for( int i = 0; i < CLASSKINDMAX; ++i ) Packet.cClassGrade[i] = pClassGrade[i];
	Packet.nMinLevel = nMinLevel;
	Packet.nMaxLevel = nMaxLevel;
	_wcscpy(Packet.wszGuildIntroduction, _countof(Packet.wszGuildIntroduction), pGuildIntroduction, (int)wcslen(pGuildIntroduction));

	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_REGISTER, (char*)&Packet, sizeof(Packet));
}
#endif

inline void SendCancelGuildRecruit()
{
	GuildRecruitSystem::CSGuildRecruitRegister Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.cRegisterType = GuildRecruitSystem::RegisterType::RegisterOff;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_REGISTER, (char*)&Packet, sizeof(Packet));
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
inline void SendGetGuildRecruitList(int nPage, BYTE cPurposeCode, const WCHAR *pGuildName, BYTE cSortType)
{
	GuildRecruitSystem::CSGuildRecruitList Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.uiPage = nPage;
	Packet.cPurposeCode = cPurposeCode;
	_wcscpy(Packet.wszGuildName, _countof(Packet.wszGuildName), pGuildName, (int)wcslen(pGuildName));
	Packet.cSortType = cSortType;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_LIST, (char*)&Packet, sizeof(Packet));
}
#else
inline void SendGetGuildRecruitList(int nPage)
{
	GuildRecruitSystem::CSGuildRecruitList Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.uiPage = nPage;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_LIST, (char*)&Packet, sizeof(Packet));
}
#endif

inline void SendGetGuildRecruitRequestCount()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_REQUESTCOUNT, NULL, 0);
}

inline void SendGetGuildRecruitMyList()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_MYLIST, NULL, 0);
}

inline void SendGetGuildRecruitCharacter()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_CHARACTER, NULL, 0);
}

inline void SendRequestJoinGuild(TGuildUID GuildUID, bool bCancel)
{
	GuildRecruitSystem::CSGuildRecruitRequest Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.GuildUID = GuildUID;
	Packet.cRequestType = bCancel ? GuildRecruitSystem::RequestType::RequestOff : GuildRecruitSystem::RequestType::RequestOn;

	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_REQUEST, (char*)&Packet, sizeof(Packet));
}

inline void SendRequestAcceptJoinGuild(bool bAccept, TGuildUID GuildUID, INT64 biAcceptCharacterDBID, const WCHAR *pToCharacterName)
{
	GuildRecruitSystem::CSGuildRecruitAccept Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.cAcceptType = bAccept ? GuildRecruitSystem::AcceptType::AcceptOn : GuildRecruitSystem::AcceptType::AcceptOff;
	Packet.GuildUID = GuildUID;
	Packet.biAcceptCharacterDBID = biAcceptCharacterDBID;
	_wcscpy(Packet.wszToCharacterName, _countof(Packet.wszToCharacterName), pToCharacterName, (int)wcslen(pToCharacterName));

	CClientSessionManager::GetInstance().SendPacket(CS_GUILDRECRUIT, eGuildRecruit::CS_GUILDRECRUIT_ACCEPT, (char*)&Packet, sizeof(Packet));
}