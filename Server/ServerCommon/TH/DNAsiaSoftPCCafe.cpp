#include "StdAfx.h"
#include "DNAsiaSoftPCCafe.h"
#include "DNIocpManager.h"
#include "DNDivisionManager.h"
#include "DNUser.h"
#include "Log.h"
//#include "TimeSet.h"

#if defined(_TH)
CDNAsiaSoftPCCafe *g_pAsiaSoftPCCafe = NULL;

CDNAsiaSoftPCCafe::CDNAsiaSoftPCCafe(char szIP[IPLENMAX], int nPort): CConnection()
{
	SetIp(szIP);
	SetPort(nPort);

	Init(1024*200, 1024*200);
	m_dwReconnectTick = 0;		
	m_bConnectDelay = false;
}

CDNAsiaSoftPCCafe::~CDNAsiaSoftPCCafe(void)
{
	
}

int CDNAsiaSoftPCCafe::AddSendData(char *pData, int nLen)
{
	int Ret = 0;

	if (GetDelete()) return -1;
	if (m_pIocpManager && m_pSocketContext && (m_pSocketContext->m_Socket != INVALID_SOCKET))
	{
		m_SendSync.Lock();
		Ret = m_pSendBuffer->Push( pData, nLen );						// 데이터
		m_SendSync.UnLock();

		// g_Log.Log(L"[SID:%u] CConnection::AddSendData (%d)\r\n", m_nSessionID, nSize);
		if (Ret == 0)
		{
			m_pIocpManager->m_nAddSendBufSize += nLen;
			m_pIocpManager->AddSendCall(m_pSocketContext);
		}
		else if (Ret < 0)
			m_pIocpManager->DetachSocket(m_pSocketContext, L"AddSendData Error");		
	}
	return Ret;
}

void CDNAsiaSoftPCCafe::Reconnect(DWORD dwCurTick)
{
	if (m_dwReconnectTick + 5000 < dwCurTick){
		m_dwReconnectTick = dwCurTick;

		if (!GetActive() && !GetConnecting() )
		{
			if( m_bConnectDelay == false )
			{
				m_dwReconnectTick+=2000;
				m_bConnectDelay = true;
				return;
			}
			m_bConnectDelay = false;
			// 여기서 이전 끊어진 거 정리하기..
			if( m_pIocpManager && m_pSocketContext )			
			{
				m_pIocpManager->ClearSocketContext(m_pSocketContext);
				SetSocketContext(NULL, NULL);
			}		
			
			SetConnecting(true);
			// 따로 만들지 않고 AUTH껄 쓴다.
			if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_TH_AUTH, m_szIP, m_wPort) < 0)
			{
				SetConnecting(false);
				g_Log.Log(LogType::_ERROR, L"AsiaSoftPCCafeServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else {
				g_Log.Log(LogType::_NORMAL, L"AsiaSoftPCCafeServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);
				
			}
		}
	}	
}

int CDNAsiaSoftPCCafe::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{	
	g_Log.Log(LogType::_NORMAL, L"[AsiaSoftPCCafe] %S\r\n", pData);
	int nResultIndex = 0;
	char* token = strtok(pData,"|");
	int nResult = 0;
	int nPcCafeLevel = 0;
	char* strOutString = NULL;

	UINT nAccountDBID = 0;
	while(token != NULL )
	{		
		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : strOutString = token; break;
		case 2 : nPcCafeLevel = atoi(token); break;				
		case 3 : nAccountDBID = strtoul(token, NULL, 10); break;
		}
		token = strtok(NULL,"|");
		++ nResultIndex;
	}

	CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(nAccountDBID);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[AsiaSoftPCCafe:MessageProcess] User NULL! (AccountDBID:%u) \r\n", nAccountDBID);
		return ERROR_NONE;
	}

	if( nResult == 0)	
	{
		if( nPcCafeLevel > PCBang::Grade::Normal && nPcCafeLevel <= PCBang::Grade::Premium )
		{
			pUser->SetPCBang(true);
			pUser->SetPCGrade(nPcCafeLevel);
			g_pDivisionManager->SendPCBangResult(pUser);
			g_Log.Log(LogType::_NORMAL, L"[AsiaSoftPCCafe:MessageProcess] PCCafe IPBonus:%d (AccountName:%s)\r\n",nPcCafeLevel, pUser->GetAccountName());	
		}	
	}
	else
	{
		//기타 나머지 에러처리..
		g_Log.Log(LogType::_ERROR, L"[AsiaSoftPCCafe:MessageProcess] PCCafe Fail! (AccountDBID:%u) %d:%S\r\n", nAccountDBID, nResult, strOutString);	
	}
	return ERROR_NONE;
}

void CDNAsiaSoftPCCafe::SendCheckIPBonus(const char *pAccountName, char *pIp, char cType,  UINT nAccountDBID)	
{	
	char strMessage[256] = {0,};
	SNPRINTFA(strMessage, 256, "%s|%s|%d|%d;", pAccountName, pIp, cType, nAccountDBID);

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));	
}

#endif