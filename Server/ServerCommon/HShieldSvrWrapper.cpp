
#pragma once

#include "stdafx.h"
#include "HShieldSvrWrapper.h"
#include "Log.h"

#if defined(_HSHIELD)

AHNHS_CLIENT_HANDLE __stdcall HShieldSvrWrapper::AhnHS_CreateClientObject(IN AHNHS_SERVER_HANDLE hServer, OUT DWORD* pRetVal)
{
	AHNHS_CLIENT_HANDLE aResult = ANTICPX_INVALID_HANDLE_VALUE;

	__try {
		aResult = _AhnHS_CreateClientObject(hServer);
	}
	__except(((pRetVal)?((*pRetVal) = GetExceptionCode()):(EXCEPTION_EXECUTE_HANDLER)), EXCEPTION_EXECUTE_HANDLER) {
		g_Log.Log( LogType::_ERROR, L"[HShiledException] _AhnHS_CreateClientObject (Exception Code : 0x%X)\r\n", (pRetVal)?((*pRetVal)):(0));
	}

	return aResult;
}

void __stdcall HShieldSvrWrapper::AhnHS_CloseClientHandle(IN AHNHS_CLIENT_HANDLE hClient, OUT DWORD* pRetVal)
{
	__try {
		_AhnHS_CloseClientHandle(hClient);
	}
	__except(((pRetVal)?((*pRetVal) = GetExceptionCode()):(EXCEPTION_EXECUTE_HANDLER)), EXCEPTION_EXECUTE_HANDLER) {
		g_Log.Log( LogType::_ERROR, L"[HShiledException] _AhnHS_CloseClientHandle (Exception Code : 0x%X)\r\n", (pRetVal)?((*pRetVal)):(0));
	}
}

unsigned long __stdcall HShieldSvrWrapper::AhnHS_VerifyResponseEx(IN AHNHS_CLIENT_HANDLE hClient, IN unsigned char *pbyResponse, IN unsigned long nResponseLength, OUT unsigned long *pnErrorCode, OUT DWORD* pRetVal)
{
	unsigned long aResult = 0;

	__try {
		aResult = _AhnHS_VerifyResponseEx(hClient, pbyResponse, nResponseLength, pnErrorCode);
	}
	__except(((pRetVal)?((*pRetVal) = GetExceptionCode()):(EXCEPTION_EXECUTE_HANDLER)), EXCEPTION_EXECUTE_HANDLER) {
		g_Log.Log( LogType::_ERROR, L"[HShiledException] _AhnHS_VerifyResponseEx (Exception Code : 0x%X)\r\n", (pRetVal)?((*pRetVal)):(0));
	}

	return aResult;
}

unsigned long __stdcall HShieldSvrWrapper::AhnHS_MakeRequest(IN AHNHS_CLIENT_HANDLE hClient, OUT PAHNHS_TRANS_BUFFER pRequestBuffer, OUT DWORD* pRetVal)
{
	unsigned long aResult = 0;

	__try {
		aResult = _AhnHS_MakeRequest(hClient, pRequestBuffer);
	}
	__except(((pRetVal)?((*pRetVal) = GetExceptionCode()):(EXCEPTION_EXECUTE_HANDLER)), EXCEPTION_EXECUTE_HANDLER) {
		g_Log.Log( LogType::_ERROR, L"[HShiledException] _AhnHS_MakeRequest (Exception Code : 0x%X)\r\n", (pRetVal)?((*pRetVal)):(0));
	}

	return aResult;
}

AHNHS_SERVER_HANDLE __stdcall HShieldSvrWrapper::AhnHS_CreateServerObject(IN const char *pszFilePath, OUT DWORD* pRetVal)
{
	AHNHS_SERVER_HANDLE aResult = ANTICPX_INVALID_HANDLE_VALUE;

	__try {
		aResult = _AhnHS_CreateServerObject(pszFilePath);
	}
	__except(((pRetVal)?((*pRetVal) = GetExceptionCode()):(EXCEPTION_EXECUTE_HANDLER)), EXCEPTION_EXECUTE_HANDLER) {
		g_Log.Log( LogType::_ERROR, L"[HShiledException] _AhnHS_CreateServerObject (Exception Code : 0x%X)\r\n", (pRetVal)?((*pRetVal)):(0));
	}

	return aResult;
}

void __stdcall HShieldSvrWrapper::AhnHS_CloseServerHandle(IN AHNHS_SERVER_HANDLE hServer, OUT DWORD* pRetVal)
{
	__try {
		_AhnHS_CloseServerHandle(hServer);
	}
	__except(((pRetVal)?((*pRetVal) = GetExceptionCode()):(EXCEPTION_EXECUTE_HANDLER)), EXCEPTION_EXECUTE_HANDLER) {
		g_Log.Log( LogType::_ERROR, L"[HShiledException] _AhnHS_CloseServerHandle (Exception Code : 0x%X)\r\n", (pRetVal)?((*pRetVal)):(0));
	}
}

#endif	// #if defined(_HSHIELD)
