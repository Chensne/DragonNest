#include "StdAfx.h"
#include "IocpManager.h"
#include "Connection.h"
#include "minidump.h"
#include "Log.h"
#include "SendThread.h"
#ifdef _USE_ACCEPTEX
#include "SocketContextMgr.h"
#include "AcceptorEx.h"
#endif
#if defined(_USE_ACCEPTEX)
#else
#include "EventAcceptor.h"
#endif //#if defined(_USE_ACCEPTEX)
#include "VarArg.h"
#include <fstream>
#if defined (_LOGINSERVER)
extern TLoginConfig g_Config;
#endif

#if !defined (_SERVICEMANAGER_EX)
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined (_SERVICEMANAGER_EX)

#if defined(_SERVER)

CIocpManager::CIocpManager(void)
: m_WaitThreadCount(0), m_SendIOCount(0), m_pSendThread(NULL), m_hIOCP(INVALID_HANDLE_VALUE)
{
	m_bThreadSwitch = true;
	m_nAddSendBufSize = 0;
	m_nPostSendSize = 0;		//buffer pop
	m_nRealSendSize = 0;
	m_nAddRecvBufSize = 0;
	m_nPostRecvBufSize = 0;
	m_nProcessBufSize = 0;
	m_pAcceptor = NULL;
}

CIocpManager::~CIocpManager(void)
{
	Final();
}

int CIocpManager::Init(int nSocketCountMax, int nWorkerThreadSize)
{
	WSADATA WsaData;
	int Ret = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (Ret < 0) return -1;

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hIOCP == NULL) return -1;

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	int Count = nWorkerThreadSize == 0 ? (SysInfo.dwNumberOfProcessors * 2) + 1 : nWorkerThreadSize;

	_GetHostIPAddress();
#ifdef _USE_ACCEPTEX
	m_uiWorkerThreadCount = Count;
	if( !CSocketContextMgr::CreateInstance( this ) )
		return -1;
	if( !CSocketContextMgr::GetInstance().bInitialize() )
		return -1;
	if( !CAcceptorEx::CreateInstance( this ) )
		return -1;
#else
	for( int i=0 ; i<nSocketCountMax ; ++i)
	{
		CSocketContext* pSocketContext = new CSocketContext;
		if (pSocketContext)
			m_SocketContexts.push_back(pSocketContext);
	}
#endif

	for( int i=0 ; i<Count; ++i )
	{
		HANDLE handle;
		UINT nThreadID;
		handle = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, &nThreadID);
#ifdef _USE_ACCEPTEX
		m_vWorkerThreadHandle.push_back( handle );
#else
		if (handle != INVALID_HANDLE_VALUE)
			CloseHandle(handle);
#endif
	}

	// SendThread 생성
	m_pSendThread = new CSendThread;
	if( m_pSendThread == NULL || !m_pSendThread->Start() )
		return -1;
#if defined(_USE_ACCEPTEX)
#else
	m_pAcceptor = new EventAcceptor(this);
	// Accept Thread 가동.
	m_pAcceptor->Start();
#endif //#if defined(_USE_ACCEPTEX)
	return 0;
}

void CIocpManager::Final()
{
	// SendThread 종료
	SAFE_DELETE( m_pSendThread );

	// 어셉터들 먼저 정리
	CloseAcceptors();
#if defined(_USE_ACCEPTEX)
#else
	SAFE_DELETE( m_pAcceptor );	
#endif //#if defined(_USE_ACCEPTEX)

	if (!m_SocketContexts.empty()){
		TSocketContextList::iterator iter;
		for (iter = m_SocketContexts.begin(); iter != m_SocketContexts.end(); ++iter){
			SAFE_DELETE(*iter);
		}
		m_SocketContexts.clear();
	}

	if (m_hIOCP != INVALID_HANDLE_VALUE){
		CloseHandle(m_hIOCP);
		m_hIOCP = INVALID_HANDLE_VALUE;
	}

	WSACleanup();
}

int CIocpManager::AttachSocket(CSocketContext *pSocketContext)
{
	if (pSocketContext->m_Socket == INVALID_SOCKET) 
		return -1;

	// 2009.02.03 김밥
	// AttachSocket 하기 전에 BufferClear() 해준다. OnAccept() 에서 해주던거 여기로 옮김
	//{
		CConnection* pConnection = static_cast<CConnection*>(pSocketContext->GetParam());
		if( pConnection == NULL )
			return -1;

		pConnection->BufferClear();
	//}
	///////////////////////////////////////////////////////////////////////////////////////////////
	if( pConnection->GetIocpHandle() == INVALID_HANDLE_VALUE )	
	{
		HANDLE handle = CreateIoCompletionPort((HANDLE)pSocketContext->m_Socket, m_hIOCP, (ULONG_PTR)pSocketContext, 0);
		if (!handle) 
			return -1;
		pConnection->SetIocpHandle(handle);
	}

	int Ret = PostRecv(pSocketContext);
	pSocketContext->DelRef();

	return Ret;
}

