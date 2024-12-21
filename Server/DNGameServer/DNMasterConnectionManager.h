#pragma once

#include "ConnectionManager.h"

class CDNUserSession;
class CDNGameRoom;
class CDNFarmGameRoom;
class CDNPeriodQuest;
class CDNMasterConnection;
class CDNMasterConnectionManager: public CConnectionManager
{
private:
	TMapConnections m_WorldIDConnections;
	CSyncLock m_MasterLock;

	DWORD m_dwReconnectTick;
#ifdef PRE_MOD_GAMESERVER_EXCEPT_MASTERMANAGER
	volatile const bool m_bUseLock;
#endif		//#ifdef PRE_MOD_GAMESERVER_EXCEPT_MASTERMANAGER

public:
	CDNMasterConnectionManager(void);
	~CDNMasterConnectionManager(void);

	void Reconnect(DWORD CurTick);

	CConnection* AddConnection(const char *pIp, const USHORT nPort);
	bool DelConnection(CConnection *pCon);
	void GetConnectedWorldID(std::vector <int> * pvList);
	CDNMasterConnection* AddWorldIDConnection(UINT nConID, char cWorldSetID);

	int GetWorldSetIDs(BYTE * pWorldID);
	bool IsAllWorldMasterConnected();

	//Master Registration
	bool IsAllMasterConnected();
	bool IsMasterConnected(char cWorldID);
	void SendAddUserList(char cWorldSetID, CDNUserSession * pSession);		// Master에 접속된 유저 죄다 보내주기 (Master 죽었다 켜졌을때)
	void SendEndofVillageInfo(char cWorldSetID, UINT nServerID);
#if defined( PRE_WORLDCOMBINE_PARTY )
	void SendConnectComplete( BYTE cWorldSetID );
#else
	void SendConnectComplete();
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )

	//User
	void SendDelUser(char cWorldSetID, UINT nAccountDBID, bool bIsIntended, UINT nSessionID);
	void SendCheckReconnectLogin(char cWorldSetID, UINT nAccountDBID);

	//Room & Server Change
	bool SendEnterGame(char cWorldSetID, UINT nAccountDBID, UINT nRoomID, UINT nServerID);
#if defined( PRE_WORLDCOMBINE_PARTY )
#if defined( PRE_WORLDCOMBINE_PVP )
	bool SendSetRoomID( char cWorldSetID, GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, int iServerIdx, int nRoomID, BYTE cZoneID, UINT * pPartyMember, TPARTYID PartyIDForBreakInto = 0, ePartyType Type = _NORMAL_PARTY, WorldPvPMissionRoom::Common::eReqType eWorldReqType = WorldPvPMissionRoom::Common::NoneType );
#else // #if defined( PRE_WORLDCOMBINE_PVP )
	bool SendSetRoomID( char cWorldSetID, GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, int iServerIdx, int nRoomID, BYTE cZoneID, UINT * pPartyMember, TPARTYID PartyIDForBreakInto = 0, ePartyType Type = _NORMAL_PARTY );
#endif // #if defined( PRE_WORLDCOMBINE_PVP )
#else // #if defined( PRE_WORLDCOMBINE_PARTY )
	bool SendSetRoomID( char cWorldSetID, GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, int iServerIdx, int nRoomID, BYTE cZoneID, UINT * pPartyMember, TPARTYID PartyIDForBreakInto = 0 );
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	void SendSetTutorialRoomID(char cWorldSetID, UINT nAccountDBID, int iServerIdx, int nRoomID, int nLoginServerID);	
	bool SendRequestNextVillageInfo(char cWorldSetID, int nMapIndex, int nEnteredGateIndex, bool bReturnVillage, CDNGameRoom * pRoom);
	bool SendRequestNextVillageInfo( char cWorldSetID, UINT uiAccountDBID, int nMapIndex, int nGateIndex, bool bRetrunVillage, INT64 nItemSerial = 0);
	bool SendRebirthVillageInfo(char cWorldSetID, UINT nAccountDBID, int nLastVillageMapIdx);
	void SendPvPGameToPvPLobby( const char cWorldSetID, const UINT uiAccountDBID, const UINT uiPvPIndex, const int iLastVillageMapIndex, bool bIsLadderRoom );
	void SendPvPCommand( const char cWorldSetID, const UINT uiCommand, const UINT uiPvPIndex, const UINT uiRoomIndex );	

	void SendLoginState(char cWorldSetID, UINT nAccountDBID);
	void SendVillageState(char cWorldSetID, UINT nAccountDBID, int nMapIndex, int nRoomID);

	void SendNoticeFromClinet(const WCHAR * pMsg, const int nLen);
	void SendWorldSystemMsg(char cWorldSetID, UINT nFromAccountDBID, char cType, int nID, int nValue, WCHAR* pwszToCharacterName = NULL);
	
	void SendGameFrame(int nServerIdx, int nWholeRoonCnt,  int nRoomCnt, int nUserCnt, int nMinFrame, int nMaxFrame, int nAvrFrame, bool bHasMargin);
