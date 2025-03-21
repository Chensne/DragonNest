#include "StdAfx.h"
#include "DNConnection.h"
#include "DNSQLConnection.h"
#include "DNSQLConnectionManager.h"
#include "Log.h"
#if defined(PRE_MODIFY_CONNECTIONSYNC_01)
#include "ConnectionManager.h"
#endif	// #if defined(PRE_MODIFY_CONNECTIONSYNC_01)

CDNConnection::CDNConnection(void): CConnection()
{
#if defined(_WORK)
	Init(100 * 1024, 100 * 1024); 
#else		// #if defined(_WORK)
	Init(1024 * 1024, 1024 * 1024); 
#endif		// #if defined(_WORK)
	m_dwLiveTick = GetTickCount();
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	m_dwLastRecvTick = m_dwLiveTick;
	m_dwSendSYNTick = 0;
	m_dwSendSYNACKTick = 0;
#endif
}

CDNConnection::~CDNConnection(void)
{
}

#if defined(PRE_MODIFY_CONNECTIONSYNC_01)
void CDNConnection::Release()
{
	DN_ASSERT(m_Access.IsOpen(),			"Not Opened!");
//	DN_ASSERT(IsAttach(),					"Invalid!");
	DN_ASSERT(NULL != m_pConPtrSafeMngr,	"Invalid!");

	const TP_CONIDX nConIdx = m_pConPtrSafeMngr->Unregister(GetConIdx());
	if (0 != nConIdx) {
		// 자신의 인덱스를 제거 성공한 후에만 삭제

		delete this;
	}
}
#endif	// #if defined(PRE_MODIFY_CONNECTIONSYNC_01)

void CDNConnection::DoUpdate(DWORD CurTick)
{
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	if( !GetActive() || GetConnecting() )
		return;

   	if( m_dwSendSYNTick > 0 && m_dwSendSYNTick + 1000*60*2 < CurTick ||
   		m_dwSendSYNACKTick > 0 && m_dwSendSYNACKTick + 1000*60*2 < CurTick )	//딱 2분만 기다림.
	{
		g_Log.Log(LogType::_FILELOG, L"[LogServer Disconnected]Heart failure!!\r\n");
		m_dwSendSYNTick = 0;
		m_dwSendSYNACKTick = 0;
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		CScopeInterlocked Scope( &m_pSocketContext->m_lActiveCount );
		if( Scope.bIsDelete() )
			return;
#endif
		//DetachReason 만 설정하고 소켓 클로즈 -> 실제 Detach 과정은 GetQueuedCompletetionStatus 에서 처리 하도록합니다.
		m_pSocketContext->SetDetachReason(L"[LogServer]Heart failure!\r\n");
		m_pIocpManager->DelSocket(m_pSocketContext);
		return;
	}

	if( m_dwSendSYNTick == 0 && m_dwLastRecvTick + CHECK_TCP_PING_TICK < CurTick )
	{
		m_dwSendSYNTick = CurTick;
		AddSendData(LOG_HEARTBEAT, SYN, NULL, 0);
	}

#else
	if( m_dwLiveTick + CHECKLIVETICK <= CurTick) // 2분?
	{	
		SendLiveCheck();
		m_dwLiveTick = CurTick;
	}
#endif
}

void CDNConnection::SendLiveCheck()
{
	AddSendData(0, 0, NULL, 0);
}

int CDNConnection::MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	return 0;
}

void CDNConnection::DBMessageProcess(char *pData, int nThreadID)
{
	DNTPacket *pPacket = (DNTPacket*)pData;
	CDNSQLConnection *pMasterLogDB = NULL, *pLogDB = NULL;
	int nRet = 0;

	switch(pPacket->cMainCmd)
	{
		case LOG_FILE:
		{
			TLogFile* pLog = reinterpret_cast<TLogFile*>(pPacket->buf);

			CDNSQLConnection* pServerpLogDB = g_SQLConnectionManager.FindServerLogDB( nThreadID );
			if( pServerpLogDB )
			{
				int nRet = pServerpLogDB->QueryLog( pLog );
			}
			else
			{
				_DANGER_POINT();
			}

			break;
		}
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
		case LOG_HEARTBEAT:
		{
			switch(pPacket->cSubCmd)
			{
			case SYN:
				{				
					m_dwSendSYNACKTick = timeGetTime();
					AddSendData(LOG_HEARTBEAT, SYN_ACK, NULL, 0);
				}
				break;
			case SYN_ACK:
				{
					m_dwSendSYNTick = 0;
					AddSendData(LOG_HEARTBEAT, ACK, NULL, 0);
				}
				break;
			case ACK:
				{
					m_dwSendSYNACKTick = 0;
				}
				break;
			}
		}
		break;		
#endif
	}
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	m_dwLastRecvTick = timeGetTime();
#endif
}
