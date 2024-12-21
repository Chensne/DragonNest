#pragma once

#include "Connection.h"

class CDNUser;
class CDNPvP;

class CDNVillageConnection: public CConnection
{
private:
	TVillageInfo m_VillageChannelInfo;
	BYTE m_cVillageID;

	bool m_bZeroPopulation;
	int m_nManagedID;
	bool m_bConnectComplete;

public:
	CDNVillageConnection(void);
	virtual ~CDNVillageConnection(void);

	bool GetConnectionCompleted() { return m_bConnectComplete; }
	void SetConnectionCompleted( bool bFlag ){ m_bConnectComplete = bFlag; }

	//NetworkMessage Process
	int MessageProcess(int iMainCmd, int iSubCmd, char * pData, int iLen);

	//VillageServer Channel Operator
	TVillageInfo * GetVillageChannelInfo() { return &m_VillageChannelInfo; }
	int GetMapIdxbyChannel(int nChannelID);
	bool HasManagedMapIdx(int nMapIdx);

	inline BYTE GetVillageID() { return m_cVillageID; }
	inline void SetManagedID(int nManagedID) { m_nManagedID = nManagedID; }
	inline int GetManagedID() { return m_nManagedID; }

#ifdef _WORK
	//For _WORK
	void SendReloadExt();
#endif		//#ifdef _WORK

	//Village -> Master Connection Flow
	void SendRegistWorldID();
	void SendReqUserList(short nRetCode);
	void SendVillageRegistComplete();

	//Village ServerInfo Syncro
	void SendVillageInfo(std::vector <TVillageInfo> * vList);	//정기적 타채널의 상태 알려주기 위함
	void SendVillageInfoDelete(int nVillageID);
	void SendLoginState(UINT nSessionID, short nRet);

	//User
	void SendCheckUser(CDNUser *pUser, UINT nSessionID, int nRet);

	void SendTargetVillageInfo(UINT nAccountDBID, BYTE cVillageID, int nTargetChannelID, int nTargetMapIdx, BYTE cTargetGateNo, const char * pIP, USHORT nPort, short nRet, bool bPartyInto = false, INT64 nItemSerial = 0);
	void SendChangeCharacterName(MAChangeCharacterName* pPacket);

	//User State
	void SendAddUserState(const WCHAR * pName, INT64 biChracterDBID, int nLocationState = -1, int nCommunityState = -1, int nChannelID = -1, int nMapIdx = -1);
	void SendDelUserState(const WCHAR * pName, INT64 biChracterDBID);
	void SendUpdateUserState(const WCHAR * pName, INT64 biChracterDBID, int nLocationState, int nCommunityState, int nChannelID, int nMapIdx);

	//Game
#if defined( PRE_WORLDCOMBINE_PARTY )
#if defined( PRE_WORLDCOMBINE_PVP )
	void SendSetGameID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, ULONG nIP, USHORT nPort, USHORT nTcpPort, int iServerIdx, USHORT wGameID, int iRoomID, USHORT unVillageChannelID, BYTE cVillageID, int nRet, ePartyType Type = _NORMAL_PARTY, WorldPvPMissionRoom::Common::eReqType eWorldReqType = WorldPvPMissionRoom::Common::NoneType );
#else	//	#if defined( PRE_WORLDCOMBINE_PVP )
	void SendSetGameID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, ULONG nIP, USHORT nPort, USHORT nTcpPort, int iServerIdx, USHORT wGameID, int iRoomID, USHORT unVillageChannelID, BYTE cVillageID, int nRet, ePartyType Type = _NORMAL_PARTY );
