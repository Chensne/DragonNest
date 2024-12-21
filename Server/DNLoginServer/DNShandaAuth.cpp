#include "StdAfx.h"
#include "DNShandaAuth.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#include "Log.h"
#include "VarArg.h"

#if defined(_CH) && defined(_FINAL_BUILD)

extern TLoginConfig g_Config;

CDNShandaAuth *g_pShandaAuth = NULL;

CDNShandaAuth::CDNShandaAuth() : m_pUserInfoAuthenObj(NULL), m_UniqueID(0)
{
	m_pUserInfoBusiness = NULL;
	m_ObjCb.GetUserInfoCallBack = NULL;
}

CDNShandaAuth::~CDNShandaAuth()
{
	Close();
}

bool CDNShandaAuth::Open()
{
	DN_ASSERT(!IsOpen(),	"Already Opened!");

	m_pUserInfoAuthenObj = sdoaCreateUserInfoAuthen();
	if (!m_pUserInfoAuthenObj) {
		DN_RETURN(false);
	}

	int nRetVal = m_pUserInfoAuthenObj->Initialize("./Config/sdoa4server.ini", AuthenCallBack);
	if (nRetVal) {
		DN_ASSERT(0, CVarArgA<MAX_PATH>("Result : %d", nRetVal));
		return false;
	}
	return SDOUAOpen();
}

void CDNShandaAuth::Close()
{
	if (m_pUserInfoAuthenObj) {
		m_pUserInfoAuthenObj->Release();
		SAFE_DELETE(m_pUserInfoAuthenObj);
	}

	m_UniqueID = 0;
	SDOUAClose();
}

bool CDNShandaAuth::SDOUAOpen()
{
	m_pUserInfoBusiness = CreateSDOAuthBusinessInstance();
	if (!m_pUserInfoBusiness) {
		DN_RETURN(false);
	}	

	int nRet = m_pUserInfoBusiness->Init("./Config/client.conf");
	// 반환값 무시 #45304 실명제 인증 관련 문제
	if (nRet)
		g_Log.Log(LogType::_ERROR, L"CDNShandaAuth::SDOUAOpen() [Result:%d]\r\n", nRet);
	/*
	if (nRet) {
		DN_ASSERT(0, CVarArgA<MAX_PATH>("Result : %d", nRet));
		return false;
	}
	*/
	m_ObjCb.GetUserInfoCallBack = GetUserInfoCallBack;
	m_pUserInfoBusiness->SetCallBack(&m_ObjCb);

	return true;
}

void CDNShandaAuth::SDOUAClose()
{
	if (m_pUserInfoBusiness) {
		SAFE_DELETE(m_pUserInfoBusiness);
	}
}

bool CDNShandaAuth::AddSdouaData(UINT pSessionID, char cIsLimited, char cIsProtected, unsigned int nRpcId)
{
	TP_LOCKAUTO LockAuto(m_SdouaLock);

	std::pair<SDOUA_LIST::const_iterator, bool> RetVal = m_SdouaList.insert(SDOUA_LIST::value_type(nRpcId, SDOUADAT(cIsLimited, cIsProtected, pSessionID)));
	if (!RetVal.second) {
		return false;
	}
	return true;
}

bool CDNShandaAuth::AtSdouaData(unsigned int nRpcId, SDOUADAT& pSdouaDat)
{
	TP_LOCKAUTO LockAuto(m_SdouaLock);

	SDOUA_LIST::const_iterator ait = m_SdouaList.find(nRpcId);
	if (m_SdouaList.end() == ait) {
		return false;
	}

	pSdouaDat = (*static_cast<const SDOUADAT*>(&ait->second));

	return true;	
}

void CDNShandaAuth::DelSdouaData(unsigned int nRpcId)
{
	TP_LOCKAUTO LockAuto(m_SdouaLock);

	m_SdouaList.erase(nRpcId);
}

