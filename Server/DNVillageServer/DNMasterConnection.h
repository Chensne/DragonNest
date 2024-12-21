#pragma once

#include "Connection.h"
#include "DNParty.h"

//----------------------------------------------------------------------------------
//								CDNMasterConnection
//
//	Master에 보내는 유저관련 패킷들은 SessionID 대신에 AccountDBID으로 보내줘야한다.
//  중복체크를 막기위해 AccountDBID으로 유저를 찾고있다.
//----------------------------------------------------------------------------------
class CDNPeriodQuest;
class CDNUserSession;
namespace LadderSystem
{
	class CRoom;
};
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
class CDNPvPRoom;
#endif

class CDNMasterConnection: public CConnection
{
public:
	std::vector < std::pair < int, sChannelInfo > > m_vChannelList;

public:
	CDNMasterConnection(void);
	virtual ~CDNMasterConnection(void);

	void Reconnect();
	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);

	//Monitor Process Elapsed Tick
	void SendProcessDelayed(UINT nDelayedTick);

	//Master <-> Village Connect Flow
	void SendVIllageRegistInfo(int nManagedID, const char * pszIP, short nPort);
	void SendEndofVillageInfo();
	void SendAddUserList(CDNUserSession *pUserObj);

	//User Connections
	void SendDelUser(UINT nAccountDBID, UINT nSessionID);
	void SendCheckUser(UINT nSessionID);
	void SendEnterVillage(UINT nAccountDBID, int nChannelID);
	void SendCheckReconnectLogin(UINT nAccountDBID);

	//Move Server
#if defined( PRE_PARTY_DB )
	void SendVillageToVillage(BYTE cReqType, INT64 biID, int nMapIndex, int nEnteredGateIndex = -1, int nMoveChannel = -1, CDNParty * pParty = NULL, INT64 nItemSerial = 0);
#else
	void SendVillageToVillage(BYTE cReqType, UINT nID, int nMapIndex, int nEnteredGateIndex = -1, int nMoveChannel = -1, CDNParty * pParty = NULL, INT64 nItemSerial = 0);
#endif // #if defined( PRE_PARTY_DB )
	void SendReqGameID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, USHORT unVillageChannelID, int nRandomSeed, int nMapIndex, char cGateNo, TDUNGEONDIFFICULTY Difficulty, bool bDirectConnect = false, CDNParty * pParty = NULL, UINT nFarmDBID = 0, char cGateSelect = 0 );
	void SendPvPLadderEnterChannel( UINT uiAccountDBID, LadderSystem::MatchType::eCode MatchType );
	void SendReqLadderGameID( LadderSystem::CRoom* pLadderRoom, LadderSystem::CRoom* pLadderRoom2, USHORT unVillageChannelID, int iRandomSeed, int iMapIndex );
	void SendLadderObserver( INT64 biCharacterDBID, int iGameServerID, int iGameRoomID );
	void SendLoginState(UINT nSessionID);

	//Village ChannelUser Count
	void SendVillageUserReport();

	//Party
#if defined( PRE_PARTY_DB )
	void SendReqPartyInvite( CDNUserSession* pReqSession, const WCHAR* pwszInviteCharName, CDNParty* pParty );
	void SendResPartyInvite( int iRet, UINT uiReqAccountDBID );
	void SendPartyInviteDenied( const WCHAR* pwszReqCharName, TPARTYID PartyID, const WCHAR* pwszInviteCharName, BYTE cIsOpenBlind );
#else
	void SendReqPartyID(UINT nLeaderAccountDBID, int nChannelID, BYTE cMemberMax, TPARTYITEMLOOTRULE LootRule, TITEMRANK LootItemRank, int nUserLvLimitMin, \
		int nUserLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cIsPartyJobDice, const WCHAR * pwszPartyName, const WCHAR * pPass, UINT nAfterInviteDBID = 0, bool bUseVoice = false);
#endif // #if defined( PRE_PARTY_DB )
	void SendInvitePartyResult(int nGameServerID, const WCHAR * pwszInviterName, const WCHAR * pwszInvitedName, int nRetCode);

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	void SendGetPartyID( UINT nSenderAccountDBID , const WCHAR* pwszCharName);
	void SendGetPartyIDResult( UINT nSenderAccountDBID , TPARTYID PartyID );
	void SendReqPartyAskJoin(CDNUserSession* pReqSession, const WCHAR* pwszTargetCharName);
	void SendResPartyAskJoin(int iRet, UINT uiReqAccountDBID, const WCHAR* pwszReqChracterName = NULL );
	void SendAskJoinAgreeInfo(const WCHAR* pwszAskerCharName, TPARTYID PartyID, int iPassword);
