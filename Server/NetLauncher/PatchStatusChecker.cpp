#include "StdAfx.h"
#include "PatchStatusChecker.h"
#include "LauncherSession.h"

CPatchStatusChecker::CPatchStatusChecker(CLauncherSession* pSession)
: m_pSession(pSession)
{
}

CPatchStatusChecker::~CPatchStatusChecker()
{
}

HRESULT CPatchStatusChecker::OnProgress( 
		/* [in] */ ULONG ulProgress,
		/* [in] */ ULONG ulProgressMax,
		/* [in] */ ULONG ulStatusCode,
		/* [in] */ LPCWSTR szStatusText)
{
	static ULONG prevStatus = 0;
	static DWORD prevTick = 0;

	switch (ulStatusCode)
	{
	case BINDSTATUS_BEGINDOWNLOADDATA:
	case BINDSTATUS_DOWNLOADINGDATA:
		{
			DWORD currentTick = GetTickCount();
			if (currentTick - prevTick <= 200)
				break;

			prevTick = currentTick;

			m_pSession->SendPatchProgress(key.c_str(), ulProgress, ulProgressMax);
			prevStatus = ulStatusCode;
		}
		break;

	case BINDSTATUS_ENDDOWNLOADDATA:
		if (prevStatus == BINDSTATUS_DOWNLOADINGDATA)
			m_pSession->SendPatchProgress(key.c_str(), ulProgress, ulProgressMax);
		break;
	
	default:
		break;
	}

	return S_OK;
}