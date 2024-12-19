#pragma once

#include <vector>
#include "NxPacket.h"

class NxClientSession
{
public:
	enum NxClientSessionType
	{
		NX_TCP = 0,
		NX_UDP = 1
	};

	NxClientSession();	
	virtual ~NxClientSession();

	/// 클라이언트용 소켓 생성
	bool			Create(NxClientSessionType nSocketType);
	bool			Bind(const TCHAR* szIPAddress, unsigned int nPortNo );

	/// 접속 / 해제
	void			ConnectTo(const TCHAR* szIPAddress, unsigned int nPort );
	void			Close();

	/// TCP / UDP 전송
	int				Send(NxPacket& Packet );
	int				SendTo(NxPacket& Packet, sockaddr_in* pToAddr);

	/// TCP / UDP 수신 - 재정의 해야함.
	virtual void	OnPacket(NxPacket& Packet )  { }
	virtual void	OnPacket(NxPacket& Packet, sockaddr_in* pFromAddr )  { }

protected:
	/// 이벤트 발생시 호출됨.
	virtual void	OnCreate() { }
	virtual void	OnDestroy() { }
	virtual void	OnConnect(DWORD nError) { }
	virtual void	OnClose() { }
	virtual void	OnReceive();
	virtual void	OnSend() { }

protected:
	typedef	sockaddr_in	SocketAddr;

	NxClientSessionType	m_SocketType;
	SOCKET				m_Socket;
	TCHAR				m_szIPAddress[20];
	SocketAddr			m_SockAddr;
	char				m_RecvBuffer[PACKETBUFFERSIZE];
	int					m_RecvPacketSize;
	WSAEVENT			m_SessionEvent;

public:

	/// 활성화된 소켓 입출력을 프로세싱 한다. 메인루프에서 호출해줘야한다. 
	static  void	UpdateClientSession();
	static	void	TraceGetLastError();

private:
	
	void			_SelectEvent();
	void			_RecvPacket_TCP();
	void			_RecvPacket_UDP();

	void			_AddClientSession(NxClientSession* pClientSession);
	bool			_RemoveClientSession(NxClientSession* pClientSession);

private:

	typedef  std::vector<NxClientSession*>  _ClientSessionList;
	typedef  _ClientSessionList::iterator	_ClientSessionListIt;
	
	static _ClientSessionList ms_SessionList;

};