#endif	//	#if defined( PRE_WORLDCOMBINE_PVP )
#else	//	#if defined( PRE_WORLDCOMBINE_PARTY )
	void SendSetGameID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, ULONG nIP, USHORT nPort, USHORT nTcpPort, int iServerIdx, USHORT wGameID, int iRoomID, USHORT unVillageChannelID, BYTE cVillageID, int nRet );
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	void SendLadderSetGameID( ULONG nIP, USHORT nPort, USHORT nTcpPort, int iServerIdx, USHORT wGameID, int iRoomID, USHORT unVillageChannelID, BYTE cVillageID, int nRet, GAMASetRoomID* pPacket=NULL, int iGameModeTableID=-1 );

	// party
#if defined( PRE_PARTY_DB )	
	void SendResPartyInvite( int iRet, UINT uiReqAccountDBID );
	void SendReqPartyInvite( const VIMAReqPartyInvite* pPacket );
	void SendPartyInviteDenied( const VIMAPartyInviteDenied* pPacket );
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	void SendReqPartyAskJoin( const VIMAReqPartyAskJoin* pPacket );
	void SendResPartyAskJoin( int iRet, UINT uiAccountDBID );
	void SendAskJoinAgreeInfo( UINT uiAccountDBID , TPARTYID PartyID, int iPassword );
#endif
#else
	void SendAddParty(UINT nLeaderAccountDBID, UINT nLeaderSessionID, TPARTYID PartyID, const WCHAR *pwszPartyName, const WCHAR * pwszPassword, int nChannelID, BYTE cPartyMemberMax, TPARTYITEMLOOTRULE ItemLootRule, TITEMRANK ItemRank, BYTE cUserLvLimitMin, BYTE cUserLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cPartyJobDice, UINT nAfterInviteAccountDBID, BYTE cUseVoice);
#endif // #if defined( PRE_PARTY_DB )
	void SendPushParty(UINT nLeaderAccountDBID, TPARTYID PartyID, int nChannelID, int nRandomSeed, TPartyData * pPartyData);
	void SendInvitepartyMember(int nGameServerID, int nVillageMapIdx, UINT nInvitedAccountDBID, const GAMAInvitePartyMember * pInvitePartyMember, const char * pPassClassIds, char cPermitLevel);
	void SendResult(UINT nAccountDBID, BYTE cMainCmd, short cRet);	
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	void SendGetPartyID( UINT nAccountDBID, UINT nSenderAccountDBID );
	void SendGetPartyIDResult( UINT nSenderAccountDBID, TPARTYID PartyID );
