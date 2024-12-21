// Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>


#include "GPKitClt.h"
#include "GPKitSvr.h"

using namespace SGPK;

const char * GetRootPath(void)
{
    static char szPath[MAX_PATH];
    static bool bFirstTime = true;

    if(bFirstTime)
    {
        bFirstTime = false;
        GetModuleFileName(NULL, szPath, sizeof(szPath));
        char *p = strrchr(szPath, '\\');
        *p = '\0';
    }

    return szPath;
}


int _tmain(int argc, _TCHAR* argv[])
{
    
	
	IGPKCltDynCode * pCltDynCode = GPKStart(NULL, "GPKDEMO");

	//gpk-client need update
	//IGPKCltDynCode * pCltDynCode2 = GPKStart("http://127.0.0.1/Updates/TestGame", "GPKDEMO");


	if(!pCltDynCode)
	{
		printf("GPK Start failed\n");
		goto fail_ret;
	}
	
	

	IGPKSvrDynCode * pSvrDynCode = GPKCreateSvrDynCode();
    if(NULL == pSvrDynCode)
    {
        printf("Create SvrDynCode component failed\n");
        goto fail_ret;
    }

    char szSvrDir[MAX_PATH];
    char szCltDir[MAX_PATH];

#ifdef BIT64
    sprintf(szSvrDir, "%s\\DynCodeBin\\Server64", GetRootPath());
#else
    sprintf(szSvrDir, "%s\\DynCodeBin\\Server", GetRootPath());
#endif

    sprintf(szCltDir, "%s\\DynCodeBin\\Client", GetRootPath());

    int nBinCount = pSvrDynCode->LoadBinary(szSvrDir, szCltDir);
    printf("Load Binary: %d binary are loaded\n", nBinCount);

    if(0 == nBinCount)
        goto fail_ret;

    const unsigned char *pCode = NULL;
    int nCodeIdx = pSvrDynCode->GetRandIdx();
    int nCodeLen = pSvrDynCode->GetCltDynCode(nCodeIdx, &pCode);
    if(nCodeLen < 0)
        goto fail_ret;

	//send data of pCode to Client

    pCltDynCode->SetDynCode(pCode, nCodeLen);

    char Data[256];
    strcpy(Data, "Hello World!");
    int nDataLen = (int)strlen(Data) + 1;
    
    printf("Data Before Encode: %s\n", Data);
    if(false == pCltDynCode->Encode((unsigned char*)Data, nDataLen))
    {
        printf("Encode fail\n");
        goto fail_ret;
    }
    printf("Encode Succeed\n");

    if(false == pSvrDynCode->Decode((unsigned char*)Data, nDataLen, nCodeIdx))
    {
        printf("Decode fail\n");
        goto fail_ret;
    }
    printf("Decode Succeed\n");
    printf("Data after decode: %s\n", Data);

    pSvrDynCode->Release();
    pCltDynCode->Release();

    return 0;

fail_ret:

    if(pSvrDynCode != NULL)
        pSvrDynCode->Release();

    if(pCltDynCode != NULL)
        pCltDynCode->Release();

	return 1;
}