#endif

	//Guild
	void SendInviteGuildMember(TGuildUID GuildUID, UINT nAccountDBID, UINT nSessionID, INT64 nCharacterDBID, LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, LPCWSTR lpwszGuildName);
	void SendInviteGuildResult(UINT nAccountDBID, int nRetCode,  bool bAck = false, LPCWSTR lpwszToCharacterName=NULL);
	void SendGuildWareInfo (TGuildUID GuildUID);
	void SendGuildWareInfoResult (MAGuildWareInfoResult& result);
	void SendChangeGuildInfo(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, BYTE btGuildUpdate, int iInt1 = 0, int iInt2 = 0, int iInt3 = 0, int iInt4 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL);
	void SendGuildMemberLevelUp (TGuildUID GuildUID, INT64 nCharacterDBID, char cLevel);
	void SendRefreshGuildItem (const TGuildUID pGuildUID, char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, __time64_t VersionDate);
	void SendRefreshGuildCoin (const TGuildUID pGuildUID, INT64 biTotalCoin);
	void SendExtendGuildWareSize (const TGuildUID pGuildUID, short wSize);
	void SendDismissGuild(const TGuildUID pGuildUID);
	void SendAddGuildMember(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszFromCharacterName, TP_JOB nJob, char cLevel, __time64_t pJoinDate);
	void SendDelGuildMember(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, bool bIsExiled);
	void SendChangeGuildMemberInfo(const TGuildUID pGuildUID, UINT nReqAccountDBID, INT64 nReqCharacterDBID, UINT nChgAccountDBID, INT64 nChgCharacterDBID, BYTE btGuildMemberUpdate, int iInt1 = 0, int iInt2 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL, bool bReturn = false);
	void SendGuildChat(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen);
	void SendGuildChangeName(const TGuildUID pGuildUID, LPCWSTR lpwszGuildName);
	void SendGuildChangeMark(const TGuildUID pGuildUID, short wMark, short wMarkBG, short wMarkBorder);

	void SendEnrollGuildWar(const TGuildUID pGuildUID, short wScheduleID, BYTE cTeamColorCode);	
	void SendSetGuildWarFinalSchedule(TGuildWarEventInfo* pFinalSchedule);
	void SendSetGuildWarPoint(int nResultCode, int nBlueTeamPoint, int nRedTeamPoint);
	void SendAddGuildWarPoint(char cTeamType, TGuildUID GuildUID, int nAddPoint);
	void SendSetGuildWarTournamentInfo(TAGetGuildWarFinalResults* pGuildWarFinalResults);
	void SendSetGuildWarFinalTeam(TAGetGuildWarPointGuildTotal* pGuildWarPointGuildTotal);	
	void SendAddGuildPoint(TGuildUID GuildUID, int nAddPoint);
	void SendSetGuilldWarPreWinSkillCoolTime(int nManagedID, DWORD dwSkillCoolTime);
	void SendAddJobReserve(int nResultCode, char cJobType, int nJobSeq);
	void SendGetJobReserve(int nResultCode, int nJobSeq, char cJobStatus);
	void SendMatchListSaveResult(int nResultCode);	

	void SendSetGuildWarPointRunning(TAGetGuildWarPointRunningTotal* pGuildWarPointRunningTotal );
	void SendSetGuildWarPreWinGuild(TGuildUID GuildUID, short wScheduleID);
	void SendSetGuildWarPreWinReward();
	void SendSetGuildWarSchedule(int nResultCode, short wSchdeduleID, short wWinersWeightRate, TGuildWarEventInfo* pEventInfo, bool bForce, bool bFinalProgress, __time64_t tRewardExpireDate);
	void SendGuildWarRefreshGuildPoint(TAGetGuildWarPointFinalRewards* pPointFinalRewards );
	void SendGuildWarScheduleReload();
	void SendUpdateGuildExp(const TGuildUID, const TAModGuildExp* pPacket);
	void SendGuildRecruitMemberResult( const TGuildUID pGuildUID, WCHAR* wszCharacterName, INT64 nCharacterDBID, TGuildView GuildView, GuildRecruitSystem::AcceptType::eType type );	// 가입된 캐릭터에게 통보
	void SendAddGuildRewardItem( const TGuildUID pGuildUID, TAAddGuildRewardItem* GuildRewardItem );	// 길드 효과 알림
	void SendExtendGuildSize ( const TGuildUID pGuildUID, short wSize );

	// MasterSystem
	void SendCheckMasterApplication( const INT64 biMasterCharacterDBID, const INT64 biPupilCharacterDBID );
	void SendMasterSystemSyncSimpleInfo( const INT64 biCharacterDBID, MasterSystem::EventType::eCode Type );
	void SendMasterSystemSyncJoin( const INT64 biCharacterDBID, bool bIsAddPupil );
	void SendMasterSystemSyncLeave( const INT64 biCharacterDBID, bool bIsDelPupil );
	void SendMasterSystemSyncGraduate( const INT64 biCharacterDBID, WCHAR* pwszCharName );
	void SendMasterSystemSyncConnect( const bool bIsConnect, WCHAR* pwszCharName, const TMasterSystemData* pMasterSystemData );
	void SendMasterSystemCheckLeave( UINT uiAccountDBID, const INT64 biDestCharacterDBID, bool bIsMaster );
	void SendMasterSystemRecallMaster( UINT uiPupilAccountDBID, WCHAR* pwszPupilCharName, WCHAR* pwszMasterCharName, const TMasterSystemData* pMasterSystemData );
	void SendMasterSystemBreakInto( int iRet, UINT uiMasterAccountDBID, WCHAR* pwszPupilCharName, const TMasterSystemData* pMasterSystemData );
	void SendMasterSystemJoinConfirm( const INT64 biMasterCharacterDBID, const BYTE cLevel, const BYTE cJob, WCHAR* pwszPupilCharName);
	void SendMasterSystemJoinConfirmResult( const int iRet, const bool bIsAccept, const INT64 biMasterCharacterDBID,  WCHAR* pwszPupilCharName);
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	void SendSyncSystemMail( TASendSystemMail* pMail );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	//Friend
	void SendFriendAddNotify(UINT nAddedAccountDBID, const WCHAR * pAddName);

	// chat
