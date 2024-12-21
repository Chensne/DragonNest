#pragma once
#include "NxSession.h"
#include "NxSessionManager.h"

class DnDummyClient;

class DnDummySession : public NxSession
{
public:
	DnDummySession(DWORD nUID, NxServerApp* pServerApp);
	virtual ~DnDummySession();

	void	SetDummyClient(DnDummyClient* pDummyClient) { m_pDummyClient = pDummyClient; }
	// Ŭ���̾�Ʈ�� ���� ������ ȣ��ȴ�. 
	virtual void	OnAccept( NxAsyncEvent* pEvent );
	// ������ ���� �Ǿ����� ȣ��ȴ�. ( Ŭ���̾�Ʈ �� ������ ���ӽ������� )
	virtual void	OnConnect( NxAsyncEvent* pEvent );
	// ���� ����
	virtual void	OnClose();
	// ����� ���� �̺�Ʈ
	virtual void	OnUserEvent( NxAsyncEvent* pEvent );
	// ��Ŷ ���Ž�
	virtual void	OnPacket( NxPacket& Packet );

	bool IsConnected() { return m_bConnected ; }
private:
	DnDummyClient* m_pDummyClient;
	bool			m_bConnected;
};

class DnDummySessionManager : public NxSessionManager
{
public:

	virtual void _CreateSessionPool(DWORD nSessionCnt);
};