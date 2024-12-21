#pragma once

#if defined(_RU) && defined(_FINAL_BUILD)

namespace MailRUAuth
{	
	const static TCHAR* AuthServerAddress = {_T("dn.gas.souz.pvt")};
	const static TCHAR* AuthUrl = {_T("/member/1/login.kreon")};
	const static TCHAR* AuthLoginID = {_T("dn_game_auth")};
	const static TCHAR* AuthPassWord = {_T("KLndva8;935j")};
};

int MailRUAuthLogin(char const *AccountID, char const *pOTPHash, char const *ip);
int ParseErrorCode(char* pResult);

#endif	// #if defined(_ID)
