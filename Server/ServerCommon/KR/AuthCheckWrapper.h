
#pragma once

#if defined(_KR)

class NexonKorAuthCheckWrapper
{

public:
	static AuthCheckError _AuthCheck_LocalCheckPassport(
		IN		const TCHAR*	szPassport,					// 1024
		OUT		TCHAR*			pszId			= NULL,		// 32
		OUT		UINT32*			pnNexonSN		= NULL,
		OUT		TCHAR*			pszLoginIp		= NULL,		// 32
		OUT		UINT32*			puSex			= NULL,
		OUT		UINT32*			puAge			= NULL,
		OUT		UINT32*			puPwdHash		= NULL,
		OUT		UINT32*			puSsnHash		= NULL,
		OUT		UINT32*			puFlag0			= NULL,
		OUT		UINT32*			puFlag1			= NULL,
		OUT		UINT32*			pnServerHash	= NULL,
		OUT		TCHAR*			pszServerName	= NULL,		// 32
		OUT		TCHAR*			pszNationCode	= NULL,		// 3
		OUT		DWORD*			pRetVal			= NULL		// 성공 : NOERROR / 실패 : 그외 (예외번호)
		);

};

#endif	// #if defined(_KR)
