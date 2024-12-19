#include "StdAfx.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#include "DNMasterConnection.h"
#include "DNMasterConnectionManager.h"
#include "DNSQLMembershipManager.h"
#include "DNSQLWorldManager.h"
#include "DNIocpManager.h"
#include "DNExtManager.h"
#include "Log.h"
#include "DNLogConnection.h"
#include "DNSecure.h"
#include "DNAuthManager.h"
#include "DNCountryUnicodeSet.h"
#include "TimeSet.h"
#include "Version.h"
#if defined( PRE_ADD_DWC )
#include "DNDWCChannelManager.h"
#endif // #if defined( PRE_ADD_DWC )

#if defined(_KR)
#include "AuthCheckWrapper.h"
#elif defined(_KRAZ)
#include "DNActozAuth.h"
#include "DNSQLActozCommonManager.h"
#elif defined(_CH)
#include "DNShandaAuth.h"
#elif defined(_TW)
#include "DNGamaniaAuth.h"
#elif defined(_US)
#include "DNNexonPI.h"
#elif defined(_SG)
#include "../ServerCommon/sg/Soap/DNCherryAuth.h"
#elif defined(_TH)
#include "DNAsiaSoftAuth.h"
#include "DNAsiaSoftOTPManager.h"
#elif defined(_ID)
#include "DNKreonAuth.h"
#elif defined(_RU)
#include "DNMailRUAuth.h"
#elif defined(_EU)
#include "DNEUAuth.h"
#endif	// _KR, _CH, _TW, _US, _SG, _TH

#if defined(_HSHIELD)
#include "HShieldSvrWrapper.h"
#endif	// #if defined(_HSHIELD)

extern TLoginConfig g_Config;

CDNUserConnection::CDNUserConnection(void)
{
	Clear();

#if defined(_HSHIELD)
	m_bUseHshield = true;

	DWORD dwRetVal = NOERROR;
	m_hHSClient = HShieldSvrWrapper::AhnHS_CreateClientObject(g_Config.hHSServer, &dwRetVal);
	if (NOERROR != dwRetVal) {
		DetachConnection(L"_AhnHS_CreateClientObject Failed [Exception]\r\n");
		return;
	}
	if (m_hHSClient == ANTICPX_INVALID_HANDLE_VALUE){
		DetachConnection(L"_AhnHS_CreateClientObject Failed\r\n");
		return;
	}

#elif defined(_GPK)
	m_dwCheckGPKTick = 0;
	if (g_Config.pDynCode)
	{
		m_nCodeIndex = g_Config.pDynCode->GetRandIdx();
		m_nCodeLen = g_Config.pDynCode->GetCltDynCode(m_nCodeIndex, &m_pCode);
		if (m_pCode == NULL || m_nCodeLen < 0 || m_nCodeLen > GPKCODELENMAX ) 
		{
			g_Log.Log(LogType::_FILELOG, L"GetCltDynCode Fail Len[%d]\n", m_nCodeLen);
			m_nCodeLen = 0;
		}
	}
	else
	{
		m_nCodeIndex = m_nCodeLen = -1;
	}

#endif	// _HSHIELD

	InitWrap(1024 * 100, 1024 * 100);
}

CDNUserConnection::~CDNUserConnection(void)
{
#if defined(_HSHIELD)
	DWORD dwRetVal = NOERROR;
	HShieldSvrWrapper::AhnHS_CloseClientHandle(m_hHSClient, &dwRetVal);
	m_hHSClient = ANTICPX_INVALID_HANDLE_VALUE;
#endif	// _HSHIELD

	SendDelUser();
}

int CDNUserConnection::InitWrap(int nRecvLen, int nSendLen, int nCompLen)
{
	int iRetVal = 0;

	__try {	// 예외 필터 시작
		iRetVal = Init(nRecvLen, nSendLen, nCompLen);
	}
	__except(CExceptionCodeLog(GetExceptionCode(), L"CDNUserConnection::InitWrap()"), EXCEPTION_CONTINUE_SEARCH) {	// 예외 필터 종료	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
		// 이 코드 블럭은 절대 수행되지 않음
	}

	return iRetVal;
}

void CDNUserConnection::Clear()
{
	m_cGender = m_cDailyCreateCount = 0;

#if defined(PRE_MOD_SELECT_CHAR)
	m_nCreateCharacterSlotMaxPerAccount = g_pExtManager->GetGlobalWeightValue(Login_Max_CreateCharCount);
	m_nDefaultCreateCharacterSlotPerAccount = g_pExtManager->GetGlobalWeightValue(Login_Base_CreateCharCount);

	m_cAvailableCharacterSlotCount = m_nDefaultCreateCharacterSlotPerAccount;	// 처음엔 기본세팅값으로

	memset(&m_SelectCharacterData, 0, sizeof(m_SelectCharacterData));

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	m_cSelectCharIndex = -1;
	m_cCharCountMax = CreateCharacterDefaultCountMax;

	m_cCharacterLimitCount = CreateCharacterDefaultCountMax + g_pExtManager->GetGlobalWeightValue(CharacterSlotMax);	// 34: 캐릭터 슬롯 개수제한
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	
	m_cVillageID = 1;
	m_nChannelID = 0;
	m_cUserState = STATE_NONE;
	m_cAccountLevel = 0;
	m_nPrmInt1 = 0;
	m_cLastServerType = 0;

	m_cRegion = 0;

	m_nTutorialMapIndex = 0;
	m_cTutorialGateNo = 0;

#if defined(PRE_MOD_SELECT_CHAR)
	m_MapCharacterList.clear();
	m_MapWorldCharCount.clear();
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	memset(&m_CharDataArray, 0, sizeof(m_CharDataArray));
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	m_ChannelList.clear();
	
	m_nNexonSN = m_nNexonUserNo = 0;
#if defined(_KRAZ)
	memset(&m_ShutdownData, 0, sizeof(m_ShutdownData));
#endif	// #if defined(_KRAZ)

	m_nAccountDBID = 0;
	memset(&m_wszAccountName, 0, sizeof(m_wszAccountName));	// 계정
	m_cWorldID = m_cWaitWorldID = 0;
	m_nWorldSetID = m_nWaitWorldSetID = 0;
	m_bAdult = false;
	m_cAge = 0;

	memset(&m_wszVirtualIp, 0, sizeof(m_wszVirtualIp));
	memset(&m_szVirtualIp, 0, sizeof(m_szVirtualIp));

	m_bPCBang = false;
	m_cPCBangGrade = 0;

	m_bIsSetSecondAuthPW	= false;
	m_bIsSetSecondAuthLock	= false;
	m_tSecondAuthResetDate	= -1;
#ifdef PRE_ADD_23829
	m_bPassSecondAuth = false;
#endif	//#ifdef PRE_ADD_23829
#ifdef PRE_ADD_IGNORESECONDAUTH_EKEYECARD
	m_bCompletlyPassSecondAuth = false;
#endif

#if defined(_HSHIELD)
	m_dwCheckLiveTick = m_dwHShieldResponseTick = timeGetTime();
#endif	// _HSHIELD

	m_PartnerType = Partner::Type::None;

	m_dwLastMessageTick = timeGetTime();
	m_cLastMainCmd = 0;
	m_cLastSubCmd = 0;
	m_dwSendInsideDisconnect = 0;
	m_cWaitWorldID = m_cWorldID = 0;

	// Auth
	m_bReconnected = false;
	m_bCertified = false;
	m_biCertifyingKey = 0;
#if !defined(PRE_MOD_SELECT_CHAR)
	m_bMasterAddUser = false;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	memset(m_szMID, 0, sizeof(m_szMID));
	m_dwGRC = 0;
#if defined(PRE_ADD_LOGIN_USERCOUNT)
	m_bSummitUserCount = false;
	m_dwKickTick = timeGetTime();
#endif

#if defined(PRE_ADD_SENDLOGOUT)
	m_bSendLogOut = false;
#endif

#ifdef PRE_ADD_MULTILANGUAGE
	m_eSelectedLanguage = MultiLanguage::eDefaultLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_COMEBACK
	m_bComebackUser = false;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	m_bReConnectNewbieReward = false;
	m_eUserGameQuitReward = GameQuitReward::RewardType::None;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(_ID)
	memset(m_szMacAddress, 0, sizeof(m_szMacAddress));
	memset(m_szKey, 0, sizeof(m_szKey));	
	m_dwKreonCN = 0;
#endif
#ifdef PRE_ADD_DOORS
	m_bMobileAuthentication = false;
#endif		//#ifdef PRE_ADD_DOORS
	m_cCharacterSortCode = Login::CharacterSortCode::None;

#ifdef PRE_ADD_STEAM_USERCOUNT
	m_bSteamUser = false;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
#if defined( PRE_LOGINDUMP_LOG )
	memset(&m_ReconnectData, 0, sizeof(m_ReconnectData));
	m_nReconnectLoginStep = 0;
#endif
}

void CDNUserConnection::DoUpdate(DWORD CurTick)
{
#if defined(PRE_ADD_LOGIN_USERCOUNT)
	// 인증받지 못한 애들은 5분이면 튕김.
	if( !IsCertified() && m_dwKickTick + CHECKNOTAUTHLOGINUSERKICKTICK <= CurTick)
	{
		DetachConnection(L"LOGIN Not Auth UserKick");
		return;
	}
	if( m_dwKickTick + CHECKLOGINUSERKICKTICK <= CurTick )
	{
		if (m_cUserState != STATE_WAITUSER)
			DetachConnection(L"LOGIN UserKick");
		return;
	}
#endif
#if defined(_HSHIELD)
	if (m_bUseHshield){
		if (m_dwCheckLiveTick + CHECKLIVETICK <= CurTick){
			SendMakeRequest();
			m_dwCheckLiveTick = CurTick;
		}
	}

#elif defined(_GPK)
	if (m_cUserState == STATE_NONE || m_cUserState == STATE_CHECKVERSION)
		return;

	if (m_dwCheckGPKTick > 0 && m_dwCheckGPKTick + CHECKGPKTICK <= CurTick){
		SendGPKData();
		SendGPKAuthData();
		m_dwCheckGPKTick = CurTick;
	}

#endif	// _HSHIELD
}

#ifdef _HSHIELD
int CDNUserConnection::RequestHShieldResponse(AHNHS_TRANS_BUFFER *pResponse, int nLen)
{
	if (sizeof(AHNHS_TRANS_BUFFER) != nLen)
		return ERROR_INVALIDPACKET;

	DWORD dwCurTick = timeGetTime();
	if (dwCurTick - m_dwHShieldResponseTick >= CHECKRESPONSETICK){
		DetachConnection(L"CS_ACKCRC response Error");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] CS_ACKCRC response Error (%d)\r\n", GetAccountDBID(), GetSessionID(), dwCurTick - m_dwHShieldResponseTick);
		return ERROR_NONE;
	}

	unsigned long ulErrorCode = 0;
	DWORD dwRetVal = NOERROR;
	unsigned long ulRet = HShieldSvrWrapper::AhnHS_VerifyResponseEx(m_hHSClient, pResponse->byBuffer, pResponse->nLength, &ulErrorCode, &dwRetVal);
	if (NOERROR != dwRetVal) {
		DetachConnection(L"_AhnHS_VerifyResponseEx Error [Exception]");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] _AhnHS_VerifyResponseEx Error [Exception] : %x (%x)\r\n", m_nAccountDBID, m_nSessionID, ulRet, ulErrorCode);
		return ERROR_NONE;
	}

	if (ulRet == ANTICPX_RECOMMAND_CLOSE_SESSION)
	{
		// 구 버전 클라이언트 지원이 중지되었습니다
		// (서버 중지 없이 HSB 파일을 업데이트하였을 경우 HSB 정책에 구 버전 클라이언트를 지원하지 않도록
		// 설정되어 있을 경우 이 오류가 발생할 수 있다. 정책에 의한 것이므로 명백한 오류는 아니다.)
		// 2010.08.27 커멘트 추가
		// 해당 에러도 정상적인 경우엔 발생하면 안되는 경우여서 끊는다.
		//if( ulErrorCode != ERROR_ANTICPXSVR_OLD_VERSION_CLIENT_EXPIRED )
		{
			DetachConnection(L"_AhnHS_VerifyResponseEx Error");
			g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] _AhnHS_VerifyResponseEx Error: %x (%x)\r\n", m_nAccountDBID, m_nSessionID, ulRet, ulErrorCode);
		}
	}

	return ERROR_NONE;
}
#endif

