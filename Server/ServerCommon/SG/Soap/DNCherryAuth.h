
#pragma once

#if defined (_SG)

namespace CherryAuth
{
#if defined (_WORK)
	//for test
	const int nCherryGameID = 3;
	const static char * pAuthAddr = { "http://116.12.134.130:22011" };
#else
	const int nCherryGameID = 3;	
	const static char * pAuthAddr = { "http://203.116.186.27:22011" };
#endif

	struct AuthResultCode
	{
		enum eAuthResultCode
		{
			None = -12345,			//생성상태
			Fail = 0,
			Success = 1,
			Locked = -1,
			Suspended = -2,
			Others = -999,
		};
	};

	struct TCherryAuthInfo
	{
		std::string strCherryID;				//아이뒤
		std::string strLoginID;					//패쓰
		std::string strToken;					//토큰???
		std::string strEmail;					//이멜
		int nStatusCode;						//현재상태
		std::string strStatusDescription;		//

		TCherryAuthInfo()
		{
			nStatusCode = CherryAuth::AuthResultCode::None;
		};
	};
};

#if defined (PRE_MOD_SG_WITH_MACADDR)
bool CherryAuthenticate(char const *endpoint, int gameID, char const *loginID, char const *processedPassword, char const *ip, char const *pMacAddr, CherryAuth::TCherryAuthInfo &AuthInfo);
#else		//#if defined (PRE_MOD_SG_WITH_MACADDR)
bool CherryAuthenticate(char const *endpoint, int gameID, char const *loginID, char const *processedPassword, char const *ip, CherryAuth::TCherryAuthInfo &AuthInfo);
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)
bool EncodePass(const char * pID, const char * pPass, std::string &out);

void Base64_Encode(const char * pin, int nlen, std::string &out);

#endif	// #if defined (_SG)

