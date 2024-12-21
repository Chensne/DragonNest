#pragma once

#include "DnActor.h"
//#include "ClientTcpSession.h"
#include "MessageListener.h"
#include "DNPacket.h"

class CVillageClientSession : public CTaskListener
{
public:
	CVillageClientSession();
	virtual ~CVillageClientSession();

	struct PartyListStruct {
		bool bRequest;
		bool bEmpty;
		bool bSecret;
		bool bMasterFatigueEmpty;
		TPARTYID PartyID;
		//int nActLvLimit;
		int nUserLvLimitMin;
#if defined( PRE_PARTY_DB )
	#ifdef _WORK
		INT64 sortPoint;
	#endif
		int nCurrentMapIdx;
		bool bIsBonus;
#else
		int nUserLvLimitMax;
#endif // #if defined( PRE_PARTY_DB )
		int nCurCount;
		int nMaxCount;
		//int nChannelID;
		tstring szPartyName;
		int nMapIdx;
		TDUNGEONDIFFICULTY Difficulty;
		bool bUseVoice;
		ePartyType PartyType;
#ifdef PRE_WORLDCOMBINE_PARTY
		int nWorldCombinePartyTableIndex;
#endif

#ifdef PRE_ADD_NEWCOMEBACK
		bool bComeback;
#endif // PRE_ADD_NEWCOMEBACK

		PartyListStruct() {
			bRequest = false;
			bEmpty = true;
			bSecret = false;
			bMasterFatigueEmpty = false;
			PartyID = -1;
			//nActLvLimit = 0;
			nUserLvLimitMin = 0;
#if defined( PRE_PARTY_DB )
	#ifdef _WORK
			sortPoint = 0;
	#endif
			nCurrentMapIdx = 0;
			bIsBonus = false;
#else
			nUserLvLimitMax = 0;
#endif // #if defined( PRE_PARTY_DB )
			nCurCount = 0;
			nMaxCount = 0;
			//nChannelID = 0;
			nMaxCount = 0;
			Difficulty = CONVERT_TO_DUNGEONDIFFICULTY(0);
			bUseVoice = false;
			PartyType = _NORMAL_PARTY;
			nMapIdx = 0;
#ifdef PRE_WORLDCOMBINE_PARTY
			nWorldCombinePartyTableIndex = Party::Constants::INVALID_WORLDCOMBINE_TABLE_INDEX;
#endif
#ifdef PRE_ADD_NEWCOMEBACK
			bComeback = false;
#endif // PRE_ADD_NEWCOMEBACK
		}
	};

	struct SPartyMemberInfo
	{
		int nUserLevel;
		int nJob;
		int nClassID;
		bool bMaster;
		wchar_t wszName[64];
#ifdef PRE_PARTY_DB
		INT64 characterDBID; // note : 현재는 사용되지 않으나 추후 구별을 통한 기능 추가를 위해 추가합니다. (12/03/26)
#endif
#ifdef PRE_ADD_NEWCOMEBACK
		bool bComeback;
#endif // PRE_ADD_NEWCOMEBACK

		SPartyMemberInfo()
			: nUserLevel(0)
			, nJob(0)
			, nClassID(0)
			, bMaster(false)
#ifdef PRE_PARTY_DB
			, characterDBID(0)
#endif
#ifdef PRE_ADD_NEWCOMEBACK
			, bComeback(false)
#endif // PRE_ADD_NEWCOMEBACK
		{
			SecureZeroMemory( wszName, 64 );
		}
	};
	enum VillageTypeEnum {
		WorldVillage,
		PvPVillage,
		PvPLobbyVillage,
		FarmVillage,
	};


	typedef std::vector<PartyListStruct>			PARTYLISTINFO_MAP;
	typedef PARTYLISTINFO_MAP::const_iterator		PARTYLISTINFO_MAP_CONST_ITER;
	typedef PARTYLISTINFO_MAP::iterator		PARTYLISTINFO_MAP_ITER;

protected:
	DnActorHandle m_hLocalPlayerActor;

	int m_nVillageMapIndex;
	int m_nVillageMapArrayIndex;
	int m_nVillageMapEnviIndex;
	int m_nVillageMapEnviArrayIndex;

	int m_nStageRandomSeed;
	int m_nGateIndex;
	int m_nMainClientUniqueID;
	int m_nPartyListCount;
	int m_nPartyListCurrentPage;
#ifdef PRE_PARTY_DB
	int m_nPartyListTotalCount;
	USHORT m_unPartyListStartPageFromServer;
	int m_nPartyListCacheCount;
#endif

	VillageTypeEnum m_VillageType;

	// ID는 실제 채널을 구분짓는 ID, Idx는 채널이동UI에서 사용하는(1,2표시하는) 인덱스
	int m_nChannelID;
	int m_nChannelIdx;