int CIocpManager::DetachSocket(CSocketContext *pSocketContext, wchar_t *pwszIdent)
{
	if (pSocketContext == NULL)
	{
#ifdef _SERVICEMANAGER
		g_Log.Log(LogType::_FILELOG, L"[pSocketContext == NULL] %s\r\n", pwszIdent);
#endif
		return 0;
	}

	if (pSocketContext->DelRef() == 0)	
	{
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
		if( !pSocketContext->Detach() )
			return 0;	//이미 Detach 됐음
#endif
		bool bRet = DelSocket(pSocketContext);
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		if( bRet && pwszIdent && wcscmp( pwszIdent, L"PostSend") == 0 )
			return IN_DISCONNECT;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		std::wstring wstrDetachReason;
		pSocketContext->GetDetachReason(wstrDetachReason);
		if( wstrDetachReason.length() == 0)	
			pSocketContext->SetDetachReason(pwszIdent);
		OnDisconnected(pSocketContext);
	}

	return 0;
}

bool CIocpManager::DelSocket(CSocketContext *pSocketContext)
{
	if (pSocketContext->m_Socket != INVALID_SOCKET)
	{
		closesocket(pSocketContext->m_Socket);
		pSocketContext->m_Socket = INVALID_SOCKET;	
		return true;
	}	
	return false;
}

int CIocpManager::AddAcceptConnection(const int nKey, const int nPort, int nBackLog)
{
#ifdef _USE_ACCEPTEX
	UINT uiPoolSize = (nKey == CONNECTIONKEY_USER) ? g_Config.nIocpMax : 10;

	if( !CAcceptorEx::GetInstance().bCreateListenSocket( nPort, uiPoolSize, nKey ) )
		return -1;
#else
	if ( m_pAcceptor->Open(nKey, nPort, nBackLog) < 0 )
		return -1;
#endif // #ifdef _USE_ACCEPTEX
	return 0;
}

void CIocpManager::_OnConnect( CSocketContext* pSocketContext )
{
	// 연결성공인지 검사.
	int nSecond;
	int nByte = sizeof(nSecond);	
	int err = getsockopt( pSocketContext->m_Socket, SOL_SOCKET, SO_CONNECT_TIME, (char *)&nSecond, (PINT)&nByte );
	if ( err == NO_ERROR ) 
	{
		if (nSecond != 0xFFFFFFFF) // 0xFFFFFFFF는 연결 실패..
		{
			// 연결 성공
			CConnection* pConnection = static_cast<CConnection*>(pSocketContext->GetParam());
			if( !pConnection )
				return;

			setsockopt( pSocketContext->m_Socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0 );

			pSocketContext->m_RecvIO.mode = IOPOST_RECV;

			OnAccept(pSocketContext, pConnection->GetIp(), pConnection->GetPort());

			if (!pSocketContext->GetParam())
			{
				OnConnectFail(pSocketContext);
				closesocket(pSocketContext->m_Socket);
				ClearSocketContext(pSocketContext);
				return;
			}

			pSocketContext->IncRef();

			if (AttachSocket(pSocketContext) < 0)
			{
				OnConnectFail(pSocketContext);
				closesocket(pSocketContext->m_Socket);
				OnDisconnected(pSocketContext);				
				return;
			}

			pConnection->SetConnecting(false);
			OnConnected(pSocketContext);			
			return;
		}		
	}
	// 여기는 연결 실패.
	OnConnectFail(pSocketContext);
	closesocket(pSocketContext->m_Socket);
	ClearSocketContext(pSocketContext);
	return;
}

int CIocpManager::AddConnectionEx(CConnection* pCon, const int nKey, const char *pIp, const int nPort)
{
	SOCKET Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (Socket == INVALID_SOCKET) return -1;

	if (!SetSocketOption(Socket)) return -1;

	LPFN_CONNECTEX lpfnConnectEx = NULL;
	DWORD dwBytes = 0;
	GUID GuidConnectEx = WSAID_CONNECTEX;

	int nRet = ::WSAIoctl( Socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL);
	if( nRet == SOCKET_ERROR ) 
	{
		closesocket(Socket);
		return -1;
	}

	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;

	// 생성된 소켓과 bind..
	nRet = ::bind(Socket, (struct sockaddr*)&localAddr, sizeof(localAddr));
	if( nRet == SOCKET_ERROR ) 
	{
		closesocket(Socket);
		return -1;
	}

	sockaddr_in targetAddr;
	targetAddr.sin_family = AF_INET;
	targetAddr.sin_addr.s_addr = inet_addr(pIp);
	targetAddr.sin_port = htons(nPort);	

#ifdef _USE_ACCEPTEX
	CSocketContext* pSocketContext = CSocketContextMgr::GetInstance().pGetSocketContext();
	if( !pSocketContext )
	{
		closesocket( Socket );
		return -1;
	}
	pSocketContext->m_Socket		= Socket;
	pSocketContext->m_dwKeyParam	= nKey;
	pSocketContext->m_RecvIO.mode	= IOPOST_RECV;
#ifdef _USE_SENDCONTEXTPOOL
	for (int i = 0; i < (int)pSocketContext->m_SendIOList.size(); i++)
	{
		pSocketContext->m_SendIOList[i]->mode = IOPOST_SEND;
		pSocketContext->m_SendIOList[i]->Len	= 0;
	}
#else
	pSocketContext->m_SendIO.mode	= IOPOST_SEND;
	pSocketContext->m_SendIO.Len	= 0;
#endif

	pSocketContext->m_RecvIO.Len	= 0;

	pSocketContext->SetListenID( 1000 );
#else
	CSocketContext *pSocketContext = GetSocketContext(nKey, Socket, &targetAddr);
	if (!pSocketContext){
		closesocket(Socket);
		return -1;
	}
#endif

	pSocketContext->SetParam(pCon);

	HANDLE handle = CreateIoCompletionPort((HANDLE)Socket, m_hIOCP, (ULONG_PTR)pSocketContext, 0);
	if (!handle) 
	{
		closesocket(Socket);
		return -1;
	}
	pCon->SetIocpHandle(handle);

	//RecvIO로 걸자..
	pSocketContext->m_RecvIO.mode = IOPOST_CONNECT;
	BOOL bRet = lpfnConnectEx( Socket, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr), NULL, 0, NULL, (LPWSAOVERLAPPED)&pSocketContext->m_RecvIO);	
	if (bRet == FALSE)	{
		nRet = WSAGetLastError();
		if (nRet != WSA_IO_PENDING)
		{
			closesocket(Socket);
			PutSocketContext(pSocketContext);
			return -1;
		}
	}
	
	return 0;	
}

