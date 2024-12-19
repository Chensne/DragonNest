#pragma once

#include "NxAsyncEvent.h"
#include "NxPacket.h"

class NxNetInit;
class NxAcceptor;
class NxCompletionPort;
class NxConnection;
class NxConnectionManager;
class NxSession;
class NxSessionManager;
class NxTimerThread;


class NxServerApp : public NxAsyncEventHandler
{
public:
	NxServerApp();
	virtual ~NxServerApp();

	// 서버 시작
	virtual bool	Create( int nWorkerTheadCnt, int nSessionPoolCnt, DWORD nTimerValue );

	virtual bool	BeginAcceptor(const TCHAR* szIPAddress, unsigned short nPort);
	virtual void	Destroy();

	// 세션 프록시 매니저를 생성한다. 재정의 해야됨
	virtual bool	CreateSessionManager();

	virtual NxSession* OpenSession(const TCHAR* szIPAddress, unsigned short nPort);
	virtual bool	CloseSession(DWORD nUID);

	void	GetLocalIpPort( DWORD nUID, OUT tstring& szIP, OUT WORD& nPort );

public:
	virtual	void	HandleEvent( NxAsyncEvent* pEvent );

	// 클라이언트가 접속 했을때 호출된다. 
	virtual void	OnAccept( DWORD nUID, NxAsyncEvent* pEvent );
	// 서버에 접속 되었을때 호출된다. ( 클라이언트 용 세션을 접속시켰을때 )
	virtual void	OnConnect( DWORD nUID, NxAsyncEvent* pEvent );
	// 접속 끊김
	virtual void	OnClose( DWORD nUID );
	// 사용자 정의 이벤트
	virtual void	OnUserEvent( DWORD nUID, NxAsyncEvent* pEvent );
	// 패킷 수신시
	virtual void	OnPacket( DWORD nUID, NxPacket& Packet );
	// 패킷 송신시
	virtual void	SendPacket( DWORD nUID, NxPacket& Packet );
	// 타이머
	virtual void	OnTimer(DWORD dt) { }
	
public:

	static void		AddSendBytes(DWORD nBytes) { m_nTotalSendBytes += nBytes; }
	static void		AddRecvBytes(DWORD nBytes) { m_nTotalRecvBytes += nBytes; }
	static void		AddSendCount() { ++m_nTotalSendCnt; }
	static void		AddRecvCount() { ++m_nTotalRecvCnt; }
	static void		ClearIOInfo() { m_nTotalSendBytes = m_nTotalRecvBytes = m_nTotalSendCnt = m_nTotalRecvCnt = 0; }

public:

	static DWORD	m_nTotalSendBytes;
	static DWORD	m_nTotalRecvBytes;

	static DWORD	m_nTotalSendCnt;
	static DWORD	m_nTotalRecvCnt;

private:

	NxNetInit*				m_pNetInit;
	NxAcceptor*				m_pAcceptor;

	// IO 용 CP
	NxCompletionPort*		m_pNetIOCompletionPort;
	// Msg 처리용 CP
	NxCompletionPort*		m_pMsgCompletionPort;
	// 커넥션 매니저
	NxConnectionManager*	m_pConnectionManager;

	// 타이머 쓰레드
	//NxTimerThread*			m_pTimerThread;
	// 마지막 타임
	DWORD					m_nLastTime;

protected:
	// 세션  매니저
	NxSessionManager*		m_pSessionManager;


};