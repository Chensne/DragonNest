

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::CEventSelectTcpClient
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
CEventSelectTcpClient<TSESSION>::CEventSelectTcpClient()
{
	Initialize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::~CEventSelectTcpClient
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
CEventSelectTcpClient<TSESSION>::~CEventSelectTcpClient()
{
	Finalize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::Initialize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 초기화
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::Initialize()
{
	m_lpfnErrorNotifyProc = NULL;
	m_lpErrorNotifyProcParam = NULL;
	m_hEventEndThread = NULL;
	m_hEventAddHandle = NULL;
	if (NOERROR != m_crEventSelectTcpClient.Open()) {
		BASE_RETURN(FALSE);
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::Finalize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 해제
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClient<TSESSION>::Finalize()
{
	End();
	m_crEventSelectTcpClient.Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::Start
//*---------------------------------------------------------------
// DESC : 객체의 기본 데이터를 세팅하고 사용가능한 상태로 세팅
// PARM :	1 . uStackSize - 스레드 스택크기 (기본값 0 → 1MB)
//			2 . m_lpPrepProc - 스레드 시작 시 실행되는 루틴 (기본값 NULL)
//			3 . m_lpPostProc - 스레드 종료 시 실행되는 루틴 (기본값 NULL)
//			4 . m_lpPrepProcParam - 스레드 시작 루틴에 전달되는 인자 (기본값 NULL)
//			5 . m_lpPostProcParam - 스레드 종료 루틴에 전달되는 인자 (기본값 NULL)
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::Start(
	UINT uStackSize, 
	CThread::LPFN_PREP_PROC lpfnPrepProc,
	CThread::LPFN_POST_PROC lpfnPostProc,
	LPVOID lpPrepProcParam,
	LPVOID lpPostProcParam
	)
{
	// 모듈 전체 동기화
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	if (m_hEventEndThread != NULL ||
		m_hEventAddHandle != NULL ||
		m_EventSelectMainWorkerThread.GetHandle()) {
		// 이미 Start() 되었음 ? → End() 이후 사용가능
		SaveErrorMessage(0, _T("m_EventSelectMainWorkerThread.GetHandle() - CEventSelectTcpClient::Start()"));
		BASE_RETURN(FALSE);
	}

	BOOL bRetVal = m_EventSelectMainWorkerThread.Start(
		MainWorkerThreadProc, this, CVarArg<64>(_T("EVTC-MAIN")), 
		uStackSize, FALSE, FALSE,
		lpfnPrepProc,
		lpfnPostProc,
		lpPrepProcParam,
		lpPostProcParam
		);
	if (!bRetVal) {		
		BASE_RETURN(FALSE);
	}
	bRetVal = m_EventSelectMainWorkerThread.SetPriority(THREAD_PRIORITY_NORMAL+2);
	if (!bRetVal) {		
		BASE_RETURN(FALSE);
	}

	m_hEventEndThread = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hEventEndThread) {
		SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClient::Start()"));
		BASE_RETURN(FALSE);
	}

	m_hEventAddHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hEventAddHandle) {
		SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClient::Start()"));
		BASE_RETURN(FALSE);
	}
		
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::End
//*---------------------------------------------------------------
// DESC : 객체의 기본 데이터를 해제하고 초기화
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClient<TSESSION>::End()
{
	// 모듈 전체 동기화
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	if (m_EventSelectMainWorkerThread.GetHandle()) {
		// 작업 스레드를 종료시킴
		::SetEvent(m_hEventEndThread);
		::WaitForSingleObject(m_EventSelectMainWorkerThread.GetHandle(), INFINITE);
		m_EventSelectMainWorkerThread .End();
	}

	if (m_hEventEndThread) {
		::CloseHandle(m_hEventEndThread);
		m_hEventEndThread = NULL;
	}
	if (m_hEventAddHandle) {
		::CloseHandle(m_hEventAddHandle);
		m_hEventAddHandle = NULL;
	}

	std::vector<CEventSelectTcpClientSession<TSESSION>*>::iterator it = m_vtEventSelectTcpClientSession.begin();
	for (; it != m_vtEventSelectTcpClientSession.end() ; ++it) {
		if ((*it)) {
			(*it)->Reset();
			(*it)->DeleteBuffer();
			m_SessionMemoryPool.ReleaseMemory(static_cast<TSESSION*>(*it));
		}
	}
	m_vtEventSelectTcpClientSession.clear();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::GetSession
//*---------------------------------------------------------------
// DESC : 사용 가능한 세션객체를 얻음
// PARM :	1 . lpfnBaseNotifyProc - 기본 이벤트 (연결, 종료, 에러) 를 통보받을 함수 포인터
//			2 . lpfnRecvNotifyProc - 수신 이벤트를 통보받을 함수 포인터
//			3 . lpBaseNotifyProcParam - 기본 이벤트 사용자 파라메터
//			4 . lpRecvNotifyProcParam - 수신 이벤트 사용자 파라메터
//			5 . iRecvAppBufferSize - 세션의 수신버퍼 크기 (응용)
//			6 . iSendAppBufferSize - 세션의 송신버퍼 크기 (응용)
//			7 . iRecvSckBufferSize - 세션의 수신버퍼 크기 (소켓)
//			8 . iSendSckBufferSize - 세션의 송신버퍼 크기 (소켓)
// RETV : 세션 객체 포인터
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
TSESSION* CEventSelectTcpClient<TSESSION>::GetSession(	
	LPFN_BASE_NOTIFY_PROC lpfnBaseNotifyProc, 
	LPFN_RECV_NOTIFY_PROC lpfnRecvNotifyProc,
	LPVOID lpBaseNotifyProcParam,
	LPVOID lpRecvNotifyProcParam,
	INT iRecvAppBufferSize,
	INT iSendAppBufferSize,
	INT iRecvSckBufferSize,
	INT iSendSckBufferSize
	)
{
	if (!lpfnBaseNotifyProc || !lpfnRecvNotifyProc) {
		// 에러
		SaveErrorMessage(0, _T("!lpfnBaseNotifyProc || !lpfnRecvNotifyProc - CEventSelectTcpClient::GetSession()"));
		return NULL;
	}

	if (iRecvAppBufferSize <= 0 || iSendAppBufferSize <= 0) {
		// 에러
		SaveErrorMessage(0, _T("iRecvAppBufferSize <= 0 || iSendAppBufferSize <= 0 - CEventSelectTcpClient::GetSession()"));
		return NULL;
	}

	// 모듈 전체 동기화
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	TSESSION* lpEventSelectTcpClientSession = m_SessionMemoryPool.GetMemory();
	if (!lpEventSelectTcpClientSession) {
		// 에러
		SaveErrorMessage(0, _T("!lpEventSelectTcpClientSession - CEventSelectTcpClient::GetSession()"));
		BASE_RETURN(NULL);
	}
	CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession2 = static_cast<CEventSelectTcpClientSession<TSESSION>*>(lpEventSelectTcpClientSession);

	// 아직 사용자에게 전달되지 않은 상태이므로 세션 동기화 필요없음

	lpEventSelectTcpClientSession2->SetIsActivated(TRUE);
	lpEventSelectTcpClientSession2->Reset();
	lpEventSelectTcpClientSession2->SetEventSelectTcpClientPtr(this);
	lpEventSelectTcpClientSession2->SetBaseNotifyProc(lpfnBaseNotifyProc, lpBaseNotifyProcParam);
	lpEventSelectTcpClientSession2->SetRecvNotifyProc(lpfnRecvNotifyProc, lpRecvNotifyProcParam);
	lpEventSelectTcpClientSession2->SetSckBufferSize(iRecvSckBufferSize, iSendSckBufferSize);

	if (lpEventSelectTcpClientSession2->CreateBuffer(iRecvAppBufferSize, iSendAppBufferSize) == FALSE) {
		// 에러
		SaveErrorMessage(0, _T("lpEventSelectTcpClientSession2->CreateBuffer() == FALSE - CEventSelectTcpClient::GetSession()"));
		BASE_RETURN(NULL);
	}

	return lpEventSelectTcpClientSession;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::ReleaseSession
//*---------------------------------------------------------------
// DESC : 사용이 끝난 세션객체를 반환
// PARM :	1 . lpEventSelectTcpClientSession - 세션 객체 포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClient<TSESSION>::ReleaseSession(TSESSION* lpEventSelectTcpClientSession)
{
	if (!lpEventSelectTcpClientSession) {
		SaveErrorMessage(0, _T("!lpEventSelectTcpClientSession - CEventSelectTcpClient::ReleaseSession()"));
		return;
	}
	CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession2 = static_cast<CEventSelectTcpClientSession<TSESSION>*>(lpEventSelectTcpClientSession);
	
	// 모듈 전체 동기화
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	// 세션 보호 → 이 함수를 호출하는 쪽에서 이루어짐 (필요할 경우에만) - 대부분 TSESSION::Disconnect() 이후에 반환하므로 큰 필요는 없을듯

	if (lpEventSelectTcpClientSession2->GetIsActivated() == FALSE) {
		// 에러
		SaveErrorMessage(0, _T("lpEventSelectTcpClientSession2->GetIsActivated() == FALSE - CEventSelectTcpClient::ReleaseSession()"));
		return;
	}

	lpEventSelectTcpClientSession2->SetIsActivated(FALSE);
	lpEventSelectTcpClientSession2->Reset();
	lpEventSelectTcpClientSession2->DeleteBuffer();

	m_SessionMemoryPool.ReleaseMemory(lpEventSelectTcpClientSession);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::RegisterSession
//*---------------------------------------------------------------
// DESC : 연결 후 소켓이벤트를 받을 수 있도록 세션객체를 관리자 객체에 등록 (자동으로 수행됨)
// PARM :	1 . lpEventSelectTcpClientSession - 세션 객체 포인터
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::RegisterSession(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{
	// 세션 보호 → 이 함수를 호출하는 쪽에서 이루어짐

	if (!lpEventSelectTcpClientSession) {
		SaveErrorMessage(0, _T("!lpEventSelectTcpClientSession - CEventSelectTcpClient::RegisterSession()"));
		return FALSE;
	}

	if (lpEventSelectTcpClientSession->GetEventHandle() == NULL) {
		SaveErrorMessage(0, _T("lpEventSelectTcpClientSession->GetEventHandle() == NULL - CEventSelectTcpClient::RegisterSession()"));
		return FALSE;
	}

	// 모듈 전체 동기화
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	// 세션의 핸들과 포인터를 관리용 벡터에 등록한다.
	std::vector<CEventSelectTcpClientSession<TSESSION>*>::iterator it = std::find(m_vtEventSelectTcpClientSession.begin(), m_vtEventSelectTcpClientSession.end(), lpEventSelectTcpClientSession);
	if (it == m_vtEventSelectTcpClientSession.end()) {
		m_vtEventSelectTcpClientSession.push_back(lpEventSelectTcpClientSession);
	}
	else {
		// 에러 - 핸들이 존재할 수 없다. 혹시라도 이런 경우가 발생하는지 체크해 보아야 한다.
		SaveErrorMessage(0, _T("it != m_vtEventSelectTcpClientSession.end() - CEventSelectTcpClient::RegisterSession()"));
	}

	// 작업 스레드에 핸들이 추가되었다는 이벤트를 보낸다.
	if (m_hEventAddHandle) {
		::SetEvent(m_hEventAddHandle);
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::UnregisterSession
//*---------------------------------------------------------------
// DESC : 종료 후 소켓이벤트를 받지 않도록 세션객체를 관리자 객체에서 등록해제 (자동으로 수행됨)
// PARM :	1 . lpEventSelectTcpClientSession - 세션 객체 포인터
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::UnregisterSession(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{
	// 세션 보호 → 이 함수를 호출하는 쪽에서 이루어짐

	if (!lpEventSelectTcpClientSession) {
		SaveErrorMessage(0, _T("!lpEventSelectTcpClientSession - CEventSelectTcpClient::UnregisterSession()"));
		return FALSE;
	}

	if (lpEventSelectTcpClientSession->GetEventHandle() == NULL) {
		SaveErrorMessage(0, _T("lpEventSelectTcpClientSession->GetEventHandle() == NULL - CEventSelectTcpClient::UnregisterSession()"));
		return FALSE;
	}

	// 모듈 전체 동기화
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	// 벡터에서 세션에 연결된 이벤트 핸들을 찾아서 제거한다.
	std::vector<CEventSelectTcpClientSession<TSESSION>*>::iterator it = std::find(m_vtEventSelectTcpClientSession.begin(), m_vtEventSelectTcpClientSession.end(), lpEventSelectTcpClientSession);
	if (it != m_vtEventSelectTcpClientSession.end()) {
		m_vtEventSelectTcpClientSession.erase(it);
	}
	else {
		// 에러 - 핸들이 존재해야 한다. 혹시라도 이런 경우가 발생하는지 체크해 보아야 한다.
		SaveErrorMessage(0, _T("it == m_vtEventSelectTcpClientSession.end() - CEventSelectTcpClient::UnregisterSession()"));
	}

	// 작업 스레드에 핸들이 제거되었다는 이벤트를 보낸다.
	if (m_hEventAddHandle) {
		::SetEvent(m_hEventAddHandle);
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::IoEventRecv
//*---------------------------------------------------------------
// DESC : 특정 세션으로 통보된 수신 이벤트를 처리
// PARM :	1 . lpEventSelectTcpClientSession - 세션 객체 포인터
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::IoEventRecv(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{
	// RECV 버퍼에 데이터를 받아서 사용자에게 통보한다.

	// 세션 동기화 → RECV 이벤트가 발생하는 스레드가 하나이므로 소켓 보호를 위해 WSARecv() 부분만 동기화
//	CLockAuto AutoLock(lpEventSelectTcpClientSession->GetCriticalSection());

	if (lpEventSelectTcpClientSession->RecvData() == FALSE) {
		// 에러 - SendLeftData() 내부에서 종료처리 하였음
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::IoEventSend
//*---------------------------------------------------------------
// DESC : 특정 세션으로 통보된 송신종료 이벤트를 처리
// PARM :	1 . lpEventSelectTcpClientSession - 세션 객체 포인터
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::IoEventSend(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{

	// 만약 이전에 보내지 못한 데이 터가 있으면 보낸다.

	// 세션 동기화
	CLockAuto AutoLock(lpEventSelectTcpClientSession->GetLock());

	if (lpEventSelectTcpClientSession->SendLeftData() == SOCKET_ERROR) {
		// 에러 - SendLeftData() 내부에서 종료처리 하였음
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::SetEventArray
//*---------------------------------------------------------------
// DESC : 이벤트 배열을 갱신
// PARM :	1 . lphEventArray - 이벤트 핸들
//			2 . lpEventSelectTcpClientSessionArray - 이벤트 배열 포인터
//			3 . iEventCount - 등록된 이벤트 개수
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClient<TSESSION>::SetEventArray(LPHANDLE lphEventArray, CEventSelectTcpClientSession<TSESSION>** lpEventSelectTcpClientSessionArray, INT& iEventCount)
{
	// 모듈 전체 동기화
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	if (!lphEventArray || !lpEventSelectTcpClientSessionArray) {
		return;
	}

	lphEventArray[0] = m_hEventEndThread;
	lphEventArray[1] = m_hEventAddHandle;
	lpEventSelectTcpClientSessionArray[0] = NULL;
	lpEventSelectTcpClientSessionArray[1] = NULL;
	iEventCount = DF_ETCL_BASE_EVENT_COUNT;

	if (m_vtEventSelectTcpClientSession.empty()) {
		// 빈번하게 발생
//		BASE_TRACE(_T("m_vtEventSelectTcpClientSession.empty() - CEventSelectTcpClient::SetEventArray()"));
		return;
	}

	iEventCount += (INT)m_vtEventSelectTcpClientSession.size();
	if (iEventCount >= WSA_MAXIMUM_WAIT_EVENTS) {
		// 이런 경우가 발생하는지 체크해 보아야 한다.
		iEventCount = WSA_MAXIMUM_WAIT_EVENTS;
		BASE_TRACE(_T("iEventCount >= WSA_MAXIMUM_WAIT_EVENTS - CEventSelectTcpClient::SetEventArray()"));
	}

	for (INT iIndex = DF_ETCL_BASE_EVENT_COUNT; iIndex < iEventCount ; ++iIndex) {
		lphEventArray[iIndex] = m_vtEventSelectTcpClientSession[iIndex-DF_ETCL_BASE_EVENT_COUNT]->GetEventHandle();
		lpEventSelectTcpClientSessionArray[iIndex] = m_vtEventSelectTcpClientSession[iIndex-DF_ETCL_BASE_EVENT_COUNT];
		if (lphEventArray[iIndex] == NULL) {
			// 에러 - 이런 경우가 생기는지 확인해 보아야 한다.
			BASE_TRACE(_T("lphEventArray[%d] == NULL - CEventSelectTcpClient::SetEventArray()"), iIndex);
			std::vector<CEventSelectTcpClientSession<TSESSION>*>::iterator it = m_vtEventSelectTcpClientSession.begin() + iIndex - DF_ETCL_BASE_EVENT_COUNT;
			m_vtEventSelectTcpClientSession.erase(it);
			--iIndex;
			--iEventCount;
		}
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::SaveErrorMessage
//*---------------------------------------------------------------
// DESC : 클래스 내부에 에러메시지를 저장, 사용자에게 알려줌
// PARM :	1 . iErrorCode - 시스템 에러코드
//			2 . lpszErrorMessage - 추가적인 에러메시지
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClient<TSESSION>::SaveErrorMessage(INT iErrorCode, LPCTSTR lpszErrorMessage)
{
	if (m_lpfnErrorNotifyProc) {
		m_lpfnErrorNotifyProc(this, iErrorCode, m_ErrorFormat.Write(iErrorCode, lpszErrorMessage), m_lpErrorNotifyProcParam);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::MainWorkerThreadProc
//*---------------------------------------------------------------
// DESC : 이벤트 처리를 진행할 메인 작업 스레드 프로시저
// PARM :	1 . lpParam - 관리자 객체의 포인터
// RETV : 스레드 종료코드 (0:OK)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
UINT __stdcall CEventSelectTcpClient<TSESSION>::MainWorkerThreadProc(LPVOID lpParam)
{
	CEventSelectTcpClient* lpEventSelectTcpClient = (CEventSelectTcpClient*) lpParam;
	if (lpEventSelectTcpClient == NULL) {
		return 0;
	}

	HANDLE hEventArray[WSA_MAXIMUM_WAIT_EVENTS];
	CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSessionArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSANETWORKEVENTS stWsaNetworkEvents;
	INT iEventCount;
	INT iEventIndex;
	INT iRetVal;
	INT iErrNo;

	BOOL bEndThread = FALSE;

	// 이벤트배열을 세팅한다.
	lpEventSelectTcpClient->SetEventArray(hEventArray, lpEventSelectTcpClientSessionArray, iEventCount);

	while(!bEndThread) {
		iRetVal = ::WSAWaitForMultipleEvents(
			iEventCount,
			hEventArray,
			FALSE,
			WSA_INFINITE,
			FALSE
			);
		if (iRetVal == WSA_WAIT_FAILED) {
			iRetVal = ::WSAGetLastError();
			switch(iRetVal) {
			case ERROR_INVALID_HANDLE :	// 잘못된 핸들이 섞여있었다면 여기서 핸들테이블을 다시 세팅한다.
				lpEventSelectTcpClient->SetEventArray(hEventArray, lpEventSelectTcpClientSessionArray, iEventCount);
				continue;
			default :
				continue;
			}
		}

		iEventIndex = iRetVal - WSA_WAIT_EVENT_0;
		if (iEventIndex >= WSA_MAXIMUM_WAIT_EVENTS) {
			// 에러
			continue;
		}

		switch(iEventIndex) {
		case DF_ETCL_EVENT_END_THREAD:	// 작업스레드 종료
			bEndThread = TRUE;
			continue;
		case DF_ETCL_EVENT_ADD_HANDLE:	// 이벤트핸들 배열 추가/삭제 로 인한 변경
			lpEventSelectTcpClient->SetEventArray(hEventArray, lpEventSelectTcpClientSessionArray, iEventCount);
			continue;
		}

		// 일반적인 소켓 이벤트 임
		if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetSocket() == INVALID_SOCKET) {
			continue;
		}
		iErrNo = ::WSAEnumNetworkEvents(
			lpEventSelectTcpClientSessionArray[iEventIndex]->GetSocket(),
			hEventArray[iEventIndex],
			&stWsaNetworkEvents
			);
		if (iErrNo == SOCKET_ERROR) {
			// 에러
			lpEventSelectTcpClient->SaveErrorMessage(::WSAGetLastError(), _T("WSAEnumNetworkEvents() - CEventSelectTcpClient::MainWorkerThreadProc()"));
			continue;
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_CONNECT) {
			if (stWsaNetworkEvents.iErrorCode[FD_CONNECT_BIT] != 0) {
				// 에러
				if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
						EV_NBN_ERROR, 
						lpEventSelectTcpClientSessionArray[iEventIndex], 
						(LPVOID)lpEventSelectTcpClient->m_ErrorFormat.Write(stWsaNetworkEvents.iErrorCode[FD_CONNECT_BIT], _T("stWsaNetworkEvents.iErrorCode[FD_CONNECT_BIT] != 0 - CEventSelectTcpClient::MainWorkerThreadProc()")), 
						lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
						);
				}
				// 세션을 종료시키고 제거함
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}

			// FD_CONNECT 작업

			// 새로운 이벤트를 등록한다.
			iErrNo = ::WSAEventSelect(lpEventSelectTcpClientSessionArray[iEventIndex]->GetSocket(),
				hEventArray[iEventIndex],
				FD_READ | FD_WRITE | FD_CLOSE
				);
			if (iErrNo == SOCKET_ERROR) {
				lpEventSelectTcpClient->SaveErrorMessage(::WSAGetLastError(), _T("WSAEventSelect() - CEventSelectTcpClient::MainWorkerThreadProc()"));
				// 세션을 종료시키고 제거함
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}
			// 사용자에게 세션이 연결되었다는 것을 알린다.
			if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
				lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(EV_NBN_SESSCNNT, lpEventSelectTcpClientSessionArray[iEventIndex], (LPVOID)NULL, lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam());
			}

			// 단독 작업이므로 다음으로 넘어감
			continue;
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_READ) {
			if (stWsaNetworkEvents.iErrorCode[FD_READ_BIT] != 0) {
				// 에러
				if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
						EV_NBN_ERROR, 
						lpEventSelectTcpClientSessionArray[iEventIndex], 
						(LPVOID)lpEventSelectTcpClient->m_ErrorFormat.Write(stWsaNetworkEvents.iErrorCode[FD_READ_BIT], _T("stWsaNetworkEvents.iErrorCode[FD_READ_BIT] != 0 - CEventSelectTcpClient::MainWorkerThreadProc()")), 
						lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
						);
				}
				// 세션을 종료시키고 제거함
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}

			// FD_READ 작업
			if (lpEventSelectTcpClient->IoEventRecv(lpEventSelectTcpClientSessionArray[iEventIndex]) == FALSE) {
				continue;
			}
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_WRITE) {
			if (stWsaNetworkEvents.iErrorCode[FD_WRITE_BIT] != 0) {
				// 에러
				if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
						EV_NBN_ERROR, 
						lpEventSelectTcpClientSessionArray[iEventIndex], 
						(LPVOID)lpEventSelectTcpClient->m_ErrorFormat.Write(stWsaNetworkEvents.iErrorCode[FD_WRITE_BIT], _T("stWsaNetworkEvents.iErrorCode[FD_WRITE_BIT] != 0 - CEventSelectTcpClient::MainWorkerThreadProc()")), 
						lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
						);
				}
				// 세션을 종료시키고 제거함
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}

			// FD_WRITE 작업
			if (lpEventSelectTcpClient->IoEventSend(lpEventSelectTcpClientSessionArray[iEventIndex]) == FALSE) {
				continue;
			}
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_CLOSE) {
			if (stWsaNetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
				// 에러
/*
				if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
						EV_NBN_ERROR, 
						lpEventSelectTcpClientSessionArray[iEventIndex], 
						(LPVOID)lpEventSelectTcpClient->m_ErrorFormat.Write(stWsaNetworkEvents.iErrorCode[FD_CLOSE_BIT], _T("stWsaNetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0 - CEventSelectTcpClient::MainWorkerThreadProc()")), 
						lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
						);
				}
				continue;
*/
			}
			// 사용자에게 세션이 종료되었다는 것을 알린다.
			if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
				lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
					EV_NBN_SESSDCNT, 
					lpEventSelectTcpClientSessionArray[iEventIndex], 
					(LPVOID)NULL, 
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
					);
			}
			// 세션을 종료시키고 제거함
			lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
		}
	}

	return 0;
}