int CIocpManager::AddConnection(const int nKey, const char *pIp, const int nPort)
{
	SOCKET Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (Socket == INVALID_SOCKET) return -1;

	if (!SetSocketOption(Socket)) return -1;

	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(pIp);
	sa.sin_port = htons(nPort);

	if (connect(Socket, (sockaddr*)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR){
		closesocket(Socket);
		return -1;
	}

#ifdef _USE_ACCEPTEX
	CSocketContext* pSocketContext = CSocketContextMgr::GetInstance().pGetSocketContext();
	if( !pSocketContext )
	{
		closesocket( Socket );
		return -1;
	}
	pSocketContext->m_Socket		= Socket;
	pSocketContext->m_dwKeyParam	= nKey;
	pSocketContext->m_RecvIO.mode	= IOPOST_RECV;
#ifdef _USE_SENDCONTEXTPOOL
	for (int i = 0; i < (int)pSocketContext->m_SendIOList.size(); i++)
	{
		pSocketContext->m_SendIOList[i]->mode = IOPOST_SEND;
		pSocketContext->m_SendIOList[i]->Len	= 0;
	}
#else
	pSocketContext->m_SendIO.mode	= IOPOST_SEND;
	pSocketContext->m_SendIO.Len	= 0;
#endif
	
	pSocketContext->m_RecvIO.Len	= 0;

	pSocketContext->SetListenID( 1000 );
#else
	CSocketContext *pSocketContext = GetSocketContext(nKey, Socket, &sa);
	if (!pSocketContext){
		closesocket(Socket);
		return -1;
	}
#endif

	OnAccept(pSocketContext, pIp, nPort);

	if (!pSocketContext->GetParam()){
		closesocket(Socket);
		PutSocketContext(pSocketContext);
		return -1;
	}

	pSocketContext->IncRef();

	if (AttachSocket(pSocketContext) < 0){
		closesocket(Socket);
		OnDisconnected(pSocketContext);
		PutSocketContext(pSocketContext);
		return -1;
	}

	OnConnected(pSocketContext);

	return 0;
}

int CIocpManager::CreateConnection(const int nKey, const char * pIp, const int nPort, UINT nSessionKey)
{
	SOCKET Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (Socket == INVALID_SOCKET) return -1;

	if (!SetSocketOption(Socket)) return -1;

	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(pIp);
	sa.sin_port = htons(nPort);

	if (connect(Socket, (sockaddr*)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR){
		closesocket(Socket);
		return -1;
	}

	CSocketContext *pSocketContext = GetSocketContext(nKey, Socket, &sa);
	if (!pSocketContext){
		closesocket(Socket);
		return -1;
	}

	OnAccept(pSocketContext, pIp, nPort, nSessionKey);

	if (!pSocketContext->GetParam()){
		closesocket(Socket);
		PutSocketContext(pSocketContext);
		return -1;
	}

	pSocketContext->IncRef();

	if (AttachSocket(pSocketContext) < 0){
		closesocket(Socket);
		OnDisconnected(pSocketContext);
		PutSocketContext(pSocketContext);
		return -1;
	}
	return 0;
}

bool CIocpManager::SetSocketOption (SOCKET Socket)
{
	bool boNoDelay = true;
	if (setsockopt(Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR*)&boNoDelay, sizeof(boNoDelay)) == SOCKET_ERROR){
		return false;
	}

	LINGER Linger;
	Linger.l_onoff = 1;
	Linger.l_linger = 0;

	if (setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(LINGER)) == SOCKET_ERROR){
		return false;
	}

	int SendBuf = 51200;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)&SendBuf, sizeof(SendBuf)) == SOCKET_ERROR){
		return false;
	}
	return true;
}

