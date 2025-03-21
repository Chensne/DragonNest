
#pragma once

#include <winsock.h>
#include "multisync.h"

const int RECVBUFFERSIZEMAX = 1024 * 10;
const int SENDBUFFERDIZEMAX = 1024 * 10;

class CBuffer;
class	CSimpleClientSession
{

public :
	CSimpleClientSession();
	virtual ~CSimpleClientSession();

	bool Start(const TCHAR *ip, int port);
	void Stop() { m_bTerminate = true; }

	int SendPacket(int header, const void *data, int size);

protected :
	virtual bool AddPacket(const DNTPacket * packet) = 0;
	virtual void Connect() {}
	virtual void Destroy();
	virtual void TimeEvent() = 0;

	bool m_bTerminate;

private :
	void ReceiveMessage(int totalbytes);

	char m_pBuff[RECVBUFFERSIZEMAX];
	int m_iLen;
	char m_pSendBuff[SENDBUFFERDIZEMAX];
	CBuffer* m_pSendQueue;
	CSyncLock m_SendSync;			//sendbuffer sync
	HANDLE m_hSendThread;

	void _FlushSendData();

	SOCKET m_hLobbySocket;

	static SOCKET CreateTCPSocket(int port);

	static UINT __stdcall SendThread(void *pParam);
} ;