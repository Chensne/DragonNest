#pragma once

#include "Log.h"

#if defined(_CH) && defined(_FINAL_BUILD)

#include "../../Extern/SDOA/Server/Src/sdoa4server.h"
#include "../../Extern/SDOUA/include/ISDOAuthBusiness.h"
#include "../../Extern/SDOUA/include/ISDOAuthCallBack.h"

class CDNShandaAuth
{

public:
	struct UNQIDDAT
	{
	public:
		UNQIDDAT()
		{
			m_UniqueID = 0;
			m_SessionID = 0;
		}
		UNQIDDAT(long pUniqueID, UINT pSessionID)
		{
			DN_ASSERT(0 < pUniqueID,	"Invalid!");
			DN_ASSERT(0 < pSessionID,	"Invalid!");
			
			m_UniqueID = pUniqueID;
			m_SessionID = pSessionID;
		}
	public:
		long m_UniqueID;
		UINT m_SessionID;
	};

	typedef	CSyncLock					TP_LOCK;
	typedef	ScopeLock<TP_LOCK>			TP_LOCKAUTO;

	typedef std::map<long, UNQIDDAT>	TP_LIST;
	typedef TP_LIST::iterator			TP_LIST_ITR;
	typedef TP_LIST::const_iterator		TP_LIST_CTR;

	struct SDOUADAT
	{
	public :
		SDOUADAT()
		{
			m_cIsLimited = 0;
			m_cIsProtected = 0;
			m_SessionID = 0;
		}
		SDOUADAT(char cIsLimited, char cIsProtected, UINT nSessionID)
		{
			m_cIsLimited = cIsLimited;
			m_cIsProtected = cIsProtected; 
			m_SessionID = nSessionID;
		}		
	public :
		char m_cIsLimited;
		char m_cIsProtected;
		UINT m_SessionID;
	};

	typedef std::map<unsigned int, SDOUADAT>	SDOUA_LIST;	

public:
	CDNShandaAuth();
	~CDNShandaAuth();

	bool Open();
	void Close();
	bool IsOpen() const { return(NULL != m_pUserInfoAuthenObj); }

	int AsyncAuthen(LPCSTR pszSession, LPCSTR pszIPAddress, UINT pSessionID);

	long AddUniqueID(UINT pSessionID);
	void DelUniqueID(long pUniqueID);
	bool AtUniqueID(long pUniqueID, UNQIDDAT& pUnqIdDat);

	bool AddSdouaData(UINT pSessionID, char cIsLimited, char cIsProtected, unsigned int nRpcId);
	bool AtSdouaData(unsigned int nRpcId, SDOUADAT& pSdouaDat);
	void DelSdouaData(unsigned int nRpcId);

	bool SDOUAOpen();
	void SDOUAClose();

private:
	TP_LOCK* GetLock() const { return(&m_Lock); }
#if defined(_DEBUG)
	BOOL IsLock() const { return(m_Lock.IsLock()); }
#endif	// _DEBUG
	
private:
	static int (SNDACALLBACK AuthenCallBack)(int nResult, unsigned long ulUniqueID, ISDOAMsg *pMsg);
	static void (SNDACALLBACK GetUserInfoCallBack)(unsigned int dwRpcId,int nResult, SKeyValue *pKeyValue, int nKeyValueNum);

private:
	ISDOAUserInfoAuthen* m_pUserInfoAuthenObj;
	ISDOAuthBusiness* m_pUserInfoBusiness;
	SSDOAuthCallBack m_ObjCb;
	SDOUA_LIST m_SdouaList;
	TP_LOCK m_SdouaLock;

	long m_UniqueID;
	TP_LIST m_List;
	mutable TP_LOCK m_Lock;
};

extern CDNShandaAuth *g_pShandaAuth;

#endif	// _CH