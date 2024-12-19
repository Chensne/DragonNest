
#pragma once

class CClientSocket;

#include "../DnPacketDll/DnPacketDll.h"

class CTestModule
{
public:

	CTestModule();
	~CTestModule();
	
	void ConnectLoginServer();

private:

	CClientSocket*			m_pTcpSocket;
	DNTest::DNPacketDll*	m_pPacketDll;
};
