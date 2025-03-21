#pragma once

#include "NxDataStructure.h"
#include "NxAsyncEvent.h"
#include "NxSocket.h"
#include "NxPacket.h"

class NxCompletionPort;

/// 리시브 버퍼 사이즈
const int RECV_BUFFER_SIZE = 8192*2;

class NxConnection : public NxAsyncEventHandler, public NxSyncObject
{
	friend class NxConnectionManager;
	NxConnection();
public:

	NxConnection(DWORD nUID, NxConnectionManager* pConnectionManager, NxCompletionPort* pMsgCompletionPort, NxAsyncEventHandler* pMsgHandler);
	virtual	~NxConnection();

	//  Derived virtual function
	virtual	void		HandleEvent(NxAsyncEvent* pEvent);

	bool				Connect(const TCHAR* szIPAddress, unsigned short nPort );
	NxSocket*			GetHandle()		{ return &m_Socket;	}
	const TCHAR*		GetIPAddress()	{ return m_Socket.GetIPAddress(); }
	WORD				GetPort()		{ return m_Socket.GetPort(); }
	DWORD				GetUID()		{ return m_nUID;	}

	void	Create();
	void	Create(SOCKET hSocket, sockaddr_in& Addr);
	void	Close();

	// 패킷 전송
	bool	SendPacket(NxPacket& Packet);
	// 패킷 수신 
	void	OnPacket(NxPacket& Packet);

	void	OnAccept(unsigned short	nPort);
	void	OnConnect();
	void	OnCreate();
	void	OnClose();

	void	OnSend(int nSize);
	void	OnRecv(int nSize);

private:

	// Overlapped 로 recv 요청을 걸어놓음
	bool			_WaitForRecv();

	virtual bool	_DispatchSend(NxAsyncEvent* pEvent);
	virtual bool	_DispatchRecv(NxAsyncEvent* pEvent);

private:

	NxCompletionPort*	 m_pMsgCompletionPort;
	NxAsyncEventHandler* m_pMsgHandler;
	


	NxConnectionManager*	m_pConnectionManager;

	DWORD				m_nUID;			
	NxSocket			m_Socket;

	/// 리시브 버퍼
	char				m_RecvBuffer[RECV_BUFFER_SIZE];
	DWORD				m_nReceivedSize;

	// N-Send, 1-Recv 
	NxAsyncEvent		m_ResultRecv;
	
};