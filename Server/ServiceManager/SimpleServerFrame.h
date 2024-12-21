
#pragma once

class CSimpleServerFrame
{
public:
	CSimpleServerFrame();
	virtual ~CSimpleServerFrame();

	bool Open(USHORT nAcceptPort);
	void Close();

	virtual void Recv(SOCKET * pSocket, const char * pData, int nLen) = 0;

private:
	HANDLE m_hThread;
	SOCKET m_hSocket;
	USHORT m_nPort;

	volatile bool m_bAlive;
	void ThreadMain();
	static unsigned int __stdcall _threadmain(void * pParam);
};