#endif

	//Guild

	void SendInviteGuildMember(MAInviteGuildMember* pPacket);
	void SendInviteGuildMemberResult(UINT nAccountDBID, int nRet, bool bAck, LPCWSTR pwszToCharacterName);
	void SendGuildWareInfo(MAGuildWareInfo* pPacket);
	void SendGuildWareInfoResult(MAGuildWareInfoResult* pPacket);
	void SendGuildMemberLevelUp(MAGuildMemberLevelUp* pPacket);
	void SendRefreshGuildItem(MARefreshGuildItem* pPacket);
	void SendRefreshGuildCoin(MARefreshGuildCoin* pPacket);
	void SendExtendGuildWareSize(MAExtendGuildWare* pPacket);
	void SendDismissGuild(MADismissGuild* pPacket);
	void SendAddGuildMember(MAAddGuildMember* pPacket);
	void SendDelGuildMember(MADelGuildMember* pPacket);
	void SendChangeGuildInfo(MAChangeGuildInfo* pPacket);
	void SendChangeGuildMemberInfo(MAChangeGuildMemberInfo* pPacket);
	void SendGuildChat(MAGuildChat* pPacket);
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	void SendDoorsGuildChat(MADoorsGuildChat* pPacket);
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	void SendChangeGuildName(MAGuildChangeName* pPacket);
	void SendChangeGuildMark(MAGuildChangeMark* pPacket);
	void SendUpdateGuildExp(MAUpdateGuildExp* pPacket);
	void SendEnrollGuildWar(MAEnrollGuildWar* pPacket);
	void SendGetGuildWarSchedule();
	void SendChangeGuildWarStep(MAChangeGuildWarStep* pPacket);
	void SendGetGuildWarFinalSchedule(short wScheduleID);
	void SendGetGuildWarPoint();
	void SendSetGuildWarPoint(MASetGuildWarPoint* pPacket);
	void SendAddGuildWarPoint(MAAddGuildWarPoint* pPacket);
	void SendGetGuildWarFinalTeam();
	void SendSetGuildWarFinalTeam(SGuildTournamentInfo* pGuildTournamentInfo);		
	void SendGetGuildWarTournamentInfo(short wScheduleID);
	void SendGetGuildWarPreWinGuild();
	void SendSetGuildWarTournamentInfo(SGuildTournamentInfo* pGuildTournamentInfo);
	void SendGetGuildWarTrialStats();
	void SendSetGuildwarFinalProcess(char cGuildFinalPart, __time64_t tBeginTime);
	void SendSetGuildWarSecretMission(MASetGuildWarSecretMission* pPacket);
	void SendSetGuildWarFinalResult(MAVISetGuildWarFinalResult* pPacket);
	void SendSetGuildWarFinalResultDB(short wScheduleID, char cMatchSequence, UINT GuildDBID, char cMatchTypeCode,bool bWin);
	void SendSetGuildWarPreWinGuild(MAGuildWarPreWinGuild* pPacket);
	void SendSetGuildWarPreWinSkillCoolTime(MAGuildWarPreWinSkillCoolTime* pPacket);
	void SendSetGuildWarSchedule(MASetGuildWarEventTime* pPacket);
	void SendSetGuildWarTournamentWin(MASetGuildWarTournamentWin* pPacket);
	void SendAddDBJobSystemReserve(char cJobType);
	void SendGetDBJobSystemReserve(int nJobSeq);
	void SendGetGuildWarPointRunning();
	void SendSetGuildWarPointRunning(MASetGuildWarPointRunningTotal* pPacket);
	void SendGetGuildWarPreWindGuildReward(short wSchedulID, UINT nGuildDBID); //제스쳐, 농장 지급되었는지 가져오기..
	void SendGuildWarRefreshGuildPoint(MAGuildWarRefreshGuildPoint* pPacket);
	void SendResetGuildWarBuyedItemCount();

	void SendGuildRecruitMember(MAGuildRecruitMember* pPacket);
	void SendAddGuildRewardItem(MAGuildRewardItem* pPacket);
	void SendExtendGuildSize(MAExtendGuildSize* pPacket);
	void SendUpdateGuildWare(int nGuildID);

	//Friend
	void SendFriendAddNotice(UINT nAddedAccountDBID, const WCHAR * pName);

	// chat
	void SendPrivateChat(UINT nToAccountDBID, const WCHAR *pwszFromCharacterName, char cType, const WCHAR *pwszChatMsg, short wChatLen, int nRet = ERROR_NONE);
	void SendChat(char cType, UINT nToAccountDBID, const WCHAR *pwszFromCharacterName, const WCHAR *pwszChatMsg, short wChatLen);
	void SendZoneChat(int nMapIdx, const WCHAR *pwszFromCharacterName, const WCHAR *pwszChatMsg, short wChatLen);
	void SendWorldSystemMsg(const WCHAR *pwszFromCharacterName, char cType, int nID, int nValue, WCHAR* pwszToCharacterName);
#if defined( PRE_PRIVATECHAT_CHANNEL )
	void SendPrivateChannelChat( const WCHAR *pwszFromCharacterName, const WCHAR *pwszChatMsg, short wChatLen, INT64 nChannelID);
