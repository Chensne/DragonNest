#include "stdafx.h"
#include "DnDummyClient.h"
#include "DnDummyClientFrame.h"
#include "DnDummySession.h"
#include "DnDummyClientMgr.h"
#include "DnServerApp.h"
#include "NetSocketRUDP.h"
#include "NetSocketUDP.h"
#include "RUDPCommonDef.h"
#include "StringUtil.h"
#include "DnDummyClientMgr.h"
#include <utility>
#include "EngineUtil.h"
#include "DnDummySettingPropertyPanel.h"
#include "Stream.h"
#include "DNProtocol.h"

int gs_Warrier[]	= { 3, 2, 4, 5, 1 };
int gs_Archer[]		= { 1, 2, 3, 4, 23 };
int gs_Sorceress[]	= { 1, 2, 3, 4, 31 };
int gs_Cleric[]		= { 3, 2, 4, 5, 1 };

const static EtVector3	gs_StartPos(342.388f, 0.0f, -634.801f );
const static EtVector3	gs_MinPos(342.388f, 0.0f, -1277.85f);
const static EtVector3	gs_MaxPos(1007.27f, 0.0f, -634.801f);

enum eCmdList
{
	MOVE_FRONT = 0,
	MOVE_BACK,
	MOVE_LEFT,
	MOVE_RIGHT,
	ATTACK,
	CMD_MAX
};
std::map<int, int*>	gs_CmdMap;


DnDummyClient::DnDummyClient()
{
	if ( gs_CmdMap.size() < 1 )
	{
		gs_CmdMap.insert( std::make_pair(CLASS_WARRIER, gs_Warrier));
		gs_CmdMap.insert( std::make_pair(CLASS_ARCHER, gs_Archer));
		gs_CmdMap.insert( std::make_pair(CLASS_SOCERESS, gs_Sorceress));
		gs_CmdMap.insert( std::make_pair(CLASS_CLERIC, gs_Cleric));
	}
	m_pSession = NULL;
	m_pRUDPSocket = NULL;
	//m_pUDPSocket = NULL;
	m_nUniqueID = 0;
	m_nNextCmdTime = 0;
	m_nLastUpdateTime = 0;
	m_nPartyReqStartIndex = 0;
	m_nPartyIndex = -1;
	m_bPartyLeader = false;
	m_nReqPartyPage = 0;
	m_UserLog.reserve(200);
	m_nPosUpdateCnt = 0;
	m_nChatSendCnt = 0;

	m_nRemainPlayTime = 0;
	m_nRemainLastUpdateTime = 0;

	m_bSwap = true;
}

DnDummyClient::~DnDummyClient()
{
	
	Destroy();
}

bool DnDummyClient::Create(CreateInfo& info)
{
	m_CreateInfo = info;

	m_Property.ConnectTime = 0;
	m_Property.nResponseTime = 0;
	m_Property.nResponseAvrTime = 0;
	m_Property.nConnectionState = StateInfo::_WAITTING;
	m_Property.nPlayState = PlayStateInfo::PS_NONE;
	m_Property.nCurPosition = CurrentServerInfo::_NONE;
	m_Property.szCharacter = info.szCharacter;
	m_Property.szUserID = info.szUserID;
	m_Property.szPasswd = info.szPasswd;
	m_Property.nHitCnt = 0;
	m_Property.nDamageCnt = 0;

	m_pRUDPSocket = NULL;
	//m_pUDPSocket = NULL;

	m_TCPSender.SetDummyClient(this);
	m_TCPReceiver.SetDummyClient(this);

	m_UDPSender.SetDummyClient(this);
	m_UDPReceiver.SetDummyClient(this);

	m_nLastReqTime= 0;
	m_nLastAckTime= 0;
	m_nLastResponseTime= 0;
	m_nLastResponseTimeCnt= 0;
	m_nAverageResponseTime= 0;


	return true;

}

void DnDummyClient::Destroy()
{
	if (m_pRUDPSocket)
	{
		delete m_pRUDPSocket;
		m_pRUDPSocket = NULL;
	}
	/*if (m_pUDPSocket)
	{
		delete m_pUDPSocket;
		m_pUDPSocket = NULL;
	}*/

}

