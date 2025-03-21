#include "StdAfx.h"
#include "DNGamaniaAuth.h"
#include "DNIocpManager.h"
#include "Log.h"
#include "DNServerPacket.h"
#include "Stream.h"
#include "Util.h"
#include "StringSet.h"
#include "TimeSet.h"
#if defined(_LOGINSERVER)
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#endif	// _LOGINSERVER

extern TLoginConfig g_Config;

#if defined(_TW) && defined(_FINAL_BUILD)

CDNGamaniaAuth* g_pGamaniaAuthLogin;

CDNGamaniaAuth* g_pGamaniaAuthLogOut;

CDNGamaniaAuth::CDNGamaniaAuth(eConnectionKey pConnectionKey, char szIP[IPLENMAX], int nPort): CConnection()
{
	DN_ASSERT(CONNECTIONKEY_DEFAULT != pConnectionKey,	"Invalid!");

	SetIp(szIP);
	SetPort(nPort);

	Init(1024 * 200, 1024 * 200);

 	m_dwReconnectTick = 0;
	m_ConnectionKey = pConnectionKey;
}

CDNGamaniaAuth::~CDNGamaniaAuth(void)
{
}

void CDNGamaniaAuth::DoUpdate(DWORD CurTick)
{
/*
	if (m_dwAuthAliveTick + 60000 < CurTick){
		SendAlive();
		m_dwAuthAliveTick = CurTick;
	}
*/
}

void CDNGamaniaAuth::Reconnect(DWORD CurTick)
{
	return;

	if (m_dwReconnectTick + 5000 < CurTick){
		m_dwReconnectTick = CurTick;

		if (!GetActive() && !GetConnecting() )
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, m_ConnectionKey, m_szIP, m_wPort) < 0) 
			{
				SetConnecting(false);
				g_Log.Log(LogType::_FILELOG, L"AuthServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else
			{
				g_Log.Log(LogType::_FILELOG, L"AuthServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);				
			}
		}
	}
}

int CDNGamaniaAuth::MessageProcess(int /*nMainCmd*/, int /*nSubCmd*/, char *pData, int nLen)
{
	return 0;


#if defined(_LOGINSERVER)	
	int nResultIndex = 0;
	char* token = strtok(pData,";");
	int nResult = 0;
	char* strOutString = NULL;
	char* strAccountID = NULL;
	char* strRegion = NULL;
	UINT nSessionID = 0;
	while(token != NULL )
	{
		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : strOutString = token; break;		
		case 2 : nSessionID = strtoul(token, NULL, 10); break;			
		case 3 : strAccountID = token; break;
		case 4 : strRegion = token; break;
		}
		token = strtok(NULL,";");
		++ nResultIndex;
	}	

#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock(g_pUserConnectionManager->m_ConSync);
#endif	// _USE_ACCEPTEX

	CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(nSessionID));
	if (!pUserCon) {
		g_Log.Log(LogType::_ERROR, L"[SID:%u] UserCon NULL\r\n", nSessionID);
		return 0;
	}

	switch(m_ConnectionKey)
	{
	case CONNECTIONKEY_TW_AUTHLOGIN:	// Authorization
		{
			if (nResult == 1) // ����
			{				
				if( strRegion )
				{
					BYTE cRegion = 0;
					if( strcmp(strRegion, "TW") == 0 )
						cRegion = 207; // �븸
					else if( strcmp(strRegion, "HK") == 0 )
						cRegion = 95; // ȫ��
					pUserCon->m_cRegion = cRegion;
				}
				pUserCon->CheckLoginTW();			
			}
			else // ����
			{
				pUserCon->SendCheckLogin(ConvertResult(strOutString));
				g_Log.Log(LogType::_ERROR, L"[SID:%u] CDNGamaniaAuth Fail %S:%S\r\n", nSessionID, strAccountID, strOutString);
			}
		}
		break;	
	default:
		DN_RETURN(ERROR_NONE);
	}
#endif //#if defined(_LOGINSERVER)
	return ERROR_NONE;
}

