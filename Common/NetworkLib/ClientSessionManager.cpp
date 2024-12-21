#include "StdAfx.h"
#include <MMSystem.h>
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "ClientTcpSession.h"
#include "ClientUdpSession.h"
#include "NetSocketRUDP.h"
#include "NetSocketUDP.h"
#include "DNPacket.h"
#include "DNProtocol.h"
#include "MessageListener.h"
#include "Log.h"
#include "DNProtocol.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CSeqReceiver::CSeqReceiver()
{
	ResetSeq();
}

CSeqReceiver::~CSeqReceiver()
{
	m_RecvSync.Lock();
	m_ProcessSync.Lock();

	std::list <_SEQ_DATA>::iterator ii;	
	for (ii = m_pRecvList.begin(); ii != m_pRecvList.end(); ii++)
		SAFE_DELETEA((*ii).pBuf);
	m_pRecvList.clear();	
	m_pProcessList.clear();

	m_ProcessSync.UnLock();
	m_RecvSync.UnLock();
}

void CSeqReceiver::RecvData(DNEncryptPacketSeq * pPacket)
{
	//from tcp
	if (pPacket->cSeq == UNSEQUENCEPACKET || m_cDataSeq == pPacket->cSeq)
	{
		m_RecvSync.Lock();
		if (pPacket->cSeq != 0) m_cDataSeq += 2;
		m_RecvSync.UnLock();
		AddProcessData(pPacket->Packet.cMainCmd, pPacket->Packet.cSubCmd, pPacket->Packet.buf, pPacket->Packet.iLen, _SEQDATA_TYPE_DIRECT);
	}
	else
		RecvSeqData(pPacket->cSeq, &pPacket->Packet);

	CheckSeqData();
}

void CSeqReceiver::RecvData(DNTCompPacket * pPacket)
{
	//from tcp compress packet
	if (pPacket->header.cSeq == UNSEQUENCEPACKET || m_cDataSeq == pPacket->header.cSeq)
	{
		m_RecvSync.Lock();
		if (pPacket->header.cSeq != 0) m_cDataSeq += 2;
		m_RecvSync.UnLock();
		AddProcessData(pPacket->header.cMainCmd, pPacket->header.cSubCmd, pPacket->buf, pPacket->header.nLen, _SEQDATA_TYPE_DIRECT);
	}
	else
		RecvSeqData(pPacket->header.cSeq, pPacket->header.cMainCmd, pPacket->header.cSubCmd, pPacket->buf, pPacket->header.nLen);

	CheckSeqData();
}

void CSeqReceiver::RecvData(DNGAME_PACKET * pPacket)
{
	//from udp
	if (pPacket->seq == UNSEQUENCEPACKET || m_cDataSeq == pPacket->seq)
	{
		m_RecvSync.Lock();
		if (pPacket->seq != 0) m_cDataSeq += 2;
		m_RecvSync.UnLock();
		AddProcessData(pPacket->header, pPacket->sub_header, pPacket->data, pPacket->datasize, _SEQDATA_TYPE_DIRECT);
	}
	else
		RecvSeqData(pPacket->seq, pPacket->header, pPacket->sub_header, pPacket->data, pPacket->datasize);

	CheckSeqData();
}

void CSeqReceiver::SkipProcess()
{
	InterlockedExchange(&m_nSkipProcess, SkipFlag);
}

void CSeqReceiver::AddProcessData(BYTE cMainCmd, BYTE cSubCmd, char * pData, int nSize, int nSeqDataType)
{
	_SEQ_DATA Seq;
	memset(&Seq, 0, sizeof(Seq));
	
	Seq.nLen = nSize;
	Seq.cMainCmd = cMainCmd;
	Seq.cSubCmd = cSubCmd;

	switch (nSeqDataType)
	{
		case _SEQDATA_TYPE_DIRECT:
		{
			if( Seq.nLen > 0 ) {
				Seq.pBuf = new char[Seq.nLen];
				memcpy(Seq.pBuf, pData, Seq.nLen);			
			}
			else {
				Seq.pBuf = NULL;
			}
			
			m_ProcessSync.Lock();
			m_pProcessList.push_back(Seq);
			m_ProcessSync.UnLock();
			break;
		}

		case _SEQDATA_TYPE_SEQ:
		{
			Seq.pBuf = pData;

			m_ProcessSync.Lock();
			m_pProcessList.push_back(Seq);
			m_ProcessSync.UnLock();
			break;
		}

		case _SEQDATA_TYPE_RECURSIVE:
		{
			if( Seq.nLen > 0 ) {
				Seq.pBuf = new char[Seq.nLen];
				memcpy(Seq.pBuf, pData, Seq.nLen);			
			}
			else {
				Seq.pBuf = NULL;
			}
			
			m_pProcessList.push_front(Seq);
			break;
		}
	}
}

void CSeqReceiver::ClearProcessData()
{
	{
		ScopeLock <CSyncLock> sync(m_RecvSync);
		for (std::list <_SEQ_DATA>::iterator ii = m_pRecvList.begin(); ii != m_pRecvList.end(); ii++)
			SAFE_DELETEA((*ii).pBuf);
		m_pRecvList.clear();
	}

	{
		ScopeLock <CSyncLock> sync(m_ProcessSync);
		for (std::list <_SEQ_DATA>::iterator ii = m_pProcessList.begin(); ii != m_pProcessList.end(); ii++)
			SAFE_DELETEA((*ii).pBuf);
		m_pProcessList.clear();
	}
}

void CSeqReceiver::ResetSeq()
{
	m_cDataSeq = 3;
	InterlockedExchange(&m_nSkipProcess, 0);
}

void CSeqReceiver::RecvSeqData(BYTE cSeq, DNTPacket * pData)
{
	_SEQ_DATA Seq;
	memset(&Seq, 0, sizeof(Seq));

	Seq.cSeq = cSeq;
	Seq.nLen = pData->iLen;
	Seq.cMainCmd = pData->cMainCmd;
	Seq.cSubCmd = pData->cSubCmd;

	Seq.pBuf = new char[Seq.nLen];
	memcpy(Seq.pBuf, pData->buf, Seq.nLen);

	m_RecvSync.Lock();
	m_pRecvList.push_back(Seq);
	m_RecvSync.UnLock();
}

void CSeqReceiver::RecvSeqData(BYTE cSeq, BYTE cMainCmd, BYTE cSubCmd, char * pData, int nSize)
{
	_SEQ_DATA Seq;
	memset(&Seq, 0, sizeof(Seq));

	Seq.cSeq = cSeq;
	Seq.nLen = nSize;
	Seq.cMainCmd = cMainCmd;
	Seq.cSubCmd = cSubCmd;

	Seq.pBuf= new char[Seq.nLen];
	memcpy(Seq.pBuf, pData, Seq.nLen);

	m_RecvSync.Lock();
	m_pRecvList.push_back(Seq);
	m_RecvSync.UnLock();
}

void CSeqReceiver::CheckSeqData()
{
	m_RecvSync.Lock();

	std::list <_SEQ_DATA>::iterator ii;
	for (ii = m_pRecvList.begin(); ii != m_pRecvList.end(); )
	{
		_SEQ_DATA * pSeq = &(*ii);
		if (m_cDataSeq == pSeq->cSeq)
		{
			m_cDataSeq += 2;
			AddProcessData(pSeq->cMainCmd, pSeq->cSubCmd, pSeq->pBuf, pSeq->nLen, _SEQDATA_TYPE_SEQ);
			ii = m_pRecvList.erase(ii);
		}
		else	ii++;
	}

	m_RecvSync.UnLock();
}

