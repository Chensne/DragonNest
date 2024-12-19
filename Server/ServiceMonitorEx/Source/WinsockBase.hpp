/*//===============================================================

	<< WINSOCK BASE >>

	PRGM : B4nFter

	FILE : WINSOCKBASE.HPP
	DESC : ��� ���� Ŭ�������� �⺻ Ŭ����
	INIT BUILT DATE : 2005. 04. 28
	LAST BUILT DATE : 2005. 04. 28

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

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
		EV_ERROR_NONE,		// ���� - none
		EV_ERROR_SYSTEM,	// ���� - system
		EV_ERROR_LOGIC		// ���� - logic
	};
	typedef std::list<std::string>	TP_IPLIST;
	typedef TP_IPLIST::iterator		TP_IPLIST_ITR;

public:
	inline CWinsockBase();
	inline virtual ~CWinsockBase();		// P.S.> ����� ����, ���� ���� (*.h, *.cpp �� �и� �ʿ�)

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
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : ��ü ���� �ڿ����� �ʱ�ȭ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ��ü ���� �ڿ����� ����
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
// DESC : ȣ��Ʈ�� ��Ʈ��ũ �����ּ� (MAC) �� ����
// PARM :	1 . lpbtPhysicalAddress - ȣ��Ʈ�� ��Ʈ��ũ �����ּ� (MAC) �� ���� BYTE�迭�� ������ (����!!> . �ּ� 6����Ʈ �̻�)
// RETV : TRUE - ���� / FALSE - ����
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
		// UUID �� ��� Data4 �� 2 ~ 7 ����Ʈ ���̰� MAC �ּ��̴�.
		lpbtPhysicalAddress[iCount-2] = stUuid.Data4[iCount];
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CWinsockBase::GetHostByName
//*---------------------------------------------------------------
// DESC : IP �̸��� dotted decimal ������ �ƴ� domain name ������ �� �̸� ���������� IP4 �ּҷ� ������ ��
// PARM :	1 . lpszHostName -  domain name ������ ȣ��Ʈ �ּ�
//			2 . uIpAddress - ���������� IP4 ȣ��Ʈ �ּҸ� ��ȯ�� ����
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
/* EXAM :
	SOCKADDR_IN stSockAddr;
	...
	...
	stSockAddr.sin_addr.s_addr = ::inet_addr(ipszAddress);
	if (INADDR_NONE == stSockAddr.sin_addr.s_addr) {
		if (TRUE == CWinsockBase::GetHostByName(ipszAddress, stSockAddr.sin_addr.s_addr)) {
			// ��������
		}
		else {
			// ����
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
// DESC : ����ȣ��Ʈ�� IP�ּ� ����Ʈ�� ����
// PARM :	1 . pLocalIpAddressList - IP�ּ� ����� �޾ƿ� ����Ʈ ��ü
// RETV : ����ȣ��Ʈ�� IP�ּ��� ���� ��ȯ
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CWinsockBase::GetLocalIpAddressList(TP_IPLIST& pLocalIpAddressList)
{
	CHAR szHostName[128];
	::gethostname(szHostName, sizeof(szHostName));

	LPHOSTENT lpstHostent = ::gethostbyname(szHostName);	// !!! ���ܹ߻� - 0x000006C5 (1733) �±װ� Ʋ���ϴ�.
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
// DESC : �� ������ ���Ͽ� ���� Ư�� �ɼ��� ����
// PARM :	1 . pSocket - �ɼ��� ������ ����
//			2 . pLevel - ���� �ɼ��� ��ġ�� ����
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			3 . pOptName - ���� ����� �ϴ� ���� �ɼ��� �̸� (pLevel ���� ���� �ٸ��Ƿ� ����)
//			4 . pOptVal - ��û�� �ɼ� ���� ���� ����
//			5 . pOptLen - ��û�� �ɼ� ���� ���� ������ ũ��
// RETV : NOERROR - ���� / �׿� - ���� (::WSAGetLastError() ��)
// PRGM : B4nFter
// P.S.>
//		- �Ʒ� MSDN URL ���� getsockopt() �ɼ� ������ ��
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
// DESC : �� ������ ���Ͽ� Ư�� �ɼ��� ����
// PARM :	1 . pSocket - �ɼ��� ������ ����
//			2 . pLevel - ���� �ɼ��� ��ġ�� ����
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			3 . pOptName - ���� �����ϰ��� �ϴ� ���� �ɼ��� �̸� (pLevel ���� ���� �ٸ��Ƿ� ����)
//			4 . pOptVal - ������ �ɼ� ���� ��ġ�� ����
//			5 . pOptLen - ������ �ɼ� ���� ��ġ�� ������ ũ��
// RETV : NOERROR - ���� / �׿� - ���� (::WSAGetLastError() ��)
// PRGM : B4nFter
// P.S.>
//		- �Ʒ� MSDN URL ���� setsockopt() �ɼ� ������ ��
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

