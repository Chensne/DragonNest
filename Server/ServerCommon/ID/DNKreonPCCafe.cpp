#include "StdAfx.h"
#include "DNKreonPCCafe.h"
#include "DNDivisionManager.h"
#include "DNUser.h"
#include "Log.h"

#if defined(_ID) && defined(_FINAL_BUILD)
CDNKreonPCCafe *g_pKreonPCCafe = NULL;

CDNKreonPCCafe::CDNKreonPCCafe()
{
	m_bThreadSwitch = true;
	m_HttpClient.Open(KreonPcCafe::PCCafeServerAddress, CHttpClient::EV_HTTP_CONNECTION_KEEPALIVE);
	CreateThread();
}
CDNKreonPCCafe::~CDNKreonPCCafe()
{
	m_bThreadSwitch = false;
	Final();
}

void CDNKreonPCCafe::Final()
{
	while (!m_ProcessCalls.empty()){
		m_ProcessCalls.pop();
	}

	if( m_hProcessThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hProcessThread, INFINITE );
		CloseHandle( m_hProcessThread );
	}	
}

int CDNKreonPCCafe::CreateThread()
{
	UINT ThreadID;

	m_hProcessThread = (HANDLE)_beginthreadex(NULL, 0, &ProcessThread, this, 0, &ThreadID);
	if (m_hProcessThread == INVALID_HANDLE_VALUE) return -1;	
	return 0;
}

void CDNKreonPCCafe::AddProcessCall(UINT uiAccountDBID)
{
	m_ProcessLock.Lock();
	m_ProcessCalls.push(uiAccountDBID);	
	m_ProcessLock.UnLock();
}

UINT __stdcall CDNKreonPCCafe::ProcessThread(void *pParam)
{
	CDNKreonPCCafe *pKreonPCCafe = (CDNKreonPCCafe*)pParam;	

	UINT uiAccountDBID = 0;
	while(pKreonPCCafe->m_bThreadSwitch)
	{
		if (!pKreonPCCafe->m_ProcessCalls.empty())
		{
			pKreonPCCafe->m_ProcessLock.Lock();
			uiAccountDBID = pKreonPCCafe->m_ProcessCalls.front();
			pKreonPCCafe->m_ProcessCalls.pop();
			pKreonPCCafe->m_ProcessLock.UnLock();

			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(uiAccountDBID);
			if( pUser )
			{
				if( strlen(pUser->m_szMacAddress) == 0)
				{
					g_Log.Log(LogType::_ERROR, pUser, L"Kreon PcCafe NULL MacAddress\r\n");
					continue;
				}
				int nPcCafeLevel = pKreonPCCafe->SendCheckIPBonus(pUser->m_szMacAddress, pUser->GetIp(), pUser->m_szKey, pUser->m_dwKreonCN);
				if( nPcCafeLevel > PCBang::Grade::Normal && nPcCafeLevel <= PCBang::Grade::Gold )
				{
					pUser->SetPCBang(true);
					pUser->SetPCGrade(nPcCafeLevel);
					g_pDivisionManager->SendPCBangResult(pUser);
				}
				else if( nPcCafeLevel == ERROR_KREON_BLOCK_PCCAFE ) // 블럭된 PC방이면.
				{
					pUser->m_bBlockPcCafe = true;
					g_pDivisionManager->SendPCBangResult(pUser);
				}
			}
		}
		Sleep(1);		
	}
	return 0;
}

int CDNKreonPCCafe::SendCheckIPBonus(const char* strMacAddress, const char* ip, const char* key, const DWORD dwKreonCN)
{		
	char szPostData[256]={0,};
	SNPRINTFA(szPostData, _countof(szPostData),"mac=%s&ip=%s&svcCd=%s&key=%s&cn=%u",
		strMacAddress, ip, KreonPcCafe::ServiceCode, key, dwKreonCN);

	int nStatus = m_HttpClient.SendRequsetPostA(KreonPcCafe::PCCafeUrl, szPostData);
	if( nStatus != 200) // 200이 아니면 다 에러?
	{
		g_Log.Log(LogType::_ERROR, L"Kreon PcCafe GetStatus!(%d)\r\n", nStatus);
		return ERROR_KREON_INVALID_REQUEST;
	}
	BYTE byBuff[256]={0,};
	if( m_HttpClient.RecvResponse((LPBYTE)byBuff, 256) == 0)
	{
		return ParseErrorCode((char*)byBuff);
	}
	return ERROR_KREON_INVALID_REQUEST;
}

int CDNKreonPCCafe::ParseErrorCode(char* pResult)
{
	int nResultIndex = 0;
	char* strResult = NULL;
	char* strMessage = NULL;	
	char* token = strtok(pResult,"	");
	while(token != NULL)
	{
		switch(nResultIndex)
		{
		case 0 : strResult = token; break;
		case 1 : strMessage = token; break;		
		}
		token = strtok(NULL,"	");
		++nResultIndex;
	}
	int nRet = ERROR_NONE;
	if( strcmp(strResult, "Normal") == 0 )
		nRet = PCBang::Grade::None;
	else if( strcmp(strResult, "Silver") == 0 )
		nRet = PCBang::Grade::Red;
	else if( strcmp(strResult, "Gold") == 0 )
		nRet = PCBang::Grade::Gold;	
	else if( strcmp(strResult, "Blocked") == 0 )
		nRet = ERROR_KREON_BLOCK_PCCAFE;
	else
		nRet = PCBang::Grade::None;	
	return nRet;
}

#endif //if defined(_ID)