int CIocpManager::PostRecv(CSocketContext *pSocketContext)
{
	if (pSocketContext->m_Socket == INVALID_SOCKET) return -1;
	if (pSocketContext->AddRef() == 0) return -1;

	WSABUF wsabuf;

	if( pSocketContext->m_RecvIO.Len >= INTERNALBUFFERLENMAX )
	{
		DetachSocket(pSocketContext, L"PostRecv-BufferOverflow");
		return -1;
	}

	wsabuf.buf = pSocketContext->m_RecvIO.buffer + pSocketContext->m_RecvIO.Len;
	wsabuf.len = INTERNALBUFFERLENMAX - pSocketContext->m_RecvIO.Len;

	DWORD dwBytesRecvd = 0, dwFlags = 0;
	int Ret = WSARecv(pSocketContext->m_Socket, &wsabuf, 1, &dwBytesRecvd, &dwFlags, (LPWSAOVERLAPPED)&pSocketContext->m_RecvIO, NULL);
	if (Ret == SOCKET_ERROR){
		Ret = WSAGetLastError();
		if (Ret != WSA_IO_PENDING){
			DetachSocket(pSocketContext, L"PostRecv");
			return -1;
		}
	}

	return 0;
}

#ifdef _USE_SENDCONTEXTPOOL
int CIocpManager::PostSend(CSocketContext *pSocketContext, TIOContext * pContext)
{
	//상위에서 sync하고 있음!
	if (pSocketContext->m_Socket == INVALID_SOCKET || pSocketContext->AddRef() == 0 || pContext->Len <= 0)
		return -1;

	//물리적인 센딩이 시도되는 부분입니다. async타입이면 완료메세지는 워커에서 처리됩니다.
	WSABUF wsabuf;
	wsabuf.buf = pContext->buffer;
	wsabuf.len = pContext->Len;

	DWORD dwBytesSent = 0, dwFlags = 0;
	int Ret = WSASend(pSocketContext->m_Socket, &wsabuf, 1, &dwBytesSent, dwFlags, (LPWSAOVERLAPPED)pContext, NULL);

	if (Ret == SOCKET_ERROR){
		Ret = WSAGetLastError();
		if (Ret != WSA_IO_PENDING){
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			int DetachRet = DetachSocket(pSocketContext, L"PostSend");
			if( DetachRet == IN_DISCONNECT )
				return IN_DISCONNECT;
#else
			DetachSocket(pSocketContext, L"PostSend");
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			return -1;
		}
	}

	//Test SendCount
	m_nRealSendSize += pContext->Len;

	InterlockedIncrement (&m_SendIOCount);	
	return 0;
}

void CIocpManager::SendComplete(CSocketContext *pSocketContext, TIOContext * pContext, int nSize)
{
	bool bDetach = false;
	{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
		if( Scope.bIsDelete() )
			return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

		CConnection *pCon = (CConnection*)pSocketContext->GetParam();

		DecSendIOCount();

		if (pCon && pCon->SendComplete(pContext, nSize) == false)
			bDetach = true;
	}

	if( bDetach )
		DetachSocket(pSocketContext, L"IOPOST_SEND");
}
#else
int CIocpManager::PostSend(CSocketContext *pSocketContext)
{
	//상위에서 sync하고 있음!
	if (pSocketContext->m_Socket == INVALID_SOCKET || pSocketContext->AddRef() == 0 || pSocketContext->m_SendIO.Len == 0)
		return -1;

	//물리적인 센딩이 시도되는 부분입니다. async타입이면 완료메세지는 워커에서 처리됩니다.
	WSABUF wsabuf;
	wsabuf.buf = pSocketContext->m_SendIO.buffer;
	wsabuf.len = pSocketContext->m_SendIO.Len;

	DWORD dwBytesSent = 0, dwFlags = 0;
	int Ret = WSASend(pSocketContext->m_Socket, &wsabuf, 1, &dwBytesSent, dwFlags, (LPWSAOVERLAPPED)&pSocketContext->m_SendIO, NULL);

	if (Ret == SOCKET_ERROR){
		Ret = WSAGetLastError();
		if (Ret != WSA_IO_PENDING){
			DetachSocket(pSocketContext, L"PostSend");
			return -1;
		}
	}

	//Test SendCount
	m_nRealSendSize += pSocketContext->m_SendIO.Len;

	InterlockedIncrement (&m_SendIOCount);	
	return 0;
}

void CIocpManager::SendComplete(CSocketContext *pSocketContext, int nSize)
{
	bool bDetach = false;
	{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
		if( Scope.bIsDelete() )
			return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

		CConnection *pCon = (CConnection*)pSocketContext->GetParam();

		DecSendIOCount();
		if (pCon && pCon->SendComplete(nSize) == false)
			bDetach = true;
	}
	
	if( bDetach )
		DetachSocket(pSocketContext, L"IOPOST_SEND");
}
#endif

void CIocpManager::PutSocketContext(CSocketContext *pSocketContext)
{
	m_SocketLock.Lock();
	if( std::find( m_SocketContexts.begin(), m_SocketContexts.end(), pSocketContext ) == m_SocketContexts.end() )
		m_SocketContexts.push_back(pSocketContext);
	m_SocketLock.UnLock();
}

