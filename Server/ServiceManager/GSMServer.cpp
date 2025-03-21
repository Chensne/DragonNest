
#include "stdafx.h"
#include "GSMServer.h"
#include "ServiceManager.h"
#include "log.h"

CGSMServer::CGSMServer()
{
	memset(m_szGsmResponse, 0x00, sizeof(m_szGsmResponse));
	strcpy_s(m_szGsmResponse[_ARGUMENT_INSUFFICIENCY], "Argument Insufficiency");
	strcpy_s(m_szGsmResponse[_UNKNONW_MESSAGE], "Unkown Message");
	strcpy_s(m_szGsmResponse[_MESSAGE_OK], "OK");
}

CGSMServer::~CGSMServer()
{
}

bool CGSMServer::Open(USHORT nAcceptPort)
{
	return CSimpleServerFrame::Open(nAcceptPort);
}


#include "GsmCmdProcess.h"
void CGSMServer::Recv(SOCKET * pSocket, const char * pData, int nLen)
{
	if (nLen <= 0 || nLen-4 >= 256) return;
	char szMsg[256], szRet[256];
	memset(szMsg, 0, sizeof(szMsg));
	memcpy(szMsg, pData+4, nLen-4);
	strcpy_s(szRet, "Unkown Message");

	if (nLen-4 < 256)
	{
		int nRet = g_pServiceManager->Parse(szMsg, szRet);
		if (nRet != 0)
		{
			strcpy_s(szRet, nRet == -1 ? m_szGsmResponse[_ARGUMENT_INSUFFICIENCY] : m_szGsmResponse[_UNKNONW_MESSAGE]);
			g_Log.Log(LogType::_FILELOG, L"Msg Process Fail [%S] [Err:%s]\n", szMsg, nRet == -1 ? L"Argument Insufficiency" : L"Unkown Message");
		}
		else
		{
			strcpy_s(szRet, m_szGsmResponse[_MESSAGE_OK]);
			g_Log.Log(LogType::_FILELOG, L"Msg Process OK [%S]\n", szMsg);
		}

		CGsmCmdProcess::GetInstance()->ParseCmdProcess(szMsg);
	}

	send(*pSocket, szRet, (int)strlen(szRet), 0); 
}