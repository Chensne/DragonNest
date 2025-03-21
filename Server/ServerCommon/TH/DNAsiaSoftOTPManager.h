#pragma once

#if defined(_TH) && defined(_FINAL_BUILD)

class CDNAsiaSoftOTP;
class CDNAsiaSoftOTPManager
{
public:
	struct Common
	{
		enum eCommon
		{
			None = 0,
		};
	};

	CDNAsiaSoftOTPManager();
	virtual ~CDNAsiaSoftOTPManager();	

	bool Init(const char * pszIP, int nPort, int nWorkerThreadSize);

	bool SendOTP(const char * pDomain, const char * pMasterAkeyID, const char * pszResponse, UINT nSessionID);

private:
	char m_szIP[IPLENMAX];
	short m_nPort;

	int m_nConnectionCount;
	std::vector <CDNAsiaSoftOTP*> m_vAsiaSoftOTPList;

	CDNAsiaSoftOTP * _GetFreeOTP();
};

extern CDNAsiaSoftOTPManager * g_pAsiaSoftOTPManager;

#endif		//#ifdef _TH