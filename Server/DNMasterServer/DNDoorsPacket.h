
#pragma once

namespace DoorsProtocol
{
	enum eDND		//DragonNest -> Doors
	{
		DND_REGIST = 1,
		DND_CHAT,
		DND_GUILDCHAT,
	};

	enum eDDN		//Doors -> DragonNest
	{
		DDN_CHAT = 1,
		DDN_GUILDCHAT,
		DDN_SELFKICK,
	};
};

#pragma pack(push, 1)

struct TDNDRegist					//Doors <-> DragonNest
{
	char cWorldID;
	char szIP[IPLENMAX];
	int nPort;
	char szIP2[IPLENMAX];
	int nPort2;
};

struct TDoorsChat					//Doors <-> DragonNest
{
	int nTransactionID;
	unsigned char cPartitionID;
	unsigned char cWorldID;
	__int64 biRoomID;
	__int64 biSrcCharacterID;			//GameUser Src
	__int64 biDestCharacterID;			//MobileUser Dest
	unsigned char cNameCount;
	unsigned short nMessageCount;
	wchar_t wszMessage[NAMELENMAX + CHATLENMAX];
};

struct TDoorsGuildChat				//Doors <-> DragonNest
{
	int nTransactionID;
	unsigned char cWorldID;
	int nGuildID;
	__int64 biSrcCharacterID;			//GameUser Src or MobileUser Src
	unsigned char cNameCount;
	unsigned short nMessageCount;
	wchar_t wszMessage[NAMELENMAX + CHATLENMAX];
};

struct TDoorsSelfKick				//Doors -> DragonNest
{
	int nAccountID;					//MobileUser
	INT64 biCharacterID;			//MobileUser
};

#pragma pack(pop)