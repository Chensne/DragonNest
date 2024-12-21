#pragma once

#include "NxAsyncEvent.h"
#include "NxPacket.h"

class NxNetInit;
class NxAcceptor;
class NxCompletionPort;
class NxConnection;
class NxConnectionManager;
class NxSession;
class NxSessionManager;
class NxTimerThread;


class NxServerApp : public NxAsyncEventHandler
{
public:
	NxServerApp();
	virtual ~NxServerApp();

	// ���� ����
	virtual bool	Create( int nWorkerTheadCnt, int nSessionPoolCnt, DWORD nTimerValue );

	virtual bool	BeginAcceptor(const TCHAR* szIPAddress, unsigned short nPort);
	virtual void	Destroy();

	// ���� ���Ͻ� �Ŵ����� �����Ѵ�. ������ �ؾߵ�
	virtual bool	CreateSessionManager();

	virtual NxSession* OpenSession(const TCHAR* szIPAddress, unsigned short nPort);
	virtual bool	CloseSession(DWORD nUID);

	void	GetLocalIpPort( DWORD nUID, OUT tstring& szIP, OUT WORD& nPort );

public:
	virtual	void	HandleEvent( NxAsyncEvent* pEvent );

	// Ŭ���̾�Ʈ�� ���� ������ ȣ��ȴ�. 
	virtual void	OnAccept( DWORD nUID, NxAsyncEvent* pEvent );
	// ������ ���� �Ǿ����� ȣ��ȴ�. ( Ŭ���̾�Ʈ �� ������ ���ӽ������� )
	virtual void	OnConnect( DWORD nUID, NxAsyncEvent* pEvent );
	// ���� ����
	virtual void	OnClose( DWORD nUID );
	// ����� ���� �̺�Ʈ
	virtual void	OnUserEvent( DWORD nUID, NxAsyncEvent* pEvent );
	// ��Ŷ ���Ž�
	virtual void	OnPacket( DWORD nUID, NxPacket& Packet );
	// ��Ŷ �۽Ž�
	virtual void	SendPacket( DWORD nUID, NxPacket& Packet );
	// Ÿ�̸�
	virtual void	OnTimer(DWORD dt) { }
	
public:

	static void		AddSendBytes(DWORD nBytes) { m_nTotalSendBytes += nBytes; }
	static void		AddRecvBytes(DWORD nBytes) { m_nTotalRecvBytes += nBytes; }
	static void		AddSendCount() { ++m_nTotalSendCnt; }
	static void		AddRecvCount() { ++m_nTotalRecvCnt; }
	static void		ClearIOInfo() { m_nTotalSendBytes = m_nTotalRecvBytes = m_nTotalSendCnt = m_nTotalRecvCnt = 0; }

public:

	static DWORD	m_nTotalSendBytes;
	static DWORD	m_nTotalRecvBytes;

	static DWORD	m_nTotalSendCnt;
	static DWORD	m_nTotalRecvCnt;

private:

	NxNetInit*				m_pNetInit;
	NxAcceptor*				m_pAcceptor;

	// IO �� CP
	NxCompletionPort*		m_pNetIOCompletionPort;
	// Msg ó���� CP
	NxCompletionPort*		m_pMsgCompletionPort;
	// Ŀ�ؼ� �Ŵ���
	NxConnectionManager*	m_pConnectionManager;

	// Ÿ�̸� ������
	//NxTimerThread*			m_pTimerThread;
	// ������ Ÿ��
	DWORD					m_nLastTime;

protected:
	// ����  �Ŵ���
	NxSessionManager*		m_pSessionManager;


};