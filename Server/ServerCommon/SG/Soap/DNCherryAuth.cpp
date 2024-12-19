
#include "Stdafx.h"
#include "Util.h"

#if defined (_SG)
#include "../ServerCommon/sg/Soap/DNCherryAuth.h"
#include "../ServerCommon/sg/Soap/AuthSoap/BasicHttpBinding_USCOREIGameAuthentication.nsmap"
#include "../ServerCommon/sg/Soap/AuthSoap/soapBasicHttpBinding_USCOREIGameAuthenticationProxy.h"

#if defined (PRE_MOD_SG_WITH_MACADDR)
bool CherryAuthenticate(char const *endpoint, int gameID, char const *loginID, char const *processedPassword, char const *ip, char const *pMacAddr, CherryAuth::TCherryAuthInfo &AuthInfo)
#else		//#if defined (PRE_MOD_SG_WITH_MACADDR)
bool CherryAuthenticate(char const *endpoint, int gameID, char const *loginID, char const *processedPassword, char const *ip, CherryAuth::TCherryAuthInfo &AuthInfo)
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)
{
	BasicHttpBinding_USCOREIGameAuthentication service;
	service.endpoint = endpoint;
	_ns3__Authenticate query;
	_ns3__AuthenticateResponse ans;
	query.gameID = &gameID;

#if defined (PRE_MOD_SG_WITH_MACADDR)
	std::string strLoginID, strPass, strUserIP, strMacAddress;
#else		//#if defined (PRE_MOD_SG_WITH_MACADDR)
	std::string strLoginID, strPass, strUserIP;
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)

	strLoginID = loginID;
	query.loginID = &strLoginID;

	EncodePass(loginID, processedPassword, strPass);
	query.password = &strPass;

	strUserIP = ip;
	query.userIP = &strUserIP;

#if defined (PRE_MOD_SG_WITH_MACADDR)
	strMacAddress = pMacAddr;
	query.macAddress = &strMacAddress;
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)

	if( service.__ns5__Authenticate(&query, &ans) == SOAP_OK )
	{
		ns3__AccountInfo aix = *ans.AuthenticateResult;

		//check ptr
		if (aix.CherryID == NULL || aix.Email == NULL || aix.LoginID == NULL || aix.StatusCode == NULL || aix.StatusDescription == NULL || aix.Token == NULL)
		{
			_ASSERT_EXPR(0, L"����? �̷��ø� ��� �ȹٷ��� ������");
			return false;
		}

		AuthInfo.strCherryID = (*aix.CherryID);
		AuthInfo.strEmail = (*aix.Email);
		AuthInfo.strLoginID = (*aix.LoginID);
		AuthInfo.nStatusCode = (*aix.StatusCode);
		AuthInfo.strStatusDescription = (*aix.StatusDescription);
		AuthInfo.strToken = (*aix.Token);
	}
	else
	{
		AuthInfo.nStatusCode = -999;
		AuthInfo.strStatusDescription = "FATAL ERROR";		
	}

	return true;
}

bool EncodePass(const char * pID, const char * pPass, std::string &out)
{
	if (pPass == NULL)
		return false;

	bool bMD5 = true;
	int nLen = (int)strlen(pID);	
	for (int i = 0; i < CHERRYLOGINIDMAX && i < nLen && pID[i] != '\0'; i++)
	{
		if (pID[i] == '@')
		{
			bMD5 = false;
			break;
		}
	}

	bMD5 ? MD5Hash(pPass, (int)strlen(pPass), out) : Base64_Encode(pPass, (int)strlen(pPass), out);
	return true;
}

#include <atlenc.h>
void Base64_Encode(const char * pin, int nlen, std::string &out)
{
	int nOutlen = CHERRYLOGINIDMAX;
	char szOut[CHERRYLOGINIDMAX+1];
	memset(szOut, 0, sizeof(szOut));

	BOOL bRet = Base64Encode((const BYTE*)pin, nlen, szOut, &nOutlen);
	if (bRet == TRUE)
	{
		out = szOut;
		return;
	}
	_ASSERT_EXPR(0, L"�̷��ȵ�����~");
}

#endif	// #if defined (_SG) && defined(_FINAL_BUILD)