#ifdef PRE_ADD_DOORS
	void SendPrivateChat(UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen, INT64 biDestCharacterDBID = 0);
#else		//#ifdef PRE_ADD_DOORS
	void SendPrivateChat(UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen);
#endif		//#ifdef PRE_ADD_DOORS
	void SendChat(char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen, int nMapIdx = -1);
	void SendWorldSystemMsg(UINT nFromAccountDBID, char cType, int nID, int nValue, const WCHAR *pwszToCharacterName = NULL);
	void SendWorldSystemMsg888(UINT nFromAccountDBID, char cType, int nID, int nValue, const WCHAR *pwszToCharacterName = NULL);
#if defined( PRE_PRIVATECHAT_CHANNEL )
	void SendPrivateChannelChat(char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen, INT64 nChannelID);
#endif

	// VoiceChat
	void SendReqVoiceChannelID( TINSTANCEID InstanceID, int nChannelType, int nPvPLobbyChannelID = 0);

	// PvP
	void SendMovePvPLobbyToPvPVillage(UINT nAccountDBID );
#if defined(PRE_ADD_PVP_TOURNAMENT)
#if defined( PRE_PVP_GAMBLEROOM )
	void SendPvPCreateRoom( const UINT uiCreateAccountDBID, const BYTE cGameMode, const USHORT unVillageChannelID, const UINT uiVillageMapIndex, const CSPVP_CREATEROOM* pPacket, TEvent* pEvent=NULL, BYTE cJob=0, TPvPGambleRoomData* GambleData=NULL );
#else // #if defined( PRE_PVP_GAMBLEROOM )
	void SendPvPCreateRoom( const UINT uiCreateAccountDBID, const BYTE cGameMode, const USHORT unVillageChannelID, const UINT uiVillageMapIndex, const CSPVP_CREATEROOM* pPacket, TEvent* pEvent=NULL, BYTE cJob=0 );