#if defined( PRE_WORLDCOMBINE_PARTY )
	void SendWorldUserState(char cWorldSetID, WCHAR* pwszCharacterName , int nMapIdx);
#endif
	void SendWorldUserState(char cWorldSetID, DNVector(std::wstring) & vName, int nMapIdx);
#if defined(PRE_ADD_MULTILANGUAGE)
	void SendBanUser(UINT nAccountDBID, char cWorldSetID, const WCHAR * pCharacterName, char cSelectedLang);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	void SendBanUser(UINT nAccountDBID, char cWorldSetID, const WCHAR * pCharacterName);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	//chat
	bool SendChat(char cWorldSetID, char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen);
#ifdef PRE_ADD_DOORS
	bool SendPrivateChat(char cWorldSetID, UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen, INT64 biDestCharacterDBID = 0);
#else		//#ifdef PRE_ADD_DOORS
	bool SendPrivateChat(char cWorldSetID, UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen);
#endif		//#ifdef PRE_ADD_DOORS
#if defined( PRE_PRIVATECHAT_CHANNEL )
	bool SendPrivateChannelChat(char cWorldSetID, char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen, INT64 nChannelID);
#endif

	//friend
	void SendFriendAddNotify(char cWorldSetID, UINT nAccountDBID, const WCHAR * pCharacterName);

	//PvP
	void SendPvPRoomSyncOK(char cWorldSetID, UINT nPvPIndex);
	void SendPvPLadderRoomSync( char cWorldSetID, INT64 biRoomIndex, INT64 biRoomIndex2, LadderSystem::RoomState::eCode State );
	void SendPvPBreakIntoOK(char cWorldSetID, UINT nPvPIndex, UINT nAccountDBID);
	void SendPvPChangeMemberGrade(char cWorldSetID, UINT nAccountDBID, USHORT nType, UINT nSessionID, bool bAsign);
	void SendPvPSwapMemberIndex(char cWorldSetID, UINT nAccountDBID, BYTE cCount, const TSwapMemberIndex * pIndex);
	void SendPvPRealTimeScore(char cWorldSetID, UINT nGuildDBID, int nScore);
	void SendPvPGuildWarResult(char cWorldSetID, UINT nWinGuildDBID, UINT nLoseGuildDBID, bool bIsDraw);
	void SendPvPDetectCrash(char cWorldSetID, UINT nRoomID, UINT nPvPIndex);

	//Guild
	void SendChangeGuildInfo(char cWorldSetID, const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, BYTE btGuildUpdate, int iInt1 = 0, int iInt2 = 0, int iInt3 = 0, int iInt4 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL);
	void SendChangeGuildMemberInfo(char cWorldSetID, const TGuildUID pGuildUID, UINT nReqAccountDBID, INT64 nReqCharacterDBID, UINT nChgAccountDBID, INT64 nChgCharacterDBID, BYTE btGuildMemberUpdate, int iInt1 = 0, int iInt2 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL, bool bReturn = false);
	void SendGuildChat(char cWorldSetID, const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen);
	void SendGuildChangeName(char cWorldSetID, const TGuildUID pGuildUID, LPCWSTR lpwszGuildName);

	void SendUpdateGuildExp(const TGuildUID pGuildUID, const TAModGuildExp* pPacket);
	void SetPreWinGuildUID( char cWorldSetID, TGuildUID GuildUID );
	void SetGuildWarFinalStartTimeForCheat( char cWorldSetID, __time64_t _tTime );
	void SendAddGuildWarPoint(char cWorldSetID, char cTeamType, TGuildUID GuildUID, int nAddPoint);
	TGuildUID GetPreWinGuildUID( char cWorldSetID );
	__time64_t GetGuildWarFinalStatTime( char cWorldSetID );

	//Party
	bool SendInvitePartyMember(CDNGameRoom * pGameRoom, char cWorldSetID, int nMapIdx, int nChannelID, UINT nAccountDBID, const WCHAR * pwszInviterName, const WCHAR * pwszInvitedName, int * pPassClassIds, int nPermitLevel);
	void SendInvitedMemberReturnMsg(char cWorldSetID, UINT nAccountDBID, int nRetCode);
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	void SendGetPartyIDResult(char cWorldSetID, UINT nSenderAccountDBID , TPARTYID PartyID );
	void SendResPartyAskJoin(char cWorldSetID, int iRet, UINT uiReqAccountDBID, const WCHAR* pwszReqChracterName = NULL );
	void SendAskJoinAgreeInfo(char cWorldSetID, const WCHAR* pwszAskerCharName, TPARTYID PartyID, int iPassword );
