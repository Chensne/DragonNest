#include "StdAfx.h"
#include "TestClient.h"

CTestClient::CTestClient(void)
{
}

CTestClient::~CTestClient(void)
{
}

void CTestClient::OnIoRead(void)
{
	CClientSession::OnIoRead();
}

void CTestClient::OnIoConnected(void)
{
}

void CTestClient::OnIoDisconnected(void)
{
}
