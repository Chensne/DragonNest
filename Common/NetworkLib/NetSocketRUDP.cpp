
#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include "NetSocketRUDP.h"
#include "DNPacket.h"
#include "ClientSessionManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CNetSocketRUDP::CNetSocketRUDP()
{
	memset(&m_DirectAddr, 0, sizeof(_ADDR));
	memset(&m_DetectAddr, 0, sizeof(_ADDR));
	m_hNetID	= 0;

	m_UDPIP = 0;
	m_UDPPort = 0;
	m_pReceiver = NULL;
	m_bIdle = false;
}

CNetSocketRUDP::~CNetSocketRUDP()
{
	if (m_hNetID != NULL)
		DisConnect();

	CRUDPSocketFrame::Close();
	m_bIdle = false;
}

bool CNetSocketRUDP::Connect(const char * ip, int port)
{
	m_bIdle = Connect(inet_addr(ip), port);
	return m_bIdle;
}

bool CNetSocketRUDP::Connect(unsigned long ip, int port)
{
	if (m_hNetID > 0)
		return true;

	SOCKADDR_IN addr;

	addr.sin_addr.S_un.S_addr = ip;
	memcpy(m_DirectAddr.ip, &ip, 4);
	m_DirectAddr.port	= port;

	if (CRUDPSocketFrame::Open(0, 0, 0, true, false) == false)
		return false;

	m_hNetID = CRUDPSocketFrame::Connect(inet_ntoa(addr.sin_addr), port);
	m_bIdle = true;
	return true;
}

void CNetSocketRUDP::DisConnect()
{
	if (m_hNetID != 0)
	{
		CRUDPSocketFrame::DisConnect(m_hNetID);
		CNetSocketRUDP::Close();
	}
}

void CNetSocketRUDP::DisConnected(int iNetID, bool bForce, bool bUnreachable)
{
	if (m_hNetID != 0)
	{
		CNetSocketRUDP::Close();
		m_hNetID = 0;
	}
}

int CNetSocketRUDP::Send(int main_header, int sub_header, void * data, int size, int prior, BYTE cSeqLevel)
{
	DNGAME_PACKET packet;
	int len;

	len = EncodeGamePacket(&packet, main_header, sub_header,data, size, cSeqLevel);
	len = SendTo(&packet, len, prior);
	return len;
}

void CNetSocketRUDP::Receive(void * data, int size, _ADDR * addr)
{
	char * ptr;
	int packetsize, i = 0;	
	DNGAME_PACKET * p;

	if (size <= 0)	return;
	for (i = 0; i < size;)
	{
		ptr = (char*)data + i;
		packetsize = CalcGamePacketSize((DNGAME_PACKET*)ptr, size - i);

		if (i + packetsize > size)	break;

		p = (DNGAME_PACKET*)ptr;
		if (DecodeGamePacket(p) == true)
		{			
#ifndef _SKIP_THREAD
			if (m_pReceiver)
				m_pReceiver->RecvData(p);
#else
			if (m_pReceiver)
				m_pReceiver->RUDPRecieve(p->header, p->sub_header, p->data, p->datasize, addr);
#endif
		}

		i += packetsize;
	}
}

void CNetSocketRUDP::DetectAddr(const char * ip, int port)
{
	if (port <= 0)	return;
	if (m_DetectAddr.port > 0)		return ;

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

			CRUDPSocketFrame::SendTo("ping", 4, &addr);
			Sleep(4000/10);
		}
	}
}

void CNetSocketRUDP::Recv(int id, void * data, int len)
{//RUDPSocketFrame단에서 recv가 발생했다. 패킷으로 처리하자.
	Receive(data, len, &m_DirectAddr);
}

int CNetSocketRUDP::SendTo(void * msg, int size, int prior)
{//버퍼모드는 지원하지 않습니다. 그냥 바로바로 보내요~
	return CRUDPSocketFrame::Send(m_hNetID, msg, size, prior);
}

bool CNetSocketRUDP::Accept(int id, SOCKADDR_IN * addr, const void * buf, int size)
{
	if (m_DetectAddr.port != 0 && addr->sin_port == ntohs(m_DetectAddr.port) && addr->sin_addr.S_un.S_addr == _inet_addr(m_DetectAddr.ip))
	{
		struct _RETURN_UDP_ADDR
		{
			unsigned short port[2];
			unsigned long addr[2];
		} * packet = (_RETURN_UDP_ADDR*)buf;

		_ASSERT(addr->sin_port == ntohs(m_DetectAddr.port) && addr->sin_addr.S_un.S_addr == _inet_addr(m_DetectAddr.ip));

		if (size == sizeof(*packet) && packet->port[0] == packet->port[1] && packet->addr[0] == packet->addr[1] && m_UDPPort == 0)
		{
			m_UDPIP = packet->addr[0];
			m_UDPPort = packet->port[0];
		}
		return true;
	}
	return false;
}