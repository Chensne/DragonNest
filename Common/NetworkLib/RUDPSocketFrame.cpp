
#include "stdafx.h"
#include <crtdbg.h>
#include <process.h>
#include <MMSystem.h>
#include "RUDPSocketFrame.h"
#include "Log.h"
#include "MemPool.h"
#include "PerfCheck.h"

#include <fstream>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

static unsigned long GetTimeTick()
{
	static int pivot = timeGetTime();
	return timeGetTime() - pivot;
}

CRUDPSocketFrame::CRUDPSocketFrame()
{
	m_hSocket = INVALID_SOCKET;
#ifdef _SYNC_RUDP
	InitializeCriticalSection(&m_CS);
#endif
	m_hThread = NULL;
	m_bAlive = true;
	m_iNetIDCnt = 1;
	memset(m_nLastCheckTime, 0, sizeof(m_nLastCheckTime));

	m_nThreadid = 0;

	memset(m_szPublicIP, 0, sizeof(m_szPublicIP));
	memset(m_szPrivateIP, 0, sizeof(m_szPrivateIP));
	m_nPort = 0;
	m_nCurTick = 0;
	
#ifdef _DEBUG
#ifdef _SYNC_RUDP
	m_ProtectedSync = false;
#endif
#endif
	::srand(timeGetTime());
	GetTimeTick();

#if defined( STRESS_TEST )
	CRUDPConnect* pConnect = new CRUDPConnect( inet_addr("10.0.3.25"), htons(5000), 0 );
	m_ConnectList[0] = pConnect;
#endif
	m_dwAffinityNum = INT_MAX;
	m_bEnableAffinityCheck = false;
	m_bClient = false;
}

CRUDPSocketFrame::~CRUDPSocketFrame()
{
	Close();
	_ASSERT(m_hThread == NULL);						// 이곳에서 Assert 한번 났습니다...
	_ASSERT(m_hSocket == INVALID_SOCKET);
#ifdef _SYNC_RUDP
	DeleteCriticalSection(&m_CS);
#endif
}

unsigned int __stdcall CRUDPSocketFrame::_threadmain(void * pParam)
{
	((CRUDPSocketFrame*)pParam)->ThreadMain();
	return 0;
}

void CRUDPSocketFrame::ThreadMain()
{
#ifdef PRE_ADD_THREADAFFINITY
	if (m_dwAffinityNum != INT_MAX)
	{
		DWORD dwNumber = GetCurrentProcessorNumber();
		if (m_dwAffinityNum != dwNumber)
			EnableAffinitySetting();
	}
#endif		//#ifdef PRE_ADD_THREADAFFINITY

	char buffer[1024*4];
	int readbytes, addrlen;
	unsigned long totalbytes, i;
	unsigned long busytime;
	SOCKADDR_IN addr;
	fd_set fdset;
	timeval tm;

#ifndef _SKIP_THREAD
	while (m_bAlive)
#endif
	{
//		PROFILE_TIME_TEST_BLOCK_START( "CRUDPSocketFrame::ThreadMain()" );
		tm.tv_sec = 0;
		tm.tv_usec = _SELECT_TIMEOUT_SRV;

		FD_ZERO(&fdset);
		FD_SET(m_hSocket, &fdset);

		m_nCurTick = GetTimeTick();
		if (select(FD_SETSIZE, (fd_set*)&fdset, (fd_set*)0, (fd_set*)0, &tm) != SOCKET_ERROR)
		{
			busytime = GetCurTick() + _SELECT_BUSYTIME;		//처리하는 도중 데이타 도착의 경우 처리를 빠르게 하기위해서 루프를 만든다. 
																								//단 너무 돌아서 좋을게 없으므로 적당한 지연시간을 갖는다.
			if (fdset.fd_count > 0)
			{
				do {	
					ioctlsocket(m_hSocket, FIONREAD, &totalbytes);	

					for (i = 0; i < totalbytes && m_bAlive == true;)
					{
						addrlen = sizeof(addr);
						readbytes = recvfrom(m_hSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);

						if (readbytes == SOCKET_ERROR)
						{
							int lasterr = GetLastError();
							if (lasterr == WSAECONNRESET)
							{
								Enter();
								std::map <SOCKADDR_IN*, CRUDPConnect*, _addr_less_>::iterator ii = m_ConnectRef.find(&addr);
								if (ii != m_ConnectRef.end() && 
									(*ii).first->sin_addr.S_un.S_addr == addr.sin_addr.S_un.S_addr && (*ii).first->sin_port == addr.sin_port)
									DisConnectPtr((*ii).second, false);
								Leave();
#ifdef _DEBUG
								_tprintf(_T("끊겨부렀삼!, %d (%d.%d.%d.%d)\n"), lasterr, 
									addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2,
									addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4, 
									addr.sin_port);
#endif
							}
							else
							{
#ifdef _DEBUG
								_tprintf(_T("아파~~~, %d (%d.%d.%d.%d)\n"), lasterr, 
									addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2,
									addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4, 
									addr.sin_port);			//oops!
#endif
							}
							break;
						}	
						else
						{//정상처리
							if (readbytes == 0)	break;
							i += readbytes;
							Recv(buffer, readbytes, &addr);
#ifdef _DEBUG
							//필요하면 디버그리포팅
#endif
						}
					}
				} while(totalbytes != 0 && GetTimeTick() < busytime);
			}
		}
		unsigned long curtime = GetCurTick();
	
		TimeEvent();
		if (m_bAlive == true && curtime - m_nLastCheckTime[2] >= _IDLE_PROCESS)
		{//살아 있고 아이들프로세스 값보다 크다면....
			m_nLastCheckTime[2] = curtime;

			Enter();
			if (curtime - m_nLastCheckTime[0] > _CHECK_UNREACHABLE)
			{//중요한 메세지인데 받았다는 연락이 아직 없다면 다시 보내 봅니다.
				m_nLastCheckTime[0] = curtime;
				CheckUnreachable(curtime);
			}

			//메세지처리 패킷을 쫘악~ 보내줍니다.
#if !defined( STRESS_TEST )
			FlushAck();
#endif
			Leave();

			//
			if (m_bClient)
			{
				static unsigned long s_nPreTick = 0;
				if (s_nPreTick == 0)
					s_nPreTick = curtime;
				else if (s_nPreTick + (_PINGSENDTICK) < curtime)
				{
					PingCheck();
					s_nPreTick = curtime;
				}
			}
			else
			{
				if (curtime - m_nLastCheckTime[1] > _CHECK_RECVTICK)
				{
					m_nLastCheckTime[1] = curtime;
					CheckRecvTick(curtime);
				}
			}
		}

		if (m_bAlive == true)
		{//잘린녀석들은 실재로 잘라 줍니다.
			Enter();
			if (m_DisConnectList.size() > 0)
			{
				std::map <int, CRUDPConnect*>::iterator ii;
				for (unsigned int j = 0; j < m_DisConnectList.size(); j++)
				{
					ii = m_ConnectList.find(m_DisConnectList[j].second);
					if (ii != m_ConnectList.end())
					{
						DisConnectPtr((*ii).second, true, m_DisConnectList[j].first);
						_ASSERT(m_ConnectList.find(m_DisConnectList[j].second) == m_ConnectList.end());
					}
				}
				m_DisConnectList.clear();
			}
			Leave();
		}
//		PROFILE_TIME_TEST_BLOCK_END();
	}
}

