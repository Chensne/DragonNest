
#pragma once

#include "CriticalSection.h"
#include "RUDPCommonDef.h"
#include "RUDPSocketFrame.h"
#include "CryptPacket.h"
#include "Util.h"
#include "buffer.h"
#include "FrameSync.h"
#include "DNGameRoom.h"

class CDNUserSession;
class CDNGameRoom;
class CDNGameServerManager;
class CSpinBuffer;
class CDNRUDPGameServer : public CRUDPSocketFrame, public CCryptoPacket
{
public :

	class CScopeRoomListInterLocked
	{
	public:
		CScopeRoomListInterLocked( std::map <unsigned int, CDNGameRoom*>& List ):m_List(List)
		{
			for( std::map <unsigned int, CDNGameRoom*>::iterator itor=m_List.begin() ; itor!=m_List.end() ; ++itor )
			{
				CDNGameRoom* pRoom = (*itor).second;
				InterlockedIncrement( &pRoom->m_lRoomProcessInterLocked );
			}
		}
		~CScopeRoomListInterLocked()
		{
			for( std::map <unsigned int, CDNGameRoom*>::iterator itor=m_List.begin() ; itor!=m_List.end() ; ++itor )
			{
				CDNGameRoom* pRoom = (*itor).second;
				InterlockedDecrement( &pRoom->m_lRoomProcessInterLocked );
			}
		}
	private:
		std::map <unsigned int, CDNGameRoom*>& m_List;
	};

	CDNRUDPGameServer(CDNGameServerManager * pManager, const char * pExternalProbeIP, unsigned short nExternalProbePort);
	~CDNRUDPGameServer();

	unsigned int GetServerID() { return m_iGameServerID; }
	bool Start(int iPort, int iServerID, int nAffinity);
	void GetAddress(unsigned long * pAddr, unsigned short * pPort);	

#if defined( PRE_WORLDCOMBINE_PARTY )
	void StoreExternalBuffer(UINT nAccountDBID, int iMainCmd, int iSubCmd, const char * pPacket, int iSize, BYTE cType, USHORT nMasterID = 0, int iWorldSetID=0 );
#else
	void StoreExternalBuffer(UINT nAccountDBID, int iMainCmd, int iSubCmd, const char * pPacket, int iSize, BYTE cType, USHORT nMasterID = 0);
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	int Send(int iNetID, int iMainCmd, int iSubCmd, const void * pPacket, int iSize, int iPrior, BYTE cSeq);
	
	bool AddConnect(int iNetID, CDNUserSession * pSession);
	void RemoveConnection(int nGameRoomID, int nNetID, UINT nAccountDBID, UINT nSessionID);

	void SendRestoreMaster(char cWorldSetID);

	//db에 저장되는 데이타
#ifdef PRE_ADD_CHAGNESERVER_LOGGING
	bool AddSaveSendData(int nMainCmd, char *pData, int iLen);
#else		//#ifdef PRE_ADD_CHAGNESERVER_LOGGING
	void AddSaveSendData(int nMainCmd, char *pData, int iLen);
#endif		//#ifdef PRE_ADD_CHAGNESERVER_LOGGING

	//외부에서 사용되므로 가급적 주의 요망!
	CDNGameRoom * GetRoom(unsigned int iRoomID);
	CDNUserSession * GetSession(unsigned int iUID);
	CDNGameServerManager * GetServerManager() {return m_pServerManager; }
	bool GetFlushSaveData() { return m_bIsAllFlushSaveData; }

	//FrameSync
	int GetFPS() { return m_nFPS; }
	void ResetTimer();
	bool IsAcceptRoom();

	CDNUserSession* GetUserSession( UINT uiAccountDBID )
	{
		std::map <unsigned int, CDNUserSession*>::iterator itor = m_ConnectList.find( uiAccountDBID );
		if( itor == m_ConnectList.end() )
			return NULL;

		return (*itor).second;
	}

	bool bIsFrameAlert(){ return m_bFrameAlert; }

	bool m_bZeroPopulation;
	UINT	m_uiConnectReqCount;
	UINT	m_uiConnectSuccessCount;

protected:
	bool Accept(int iNetID, SOCKADDR_IN *pAddr, const void * pData, int iLen);
	void DisConnected(int iNetID, bool bForce, bool bUnreachable);

	void TimeEvent();

private:

	bool	m_bFrameAlert;
	UINT	m_uiFrameAlertCount;

	//values
	volatile bool m_bIsCloseService;
	volatile bool m_bIsAllFlushSaveData;
	unsigned int m_iGameServerID;							//요게 인덱스 이자 서버의 아뒤!
		
	unsigned long m_iUDPAddr;									//서비스중인 서버 주소
	unsigned short m_iUDPPort;								//서비스중인 서버 포트

