#include "stdafx.h"
#include "DNBAM.h"
#include "Log.h"

//#if defined(_KR)
//
//CDNBAM* g_pBAM = NULL;
//
//CDNBAM::CDNBAM(void)
//{
//	::CoInitialize(NULL);
//
//	HRESULT hr = m_pObj.CreateInstance(__uuidof(DiagnosticListener));
//	if (SUCCEEDED(hr)) g_Log.Log(L"BAM Success! \r\n");
//	else g_Log.Log(L"BAM Fail! \r\n");
//}
//
//CDNBAM::~CDNBAM(void)
//{
//	::CoUninitialize();
//}
//
//void CDNBAM::CurrentConnections(WCHAR *pServerName, int nCount)
//{
//	m_pObj->IncrementBy(pServerName, 5, nCount);
//}
//
//#endif	// #if defined(_KR)
