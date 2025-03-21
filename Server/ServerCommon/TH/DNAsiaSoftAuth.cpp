#include "StdAfx.h"
#include "DNAsiaSoftAuth.h"
#include "DNIocpManager.h"
#include "Log.h"
#include "DNServerPacket.h"
#include "Stream.h"
#include "Util.h"
#include "StringSet.h"
#include "TimeSet.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"

#if defined(_TH) && defined(_FINAL_BUILD)

CDNAsiaSoftAuth * g_pAsiaSoftAuth = NULL;

CDNAsiaSoftAuth::CDNAsiaSoftAuth(const char * pszIP, int nPort): CConnection()
{
	SetIp(pszIP);
	SetPort(nPort);

	Init(1024 * 200, 1024 * 200);

 	m_dwReconnectTick = 0;
}

CDNAsiaSoftAuth::~CDNAsiaSoftAuth(void)
{
}

#if !defined( _FINAL_BUILD )
volatile static long g_nSum = 0;
volatile static long g_nCount = 0;
volatile static long g_nFalseCount = 0;
volatile static long g_nStartTick = 0;
volatile static long g_nLastProcess = 0;
#endif		//#if !defined( _FINAL_BUILD )

void CDNAsiaSoftAuth::Reconnect(DWORD CurTick)
{
	if (m_dwReconnectTick + 5000 < CurTick){
		m_dwReconnectTick = CurTick;

		if (!GetActive() && !GetConnecting() )
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_TH_AUTH, m_szIP, m_wPort) < 0) 
			{
				SetConnecting(false);
				g_Log.Log(LogType::_FILELOG, L"AuthServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else
			{
				g_Log.Log(LogType::_FILELOG, L"AuthServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);				
			}
		}

#if !defined( _FINAL_BUILD )
		if (m_PerfTestIDs.empty() == false)
		{
			g_Log.Log(LogType::_FILELOG, L"AsiaSoft Auth Perf Test Elapsed[%d] Now[%d] Avr[%d] QueryCnt[%d] Cnt[%d] FalseCnt[%d] fullCnt[%d]\n", g_nLastProcess - g_nStartTick, timeGetTime(), g_nCount == 0 ? 0 : g_nSum / g_nCount, (int)m_PerfTestIDs.size(), g_nCount, g_nFalseCount, g_nCount + g_nFalseCount);
		}
#endif		//#if !defined( _FINAL_BUILD )
	}
}

int CDNAsiaSoftAuth::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	int nResultIndex = 0;
	AsiaAuth::TAsiaSoftAuthInfo AuthInfo;
	std::vector <std::string> Tokens;
	_Tokenize(pData, Tokens, "|;", nLen);

	for (int i = 0; i < (int)Tokens.size(); i++)
	{
		switch (nResultIndex)
		{
		case AsiaAuth::Auth::AuthReturnCode: AuthInfo.nAuthReturnCode = atoi(Tokens[i].c_str()); break;
		case AsiaAuth::Auth::AuthDescription: AuthInfo.strDescription = Tokens[i]; break;
		case AsiaAuth::Auth::AuthMaster_ID: AuthInfo.strMasterID = Tokens[i]; break;
		case AsiaAuth::Auth::AuthAkeyFlag: AuthInfo.bAkeyOTPFlag = atoi(Tokens[i].c_str()) > 0 ? true : false; break;
		case AsiaAuth::Auth::AuthMaster_Akey_ID: AuthInfo.strAkeyMasterID = Tokens[i]; break;
		case AsiaAuth::Auth::AuthSessionID:	AuthInfo.nSessionID = static_cast<UINT>(strtoul(Tokens[i].c_str(), NULL, 10)); break;
		default:
			{
				//여긴 들어올일 없지만
				_DANGER_POINT_MSG(L"AuthFormat Count OverFlow");
				return ERROR_GENERIC_UNKNOWNERROR;
			}
		}
		nResultIndex++;
	}

