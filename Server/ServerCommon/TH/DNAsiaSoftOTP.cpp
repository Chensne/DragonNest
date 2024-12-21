
#include "StdAfx.h"
#include "DNAsiaSoftOTP.h"
#include "DNIocpManager.h"
#include "DNServerPacket.h"
#include "Stream.h"
#include "Util.h"
#include "StringSet.h"
#include "TimeSet.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#include "DNIocpManager.h"

#if defined(_TH) && defined(_FINAL_BUILD)

CDNAsiaSoftOTP::CDNAsiaSoftOTP(const char * pszIP, short nPort)
{
	//CONNECTIONKEY_TH_OTP
	SetIp(pszIP);
	SetPort(nPort);

	Init(2048, 2048);

	InterlockedExchange(&m_lStatus, CDNAsiaSoftOTP::Status::Free);
	_ResetValue();
}

CDNAsiaSoftOTP::~CDNAsiaSoftOTP()
{
	_ScopeReleaseLockAndDetachConnection ScopeRelease(this);
}

bool CDNAsiaSoftOTP::Connect()
{
	SetConnecting(true);
	if (g_pIocpManager->AddConnectionEx(static_cast<CConnection*>(this), CONNECTIONKEY_TH_OTP, GetIp(), GetPort()) < 0)
	{
		SetConnecting(false);
		_ScopeReleaseLockAndDetachConnection ScopeRelease(this);
		g_Log.Log(LogType::_ASIASOFTLOG, L"AsiaSoft OTP Connect Error\n");
		return false;
	}

	g_Log.Log(LogType::_ASIASOFTLOG, L"AsiaSoft OTP Connecting..\n");
	return true;
}

void CDNAsiaSoftOTP::OnConnected()
{
	if (_SendOPT(m_strDomain.c_str(), m_strMasterKey.c_str(), m_strResponse.c_str(), m_nSessionID) != ERROR_NONE)
	{
#ifdef _USE_ACCEPTEX
		ScopeLock<CSyncLock> Lock(g_pUserConnectionManager->m_ConSync);
#endif	// _USE_ACCEPTEX

		CDNUserConnection *pUserCon = pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(m_nSessionID));	
		if (!pUserCon)
		{
			g_Log.Log(LogType::_ASIASOFTLOG, L"[SID:%u] UserCon NULL\r\n", m_nSessionID);
		}
		else
		{
			pUserCon->DetachConnection(L"OTPSendError");
			g_Log.Log(LogType::_ASIASOFTLOG, pUserCon, L"AsiaSoft OTPSend Error AID[%u] SID[%u]\n", pUserCon->GetAccountDBID(), pUserCon->GetSessionID());
		}

		_ScopeReleaseLockAndDetachConnection ScopeRelease(this);
	}
}

void CDNAsiaSoftOTP::OnDisconnected()
{
	_ReleaseLock();
	g_Log.Log(LogType::_ASIASOFTLOG, L"AsiaSoft OTP Disconnected\n");
}

int CDNAsiaSoftOTP::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	_ScopeReleaseLockAndDetachConnection ScopeRelease(this);

	int nResultIndex = 0;
	std::vector <std::string> Tokens;
	TokenizeA(pData, Tokens, "|");

	//Format: “|Command|Packet Length|Status|Message|EndPoint|”
	if (AsiaAuth::OTP::OTPFormatCountMax != Tokens.size())
	{
		//님하 이러심 곤란 토큰처리하다 오버플로우 나도 모르는거임 이러면
		g_Log.Log(LogType::_ASIASOFTLOG, L"OTPFormat Count MisMatched\n");		
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	int nCommand = 0;
	int nLength = 0;
	std::string strStatus, strMessage;
	UINT nEndPoint = 0;
	for (int i = 0; i < AsiaAuth::OTP::OTPFormatCountMax; i++)
	{
		switch (nResultIndex)
		{
		case AsiaAuth::OTP::OTPCommand: nCommand = atoi(Tokens[i].c_str()); break;
		case AsiaAuth::OTP::OTPLength: nLength = atoi(Tokens[i].c_str()); break;
		case AsiaAuth::OTP::OTPStatus: strStatus = Tokens[i]; break;
		case AsiaAuth::OTP::OTPMessage: strMessage = Tokens[i]; break;
		case AsiaAuth::OTP::OTPEndPoint: nEndPoint = atoi(Tokens[i].c_str()); break;
		default:
			{
				//여긴 들어올일 없지만
				g_Log.Log(LogType::_ASIASOFTLOG, L"OTPFormat OverFlow\n");
				return ERROR_GENERIC_UNKNOWNERROR;
			}
		}
		nResultIndex++;
	}

	if (nCommand != AsiaAuth::Common::OTPRetCommond)
	{
		g_Log.Log(LogType::_ASIASOFTLOG, L"Unhandled OTP Command\n");
		return ERROR_GENERIC_UNKNOWNERROR;
	}

#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock(g_pUserConnectionManager->m_ConSync);
#endif	// _USE_ACCEPTEX

	CDNUserConnection *pUserCon = pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(m_nSessionID));	
	if (!pUserCon) {
		g_Log.Log(LogType::_ASIASOFTLOG, L"[SID:%u] UserCon NULL\r\n", m_nSessionID);
		return ERROR_NONE;
	}

	int nConvertRet = _ConvertOTPResultCode(strStatus.c_str());	
	if (nConvertRet == ERROR_NONE)
	{
		pUserCon->SendAsiaOTPResult(nConvertRet);
		pUserCon->CheckLoginTH();
	}
	else
	{
		pUserCon->SendCompleteDetachMsg(ERROR_ASIASOFT_OTP_FAILED, L"OTP Failed");
		//pUserCon->SendCheckLogin(ERROR_ASIASOFT_OTP_FAILED);
		g_Log.Log(LogType::_ASIASOFTLOG, L"[SID:%u] CDNAsiaSoftOTP Fail Description[%S]\r\n", m_nSessionID, strMessage.c_str());
	}

	return ERROR_NONE;
}

