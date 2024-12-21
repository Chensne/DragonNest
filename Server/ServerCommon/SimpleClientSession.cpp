
#include "stdafx.h"
#include <crtdbg.h>
#include <stdio.h>
#include "SimpleClientSession.h"
#include "DNPacket.h"
#include "DNProtocol.h"
#include "Buffer.h"

#if defined(_LAUNCHER)
extern void TextOut(const TCHAR * format, ...);
#endif

CSimpleClientSession::CSimpleClientSession()
{
	m_hLobbySocket	= INVALID_SOCKET;
	m_iLen			= 0;
	m_bTerminate = false;
	memset(&m_pBuff, 0, sizeof(m_pBuff));

	ScopeLock <CSyncLock> sync(m_SendSync);
	memset(m_pSendBuff, 0, sizeof(m_pSendBuff));
	m_pSendQueue = new CBuffer(SENDBUFFERDIZEMAX);
	m_hSendThread = INVALID_HANDLE_VALUE;
}

CSimpleClientSession::~CSimpleClientSession()
{
	ScopeLock <CSyncLock> sync(m_SendSync);	
	if( m_hSendThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject(m_hSendThread, INFINITE);
		CloseHandle(m_hSendThread);
	}
	SAFE_DELETE(m_pSendQueue);
}

bool CSimpleClientSession :: Start(const TCHAR *ip, int port)
{
	WSADATA	WSAData;
	if (WSAStartup(MAKEWORD(1, 1), &WSAData))
	{
		_ASSERT_EXPR(0, L"Fail WSAStartup");
		return false;
	}

	//	서버에 접속합니다.
	m_hLobbySocket = CreateTCPSocket(0);
	
	if (m_hLobbySocket == INVALID_SOCKET)
		return false;

	SOCKADDR_IN addr;

	char charip[16];
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, ip, (int)_tcslen(ip) + 1, charip, 16, NULL, NULL);
#else
	_tcscpy_s(charip, ip);
#endif

	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(port);
	addr.sin_addr.S_un.S_addr	= inet_addr(charip);
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	if (connect(m_hLobbySocket, (struct sockaddr *)&addr, sizeof(addr)) == INVALID_SOCKET)
	{
		closesocket(m_hLobbySocket);
		m_hLobbySocket = INVALID_SOCKET;
		return false;
	}

	Connect();

	m_bTerminate = false;
	UINT nThreadID;
	m_hSendThread=(HANDLE)_beginthreadex(NULL, 0, SendThread, this, 0, &nThreadID);	

	while(m_bTerminate == false)
	{
		fd_set fds;
		timeval tv;

		tv.tv_sec	= 0;
		tv.tv_usec	= 500;

		FD_ZERO(&fds);
		FD_SET(m_hLobbySocket, &fds);

		if (select(FD_SETSIZE, (fd_set*)&fds, (fd_set*)0, (fd_set*)0, &tv) != SOCKET_ERROR)
		{
			if (fds.fd_count > 0)
			{
				unsigned long totalbytes;

				_ASSERT(fds.fd_array[0] == m_hLobbySocket);

				ioctlsocket(fds.fd_array[0], FIONREAD, &totalbytes);
				if (totalbytes == 0)
				{
					//	접속 끊김
					break;
				}
				else
				{
					ReceiveMessage(totalbytes);
				}
			}
			else
			{
				TimeEvent();
			}
		}

		//_FlushSendData();
	}

	closesocket(m_hLobbySocket);
	m_hLobbySocket = INVALID_SOCKET;

	Destroy();

	return true;
}

UINT __stdcall CSimpleClientSession::SendThread(void *pParam)
{
	CSimpleClientSession* pSession = (CSimpleClientSession*)pParam;

	while(pSession->m_bTerminate == false)
	{
		pSession->_FlushSendData();
		Sleep(100);
	}
	return 0;
}

int CSimpleClientSession::SendPacket(int header, const void *data, int size)
{
	if (m_hLobbySocket == INVALID_SOCKET)
		return -1;

	DNTPacketHeader dnHeader;
	dnHeader.iLen		= static_cast<unsigned short>(sizeof(dnHeader)+size);
	dnHeader.cMainCmd	= static_cast<unsigned char>(header);
	dnHeader.cSubCmd	= 0;

	ScopeLock <CSyncLock> sync(m_SendSync);
	int Ret = m_pSendQueue->Push((char*)&dnHeader, sizeof(dnHeader));	// 헤더
	if(Ret == 0 && size > 0)	
		Ret = m_pSendQueue->Push(reinterpret_cast<const char*>(data), size);						// 데이터
	return Ret;
}


SOCKET CSimpleClientSession :: CreateTCPSocket(int port)
{
	SOCKADDR_IN addr;
	SOCKET s;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s == INVALID_SOCKET)
		return INVALID_SOCKET;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(s);

		return INVALID_SOCKET;
	}

	return s;
}

void CSimpleClientSession::Destroy()
{
	if (m_pSendQueue)
		SAFE_DELETE(m_pSendQueue);
}


void CSimpleClientSession::ReceiveMessage(int totalbytes)
{
	int len;

	if (totalbytes > (int)sizeof(m_pBuff) - m_iLen)
	{
		int seg = (int)sizeof(m_pBuff) - m_iLen;
		ReceiveMessage(seg);
		ReceiveMessage(totalbytes - seg);
		return;
	}

	if (m_iLen >= sizeof(m_pBuff))
	{
		_ASSERT(0);
		return;
	}
	
	len = recv(m_hLobbySocket, m_pBuff + m_iLen, totalbytes, 0);

	if (len < 0)
	{
		Sleep(1000);
		return;
	}

	m_iLen += len;

	int packetsize, i=0;
	while(1)
	{
		packetsize = ((DNTPacket*)&m_pBuff[i])->iLen;
		if (i + packetsize > m_iLen || packetsize == 0)
			break;

		if (AddPacket((DNTPacket*)&m_pBuff[i]) == false)
		{
#ifdef _LAUNCHER
			TextOut(_T("AddPacket Failed!"));
#endif		//#ifdef _LAUNCHER
		}

		i += packetsize;
	}

	if (m_iLen != i)
		memcpy(m_pBuff, m_pBuff+i, m_iLen-i);
	m_iLen -= i;
}

void CSimpleClientSession::_FlushSendData()
{
	if (m_hLobbySocket == INVALID_SOCKET)
		return;

	ScopeLock <CSyncLock> sync(m_SendSync);

	while (1)
	{
		int nSize = m_pSendQueue->GetCount();
		if (nSize <= 0)
			return;

		if (nSize > SENDBUFFERDIZEMAX)
			nSize = SENDBUFFERDIZEMAX;

		memset(m_pSendBuff, 0, sizeof(m_pSendBuff));
		if (m_pSendQueue->View(m_pSendBuff, nSize) < 0)
			continue;

		int i, readbytes;
		for(i = 0; i < nSize;)
		{
			readbytes = send(m_hLobbySocket, (const char*) &m_pSendBuff + i, nSize-i, 0);
			if (readbytes <= 0)
				break;
			i += readbytes;
		}

		m_pSendQueue->Skip(nSize);
	}
}