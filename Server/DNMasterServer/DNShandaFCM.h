#pragma once

#if defined(_CH) && defined(_FINAL_BUILD)

class CDNShandaFCM
{
private:
	CFCMSDKBase* m_pFCM;

public:
	CDNShandaFCM(void);
	~CDNShandaFCM(void);

	bool InitService();

	int FCMQueryState(const char* szUserID, const char* szRoleID, const char *IPAddr, int *OnlineTimeVal, int *OfflineTimeVal);
	int FCMQueryStateNoBlock(const char* szUserID, const char* szRoleID, const char *IPAddr);

	int FCMIDOnline(const char* szUserID, const char* szRoleID, const char *IPAddr);
	int FCMIDOffline(const char* szUserID, const char* szRoleID, const char *IPAddr);
};

extern CDNShandaFCM *g_pShandaFCM;

#endif	// _CH