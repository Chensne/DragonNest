

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::CEventSelectTcpClientSession
//*---------------------------------------------------------------
// DESC : ������
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
CEventSelectTcpClientSession<TSESSION>::CEventSelectTcpClientSession()
{
	Initialize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::~CEventSelectTcpClientSession
//*---------------------------------------------------------------
// DESC : �Ҹ���
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
CEventSelectTcpClientSession<TSESSION>::~CEventSelectTcpClientSession()
{
	Finalize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::Initialize
//*---------------------------------------------------------------
// DESC : ��ü ���� �ڿ����� �ʱ�ȭ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::Initialize()
{
	if (NOERROR != TP_SUPER::Open()) {
		BASE_RETURN(FALSE);
	}
	m_sckThis = INVALID_SOCKET;
	m_lpfnBaseNotifyProc = NULL;
	m_lpfnRecvNotifyProc = NULL;
	m_lpBaseNotifyProcParam = NULL;
	m_lpRecvNotifyProcParam = NULL;
	m_hEvent = WSA_INVALID_EVENT;
//	m_lpParam = NULL;
	m_lpEventSelectTcpClient = NULL;
	m_bIsActivated = FALSE;
	Reset();
// 	if (NOERROR != m_Lock.Open()) {
// 		BASE_RETURN(FALSE);
// 	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::Finalize
//*---------------------------------------------------------------
// DESC : ��ü ���� �ڿ����� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::Finalize()
{
	m_RecvIoBuffer.Finalize();
	m_SendIoBuffer.Finalize();
//	m_Lock.Close();
	TP_SUPER::Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::CreateBuffer
//*---------------------------------------------------------------
// DESC : ������ ��/���ſ� ���۸� ����
// PARM :	1 . iRecvBufferSize - ������ ���ſ� ������ ũ��
//			2 . iSendBufferSize - ������ �۽ſ� ������ ũ��
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::CreateBuffer(INT iRecvBufferSize, INT iSendBufferSize)
{
	if (iRecvBufferSize <= 0 || iSendBufferSize <= 0) {
		return FALSE;
	}

	if (m_RecvIoBuffer.GetBufferSize() > 0) {
		m_RecvIoBuffer.Finalize();
	}

	if (m_SendIoBuffer.GetBufferSize() > 0) {
		m_SendIoBuffer.Finalize();
	}

	if (m_RecvIoBuffer.Initialize(iRecvBufferSize) == FALSE) {
		BASE_RETURN(FALSE);
	}
	
	if (m_SendIoBuffer.Initialize(iSendBufferSize) == FALSE) {
		BASE_RETURN(FALSE);
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::DeleteBuffer
//*---------------------------------------------------------------
// DESC : ������ ���ſ� �����ڿ��� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::DeleteBuffer()
{
	m_RecvIoBuffer.Finalize();
	m_SendIoBuffer.Finalize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::Reset
//*---------------------------------------------------------------
// DESC : ��ü���� �ڿ� �ʱ�ȭ
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::Reset()
{
//	m_lpParam = NULL;
	::memset(&m_stRemoteSockAddrIn, 0, sizeof(m_stRemoteSockAddrIn));

	TP_SUPER::Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::CreateSocket
//*---------------------------------------------------------------
// DESC : ���� ����ڸ� ����
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::CreateSocket()
{
	if (m_sckThis != INVALID_SOCKET) {
		// miss
		BASE_RETURN(FALSE);
	}
	m_sckThis = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_sckThis == INVALID_SOCKET) {
		// ����
		BASE_RETURN(FALSE);
	}
	return TRUE;	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::CloseSocket
//*---------------------------------------------------------------
// DESC : ���� ����ڸ� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::CloseSocket()
{
	SAFE_CLOSESOCKET(m_sckThis);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::Connect
//*---------------------------------------------------------------
// DESC : Ư�� TCP ������ TCP ����
// PARM :	1 . lpszIpAddress - Ư�� TCP ������ IP�ּ� (IP�ּ� Ȥ�� �������̸�)
//			2 . wPortNumber - Ư�� TCP ������ ��Ʈ��ȣ
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::Connect(LPCSTR lpszIpAddress, WORD wPortNumber)
{
	ULONG uIpAddress = ::inet_addr(lpszIpAddress);
	if (INADDR_NONE == uIpAddress) {
		if (FALSE == CWinsockBase::GetHostByName(lpszIpAddress, uIpAddress)) {
			BASE_RETURN(FALSE);
		}
	}

	return Connect(uIpAddress, wPortNumber);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::Connect
//*---------------------------------------------------------------
// DESC : Ư�� TCP ������ TCP ����
// PARM :	1 . uIpAddress - Ư�� TCP ������ IP�ּ� (��Ʈ��ũ ����Ʈ ����)
//			2 . wPortNumber - Ư�� TCP ������ ��Ʈ��ȣ
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::Connect(ULONG uIpAddress, WORD wPortNumber)
{
	// ���� ����ȭ
	CLockAuto AutoLock(&m_Lock);

	if (!m_bIsActivated) {
		return FALSE;
	}

	if (!CreateSocket()) {
		// ����
		SaveErrorMessage(0, _T("!CreateSocket() - CEventSelectTcpClientSession::Connect()"));
		// ���� ����
//		DisconnectNoLock();
		return FALSE;
	}
	if (0 <= GetRecvSckBufferSize()) {
		INT iRecvSckBufferSize = GetRecvSckBufferSize();
		INT iRetVal = SetSockOpt(SOL_SOCKET, SO_RCVBUF, ((PCHAR)&iRecvSckBufferSize), sizeof(iRecvSckBufferSize));
		if (NOERROR != iRetVal) {
			// ����
			SaveErrorMessage(iRetVal, _T("!SetSockOpt() - CEventSelectTcpClientSession::Connect() : SO_RCVBUF"));
			// ���� ����
//			DisconnectNoLock();
			BASE_RETURN(FALSE);
		}
	}
	if (0 <= GetSendSckBufferSize()) {
		INT iSendSckBufferSize = GetSendSckBufferSize();
		INT iRetVal = SetSockOpt(SOL_SOCKET, SO_SNDBUF, ((PCHAR)&iSendSckBufferSize), sizeof(iSendSckBufferSize));
		if (NOERROR != iRetVal) {
			// ����
			SaveErrorMessage(iRetVal, _T("!SetSockOpt() - CEventSelectTcpClientSession::Connect() : SO_SNDBUF"));
			// ���� ����
//			DisconnectNoLock();
			BASE_RETURN(FALSE);
		}
	}

	if (m_RecvIoBuffer.GetBufferSize() <= 0 || m_SendIoBuffer.GetBufferSize() <= 0) {
		// ����
		SaveErrorMessage(0, _T("m_RecvIoBuffer.GetBufferSize() <= 0 (%d) || m_SendIoBuffer.GetBufferSize() <= 0 - CEventSelectTcpClientSession::Connect()"));
		// ���� ����
		DisconnectNoLock();
		return FALSE;
	}
	m_RecvIoBuffer.Reset();
	m_SendIoBuffer.Reset();

	LINGER stLinger;
	stLinger.l_onoff = 1;
	stLinger.l_linger = 0;
	if (SOCKET_ERROR == ::setsockopt(m_sckThis, SOL_SOCKET, SO_LINGER, (CHAR*)&stLinger, sizeof(stLinger))) {
		// ����
		SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::Connect()"));
		// ���� ����
		DisconnectNoLock();
		return FALSE;
	}

	SOCKADDR_IN	stSockAddrIn;
	stSockAddrIn.sin_family = AF_INET;
	stSockAddrIn.sin_port = ::htons(wPortNumber);
	stSockAddrIn.sin_addr.s_addr = uIpAddress;

	HANDLE hEvent = ::WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT) {
		BASE_TRACE(_T("hEvent == WSA_INVALID_EVENT (%d) - CEventSelectTcpClientSession::Connect()"), ::WSAGetLastError());
		return FALSE;
	}

	INT iErrNo = ::WSAEventSelect(m_sckThis, hEvent, FD_CONNECT | FD_CLOSE);
	if (iErrNo == SOCKET_ERROR) {
		// ����
		SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::Connect()"));
		// ���� ����
		DisconnectNoLock();
		return FALSE;
	}

	iErrNo = ::WSAConnect(m_sckThis, (LPSOCKADDR)&stSockAddrIn, sizeof(stSockAddrIn), NULL, NULL, NULL, NULL);
	if (iErrNo == SOCKET_ERROR) {
		iErrNo = ::WSAGetLastError();
		if (iErrNo != WSA_IO_PENDING && iErrNo != WSAEWOULDBLOCK) {
			// ����
			SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::Connect()"));
			// ���� ����
			DisconnectNoLock();
			return FALSE;
		}
	}

	m_stRemoteSockAddrIn = stSockAddrIn;

	SetEventHandle(hEvent);

	// �� ������ �۾��������� �ڵ�迭�� ����Ѵ�.
	m_lpEventSelectTcpClient->RegisterSession(this);

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::Disconnect
//*---------------------------------------------------------------
// DESC : ������ ������ ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::Disconnect()
{
	// ���� ����ȭ
	CLockAuto AutoLock(&m_Lock);

	DisconnectNoLock();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::DisconnectNoLock
//*---------------------------------------------------------------
// DESC : Ư�� TCP �������� TCP ���� ���� (����ȭ ����)
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::DisconnectNoLock()
{

// 	if (!m_bIsActivated) {
// 		return;
// 	}

	if (m_sckThis == INVALID_SOCKET) {
		return;
	}

	if (WSA_INVALID_EVENT != m_hEvent) {
		// �� ������ �۾��������� �ڵ�迭���� �����Ѵ�.
		m_lpEventSelectTcpClient->UnregisterSession(this);
		::WSACloseEvent(m_hEvent);
		m_hEvent = WSA_INVALID_EVENT;
	}

	// �ڵ��� ���ŵ� �Ŀ� ������ ������ �Ѵ�.
	CloseSocket();
	Reset();

	m_RecvIoBuffer.Reset();
	m_SendIoBuffer.Reset();

}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::RecvData
//*---------------------------------------------------------------
// DESC : Ư�� TCP ������ ���� ������ ����
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::RecvData()
{
	// ���� ��ȣ �� �� �Լ��� ȣ���ϴ� �ʿ��� �̷����

	if (m_sckThis == INVALID_SOCKET) {
		return FALSE;
	}

	if (!m_bIsActivated) {
		return FALSE;
	}

	WSABUF stWsaBuf;
	stWsaBuf.buf = (CHAR*)m_RecvIoBuffer.GetBuffer() + m_RecvIoBuffer.GetBufferTailPtr();
	stWsaBuf.len = m_RecvIoBuffer.GetBufferSize() - m_RecvIoBuffer.GetBufferTailPtr();

	ULONG uIoBytes;
	ULONG uFlag = 0;	// important to set 0 !!!
	{
		CLockAuto AutoLock(&m_Lock);

		INT iErrNo = ::WSARecv(m_sckThis, &stWsaBuf, 1, &uIoBytes, &uFlag, NULL, NULL);
		if (iErrNo == SOCKET_ERROR) {
			iErrNo = ::WSAGetLastError();
			if (iErrNo != WSA_IO_PENDING && iErrNo != WSAEWOULDBLOCK) {
				// ����
				SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::RecvData()"));
				// ���� ����
				DisconnectNoLock();
				return FALSE;
			}
			else {
				return TRUE;
			}
		}
	}

	if (!uIoBytes) {
		// !!1 . gracefully closed - FD_CLOSE �� �� �����Ƿ� ���⿡ �� ���ɼ��� ���� ���� ??
		// ���� ����
		DisconnectNoLock();
		return TRUE;
	}

	m_RecvIoBuffer.AddBufferTailPtr(uIoBytes);
	if (m_RecvIoBuffer.GetBufferTailPtr() > m_RecvIoBuffer.GetBufferSize()) {
		// �̸� ���� ũ�⸦ ����ϰ� WSARecv() �� ȣ���ϹǷ� �̷� ��찡 �߻��Ҹ��� �������� Ȥ�� �߻��ϴ��� üũ�� ���ƾ� �Ѵ�.
		m_RecvIoBuffer.SetBufferTailPtr(m_RecvIoBuffer.GetBufferSize());
		BASE_TRACE(_T("m_RecvIoBuffer.GetBufferTailPtr() > m_RecvIoBuffer.GetBufferSize() - CEventSelectTcpClientSession::RecvData()"));
	}

	// �����͸� �޾���
	if (m_lpfnRecvNotifyProc) {
		BOOL bRetVal = m_lpfnRecvNotifyProc(
			this,
			(CHAR*)m_RecvIoBuffer.GetBuffer() + m_RecvIoBuffer.GetBufferHeadPtr(),
			m_RecvIoBuffer.RefBufferHeadPtr(),
			m_RecvIoBuffer.GetBufferTailPtr() - m_RecvIoBuffer.GetBufferHeadPtr(),
			m_RecvIoBuffer.GetBufferSize(),
			m_lpRecvNotifyProcParam,
			GetRemoteSockAddrIn().sin_addr,
			GetRemotePortNumberN()
			);
		if (!bRetVal) {
			// ����
			SaveErrorMessage(0, _T("FALSE == m_lpfnRecvNotifyProc() - CEventSelectTcpClientSession::RecvData()"));
			// ���� ����
			DisconnectNoLock();
			return FALSE;
		}
	}
	MoveLeftRecvBufferToHead();
	
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::SendData
//*---------------------------------------------------------------
// DESC : ����� �ش� �������� Ư�� ������ �۽�
// PARM :	1 . lpbtBuffer - ������ ������ ���� ������
//			2 . iBufferSize - ������ �������� ũ��
//			3 . bDoLock - ��� ��뿩��
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::SendData(LPBYTE lpbtBuffer, INT iBufferSize, BOOL bDoLock)
{
	if (!lpbtBuffer || iBufferSize <= 0) {
		return FALSE;
	}

	// ���� ����ȭ
	CCriticalSection* aLock = (bDoLock)?(GetLock()):(NULL);
	CLockAuto AutoLock(aLock);
	

	INT iLeftDataLen = SendLeftData();
	if (iLeftDataLen == SOCKET_ERROR) {
		// ���� - SendLeftData() ���ο��� ����ó�� �Ͽ���
		return FALSE;
	}

	if (!m_bIsActivated) {
		return FALSE;
	}

	if (!iLeftDataLen) {
		// ���� ���� �����Ͱ� ����. �׳� ������.
		WSABUF stWsaBuf;
		ULONG uIoBytes;
		INT iErrNo;
		INT iBufferHeadPtr = 0, iBufferTailPtr= iBufferSize;

		BOOL bInfinite = TRUE;
		while(bInfinite) {
			stWsaBuf.buf = (CHAR*)lpbtBuffer + iBufferHeadPtr;
			stWsaBuf.len = iBufferTailPtr - iBufferHeadPtr;
			
			iErrNo = ::WSASend(m_sckThis, &stWsaBuf, 1, &uIoBytes, 0, NULL, NULL);
			if (iErrNo == SOCKET_ERROR) {
				iErrNo = ::WSAGetLastError();
				if (iErrNo != WSA_IO_PENDING && iErrNo != WSAEWOULDBLOCK) {	
					// ����
					SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::SendData()"));
					// ���� ����
					DisconnectNoLock();
					return FALSE;
				}
				else {
					// ó���� ������ ��� �����Ͱ� �����ִٸ� ������ ������ �Ѵ�.
					iBufferHeadPtr += uIoBytes;
					if (iBufferHeadPtr >= iBufferTailPtr) {
						// ��� ���´�.
						return TRUE;
					}
					else {
						// ���� �����Ͱ� ���� �����ִ�. ���Ŀ� ������ ���� �۽Ź��ۿ� �ִ´�.
						MoveLeftSendBufferToHead();

						if (m_SendIoBuffer.GetFreeBufferSize() < iBufferTailPtr - iBufferHeadPtr) {
							// ����
							SaveErrorMessage(0, _T("m_SendIoBuffer.GetFreeBufferSize() < iBufferTailPtr - iBufferHeadPtr - CEventSelectTcpClientSession::SendData()"));
							// ���� ����
							DisconnectNoLock();
							return FALSE;
						}

						::memcpy(m_SendIoBuffer.GetBuffer() + m_SendIoBuffer.GetBufferHeadPtr(), lpbtBuffer + iBufferHeadPtr, iBufferTailPtr - iBufferHeadPtr);
						m_SendIoBuffer.AddBufferTailPtr(iBufferTailPtr - iBufferHeadPtr);
						return TRUE;
					}
				}
			}

			iBufferHeadPtr += uIoBytes;
			if (iBufferHeadPtr >= iBufferTailPtr) {
				// ��� ���´�.
				return TRUE;
			}
		}
	}
	else {
		// ���� ���� �����Ͱ� ����. ���ۿ� �����͸� �����ϰ� ������.
		if (m_SendIoBuffer.GetFreeBufferSize() < iBufferSize) {
			// ����
			SaveErrorMessage(0, _T("m_SendIoBuffer.GetFreeBufferSize() < iBufferSize - CEventSelectTcpClientSession::SendData()"));
			// ���� ����
			DisconnectNoLock();
			return FALSE;
		}

		::memcpy(m_SendIoBuffer.GetBuffer() + m_SendIoBuffer.GetBufferHeadPtr(), lpbtBuffer, iBufferSize);
		m_SendIoBuffer.AddBufferTailPtr(iBufferSize);

		if (SendLeftData() == SOCKET_ERROR) {
			// ���� - SendLeftData() ���ο��� ����ó�� �Ͽ���
			return FALSE;
		}
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::SendLeftData
//*---------------------------------------------------------------
// DESC : ���� �۽� �� ���� ���ۿ� ���� �����Ͱ� �ִ��� üũ�Ͽ� �������� �۽�
// PARM : N/A
// RETV : 0 - ���� / 0 �̻� - ���� ������ ũ�� / SOCKET_ERROR - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
INT CEventSelectTcpClientSession<TSESSION>::SendLeftData()
{
	// ���� ��ȣ �� �� �Լ��� ȣ���ϴ� �ʿ��� �̷����

	if (!m_SendIoBuffer.GetBufferTailPtr()) {
		// ���� ���� �߿��ϰ� üũ�� ��ƾ�̹Ƿ� ���� ���ʿ� ��ġ
		return 0;
	}

	if (m_sckThis == INVALID_SOCKET) {
		return SOCKET_ERROR;
	}

	if (m_SendIoBuffer.GetBufferTailPtr() < 0) {
		// �̷� ��찡 �߻��ϴ��� Ȯ��

		// ����
		SaveErrorMessage(0, _T("m_SendIoBuffer.GetBufferTailPtr() < 0 - CEventSelectTcpClientSession::SendLeftData()"));
		// ���� ����
		DisconnectNoLock();
		return SOCKET_ERROR;
	}

	if (m_SendIoBuffer.GetBufferTailPtr() > m_SendIoBuffer.GetBufferHeadPtr()) {
		// ���� �����Ͱ� �ִ�.
		WSABUF stWsaBuf;
		ULONG uIoBytes;
		INT iErrNo;

		BOOL bInfinite = TRUE;
		while(bInfinite) {
			stWsaBuf.buf = (CHAR*)m_SendIoBuffer.GetBuffer() + m_SendIoBuffer.GetBufferHeadPtr();
			stWsaBuf.len = m_SendIoBuffer.GetBufferTailPtr() - m_SendIoBuffer.GetBufferHeadPtr();

			iErrNo = ::WSASend(m_sckThis, &stWsaBuf, 1, &uIoBytes, 0, NULL, NULL);
			if (iErrNo == SOCKET_ERROR) {
				iErrNo = ::WSAGetLastError();
				if (iErrNo != WSA_IO_PENDING && iErrNo != WSAEWOULDBLOCK) {	
					// ����
					SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::SendLeftData()"));
					// ���� ����
					DisconnectNoLock();
					return SOCKET_ERROR;
				}
				else {
					// ó���� ������ ��� �����Ͱ� �����ִٸ� ������ ������ �Ѵ�.
					m_SendIoBuffer.AddBufferHeadPtr(uIoBytes);
					if (m_SendIoBuffer.GetBufferHeadPtr() >= m_SendIoBuffer.GetBufferTailPtr()) {
						// ��� ���´�.
						m_SendIoBuffer.Reset();
						return 0;
					}
					else {
						// ���� �����Ͱ� ���� �����ִ�.
						MoveLeftSendBufferToHead();
						return m_SendIoBuffer.GetBufferTailPtr();
					}
				}
			}

			m_SendIoBuffer.AddBufferHeadPtr(uIoBytes);
			if (m_SendIoBuffer.GetBufferHeadPtr() >= m_SendIoBuffer.GetBufferTailPtr()) {
				// ��� ���´�.
				m_SendIoBuffer.Reset();
				return 0;
			}
		}
	}
	else {
		// �̷� ���°� �߻��� ���� ���� �������� üũ�� ���ƾ� �Ѵ�.
		m_SendIoBuffer.Reset();
	}

	return 0;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::MoveLeftRecvBufferToHead
//*---------------------------------------------------------------
// DESC : ���� �۾� �� ���� �����͸� ������ �� �������� �̵���Ŵ
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::MoveLeftRecvBufferToHead()
{
	// ���� ����ȭ�� ȣ���ϴ� �ʿ��� �̷�� ��

	if (m_RecvIoBuffer.GetBuffer() && m_RecvIoBuffer.GetBufferHeadPtr() > 0) {
		if (m_RecvIoBuffer.GetBufferTailPtr() > m_RecvIoBuffer.GetBufferHeadPtr()) {
			::memmove(m_RecvIoBuffer.GetBuffer(), m_RecvIoBuffer.GetBuffer() + m_RecvIoBuffer.GetBufferHeadPtr(), m_RecvIoBuffer.GetBufferTailPtr() - m_RecvIoBuffer.GetBufferHeadPtr());
			m_RecvIoBuffer.AddBufferTailPtr(-m_RecvIoBuffer.GetBufferHeadPtr());
			m_RecvIoBuffer.SetBufferHeadPtr(0);
		}
		else {
//			m_RecvIoBuffer.GetBufferTailPtr() < m_RecvIoBuffer.GetBufferHeadPtr() �� ��Ȳ�� �߻��Ѵٸ� ������
			m_RecvIoBuffer.Reset();
		}
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::MoveLeftSendBufferToHead
//*---------------------------------------------------------------
// DESC : �۽� �۾� �� ���� �����͸� ������ �� �������� �̵���Ŵ
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::MoveLeftSendBufferToHead()
{
	// ���� ����ȭ�� ȣ���ϴ� �ʿ��� �̷�� ��
	
	if (m_SendIoBuffer.GetBuffer() && m_SendIoBuffer.GetBufferHeadPtr() > 0) {
		if (m_SendIoBuffer.GetBufferTailPtr() > m_SendIoBuffer.GetBufferHeadPtr()) {
			::memmove(m_SendIoBuffer.GetBuffer(), m_SendIoBuffer.GetBuffer() + m_SendIoBuffer.GetBufferHeadPtr(), m_SendIoBuffer.GetBufferTailPtr() - m_SendIoBuffer.GetBufferHeadPtr());
			m_SendIoBuffer.AddBufferTailPtr(-m_SendIoBuffer.GetBufferHeadPtr());
			m_SendIoBuffer.SetBufferHeadPtr(0);
		}
		else {
//			m_SendIoBuffer.GetBufferTailPtr() < m_SendIoBuffer.GetBufferHeadPtr() �� ��Ȳ�� �߻��Ѵٸ� ������
			m_SendIoBuffer.Reset();
		}
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::SaveErrorMessage
//*---------------------------------------------------------------
// DESC : Ŭ���� ���ο� �����޽����� ����, ����ڿ��� �˷���
// PARM :	1 . iErrorCode - �ý��� �����ڵ�
//			2 . lpszErrorMessage - �߰����� �����޽���
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::SaveErrorMessage(INT iErrorCode, LPCTSTR lpszErrorMessage)
{
	if (m_lpfnBaseNotifyProc) {
		m_lpfnBaseNotifyProc(EV_NBN_ERROR, this, m_ErrorFormat.Write(iErrorCode, lpszErrorMessage), m_lpBaseNotifyProcParam);
	}
}

