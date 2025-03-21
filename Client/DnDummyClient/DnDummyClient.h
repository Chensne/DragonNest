#pragma once
#include "NxSyncObject.h"
#include "DnDummyClientSender.h"
#include "DnDummyClientReceiver.h"
#include <winsock2.h>
#include "RUDPCommonDef.h"
#include "ClientSessionManager.h"
#include "DnPacket.h"
#include <mmsystem.h>


enum eClass
{
	CLASS_WARRIER = 1,
	CLASS_ARCHER,
	CLASS_SOCERESS,
	CLASS_CLERIC,
};

struct  StateInfo
{
	enum _info
	{
		_WAITTING = 0,
		_TRY_CONNECT,
		_CONNECTED,
		_DISCONNECTED,
		_ERROR,
		_MAX_STATE
	};

	int			 nState;
	COLORREF	 color; 
	std::wstring szState;
};

const static StateInfo gs_State[] = 
{
	{ StateInfo::_WAITTING,		RGB(255,255,255), _T("대기중") },
	{ StateInfo::_TRY_CONNECT,	0xff5ad2ff, _T("연결중") },
	{ StateInfo::_CONNECTED,		0xff32aaff, _T("연결됨") },
	{ StateInfo::_DISCONNECTED,	0xff2f4f4f,	_T("연결끊김") },
	{ StateInfo::_ERROR,	RGB(255,0,0),	_T("오류") },
};


struct  CurrentServerInfo
{
	enum _info
	{
		_NONE = 0,
		_LOGIN_SERVER,
		_VILLAGE_SERVER,
		_GAME_SERVER,
	};

	int			 nState;
	COLORREF	 color; 
	std::wstring szState;
};
const static StateInfo gs_CurrentServerInfo[] = 
{
	{ CurrentServerInfo::_NONE,		RGB(255,255,255), _T("대기중") },
	{ CurrentServerInfo::_LOGIN_SERVER,	0xff5ad2ff, _T("로그인서버") },
	{ CurrentServerInfo::_VILLAGE_SERVER, 0xff32aaff, _T("빌리지서버") },
	{ CurrentServerInfo::_GAME_SERVER,	0xff2f4f4f,	_T("게임서버") },
	
};


struct  PlayStateInfo
{
	enum _info
	{
		PS_NONE = 0,
		PS_CREATECHAR,
		PS_VILLAGE_LOGIN,
		PS_COMPLETE_VLOGIN,
		PS_IDLE,
		PS_USERPOS,
		PS_CHAT,
		PS_THINKING_PARTY,
		PS_CREATE_REQ_PARTY,	// 파티 생성 신청
		PS_PARTYLIST_REQ,		// 파티 리스트 신청
		PS_JOIN_REQ_PARTY,
		PS_COMPLETE_CREATE_PARTY,
		PS_COMPLETE_PARTY_JOIN,
		PS_START_STAGE,
		

		PS_WAIT_GAME_START,
		PS_GAME_PLAY,
		PS_RETURN_TO_VILLAGE,
		PS_ERROR,
		PS_MAX_STATE
	};

	int			 nState;
	COLORREF	 color; 
	std::wstring szState;
};

const static PlayStateInfo gs_PlayState[] = 
{
	{ PlayStateInfo::PS_NONE,				RGB(255,255,255), _T("") },
	{ PlayStateInfo::PS_CREATECHAR,	RGB(40,60,50), _T("캐릭터생성중") },
	{ PlayStateInfo::PS_VILLAGE_LOGIN,		RGB(25,120,120), _T("빌리지로그인중") },
	{ PlayStateInfo::PS_COMPLETE_VLOGIN,	RGB(25,120,220), _T("빌리지입장됨") },
	{ PlayStateInfo::PS_IDLE,	RGB(25,0,220), _T("아이들") },
	{ PlayStateInfo::PS_USERPOS,	RGB(100,0,190), _T("꼬물꼬물") },
	{ PlayStateInfo::PS_CHAT,	RGB(100,60,50), _T("조잘조잘") },
	{ PlayStateInfo::PS_THINKING_PARTY,		RGB(125,220,120), _T("파티고민중") },
	{ PlayStateInfo::PS_CREATE_REQ_PARTY,	RGB(125,220,240), _T("파티생성 신청중") },
	{ PlayStateInfo::PS_PARTYLIST_REQ,		RGB(125,202,230), _T("파티목록 요청") },
	{ PlayStateInfo::PS_JOIN_REQ_PARTY,			RGB(150,70,200), _T("파티참가중") },
	{ PlayStateInfo::PS_COMPLETE_CREATE_PARTY,	RGB(25,254,170), _T("파티생성완료") },
	{ PlayStateInfo::PS_COMPLETE_PARTY_JOIN,	0xffee82ee,  _T("파티참가완료") },

	{ PlayStateInfo::PS_START_STAGE,		0xffeee8aa, _T("게임시작중") },
	{ PlayStateInfo::PS_WAIT_GAME_START,	0xff00ffff, _T("동기화대기중") },
	{ PlayStateInfo::PS_GAME_PLAY,			0xffff1493, _T("게임중") },
	{ PlayStateInfo::PS_RETURN_TO_VILLAGE,	0xff00ffff, _T("마을로회귀중") },

	{ PlayStateInfo::PS_ERROR,				RGB(255,0,0),	_T("오류") },
};

