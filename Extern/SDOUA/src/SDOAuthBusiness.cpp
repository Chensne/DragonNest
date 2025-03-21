#include "SDOAuthBusiness.h"
#include <boost/asio.hpp>
#include <vector>
using std::vector;
#ifdef WIN32
#define strcasecmp _stricmp
#endif

typedef struct stKeyValueInner
{
	unsigned int nKey;
	unsigned char szValue[0];
}SKeyValueInner;
SNDAAPI(ISDOAuthBusiness*) CreateSDOAuthBusinessInstance()
{
	return CSDOAuthBusiness::GetInstance();
}

CSDOAuthBusiness * CSDOAuthBusiness::sm_instance=NULL;
SSDOAuthCallBack * CSDOAuthBusiness::m_pCbObj = NULL;

CSDOAuthBusiness * CSDOAuthBusiness::GetInstance()
{
	if (sm_instance==NULL)
	{
		sm_instance=new CSDOAuthBusiness();
	}
	return sm_instance;
}
CSDOAuthBusiness::CSDOAuthBusiness()
{
	m_handle=sdoCreateCommonHandler();
}
CSDOAuthBusiness::~CSDOAuthBusiness()
{
	m_handle->Release();
}
int CSDOAuthBusiness::Init(IN const char* pConfig)
{
	return m_handle->Initialize(pConfig,CSDOAuthBusiness::RequestCallBack,CSDOAuthBusiness::ResponseCallBack);
}
void CSDOAuthBusiness::SetCallBack(SSDOAuthCallBack * pCbObj)
{
	m_pCbObj=pCbObj;
}

int CSDOAuthBusiness::AsyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
				SKeyValue *pAdditional, int nAdditional, unsigned int *pRpcId)
{
	ISDOCommonMsg * pMsg=sdoCreateRequestMsg("Authen.getUserInfo");
	pMsg->SetValue("userType",nUserIdType);
	pMsg->SetValue("userId",szUserId);
	for(int i = 0; i < nKeyNum; i++)
	{
		int nKey = pKeyId[i];
		pMsg->SetValue("key", nKey);
	}
	for(int i = 0; i < nAdditional; i++)
	{
		SKeyValue *pTemp = &pAdditional[i];
		char szBuffer[1024];
		SKeyValueInner *pReq = (SKeyValueInner*)szBuffer;
		pReq->nKey = htonl(pTemp->nKey);
		strcpy((char *)pReq->szValue, pTemp->pValue);
		pMsg->SetValue("keyValue", (void*)szBuffer, strlen(pTemp->pValue) + sizeof(SKeyValueInner));
	}
	int nResult=m_handle->ASyncSendRequest(pMsg,pRpcId);
	pMsg->Release();
	return nResult;
}
int CSDOAuthBusiness::RequestCallBack(unsigned int dwRpcId, int nResult, const ISDOCommonMsg* pRequest)
{
	return 0;
}
int CSDOAuthBusiness::ResponseCallBack(unsigned int dwRpcId, int nResult,const ISDOCommonMsg* pResponse)
{
	ISDOCommonMsg *pMsg = (ISDOCommonMsg *)pResponse;
	char *pServiceName = pMsg->GetServiceName();
	if(pServiceName != NULL && strcasecmp(pServiceName, "Authen.getUserInfo") == 0)
	{
		vector<SKeyValueInner *> vecKeyValue;
		if (nResult==0 && pResponse!=NULL)
		{
			char *pValue;
			while(1)
			{
				if(pMsg->GetValue("keyValue", &pValue) == 0)
				{
					SKeyValueInner *pTemp = (SKeyValueInner *)pValue;
					pTemp->nKey = ntohl(pTemp->nKey);
					vecKeyValue.push_back(pTemp);
				}
				else 
				{
					break;
				}
			}
		}
		if(vecKeyValue.size() > 0)
		{
			SKeyValue *pKeyValueArr = (SKeyValue *)malloc(sizeof(SKeyValue) * vecKeyValue.size());
			vector<SKeyValueInner*>::iterator itr;
			for(int i = 0; i < vecKeyValue.size(); i++)
			{
				SKeyValueInner *pTemp = vecKeyValue[i];
				
				SKeyValue *pKeyValue = &(pKeyValueArr[i]);
				pKeyValue->nKey = pTemp->nKey;
				pKeyValue->pValue = (char *)malloc(strlen((char *)pTemp->szValue) + 1);
				strcpy(pKeyValue->pValue, (char *)pTemp->szValue);
			}
			m_pCbObj->GetUserInfoCallBack(dwRpcId,nResult, pKeyValueArr, vecKeyValue.size());
			for(int i = 0; i < vecKeyValue.size(); i++)
			{
				SKeyValue *pKeyValue = &(pKeyValueArr[i]);
				free(pKeyValue->pValue);
			}
			free(pKeyValueArr);
		}
		else 
		{
			m_pCbObj->GetUserInfoCallBack(dwRpcId,nResult,NULL, 0);
		}
	}
	return 0;
}