#endif

	//PvP
	void	SendPvPCreateRoom( const short nRetCode, const CDNPvP* pPvPRoom, const VIMAPVP_CREATEROOM* pPacket );
	void	SendPvPModifyRoom( const short nRetCode, const CDNPvP* pPvPRoom, const VIMAPVP_MODIFYROOM* pPacket );
	void	SendPvPDestroyRoom( const USHORT unVillageChannelID, const UINT uiPvPIndex );
	void	SendPvPLeaveRoom( const short nRetCode, const USHORT unVillageChannelID, const UINT uiPvPIndex, const UINT uiLeaveAccountDBID, PvPCommon::LeaveType::eLeaveType Type );
	void	SendPvPRoomList( const VIMAPVP_ROOMLIST* pPacket );
	void	SendPvPWaitUserList( const VIMAPVP_WAITUSERLIST* pPacket );
	void	SendPvPJoinRoom( const short nRetCode, const UINT uiAccountDBID, const USHORT unVillageChannelID=0, const UINT uiPvPIndex=0, const USHORT unPvPTeam=PvPCommon::Team::Max, const UINT uiUserState=PvPCommon::UserState::None, char cIndex = -1);
	void	SendPvPRoomState( const USHORT unVillageChannelID, const UINT uiPvPIndex, const UINT uiRoomState );
	void	SendPvPChangeTeam( const short nRetCode, const USHORT unVillageChannelID, const UINT uiPvPIndex, const VIMAPVP_CHANGETEAM* pPacket, char cTeamSlotIndex = -1 );
	void	SendPvPChangeUserState( const USHORT unVillageChannelID, const UINT uiPvPIndex, const UINT uiAccountDBID, const UINT uiUserState );
	void	SendPvPStart( const short nRetCode, const USHORT unVillageChannelID, const UINT uiPvPIndex, const UINT uiAccountDBID );
	void	SendPvPStartMsg( const USHORT unVilalgeChannelID, const UINT uiPvPIndex, const BYTE cSec );
	void	SendPvPRoomInfo( CDNUser* pUser, CDNPvP* pPvP );
	void	SendPvPMemberGrade(UINT nAccountDBID, USHORT nTeam, UINT uiUserState, UINT nChangedSessionID, int nRetCode);
	void	SendPvPMemberIndex(USHORT nTeam, const VIMAPVPSwapMemberIndex * pIndex, int nRetCode);
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	void	SendPvPRoomListRelay(const BYTE cVillageID, const VIMAPVP_ROOMLIST* pPacket);
	void	SendPvPRoomListRelayAck(PVP_ROOMLIST_RELAY* pPacket);
#endif
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void SendPvPChangeChannelResult(UINT nAccountDBID, BYTE cType, int nRetCode);
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

#if defined(PRE_ADD_PVP_TOURNAMENT)
	void	SendPvPSwapTournamentIndex( const short nRetCode, const UINT nAccounDBID, const char cSourceIndex, const char cDestIndex);	
	void	SendPvPShuffleTournamentIndex(CDNPvP* pPvPRoom);
