#ifndef _I_SDOA_AUTH_BUSINESS_H_
#define _I_SDOA_AUTH_BUSINESS_H_
#include "ISDOAuthCallBack.h"

typedef enum
{
	EM_SDO_UserInfo_FirstNeedFullInfo=13001,
	EM_SDO_UserInfo_FullInfoUrl=13002,
	EM_SDO_UserInfo_SecondNeedFullInfo=13003,
}EUserInfoKey;

/*
-10242001	Load the configuration file failed
-10242002	Load the service description file failed
-10242003	Authorization request to the server configuration failed
-10242004	Local connection timed out
-10242008	Server connection failed
-10242005	Package SP information failed 
-10242006	Package request information failed
-10242007	Unpackaged message response failed
-10242400	request invalid
-10242401	unregistered
-10242403	Request rejected
-10242404	Requested service not found
-10242405	The request message does not supported
-10242408	Request timeout
-10242410	Request failed
-10242504	Request handled time-out
*/

interface ISDOAuthBusiness
{
	SNDAMETHOD(int)  Init(const char* pConfig)=0;

	SNDAMETHOD(void) SetCallBack(SSDOAuthCallBack * pCbObj)=0;

	SNDAMETHOD(int)  AsyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
		SKeyValue *pAdditional, int nAdditional,unsigned int *dwRpcId)=0;

	SNDAMETHOD(int)  SyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
		SKeyValue *pAdditional, int nAdditional, SKeyValue ** ppKeyValue, int *nKeyValueNum)=0;
	SNDAMETHOD(void)  SyncFreeResult(SKeyValue * pResult, int nKeyValueNum) = 0;
	
};
SNDAAPI(ISDOAuthBusiness*) CreateSDOAuthBusinessInstance();
#endif