CSocketContext* CIocpManager::GetSocketContext (int nKey, SOCKET Socket, sockaddr_in *pAddr)
{
	CSocketContext *pSocketContext = NULL;

	m_SocketLock.Lock();
	if (!m_SocketContexts.empty()){
		pSocketContext = m_SocketContexts.front();
		m_SocketContexts.pop_front();

		pSocketContext->m_Socket = Socket;
#ifdef _USE_SENDCONTEXTPOOL
		pSocketContext->m_RecvIO.mode = IOPOST_RECV;
		pSocketContext->m_RecvIO.Len = 0;
		
		//돌리자
		pSocketContext->m_SendIOLock.Lock();
		for (int i = 0; i < (int)pSocketContext->m_SendIOList.size(); i++)
		{
			pSocketContext->m_SendIOList[i]->mode = IOPOST_SEND;
			pSocketContext->m_SendIOList[i]->Len = 0;
		}
		pSocketContext->m_SendIOLock.UnLock();
#else
		pSocketContext->m_RecvIO.mode = IOPOST_RECV;
		pSocketContext->m_RecvIO.Len = 0;
		pSocketContext->m_SendIO.mode = IOPOST_SEND;
		pSocketContext->m_SendIO.Len = 0;
#endif
		pSocketContext->m_dwKeyParam = nKey;
	}
	else
	{
		_DANGER_POINT();
	}

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	if( pSocketContext )
		pSocketContext->m_lActiveCount = 0;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	if( pSocketContext )
		pSocketContext->m_lDetached = 0;
#endif	//#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)

	m_SocketLock.UnLock();

	return pSocketContext;
}

void CIocpManager::ClearSocketContext(CSocketContext *pSocketContext)
{
#ifdef _USE_ACCEPTEX
	return;
#else
	if( pSocketContext )
	{
		pSocketContext->Clear();
		PutSocketContext(pSocketContext);
	}
	else
	{
		_DANGER_POINT();
	}
#endif
}

int CIocpManager::GetSocketContextCount()
{
	int nCount = 0;
	m_SocketLock.Lock();
	nCount = static_cast<int>(m_SocketContexts.size());
	m_SocketLock.UnLock();

	return nCount;
}

void CIocpManager::ThreadStop()
{
	m_bThreadSwitch = false;

#ifdef _USE_ACCEPTEX
	int Count = m_uiWorkerThreadCount;
#else
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	int Count = (SysInfo.dwNumberOfProcessors * 2) + 1;
#endif

	for( int i=0 ; i<Count ; ++i)
		PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);

#ifdef _USE_ACCEPTEX
	for( int i=0 ; i<Count ; ++i )
	{
		WaitForSingleObject( m_vWorkerThreadHandle[i], INFINITE );
		CloseHandle( m_vWorkerThreadHandle[i] );
	}
#endif
}

void CIocpManager::CloseAcceptors()
{
#ifdef _USE_ACCEPTEX

	SAFE_DELETE( m_pSendThread );

	if( CAcceptorEx::GetInstancePtr() )
		delete CAcceptorEx::GetInstancePtr();

	if( CSocketContextMgr::GetInstancePtr() )
		delete CSocketContextMgr::GetInstancePtr();
#else
	if( m_pAcceptor)
		m_pAcceptor->Close();
#endif
}

void CIocpManager::AddSendCall( CSocketContext* pSocketContext )
{
	if( m_pSendThread )
		m_pSendThread->PushSendQueue( pSocketContext );
	else
		_DANGER_POINT();
}

#ifdef _USE_ACCEPTEX

#include <mswsock.h>

void CIocpManager::_OnAccept( CSocketContext* pSocketContext )
{
	sockaddr_in* pLocalAddr = NULL;
	sockaddr_in* pPeerAddr  = NULL;
	INT32 iLocalLen, iPeerLen;

	GetAcceptExSockaddrs( pSocketContext->m_RecvIO.buffer, 0, sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16, reinterpret_cast<sockaddr**>(&pLocalAddr), &iLocalLen, reinterpret_cast<sockaddr**>(&pPeerAddr), &iPeerLen );

	pSocketContext->OnAccept( pLocalAddr, pPeerAddr );
	
	SOCKET ListenSocket = CAcceptorEx::GetInstance().GetSocket( pSocketContext->uiGetListenID() );
	SOCKET socket		= pSocketContext->m_Socket;

	setsockopt( socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&ListenSocket), sizeof(SOCKET) );
	//CreateIoCompletionPort( (HANDLE)socket, m_hIOCP, (ULONG_PTR)pSocketContext, 0 );

	CAcceptorEx::GetInstance().bOnAccept( pSocketContext->uiGetListenID() );

	// KeepAlive 설정
	/*
	tcp_keepalive sKeepAlive;
	sKeepAlive.onoff				= 1;
	sKeepAlive.keepalivetime		= 10000;
	sKeepAlive.keepaliveinterval	= 1000;
	DWORD dwBytesReturned	= 0;
	WSAIoctl( socket, SIO_KEEPALIVE_VALS, &sKeepAlive, sizeof(sKeepAlive), 0, 0, &dwBytesReturned, 0, 0 );
	*/
	
	pSocketContext->m_RecvIO.mode	= IOPOST_RECV;	