#ifdef PRE_ADD_THREADAFFINITY
void CRUDPSocketFrame::CheckAffinitySetting(DWORD dwTime)
{
	if (m_bEnableAffinityCheck && m_dwAffinityNum != INT_MAX)
	{
		DWORD dwNumber = GetCurrentProcessorNumber();
		if (dwNumber != m_dwAffinityNum)
			_SetThreadAffinityMask(m_dwAffinityNum);

		m_bEnableAffinityCheck = false;
	}
}

void CRUDPSocketFrame::EnableAffinitySetting()
{
	m_bEnableAffinityCheck = true;
}
#endif		//#ifdef PRE_ADD_THREADAFFINITY

#include <Iprtrmib.h>

#define CXIP_A(IP)	((IP&0xFF000000)>>24)
#define CXIP_B(IP)	((IP&0x00FF0000)>>16)
#define CXIP_C(IP)	((IP&0x0000FF00)>>8)
#define CXIP_D(IP)	(IP&0x000000FF)

bool FileExists(const char* path)
{
	std::ifstream my_file(path);
	if (my_file)
	{
		return true;
	}
	return false;
}

void CRUDPSocketFrame::GetHostAddr()
{
	
	if(FileExists(".\\RLKT\\IP.ini") == true)
	{
		char PrivateIP[255];
		char PublicIP[255];
		printf("[RLKT]LOADING IP Data from file.\n");
		GetPrivateProfileStringA("GameServer","PublicIP","127.0.0.1",PublicIP,255,".\\RLKT\\IP.ini");
		GetPrivateProfileStringA("GameServer","PrivateIP","127.0.0.1",PrivateIP,255,".\\RLKT\\IP.ini");

		strcpy(m_szPublicIP,PublicIP);
		strcpy(m_szPrivateIP,PrivateIP);

		return;
	}

	DWORD dwPrivateIP		= 0;
	DWORD dwPrivateIPMask	= 0;
	DWORD dwPublicIP		= 0;
	DWORD dwPublicIPMask	= 0;

	HMODULE hIPHLP = LoadLibrary( _T("iphlpapi.dll") );
	if( hIPHLP )
	{
		typedef	BOOL (WINAPI * LPGIPT)(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize, BOOL bOrder);
		LPGIPT fnGetIpAddrTable=(LPGIPT)GetProcAddress(hIPHLP, "GetIpAddrTable");
		if( fnGetIpAddrTable )
		{
			PMIB_IPADDRTABLE pIPAddrTable;
			DWORD dwSize=0;

			pIPAddrTable=(MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
			if(!pIPAddrTable) 
			{
				FreeLibrary(hIPHLP);
				return;
			}

			if( fnGetIpAddrTable(pIPAddrTable, &dwSize, 0)==ERROR_INSUFFICIENT_BUFFER )
			{
				free(pIPAddrTable);
				pIPAddrTable=(MIB_IPADDRTABLE *)malloc(dwSize);
				if(!pIPAddrTable) 
				{
					FreeLibrary(hIPHLP);
					return;
				}
			}

			if( fnGetIpAddrTable(pIPAddrTable, &dwSize, 0) == NO_ERROR )
			{ 
				for( DWORD i=0; i<pIPAddrTable->dwNumEntries ; ++i )
				{
					DWORD	dwIP		= ntohl(pIPAddrTable->table[i].dwAddr);
					BOOL	bPrivate	= false;

					if(CXIP_A(dwIP)==127)
					{
						continue;
					}
					else if(CXIP_A(dwIP)==10)
					{
						bPrivate=true;
					}
					else if(CXIP_A(dwIP)==172)
					{
						if(CXIP_B(dwIP)>=16 && CXIP_B(dwIP)<=31) 
							bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==192)
					{
#if defined(_ID) // 인도네시아 VPN 192.168.2 대역은 그냥 패쓰합니다.
						if(CXIP_B(dwIP)==168)
						{
							if( CXIP_C(dwIP)==2)
								continue;					
#else
						if(CXIP_B(dwIP)==168) 
						{						
#endif
							bPrivate=TRUE;
						}
					}

					if(bPrivate)
					{
						if( !dwPrivateIP || dwPrivateIP>dwIP )
						{
							dwPrivateIP=dwIP;
							dwPrivateIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						}
					}
					else
					{
						if( !dwPublicIP )
						{
							dwPublicIP=dwIP;
							dwPublicIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						}
					}

					if( dwPrivateIP &&  dwPublicIP) 
						break;
				}
			}
			else
			{
				FreeLibrary(hIPHLP);
				return;
			}

			BOOL bIPAdjust=FALSE;
			// Check Public IP
			if(dwPrivateIP && !dwPublicIP)
			{
				bIPAdjust=TRUE;

				for(DWORD i=0; i<pIPAddrTable->dwNumEntries; ++i)
				{
					DWORD dwIP=ntohl(pIPAddrTable->table[i].dwAddr);
					BOOL bPrivate=FALSE;

					if(CXIP_A(dwIP)==127)
					{
						continue;
					}
					else if(CXIP_A(dwIP)==10)
					{
						bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==172)
					{
						if(CXIP_B(dwIP)>=16 && CXIP_B(dwIP)<=31) 
							bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==192)
					{
#if defined(_ID) // 인도네시아 VPN 192.168.2 대역은 그냥 패쓰합니다.
						if(CXIP_B(dwIP)==168)
						{
							if( CXIP_C(dwIP)==2)
								continue;					
#else
						if(CXIP_B(dwIP)==168) 
						{						
#endif
							bPrivate=TRUE;
						}
					
					}

					if(bPrivate && dwPrivateIP!=dwIP)
					{
						dwPublicIP=dwIP;
						dwPublicIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						break;
					}
				}
			}

			// Check Not Found Public IP
			if(!dwPublicIP)
			{
				dwPublicIP		= dwPrivateIP;
				dwPublicIPMask	= dwPrivateIPMask;
			}
			else
			{
				if( bIPAdjust && dwPrivateIP>dwPublicIP )
				{
					DWORD dwIP		= dwPrivateIP;
					DWORD dwIPMask	= dwPrivateIPMask;

					dwPrivateIP		= dwPublicIP;
					dwPrivateIPMask	= dwPublicIPMask;
					dwPublicIP		= dwIP;
					dwPublicIPMask	= dwIPMask;
				}
			}

			// Clear
			free(pIPAddrTable);
		}
		else
		{
			FreeLibrary(hIPHLP);
			return;
		}

		// Free Libary
		FreeLibrary(hIPHLP);
	}
	else
	{
		printf("GetAddr Failed\n");
		return;
	}

	// Check IP
	if(!dwPrivateIP && !dwPublicIP)
		return;

	DWORD dwNPublicIP = htonl(dwPublicIP);
	_strcpy(m_szPublicIP, _countof(m_szPublicIP), inet_ntoa(*((in_addr*)&dwNPublicIP)), (int)strlen(inet_ntoa(*((in_addr*)&dwNPublicIP))) );

	DWORD dwNPrivateIP = htonl(dwPrivateIP);
	_strcpy(m_szPrivateIP, _countof(m_szPrivateIP), inet_ntoa(*((in_addr*)&dwNPrivateIP)), (int)strlen(inet_ntoa(*((in_addr*)&dwNPrivateIP))) );

	printf("public [ip:%s]\n", m_szPublicIP);
	printf("private [ip:%s]\n", m_szPrivateIP);
}

bool CRUDPSocketFrame::Open(int nID, int nAffinity, int iPort, bool bIsClient, bool bUseAffinity)
{
	_ASSERT(m_hSocket == INVALID_SOCKET && m_hThread == NULL);
	m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_hSocket == INVALID_SOCKET)		return false;

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(iPort);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(m_hSocket, (struct sockaddr *)&addr, sizeof(addr)) == INVALID_SOCKET)
		return false;

	DWORD dwMode = 1;
	if (bIsClient)
		dwMode = 0;
	::ioctlsocket( m_hSocket, FIONBIO, &dwMode );

#ifdef _DEBUG
	int len = sizeof(addr);
	getsockname(m_hSocket, (struct sockaddr*)&addr, &len);
	_tprintf(_T("UDP(port:%d)\n"), ntohs(addr.sin_port));
#endif

	int SendBuf = 51200;
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&SendBuf, sizeof(SendBuf)) == SOCKET_ERROR){
		return false;
	}