#if defined( PRE_TEST_PACKETMODULE )
CClientSessionManager::CClientSessionManager( CClientSocket* pTcpSocket/*=NULL*/)
#else
CClientSessionManager::CClientSessionManager()
#endif // #if defined( PRE_TEST_PACKETMODULE )
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	
#if defined( PRE_TEST_PACKETMODULE )
	if( pTcpSocket )
		m_pTcpSocket = pTcpSocket;
	else
		m_pTcpSocket = new CClientSocket;
#else
	m_pTcpSocket = new CClientSocket;
#endif // #if defined( PRE_TEST_PACKETMODULE )
	m_pTcpSocket->SetReceiver(this);
	m_pRUDPSocket = NULL;
	m_pUDPSocket = NULL;

	m_bConnectLoginServer = false;
	m_bConnectVillageServer = false;
	m_bConnectGameServer = false;
	m_bDisConnectServer = false;

	m_ConnectState = BridgeUnknown;

	memset(m_szVillageIP, 0, sizeof(m_szVillageIP));
	m_nVillagePort = 0;

	m_nGameServerIP = 0;;
	m_nGameServerUdpPort = m_nGameServerTcpPort = 0;

	m_nLastLoginConnectOffset = 0;
}

CClientSessionManager::~CClientSessionManager()
{
	DisconnectGameServer( true );
	DisconnectLoginServer( true );
	DisconnectVillageServer( true );
	SAFE_DELETE( m_pTcpSocket );
	SAFE_DELETE( m_pRUDPSocket );
	SAFE_DELETE( m_pUDPSocket );
	SAFE_DELETE_VEC(m_pTaskListener);

	WSACleanup();
}

void CClientSessionManager::SendTcpPacket(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	m_pTcpSocket->AddSendData(iMainCmd, iSubCmd, pData, iLen);
	WriteLog( 1, ", Info, CClientSessionManager::SendPacket : TCP - iMainCmd(%d), iSubCmd(%d), iLen(%d), \n", iMainCmd, iSubCmd, iLen );
}

void CClientSessionManager::SendPacket(int iMainCmd, int iSubCmd, char * pData, int iLen, int iPrior, BYTE cSeqLevel)
{
	bool bChecker = false;
	WriteLog( 1, ", Info, Send Connection State : Login(%c), Village(%c), Game(%c)\n", ( m_bConnectLoginServer ) ? 'o' : 'x', ( m_bConnectVillageServer ) ? 'o' : 'x', ( m_bConnectGameServer ) ? 'o' : 'x' );
	
	//rlkt_udp !
	if (m_bConnectLoginServer || m_bConnectVillageServer ) //|| m_bConnectGameServer
	{
		m_pTcpSocket->AddSendData(iMainCmd, iSubCmd, pData, iLen);
		bChecker = true;
		WriteLog( 1, ", Info, CClientSessionManager::SendPacket : TCP - iMainCmd(%d), iSubCmd(%d), iLen(%d), iPrior(%d)\n", iMainCmd, iSubCmd, iLen, iPrior );
	}	
	if (m_bConnectGameServer)
	{
		_ASSERT(bChecker != true);
		RUDPSendPacket(iMainCmd, iSubCmd, pData, iLen, iPrior, cSeqLevel);
		WriteLog( 1, ", Info, CClientSessionManager::SendPacket : UDP - iMainCmd(%d), iSubCmd(%d), iLen(%d), iPrior(%d)\n", iMainCmd, iSubCmd, iLen, iPrior );
	}
}

void CClientSessionManager::RecvPacket(int iMainCmd, int iSubCmd, char * pData, int iLen, int nInsertAfter)
{
	AddProcessData((BYTE)iMainCmd, (BYTE)iSubCmd, pData, iLen, _SEQDATA_TYPE_RECURSIVE);
	WriteLog( 1, ", Info, CClientSessionManager::RecvPacket : iMainCmd(%d), iSubCmd(%d), iLen(%d), nInsertAfter(%d)\n", iMainCmd, iSubCmd, iLen, nInsertAfter );
}

void CClientSessionManager::DisConnectServer(bool bValidDisconnect)
{
	m_bValidDisConnect = bValidDisconnect;
	m_bDisConnectServer = true;
}

bool CClientSessionManager::ConnectLoginServer(const char *pIP, USHORT nPort, bool bFirstConnect)
{
	if (bFirstConnect)
	{
		WriteLog( 0, ", Info, Connect LoginServer..\n" );
		WriteLog( 1, ", Info, ConnectLoginServer : IP(%s), Port(%d)\n", pIP, nPort );
		WriteLog( 1, ", Info, Cur Connection State : Login(%c), Village(%c), Game(%c)\n", ( m_bConnectLoginServer ) ? 'o' : 'x', ( m_bConnectVillageServer ) ? 'o' : 'x', ( m_bConnectGameServer ) ? 'o' : 'x' );
		if( m_bConnectLoginServer ) DisconnectLoginServer( true );
		if( m_bConnectVillageServer ) DisconnectVillageServer( true );
		if( m_bConnectGameServer ) DisconnectGameServer( true );

		bool bResult = m_pTcpSocket->Connect( pIP, nPort );
		if( bResult == false ) {
			WriteLog( 1, ", Error, ConnectLoginServer Result Failed\n" );
			WriteLog( 0, ", Error, Connect LoginServer Failed\n" );
			return false;
		}
		m_bConnectLoginServer = true;
		WriteLog( 1, ", Info, ConnectLoginServer Result Succeess\n" );
		WriteLog( 0, ", Info, Connect LoginServer Success\n" );

#if defined( PRE_ADD_HSHIELD_LOG )
		if( strstr( pIP, "211.39.159.3" ) )
			CGlobalInfo::GetInstance().m_bUseHShieldLog = true;
#endif	// #if defined( PRE_ADD_HSHIELD_LOG )
	}
	else
	{
//		strcpy_s(m_szLoginIP, pIP);
//		m_nLoginPort = nPort;
		m_ConnectState = (eBridgeState)VillageToLogin;		//컨넥트에만 사용 어디서 오는건지는 그다지 상관없다.
	}

	return true;
}

bool CClientSessionManager::ConnectLoginServer(std::vector<std::string> &szVecIP, std::vector<USHORT> &nVecPort, bool bFirstConnect)
{
	srand( timeGetTime() );
	m_szLoginVecIPList = szVecIP;
	m_nVecLoginPortList = nVecPort;

	if( m_nLastLoginConnectOffset >= (int)m_szLoginVecIPList.size() ) m_nLastLoginConnectOffset = 0;

	if( bFirstConnect ) {
		int nConnectTrialCount = 1 + ( (int)m_szLoginVecIPList.size() * 2 );
#if defined(PRE_FIX_LOGINSERVER_CONNECT)
		m_nLastLoginConnectOffset = rand()%(int)m_szLoginVecIPList.size();
#else	// #if defined(PRE_FIX_LOGINSERVER_CONNECT)
		int m_nLastLoginConnectOffset = rand()%(int)m_szLoginVecIPList.size();
#endif	// #if defined(PRE_FIX_LOGINSERVER_CONNECT)

		for( int i=0; i<nConnectTrialCount; i++ ) {
			std::string szIP = m_szLoginVecIPList[m_nLastLoginConnectOffset];
			USHORT nPort = m_nVecLoginPortList[m_nLastLoginConnectOffset];

			if( ConnectLoginServer( szIP.c_str(), nPort, true ) ) return true;

			m_nLastLoginConnectOffset++;
			if( m_nLastLoginConnectOffset >= (int)m_szLoginVecIPList.size() ) m_nLastLoginConnectOffset = 0;
		}
	}
	else {
		std::string szIP = m_szLoginVecIPList[m_nLastLoginConnectOffset];
		USHORT nPort = m_nVecLoginPortList[m_nLastLoginConnectOffset];
		return ConnectLoginServer( szIP.c_str(), nPort, false );
	}
	return false;
}

