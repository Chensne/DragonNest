#pragma once

class CClientSession;
class CMemoryStream;
class CTask;

#include "Task.h"
//#include "ClientUdpSession.h"
//#include "ClientTcpSession.h"
#include "MessageListener.h"

class CDnBaseTransAction {
public:
	CDnBaseTransAction() : m_DefaultSession( ClientSessionTypeEnum::Unknown ), m_pTask( NULL ) {}
	virtual ~CDnBaseTransAction() {}

protected:
	CTask *m_pTask;
	ClientSessionTypeEnum m_DefaultSession;

public:
	virtual void InitializeSession( CTask *pTask ) { 
		m_pTask = pTask;

		/*bool bTcp = false;
		bool bUdp = false;
		if( dynamic_cast<CClientTcpSession *>(m_pTask) ) bTcp = true;
		if( dynamic_cast<CClientUdpSession *>(m_pTask) ) bUdp = true;
		if( bTcp && !bUdp ) m_DefaultSession = ClientSessionTypeEnum::Tcp;
		else if( !bTcp && bUdp ) m_DefaultSession = ClientSessionTypeEnum::Udp;
		else if( bTcp && bUdp ) m_DefaultSession = ClientSessionTypeEnum::Udp;
		else if( !bTcp && !bUdp ) m_DefaultSession = ClientSessionTypeEnum::Unknown;*/
		if (dynamic_cast<CTaskListener*>(m_pTask))
			m_DefaultSession = dynamic_cast<CTaskListener*>(m_pTask)->GetType() == true ? \
			ClientSessionTypeEnum::Udp : ClientSessionTypeEnum::Tcp;
		else
			m_DefaultSession = ClientSessionTypeEnum::Unknown;
	}
	CTask *GetSession() { return m_pTask; }

	virtual void Send( DWORD dwProtocol, DnActorHandle hActor, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto ) = 0;
	virtual void Send( DWORD dwProtocol, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto, bool bImmediate = true ) = 0;
	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket ) = 0;
};