#ifndef _SKIP_THREAD
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, _threadmain, (void*)this, 0, &m_nThreadid);
	if (bIsClient == false && bUseAffinity == true)
	{
		m_dwAffinityNum = nAffinity;
		if (_SetThreadAffinityMask(m_dwAffinityNum) == false)
			return false;
	}
#endif
	m_bClient = bIsClient;
	m_nPort = iPort;
	return true;
}

void CRUDPSocketFrame::Close()
{//die die
	
//	while(m_hThread != NULL)
	//{
		m_bAlive = false;
		if (WaitForSingleObject(m_hThread, 2000) != WAIT_TIMEOUT)
		{
			if( m_hThread ) {
				CloseHandle(m_hThread);
				m_hThread = NULL;
			}
		}
		else
			_ASSERT(0);
	//}
	//if( !m_hThread ) m_bAlive = false;

	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	std::map <int, CRUDPConnect*>::iterator ii;
	Enter();
	for (ii = m_ConnectList.begin(); ii != m_ConnectList.end(); ii++)
		delete (*ii).second;
	m_ConnectList.clear();
	m_ConnectRef.clear();
	Leave();
}

bool CRUDPSocketFrame::_SetThreadAffinityMask(DWORD dwAffinityMask)
{
	DWORD_PTR dwMask = SetThreadAffinityMask(m_hThread, 1 << dwAffinityMask);
	if (dwMask == ERROR)
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_INVALID_PARAMETER)
			g_Log.Log(LogType::_FILELOG, L"Invalid thread mask parameter");
		else
			g_Log.Log(LogType::_FILELOG, L"Invalid thread mask error [%d]", dwError);
		return false;
	}
	return true;
}

int CRUDPSocketFrame::Connect(const char * pIP, int iPort)
{
	CRUDPConnect * con = Connect(inet_addr(pIP), htons(iPort));
	return con->GetID();
}

void CRUDPSocketFrame::DisConnect(int iNetID)
{//외부에서 호출이 되는 녀석입니다. 삭제리스트에 담기만 하고 한꺼번에 지워집니다.
	std::map <int, CRUDPConnect*>::iterator ii;
	unsigned int i;

	Enter();
	ii = m_ConnectList.find(iNetID);
	if (ii != m_ConnectList.end())
	{
		for (i = 0; i < m_DisConnectList.size(); i++)
			if (m_DisConnectList[i].second == iNetID)
				break;

		if (i == m_DisConnectList.size())
		{
			_RELIABLE_UDP_HEADER ack;
			ack.combo = _PACKET_HEADER(7, 0);
			ack.crc = 0;
			ack.crc = CRUDPConnect::GetCRC((unsigned char*)&ack, sizeof(ack));
			SendTo(&ack, sizeof(ack), (*ii).second->GetAddr());
			SendTo(&ack, sizeof(ack), (*ii).second->GetAddr());

			m_DisConnectList.push_back(std::make_pair(false, iNetID));
		}
	}
	Leave();
}