	char m_szExternalProbeIP[IPLENMAX];
	unsigned short m_nExternalProbePort;

	CDNGameServerManager * m_pServerManager;		//메니져 포인터
	//마스터가 죽었다가 다시 살았을 경우 처리 플로우
	enum eRestoreStep
	{
		_RESTORESTEP_SENDUSER = 0,
		//_RESTORESTEP_SENDPARTY,
		_RESTORESTEP_END,
	};
	struct tRestoreMaster
	{
		char cWorldSetID;
		int nStep;
	};
	std::vector <tRestoreMaster> m_RestoreMaster;
	CSyncLock m_RestoreSync;
	
	//ConnectList
	std::map <unsigned int, CDNUserSession*> m_ConnectList;		//key = uid 생성된 세션 객체리스트	(AccountDBID = dbid)
	std::map <unsigned int, CDNUserSession*> m_VerifyList;			//key = uid 따로 생성된 아이디 (대략 오브젝트 겠지.....젝일...구질하다)
	std::map <int, CDNUserSession*> m_ClientList;							//key = netid 실재 컨넥트된 객체리스트

private:
	//RoomList
	std::map <unsigned int, CDNGameRoom*> m_GameRoomList;	//서버가 담당하는 게임룸 리슷흐

	//Process!
	void IdleProcess();				//아이들링!
	void StoredProcess();
	bool CashParsePacket(int iMainCmd, int iSubCmd, const void * pPacket, int iLen);
	bool DataBaseParsePacket(int iMainCmd, int iSubCmd, const void * pPacket, int iLen);
	bool GameRoomDataBaseParsePacket(int iMainCmd, int iSubCmd, const void * pPacket, int iLen);
	void DataBaseUpdate(unsigned long iCurTick);

	//recieve packet & message Parser
	void Parse(int iMainCmd, int iSubCmd, const void * pData, int iLen, int iNetID, BYTE cSeqLevel);
	void Recv(int iNetID, void * pData, int iLen);

	CSpinBuffer*	m_pStoredPacketBuffer;

	//DB data
	DWORD m_dwChangeSaveTick;
	DWORD m_dwLastSaveTick;

	CBuffer* m_pChangeSaveBuffer;
	CBuffer* m_pLastSaveBuffer;

	//function
	CDNUserSession * GetSessionByADBID(unsigned int nAccountDBID);
	CDNGameRoom * CreateGameRoom(MAGAReqRoomID * pPacket);
	CDNGameRoom * CreateGameRoom(MAGAReqTutorialRoomID * pPacket);
	
	bool VerifyUserIDs(UINT nAccountDBID, UINT nSessionID);
	void RequestedCreateGameRoomFromMaster(char cWorldSetID, char * pData, bool bTutorial = false);
	void RequestedBreakintoGameRoomFromMaster( char cWorldSetID, MAGABreakIntoRoom* pPacket );
	void RequestedVillageState( MAGAVillageState *pPacket );
	void SendRestoreMaster();		//마스터 컨넥시에 보내 주는 것
	void DestroyAllGameRoom();
#if defined( PRE_WORLDCOMBINE_PARTY )
	void DestroyAllFarmGameRoom( int iWorldSetID );	
#else
	void DestroyAllFarmGameRoom();
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	void SendSetGameRoomID(char cWorldSetID, CDNGameRoom * pRoom, MAGAReqRoomID * pReqRoomID, UINT * pPartyMember);

#if defined( PRE_WORLDCOMBINE_PARTY )
	void SendWorldPartyMember(char cWorldSetID, MAGetWorldPartyMember* pPacket);
#endif

	// PvP
	void	InitializePvPGameMode( const MAGAPVP_GAMEMODE* pPacket );

	void InitialzeWorldPvPGameMode(CDNGameRoom * pRoom, MAGAReqRoomID * pReqRoomID);
	
#if defined( PRE_WORLDCOMBINE_PVP )
#endif

	// FrameSync
//	CFrameSync m_FrameSync;
//	CFrameSkip m_FrameSync;
	void ResetFPSHistory();
	void CheckAcceptRoom();

	bool m_bIsAcceptRoom;
	DWORD m_dwPreviewTime;
	DWORD m_dwLastUpdateTime;

	ULONG m_nFrameReportPivot;
#ifdef PRE_ADD_FRAMEDELAY_LOG
	ULONG m_nFrameReportLogPivot;
#endif		//#ifdef PRE_ADD_FRAMEDELAY_LOG

	int m_nCallCount;
	int m_nFPS;

	int m_nMinFPS;
	int m_nMaxFPS;
	int m_nTotalFPS;
	std::vector<int> m_nVecFPSList;

	// Cmd
	bool _CmdRecall( MAGAResRecall* pPacket );
};
