
#pragma once

#if defined(_HSHIELD)

class HShieldSvrWrapper
{

public:
	static AHNHS_CLIENT_HANDLE __stdcall AhnHS_CreateClientObject(
		IN AHNHS_SERVER_HANDLE hServer, 
		OUT DWORD* pRetVal /* = NULL */ /* 성공 : NOERROR / 실패 : 그외 (예외번호) */
		);
	static void __stdcall AhnHS_CloseClientHandle(
		IN AHNHS_CLIENT_HANDLE hClient, 
		OUT DWORD* pRetVal /* = NULL */ /* 성공 : NOERROR / 실패 : 그외 (예외번호) */
		);
	static unsigned long __stdcall AhnHS_VerifyResponseEx(
		IN AHNHS_CLIENT_HANDLE hClient, 
		IN unsigned char *pbyResponse, 
		IN unsigned long nResponseLength, 
		OUT unsigned long *pnErrorCode, 
		OUT DWORD* pRetVal /* = NULL */ /* 성공 : NOERROR / 실패 : 그외 (예외번호) */
		);
	static unsigned long __stdcall AhnHS_MakeRequest(
		IN AHNHS_CLIENT_HANDLE hClient, 
		OUT PAHNHS_TRANS_BUFFER pRequestBuffer, 
		OUT DWORD* pRetVal /* = NULL */ /* 성공 : NOERROR / 실패 : 그외 (예외번호) */
		);
	static AHNHS_SERVER_HANDLE __stdcall AhnHS_CreateServerObject(
		IN const char *pszFilePath, 
		OUT DWORD* pRetVal /* = NULL */ /* 성공 : NOERROR / 실패 : 그외 (예외번호) */
		);
	static void __stdcall AhnHS_CloseServerHandle(
		IN AHNHS_SERVER_HANDLE hServer, 
		OUT DWORD* pRetVal /* = NULL */ /* 성공 : NOERROR / 실패 : 그외 (예외번호) */
		);

};

#endif	// #if defined(_HSHIELD)