void CRUDPSocketFrame::DisConnectAsync(int iNetID)
{
	std::map <int, CRUDPConnect*>::iterator ii;
	unsigned int i;
	
	ii = m_ConnectList.find(iNetID);
	if (ii != m_ConnectList.end())
	{
		for (i = 0; i < m_DisConnectList.size(); i++)
			if (m_DisConnectList[i].second == iNetID)
				break;

		if (i == m_DisConnectList.size())
		{
			_RELIABLE_UDP_HEADER ack;
			ack.combo = _PACKET_HEADER(7, 0);
			ack.crc = 0;
			ack.crc = CRUDPConnect::GetCRC((unsigned char*)&ack, sizeof(ack));
			SendTo(&ack, sizeof(ack), (*ii).second->GetAddr());
			SendTo(&ack, sizeof(ack), (*ii).second->GetAddr());

			m_DisConnectList.push_back(std::make_pair(true, iNetID));
		}
	}
}

int CRUDPSocketFrame::Send(int iNetID, void * data, int len, int prior)
{//외부에서 호출이 됩니다. 동기화 주의!
	std::map <int, CRUDPConnect*>::iterator ii;
	int ret = -1;

	Enter();
	ii = m_ConnectList.find(iNetID);
	if (ii != m_ConnectList.end())
	{
#ifdef _DEBUG
		_ASSERT(prior<3);
#endif
		ret = (*ii).second->Send(data, len, prior, this);
	}
	Leave();
	return ret;
}

void CRUDPSocketFrame::SendTo(void * data, int len, SOCKADDR_IN * addr)
{
#ifdef _SEND_CHECK
	static int _lastchecktime = 0;
	static int _sendbytes = 0, _cnt = 0;
#endif

	int ret = sendto(m_hSocket, (char*)data, len, 0, (struct sockaddr*)addr, sizeof(SOCKADDR_IN));

#ifdef _SEND_CHECK
	if (m_bClient == false && ret == SOCKET_ERROR)
	{
		DWORD err = GetLastError();
		_tprintf(_T("UDPSENDERR %d\n"), GetLastError());
	}
	_sendbytes += len;
	_cnt++;
	if (GetCurTick()- _lastchecktime > 10*1000)
	{
		_tprintf(_T("SERVER UDP [S %d:%d:%d]\n"), _sendbytes * 1000 / (GetCurTick() - _lastchecktime), _sendbytes, _cnt);
		_lastchecktime = GetCurTick();
		_sendbytes = 0;
		_cnt = 0;
	}
#endif
}

int CRUDPSocketFrame::CheckPacket(const void * data, int len, void * outbuf)
{//패킷을 체크하여 정상여부 확인
	_RELIABLE_UDP_HEADER * header = (_RELIABLE_UDP_HEADER*)data;

	if (len <= sizeof(_RELIABLE_UDP_HEADER))		return 0;
	unsigned char crc = header->crc, crc2;
	header->crc = 0;
	crc2 = CRUDPConnect::GetCRC((unsigned char*)data, len);
	header->crc = crc;
	if (crc != crc2)	return 0;

	memcpy(outbuf, &header[1], len - sizeof(_RELIABLE_UDP_HEADER));
	return len - sizeof(_RELIABLE_UDP_HEADER);
}

CRUDPConnect * CRUDPSocketFrame::Connect(unsigned long iIP, int iPort)
{
	CRUDPConnect * con = NULL;
	int id;

	Enter();
	id = m_iNetIDCnt++;
	con = new CRUDPConnect(iIP, iPort, id);

	if (m_iNetIDCnt == 0) m_iNetIDCnt++;
	_ASSERT(m_ConnectList.find(con->GetID()) == m_ConnectList.end());
	m_ConnectList[con->GetID()] = con;
	m_ConnectRef[con->GetAddr()] = con;
	Leave();	
	return con;
}

void CRUDPSocketFrame::DisConnectPtr(CRUDPConnect * pCon, bool bForce, bool bUnreachable)
{
	// 상위에서 동기화 되는 함수
#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(m_ProtectedSync == true);
#endif
#endif

#if defined( STRESS_TEST )
	return;
#endif

	std::map<int, CRUDPConnect*>::iterator itor = m_ConnectList.find( pCon->GetID() );
	if( itor != m_ConnectList.end())
		m_ConnectList.erase( itor );

	std::map<SOCKADDR_IN*,CRUDPConnect*,_addr_less_>::iterator itor2 = m_ConnectRef.find(pCon->GetAddr());
	if ( itor2 != m_ConnectRef.end())
		m_ConnectRef.erase( itor2 );
	
	Leave();

#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(m_ProtectedSync == false);
#endif
#endif
	DisConnected(pCon->GetID(), bForce, bUnreachable);

	Enter();
	delete pCon;
}

void CRUDPSocketFrame::Recv(void * pData, int iLen, SOCKADDR_IN * pAddr)
{//동기화 주의!
	_RELIABLE_UDP_HEADER * header = (_RELIABLE_UDP_HEADER*)pData;
	std::map <SOCKADDR_IN*, CRUDPConnect*, _addr_less_>::iterator ii;
	CRUDPConnect * con = NULL;

	Enter();
	ii = m_ConnectRef.find(pAddr);
	if (ii != m_ConnectRef.end() && (*ii).first->sin_addr.S_un.S_addr == pAddr->sin_addr.S_un.S_addr && (*ii).first->sin_port == pAddr->sin_port)
		con = (*ii).second;
	Leave();

	if (con == NULL)
	{
#ifdef _DEBUG
#ifdef _SYNC_RUDP
		_ASSERT(m_ProtectedSync == false);
#endif
#endif
		if (Accept(m_iNetIDCnt, pAddr, pData, iLen) == false)		return;
		con = Connect(pAddr->sin_addr.S_un.S_addr, pAddr->sin_port);
	}

	_ASSERT(con);
	con->Recv(pData, iLen, this);
}