#ifdef _USE_SENDCONTEXTPOOL
	for (int i = 0; i < (int)pSocketContext->m_SendIOList.size(); i++)
		pSocketContext->m_SendIOList[i]->mode = IOPOST_SEND;
#else
	pSocketContext->m_SendIO.mode	= IOPOST_SEND;
#endif
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	pSocketContext->m_lActiveCount = 0;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
	pSocketContext->m_lDetached = 0;
#endif	//#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)


	OnAccept( pSocketContext, inet_ntoa(pPeerAddr->sin_addr), ntohs(pPeerAddr->sin_port) );
	if( pSocketContext->GetParam() )
	{
		pSocketContext->IncRef();

		int Ret = AttachSocket( pSocketContext );
		if (Ret == 0)
		{
			OnConnected( pSocketContext );
		}
		else 
		{
			closesocket( socket );
			OnDisconnected( pSocketContext );
			pSocketContext->Clear();
			PutSocketContext( pSocketContext );
#if defined(_FINAL_BUILD)
			g_Log.Log( LogType::_NORMAL, L"[CIocpManager::_OnAccept] closesocket - AttachSocket\r\n");
#endif	// #if defined(_FINAL_BUILD)
		}
	}
	else 
	{
		closesocket( socket );
		PutSocketContext( pSocketContext );
#if defined(_FINAL_BUILD)
		g_Log.Log( LogType::_NORMAL, L"[CIocpManager::_OnAccept] closesocket - pSocketContext->GetParam()\r\n");
#endif	// #if defined(_FINAL_BUILD)
	}


	/*
	SOCKET sListenSocket = CAcceptor<T>::GetInstance().GetListenSocket();
	SOCKET sSocket = _pcOV->GetSocket();
	setsockopt( sSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&sListenSocket), sizeof(SOCKET) );
	CreateIoCompletionPort( (HANDLE)sSocket, m_hIOCP, (ULONG_PTR)_pcOV, 0 );

	CAcceptor<T>::GetInstance().OnAccept( _pcOV->stGetListenID() );

	Util::CS cs( &m_cConnectListCS );

	OnBeginningRecvPost( _pcOV );

	return m_fptFunc[Common::FuncPtr::eAccept_Index].Call( _pcOV, 0, 0 );
	*/
}

#endif

void CIocpManager::OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort)
{
}

void CIocpManager::OnConnected(CSocketContext *pSocketContext)
{
}

void CIocpManager::OnConnectFail(CSocketContext *pSocketContext)
{	
	CConnection* pConnection = static_cast<CConnection*>(pSocketContext->GetParam());
	if( !pConnection )
		return;

	pConnection->SetConnecting(false);
}

void CIocpManager::OnDisconnected(CSocketContext *pSocketContext)
{
	CConnection *pCon = (CConnection*)pSocketContext->GetParam();

	if (pCon){
		ClearSocketContext(pSocketContext);
		pCon->SetSocketContext(NULL, NULL);
		delete pCon;
		pCon = NULL;
	}
}