bool CClientSessionManager::ConnectVillageServer(const char *pIP, USHORT nPort, int nBridgeState)
{
	_strcpy(m_szVillageIP, _countof(m_szVillageIP), pIP, (int)strlen(pIP));
	m_nVillagePort = nPort;
	m_ConnectState = (eBridgeState)nBridgeState;	
	return true;
}

void CClientSessionManager::ConnectServer()
{
	switch (m_ConnectState)
	{
	case BridgeUnknown:
		return;
	case GameToLogin:
	case VillageToLogin:
		{
			WriteLog( 0, ", Info, Connect LoginServer..\n" );
			WriteLog( 1, ", Info, ConnectLoginServer : IP(%s), Port(%d)\n", m_szLoginVecIPList[m_nLastLoginConnectOffset].c_str(), m_nVecLoginPortList[m_nLastLoginConnectOffset] );
			WriteLog( 1, ", Info, Cur Connection State : Login(%c), Village(%c), Game(%c)\n", ( m_bConnectLoginServer ) ? 'o' : 'x', ( m_bConnectVillageServer ) ? 'o' : 'x', ( m_bConnectGameServer ) ? 'o' : 'x' );
			if( m_bConnectLoginServer ) DisconnectLoginServer( true );
			if( m_bConnectVillageServer ) DisconnectVillageServer( true );
			if( m_bConnectGameServer ) DisconnectGameServer( true );

			bool bResult = m_pTcpSocket->Connect( m_szLoginVecIPList[m_nLastLoginConnectOffset].c_str(), m_nVecLoginPortList[m_nLastLoginConnectOffset] );
			if( bResult == false ) {
				WriteLog( 1, ", Error, ConnectLoginServer Result Failed\n" );
				WriteLog( 0, ", Error, Connect LoginServer Failed\n" );
				m_nLastLoginConnectOffset++;
				if( m_nLastLoginConnectOffset >= (int)m_szLoginVecIPList.size() ) m_nLastLoginConnectOffset = 0;
				return ;
			}
			m_bConnectLoginServer = true;
			WriteLog( 1, ", Info, ConnectLoginServer Result Succeess\n" );
			WriteLog( 0, ", Info, Connect LoginServer Success\n" );
			for (int i = 0; i < (int)m_pTaskListener.size(); i++)
				m_pTaskListener[i]->ConnectResult( true, m_ConnectState, 0, 0 );
			break;
		}
	case LoginToVillage:
	case GameToVillage:
	case VillageToVillage:
		{
			WriteLog( 0, ", Info, Connect VillageServer..\n" );
			WriteLog( 1, ", Info, ConnectVillageServer : IP(%s), Port(%d)\n", m_szVillageIP, m_nVillagePort );
			WriteLog( 1, ", Info, Cur Connection State : Login(%c), Village(%c), Game(%c)\n", ( m_bConnectLoginServer ) ? 'o' : 'x', ( m_bConnectVillageServer ) ? 'o' : 'x', ( m_bConnectGameServer ) ? 'o' : 'x' );
			if( m_bConnectLoginServer ) DisconnectLoginServer( true );
			if( m_bConnectVillageServer ) {
			if( strcmp( m_pTcpSocket->GetServerIP(), m_szVillageIP ) == NULL && m_pTcpSocket->GetServerPort() == m_nVillagePort )
			{
				for (int i = 0; i < (int)m_pTaskListener.size(); i++)
					m_pTaskListener[i]->ConnectResult(true, m_ConnectState, 0, 0);
				break;
			}
			DisconnectVillageServer( true );
			}
			if( m_bConnectGameServer ) DisconnectGameServer( true );
			bool bResult = m_pTcpSocket->Connect( m_szVillageIP, m_nVillagePort );

			if( bResult == false ) {
				WriteLog( 1, ", Error, ConnectVillageServer Result Failed\n" );
				WriteLog( 0, ", Error, Connect VillageServer Failed\n" );
				return;
			}
			m_bConnectVillageServer = true;
			WriteLog( 1, ", Info, ConnectVillageServer Result Success\n" );
			WriteLog( 0, ", Info, Connect VillageServer Success\n" );

			for (int i = 0; i < (int)m_pTaskListener.size(); i++)
				m_pTaskListener[i]->ConnectResult(true, m_ConnectState, 0, 0);
			break;
		}

	case VillageToGame:
		{
			WriteLog( 0, ", Info, Connect GameServer..\n" );
			WriteLog( 1, ", Info, ConnectGameServer : IP(%d), Port(%d), TcpPort(%d), UID(%d)\n", m_nGameServerIP, m_nGameServerUdpPort, m_nGameServerTcpPort, m_nSessionID );
			WriteLog( 1, ", Info, Cur Connection State : Login(%c), Village(%c), Game(%c)\n", ( m_bConnectLoginServer ) ? 'o' : 'x', ( m_bConnectVillageServer ) ? 'o' : 'x', ( m_bConnectGameServer ) ? 'o' : 'x' );

			if( m_bConnectLoginServer ) DisconnectLoginServer( true );
			if( m_bConnectVillageServer ) DisconnectVillageServer( true );
			if( m_bConnectGameServer ) DisconnectGameServer( true );
			
			if( InitializeUdpSession( m_nGameServerUdpPort ) == false ) {
				WriteLog( 1, ", Error, ConnectGameServer Result Failed : InitializeUpdSession \n" );
				WriteLog( 0, ", Error, Connect GameServer Failed\n" );
				for (int i = 0; i < (int)m_pTaskListener.size(); i++)
					m_pTaskListener[i]->ConnectResult(false, m_ConnectState, m_cReqGameIDType, m_nGameTaskType);
				break;
			}

			ResetSeq();
			if( RUDPConnect( m_nGameServerIP, m_nGameServerUdpPort, m_nSessionID ) == false ) {
				WriteLog( 1, ", Error, ConnectGameServer Result Failed : RUDPConnect \n" );
				WriteLog( 0, ", Error, Connect GameServer Failed\n" );
				for (int i = 0; i < (int)m_pTaskListener.size(); i++)
					m_pTaskListener[i]->ConnectResult(false, m_ConnectState, m_cReqGameIDType, m_nGameTaskType);
				break;
			}
			char szIP[IPLENMAX];
			_inet_addr(m_nGameServerIP, szIP);
#ifdef PRE_MOD_INDUCE_TCPCONNECT			
			bool bConnected = false;
			for (int nTryCount = 0; nTryCount < 5; nTryCount++)
			{
				if (m_pTcpSocket->Connect(szIP, m_nGameServerTcpPort))
				{
					bConnected = true;
					break;
				}
					
				Sleep(100);
				continue;
			}

			if (bConnected == false)
			{
				WriteLog( 1, ", Error, ConnectGameServer Result Failed : TCPConnect \n" );
				WriteLog( 0, ", Error, Connect GameServer Failed\n" );
				for (int i = 0; i < (int)m_pTaskListener.size(); i++)
					m_pTaskListener[i]->ConnectResult(false, m_ConnectState, m_cReqGameIDType, m_nGameTaskType);
				break;
			}
#else		//#ifdef PRE_MOD_INDUCE_TCPCONNECT
			if (m_pTcpSocket->Connect(szIP, m_nGameServerTcpPort) == false)
			{
				WriteLog( 1, ", Error, ConnectGameServer Result Failed : TCPConnect \n" );
				WriteLog( 0, ", Error, Connect GameServer Failed\n" );
				for (int i = 0; i < (int)m_pTaskListener.size(); i++)
					m_pTaskListener[i]->ConnectResult(false, m_ConnectState, m_cReqGameIDType, m_nGameTaskType);
				break;
			}
#endif		//#ifdef PRE_MOD_INDUCE_TCPCONNECT

			m_bConnectGameServer = true;			WriteLog( 1, ", Info, ConnectGameServer Result Success \n" );
			WriteLog( 0, ", Info, Connect GameServer Success\n" );

			for (int i = 0; i < (int)m_pTaskListener.size(); i++)
				m_pTaskListener[i]->ConnectResult(true, m_ConnectState, m_cReqGameIDType, m_nGameTaskType);
		}
	}	
	m_ConnectState = BridgeUnknown;
}

