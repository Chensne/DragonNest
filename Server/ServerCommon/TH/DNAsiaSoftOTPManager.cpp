
#include "Stdafx.h"
#include "DNAsiaSoftOTPManager.h"
#include "DNAsiaSoftOTP.h"
#include "Log.h"
#include "Util.h"

#if defined(_TH) && defined(_FINAL_BUILD)

CDNAsiaSoftOTPManager * g_pAsiaSoftOTPManager = NULL;

CDNAsiaSoftOTPManager::CDNAsiaSoftOTPManager()
{
	memset(m_szIP, 0, sizeof(m_szIP));
	m_nPort = 0;
	m_nConnectionCount = 0;
}

CDNAsiaSoftOTPManager::~CDNAsiaSoftOTPManager()
{	
	SAFE_DELETE_PVEC(m_vAsiaSoftOTPList);
}

bool CDNAsiaSoftOTPManager::Init(const char * pszIP, int nPort, int nWorkerThreadSize)
{
	if (pszIP == NULL)
	{
		g_Log.Log(LogType::_ERROR, L"CDNAsiaSoftOTPManager Init Fail!\n");
		return false;
	}

	if (m_nConnectionCount != 0)
	{
		g_Log.Log(LogType::_ERROR, L"CDNAsiaSoftOTPManager ReInit! Check!!\n");
		return false;
	}

	_strcpy(m_szIP, IPLENMAX, pszIP, IPLENMAX);
	m_nPort = nPort;

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	m_nConnectionCount = nWorkerThreadSize == 0 ? (SysInfo.dwNumberOfProcessors * 2) + 1 : nWorkerThreadSize;

	for (int i = 0; i < m_nConnectionCount; i++)
	{
		CDNAsiaSoftOTP * pOTP = new CDNAsiaSoftOTP(m_szIP, m_nPort);
		m_vAsiaSoftOTPList.push_back(pOTP);
	}

	return true;
}

bool CDNAsiaSoftOTPManager::SendOTP(const char * pDomain, const char * pMasterAkeyID, const char * pszResponse, UINT nSessionID)
{
	if (pDomain == NULL || pMasterAkeyID == NULL || pszResponse == NULL || nSessionID == 0)
		return false;

	CDNAsiaSoftOTP * pOTP = _GetFreeOTP();
	if (pOTP)
	{
		if (pOTP->Connect())
		{
			pOTP->SetValue(pDomain, pMasterAkeyID, pszResponse, nSessionID);
			return true;
		}
	}
	return false;
}

CDNAsiaSoftOTP * CDNAsiaSoftOTPManager::_GetFreeOTP()
{
	ULONG nStartTick = timeGetTime();

	CDNAsiaSoftOTP * pOTP = NULL;
	while (pOTP == NULL)
	{
		for (int i = 0; i < m_nConnectionCount; i++)
		{
			if (m_vAsiaSoftOTPList[i]->GetLock())
				return m_vAsiaSoftOTPList[i];
		}

		if (nStartTick + AsiaAuth::Common::OTPTimeOut < timeGetTime())
			break;
	}
	return NULL;
}

#endif		//#ifdef _TH