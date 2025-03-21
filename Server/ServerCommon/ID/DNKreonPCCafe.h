#pragma once

#if defined(_ID) && defined(_FINAL_BUILD)
#include "HttpClient.h"

namespace KreonPcCafe
{	
	const static char* ServiceCode = {"DRNEST"};
	const static TCHAR* PCCafeServerAddress = {_T("gwas.gemscool.com")};
	const static TCHAR* PCCafeUrl = {_T("/warnet/premium.kreon")};	
};

class CDNKreonPCCafe
{
private:
	queue<UINT> m_ProcessCalls; //AccountDBID
	CSyncLock m_ProcessLock;

	HANDLE m_hProcessThread;
	bool m_bThreadSwitch;

	CHttpClient m_HttpClient;

	static UINT __stdcall ProcessThread(void *pParam);

public:
	CDNKreonPCCafe();
	~CDNKreonPCCafe(void);

	void Final();	

	int CreateThread();

	void AddProcessCall(UINT uiAccountDBID);
	int SendCheckIPBonus(const char* strMacAddress, const char* ip, const char* key, const DWORD dwKreonCN);
	int ParseErrorCode(char* pResult);
};

extern CDNKreonPCCafe *g_pKreonPCCafe;
#endif	// #if defined(_ID)