void CRUDPSocketFrame::CheckUnreachable(unsigned long nCurTick)
{
#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(m_ProtectedSync == true);
#endif
	return;		//debug버전에서는 체크하지 않습니다.
#endif

#if defined( STRESS_TEST )
	return;
#endif
	std::map <int, CRUDPConnect*>::iterator ii;
	for (ii = m_ConnectList.begin(); ii != m_ConnectList.end(); ii++)
	{
		if ((*ii).second->CheckUnreachable(nCurTick, this) == false)
		{
#ifdef _GAMESERVER
			g_Log.Log( LogType::_UNREACHABLE, L"[CRUDPSocketFrame::CheckUnreachable] NetID=%d SendQueueSize=%d", (*ii).second->GetID(), (*ii).second->GetSendQueueSize() );
#endif
			DisConnectAsync((*ii).second->GetID());
		}
	}
}

void CRUDPSocketFrame::CheckRecvTick(unsigned long nCurTick)
{
#if defined( STRESS_TEST )
	return;
#endif

#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(m_ProtectedSync == true);
#endif
	return; //디버그버전에서는 체크하지 않습니다.
#endif
	std::map <int, CRUDPConnect*>::iterator ii;
	for (ii = m_ConnectList.begin(); ii != m_ConnectList.end(); ii++)
		if ((*ii).second->CheckRecvTick(nCurTick) == false)
			DisConnectAsync((*ii).second->GetID());
}

void CRUDPSocketFrame::FlushAck()
{
#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(m_ProtectedSync == true);
#endif
#endif
	std::map <int, CRUDPConnect*>::iterator ii;
	for (ii = m_ConnectList.begin(); ii != m_ConnectList.end(); ii++)
		(*ii).second->FlushAck(this);
}

void CRUDPSocketFrame::PingCheck()
{
	if( !m_ConnectList.empty() )
	{
		std::map <int, CRUDPConnect*>::iterator ii;
		for (ii = m_ConnectList.begin(); ii != m_ConnectList.end(); ii++)
			(*ii).second->PingCheck(this);
	}
}


void CRUDPSocketFrame::Enter()
{
#ifdef _SYNC_RUDP
	EnterCriticalSection(&m_CS);
#endif

#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(m_ProtectedSync == false);
	m_ProtectedSync = true;
#endif
#endif
}

void CRUDPSocketFrame::Leave()
{
#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(m_ProtectedSync == true);
	m_ProtectedSync = false;
#endif
#endif

#ifdef _SYNC_RUDP
	LeaveCriticalSection(&m_CS);
#endif
}

//		Class : CRUDPConnect		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRUDPConnect::CRUDPConnect(unsigned long iIP, int iPort, int iNetID)
{
	memset(&m_TargetAddr, 0, sizeof(SOCKADDR_IN));
	m_TargetAddr.sin_family = AF_INET;
	m_TargetAddr.sin_port = iPort;
	m_TargetAddr.sin_addr.S_un.S_addr = iIP;

	m_iNetID = iNetID;
	m_cUnreachableCnt = 0;
	memset(m_AckList, 0, sizeof(m_AckList));
	memset(m_AckListCnt, 0, sizeof(m_AckListCnt));
	m_RTOTick = _RESEND_TIME;
	m_SeqCnt = 0;
	m_AckCnt = 0;
	m_FastCnt = 0;
	m_RecvFastCnt = 0;
	m_RecvSeqCnt = 0;
	m_RecvAckCnt = 0;
	m_RecvTick = 0;

	m_lastchecktime = 0;
	m_sendbytes = m_cnt = 0;
#ifdef _DEBUG
	memset(m_nDebugPriorCnt, 0, sizeof(m_nDebugPriorCnt));
#endif
	m_uiRTOTick = 0;
}

CRUDPConnect::~CRUDPConnect()
{
	for (unsigned int i = 0; i < m_AllocedQueue.size(); i++)
		SAFE_DELETEA(m_AllocedQueue[i]);
}

bool CRUDPConnect::CheckUnreachable(unsigned long iTimeTick, CRUDPSocketFrame * pSocket)
{//받았다는 연락이  없다면 다시 보내야지요~
#ifdef _DEBUG
#ifdef _SYNC_RUDP
	_ASSERT(pSocket->m_ProtectedSync == true);
#endif
#endif

#ifdef NDEBUG
#ifndef _RDEBUG
	if (_RTO_LIST_MAX < m_SendQueue.size())		//쌓이는 메세지수가 넘후 늘어나면 안뎀!
	{
		if( m_uiRTOTick == 0 )
		{
			m_uiRTOTick = iTimeTick;
		}
		else
		{
			if( iTimeTick - m_uiRTOTick >= _RTO_TICK_MAX )
				return false;
		}
	}
	else
	{
		m_cUnreachableCnt = 0;
		m_uiRTOTick = 0;
	}
#endif // #ifndef _RDEBUG
#endif // #ifdef NDEBUG

	std::list <_PACKET_QUEUE*>::iterator ii;
	for (ii = m_SendQueue.begin(); ii != m_SendQueue.end(); ii++)
	{
		if (iTimeTick - (*ii)->tick >= m_RTOTick)
		{
			pSocket->SendTo((*ii)->data, (*ii)->len, &m_TargetAddr);
			(*ii)->tick = iTimeTick;
		}
	}

	return true;
}

bool CRUDPConnect::CheckRecvTick(unsigned long nCurTick)
{
#ifdef NDEBUG
#ifndef _RDEBUG
	if (nCurTick > m_RecvTick + _CHECK_RECVTICK)
		return false;
#endif
#endif
	return true;
}

void CRUDPConnect::FlushAck(CRUDPSocketFrame * pSocket)
{
	for (int i = 0; i < 3; i++)
	{
		if (m_AckListCnt[i] > 0)
		{
			_ASSERT(m_AckListCnt[i] <= 16);
			unsigned char packet[sizeof(_RELIABLE_UDP_HEADER) + 16 * 2];
			_RELIABLE_UDP_HEADER * _packet = (_RELIABLE_UDP_HEADER*)packet;

			_packet->combo = _PACKET_HEADER((i+4), (m_AckListCnt[i] << 3));
			memcpy(packet + sizeof(_RELIABLE_UDP_HEADER), m_AckList[i], m_AckListCnt[i]*2);

			_packet->crc = 0;
			_packet->crc = GetCRC(packet, sizeof(_RELIABLE_UDP_HEADER) + m_AckListCnt[i] * 2);

			pSocket->SendTo(packet, sizeof(_RELIABLE_UDP_HEADER) + m_AckListCnt[i] * 2, &m_TargetAddr);
			m_AckListCnt[i] = 0;
		}
	}
}