UINT __stdcall CIocpManager::WorkerThread(void *pParam)
{
	g_Log.Log(LogType::_FILELOG, L"[Thread-Start] WorkerThread - TID : %d\r\n", ::GetCurrentThreadId());

	CIocpManager *pIOCP = (CIocpManager*)pParam;
	HANDLE hIOCP = pIOCP->GetIocpHandle();
	int Ret = 0;
	CSocketContext *pSocketContext;
	TIOContext *pIOContext;
	DWORD dwBytesTransferred;

	while (pIOCP->m_bThreadSwitch)
	{
		__try {
			pIOCP->IncThreadCount();
			Ret = GetQueuedCompletionStatus(hIOCP, &dwBytesTransferred, (PULONG_PTR)&pSocketContext, (LPOVERLAPPED*)&pIOContext, INFINITE);
			pIOCP->DecThreadCount();

			if( Ret == FALSE )
			{
				if (pIOContext == NULL)
					continue;
				else if (pIOContext != NULL)
				{
	#ifdef _USE_ACCEPTEX
					if( !pSocketContext )
						continue;
	#endif
					int Error = WSAGetLastError();
					if( Error == ERROR_NETNAME_DELETED)
					{
						pIOCP->DetachSocket( pSocketContext, L"GQCSError" );
						continue;
					}
				}
			}

	#ifdef _USE_ACCEPTEX
			if( pIOContext && pIOContext->mode == IOPOST_ACCEPT )
				pSocketContext = pIOContext->pSocketContext;
	#endif
			if( !pSocketContext ) 
				return 0;
	#if defined(_USE_ACCEPTEX)		
			if( pIOContext->mode != IOPOST_CONNECT && pIOContext->mode != IOPOST_ACCEPT && dwBytesTransferred == 0)
	#else
			if( pIOContext->mode != IOPOST_CONNECT && dwBytesTransferred == 0)
	#endif
			{
				pIOCP->DetachSocket(pSocketContext, L"GQCS");
				continue;
			}

			switch (pIOContext->mode)
			{
	#ifdef _USE_ACCEPTEX
				case IOPOST_ACCEPT:
				{
					__try {
						pIOCP->_OnAccept( pSocketContext );
					}
					__except(CExceptionCodeLog(GetExceptionCode(), L"WorkerThread - IOPOST_ACCEPT"), EXCEPTION_CONTINUE_SEARCH) {	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
						// 이 코드 블럭은 절대 수행되지 않음
					}
					break;
				}
	#endif
				case IOPOST_CONNECT :
					{
						__try{
							pIOCP->_OnConnect( pSocketContext );
						}
						__except(CExceptionCodeLog(GetExceptionCode(), L"WorkerThread - IOPOST_CONNECT"), EXCEPTION_CONTINUE_SEARCH) {	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
							// 이 코드 블럭은 절대 수행되지 않음
						}
						break;
					}
				case IOPOST_RECV:
				{
					__try {
						if (&pSocketContext->m_RecvIO != pIOContext)
							continue;

						pSocketContext->m_RecvIO.Len += dwBytesTransferred;
						pIOCP->OnReceive(pSocketContext, dwBytesTransferred);
						pIOCP->PostRecv(pSocketContext);
						pIOCP->DetachSocket(pSocketContext, L"IOPOST_RECV");
					}
					__except(CExceptionCodeLog(GetExceptionCode(), L"WorkerThread - IOPOST_RECV"), EXCEPTION_CONTINUE_SEARCH) {	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
						// 이 코드 블럭은 절대 수행되지 않음
					}
					break;
				}

				case IOPOST_SEND:
				{
					__try {

	#ifdef _USE_SENDCONTEXTPOOL
						if (pSocketContext != pIOContext->pSocketContext)
							continue;

						pIOCP->SendComplete(pSocketContext, pIOContext, dwBytesTransferred);
	#else
						if (&pSocketContext->m_SendIO != pIOContext)
							continue;

						//Send는 SendThread에서 담당하고 Complete Sign은 workerthread에서 담당합니다.
						pIOCP->SendComplete(pSocketContext, dwBytesTransferred);
	#endif
					}
					__except(CExceptionCodeLog(GetExceptionCode(), L"WorkerThread - IOPOST_SEND"), EXCEPTION_CONTINUE_SEARCH) {	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
						// 이 코드 블럭은 절대 수행되지 않음
					}
					break;
				}
			}
		}
		__except(CExceptionCodeLog(GetExceptionCode(), L"WorkerThread"), EXCEPTION_CONTINUE_SEARCH) {	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
			// 이 코드 블럭은 절대 수행되지 않음
		}
	}

	g_Log.Log(LogType::_FILELOG, L"[Thread-End] WorkerThread - TID : %d\r\n", ::GetCurrentThreadId());

	return 0;
}

void CIocpManager::StoreMiniDump()
{
	WCHAR szFileName[256];
	WCHAR szHead[256];

	if (GetModuleFileName((HMODULE)GetModuleHandle(NULL), szFileName, 256) > 0)
	{
		if (wcschr(szFileName, '\\'))
			wcscpy(szHead, wcsrchr(szFileName, '\\') + 1);
		else
			wcscpy(szHead, szFileName);

		if (wcschr(szHead, '.'))
			wcscpy(wcschr(szHead, '.'), L"");
	}	else
		wcscpy(szHead, L"exe");

	_swprintf(szFileName, _T("%s.%d%d%d%d.dmp"), szHead, rand()%10, rand()%10, rand()%10, rand()%10);
	::StoreMiniDump(szFileName, MiniDumpWithFullMemory);
}

#ifdef _USE_ACCEPTEX

bool CIocpManager::bOnReUse( CSocketContext* pSocketContext )
{
	if( !CAcceptorEx::GetInstance().bOnClose( pSocketContext ) )
		return false;

	//g_Log.LogA( "CIocpManager::OnReUse() 소켓 재활용\r\n", pSocketContext->m_Socket );
	return true;
}

#endif

#include <Iprtrmib.h>

#define CXIP_A(IP)	((IP&0xFF000000)>>24)
#define CXIP_B(IP)	((IP&0x00FF0000)>>16)
#define CXIP_C(IP)	((IP&0x0000FF00)>>8)
#define CXIP_D(IP)	(IP&0x000000FF)

bool FileExists2(const char* path)
{
	std::ifstream my_file(path);
	if (my_file)
	{
		return true;
	}
	return false;
}