const int BUFFER_SIZE = 8192;
 struct Event
 {
	 enum
	 {
		 CONNECT = 0,
		 DISCONNECT,
		 PACKET
	 };
	 int nEventType;
	 DWORD nEvtReserve1;
	 DWORD nEvtReserve2;
	 //std::vector<char>	Buffer;
	 char	BufferArray[BUFFER_SIZE];
	 int	nBufferSize;
 };

 struct PartyListStruct {
	 bool bRequest;
	 bool bEmpty;
	 bool bSecret;
	 int nPartyIndex;
	 int nActLvLimit;
	 int nUserLvLimit;
	 int nLeaderLevel;
	 int nCurCount;
	 int nMaxCount;
	 int nRequirementIndex;
	 tstring szPartyName;

	 PartyListStruct() {
		 bRequest = false;
		 bEmpty = true;
		 bSecret = false;
		 nPartyIndex = -1;
		 nActLvLimit = 0;
		 nUserLvLimit = 0;
		 nLeaderLevel = 0;
		 nCurCount = 0;
		 nMaxCount = 0;
		 nRequirementIndex = 0;
	 }
 };


class CNetSocketRUDP;
class CNetSocketUDP;
class DnDummySession;

typedef std::map<int,PartyListStruct>	PARTYLISTINFO_MAP;
typedef PARTYLISTINFO_MAP::iterator		PARTYLISTINFO_MAP_ITER;


#define PROPERTY(dataType, getter, setter) __declspec(property(get=getter, put=setter)) dataType 

class DnDummyClient : public CSeqReceiver
{
	friend class DnDummyClientTCPReceiver;
	friend class DnDummyClientTCPSender;
	friend class DnDummyClientUDPReceiver;
	friend class DnDummyClientUDPSender;

public:

	struct Property 
	{
		int			 nConnectionState;
		std::wstring szUserID;
		std::wstring szPasswd;
		std::wstring szCharacter;
		std::wstring szVillageIP;
		int			 nPlayState;
		int			 nCurPosition;
		time_t		 ConnectTime;
		DWORD		 nResponseTime;
		DWORD		 nResponseAvrTime;
		char		 cClass;
		int			 nDamageCnt;
		int			 nHitCnt;

		EtVector3 m_vPos;
		EtVector3 m_vNewPos;
		EtVector2 m_vDir;

		unsigned long iIP;
		unsigned short iPort;
	};
	struct CreateInfo
	{
		std::wstring szUserID;
		std::wstring szPasswd;
		std::wstring szCharacter;
		std::wstring szExtra;
	};

	DnDummyClient();
	virtual ~DnDummyClient();

	bool Create(CreateInfo& info);
	void Destroy();

	bool ConnectToLoginServer(const TCHAR* szIPAddress, unsigned short nPort);
	bool ConnectToVillageServer(const TCHAR* szIPAddress, unsigned short nPort);
	bool ConnectToGameServer(unsigned long iIP, unsigned short iPort, unsigned short nTcpPort);
	
	void Disconnect();
	bool IsConnected();
	
	bool ConnectToRUDP(const TCHAR* szIPAddress, unsigned short nPort);

