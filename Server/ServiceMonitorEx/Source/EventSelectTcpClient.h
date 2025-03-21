/*//===============================================================

	<< EVENTSELECT TCP CLIENT >>

	PRGM : B4nFter

	FILE : EVENTSELECTTCPCLIENT.H, EVENTSELECTTCPCLIENT.INL
	DESC : WSAEventSelect() I/O 모델을 이용하여 만든 TCP 클라이언트 모듈 (최대 64-2 개의 세션까지만 접속 가능함)
	INIT BUILT DATE : 2005. 11. 20
	LAST BUILT DATE : 2005. 11. 20

	P.S.>	
		- 템플릿 인자로 자신이 관리할 세션 클래스를 명시
		- 세션 클래스는 TSESSION 가 될 수는 없고 반드시 이를 상속받은 클래스이어야 함

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "EVENTSELECTTCPCLIENTSESSION.H"
#include "MEMPOOLEX.HPP"
#include "THREAD.H"
#include "VARARG.HPP"
#include <VECTOR>
#include <MAP>
#include <ALGORITHM>


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define	DF_ETCL_BASE_EVENT_COUNT				(2)		// WaitForMultipleEvents() 에서 기본으로 대기하는 이벤트 수 (종료이벤트, 핸들추가이벤트)
#define DF_ETCL_EVENT_END_THREAD				(0-WSA_WAIT_EVENT_0)
#define DF_ETCL_EVENT_ADD_HANDLE				(1-WSA_WAIT_EVENT_0)
#define DF_ETCL_DEFAULT_RECVBUFFER_SIZE			(4096)
#define DF_ETCL_DEFAULT_SENDBUFFER_SIZE			(4096)


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CEventSelectTcpClient
//*---------------------------------------------------------------
// DESC : EventSelect TCP 클라이언트 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
class CEventSelectTcpClient : public CWinsockBase
{

public:
	typedef struct EVENTDATA
	{
	public:
		EVENTDATA() {
			Reset();
		}
		VOID Reset() {
			::memset(m_hEventArray, 0, sizeof(m_hEventArray));
			::memset(m_lpEventSelectTcpClientSessionArray, 0, sizeof(m_lpEventSelectTcpClientSessionArray));
			m_iEventCount = 0;
		}
	public:
		HANDLE m_hEventArray[WSA_MAXIMUM_WAIT_EVENTS];
		CEventSelectTcpClientSession<TSESSION>* m_lpEventSelectTcpClientSessionArray[WSA_MAXIMUM_WAIT_EVENTS];
		INT m_iEventCount;
	} *LPEVENTDATA;

public:
	CEventSelectTcpClient();
	virtual ~CEventSelectTcpClient();

	BOOL Start(
		UINT uStackSize = 0, 
		CThread::LPFN_PREP_PROC lpfnPrepProc = NULL, 
		CThread::LPFN_POST_PROC lpfnPostProc = NULL, 
		LPVOID lpPrepProcParam = NULL, 
		LPVOID lpPostProcParam = NULL
		);
	VOID End();

	TSESSION* GetSession(
		LPFN_BASE_NOTIFY_PROC lpfnBaseNotifyProc, 
		LPFN_RECV_NOTIFY_PROC lpfnRecvNotifyProc,
		LPVOID lpBaseNotifyProcParam,
		LPVOID lpRecvNotifyProcParam,
		INT iRecvBufferSize = DF_ETCL_DEFAULT_RECVBUFFER_SIZE,
		INT iSendBufferSize = DF_ETCL_DEFAULT_SENDBUFFER_SIZE,
		INT iRecvSckBufferSize = -1,
		INT iSendSckBufferSize = -1
		);
	VOID ReleaseSession(TSESSION* lpEventSelectTcpClientSession);

	BOOL RegisterSession(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession);
	BOOL UnregisterSession(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession);

	VOID SetErrorNotifyProc(LPFN_ERROR_NOTIFY_PROC lpfnErrorNotifyProc, LPVOID lpErrorNotifyProcParam = NULL) { 
		m_lpfnErrorNotifyProc = lpfnErrorNotifyProc; 
		m_lpErrorNotifyProcParam = lpErrorNotifyProcParam;
	}
	LPFN_ERROR_NOTIFY_PROC GetErrorNotifyProc() { return m_lpfnErrorNotifyProc; }
	LPVOID GetErrorNotifyProcParam() { return m_lpErrorNotifyProcParam; }

private:
	BOOL Initialize();
	VOID Finalize();

protected:
	BOOL IoEventRecv(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession);
	BOOL IoEventSend(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession);

	VOID SetEventArray(LPHANDLE lphEventArray, CEventSelectTcpClientSession<TSESSION>** lpEventSelectTcpClientSessionArray, INT& iEventCount);
	VOID SetEventArrayNoLock(LPHANDLE lphEventArray, CEventSelectTcpClientSession<TSESSION>** lpEventSelectTcpClientSessionArray, INT& iEventCount);

	VOID SaveErrorMessage(INT iErrorCode, LPCTSTR lpszErrorMessage = NULL);

	static UINT __stdcall MainWorkerThreadProc(LPVOID lpParam);	// WaitForMultipleEvents() - CNNT / RECV / SEND / CLSE 작업스레드 프로시저

private:
	std::vector<CEventSelectTcpClientSession<TSESSION>*> m_vtEventSelectTcpClientSession;

	CCriticalSection m_crEventSelectTcpClient;

	CMemPoolEx<TSESSION, 16, 16> m_SessionMemoryPool;

	LPFN_ERROR_NOTIFY_PROC m_lpfnErrorNotifyProc;
	LPVOID m_lpErrorNotifyProcParam;

	HANDLE m_hEventEndThread;	// 작업 스레드를 종료한다는 이벤트
	HANDLE m_hEventAddHandle;	// 이벤트 핸들 배열에 이벤트를 추가한다는 이벤트
	CThread m_EventSelectMainWorkerThread;
	EVENTDATA m_EventData;
};


#include "EVENTSELECTTCPCLIENT.INL"
#include "EVENTSELECTTCPCLIENTSESSION.INL"