bool CClientSessionManager::ReconnectGameServer(UINT nSessionID)
{
	WriteLog( 0, ", Info, Reconnect GameServer..\n" );
	WriteLog( 1, ", Info, ReconnectGameServer : SessionID(%d)\n", nSessionID );
	WriteLog( 1, ", Info, Cur Connection State : Login(%c), Village(%c), Game(%c)\n", ( m_bConnectLoginServer ) ? 'o' : 'x', ( m_bConnectVillageServer ) ? 'o' : 'x', ( m_bConnectGameServer ) ? 'o' : 'x' );
	if( m_bConnectGameServer )
	{
		//여기 상황이야 어찌되었건! 서버에서 끊겼다고 판정이 되었습니다. 어떻게 되든 서버에서 먼가 rudp의 메세지를 받는데에 문제가 생긴겁니다.
		//tcp가 끊겨야 실재 끊김이 되는 것! 이므로 재연결을 시도합니다.
		if( InitializeUdpSession( m_nGameServerUdpPort ) == false ) {
			WriteLog( 1, ", Error, ReconnectGameServer Result Failed : m_nGameServerUdpPort(%d)\n", m_nGameServerUdpPort );
			WriteLog( 0, ", Error, Reconnect GameServer Failed\n" );
			return false;
		}
		RUDPDisConnect();
		bool bResult = RUDPConnect(m_nGameServerIP, m_nGameServerUdpPort, nSessionID);
		if( !bResult ) {
			WriteLog( 1, ", Error, ReconnectGameServer Result Failed : RUDPConnect : m_nGameServerIP(%d), m_nGameServerUdpPort(%d)\n", m_nGameServerIP, m_nGameServerUdpPort );
			WriteLog( 0, ", Error, Reconnect GameServer Failed\n" );
		}
		else {
			WriteLog( 1, ", Info, ReconnectGameServer Result Success\n" );
			WriteLog( 0, ", Info, Reconnect GameServer Success\n" );
		}
		return bResult;
	}
	return false;
}

void CClientSessionManager::DisconnectLoginServer( bool bValidDisconnect )
{
	if( !m_bConnectLoginServer ) return;

	m_pTcpSocket->Close( true, bValidDisconnect );
	m_bConnectLoginServer = false;
	WriteLog( 0, ", Info, Disconnect LoginServer..\n" );
	
	ClearProcessData();
}

void CClientSessionManager::DisconnectVillageServer( bool bValidDisconnect )
{
	if( !m_bConnectVillageServer ) return;
	m_pTcpSocket->Close( true, bValidDisconnect );
	m_bConnectVillageServer = false;
	WriteLog( 0, ", Info, Disconnect VillageServer..\n" );

	ClearProcessData();
}

bool CClientSessionManager::ConnectGameServer(unsigned long iIP, unsigned short iPort, unsigned short nTcpPort, int iUID, int nBridgeState, char cReqGameIDType, int nGameTaskType)
{
	m_nGameServerIP = iIP;
	m_nGameServerUdpPort = iPort;
	m_nGameServerTcpPort = nTcpPort;
	m_nSessionID = iUID;
	m_ConnectState = (eBridgeState)nBridgeState;
	m_cReqGameIDType = cReqGameIDType;
	m_nGameTaskType = nGameTaskType;
	return true;
}

void CClientSessionManager::DisconnectGameServer( bool bValidDisconnect )
{
	if( !m_bConnectGameServer ) return;
	if( bValidDisconnect ) {
		SendPacket(CS_SYSTEM, eSystem::CS_INTENDED_DISCONNECT, NULL, 0, _RELIABLE);
	}

#ifdef PRE_ADD_THREAD_ROOMDESTROY_TEST
	m_pTcpSocket->Close( true, bValidDisconnect );	
	ULONG uiStartTick = timeGetTime();
	while (1)
	{
		SendPacket(CS_PARTY, eParty::CS_PARTYOUT, NULL, 0);
		Sleep(10);
		if (timeGetTime() - uiStartTick > (10 * 1000))
			break;
	}
	RUDPDisConnect();
	m_bConnectGameServer = false;
#else		//#ifdef PRE_ADD_THREAD_ROOMDESTROY_TEST
	RUDPDisConnect();
	m_pTcpSocket->Close( true, bValidDisconnect );
	m_bConnectGameServer = false;
#endif		//#ifdef PRE_ADD_THREAD_ROOMDESTROY_TEST
	WriteLog( 0, ", Info, Disconnect GameServer..\n" );

	ClearProcessData();
}

bool CClientSessionManager::RUDPConnect(unsigned long iIP, unsigned short iPort, int iUID)
{
	if (m_pRUDPSocket)
	{
		struct in_addr addr;
		addr.S_un.S_addr = iIP;
		if (m_pRUDPSocket->Connect(addr.S_un.S_addr, iPort) == false) {
			WriteLog( 1, ", Error, CClientSessionManager::RUDPConnect1 Result Failed : IP(%d), Port(%d) \n", addr.S_un.S_addr, iPort );
			return false;
		}

#ifdef _USE_NATPROBE
		m_pRUDPSocket->DetectAddr(s_szProbeIP[1], s_nProbePort[1]);
#endif
		
		CSConnectRequest packet;
		packet.nSessionID= iUID;
		WCHAR wszIP[32];
		if (GetLocalIP(wszIP))
			packet.nAddrIP = _inet_addr(wszIP);
		else
			_ASSERT(0);
		packet.nPort = GetUdpPort();

		m_pRUDPSocket->Send(CS_SYSTEM, eSystem::CS_CONNECT_REQUEST, &packet, sizeof(packet), _RELIABLE, 0);
		OutputDebug("Send RUDP Connect Msg\n");
	}
	return true;
}

