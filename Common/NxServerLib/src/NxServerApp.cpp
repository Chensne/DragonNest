#include "stdafx.h"
#include "NxServerApp.h"

#include "NxNetInit.h"
#include "NxConnection.h"
#include "NxConnectionManager.h"
#include "NxSession.h"
#include "NxSessionManager.h"
#include "NxCompletionPort.h"
#include "NxAcceptor.h"


DWORD NxServerApp::m_nTotalSendBytes = 0;
DWORD NxServerApp::m_nTotalRecvBytes = 0;

DWORD NxServerApp::m_nTotalSendCnt = 0;
DWORD NxServerApp::m_nTotalRecvCnt = 0;


NxServerApp::NxServerApp()
{
	m_pAcceptor = NULL;
	m_pNetIOCompletionPort = NULL;
	m_pMsgCompletionPort = NULL;
	m_pConnectionManager = NULL;
	m_pSessionManager = NULL;
	m_pNetInit = NULL;
//	m_pTimerThread = NULL;
}

NxServerApp::~NxServerApp()
{
	Destroy();

}


bool
NxServerApp::Create( int nWorkerTheadCnt, int nConnectionPoolCnt , DWORD nTimerValue)
{


	m_pNetInit = new NxNetInit();
	m_pNetInit->StartupWinSock();

	// Network IO Completion Port 초기화
	m_pNetIOCompletionPort = new NxCompletionPort();
	m_pNetIOCompletionPort->Create(nWorkerTheadCnt, "Worker");

	// 로직용은 쓰레드 1개
	m_pMsgCompletionPort = new NxCompletionPort();
	m_pMsgCompletionPort->Create(1, "Reactor");

	// 커넥션 매니저 생성
	m_pConnectionManager = new NxConnectionManager();
	m_pConnectionManager->Create(nConnectionPoolCnt, m_pNetIOCompletionPort, m_pMsgCompletionPort, this);

	// 세션  매니저 생성
	CreateSessionManager();
	m_pSessionManager->Create(nConnectionPoolCnt, this);

	// 타이머가 필요하면 돌린다.
	if ( nTimerValue )
	{
		//m_pTimerThread = new NxTimerThread(m_pMsgCompletionPort, this, nTimerValue);
		//m_pTimerThread->Start();
	}
	
	return true;
}

bool
NxServerApp::BeginAcceptor(const TCHAR* szIPAddress, unsigned short nPort)
{
	// 어셉터 쓰레드 시작
	m_pAcceptor = new NxAcceptor(m_pConnectionManager);
	if ( m_pAcceptor->Open(szIPAddress, nPort) == false )
	{
		NxGetLastError::Trace();
		return false;
	}

	return true;
}


void 
NxServerApp::Destroy()
{

	SAFE_DELETE(m_pAcceptor);
//	SAFE_DELETE(m_pTimerThread);
	SAFE_DELETE(m_pNetIOCompletionPort);
	SAFE_DELETE(m_pMsgCompletionPort);
	SAFE_DELETE(m_pSessionManager);
	SAFE_DELETE(m_pConnectionManager);



	// 윈속 클린업
	m_pNetInit->CleanupWinSock();
	SAFE_DELETE(m_pNetInit);
}

void
NxServerApp::HandleEvent( NxAsyncEvent* pEvent )
{
	switch( pEvent->nEventType )
	{

	case NxAsyncEventType::Event_Accept:
		{
			OnAccept(pEvent->nUID, pEvent);
		}
		break;
	case NxAsyncEventType::Event_Connect:
		{
			OnConnect(pEvent->nUID, pEvent);
		}
		break;
	case NxAsyncEventType::Event_Close:
		{
			OnClose(pEvent->nUID);
		}
		break;
	case NxAsyncEventType::Event_UserEvent:
		{
			OnUserEvent(pEvent->nUID, pEvent);
		}
		break;
	case NxAsyncEventType::Event_Packet:
		{
			NxPacket* pPacket = (NxPacket*)pEvent->pPacket;
			OnPacket(pEvent->nUID, *pPacket);
			NxPacket::Free(pPacket);
		}
		break;
	case NxAsyncEventType::Event_Timer:
		{
			DWORD nTime = timeGetTime();
			DWORD dt =  nTime - m_nLastTime;

			OnTimer(dt);
			m_nLastTime = nTime;

		}
		break;
	}

	//delete pEvent;
	NxAsyncEventPool::Free(pEvent);
}

// 클라이언트가 접속 했을때 호출된다. 
void
NxServerApp::OnAccept( DWORD nUID, NxAsyncEvent* pEvent )
{
	if ( !m_pSessionManager ) return;
	NxSession* pSession = m_pSessionManager->ActiveSession(nUID);
	pSession->OnAccept(pEvent);

}

// 서버에 접속 되었을때 호출된다. ( 클라이언트 용 세션을 접속시켰을때 )
void
NxServerApp::OnConnect( DWORD nUID, NxAsyncEvent* pEvent )
{
	if ( !m_pSessionManager ) return;
	NxSession* pSession = m_pSessionManager->Find(nUID);
	pSession->OnConnect(pEvent);
}

// 접속 끊김
void	
NxServerApp::OnClose( DWORD nUID )
{
	if ( !m_pSessionManager ) return;
	NxSession* pSession = m_pSessionManager->Find(nUID);
	pSession->OnClose();

}

// 사용자 정의 이벤트
void
NxServerApp::OnUserEvent( DWORD nUID, NxAsyncEvent* pEvent )
{
	if ( !m_pSessionManager ) return;
	NxSession* pSession = m_pSessionManager->Find(nUID);
	pSession->OnUserEvent(pEvent);

}

// 패킷 수신시
void	
NxServerApp::OnPacket( DWORD nUID, NxPacket& Packet )
{
	if ( !m_pSessionManager ) return;
	NxSession* pSession = m_pSessionManager->Find(nUID);
	pSession->OnPacket(Packet);

	AddRecvCount();
	AddRecvBytes(Packet.GetPacketSize());

}


void
NxServerApp::SendPacket( DWORD nUID, NxPacket& Packet )
{
	NxConnection* pConnection = m_pConnectionManager->Find(nUID);
	if ( pConnection )
	{
		pConnection->SendPacket(Packet);
		AddSendCount();
		AddSendBytes(Packet.GetPacketSize());
	}
}


void
NxServerApp::GetLocalIpPort( DWORD nUID, OUT tstring& szIP, OUT WORD& nPort )
{
	NxConnection* pConnection = m_pConnectionManager->Find(nUID);
	if ( pConnection )
	{
		szIP = pConnection->GetIPAddress();
		nPort = pConnection->GetPort();
	}
}


bool
NxServerApp::CreateSessionManager()
{
	m_pSessionManager = new NxSessionManager();
	return true;
}

NxSession*
NxServerApp::OpenSession(const TCHAR* szIPAddress, unsigned short nPort)
{
	if ( m_pConnectionManager )
	{
		NxConnection* pConnection = m_pConnectionManager->ActiveConnection(szIPAddress, nPort);
		if ( pConnection )
		{
			return m_pSessionManager->ActiveSession(pConnection->GetUID());
			
		}
	}
	return NULL;
}

bool
NxServerApp::CloseSession(DWORD nUID)
{
	if ( m_pConnectionManager && m_pSessionManager )
	{
		return m_pConnectionManager->InactiveConnection(nUID);
	}
	return false;
}