	//접속된 게임서버의 IP & Port
	unsigned long m_iGameServerIP;
	unsigned short m_iGameServerPort;

	// 파티 목록
	bool m_bRefreshPartyList;
	float m_fRefreshPartyDelta;

	TMeritInfo m_MeritInfo;

	std::vector<SPartyMemberInfo> m_vecPartyMemberInfo;	// Note : 현재 선택된 파티리스트의 파티원 정보
	PARTYLISTINFO_MAP m_mapPartyListInfo;

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	bool m_bPVPChannelGradeUpped;
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER

protected:
	void OnRecvCharMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvActorMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvPartyMessage( int nSubCmd, char *pData, int nSize );

	//blondy
	void OnRecvPVPMessage(int nSubCmd, char *pData, int nSize );
	//blondy end

	void OnRecvFarmMessage(int nSubCmd, char * pData, int nSize);
	virtual void OnRecvFarmInfo( SCFarmInfo * pPacket );
	virtual void OnRecvFarmPlantedInfo( SCFarmPlantedInfo * pPacket );
	virtual void OnRecvOpenFarmList();

public:
	// Loading 관련
	static bool __stdcall OnCheckLoadingPacket( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nSize );

public:
#ifdef PRE_PARTY_DB
	void RequestPartyListInfo( int nPage );
	void RequestPartyListInfo( int nPage, const std::wstring& searchWord );
	int GetPartyMaxPage() const;
	bool HasPartyListInfoPage(int nPage) const;
	void ResetPartyList();
	#ifdef _WORK
	void SetPartyRefreshDelta(float fDelta) { m_fRefreshPartyDelta = fDelta; }
	#endif
#else
	void RequestPartyListInfo( int nPage );
#endif
	static bool CompareChannelInfo( const sChannelInfo *s1, const sChannelInfo *s2);
	VillageTypeEnum GetVillageType() { return m_VillageType; }


public:
	virtual void OnConnectTcp();
	virtual void OnDisconnectTcp( bool bValidDisconnect );
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	// Char
	virtual void OnRecvCharMapInfo( SCMapInfo *pPacket );
	virtual void OnRecvCharEntered( SCEnter *pPacket ) {}
	virtual void OnRecvCharUserEntered( SCEnterUser *pPacket, int nSize ) {}
	virtual void OnRecvCharUserLeave( SCLeaveUser *pPacket ) {}
	virtual void OnRecvCharNPCEntered( SCEnterNpc *pPacket ) {}
	virtual void OnRecvCharHide( SCHide *pPacket ) {}

#ifdef PRE_PARTY_DB
	virtual bool OnRecvPartyListInfo( SCPartyListInfo *pPacket );
#else
	virtual void OnRecvPartyListInfo( SCPartyListInfo *pPacket );
#endif
	virtual void OnRecvPartyListInfoErr(SCPartyInfoErr * pPacket);
	virtual void OnRecvPartyMemberInfo( SCPartyMemberInfo *pPacket );
	virtual void OnRecvPartyInfoError( SCPartyInfoErr *pPacket );

	virtual void OnRecvPartyCreateParty( SCCreateParty *pPacket ) {}
	virtual void OnRecvPartyRefresh( SCRefreshParty *pPacket ) {}
	virtual void OnRecvPartyJoinParty( SCJoinParty *pPacket ) {}
	virtual void OnRecvPartyOutParty( SCPartyOut *pPacket ) {}
	virtual void OnRecvPartyRefreshGateInfo( SCRefreshGateInfo *pPacket ) {}
	virtual void OnRecvPartyReadyGate( SCGateInfo *pPacket );
	virtual void OnRecvPartyStageCancel( SCCancelStage *pPacket );
	virtual void OnRecvPartyMoveChannelList(SCChannelList * pPacket);
	virtual void OnRecvPartyMevoChannelFail(SCMoveChannelFail * pData);
	virtual void OnRecvPartyMemberMove(SCPartyMemberMove *pData) {}

	//blondy
	virtual void OnRecvPVPRoomList( SCPVP_ROOMLIST * pData );
	virtual void OnRecvJoinRoom( SCPVP_JOINROOM * pData );
	virtual void OnRecvLeaveRoom( SCPVP_LEAVEROOM * pData );
	virtual void OnRecvCreateRoom( SCPVP_CREATEROOM * pData );
	virtual void OnRecvJoinUser( SCPVP_JOINUSER * pData );
	virtual void OnRecvUserState( SCPVP_USERSTATE * pData );
	virtual void OnRecvChangeTeam( SCPVP_CHANGETEAM * pData );
	virtual void OnRecvStart( SCPVP_START * pData );
	virtual void OnRecvStart( SCPVP_ROOMSTATE * pData );
	virtual void OnRecvStartMsg( SCPVP_STARTMSG * pData ){};
	virtual void OnRecvModifyRoom( SCPVP_MODIFYROOM * pData );
	virtual void OnRecvPvPRoomInfo( char* pData );
	virtual void OnRecvPvPLevel( char* pData );
	//blondyend