	void	AddUserLog( const WCHAR* fmt, ... );
	inline std::vector<std::wstring>& GetUserLog() { return m_UserLog; }
	inline Property&	GetProperty() { return m_Property; }
	
	void	Update();

	void	_UpdateReqPartyList();
	void SendPartyOut();
	void	_UpdateThinkingParty();
	void	_UpdateCompleteCreateParty();
	void	_UpdateGamePlay();
	void _UpdateVillageUserPos();
	void _UpdateChat();
	void SwapItem();
	void _UpdateIdle();

	void	AddEvent(Event& evt);

	void	SetNextCmdTime(int nBaseTime = 10 , int nRandomTime = 200);

	bool	IsPartyLeader() { return m_bPartyLeader; }
	EtVector3	GetNewPos();

	int GetPossibleJoinPartyIndex();

	void Query(int nMainCmd, int nSubCmd, ULONG nTick);
	void Answer(UINT nUID, int nMainCmd, int nSubCmd, ULONG nTick);

	DWORD GetAverageResponseTime() { return m_nAverageResponseTime; }

	int  GetRemainPlayTime( );

	void SetRemainPlayTime(int nTime )
	{
		m_nRemainPlayTime = nTime;
	}
public:
	// 클라이언트가 접속 했을때 호출된다. 
	void	OnAccept(  );
	// 서버에 접속 되었을때 호출된다. ( 클라이언트 용 세션을 접속시켰을때 )
	void	OnConnect(  );
	// 접속 끊김
	void	OnClose();
	// 패킷 수신시
	void	OnPacket( Event& evt );
	
	virtual void RUDPRecieve(int header, int sub_header, char * data, int size, _ADDR * addr);

	void	OnCalcResponseTimeRecv();
	void	OnCalcResponseTimeSend();

	bool m_bSwap;

private:

	CreateInfo	m_CreateInfo;
	Property	m_Property;
	DnDummySession* m_pSession;
	CNetSocketRUDP* m_pRUDPSocket;
	//CNetSocketUDP*  m_pUDPSocket;

	std::vector<std::wstring>	m_UserLog;

	std::queue<Event>	m_EventQueue;
	NxSyncObject		m_EventQueueLock;


	DnDummyClientTCPReceiver	m_TCPReceiver;
	DnDummyClientTCPSender		m_TCPSender;

	DnDummyClientUDPReceiver	m_UDPReceiver;
	DnDummyClientUDPSender		m_UDPSender;

	UINT	m_nUniqueID;
	UINT	m_nAccountDBID;
	int		m_nRemainPlayTime;
	int		m_nRemainLastUpdateTime;
	INT64 m_biCertifyingKey;
	

	// 너무 빠른 처리를 막기 위해서...
	int		m_nNextCmdTime;
	DWORD	m_nLastUpdateTime;

	DWORD	m_nLastReqTime;
	DWORD	m_nLastAckTime;
	DWORD	m_nLastResponseTime;
	DWORD	m_nLastResponseTimeCnt;
	DWORD	m_nAverageResponseTime;


	//std::vector<PartyListStruct>	m_PartyList;
	PARTYLISTINFO_MAP m_mapPartyListInfo;

	int m_nPartyListCount;

	int	m_nPartyReqStartIndex;
	int m_nPartyReqCount;	// 파티 정보 몇개 요청했는지
	int m_nPartyIndex;
	bool m_bPartyLeader;
	SPartyMemberInfo m_PartyMember[PARTYMAX];
	int m_nPosUpdateCnt;
	int m_nChatSendCnt;

	int _GetPartyMemberCnt();

	int m_nReqPartyPage;

	//AnswerTime
	struct Req
	{
		int nMainCmd;
		int nSubCmd;
		ULONG nTick;
	};
	std::queue <Req> m_ReqList;
public:
	int  GetReqPartyPage() { return m_nReqPartyPage; }
	void SetReqPartyPage(int nPage) 
	{
		m_nReqPartyPage = nPage;  
		if ( m_nReqPartyPage < 0 )
			m_nReqPartyPage = 0;
	}
	PROPERTY(int, GetReqPartyPage, SetReqPartyPage) m_nReqPartyPage;

};


#define FUNC_USER_LOG()	if ( !m_pDummyClient) return; m_pDummyClient->AddUserLog(_T("%s"), __FUNCTIONW__ )