#endif
	// LadderSystem
	void	SendLadderSystemDelUser( CDNUser* pUser );
	void	SendLadderSystemRoomSync( GAMAPVP_LADDERROOMSYNC* pPacket );

	// MasterSystem
	void SendMasterSystemCheckMasterApplication( short nRet, UINT uiPupilAccountDBID, INT64 biMasterCharacterDBID );
	void SendMasterSystemSyncSimpleInfo( UINT uiAccountDBID, INT64 biCharacterDBID, MasterSystem::EventType::eCode EventCode );
	void SendMasterSystemSyncJoin( UINT uiAccountDBID, INT64 biCharacterDBID, bool bIsAddPupil );
	void SendMasterSystemSyncLeave( UINT uiAccountDBID, INT64 biCharacterDBID, bool bIsDelPupil );
	void SendMasterSystemSyncGraduate( UINT uiAccountDBID, MasterSystem::VIMASyncGraduate* pPacket );
	void SendMasterSystemSyncConnect( UINT uiAccountDBID, bool bIsConnect, WCHAR* pwszCharName );
	void SendMasterSystemCheckLeave( int iRet, UINT uiAccountDBID, INT64 biDestCharacterDBID, bool bIsMaster );
	void SendMasterSystemRecallMaster( int iRet, UINT uiPupilAccountDBID, WCHAR* pwszCharName, bool bIsConfirm );
	void SendMasterSystemJoinConfirm( const UINT uiMasterAccountDBID, const BYTE cLevel, const BYTE cJob, WCHAR* pwszPupilCharName);
	void SendMasterSystemJoinConfirmResult( const int iRet, const bool bIsAccept, const INT64 biMasterCharacterDBID, WCHAR* pwszPupilCharName);

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	void SendSyncSystemMail( UINT uiAccountDBID, VIMASyncSystemMail* pMail );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	// Cheat 관련
	void SendResRecall( CDNUser* pRecallUser, const VIMAReqRecall* pPacket );
	void SendVillageTrace( CDNUser* pTargetUser, UINT uiAccountDBID, bool bIsGMCall, char cSelectedLang );
	void SendUserRestraint(UINT nAccountDBID);

	//voicechat
	void SendVoiceChannelID(TINSTANCEID InstanceID, int nVoiceChannelType, int nPvPLobbyChannelID, UINT *nVoiceChannelID);

	void SendFarmSync( UINT uiAccountDBID, INT64 biCharacterDBID, Farm::ServerSyncType::eType Type );

	//notice
	void SendNotice(const WCHAR * pMsg, const int nLen, int nSec);
	void SendNoticeZone(int nMapIndex, const WCHAR * pMsg, const int nLen, int nSec);
	void SendNoticeChannel(int nChannelID, const WCHAR * pMsg, int nLen, int nSec);
	void SendNoticeServer(int nMID, const WCHAR * pMsg, int nLen, int nSec);
	void SendNoticeCancel();

	void SendDetachUser(UINT nAccountDBID, bool bIsDuplicate=false, UINT nSessionID=0);

#if defined(_CH)
	void SendFCMState(UINT nAccountDBID, int nOnline, bool bSend);	// 피로도 상태 알리기
#endif	// _CH

	void SendNotifyMail(UINT nToAccountDBID, INT64 biToCharacterDBID, short wTotalMailCount, short wNotReadMailCount, short w7DaysLeftCount, bool bNewMail);
	void SendNotifyMarket(UINT nSellerAccountDBID, INT64 biSellerCharacterDBID, int nItemID, short wCalculationCount);
	void SendNotifyGift(UINT nToAccountDBID, INT64 biToCharacterDBID, bool bNew, int nGiftCount);

	void SendPCBangResult(UINT nAccountDBID, CDNUser *pUser);
#if defined(PRE_ADD_REMOTE_QUEST)
	void SendUserTempDataResult(UINT uiAccountDBID, CDNUser *pUser);
#else
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	void SendUserTempDataResult(UINT uiAccountDBID, int nDungeonClearCount, GameQuitReward::RewardType::eType eUserGameQuitRewardType);
#else	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
	void SendUserTempDataResult(UINT uiAccountDBID, int nDungeonClearCount);
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#endif

	void SendCheckLastDungeonInfo( UINT uiAccountDBID, INT64 biCharacterDBID, bool bIsCheck, const WCHAR* pwszPartyName );
#if defined( PRE_PARTY_DB )
	void SendConfirmLastDungeonInfo( int iRet, UINT uiAccountDBID, BreakInto::Type::eCode BreakIntoType );
#else
	void SendConfirmLastDungeonInfo( int iRet, UINT uiAccountDBID );
#endif // #if defined( PRE_PARTY_DB )

#if defined(PRE_ADD_QUICK_PVP)
	void SendMakeQuickPvPRoom(int nRet, UINT nMasterAccountDBID, UINT nSlaveAccountDBID);
#endif