void 
DnDummyClient::AddUserLog( const WCHAR* fmt, ... )
{

	while ( m_UserLog.size() > 20 )
	{
		m_UserLog.erase( m_UserLog.begin() );
	}
	
	WCHAR s_wszBuffer[8192] = L"";

	ZeroMemory(s_wszBuffer, sizeof(WCHAR) * 8192);
	va_list args;
	va_start( args, fmt );
	_vsnwprintf_s( s_wszBuffer, 8192-1, fmt, args );
	va_end( args );
	s_wszBuffer[8192-1] = 0;

	m_UserLog.push_back(std::wstring(s_wszBuffer));
}



bool DnDummyClient::ConnectToLoginServer(const TCHAR* szIPAddress, unsigned short nPort)
{
	DnDummySession* pSession = (DnDummySession*)g_ServerApp.OpenSession(szIPAddress, nPort);

	if ( pSession == NULL )
		return false;
	
	m_pSession = pSession;
	m_pSession->SetDummyClient(this);



	m_Property.nConnectionState = StateInfo::_TRY_CONNECT;
	AddUserLog( _T("로그인서버 연결 시도중 : %s:%d"), szIPAddress, (int)nPort);

	m_Property.nCurPosition = CurrentServerInfo::_LOGIN_SERVER;
	if ( m_pSession->IsConnected() )
	{
		
		OnConnect(  );
		return true;
	}

	return false;
}

bool
DnDummyClient::ConnectToVillageServer(const TCHAR* szIPAddress, unsigned short nPort)
{
	DnDummySession* pSession = (DnDummySession*)g_ServerApp.OpenSession(szIPAddress, nPort);

	if ( pSession == NULL )
		return false;

	m_pSession = pSession;
	m_pSession->SetDummyClient(this);

	m_Property.nConnectionState = StateInfo::_TRY_CONNECT;
	AddUserLog( _T("빌리지서버 연결 시도중 : %s:%d"), szIPAddress, (int)nPort);

	m_Property.nCurPosition = CurrentServerInfo::_VILLAGE_SERVER;
	m_Property.szVillageIP = szIPAddress;
	m_Property.iPort = nPort;

		
	OnConnect(  );
	return true;

}

