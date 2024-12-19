#pragma once

#include "ClientSession.h"

enum SocketError
{
	ON_CONNECT = 1,
	ON_CLOSE,
	ON_WRITE,
	ON_READ,
	ON_CONNECT_ERROR = 11,
	ON_CLOSE_ERROR,
	ON_WRITE_ERROR,
	ON_READ_ERROR,
};

class CSeqReceiver;
class CClientSocket : public CClientSession
{
private:
	bool m_bConnected;
	bool m_bWrite;
	bool m_bThreadClose;

	CSeqReceiver * m_pReceiver;
	char*	m_pTempBuffer;
	int		m_iTempBufferSize;

protected:
	HANDLE m_hEvent;
	HANDLE m_hThread;
	HANDLE m_hTerminateEvent;
	DWORD m_dwThreadExitCode;
	bool m_bRunning;

	std::string m_szServerIP;
	USHORT m_nServerPort;
	
	void DoSend();
	bool CreateRecvThread();
	void DestroyRecvThread();

	virtual void OnConnect();
	virtual void OnDisconnect( bool bValidDisconnect );
	virtual void OnRecv();
	virtual void OnSend();
	virtual void OnError(int nError);

	int ThreadUpdate();
	static UINT __stdcall RecvThread(void *pParam);	

public:
	CClientSocket(void);
	CClientSocket(int nSize);
	~CClientSocket(void);

	HANDLE GetTerminateEvent() { return m_hTerminateEvent; }
	void SetReceiver(CSeqReceiver * pReceiver) { m_pReceiver = pReceiver; }

	void Init();
	void Clear();

	bool Connect(const char *pIp, const USHORT nPort);
	void Close(bool boGraceful = true, bool bValidDisconnect = false);

	int AddSendData(int iMainCmd, int iSubCmd, char *pData, int iLen, int nInsertAfter = -1);
	int AddRecvData(int iMainCmd, int iSubCmd, char *pData, int iLen, int nInsertAfter = -1);

	virtual int DoUpdate();

	const char *GetServerIP() { return m_szServerIP.c_str(); }
	USHORT GetServerPort() { return m_nServerPort; }

	inline bool Connected() { return m_bConnected; }

	CClientSocket &operator = ( CClientSocket &e );
};