void (SNDACALLBACK CDNShandaAuth::GetUserInfoCallBack)(unsigned int dwRpcId,int nResult, SKeyValue *pKeyValue, int nKeyValueNum)
{
	SDOUADAT SdouaDat;
	bool bRet = g_pShandaAuth->AtSdouaData(dwRpcId, SdouaDat);
	if (!bRet) {
		g_Log.Log(LogType::_ERROR, L"[UID:%u] AtSdouaData() failed\r\n", dwRpcId);
		return;
	}
	//바로 지우자.
	g_pShandaAuth->DelSdouaData(dwRpcId);

#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock(g_pUserConnectionManager->m_ConSync);
#endif	// _USE_ACCEPTEX

	CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(SdouaDat.m_SessionID));
	if (!pUserCon) {
		g_Log.Log(LogType::_ERROR, L"[SID:%u] UserCon NULL\r\n", SdouaDat.m_SessionID);
		return;
	}

	if( nResult == 0)
	{
		for(int i = 0; i< nKeyValueNum; i++)
		{
			SKeyValue *pTemp = &(pKeyValue[i]);
			if( i == 0 && pTemp->nKey == 13001 && pTemp->pValue[0] == '0' )
			{
				// 상세정보 입력 필요없다.
				pUserCon->CheckLoginCH(SdouaDat.m_cIsLimited, SdouaDat.m_cIsProtected);
				return;
			}
			 // URL정보가 없으면 그냥 로그인 체크 넘어가자..
			else if(i == 2 && pTemp->pValue[0] == '0')
			{
				// 상세정보 입력 필요없다.
				pUserCon->CheckLoginCH(SdouaDat.m_cIsLimited, SdouaDat.m_cIsProtected);
				return;
			}
			else if(i == 2) // 13003
			{
				// URL 값 전송..
				pUserCon->SendCheckLogin(ERROR_LOGIN_SDOUA_NEED_INFO, pTemp->pValue);
				return;
			}
		}
	}
}

int CDNShandaAuth::AsyncAuthen(LPCSTR pszSession, LPCSTR pszIPAddress, UINT pSessionID)
{
	DN_ASSERT(NULL != m_pUserInfoAuthenObj,	"Invalid!");
	DN_ASSERT(NULL != pszSession,			"Invalid!");
	DN_ASSERT(NULL != pszIPAddress,			"Invalid!");
	DN_ASSERT(0 < pSessionID,				"Invalid!");

	unsigned long ulUniqueID = AddUniqueID(pSessionID);
	if (!ulUniqueID) {
		DN_RETURN(1);
	}	

	return(m_pUserInfoAuthenObj->AsyncGetUserInfo(pszSession, pszIPAddress, ulUniqueID));
}