bool
DnDummyClient::ConnectToGameServer(unsigned long iIP, unsigned short iPort, unsigned short nTcpPort)
{
	//--------------------------------------------------------------------------------------------------
	// 게임 서버 TCP 접속 시도
	std::wstring szIP;
	szIP = FormatW(L"%d.%d.%d.%d", ((unsigned char*)&iIP)[0], ((unsigned char*)&iIP)[1], ((unsigned char*)&iIP)[2], ((unsigned char*)&iIP)[3]);

	DnDummySession* pSession = (DnDummySession*)g_ServerApp.OpenSession(szIP.c_str(), nTcpPort);

	if ( pSession == NULL )
		return false;

	m_pSession = pSession;
	m_pSession->SetDummyClient(this);

	m_Property.nConnectionState = StateInfo::_TRY_CONNECT;
	AddUserLog( _T("게임서버 연결 시도중 : %s:%d"), szIP.c_str(), (int)nTcpPort);

	m_Property.nCurPosition = CurrentServerInfo::_GAME_SERVER;

	OnConnect(  );

	//--------------------------------------------------------------------------------------------------
	// 게임서버 RUDP 접속 시도
	SAFE_DELETE(m_pRUDPSocket);
	//SAFE_DELETE(m_pUDPSocket);
	m_pRUDPSocket = new CNetSocketRUDP();
	//m_pUDPSocket = new CNetSocketUDP("UDPThread", m_pRUDPSocket);

	/*if (m_pUDPSocket->Create() == true)
		m_pUDPSocket->SetReciever(this);*/

	if (m_pRUDPSocket)
	{
		m_pRUDPSocket->SetReceiver(this);
		char ip[255];
		ZeroMemory(ip, 255);

		if (m_pRUDPSocket->Connect(iIP, iPort) == false)
			return false;

		//일단 다 합쳐져 있습니다. 오픈과 커넥트연결 컨넥트 리퀘스트 분리됩니다.

		//선행작업 필요 UDP adress detecting해서 자신의 어드레스를 찾아 놓자


		char Name[256] = {0,};
		CHAR szIP[256] = {0, };

		gethostname(Name, sizeof(Name));

		PHOSTENT host = gethostbyname(Name);
		strcpy(szIP, inet_ntoa(*(struct in_addr*)*host->h_addr_list));

		//Agent->RUDPConnect Request
		CSConnectRequest packet;
		packet.nSessionID = m_nUniqueID;
		packet.nAddrIP = _inet_addr(szIP);
		//packet.nPort = m_pUDPSocket->GetPort();

		m_pRUDPSocket->Send(CS_SYSTEM, eSystem::CS_CONNECT_REQUEST, &packet, sizeof(packet), _RELIABLE, 0);
	}
	return true;
}
bool DnDummyClient::ConnectToRUDP(const TCHAR* szIPAddress, unsigned short nPort)
{
	SAFE_DELETE(m_pRUDPSocket);
	m_pRUDPSocket = new CNetSocketRUDP();

	if (m_pRUDPSocket)
	{
		m_pRUDPSocket->SetReceiver(this);
		char ip[255];
		ZeroMemory(ip, 255);

		std::string szIP;
		ToMultiString(std::wstring(szIPAddress), szIP);

		strcpy(ip, szIP.c_str());
		int s_b[4] = { 0, 0, 0, 0 };
		struct in_addr addr;
		sscanf(ip, "%d.%d.%d.%d", &s_b[0], &s_b[1], &s_b[2], &s_b[3]);
		addr.S_un.S_un_b.s_b1 = s_b[0];
		addr.S_un.S_un_b.s_b2 = s_b[1];
		addr.S_un.S_un_b.s_b3 = s_b[2];
		addr.S_un.S_un_b.s_b4 = s_b[3];
		if (m_pRUDPSocket->Connect(addr.S_un.S_addr, nPort) == false)
			return false;

		//일단 다 합쳐져 있습니다. 오픈과 커넥트연결 컨넥트 리퀘스트 분리됩니다.

		//선행작업 필요 UDP adress detecting해서 자신의 어드레스를 찾아 놓자

		
		//Agent->RUDPConnect Request
		CSConnectRequest packet;
		packet.nSessionID = m_nUniqueID;
		packet.nAddrIP = _inet_addr(ip);
		packet.nPort = nPort;

		m_pRUDPSocket->Send(CS_SYSTEM, eSystem::CS_CONNECT_REQUEST, &packet, sizeof(packet), _RELIABLE, 0);
	}
	return true;

}

void DnDummyClient::Disconnect()
{
	if ( !m_pSession )
	{
		m_Property.nConnectionState = StateInfo::_WAITTING;
		AddUserLog( _T("연결 안됨"));
		return ;
	}
	g_ServerApp.CloseSession(m_pSession->GetUID());
	m_pSession = NULL;

	m_Property.nConnectionState = StateInfo::_DISCONNECTED;
	AddUserLog( _T("연결 종료"));
	AddUserLog(_T("%s"), __FUNCTIONW__ );

	SAFE_DELETE(m_pRUDPSocket);
	//SAFE_DELETE(m_pUDPSocket);
}


bool DnDummyClient::IsConnected()
{
	if ( m_pSession == NULL )
		return false;

	if( m_Property.nConnectionState == StateInfo::_ERROR )
	{
		return false;
	}

	return m_pSession->IsConnected();
}
void DnDummyClient::AddEvent(Event& evt)
{
	m_EventQueueLock.Lock();
	m_EventQueue.push(evt);
	m_EventQueueLock.Unlock();
}

void DnDummyClient::SetNextCmdTime(int nBaseTime, int nRandomTime)
{
	m_nNextCmdTime = nBaseTime;
	m_nNextCmdTime += rand() % nRandomTime;
	m_nLastUpdateTime = GetTickCount();
}


void _NxTrace( LPCTSTR szMsg, ... )
{
#if defined(DEBUG) || defined(_DEBUG)
	TCHAR szBuffer[1024] = _T("0");

	va_list args;
	va_start(args, szMsg);
	StringCchVPrintf( szBuffer, 1023, szMsg, args );
	szBuffer[511] = _T('\0');
	va_end(args);

	OutputDebugString( szBuffer );
#endif

}