#endif // #if defined( PRE_PVP_GAMBLEROOM )
#else // #if defined(PRE_ADD_PVP_TOURNAMENT)
	void SendPvPCreateRoom( const UINT uiCreateAccountDBID, const BYTE cGameMode, const USHORT unVillageChannelID, const UINT uiVillageMapIndex, const CSPVP_CREATEROOM* pPacket, TEvent* pEvent=NULL );
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
	void SendPvPModifyRoom( const UINT uiAccountDBID, const BYTE cGameMode, const CSPVP_MODIFYROOM* pPacket );
	void SendPvPLeaveRoom( const UINT uiLeaveAccountDBID );
	void SendPvPChangeCaptain( const UINT uiAccountDBID, const CSPVP_CHANGECAPTAIN* pPacket );
	void SendPvPRoomList( const UINT uiAccountDBID, const USHORT unVillageChannelID, const CSPVP_ROOMLIST* pPacket );
	void SendPvPWaitUserList( const UINT uiAccountDBID, const USHORT unVillageChannelID, const CSPVP_WAITUSERLIST* pPacket );

	void SendPvPJoinRoom( const UINT uiAccountDBID, BYTE cUserLevel, const CSPVP_JOINROOM* pPacket, CDNUserSession * pSession );
	void SendPvPRandomJoinRoom( const UINT uiAccountDBID, BYTE cUserLevel );
	void SendPvPReady( const UINT uiAccountDBID, const CSPVP_READY* pPacket );
	void SendPvPStart( const UINT uiAccountDBID, const CSPVP_START* pPacket );
	void SendPvPChangeTeam( const UINT uiAccountDBID, BYTE cLevel, const CSPVP_CHANGETEAM* pPacket );
	void SendPvPBan( const UINT uiAccountDBID, const CSPVP_BAN* pPacket );
	void SendPvPEnterLobby( const UINT uiAccountDBID, const USHORT unVillageChannelID );
	void SendPvPLadderInviteUser( UINT uiAccountDBID, const WCHAR* pwszCharName );
	void SendPvPLadderInviteConfirm( UINT uiAccountDBID, const WCHAR* pwszConfirmCharName, const LadderSystem::CS_INVITE_CONFIRM* pPacket );
#if defined( PRE_ADD_PVP_VILLAGE_ACCESS)
	void SendPvPRoomListRelay( const BYTE cVIllageID, const UINT uiAccountDBID, const short nRetCode, std::vector<CDNPvPRoom*>& vPvPRoom, const UINT uiMaxPage);
#endif
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void SendPvPChangeChannel(UINT nAccountDBID, BYTE cType, BYTE cPvPLevel, BYTE cSeperateLevel, bool bSend = true);
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	//
	void SendPvPFatigueOption( const UINT uiAccountDBID, bool bFatigueOption);
	void SendPvPSwapMemberIndex(UINT nAccountDBID, BYTE cCount, const TSwapMemberIndex * pIndex);
	void SendPvPChangeMemberGrade(UINT nAccountDBID, USHORT nType, UINT nSessionID, bool bAsign);
	void SendNotifyMail(UINT nToAccountDBID, INT64 biToCharacterDBID, short wTotalMailCount, short wNotReadMailCount, short w7DaysLeftCount, bool bNewMail);
	void SendNotifyMarket(UINT nSellerAccountDBID, INT64 biSellerCharacterDBID, int nItemID, short wCalculationCount);
	void SendNotifyGift(UINT nToAccountDBID, INT64 biToCharacterDBID, bool bNew, int nGiftCount);
#if defined(PRE_ADD_PVP_TOURNAMENT)
	void SendPvPSwapTournamentIndex(UINT nAccountDBID, char cSourceIndex, char cDestIndex);
#endif

#if defined( PRE_WORLDCOMBINE_PVP )
	void SendWorldPvPCreateRoom( int nRoomIndex, TWorldPvPMissionRoom Data, UINT nAccountDBID = 0 );
	void SendWorldPvPJoinRoom( UINT nAccountDBID, INT64 biCharacterDBID, int nRoomIndex, TWorldPvPRoomDBData *Data, UINT uiTeam, bool bPvPLobby );
#endif

	//Farm
	void SendFarmListGot(TFarmItemFromDB * pFarm, BYTE cCount);

	//Cheat Notice
	void SendNoticeFromClient(const WCHAR * pMsg, int nLen);
	void SendZoneNoticeFromClient(int nMapIndex, const WCHAR * pMsg, int nLen);

	// 치트관련
