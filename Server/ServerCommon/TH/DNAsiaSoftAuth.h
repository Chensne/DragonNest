
#pragma once

#include "Connection.h"

#if defined(_TH) && defined(_FINAL_BUILD)

class CDNAsiaSoftAuth : public CConnection
{
public:
	CDNAsiaSoftAuth(const char * pszIP, int nPort);
	~CDNAsiaSoftAuth(void);

	void Reconnect(DWORD CurTick);
	int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	int AddSendData(char *pData, int nLen);

	int SendLogin(const char *pAccountName, const char *pPassword, const char* pDomain, const char *pIp, UINT nSessionID);

#if !defined( _FINAL_BUILD )
	void AsiaAuthPerfTest();
#endif		//#if !defined( _FINAL_BUILD )

private:
	DWORD m_dwReconnectTick;

	int _ConvertAuthResultCode(int RetCode);
	void _Tokenize(const char * pstr, std::vector<std::string>& tokens, const std::string& delimiters, int nSrcsize);
	bool _IsDelimiters(const char * pstr, const std::string& delimiters);

#if !defined( _FINAL_BUILD )
	struct _PERFTEST
	{
		DWORD dwQueryTime;
		std::string strID;
		UINT nSessionID;

		_PERFTEST()
		{
			dwQueryTime = 0;
			strID.clear();
			nSessionID = 0;
		}
	};
	std::vector <CDNAsiaSoftAuth::_PERFTEST> m_PerfTestIDs;
#endif		//#if !defined( _FINAL_BUILD )
};

extern CDNAsiaSoftAuth* g_pAsiaSoftAuth;

#endif	// #if defined(_TW)