#if defined(_GPK)
int CDNUserConnection::RequestGPKData(CSGPKData *pData, int nLen)
{
	if (sizeof(CSGPKData) != nLen)
		return ERROR_INVALIDPACKET;

	if (!g_Config.pDynCode->Decode((unsigned char*)pData->Data, GPKDATALENMAX, m_nCodeIndex)){
		g_Config.pDynCode->Release();

		g_Log.Log(LogType::_GPKERROR, this, L"[ADBID:%u, SID:%u] RequestGPKData error\r\n", m_nAccountDBID, m_nSessionID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if (strcmp(pData->Data, "ClientCheck!!!"))
	{
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	return ERROR_NONE;
}

int CDNUserConnection::RequestGPKAuthData(CSGPKAuthData *pData, int nLen)
{
	if (!g_Config.pGpkCmd) return ERROR_NONE;
	if (sizeof(CSGPKAuthData) != nLen)
		return ERROR_INVALIDPACKET;

	const unsigned char *p = NULL;
	int nRet = g_Config.pGpkCmd->CheckAuthReply(&p, pData->Data, pData->nLen);
	return ERROR_NONE;
}

#endif	// _GPK

#if defined(_JP)
int CDNUserConnection::GetJpnLoginMid(int nRet)
{
	switch(nRet)
	{
	case HAN_AUTHSVR_OK:					return	ERROR_HAN_AUTHSVR_OK;
	case HAN_AUTHSVR_ARGUMENT_INVALID:		return	ERROR_HAN_AUTHSVR_ARGUMENT_INVALID;
	case HAN_AUTHSVR_INITED_ALREADY:		return	ERROR_HAN_AUTHSVR_INITED_ALREADY;
	case HAN_AUTHSVR_INITED_NOT:			return	ERROR_HAN_AUTHSVR_INITED_NOT;
	case HAN_AUTHSVR_INITED_FAIL:			return	ERROR_HAN_AUTHSVR_INITED_FAIL;
	case HAN_AUTHSVR_CONNPOOL_CREATEFAIL:	return	ERROR_HAN_AUTHSVR_CONNPOOL_CREATEFAIL;
	case HAN_AUTHSVR_GETCONNECTION_FAIL:	return	ERROR_HAN_AUTHSVR_GETCONNECTION_FAIL;
	case HAN_AUTHSVR_OPENCONNECTION_FAIL:	return	ERROR_HAN_AUTHSVR_OPENCONNECTION_FAIL;
	case HAN_AUTHSVR_SENDCONNECTION_FAIL:	return	ERROR_HAN_AUTHSVR_SENDCONNECTION_FAIL;
	case HAN_AUTHSVR_RECVCONNECTION_FAIL:	return	ERROR_HAN_AUTHSVR_RECVCONNECTION_FAIL;
	case HAN_AUTHSVR_RETURNVALUE_INVALID:	return	ERROR_HAN_AUTHSVR_RETURNVALUE_INVALID;
	case HAN_AUTHSVR_AUTHSTRING_INVALID:	return	ERROR_HAN_AUTHSVR_AUTHSTRING_INVALID;
	case HAN_AUTHSVR_DLL_UNEXPECTED:		return	ERROR_HAN_AUTHSVR_DLL_UNEXPECTED;
	case HAN_AUTHSVR_PARAM_INVALID:			return	ERROR_HAN_AUTHSVR_PARAM_INVALID;
	case HAN_AUTHSVR_IP_INVALID 	:		return	ERROR_HAN_AUTHSVR_IP_INVALID;
	case HAN_AUTHSVR_MEMBERID_INVALID:		return	ERROR_HAN_AUTHSVR_MEMBERID_INVALID;
	case HAN_AUTHSVR_PASSWORD_INCORRECT:	return	ERROR_HAN_AUTHSVR_PASSWORD_INCORRECT;
	case HAN_AUTHSVR_PASSWORD_MISMATCHOVER:	return	ERROR_HAN_AUTHSVR_PASSWORD_MISMATCHOVER;
	case HAN_AUTHSVR_MEMBERID_NOTEXIST:		return	ERROR_HAN_AUTHSVR_MEMBERID_NOTEXIST;
	case HAN_AUTHSVR_SYSTEM_ERROR:			return	ERROR_HAN_AUTHSVR_SYSTEM_ERROR;
	case HAN_AUTHSVR_COOKIE_SETTINGERR:		return	ERROR_HAN_AUTHSVR_COOKIE_SETTINGERR;
	case HAN_AUTHSVR_COOKIE_NOTEXIST:		return	ERROR_HAN_AUTHSVR_COOKIE_NOTEXIST;
	case HAN_AUTHSVR_COOKIE_TIMEOUT:		return	ERROR_HAN_AUTHSVR_COOKIE_TIMEOUT;
	}
	return ERROR_HAN_AUTHSVR_0; 
}
#endif	// #if defined(_JP)

#if defined(_SG)
int CDNUserConnection::ConvertCherryCode2DNCode(int nCherryCode)
{
	switch (nCherryCode)
	{
	case CherryAuth::AuthResultCode::Fail: return ERROR_LOGIN_FAIL;
	case CherryAuth::AuthResultCode::Success: return ERROR_NONE;
	case CherryAuth::AuthResultCode::Locked: return ERROR_CHERRY_AUTHBLOCKED;
	case CherryAuth::AuthResultCode::Suspended: return ERROR_CHERRY_AUTHSUSPENDED;
	case CherryAuth::AuthResultCode::Others: return ERROR_CHERRY_AUTHOTHER;
	default:
		{
			_DANGER_POINT();		//이러시면 곤란합니다.
			return ERROR_GENERIC_UNKNOWNERROR;
		}
	}

	return ERROR_NONE;
}
#endif	// #if defined(_SG)

int CDNUserConnection::OnRecvLoginMessage(int nSubCmd, const char * pData, int nLen)
{
	switch (nSubCmd)
	{
	case CS_CHECKVERSION: return RequestCheckVersion((CSCheckVersion*)pData, nLen);

#if !defined(_FINAL_BUILD)
	case CS_CHECKLOGIN:	return RequestCheckLogin((CSCheckLogin*)pData, nLen);
#endif	// #if !defined(_FINAL_BUILD)
	case CS_CHECKLOGIN_KR: return RequestCheckLoginKR((CSCheckLoginKR*)pData, nLen);
	case CS_CHECKLOGIN_KRAZ: return RequestCheckLoginKRAZ((CSCheckLoginKRAZ*)pData, nLen);
	case CS_CHECKLOGIN_CH: return RequestCheckLoginCH((CSCheckLoginCH*)pData, nLen);
	case CS_CHECKLOGIN_JP: return RequestCheckLoginJP((CSCheckLoginJP*)pData, nLen);
	case CS_CHECKLOGIN_TW: return RequestCheckLoginTW((CSCheckLoginTW*)pData, nLen);
	case CS_CHECKLOGIN_US: return RequestCheckLoginUS((CSCheckLoginUS*)pData, nLen);
	case CS_CHECKLOGIN_SG: return RequestCheckLoginSG((CSCheckLoginSG*)pData, nLen);
	case CS_CHECKLOGIN_TH: return RequestCheckLoginTH((CSCheckLoginTH*)pData, nLen);
	case CS_CHECKLOGIN_TH_OTP: return RequestCheckLoginTHOTP((CSCheckLoginTHOTP*)pData, nLen);
	case CS_CHECKLOGIN_ID : return RequestCheckLoginID((CSCheckLoginID*)pData, nLen);
	case CS_CHECKLOGIN_RU : return RequestCheckLoginRU((CSCheckLoginRU*)pData, nLen);
	case CS_CHECKLOGIN_EU : return RequestCheckLoginEU((CSCheckLoginEU*)pData, nLen);

	case CS_SERVERLIST:	return RequestServerList(nLen);
	case CS_SELECTSERVER: return RequestSelectServer((CSSelectServer*)pData, nLen);
	case CS_SELECTCHAR: return RequestSelectCharacter((CSSelectChar*)pData, nLen);
	case CS_CREATECHAR: return RequestCreateCharacter((CSCreateChar*)pData, nLen);
	case CS_DELETECHAR: return RequestDeleteCharacter((CSDeleteChar*)pData, nLen);
	case CS_SELECTCHANNEL: return RequestSelectChannel((CSSelectChannel*)pData, nLen);
	case CS_BACKBUTTON: return RequestBackButton(nLen);
	case CS_CHANNELLIST: return RequestChannelList(nLen);
	case CS_BACKBUTTON_LOGIN: return RequestBackButton(nLen);
	case CS_REVIVECHAR: return RequestReviveCharacter((CSReviveChar*)pData, nLen);
	case CS_CHARACTERSORTCODE: return RequestCharacterSortCode((CSCharacterSortCode*)pData, nLen);
#if defined( PRE_ADD_DWC )
	case CS_CREATEDWCCHAR: return RequestCreateDWCCharacter((CSCreateDWCChar*)pData, nLen);
#endif // #if defined( PRE_ADD_DWC )
	}
	return ERROR_UNKNOWN_HEADER;
}

int CDNUserConnection::OnRecvSystemMessage(int nSubCmd, const char * pData, int nLen)
{
	switch(nSubCmd)
	{
	case CS_RECONNECTLOGIN:	return RequestReconnectLogin((CSReconnectLogin*)pData, nLen);	// village, game에서 login으로 다시 붙어서 캐릭터 셀렉트 창으로 보여줄때
#if defined(_GPK)
	case CS_CHN_GPKDATA: return RequestGPKData((CSGPKData*)pData, nLen);
	case CS_CHN_GPKAUTHDATA: return RequestGPKAuthData((CSGPKAuthData*)pData, nLen);
#endif	// _GPK
#if defined(_HSHIELD) // HackShield
	case CS_MAKERESPONSE: return RequestHShieldResponse((AHNHS_TRANS_BUFFER*)pData, nLen);
#endif	// _HSHIELD
	}
	return ERROR_UNKNOWN_HEADER;
}

int CDNUserConnection::OnRecvGameOptionMessage( int nSubCmd, const char* pData, int nLen )
{
	switch( nSubCmd )
	{
		case eGameOption::CS_UPDATE_SECONDAUTH_PASSWORD:	return RequestUpdateSecondAuthPassword( pData, nLen );
		case eGameOption::CS_INIT_SECONDAUTH:				return RequestInitSecondAuth( pData, nLen );
	}

	return ERROR_UNKNOWN_HEADER;
}

#ifdef PRE_ADD_DOORS
int CDNUserConnection::OnRecvDoorsMessage(int nSubCmd, const char * pData, int nLen)
{
	switch( nSubCmd )
	{
	case eDoors::CS_GET_AUTHKEY: return RequestGetDoorsAuthKey(pData, nLen);
	case eDoors::CS_CANCEL_AUTH: return RequestCancelDoorsAuth(pData, nLen);
	case eDoors::CS_GET_MOBILEAUTHFLAG: return RequestGetAuthFlag(pData, nLen);
	}

	return ERROR_UNKNOWN_HEADER;
}
#endif		//#ifdef PRE_ADD_DOORS

int CDNUserConnection::RequestCheckVersion(CSCheckVersion *pVersion, int nLen)
{
	if (sizeof(CSCheckVersion) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_NONE){
		DetachConnection(L"RequestCheckVersion State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCheckVersion State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

	bool bVersionCheck = false;
	if ((pVersion->cNation == g_Config.nNation) && (pVersion->nMajorVersion == g_Config.nMajorVersion) && (pVersion->nMinorVersion == g_Config.nMinorVersion))
		bVersionCheck = true;

#ifdef _FINAL_BUILD
	if (bVersionCheck)		//화이날 빌드에만 검사한다.
#endif //_FINAL_BUILD
	{
		SendCheckVersion(ERROR_NONE);	// 정상

#if defined(_HSHIELD)
		m_bUseHshield = pVersion->bCheck;

		if (m_bUseHshield){
			SendMakeRequest();	// CRC 요청
		}

#elif defined(_GPK)
		SendGPKCode();	// 보안코드 보내주고 (응답에 관계없이 그냥 보낸다)
		SendGPKAuthData();
		SendGPKData();
		m_dwCheckGPKTick = timeGetTime();
#endif

		m_cUserState = STATE_CHECKVERSION;

#if defined(_KR)
		memcpy(m_szMID, pVersion->szMID, sizeof(m_szMID));
		m_dwGRC = pVersion->dwGRC;
#endif

		return ERROR_NONE;
	}
#ifdef _FINAL_BUILD
	else
	{
		SendCheckVersion(ERROR_LOGIN_VERSION_UNMATCH);	// 비정상(?)
		return ERROR_GENERIC_INVALIDREQUEST;
	}
#endif //_FINAL_BUILD
	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLogin(CSCheckLogin *pLogin, int nLen)
{
	if (sizeof(CSCheckLogin) != nLen)
		return ERROR_INVALIDPACKET;

	if (pLogin->wszUserId[0] == 0 || pLogin->wszPassword[0] == 0){
		DetachConnection(L"Login Fail");
		return ERROR_NONE;
	}

	if (m_cUserState != STATE_CHECKVERSION){	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"RequestCheckLogin State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCheckLogin State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

	wcsncpy(m_wszAccountName, pLogin->wszUserId, IDLENMAX);
	g_Log.Log(LogType::_NORMAL, this, L"ID:%s, Password:%s\r\n", m_wszAccountName, pLogin->wszPassword);

	wcsncpy(m_wszVirtualIp, pLogin->wszVirtualIp, IPLENMAX);
	WideCharToMultiByte(CP_ACP, 0, m_wszVirtualIp, -1, m_szVirtualIp, IPLENMAX, NULL, NULL);

	int nRet = ERROR_DB;
	nRet = g_pSQLMembershipManager->QueryLogin(this, pLogin->wszPassword, TRUE);

	// M:-1 방지하기위해 로그인시 에러는 ERROR_LOGIN_FAIL 로 대체한다.
	if( nRet == ERROR_DB )
		nRet = ERROR_LOGIN_FAIL;

	SendCheckLogin(nRet);

	if (nRet == ERROR_NONE){
		m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
#ifdef PRE_ADD_MULTILANGUAGE
		m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(pLogin->cSelectedLanguage);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
	}
	else {
		// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
		SetAccountDBID(0);
	}

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginKR(CSCheckLoginKR *pLogin, int nLen)
{
	if (sizeof(CSCheckLoginKR) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION){	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_KR)
	WCHAR wszLoginIp[IPLENMAX] = { 0, };
	UINT nPwdHash = 0, nSsnHash = 0, nGender = 0;

	int nRet = -1;
	DWORD dwRetVal = NOERROR;
	nRet = NexonKorAuthCheckWrapper::_AuthCheck_LocalCheckPassport(pLogin->wszPassPort, m_wszAccountName, &m_nNexonSN, wszLoginIp, &nGender, (UINT32*)&m_cAge, &nPwdHash, &nSsnHash, NULL, NULL, NULL, NULL, NULL, &dwRetVal);
	if (NOERROR != dwRetVal) {
		// 예외 발생
		nRet = AUTHCHECK_ERROR_INVALID_ARGUMENT;
	}
	if (nRet == AUTHCHECK_ERROR_OK){
		SetPrmInt1(m_nNexonSN);

		wcsncpy(m_wszVirtualIp, pLogin->wszVirtualIp, IPLENMAX);
		WideCharToMultiByte(CP_ACP, 0, m_wszVirtualIp, -1, m_szVirtualIp, IPLENMAX, NULL, NULL);

		int nRet = ERROR_DB;
		nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, TRUE);

		SendCheckLogin(nRet);

		if (nRet == ERROR_NONE){
			m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
		}
		else {
			// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
			SetAccountDBID(0);
		}
	}
	else
	{
		// M:-1 방지하기위해 로그인시 에러는 ERROR_LOGIN_FAIL 로 대체한다.
		SendCheckLogin(ERROR_LOGIN_FAIL);
		g_Log.Log(LogType::_ERROR, this, L"AuthCheck_CheckPassport Error(Ret:%d) \r\n", nRet);
	}
#endif	// #if defined(_KR)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginKRAZ(CSCheckLoginKRAZ *pLogin, int nLen)
{
	if (sizeof(CSCheckLoginKRAZ) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION){	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_KRAZ) && defined(_FINAL_BUILD)
	wcsncpy(m_wszVirtualIp, pLogin->wszVirtualIp, IPLENMAX);
	WideCharToMultiByte(CP_ACP, 0, m_wszVirtualIp, -1, m_szVirtualIp, IPLENMAX, NULL, NULL);

	g_pActozAuth->SendWebLogin(GetSessionID(), pLogin->wszKeyID, pLogin->wszRequestTime, GetwszIp());

#else	// #if defined(_KRAZ) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	
#endif	// #if defined(_KRAZ) && defined(_FINAL_BUILD)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginUS(CSCheckLoginUS *pLogin, int nLen)
{
	if (sizeof(CSCheckLoginUS) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION){	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_US) && defined(_FINAL_BUILD)
	WCHAR wszLoginIp[IPLENMAX] = { 0, };
	UINT nPwdHash = 0, nSsnHash = 0, nGender = 0;

	// 넥슨 인증서버에 물어보기
	int nRet = AuthCheck_CheckPassport(pLogin->wszPassPort, GetwszIp(), true, m_wszAccountName, &m_nNexonSN, wszLoginIp, &nGender, (UINT32*)&m_cAge, &nPwdHash, &nSsnHash, NULL, NULL, NULL, NULL);
	if (nRet == AUTHCHECK_ERROR_OK){
		SetPrmInt1(m_nNexonSN);
#ifdef PRE_ADD_STEAM_USERCOUNT
		m_bSteamUser = pLogin->bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

		wcsncpy(m_wszVirtualIp, pLogin->wszVirtualIp, IPLENMAX);
		WideCharToMultiByte(CP_ACP, 0, m_wszVirtualIp, -1, m_szVirtualIp, IPLENMAX, NULL, NULL);

		nRet = ERROR_DB;
		nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, TRUE);

		if (nRet == ERROR_NONE){
			ImmigratorErrors Errors;
			if (m_nNexonUserNo == 0){	// 디비에 UserNo가 없을 경우 처음만 얻어온다
				char szAccountName[NAMELENMAX] = { 0, };
				WideCharToMultiByte(CP_ACP, 0, m_wszAccountName, -1, szAccountName, NAMELENMAX, NULL, NULL);

				Errors = g_pNexonPI->RequestRegistration(szAccountName, NULL, GetIp(), &m_nNexonUserNo);
				switch (Errors)
				{
				case Immigrator_OK:
				case Immigrator_OK_with_Custom_Event_Information:
					{
						g_pSQLMembershipManager->QueryAddAccountKey(m_nAccountDBID, m_nNexonUserNo);
					}
					break;

				default:
					{
						SendCheckLogin(ERROR_LOGIN_FAIL);
						g_Log.Log(LogType::_ERROR, this, L"[PI]RequestRegistration Error(%d)\r\n", Errors);
						return ERROR_NONE;
					}
				}
			}

			ImmigratorUserEntity UserEntity;
			Errors = g_pNexonPI->RequestLogin(m_nNexonUserNo, NULL, GetIp(), &UserEntity);
			switch (Errors)
			{
			case Immigrator_OK:
			case Immigrator_OK_with_Custom_Event_Information:
				{
					m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
				}
				break;

			default:
				{
					g_Log.Log(LogType::_ERROR, this, L"[PI]RequestLogin Error(%d)\r\n", Errors);
					nRet = ERROR_LOGIN_FAIL;
				}
				break;
			}
		}
		else {
			SetAccountDBID(0);
		}

		SendCheckLogin(nRet);
	}
	else
	{
		int nNexonResult = ConvertNexonAuthResult(nRet);
		SendCheckLogin(nNexonResult);
		g_Log.Log(LogType::_ERROR, this, L"AuthCheck_CheckPassport Error(Ret:%d, %d) \r\n", nRet, nNexonResult);
	}

#else	// #if defined(_US) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	

#endif	// #if defined(_US)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginCH(CSCheckLoginCH *pLogin, int nLen)
{
	if (m_cUserState != STATE_CHECKVERSION) {	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_CH) && defined(_FINAL_BUILD)

	if (pLogin->szPassport[0] == 0) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	wcsncpy_s(m_wszVirtualIp, _countof(m_wszVirtualIp), pLogin->wszVirtualIp, _countof(m_wszVirtualIp));
	WideCharToMultiByte(CP_ACP, 0, m_wszVirtualIp, -1, m_szVirtualIp, IPLENMAX, NULL, NULL);

	int nRetVal = g_pShandaAuth->AsyncAuthen(pLogin->szPassport, GetIp(), GetSessionID());
	if (nRetVal) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#else	// #if defined(_CH) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	
#endif	// #if defined(_CH)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginJP(CSCheckLoginJP *pLogin, int nLen)
{
	if (sizeof(CSCheckLoginJP) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION){	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_JP) && defined(_FINAL_BUILD)
	if( pLogin->PartnerType < Partner::Type::None || pLogin->PartnerType >= Partner::Type::Max )
	{
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] Invalid PartnerType:%d\r\n", m_nAccountDBID, m_nSessionID, pLogin->PartnerType );
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	char szMemberID[100] = { 0, };
	sprintf(szMemberID, "%I64d", pLogin->nMemberID);

	WideCharToMultiByte(CP_ACP, 0, m_wszVirtualIp, -1, m_szVirtualIp, IPLENMAX, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, szMemberID, -1, m_wszAccountName, IDLENMAX);

	char szOutBuf[SIZE_OUTBUFFER] = { 0x00 };
	int nRet = HanAuthForSvr(szMemberID, pLogin->szAuthString, szOutBuf, sizeof(szOutBuf), m_szVirtualIp);

	if (nRet == HAN_AUTHSVR_OK){
		int nRet = ERROR_DB;
		m_PartnerType = pLogin->PartnerType;
		nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, TRUE);

		SendCheckLogin(nRet);

		if (nRet == ERROR_NONE){
			m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
		}
		else {
			// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
			SetAccountDBID(0);
		}
	}
	else{
		SendCheckLogin(GetJpnLoginMid(nRet));
		g_Log.Log(LogType::_ERROR, this, L"HanAuthForSvr Error(Ret:%d) \r\n", nRet);
	}
#else	// #if defined(_JP) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	

#endif	// #if defined(_JP)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginTW(CSCheckLoginTW *pLogin, int nLen)
{
	if (sizeof(CSCheckLoginTW) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION) {	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_TW) && defined(_FINAL_BUILD)
	if (pLogin->szAccountName[0] == 0 || pLogin->szPassword[0] == 0) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}
	// 영어..숫자 이외 체크
	{
		USES_CONVERSION;
		if( !g_CountryUnicodeSet.Check(A2CW(pLogin->szAccountName), ALLOW_STRING_DEFAULT_ENG) 
			|| !g_CountryUnicodeSet.Check(A2CW(pLogin->szPassword), ALLOW_STRING_DEFAULT_ENG) ) 		
		{	
			SendCheckLogin(ERROR_GAMANIA_SPECIAL_CHAR);
			return ERROR_NONE;
		}	
	}

	// 계정 비번 길이 체크(최소 8자)
	if( strlen(pLogin->szAccountName) < 8 || strlen(pLogin->szPassword) < 8 )
	{
		SendCheckLogin(ERROR_GAMANIA_NO_SUCH_ACCOUNT);
		return ERROR_NONE;
	}

	// 다른 국가들 처럼 인증 결과 성공 시 다시 받을 수 없으므로 여기서 미리 지정
	{
		USES_CONVERSION;

		SetAccountName(A2CW(pLogin->szAccountName));
	}
	strncpy(m_szVirtualIp, pLogin->szVirtualIp, IPLENMAX-1);
	MultiByteToWideChar(CP_ACP, 0, m_szVirtualIp, -1, m_wszVirtualIp, IPLENMAX);

	g_pGamaniaAuthLogin->SendLogin(pLogin->szAccountName, pLogin->szPassword, GetIp(), true, false, m_nSessionID);

#else	// #if defined(_TW) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	

#endif	// #if defined(_TW)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginSG(CSCheckLoginSG *pLogin, int nLen)
{
	if (sizeof(CSCheckLoginSG) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION) {	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_SG)
	if (pLogin->szLoginID[0] == 0 || pLogin->szPassWord[0] == 0) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	// 영어..숫자 이외 체크
	{
		USES_CONVERSION;
		if( !g_CountryUnicodeSet.Check(A2CW(pLogin->szLoginID), ALLOW_STRING_LOGIN_SG) 
			|| !g_CountryUnicodeSet.Check(A2CW(pLogin->szPassWord), ALLOW_STRING_PASS_SG) )
		{	
			SendCheckLogin(ERROR_CHERRY_NOTALLOWED_SPECIALCHAR);
			return ERROR_NONE;
		}	
	}

	// 계정 비번 길이 체크(최소 4자)
	if( strlen(pLogin->szLoginID) < 6 || strlen(pLogin->szPassWord) < 6 )
	{
		SendCheckLogin(ERROR_CHERRY_NO_SUCH_ACCOUNT);
		return ERROR_NONE;
	}

	DWORD dwBeginTime = timeGetTime();
	g_Log.Log(LogType::_ERROR, this, L"CherryAuthenticate ThreadID[%d] SeesionID[%d] LoginID[%S] begin time[%d]\n", GetCurrentThreadId(), GetSessionID(), pLogin->szLoginID, dwBeginTime);

#if defined (PRE_MOD_SG_WITH_MACADDR)
	CherryAuthenticate(g_Config.szCherryAuthAddr, g_Config.nCherryGameID, pLogin->szLoginID, pLogin->szPassWord, GetIp(), pLogin->szMacAddress, m_CherryAuthInfo);
#else		//#if defined (PRE_MOD_SG_WITH_MACADDR)
	CherryAuthenticate(g_Config.szCherryAuthAddr, g_Config.nCherryGameID, pLogin->szLoginID, pLogin->szPassWord, GetIp(), m_CherryAuthInfo);
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)

	g_Log.Log(LogType::_ERROR, this, L"CherryAuthenticate ThreadID[%d] SessionID[%d] LoginID[%S] end time[%d]\n", GetCurrentThreadId(), GetSessionID(), pLogin->szLoginID, timeGetTime() - dwBeginTime);

	int nRetCode = ConvertCherryCode2DNCode(m_CherryAuthInfo.nStatusCode);
	if (nRetCode == ERROR_NONE && m_CherryAuthInfo.strCherryID.empty() == false)
	{
		{
			USES_CONVERSION;
			SetAccountName(A2CW(m_CherryAuthInfo.strCherryID.c_str()));
		}

		int nRet = ERROR_DB;
		nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, TRUE);

		SendCheckLogin(nRet);
		if( nRet == ERROR_NONE)
			m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
		else
			// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
			SetAccountDBID(0);
	}
	else
	{
		// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
		SetAccountDBID(0);
		SendCheckLogin(nRetCode);
		g_Log.Log(LogType::_ERROR, this, L"CherryAuth Error(Ret:%d) \r\n", nRetCode);
	}
#endif	// #if defined(_SG)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginTH(CSCheckLoginTH *pLogin, int nLen)
{
	if (sizeof(CSCheckLoginTH) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION) {	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_TH) && defined(_FINAL_BUILD)
	if (pLogin->szAccountName[0] == 0 || pLogin->szPassword[0] == 0) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	// 아이디 패스워드 사이즈 체크
	if (strlen(pLogin->szAccountName) <= 0 || strlen(pLogin->szAccountName) >= IDLENMAX || strlen(pLogin->szPassword) <= 0 || strlen(pLogin->szPassword) >= PASSWORDLENMAX)
	{
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	// 영어..숫자 이외 체크
	{
		USES_CONVERSION;
		if( !g_CountryUnicodeSet.Check(A2CW(pLogin->szAccountName), ALLOW_STRING_LOGIN_TH) 
			|| !g_CountryUnicodeSet.Check(A2CW(pLogin->szPassword), ALLOW_STRING_DEFAULT_ENG) ) 		
		{	
			SendCheckLogin(ERROR_ASIASOFT_SPECIAL_CHAR);
			return ERROR_NONE;
		}	
	}

	strncpy(m_szVirtualIp, pLogin->szVirtualIp, IPLENMAX);
	MultiByteToWideChar(CP_ACP, 0, m_szVirtualIp, -1, m_wszVirtualIp, IPLENMAX);

	m_AsiaAuthInfo.strDomain = pLogin->szDomain;

	int nSendRet = g_pAsiaSoftAuth->SendLogin(pLogin->szAccountName, pLogin->szPassword, pLogin->szDomain, GetIp(), m_nSessionID);
	if (nSendRet != ERROR_NONE)
	{
		SendCheckLogin(ERROR_ASIASOFT_INVALID_REQUEST);
		return ERROR_NONE;
	}

#else	// #if defined(_TH) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	
#endif	// #if defined(_TH)

	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginTHOTP(CSCheckLoginTHOTP * pLogin, int nLen)
{
#if defined (_TH) && defined(_FINAL_BUILD)
	if (sizeof(CSCheckLoginTHOTP) != nLen)
		return ERROR_GENERIC_INVALIDREQUEST;

	g_pAsiaSoftOTPManager->SendOTP(m_AsiaAuthInfo.strDomain.c_str(), m_AsiaAuthInfo.strAkeyMasterID.c_str(), pLogin->szOTPResponse, GetSessionID());
#else		//#if defined (_TH)
	return ERROR_GENERIC_INVALIDREQUEST;
#endif		//#if defined (_TH)	
	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginID(CSCheckLoginID* pLogin, int nLen)
{
	if (sizeof(CSCheckLoginID) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION) {	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_ID) && defined(_FINAL_BUILD)
	if (pLogin->szAccountName[0] == 0 || pLogin->szPassword[0] == 0) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}	

	// 아이디 패스워드 사이즈 체크
	if (strlen(pLogin->szAccountName) <= 0 || strlen(pLogin->szAccountName) >= IDLENMAX || strlen(pLogin->szPassword) <= 0 || strlen(pLogin->szPassword) != PASSWORDLENMAX-1)
	{
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	strncpy(m_szVirtualIp, pLogin->szVirtualIp, IPLENMAX);
	MultiByteToWideChar(CP_ACP, 0, m_szVirtualIp, -1, m_wszVirtualIp, IPLENMAX);

	DWORD dwBeginTime = timeGetTime();
	g_Log.Log(LogType::_ERROR, this, L"KreonAuth ThreadID[%d] SeesionID[%d] LoginID[%S] begin time[%d]\n", GetCurrentThreadId(), GetSessionID(), pLogin->szAccountName, dwBeginTime);

	int nRetCode = KreonAuthLogin(pLogin->szAccountName, pLogin->szPassword, GetIp(), m_dwKreonCN);

	g_Log.Log(LogType::_ERROR, this, L"KreonAuth ThreadID[%d] SessionID[%d] LoginID[%S] end time[%d]\n", GetCurrentThreadId(), GetSessionID(), pLogin->szAccountName, timeGetTime() - dwBeginTime);	
	
	if (nRetCode == ERROR_NONE )
	{
		{
			USES_CONVERSION;
			SetAccountName(A2CW(pLogin->szAccountName));
		}

		int nRet = ERROR_DB;
		nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, TRUE);		

		SendCheckLogin(nRet);
		if( nRet == ERROR_NONE)
		{
			m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
			_strcpy(m_szMacAddress, _countof(m_szMacAddress), pLogin->szMacAddress, (int)strlen(pLogin->szMacAddress));	
			_strcpy(m_szKey, _countof(m_szKey), pLogin->szKey, (int)strlen(pLogin->szKey));
			SetPrmInt1(m_dwKreonCN);
			if( strlen(pLogin->szMacAddress) == 0)
			{
				g_Log.Log(LogType::_ERROR, this, L"Kreon PcCafe NULL MacAddress Result:%d\r\n", pLogin->nReturnError);
			}
		}
		else
			// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
			SetAccountDBID(0);
	}
	else
	{
		// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
		SetAccountDBID(0);
		SendCheckLogin(nRetCode);
		g_Log.Log(LogType::_ERROR, this, L"KreonAuth Error(Ret:%d) \r\n", nRetCode);
	}	
#else	// #if defined(_ID) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	

#endif	// #if defined(_ID)
	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginRU(CSCheckLoginRU* pLogin, int nLen)
{
	if (sizeof(CSCheckLoginRU) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHECKVERSION) {	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_RU) && defined(_FINAL_BUILD)
	if (pLogin->szAccountName[0] == 0 || pLogin->szOTPHash[0] == 0) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}
	pLogin->szAccountName[IDLENMAX-1] = 0;
	pLogin->szOTPHash[MAILRUOTPMAX-1] = 0;
	pLogin->szVirtualIp[IPLENMAX-1] = 0;

	// 아이디 패스워드 사이즈 체크
	if (strlen(pLogin->szAccountName) <= 0 || strlen(pLogin->szAccountName) >= IDLENMAX || strlen(pLogin->szOTPHash) <= 0 || strlen(pLogin->szOTPHash) != MAILRUOTPMAX-1)
	{
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	strncpy(m_szVirtualIp, pLogin->szVirtualIp, IPLENMAX);
	MultiByteToWideChar(CP_ACP, 0, m_szVirtualIp, -1, m_wszVirtualIp, IPLENMAX);

	DWORD dwBeginTime = timeGetTime();
	g_Log.Log(LogType::_ERROR, this, L"MailRUAuth ThreadID[%d] SeesionID[%d] LoginID[%S] begin time[%d]\n", GetCurrentThreadId(), GetSessionID(), pLogin->szAccountName, dwBeginTime);

	int nRetCode = MailRUAuthLogin(pLogin->szAccountName, pLogin->szOTPHash, GetIp());

	g_Log.Log(LogType::_ERROR, this, L"MailRUAUth ThreadID[%d] SessionID[%d] LoginID[%S] end time[%d]\n", GetCurrentThreadId(), GetSessionID(), pLogin->szAccountName, timeGetTime() - dwBeginTime);	

	if (nRetCode == ERROR_NONE )
	{
		{
			USES_CONVERSION;
			SetAccountName(A2CW(pLogin->szAccountName));
		}

		int nRet = ERROR_DB;
		nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, TRUE);		

		SendCheckLogin(nRet);
		if( nRet == ERROR_NONE)
		{
			m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
		}
		else
			// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
			SetAccountDBID(0);
	}
	else
	{
		// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
		SetAccountDBID(0);
		SendCheckLogin(nRetCode);
		g_Log.Log(LogType::_ERROR, this, L"MailRUAUth Error(Ret:%d) \r\n", nRetCode);
	}

#else	// #if defined(_RU) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	

#endif	// #if defined(_RU)
	return ERROR_NONE;
}

int CDNUserConnection::RequestCheckLoginEU(CSCheckLoginEU* pLogin, int nLen)
{
	if (m_cUserState != STATE_CHECKVERSION) {	// checkversion 상태가 아닌데 들어오면 안된다
		DetachConnection(L"state_none mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHECKVERSION mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(_EU) && defined(_FINAL_BUILD)
	if (pLogin->szAccountName[0] == 0 || pLogin->szPassword[0] == 0) {
		DetachConnection(L"Login Fail");
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	_strcpy(m_szVirtualIp, IPLENMAX, pLogin->szVirtualIp, IPLENMAX);
	MultiByteToWideChar(CP_ACP, 0, m_szVirtualIp, -1, m_wszVirtualIp, IPLENMAX);

	g_pEUAuth->Authen(GetSessionID(), pLogin->szAccountName, pLogin->szPassword, GetIp());

#else	// #if defined(_EU) && defined(_FINAL_BUILD)
	SetAccountDBID(0);
	SendCheckLogin(ERROR_LOGIN_FAIL);	
#endif	// #if defined(_CH)

#ifdef PRE_ADD_MULTILANGUAGE
	m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(pLogin->cSelectedLanguage);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

	return ERROR_NONE;
}

int CDNUserConnection::RequestServerList(int nLen)
{
	if (nLen != 0)
		return ERROR_INVALIDPACKET;

	if ((m_cUserState != STATE_CHECKLOGIN) && (m_cUserState != STATE_CHARLIST) && (m_cUserState != STATE_WAITUSER)){
		DetachConnection(L"RequestServerList State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestServerList State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

	SendDelUser();

#if defined(PRE_MOD_SELECT_CHAR)
	g_pSQLMembershipManager->QueryGetCharacterCount( this, m_MapWorldCharCount );	// 해당 SP 는 부가 정보이기 때문에 별도의 Ret 처리를 하지 않는다.
	SendServerList( m_MapWorldCharCount );
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	std::map<int,int> mWorldCharCount;	// WorldID,CharCount
	g_pSQLMembershipManager->QueryGetCharacterCount( this, mWorldCharCount );	// 해당 SP 는 부가 정보이기 때문에 별도의 Ret 처리를 하지 않는다.
	SendServerList( mWorldCharCount );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	m_cUserState = STATE_SERVERLIST;

	return ERROR_NONE;
}

int CDNUserConnection::RequestSelectServer(CSSelectServer *pSelectServer, int nLen)
{
	if (sizeof(CSSelectServer) != nLen)
		return ERROR_INVALIDPACKET;

	if ((m_cUserState != STATE_SERVERLIST) && (m_cUserState != STATE_CHECKLOGIN) && (m_cUserState != STATE_CHANNELLIST)){
		DetachConnection(L"RequestSelectServer State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestSelectServer State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}
	// SelectServer 했을 때 2차 인증 정보 얻어온다.
	int iRet = g_pSQLMembershipManager->QueryGetSecondAuthStatus( m_nAccountDBID, m_bIsSetSecondAuthPW, m_bIsSetSecondAuthLock, m_tSecondAuthResetDate );
	if( iRet != ERROR_NONE )
	{
		DetachConnection(L"QueryGetSecondAuthStatus Failed");
		return ERROR_NONE;
	}

	// 2차 비밀번호가 설정되어 있지만 2차 보안 계정 Lock 이 설정되어 있지 않다면
	// SelectServer 했을 때 강제로 2차 보안 계정 Lock 상태로 만들어준다.
	if( m_bIsSetSecondAuthPW && !m_bIsSetSecondAuthLock )
	{
		iRet = g_pSQLMembershipManager->QueryModSecondAuthLockFlag( m_nAccountDBID, true );
		if( iRet == ERROR_NONE )
		{
			m_bIsSetSecondAuthLock = true;
		}
		else
		{
			DetachConnection(L"QueryModSecondAuthLockFlag Failed");
			return ERROR_NONE;
		}
	}

#ifdef PRE_ADD_IGNORESECONDAUTH_EKEYECARD
	m_bPassSecondAuth = (CheckSndaAuthFlag(eChSndaAuthFlag_UseEKey) || CheckSndaAuthFlag(eChSndaAuthFlag_UseECard));
	if (m_bIsSetSecondAuthPW)
		m_bCompletlyPassSecondAuth = true;
#endif

#ifdef PRE_ADD_23829
	if (m_bPassSecondAuth == false && m_bIsSetSecondAuthPW == false)
	{
		//이키, 이카드, 이차인증 다 안쓰고 있다면 체킹한번 해보자.
		iRet = g_pSQLMembershipManager->QueryCheckLastSecondAuthNotifyDate(m_nAccountDBID, SECONDAUTHIGNOREPERIOD);
		if (iRet == ERROR_NONE)		//아직 체크피리어드가 남았다면 물어보는 페이지 Pass!!!!
			m_bPassSecondAuth = true;
	}	
#endif

	// 2차 인증 정보 보내준다.
	SendSecondAuthInfo();

	if (CHECK_RANGE(pSelectServer->cWorldID, 1, WORLDCOUNTMAX)) 
	{
		//셋하기전에 알아 본다.
	}
	else if (pSelectServer->cWorldID != -1)
	{
		DetachConnection(L"ServerIndex mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] pSelectServer->cWorldID (%d)\r\n", m_nAccountDBID, m_nSessionID, pSelectServer->cWorldID);
		return ERROR_NONE;
	}

#if defined(PRE_MOD_SELECT_CHAR)
#if defined(_TW)
	int nWorldSetID = pSelectServer->cWorldID;
	nWorldSetID = g_pExtManager->GetWorldSetID(nWorldSetID);
	if( nWorldSetID == g_Config.nDenyWorld && m_cRegion == 95) // 95--홍콩
	{
		// 리스트도 안보내주는데 들어올려고 하다니..
		DetachConnection(L"Deny World");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] HongKong User Deny World (%d)\r\n", m_nAccountDBID, m_nSessionID, nWorldSetID);
		return ERROR_NONE;
	}
#endif //#if defined(_TW)

	SetWorldID(pSelectServer->cWorldID);

	SendCharacterList(false);

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if (m_bMasterAddUser == false)
	{
#ifdef _USE_ACCEPTEX
		ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif

		int nWorldSetID = pSelectServer->cWorldID;
		nWorldSetID = g_pExtManager->GetWorldSetID(nWorldSetID);
#if defined(_TW)
		if( nWorldSetID == g_Config.nDenyWorld && m_cRegion == 95) // 95--홍콩
		{
			// 리스트도 안보내주는데 들어올려고 하다니..
			DetachConnection(L"Deny World");
			g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] HongKong User Deny World (%d)\r\n", m_nAccountDBID, m_nSessionID, nWorldSetID);
			return ERROR_NONE;
		}
#endif //#if defined(_TW)

		CDNMasterConnection *pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(nWorldSetID, false));
		if (pMasterCon)
			pMasterCon->SendAddUser(this, pSelectServer->cWorldID);
		else
		{
			DetachConnection(L"MasterCon NOtFound");
			g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] pSelectServer->cWorldID (%d)\r\n", m_nAccountDBID, m_nSessionID, pSelectServer->cWorldID);
			return ERROR_NONE;
		}
	}
	else
	{
		SendCharacterList(false);
	}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	return ERROR_NONE;
}

bool CDNUserConnection::CheckAbuserCharLength()
{
#if defined( _CH ) || defined( _TW )
	return true;
#endif
#if defined(PRE_MOD_SELECT_CHAR)
	if (m_MapCharacterList.empty()) return true;

	TMapCharacterList::iterator iter;
	for (iter = m_MapCharacterList.begin(); iter != m_MapCharacterList.end(); iter++)
	{
		if (iter->second.biCharacterDBID <= 0 ) 
			continue;		

		int nMaxLength = g_pExtManager->GetGlobalWeightValue(CHARACTERNAME_MAXLENGTH);

		if( (nMaxLength > 0) && ((int)(wcslen(iter->second.wszCharacterName)) >= nMaxLength) )
			return false;
	}

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	for (int i = 0; i < m_cCharacterLimitCount; i++)
	{
		if (m_CharDataArray[i].biCharacterDBID <= 0 ) 
			continue;		

		int nMaxLength = g_pExtManager->GetGlobalWeightValue(CHARACTERNAME_MAXLENGTH);

		if( (nMaxLength > 0) && ((int)(wcslen(m_CharDataArray[i].wszCharacterName)) >= nMaxLength) )
			return false;
	}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	return true;
}

void CDNUserConnection::QueryAddRestraint(wchar_t* pwszReason)
{
	//영구 블럭
	TIMESTAMP_STRUCT StartDate, EndDate;
	// 시작시각
	CTimeSet TimeSet;
	memcpy( &StartDate, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT ));
	// 끝시간..10년간 블럭
	TimeSet.Set(TimeSet.GetYear()+10, TimeSet.GetMonth(), TimeSet.GetDay(), TimeSet.GetHour(), TimeSet.GetMinute(), TimeSet.GetSecond() );
	memcpy( &EndDate, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT ));

	g_pSQLMembershipManager->QueryAddRestraint( DBDNWorldDef::RestraintTargetCode::Account, m_nAccountDBID, 0, 110, DBDNWorldDef::RestraintTypeCode::ConnectRestraint, pwszReason, NULL, StartDate, EndDate );
	DetachConnection( pwszReason );
	g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] CheckAbuserCharLength Restraint \r\n", m_nAccountDBID, m_nSessionID);
}

#if defined( PRE_ADD_DWC )
int CDNUserConnection::QueryCharacterListAndSecondAuth( bool bSecondAuthProcess, INT64 biDWCCharacterDBID )
#else // #if defined( PRE_ADD_DWC )
int CDNUserConnection::QueryCharacterListAndSecondAuth( bool bSecondAuthProcess )
#endif // #if defined( PRE_ADD_DWC )
{
	ClearCharacterData();
#if defined(PRE_MOD_SELECT_CHAR)
	SetAvailableCharacterSlotCount();
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	m_cCharCountMax = CreateCharacterDefaultCountMax;
	g_pSQLMembershipManager->QueryGetCharacterSlotCount(m_nAccountDBID, m_cWorldID, 0, m_cCharCountMax);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined( PRE_ADD_DWC )
	char cDWCSlotCount = biDWCCharacterDBID ? 1 : 0;
#endif // #if defined( PRE_ADD_DWC )

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 16;
#endif

#if defined(PRE_MOD_SELECT_CHAR)
	g_pSQLMembershipManager->QueryGetCharacterCount( this, m_MapWorldCharCount );	// 해당 SP 는 부가 정보이기 때문에 별도의 Ret 처리를 하지 않는다.

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 17;
#endif

	int nRet = ERROR_NONE;
	if (!m_MapWorldCharCount.empty()){
		nRet = ERROR_DB;
		for (TMapWorldCharCount::iterator iter = m_MapWorldCharCount.begin(); iter != m_MapWorldCharCount.end(); iter++){
#if defined(_WORK)
			int nWorldSetID = GetWorldSetID();
#else	// #if defined(_WORK)
			int nWorldSetID = g_pExtManager->GetWorldSetID(iter->first);
#endif	// #if defined(_WORK)

#if defined( PRE_ADD_DWC )
			nRet = g_pSQLWorldManager->QueryGetListCharacter(nWorldSetID, iter->first, m_nAccountDBID, m_cAvailableCharacterSlotCount + cDWCSlotCount, m_MapCharacterList, biDWCCharacterDBID, GetAccountLevel());
#else // #if defined( PRE_ADD_DWC )
			nRet = g_pSQLWorldManager->QueryGetListCharacter(nWorldSetID, iter->first, m_nAccountDBID, m_cAvailableCharacterSlotCount, m_MapCharacterList);
#endif // #if defined( PRE_ADD_DWC )
			if (nRet != ERROR_NONE)
				return nRet;
		}
	}
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 18;
#endif

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	int nWorldID = m_cWorldID;
#if defined(_WORK)
	nWorldID = 0;
#endif	// #if defined(_WORK)

#if defined( PRE_ADD_DWC )
	int nRet = g_pSQLWorldManager->QueryGetListCharacter(m_nWorldSetID, nWorldID, m_nAccountDBID, m_cCharCountMax + cDWCSlotCount, m_CharDataArray, biDWCCharacterDBID, GetAccountLevel());
#else // #if defined( PRE_ADD_DWC )
	int nRet = g_pSQLWorldManager->QueryGetListCharacter(m_nWorldSetID, nWorldID, m_nAccountDBID, m_cCharCountMax, m_CharDataArray);
#endif // #if defined( PRE_ADD_DWC )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	if( nRet == ERROR_NONE ) 
	{
		if( bSecondAuthProcess )
		{
#if defined( PRE_LOGINDUMP_LOG )	
			m_nReconnectLoginStep = 19;
#endif
			// 2차 인증 정보 얻어오기
			if( g_pSQLMembershipManager->QueryGetSecondAuthStatus( m_nAccountDBID, m_bIsSetSecondAuthPW, m_bIsSetSecondAuthLock, m_tSecondAuthResetDate ) == ERROR_NONE )
			{
#ifdef PRE_ADD_IGNORESECONDAUTH_EKEYECARD
				m_bPassSecondAuth = (CheckSndaAuthFlag(eChSndaAuthFlag_UseEKey) || CheckSndaAuthFlag(eChSndaAuthFlag_UseECard));
				if (m_bIsSetSecondAuthPW)
					m_bCompletlyPassSecondAuth = true;
#endif

#if defined( PRE_LOGINDUMP_LOG )	
				m_nReconnectLoginStep = 20;
#endif

#ifdef PRE_ADD_23829
				if (m_bPassSecondAuth == false && m_bIsSetSecondAuthPW == false)
				{
					//이키, 이카드, 이차인증 다 안쓰고 있다면 체킹한번 해보자.
					int iRet = g_pSQLMembershipManager->QueryCheckLastSecondAuthNotifyDate(m_nAccountDBID, SECONDAUTHIGNOREPERIOD);
					if (iRet == ERROR_NONE)		//아직 체크피리어드가 남았다면 물어보는 페이지 Pass!!!!
						m_bPassSecondAuth = true;
				}	
#endif
#if defined( PRE_LOGINDUMP_LOG )	
				m_nReconnectLoginStep = 21;
#endif

				SendSecondAuthInfo();
				m_cUserState = STATE_CHARLIST;
			}
			else
			{
				_DANGER_POINT();
				nRet = ERROR_LOGIN_CHARACTERLIST_ERROR;
			}
		}
		else
		{
			m_cUserState = STATE_CHARLIST;
		}
	}

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 22;
#endif
	return nRet;
}

int CDNUserConnection::QueryValidataSecondAuthPassphrase( WCHAR* pwszPW, BYTE& cFailCount, bool bSetIgnoreSecondAuth )
{
	// 2차 비밀번호가 설정된 계정이라면 2차 비밀번호 확인함
	if( m_bIsSetSecondAuthPW )
	{
		WCHAR wszSecondAuthPW[SecondAuth::Common::PWMaxLength+1];
		memset( wszSecondAuthPW, 0, sizeof(wszSecondAuthPW) );
		memcpy( wszSecondAuthPW, pwszPW, sizeof(WCHAR)*SecondAuth::Common::PWMaxLength );

		int nRet = g_pSQLMembershipManager->QueryValidataSecondAuthPassphrase( m_nAccountDBID, wszSecondAuthPW, cFailCount );
		if( nRet != ERROR_NONE )
		{
			if( cFailCount >= SecondAuth::Common::LimitCount )
			{
				TIMESTAMP_STRUCT StartDate,EndDate;
				// 시작시각
				CTimeSet TimeSet;
				memcpy( &StartDate, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT ));
				// 끝시간
				TimeSet.AddSecond( 60*SecondAuth::Common::RestraintTimeMin );
				memcpy( &EndDate, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT ));

				g_pSQLMembershipManager->QueryAddRestraint( DBDNWorldDef::RestraintTargetCode::Account, m_nAccountDBID, 0, SecondAuth::Common::RestraintReasonID, DBDNWorldDef::RestraintTypeCode::ConnectRestraint, NULL, NULL, StartDate, EndDate );
				SendCompleteDetachMsg( ERROR_SECONDAUTH_CHECK_MAXFAILED, L"SecondAuth Restraint" );
			}
		}

		return nRet;
	}
#ifdef PRE_ADD_23829
	else if (bSetIgnoreSecondAuth)
	{
		int nRet = g_pSQLMembershipManager->QueryModLastSecondAuthNotifyDate(m_nAccountDBID);
		if (nRet == ERROR_NONE)
		{
			m_bPassSecondAuth = true;
			SendSecondAuthInfo();
		}
		return nRet;
	}
#endif

	return ERROR_NONE;
}

int CDNUserConnection::RequestSelectCharacter(CSSelectChar *pSelect, int nLen)
{
	if (sizeof(CSSelectChar) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHARLIST){
		DetachConnection(L"RequestSelectCharacter State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestSelectCharacter State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

#if defined(PRE_MOD_SELECT_CHAR)
	if (!IsValidCharacter(pSelect->biCharacterDBID)){
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if (!IsValidCharacterIndex(pSelect->cCharIndex)){
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
		DetachConnection(L"Select Char Fail(Index not found)");
		return ERROR_NONE;
	}

#if defined(PRE_MOD_SELECT_CHAR)
#if !defined(_WORK)
	int cSelectWorldSetID = GetWorldSetIDByCharacterDBID(pSelect->biCharacterDBID);
	CDNMasterConnection *pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(cSelectWorldSetID, false));
	if (!pMasterCon){
		SendChannelList(ERROR_GENERIC_MASTERCON_NOT_FOUND, 0);
		return ERROR_NONE;
	}
#endif	// #if !defined(_WORK)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	BYTE cFailCount = 0;
#ifdef PRE_ADD_23829
	int iRet = QueryValidataSecondAuthPassphrase( (WCHAR*)MakeSecondPasswordString( szVersion, pSelect->nSeed, pSelect->nValue ).c_str(), cFailCount, pSelect->bSetIgnoreAuthNotify );
#else
	int iRet = QueryValidataSecondAuthPassphrase( (WCHAR*)MakeSecondPasswordString( szVersion, pSelect->nSeed, pSelect->nValue ).c_str(), cFailCount );
#endif
	if( iRet != ERROR_NONE )
	{
		SendChannelList( iRet, cFailCount );
		return ERROR_NONE;
	}

#if defined(PRE_MOD_SELECT_CHAR)
	SetSelectCharacterData(pSelect->biCharacterDBID);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	m_cSelectCharIndex = pSelect->cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined( PRE_ADD_DWC ) // DWC 캐릭터는 튜토리얼을 세팅하지 않음
	if( GetSelectCharacterAccountLevel() == AccountLevel_DWC )
	{
		if(!g_pDWCChannelManager->CheckValidDate())
		{
			SendChannelList( ERROR_LOGIN_DWC_WRONG_DATE, 0 ); // 대회 기간이 아님.
			return ERROR_NONE;
		}
	}
	else
	{
		// 튜토리얼 세팅
		if (GetSelectCharacterMapIndex() == 0){
#if defined(PRE_MOD_SELECT_CHAR)
			g_pExtManager->GetCreateDefaultMapInfo(m_SelectCharacterData.cJob, m_SelectCharacterData.nMapID, m_nTutorialMapIndex, m_cTutorialGateNo);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
			g_pExtManager->GetCreateDefaultMapInfo(SelectCharData()->cJob, SelectCharData()->nMapID, m_nTutorialMapIndex, m_cTutorialGateNo);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
		}
	}
#else // #if defined( PRE_ADD_DWC )
	// 튜토리얼 세팅
	if (GetSelectCharacterMapIndex() == 0){
#if defined(PRE_MOD_SELECT_CHAR)
		g_pExtManager->GetCreateDefaultMapInfo(m_SelectCharacterData.cJob, m_SelectCharacterData.nMapID, m_nTutorialMapIndex, m_cTutorialGateNo);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
		g_pExtManager->GetCreateDefaultMapInfo(SelectCharData()->cJob, SelectCharData()->nMapID, m_nTutorialMapIndex, m_cTutorialGateNo);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	}
#endif // #if defined( PRE_ADD_DWC )

	SendChannelList();

	m_cUserState = STATE_CHANNELLIST;

	return ERROR_NONE;
}

int CDNUserConnection::RequestCreateCharacter(CSCreateChar * pCreate, int nLen)
{
	if (sizeof(CSCreateChar) != nLen)
		return ERROR_INVALIDPACKET;

	if ((m_cUserState != STATE_CHARLIST)/* && (m_cUserState != STATE_CREATEDCHAR)*/){
		DetachConnection(L"RequestCreateCharacter State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateCharacter State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}
	if (m_nAccountDBID == 0){
		SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_CREATE_CHARACTER_FAIL);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] CreateChar Fail (%s)\r\n", m_nAccountDBID, m_nSessionID, pCreate->wszCharacterName);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if (m_cDailyCreateCount == 0){	// 더이상 만들 수 없다
		SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_CANNOT_CREATE_CHARACTER_TODAY);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] CreateChar Fail (%s)\r\n", m_nAccountDBID, m_nSessionID, pCreate->wszCharacterName);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

#if defined( _CH ) || defined( _TW )
#else
	//최대 이름 길이 초과
	int nMaxLength = g_pExtManager->GetGlobalWeightValue(CHARACTERNAME_MAXLENGTH);
	if((nMaxLength > 0) && ((int)(wcslen(pCreate->wszCharacterName)) > nMaxLength))
	{
		QueryAddRestraint(L"CheckAbuserCharLength");
		return ERROR_GENERIC_UNKNOWNERROR;
	}
#endif

	// #52058 최소 이름 길이 미만
	if( (int)(wcslen(pCreate->wszCharacterName)) < CHARNAMEMIN )
	{
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	DWORD dwCheckType = ALLOW_STRING_DEFAULT;
#if defined (_US)
	dwCheckType = ALLOW_STRING_CHARACTERNAME_ENG;
#endif
	if (!g_CountryUnicodeSet.Check(pCreate->wszCharacterName, dwCheckType)) {
		SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_PROHIBITWORD);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if (CheckLastSpace(pCreate->wszCharacterName))
	{
		SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_PROHIBITWORD);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

#if defined(PRE_ADD_MULTILANGUAGE)
	if (g_pExtManager->CheckProhibitWord(m_eSelectedLanguage, pCreate->wszCharacterName)){	// 금칙어 리스트에 있는 경우
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	if (g_pExtManager->CheckProhibitWord(pCreate->wszCharacterName)){	// 금칙어 리스트에 있는 경우
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_PROHIBITWORD);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

#if defined(PRE_MOD_SELECT_CHAR)
	if (GetEmptyCharacterSlotCount() <= 0){
		SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_CANNOT_CREATE_CHARACTER_ANYMORE);	// retcode 정의해야함
		return ERROR_GENERIC_UNKNOWNERROR;
	}

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	char cCharIndex = GetEmptyCharIndex();
	if (cCharIndex < 0){
		SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_CANNOT_CREATE_CHARACTER_ANYMORE);	// retcode 정의해야함
		return ERROR_GENERIC_UNKNOWNERROR;
	}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	for (int i = 0; i < EQUIPMAX; i++){
		if (pCreate->nEquipArray[i] > 0){
			if (!g_pExtManager->IsDefaultParts(pCreate->cClass, i, pCreate->nEquipArray[i])){
				SendCreateChar(pCreate->wszCharacterName, ERROR_LOGIN_PARTS_NOT_FOUND);	// retcode 정의해야함
				return ERROR_GENERIC_UNKNOWNERROR;
			}
		}
	}
	if( g_pExtManager->IsDefaultHairColor( pCreate->cClass, pCreate->dwHairColor ) == false ) {
		SendCreateChar( pCreate->wszCharacterName, ERROR_LOGIN_HAIRCOLOR_NOT_FOUND);
		return ERROR_GENERIC_UNKNOWNERROR;
	}
	if( g_pExtManager->IsDefaultSkinColor( pCreate->cClass, pCreate->dwSkinColor ) == false ) {
		SendCreateChar( pCreate->wszCharacterName, ERROR_LOGIN_SKINCOLOR_NOT_FOUND);
		return ERROR_GENERIC_UNKNOWNERROR;
	}
	if( g_pExtManager->IsDefaultEyeColor( pCreate->cClass, pCreate->dwEyeColor ) == false ) {
		SendCreateChar( pCreate->wszCharacterName, ERROR_LOGIN_EYECOLOR_NOT_FOUND);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	TCreateCharacterItem CreateItemArray[DEFAULTITEMMAX] = { 0, };
	int nCount = g_pExtManager->GetCreateDefaultItem(pCreate->cClass, CreateItemArray);
	
	TPosition Position = { 0, };
	g_pExtManager->GetCreateDefaultPosition(pCreate->cClass, Position.nX, Position.nZ);

	int nSkillArray[DEFAULTSKILLMAX] = { 0, };
	g_pExtManager->GetCreateDefaultSkill(pCreate->cClass, nSkillArray);

	int nUnlockSkillArray[DEFAULTUNLOCKSKILLMAX] = { 0, };
	g_pExtManager->GetCreateDefaultUnlockSkill(pCreate->cClass, nUnlockSkillArray);

	TQuickSlot QuickSlotArray[DEFAULTQUICKSLOTMAX] = { 0, };
	g_pExtManager->GetCreateDefaultQuickSlot(pCreate->cClass, QuickSlotArray);

	int nGestureArr[DEFAULTGESTUREMAX] = { 0, };
	g_pExtManager->GetCreateDefaultGesture(pCreate->cClass, nGestureArr);

#if defined(PRE_MOD_SELECT_CHAR)
	if( pCreate->cWorldID <= 0 )
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if( m_cWorldID <= 0 )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	{
		DetachConnection(L"RequestCreateCharacter WorldID Mismatch");
#if defined(PRE_MOD_SELECT_CHAR)
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateCharacter WorldID Mismatch(worldid:%d)\r\n", m_nAccountDBID, m_nSessionID, static_cast<int>(pCreate->cWorldID) );
#else	// #if defined(PRE_MOD_SELECT_CHAR)
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateCharacter WorldID Mismatch(worldid:%d)\r\n", m_nAccountDBID, m_nSessionID, static_cast<int>(m_cWorldID) );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
		return ERROR_NONE;
	}

	INT64 biCharacterDBID = 0;
	TIMESTAMP_STRUCT tCreateDate = {0,};
	int nRet = 0;
#if defined(PRE_MOD_SELECT_CHAR)
#if defined(PRE_ADD_DWC)
	nRet = g_pSQLMembershipManager->QueryAddCharacter(m_nAccountDBID, pCreate->wszCharacterName, pCreate->cWorldID, m_nDefaultCreateCharacterSlotPerAccount + (IsExistDWCCharacter() ? 1 : 0), biCharacterDBID, tCreateDate);
#else		//#if defined(PRE_ADD_DWC)
	nRet = g_pSQLMembershipManager->QueryAddCharacter(m_nAccountDBID, pCreate->wszCharacterName, pCreate->cWorldID, m_nDefaultCreateCharacterSlotPerAccount, biCharacterDBID, tCreateDate);
#endif		//#if defined(PRE_ADD_DWC)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	nRet = g_pSQLMembershipManager->QueryAddCharacter(m_nAccountDBID, pCreate->wszCharacterName, m_cWorldID, 0, biCharacterDBID, tCreateDate);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	if (nRet == ERROR_NONE){
		int nPCBangRebirthCoin = 0;
		nPCBangRebirthCoin = g_pExtManager->GetPCBangParam1(PCBang::Type::RebirthCoin, m_cPCBangGrade);

		UINT nBeginnerGuildDBID = 0;
#if defined(PRE_MOD_SELECT_CHAR)

		int nWorldSetID = g_pExtManager->GetWorldSetID(pCreate->cWorldID);
#ifdef PRE_ADD_BEGINNERGUILD
		nRet = g_pSQLWorldManager->QueryAddCharacter(nWorldSetID, biCharacterDBID, m_nAccountDBID, m_wszAccountName, m_cAccountLevel, pCreate->cWorldID, pCreate->wszCharacterName, pCreate->cClass, 0, pCreate->dwHairColor, pCreate->dwEyeColor, 
			pCreate->dwSkinColor, 0, Position, 0, g_pExtManager->GetRebirthCoin(1, nWorldSetID), nPCBangRebirthCoin, pCreate->nEquipArray, nSkillArray, nUnlockSkillArray, QuickSlotArray, CreateItemArray, nCount, true, nBeginnerGuildDBID, GetwszIp() );
#else		//#ifdef PRE_ADD_BEGINNERGUILD
		nRet = g_pSQLWorldManager->QueryAddCharacter(nWorldSetID, biCharacterDBID, m_nAccountDBID, m_wszAccountName, m_cAccountLevel, pCreate->cWorldID, pCreate->wszCharacterName, pCreate->cClass, 0, pCreate->dwHairColor, pCreate->dwEyeColor, 
			pCreate->dwSkinColor, 0, Position, 0, g_pExtManager->GetRebirthCoin(1, nWorldSetID), nPCBangRebirthCoin, pCreate->nEquipArray, nSkillArray, nUnlockSkillArray, QuickSlotArray, CreateItemArray, nCount, false, nBeginnerGuildDBID, GetwszIp() );
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#else	// #if defined(PRE_MOD_SELECT_CHAR)

#ifdef PRE_ADD_BEGINNERGUILD
		nRet = g_pSQLWorldManager->QueryAddCharacter(m_nWorldSetID, biCharacterDBID, m_nAccountDBID, m_wszAccountName, m_cAccountLevel, m_cWorldID, pCreate->wszCharacterName, pCreate->cClass, cCharIndex, pCreate->dwHairColor, pCreate->dwEyeColor, 
			pCreate->dwSkinColor, 0, Position, 0, g_pExtManager->GetRebirthCoin(1, m_nWorldSetID), nPCBangRebirthCoin, pCreate->nEquipArray, nSkillArray, nUnlockSkillArray, QuickSlotArray, CreateItemArray, nCount, true, nBeginnerGuildDBID, GetwszIp() );
#else		//#ifdef PRE_ADD_BEGINNERGUILD
		nRet = g_pSQLWorldManager->QueryAddCharacter(m_nWorldSetID, biCharacterDBID, m_nAccountDBID, m_wszAccountName, m_cAccountLevel, m_cWorldID, pCreate->wszCharacterName, pCreate->cClass, cCharIndex, pCreate->dwHairColor, pCreate->dwEyeColor, 
			pCreate->dwSkinColor, 0, Position, 0, g_pExtManager->GetRebirthCoin(1, m_nWorldSetID), nPCBangRebirthCoin, pCreate->nEquipArray, nSkillArray, nUnlockSkillArray, QuickSlotArray, CreateItemArray, nCount, false, nBeginnerGuildDBID, GetwszIp() );
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#endif	// #if defined(PRE_MOD_SELECT_CHAR)

		if (nRet == ERROR_NONE){
			DelDailyCreateCount();
			m_cUserState = STATE_SERVERLIST;

#ifdef PRE_ADD_BEGINNERGUILD
			MAAddGuildMember AddGuild;
			memset(&AddGuild, 0, sizeof(MAAddGuildMember));

			AddGuild.nManagedID = g_Config.nManagedID;
			AddGuild.GuildUID.Set(m_nWorldSetID, nBeginnerGuildDBID);
			AddGuild.cLevel = 1;
			time(&AddGuild.JoinDate);
			AddGuild.Location.Reset();
			AddGuild.Location.cServerLocation = static_cast<BYTE>(_LOCATION_LOGIN);
			AddGuild.nAccountDBID = m_nAccountDBID;
			AddGuild.nCharacterDBID = biCharacterDBID;
			AddGuild.nJob = pCreate->cClass;
			_wcscpy(AddGuild.wszCharacterName, NAMELENMAX, pCreate->wszCharacterName, NAMELENMAX);
#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif
			CDNMasterConnection *pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(m_nWorldSetID, false));
			if (pMasterCon)
				pMasterCon->SendGuildMemberAdd(&AddGuild);

#if defined(PRE_MOD_SELECT_CHAR)
			int nWillSendRet = g_pSQLWorldManager->QueryAddWillSendMail(nWorldSetID, biCharacterDBID, g_pExtManager->GetGlobalWeightValue(BeginnerGuild_GiftMailID), L"");
#else	// #if defined(PRE_MOD_SELECT_CHAR)
			int nWillSendRet = g_pSQLWorldManager->QueryAddWillSendMail(m_nWorldSetID, biCharacterDBID, g_pExtManager->GetGlobalWeightValue(BeginnerGuild_GiftMailID), L"");
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
			if (nWillSendRet != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] QueryAddWillSendMail Error(ret:%d)\r\n", m_nAccountDBID, m_nSessionID, nWillSendRet);
			
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#if defined(_KRAZ) && defined(_FINAL_BUILD)
			g_pSQLActozCommonManager->QueryAddCharacterInfo(biCharacterDBID, pCreate->cWorldID, pCreate->wszCharacterName, m_nAccountDBID, m_wszAccountName, pCreate->cClass, tCreateDate, GetIp());
#endif	// #if defined(_KRAZ)
		}
		else{
#if defined(PRE_MOD_SELECT_CHAR)
			g_pSQLWorldManager->QueryRollbackAddCharacter(nWorldSetID, biCharacterDBID);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
			g_pSQLWorldManager->QueryRollbackAddCharacter(m_nWorldSetID, biCharacterDBID);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
			g_pSQLMembershipManager->QueryRollbackAddCharacter(biCharacterDBID);
			g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] QueryAddCharacter Error(ret:%d)\r\n", m_nAccountDBID, m_nSessionID, nRet );
		}
	}

	SendCreateChar(pCreate->wszCharacterName, nRet);

	return ERROR_NONE;
}

int CDNUserConnection::RequestDeleteCharacter(CSDeleteChar * pDelete, int nLen)
{
	if (sizeof(CSDeleteChar) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHARLIST){
		DetachConnection(L"RequestDeleteCharacter State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestDeleteCharacter State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

#if defined(PRE_MOD_SELECT_CHAR)
	if (!IsValidCharacter(pDelete->biCharacterDBID))
	{
		SendDeleteChar(pDelete->biCharacterDBID, ERROR_LOGIN_CHARACTER_INDEX_UNMATCH);	// retcode 정의해야함
		return ERROR_GENERIC_UNKNOWNERROR;
	}
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if (!IsValidCharacterIndex(pDelete->cCharIndex))
	{
		SendDeleteChar(pDelete->cCharIndex, ERROR_LOGIN_CHARACTER_INDEX_UNMATCH);	// retcode 정의해야함
		return ERROR_GENERIC_UNKNOWNERROR;
	}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	BYTE cFailCount = 0;
	std::wstring wszSecondAuthPW = MakeSecondPasswordString( szVersion, pDelete->nSeed, pDelete->nValue );
	int iRet = QueryValidataSecondAuthPassphrase( (WCHAR*)wszSecondAuthPW.c_str(), cFailCount );

	if( iRet != ERROR_NONE )
	{
#if defined(PRE_MOD_SELECT_CHAR)
		SendDeleteChar(pDelete->biCharacterDBID, iRet, cFailCount );
#else	// #if defined(PRE_MOD_SELECT_CHAR)
		SendDeleteChar(pDelete->cCharIndex, iRet, cFailCount );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
		return ERROR_NONE;
	}

	bool bFirstVillage = true;
	TIMESTAMP_STRUCT DeleteDate;
	memset( &DeleteDate, 0, sizeof(DeleteDate) );
#if defined(PRE_MOD_SELECT_CHAR)
	int nWorldSetID = GetWorldSetIDByCharacterDBID(pDelete->biCharacterDBID);
	int nRet = g_pSQLWorldManager->QueryDelCharacter(nWorldSetID, pDelete->biCharacterDBID, bFirstVillage, DeleteDate );
	if (nRet == ERROR_NONE){
		g_pSQLMembershipManager->QueryDelCharacter(pDelete->biCharacterDBID, m_wszVirtualIp, m_wszIP, bFirstVillage, DeleteDate );
		DelCharacterData(pDelete->biCharacterDBID);
		m_cUserState = STATE_SERVERLIST;

#if defined(_KRAZ) && defined(_FINAL_BUILD)
		g_pSQLActozCommonManager->QueryUpdateCharacterStatus(pDelete->biCharacterDBID, ActozCommon::CharacterStatus::Delete);
#endif	// #if defined(_KRAZ)
	}

	SendDeleteChar(pDelete->biCharacterDBID, nRet);

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	int nRet = g_pSQLWorldManager->QueryDelCharacter(m_nWorldSetID, GetCharacterData(pDelete->cCharIndex)->biCharacterDBID, bFirstVillage, DeleteDate );
	if (nRet == ERROR_NONE){
		g_pSQLMembershipManager->QueryDelCharacter(GetCharacterData(pDelete->cCharIndex)->biCharacterDBID, m_wszVirtualIp, m_wszIP, bFirstVillage, DeleteDate );
		DelCharData(pDelete->cCharIndex);
		m_cUserState = STATE_SERVERLIST;
	}

	SendDeleteChar(pDelete->cCharIndex, nRet);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	return ERROR_NONE;
}

int CDNUserConnection::RequestReviveCharacter( CSReviveChar* pRevive, int nLen )
{
	if (sizeof(CSReviveChar) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHARLIST)
	{
		DetachConnection(L"RequestReviveCharacter State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestReviveCharacter State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

#if defined(PRE_MOD_SELECT_CHAR)
	if (IsValidCharacter(pRevive->biCharacterDBID) == false)
	{
		SendReviveChar( pRevive->biCharacterDBID, ERROR_LOGIN_CHARACTER_INDEX_UNMATCH );
		return ERROR_NONE;
	}

	int nWorldSetID = GetWorldSetIDByCharacterDBID(pRevive->biCharacterDBID);

	// World
	int nRet = g_pSQLWorldManager->QueryReviveCharacter( nWorldSetID, pRevive->biCharacterDBID );
	if( nRet != ERROR_NONE )
	{
		SendReviveChar( pRevive->biCharacterDBID, nRet );
		return ERROR_NONE;
	}

	// Membership
	nRet = g_pSQLMembershipManager->QueryReviveCharacter( pRevive->biCharacterDBID, m_wszVirtualIp, m_wszIP );
	if( nRet == ERROR_NONE ){
		m_cUserState = STATE_SERVERLIST;

#if defined(_KRAZ) && defined(_FINAL_BUILD)
		g_pSQLActozCommonManager->QueryUpdateCharacterStatus(pRevive->biCharacterDBID, ActozCommon::CharacterStatus::Revive);
#endif	// #if defined(_KRAZ)
	}
	SendReviveChar( pRevive->biCharacterDBID, nRet );

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if (IsValidCharacterIndex(pRevive->cCharIndex) == false)
	{
		SendDeleteChar(pRevive->cCharIndex, ERROR_LOGIN_CHARACTER_INDEX_UNMATCH);	// retcode 정의해야함
		return ERROR_NONE;
	}

	// World
	int nRet = g_pSQLWorldManager->QueryReviveCharacter( m_nWorldSetID, GetCharacterData(pRevive->cCharIndex)->biCharacterDBID );
	if( nRet != ERROR_NONE )
	{
		SendReviveChar( pRevive->cCharIndex, nRet );
		return ERROR_NONE;
	}

	// Membership
	nRet = g_pSQLMembershipManager->QueryReviveCharacter( GetCharacterData(pRevive->cCharIndex)->biCharacterDBID, m_wszVirtualIp, m_wszIP );
	if( nRet == ERROR_NONE ){
		m_cUserState = STATE_SERVERLIST;
	}
	SendReviveChar( pRevive->cCharIndex, nRet );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	return ERROR_NONE;
}

int CDNUserConnection::RequestSelectChannel(CSSelectChannel *pSelectChannel, int nLen)
{
	if (sizeof(CSSelectChannel) != nLen)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHANNELLIST){
		DetachConnection(L"RequestSelectChannel State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestSelectChannel State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

#if defined( PRE_ADD_DWC )
	if( GetSelectCharacterAccountLevel() == AccountLevel_DWC )
	{
		if(!g_pDWCChannelManager->CheckValidDate())
		{
			SendVillageInfo(ERROR_LOGIN_DWC_WRONG_DATE, 0, 0);
			return ERROR_NONE;
		}
	}
#endif // #if defined( PRE_ADD_DWC )

#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif
	CDNMasterConnection *pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(m_nWorldSetID, false));
	if (!pMasterCon)
	{
		if (GetSelectCharacterLastMapIndex() == 0){
			SendGameInfo(0, 0, 0, ERROR_GENERIC_MASTERCON_NOT_FOUND, 0, 0);
		}
		else {
			SendVillageInfo(ERROR_GENERIC_MASTERCON_NOT_FOUND, 0, 0);
		}
		g_Log.Log(LogType::_ERROR, this, L"[RequestSelectChannel] MasterCon NULL!!\r\n");
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	m_nChannelID = pSelectChannel->nChannelID;
	TChannelInfoEx *pChannel = GetChannelInfo(m_nChannelID);
	if (!pChannel){
		DetachConnection(L"ChannelData NULL");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] pChannel mismatch(%d)\r\n", m_nAccountDBID, m_nSessionID, m_nChannelID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if ( GetAccountLevel() == 0 && pChannel->Channel.nCurrentUser >= pChannel->Channel.nChannelMaxUser)
	{
		//채널인원이상 들어온경우 (실시간동기를 맞추고 있지 않으므로 나올 수 있음) 다른 채널로 보내버린다.
		pChannel = GetOtherChannelInfo(m_nChannelID);
		if (pChannel)
		{
			//일단은~ 요것만 다시 갱신해주심 뎁니다.
			m_nChannelID = pChannel->Channel.nChannelID;
		}
		else
		{
			//얻어진 채널이 없으면 지금 진짜 다 꽉찬거다 대기자 만들어지기 전까지는 접속을 받지 않고 짤라버림!
			DetachConnection(L"ChannelData NULL");
			g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] pChannel Congestion(%d)\r\n", m_nAccountDBID, m_nSessionID, m_nChannelID);
			return ERROR_GENERIC_UNKNOWNERROR;
		}
	}
	
	m_cVillageID = pChannel->cVillageID;

#if defined(PRE_MOD_SELECT_CHAR)
	pMasterCon->SendAddUser(this);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	pMasterCon->SendUserInfo(this, m_nChannelID, m_bAdult, GetVirtualIp(), GetIp(), (GetSelectCharacterLastMapIndex() == 0) ? true : false);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	return ERROR_NONE;
}

int CDNUserConnection::RequestBackButton(int nLen)
{
	if (nLen != 0)
		return ERROR_INVALIDPACKET;

	switch(m_cUserState)
	{
	case STATE_SERVERLIST:
		{
			if (IsCertified()) {	// 주의 !!! - 반드시 QUERY_BEGINAUTH, QUERY_CHECKAUTH 를 통과하여 인증을 받은 사용자에 한해서만 인증정보 리셋이 수행되어야 함 !!!
#if defined(_TW) && defined(_FINAL_BUILD)
				if( m_nAccountDBID != 0 && wcslen(GetAccountName()) != 0)
				{
					USES_CONVERSION;
					ScopeLock<CSyncLock> Lock(m_SendLogOutLock);
					if( !m_bSendLogOut )
					{
						int iResult = g_pGamaniaAuthLogOut->SendLogout(W2A(GetAccountName()), GetIp());
						if (0 >= iResult) {
							// 오류
						}
						m_bSendLogOut = true;
						g_Log.Log(LogType::_FILELOG, L"[GASH] LogOut RequestBackButton %s, %d\r\n", GetAccountName(), GetSessionID());
					}
				}
#endif	//#if defined(_TW) 

				{
					int iResult = g_pAuthManager->QueryResetAuth(m_nWorldSetID, m_nAccountDBID, m_nSessionID);

					// 인증정보가 초기화된 세션의 이후 작업 처리
					if (iResult != ERROR_NONE)
					{
						Clear();
						m_cUserState = STATE_CHECKVERSION;
						g_Log.Log(LogType::_ERROR, this, L"[QueryResetAuth Fail] LoginServer %d:%d,%u,%u \r\n", iResult, m_nWorldSetID, m_nAccountDBID, m_nSessionID);

						DetachConnection(L"QueryResetAuth Fail");
						break;
					}
				}

				g_pSQLMembershipManager->QueryLogout(m_nAccountDBID, m_nSessionID, m_szMID);
				m_bCertified = false;
			}

			SendBackButtonLogin();
			Clear();
			m_cUserState = STATE_CHECKVERSION;
			m_bReconnected = false;

			g_pUserConnectionManager->DelConnection(this);	// 일단 먼저 지우고
			UINT nNewSessionID = g_IDGenerator.GetUserConnectionID();
			SetSessionID(nNewSessionID);	// 새로 발급

			if (g_pUserConnectionManager->PushConnection(this) == ERROR_GENERIC_DUPLICATESESSIONID){	// 다시 푸쉬~ 
				SendReconnectLogin(ERROR_GENERIC_UNKNOWNERROR, 0, 0);
				DetachConnection(L"Reconnect SessonID Duplicate");
				g_Log.Log(LogType::_ERROR, this, L"[SID:%u] RequestBackButton SessionID Duplicate\r\n", nNewSessionID);	
			}
		}
		break;

	case STATE_CHARLIST:
		{
			SendDelUser();
			
			m_cUserState = STATE_CHECKLOGIN;
			m_cWorldID = 0;
			m_nWorldSetID = 0;
		}
		break;

	case STATE_CHANNELLIST:
		{
			m_cUserState = STATE_CHARLIST;
			SendCharList(ERROR_NONE);
#if defined( PRE_ADD_DWC )
			SendDWCChannelInfo();
#endif // #if defined( PRE_ADD_DWC )
		}
		break;

	case STATE_WAITUSER:
		{
#if defined(PRE_MOD_SELECT_CHAR)
			SendDelUser();

			m_cUserState = STATE_CHARLIST;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
		}
		break;

	default:
		m_cUserState -= 1;
	}

	return ERROR_NONE;
}

int CDNUserConnection::RequestReconnectLogin(CSReconnectLogin *pReconnect, int nLen)
{
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 0;
#endif
	if (sizeof(CSReconnectLogin) != nLen)
		return ERROR_INVALIDPACKET;

#if defined( PRE_LOGINDUMP_LOG )
	m_ReconnectData = *pReconnect;
#endif
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 1;
#endif
	g_pUserConnectionManager->DelConnection(this);	// 일단 리스트에서 없앤다 (새로 부여받은 SessionID이기 때문에)
	SetSessionID(pReconnect->nSessionID);	// 기존 sessionid로 세팅하고
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 2;
#endif
	if (g_pUserConnectionManager->PushConnection(this) == ERROR_GENERIC_DUPLICATESESSIONID){	// 다시 푸쉬~ 
		SendReconnectLogin(ERROR_GENERIC_UNKNOWNERROR, 0, 0);
		DetachConnection(L"Reconnect SessonID Duplicate");
		g_Log.Log(LogType::_ERROR, this, L"[SID:%u] ReconnectLogin SessionID Duplicate\r\n", pReconnect->nSessionID);
		return ERROR_NONE;
	}

	m_bReconnected = true;

	m_nAccountDBID = pReconnect->nAccountDBID;
	m_biCertifyingKey = pReconnect->biCertifyingKey;

	BYTE cAdult = 0;
	UINT nSessionID = 0;
	INT64 biCharacterDBID = 0;
	WCHAR wszCharName[NAMELENMAX] = {0,};
	char cWorldSetID = 0;
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 3;
#endif
	int nRet = g_pSQLMembershipManager->QueryCheckAuth(pReconnect->nAccountDBID, pReconnect->biCertifyingKey, g_pAuthManager->GetServerID(), cWorldSetID, nSessionID, biCharacterDBID, static_cast<LPWSTR*>(static_cast<LPVOID>(&m_wszAccountName)), static_cast<LPWSTR*>(static_cast<LPVOID>(&wszCharName)), m_cAccountLevel, cAdult, m_cAge, m_cDailyCreateCount, m_nPrmInt1, m_cLastServerType);
	m_nWorldSetID = cWorldSetID;
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 4;
#endif

	g_Log.Log((0 > nRet)?(LogType::_ERROR):(LogType::_NORMAL), this, L"[Auth] CheckAuth (REQ) - Result:%d, nAccountDBID:%d, CertifyingKey:%I64d, CerServerID:%d\r\n", nRet, pReconnect->nAccountDBID, pReconnect->biCertifyingKey, g_pAuthManager->GetServerID());

	if (nRet != ERROR_NONE){
		DetachConnection(L"ReconnectLogin CheckAuth Failed");
		return ERROR_NONE;
	}

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 5;
#endif
	int nWorldID = 0;
	g_pSQLWorldManager->QueryGetCharacterPartialy8(m_nWorldSetID, biCharacterDBID, NULL, nWorldID);
	m_cWorldID = nWorldID;
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 6;
#endif

#if !defined(PRE_MOD_SELECT_CHAR)
	m_cCharCountMax = CreateCharacterDefaultCountMax;
	g_pSQLMembershipManager->QueryGetCharacterSlotCount(m_nAccountDBID, m_cWorldID, 0, m_cCharCountMax);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	// 다른 로그인으로 들어오는 애들을 위해 WorldID를 0으로 셋팅한다.
	g_pSQLMembershipManager->QuerySetWorldID(0, pReconnect->nAccountDBID);
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 7;
#endif
#if defined(PRE_CHARLIST_SORTING)
	g_pSQLMembershipManager->QueryGetCharacterSortCode(m_nAccountDBID, m_cCharacterSortCode);
#endif	// #if defined(PRE_CHARLIST_SORTING)
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 8;
#endif

	m_bCertified = true;
	m_bAdult = cAdult ? true : false;

#if defined(_KR)
	m_nNexonSN = (UINT)m_nPrmInt1;

	memcpy(m_szMID, pReconnect->szMID, sizeof(m_szMID));
	m_dwGRC = pReconnect->dwGRC;
#endif	// _KR
#if defined(_ID)
	strncpy(m_szVirtualIp, pReconnect->szVirtualIp, IPLENMAX);
	MultiByteToWideChar(CP_ACP, 0, m_szVirtualIp, -1, m_wszVirtualIp, IPLENMAX);
	_strcpy(m_szMacAddress, _countof(m_szMacAddress), pReconnect->szMacAddress, (int)strlen(pReconnect->szMacAddress));				
	_strcpy(m_szKey, _countof(m_szKey), pReconnect->szKey, (int)strlen(pReconnect->szKey));				
	m_dwKreonCN = (DWORD)m_nPrmInt1;
#endif
#if defined(_KRAZ)
	if (m_nPrmInt1 > 0){
		if (m_nPrmInt1 == 1){
			m_ShutdownData.cType = Actoz::ShutdownType::Force;
		}
		else{
			char buffer[65] = {0,};
			_itoa_s(m_nPrmInt1, buffer, 65, 10);
			std::string strTemp = buffer;

			std::string strValue = strTemp.substr(0, 4);
			m_ShutdownData.nYear = atoi(strValue.c_str());
			strValue = strTemp.substr(4, 2);
			m_ShutdownData.nMonth = atoi(strValue.c_str());
			strValue = strTemp.substr(6, 2);
			m_ShutdownData.nDay = atoi(strValue.c_str());
			strValue = strTemp.substr(8, 2);
			m_ShutdownData.nHour = atoi(strValue.c_str());

			m_ShutdownData.cType = Actoz::ShutdownType::Selective;
		}
	}
#endif	// #if defined(_KRAZ)	

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 9;
#endif	
	// pcbang체크
	CheckPCBangIp();

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 10;
#endif

#if defined(_TW)
	// 국가 코드 얻어오기
	g_pSQLMembershipManager->QueryGetNationalityCode(m_nAccountDBID, m_cRegion);
#endif

	m_cUserState = STATE_RECONNECTLOGIN;	// 로그인 재접속 상태로 변경

#if defined(PRE_MOD_SELECT_CHAR)
	g_pMasterConnectionManager->SendDetachUserConnectionByWorldSetID(m_nWorldSetID, m_nAccountDBID, true, false, 0);	// 20090908	// village, game에 남아있을 수도있으므로 접속 끊어버리라고 날려준다.
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	m_bMasterAddUser = true;		//리컨넥으로 들어올 경우 마스터에 애드되어 있는 상태

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	g_pMasterConnectionManager->SendDetachUserConnectionByWorldSetID(m_nWorldSetID, m_nAccountDBID, m_cAccountLevel, true, m_szMID, m_dwGRC, false, 0, GetIp());	// 20090908	// village, game에 남아있을 수도있으므로 접속 끊어버리라고 날려준다.
#else		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	g_pMasterConnectionManager->SendDetachUserConnectionByWorldSetID(m_nWorldSetID, m_nAccountDBID, m_cAccountLevel, true, m_szMID, m_dwGRC);	// 20090908	// village, game에 남아있을 수도있으므로 접속 끊어버리라고 날려준다.
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	SendReconnectLogin(ERROR_NONE, m_nAccountDBID, m_biCertifyingKey);

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 11;
#endif

	SendCharacterList(true);
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 24;
#endif

#if defined(_HSHIELD)
	if (m_cAccountLevel != AccountLevel_Developer){	// 개발자가 아니라면
		SendMakeRequest();	// CRC 요청
	}

#elif defined(_GPK)
	SendGPKCode();
	SendGPKAuthData();
	SendGPKData();
	m_dwCheckGPKTick = timeGetTime();
#endif	// _HSHIELD

	return ERROR_NONE;
}

int CDNUserConnection::RequestChannelList(int nLen)
{
	if (nLen != 0)
		return ERROR_INVALIDPACKET;

	if (m_cUserState != STATE_CHANNELLIST)
	{
		DetachConnection(L"STATE_CHANNELLIST mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] STATE_CHANNELLIST mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

	SendChannelList();
	return ERROR_NONE;
}

int CDNUserConnection::RequestUpdateSecondAuthPassword( const char* pData, int nLen )
{
	const CSGameOptionUpdateSecondAuthPassword* pPacket = reinterpret_cast<const CSGameOptionUpdateSecondAuthPassword*>(pData);

	BYTE cFailCount = 0;
	std::wstring wszOldPW = ( m_bIsSetSecondAuthPW ) ? MakeSecondPasswordString( szVersion, pPacket->nOldSeed, pPacket->nOldValue ) : L"";
	std::wstring wszNewPW = MakeSecondPasswordString( szVersion, pPacket->nNewSeed, pPacket->nNewValue );
	int iVaildateRet = QueryValidataSecondAuthPassphrase( (WCHAR*)wszOldPW.c_str(), cFailCount);
	if( iVaildateRet != ERROR_NONE )
	{
		// 2차 인증 비밀번호 변경 결과 통보
		SCGameOptionUpdateSecondAuthPassword TxPacket;
		memset( &TxPacket, 0, sizeof(TxPacket) );

		TxPacket.iRet = iVaildateRet;
		TxPacket.cFailCount	= cFailCount;

		AddSendData( SC_GAMEOPTION, eGameOption::SC_UPDATE_SECONDAUTH_PASSWORD, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );

		return ERROR_NONE;
	}

	int iRet = g_pSQLMembershipManager->QueryModSecondAuthPassphrase( m_nAccountDBID, wszOldPW.c_str(), wszNewPW.c_str(), cFailCount );

	// 2차 인증 비밀번호 변경 결과 통보
	SCGameOptionUpdateSecondAuthPassword TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iRet = iRet;
	TxPacket.cFailCount	= cFailCount;

	AddSendData( SC_GAMEOPTION, eGameOption::SC_UPDATE_SECONDAUTH_PASSWORD, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
	
	// 2차 인증 정보 갱신
	if( iRet == ERROR_NONE )
	{
		if( !m_bIsSetSecondAuthPW )
		{
			m_bIsSetSecondAuthPW = true;

			if( g_pSQLMembershipManager->QueryModSecondAuthLockFlag( m_nAccountDBID, true ) == ERROR_NONE )
				m_bIsSetSecondAuthLock = true;

#ifdef PRE_ADD_23829
			m_bPassSecondAuth = false;
#endif	//#ifdef PRE_ADD_23829

			SendSecondAuthInfo();
		}
	}

	return iRet;
}

int CDNUserConnection::RequestInitSecondAuth( const char* pData, int nLen )
{
	// 2차 비밀번호가 설정 안되었는데 초기화를 왜???
	if( !m_bIsSetSecondAuthPW )
	{
		SendResInitSecondAuth( ERROR_INVALIDPACKET );
		return ERROR_NONE;
	}

	const CSGameOptionInitSecondAuth* pPacket = reinterpret_cast<const CSGameOptionInitSecondAuth*>(pData);

	// 비밀번호 확인
	BYTE cFailCount = 0;
	std::wstring wszSecondAuthPW = MakeSecondPasswordString( szVersion, pPacket->nSeed, pPacket->nValue );
	int iRet = QueryValidataSecondAuthPassphrase( (WCHAR*)wszSecondAuthPW.c_str(), cFailCount );
	if( iRet != ERROR_NONE )
	{
		SendResInitSecondAuth( iRet, cFailCount );
		return ERROR_NONE;
	}

	// 2차인증 초기화
#ifdef PRE_ADD_23829
	iRet = InitSecondAuth();
	if (iRet == ERROR_NONE)
		SendSecondAuthInfo();
	else
		SendResInitSecondAuth( iRet );
#else
	iRet = g_pSQLMembershipManager->QueryInitSecondAuth(GetAccountDBID() );
	if( iRet == ERROR_NONE )
	{
		// 2차인증 정보 보내기
		m_bIsSetSecondAuthLock	= false;
		m_bIsSetSecondAuthPW	= false;
		SendSecondAuthInfo();
	}
	SendResInitSecondAuth( iRet );
#endif	
	return ERROR_NONE;
}

#ifdef PRE_ADD_DOORS
int CDNUserConnection::RequestGetDoorsAuthKey(const char * pData, int nLen)
{
	if (m_bMobileAuthentication)
		return ERROR_GENERIC_INVALIDREQUEST;

	char szAuthKey[Doors::Common::AuthenticationKey_LenMax];
	memset(szAuthKey, 0, sizeof(szAuthKey));

	bool bFlag = false;
	int nRetCode = g_pSQLMembershipManager->QueryGetAuthenticationFlag(GetAccountDBID(), bFlag);
	if (nRetCode == ERROR_NONE)
		m_bMobileAuthentication = bFlag;
	else if (nRetCode == 101115 || nRetCode != ERROR_NONE)
		m_bMobileAuthentication = false;
	
	nRetCode = g_pSQLMembershipManager->QueryGetDoorsAuthentication(GetAccountDBID(), szAuthKey);
	if (nRetCode != ERROR_NONE)
		_DANGER_POINT();
	
	SendMobileAuthenticationKey(szAuthKey, nRetCode, m_bMobileAuthentication);
	return ERROR_NONE;
}

int CDNUserConnection::RequestCancelDoorsAuth(const char * pData, int nLen)
{
	if (m_bMobileAuthentication == false)
		return ERROR_GENERIC_INVALIDREQUEST;

	int nWorldID = g_pSQLMembershipManager->QueryCancelDoorsAuthentication(GetAccountDBID());	
	//int nWorldRetCode = g_pSQLWorldManager->QueryCancelDoorsAuthentication(nWorldID, GetAccountDBID());
	//if (nWorldRetCode == ERROR_NONE)// && nWorldRetCode == ERROR_NONE)
	if (nWorldID == ERROR_NONE)// && nWorldRetCode == ERROR_NONE)
		m_bMobileAuthentication = false;
	///SendMobileCancelAuthentication(nWorldRetCode);
	SendMobileCancelAuthentication(nWorldID);
	return ERROR_NONE;
}

int CDNUserConnection::RequestGetAuthFlag(const char * pData, int nLen)
{
	bool bAuth = false;
	int nRet = g_pSQLMembershipManager->QueryGetAuthenticationFlag(GetAccountDBID(), bAuth);
	if (nRet == 0)
		m_bMobileAuthentication = bAuth;
	SendMobileAuthenticationFlag(m_bMobileAuthentication);
	return ERROR_NONE;
}
#endif		//#ifdef PRE_ADD_DOORS

int CDNUserConnection::RequestCharacterSortCode(CSCharacterSortCode *pSortCode, int nLen)
{
	if (sizeof(CSCharacterSortCode) != nLen)
		return ERROR_INVALIDPACKET;

	if (pSortCode->cCharacterSortCode <= Login::CharacterSortCode::None) return ERROR_GENERIC_INVALIDREQUEST;
	if (pSortCode->cCharacterSortCode >= Login::CharacterSortCode::Max) return ERROR_GENERIC_INVALIDREQUEST;

	m_cCharacterSortCode = pSortCode->cCharacterSortCode;
	g_pSQLMembershipManager->QueryModCharacterSortCode(m_nAccountDBID, m_cCharacterSortCode);

	return ERROR_NONE;
}

#if defined( PRE_ADD_DWC )
int CDNUserConnection::RequestCreateDWCCharacter(CSCreateDWCChar *pDWCCreate, int nLen)
{
	if (sizeof(CSCreateDWCChar) != nLen)
		return ERROR_INVALIDPACKET;

	if ((m_cUserState != STATE_CHARLIST)/* && (m_cUserState != STATE_CREATEDCHAR)*/){
		DetachConnection(L"RequestCreateDWCCharacter State Mismatch");
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateDWCCharacter State Mismatch(state:%d)\r\n", m_nAccountDBID, m_nSessionID, m_cUserState);
		return ERROR_NONE;
	}

	if( !g_pDWCChannelManager->CheckValidDate() )
	{
		SendCreateDWCChar(ERROR_LOGIN_DWC_WRONG_DATE);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateDWCCharacter Wrong Date\r\n", m_nAccountDBID, m_nSessionID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if( m_cDailyCreateCount == 0 ) // 더이상 만들 수 없다
	{	
		SendCreateDWCChar(ERROR_LOGIN_CANNOT_CREATE_CHARACTER_TODAY);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateDWCCharacter DailyCreateCount Zero\r\n", m_nAccountDBID, m_nSessionID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if( IsExistDWCCharacter() ) // DWC 캐릭터를 이미 가지고 있음
	{
		SendCreateDWCChar(ERROR_LOGIN_CANNOT_CREATE_DWCCHARACTER_EXIST);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateDWCCharacter Exist\r\n", m_nAccountDBID, m_nSessionID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	TDWCChannelInfo tDWCChannelInfo;
	g_pDWCChannelManager->GetChannelInfo( tDWCChannelInfo );
	if( !CheckDWCLimitCharacterLevel( tDWCChannelInfo.cLimitCharacterLevel ) ) // DWC 캐릭터를 만들기 위해 필요한 캐릭터 레벨을 보유하고 있지 않음 
	{
		SendCreateDWCChar(ERROR_LOGIN_CANNOT_CREATE_DWCCHARACTER_LIMITLEVEL);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateDWCCharacter Limit CharacterLevel\r\n", m_nAccountDBID, m_nSessionID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

#if defined( PRE_MOD_SELECT_CHAR )
	const TDBListCharData * pSelectData = GetCharacterData( pDWCCreate->biSelectCharacterDBID );
#else // #if defined( PRE_MOD_SELECT_CHAR )
	const TDBListCharData * pSelectData = GetCharacterData( pDWCCreate->cSelectCharIndex );
#endif // #if defined( PRE_MOD_SELECT_CHAR )
	if( !pSelectData )
	{
		SendCreateDWCChar(ERROR_LOGIN_CREATE_CHARACTER_FAIL);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateDWCCharacter Wrong Select Character\r\n", m_nAccountDBID, m_nSessionID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	// 테이블 참조
	TDWCCreateData * pDWCCreateData = g_pExtManager->GetDWCCreateData( pDWCCreate->cJobCode );
	if( !pDWCCreateData )
	{
		SendCreateDWCChar(ERROR_LOGIN_CREATE_CHARACTER_FAIL);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] RequestCreateDWCCharacter Wrong JobCode(%d)\r\n", m_nAccountDBID, m_nSessionID, pDWCCreate->cJobCode);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	WCHAR wszCharacterName[NAMELENMAX] = { L"" };
	wcscpy( wszCharacterName, DWCPREFIX );
	wcscat( wszCharacterName, pSelectData->wszCharacterName );

	if (m_nAccountDBID == 0){
		SendCreateDWCChar(ERROR_LOGIN_CREATE_CHARACTER_FAIL);
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] CreateDWCChar Fail\r\n", m_nAccountDBID, m_nSessionID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	char cCharIndex = 0;
#if defined(PRE_MOD_SELECT_CHAR)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	cCharIndex = m_cCharacterLimitCount;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	TCreateCharacterItem CreateItemArray[DEFAULTITEMMAX] = { 0, };
	int nCreateItemMax = g_pExtManager->GetCreateDWCItem(pDWCCreate->cJobCode, CreateItemArray);

	TPosition Position = { 0, };
	g_pExtManager->GetCreateDWCPosition(pDWCCreate->cJobCode, Position.nX, Position.nZ);

	int nEquipArray[EQUIPMAX] = { 0, };
	g_pExtManager->GetCreateDWCEquip(pDWCCreate->cJobCode, nEquipArray);

	int nSkillArray[DEFAULTSKILLMAX] = { 0, };
	g_pExtManager->GetCreateDWCSkill(pDWCCreate->cJobCode, nSkillArray);

	TQuickSlot QuickSlotArray[DEFAULTQUICKSLOTMAX] = { 0, };
	g_pExtManager->GetCreateDWCQuickSlot(pDWCCreate->cJobCode, QuickSlotArray);

	int nGestureArr[DEFAULTGESTUREMAX] = { 0, };
	g_pExtManager->GetCreateDWCGesture(pDWCCreate->cJobCode, nGestureArr);

	// 스킬 정보는 DWC캐릭터를 따로 구분 하지 않는다. 
	int nUnlockSkillArray[DEFAULTUNLOCKSKILLMAX] = { 0, };
	g_pExtManager->GetCreateDefaultUnlockSkill(pDWCCreateData->cClassID, nUnlockSkillArray);

	INT64 biCharacterDBID = 0;
	TIMESTAMP_STRUCT tCreateDate = {0,};
	int nRet = 0;
#if defined(PRE_MOD_SELECT_CHAR)
	nRet = g_pSQLMembershipManager->QueryAddCharacter(m_nAccountDBID, wszCharacterName, DWC::DWCWorldID, m_nDefaultCreateCharacterSlotPerAccount + 1, biCharacterDBID, tCreateDate);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	nRet = g_pSQLMembershipManager->QueryAddCharacter(m_nAccountDBID, wszCharacterName, DWC::DWCWorldID, 1, biCharacterDBID, tCreateDate);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	if (nRet == ERROR_NONE){
		int nRebirthCoin = g_pExtManager->GetRebirthCoin(pDWCCreateData->cLevel, GetDWCWorldSetID());
		int nPCBangRebirthCoin = g_pExtManager->GetPCBangParam1(PCBang::Type::RebirthCoin, m_cPCBangGrade);
		// 캐릭터 추가
		nRet = g_pSQLWorldManager->QueryAddDWCCharacter(GetDWCWorldSetID(), biCharacterDBID, m_nAccountDBID, m_wszAccountName, DWC::DWCWorldID, wszCharacterName, pDWCCreateData->cClassID, pDWCCreateData->cJobCode1, pDWCCreateData->cJobCode2, pDWCCreateData->cLevel, pDWCCreateData->nExp, cCharIndex, 
			pDWCCreateData->dwHairColor, pDWCCreateData->dwEyeColor, pDWCCreateData->dwSkinColor, DWC::DWCMapID, Position, 0, nRebirthCoin, nPCBangRebirthCoin, nEquipArray, nSkillArray, nUnlockSkillArray, QuickSlotArray, CreateItemArray, nCreateItemMax, GetwszIp(), pDWCCreateData->nSkillPoint, pDWCCreateData->nGold );

		if (nRet == ERROR_NONE)
		{
			DelDailyCreateCount();
			m_cUserState = STATE_SERVERLIST;

#if defined(_KRAZ) && defined(_FINAL_BUILD)
			g_pSQLActozCommonManager->QueryAddCharacterInfo(biCharacterDBID, DWC::DWCWorldID, wszCharacterName, m_nAccountDBID, m_wszAccountName, pDWCCreate->cJobCode, tCreateDate, GetIp());
#endif	// #if defined(_KRAZ)
		}
		else
		{
			g_pSQLWorldManager->QueryRollbackAddCharacter(GetDWCWorldSetID(), biCharacterDBID);
			g_pSQLMembershipManager->QueryRollbackAddCharacter(biCharacterDBID);
			g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] QueryAddDWCCharacter Error(ret:%d)\r\n", m_nAccountDBID, m_nSessionID, nRet );
		}
	}

	SendCreateDWCChar(nRet);

	return ERROR_NONE;
}
#endif // #if defined( PRE_ADD_DWC )

#ifdef PRE_ADD_23829
int CDNUserConnection::InitSecondAuth()
{
	// 2차인증 초기화
	int nRetCode = g_pSQLMembershipManager->QueryInitSecondAuth(GetAccountDBID() );
	if(nRetCode == ERROR_NONE)
	{
		// 2차인증 정보 보내기
		m_bIsSetSecondAuthLock	= false;
		m_bIsSetSecondAuthPW	= false;
	}
	return nRetCode;
}
#endif //#ifdef PRE_ADD_23829

#if defined(PRE_MOD_SELECT_CHAR)
TDBListCharData* CDNUserConnection::GetCharacterData(INT64 biCharacterDBID)
{
	if (biCharacterDBID <= 0) return NULL;
	if (m_MapCharacterList.empty()) return NULL;

	TMapCharacterList::iterator iter = m_MapCharacterList.find(biCharacterDBID);
	if (iter == m_MapCharacterList.end()) return NULL;

	return &(iter->second);	
}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

int CDNUserConnection::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	__try {
		DWORD dwCurTick = timeGetTime();
		if ((m_cLastMainCmd == nMainCmd) && (m_cLastSubCmd == nSubCmd)){
			if (dwCurTick - m_dwLastMessageTick <= 5){
				g_Log.Log(LogType::_ERROR, this, L"CDNUserConnection::MessageProcess repeat command(%d, %d)\n", nMainCmd, nSubCmd);
				return ERROR_GENERIC_UNKNOWNERROR;
			}
		}
		m_dwLastMessageTick = dwCurTick;
		m_cLastMainCmd = nMainCmd;
		m_cLastSubCmd = nSubCmd; 

		switch(nMainCmd)
		{
			case CS_LOGIN:		return OnRecvLoginMessage( nSubCmd, pData, nLen );
			case CS_SYSTEM:		return OnRecvSystemMessage( nSubCmd, pData, nLen );
			case CS_GAMEOPTION:	return OnRecvGameOptionMessage( nSubCmd, pData, nLen );
#ifdef PRE_ADD_DOORS
			case CS_DOORS: return OnRecvDoorsMessage(nSubCmd, pData, nLen);
#endif		//#ifdef PRE_ADD_DOORS
		}

		g_Log.Log(LogType::_ERROR, this, L"CDNUserConnection::MessageProcess (%d, %d)\n", nMainCmd, nSubCmd);
	}
#if defined( PRE_LOGINDUMP_LOG )
	__except(CExceptionCodeLogLoginServer(GetExceptionCode(), L"CDNUserConnection::MessageProcess()", nMainCmd, nSubCmd, m_nReconnectLoginStep, &m_ReconnectData), EXCEPTION_CONTINUE_SEARCH) {	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
		// 이 코드 블럭은 절대 수행되지 않음
	}
#else
	__except(CExceptionCodeLog(GetExceptionCode(), L"CDNUserConnection::MessageProcess()", nMainCmd, nSubCmd), EXCEPTION_CONTINUE_SEARCH) {	// 여기서 예외를 처리하지 않고 상위 예외 처리자를 찾음
		// 이 코드 블럭은 절대 수행되지 않음
	}
#endif

	return ERROR_UNKNOWN_HEADER;
}

#if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SetAvailableCharacterSlotCount()
{
	m_cAvailableCharacterSlotCount = m_nDefaultCreateCharacterSlotPerAccount;
	char cAddSlotCount = 0;
	g_pSQLMembershipManager->QueryGetCharacterSlotCount(m_nAccountDBID, 0, 0, cAddSlotCount);
	m_cAvailableCharacterSlotCount += cAddSlotCount;

	if (m_cAvailableCharacterSlotCount < 0){
		g_Log.Log(LogType::_FILEDBLOG, this, L"[SetAvailableCharacterSlotCount] %d", m_cAvailableCharacterSlotCount);
		m_cAvailableCharacterSlotCount = 0;
	}
	if (m_cAvailableCharacterSlotCount > m_nCreateCharacterSlotMaxPerAccount){
		g_Log.Log(LogType::_FILEDBLOG, this, L"[SetAvailableCharacterSlotCount] %d", m_cAvailableCharacterSlotCount);
		m_cAvailableCharacterSlotCount = m_nCreateCharacterSlotMaxPerAccount;
	}
}

char CDNUserConnection::GetAvailableCharacterSlotCount()
{
	return m_cAvailableCharacterSlotCount;
}

int CDNUserConnection::GetEmptyCharacterSlotCount()
{
	if (m_MapCharacterList.empty())
		return m_cAvailableCharacterSlotCount;

#if defined(PRE_ADD_DWC)
	int nCharacterCount = 0;
	nCharacterCount = static_cast<int>(m_MapCharacterList.size()) - (IsExistDWCCharacter() ? 1 : 0);
	return m_cAvailableCharacterSlotCount - nCharacterCount;
#else		//#if defined(PRE_ADD_DWC)
	return m_cAvailableCharacterSlotCount - (int)m_MapCharacterList.size();
#endif		//#if defined(PRE_ADD_DWC)
}

#else	// #if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SetCharacterCountMax(BYTE cMaxCount)
{
	m_cCharCountMax = cMaxCount;

	if (m_cCharCountMax < 0) m_cCharCountMax = 0;
	if (m_cCharCountMax > m_cCharacterLimitCount) m_cCharCountMax = m_cCharacterLimitCount;
}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

void CDNUserConnection::ClearCharacterData()
{
#if defined(PRE_MOD_SELECT_CHAR)
	m_MapCharacterList.clear();
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	memset(&m_CharDataArray, 0, sizeof(m_CharDataArray));
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

#if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SetSelectCharacterData(INT64 biCharacterDBID)
{
	TDBListCharData *pCharData = GetCharacterData(biCharacterDBID);
	if (pCharData){
		m_SelectCharacterData = *pCharData;

#if defined(_WORK)
		/*
		m_cAccountLevel = 0;
		SetWorldID(pCharData->cWorldID);
		*/

#else	// #if defined(_WORK)
		SetWorldID(pCharData->cWorldID);
#endif	// #if defined(_WORK)
	}
}

void CDNUserConnection::DelCharacterData(INT64 biCharacterDBID)
{
	if (biCharacterDBID <= 0) return;
	if (m_MapCharacterList.empty()) return;

	TMapCharacterList::iterator iter = m_MapCharacterList.find(biCharacterDBID);
	if (iter == m_MapCharacterList.end()) return;

	m_MapCharacterList.erase(iter);
}

#else	// #if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SetCharData(char cCharIndex, TDBListCharData &CharData)
{
	if (IsValidCharacterIndex(cCharIndex) == false) return;
	m_CharDataArray[cCharIndex] = CharData;
}

void CDNUserConnection::DelCharData(char cCharIndex)
{
	if (IsValidCharacterIndex(cCharIndex) == false) return;
	memset(&m_CharDataArray[cCharIndex], 0, sizeof(TDBListCharData));
}

TDBListCharData* CDNUserConnection::SelectCharData()
{
	if (IsValidCharacterIndex(m_cSelectCharIndex) == false) return NULL;
	return &m_CharDataArray[m_cSelectCharIndex];
}

TDBListCharData* CDNUserConnection::GetCharacterData(char cCharIndex)
{
	if (IsValidCharacterIndex(cCharIndex) == false) return NULL;
	return &m_CharDataArray[cCharIndex];
}

int CDNUserConnection::GetEmptyCharIndex()
{
	for (int i = 0; i < m_cCharacterLimitCount; i++){
		if (m_CharDataArray[i].biCharacterDBID <= 0) return i;
	}

	return -1;
}

INT64 CDNUserConnection::GetCharacterDBID( BYTE cIndex )
{
	if (IsValidCharacterIndex(cIndex) == false) return 0;
	return m_CharDataArray[cIndex].biCharacterDBID;
}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

INT64 CDNUserConnection::GetSelectCharacterDBID()
{
#if defined(PRE_MOD_SELECT_CHAR)
	return m_SelectCharacterData.biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if (IsValidCharacterIndex(m_cSelectCharIndex) == false) return 0;
	return m_CharDataArray[m_cSelectCharIndex].biCharacterDBID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

WCHAR* CDNUserConnection::GetSelectCharacterName()
{
#if defined(PRE_MOD_SELECT_CHAR)
	return m_SelectCharacterData.wszCharacterName;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if (IsValidCharacterIndex(m_cSelectCharIndex) == false) return NULL;
	return m_CharDataArray[m_cSelectCharIndex].wszCharacterName;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

int CDNUserConnection::GetSelectCharacterLastMapIndex()
{
#if defined(PRE_MOD_SELECT_CHAR)
	return m_SelectCharacterData.nLastVillageMapID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	if (IsValidCharacterIndex(m_cSelectCharIndex) == false) return 0;
	return m_CharDataArray[m_cSelectCharIndex].nLastVillageMapID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

int CDNUserConnection::GetSelectCharacterMapIndex()
{
#if defined(PRE_MOD_SELECT_CHAR)
	return m_SelectCharacterData.nMapID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	return m_CharDataArray[m_cSelectCharIndex].nMapID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

BYTE CDNUserConnection::GetSelectCharacterJob()
{
#if defined(PRE_MOD_SELECT_CHAR)
	return m_SelectCharacterData.cJob;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	return m_CharDataArray[m_cSelectCharIndex].cJob;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

#if defined(PRE_ADD_DWC)
BYTE CDNUserConnection::GetSelectCharacterAccountLevel()
{
#if defined(PRE_MOD_SELECT_CHAR)
	return m_SelectCharacterData.cAccountLevel;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	return m_CharDataArray[m_cSelectCharIndex].cAccountLevel;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

int CDNUserConnection::GetDWCWorldSetID()
{
#if defined(PRE_MOD_SELECT_CHAR)
	return g_pExtManager->GetWorldSetID(DWC::DWCWorldID);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	return DWC::DWCWorldID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

bool CDNUserConnection::IsExistDWCCharacter()
{
	bool bExist = false;

#if defined( PRE_MOD_SELECT_CHAR )
	for( TMapCharacterList::iterator iter = m_MapCharacterList.begin() ; m_MapCharacterList.end() != iter ; ++ iter )
	{
		if( iter->second.cAccountLevel != AccountLevel_DWC )
			continue;

		bExist = true;
		break;
	}
#else // #if defined( PRE_MOD_SELECT_CHAR )
	for (int i = 0; i < m_cCharacterLimitCount; ++i)
	{
		if (m_CharDataArray[i].biCharacterDBID <= 0 || m_CharDataArray[i].cAccountLevel != AccountLevel_DWC) 
			continue;

		bExist = true;
		break;
	}
#endif // #if defined( PRE_MOD_SELECT_CHAR )

	return bExist;
}

bool CDNUserConnection::CheckDWCLimitCharacterLevel( BYTE cLimitCharacterLevel )
{
#if defined( PRE_MOD_SELECT_CHAR )
	for( TMapCharacterList::iterator iter = m_MapCharacterList.begin() ; m_MapCharacterList.end() != iter ; ++ iter )
	{
		if( iter->second.cLevel >= cLimitCharacterLevel )
			return true;
	}
#else // #if defined( PRE_MOD_SELECT_CHAR )
	for (int i = 0; i < m_cCharacterLimitCount; ++i)
	{
		if (m_CharDataArray[i].biCharacterDBID <= 0) 
			continue;

		if (m_CharDataArray[i].cLevel >= cLimitCharacterLevel)
			return true;

	}
#endif // #if defined( PRE_MOD_SELECT_CHAR )

	return false;
}

#endif // #if defined(PRE_ADD_DWC)

TChannelInfoEx* CDNUserConnection::GetChannelInfo(int nChannelID)
{
	if (m_ChannelList.empty()) return NULL;

	TMapChannel::iterator iter = m_ChannelList.find(nChannelID);
	if (iter != m_ChannelList.end()){
		return &iter->second;
	}

	return NULL;
}

TChannelInfoEx* CDNUserConnection::GetOtherChannelInfo(int nChannelID)
{
	if (m_ChannelList.empty()) return NULL;

	TMapChannel::iterator ii;
	for (ii = m_ChannelList.begin(); ii != m_ChannelList.end(); ii++)
	{
		if ((*ii).second.Channel.nChannelID != nChannelID && (*ii).second.Channel.nChannelMaxUser > (*ii).second.Channel.nCurrentUser)
			return &(*ii).second;
	}

	return NULL;
}

void CDNUserConnection::SetWorldID(char cWorldID)
{
	m_cWorldID = cWorldID;
	m_cWaitWorldID = 0;
	m_nWaitWorldSetID = 0;
#if defined(PRE_MOD_SELECT_CHAR)
	m_nWorldSetID = g_pExtManager->GetWorldSetID(m_cWorldID);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

void CDNUserConnection::DelDailyCreateCount()
{
	if (m_cDailyCreateCount == 0) return;

	m_cDailyCreateCount -= 1;
	if (m_cDailyCreateCount < 0) m_cDailyCreateCount = 0;
}

void CDNUserConnection::SendInsideDisconnectPacket(wchar_t *pIdent)
{
#ifdef _USE_ACCEPTEX
	if( ::InterlockedIncrement( &m_dwSendInsideDisconnect ) != 1 )
		return;
#endif

	if(IsCertified()) // 인증이 완료된 애들만 LogOut 시킨다.
	{
		if ((m_cUserState != STATE_CONNECTVILLAGE) && (m_cUserState != STATE_CONNECTGAME))
		{		// village, game 로 넘어가서 끊기는 유저가 아님
			g_pSQLMembershipManager->QueryLogout(m_nAccountDBID, m_nSessionID, m_szMID);
		}
	}

	DNEncryptPacketSeq EnPacket = { 0, };
	EnPacket.Packet.iLen = static_cast<unsigned short>(sizeof(DNTPacketHeader));
	EnPacket.Packet.cMainCmd = IN_DISCONNECT;
	EnPacket.Packet.cSubCmd = CONNECTIONKEY_USER;

	EnPacket.nLen = EnPacket.Packet.iLen + sizeof(BYTE) + sizeof(USHORT);
	CDNSecure::GetInstance().Tea_encrypt( reinterpret_cast<char*>(&EnPacket.Packet), EnPacket.Packet.iLen );

	SetDelete(true);
	AddRecvData( EnPacket );

#ifdef _USE_ACCEPTEX
	g_pIocpManager->PostDisconnect( m_pSocketContext );
#else
	g_pIocpManager->AddProcessCall(m_pSocketContext);
#endif

	m_bReconnected = false;
}

#if defined (_TH)
bool CDNUserConnection::SetAsiaAuthInfo(AsiaAuth::TAsiaSoftAuthInfo &AuthInfo)
{
	AuthInfo.strDomain = m_AsiaAuthInfo.strDomain;
	m_AsiaAuthInfo = AuthInfo;

	if (m_AsiaAuthInfo.nAuthReturnCode == AsiaAuth::AuthResultCode::None)
	{
		g_Log.Log(LogType::_ERROR, this, L"CDNUserConnection::SetAsiaAuthInfo m_AsiaAuthInfo.nAuthReturnCode[%d]\n", m_AsiaAuthInfo.nAuthReturnCode);
		return false;
	}
	return true;
}
#endif		//#if defined (_TH)

#if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SetWaitWorldIDBySelectCharacter()
{
	m_cWaitWorldID = m_SelectCharacterData.cWorldID;
	m_nWaitWorldSetID = g_pExtManager->GetWorldSetID(m_cWaitWorldID);
}

BYTE CDNUserConnection::GetWorldIDByCharacterDBID(INT64 biCharacterDBID)
{
	TDBListCharData *pCharData = GetCharacterData(biCharacterDBID);
	if (pCharData)
		return pCharData->cWorldID;

	return 0;
}

int CDNUserConnection::GetWorldSetIDByCharacterDBID(INT64 biCharacterDBID)
{
	return g_pExtManager->GetWorldSetID(GetWorldIDByCharacterDBID(biCharacterDBID));
}

bool CDNUserConnection::IsValidCharacter(INT64 biCharacterDBID)
{
	TDBListCharData *pCharData = GetCharacterData(biCharacterDBID);
	if (pCharData) return true;

	return false;
}

#else	// #if defined(PRE_MOD_SELECT_CHAR)
bool CDNUserConnection::IsValidCharacterIndex(char cCharIndex)
{
	if ((cCharIndex < 0) || (cCharIndex >= m_cCharacterLimitCount)) return false;
	return true;
}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

void CDNUserConnection::CheckVillageGameInfo()
{
#if defined( PRE_ADD_DWC )
	if (GetSelectCharacterLastMapIndex() == 0 && GetSelectCharacterAccountLevel() != AccountLevel_DWC){	// 선택한 캐릭터가 튜토리얼을 못끝낸 상태라면(DWC캐릭터는 튜토리얼을 거치지 않는다)
#else // #if defined( PRE_ADD_DWC )
	if (GetSelectCharacterLastMapIndex() == 0){	// 선택한 캐릭터가 튜토리얼을 못끝낸 상태라면
#endif // #if defined( PRE_ADD_DWC )
		if (m_nTutorialMapIndex > 0) {
#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif
			CDNMasterConnection* pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(m_nWorldSetID, false));
			if(!pMasterCon) {
				SendVillageInfo(ERROR_GENERIC_MASTERCON_NOT_FOUND, GetAccountDBID(), GetCertifyingKey());
				return;
			}
			 pMasterCon->SendReqTutorialGameID(GetAccountDBID(), GetSessionID(), m_nTutorialMapIndex, m_cTutorialGateNo);
		}
		else {
			SendVillageInfo(ERROR_NONE, GetAccountDBID(), GetCertifyingKey());
			m_cUserState = STATE_CONNECTVILLAGE;
		}
	}
	else{
		// 선택한 채널이 CHANNEL_ATT_USELASTMAPINDEX 속성이면 MapIndex 를 LastMapIndex 로 대체해서 Update 한다.
		TChannelInfoEx* pChannelInfo = GetChannelInfo( m_nChannelID );
		if( pChannelInfo && pChannelInfo->Channel.nAttribute&GlobalEnum::CHANNEL_ATT_USELASTMAPINDEX )
		{
#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif
			// MapIndex 가 Village 인지 확인 사살 검사 
			if( g_pExtManager->GetMapType(GetSelectCharacterMapIndex()) == GlobalEnum::MAP_VILLAGE )
			{
				CDNMasterConnection* pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(m_nWorldSetID, false));

				bool bUpdate = false;
				if( pMasterCon && !pMasterCon->bIsUseLastMapIndex(GetSelectCharacterMapIndex()))
					bUpdate = true;

				if( bUpdate )
					g_pSQLWorldManager->QueryModLastVillageMapID(m_nWorldSetID, GetAccountDBID(), GetSelectCharacterDBID(), GetSelectCharacterMapIndex());
			}
			else
			{
				_DANGER_POINT();
			}
		}

		SendVillageInfo(ERROR_NONE, GetAccountDBID(), GetCertifyingKey());
		m_cUserState = STATE_CONNECTVILLAGE;
	}
}

void CDNUserConnection::PCBangResult(char cResult)
{
#if defined(_KR)	// PCBang관련 
	switch (cResult)
	{
	case Result_Forbidden:	// 미가입PC방 또는 IP당 접속수를 초과한 경우
		DetachConnection(L"PCBangResult(Result_Forbidden)");
		break;

	case Result_Terminate:	// 정량제 이용하다 시간 만료된 경우
		DetachConnection(L"PCBangResult(Result_Terminate)");
		break;

	case Result_Allowed:	// 가입된 PC방
		m_bPCBang = true;
		break;

	case Result_Trial:
	case Result_Message:
		break;
	}
#endif	// #if defined(_KR)
}

void CDNUserConnection::CheckLoginCH(char cIsLimited, char cIsProtected)
{
#if defined(_CH)
	int nRet = ERROR_DB;
	nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, FALSE);

#if defined(_FINAL_BUILD)
	if (nRet == ERROR_NONE){
		if (cIsLimited == 0){
			if (cIsProtected == 0) nRet = ERROR_LOGIN_FCM_PROTECTED;	// 입력미완료
			else if (cIsProtected == 1) nRet = ERROR_LOGIN_FCM_LIMITED; 	// 미성년
		}
	}
#endif

	SendCheckLogin(nRet);

	if ((nRet == ERROR_NONE) || (nRet == ERROR_LOGIN_FCM_PROTECTED) || (nRet == ERROR_LOGIN_FCM_LIMITED)){
		m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
	}
	else {
		// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
		SetAccountDBID(0);
	}
#endif	// #if defined(_CH)
}

void CDNUserConnection::CheckLoginTW()
{
#if defined(_TW)
	int nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, FALSE);
	if (ERROR_NONE == nRet) {
		m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
		g_Log.Log(LogType::_FILELOG, L"[GASH] LoginSuccess %s, %d\r\n", GetAccountName(), m_nSessionID);
	}
	else {
		// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
		SetAccountDBID(0);
		SetAccountName(L"");	// 대만은 미리 지정한 계정이름도 함께 초기화 함
	}

	SendCheckLogin(nRet);
#endif	// #if defined(_TW)
}

void CDNUserConnection::CheckLoginTH()
{
#if defined(_TH)
	int nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, FALSE);
	if (ERROR_NONE == nRet) {
		m_cUserState = STATE_CHECKLOGIN;	// 상태 바꿔준다.
		g_Log.Log(LogType::_FILELOG, L"[ASIASOFT] LoginSuccess %s, %u\r\n", GetAccountName(), m_nSessionID);
	}
	else {
		// P.S.> 여기서 사용자 계정번호를 초기화 해주어야 재로그인 시도 시 CDNUserConnectionManager::GetConnectionByAccountDBID(...) 체크 시 종료되지 않음	// 20100112
		SetAccountDBID(0);
		SetAccountName(L"");	// 대만은 미리 지정한 계정이름도 함께 초기화 함
	}

	SendCheckLogin(nRet);
#endif	// #elif defined(_TH)
}

void CDNUserConnection::CheckLoginEU(int nResult, const char *pszAccountName)
{
#if defined(_EU)
	switch(nResult)
	{
	case 1:		// normal user : 1,
		{
			{
				USES_CONVERSION;
				SetAccountName(A2CW(pszAccountName));
			}

			int nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, FALSE);
			if (ERROR_NONE == nRet)
				m_cUserState = STATE_CHECKLOGIN;
			else
				SetAccountDBID(0);

			SendCheckLogin(nRet);
		}
		break;

	case 2:		// temporally suspended : 2
	case 3: 	// permanently terminated : 3
	case 4:		// deactivated user : 4
		{
			SetAccountDBID(0);
			SendCheckLogin(ERROR_LOGIN_FAIL);	
		}
		break;

	case 5:		// wrong password or account : 5
		{
			SetAccountDBID(0);
			SendCheckLogin(ERROR_LOGIN_ID_PASS_UNMATCH);
		}
		break;
	}

#endif	// #if defined(_EU)
}

void CDNUserConnection::CheckLoginKRAZ(char cResult, const WCHAR *pReservedFields, const WCHAR *pwszAccountName)
{
#if defined(_KRAZ)
	switch (cResult)
	{
	case 'S':	// 셧다운 대상
		{
			SendCompleteDetachMsg(ERROR_NEXONAUTH_SHUTDOWNED_3, L"CheckLoginKRAZ(S)");			
		}
		break;

	case 'K':	// 선택적 셧다운
		{
			SendCompleteDetachMsg(ERROR_NEXONAUTH_SHUTDOWNED_4, L"CheckLoginKRAZ(K)");			
		}
		break;

	case 'O':
		{
			// 셧다운 코드 S or K+:+채널링 넘버(CSiteNo)+:+셧다운 날짜('년/월/일')+:+셧다운 시간(시(24시 기준))
			if (pReservedFields){
				std::wstring wstrFields = pReservedFields;
				std::vector<std::wstring> Tokens;
				TokenizeW(wstrFields, Tokens, L":");

				std::string strTemp;
				strTemp.clear();
				if (!Tokens.empty()){
					CTimeSet CurTime;
					if (wcscmp(Tokens[0].c_str(), L"S") == 0){
						if (CurTime.GetHour() <= 6){
							SendCompleteDetachMsg(ERROR_NEXONAUTH_SHUTDOWNED_3, L"Shutdown(S)");
							return;
						}

						m_ShutdownData.cType = Actoz::ShutdownType::Force;

						SetPrmInt1(1);
					}
					else if (wcscmp(Tokens[0].c_str(), L"K") == 0){
						if (Tokens.size() != 4)
							break;

						std::wstring wstrTime = Tokens[2].c_str();
						std::vector<std::wstring> TokenTime;
						TokenizeW(wstrTime, TokenTime, L"/");

						if (TokenTime.size() != 3)
							break;

						m_ShutdownData.cType = Actoz::ShutdownType::Selective;
						m_ShutdownData.nYear = _wtoi(TokenTime[0].c_str());
						m_ShutdownData.nMonth = _wtoi(TokenTime[1].c_str());
						m_ShutdownData.nDay = _wtoi(TokenTime[2].c_str());
						m_ShutdownData.nHour = _wtoi(Tokens[3].c_str());

						if ((CurTime.GetYear() == m_ShutdownData.nYear) && (CurTime.GetMonth() == m_ShutdownData.nMonth) && (CurTime.GetDay() == m_ShutdownData.nDay) && (CurTime.GetHour() == m_ShutdownData.nHour)){
							SendCompleteDetachMsg(ERROR_NEXONAUTH_SHUTDOWNED_4, L"Shutdown(K)");
							return;
						}

						strTemp = FormatA("%d%d%d%d", m_ShutdownData.nYear, m_ShutdownData.nMonth, m_ShutdownData.nDay, m_ShutdownData.nHour);
						SetPrmInt1(atoi(strTemp.c_str()));
					}
				}
			}

			SetAccountName(pwszAccountName);

			int nRet = g_pSQLMembershipManager->QueryLogin(this, NULL, FALSE);
			if (ERROR_NONE == nRet)
				m_cUserState = STATE_CHECKLOGIN;
			else
				SetAccountDBID(0);

			SendCheckLogin(nRet);
		}
		break;

	default:
		{
			SetAccountDBID(0);
			SendCheckLogin(ERROR_LOGIN_FAIL);	
		}
		break;
	}
#endif	// #if defined(_KRAZ)
}

void CDNUserConnection::SendDelUser()
{
	if (m_nWaitWorldSetID > 0 || m_nWorldSetID > 0)
	{
#ifdef _USE_ACCEPTEX
		ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif
		int nWorldSetID = m_cUserState == STATE_WAITUSER ? m_nWaitWorldSetID : m_nWorldSetID;
		CDNMasterConnection *pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(nWorldSetID, false));
		if (pMasterCon)
		{
			if (m_cUserState == STATE_WAITUSER)
			{
				pMasterCon->SendDelWaitUser(g_pAuthManager->GetServerID(), GetAccountDBID());
				pMasterCon->DelWaitUser(GetAccountDBID());
				m_cWaitWorldID = 0;
			}
			else
			{
				if (m_cUserState != STATE_CONNECTVILLAGE && m_cUserState != STATE_CONNECTGAME)
				{
#if defined(PRE_MOD_SELECT_CHAR)
					pMasterCon->SendDetachUser(GetAccountDBID());
#else	// #if defined(PRE_MOD_SELECT_CHAR)
					pMasterCon->SendDetachUser(GetAccountDBID(), m_cAccountLevel);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
				}
#if !defined(PRE_MOD_SELECT_CHAR)
				m_bMasterAddUser = false;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
			}
		}
		else
			_DANGER_POINT();
	}
}

// SC_System
void CDNUserConnection::SendCompleteDetachMsg(int nRetCode, const WCHAR * pwszDetachReason)
{
	SCDetachMessage packet;
	memset(&packet, 0, sizeof(SCDetachMessage));

	packet.nRetCode = nRetCode;

	AddSendData(SC_SYSTEM, eSystem::SC_DETACHCONNECTION_MSG, (char*)&packet, sizeof(packet));
	DelayDetachConnection(const_cast<wchar_t*>(pwszDetachReason));
}

void CDNUserConnection::SendVillageInfo(int nRet, UINT nAccountDBID, INT64 biCertifyingKey)
{
	SCVillageInfo VillageInfo;
	memset(&VillageInfo, 0, sizeof(SCVillageInfo));

	VillageInfo.nSessionID = m_nSessionID;
	VillageInfo.nAccountDBID = nAccountDBID;
	VillageInfo.biCertifyingKey = biCertifyingKey;

	TChannelInfoEx *pChannel = GetChannelInfo(m_nChannelID);
	if (pChannel){
		_strcpy(VillageInfo.szVillageIp, IPLENMAX, pChannel->szIP, (int)strlen(pChannel->szIP));
		VillageInfo.wVillagePort = pChannel->nPort;
	}

	VillageInfo.nRet = nRet;

	AddSendData(SC_SYSTEM, SC_VILLAGEINFO, (char*)&VillageInfo, sizeof(SCVillageInfo));	
}

void CDNUserConnection::SendGameInfo(ULONG nIP, USHORT nPort, USHORT nTcpPort, int nRet, UINT nAccountDBID, INT64 biCertifyingKey)
{
	SCGameInfo GameInfo;
	memset(&GameInfo, 0, sizeof(SCGameInfo));

	GameInfo.GameTaskType = GameTaskType::Normal;
	GameInfo.nGameServerIP = nIP;
	GameInfo.nGameServerPort = nPort;
	GameInfo.nGameServerTcpPort = nTcpPort;
	GameInfo.nRet = nRet;
	GameInfo.nAccountDBID = nAccountDBID;
	GameInfo.biCertifyingKey = biCertifyingKey;

	AddSendData(SC_SYSTEM, SC_GAMEINFO, (char*)&GameInfo, sizeof(GameInfo));
}

void CDNUserConnection::SendReconnectLogin(int nRet, UINT nAccountDBID, INT64 biCertifyingKey)
{
	SCReconnectLogin Reconnect = { 0, };
	Reconnect.nRet = nRet;
	Reconnect.nAccountDBID = nAccountDBID;
	Reconnect.biCertifyingKey = biCertifyingKey;

	AddSendData(SC_SYSTEM, SC_RECONNECTLOGIN, (char*)&Reconnect, sizeof(SCReconnectLogin));
#if defined(PRE_ADD_LOGIN_USERCOUNT)
	// 여기에 찡겨넣자.
	if( nRet == ERROR_NONE && !m_bSummitUserCount)
	{		
		g_pUserConnectionManager->AddUserCount();
		m_bSummitUserCount = true;
	}
#endif
}

// SC_Login
void CDNUserConnection::SendCheckVersion(int nRet)
{
	SCCheckVersion Check;
	memset(&Check, 0, sizeof(SCCheckVersion));
	_strcpy(Check.szServerVersion, SERVERVERSIONMAX, g_Config.szVersion, (int)strlen(g_Config.szVersion));
	Check.nRet = nRet;
	memcpy( &Check.MembershipDBVersion, g_pSQLMembershipManager->GetVersion(), sizeof(Check.MembershipDBVersion) );
	memcpy( &Check.WorldDBVersion, g_pSQLWorldManager->GetVersion(GetWorldSetID()), sizeof(Check.WorldDBVersion) );

	AddSendData(SC_LOGIN, SC_CHECKVERSION, (char*)&Check, sizeof(SCCheckVersion));
}

void CDNUserConnection::SendCheckLogin(int nRet, const char* pInfoURL/*=NULL*/)
{
	SCCheckLogin Login;
	memset(&Login, 0, sizeof(SCCheckLogin));

	Login.nRet = nRet;
	Login.nSessionID = m_nSessionID;
#if !defined(PRE_MOD_SELECT_CHAR)
	Login.cGender = m_cGender;
	Login.cCharCountMax = m_cCharCountMax;
#endif	// #if !defined(PRE_MOD_SELECT_CHAR)

	__time64_t servertime;
	time(&servertime);
	Login._tServerTime = servertime;

	memcpy_s(&Login.GameOption, sizeof(TGameOptions), &m_GameOptions, sizeof(TGameOptions));	
#ifdef PRE_ADD_COMEBACK
	Login.bComebackUser = m_bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	Login.bReConnectNewbieReward = m_bReConnectNewbieReward;
	Login._tAccountRegistTime = m_tAccountRegistDate;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#ifdef PRE_ADD_DOORS
	Login.bMobileAuthentication = m_bMobileAuthentication;
#endif		//PRE_ADD_DOORS

#if defined(_CH) || defined(_KRAZ)
	_wcscpy(Login.wszAccountName, IDLENMAX, GetAccountName(), (int)wcslen(GetAccountName()));
#endif	// #if defined(_CH) || defined(_KRAZ)

	int nLen = sizeof(SCCheckLogin);

#if defined(_CH)
	if( pInfoURL )
		_strcpy(Login.szInfoURL, SNDAOAPASSPORTMAX, pInfoURL, (int)strlen(pInfoURL));

	nLen = (int)(sizeof(SCCheckLogin)-(SNDAOAPASSPORTMAX-strlen(Login.szInfoURL)));
#endif

	AddSendData(SC_LOGIN, SC_CHECKLOGIN, (char*)&Login, nLen);

#if defined(PRE_ADD_LOGIN_USERCOUNT)
	// 여기에 찡겨넣자.
	if( nRet == ERROR_NONE && !m_bSummitUserCount)
	{
		g_pUserConnectionManager->AddUserCount();
		m_bSummitUserCount = true;
	}
#endif
}

void CDNUserConnection::SendServerList( std::map<int,int>& mWorldUserCount )
{
	SCServerList ServerList;
	memset(&ServerList, 0, sizeof(ServerList));

#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif

#if defined(_TW)
	bool bDenyServer = m_cRegion == 95 ? true : false;
	g_pExtManager->GetServerList(ServerList.ServerListData, ServerList.cServerCount, bDenyServer);
	g_Log.Log(LogType::_FILELOG, L"[ADBID:%u SID:%u] Send ServerList Region:%d, ServerCount:%d !!\r\n", GetAccountDBID(), GetSessionID(), m_cRegion, ServerList.cServerCount);
#elif defined(PRE_IDN_PVP)
	bool bDenyServer = m_cAccountLevel > 0 ? false : true;
	g_pExtManager->GetServerList(ServerList.ServerListData, ServerList.cServerCount, bDenyServer);	
#else
	g_pExtManager->GetServerList(ServerList.ServerListData, ServerList.cServerCount);
#endif
	for (int i = 0; i < ServerList.cServerCount; i++)
	{
		int nWorldSetID = g_pExtManager->GetWorldSetID(ServerList.ServerListData[i].cWorldID);
		CDNMasterConnection * pCon = (CDNMasterConnection*)g_pMasterConnectionManager->GetConnection(nWorldSetID, false);
		if (pCon)
		{
			ServerList.ServerListData[i].nWorldMaxUser = pCon->GetWorldMaxUserCount();
			ServerList.ServerListData[i].nWorldCurUser = pCon->GetWorldCurUserCount();
			BYTE cMyCharCount = 0;
			std::map<int,int>::iterator itor = mWorldUserCount.find( ServerList.ServerListData[i].cWorldID );
			if( itor != mWorldUserCount.end() )
			{
				cMyCharCount = static_cast<BYTE>(itor->second);
			}

			ServerList.ServerListData[i].cMyCharCount = cMyCharCount;

		}
		//else
		//	_DANGER_POINT();
	}

	AddSendData(SC_LOGIN, SC_SERVERLIST, (char*)&ServerList, sizeof(SCServerList) - \
		sizeof(ServerList.ServerListData) + (sizeof(TServerListData) * ServerList.cServerCount));
}

void CDNUserConnection::SendCharList(int nRet)
{
#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif
	SCCharList CharList;
	memset(&CharList, 0, sizeof(SCCharList));

	CharList.nRet = nRet;
	CharList.cAccountLevel	= GetAccountLevel();

#if defined( PRE_ADD_DWC )
	char cDWCCharacterCount = 0;
#endif // #if defined( PRE_ADD_DWC )

#if !defined(PRE_MOD_SELECT_CHAR)
	CDNMasterConnection* pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(m_nWorldSetID, false));
	if( !pMasterCon )
	{
		//마스터 서버가 없는 상태에서 캐릭터를 지우거나 머 등등의 상태에서 에러메세지를 주기 위해서
		AddSendData(SC_LOGIN, SC_CHARLIST, (char*)&CharList, sizeof(CharList) - sizeof(CharList.CharListData) + (sizeof(TCharListData) * CharList.cCharCount));
		return;
	}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	if (nRet == ERROR_NONE){
#if defined(PRE_CHARLIST_SORTING)
		CharList.cCharacterSortCode = m_cCharacterSortCode;
#endif	// #if defined(PRE_CHARLIST_SORTING)

		int nCount = 0;
#if defined(PRE_MOD_SELECT_CHAR)
		for (TMapCharacterList::iterator iter = m_MapCharacterList.begin(); iter != m_MapCharacterList.end(); iter++){
			if (iter->second.biCharacterDBID <= 0) continue;

			CharList.CharListData[nCount].cWorldID = iter->second.cWorldID;
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
			CharList.CharListData[nCount].cWorldSetID = g_pExtManager->GetWorldSetID(iter->second.cWorldID);
#endif
			CharList.CharListData[nCount].biCharacterDBID = iter->second.biCharacterDBID;
			_wcscpy(CharList.CharListData[nCount].wszCharacterName, NAMELENMAX, iter->second.wszCharacterName, NAMELENMAX);
			CharList.CharListData[nCount].cJob = iter->second.cJob;
			CharList.CharListData[nCount].cLevel = iter->second.cLevel;
			CharList.CharListData[nCount].biUniqueID = iter->second.biCharacterDBID;

			int nMapID = iter->second.nMapID;
			if (g_pExtManager->GetMapType(nMapID) != GlobalEnum::MAP_VILLAGE || iter->second.nLastVillageMapID == 0)
				nMapID = iter->second.nLastVillageMapID;

			int nWorldSetID = g_pExtManager->GetWorldSetID(iter->second.cWorldID);
			CDNMasterConnection* pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(nWorldSetID, false));
			if (pMasterCon && (pMasterCon->bIsUseLastMapIndex(nMapID)))
				nMapID = iter->second.nLastVillageMapID;

			CharList.CharListData[nCount].nMapID = nMapID;
			CharList.CharListData[nCount].nDefaultBody = iter->second.nDefaultBody;
			CharList.CharListData[nCount].nDefaultFoot = iter->second.nDefaultFoot;
			CharList.CharListData[nCount].nDefaultHand = iter->second.nDefaultHand;
			CharList.CharListData[nCount].nDefaultLeg = iter->second.nDefaultLeg;
			CharList.CharListData[nCount].dwHairColor = iter->second.dwHairColor;
			CharList.CharListData[nCount].dwEyeColor = iter->second.dwEyeColor;
			CharList.CharListData[nCount].dwSkinColor = iter->second.dwSkinColor;
			CharList.CharListData[nCount].bDeleteFlag = iter->second.bDeleteFlag;
			CharList.CharListData[nCount].DeleteDate = iter->second.DeleteDate;
#if defined(PRE_CHARLIST_SORTING)
			CharList.CharListData[nCount].LastLoginDate = iter->second.LastLoginDate;
			CharList.CharListData[nCount].CreateDate = iter->second.CreateDate;
#endif	// #if defined(PRE_CHARLIST_SORTING)

			memcpy(&CharList.CharListData[nCount].nEquipArray, iter->second.nEquipArray, sizeof(int) * EQUIPMAX);
			memcpy(&CharList.CharListData[nCount].nCashEquipArray, iter->second.nCashEquipArray, sizeof(int) * CASHEQUIPMAX);
#if defined( PRE_ADD_NEWCOMEBACK )
			CharList.CharListData[nCount].bCombackEffectItem = iter->second.bCombackEffectItem;
#endif
#if defined( PRE_ADD_DWC )
			CharList.CharListData[nCount].cAccountLevel = iter->second.cAccountLevel;
			if( AccountLevel_DWC == iter->second.cAccountLevel )
			{
				CharList.CharListData[nCount].nMapID = DWC::DWCMapID;
				++ cDWCCharacterCount;
			}
#endif // #if defined( PRE_ADD_DWC )
			nCount++;
		}
		CharList.cCharCount = nCount;
		CharList.cCharCountMax = m_cAvailableCharacterSlotCount;

#else	// #if defined(PRE_MOD_SELECT_CHAR)
		for (int i = 0; i < m_cCharacterLimitCount; i++)
		{
			if (m_CharDataArray[i].biCharacterDBID <= 0) 
				continue;
			wcsncpy(CharList.CharListData[nCount].wszCharacterName, m_CharDataArray[i].wszCharacterName, NAMELENMAX);
			CharList.CharListData[nCount].cJob = m_CharDataArray[i].cJob;
			CharList.CharListData[nCount].cLevel = m_CharDataArray[i].cLevel;
			CharList.CharListData[nCount].biUniqueID = m_CharDataArray[i].biCharacterDBID;

			int nMapID = m_CharDataArray[i].nMapID;
			if (g_pExtManager->GetMapType(nMapID) != GlobalEnum::MAP_VILLAGE || pMasterCon->bIsUseLastMapIndex(nMapID) || m_CharDataArray[i].nLastVillageMapID == 0 ) 
				nMapID = m_CharDataArray[i].nLastVillageMapID;

			CharList.CharListData[nCount].nMapID = nMapID;
			CharList.CharListData[nCount].cCharIndex = m_CharDataArray[i].cCharIndex;
			CharList.CharListData[nCount].nDefaultBody = m_CharDataArray[i].nDefaultBody;
			CharList.CharListData[nCount].nDefaultFoot = m_CharDataArray[i].nDefaultFoot;
			CharList.CharListData[nCount].nDefaultHand = m_CharDataArray[i].nDefaultHand;
			CharList.CharListData[nCount].nDefaultLeg = m_CharDataArray[i].nDefaultLeg;
			CharList.CharListData[nCount].dwHairColor = m_CharDataArray[i].dwHairColor;
			CharList.CharListData[nCount].dwEyeColor = m_CharDataArray[i].dwEyeColor;
			CharList.CharListData[nCount].dwSkinColor = m_CharDataArray[i].dwSkinColor;
			CharList.CharListData[nCount].bDeleteFlag = m_CharDataArray[i].bDeleteFlag;
			CharList.CharListData[nCount].DeleteDate = m_CharDataArray[i].DeleteDate;
#if defined(PRE_CHARLIST_SORTING)
			CharList.CharListData[nCount].LastLoginDate = m_CharDataArray[i].LastLoginDate;
#endif	// #if defined(PRE_CHARLIST_SORTING)

			memcpy(&CharList.CharListData[nCount].nEquipArray, m_CharDataArray[i].nEquipArray, sizeof(int) * EQUIPMAX);
			memcpy(&CharList.CharListData[nCount].nCashEquipArray, m_CharDataArray[i].nCashEquipArray, sizeof(int) * CASHEQUIPMAX);
#if defined( PRE_ADD_NEWCOMEBACK )
			CharList.CharListData[nCount].bCombackEffectItem	= m_CharDataArray[i].bCombackEffectItem;
#endif
#if defined( PRE_ADD_DWC )
			CharList.CharListData[nCount].cAccountLevel = m_CharDataArray[i].cAccountLevel;
			if( AccountLevel_DWC == m_CharDataArray[i].cAccountLevel )
			{
				CharList.CharListData[nCount].nMapID = DWC::DWCMapID;
				++ cDWCCharacterCount;
			}
#endif // #if defined( PRE_ADD_DWC )
			nCount++;
		}
		CharList.cCharCount = nCount;
		CharList.cCharCountMax = m_cCharCountMax;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined( PRE_ADD_DWC )
		CharList.cCharCountMax += cDWCCharacterCount;
#endif // #if defined( PRE_ADD_DWC )
	}
	
	AddSendData(SC_LOGIN, SC_CHARLIST, (char*)&CharList, sizeof(CharList) - sizeof(CharList.CharListData) + (sizeof(TCharListData) * CharList.cCharCount));
}

void CDNUserConnection::SendCharacterList(bool bProcessSecondAuth)
{
	// 캐릭터리스트와 2차인증 정보 얻어온다.

#if defined( PRE_ADD_DWC )
	INT64 biDWCCharacterDBID = 0;
	int nDeleteWaitingTime = DELETECHAR_WAITTIME_MINUTE;
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 12;
#endif
	int nRet = g_pSQLWorldManager->QueryGetDWCCharacterID(GetDWCWorldSetID(), GetAccountDBID(), biDWCCharacterDBID, nDeleteWaitingTime );
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 13;
#endif
	if( ERROR_NONE == nRet )
		nRet = QueryCharacterListAndSecondAuth( bProcessSecondAuth, biDWCCharacterDBID );
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 14;
#endif
#if defined(_FINAL_BUILD)
	if( biDWCCharacterDBID && !IsExistDWCCharacter() ) // 즉시삭제 되는 경우가 없다.
		nRet = ERROR_LOGIN_CANNOT_FIND_DWCCHARACTER;
#endif // #if defined(_FINAL_BUILD)

#if defined(PRE_MOD_SELECT_CHAR)
#else // #if defined(PRE_MOD_SELECT_CHAR)
	if( IsExistDWCCharacter() )
		m_cCharacterLimitCount = CreateCharacterDefaultCountMax + g_pExtManager->GetGlobalWeightValue(CharacterSlotMax) + 1;
#endif // #if defined(PRE_MOD_SELECT_CHAR)

#else // #if defined( PRE_ADD_DWC )
#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 15;
#endif
	int nRet = QueryCharacterListAndSecondAuth( bProcessSecondAuth );
#endif // #if defined( PRE_ADD_DWC )

	SendCharList(nRet);

#if defined( PRE_LOGINDUMP_LOG )	
	m_nReconnectLoginStep = 23;
#endif

#if defined( PRE_ADD_DWC )
	SendDWCChannelInfo();
#endif // #if defined( PRE_ADD_DWC )
}

void CDNUserConnection::SendChannelList( int nRet/*=ERROR_NONE*/, BYTE cFailCount/*=0*/ )
{
	SCChannelList ChannelList;
	memset(&ChannelList, 0, sizeof(SCChannelList));

	ChannelList.nRet = nRet;
	if( nRet != ERROR_NONE )
	{
		ChannelList.cFailCount = cFailCount;
		AddSendData( SC_LOGIN, SC_CHANNELLIST, (char*)&ChannelList, sizeof(SCChannelList) - sizeof(ChannelList.Info)- sizeof(ChannelList.cCount) );
		return;
	}

#if defined(PRE_MOD_SELECT_CHAR)
#if defined(PRE_ADD_DWC)
	if( GetSelectCharacterAccountLevel() == AccountLevel_DWC )
	{
#if defined(_WORK)
		g_pMasterConnectionManager->GetDWCChannelList(m_nWorldSetID, &m_SelectCharacterData, ChannelList.Info, ChannelList.cCount, &m_ChannelList);
		if(0 == ChannelList.cCount)
			g_pMasterConnectionManager->GetDWCChannelList(GetDWCWorldSetID(), &m_SelectCharacterData, ChannelList.Info, ChannelList.cCount, &m_ChannelList);
#else // #if defined(_WORK)
		g_pMasterConnectionManager->GetDWCChannelList(GetDWCWorldSetID(), &m_SelectCharacterData, ChannelList.Info, ChannelList.cCount, &m_ChannelList);
#endif // #if defined(_WORK)		
	}
	else
#endif
		g_pMasterConnectionManager->GetChannelListByMapIdx(m_nWorldSetID, &m_SelectCharacterData, ChannelList.Info, ChannelList.cCount, &m_ChannelList);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
#if defined(PRE_ADD_DWC)
	if( GetSelectCharacterAccountLevel() == AccountLevel_DWC )
	{		
#if defined(_WORK)
		g_pMasterConnectionManager->GetDWCChannelList(m_nWorldSetID, &m_CharDataArray[m_cSelectCharIndex], ChannelList.Info, ChannelList.cCount, &m_ChannelList);
		if(0 == ChannelList.cCount)
			g_pMasterConnectionManager->GetDWCChannelList(GetDWCWorldSetID(), &m_CharDataArray[m_cSelectCharIndex], ChannelList.Info, ChannelList.cCount, &m_ChannelList);
#else // #if defined(_WORK)
		g_pMasterConnectionManager->GetDWCChannelList(GetDWCWorldSetID(), &m_CharDataArray[m_cSelectCharIndex], ChannelList.Info, ChannelList.cCount, &m_ChannelList);
#endif // #if defined(_WORK)
	}
	else
#endif
		g_pMasterConnectionManager->GetChannelListByMapIdx(m_nWorldSetID, &m_CharDataArray[m_cSelectCharIndex], ChannelList.Info, ChannelList.cCount, &m_ChannelList );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	AddSendData(SC_LOGIN, SC_CHANNELLIST, (char*)&ChannelList, sizeof(SCChannelList) - sizeof(ChannelList.Info) + (sizeof(sChannelInfo) * ChannelList.cCount));
}

void CDNUserConnection::SendCreateChar(WCHAR *pName, int nRet)
{
	SCCreateChar Create;
	memset(&Create, 0, sizeof(SCCreateChar));

	wcsncpy(Create.wszCharacterName, pName, NAMELENMAX);
	Create.nRet = nRet;
	if (nRet == ERROR_LOGIN_CANNOT_CREATE_CHARACTER_TODAY)
		Create.cDailyCreateMaxCount = 7;	// 일단 박아놓자

	AddSendData(SC_LOGIN, SC_CREATECHAR, (char*)&Create, sizeof(SCCreateChar));
}

#if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SendDeleteChar(INT64 biCharacterDBID, int nRet, BYTE cFailCount/*=0*/)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SendDeleteChar( BYTE cCharIndex, int nRet, BYTE cFailCount/*=0*/ )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	SCDeleteChar Delete;
	memset(&Delete, 0, sizeof(SCDeleteChar));

#if defined(PRE_MOD_SELECT_CHAR)
	Delete.biCharacterDBID = biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	Delete.cCharIndex	= cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	Delete.nRet	= nRet;
	Delete.cFailCount = cFailCount;

	AddSendData(SC_LOGIN, SC_DELETECHAR, (char*)&Delete, sizeof(SCDeleteChar));	
}

#if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SendReviveChar(INT64 biCharacterDBID, int nRet)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
void CDNUserConnection::SendReviveChar( BYTE cCharIndex, int nRet )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	SCReviveChar TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

#if defined(PRE_MOD_SELECT_CHAR)
	TxPacket.biCharacterDBID = biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	TxPacket.cCharIndex = cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	TxPacket.nRet = nRet;

	AddSendData(SC_LOGIN, SC_REVIVECHAR, (char*)&TxPacket, sizeof(TxPacket));	
}

void CDNUserConnection::SendTutorialMapInfo(int nMapIndex, char cGateNo)
{
	SCTutorialMapInfo MapInfo = { 0, };

	MapInfo.nMapIndex = nMapIndex;
	MapInfo.cGateNo = cGateNo;

	AddSendData(SC_LOGIN, SC_TUTORIALMAPINFO, (char*)&MapInfo, sizeof(SCTutorialMapInfo));
}

void CDNUserConnection::SendWaitUser(BYTE cWorldID, UINT nWaitNumber, ULONG nEstimateTime)
{
	SCWaitUser packet;				// SC_LOGIN / SC_WAITUSER
	memset(&packet, 0, sizeof(packet));

	packet.cWorldID = cWorldID;
	packet.nWaitUserCount = nWaitNumber;
	packet.nEstimateTime = nEstimateTime;

	AddSendData(SC_LOGIN, SC_WAITUSER, (char*)&packet, sizeof(packet));
}

void CDNUserConnection::SendBackButtonLogin()
{
	AddSendData(SC_LOGIN, SC_BACKBUTTON_LOGIN, NULL, 0);
#if defined(PRE_ADD_LOGIN_USERCOUNT)
	// 여기에 찡겨넣자.
	if( m_bSummitUserCount )
	{		
		g_pUserConnectionManager->DelUserCount();
		m_bSummitUserCount = false;
	}
#endif
}

void CDNUserConnection::SendCheckBlock(__time64_t serverTime, TRestraintForAccountAndCharacter* pRestraint )
{
	SCCHeckBlock packet;
	memset(&packet, 0, sizeof(SCCHeckBlock));

	packet.ServerDate = serverTime;
	packet.StartDate = pRestraint->_tStartDate;
	packet.EndDate = pRestraint->_tEndDate;
	_wcscpy(packet.wszBlockReason, RESTRAINTREASONMAX, pRestraint->wszRestraintReason, (int)wcslen(pRestraint->wszRestraintReason));
	packet.cType = pRestraint->cLevelCode;
	packet.iReasonID = pRestraint->iReasonID;

	AddSendData(SC_LOGIN, SC_CHECKBLOCK, (char*)&packet, sizeof(SCCHeckBlock));
}

#if defined (_TH)
void CDNUserConnection::SendAsiaAuthRequestOTP()
{
	AddSendData(SC_LOGIN, SC_ASIASOFT_REQ_OTP, NULL, 0);
}

void CDNUserConnection::SendAsiaOTPResult(int nRet)
{
	SCOTRPResult packet;
	memset(&packet, 0, sizeof(SCOTRPResult));

	packet.nRetCode = nRet;

	AddSendData(SC_LOGIN, SC_ASIASOFT_RET_OTP, (char*)&packet, sizeof(SCOTRPResult));
}
#endif		//#if defined (_TH)

#if defined( PRE_ADD_DWC )
void CDNUserConnection::SendCreateDWCChar(int nRet)
{
	SCCreateDWCChar DWCCreate;
	memset(&DWCCreate, 0, sizeof(SCCreateDWCChar));
	DWCCreate.nRet = nRet;
	AddSendData(SC_LOGIN, SC_CREATEDWCCHAR, (char*)&DWCCreate, sizeof(SCCreateDWCChar));
}

void CDNUserConnection::SendDWCChannelInfo()
{
	SCDWCChannelInfo DWCChannelInfo;
	g_pDWCChannelManager->GetChannelInfo(DWCChannelInfo.ChannelInfo);
	AddSendData(SC_DWC, eDWC::SC_DWCCHANNELINFO, (char*)&DWCChannelInfo, sizeof(SCDWCChannelInfo));
}
#endif // #if defined( PRE_ADD_DWC )

void CDNUserConnection::SendSecondAuthInfo()
{
	SCGameOptionNotifySecondAuthInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.bIsSetSecondAuthLock	= m_bIsSetSecondAuthLock;
	TxPacket.bIsSetSecondAuthPW		= m_bIsSetSecondAuthPW;
	TxPacket.tSecondAuthResetDate	= m_tSecondAuthResetDate;
#ifdef PRE_ADD_23829
	TxPacket.bSecondAuthPassFlag = m_bPassSecondAuth;
#endif	//#ifdef PRE_ADD_23829
#ifdef PRE_ADD_IGNORESECONDAUTH_EKEYECARD
	TxPacket.bCompletlySecondAuthPassFlag = m_bCompletlyPassSecondAuth;
#endif

	AddSendData( SC_GAMEOPTION, eGameOption::SC_NOTIFY_SECONDAUTHINFO, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNUserConnection::SendResInitSecondAuth( int iRet, BYTE cFailCount/*=0*/ )
{
	SCGameOptionInitSecondAuth TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iRet = iRet;
	TxPacket.cFailCount = cFailCount;

	AddSendData( SC_GAMEOPTION, eGameOption::SC_INIT_SECONDAUTH, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined(_HSHIELD)
void CDNUserConnection::SendMakeRequest()
{
	if( !GetActive() )
		return;

	AHNHS_TRANS_BUFFER Packet = { 0, };

	DWORD dwRetVal = NOERROR;
	unsigned long nRet = HShieldSvrWrapper::AhnHS_MakeRequest(m_hHSClient, &Packet, &dwRetVal);
	if (NOERROR != dwRetVal) {
		DetachConnection(L"_AhnHS_MakeRequest Error [Exception]");
		g_Log.Log(LogType::_HACKSHIELD, this, L"[ADBID:%u, SID:%u] _AhnHS_MakeRequest [Exception] Ret:%d\r\n", m_nAccountDBID, m_nSessionID, nRet);
		return;
	}

	if (nRet != ERROR_SUCCESS){	// 일단 끊어;;
		DetachConnection(L"_AhnHS_MakeRequest Error");
		g_Log.Log(LogType::_HACKSHIELD, this, L"[ADBID:%u, SID:%u] _AhnHS_MakeRequest Ret:%x\r\n", m_nAccountDBID, m_nSessionID, nRet);
		return;
	}

	AddSendData(SC_SYSTEM, SC_MAKEREQUEST, (char*)&Packet, sizeof(AHNHS_TRANS_BUFFER));

	m_dwHShieldResponseTick = timeGetTime();
}
#elif defined(_GPK)
void CDNUserConnection::SendGPKCode()
{
	if(GetActive())
	{
		if (m_pCode == NULL || m_nCodeLen < 0 || m_nCodeLen > GPKCODELENMAX)
		{
			DetachConnection(L"GPKCode LoadFail\n");
			g_Log.Log(LogType::_FILELOG, L"SendGPKCode() m_pSession->m_nCodeLen\n");
			return;
		}

		SCGPKCode GPKCode;
		memset(&GPKCode, 0, sizeof(SCGPKCode));

		if (m_pCode == NULL)
			g_Log.Log(LogType::_FILELOG, L"SendGPKCode() m_pSession->m_nCodeLen\n");

		GPKCode.nCodeLen = m_nCodeLen;
		memcpy(GPKCode.Code, m_pCode, m_nCodeLen);

		AddSendData(SC_SYSTEM, eSystem::SC_CHN_GPKCODE, (char*)&GPKCode, sizeof(int) + (sizeof(BYTE) * m_nCodeLen));
		m_dwCheckGPKTick = timeGetTime();
	}
	else
	{
		//대략 code를 보내는 타이밍은 한타이밍입니다. 근데 여기서 안보내지면 꼬이겠지유? 확인을 위해서 로그를 찍습니다.
		//여기에서 센드가 실패하게 된다면 이후 SendGPKData에서도 실패하게 되어집니다. 끊어버립니다.
		DetachConnection(L"GPKCode SendError\n");
	}
}

void CDNUserConnection::SendGPKData()
{
	if (GetActive())
	{
		SCGPKData packet;
		memset(&packet, 0, sizeof(SCGPKData));

		_strcpy(packet.Data, GPKDATALENMAX, "ServerCheck!!", (int)strlen("ServerCheck!!"));

		if (!g_Config.pDynCode->Encode((unsigned char*)&packet.Data, GPKDATALENMAX, m_nCodeIndex)){
			DetachConnection(L"GetAuthData GetData NULL\n");
			g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u SID:%u] SendGPKData Encode Error!!\r\n", GetAccountDBID(), GetSessionID());
			return;
		}
		AddSendData(SC_SYSTEM, eSystem::SC_CHN_GPKDATA, (char*)&packet, sizeof(packet));
	}
}

void CDNUserConnection::SendGPKAuthData()
{
	if (!g_Config.pGpkCmd) return;

	SCGPKAuthData GPKData;
	memset(&GPKData, 0, sizeof(SCGPKAuthData));

	const unsigned char *p = NULL;
	int nLen = g_Config.pGpkCmd->GetAuthData(&p, NULL, NULL);

	memcpy(GPKData.Data, p, sizeof(GPKData.Data));
	GPKData.nLen = nLen;

	AddSendData(SC_SYSTEM, SC_CHN_GPKAUTHDATA, (char*)&GPKData, sizeof(SCGPKAuthData));
}

#endif	// _HSHIELD

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
void CDNUserConnection::QueryAddRestraintIP(const WCHAR * pwszReason, const char * pszIP)
{
	if (pwszReason == NULL || pszIP == NULL)
	{
		_DANGER_POINT();
		return;
	}

	g_pSQLMembershipManager->QueryAddBlockedIP(GetWorldSetID(), GetAccountDBID(), pszIP);
	DetachConnection( const_cast<wchar_t*>(pwszReason) );
	g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, SID:%u] QueryAddRestraintIP Restraint \r\n", m_nAccountDBID, m_nSessionID);
}
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

int CDNUserConnection::ConvertNexonAuthResult(int nAuthResult)
{
	switch (nAuthResult)
	{
	case 0: return ERROR_NONE;									// AUTHCHECK_ERROR_OK 	= 0, //	0: OK
	case 1: return ERROR_LOGIN_AUTHCHECK_INVALID_ARGUMENT;		// AUTHCHECK_ERROR_INVALID_ARGUMENT,					//	1: Argument is not valid
	case 2: return ERROR_LOGIN_AUTHCHECK_INVALID_PASSPORT;		// AUTHCHECK_ERROR_INVALID_PASSPORT,					//	2: NexonPassport is not valid
	case 3: return ERROR_LOGIN_AUTHCHECK_EXPIRED;				// AUTHCHECK_ERROR_EXPIRED = AUTHCHECK_ERROR_TIMEOUT,	//	3: NexonPassport is valid, but arleady expired
	case 4: return ERROR_LOGIN_AUTHCHECK_AUTH_SERVER_SHUTDOWN;	// AUTHCHECK_ERROR_AUTH_SERVER_SHUTDOWN,				//	4: Authentication server is shutdown
	case 5: return ERROR_LOGIN_AUTHCHECK_UNSUPPORTED_LOCALE;	// AUTHCHECK_ERROR_UNSUPPORTED_LOCALE,					//	5: Unsupported Locale ID
	case 6: return ERROR_LOGIN_AUTHCHECK_SET_LOCALE_NEEDED;		// AUTHCHECK_ERROR_SET_LOCALE_NEEDED,					//	6: call AuthCheck_SetLocale first
	case 7: return ERROR_LOGIN_AUTHCHECK_NO_SERVER_INFORMATION;	// AUTHCHECK_ERROR_NO_SERVER_INFORMATION,				//	7: no server information in config file
	case 8: return ERROR_LOGIN_AUTHCHECK_USER_IP_MISMATCHED;	// AUTHCHECK_ERROR_USER_IP_MISMATCHED,					//	8: client ip address is mismatched with sso servers data
	case 9: return ERROR_LOGIN_AUTHCHECK_NO_DECRYPTION_KEY;		// AUTHCHECK_ERROR_NO_DECRYPTION_KEY,					//	9: There is no passport decryption key. Check AuthCheck_LoadKey function or key file.
	case 10: return ERROR_LOGIN_AUTHCHECK_LOW_LEVEL_PASSPORT;	// AUTHCHECK_ERROR_LOW_LEVEL_PASSPORT,					//	10: passport level is lower than minimum passport level
	case 11: return ERROR_LOGIN_AUTHCHECK_GAMECODE_MISMATCHED;	// AUTHCHECK_ERROR_GAMECODE_MISMATCHED,					//	11: Mismatch between the caller's gamecode and the passport's gamecode.
	case 12: return ERROR_LOGIN_AUTHCHECK_SESSION_NOT_EXIST;	// AUTHCHECK_ERROR_SESSION_NOT_EXIST,					//	12: User session data not exists. ( Maybe timeout or something. )
	case 13: return ERROR_LOGIN_AUTHCHECK_DISCONNECTED;			// AUTHCHECK_ERROR_DISCONNECTED,						//	13: User IP or session key missmatched with current login session. Maybe logged out by another session.
	case 100: return ERROR_LOGIN_AUTHCHECK_SERVER_FAILED;		// AUTHCHECK_ERROR_SERVER_FAILED	= 100,				//	100: Internal error.
	}

	return ERROR_LOGIN_FAIL;
}

void CDNUserConnection::CheckPCBangIp()
{
#if defined(_CH) || defined(_TW) || defined(_SG)
	// pcbang체크
	g_pSQLMembershipManager->QueryCheckPCRoomIP(GetIp(), m_bPCBang, m_cPCBangGrade);
#endif	// #if defined(_CH) || defined(_TW) || defined(_SG)
}

#ifdef PRE_ADD_DOORS
//Doors
void CDNUserConnection::SendMobileAuthenticationKey(const char * szKeys, int nRetCode, bool bFlag)
{
	if (szKeys == NULL || strlen(szKeys) < Doors::Common::AuthenticationKey_LenMax - 1) return;

	SCDoorsGetAuthKey packet;
	memset(&packet, 0, sizeof(SCDoorsGetAuthKey));

	packet.nRetCode = nRetCode;
	packet.bMobileAuthentication = bFlag;
	MultiByteToWideChar(CP_ACP, 0, szKeys, -1, packet.wszAuthKey, Doors::Common::AuthenticationKey_LenMax);

	AddSendData(SC_DOORS, eDoors::SC_GET_AUTHKEY, (char*)&packet, sizeof(SCDoorsGetAuthKey));
}

void CDNUserConnection::SendMobileCancelAuthentication(int nRetCode)
{
	SCDoorsCancelAuth packet;
	memset(&packet, 0, sizeof(SCDoorsCancelAuth));

	packet.nRetCode = nRetCode;

	AddSendData(SC_DOORS, eDoors::SC_CANCEL_AUTH, (char*)&packet, sizeof(SCDoorsCancelAuth));
}

void CDNUserConnection::SendMobileAuthenticationFlag(bool bFlag)
{
	SCDoorsGetAuthFlag packet;
	memset(&packet, 0, sizeof(SCDoorsGetAuthFlag));

	packet.bFlag = bFlag;

	AddSendData(SC_DOORS, eDoors::SC_GET_MOBILEAUTHFLAG, (char*)&packet, sizeof(SCDoorsGetAuthFlag));
}
#endif		//#ifdef PRE_ADD_DOORS