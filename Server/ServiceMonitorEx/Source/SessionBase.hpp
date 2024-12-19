/*//===============================================================

	<< SESSION BASE >>

	PRGM : B4nFter

	FILE : SESSIONBASE.HPP
	DESC : 세션 클래스들의 기본 클래스 (추상 클래스로 기본 인터페이스만 제공)
	INIT BUILT DATE : 2007. 09. 09
	LAST BUILT DATE : 2007. 09. 12

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "ERRORFORMAT.H"
#include "CRITICALSECTION.HPP"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

class CSessionBase;							// 전방 참조선언

enum EF_NETWORK_BASE_NOFITY {				// 네트워크 이벤트 통보 타입
	EV_NBN_ERROR = 0,						// 모듈 에러 (TCP/UDP)
	EV_NBN_SESSCNNT,						// 세션 연결 (TCP)
	EV_NBN_SESSDCNT,						// 세션 종료 (TCP) / 유효한 세션이 아님 (UDP)
	EV_NBN_SESSERROR,						// 세션 에러 (TCP/UDP)
};

typedef VOID (*LPFN_BASE_NOTIFY_PROC) (		// 소켓 기본 이벤트 통보 프로시저
	EF_NETWORK_BASE_NOFITY eNotifyType,		// 네트워크 이벤트 통보 타입
	CSessionBase* lpSession,				// 세션 포인터
	LPVOID lpResultParam,					// 결과 인자 - 다양한 용도로 사용됨
	LPVOID lpBaseNotifyProcParam			// 프로시저와 함께 등록한 인자
	);
typedef BOOL (*LPFN_RECV_NOTIFY_PROC) (		// 세션 수신 이벤트 통보 프로시저
	CSessionBase* lpSession,				// 세션 포인터
	CHAR* lpcBuffer,						// 세션에 속한 수신버퍼의 시작 포인터
	/*in,out*/ INT& iBufferPtr,				// 세션에 속한 수신버퍼의 시작포인터로 부터 데이터가 시작(입력)/처리(출력) 된 위치 (앞에서 어느 위치까지 처리했는지 사용자가 알려주어야 함)
	UINT uIoBytes,							// 세션에 속한 수신된 데이터의 크기
	UINT uTotalBufferSize,					// 세션에 속한 수신버퍼의 실제크기
	LPVOID lpRecvNotifyProcParam,			// 프로시저와 함께 등록된 인자
	IN_ADDR stIpAddress,					// 세션의 원격지 IP주소
	WORD wPortNumber						// 세션의 원격지 포트번호 (네트워크 바이트 오더)
	);
typedef VOID (*LPFN_ERROR_NOTIFY_PROC) (	// 네트워크 모듈의 에러를 통보
	LPVOID lpModule,						// 네트워크 모듈객체 포인터
	INT iErrorCode,							// 에러 코드
	LPCTSTR lpszErrorMessage,				// 에러 메시지
	LPVOID lpErrorNotifyProcParam			// 프로시저와 함께 등록된 인자
	);


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CSessionBase
//*---------------------------------------------------------------
// DESC : Session Base Class
// PRGM : B4nFter
//*---------------------------------------------------------------
class CSessionBase
{

public:
	typedef	CCriticalSection	TP_LOCK;
	typedef CLockAuto			TP_LOCKAUTO;
	
public:
	CSessionBase();
	virtual ~CSessionBase();

	virtual DWORD Open();
	virtual VOID Close();
	virtual VOID Reset();

	SOCKET GetSocket() const { return m_sckThis; }
	BOOL IsValid() const { return(INVALID_SOCKET != m_sckThis); }

	TP_LOCK* GetLock() const { return &m_Lock; }
	MODE_DBG(BOOL IsLock() const { return m_Lock.IsLock(); });

	LPVOID GetParam() const { return m_lpParam; }
	VOID SetParam(LPVOID lpParam) { m_lpParam = lpParam; }
	
	LPVOID GetThis() { return this; }
	INT GetErrorCode() { return m_ErrorFormat.GetErrorCode(); }

	VOID SetSckBufferSize(INT pRecvSckBufferSize, INT pSendSckBufferSize) {
		m_RecvSckBufferSize = pRecvSckBufferSize;
		m_SendSckBufferSize = pSendSckBufferSize;
	}
	VOID SetRecvSckBufferSize(INT pRecvSckBufferSize) { m_RecvSckBufferSize = pRecvSckBufferSize; }
	VOID SetSendSckBufferSize(INT pSendSckBufferSize) { m_SendSckBufferSize = pSendSckBufferSize; }
	INT GetRecvSckBufferSize() const { return m_RecvSckBufferSize; }
	INT GetSendSckBufferSize() const { return m_SendSckBufferSize; }

	INT GetSockOpt(INT pLevel, INT pOptName, PCHAR pOptVal, PINT pOptLen) const;
	INT SetSockOpt(INT pLevel, INT pOptName, PCHAR pOptVal, INT pOptLen);

