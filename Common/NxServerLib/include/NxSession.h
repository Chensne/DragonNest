#pragma once

#include "NxPacket.h"

struct NxAsyncEvent;
class NxServerApp;
class NxSession
{
public:
	NxSession(DWORD nUID, NxServerApp* pServerApp);
	virtual ~NxSession();

	DWORD GetUID()	{ return m_nUID; }

	// 클라이언트가 접속 했을때 호출된다. 
	virtual void	OnAccept( NxAsyncEvent* pEvent ) { }
	// 서버에 접속 되었을때 호출된다. ( 클라이언트 용 세션을 접속시켰을때 )
	virtual void	OnConnect( NxAsyncEvent* pEvent ) { }
	// 접속 끊김
	virtual void	OnClose() { }
	// 사용자 정의 이벤트
	virtual void	OnUserEvent( NxAsyncEvent* pEvent ) { }
	// 패킷 수신시
	virtual void	OnPacket( NxPacket& Packet ) { }
	// 패킷 송신시
	virtual void	SendPacket( NxPacket& Packet );

private:

	NxServerApp*	m_pServerApp;
	DWORD	m_nUID;
};