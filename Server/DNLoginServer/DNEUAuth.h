#pragma once

#if defined(_EU) && defined(_FINAL_BUILD)

class CDNEUAuth
{
public:
	CDNEUAuth(void);
	~CDNEUAuth(void);

	bool Init();
	void Authen(unsigned __int64 nSessionId, const char *pszUsername, const char *pszPassword, const char *pszClientip);
	void Terminate();
};

extern CDNEUAuth *g_pEUAuth;

#endif	// #if defined(_EU)