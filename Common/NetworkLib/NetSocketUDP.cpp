
#include "stdafx.h"
#include "NetSocketUDP.h"
#include "NetSocketRUDP.h"
#include "DNPacket.h"
#include "ClientSessionManager.h"
#include <MMSystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CNetSocketUDP::CNetSocketUDP(const char * szNick, CNetSocketRUDP * pSocket) : Thread(szNick)
{
	memset(&m_DirectionAddr, 0, sizeof(SOCKADDR_IN));
	memset(&m_DetectAddr, 0, sizeof(_ADDR));
	m_hSocket = INVALID_SOCKET;
	m_UDPIP = 0;
	m_UDPPort = 0;
	m_pReciever = NULL;
	m_pNetSocket = pSocket;
}

CNetSocketUDP::~CNetSocketUDP()
{
	m_bRunning = false;

	if (WaitForTerminate(100) == false)
		Terminate(0);

	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

void CNetSocketUDP::DetectAddr(const char * ip, int port)
{
	if (port <= 0) return;
	if (m_DetectAddr.port > 0)	return ;

	unsigned long iIP = _inet_addr(ip);
	memcpy(m_DetectAddr.ip, &iIP, 4);
	m_DetectAddr.port = port;

	if (m_DetectAddr.port > 0)
	{//NATProbe를 통하여 UDP IP와 Port를 알아 냅니다.
		for (int i = 0; m_UDPPort == 0 && i < 4000; i +=4000/10)
		{
			SOCKADDR_IN addr;
			addr.sin_family = AF_INET;
			addr.sin_port = ntohs(m_DetectAddr.port);
			addr.sin_addr.S_un.S_addr = _inet_addr(m_DetectAddr.ip);

			sendto(m_hSocket, "ping", 4, 0, (struct sockaddr *)&addr, sizeof(addr));

			Sleep(4000/10);
		}
	}
}

void CNetSocketUDP::SetReciever(CRUDPReceiver * pReciever)
{
	m_pReciever = pReciever;
}

bool CNetSocketUDP::Create(int port)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)		return false;

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	if (::bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(sock);
		return false;
	}

	DWORD dwMode = 1;
	::ioctlsocket( sock, FIONBIO, &dwMode );

	m_hSocket = sock;
	m_bRunning = true;
	Start();
	return true;
}

int CNetSocketUDP::Send(unsigned int main_header, unsigned int sub_header, void * data, int size, unsigned long addr, unsigned short port)
{
	unsigned char packet[1024*4];
	SetDirection(addr, port);		//방향지시하고!
	int enlen = EncodeGamePacket(&packet, main_header, sub_header, data, size);
	return SendTo(&packet, enlen, &m_DirectionAddr);
}

void CNetSocketUDP::Receive(void * data, int size, _ADDR * addr)
{
	char * ptr;
	int packetsize, i=0;
	DNGAME_PACKET * p;

	if (size <= 0)
		return;

	if (m_DetectAddr.port != 0 && addr->port == ntohs(m_DetectAddr.port) && _inet_addr(addr->ip) == _inet_addr(m_DetectAddr.ip))
	{
		struct _RETURN_UDP_ADDR
		{
			unsigned short port[2];
			unsigned long addr[2];
		} * packet = (_RETURN_UDP_ADDR*)data;

		_ASSERT(addr->port == ntohs(m_DetectAddr.port) && _inet_addr(addr->ip) == _inet_addr(m_DetectAddr.ip));

		if (size == sizeof(*packet) && packet->port[0] == packet->port[1] && packet->addr[0] == packet->addr[1] && m_UDPPort == 0)
		{
			m_UDPIP = packet->addr[0];
			m_UDPPort = packet->port[0];
		}
		return ;
	}

	for(i=0; i<size;)	// 받은 내용을 패킷으로 쭉 처리
	{
		ptr = (char*)data + i;
		packetsize = CalcGamePacketSize((DNGAME_PACKET*)ptr, size - i);

		if (i+packetsize > size)
			break;		

		p = (DNGAME_PACKET*)ptr;
		if (DecodeGamePacket(p) == true)
		{
			if (p->header == 2 && p->sub_header == 62)	_ASSERT(0);
			//m_pNetSocket->RecvData(p);
		}

		i += packetsize;
	}
}

unsigned short CNetSocketUDP::GetPort()
{
	if (m_UDPPort > 0)
		return m_UDPPort;

	SOCKADDR_IN addr;
	int addrlen = sizeof(sockaddr_in);
	getsockname(m_hSocket, (struct sockaddr*)&addr, &addrlen);
	return htons(addr.sin_port);
}

unsigned long CNetSocketUDP::GetIP()
{
	return m_UDPIP;
}

int CNetSocketUDP::SendTo(void * data, int size, SOCKADDR_IN * addr)
{
	sendto(m_hSocket, (char*) data, size, 0, (struct sockaddr *)addr, sizeof(*addr));
	return size;;
}

void CNetSocketUDP::SetDirection(unsigned long iIP, unsigned short iPort)
{
	memset(&m_DirectionAddr, 0, sizeof(m_DirectionAddr));

	m_DirectionAddr.sin_family = AF_INET;
	m_DirectionAddr.sin_port = htons(iPort);
	m_DirectionAddr.sin_addr.S_un.S_addr = iIP;
}

void CNetSocketUDP::Run()
{
	char buffer[CLIENTPACKETMAX];
	SOCKADDR_IN remote_addr;
	int remote_addrlen, readbytes;
	unsigned long totalbytes, i;
	fd_set fdset;

	srand(timeGetTime());

	while (m_bRunning == true)
	{
		timeval tm;

		tm.tv_sec	= 0;
		tm.tv_usec	= 500;

		FD_ZERO(&fdset);
		FD_SET(m_hSocket, &fdset);

		if (select(FD_SETSIZE, (fd_set*)&fdset, (fd_set*)0, (fd_set*)0, &tm) != SOCKET_ERROR)
		{
			if (fdset.fd_count > 0 && m_bRunning == true)
			{
				ioctlsocket(m_hSocket, FIONREAD, &totalbytes);

				for(i=0; i<totalbytes;)
				{
					remote_addrlen = sizeof(remote_addr);
					readbytes = recvfrom(m_hSocket, buffer, sizeof(buffer), 0, (struct sockaddr*) &remote_addr, &remote_addrlen);

					i += readbytes;

					if (readbytes > 0)
					{
						_ADDR addr;
						addr.ip[0] = remote_addr.sin_addr.S_un.S_un_b.s_b1;
						addr.ip[1] = remote_addr.sin_addr.S_un.S_un_b.s_b2;
						addr.ip[2] = remote_addr.sin_addr.S_un.S_un_b.s_b3;
						addr.ip[3] = remote_addr.sin_addr.S_un.S_un_b.s_b4;
						addr.port = remote_addr.sin_port;
						Receive(buffer, readbytes, &addr);			// remote_addr 에서 전송되었음 (받은 IP 전달 인터페이스가 없음)
					}
				}
			}
		}
	}
}