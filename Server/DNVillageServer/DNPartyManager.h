#pragma once

#include "DNParty.h"

typedef map<TPARTYID, CDNParty*> TMapParty;

#if defined( PRE_PARTY_DB )
#else
typedef vector<CDNParty*> TVecParty;
typedef std::multimap <UINT, CDNParty*> TMultiMapParty;
#endif // #if defined( PRE_PARTY_DB )

class CDNUserSession;
class CDNUserSendManager;
class CDNDBConnection;

class CDNPartyManager
{
private:
	int m_nProcessCount;
	ULONG m_nLastCheckTick;

	TMapParty m_pMapPartyList;
#if defined( PRE_PARTY_DB )
	CDNDBConnection* m_pDBCon;

	bool m_bDeletePartyForServer;
	typedef std::map<int,DWORD> TDELETEPARTYFORSERVERLIST;
	TDELETEPARTYFORSERVERLIST m_DeletePartyForServerList;

	typedef std::map<TPARTYID,DWORD> TDELETEPARTYLIST;
	TDELETEPARTYLIST m_DeletePartyList;

	typedef std::map<std::tr1::tuple<TPARTYID,UINT,INT64,UINT,Party::QueryOutPartyType::eCode>,DWORD> TOUTPARTYLIST;
	TOUTPARTYLIST m_OutPartyList;

	typedef std::map<UINT,TPARTYID> TADDPARTYMEMBERLIST;
	TADDPARTYMEMBERLIST m_AddPartyMemberList;

	DWORD m_dwGetListPartyTick;

#else
	std::vector <CDNParty*> m_pVecPartyList;
	TMultiMapParty m_pMultiMapWaitParty;
	CSyncLock m_Sync;	
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_WORLDCOMBINE_PARTY )
	ULONG m_nCreateWorldPartyTick;
	typedef std::map<int,WorldCombineParty::WrldCombinePartyData> TWORLDCOBINEPARTY;		
	TWORLDCOBINEPARTY m_WorldReqCombineParty;			// 게임서버에 리퀘스트 한 목록	
#endif

public:
	CDNPartyManager(void);
	virtual ~CDNPartyManager(void);

#if defined( PRE_PARTY_DB )
	CDNDBConnection* GetDBConnection();
	void InternalDoUpdate( DWORD dwCurTick );
#endif // #if defined( PRE_PARTY_DB )
	void DoUpdate(DWORD CurTick);

#if defined( PRE_PARTY_DB )
	CDNParty* CreateParty( CDNUserSession* pUser, TPARTYID PartyID, const DBCreateParty* pData );
	void PushParty( const Party::Data& Data );
	CDNParty * PushParty(TAAddPartyAndMemberVillage * pPacket);
	void ModParty( const TAModParty* pPacket );
#else
	CDNParty * CreateParty(CDNUserSession * pUser, TPARTYID PartyID, const WCHAR * pPartyName, const WCHAR * pPassWord, int nMemberMax, TPARTYITEMLOOTRULE ItemLootRule, TITEMRANK ItemRank, int nLvLimitMin, int nLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cIsPartyJobDice);		
#endif // #if defined( PRE_PARTY_DB )		
	CDNParty * PushParty(MAVIPushParty * pPacket);

	bool DestroyParty(TPARTYID PartyID, bool bLog=true );
	CDNParty * GetParty(TPARTYID PartyID);	

#if defined( PRE_PARTY_DB )
#else
	void PushWaitPartyList(CDNParty *pParty);			// 대기중인 리스트에 파티 넣기
	bool EraseWaitPartyList(CDNParty *pParty);			// 대기중인 리스트에 파티 빼기
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
	void GetPartyListInfo(int nMapIdx, int nChannelID, int nChannelMeritID, int nOffSetCount, const CSPartyListInfo* pPacket, std::list<CDNParty*>& PartyList );
#else
	int GetPartyListInfo(int nMapIdx, int nChannelID, int nChannelMeritID, int nOffSetCount, int nPageNum, int * nSortMapIdx, TDUNGEONDIFFICULTY Difficulty, int &nRet, std::list<SPartyListInfo> * pPartyList);
#endif // #if defined( PRE_PARTY_DB )
	void AdjustParty(std::vector <std::pair<int, TPARTYID>> &vList);

	//Party Send
	void SendPartyMemberPart(TPARTYID PartyID, CDNUserSession *pSession);
	void SendPartyUseRadio(TPARTYID nPartyID, UINT nSessionID, USHORT nRadioID);
	void SendPartyReadyRequest(TPARTYID nPartyID, UINT nAccountDBID);

	// Func
#if defined( PRE_PARTY_DB )
	int DelPartyMember( CDNParty* pParty, CDNUserSession* pSession, const TAOutParty* pPacket );
#else
	int DelPartyMember( CDNParty* pParty, CDNUserSession* pSession );
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
	TPARTYID GetPartyID( UINT nAccountDBID );
	int DelPartyMember( CDNParty* pParty, const TAOutParty* pA );

	void QueryDelPartyForServer( int iServerID, DWORD dwTick=0 );
	void QueryDelPartyForServerResponse( const TADelPartyForServer* pPacket );

	void QueryGetListParty( DWORD dwTick=0 );
	void QueryGetListPartyResponse( const TAGetListParty* pPacket );

	bool QueryDelParty( TPARTYID PartyID, DWORD dwTick=0, CDNDBConnection* pLoopDBCon=NULL );
	void QueryDelPartyResponse( const TADelParty* pPacket );
	
	bool QueryOutParty( TPARTYID PartyID, UINT uiAccountDBID, INT64 biCharacterDBID, UINT nSessionID, Party::QueryOutPartyType::eCode Type, DWORD dwTick=0, CDNDBConnection* pLoopDBCon=NULL );
	void QueryOutPartyResponse( const TAOutParty* pPacket );

#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_WORLDCOMBINE_PARTY )	
	bool QueryGetListWorldParty();
	void QueryGetListWorldPartyResponse( const TAGetListParty* pPacket );
	void ReqAddWorldParty( WorldCombineParty::WrldCombinePartyData *Data ); 	
	void DelCombinePartyData( int nPrimaryIndex );
	void ClearReqWorldParty();
#endif
};

extern CDNPartyManager* g_pPartyManager;
