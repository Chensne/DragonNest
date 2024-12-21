#pragma once

#ifdef _HSHIELD

#define WM_ALERT_HSHIELD	WM_USER + 101

class HShieldSetup
{
public:
	static int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam);
	static int Init(HWND hWnd);
	static int Update();
	static void SetMyID(const WCHAR* szUserID);
	static int Start();
	static int Stop();
	static void OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen);

	static std::wstring GetLastErrorMsg() { return m_szLastErrorMsg; }
	static void HSErrorMessageBox();

	static UINT __stdcall ForceQuitThread( void *pParam );

private:
	struct Info
	{
		TCHAR	m_szHShieldPath[MAX_PATH];
		TCHAR	m_szFullFileName[MAX_PATH];
		int		m_nServerPort;
		char	m_szServerIP[20];
		TCHAR*	m_szWinTitle;
		TCHAR	m_ID[256]; 
		DWORD	m_dwMainThreadID;
		HWND	m_hWnd;
	};

	static Info m_Info;
	static std::wstring m_szLastErrorMsg;
	static HANDLE m_hForceQuitThread;
	static unsigned m_dwForceQuitThreadID;
	static bool m_bStartMonitor;
};

#endif //_HSHIELD