//bool CClientSessionManager::RUDPConnect(const char * pIP, unsigned short iPort, int iUID)
//{
//	if (m_pRUDPSocket)
//	{
//		struct in_addr addr;
//		addr.S_un.S_addr = _inet_addr(pIP);
//		if (m_pRUDPSocket->Connect(addr.S_un.S_addr, iPort) == false) {
//			WriteLog( 1, ", Error, CClientSessionManager::RUDPConnect2 Result Failed : IP(%s, %d), Port(%d), iUID(%d) \n", pIP, addr.S_un.S_addr, iPort, iUID );
//			return false;
//		}
//
//#ifdef _USE_NATPROBE
//		m_pRUDPSocket->DetectAddr(s_szProbeIP[1], s_nProbePort[1]);
//		if (m_pUDPSocket)
//			m_pUDPSocket->DetectAddr(s_szProbeIP[1], s_nProbePort[1]);
//#endif
//
//		CSConnectRequest packet;
//		memset(&packet, 0, sizeof(packet));
//		packet.nSessionID= iUID;
//
//		m_pRUDPSocket->GetAddr(&packet.nAddrIP, &packet.nPort);
//#ifdef _USE_NATPROBE
//		if (packet.nPort <= 0)
//		{
//			//Probe를 통하여 주소를 얻어 오는 것이 실패 했습니다. 가장 안정적인 TCP쪽 public주소를 얻어와서 보내줍니다. 
//			//클라이언트가 공유기에 있을 경우 포트가 다를 수 있습니다. 포트는 로커호스트에서 얻어 옵니다.
//			WCHAR wszIP[32];
//			if (GetLocalIP(wszIP))
//				packet.nAddrIP = _inet_addr(wszIP);
//			else	_ASSERT(0);
//			packet.nPort = GetUdpPort();
//		}
//#endif
//
//		m_pRUDPSocket->Send(CS_SYSTEM, eSystem::CS_CONNECT_REQUEST, &packet, sizeof(packet), _RELIABLE);
//		OutputDebug("Send RUDP Connect Msg\n");
//	}
//	return true;
//}

void CClientSessionManager::RUDPDisConnect()
{
	if (m_pRUDPSocket)
		m_pRUDPSocket->DisConnect();
	m_UDPPeerList.clear();
}

//void CClientSessionManager::RUDPRecieve(int iMainCmd, int iSubCmd, char * data, int size, _ADDR * addr)
//{
//	//Check PeerManage
//	if (iMainCmd == SC_SYSTEM)
//	{
//		switch (iSubCmd)
//		{
//			case eSystem::CC_PEER_CONNECT_REQUEST:
//			{
//				//Client Peer Connect Request
//				CCPeerConnectRequest * pPacket = (CCPeerConnectRequest*)data;
//
//				if (pPacket->cState == 0)
//				{		//Client A -> client B Peer Connect Request Msg
//					if (IsPeerConnected(pPacket->nSessionID[0]) == false)		//Check Sender ID
//					{		//if Not Connect Connection
//						//Make Packet
//						CCPeerConnectRequest packet;
//						memcpy(&packet, pPacket, sizeof(CCPeerConnectRequest));
//						packet.cState = 1;
//
//						OutputDebug("step 2 connect request sender con : %d, recv con : %d, ip:%d.%d.%d.%d port:%d\n", pPacket->nSessionID[0], pPacket->nSessionID[1], \
//							addr->ip[0], addr->ip[1], addr->ip[2], addr->ip[3], htons(addr->port));
//						WriteLog(1, ", Info, step 2 connect request sender con : %d, recv con : %d, ip:%d.%d.%d.%d port:%d\n", pPacket->nSessionID[0], pPacket->nSessionID[1], \
//							addr->ip[0], addr->ip[1], addr->ip[2], addr->ip[3], htons(addr->port));
//
//						//Notify To Sender
//						for (int i  = 0; i < 3; i++)		//considering loss packet
//							UDPSendPacket( CS_SYSTEM, eSystem::CC_PEER_CONNECT_REQUEST, &packet, sizeof(packet), _inet_addr(addr->ip), htons(addr->port));
//					}
//				}
//				else	if (pPacket->cState == 1)
//				{		//Client B -> Client A Peer Connected Msg
//					if (IsPeerConnected(pPacket->nSessionID[1]) == false)
//					{
//						OutputDebug("step 3 connected result sender con : %d, recv con : %d, ip:%d.%d.%d.%d port:%d\n", pPacket->nSessionID[0], pPacket->nSessionID[1], \
//							addr->ip[0], addr->ip[1], addr->ip[2], addr->ip[3], htons(addr->port));
//						WriteLog(1, ", Info, step 3 connected result sender con : %d, recv con : %d, ip:%d.%d.%d.%d port:%d\n", pPacket->nSessionID[0], pPacket->nSessionID[1], \
//							addr->ip[0], addr->ip[1], addr->ip[2], addr->ip[3], htons(addr->port));
//
//						AddPeer(_inet_addr(addr->ip), htons(addr->port), pPacket->nSessionID[1]);		//adding peer
//						CCPeerConnectRequest packet;
//						memcpy(&packet, pPacket, sizeof(CCPeerConnectRequest));
//						//Notify To Server
//						RUDPSendPacket( CS_SYSTEM, eSystem::CC_PEER_CONNECT_REQUEST, &packet, sizeof(packet), _RELIABLE);
//					}
//				}
//				else	if (pPacket->cState == 2)
//				{		//Client A -> B Peer DisConnected Msg
//					if (IsPeerConnected(pPacket->nSessionID[0]) == true)
//						RemovePeer(pPacket->nSessionID[0]);
//				}
//				return;
//			}
//
//			case eSystem::SC_PEER_CONNECT_REQUEST:
//			{
//				//Server Peer Connect Request
//				SCPeerConnectRequest * pPacket = (SCPeerConnectRequest*)data;
//				CCPeerConnectRequest packet;
//
//				//게임서버에서 피어컨넥트하라는 요청을 받았습니다.
//				packet.cState = 0;
//				packet.nSessionID[0] = pPacket->nSessionID[0];			//My ConID
//				packet.nSessionID[1] = pPacket->nSessionID[1];			//Your ConID
//				OutputDebug("step 1 connect request recv from server sender con : %d, recv con : %d, ip:%d.%d.%d.%d port:%d\n", pPacket->nSessionID[0], pPacket->nSessionID[1], \
//					((unsigned char*)&pPacket->nDestAddrIP)[0], \
//					((unsigned char*)&pPacket->nDestAddrIP)[1], ((unsigned char*)&pPacket->nDestAddrIP)[2], \
//					((unsigned char*)&pPacket->nDestAddrIP)[3], pPacket->nDestAddrPort);
//				WriteLog(1, ", Info, step 1 connect request recv from server sender con : %d, recv con : %d, ip:%d.%d.%d.%d port:%d\n", pPacket->nSessionID[0], pPacket->nSessionID[1], \
//					((unsigned char*)&pPacket->nDestAddrIP)[0], \
//					((unsigned char*)&pPacket->nDestAddrIP)[1], ((unsigned char*)&pPacket->nDestAddrIP)[2], \
//					((unsigned char*)&pPacket->nDestAddrIP)[3], pPacket->nDestAddrPort);
//				for (int i  = 0; i < 3; i++)		//considering loss packet
//					UDPSendPacket( CS_SYSTEM, eSystem::CC_PEER_CONNECT_REQUEST, &packet, sizeof(packet), pPacket->nDestAddrIP, pPacket->nDestAddrPort);
//				return;
//			}
//
//			case eSystem::CC_PEER_PING:
//			{
//				//핑받으면 퐁쏩니다.
//				int seq;
//				memcpy(&seq, data, sizeof(seq));
//				UDPSendPacket(CS_SYSTEM, eSystem::CC_PEER_PONG, (char*)&seq, sizeof(seq), _inet_addr(addr->ip), htons(addr->port));
//				return;
//			}
//
//			case eSystem::CC_PEER_PONG:
//			{
//				static unsigned int s_nAmount = 0;
//				static unsigned int s_nPing = 0;
//				static unsigned int s_nCnt = 0;
//				int seq;
//				memcpy(&seq, data, sizeof(seq));
//				
//				std::vector<_PEER>::iterator ii;
//				for (ii = m_UDPPeerList.begin(); ii != m_UDPPeerList.end(); ii++)
//				{
//					if (_inet_addr((*ii).addr.ip) == _inet_addr(addr->ip) && (*ii).addr.port == htons(addr->port))
//					{
//						(*ii).Ping[seq].nPing = timeGetTime() - (*ii).Ping[seq].nSendTick;
//
//						s_nAmount += (*ii).Ping[seq].nPing;
//						s_nCnt++;
//						s_nPing = s_nAmount/s_nCnt;
//						
//						if (s_nCnt >= 32)
//						{	//대략 32번에 한번씩은 초기화 합니다.
//							OutputDebug("Peer Ping seq : [%d] ping : [%d]\n", seq, s_nPing);
//							s_nAmount = s_nCnt = 0;
//							//대략 요타이밍에 평균치가 일정이상으로 떨어지면 경고!
//							if (s_nPing > 400)
//							{//대략 400정도가 나오면 갔다가 오는대 평균치로 200정도 게임하기 힘들어요.. 보완 서버로 라우팅이 빠른지 확인 절차를 만들고
//								//좀더 빠른쪽으로 진행하는 루틴을 만듭시다.
//								UINT nSessionID = (*ii).iUID;
//								RemovePeer(nSessionID);
//
//								CCPeerConnectRequest packet;
//								memset(&packet, 0, sizeof(packet));
//
//								packet.cState = 2;
//								packet.nSessionID[1] = nSessionID;
//
//								RUDPSendPacket( CS_SYSTEM, eSystem::CC_PEER_CONNECT_REQUEST, &packet, sizeof(packet), _RELIABLE);
//							}
//						}
//						return ;
//					}
//				}
//				OutputDebug("Peer Ping Update Failed\n");
//				WriteLog(1, ", Error, Peer Ping Update Failed\n");
//				return;
//			}
//		}
//	}
//	
//	for(unsigned int i = 0; i < (unsigned int)m_pVecUdpSession.size(); i++) 
//		m_pVecUdpSession[i]->OnDispatchMessage( iMainCmd, iSubCmd, data, size );
//}