#if !defined( _FINAL_BUILD )
	if (m_PerfTestIDs.empty() == false)
	{
		g_nLastProcess = timeGetTime();
		std::vector <CDNAsiaSoftAuth::_PERFTEST>::iterator ii;
		for (ii = m_PerfTestIDs.begin(); ii != m_PerfTestIDs.end(); ii++)
		{
			if ((*ii).nSessionID == AuthInfo.nSessionID)
			{
				DWORD dwGapTime = g_nLastProcess - (*ii).dwQueryTime;
				long nTempSum = InterlockedExchangeAdd(&g_nSum, dwGapTime);
				long nTempCount = InterlockedExchangeAdd(&g_nCount, 1);
				//g_Log.Log(LogType::_FILELOG, L"AsiaSoft Auth Perf Test SessionID[%d] Desc[%S] Gap[%d] Avr[%d] Cnt[%d]\n", AuthInfo.nSessionID, AuthInfo.strDescription.c_str(), dwGapTime, nTempCount == 0 ? 0 : nTempSum / nTempCount, nTempCount);
				
				return ERROR_NONE;
			}
		}

		int nConvertedResult = ERROR_GENERIC_UNKNOWNERROR;
		nConvertedResult = _ConvertAuthResultCode(AuthInfo.nAuthReturnCode);
		
		long nTempFalseCount = InterlockedExchangeAdd(&g_nFalseCount, 1);
		//g_Log.Log(LogType::_FILELOG, L"AsiaSoft Auth Perf Test Desc[%S] Cnt[%d]\n", AuthInfo.strDescription.c_str(), nTempFalseCount);
		return ERROR_NONE;
	}
#endif		//#if !defined( _FINAL_BUILD )


#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock(g_pUserConnectionManager->m_ConSync);
#endif	// _USE_ACCEPTEX

	CDNUserConnection *pUserCon = pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(AuthInfo.nSessionID));	
	if (!pUserCon) {
		g_Log.Log(LogType::_ERROR, L"[SID:%u] UserCon NULL\r\n", AuthInfo.nSessionID);
		return ERROR_NONE;
	}

	int nConvertedResult = ERROR_GENERIC_UNKNOWNERROR;
	nConvertedResult = _ConvertAuthResultCode(AuthInfo.nAuthReturnCode);
	if (nConvertedResult == ERROR_NONE) // 성공
	{
		//와우! 정상 그럼 일단 세션에 데이타를 넣어두자 값들이 인증이후에도 필요해지면 해당 구조체를 마스터로 올리자
		if (pUserCon->SetAsiaAuthInfo(AuthInfo) == false)
		{
			//이럼 안데는데 일단은 로그만 박아보자 플로우에 영향을 주진 않는다.
			_DANGER_POINT_MSG(L"SetAsiaAuthInfo(&AuthInfo) == false");
		}

		{
			USES_CONVERSION;
			std::string strAccountName;
			strAccountName.append(AuthInfo.strDomain);
			strAccountName.append(".");
			strAccountName.append(AuthInfo.strMasterID);

			pUserCon->SetAccountName(A2CW(strAccountName.c_str()));
		}

		//인증성공 OTP를 거쳐야하는 유저인지? 확인~
		if (AuthInfo.bAkeyOTPFlag)
		{
			pUserCon->SendAsiaAuthRequestOTP();
		}
		else
		{
			pUserCon->CheckLoginTH();
		}
	}
	else // 실패
	{
		pUserCon->SendCheckLogin(nConvertedResult);
		g_Log.Log(LogType::_ERROR, L"[SID:%u] CDNAsiaSoftAuth Fail Master_ID[%S] Description[%S] ReturnCode:%d \r\n", AuthInfo.nSessionID, AuthInfo.strMasterID.c_str(), AuthInfo.strDescription.c_str(), nConvertedResult);
	}

	return ERROR_NONE;
}

int CDNAsiaSoftAuth::AddSendData(char *pData, int nLen)
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

int CDNAsiaSoftAuth::SendLogin(const char *pAccountName, const char *pPassword, const char* pDomain, const char *pIp, UINT nSessionID)
{
	if (GetActive() == false) return -1;
	if (pAccountName == NULL || pPassword == NULL || pDomain == NULL || pIp == NULL)
		return -1;

	std::string strMd5Pass;
	MD5Hash(pPassword, (int)strlen(pPassword), strMd5Pass);

	ToLowerA(strMd5Pass);

	char szPacket[MAX_PATH * 2] = { '\0', };
	SNPRINTFA(szPacket, _countof(szPacket), "%s|%s|%s|%s|%u;", pAccountName, strMd5Pass.c_str(), pDomain, pIp, nSessionID);
	//g_Log.Log(LogType::_NORMAL, L"AsiaSoft Auth [%S]\n", szPacket);
	
	return AddSendData(szPacket, static_cast<int>(::strlen(szPacket)));
}

