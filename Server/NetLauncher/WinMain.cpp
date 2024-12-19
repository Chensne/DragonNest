
#include "stdafx.h"
#include "resource.h"
#include "ProcessManager.h"
#include "launchersession.h"
#include "MiniDump.h"
#include "DNConfig.h"
#include "IniFile.h"
#include "Version.h"
#include <io.h>
#include "BatchManager.h"
#include "LogCleaner.h"

TNetLauncherConfig g_Config;

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE :
			{
				CLogCleaner cleaner;
				cleaner.CleanLog();
			}
			break;

		case WM_CLOSE :
			if (MessageBox(hWnd, _T("Quit Launcher ?"), _T("NetLauncher"), MB_YESNO) == IDNO)
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_INITDIALOG: {
			return TRUE;
		}
	}
	return FALSE;
}

static HWND s_hDlg;

CSyncLock g_Textsync;
void TextOut(const TCHAR * format, ...)
{
	g_Textsync.Lock();
	static TCHAR msg[4096*2];
	TCHAR msg2[4096*2];
	int i, j, l;
	TCHAR text[4096*2];
	va_list ap;
	time_t ltime;
	struct tm * t = new struct tm;
	time(&ltime);
	localtime_s(t, &ltime);
	va_start (ap, format);
	_vstprintf_s(text, format, ap);
	va_end (ap);

	_stprintf_s(msg2, _T("%02d/%02d %02d:%02d:%02d %s\r\n"), t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, text);
	l = (int)_tcslen(msg2);

	for(i=0, j=0; msg[i]; i++)
		if (msg[i] == '\r' && i + l + 1< sizeof(msg)/sizeof(TCHAR))
			j = i;

	if (i + l + 1 < sizeof(msg)/sizeof(TCHAR))
		j = i;

	if (j > 0)
	{
		memcpy(msg2+l, msg, j*sizeof(TCHAR));
		msg2[l+j] = '\0';
	}

	_tcscpy_s(msg, msg2);

	SetWindowText(GetDlgItem(s_hDlg, IDI_LOG), msg);
	delete t;
	g_Textsync.UnLock();
}

bool LoadConfig(const char * pPath)
{
	GetCurrentDirectoryA(sizeof(g_Config.szExcutePath), g_Config.szExcutePath);

	char szPath[1024];
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "./Config/DNNetLauncher.ini");

	if (_access(szPath, 0) == -1)
		return false;
	
	g_IniFile.Open(szPath);
	g_IniFile.GetValue("ServiceManager", "IP", g_Config.szServiceManagerIP, _countof(g_Config.szServiceManagerIP));
	g_IniFile.GetValue("ServiceManager", "Port", &g_Config.nServiceManagerPort);
	strcpy_s(g_Config.szVersion, szNetLauncherVersion);
	return true;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wcl;
	HWND hWnd, hDlg;
	MSG msg;

	if (FindWindow(_T("netlauncher"), NULL) != NULL)
		return 0;

	wcl.style = 0;
	wcl.lpfnWndProc = WinProc;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hInstance = hInst;
	wcl.hIcon = NULL;
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = _T("netlauncher");
	
	if (!RegisterClass(&wcl))
	{
		_ASSERT(0);
		return 0;
	}

	int style = (WS_OVERLAPPEDWINDOW|WS_SYSMENU)&(~(WS_THICKFRAME|WS_MAXIMIZEBOX));
	RECT rect;

	hWnd = CreateWindow(_T("netlauncher"), _T("netlauncher"), style,
		CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,
		NULL, NULL, hInst, NULL);

#ifdef _UNICODE
	s_hDlg = hDlg = CreateDialog(hInst, (LPCWSTR)IDD_DIALOG1, hWnd, DlgProc);
#else
	s_hDlg = hDlg = CreateDialog(hInst, (LPCSTR)IDD_DIALOG1, hWnd, DlgProc);
#endif
	GetClientRect(hDlg, &rect);
	AdjustWindowRect(&rect, style, FALSE);

	MoveWindow(hWnd, 100, 100, rect.right-rect.left, rect.bottom-rect.top, TRUE);
	ShowWindow(hWnd, SW_NORMAL);

	SetMiniDump();
	CProcessManager::GetInstance()->Start();
	if (LoadConfig(lpCmdLine) == false)
	{
		_ASSERT_EXPR(0, L"Fail LoadConfig");
		return 0;
	}

	if (CLauncherSession::GetInstance()->Start(g_Config.szServiceManagerIP, g_Config.nServiceManagerPort) == false)
	{
		_ASSERT_EXPR(0, L"Fail CLauncherSession::GetInstance()->Start");
		return 0;
	}

	CBatchManager::GetInstance()->Start();

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	CProcessManager::GetInstance()->Stop();
	CLauncherSession::GetInstance()->Stop();
	CBatchManager::GetInstance()->Stop();

	return 0;
}