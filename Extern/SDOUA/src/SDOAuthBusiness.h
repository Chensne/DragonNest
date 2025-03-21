#ifndef _SDOA_AUTH_BUSINESS_H_
#define _SDOA_AUTH_BUSINESS_H_
#include "ISDOAuthBusiness.h"
#include "CommonSDK.h"
class CSDOAuthBusiness:public ISDOAuthBusiness
{
public:
	static CSDOAuthBusiness * GetInstance();
	SNDAMETHOD(int)  Init(const char* pConfig);
	SNDAMETHOD(void) SetCallBack(SSDOAuthCallBack * pCbObj);
	SNDAMETHOD(int)  AsyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
		SKeyValue *pAdditional, int nAdditional, unsigned int *dwRpcId);
	SNDAMETHOD(int)  SyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
		SKeyValue *pAdditional, int nAdditional, SKeyValue **ppKeyValue, int *nKeyValueNum);
	SNDAMETHOD(void)  SyncFreeResult(SKeyValue * pResult, int nKeyValueNum);
private:
	static int SNDACALLBACK RequestCallBack(unsigned int dwRpcId, int nResult, const ISDOCommonMsg* pRequest);
	static int SNDACALLBACK ResponseCallBack(unsigned int dwRpcId, int nResult,const ISDOCommonMsg* pResponse);
private:
	CSDOAuthBusiness();
	~CSDOAuthBusiness();
	static CSDOAuthBusiness * sm_instance;
	ISDOCommonHandler *m_handle;
	static SSDOAuthCallBack *m_pCbObj;
};
#endif