#if defined(PRE_ADD_MULTILANGUAGE)
	void SendTraceBreakInto(int nType, const UINT uiAccountDBID, const WCHAR* pwszCharacterName, char cSelectedLang );
	void SendReqRecall( const UINT uiAccountDBID, const UINT uiRecallChannelID, const WCHAR* pwszCharacterName, char cSelectedLang );
	void SendBanUser(UINT nAccountDBID, const WCHAR * pCharacterName, char cSelectedLang);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	void SendTraceBreakInto(int nType, const UINT uiAccountDBID, const WCHAR* pwszCharacterName );
	void SendReqRecall( const UINT uiAccountDBID, const UINT uiRecallChannelID, const WCHAR* pwszCharacterName );
	void SendBanUser(UINT nAccountDBID, const WCHAR * pCharacterName);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	void SendForceDestroyPvP(UINT nAccountDBID, int nPvPIndex, const WCHAR * pForceWinGuildName);

#if defined(PRE_ADD_QUICK_PVP)
	void SendMakeQuickPvPRoom(UINT nMasterAccountDBID, UINT nSlaveAccountDBID);
#endif

	//
	bool				VerifyChannelID(int nChannelID, int nMapIdx);
	int					GetChannelIndex(int nChannelID);
	const sChannelInfo*	GetChannelInfo( const int nChannelID );
	const sChannelInfo*	GetChannelInfoFromMapIndex( const int nMapIndex );
	const sChannelInfo* GetChannelInfoFromAttr( const int iAttr );
	int GetChannelMapIndex(int nChannelID);
	int GetMeritBonusID(int nChannelID);

	//Connection Verify
	void SetInComplete() { m_bConnectCompleted = false; }
	bool GetActive();

	void SendUpdateChannelShowInfo( USHORT unChannelID, bool bShow, int nServerID, BYTE cThreadID );
#if defined(_KR) || defined(_TH) || defined(_ID)
	void SendPCBangResult(UINT nAccountDBID);
#endif	// _KR

	void SendChangeCharacterName( const TGuildUID pGuildUID, char cWorldSetID, UINT uiAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszOriginName, LPCWSTR lpwszCharacterName, INT64 biBFCharacterDBID=0 );
	void SendSaveUserTempData(CDNUserSession* pSession);
	void SendLoadUserTempData(UINT uiAccountDBID);

	void SendDuplicateLogin(UINT nAccountDBID, bool bIsDetach, UINT nSessionID);

	// Quest
	void SendWorldEventCounter (char cWorldSetID, int nScheduleID, int nCount);
	void SendCheckLastDungeonInfo( CDNUserSession* pSession, INT64 biValue );
#if defined( PRE_PARTY_DB )
	void SendConfirmLastDungeonInfo( CDNUserSession* pSession, INT64 biValue, BreakInto::Type::eCode BreakIntoType=BreakInto::Type::PartyRestore, bool bIsOK=true );
#else
	void SendConfirmLastDungeonInfo( CDNUserSession* pSession, INT64 biValue, bool bIsOK=true );
#endif // #if defined( PRE_PARTY_DB )

#if defined (PRE_ADD_BESTFRIEND)
	void SendSearchBestFriend(UINT nAccountDBID, int nRet, bool bAck, LPCWSTR lpwszCharacterName, char cLevel = 0, char cJob = 0);
	void SendRegistBestFriend(UINT nAccountDBID, INT64 biCharacterDBID, LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, int nRegistItemID);
	void SendRegistBestFriendResult(UINT nAccountDBID, int nRetCode, bool bAck, INT64 biFromCharacterDBID, LPCWSTR lpwszFromCharacterName, UINT nToAccountDBID, INT64 biToCharacterDBID, LPCWSTR lpwszToCharacterName);
	void SendCompleteBestFriend(UINT nFromAccountDBID, UINT nToAccountDBID, INT64 biCharacterDBID, LPCWSTR lpwszCharacterName, int nItemID);
	void SendEditBestFriendMemo(UINT nFromAccountDBID, INT64 biToCharacterDBID, LPCWSTR lpwszToMemo);
	void SendCancelBestFriend(LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, bool bCancel);
	void SendCloseBestFriend(LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName);
	void SendLevelUpBestFriend(BYTE cLevel, LPCWSTR lpwszCharacterName);
#endif // #if defined (PRE_ADD_BESTFRIEND)

