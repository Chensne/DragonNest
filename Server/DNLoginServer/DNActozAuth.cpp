#include "StdAfx.h"
#include "DNActozAuth.h"
#include "../ServerCommon/KRAZ/OzMemberSDK.h"
#include "DNUserConnectionManager.h"
#include "DNUserConnection.h"

#if defined(_KRAZ) && defined(_FINAL_BUILD)

CDNActozAuth *g_pActozAuth = NULL;
bool g_bSDKAuth = false;

void __stdcall OnResError(const unsigned __int64 _ui64SeqNum, const int _iErrorCode)
{
	g_Log.Log(LogType::_FILEDBLOG, L"[ActozAuth] OnResError SeqNum:%I64d ErrorCode:%d\r\n", _ui64SeqNum, _iErrorCode);
}

void __stdcall OnServerAuth(const bool _bIsOK, const int _iErrorCode)
{
	if (_bIsOK){
		g_bSDKAuth = true;
		g_Log.Log(LogType::_FILELOG, L"[ActozAuth] OnServerAuth OK!! ErrorCode:%d\r\n", _iErrorCode);
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"[ActozAuth] OnServerAuth Fail!! ErrorCode:%d\r\n", _iErrorCode);
	}
}

void __stdcall OnLoginResult(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiPcRoomID, const LPCWSTR _szToken, const LPCWSTR _szReservedFields, const char _cResultValue, const int _iCSite)
{
#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif

	int nSession = (UINT)_i64SeqNum;
	CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(nSession));
	if (!pUserCon){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, nSession, L"[ActozAuth] [SID:%u] UserCon NULL Result:%d \r\n", nSession);
		return;
	}
	g_Log.Log(LogType::_NORMAL, 0, 0, 0, nSession, L"[ActozAuth] OnLoginResult SessionID:%d Result:%s %d (%d)\r\n", nSession, _szPlayerID, _uiPcRoomID, _cResultValue);

	pUserCon->CheckLoginKRAZ(_cResultValue, _szReservedFields, _szPlayerID);	
}

CDNActozAuth::CDNActozAuth(void): m_dwReconnectTick(0)
{
}

CDNActozAuth::~CDNActozAuth(void)
{
}

bool CDNActozAuth::Init()
{
	HMODULE hDll = NULL;
	hDll = LoadLibraryW(L"OzMemberSDK_x64.dll");
	if (!hDll){
		g_Log.Log(LogType::_FILELOG, L"ActozAuth Dll Load Fail!!\r\n");
		return false;
	}

	Init_SDK = (fnInit_SDK)GetProcAddress(hDll, "Init_SDK");
	ConSock = (fnConSock)GetProcAddress(hDll, "SDKConnect");	//OSP에서 Call할 함수구조체와 접속할 IP와 Port를 Parameter로 받는다..
	OSP_CONNECTION_CHECK = (fnOSP_CONNECTION_CHECK)GetProcAddress(hDll, "OSP_CONNECTION_CHECK");
	WebLogin = (fnWebLogin)GetProcAddress(hDll, "WebLogin");//웹로긴

	funcID.OnResError = &OnResError;	//Requeset Packet에대한 Error값(Error일경우만)
	funcID.OnServerAuth = &OnServerAuth;	//게임서버의 서비스번호및 프로토콜 버전등록에 대한 Response값
	funcID.OnLoginResult = &OnLoginResult;	//로그인 Response값

#define OzMemberSDK_CODE "7058743BA2470DA224C142921939EDEC"

	char szMD5[32+1] = {0,};	 
	//1. Init_SDK Function return: -1 = Fail, 0= Success
	if (Init_SDK(szMD5) == -1){
		g_Log.Log(LogType::_FILELOG, L"ActozAuth Init_SDK Fail!!\r\n");
		return false;
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"ActozAuth Init_SDK OK!!\r\n");
	}
	//2.MD5 not equal, shutdown program
	if (strcmp(OzMemberSDK_CODE, szMD5) != 0) 
	{
		g_Log.Log(LogType::_FILELOG, L"ActozAuth OzMemberSDK_CODE Fail!!\r\n");
		return false;
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"ActozAuth OzMemberSDK_CODE OK!!\r\n");
	}

	int nCon = ConSock(funcID, true);	//함수를 등록하고 DebugMode(확인로그를 남긴다..)값을 넘겨줘 OSP에 접속한다
	if (nCon < 0){
		g_Log.Log(LogType::_FILELOG, L"ActozAuth ConSock Fail!!\r\n");
		return false;
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"ActozAuth ConSock OK!!\r\n");
	}

	return true;
}

void CDNActozAuth::Reconnect(DWORD dwCurTick)
{
	if (m_dwReconnectTick + 5000 < dwCurTick){
		m_dwReconnectTick = dwCurTick;

		if (!g_bSDKAuth){
			int nResult = OSP_CONNECTION_CHECK();
			if (nResult == 0){
				g_bSDKAuth = false;
				g_Log.Log(LogType::_FILEDBLOG, L"ActozAuth OSP_CONNECTION_CHECK Fail!! (Result:%d)\r\n", nResult);
			}
			else{
				g_bSDKAuth = true;
				g_Log.Log(LogType::_FILEDBLOG, L"ActozAuth OSP_CONNECTION_CHECK OK!! (Result:%d)\r\n", nResult);
			}
		}
	}
}

void CDNActozAuth::SendWebLogin(UINT nSessionID, WCHAR *pwszKeyID, WCHAR *pwszRequestTime, WCHAR *pwszIp)
{
	WebLogin(nSessionID, pwszKeyID, pwszRequestTime, pwszIp);
}

#endif	// #if defined(_KRAZ)