int CDNGamaniaAuth::AddSendData(char *pData, int nLen)
{
	int Ret = 0;

	if (GetDelete()) return -1;
	if (m_pIocpManager && m_pSocketContext && (m_pSocketContext->m_Socket != INVALID_SOCKET))
	{
		m_SendSync.Lock();
		Ret = m_pSendBuffer->Push( pData, nLen );						// ������
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

int CDNGamaniaAuth::SendLogin(char *pAccountName, char *pPassword, char *pIp, bool bAuthenticationFlag, bool bDuplicationCheck, UINT pSessionID)
{
	DN_ASSERT(NULL != pAccountName,	"Invalid!");
	DN_ASSERT(NULL != pPassword,	"Invalid!");
	DN_ASSERT(NULL != pIp,			"Invalid!");

	char szPacket[MAX_PATH * 2] = { '\0', };
	SNPRINTFA(szPacket, _countof(szPacket), "610401'AQ'%s'%s'%d'I'%d'%s'%u'\r\n", pAccountName, pPassword, bAuthenticationFlag, bDuplicationCheck, pIp, pSessionID);
	
	return AddSendData(szPacket, static_cast<int>(::strlen(szPacket)));
}

int CDNGamaniaAuth::SendLogout(const char *pAccountName, char *pIp)
{
	DN_ASSERT(NULL != pAccountName,	"Invalid!");
	DN_ASSERT(NULL != pIp,			"Invalid!");	

	char szPacket[MAX_PATH * 2] = { '\0', };

	SNPRINTFA(szPacket, _countof(szPacket), "610401'AQ'%s'R'''%s'0'\r\n", pAccountName, pIp);
	return AddSendData(szPacket, static_cast<int>(::strlen(szPacket)));
}

int CDNGamaniaAuth::ConvertResult(char* strOutString)
{
	if (strOutString == NULL)
		return ERROR_GENERIC_UNKNOWNERROR;
	
	int nRet = ERROR_NONE;

	if( strcmp(strOutString, "Wrong_ID_or_Password") == 0 )
		nRet = ERROR_GAMANIA_INVALID_ID_OR_PASSWORD;
	else if( strcmp(strOutString, "No_Such_ServiceAccount") == 0 )
		nRet = ERROR_GAMANIA_NO_SUCH_ACCOUNT;
	else if( strcmp(strOutString, "No_Such_Service") == 0 )
		nRet = ERROR_GAMANIA_NO_SUCH_SERVICE;
	else if( strcmp(strOutString, "No_Enough_Point") == 0 )
		nRet = ERROR_GAMANIA_NO_ENOUGH_POINT;
	else if( strcmp(strOutString, "Wrong_EventOTP") == 0 )
		nRet = ERROR_GAMANIA_WRONG_EVENTOTP;
	else if( strcmp(strOutString, "Wrong_OTP") == 0 )
		nRet = ERROR_GAMANIA_WRONG_OTP;	
	else if( strcmp(strOutString, "ServiceAccount_Locked") == 0 )
		nRet = ERROR_GAMANIA_ACCOUNT_LOCK;
	else if( strcmp(strOutString, "Service_Not_Available") == 0 )
		nRet = ERROR_GAMANIA_SERVICE_NOT_AVAILABLE;	
	else if( strcmp(strOutString, "Time_Out") == 0 )
		nRet = ERROR_GAMANIA_TIME_OUT;	
	else if( strcmp(strOutString, "Contract_Block") == 0 )
		nRet = ERROR_GAMANIA_CONTRACT_BLOCK;
	else if( strcmp(strOutString, "ServiceAccount_Already_Login") == 0 )
		nRet = ERROR_GAMANIA_ALREADY_LOGIN;
	else if( strcmp(strOutString, "ServiceAccount_Already_Waiting") == 0 )
		nRet = ERROR_GAMANIA_ALREADY_WAITING;
	else
		nRet = ERROR_GENERIC_UNKNOWNERROR;
	return nRet;
}

#endif	// #if defined(_TW)