int  CSDOAuthBusiness::SyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
	SKeyValue *pAdditional, int nAdditional, SKeyValue **ppKeyValue, int *nKeyValueNum)
{
	ISDOCommonMsg * pMsg=sdoCreateRequestMsg("Authen.getUserInfo");
	pMsg->SetValue("userType",nUserIdType);
	pMsg->SetValue("userId",szUserId);
	for(int i = 0; i < nKeyNum; i++)
	{
		int nKey = pKeyId[i];
		pMsg->SetValue("key", nKey);
	}
	for(int i = 0; i < nAdditional; i++)
	{
		SKeyValue *pTemp = &pAdditional[i];
		char szBuffer[1024];
		SKeyValueInner *pReq = (SKeyValueInner*)szBuffer;
		pReq->nKey = htonl(pTemp->nKey);
		strcpy((char *)pReq->szValue, pTemp->pValue);
		pMsg->SetValue("keyValue", (void*)szBuffer, strlen(pTemp->pValue) + sizeof(SKeyValueInner));
	}
	ISDOCommonMsg * pResponse=sdoCreateResponseMsg("Authen.getUserInfo");
	int nResult=m_handle->SyncSendRequest(pMsg,pResponse);
	if(nResult == 0)
	{
		vector<SKeyValueInner*> vecKeyValue;
		if (nResult==0 && pResponse!=NULL)
		{
			char *pValue;
			ISDOCommonMsg *pMsg = (ISDOCommonMsg *)pResponse;
			while(1)
			{
				if(pMsg->GetValue("keyValue", &pValue) == 0)
				{

					SKeyValueInner *pTemp = (SKeyValueInner *)pValue;
					pTemp->nKey = ntohl(pTemp->nKey);
					vecKeyValue.push_back(pTemp);
				}
				else 
				{
					break;
				}
			}
		}
		if(vecKeyValue.size() > 0)
		{
			SKeyValue *pKeyValueArr = (SKeyValue *)malloc(sizeof(SKeyValue) * vecKeyValue.size());
			vector<SKeyValue*>::iterator itr;
			for(int i = 0; i < vecKeyValue.size(); i++)
			{
				SKeyValueInner *pTemp = vecKeyValue[i];

				SKeyValue* pKeyValueTemp = &(pKeyValueArr[i]);
				pKeyValueTemp->nKey = pTemp->nKey;
				pKeyValueTemp->pValue = (char *)malloc(strlen((char *)pTemp->szValue) + 1);
				strcpy(pKeyValueTemp->pValue, (char*)pTemp->szValue);
			}
			*ppKeyValue = pKeyValueArr;
			*nKeyValueNum = vecKeyValue.size();
		}
		else 
		{
			*ppKeyValue = NULL;
			*nKeyValueNum  = 0;
		}
	}
	pMsg->Release();
	pResponse->Release();
	return nResult;

}


void CSDOAuthBusiness::SyncFreeResult(SKeyValue * pResult, int nKeyValueNum)
{
	if(pResult && nKeyValueNum > 0)
	{
		for(int i = 0; i < nKeyValueNum; i++)
		{
			SKeyValue *pTemp = &(pResult[i]);
			free(pTemp->pValue);
		}
		free(pResult);
	}
}