#if defined( PRE_WORLDCOMBINE_PARTY )
	void SendGetWorldPartyMember(UINT nAccountDBID, INT64 biToCharacterDBID, const Party::Data &PartyData);
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	void SendAddPrivateChatChannel(TPrivateChatChannelInfo tPrivateChatChannel);
	void SendAddPrivateChatChannelMember(INT64 nChannelID, TPrivateChatChannelMember Member);
	void SendInvitePrivateChatChannel(INT64 nChannelID, UINT nAccountDBID, WCHAR* wszInviteCharacterName);
	void SendInvitePrivateChatChannelResult(int nRet, UINT nAccountDBID);
	void SendDelPrivateChatChannelMember(PrivateChatChannel::Common::eModType eType, INT64 nChannelID, INT64 biCharacterDBID, const WCHAR* wszName);
	void SendKickPrivateChatChannelMemberResult(INT64 biCharacterDBID);
	void SendModPrivateChatChannelInfo(INT64 nChannelID, PrivateChatChannel::Common::eModType eModType, int nPassWord, INT64 biCharacterDBID);	
	void SendModPrivateChatChannelMemberName(INT64 nChannelID, INT64 biCharacterDBID, const WCHAR* wszName );	
#endif
#if defined( PRE_ADD_MUTE_USERCHATTING )
#if defined(PRE_ADD_MULTILANGUAGE)
	void SendMuteUserFind(UINT uiAccountDBID, char cWorldSetID, const WCHAR * pMuteCharacterName, int nMuteMinute, char cSelectLanguage );
#else	// #if defined(PRE_ADD_MULTILANGUAGE)
	void SendMuteUserFind(UINT uiAccountDBID, char cWorldSetID, const WCHAR * pMuteCharacterName, int nMuteMinute );
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING )

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	void SendAddAlteiaWorldSendTicket( char cWorldSetID, const WCHAR* wszRecvCharacterName, INT64 biSendCharacterDBID, WCHAR* wszSendCharacterName);
	void SendAddAlteiaWorldSendTicketResult( int nRetCode, INT64 biSendCharacterDBID );
#endif
#if defined(PRE_ADD_CHNC2C)
	void SendC2CCoinBalanceResult( int nRetCode, Int64 biCoinBalance, const char* szSeqID);
	void SendC2CAddCoinResult( int nRetCode, const char* szSeqID);
	void SendC2CReduceCoinResult( int nRetCode, const char* szSeqID);
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined( PRE_DRAGONBUFF )
	void SendApplyWorldBuff( WCHAR *wszCharacterName, int nItemID, int nMapIdx );
#endif

#if defined(PRE_ADD_DWC)
	void SendInviteDWCTeamMember(UINT nTeamID, UINT nAccountDBID, LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, LPCWSTR lpwszTeamName);
	void SendInviteDWCTeamMemberACK(UINT nAccountDBID, int nRetCode, bool bAck, LPCWSTR pwszToCharacterName);
	void SendAddDWCTeamMember(UINT nTargetUserAccountDBID, UINT nTeamID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszFromCharacterName, TP_JOB nJob, TCommunityLocation* pLocation, bool bAlredySentByVillage);
	void SendDismissDWCTeam(UINT nTargetUserAccountDBID, UINT nTeamID, int nRet, bool bAlredySentByVillage);
	void SendLeaveDWCTeamMember(UINT nTargetUserAccountDBID, UINT nTeamID, INT64 biLeaveUserCharacterDBID, LPCWSTR pwszCharacterName, int nRet, bool bAlredySentByVillage);
	void SendChangeDWCTeamMemberState(UINT nTargetUserAccountDBID, UINT nTeamID, INT64 biCharacterDBID, TCommunityLocation* pLocation, bool bLogin);
	void SendDWCTeamMemberList(UINT nAccountDBID, UINT nTeamID, std::vector<INT64> &VecMemberList);
	void SendDWCTeamChat(UINT nTeamID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen);
#endif

#if defined( PRE_PVP_GAMBLEROOM )
	void SendCreateGambleRoomFlag(bool bFlag);
	void SendDelGambleRoom();
#endif

private:
	bool	_CmdRecall( MAVIResRecall* pPacket );
	bool	_CmdVillageTrace( MAVIVillageTrace* pPacket );

	bool m_bConnectCompleted;
};

extern CDNMasterConnection* g_pMasterConnection;
