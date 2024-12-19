

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
// DESC : 생성자
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
// DESC : 소멸자
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
// DESC : 객체 관련 자원들을 초기화
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
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
// DESC : 객체 관련 자원들을 해제
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
// DESC : 데이터 송/수신용 버퍼를 생성
// PARM :	1 . iRecvBufferSize - 데이터 수신용 버퍼의 크기
//			2 . iSendBufferSize - 데이터 송신용 버퍼의 크기
// RETV : TRUE - 성공 / FALSE - 실패
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
// DESC : 데이터 수신용 버퍼자원을 해제
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
// DESC : 객체관련 자원 초기화
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
// DESC : 소켓 기술자를 생성
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
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
		// 에러
		BASE_RETURN(FALSE);
	}
	return TRUE;	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::CloseSocket
//*---------------------------------------------------------------
// DESC : 소켓 기술자를 닫음
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
// DESC : 특정 TCP 서버로 TCP 연결
// PARM :	1 . lpszIpAddress - 특정 TCP 서버의 IP주소 (IP주소 혹은 도메인이름)
//			2 . wPortNumber - 특정 TCP 서버의 포트번호
// RETV : TRUE - 성공 / FALSE - 실패
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
// DESC : 특정 TCP 서버로 TCP 연결
// PARM :	1 . uIpAddress - 특정 TCP 서버의 IP주소 (네트워크 바이트 오더)
//			2 . wPortNumber - 특정 TCP 서버의 포트번호
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::Connect(ULONG uIpAddress, WORD wPortNumber)
{
	// 세션 동기화
	CLockAuto AutoLock(&m_Lock);

	if (!m_bIsActivated) {
		return FALSE;
	}

	if (!CreateSocket()) {
		// 에러
		SaveErrorMessage(0, _T("!CreateSocket() - CEventSelectTcpClientSession::Connect()"));
		// 세션 종료
//		DisconnectNoLock();
		return FALSE;
	}
	if (0 <= GetRecvSckBufferSize()) {
		INT iRecvSckBufferSize = GetRecvSckBufferSize();
		INT iRetVal = SetSockOpt(SOL_SOCKET, SO_RCVBUF, ((PCHAR)&iRecvSckBufferSize), sizeof(iRecvSckBufferSize));
		if (NOERROR != iRetVal) {
			// 에러
			SaveErrorMessage(iRetVal, _T("!SetSockOpt() - CEventSelectTcpClientSession::Connect() : SO_RCVBUF"));
			// 세션 종료
//			DisconnectNoLock();
			BASE_RETURN(FALSE);
		}
	}
	if (0 <= GetSendSckBufferSize()) {
		INT iSendSckBufferSize = GetSendSckBufferSize();
		INT iRetVal = SetSockOpt(SOL_SOCKET, SO_SNDBUF, ((PCHAR)&iSendSckBufferSize), sizeof(iSendSckBufferSize));
		if (NOERROR != iRetVal) {
			// 에러
			SaveErrorMessage(iRetVal, _T("!SetSockOpt() - CEventSelectTcpClientSession::Connect() : SO_SNDBUF"));
			// 세션 종료
//			DisconnectNoLock();
			BASE_RETURN(FALSE);
		}
	}

	if (m_RecvIoBuffer.GetBufferSize() <= 0 || m_SendIoBuffer.GetBufferSize() <= 0) {
		// 에러
		SaveErrorMessage(0, _T("m_RecvIoBuffer.GetBufferSize() <= 0 (%d) || m_SendIoBuffer.GetBufferSize() <= 0 - CEventSelectTcpClientSession::Connect()"));
		// 세션 종료
		DisconnectNoLock();
		return FALSE;
	}
	m_RecvIoBuffer.Reset();
	m_SendIoBuffer.Reset();

	LINGER stLinger;
	stLinger.l_onoff = 1;
	stLinger.l_linger = 0;
	if (SOCKET_ERROR == ::setsockopt(m_sckThis, SOL_SOCKET, SO_LINGER, (CHAR*)&stLinger, sizeof(stLinger))) {
		// 에러
		SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::Connect()"));
		// 세션 종료
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
		// 에러
		SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::Connect()"));
		// 세션 종료
		DisconnectNoLock();
		return FALSE;
	}

	iErrNo = ::WSAConnect(m_sckThis, (LPSOCKADDR)&stSockAddrIn, sizeof(stSockAddrIn), NULL, NULL, NULL, NULL);
	if (iErrNo == SOCKET_ERROR) {
		iErrNo = ::WSAGetLastError();
		if (iErrNo != WSA_IO_PENDING && iErrNo != WSAEWOULDBLOCK) {
			// 에러
			SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::Connect()"));
			// 세션 종료
			DisconnectNoLock();
			return FALSE;
		}
	}

	m_stRemoteSockAddrIn = stSockAddrIn;

	SetEventHandle(hEvent);

	// 이 세션을 작업스레드의 핸들배열에 등록한다.
	m_lpEventSelectTcpClient->RegisterSession(this);

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::Disconnect
//*---------------------------------------------------------------
// DESC : 세션의 연결을 해제
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::Disconnect()
{
	// 세션 동기화
	CLockAuto AutoLock(&m_Lock);

	DisconnectNoLock();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::DisconnectNoLock
//*---------------------------------------------------------------
// DESC : 특정 TCP 서버와의 TCP 연결 해제 (동기화 없음)
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
		// 이 세션을 작업스레드의 핸들배열에서 제거한다.
		m_lpEventSelectTcpClient->UnregisterSession(this);
		::WSACloseEvent(m_hEvent);
		m_hEvent = WSA_INVALID_EVENT;
	}

	// 핸들이 제거된 후에 소켓이 닫혀야 한다.
	CloseSocket();
	Reset();

	m_RecvIoBuffer.Reset();
	m_SendIoBuffer.Reset();

}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::RecvData
//*---------------------------------------------------------------
// DESC : 특정 TCP 서버로 부터 데이터 수신
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::RecvData()
{
	// 세션 보호 → 이 함수를 호출하는 쪽에서 이루어짐

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
				// 에러
				SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::RecvData()"));
				// 세션 종료
				DisconnectNoLock();
				return FALSE;
			}
			else {
				return TRUE;
			}
		}
	}

	if (!uIoBytes) {
		// !!1 . gracefully closed - FD_CLOSE 로 갈 것으므로 여기에 올 가능성은 거의 없음 ??
		// 세션 종료
		DisconnectNoLock();
		return TRUE;
	}

	m_RecvIoBuffer.AddBufferTailPtr(uIoBytes);
	if (m_RecvIoBuffer.GetBufferTailPtr() > m_RecvIoBuffer.GetBufferSize()) {
		// 미리 받을 크기를 계산하고 WSARecv() 를 호출하므로 이런 경우가 발생할리는 없겠지만 혹시 발생하는지 체크해 보아야 한다.
		m_RecvIoBuffer.SetBufferTailPtr(m_RecvIoBuffer.GetBufferSize());
		BASE_TRACE(_T("m_RecvIoBuffer.GetBufferTailPtr() > m_RecvIoBuffer.GetBufferSize() - CEventSelectTcpClientSession::RecvData()"));
	}

	// 데이터를 받았음
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
			// 에러
			SaveErrorMessage(0, _T("FALSE == m_lpfnRecvNotifyProc() - CEventSelectTcpClientSession::RecvData()"));
			// 세션 종료
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
// DESC : 연결된 해당 세션으로 특정 데이터 송신
// PARM :	1 . lpbtBuffer - 전송할 데이터 버퍼 포인터
//			2 . iBufferSize - 전송할 데이터의 크기
//			3 . bDoLock - 잠금 사용여부
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
BOOL CEventSelectTcpClientSession<TSESSION>::SendData(LPBYTE lpbtBuffer, INT iBufferSize, BOOL bDoLock)
{
	if (!lpbtBuffer || iBufferSize <= 0) {
		return FALSE;
	}

	// 세션 동기화
	CCriticalSection* aLock = (bDoLock)?(GetLock()):(NULL);
	CLockAuto AutoLock(aLock);
	

	INT iLeftDataLen = SendLeftData();
	if (iLeftDataLen == SOCKET_ERROR) {
		// 에러 - SendLeftData() 내부에서 종료처리 하였음
		return FALSE;
	}

	if (!m_bIsActivated) {
		return FALSE;
	}

	if (!iLeftDataLen) {
		// 보낼 남은 데이터가 없음. 그냥 보낸다.
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
					// 에러
					SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::SendData()"));
					// 세션 종료
					DisconnectNoLock();
					return FALSE;
				}
				else {
					// 처리가 지연될 경우 데이터가 남아있다면 다음에 보내야 한다.
					iBufferHeadPtr += uIoBytes;
					if (iBufferHeadPtr >= iBufferTailPtr) {
						// 모두 보냈다.
						return TRUE;
					}
					else {
						// 보낼 데이터가 이직 남아있다. 차후에 보내기 위해 송신버퍼에 넣는다.
						MoveLeftSendBufferToHead();

						if (m_SendIoBuffer.GetFreeBufferSize() < iBufferTailPtr - iBufferHeadPtr) {
							// 에러
							SaveErrorMessage(0, _T("m_SendIoBuffer.GetFreeBufferSize() < iBufferTailPtr - iBufferHeadPtr - CEventSelectTcpClientSession::SendData()"));
							// 세션 종료
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
				// 모두 보냈다.
				return TRUE;
			}
		}
	}
	else {
		// 보낼 남은 데이터가 있음. 버퍼에 데이터를 복사하고 보낸다.
		if (m_SendIoBuffer.GetFreeBufferSize() < iBufferSize) {
			// 에러
			SaveErrorMessage(0, _T("m_SendIoBuffer.GetFreeBufferSize() < iBufferSize - CEventSelectTcpClientSession::SendData()"));
			// 세션 종료
			DisconnectNoLock();
			return FALSE;
		}

		::memcpy(m_SendIoBuffer.GetBuffer() + m_SendIoBuffer.GetBufferHeadPtr(), lpbtBuffer, iBufferSize);
		m_SendIoBuffer.AddBufferTailPtr(iBufferSize);

		if (SendLeftData() == SOCKET_ERROR) {
			// 에러 - SendLeftData() 내부에서 종료처리 하였음
			return FALSE;
		}
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::SendLeftData
//*---------------------------------------------------------------
// DESC : 이전 송신 후 남은 버퍼에 남은 데이터가 있는지 체크하여 나머지도 송신
// PARM : N/A
// RETV : 0 - 성공 / 0 이상 - 남은 데이터 크기 / SOCKET_ERROR - 에러
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
INT CEventSelectTcpClientSession<TSESSION>::SendLeftData()
{
	// 세션 보호 → 이 함수를 호출하는 쪽에서 이루어짐

	if (!m_SendIoBuffer.GetBufferTailPtr()) {
		// 가장 많이 중요하게 체크될 루틴이므로 가장 앞쪽에 배치
		return 0;
	}

	if (m_sckThis == INVALID_SOCKET) {
		return SOCKET_ERROR;
	}

	if (m_SendIoBuffer.GetBufferTailPtr() < 0) {
		// 이런 경우가 발생하는지 확인

		// 에러
		SaveErrorMessage(0, _T("m_SendIoBuffer.GetBufferTailPtr() < 0 - CEventSelectTcpClientSession::SendLeftData()"));
		// 세션 종료
		DisconnectNoLock();
		return SOCKET_ERROR;
	}

	if (m_SendIoBuffer.GetBufferTailPtr() > m_SendIoBuffer.GetBufferHeadPtr()) {
		// 보낼 데이터가 있다.
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
					// 에러
					SaveErrorMessage(::GetLastError(), _T(" - CEventSelectTcpClientSession::SendLeftData()"));
					// 세션 종료
					DisconnectNoLock();
					return SOCKET_ERROR;
				}
				else {
					// 처리가 지연될 경우 데이터가 남아있다면 다음에 보내야 한다.
					m_SendIoBuffer.AddBufferHeadPtr(uIoBytes);
					if (m_SendIoBuffer.GetBufferHeadPtr() >= m_SendIoBuffer.GetBufferTailPtr()) {
						// 모두 보냈다.
						m_SendIoBuffer.Reset();
						return 0;
					}
					else {
						// 보낼 데이터가 이직 남아있다.
						MoveLeftSendBufferToHead();
						return m_SendIoBuffer.GetBufferTailPtr();
					}
				}
			}

			m_SendIoBuffer.AddBufferHeadPtr(uIoBytes);
			if (m_SendIoBuffer.GetBufferHeadPtr() >= m_SendIoBuffer.GetBufferTailPtr()) {
				// 모두 보냈다.
				m_SendIoBuffer.Reset();
				return 0;
			}
		}
	}
	else {
		// 이런 상태가 발생할 일은 거의 없겠지만 체크해 보아야 한다.
		m_SendIoBuffer.Reset();
	}

	return 0;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::MoveLeftRecvBufferToHead