#endif

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	void SendSyncSystemMail( TASendSystemMail* pA );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	void SendMasterSystemSyncSimpleInfo( BYTE cWorldSetID, INT64 biCharacterDBID, MasterSystem::EventType::eCode Type );
	void SendMasterSystemSyncGraduate( BYTE cWorldSetID, INT64 biCharacterDBID, WCHAR* pwszCharName );
	void SendMasterSystemSyncConnect( BYTE cWorldSetID, bool bIsConnect, WCHAR* pwszCharName, const TMasterSystemData* pMasterSystemData );

	//Farm
	bool SendFarmDataLoaded(int nWorldID, int nGameServerIdx, UINT nRoomID, UINT nFarmDBID, int nMapID, int nFarmMaxUser);
	bool SendFarmUserCount(int nWorldID, UINT nFarmDBID, UINT nFarmRoomID, int nCurUserCount, bool bStarted);
	bool SendFarmIntendedDestroy(int nWorldID, UINT nFarmDBID);
	void SendFarmSync( int nWorldID, INT64 biCharacterDBID, Farm::ServerSyncType::eType Type );
	void SendFarmSyncAddWater( int nWorldID, INT64 biCharacterDBID, WCHAR* pwszCharName, int iAddWaterPoint );

	void SendGuildMemberLevelUp(int nWorldID, TGuildUID GuildUID, INT64 nCharacterDBID, char cLevel);

	void SendLoadUserTempData(int nWorldID, UINT uiAccountDBID);
	void SendSaveUserTempData(int nWorldID, CDNUserSession* pSession);

#if defined(_KR)
	void SendPCBangResult(char cWorldSetID, UINT nAccountDBID);
#endif	// #if defined(_KR)
	//bool GetGuildWarScheduleInfo(char cWorldSetID, char &cStep, short &nScheduleID);
	char GetGuildWarStepIndex(int nWorldID);	
	short GetGuildWarScheduleID(int nWorldID);
	int GetBlueTeamPoint(int nWorldID);
	int GetRedTeamPoint(int nWorldID);
	bool IsSecretMission(int nWorldID, BYTE cTeamCode, int nMissionID);

	void GetGuildWarSeqInfo(char cWorldSetID, short& nScheduleID, char &cStepIndex, char &cFinalPart);
	char GetGuildWarFinalPart(int nWorldID);
	void SendDuplicateLogin(char cWorldSetID, UINT nAccountDBID, bool bIsDetach, UINT nSessionID);

	void SendCheckLastDungeonInfo( char cWorldSetID, UINT uiAccountDBID, INT64 biCharacterDBID, bool bIsCheck, const WCHAR* pwszPartyName=NULL );