void CClientSessionManager::RUDPSendPacket(int iMainCmd, int iSubCmd, void * pData, int iSize, int iPrior, BYTE cSeqLevel)
{
	if (m_pRUDPSocket)
		m_pRUDPSocket->Send(iMainCmd, iSubCmd, pData, iSize, iPrior, cSeqLevel);

	if (m_pUDPSocket != NULL && (iMainCmd == CS_ACTOR || iMainCmd == CS_ACTORBUNDLE))// || iMainCmd == CS_PROP))
	{
		//Peer에 바로 쏴도 되는건 쏩니다. (Actor Msg, Prop Msg)
		unsigned short iSwapHeader = SC_ACTOR;
		std::vector<_PEER>::iterator ii;
		for (ii = m_UDPPeerList.begin(); ii != m_UDPPeerList.end(); ii++)
			UDPSendPacket(iSwapHeader, iSubCmd, pData, iSize, _inet_addr((*ii).addr.ip), (*ii).addr.port);
	}
}

void CClientSessionManager::UDPSendPacket(int iMainCmd, int iSubCmd, void * pData, int iSize, unsigned long iIP, unsigned short iPort)
{
	if (m_pUDPSocket)
		m_pUDPSocket->Send(iMainCmd, iSubCmd, pData, iSize, iIP, iPort);
}

bool CClientSessionManager::InitializeUdpSession( USHORT nPort )
{
	if (m_pRUDPSocket == NULL)
	{
		m_pRUDPSocket = new CNetSocketRUDP;
		if (!m_pRUDPSocket)	return false;
		m_pRUDPSocket->SetReceiver(this);
	}
#ifdef _USE_PERRCONNECT
	if( m_pUDPSocket == NULL ) {
		m_pUDPSocket = new CNetSocketUDP("UDPThread", m_pRUDPSocket);
		if (!m_pUDPSocket)	return false;
		if (m_pUDPSocket->Create() == true)
			m_pUDPSocket->SetReciever(this);
	}
#endif
	return true;
}

bool CClientSessionManager::FinalizeUdpSession()
{
	SAFE_DELETE(m_pRUDPSocket);
	SAFE_DELETE(m_pUDPSocket);
	return true;
}

//void CClientSessionManager::AddTcpSession( CClientTcpSession *pSession )
//{
//	for( DWORD i=0; i<m_pVecTcpSession.size(); i++ ) {
//		if( m_pVecTcpSession[i] == pSession ) return;
//	}
//	m_pVecTcpSession.push_back( pSession );
//}
//
//void CClientSessionManager::AddUdpSession( CClientUdpSession *pSession )
//{
//	for( DWORD i=0; i<m_pVecUdpSession.size(); i++ ) {
//		if( m_pVecUdpSession[i] == pSession ) return;
//	}
//	m_pVecUdpSession.push_back( pSession );
//}
//
//void CClientSessionManager::RemoveTcpSession( CClientTcpSession *pSession )
//{
//	for( DWORD i=0; i<m_pVecTcpSession.size(); i++ ) {
//		if( m_pVecTcpSession[i] == pSession ) {
//			m_pVecTcpSession.erase( m_pVecTcpSession.begin() + i ) ;
//			return;
//		}
//	}
//}
//
//void CClientSessionManager::RemoveUdpSession( CClientUdpSession *pSession )
//{
//	for( DWORD i=0; i<m_pVecUdpSession.size(); i++ ) {
//		if( m_pVecUdpSession[i] == pSession ) {
//			m_pVecUdpSession.erase( m_pVecUdpSession.begin() + i );
//			return;
//		}
//	}
//}

void CClientSessionManager::OnConnectTcp()
{
	std::vector <CTaskListener*>::iterator ii;
	for (ii = m_pTaskListener.begin(); ii != m_pTaskListener.end(); ii++)
		(*ii)->OnConnectTcp();
	
	/*DWORD dwCount = (DWORD)m_pVecTcpSession.size();
	for( DWORD i=0; i<dwCount; i++ ) {
		m_pVecTcpSession[i]->OnConnectTcp();
	}*/
}

void CClientSessionManager::OnDisconnectTcp( bool bValidDisconnect )
{
	std::vector <CTaskListener*>::iterator ii;
	for (ii = m_pTaskListener.begin(); ii != m_pTaskListener.end(); ii++)
		(*ii)->OnDisconnectTcp(bValidDisconnect);

	/*DWORD dwCount = (DWORD)m_pVecTcpSession.size();
	for( DWORD i=0; i<dwCount; i++ ) {
		m_pVecTcpSession[i]->OnDisconnectTcp( bValidDisconnect );
	}*/
}

