
#pragma once

#if defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)

#include "Log.h"

class CDNNHNNetCafe
{
private:
	DWORD m_dwReconnectTick;
	std::vector <std::string> m_vUserLogOutPool;
	void PushUserLogOutPool(const char * pszUserID);
	void FlushUserLogOutPool();

public:
	CDNNHNNetCafe();
	~CDNNHNNetCafe();

	void Reconnect(DWORD CurTick);

	bool NetCafe_Initialize();
	void NetCafe_Finalize();

	bool NetCafe_UserLogin(const char * pszUserID, const char * pszIP, bool &bNetCafeIP);
	bool NetCafe_UserLogOut(const char * pszUserID);

	bool NetCafe_GetNetCafeCode(const char * pszUserID, char * pszOutBuffer, int nSize);
	bool NetCafe_GetProductCode(const char * pszUserID, char * pszOutbuffer, int nSize);
};

extern CDNNHNNetCafe * g_pNHNNetCafe;

#endif