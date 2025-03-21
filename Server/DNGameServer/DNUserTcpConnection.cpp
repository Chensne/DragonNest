
#include "stdafx.h"
#include "DNUserTcpConnection.h"
#include "DNUserSession.h"
#include "DNGameRoom.h"
#include "DNRUDPGameServer.h"
#include "DNGameServerManager.h"
#include "DNSecure.h"
#include "SpinBuffer.h"

extern TGameConfig g_Config;

CDNTcpConnection::CDNTcpConnection(CDNGameServerManager * pManager)
{
	m_bAttached = false;
	m_bAttachVerify = false;
	m_pSession = NULL;
	m_pManager = pManager;
	Init(1024 * 100, 1024 * 100);
}

CDNTcpConnection::~CDNTcpConnection()
{
	m_bAttached = false;
	m_bAttachVerify = false;
	m_bAttached = false;
	m_pSession = NULL;
	m_pManager = NULL;
}

void CDNTcpConnection::TcpConnected(char * pData, int iLen)
{
	CSConnectGame * pPacket = (CSConnectGame*)pData;
	CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByUID(pPacket->nSessionID);
	if (pServer)
	{
		++pServer->m_uiConnectSuccessCount;

		CDNUserSession * pSession = pServer->GetSession(pPacket->nSessionID);
		if (pSession != NULL)
		{
//			pSession->m_nAccountDBID = pPacket->nAccountDBID;		// 추가 구현 필요 ??? (이렇게 하면 않됨 ???)
			pSession->m_biCertifyingKey = pPacket->biCertifyingKey;
#if defined (_KR)
			memcpy(pSession->m_szMID, pPacket->szMID, sizeof(pSession->m_szMID));
			pSession->m_dwGRC = pPacket->dwGRC;
#endif

#ifdef _SKIP_BLOCK
			if (pSession->GetTcpConnection() != NULL)
			{
				_DANGER_POINT();
				if (this != pSession->GetTcpConnection())
					_ASSERT(0);
			}
			else
			{
				if (pSession->TCPConnected(this))
				{
					if (SetSession(pSession))
					{
						m_bAttachVerify = true;
						return;
					}
					else
						g_Log.Log(LogType::_ERROR, L"Connect|Session Attached Already\n");
				}
				else
					g_Log.Log(LogType::_ERROR, L"Connect|RUDP Connect Notyet\n");
			}
#else
			if (pSession->GetTcpConnection() != NULL)	
			{
				//일단 나오면 안데요호~!
				_DANGER_POINT();
				if (this != pSession->GetTcpConnection())
					m_pManager->PushOrphanPtr(pSession->GetTcpConnection());	//혹시나 나올경우를 대비해서 처리됩니다.
				else
					_ASSERT(0);				//같아 버리면..........머냐......이러면 복잡.................--;
			}

			if (pSession->TCPConnected(this) == true)
			{
				//타이밍처리
				//m_pSession = pSession;
				g_pGameServerManager->PushToEjectTcpConnection(this, pSession);
			}
			else
				g_Log.Log(LogType::_ERROR, L"Connect|RUDP Connect Fail\n");
#endif
		}
		else
		{
			g_Log.Log( LogType::_GAMECONNECTLOG, 0, pPacket->nAccountDBID, 0, pPacket->nSessionID, L"pSession Not NULL!!\n" );
		}
	}
	else
	{
		g_Log.Log( LogType::_GAMECONNECTLOG, 0, pPacket->nAccountDBID, 0, pPacket->nSessionID, L"pServer NULL!!\n" );
	}
}

void CDNTcpConnection::TcpDisconnected(wchar_t *pIdent)
{
	if (m_pSession)
	{
		g_Log.Log(LogType::_NORMAL, m_pSession, L"TcpDisconnected(%s) SessionState=%d RoomState=%d\r\n", 
			pIdent, m_pSession->GetState(), m_pSession->GetGameRoom() ? m_pSession->GetGameRoom()->GetRoomState() : -1);
	}

	DNEncryptPacketSeq EnPacket = { 0, };
	EnPacket.Packet.iLen = static_cast<unsigned short>(sizeof(DNTPacketHeader));
	EnPacket.Packet.cMainCmd = IN_DISCONNECT;
	EnPacket.Packet.cSubCmd = CONNECTIONKEY_USER;

	EnPacket.nLen = EnPacket.Packet.iLen + sizeof(BYTE) + sizeof(USHORT);
	CDNSecure::GetInstance().Tea_encrypt( reinterpret_cast<char*>(&EnPacket.Packet), EnPacket.Packet.iLen );

	AddRecvData( EnPacket );
}

bool CDNTcpConnection::SetSession(CDNUserSession * pSession)
{
	if (m_pSession)	return false;
	m_pSession = pSession;
	return true;
}

int CDNTcpConnection::MessageProcess(int iMainCmd, int iSubCmd, char * pData, int iSize)
{
	if (m_pSession)
	{
#ifdef PRE_MOD_INDUCE_TCPCONNECT
#else		//#ifdef PRE_MOD_INDUCE_TCPCONNECT
		if (iMainCmd == CS_SYSTEM && iSubCmd == eSystem::CS_CONNECTGAME)
		{
			g_Log.Log( LogType::_GAMECONNECTLOG, L"MessageProcess m_pSession()!!\n" );
		}
#endif		//#ifdef PRE_MOD_INDUCE_TCPCONNECT

		if (m_pSession->GetGameRoom())
			m_pSession->GetGameRoom()->OnDispatchMessage(m_pSession, iMainCmd, iSubCmd, pData, iSize, 0);
	}
	else if (iMainCmd == CS_SYSTEM && iSubCmd == eSystem::CS_CONNECTGAME)
	{
		CSConnectGame * pPacket = (CSConnectGame*)pData;
		g_Log.Log( LogType::_GAMECONNECTLOG, 0, pPacket->nAccountDBID, 0, pPacket->nSessionID, L"[%d] CS_CONNECTGAME()!!\n", g_Config.nManagedID );
		TcpConnected(pData, iSize);
	}
	else
		_DANGER_POINT();
	return ERROR_NONE;
}

void CDNTcpConnection::SetAttachSession()
{
	if (m_bAttached)
	{
		_DANGER_POINT();
		return;
	}
	
	m_bAttached = true;
}

