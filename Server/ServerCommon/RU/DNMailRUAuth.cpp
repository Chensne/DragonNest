#include "StdAfx.h"

#if defined(_RU) && defined(_FINAL_BUILD)

#include "DNMailRUAuth.h"
#include "HttpClientManager.h"
#include "Util.h"
#include "Log.h"
#include "XMLParser.h"

int MailRUAuthLogin(char const *AccountID, char const *pOTPHash, char const *ip)
{
	CHttpClient* pHttpClient = g_pHttpClientManager->FindHttpClient();
	if (!pHttpClient) {
		return ERROR_MAILRU_ERROR;
	}	
	wchar_t szTemp[128] = {0,};

	SNPRINTFW(szTemp, _countof(szTemp), L"/Client.php?uid=%S&hash=%S&ip=%S", AccountID, pOTPHash, ip);	

	CHttpClientAuto Auto(pHttpClient, g_pHttpClientManager);	

	int nStatus = pHttpClient->SendRequestGet(szTemp);
	if( nStatus != 200) // 200이 아니면 다 에러?
	{
		g_Log.Log(LogType::_ERROR, L"MailRU Auth GetStatus!(%d), ID:%S\r\n", nStatus, AccountID);
		return ERROR_MAILRU_ERROR;
	}
	BYTE byBuff[1024]={0,};
	if( pHttpClient->RecvResponse((LPBYTE)byBuff, 1024) == 0)
	{
		return ParseErrorCode((char*)byBuff);
	}
	return ERROR_MAILRU_ERROR;	
}

int ParseErrorCode(char* pResult)
{ 
	//Xml 파싱..
	CXMLParser parser;
	if (parser.OpenByBuffer(pResult) == false)
	{
		g_Log.Log(LogType::_ERROR, L"MailRU Auth XML Parser Error!(%S)\r\n", pResult);
		return ERROR_MAILRU_INVALID_REQUEST;
	}
	if (parser.FirstChildElement("gasreply", true) == true)
	{
		if (parser.FirstChildElement("status", false) == true)
		{
			if( wcscmp(parser.GetText(), L"gas_ok") == 0)
			{
				return ERROR_NONE;
			}
			else if( wcscmp( parser.GetText(), L"gas_auth_failed") == 0)
			{
				// ID, Pass 틀림
				return ERROR_MAILRU_WORNG_PASSWORD;
			}
			else if( wcscmp( parser.GetText(), L"gas_error") == 0)
			{				
				// 에러이면 에러코드 및 에러메시지 받기.
				if( parser.FirstChildElement("info", true) == true)
				{
					const WCHAR* pErrorCode = NULL;
					const WCHAR* pErrorMSG  = NULL;
					if( parser.FirstChildElement("errorcode", false) == true )
					{
						pErrorCode = parser.GetText();
					}
					if( parser.FirstChildElement("errormessage", false) == true)
					{
						pErrorMSG = parser.GetText();
					}
					if( pErrorCode && pErrorMSG )
						g_Log.Log(LogType::_ERROR, L"MailRU Auth Error Code:%s, Message:%s\r\n", pErrorCode, pErrorMSG);
					return ERROR_MAILRU_ERROR;
				}
			}
			else if( wcscmp(parser.GetText(), L"gas_user_banned") == 0)
			{
				// 에러이면 에러코드 및 에러메시지 받기.
				if( parser.FirstChildElement("info", true) == true)
				{
					// bants에 밴이 완료된 시간이 있지만 그냥 pass					
					if( parser.FirstChildElement("banreason", false) == true)
					{
						g_Log.Log(LogType::_ERROR, L"MailRU Auth Banned User Message:%s\r\n", parser.GetText());						
					}
					return ERROR_MAILRU_BLOCK;
				}
			}		
		}
	}
	return ERROR_MAILRU_INVALID_REQUEST;
}

#endif //if defined(_RU)