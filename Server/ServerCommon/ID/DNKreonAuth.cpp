#include "StdAfx.h"

#if defined(_ID) && defined(_FINAL_BUILD)

#include "DNKreonAuth.h"
#include "HttpClientManager.h"
#include "Util.h"
#include "Log.h"

int KreonAuthLogin(char const *AccountID, char const *pPassWord, char const *ip, DWORD& KreonCN)
{
	CHttpClient* pHttpClient = g_pHttpClientManager->FindHttpClient();
	if (!pHttpClient) {
		return ERROR_KREON_INVALID_REQUEST;
	}	
	char szTemp[128] = {0,};
	char szToday[32] = {0,};

	tm pToday;
	time_t raw;
	time(&raw);
	localtime_s(&pToday, &raw);
	strftime(szToday, 256, "%Y%m%d%H%M", &pToday);	

	SNPRINTFA(szTemp, _countof(szTemp), "%s%s%s%s%s", KreonAuth::ServiceCode, AccountID, ip, pPassWord, szToday);
	std::string strKey;
	MD5Hash(szTemp, (int)strlen(szTemp), strKey);

	CHttpClientAuto Auto(pHttpClient, g_pHttpClientManager);
	char szPostData[256]={0,};
	SNPRINTFA(szPostData, _countof(szPostData),"id=%s&ip=%s&key=%s&password=%s&reqDate=%s&svcCd=%s",
		AccountID, ip, strKey.c_str(), pPassWord, szToday, KreonAuth::ServiceCode);	

	int nStatus = pHttpClient->SendRequsetPostA(KreonAuth::AuthUrl, szPostData);
	if( nStatus != 200) // 200�� �ƴϸ� �� ����?
	{
		g_Log.Log(LogType::_ERROR, L"Kreon Auth GetStatus!(%d)\r\n", nStatus);
		return ERROR_KREON_INVALID_REQUEST;
	}
	BYTE byBuff[1024]={0,};
	if( pHttpClient->RecvResponse((LPBYTE)byBuff, 1024) == 0)
	{
		return ParseErrorCode((char*)byBuff, KreonCN);
	}
	return ERROR_KREON_INVALID_REQUEST;	
}

int ParseErrorCode(char* pResult, DWORD& KreonCN)
{
	int nResultIndex = 0;
	char* strResult = NULL;
	char* strMessage = NULL;	
	char* strCN = NULL;
	char* token = strtok(pResult,"	");
	while(token != NULL)
	{
		switch(nResultIndex)
		{
		case 0 : strResult = token; break;
		case 1 : strMessage = token; break;		
		case 2 : strCN = token; break;
		}
		token = strtok(NULL,"	");
		++nResultIndex;
	}
	int nRet = ERROR_NONE;
	if( strcmp(strResult, "S000") == 0 )
	{
		nRet = ERROR_NONE;
		KreonCN = strtoul(strCN, NULL, 10);
	}
	else if( strcmp(strResult, "E202") == 0 )
		nRet = ERROR_KREON_INVALID_USERID;
	else if( strcmp(strResult, "E203") == 0 )
		nRet = ERROR_KREON_WRONG_PASSWORD;
	else if( strcmp(strResult, "E204") == 0 )
		nRet = ERROR_KREON_BLOCK_GEMSCOOL;
	else if( strcmp(strResult, "E205") == 0 )
		nRet = ERROR_KREON_BLOCK_DN;
	else if( strcmp(strResult, "E206") == 0 )
		nRet = ERROR_KREON_ACCOUNT_LOCKED;
	else if( strcmp(strResult, "E901") == 0 )
		nRet = ERROR_KREON_INTERNAL_ERR;
	else if( strcmp(strResult, "E902") == 0 )
		nRet = ERROR_KREON_INTERNAL_DB_ERR;
	else
		nRet = ERROR_KREON_INVALID_REQUEST;	
	if( nRet == ERROR_KREON_INVALID_REQUEST && strMessage)
		g_Log.Log(LogType::_ERROR, L"Kreon Auth Fail!!(%S)\r\n", strMessage);
	return nRet;
}

#endif //if defined(_ID)