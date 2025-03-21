/*//===============================================================

	<< EVENTSELECT TCP CLIENT SESSION >>

	PRGM : B4nFter

	FILE : EVENTSELECTTCPCLIENTSESSION.H, EVENTSELECTTCPCLIENTSESSION.INL
	DESC : CEventSelectTcpClient 클래스의 관리를 받는 세션 클래스
	INIT BUILT DATE : 2005. 11. 21
	LAST BUILT DATE : 2005. 11. 21

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "WINSOCKBASE.HPP"
#include "SESSIONBASE.HPP"
#include "IOBUFFER.HPP"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

template<class TSESSION>
class CEventSelectTcpClient;


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CEventSelectTcpClientSession
//*---------------------------------------------------------------
// DESC : EventSelect TCP 클라이언트 세션 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
class CEventSelectTcpClientSession : public CSessionBase
{

public:
	typedef	CSessionBase	TP_SUPER;

public:
	CEventSelectTcpClientSession<TSESSION>();
	virtual ~CEventSelectTcpClientSession<TSESSION>();

	virtual VOID Reset();

	BOOL Connect(LPCSTR lpszIpAddress, WORD wPortNumber);
	BOOL Connect(ULONG uIpAddress, WORD wPortNumber);
	VOID Disconnect();

	BOOL CreateBuffer(INT iRecvBufferSize, INT iSendBufferSize);
	VOID DeleteBuffer();

	BOOL CreateSocket();

	BOOL RecvData();
	BOOL SendData(LPBYTE lpbtBuffer, INT iBufferSize, BOOL bDoLock = TRUE);
	INT SendLeftData();

	VOID SetEventHandle(HANDLE hEvent) { m_hEvent = hEvent; }
	HANDLE GetEventHandle() { return m_hEvent; }

	BOOL GetIsActivated() const { return m_bIsActivated; }
	VOID SetIsActivated(BOOL bIsActivated) { m_bIsActivated = bIsActivated; }

	VOID SetEventSelectTcpClientPtr(CEventSelectTcpClient<TSESSION>* lpEventSelectTcpClient) { m_lpEventSelectTcpClient = lpEventSelectTcpClient;	}
	CEventSelectTcpClient<TSESSION>* GetEventSelectTcpClientPtr() { return m_lpEventSelectTcpClient; }

	VOID SetRemoteSockAddrIn(SOCKADDR_IN& stRemoteSockAddrIn) { m_stRemoteSockAddrIn = stRemoteSockAddrIn; }
	const SOCKADDR_IN& GetRemoteSockAddrIn() const { return m_stRemoteSockAddrIn;	}
	ULONG GetRemoteIpAddressN() const { return m_stRemoteSockAddrIn.sin_addr.s_addr; }
	ULONG GetRemoteIpAddressH() const { return ::ntohl(m_stRemoteSockAddrIn.sin_addr.s_addr); }
	WORD GetRemotePortNumberN() const { return m_stRemoteSockAddrIn.sin_port; }
	WORD GetRemotePortNumberH() const { return ::ntohs(m_stRemoteSockAddrIn.sin_port); }
	LPCSTR GetRemoteIpAddressString() const { return ::inet_ntoa(m_stRemoteSockAddrIn.sin_addr);}

	CIoBuffer* GetRecvIoBuffer() { return &m_RecvIoBuffer; }
	CIoBuffer* GetSendIoBuffer() { return &m_SendIoBuffer; }

	VOID SetBaseNotifyProc(LPFN_BASE_NOTIFY_PROC lpfnBaseNotifyProc, LPVOID lpBaseNotifyProcParam) { 
		m_lpfnBaseNotifyProc = lpfnBaseNotifyProc;
		m_lpBaseNotifyProcParam = lpBaseNotifyProcParam;
	}
	LPFN_BASE_NOTIFY_PROC GetBaseNotifyProc() { return m_lpfnBaseNotifyProc; }
	LPVOID GetBaseNotifyProcParam() { return m_lpBaseNotifyProcParam; }

	VOID SetRecvNotifyProc(LPFN_RECV_NOTIFY_PROC lpfnRecvNotifyProc, LPVOID lpRecvNotifyProcParam) { 
		m_lpfnRecvNotifyProc = lpfnRecvNotifyProc; 
		m_lpRecvNotifyProcParam = lpRecvNotifyProcParam;
	}
	LPFN_RECV_NOTIFY_PROC GetRecvNotifyProc() { return m_lpfnRecvNotifyProc; }
	LPVOID GetRecvNotifyProcParam() { return m_lpRecvNotifyProcParam; }

	// !!! 주의 - CSessionBase 로 부터의 순수가상 함수 잔여부분 정의 (형식상이며 사용하지 않음)
#pragma warning (disable:4100)
	BOOL SendData(LPBYTE lpbtBuffer, INT iBufferSize, LPCSTR lpszIpAddress, WORD wPortNumber, BOOL bDoLock = TRUE)	{ UNUSED_ALWAYS(lpbtBuffer), UNUSED_ALWAYS(iBufferSize), UNUSED_ALWAYS(lpszIpAddress), UNUSED_ALWAYS(wPortNumber), UNUSED_ALWAYS(bDoLock); return FALSE; }
	BOOL SendData(LPBYTE lpbtBuffer, INT iBufferSize, ULONG uIpAddress, WORD wPortNumber, BOOL bDoLock = TRUE)		{ UNUSED_ALWAYS(lpbtBuffer), UNUSED_ALWAYS(iBufferSize), UNUSED_ALWAYS(uIpAddress), UNUSED_ALWAYS(wPortNumber), UNUSED_ALWAYS(bDoLock); return FALSE; }
#pragma warning (default:4100)

private:
	BOOL Initialize();
	VOID Finalize();
	VOID CloseSocket();

protected:
	VOID DisconnectNoLock();

	VOID MoveLeftRecvBufferToHead();
	VOID MoveLeftSendBufferToHead();

	VOID SaveErrorMessage(INT iErrorCode, LPCTSTR lpszErrorMessage = NULL);

private:
	HANDLE m_hEvent;
	CEventSelectTcpClient<TSESSION>* m_lpEventSelectTcpClient;
	SOCKADDR_IN m_stRemoteSockAddrIn;

	CIoBuffer m_RecvIoBuffer;
	CIoBuffer m_SendIoBuffer;

	BOOL m_bIsActivated;

	LPFN_BASE_NOTIFY_PROC m_lpfnBaseNotifyProc;
	LPFN_RECV_NOTIFY_PROC m_lpfnRecvNotifyProc;
	LPVOID m_lpBaseNotifyProcParam;
	LPVOID m_lpRecvNotifyProcParam;
};

