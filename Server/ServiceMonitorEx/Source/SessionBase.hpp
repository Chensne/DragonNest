/*//===============================================================

	<< SESSION BASE >>

	PRGM : B4nFter

	FILE : SESSIONBASE.HPP
	DESC : ���� Ŭ�������� �⺻ Ŭ���� (�߻� Ŭ������ �⺻ �������̽��� ����)
	INIT BUILT DATE : 2007. 09. 09
	LAST BUILT DATE : 2007. 09. 12

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

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

class CSessionBase;							// ���� ��������

enum EF_NETWORK_BASE_NOFITY {				// ��Ʈ��ũ �̺�Ʈ �뺸 Ÿ��
	EV_NBN_ERROR = 0,						// ��� ���� (TCP/UDP)
	EV_NBN_SESSCNNT,						// ���� ���� (TCP)
	EV_NBN_SESSDCNT,						// ���� ���� (TCP) / ��ȿ�� ������ �ƴ� (UDP)
	EV_NBN_SESSERROR,						// ���� ���� (TCP/UDP)
};

typedef VOID (*LPFN_BASE_NOTIFY_PROC) (		// ���� �⺻ �̺�Ʈ �뺸 ���ν���
	EF_NETWORK_BASE_NOFITY eNotifyType,		// ��Ʈ��ũ �̺�Ʈ �뺸 Ÿ��
	CSessionBase* lpSession,				// ���� ������
	LPVOID lpResultParam,					// ��� ���� - �پ��� �뵵�� ����
	LPVOID lpBaseNotifyProcParam			// ���ν����� �Բ� ����� ����
	);
typedef BOOL (*LPFN_RECV_NOTIFY_PROC) (		// ���� ���� �̺�Ʈ �뺸 ���ν���
	CSessionBase* lpSession,				// ���� ������
	CHAR* lpcBuffer,						// ���ǿ� ���� ���Ź����� ���� ������
	/*in,out*/ INT& iBufferPtr,				// ���ǿ� ���� ���Ź����� ���������ͷ� ���� �����Ͱ� ����(�Է�)/ó��(���) �� ��ġ (�տ��� ��� ��ġ���� ó���ߴ��� ����ڰ� �˷��־�� ��)
	UINT uIoBytes,							// ���ǿ� ���� ���ŵ� �������� ũ��
	UINT uTotalBufferSize,					// ���ǿ� ���� ���Ź����� ����ũ��
	LPVOID lpRecvNotifyProcParam,			// ���ν����� �Բ� ��ϵ� ����
	IN_ADDR stIpAddress,					// ������ ������ IP�ּ�
	WORD wPortNumber						// ������ ������ ��Ʈ��ȣ (��Ʈ��ũ ����Ʈ ����)
	);
typedef VOID (*LPFN_ERROR_NOTIFY_PROC) (	// ��Ʈ��ũ ����� ������ �뺸
	LPVOID lpModule,						// ��Ʈ��ũ ��ⰴü ������
	INT iErrorCode,							// ���� �ڵ�
	LPCTSTR lpszErrorMessage,				// ���� �޽���
	LPVOID lpErrorNotifyProcParam			// ���ν����� �Բ� ��ϵ� ����
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
	virtual VOID CloseSocket() = 0;		// ������ �ܺη��� ������ ���ϰ� ��ӹ��� �ʿ��� �ʿ��� ��츸 public ����� ���� �̵� (�����Լ� ���迡 �������)

protected:
	SOCKET m_sckThis;
	LPVOID m_lpParam;
	CErrorFormat m_ErrorFormat;
	mutable TP_LOCK m_Lock;
	INT m_RecvSckBufferSize;		// ������ ���ſ� ���Ϲ��� ũ�� (�����뵵)
	INT m_SendSckBufferSize;		// ������ ���ſ� ���Ϲ��� ũ�� (�����뵵)

};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSessionBase::CSessionBase
//*---------------------------------------------------------------
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : ��ü �ڿ� ����
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
// DESC : �⺻ ���� ����
// PARM : N/A
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : �⺻ ���� ����
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
// DESC : �� ������ ���Ͽ� ���� Ư�� �ɼ��� ����
// PARM :	1 . pLevel - ���� �ɼ��� ��ġ�� ����
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			2 . pOptName - ���� ����� �ϴ� ���� �ɼ��� �̸� (pLevel ���� ���� �ٸ��Ƿ� ����)
//			3 . pOptVal - ��û�� �ɼ� ���� ���� ����
//			4 . pOptLen - ��û�� �ɼ� ���� ���� ������ ũ��
// RETV : NOERROR - ���� / �׿� - ���� (::WSAGetLastError() ��)
// PRGM : B4nFter
// P.S.>
//		- �Ʒ� MSDN URL ���� getsockopt() �ɼ� ������ ��
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
// DESC : �� ������ ���Ͽ� Ư�� �ɼ��� ����
// PARM :	1 . pLevel - ���� �ɼ��� ��ġ�� ����
/*
				IPPROTO_IP
				IPPROTO_IPV6
				IPPROTO_TCP
				IPPROTO_UDP
				SOL_SOCKET
				SOL_IRLMP
*/
//			2 . pOptName - ���� �����ϰ��� �ϴ� ���� �ɼ��� �̸� (pLevel ���� ���� �ٸ��Ƿ� ����)
//			3 . pOptVal - ������ �ɼ� ���� ��ġ�� ����
//			4 . pOptLen - ������ �ɼ� ���� ��ġ�� ������ ũ��
// RETV : NOERROR - ���� / �׿� - ���� (::WSAGetLastError() ��)
// PRGM : B4nFter
// P.S.>
//		- �Ʒ� MSDN URL ���� setsockopt() �ɼ� ������ ��
//			http://msdn.microsoft.com/en-us/library/ms740476.aspx
//*---------------------------------------------------------------
inline INT CSessionBase::SetSockOpt(INT pLevel, INT pOptName, PCHAR pOptVal, INT pOptLen)
{
	INT iRetVal = ::setsockopt(m_sckThis, pLevel, pOptName, pOptVal, pOptLen);
	if (SOCKET_ERROR == iRetVal)
		BASE_RETURN(::WSAGetLastError());
	
	return NOERROR;
}

