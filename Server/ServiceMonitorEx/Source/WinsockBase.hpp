/*//===============================================================

	<< WINSOCK BASE >>

	PRGM : B4nFter

	FILE : WINSOCKBASE.HPP
	DESC : 모든 원속 클래스들의 기본 클래스
	INIT BUILT DATE : 2005. 04. 28
	LAST BUILT DATE : 2005. 04. 28

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "ERRORFORMAT.H"
#include "SYSBASEBASE.H"
#include "IPV4.HPP"
#include <LIST>


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CWinsockBase
//*---------------------------------------------------------------
// DESC : Winsock Base Socket Class
// PRGM : B4nFter
//*---------------------------------------------------------------
class CWinsockBase
{
	
public:
	static enum {
		EV_ERROR_NONE,		// 에러 - none
		EV_ERROR_SYSTEM,	// 에러 - system
		EV_ERROR_LOGIC		// 에러 - logic
	};
	typedef std::list<std::string>	TP_IPLIST;
	typedef TP_IPLIST::iterator		TP_IPLIST_ITR;

public:
	inline CWinsockBase();
	inline virtual ~CWinsockBase();		// P.S.> 명백한 오류, 차후 수정 (*.h, *.cpp 로 분리 필요)

	INT GetErrorCode() { return m_ErrorFormat.GetErrorCode(); }

	inline static BOOL GetPhysicalAddress(LPBYTE lpbtPhysicalAddress);
	inline static BOOL GetHostByName(LPCSTR lpszHostName, ULONG& uIpAddress);
	inline static INT GetLocalIpAddressList(TP_IPLIST& pLocalIpAddressList);

	inline static INT GetSockOpt(SOCKET pSocket, INT pLevel, INT pOptName, PCHAR pOptVal, PINT pOptLen);
	inline static INT SetSockOpt(SOCKET pSocket, INT pLevel, INT pOptName, PCHAR pOptVal, INT pOptLen);

private:
	inline BOOL Initialize();
	inline VOID Finalize();

protected:
	SOCKADDR_IN m_stSockAddr;
	WSADATA m_stWsaData;
	CErrorFormat m_ErrorFormat;
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::CWinsockBase
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CWinsockBase::CWinsockBase()
{
	Initialize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::~CWinsockBase
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CWinsockBase::~CWinsockBase()
{
	Finalize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::Initialize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 초기화
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CWinsockBase::Initialize()
{
	INT iErrNo	= 0;
	iErrNo = ::WSAStartup(MAKEWORD(2,2), & m_stWsaData);

	if (iErrNo) {
		switch (iErrNo) {
		case WSASYSNOTREADY :
		case WSAVERNOTSUPPORTED :
		case WSAEINPROGRESS :
		case WSAEPROCLIM :
		case WSAEFAULT :
			{
				TCHAR szText[128] = { _T('\0'), };
				SNPRINTF(MODE_DBG_EX(szText, COUNT_OF(szText)), COUNT_OF(szText), _T("CWinsockBase::Initialize() - WSAStartup() error : %d"), iErrNo);
				m_ErrorFormat.Write(szText);
				return FALSE;
			}
		}
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::Finalize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 해제
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CWinsockBase::Finalize()
{
	::WSACleanup();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::GetPhysicalAddress
//*---------------------------------------------------------------
// DESC : 호스트의 네트워크 물리주소 (MAC) 를 얻음
// PARM :	1 . lpbtPhysicalAddress - 호스트의 네트워크 물리주소 (MAC) 를 받을 BYTE배열의 포인터 (주의!!> . 최소 6바이트 이상)
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CWinsockBase::GetPhysicalAddress(LPBYTE lpbtPhysicalAddress)
{
	UUID stUuid;
	INT iRetVal = ::UuidCreateSequential(&stUuid);    // Ask OS to create UUID
	if (iRetVal != RPC_S_OK) {
		BASE_TRACE(_T("iRetVal != RPC_S_OK : %d - CWinsockBase::GetPhysicalAddress()"), iRetVal);
		return FALSE;
	}

	for (INT iCount = 2 ; iCount<8 ; ++iCount)	{
		// UUID 의 멤버 Data4 의 2 ~ 7 바이트 사이가 MAC 주소이다.
		lpbtPhysicalAddress[iCount-2] = stUuid.Data4[iCount];
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::GetHostByName
//*---------------------------------------------------------------
// DESC : IP 이름이 dotted decimal 형식이 아닌 domain name 형식일 때 이를 정수형태의 IP4 주소로 변경해 줌
// PARM :	1 . lpszHostName -  domain name 형식의 호스트 주소
//			2 . uIpAddress - 정수형식의 IP4 호스트 주소를 반환할 변수
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
/* EXAM :
	SOCKADDR_IN stSockAddr;
	...
	...
	stSockAddr.sin_addr.s_addr = ::inet_addr(ipszAddress);
	if (INADDR_NONE == stSockAddr.sin_addr.s_addr) {
		if (TRUE == CWinsockBase::GetHostByName(ipszAddress, stSockAddr.sin_addr.s_addr)) {
			// 번역성공
		}
		else {
			// 에러
		}
	}
*/
//*---------------------------------------------------------------
BOOL CWinsockBase::GetHostByName(LPCSTR lpszHostName, ULONG& uIpAddress)
{
	LPHOSTENT	lpstHostent;
	lpstHostent = ::gethostbyname(lpszHostName);
	if (lpstHostent) {
		uIpAddress = ((LPIN_ADDR)lpstHostent->h_addr)->s_addr;
	}
	else {
		return FALSE;
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::GetLocalIpAddressList
//*---------------------------------------------------------------
// DESC : 로컬호스트의 IP주소 리스트를 얻음
// PARM :	1 . pLocalIpAddressList - IP주소 목록을 받아올 리스트 객체
// RETV : 로컬호스트의 IP주소의 개수 반환
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CWinsockBase::GetLocalIpAddressList(TP_IPLIST& pLocalIpAddressList)
{
	CHAR szHostName[128];
	::gethostname(szHostName, sizeof(szHostName));

	LPHOSTENT lpstHostent = ::gethostbyname(szHostName);	// !!! 예외발생 - 0x000006C5 (1733) 태그가 틀립니다.
	if (!lpstHostent) {
		return 0;
	}

	LPIN_ADDR lpstInAddr;
	std::string stTempIpAddress;
	switch (lpstHostent->h_addrtype) {
	case AF_INET :
		while(NULL != (lpstInAddr = ((LPIN_ADDR)*lpstHostent->h_addr_list++))) {
			stTempIpAddress = ::inet_ntoa(*lpstInAddr);
			pLocalIpAddressList.push_back(stTempIpAddress);
		}
		break;
	}

	return (INT)pLocalIpAddressList.size();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::GetSockOpt
//*---------------------------------------------------------------
// DESC : 본 세션의 소켓에 대한 특정 옵션을 얻음
// PARM :	1 . pSocket - 옵션을 지정할 소켓
//			2 . pLevel - 소켓 옵션이 위치한 레벨
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			3 . pOptName - 값을 얻고자 하는 소켓 옵션의 이름 (pLevel 마다 각기 다르므로 참고)
//			4 . pOptVal - 요청한 옵션 값을 얻어올 버퍼
//			5 . pOptLen - 요청한 옵션 값을 얻어올 버퍼의 크기
// RETV : NOERROR - 성공 / 그외 - 실패 (::WSAGetLastError() 값)
// PRGM : B4nFter
// P.S.>
//		- 아래 MSDN URL 에서 getsockopt() 옵션 참고할 것
//			http://msdn.microsoft.com/en-us/library/ms738544.aspx
//*---------------------------------------------------------------
INT CWinsockBase::GetSockOpt(SOCKET pSocket, INT pLevel, INT pOptName, PCHAR pOptVal, PINT pOptLen)
{
	INT iRetVal = ::getsockopt(pSocket, pLevel, pOptName, pOptVal, pOptLen);
	if (SOCKET_ERROR == iRetVal) {
		BASE_RETURN(::WSAGetLastError());
	}
	
	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::SetSockOpt
//*---------------------------------------------------------------
// DESC : 본 세션의 소켓에 특정 옵션을 지정
// PARM :	1 . pSocket - 옵션을 지정할 소켓
//			2 . pLevel - 소켓 옵션이 위치한 레벨
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			3 . pOptName - 값을 지정하고자 하는 소켓 옵션의 이름 (pLevel 마다 각기 다르므로 참고)
//			4 . pOptVal - 지정할 옵션 값이 위치한 버퍼
//			5 . pOptLen - 지정할 옵션 값이 위치한 버퍼의 크기
// RETV : NOERROR - 성공 / 그외 - 실패 (::WSAGetLastError() 값)
// PRGM : B4nFter
// P.S.>
//		- 아래 MSDN URL 에서 setsockopt() 옵션 참고할 것
//			http://msdn.microsoft.com/en-us/library/ms740476.aspx
//*---------------------------------------------------------------
INT CWinsockBase::SetSockOpt(SOCKET pSocket, INT pLevel, INT pOptName, PCHAR pOptVal, INT pOptLen)
{
	INT iRetVal = ::setsockopt(pSocket, pLevel, pOptName, pOptVal, pOptLen);
	if (SOCKET_ERROR == iRetVal) {
		BASE_RETURN(::WSAGetLastError());
	}
	
	return NOERROR;
}

