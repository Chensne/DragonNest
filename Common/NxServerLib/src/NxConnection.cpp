#include "stdafx.h"
#include "NxConnection.h"
#include "NxConnectionManager.h"
#include "NxServerApp.h"
#include "NxCompletionPort.h"

NxConnection::NxConnection(DWORD nUID, NxConnectionManager* pSessionManager, NxCompletionPort* pMsgCompletionPort, NxAsyncEventHandler* pMsgHandler)
{
	m_pConnectionManager = pSessionManager;
	m_pMsgCompletionPort = pMsgCompletionPort;
	m_pMsgHandler = pMsgHandler;

	m_nReceivedSize = 0;
	m_nUID			= nUID;
}

NxConnection::~NxConnection()
{
	Close();
}

void 
NxConnection::Create()
{
	NxGuard<NxConnection>	Guard(*this);
	m_Socket.Create( true );
}

void
NxConnection::Create(SOCKET hSocket, sockaddr_in& Addr)
{
	NxGuard<NxConnection>	Guard(*this);
	m_Socket.Attach( hSocket, &Addr );
}

void 
NxConnection::Close()
{
	NxGuard<NxConnection>	Guard(*this);
	m_Socket.Close();
}

bool
NxConnection::Connect(const TCHAR* szIPAddress, unsigned short nPort )
{
	NxGuard<NxConnection>	Guard(*this);
    m_Socket.Create();
	return m_Socket.Connect(szIPAddress, nPort);
}
void
NxConnection::OnAccept(unsigned short	nPort)
{
	NxGuard<NxConnection>	Guard(*this);
	NxAsyncEvent* pEvent = NxAsyncEventPool::Alloc();
	pEvent->Clear();
	pEvent->nEventType = NxAsyncEventType::Event_Accept;
	pEvent->pHandler   = m_pMsgHandler;
	pEvent->nUID	   = m_nUID;
	m_pMsgCompletionPort->PostCompletion(m_pMsgHandler, pEvent );
}

void	
NxConnection::OnConnect()
{
	NxGuard<NxConnection>	Guard(*this);
	NxAsyncEvent* pEvent = NxAsyncEventPool::Alloc();
	pEvent->Clear();
	pEvent->nEventType = NxAsyncEventType::Event_Connect;
	pEvent->pHandler   = m_pMsgHandler;
	pEvent->nUID	   = m_nUID;
	m_pMsgCompletionPort->PostCompletion(m_pMsgHandler, pEvent );
}

void
NxConnection::OnCreate()
{
	NxGuard<NxConnection>	Guard(*this);
	_WaitForRecv();

}

void	
NxConnection::OnClose()
{
	NxGuard<NxConnection>	Guard(*this);
	Close();

	//NxAsyncEvent* pEvent = new NxAsyncEvent;
	NxAsyncEvent* pEvent = NxAsyncEventPool::Alloc();
	pEvent->Clear();
	pEvent->nEventType = NxAsyncEventType::Event_Close;
	pEvent->pHandler   = m_pMsgHandler;
	pEvent->nUID	   = m_nUID;
	m_pMsgCompletionPort->PostCompletion(m_pMsgHandler, pEvent );
	
}

void
NxConnection::OnSend(int nSize)
{
	NxGuard<NxConnection>	Guard(*this);
	NxServerApp::AddSendBytes(nSize);
}

void
NxConnection::OnRecv(int nSize)
{
	NxGuard<NxConnection>	Guard(*this);
	NxServerApp::AddRecvBytes(nSize);
}



void
NxConnection::HandleEvent( NxAsyncEvent* pEvent )
{
	NxGuard<NxConnection>	Guard(*this);
	switch( pEvent->nEventType )
	{
	case NxAsyncEventType::Event_Send:
		{
			if ( pEvent->nTransBytes == 0 )
				return;

			OnSend(pEvent->nTransBytes);

			// 송신 오류면 끊어버린다.
			if ( _DispatchSend(pEvent) == false )
			{
				m_pConnectionManager->InactiveConnection(this);
				return;
			}

			NxServerApp::AddSendCount();
		}
		break;

	case NxAsyncEventType::Event_Receive:
		{
			if ( pEvent->nTransBytes == 0 )				return;

			OnRecv(pEvent->nTransBytes);
			_DispatchRecv(pEvent);

			// recv 걸었는데 오류면 끊어버린다.
			if ( _WaitForRecv() == false )
			{
				m_pConnectionManager->InactiveConnection(this);
				return;
			}
			NxServerApp::AddRecvCount();
		}
		break;

	case NxAsyncEventType::Event_Close:
		{
			if ( m_pConnectionManager )
			{
				m_pConnectionManager->InactiveConnection(this);
			}
		}
		break;

	}
}

