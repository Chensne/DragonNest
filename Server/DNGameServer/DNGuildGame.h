#pragma once

#include "DNGuildBase.h"
#include "DNRUDPGameServer.h"
#include "DNGameServerManager.h"

class CDNGuildGame : public CDNGuildBase, public TBoostMemoryPool<CDNGuildGame>
{
public:
	CDNGuildGame();
	~CDNGuildGame();

	void	Reset();

	bool	CheckGuildResource(TGuildMember* pGuildMemeber);

	void	SendGuildMemberLoginList(SCGuildMemberLoginList* pPacket);
	void	SendAddGuildMember(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszCharacterName, TP_JOB nJob, char cLevel, __time64_t pJoinDate, TCommunityLocation* pLocation);
	void	SendDelGuildMember(UINT nAccountDBID, INT64 nCharacterDBID, bool bIsExiled);
	void	SendChangeGuildMemberInfo(UINT nReqAccountDBID, INT64 nReqCharacterDBID, UINT nChgAccountDBID, INT64 nChgCharacterDBID, BYTE btGuildMemberUpdate, int iInt1, int iInt2, INT64 biInt64, LPCWSTR pText);
	void	SendGuildChat(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen);
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	void SendDoorsGuildChat(INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen);
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	void	SendChangeGuildName(const wchar_t* wszGuildName);
	void	SendChangeGuildMark(const MAGuildChangeMark *pPacket);
	void	UpdateGuildInfo(CDNUserSession* pSession, const TAChangeGuildInfo *pPacket);
	void	UpdateMemberInfo(CDNUserSession* pSession, const TAChangeGuildMemberInfo *pPacket);
	void	UpdateWarInfoOnAllMember(short wScheduleID, BYTE cTeamColorCode);	
	void	SendGuildLevelUp(int nLevel);
	void	SendUpdateGuildExp(BYTE cPointType, int nPointValue, int nGuildExp, INT64 biCharacterDBID, int nMissionID);
	void	SendGuildRewardItem( TGuildRewardItem* GuildRewardItem );
	void	SendAddGuildRewardItem( TGuildRewardItem GuildRewardItem ) {};	
	void	SendExtendGuildSize(short nGuildSize) {};
};