#if defined (PRE_ADD_BESTFRIEND)
	void SendSearchBestFriend(MASearchBestFriend* pPacket);
	void SendRegistBestFriend(MARegistBestFriend* pPacket);
	void SendRegistBestFriendResult(MARegistBestFriendResult* pPacket);
	void SendCompleteBestFriend(MACompleteBestFriend* pPacket);
	void SendEditBestFriendMemo(MAEditBestFriendMemo* pPacket);
	void SendCancelBestFriend(MACancelBestFriend* pPacket);
	void SendCloseBestFriend(MACloseBestFriend* pPacket);
	void SendLevelBestFriend(MALevelUpBestFriend* pPacket);
#endif

#if defined( PRE_WORLDCOMBINE_PARTY )
	void SendDelWorldParty( MADelWorldParty *pPacket );
	void SendWorldPartyMember( GAMASendWorldPartyMember *pPacket );
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	void SendPrivateChatChannelAdd(MAAddPrivateChannel* pPacket);
	void SendPrivateChatChannelMemberAdd(MAAddPrivateChannelMember* pPacket);
	void SendPrivateChatChannelMemberInvite(MAInvitePrivateChannelMember* pPacket);
	void SendPrivateChatChannelMemberInviteResult(MAInvitePrivateChannelMemberResult* pPacket);
	void SendPrivateChatChannelMemberDel(MADelPrivateChannelMember* pPacket);
	void SendPrivateChatChannelMemberKickResult(MAKickPrivateChannelMemberResult* pPacket);
	void SendPrivateChatChannelMod(MAModPrivateChannel* pPacket);
	void SendPrivateChatChannelModMemberName(MAModPrivateChannelMemberName* pPacket);
#endif
#if defined(PRE_ADD_MUTE_USERCHATTING)
	void SendMuteUserChatting(MAMuteUserChat* pPacket);
#endif	// #if defined(PRE_ADD_MUTE_USERCHATTING)
#if defined( PRE_WORLDCOMBINE_PVP )
	void SendDelWorldPvPRoom( MADelWorldPvPRoom *pPacket );
	void SendWorldPvPRoomJoinResult( MAWorldPvPRoomJoinResult *pPacket );
	void SendWorldPvPRoomGMCreateResult( MAWorldPvPRoomGMCreateResult *pPacket );
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	void SendAlteiaWorldSendTicket( MAAlteiaWorldSendTicket *pPacket );
	void SendAlteiaWorldSendTicketResult( MAAlteiaWorldSendTicketResult *pPacket );
#endif
#if defined(PRE_ADD_CHNC2C)
	void SendC2CGetCoinBalance( INT64 biCharacterDBID, const char* szSeqID );
	void SendC2CAddCoin( UINT uiAccountDBID,INT64 biCharacterDBID, INT64 biAddCoin, const char* szSeqID, const char* szBookID );
	void SendC2CReduceCoin( UINT uiAccountDBID,INT64 biCharacterDBID, INT64 biReduceCoin, const char* szSeqID, const char* szBookID );
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined( PRE_DRAGONBUFF )
	void SendApplyWorldBuff( MAApplyWorldBuff *pPacket );
#endif
#if defined(PRE_ADD_DWC)
	void SendInviteDWCTeamMember(MAInviteDWCTeamMember* pPacket);	
	void SendInviteDWCTeamMemberAck(MAInviteDWCTeamMemberAck* pPacket);
	void SendInviteDWCTeamMemberAck(UINT nAccountDBID, int nRet);
	void SendAddDWCTeamMember(MAAddDWCTeamMember* pPacket);
	void SendLeaveDWCTeamMember(MALeaveDWCTeamMember* pPacket);
	void SendDismissDWCTeam(MADismissDWCTeam* pPacket);
	void SendChangeDWCTeamMemberState(MAChangeDWCTeamMemberState* pPacket);
	void SendUpdateDWCScore(MADWCUpdateScore *pPacket);
#endif
#if defined( PRE_PVP_GAMBLEROOM )
	void SendGambleRoomCreate(int nTotalGambleRoomCount );
#endif
};