void CRUDPConnect::PingCheck(CRUDPSocketFrame * pSocket)
{
	_RELIABLE_UDP_HEADER packet;
	memset(&packet, 0, sizeof(packet));

	packet.combo = _PACKET_HEADER(3, 0);
	packet.crc = 0;
	packet.crc = GetCRC((unsigned char*)&packet, sizeof(packet));

	pSocket->SendTo(&packet, sizeof(packet), &m_TargetAddr);
}

int CRUDPConnect::Send(void * pData, int iLen, int iPrior, CRUDPSocketFrame * pSocket)
{
	_ASSERT(iLen < 512);		//512까지 허용 합니다만 가급적 256이하로 작성해주세요.
	unsigned char buffer[512 + sizeof(_RELIABLE_UDP_HEADER)];
	_RELIABLE_UDP_HEADER * header = (_RELIABLE_UDP_HEADER*)buffer;

	//if (iLen > sizeof(buffer) + sizeof(_RELIABLE_UDP_HEADER))		return -1;
	if (iLen > sizeof(buffer) - sizeof(_RELIABLE_UDP_HEADER))		return -1;
	//헤더를 만들고
	header->flags = 0x00;
	if (iPrior == _FAST)
	{
		_ASSERT((m_FastCnt&0x07) == 0);
		header->combo = _PACKET_HEADER(_FAST, m_FastCnt);
		m_FastCnt += 8;
	}	else
	if (iPrior == _RELIABLE_NOORDER)
	{
		_ASSERT((m_AckCnt&0x07) == 0);
		header->combo = _PACKET_HEADER(_RELIABLE_NOORDER, m_AckCnt);
		m_AckCnt += 8;
	}	else
	if (iPrior == _RELIABLE)
	{
		_ASSERT((m_SeqCnt&0x07) == 0);
		header->combo = _PACKET_HEADER(_RELIABLE, m_SeqCnt);
		m_SeqCnt += 8;
	}	else
		return -1;	//무엇?

	//내용을 붙입니다.
	memcpy(buffer + sizeof(_RELIABLE_UDP_HEADER), pData, iLen);
	header->crc = 0;
	header->crc = GetCRC(buffer, sizeof(_RELIABLE_UDP_HEADER) + iLen);

	//확실히 보내야 하는 메세지라면! 보내기 큐에 저장합니다.
#if defined( STRESS_TEST )
	iPrior = _FAST;
#endif

	if (iPrior != _FAST)
	{
#ifdef _DEBUG
#ifdef _SYNC_RUDP
		_ASSERT(pSocket->m_ProtectedSync == true);
#endif
#endif
		_PACKET_QUEUE * pQueue = CreateQueue(pSocket, buffer, sizeof(_RELIABLE_UDP_HEADER) + iLen, iPrior == _RELIABLE_NOORDER ? 4 : 5);
		if (pQueue == NULL)	return -1;
		m_SendQueue.push_back(pQueue);
		if ((!(m_AckCnt&(7*8)) || !(m_SeqCnt&(7*8))) &&	m_RTOTick > 100)
			m_RTOTick -= 1;
	}

	//자자 보냅시다아~
	pSocket->SendTo(buffer, sizeof(_RELIABLE_UDP_HEADER) + iLen, &m_TargetAddr);

#ifdef _DEBUG
	m_nDebugPriorCnt[iPrior]++;
#endif

#ifdef _SEND_CHECK
	m_sendbytes += sizeof(_RELIABLE_UDP_HEADER) + iLen;
	m_cnt++;
	if (pSocket->GetCurTick()- m_lastchecktime > 60*1000)
	{
		//초당 평균 4K Bytes가 넘어가면 Output
		if (m_sendbytes * 1000 / (pSocket->GetCurTick() - m_lastchecktime) > (1024 * 4))
			_tprintf(_T("INDIVIDUAL UDP [S %d : C %d]\n"), m_sendbytes * 1000 / (pSocket->GetCurTick() - m_lastchecktime), m_cnt);
		m_lastchecktime = pSocket->GetCurTick();
		m_sendbytes = 0;
		m_cnt = 0;
	}
#endif
	
	return iLen;
}