bool
NxConnection::_WaitForRecv()
{
	NxGuard<NxConnection>	Guard(*this);
	::memset( &m_ResultRecv, 0, sizeof(NxAsyncEvent) );

	m_ResultRecv.pHandler	= this;
	m_ResultRecv.nEventType	= NxAsyncEventType::Event_Receive;

	DWORD dwBufSize = PACKETBUFFERSIZE - m_nReceivedSize;
	// 리시브 요청을 걸어 놓는다.
	m_Socket.Recv( &(m_RecvBuffer[m_nReceivedSize]), dwBufSize, &m_ResultRecv );

	if( m_ResultRecv.nError != ERROR_SUCCESS && m_ResultRecv.nError != ERROR_IO_PENDING )
		return false;

	return true;
	
}

bool
NxConnection::SendPacket( NxPacket& Packet )
{
	NxGuard<NxConnection>	Guard(*this);
	if ( (SOCKET)m_Socket.GetNativeHandle() == INVALID_SOCKET )
		return false;

	// 걍 복사해서 사용하자.
	NxPacket* pNewPacket = NxPacket::Alloc();
	if ( pNewPacket == NULL )
	{
		m_pConnectionManager->InactiveConnection(this);
		return false;
	}

	*pNewPacket = Packet;

	// Pool 에서 하나 할당 받는다. 
	NxAsyncEvent* pSendEvent = NxAsyncEventPool::Alloc();

	/// 바로 전송한다. WSASend 를 건다.
	pSendEvent->Clear();

	pSendEvent->pHandler	= this;
	pSendEvent->nEventType	= NxAsyncEventType::Event_Send;
	pSendEvent->pPacket     = (void*)pNewPacket;

	// 전송 요청을 걸어 놓는다.
	m_Socket.Send( pNewPacket->GetPacketBuffer(), pNewPacket->GetPacketSize(), pSendEvent );

	if( pSendEvent->nError != ERROR_SUCCESS && pSendEvent->nError != ERROR_IO_PENDING )
	{
		return false;
	}

	return true;
}

bool	
NxConnection::_DispatchSend(NxAsyncEvent* pEvent)
{
	NxGuard<NxConnection>	Guard(*this);
	if ( (SOCKET)m_Socket.GetNativeHandle() == INVALID_SOCKET )
		return false;

	NxPacket* pSentPacket = (NxPacket*)pEvent->pPacket;
	if ( pEvent->nTransBytes == pSentPacket->GetPacketSize() )
	{
		// overlapped 와 패킷을 풀에 반납한다.
		//delete pEvent;
		NxAsyncEventPool::Free(pEvent);
		NxPacket::Free(pSentPacket);
		return true;
	}

	// 과연..? 한번에 전송 안되는 경우가 있을까?
	else
	{
		return false;
	}

	return false;
}


bool	
NxConnection::_DispatchRecv(NxAsyncEvent* pEvent)
{
	NxGuard<NxConnection>	Guard(*this);
	NxPacket ReceivedPacket;

	m_nReceivedSize += pEvent->nTransBytes;

	while( m_nReceivedSize > 0 )
	{
		ReceivedPacket.CopyToBuffer( m_RecvBuffer, m_nReceivedSize );

		if( ReceivedPacket.IsValidPacket() == true &&
			m_nReceivedSize >= ( int )ReceivedPacket.GetPacketSize() )
		{
			//  Packet parsing
			OnPacket(ReceivedPacket);

			char buffer[RECV_BUFFER_SIZE] = "";

			m_nReceivedSize -= ReceivedPacket.GetPacketSize();

			if ( m_nReceivedSize > 0 )
			{
				::CopyMemory( buffer, ( m_RecvBuffer + ReceivedPacket.GetPacketSize() ), m_nReceivedSize );
				::CopyMemory( m_RecvBuffer, buffer, m_nReceivedSize );
			}
		}
		else
		{
			break;
		}
	}

	return true;
}

/// 패킷 수신 
void	
NxConnection::OnPacket(NxPacket& Packet)
{
	NxGuard<NxConnection>	Guard(*this);
	// 패킷을 복사해서 사용한다.
	NxPacket* pPacket = NxPacket::Alloc();
	*pPacket = Packet;

	NxAsyncEvent* pEvent = NxAsyncEventPool::Alloc();
	pEvent->Clear();
	pEvent->nEventType = NxAsyncEventType::Event_Packet;

	pEvent->pHandler   = m_pMsgHandler;
	pEvent->pPacket    = pPacket;
	pEvent->nUID	   = m_nUID;
	m_pMsgCompletionPort->PostCompletion(m_pMsgHandler, pEvent );

}
