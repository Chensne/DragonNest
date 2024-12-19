// DnPacketDllTest.cpp : Defines the entry point for the console application.
//


/*
테스트를 하려면 반드시 DNDefine.h 에서 아래 디파인을 활성화 시켜야 합니다.
PRE_TEST_PACKETMODULE
*/

#include "stdafx.h"
#include "DNTest.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"

class CDnClientSocket : public CClientSocket
{
public:

	CDnClientSocket( DNTest::DNPacketDll* pPacketDll )
		:m_pPacketDll(pPacketDll)
	{
	}

	virtual void OnRecv()
	{
		CClientSocket::OnRecv();

		UINT uiSize = m_pRecvBuffer->GetCount();
		if( uiSize )
		{
			char buffer[32768];
			memset( buffer, 0, sizeof(buffer) );

			m_pRecvBuffer->Pop( buffer, uiSize );

			UINT uiParseSize = m_pPacketDll->OnReceive( buffer, uiSize );
			if( uiSize > uiParseSize )
			{
				m_pRecvBuffer->Push( buffer+uiParseSize, uiSize-uiParseSize );
			}
		}
	}

	virtual void OnConnect()
	{
		CClientSocket::OnConnect();
		m_pPacketDll->OnConnect();
	}

	virtual void OnDisconnect( bool bValidDisconnect )
	{
		CClientSocket::OnDisconnect( bValidDisconnect );
		m_pPacketDll->OnDisconnect();
	}

	void Push( const char* pBuf, const UINT uiSize )
	{
		m_pSendBuffer->Push( pBuf, uiSize );
		DoSend();
	}

private:

	DNTest::DNPacketDll* m_pPacketDll;
};

class CDNPacketDll : public DNTest::DNPacketDll
{
public:

	// 실제로 Send 구현.
	virtual void OnSend( const char* pBuf, const UINT uiSize )
	{
		m_pSocket->Push( pBuf, uiSize );
	}

	virtual void OnFinishLoginProcess()
	{
		m_pSocket->Close();
	}

	void SetSocket( CDnClientSocket* pSocket )
	{
		m_pSocket = pSocket;
	}

private:

	CDnClientSocket* m_pSocket;
};

//#####################################################################################################################
// CTestModule
//#####################################################################################################################

CTestModule::CTestModule()
{
	m_pPacketDll	= new CDNPacketDll();
	m_pPacketDll->SetAccountInfo( L"kimbab", L"1111" );
	m_pTcpSocket	= new CDnClientSocket( m_pPacketDll );
	dynamic_cast<CDNPacketDll*>(m_pPacketDll)->SetSocket( dynamic_cast<CDnClientSocket*>(m_pTcpSocket) );
	new CClientSessionManager( m_pTcpSocket );
}

CTestModule::~CTestModule()
{
	delete m_pPacketDll;
	CClientSessionManager::DestroyInstance();
}

void CTestModule::ConnectLoginServer()
{
	//CClientSessionManager::GetInstance().ConnectLoginServer( "10.0.3.25", 14300, true );
	CClientSessionManager::GetInstance().ConnectLoginServer( "192.168.0.21", 14300, true );
}

//#####################################################################################################################
// Test
//#####################################################################################################################

int _tmain(int argc, _TCHAR* argv[])
{
	CTestModule Test;
	Test.ConnectLoginServer();

	char szCmd[256] = {0};	
	while (1)
	{
		if (strcmp(szCmd, "exit") == 0)	
			break;

		gets(szCmd);
	}

	return 0;
}