bool CDNAsiaSoftOTP::GetLock()
{
	if (InterlockedCompareExchange(&m_lStatus, CDNAsiaSoftOTP::Status::Using, CDNAsiaSoftOTP::Status::Free) == CDNAsiaSoftOTP::Status::Free)
	{
		ScopeLock <CSyncLock> Sync(m_Sync);
		m_dwSendedTime = timeGetTime();
		return true;
	}

	DWORD dwTempSendedTime = 0;
	{
		ScopeLock <CSyncLock> Sync(m_Sync);
		dwTempSendedTime = m_dwSendedTime;
	}

	if (dwTempSendedTime + AsiaAuth::Common::OTPTimeOut < timeGetTime())
	{
		_ScopeReleaseLockAndDetachConnection ScopeRelease(this);

#ifdef _USE_ACCEPTEX
		ScopeLock<CSyncLock> Lock(g_pUserConnectionManager->m_ConSync);
#endif	// _USE_ACCEPTEX

		CDNUserConnection *pUserCon = pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(m_nSessionID));	
		if (!pUserCon) {
			g_Log.Log(LogType::_ASIASOFTLOG, L"[SID:%u] UserCon NULL\r\n", m_nSessionID);
			return false;
		}

		pUserCon->SendCompleteDetachMsg(ERROR_ASIASOFT_OTP_FAILED, L"OTP Failed");
		g_Log.Log(LogType::_ASIASOFTLOG, L"[SID:%u] CDNAsiaSoftOTP Fail TimeOver\r\n", m_nSessionID);
	}
	return false;
}

void CDNAsiaSoftOTP::SetValue(const char * pDomain, const char * pMasterAkeyID, const char * pszResponse, UINT nSessionID)
{
	if (pDomain == NULL || pMasterAkeyID == NULL || pszResponse == NULL || nSessionID == 0)
		_DANGER_POINT_MSG(L"상위에서 체크하고 들어온다 근데 이상하면....");

	m_strDomain = pDomain;
	m_strMasterKey = pMasterAkeyID;
	m_strResponse = pszResponse;
	m_nSessionID = nSessionID;
}

int CDNAsiaSoftOTP::_AddSendData(char *pData, int nLen)
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

int CDNAsiaSoftOTP::_ConvertOTPResultCode(const char * pszStatus)
{
	if (!stricmp("true", pszStatus))
		return ERROR_NONE;
	return ERROR_GENERIC_UNKNOWNERROR;
}

void CDNAsiaSoftOTP::_ReleaseLock()
{
	_ResetValue();
	InterlockedExchange(&m_lStatus, CDNAsiaSoftOTP::Status::Free);
}

void CDNAsiaSoftOTP::_ResetValue()
{
	m_strDomain.clear();
	m_strMasterKey.clear();
	m_strResponse.clear();
	m_nSessionID = 0;

	ScopeLock <CSyncLock> Sync(m_Sync);
	m_dwSendedTime = 0;
}

int CDNAsiaSoftOTP::_SendOPT(const char * pDomain, const char * pMasterAkeyID, const char * pszResponse, UINT nSessionID)
{
	if (pDomain == NULL || pMasterAkeyID == NULL || pszResponse == NULL)
	{
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	// format |Command|Packet Length|Domain|UserID|Pin|DpResponse|EndPoint|
	std::string strFirst;
	//adding first
	strFirst = "|1001|";

	char szTemp[MAX_PATH];
	SNPRINTFA(szTemp, _countof(szTemp), "|%s|%s||%s|9999|", pDomain, pMasterAkeyID, pszResponse);

	int nCalcPacketSize = static_cast<int>(strFirst.size() + strlen(szTemp));

	{
		USES_CONVERSION;
		nCalcPacketSize += (int(strlen(I2A(nCalcPacketSize))));
	}

	char szOTPPackage[MAX_PATH];
	memset(szOTPPackage, 0, sizeof(szOTPPackage));
	SNPRINTFA(szOTPPackage, _countof(szTemp), "%s%d%s", strFirst.c_str(), nCalcPacketSize, szTemp);
	
	return _AddSendData(szOTPPackage, static_cast<int>(::strlen(szOTPPackage)));
}

#endif		//#ifdef _TH