#if defined( PRE_PARTY_DB )
	void SendConfirmLastDungeonInfo( char cWorldSetID, int iRet, UINT uiAccountDBID, BreakInto::Type::eCode BreakIntoType );
#else
	void SendConfirmLastDungeonInfo( char cWorldSetID, int iRet, UINT uiAccountDBID );
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_WORLDCOMBINE_PARTY )
	void SendDelWorldParty( char cWorldSetID, int nPrimaryIndex );	
	void SendWorldPartyMember( char cWorldSetID, UINT nAccountDBID, INT64 biCharacterDBID, TPARTYID PartyID, short nCount, Party::MemberData *MemberData, int nRet );
#endif
#if defined (PRE_ADD_BESTFRIEND)
	void SendLevelUpBestFriend(char cWorldSetID, BYTE cLevel, LPCWSTR lpwszCharacterName);
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
	void SendAddPrivateChatChannel(char cWorldSetID,TPrivateChatChannelInfo tPrivateChatChannel);
	void SendAddPrivateChatChannelMember(char cWorldSetID,INT64 nChannelID, TPrivateChatChannelMember Member);
	void SendInvitePrivateChatChannel(char cWorldSetID, INT64 nChannelID, UINT nAccountDBID, WCHAR* wszInviteCharacterName);
	void SendInvitePrivateChatChannelResult(char cWorldSetID, int nRet, UINT nAccountDBID);
	void SendDelPrivateChatChannelMember(char cWorldSetID,PrivateChatChannel::Common::eModType eType, INT64 nChannelID, INT64 biCharacterDBID, const WCHAR* wszKickName);
	void SendKickPrivateChatChannelMemberResult(char cWorldSetID, INT64 biCharacterDBID);
	void SendModPrivateChatChannelInfo(char cWorldSetID,INT64 nChannelID, PrivateChatChannel::Common::eModType eModType, int nPassWord, INT64 biCharacterDBID);
#endif

#if defined( PRE_WORLDCOMBINE_PVP )
	void SendDelWorldPvPRoom( char cWorldSetID, int nRoomIndex );
	void SendWorldPvPRoomJoinResult( char cWorldSetID, int nRet, UINT uiAccountDBID );
	void SendWorldPvPRoomGMCreateResult( char cWorldSetID, int nRet, UINT uiAccountDBID, TWorldPvPRoomDBData* Data, UINT uiPvPIndex );
#endif
#if defined( PRE_ADD_MUTE_USERCHATTING)
#if defined(PRE_ADD_MULTILANGUAGE)
	void SendMuteUserFind(UINT uiAccountDBID, char cWorldSetID, const WCHAR * pMuteName, int nMuteTime, char cSelectLanguage );
#else	// #if defined(PRE_ADD_MULTILANGUAGE)
	void SendMuteUserFind(UINT uiAccountDBID, char cWorldSetID, const WCHAR * pMuteName, int nMuteTime );
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING)
#if defined( PRE_MOD_71820 )
	void SendNotifyMail(char cWorldSetID, UINT nToAccountDBID, INT64 biToCharacterDBID, short wTotalMailCount, short wNotReadMailCount, short w7DaysLeftCount, bool bNewMail);
#endif // #if defined( PRE_MOD_71820 )

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	void SendAddAlteiaWorldSendTicketResult( char cWorldSetID, int nRetCode, INT64 biSendCharacterDBID );
#endif
#if defined(PRE_ADD_CHNC2C)
	void SendC2CAddCoinResult( char cWorldSetID, int nRetCode, const char* szSeqID);
	void SendC2CReduceCoinResult( char cWorldSetID, int nRetCode, const char* szSeqID);
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined(PRE_ADD_DWC)
	bool SendDWCTeamChat(char cWorldSetID, UINT nAccountDBID, INT64 biCharacterDBID, WCHAR *pwszChatMsg, short wChatLen);
	bool SendUpdateDWCScore(char cWorldSetID, UINT nTeamID, TDWCScore &DWCScore);
#endif

private:
	CDNMasterConnection* GetMasterConnectionByWorldID(char cWorldSetID);
};

extern CDNMasterConnectionManager* g_pMasterConnectionManager;