//void CClientSessionManager::OnDispatchMessageTcp( int iMainCmd, int iSubCmd, char * pData, int iLen )
//{
//	DWORD dwCount = (DWORD)m_pVecTcpSession.size();
//	for( DWORD i=0; i<dwCount; i++ ) {
//		m_pVecTcpSession[i]->OnDispatchMessage( iMainCmd, iSubCmd, pData, iLen );
//	}
//
//
//	std::vector<MessageListener*>::iterator it = m_MessageListenerArray.begin();
//	for ( ; it != m_MessageListenerArray.end() ; it++ )
//	{
//		MessageListener* pListener = NULL;
//		pListener = *it;
//		if ( pListener  )
//		{
//			pListener->OnDispatchMessage(iMainCmd, iSubCmd, pData, iLen);
//		}
//	}
//}

//void CClientSessionManager::SendGameTcpConnect(UINT nSessionID, UINT nAccountDBID, INT64 biCertifyingKey)
//{
//	//일단 여기서 정보를 날려주자
//	CSConnectGame packet;
//	memset(&packet, 0, sizeof(packet));
//	packet.nSessionID = nSessionID;
//	packet.nAccountDBID = nAccountDBID;
//	packet.biCertifyingKey = biCertifyingKey;
//#if defined(_KR)
//	{
//		CSystemInfo Info;
//		Info.Init();
//
//		memcpy(Login.szMID, Info.GetMachineId(), sizeof(Login.szMID));
//		Login.dwGRC = Info.GetGameRoomClient();
//	}
//#endif
//
//	m_pTcpSocket->AddSendData(CS_SYSTEM, eSystem::CS_CONNECTGAME, (char*)&packet, sizeof(packet));
//}

void CClientSessionManager::OnDisconnectUdp( bool bValidDisconnect )
{
	std::vector <CTaskListener*>::iterator ii;
	for (ii = m_pTaskListener.begin(); ii != m_pTaskListener.end(); ii++)
		(*ii)->OnDisconnectUdp(bValidDisconnect);
}

void CClientSessionManager::Process()
{
	if( m_pTcpSocket )
	{
		if (m_pTcpSocket->DoUpdate() == -1)
		{ 
			m_pTcpSocket->Close();
			return;
		}
		else
		{
			//대략 세개의 쓰레드에서 억세스 클라이언트 메인쓰레드, 네트워크 쓰레드(udp, tcp), 로딩쓰레드가 데겠습니다.
			//패킷데이타가 모여서 여기서 플러싱 해주는데 뭉치가 모여서 디스패치단 처리가 느려질경우 네트워크쓰레드도 크리티칼섹션에서 블럭이 걸리는 형태로 진행이 되므로,
			//일단 급하게 처리변경.

			std::list <_SEQ_DATA> ProcessList;
			std::list <_SEQ_DATA>::iterator ii;

			{
				ScopeLock <CSyncLock> sync(m_ProcessSync);			
				ProcessList = m_pProcessList;
				m_pProcessList.clear();
			}

			for (ii = ProcessList.begin(); ii != ProcessList.end(); ii++)
			{
				for (int i = 0; i < (int)m_pTaskListener.size(); i++)
					m_pTaskListener[i]->OnDispatchMessage((*ii).cMainCmd, (*ii).cSubCmd, (*ii).pBuf, (*ii).nLen);

				for (int i = 0; i < (int)m_MessageListenerArray.size(); i++)
				{
					if (m_MessageListenerArray[i])
						m_MessageListenerArray[i]->OnDispatchMessage((*ii).cMainCmd, (*ii).cSubCmd, (*ii).pBuf, (*ii).nLen);
				}

				if (m_bDisConnectServer)
					break;
				if (InterlockedIncrement(&m_nSkipProcess)&SkipFlag)
				{
					InterlockedExchange(&m_nSkipProcess, 0);
					break;
				}
			}

			for (ii = ProcessList.begin(); ii != ProcessList.end(); ii++)
				SAFE_DELETEA((*ii).pBuf);
			ProcessList.clear();
		}
	}	

	if (m_bDisConnectServer)
	{
		DisconnectLoginServer(m_bValidDisConnect);
		DisconnectVillageServer(m_bValidDisConnect);
		DisconnectGameServer(m_bValidDisConnect);
		m_bDisConnectServer = false;
	}

	ConnectServer();
}

void CClientSessionManager::AddTaskListener(CTaskListener * pListener)
{
	std::vector <CTaskListener*>::iterator ii;
	for (ii = m_pTaskListener.begin(); ii != m_pTaskListener.end(); ii++)
		if ((*ii) == pListener) return;
	m_pTaskListener.push_back(pListener);
}

void CClientSessionManager::RemoveTaskListener(CTaskListener * pListener)
{
	std::vector <CTaskListener*>::iterator ii;
	for (ii = m_pTaskListener.begin(); ii != m_pTaskListener.end(); ii++)
	{
		if ((*ii) == pListener)
		{
			m_pTaskListener.erase(ii);
			return;
		}
	}
}

BOOL CClientSessionManager::GetLocalIP(WCHAR* pIP)
{
	return m_pTcpSocket->GetLocalIP( pIP );
}

USHORT CClientSessionManager::GetLocalPort(void)
{
	return m_pTcpSocket->GetLocalPort();
}

const char *CClientSessionManager::GetUdpIP()
{
	return m_pTcpSocket->GetServerIP();
}

USHORT CClientSessionManager::GetUdpPort()
{
	return m_pUDPSocket ? m_pUDPSocket->GetPort() : 0;
}

#ifdef _USE_NATPROBE
void CClientSessionManager::SetProbeInfo(const char * pTcpIP, const char * pUdpIP, int nTcpPort, int nUdpPort, int nOpenPort)
{
	_strcpy(s_szProbeIP[0], _countof(s_szProbeIP[0]), pTcpIP, (int)strlen(pTcpIP));
	_strcpy(s_szProbeIP[1], _countof(s_szProbeIP[1]), pUdpIP, (int)strlen(pUdpIP));	
	s_nProbePort[0] = nTcpPort;
	s_nProbePort[1] = nUdpPort;
	s_bNetOpen = false;
	s_nPeerOpenPort = nOpenPort;

	DWORD dwThreadID;
	HANDLE hThread;
	hThread = CreateThread(0, 0, _Accept_Test, (void*) NULL, 0, &dwThreadID);
	WaitForSingleObject(hThread, 10*1000);
}
#endif