#if !defined( _FINAL_BUILD )
void CDNAsiaSoftAuth::AsiaAuthPerfTest()
{
	g_nSum = 0;
	g_nCount = 0;
	g_nFalseCount = 0;
	g_nStartTick = 0;
	g_nLastProcess = 0;

	if (m_PerfTestIDs.empty())
	{
		for (int i = 1; i <= 1000; i++)
		{
			CDNAsiaSoftAuth::_PERFTEST PerfTest;
			char szTemp[IDLENMAX];
			memset(szTemp, 0, sizeof(szTemp));

			sprintf(szTemp, "DNEYE%04d", i);
			PerfTest.strID = szTemp;
			PerfTest.nSessionID = i;

			m_PerfTestIDs.push_back(PerfTest);
		}
	}

	g_nStartTick = timeGetTime();
	std::vector <CDNAsiaSoftAuth::_PERFTEST>::iterator ii;
	for (ii = m_PerfTestIDs.begin(); ii != m_PerfTestIDs.end(); ii++)
	{
		SendLogin((*ii).strID.c_str(), "11111111", "THAS", "10.0.3.22", (*ii).nSessionID);
		(*ii).dwQueryTime = timeGetTime();
		//Sleep(10);
	}
}
#endif		//#if !defined( _FINAL_BUILD )

int CDNAsiaSoftAuth::_ConvertAuthResultCode(int nRetCode)
{
	switch (nRetCode)
	{
	case 0: return ERROR_NONE;
	case -1001: return ERROR_ASIASOFT_INVALID_REQUEST;
	case -1002: return ERROR_ASIASOFT_INVALID_USERID;
	case -1003: return ERROR_ASIASOFT_USER_ID_NOT_FOUND;
	case -1004: return ERROR_ASIASOFT_WORNG_PASSWORD;
	case -1005: return ERROR_ASIASOFT_DISABLE_USER;
	case -9009: return ERROR_ASIASOFT_DATABASE_ERROR;
	case -9010: return ERROR_ASIASOFT_UNEXPECTED_ERROR;
	default:
		{
			g_Log.Log(LogType::_ERROR, L"Unhandled AsiaSoft Return Message [%d]\n", nRetCode);
			break;
		}
	}
	return ERROR_GENERIC_UNKNOWNERROR;
}

void CDNAsiaSoftAuth::_Tokenize(const char * pstr, std::vector<std::string>& tokens, const std::string& delimiters, int nSrcsize)
{
	int nFirst = 0;
	std::string strToken, strData;
	strData = pstr;

	for (int i = 0; pstr[i] != NULL && pstr[i] != '\0' && i < nSrcsize; i++)
	{
		if (_IsDelimiters(&pstr[i], delimiters))
		{
			if (nFirst == 0)
			{
				nFirst = ++i;
				if (tokens.size() == 0)
					tokens.push_back(strData.substr(0, nFirst - 1));
				continue;
			}

			if (nFirst != 0)
			{
				tokens.push_back(strData.substr(nFirst, i - nFirst));

				if (_IsDelimiters(&pstr[++i], delimiters))
				{
					strToken = "";
					tokens.push_back(strToken);
					nFirst = ++i;
				}

				nFirst = i;
			}
		}

		if (pstr[i+1] == '\0')
		{
			tokens.push_back(strData.substr(nFirst, (i+1) - nFirst));
			break;
		}
	}
}

bool CDNAsiaSoftAuth::_IsDelimiters(const char * pstr, const std::string& delimiters)
{
	char token = *pstr;
	std::string::const_iterator ii;
	for (ii = delimiters.begin(); ii != delimiters.end(); ii++)
	{
		if (token == (*ii))
			return true;
	}
	return false;
}


#endif	// #if defined(_TH)