int (SNDACALLBACK CDNShandaAuth::AuthenCallBack)(int nResult, unsigned long ulUniqueID, ISDOAMsg *pMsg)
{
	DN_ASSERT(0 < ulUniqueID,	"Invalid!");
	DN_ASSERT(NULL != pMsg,		"Invalid!");

	UNQIDDAT UnqIdDat;
	BOOL bRetVal = g_pShandaAuth->AtUniqueID(ulUniqueID, UnqIdDat);
	if (!bRetVal) {
		g_Log.Log(LogType::_ERROR, L"[UID:%u] AtUniqueID() failed\r\n", ulUniqueID);
		return 0;
	}

	g_pShandaAuth->DelUniqueID(ulUniqueID);	// ulUniqueID 는 1회성이므로 이후로 사용될 일이 없음 ?

	UINT nSessionID = UnqIdDat.m_SessionID;
	DN_ASSERT(0 != nSessionID,	"Check!");

#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock(g_pUserConnectionManager->m_ConSync);
#endif	// _USE_ACCEPTEX

	CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(nSessionID));
	if (!pUserCon) {
		g_Log.Log(LogType::_ERROR, L"[SID:%u] UserCon NULL\r\n", nSessionID);
		return 0;
	}

	if (!nResult) {
		// 성공
		{
			USES_CONVERSION;

			pUserCon->SetAccountName(A2CW(pMsg->GetValue("PTID")));		// 아래에서 계정 DB 체크시 하므로 원래 여기는 필요 없음 ?
			//pUserCon->SetAccountName(A2CW(pMsg->GetValue("SNDAID")));	// 아래에서 계정 DB 체크시 하므로 원래 여기는 필요 없음 ?
			if (L'\0' == pUserCon->GetAccountName()[0]) {
				DN_RETURN(0);
			}
		}

		{
			DWORD dwSndaAuthFlag = 0;

			if (pMsg->GetValue("eKeyBindFlag") && (::atoi(pMsg->GetValue("eKeyBindFlag")))) {
				dwSndaAuthFlag |= eChSndaAuthFlag_UseEKey;
			}
			if (pMsg->GetValue("ecardBindFlag") && (::atoi(pMsg->GetValue("ecardBindFlag")))) {
				dwSndaAuthFlag |= eChSndaAuthFlag_UseECard;
			}

			pUserCon->SetPrmInt1(static_cast<int>(dwSndaAuthFlag));
		}

		std::string strIsLimited, strIsProtected;
		if (pMsg->GetValue("adult"))
			strIsLimited = pMsg->GetValue("adult");
		if (pMsg->GetValue("applingAdult"))
			strIsProtected = pMsg->GetValue("applingAdult");

		if( g_pShandaAuth->m_pUserInfoBusiness )
		{
			unsigned int dwRpcId=0;
			int arrKey[2];
			arrKey[0] = 13001;
			arrKey[1] = 13002;
			SKeyValue oKeyValue;
			oKeyValue.nKey = 1;			
			if (pMsg->GetValue("PTID")) oKeyValue.pValue = (char*)pMsg->GetValue("PTID");			

			g_pShandaAuth->m_pUserInfoBusiness->AsyncGetUserInfo(0, pMsg->GetValue("SNDAID"), arrKey, 2, &oKeyValue, 1, &dwRpcId);
			
			g_pShandaAuth->AddSdouaData(nSessionID, (strIsLimited.empty())?(0):(('Y' == strIsLimited.at(0))?(1):(0)),
				(strIsProtected.empty())?(0):(('Y' == strIsProtected.at(0))?(1):(0)), dwRpcId);			
		}		
	}
	else {
		// 실패
		pUserCon->SendCheckLogin(ERROR_LOGIN_ID_PASS_UNMATCH);
	}

	return 0;
}

long CDNShandaAuth::AddUniqueID(UINT pSessionID)
{
	DN_ASSERT(0 < pSessionID,	"Invalid!");

	LONG nUniqueID = InterlockedIncrement(&m_UniqueID);
	if (!nUniqueID) {
		nUniqueID = InterlockedIncrement(&m_UniqueID);
	}
	DN_ASSERT(0 < nUniqueID,	"Check!");

	TP_LOCKAUTO LockAuto(GetLock());

	std::pair<TP_LIST_CTR, bool> RetVal = m_List.insert(TP_LIST::value_type(nUniqueID, UNQIDDAT(nUniqueID, pSessionID)));
	if (!RetVal.second) {
		DN_RETURN(0);
	}

	return nUniqueID;
}

void CDNShandaAuth::DelUniqueID(long pUniqueID)
{
	DN_ASSERT(0 < pUniqueID,	"Invalid!");

	TP_LOCKAUTO LockAuto(GetLock());

	m_List.erase(pUniqueID);
}

bool CDNShandaAuth::AtUniqueID(long pUniqueID, UNQIDDAT& pUnqIdDat)
{
	DN_ASSERT(0 < pUniqueID,	"Invalid!");

	TP_LOCKAUTO LockAuto(GetLock());

	TP_LIST_CTR ait = m_List.find(pUniqueID);
	if (m_List.end() == ait) {
		return false;
	}

	pUnqIdDat = (*static_cast<const UNQIDDAT*>(&ait->second));

	return true;
}

#endif	// _CH