void CRUDPConnect::Recv(void * pData, int iLen, CRUDPSocketFrame * pSocket)
{
	unsigned long nCurTick = pSocket->GetCurTick();
	m_RecvTick = nCurTick;

	_RELIABLE_UDP_HEADER * header = (_RELIABLE_UDP_HEADER*)pData;
	if (iLen < sizeof(_RELIABLE_UDP_HEADER))
		return;

	unsigned char crc = header->crc;
	header->crc = 0;
	if (crc != GetCRC((unsigned char*)pData, iLen))
	{
		//_tprintf(_T("recv err\n"));
		return;
	}
	
	switch(_PACKET_TYPE(header->flags))
	{
		case 0: {
			//	이전에 처리한 녀석이면 말고 (최근 버전보다 이전 녀석)
			if (((_PACKET_SEQ(header->seq) < m_RecvFastCnt && (int)_PACKET_SEQ(header->seq) + 0x8000 > (int)m_RecvFastCnt) ||
				(_PACKET_SEQ(header->seq) > m_RecvFastCnt && (int)_PACKET_SEQ(header->seq) - 0x8000 > (int)m_RecvFastCnt)))
				return ;
			pSocket->Recv(m_iNetID, (char*)pData + sizeof(_RELIABLE_UDP_HEADER), iLen - sizeof(_RELIABLE_UDP_HEADER));
			m_RecvFastCnt += 8;
			break;
		}
		case 1 : {
			std::list <unsigned short>::iterator ii;
			int lastcnt;

			// ack신호를 보낸다
			SendAck(0x04, _PACKET_ACK(header->ack), &m_TargetAddr, pSocket);
			//	이전에 처리한 녀석이면 말고 (최근 버전보다 이전 녀석)
			if (((_PACKET_SEQ(header->seq) < m_RecvAckCnt && (int)_PACKET_SEQ(header->seq) + 0x8000 > (int)m_RecvAckCnt) ||
				(_PACKET_SEQ(header->seq) > m_RecvAckCnt && (int)_PACKET_SEQ(header->seq) - 0x8000 > (int)m_RecvAckCnt)))
				return ;

			//	최근 녀석이 아니면 받았다는 표시를 해둔다. (먼저 표시가 되어 있다면 받았던 녀석)
			if (m_RecvAckCnt != _PACKET_ACK(header->ack))
			{
				//std::list <unsigned short> ::iterator ii;
				for(ii=m_AckQueue.begin(); ii!=m_AckQueue.end(); ii++)
					if ((*ii) == _PACKET_ACK(header->ack))
						return;

				m_AckQueue.push_back(_PACKET_ACK(header->ack));
			}	else
			{	//	최근 녀석이므로 받았다는 표시해두고, 이미 받는 녀석들 중에서 다음 녀석이 있는지 검사
				m_RecvAckCnt += 8;
				do {
					lastcnt = m_RecvAckCnt;
					for(ii=m_AckQueue.begin(); ii!=m_AckQueue.end(); ii++)
					{
						if ((*ii) == m_RecvAckCnt)
						{
							m_AckQueue.erase(ii);
							m_RecvAckCnt += 8;
							break;
						}
					}
				}	while(lastcnt != m_RecvAckCnt);
			}

			//메세지 받은 처리.
			pSocket->Recv(m_iNetID, (char*)pData + sizeof(_RELIABLE_UDP_HEADER), iLen - sizeof(_RELIABLE_UDP_HEADER));
			break ;
		}
		case 2 : {
			//	순서도 지켜주3			

			//	이전에 처리한 녀석이면 말고...
			//	(상대쪽에서 ack 신호를 못받았을 경우...)
			//	(최근 32767번째 이하만 유효함)
			if (((_PACKET_SEQ(header->seq) < m_RecvSeqCnt && (int)_PACKET_SEQ(header->seq) + 0x8000 > (int)m_RecvSeqCnt) ||
				(_PACKET_SEQ(header->seq) > m_RecvSeqCnt && (int)_PACKET_SEQ(header->seq) - 0x8000 > (int)m_RecvSeqCnt)))
			{
				//	받았어요~ : ack 신호를 보낸다
				SendAck(0x05, _PACKET_ACK(header->ack), &m_TargetAddr, pSocket);
				return ;
			}

			//	만약 다음 순서의 패킷이 아니라면 큐에 넣는다.(단 큐에 이미 들어가 있으면 제외)
			if (m_RecvSeqCnt != _PACKET_SEQ(header->seq))
			{// m_RecvQueue 는 recv 안에서만 호출되면 recv 는 하나의 쓰레드에서만 호출되므로 동기화 불필요!
				std::list <_PACKET_QUEUE*> ::iterator ii;
				for(ii=m_RecvQueue.begin(); ii!=m_RecvQueue.end(); ii++)
				{
					if ((*ii)->seq == _PACKET_SEQ(header->seq))
					{
#ifdef _DEBUG
						_ASSERT(memcmp((*ii)->data, pData, iLen) == 0);
#endif
						//	이미 받은걸 다시 보냈네요. 받았어요~ : ack 신호를 보낸다
						SendAck(0x05, _PACKET_ACK(header->ack), &m_TargetAddr, pSocket);
						return;
					}
				}				

				//요거 못받았어요.
				SendAck(0x06, _PACKET_ACK(header->ack), &m_TargetAddr, pSocket);

				// 나중것이 먼저 왔다. 일단 큐에 저장하고 
				pSocket->Enter();
				_PACKET_QUEUE * pQueue = CreateQueue(pSocket, pData, iLen);
				if (pQueue != NULL)
					m_RecvQueue.push_back(pQueue);
				else
				{
					//메모리가 없어서 큐생성을 못한경우 시컨스처리가 불가능해진다. 끊어버림
					pSocket->DisConnectPtr(this, true);
				}
				pSocket->Leave();
				return;
			}

			// 제대로 도착한 경우에만 ack신호를 보낸다.
			//	받았어요~ : ack 신호를 보낸다
			SendAck(0x05, _PACKET_ACK(header->ack), &m_TargetAddr, pSocket);

			//	제대로 도착한 경우 처리하고 큐에서 처리할 것들이 있는 지 찾는다
			m_RecvSeqCnt += 8;
			pSocket->Recv(m_iNetID, (char*)pData + sizeof(_RELIABLE_UDP_HEADER), iLen - sizeof(_RELIABLE_UDP_HEADER));
			// 임시 Siva
			if( !pSocket->IsAlive() ) break;

			//	자, 먼저 도착한 다음 녀석들을 찾아서 순서대로 처리한다
			unsigned short lastcnt;
			do {
				std::list <_PACKET_QUEUE*> ::iterator ii;
				lastcnt = m_RecvSeqCnt;
				for(ii=m_RecvQueue.begin(); ii!=m_RecvQueue.end(); ii++)
				{
					if (_PACKET_SEQ((*ii)->seq) == m_RecvSeqCnt)
					{
						pSocket->Recv(m_iNetID, (char*)(*ii)->data + sizeof(_RELIABLE_UDP_HEADER), (*ii)->len - sizeof(_RELIABLE_UDP_HEADER));
						pSocket->Enter();
						ReleaseQueue(*ii);
						pSocket->Leave();

						m_RecvQueue.erase(ii);
						m_RecvSeqCnt += 8;
						break;
					}
				}
			}	while(lastcnt != m_RecvSeqCnt);
			break;
		}
		case 4 : case 5 :
			//	보낸 큐에서 ack가 리턴되면 리스트에서 삭제한다
#ifdef _FINAL_BUILD
			if (_PACKET_ACKN(header->acknum) > 16) break;
#else
			_ASSERT(_PACKET_ACKN(header->acknum) <= 16);
#endif

			pSocket->Enter();
			if (RemoveQueue(_PACKET_TYPE(header->flags), (unsigned short *)&header[1], _PACKET_ACKN(header->acknum)) == false)
			{	//	ack 가 연달아 왔다는 얘기는 한번 보내도 되는 데이터를 두번 보낸것이라고
				//	볼 수 있다. RTO 타이밍을 늘려준다. 단, 400은 넘지 말자
				if (m_RTOTick < 450)
					m_RTOTick += 15;
			}
			pSocket->Leave();
			break;
		case 6 :
			//RELIABLE처리중 빠진게 있으면 요청 해온다. 잃어 버린 시컨스의 패킷을 빠르게 다시 보내기 위함
#ifdef _FINAL_BUILD
			if (_PACKET_ACKN(header->acknum) > 16) break;
#else
			_ASSERT(_PACKET_ACKN(header->acknum) <= 16);
#endif

			pSocket->Enter();
			ReSendQueue(_PACKET_TYPE(header->flags), (unsigned short *)&header[1], _PACKET_ACKN(header->acknum), pSocket);
			pSocket->Leave();
			break;
		case 3 : {
			////	PING 이요, PONG 하오
			//_RELIABLE_UDP_HEADER ack;
			//ack.combo	= _PACKET_HEADER(6, 0);//_PACKET_PINGCNT(header->pingcnt));
			//ack.crc		= 0;
			//ack.crc		= GetCRC((unsigned char*)&ack, sizeof(ack));
			//pSocket->SendTo(&ack, sizeof(ack), &m_TargetAddr);
			break;
		}
		case 7 :
			pSocket->Enter();
			pSocket->DisConnectPtr(this, true);
			pSocket->Leave();
			break;
		default :
			_ASSERT(0);
			break;
	}
}

