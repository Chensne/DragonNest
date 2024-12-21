#pragma once

#include "ClientSession.h"
#include "RUDPSession.h"

using namespace std;

#pragma pack(push, 1)

struct TUdpData
{
	USHORT Len;
	sockaddr_in Addr;
	char Data[CLIENTPACKETMAX];
};

#pragma pack(pop)

//##
/*
class CClientUdpSocket : public CClientSession
{
private:
	bool m_bServer;
	bool m_bRecvThreadClose;

	HANDLE m_hThread;
	DWORD m_dwThreadExitCode;
	UINT m_nConnectUniqueID;

protected:
	virtual void OnConnect();
	virtual void OnDisconnect();
	void OnRecv();
	void OnSend();
	void OnError(int nError);

	void DoRecv();
	void DestroyRecvThread();

	//int ThreadUpdate();
	static UINT __stdcall RecvThread(void *pParam);

public:
	CClientUdpSocket(void);
	CClientUdpSocket(int nSize);
	~CClientUdpSocket(void);

	bool Init(const USHORT nPort, bool bServer = false);
	void Clear();

	int Disconnect(int nConnectionID);

	void Send(BYTE cMode, char *pData, int nSize, SOCKADDR_IN *pAddr );
	void SendData(char *pData, int nSize, SOCKADDR_IN *pAddr);

	virtual int DoUpdate();
	virtual void MessageProcess(char *pData, int nSize);
	void Flush();

	void RecvMessageProcess(char *pData, int nSize, SOCKADDR_IN *pAddr);

	void SendReplyMsg(BYTE cMode, SOCKADDR_IN *pAddr);
};

*/

class CClientRUdpSocket : public RUDPSession
{
public:
	CClientRUdpSocket();
	virtual ~CClientRUdpSocket();

	virtual void OnConnect(int nHandle, SOCKADDR_IN Addr);
	virtual void OnDisconnect(int nHandle, SOCKADDR_IN Addr);
	virtual void OnReceive(int nHandle, SOCKADDR_IN Addr, const void* ptr, unsigned short nLen);
};