void 
__NxTrace( LPCTSTR szMsg, ... )
{
#if defined(DEBUG) || defined(_DEBUG)
	TCHAR szBuffer[1024] = _T("0");

	va_list args;
	va_start(args, szMsg);
	StringCchVPrintf( szBuffer, 1023, szMsg, args );
	szBuffer[511] = _T('\0');
	va_end(args);

	OutputDebugString( szBuffer );
#endif

} 
void DnDummyClient::Update()
{
	m_EventQueueLock.Lock();
	if ( m_EventQueue.size()  >=  1  )
	{
		Event evt = m_EventQueue.front();
		m_EventQueue.pop();
		m_EventQueueLock.Unlock();

		switch( evt.nEventType )
		{
		case Event::CONNECT:
			OnConnect();
			break;
		case Event::DISCONNECT:
			OnClose();
			break;
		case Event::PACKET:
			OnPacket(evt);
		}
	}
	else
	{
		m_EventQueueLock.Unlock();
	}

	if ( m_pSession == NULL )
		return;

	if ( m_pSession && m_Property.nConnectionState == StateInfo::_TRY_CONNECT && m_pSession->IsConnected()  )
	{
		OnConnect();
	}

	if ( m_pSession && m_pSession->IsConnected() )
	{
		m_Property.nConnectionState = StateInfo::_CONNECTED;
	}
	if ( m_pSession && m_pSession->IsConnected() == false )
	{
		m_Property.nConnectionState = StateInfo::_DISCONNECTED;
	}
	

	if ( m_Property.nConnectionState == StateInfo::_CONNECTED && m_nNextCmdTime >= 0 )
	{
		DWORD nCurTime = GetTickCount();
		
		
		m_nNextCmdTime -= nCurTime - m_nLastUpdateTime;
		//__NxTrace(_T("m_nNextCmdTime : %d minus:%d\n"), m_nNextCmdTime, nCurTime - m_nLastUpdateTime);
		
		if ( m_nNextCmdTime < 0 )
			m_nNextCmdTime = 0;
		

		if ( m_nNextCmdTime == 0 )
		{
			//_UpdateVillageUserPos();
			
			switch( m_Property.nPlayState )
			{
			case PlayStateInfo::PS_IDLE:
			case PlayStateInfo::PS_COMPLETE_PARTY_JOIN:
				{
					_UpdateIdle();
//					SetNextCmdTime(5000);
					break;
				}

			case PlayStateInfo::PS_USERPOS:
				{
					_UpdateVillageUserPos();
					SetNextCmdTime(2000+rand()%5000);
					break;
				}

			case PlayStateInfo::PS_CHAT:
				{
					_UpdateChat();
					SetNextCmdTime(5000);
					break;
				}

			// 파티 리스트를 요청한 상황일때 
			case PlayStateInfo::PS_PARTYLIST_REQ:
				{
					_UpdateReqPartyList();
				}
				break;

			// 파티를 만들거나 참여해야할 상황일때 
			case PlayStateInfo::PS_THINKING_PARTY:
				{
					_UpdateThinkingParty();
				}
				break;


			// 파티가 만들어졌을때.
			case PlayStateInfo::PS_COMPLETE_CREATE_PARTY:
				{
					_UpdateCompleteCreateParty();
					
					SetNextCmdTime();

				}
				break;

			case PlayStateInfo::PS_GAME_PLAY:
				{
					_UpdateGamePlay();
				}
				break;

			}

			
		}

	}

	

	/*if (m_pRUDPSocket)
		m_pRUDPSocket->DoUpdate();*/


	m_nLastUpdateTime = GetTickCount();
}

// 클라이언트가 접속 했을때 호출된다. 
void	
DnDummyClient::OnAccept(  )
{

}

void
DnDummyClient::OnConnect(  )
{
	if ( m_Property.nConnectionState != StateInfo::_TRY_CONNECT )
		return;

	time_t _time;
	time(&_time);

	m_Property.nConnectionState = StateInfo::_CONNECTED;
	m_Property.ConnectTime = _time;
	
	AddUserLog( _T("연결됨 : %s"), GetTimeString( _time ).c_str());
	AddUserLog(_T("%s"), __FUNCTIONW__ );

	switch(m_Property.nCurPosition)
	{
	case CurrentServerInfo::_LOGIN_SERVER:
		m_TCPSender.SendCheckVersion(1,6);
		break;
	case CurrentServerInfo::_VILLAGE_SERVER:
		m_TCPSender.SendConnectVillage(m_nUniqueID, m_nAccountDBID, m_biCertifyingKey, m_Property.szVillageIP.c_str());
		break;
	}

		
}