//*---------------------------------------------------------------
// DESC : 수신 작업 후 남은 데이터를 버퍼의 맨 앞쪽으로 이동시킴
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::MoveLeftRecvBufferToHead()
{
	// 세션 동기화는 호출하는 쪽에서 이루어 짐

	if (m_RecvIoBuffer.GetBuffer() && m_RecvIoBuffer.GetBufferHeadPtr() > 0) {
		if (m_RecvIoBuffer.GetBufferTailPtr() > m_RecvIoBuffer.GetBufferHeadPtr()) {
			::memmove(m_RecvIoBuffer.GetBuffer(), m_RecvIoBuffer.GetBuffer() + m_RecvIoBuffer.GetBufferHeadPtr(), m_RecvIoBuffer.GetBufferTailPtr() - m_RecvIoBuffer.GetBufferHeadPtr());
			m_RecvIoBuffer.AddBufferTailPtr(-m_RecvIoBuffer.GetBufferHeadPtr());
			m_RecvIoBuffer.SetBufferHeadPtr(0);
		}
		else {
//			m_RecvIoBuffer.GetBufferTailPtr() < m_RecvIoBuffer.GetBufferHeadPtr() 의 상황이 발생한다면 에러임
			m_RecvIoBuffer.Reset();
		}
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::MoveLeftSendBufferToHead
//*---------------------------------------------------------------
// DESC : 송신 작업 후 남은 데이터를 버퍼의 맨 앞쪽으로 이동시킴
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TSESSION>
VOID CEventSelectTcpClientSession<TSESSION>::MoveLeftSendBufferToHead()
{
	// 세션 동기화는 호출하는 쪽에서 이루어 짐
	
	if (m_SendIoBuffer.GetBuffer() && m_SendIoBuffer.GetBufferHeadPtr() > 0) {
		if (m_SendIoBuffer.GetBufferTailPtr() > m_SendIoBuffer.GetBufferHeadPtr()) {
			::memmove(m_SendIoBuffer.GetBuffer(), m_SendIoBuffer.GetBuffer() + m_SendIoBuffer.GetBufferHeadPtr(), m_SendIoBuffer.GetBufferTailPtr() - m_SendIoBuffer.GetBufferHeadPtr());
			m_SendIoBuffer.AddBufferTailPtr(-m_SendIoBuffer.GetBufferHeadPtr());
			m_SendIoBuffer.SetBufferHeadPtr(0);
		}
		else {
//			m_SendIoBuffer.GetBufferTailPtr() < m_SendIoBuffer.GetBufferHeadPtr() 의 상황이 발생한다면 에러임
			m_SendIoBuffer.Reset();
		}
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CEventSelectTcpClientSession::SaveErrorMessage
//*---------------------------------------------------------------
// DESC : 클래스 내부에 에러메시지를 저장, 사용자에게 알려줌
// PARM :	1 . iErrorCode - 시스템 에러코드
//			2 . lpszErrorMessage - 추가적인 에러메시지
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

