

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
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : ��ü ���� �ڿ����� �ʱ�ȭ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ��ü ���� �ڿ����� ����
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
// DESC : ��ü�� �⺻ �����͸� �����ϰ� ��밡���� ���·� ����
// PARM :	1 . uStackSize - ������ ����ũ�� (�⺻�� 0 �� 1MB)
//			2 . m_lpPrepProc - ������ ���� �� ����Ǵ� ��ƾ (�⺻�� NULL)
//			3 . m_lpPostProc - ������ ���� �� ����Ǵ� ��ƾ (�⺻�� NULL)
//			4 . m_lpPrepProcParam - ������ ���� ��ƾ�� ���޵Ǵ� ���� (�⺻�� NULL)
//			5 . m_lpPostProcParam - ������ ���� ��ƾ�� ���޵Ǵ� ���� (�⺻�� NULL)
// RETV : TRUE - ���� / FALSE - ����
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
	// ��� ��ü ����ȭ
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	if (m_hEventEndThread != NULL ||
		m_hEventAddHandle != NULL ||
		m_EventSelectMainWorkerThread.GetHandle()) {
		// �̹� Start() �Ǿ��� ? �� End() ���� ��밡��
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
// DESC : ��ü�� �⺻ �����͸� �����ϰ� �ʱ�ȭ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClient<TSESSION>::End()
{
	// ��� ��ü ����ȭ
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	if (m_EventSelectMainWorkerThread.GetHandle()) {
		// �۾� �����带 �����Ŵ
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
// DESC : ��� ������ ���ǰ�ü�� ����
// PARM :	1 . lpfnBaseNotifyProc - �⺻ �̺�Ʈ (����, ����, ����) �� �뺸���� �Լ� ������
//			2 . lpfnRecvNotifyProc - ���� �̺�Ʈ�� �뺸���� �Լ� ������
//			3 . lpBaseNotifyProcParam - �⺻ �̺�Ʈ ����� �Ķ����
//			4 . lpRecvNotifyProcParam - ���� �̺�Ʈ ����� �Ķ����
//			5 . iRecvAppBufferSize - ������ ���Ź��� ũ�� (����)
//			6 . iSendAppBufferSize - ������ �۽Ź��� ũ�� (����)
//			7 . iRecvSckBufferSize - ������ ���Ź��� ũ�� (����)
//			8 . iSendSckBufferSize - ������ �۽Ź��� ũ�� (����)
// RETV : ���� ��ü ������
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
		// ����
		SaveErrorMessage(0, _T("!lpfnBaseNotifyProc || !lpfnRecvNotifyProc - CEventSelectTcpClient::GetSession()"));
		return NULL;
	}

	if (iRecvAppBufferSize <= 0 || iSendAppBufferSize <= 0) {
		// ����
		SaveErrorMessage(0, _T("iRecvAppBufferSize <= 0 || iSendAppBufferSize <= 0 - CEventSelectTcpClient::GetSession()"));
		return NULL;
	}

	// ��� ��ü ����ȭ
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	TSESSION* lpEventSelectTcpClientSession = m_SessionMemoryPool.GetMemory();
	if (!lpEventSelectTcpClientSession) {
		// ����
		SaveErrorMessage(0, _T("!lpEventSelectTcpClientSession - CEventSelectTcpClient::GetSession()"));
		BASE_RETURN(NULL);
	}
	CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession2 = static_cast<CEventSelectTcpClientSession<TSESSION>*>(lpEventSelectTcpClientSession);

	// ���� ����ڿ��� ���޵��� ���� �����̹Ƿ� ���� ����ȭ �ʿ����

	lpEventSelectTcpClientSession2->SetIsActivated(TRUE);
	lpEventSelectTcpClientSession2->Reset();
	lpEventSelectTcpClientSession2->SetEventSelectTcpClientPtr(this);
	lpEventSelectTcpClientSession2->SetBaseNotifyProc(lpfnBaseNotifyProc, lpBaseNotifyProcParam);
	lpEventSelectTcpClientSession2->SetRecvNotifyProc(lpfnRecvNotifyProc, lpRecvNotifyProcParam);
	lpEventSelectTcpClientSession2->SetSckBufferSize(iRecvSckBufferSize, iSendSckBufferSize);

	if (lpEventSelectTcpClientSession2->CreateBuffer(iRecvAppBufferSize, iSendAppBufferSize) == FALSE) {
		// ����
		SaveErrorMessage(0, _T("lpEventSelectTcpClientSession2->CreateBuffer() == FALSE - CEventSelectTcpClient::GetSession()"));
		BASE_RETURN(NULL);
	}

	return lpEventSelectTcpClientSession;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::ReleaseSession
//*---------------------------------------------------------------
// DESC : ����� ���� ���ǰ�ü�� ��ȯ
// PARM :	1 . lpEventSelectTcpClientSession - ���� ��ü ������
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
	
	// ��� ��ü ����ȭ
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	// ���� ��ȣ �� �� �Լ��� ȣ���ϴ� �ʿ��� �̷���� (�ʿ��� ��쿡��) - ��κ� TSESSION::Disconnect() ���Ŀ� ��ȯ�ϹǷ� ū �ʿ�� ������

	if (lpEventSelectTcpClientSession2->GetIsActivated() == FALSE) {
		// ����
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
// DESC : ���� �� �����̺�Ʈ�� ���� �� �ֵ��� ���ǰ�ü�� ������ ��ü�� ��� (�ڵ����� �����)
// PARM :	1 . lpEventSelectTcpClientSession - ���� ��ü ������
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::RegisterSession(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{
	// ���� ��ȣ �� �� �Լ��� ȣ���ϴ� �ʿ��� �̷����

	if (!lpEventSelectTcpClientSession) {
		SaveErrorMessage(0, _T("!lpEventSelectTcpClientSession - CEventSelectTcpClient::RegisterSession()"));
		return FALSE;
	}

	if (lpEventSelectTcpClientSession->GetEventHandle() == NULL) {
		SaveErrorMessage(0, _T("lpEventSelectTcpClientSession->GetEventHandle() == NULL - CEventSelectTcpClient::RegisterSession()"));
		return FALSE;
	}

	// ��� ��ü ����ȭ
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	// ������ �ڵ�� �����͸� ������ ���Ϳ� ����Ѵ�.
	std::vector<CEventSelectTcpClientSession<TSESSION>*>::iterator it = std::find(m_vtEventSelectTcpClientSession.begin(), m_vtEventSelectTcpClientSession.end(), lpEventSelectTcpClientSession);
	if (it == m_vtEventSelectTcpClientSession.end()) {
		m_vtEventSelectTcpClientSession.push_back(lpEventSelectTcpClientSession);
	}
	else {
		// ���� - �ڵ��� ������ �� ����. Ȥ�ö� �̷� ��찡 �߻��ϴ��� üũ�� ���ƾ� �Ѵ�.
		SaveErrorMessage(0, _T("it != m_vtEventSelectTcpClientSession.end() - CEventSelectTcpClient::RegisterSession()"));
	}

	// �۾� �����忡 �ڵ��� �߰��Ǿ��ٴ� �̺�Ʈ�� ������.
	if (m_hEventAddHandle) {
		::SetEvent(m_hEventAddHandle);
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::UnregisterSession
//*---------------------------------------------------------------
// DESC : ���� �� �����̺�Ʈ�� ���� �ʵ��� ���ǰ�ü�� ������ ��ü���� ������� (�ڵ����� �����)
// PARM :	1 . lpEventSelectTcpClientSession - ���� ��ü ������
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::UnregisterSession(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{
	// ���� ��ȣ �� �� �Լ��� ȣ���ϴ� �ʿ��� �̷����

	if (!lpEventSelectTcpClientSession) {
		SaveErrorMessage(0, _T("!lpEventSelectTcpClientSession - CEventSelectTcpClient::UnregisterSession()"));
		return FALSE;
	}

	if (lpEventSelectTcpClientSession->GetEventHandle() == NULL) {
		SaveErrorMessage(0, _T("lpEventSelectTcpClientSession->GetEventHandle() == NULL - CEventSelectTcpClient::UnregisterSession()"));
		return FALSE;
	}

	// ��� ��ü ����ȭ
	CLockAuto AutoLock(&m_crEventSelectTcpClient);

	// ���Ϳ��� ���ǿ� ����� �̺�Ʈ �ڵ��� ã�Ƽ� �����Ѵ�.
	std::vector<CEventSelectTcpClientSession<TSESSION>*>::iterator it = std::find(m_vtEventSelectTcpClientSession.begin(), m_vtEventSelectTcpClientSession.end(), lpEventSelectTcpClientSession);
	if (it != m_vtEventSelectTcpClientSession.end()) {
		m_vtEventSelectTcpClientSession.erase(it);
	}
	else {
		// ���� - �ڵ��� �����ؾ� �Ѵ�. Ȥ�ö� �̷� ��찡 �߻��ϴ��� üũ�� ���ƾ� �Ѵ�.
		SaveErrorMessage(0, _T("it == m_vtEventSelectTcpClientSession.end() - CEventSelectTcpClient::UnregisterSession()"));
	}

	// �۾� �����忡 �ڵ��� ���ŵǾ��ٴ� �̺�Ʈ�� ������.
	if (m_hEventAddHandle) {
		::SetEvent(m_hEventAddHandle);
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::IoEventRecv
//*---------------------------------------------------------------
// DESC : Ư�� �������� �뺸�� ���� �̺�Ʈ�� ó��
// PARM :	1 . lpEventSelectTcpClientSession - ���� ��ü ������
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::IoEventRecv(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{
	// RECV ���ۿ� �����͸� �޾Ƽ� ����ڿ��� �뺸�Ѵ�.

	// ���� ����ȭ �� RECV �̺�Ʈ�� �߻��ϴ� �����尡 �ϳ��̹Ƿ� ���� ��ȣ�� ���� WSARecv() �κи� ����ȭ
//	CLockAuto AutoLock(lpEventSelectTcpClientSession->GetCriticalSection());

	if (lpEventSelectTcpClientSession->RecvData() == FALSE) {
		// ���� - SendLeftData() ���ο��� ����ó�� �Ͽ���
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::IoEventSend
//*---------------------------------------------------------------
// DESC : Ư�� �������� �뺸�� �۽����� �̺�Ʈ�� ó��
// PARM :	1 . lpEventSelectTcpClientSession - ���� ��ü ������
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClient<TSESSION>::IoEventSend(CEventSelectTcpClientSession<TSESSION>* lpEventSelectTcpClientSession)
{

	// ���� ������ ������ ���� ���� �Ͱ� ������ ������.

	// ���� ����ȭ
	CLockAuto AutoLock(lpEventSelectTcpClientSession->GetLock());

	if (lpEventSelectTcpClientSession->SendLeftData() == SOCKET_ERROR) {
		// ���� - SendLeftData() ���ο��� ����ó�� �Ͽ���
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClient::SetEventArray
//*---------------------------------------------------------------
// DESC : �̺�Ʈ �迭�� ����
// PARM :	1 . lphEventArray - �̺�Ʈ �ڵ�
//			2 . lpEventSelectTcpClientSessionArray - �̺�Ʈ �迭 ������
//			3 . iEventCount - ��ϵ� �̺�Ʈ ����
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClient<TSESSION>::SetEventArray(LPHANDLE lphEventArray, CEventSelectTcpClientSession<TSESSION>** lpEventSelectTcpClientSessionArray, INT& iEventCount)
{
	// ��� ��ü ����ȭ
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
		// ����ϰ� �߻�
//		BASE_TRACE(_T("m_vtEventSelectTcpClientSession.empty() - CEventSelectTcpClient::SetEventArray()"));
		return;
	}

	iEventCount += (INT)m_vtEventSelectTcpClientSession.size();
	if (iEventCount >= WSA_MAXIMUM_WAIT_EVENTS) {
		// �̷� ��찡 �߻��ϴ��� üũ�� ���ƾ� �Ѵ�.
		iEventCount = WSA_MAXIMUM_WAIT_EVENTS;
		BASE_TRACE(_T("iEventCount >= WSA_MAXIMUM_WAIT_EVENTS - CEventSelectTcpClient::SetEventArray()"));
	}

	for (INT iIndex = DF_ETCL_BASE_EVENT_COUNT; iIndex < iEventCount ; ++iIndex) {
		lphEventArray[iIndex] = m_vtEventSelectTcpClientSession[iIndex-DF_ETCL_BASE_EVENT_COUNT]->GetEventHandle();
		lpEventSelectTcpClientSessionArray[iIndex] = m_vtEventSelectTcpClientSession[iIndex-DF_ETCL_BASE_EVENT_COUNT];
		if (lphEventArray[iIndex] == NULL) {
			// ���� - �̷� ��찡 ������� Ȯ���� ���ƾ� �Ѵ�.
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
// DESC : Ŭ���� ���ο� �����޽����� ����, ����ڿ��� �˷���
// PARM :	1 . iErrorCode - �ý��� �����ڵ�
//			2 . lpszErrorMessage - �߰����� �����޽���
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
// DESC : �̺�Ʈ ó���� ������ ���� �۾� ������ ���ν���
// PARM :	1 . lpParam - ������ ��ü�� ������
// RETV : ������ �����ڵ� (0:OK)
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

	// �̺�Ʈ�迭�� �����Ѵ�.
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
			case ERROR_INVALID_HANDLE :	// �߸��� �ڵ��� �����־��ٸ� ���⼭ �ڵ����̺��� �ٽ� �����Ѵ�.
				lpEventSelectTcpClient->SetEventArray(hEventArray, lpEventSelectTcpClientSessionArray, iEventCount);
				continue;
			default :
				continue;
			}
		}

		iEventIndex = iRetVal - WSA_WAIT_EVENT_0;
		if (iEventIndex >= WSA_MAXIMUM_WAIT_EVENTS) {
			// ����
			continue;
		}

		switch(iEventIndex) {
		case DF_ETCL_EVENT_END_THREAD:	// �۾������� ����
			bEndThread = TRUE;
			continue;
		case DF_ETCL_EVENT_ADD_HANDLE:	// �̺�Ʈ�ڵ� �迭 �߰�/���� �� ���� ����
			lpEventSelectTcpClient->SetEventArray(hEventArray, lpEventSelectTcpClientSessionArray, iEventCount);
			continue;
		}

		// �Ϲ����� ���� �̺�Ʈ ��
		if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetSocket() == INVALID_SOCKET) {
			continue;
		}
		iErrNo = ::WSAEnumNetworkEvents(
			lpEventSelectTcpClientSessionArray[iEventIndex]->GetSocket(),
			hEventArray[iEventIndex],
			&stWsaNetworkEvents
			);
		if (iErrNo == SOCKET_ERROR) {
			// ����
			lpEventSelectTcpClient->SaveErrorMessage(::WSAGetLastError(), _T("WSAEnumNetworkEvents() - CEventSelectTcpClient::MainWorkerThreadProc()"));
			continue;
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_CONNECT) {
			if (stWsaNetworkEvents.iErrorCode[FD_CONNECT_BIT] != 0) {
				// ����
				if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
						EV_NBN_ERROR, 
						lpEventSelectTcpClientSessionArray[iEventIndex], 
						(LPVOID)lpEventSelectTcpClient->m_ErrorFormat.Write(stWsaNetworkEvents.iErrorCode[FD_CONNECT_BIT], _T("stWsaNetworkEvents.iErrorCode[FD_CONNECT_BIT] != 0 - CEventSelectTcpClient::MainWorkerThreadProc()")), 
						lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
						);
				}
				// ������ �����Ű�� ������
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}

			// FD_CONNECT �۾�

			// ���ο� �̺�Ʈ�� ����Ѵ�.
			iErrNo = ::WSAEventSelect(lpEventSelectTcpClientSessionArray[iEventIndex]->GetSocket(),
				hEventArray[iEventIndex],
				FD_READ | FD_WRITE | FD_CLOSE
				);
			if (iErrNo == SOCKET_ERROR) {
				lpEventSelectTcpClient->SaveErrorMessage(::WSAGetLastError(), _T("WSAEventSelect() - CEventSelectTcpClient::MainWorkerThreadProc()"));
				// ������ �����Ű�� ������
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}
			// ����ڿ��� ������ ����Ǿ��ٴ� ���� �˸���.
			if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
				lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(EV_NBN_SESSCNNT, lpEventSelectTcpClientSessionArray[iEventIndex], (LPVOID)NULL, lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam());
			}

			// �ܵ� �۾��̹Ƿ� �������� �Ѿ
			continue;
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_READ) {
			if (stWsaNetworkEvents.iErrorCode[FD_READ_BIT] != 0) {
				// ����
				if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
						EV_NBN_ERROR, 
						lpEventSelectTcpClientSessionArray[iEventIndex], 
						(LPVOID)lpEventSelectTcpClient->m_ErrorFormat.Write(stWsaNetworkEvents.iErrorCode[FD_READ_BIT], _T("stWsaNetworkEvents.iErrorCode[FD_READ_BIT] != 0 - CEventSelectTcpClient::MainWorkerThreadProc()")), 
						lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
						);
				}
				// ������ �����Ű�� ������
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}

			// FD_READ �۾�
			if (lpEventSelectTcpClient->IoEventRecv(lpEventSelectTcpClientSessionArray[iEventIndex]) == FALSE) {
				continue;
			}
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_WRITE) {
			if (stWsaNetworkEvents.iErrorCode[FD_WRITE_BIT] != 0) {
				// ����
				if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
						EV_NBN_ERROR, 
						lpEventSelectTcpClientSessionArray[iEventIndex], 
						(LPVOID)lpEventSelectTcpClient->m_ErrorFormat.Write(stWsaNetworkEvents.iErrorCode[FD_WRITE_BIT], _T("stWsaNetworkEvents.iErrorCode[FD_WRITE_BIT] != 0 - CEventSelectTcpClient::MainWorkerThreadProc()")), 
						lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
						);
				}
				// ������ �����Ű�� ������
				lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
				continue;
			}

			// FD_WRITE �۾�
			if (lpEventSelectTcpClient->IoEventSend(lpEventSelectTcpClientSessionArray[iEventIndex]) == FALSE) {
				continue;
			}
		}

		if (stWsaNetworkEvents.lNetworkEvents & FD_CLOSE) {
			if (stWsaNetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
				// ����
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
			// ����ڿ��� ������ ����Ǿ��ٴ� ���� �˸���.
			if (lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()) {
				lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProc()(
					EV_NBN_SESSDCNT, 
					lpEventSelectTcpClientSessionArray[iEventIndex], 
					(LPVOID)NULL, 
					lpEventSelectTcpClientSessionArray[iEventIndex]->GetBaseNotifyProcParam()
					);
			}
			// ������ �����Ű�� ������
			lpEventSelectTcpClientSessionArray[iEventIndex]->Disconnect();
		}
	}

	return 0;
}