void
DnDummyClient::OnClose()
{
	//m_Property.nConnectionState = StateInfo::_DISCONNECTED;
	time_t _time;
	time(&_time);

	AddUserLog( _T("연결 종료 : %s"), GetTimeString( _time ).c_str());
	AddUserLog(_T("%s"), __FUNCTIONW__ );
}

void 
DnDummyClient::OnCalcResponseTimeRecv()
{
	m_nLastAckTime = timeGetTime();
	DWORD nDT = m_nLastAckTime - m_nLastReqTime;
	
	m_Property.nResponseTime = nDT ;

	m_nLastResponseTime += nDT;
	m_nLastResponseTimeCnt++;
	m_Property.nResponseAvrTime  = m_nLastResponseTime / m_nLastResponseTimeCnt;

	if ( m_nLastResponseTimeCnt > 1000000000 )
	{
		m_nLastResponseTime = 1;
		m_nLastResponseTimeCnt = 1;
	}
	
}
void 
DnDummyClient::OnCalcResponseTimeSend()
{
	m_nLastReqTime = timeGetTime();

}
void
DnDummyClient::OnPacket( Event& evt )
{
	OnCalcResponseTimeRecv();


	NxPacket Packet;
	memcpy(Packet.GetPacketBuffer(), &(evt.BufferArray[0]), evt.nBufferSize);
	m_TCPReceiver.OnPacket( Packet );
}

void 
DnDummyClient::RUDPRecieve(int header, int sub_header, char * data, int size, _ADDR * addr)
{
	OnCalcResponseTimeRecv();
	//m_nLastAckTime = timeGetTime();

	m_TCPReceiver._OnPacket(header, sub_header, data, size);
	//m_UDPReceiver.OnPacket(header, sub_header, data, size, addr);
	g_DummyClientMgr.AddRecvBytes(size);
	g_DummyClientMgr.AddRecvCount();
}

void
DnDummyClient::_UpdateReqPartyList()
{

}

void DnDummyClient::SendPartyOut()
{
	m_TCPSender.SendPartyOut();
	m_nPartyIndex = -1;
}

void
DnDummyClient::_UpdateThinkingParty()
{
	AddUserLog(_T("파티 고민중.."));

	if( m_nPartyIndex != -1 )
	{
		if( rand() % 2 )
		{
			SendPartyOut();
		}
		m_Property.nPlayState = PlayStateInfo::PS_IDLE;
		return;
	}

	// 현재 파티가 하나도 없을떄
	if ( m_mapPartyListInfo.size() == 0 )
	{
		AddUserLog(_T("파티가 하나도 없다."));
		int nCnt = g_DummyClientMgr.GetPartingUserCnt();
		int n = nCnt == 0 ? 0 : nCnt;
		if ( n == 0)
		{
			AddUserLog(_T("파티 생성 요청"));
			m_TCPSender.SendCreateParty();
			SetNextCmdTime();
			m_Property.nPlayState = PlayStateInfo::PS_CREATE_REQ_PARTY;
			return;
		}
		else
		{
			AddUserLog(_T("파티 목록 다시 요청후 대기"));
			// 아니면 파티 갯수를 다시 요청하고 기다린다.
			m_nReqPartyPage = m_nReqPartyPage+1;
			m_TCPSender.SendReqPartyListInfo(m_nReqPartyPage);
			SetNextCmdTime();
			m_Property.nPlayState = PlayStateInfo::PS_PARTYLIST_REQ;
			return;
		}
	}

	// 파티가 있다면 
	else
	{
//		int n = rand()%(g_DummyClientMgr.GetPartingUserCnt() * 6 + 1);

		if( rand() % 3 == 0 )
		{
			AddUserLog(_T("파티 생성 요청"));
			m_TCPSender.SendCreateParty();
			SetNextCmdTime();
			m_Property.nPlayState = PlayStateInfo::PS_CREATE_REQ_PARTY;
			return;
		}

		AddUserLog(_T("파티가 %d 개 있다."), (int)m_mapPartyListInfo.size());
		AddUserLog(_T("m_nPartyReqStartIndex %d m_PartyList.size() %d "), m_nPartyReqStartIndex, (int)m_mapPartyListInfo.size());

		m_nPartyReqCount = 0;
		int nCount = 10;
		// 각 파티 정보를 요청한다.
		for ( int i = m_nPartyReqStartIndex ; i < (int)m_mapPartyListInfo.size() ; i++ )
		{
			if ( nCount == 0 )
			{
				AddUserLog(_T("파티상세 정보 10개 요청 완료"));
				m_Property.nPlayState = PlayStateInfo::PS_PARTYLIST_REQ;
				SetNextCmdTime();
				return;
			}

			nCount--;

			m_TCPSender.SendReqPartyListInfo(i);
			m_nPartyReqCount++;	// 파티 정보 요청한 갯수
			AddUserLog(_T("파티 %d 번 상세 파티 정보 요청"), i);

		}

		m_Property.nPlayState = PlayStateInfo::PS_PARTYLIST_REQ;
		SetNextCmdTime();

	}
}