void CIocpManager::_GetHostIPAddress()
{
	DWORD dwPrivateIP		= 0;
	DWORD dwPrivateIPMask	= 0;
	DWORD dwPublicIP		= 0;
	DWORD dwPublicIPMask	= 0;

	if(FileExists2(".\\RLKT\\IP.ini") == true)
	{
		char PrivateIP[255];
		char PublicIP[255];
		printf("[RLKT]LOADING IP Data from file.\n");
		GetPrivateProfileStringA("GameServer","PublicIP","127.0.0.1",PublicIP,255,".\\RLKT\\IP.ini");
		GetPrivateProfileStringA("GameServer","PrivateIP","127.0.0.1",PrivateIP,255,".\\RLKT\\IP.ini");

		m_strPublicIP = PublicIP;
		m_strPrivateIP = PrivateIP;
		return;
	}

	HMODULE hIPHLP = LoadLibrary( _T("iphlpapi.dll") );
	if( hIPHLP )
	{
		typedef	BOOL (WINAPI * LPGIPT)(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize, BOOL bOrder);
		LPGIPT fnGetIpAddrTable=(LPGIPT)GetProcAddress(hIPHLP, "GetIpAddrTable");
		if( fnGetIpAddrTable )
		{
			PMIB_IPADDRTABLE pIPAddrTable;
			DWORD dwSize=0;

			pIPAddrTable=(MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
			if(!pIPAddrTable) 
			{
				FreeLibrary(hIPHLP);
				return;
			}

			if( fnGetIpAddrTable(pIPAddrTable, &dwSize, 0)==ERROR_INSUFFICIENT_BUFFER )
			{
				free(pIPAddrTable);
				pIPAddrTable=(MIB_IPADDRTABLE *)malloc(dwSize);
				if(!pIPAddrTable) 
				{
					FreeLibrary(hIPHLP);
					return;
				}
			}

			if( fnGetIpAddrTable(pIPAddrTable, &dwSize, 0) == NO_ERROR )
			{ 
				for( DWORD i=0; i<pIPAddrTable->dwNumEntries ; ++i )
				{
					DWORD	dwIP		= ntohl(pIPAddrTable->table[i].dwAddr);
					BOOL	bPrivate	= false;

					if(CXIP_A(dwIP)==127)
					{
						continue;
					}
					else if(CXIP_A(dwIP)==10)
					{
						bPrivate=true;
					}
					else if(CXIP_A(dwIP)==172)
					{
						if(CXIP_B(dwIP)>=16 && CXIP_B(dwIP)<=31) 
							bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==192)
					{
#if defined(_ID) // 인도네시아 VPN 192.168.2 대역은 그냥 패쓰합니다.
						if(CXIP_B(dwIP)==168)
						{
							if( CXIP_C(dwIP)==2)
								continue;
#else
						if(CXIP_B(dwIP)==168) 
						{						
#endif
							bPrivate=TRUE;
						}
					}

					if(bPrivate)
					{
						if( !dwPrivateIP || dwPrivateIP>dwIP )
						{
							dwPrivateIP=dwIP;
							dwPrivateIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						}
					}
					else
					{
						if( !dwPublicIP )
						{
							dwPublicIP=dwIP;
							dwPublicIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						}
					}

					if( dwPrivateIP &&  dwPublicIP) 
						break;
				}
			}
			else
			{
				FreeLibrary(hIPHLP);
				return;
			}

			BOOL bIPAdjust=FALSE;
			// Check Public IP
			if(dwPrivateIP && !dwPublicIP)
			{
				bIPAdjust=TRUE;

				for(DWORD i=0; i<pIPAddrTable->dwNumEntries; ++i)
				{
					DWORD dwIP=ntohl(pIPAddrTable->table[i].dwAddr);
					BOOL bPrivate=FALSE;

					if(CXIP_A(dwIP)==127)
					{
						continue;
					}
					else if(CXIP_A(dwIP)==10)
					{
						bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==172)
					{
						if(CXIP_B(dwIP)>=16 && CXIP_B(dwIP)<=31) 
							bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==192)
					{
#if defined(_ID) // 인도네시아 VPN 192.168.2 대역은 그냥 패쓰합니다.
						if(CXIP_B(dwIP)==168)
						{
							if( CXIP_C(dwIP)==2)
								continue;
#else
						if(CXIP_B(dwIP)==168) 
						{
#endif
							bPrivate=TRUE;
						}
					}

					if(bPrivate && dwPrivateIP!=dwIP)
					{
						dwPublicIP=dwIP;
						dwPublicIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						break;
					}
				}
			}
			// Check Not Found Public IP
			if(!dwPublicIP)
			{
				dwPublicIP		= dwPrivateIP;
				dwPublicIPMask	= dwPrivateIPMask;
			}
			else
			{
				if( bIPAdjust && dwPrivateIP>dwPublicIP )
				{
					DWORD dwIP		= dwPrivateIP;
					DWORD dwIPMask	= dwPrivateIPMask;

					dwPrivateIP		= dwPublicIP;
					dwPrivateIPMask	= dwPublicIPMask;
					dwPublicIP		= dwIP;
					dwPublicIPMask	= dwIPMask;
				}
			}

			// Clear
			free(pIPAddrTable);
		}
		else
		{
			FreeLibrary(hIPHLP);
			return;
		}
	}
	else
	{
		return;
	}

	FreeLibrary(hIPHLP);

	// Check IP
	if(!dwPrivateIP && !dwPublicIP)
		return;

	DWORD dwNPublicIP = htonl(dwPublicIP);
	m_strPublicIP = inet_ntoa( *((in_addr*)&dwNPublicIP) );

	DWORD dwNPrivateIP = htonl(dwPrivateIP);
	m_strPrivateIP = inet_ntoa( *((in_addr*)&dwNPrivateIP) );
}

#endif	// #if defined(_SERVER)