
#include "stdafx.h"
#include <crtdbg.h>
#include "SimpleServerFrame.h"

CSimpleServerFrame::CSimpleServerFrame()
{
	m_hThread = INVALID_HANDLE_VALUE;
	m_hSocket = INVALID_SOCKET;
	m_bAlive = true;
	m_nPort = 0;
}

CSimpleServerFrame::~CSimpleServerFrame()
{
	Close();
}

bool CSimpleServerFrame::Open(USHORT nAcceptPort)
{
	if(m_hThread != INVALID_HANDLE_VALUE || m_hSocket != INVALID_SOCKET)
		return false;

	SOCKADDR_IN addr;
	memset (&addr, 0, sizeof(addr));

	m_hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_hSocket == INVALID_SOCKET)
		return false;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nAcceptPort);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(m_hSocket, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(m_hSocket);
		return false;
	}

	if (listen(m_hSocket, 0) == SOCKET_ERROR){
		closesocket(m_hSocket);
		return false;
	}

	m_hThread = (HANDLE) _beginthreadex(NULL, 0, _threadmain, (void*)this, 0, NULL);
	return true;
}

void CSimpleServerFrame::Close()
{
	while(m_hThread != INVALID_HANDLE_VALUE)
	{
		m_bAlive = false;
		if (WaitForSingleObject(m_hThread, 2000) != WAIT_TIMEOUT)
		{
			CloseHandle(m_hThread);
			m_hThread = INVALID_HANDLE_VALUE;
		}
	}

	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

void CSimpleServerFrame::ThreadMain()
{
	char buffer[1024];
	int readbytes, addrlen = sizeof(sockaddr_in);
	unsigned long totalbytes;
	SOCKADDR_IN addr;
	fd_set fdset;
	timeval tm;

	while (m_bAlive && m_hSocket != INVALID_SOCKET)
	{
		tm.tv_sec = 0;
		tm.tv_usec = INFINITE;

		FD_ZERO(&fdset);
		FD_SET(m_hSocket, &fdset);

		if (select(FD_SETSIZE, (fd_set*)&fdset, (fd_set*)0, (fd_set*)0, &tm) != SOCKET_ERROR)
		{
			if( FD_ISSET( m_hSocket, &fdset ) )
			{
				SOCKET socket = accept(m_hSocket, (sockaddr*)&addr, &addrlen);
				if (socket != INVALID_SOCKET)
				{
					ioctlsocket(m_hSocket, FIONREAD, &totalbytes);
					readbytes = recv(socket, buffer, sizeof(buffer), 0);
					Recv(&socket, buffer, readbytes);

					closesocket(socket);
					socket = INVALID_SOCKET;
				}
			}
		}
		Sleep(500);
	}
}

unsigned int __stdcall CSimpleServerFrame::_threadmain(void * pParam)
{
	((CSimpleServerFrame*)pParam)->ThreadMain();
	return 0;
}