int 
DnDummyClient::_GetPartyMemberCnt()
{
	int nCnt = 0;

	for ( int i = 0 ; i < PARTYMAX ; i++ )
	{
		if ( m_PartyMember[i].nSessionID != 0 )
			nCnt++;
	}

	return nCnt;
}

void
DnDummyClient::_UpdateCompleteCreateParty()
{

	// 파티장일때..
	if ( m_bPartyLeader )
	{
		if ( _GetPartyMemberCnt() >= g_SettingPropertyPanel->GetSettingPropertyData().nGameStartUserCnt  )
		{
//#ifdef _GAME_TEST
			if ( g_DummyClientMgr.GetMode() == DnDummyClientMgr::DefaultMode )
			{
				//m_UDPSender.SendStartStage(1);
				m_Property.nPlayState = PlayStateInfo::PS_START_STAGE;
				return;
			}
//#endif
		}

	}
	
	// 파티원일때 
	else
	{
	}

	if ( g_DummyClientMgr.GetMode() == DnDummyClientMgr::VillageMode )
		m_Property.nPlayState = PlayStateInfo::PS_IDLE;

}


void 
DnDummyClient::_UpdateGamePlay()
{
	if ( g_DummyClientMgr.GetMode() == DnDummyClientMgr::MoveMode )
	{
		DWORD nCurTime = GetTickCount();

		if ( m_nRemainLastUpdateTime > 0 )
			m_nRemainPlayTime -= nCurTime - m_nRemainLastUpdateTime;

		m_nRemainLastUpdateTime = nCurTime;
		//__NxTrace(_T("m_nNextCmdTime : %d minus:%d\n"), m_nNextCmdTime, nCurTime - m_nLastUpdateTime);

		if ( m_nRemainPlayTime < 0 )
			m_nRemainPlayTime = 0;

		/*if ( m_nRemainPlayTime == 0 && GetProperty().nPlayState != PlayStateInfo::PS_RETURN_TO_VILLAGE )
		{
			
			m_TCPSender.SendStartStage(g_SettingPropertyPanel->GetSettingPropertyData().nMapIndex);
			GetProperty().nPlayState = PlayStateInfo::PS_START_STAGE;
			return ;
			
		}*/
	}
		
	switch( m_Property.cClass )
	{
	case CLASS_WARRIER:

		break;
	case CLASS_ARCHER:
		break;
	case CLASS_SOCERESS:
		break;
	case CLASS_CLERIC:
		break;
	}

	int* pCmd = gs_CmdMap[(int)m_Property.cClass];

	int nLoop = rand()%3;
	int nCmdIdx = rand()%(CMD_MAX*2);
	
	int nCmd = 0;
	if ( 0 )
	{
		

	}

	static bool bStand = false;
	if ( bStand )
	{
		nCmd = pCmd[ATTACK];
		m_UDPSender.SendCmdStop();
		m_UDPSender.SendCmdAction(nCmd, nLoop, 8.0f);
		
	}
	
	static bool bActive = true;
	if ( bActive )
	{
		if ( nCmdIdx >= ATTACK )
		{
			nCmd = pCmd[ATTACK];
			m_UDPSender.SendCmdStop();
			m_UDPSender.SendCmdAction(nCmd, nLoop, 8.0f);
		}
		else
		{
			int nCmdIdx = rand()%(MOVE_RIGHT);
			//int nCmdIdx = MOVE_FRONT;

			nCmd = pCmd[nCmdIdx];
			char m_cMovePushKeyFlag = 0;
			if( nCmdIdx == MOVE_FRONT ) m_cMovePushKeyFlag |= 0x04;
			if( nCmdIdx == MOVE_BACK ) m_cMovePushKeyFlag |= 0x08;
			if( nCmdIdx == MOVE_LEFT ) m_cMovePushKeyFlag |= 0x01;
			if( nCmdIdx == MOVE_RIGHT ) m_cMovePushKeyFlag |= 0x02;

			EtVector3 vNewPos = GetNewPos();
			EtVector3 vDir = m_Property.m_vPos - vNewPos;
			EtVec3Normalize(&vDir,&vDir);
			m_Property.m_vDir = EtVec3toVec2(vDir);


			m_UDPSender.SendCmdMove(nCmd, m_cMovePushKeyFlag,  nLoop, 8.0f);
			m_Property.m_vPos = vNewPos;
		}

	}
	
	
	m_nLastUpdateTime = GetTickCount();
	//__NxTrace(_T("[%d]액션~~~~ %d  \n"), GetTickCount(), this->m_nUniqueID);
	SetNextCmdTime(100, 500);

	
}

