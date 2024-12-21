#pragma once

#if defined(_ID) && defined(_FINAL_BUILD)

namespace KreonAuth
{
	const static char* ServiceCode = {"DRNEST"};
	const static TCHAR* AuthServerAddress = {_T("gas.gemscool.com")};
	const static TCHAR* AuthUrl = {_T("/member/2/login.kreon")};	
};

int KreonAuthLogin(char const *AccountID, char const *pPassWord, char const *ip, DWORD& KreonCN);
int ParseErrorCode(char* pResult, DWORD& KreonCN);

#endif	// #if defined(_ID)
