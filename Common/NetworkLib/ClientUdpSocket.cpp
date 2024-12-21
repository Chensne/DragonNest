#include "StdAfx.h"
#include "ClientUdpSocket.h"
#include "ClientSessionManager.h"

//##
/*
CClientUdpSocket::CClientUdpSocket(void): CClientSession()
 , m_hThread(0)
{
	m_bServer = m_bRecvThreadClose = false;
}

CClientUdpSocket::CClientUdpSocket(int nSize): CClientSession(nSize)
 , m_hThread(0)
{
	m_bServer = m_bRecvThreadClose = false;

}

CClientUdpSocket::~CClientUdpSocket(void)
{
	Clear();
}

bool CClientUdpSocket::Init(const USHORT nPort, bool bServer)
{
	if (nPort <= 0) return false;

	Clear();

	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_Socket == INVALID_SOCKET) return false;
	
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(nPort);

	if (bind(m_Socket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR){
		int nError = WSAGetLastError();
		closesocket(m_Socket);
		return false;
	}

	m_bServer = bServer;

	UINT nThreadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, RecvThread, this, 0, &nThreadID);
	if( m_hThread == INVALID_HANDLE_VALUE ) return false;
	GetExitCodeThread( m_hThread, &m_dwThreadExitCode );

	return true;
}

void CClientUdpSocket::Clear()
{
	DestroyRecvThread();

	if (m_Socket != INVALID_SOCKET){
		OnDisconnect();
		closesocket(m_Socket);
	}

	m_Socket = INVALID_SOCKET;
	m_bRecvThreadClose = false;
}

void CClientUdpSocket::DoRecv()
{
	static char buffer[32768];
	USHORT nRecv;

	TUdpData *pUdpData;
	while (m_pRecvBuffer->GetCount () > sizeof(USHORT)) {
		if (m_pRecvBuffer->View((char *)&nRecv, sizeof(USHORT)) < 0) break;

		if (m_pRecvBuffer->GetCount () < nRecv) break;
		if (m_pRecvBuffer->Pop(buffer, nRecv) < 0) break;

		pUdpData = (TUdpData*)buffer;

		RecvMessageProcess(pUdpData->Data, ( pUdpData->Len - sizeof(USHORT) - sizeof(sockaddr_in) ), &pUdpData->Addr);
	}
}

void CClientUdpSocket::DestroyRecvThread()
{
	if( m_hThread ) TerminateThread( m_hThread, m_dwThreadExitCode );
	m_hThread = 0;
}

int CClientUdpSocket::Disconnect(int nConnectionID)
{
	return 1;
}

void CClientUdpSocket::Send(BYTE cMode, char *pData, int nSize, SOCKADDR_IN *pAddr)
{
	switch(cMode)
	{
	case UDPMODE_SEND_FAST:
		SendData(pData, nSize, pAddr);
		break;

	case UDPMODE_SEND_RELIABLE:
		SendReplyMsg(UDPMODE_SEND_RELIABLE, pAddr);
		SendData(pData, nSize, pAddr);
		break;

	case UDPMODE_SEND_SEQUENTIAL:
		break;

	default:
		break;
	}
}

void CClientUdpSocket::Flush()
{
}

void CClientUdpSocket::SendData(char *pData, int nSize, SOCKADDR_IN *pAddr)
{
	sendto(m_Socket, pData, nSize, 0, (sockaddr*)pAddr, sizeof(SOCKADDR_IN));
}

int CClientUdpSocket::DoUpdate()
{
	DoRecv();
	return 1;
}

void CClientUdpSocket::MessageProcess(char *pData, int nSize)
{
	if( CClientSessionManager::GetInstance().IsActive() )
		CClientSessionManager::GetInstance().OnDispatchMessageUdp( pData, nSize );
}

void CClientUdpSocket::RecvMessageProcess(char *pData, int nSize, SOCKADDR_IN *pAddr)
{
	TUdpHeader *pHeader = (TUdpHeader*)pData;

	switch(pHeader->Mode)
	{
	case UDPMODE_PING:
		break;

	case UDPMODE_PONG:
		break;

	case UDPMODE_DISCONNECT:
		break;

	case UDPMODE_SEND_FAST:
		MessageProcess(pData, nSize);
		break;

	case UDPMODE_SEND_RELIABLE:
		{
			SendReplyMsg(UDPMODE_ACK_RELIABLE, pAddr);
			MessageProcess(pData, nSize);
		}
		break;

	case UDPMODE_SEND_SEQUENTIAL:
		break;

	case UDPMODE_SEND_STATE:
		break;

	case UDPMODE_ACK_RELIABLE:

		break;

	case UDPMODE_ACK_SEQUENTIAL:
		break;

	case UDPMODE_ACK_STATE:
		break;
	}
}

void CClientUdpSocket::SendReplyMsg(BYTE cMode, SOCKADDR_IN *pAddr)
{
	TUdpHeader Header;
	memset(&Header, 0, sizeof(TUdpHeader));

	Header.Len = sizeof(TUdpHeader);
	Header.Mode = UDPMODE_ACK_RELIABLE;

	SendData((char*)&Header, Header.Len, pAddr);
}

void CClientUdpSocket::OnConnect()
{
}

void CClientUdpSocket::OnDisconnect()
{
	CClientSession::OnDisconnect();

	if( CClientSessionManager::GetInstance().IsActive() )
		CClientSessionManager::GetInstance().OnDisconnectUdp();
}

void CClientUdpSocket::OnRecv()
{
	char tmpBuf[CLIENTPACKETMAX];
	memset(tmpBuf, 0, sizeof(tmpBuf));

	SOCKADDR_IN Addr;
	int AddrLen = sizeof(Addr);
	int RecvSize = 0;

	RecvSize = recvfrom(m_Socket, (char*)tmpBuf, sizeof(tmpBuf), 0, (sockaddr*)&Addr, (int*)&AddrLen);
	if (RecvSize > 0){
		TUdpData UdpData;
		memset(&UdpData, 0, sizeof(TUdpData));

		UdpData.Len = (USHORT)(sizeof(USHORT) + sizeof(sockaddr_in) + RecvSize);
		UdpData.Addr = Addr;
		memcpy(UdpData.Data, tmpBuf, RecvSize);

		m_pRecvBuffer->Push((char*)&UdpData, UdpData.Len);
	}
}

void CClientUdpSocket::OnSend()
{

}

void CClientUdpSocket::OnError(int nError)
{

}

UINT __stdcall CClientUdpSocket::RecvThread(void *pParam)
{
	CClientUdpSocket *pUdp = (CClientUdpSocket*)pParam;

	while (1){
		pUdp->OnRecv();
	}
	return 1;
}
*/

//---------------------------------------------------------------

CClientRUdpSocket::CClientRUdpSocket()
{

}

CClientRUdpSocket::~CClientRUdpSocket()
{

}

void CClientRUdpSocket::OnConnect(int nHandle, SOCKADDR_IN Addr)
{

}

void CClientRUdpSocket::OnDisconnect(int nHandle, SOCKADDR_IN Addr)
{

}

void CClientRUdpSocket::OnReceive(int nHandle, SOCKADDR_IN Addr, const void* ptr, unsigned short nLen)
{
	if ( UseThread() == false )
	{
		if( CClientSessionManager::GetInstance().IsActive() )
			CClientSessionManager::GetInstance().OnDispatchMessageUdp( (char*)ptr, nLen );
	}

}