unsigned char CRUDPConnect::GetCRC(const unsigned char * data, int len)
{
	unsigned int code = 0;
	for (int i = 0; i < len; i++)
		code += data[i];
	return (0x78 - code + len);
}

void CRUDPConnect::SendAck(int iFlags, int iAck, SOCKADDR_IN * pAddr, CRUDPSocketFrame * pSocket)
{
	_ASSERT(iFlags == 4 || iFlags == 5 || iFlags == 6);
	_ASSERT((iAck&7) == 0);
	_ASSERT(!memcmp(&m_TargetAddr, pAddr, sizeof(SOCKADDR_IN)));
	int n = iFlags-4, i;

	for(i=0; i < m_AckListCnt[n]; i++)
		if (m_AckList[n][i] == iAck)
			return ;

	if (m_AckListCnt[n] >= 16)
		FlushAck(pSocket);

	m_AckList[n][m_AckListCnt[n]++] = iAck;
}

bool CRUDPConnect::ReSendQueue(unsigned short iType, unsigned short * iAckList, int iAckNum, CRUDPSocketFrame * pSocket)
{
	std::list <_PACKET_QUEUE*>::iterator ii;
	for (ii = m_SendQueue.begin(); ii != m_SendQueue.end(); ii++)
	{
		_PACKET_QUEUE * pq = (*ii);
		for (int i = 0; i < iAckNum; i++)
		{
			_ASSERT((iAckList[i]&7) == 0);
			//이 시컨스 이전에 것들 남아 있는게 있으면 이타이밍에 후다다닥 보낸다.
			if (pq->seq < iAckList[i])
			{
				pSocket->SendTo(pq->data, pq->len, &m_TargetAddr);
				pq->tick = pSocket->GetCurTick();		//너무 짧은 시간에 다시 쏘지 않도록 갱신
			}
			else if (pq->seq <= iAckList[i]) 
				return true;
		}
	}
	return false;
}

bool CRUDPConnect::RemoveQueue(unsigned short iType, unsigned short * iAckList, int iAckNum)
{
	std::list <_PACKET_QUEUE*>::iterator ii;
	int i , cnt = 0;
	for (ii = m_SendQueue.begin(); ii != m_SendQueue.end() && cnt < iAckNum;)
	{
		_PACKET_QUEUE * pq = (*ii);
		for (i = 0; i < iAckNum; i++)
		{
			_ASSERT((iAckList[i]&7) == 0);
			if (pq->seq == iAckList[i] && pq->type == iType)
				break;
		}
		if (i < iAckNum)
		{
			ReleaseQueue(pq);
			m_SendQueue.erase(ii);
			ii = m_SendQueue.begin();
			cnt++;
		}
		else
			ii++;
	}
	return cnt > 0 ? true : false;
}

_PACKET_QUEUE * CRUDPConnect::CreateQueue(CRUDPSocketFrame *pSocket, void * buffer, int len, int type)
{
	_PACKET_QUEUE * q;
	int lv, i;
	_ASSERT(len < 512 - sizeof(_PACKET_QUEUE));
	lv = MemLevel(sizeof(_PACKET_QUEUE) + len) ;
	static const int _size[] = { 32, 64, 128, 256, 512 };
	if( lv >= _countof(_size) )
		return NULL;
	if( m_EmptyQueue[lv].empty() )
	{
		char * ptr = new char [1024];
		if (ptr == NULL) return NULL;
		m_AllocedQueue.push_back(ptr);
		for(i=0; i<1024; i+=_size[lv])
			m_EmptyQueue[lv].push_back((_PACKET_QUEUE*)(ptr + i));
	}

	_ASSERT( !m_EmptyQueue[lv].empty() );
	q = m_EmptyQueue[lv].back();
	m_EmptyQueue[lv].pop_back();

	q->seq	= _PACKET_SEQ(((_RELIABLE_UDP_HEADER*)buffer)->seq);
	q->type = type;
	q->len	= (unsigned short) len;
	q->origintick = q->tick	= pSocket->GetCurTick();

	memcpy(q->data, buffer, len);
	return q;
}

void CRUDPConnect::ReleaseQueue(_PACKET_QUEUE * pQueue)
{
	int lv = MemLevel(sizeof(_PACKET_QUEUE) + pQueue->len) ;
	m_EmptyQueue[lv].push_back(pQueue);
}

int CRUDPConnect::MemLevel(int size)
{
	_ASSERT(size < 512);

	if (size <= 64)
		return size <= 32 ? 0 : 1;
	if (size <= 256)
		return size <= 128 ? 2 : 3;
	return size <= 512 ? 4 : 5;
}