DWORD WINAPI CClientSessionManager::_Accept_Test(void * param)
{
	SOCKET hUDP;
	fd_set fdset;
	int readbytes, addrlen, cnt, udpport;
	char buffer[2048];
	int pingtotal = 0, pingtest=0;
	timeval tm;
	SOCKADDR_IN addr;
	int state = 0;
	int pivot;

	timeBeginPeriod(1);

	hUDP = CreateUDPSocket(s_nPeerOpenPort);

	for(cnt=0; cnt<10 && state < 2; cnt++) // 10 초 동안은 대기해준다.
	{
		switch(state)
		{
		case 0 : {
			static char _msg[] = "orz";

			memset(&addr, 0, sizeof(addr));
			addr.sin_family	= AF_INET;
			addr.sin_port	= htons(s_nProbePort[1]);
			addr.sin_addr.S_un.S_addr	= _inet_addr(s_szProbeIP[1]);

			pivot = timeGetTime();
			int senbytes = sendto(hUDP, _msg, 3, 0, (struct sockaddr*) &addr, sizeof(addr));
			//				if (senbytes == -1)
			//					_TRACE("%d\n", GetLastError());
			break;
				 }
		case 1 : {
			addr.sin_family				= AF_INET;
			addr.sin_port				= htons(s_nProbePort[0]);
			addr.sin_addr.S_un.S_addr	= _inet_addr(s_szProbeIP[0]);

			for(int i=0; i<3; i++)
			{
				SOCKET hSocket = CreateTCPSocket(0);

				if (connect(hSocket, (struct sockaddr *)&addr, sizeof(addr)) != SOCKET_ERROR)
				{
					unsigned short k[2];
					k[0] = udpport;
					k[1] = (rand()&1) ? k[0] - 9321 : k[0] - 1110;
					send(hSocket, (char*)&k, 4, 0);
					state = 1;
					closesocket(hSocket);
				}
				//					Sleep(50);
			}
			break;
				 }
		}

		tm.tv_sec	= 1;
		tm.tv_usec	= 0;

		FD_ZERO(&fdset);
		FD_SET(hUDP, &fdset);

		if (select(FD_SETSIZE, (fd_set*)&fdset, (fd_set*)0, (fd_set*)0, &tm) != SOCKET_ERROR)
		{
			if (fdset.fd_count == 0) // timeout
				continue;

			addrlen = sizeof(addr);
			readbytes = recvfrom(hUDP, buffer, sizeof(buffer), 0, (struct sockaddr*) &addr, &addrlen);

			//			if (readbytes == -1)
			//				_TRACE("%d\n", GetLastError());

			switch(state)
			{
			case 0 :
				if (readbytes == 12)
				{
					cnt = 0;
					struct _RETURN_UDP_ADDR
					{
						unsigned short	port[2];
						unsigned long	addr[2];
					}	* _udp_detect = (_RETURN_UDP_ADDR*) buffer;

					if (_udp_detect->port[0] == _udp_detect->port[1] &&
						_udp_detect->addr[0] == _udp_detect->addr[1])
					{
						udpport = _udp_detect->port[0];

						pingtotal += (timeGetTime() - pivot);
						pingtest++;

						if (pingtest >= PING_TEST_COUNT)
							state = 1;
						Sleep(10);
					}
				}
				break;
			case 1 :
				if (readbytes == 3 && !memcmp(buffer, "OTL", 3))
				{
					s_bNetOpen = true;
					state = 2;
				}
				break;
			}
		}
	}

	closesocket(hUDP);

	int pingtick = (pingtotal+PING_TEST_COUNT-1) / PING_TEST_COUNT; // 올림
	static int s_iPingCount = pingtick;

	return 0;
}

SOCKET CClientSessionManager::CreateUDPSocket(int port)
{
	SOCKET s;
	SOCKADDR_IN addr;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (s == INVALID_SOCKET)
		return false;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family			= AF_INET;
	addr.sin_port			= htons(port);
	addr.sin_addr.s_addr	= INADDR_ANY;

	if (::bind(s, (struct sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(s);
		return INVALID_SOCKET;
	}

	DWORD dwMode = 1;
	::ioctlsocket( s, FIONBIO, &dwMode );

	return s;
}

SOCKET CClientSessionManager::CreateTCPSocket(int port)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
		return INVALID_SOCKET;

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(port);
	addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);

	if (::bind(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(s);

		return INVALID_SOCKET;
	}

	return s;
}

bool CClientSessionManager::IsPeerConnected(unsigned int iUID)
{
	std::vector<_PEER>::iterator ii;
	for (ii = m_UDPPeerList.begin(); ii != m_UDPPeerList.end(); ii++)
		if ((*ii).iUID == iUID)
			return true;
	return false;
}

void CClientSessionManager::AddPeer(unsigned long iAddr, unsigned short iPort, unsigned int iUID)
{
	_PEER _peer;
	memset(&_peer, 0, sizeof(_PEER));

	for (int i = 0; i < sizeof(_peer.addr.ip)/sizeof(*_peer.addr.ip); i++)
		_peer.addr.ip[i] = ((unsigned char*)&iAddr)[i];
	_peer.addr.port = iPort;
	_peer.iUID = iUID;
	m_UDPPeerList.push_back(_peer);
}

void CClientSessionManager::RemovePeer(unsigned int iUID)
{
	std::vector<_PEER>::iterator ii;
	for (ii = m_UDPPeerList.begin(); ii != m_UDPPeerList.end(); ii++)
	{
		if ((*ii).iUID == iUID)
		{
			m_UDPPeerList.erase(ii);
			return;
		}
	}
#ifdef _DEBUG
	_ASSERT(0);
#endif
}

void CClientSessionManager::CheckPeerPing()
{
	if (m_pUDPSocket == NULL) return;

	static unsigned long s_nLastTick = 0;
	unsigned long nCurTick = timeGetTime();

	if (s_nLastTick == 0)	s_nLastTick = nCurTick;
	if (s_nLastTick != 0 && nCurTick - s_nLastTick > 500)
	{
		std::vector <UINT> vDisconList;
		std::vector <_PEER>::iterator ii;
		for (ii = m_UDPPeerList.begin(); ii != m_UDPPeerList.end(); ii++)
		{
			int cnt = (*ii).nSeq++&31;
			(*ii).Ping[cnt].nSendTick = nCurTick;
			(*ii).Ping[cnt].nPing = 0;
			UDPSendPacket(CS_SYSTEM, eSystem::CC_PEER_PING, (char*)&cnt, sizeof(cnt), _inet_addr((*ii).addr.ip), (*ii).addr.port);
			
			//간간히 한번씩 핑을 보냈는데 응답이 없는지 확인 합니다.
			//대략 0.5초에 한번씩 ping을 쏘므로 32번의 트래버스를 도는데 16초가 걸립니다. 대략 한 8초에 한번식 체킹을 하는게 좋겠습니다.
			//실재 끊어짐은 16초를 기점으로 잡고 16초 동안 핑데이타의 업데이트가 없으면 응답이 없는 것으로 간주 피어리스트에서 삭제.
			//rudpserver에도 알려서 메세지를 server를 통해 중계 하도록 유도합니다.
			bool bFlag = false;
			if (cnt%16 == 0)
			{
				for (int i = 0; i < sizeof((*ii).Ping)/sizeof(*(*ii).Ping) || bFlag == false; i++)
					bFlag = (*ii).Ping[i].nPing != 0 ? true : false;

				if (bFlag == false)
				{
					vDisconList.push_back((*ii).iUID);
					OutputDebug("Peer DisConnected SID : [%d]\n", (*ii).iUID);
				}
			}
		}

		for (int i = 0; i < (int)vDisconList.size(); i++)
		{
			RemovePeer(vDisconList[i]);

			CCPeerConnectRequest packet;
			memset(&packet, 0, sizeof(packet));

			packet.cState = 2;
			packet.nSessionID[1] = vDisconList[i];

			RUDPSendPacket( CS_SYSTEM, eSystem::CC_PEER_CONNECT_REQUEST, &packet, sizeof(packet), _RELIABLE, 0);
		}

		s_nLastTick = nCurTick;
	}
}

void CClientSessionManager::RemoveListener(MessageListener* pListener)
{
	std::vector<MessageListener*>::iterator it = m_MessageListenerArray.begin();
	for ( ; it != m_MessageListenerArray.end() ; it++ )
	{
		if ( pListener == *it )
		{
			m_MessageListenerArray.erase(it);
			return;
		}
	}
}

void CClientSessionManager::LockProcessDispatchMessage( bool bLock )
{
	/*if( bLock ) m_ProcessDispatchLock.Lock();
	else m_ProcessDispatchLock.UnLock();*/

	if( bLock ) m_ProcessSync.Lock();
	else m_ProcessSync.UnLock();
}