	virtual void OnRecvPvPWaitUserList(SCPVP_WAITUSERLIST *pData);

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	virtual void OnRecvPvPChangeChannel(SCPVP_CHANGE_CHANNEL * pData);
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
	virtual void OnRecvPvPEnterLadderChannel(LadderSystem::SC_ENTER_CHANNEL *pData);
	virtual void OnRecvPvPLeaveLadderChannel(LadderSystem::SC_LEAVE_CHANNEL *pData);
	virtual void OnRecvPVPNotifyLeader(LadderSystem::SC_NOTIFY_LEADER *pData);
	virtual void OnRecvPVPLadderState(LadderSystem::SC_NOTIFY_ROOMSTATE *pData);
	virtual void OnRecvPVPMatchingResult(LadderSystem::SC_LADDER_MATCHING *pData);
	virtual void OnRecvPVPLadderGameMode(LadderSystem::SC_NOTIFY_GAMEMODE_TABLEID *pData);
	virtual void OnRecvPVPLadderRoomList(LadderSystem::SC_PLAYING_ROOMLIST *pData);
	virtual void OnRecvPVPLadderObserverMsg(LadderSystem::SC_OBSERVER *pData);
	virtual void OnRecvPVPLadderScoreInfo(LadderSystem::SC_SCOREINFO *pData);
	virtual void OnRecvPVPLadderJobScoreInfo(LadderSystem::SC_SCOREINFO_BYJOB *pData);
	virtual void OnRecvPVPLadderTimeLeft(LadderSystem::SC_MATCHING_AVGSEC *pData);
	virtual void OnRecvPVPLadderPointRefresh(LadderSystem::SC_LADDERPOINT_REFRESH *pData);
	virtual void OnRecvPVPLadderJoinUser(LadderSystem::SC_NOTIFY_JOINUSER *pData);
	virtual void OnRecvPVPLadderLeaveUser(LadderSystem::SC_NOTIFY_LEAVEUSER *pData);
	virtual void OnRecvPVPLadderInviteUser(LadderSystem::SC_INVITE *pData);
	virtual void OnRecvPVPLadderInviteConfirmReq(LadderSystem::SC_INVITE_CONFIRM_REQ *pData);
	virtual void OnRecvPVPLadderInviteConfirm(LadderSystem::SC_INVITE_CONFIRM *pData);
	virtual void OnRecvPVPLadderRefreshUserInfo(LadderSystem::SC_REFRESH_USERINFO *pData);
	virtual void OnRecvPVPLadderKickOut(LadderSystem::SC_KICKOUT *pData);
	virtual void OnRecvPVPGhoulScoreInfo(SCPVP_GHOUL_SCORES *pData);
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	void OnRecvOpenPVPVillageAcess(SCPvPListOpenUI *pData);
#endif
#ifdef PRE_ADD_PVP_TOURNAMENT
	void OnRecvPvPTournamentSwapIndex(SCPvPSwapTournamentIndex* pData);
#endif

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	BOOL Begin( char *pRemoteAddress, USHORT nRemotePort, LPTSTR pID, LPTSTR pPassword );

	const PARTYLISTINFO_MAP& GetPartyList() const { return m_mapPartyListInfo; }
	const PartyListStruct* GetPartyListInfo( TPARTYID PartyIndex ) const;
	std::vector<SPartyMemberInfo>* GetPartyMemberList() { return &m_vecPartyMemberInfo; }

	DnActorHandle GetLocalPlayer() { return m_hLocalPlayerActor; }

	void EnableRefreshPartyList( bool bEnable );

	int GetVillageMapIndex() { return m_nVillageMapIndex; }
	int GetEnteredGateIndex() { return m_nGateIndex; }
	int GetStageRandomSeed() { return m_nStageRandomSeed; }
	int GetPartyListCount() { return m_nPartyListCount; }
#ifdef PRE_PARTY_DB
	int GetPartyListTotalCount() const { return m_nPartyListTotalCount; }
	int GetPartyListCountOffset(int nPage) const;
	USHORT GetPartyListStartPageFromServer() const { return m_unPartyListStartPageFromServer; }
#endif
	void SetPartyListCurrentPage( int nPage ) { m_nPartyListCurrentPage = nPage; }
	int GetChannelID() { return m_nChannelID; }
	int GetChannelIdx() { return m_nChannelIdx; }
	void GetMeritChannelText(std::wstring& result, int userLevel) const;
};