void DnDummyClient::_UpdateVillageUserPos()
{
	if ( GetProperty().nCurPosition != CurrentServerInfo::_VILLAGE_SERVER )
		return ;

	m_nPosUpdateCnt++;
	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	DWORD dwGap = GetTickCount();
	int nActionIndex = 3;
	char cFlag = 1;
	EtVector2 vZVec( -5000 + rand()%10000, -5000 + rand()%10000 );
	EtVector2 vLook( -5000 + rand()%10000, -5000 + rand()%10000 );
	EtVec2Normalize( &vZVec, &vZVec );
	EtVec2Normalize( &vLook, &vLook );
	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	m_Property.m_vNewPos.x = -1000 + rand()%2000;
	m_Property.m_vNewPos.z = -1000 + rand()%2000;
	Stream.Write( &m_Property.m_vNewPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &cFlag, sizeof(char) );
	m_TCPSender.SendActorMsg(m_nUniqueID, eActor::CS_CMDMOVE, (BYTE*)Stream.GetBuffer(), Stream.Tell());
	//Query(CS_ACTOR, eActor::CS_CMDMOVE, timeGetTime());

	//보내고 다시 아이들
	GetProperty().nPlayState = PlayStateInfo::PS_IDLE;
	AddUserLog(_T("유저아이들"));
}

WCHAR *g_szDummyMsg[] =
{
	L"초딩돈 빨아서 살림살이 좀 나아지셨습니까?",
	L"부러우면 지는거다",
	L"카타콤 마스터 팟 구해요",
	L"사장님 월급 좀 올려줘요",
	L"응모권이랑 골드 교환해요",
	L"진리의 아처 꿀벅지",
	L"우유빛깔 소서"
	L"나는야 천재",
	L"인센으로 집사자",
	L"엔진프로그래머 구해요",
	L"이거 말고 그냥 와우 해요",
	L"그깟 공놀이 별루야",
	L"이것도 자이언트 서버 인가요?",
};

void DnDummyClient::_UpdateChat()
{	
	if ( GetProperty().nCurPosition != CurrentServerInfo::_VILLAGE_SERVER )
		return ;

	WCHAR szMsg[128];
	swprintf(szMsg, L"%s", g_szDummyMsg[ rand() % ( sizeof( g_szDummyMsg ) / sizeof( WCHAR * ) ) ] );
	
	m_TCPSender.SendChatMsg(szMsg);
	Query(CS_CHAT, eChat::CS_CHATMSG, timeGetTime());
	GetProperty().nPlayState = PlayStateInfo::PS_IDLE;
	AddUserLog(_T("유저아이들"));
}

void DnDummyClient::SwapItem()
{
	CSMoveItem Item;

	if( m_bSwap )
	{
		Item.cMoveType = 2;
		Item.cSrcIndex = 0;
		Item.cDestIndex = 2;
		Item.wCount = 1;
		m_TCPSender.SendPacket( SC_ITEM, eItem::CS_MOVEITEM, &Item, sizeof( CSMoveItem ) );
		Item.cSrcIndex = 1;
		Item.cDestIndex = 3;
		m_TCPSender.SendPacket( SC_ITEM, eItem::CS_MOVEITEM, &Item, sizeof( CSMoveItem ) );
	}
	else
	{
		Item.cMoveType = 2;
		Item.cSrcIndex = 2;
		Item.cDestIndex = 0;
		Item.wCount = 1;
		m_TCPSender.SendPacket( SC_ITEM, eItem::CS_MOVEITEM, &Item, sizeof( CSMoveItem ) );
		Item.cSrcIndex = 3;
		Item.cDestIndex = 1;
		m_TCPSender.SendPacket( SC_ITEM, eItem::CS_MOVEITEM, &Item, sizeof( CSMoveItem ) );
	}
	m_bSwap = !m_bSwap;
}

