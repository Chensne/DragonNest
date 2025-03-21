#pragma once
#include "NxSession.h"
#include "NxSessionManager.h"

class DnDummyClient;

class DnDummySession : public NxSession
{
public:
	DnDummySession(DWORD nUID, NxServerApp* pServerApp);
	virtual ~DnDummySession();

	void	SetDummyClient(DnDummyClient* pDummyClient) { m_pDummyClient = pDummyClient; }
	// 클라이언트가 접속 했을때 호출된다. 
	virtual void	OnAccept( NxAsyncEvent* pEvent );
	// 서버에 접속 되었을때 호출된다. ( 클라이언트 용 세션을 접속시켰을때 )
	virtual void	OnConnect( NxAsyncEvent* pEvent );
	// 접속 끊김
	virtual void	OnClose();
	// 사용자 정의 이벤트
	virtual void	OnUserEvent( NxAsyncEvent* pEvent );
	// 패킷 수신시
	virtual void	OnPacket( NxPacket& Packet );

	bool IsConnected() { return m_bConnected ; }
private:
	DnDummyClient* m_pDummyClient;
	bool			m_bConnected;
};

class DnDummySessionManager : public NxSessionManager
{
public:

	virtual void _CreateSessionPool(DWORD nSessionCnt);
};