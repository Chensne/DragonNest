#pragma once

#if defined(_KRAZ) && defined(_FINAL_BUILD)

class CDNActozAuth
{
private:
	DWORD m_dwReconnectTick;

public:
	CDNActozAuth(void);
	~CDNActozAuth(void);

	bool Init();
	void Reconnect(DWORD dwCurTick);
	void SendWebLogin(UINT nSessionID, WCHAR *pwszKeyID, WCHAR *pwszRequestTime, WCHAR *pwszIp);
};

extern CDNActozAuth *g_pActozAuth;

#endif	// #if defined(_KRAZ)