void DnDummyClient::_UpdateIdle()
{
	if ( GetProperty().nCurPosition != CurrentServerInfo::_VILLAGE_SERVER )
		return ;
	
	if ( g_DummyClientMgr.GetMode() != DnDummyClientMgr::VillageMode)
	{
		if (m_nPartyIndex == -1)
		{
			if (g_SettingPropertyPanel->GetSettingPropertyData().nGameStartUserCnt == 1)
			{
				AddUserLog(_T("파티 생성 요청"));
				m_TCPSender.SendCreateParty();
				SetNextCmdTime();
				GetProperty().nPlayState = PlayStateInfo::PS_CREATE_REQ_PARTY;
				//m_TCPSender.SendStartStage(g_SettingPropertyPanel->GetSettingPropertyData().nMapIndex);
				//GetProperty().nPlayState = PlayStateInfo::PS_START_STAGE;
			}
			else
				GetProperty().nPlayState = PlayStateInfo::PS_THINKING_PARTY;
		}
		else
			GetProperty().nPlayState = PlayStateInfo::PS_COMPLETE_CREATE_PARTY;
	}
	
	else if ( g_DummyClientMgr.GetMode() == DnDummyClientMgr::VillageMode )
	{
		switch( rand() % 5 )
		{
			case 0:
				GetProperty().nPlayState = PlayStateInfo::PS_USERPOS;
				break;
			case 1:
				GetProperty().nPlayState = PlayStateInfo::PS_CHAT;
				break;
			case 2:
				GetProperty().nPlayState = PlayStateInfo::PS_THINKING_PARTY;
				break;
			case 3:
				if( rand() % 30 == 0 )
				{
					Disconnect();
				}
				break;
			case 4:
				SwapItem();
				break;
		}
	}

	//GetProperty().nPlayState = PlayStateInfo::PS_START_STAGE;
}


EtVector3 
DnDummyClient::GetNewPos()
{


	int nX = int(gs_MaxPos.x - gs_MinPos.x);
	int nY = int(gs_MaxPos.y - gs_MinPos.y);
	int nZ = int(gs_MaxPos.z - gs_MinPos.z);
	float fResultX = float(rand()%nX + gs_MinPos.x);
	float fResultY = gs_MaxPos.y;//float(rand()%nY + gs_MinPos.Y);
	float fResultZ = float(rand()%nZ + gs_MinPos.z);

	return EtVector3(fResultX, 0.0f, fResultZ);
}


int DnDummyClient::GetPossibleJoinPartyIndex()
{
	PARTYLISTINFO_MAP_ITER iter = m_mapPartyListInfo.begin();
	for ( ; iter != m_mapPartyListInfo.end() ; ++iter )
	{
		PartyListStruct Struct = iter->second;
		if ( Struct.nCurCount < Struct.nMaxCount )
		{
			return Struct.nPartyIndex;
		}
	}

	return -1;
}

void DnDummyClient::Query(int nMainCmd, int nSubCmd, ULONG nTick)
{
	Req req;
	memset(&req, 0, sizeof(req));

	req.nMainCmd = nMainCmd;
	req.nSubCmd = nSubCmd;
	req.nTick = nTick;

	m_ReqList.push(req);
}

void DnDummyClient::Answer(UINT nUID, int nMainCmd, int nSubCmd, ULONG nTick)
{
	if (m_nUniqueID != nUID)	return;
	Req req = m_ReqList.front();
	m_ReqList.pop();

	ULONG nDelta = nTick - req.nTick;		//이게 걸린시간
	return;
}

int  DnDummyClient::GetRemainPlayTime( )
{
	if ( g_DummyClientMgr.GetMode() != DnDummyClientMgr::MoveMode )
		return 1;

	return m_nRemainPlayTime;
}
