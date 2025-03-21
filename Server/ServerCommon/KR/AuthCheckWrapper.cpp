
#pragma once

#include "stdafx.h"
#include "AuthCheckWrapper.h"
#include "Log.h"

#if defined(_KR)

AuthCheckError NexonKorAuthCheckWrapper::_AuthCheck_LocalCheckPassport(
	IN		const TCHAR*	szPassport,			// 1024
	OUT		TCHAR*			pszId,				// 32
	OUT		UINT32*			pnNexonSN,
	OUT		TCHAR*			pszLoginIp,			// 32
	OUT		UINT32*			puSex,
	OUT		UINT32*			puAge,
	OUT		UINT32*			puPwdHash,
	OUT		UINT32*			puSsnHash,
	OUT		UINT32*			puFlag0,
	OUT		UINT32*			puFlag1,
	OUT		UINT32*			pnServerHash,
	OUT		TCHAR*			pszServerName,		// 32
	OUT		TCHAR*			pszNationCode,		// 3
	OUT		DWORD*			pRetVal
	)
{
	AuthCheckError aResult = AUTHCHECK_ERROR_INVALID_ARGUMENT;

	__try {
#if defined(_WIN32)
		aResult = AuthCheck_LocalCheckPassport(szPassport, pszId, pnNexonSN, pszLoginIp, puSex, puAge, puPwdHash, puSsnHash, puFlag0, puFlag1, pnServerHash, pszServerName, pszNationCode);
#elif defined(_WIN64)
		
#endif	// _WIN64
	}
	__except(((pRetVal)?((*pRetVal) = GetExceptionCode()):(EXCEPTION_EXECUTE_HANDLER)), EXCEPTION_EXECUTE_HANDLER) {
		g_Log.Log( LogType::_ERROR, L"[NexonKorAuthCheck] AuthCheck_LocalCheckPassport (Exception Code : 0x%X)\r\n", (pRetVal)?((*pRetVal)):(0));
	}

	return aResult;
}

#endif	// #if defined(_KR)