	virtual BOOL CreateSocket() = 0;
	virtual BOOL Connect(LPCSTR lpszIpAddress, WORD wPortNumber) = 0;
	virtual BOOL Connect(ULONG uIpAddress, WORD wPortNumber) = 0;
	virtual VOID Disconnect() = 0;

	virtual BOOL SendData(LPBYTE lpbtBuffer, INT iBufferSize, LPCSTR lpszIpAddress, WORD wPortNumber, BOOL bDoLock = TRUE) = 0;			// UDP
	virtual BOOL SendData(LPBYTE lpbtBuffer, INT iBufferSize, ULONG uIpAddress, WORD wPortNumber, BOOL bDoLock = TRUE) = 0;				// UDP
	virtual BOOL SendData(LPBYTE lpbtBuffer, INT iBufferSize, BOOL bDoLock = TRUE) = 0;													// TCP / UDP

protected:
	virtual VOID CloseSocket() = 0;		// 가급적 외부로의 노출을 피하고 상속받은 쪽에서 필요한 경우만 public 멤버로 선언 이동 (가상함수 관계에 지장없음)

protected:
	SOCKET m_sckThis;
	LPVOID m_lpParam;
	CErrorFormat m_ErrorFormat;
	mutable TP_LOCK m_Lock;
	INT m_RecvSckBufferSize;		// 세션의 수신용 소켓버퍼 크기 (보관용도)
	INT m_SendSckBufferSize;		// 세션의 수신용 소켓버퍼 크기 (보관용도)

};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::CSessionBase
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CSessionBase::CSessionBase() : m_sckThis(INVALID_SOCKET), m_lpParam(NULL), m_RecvSckBufferSize(-1), m_SendSckBufferSize(-1)
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::~CSessionBase
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CSessionBase::~CSessionBase()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::Reset
//*---------------------------------------------------------------
// DESC : 객체 자원 리셋
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CSessionBase::Reset()
{
//	m_lpParam = NULL;
// 	m_RecvSckBufferSize = -1;
// 	m_SendSckBufferSize = -1;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::Open
//*---------------------------------------------------------------
// DESC : 기본 세션 시작
// PARM : N/A
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
inline DWORD CSessionBase::Open()
{
	DWORD dwRetVal = m_Lock.Open();
	if (NOERROR != dwRetVal) {
		return dwRetVal;
	}
	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::Close
//*---------------------------------------------------------------
// DESC : 기본 세션 종료
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CSessionBase::Close()
{
	m_Lock.Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::GetSockOpt
//*---------------------------------------------------------------
// DESC : 본 세션의 소켓에 대한 특정 옵션을 얻음
// PARM :	1 . pLevel - 소켓 옵션이 위치한 레벨
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			2 . pOptName - 값을 얻고자 하는 소켓 옵션의 이름 (pLevel 마다 각기 다르므로 참고)
//			3 . pOptVal - 요청한 옵션 값을 얻어올 버퍼
//			4 . pOptLen - 요청한 옵션 값을 얻어올 버퍼의 크기
// RETV : NOERROR - 성공 / 그외 - 실패 (::WSAGetLastError() 값)
// PRGM : B4nFter
// P.S.>
//		- 아래 MSDN URL 에서 getsockopt() 옵션 참고할 것
//			http://msdn.microsoft.com/en-us/library/ms738544.aspx
//*---------------------------------------------------------------
inline INT CSessionBase::GetSockOpt(INT pLevel, INT pOptName, PCHAR pOptVal, PINT pOptLen) const
{
	INT iRetVal = ::getsockopt(m_sckThis, pLevel, pOptName, pOptVal, pOptLen);
	if (SOCKET_ERROR == iRetVal) 
		BASE_RETURN(::WSAGetLastError());

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::SetSockOpt
//*---------------------------------------------------------------
// DESC : 본 세션의 소켓에 특정 옵션을 지정
// PARM :	1 . pLevel - 소켓 옵션이 위치한 레벨
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			2 . pOptName - 값을 지정하고자 하는 소켓 옵션의 이름 (pLevel 마다 각기 다르므로 참고)
//			3 . pOptVal - 지정할 옵션 값이 위치한 버퍼
//			4 . pOptLen - 지정할 옵션 값이 위치한 버퍼의 크기
// RETV : NOERROR - 성공 / 그외 - 실패 (::WSAGetLastError() 값)
// PRGM : B4nFter
// P.S.>
//		- 아래 MSDN URL 에서 setsockopt() 옵션 참고할 것
//			http://msdn.microsoft.com/en-us/library/ms740476.aspx
//*---------------------------------------------------------------
inline INT CSessionBase::SetSockOpt(INT pLevel, INT pOptName, PCHAR pOptVal, INT pOptLen)
{
	INT iRetVal = ::setsockopt(m_sckThis, pLevel, pOptName, pOptVal, pOptLen);
	if (SOCKET_ERROR == iRetVal)
		BASE_RETURN(::WSAGetLastError());
	
	return NOERROR;
}

