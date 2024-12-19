#pragma once

#include "HttpClient.h"

class CHttpClientManager
{
public:
	typedef std::vector<CHttpClient*> TVecHttpClient;
	typedef std::queue<CHttpClient*> TQueueHttpClient;
	typedef std::map<DWORD, CHttpClient*> TMapHttpClient;

	CHttpClientManager();
	virtual ~CHttpClientManager();	

	void CreateHttpClinet(int nWorkerThreadSize, const TCHAR* szServerAddress, const TCHAR* szUserName=NULL, const TCHAR* szPassWord=NULL);
	
	CHttpClient* FindHttpClient();
	void FreeHttpClient(CHttpClient* pHttpClient);

private:
	CSyncLock m_Sync;
	TVecHttpClient m_pVecHttpClient;
	TQueueHttpClient m_pQueueHttpClient;
	TMapHttpClient m_pMapHttpClient;

	void Clear();
};

extern CHttpClientManager * g_pHttpClientManager;

class CHttpClientAuto
{
public:
	CHttpClientAuto(CHttpClient* pThis, CHttpClientManager* pOwner)
	{		
		m_This = pThis;
		m_Owner = pOwner;
	}
	~CHttpClientAuto()
	{
		if (m_This && m_Owner) {
			m_Owner->FreeHttpClient(m_This);
		}
	}
private:
	CHttpClient* m_This;
	CHttpClientManager* m_Owner;
};
