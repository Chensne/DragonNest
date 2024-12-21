
#pragma once

#include "Connection.h"
#include "CriticalSection.h"

class CPatcherSession : public CConnection
{
public:
	CPatcherSession();
	~CPatcherSession();

	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);

	void SendConnectedResult(const WCHAR * pBaseUrl, const WCHAR * pPatchUrl);
	void SendStartNetLauncher();
	void SendStopNetLauncher();
	bool SendNetLauncherPatch();

	void SetPatcherServiceID(int nPSID) { m_nPSID = nPSID; }
	int GetPSID() { return m_nPSID; }

protected:
	int m_nPSID;
};