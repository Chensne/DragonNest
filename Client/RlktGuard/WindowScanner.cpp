#include <Windows.h>
#include <list>
#include <string>

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

std::list<std::wstring> window;
std::list<std::wstring> windowclass;

void LoadWindowNameData()
{
		window.push_back(L"Immunity Debugger - [CPU]");
		window.push_back(L"Cheat Engine 6");
		window.push_back(L"OllyDbg - [CPU]");
		window.push_back(L"DragonShark 1.0.0.0");
		window.push_back(L"Cheat Engine 5.0");
		window.push_back(L"Cheat Engine 5.1");
		window.push_back(L"Cheat Engine 5.1.1");
		window.push_back(L"Cheat Engine 5.2");
		window.push_back(L"Cheat Engine 5.3");
		window.push_back(L"Cheat Engine 5.4");
		window.push_back(L"Cheat Engine 5.5");
		window.push_back(L"Cheat Engine 5.6");
		window.push_back(L"Cheat Engine 5.6.1");
		window.push_back(L"Cheat Engine 6.0");
		window.push_back(L"Cheat Engine 6.1");
		window.push_back(L"Cheat Engine 6.2");
		window.push_back(L"Cheat Engine 6.3");
		window.push_back(L"Cheat Engine 6.4");
		window.push_back(L"Cheat Engine 6.5");
		window.push_back(L"Cheat Engine");
		window.push_back(L"Sandiebox");
		window.push_back(L"Sandiebox Controwindow.push_back(L");
		window.push_back(L"Run Sandboxed");
		window.push_back(L"Process Explorer");
		window.push_back(L"Process Explorer 11.33");
		window.push_back(L"ArtMoney SE v7.44");
		window.push_back(L"ArtMoney SE v8.00");
		window.push_back(L"ArtMoney SE v7.43");
		window.push_back(L"ArtMoney SE v7.42");
		window.push_back(L"ArtMoney SE v7.41");
		window.push_back(L"ArtMoney SE v7.40");
		window.push_back(L"ArtMoney SE v7.39");
		window.push_back(L"ArtMoney SE v7.38");
		window.push_back(L"ArtMoney SE v7.37");
		window.push_back(L"ArtMoney SE v7.36");
		window.push_back(L"ArtMoney SE v7.35");
		window.push_back(L"ArtMoney SE v7.34");
		window.push_back(L"ArtMoney SE v7.33");
		window.push_back(L"ArtMoney SE v7.32");
		window.push_back(L"ArtMoney SE v7.31");
		window.push_back(L"Art*Mo*ney");
		window.push_back(L"OllyDbg");
		window.push_back(L"TCPView");
		window.push_back(L"Sysinternals");
		window.push_back(L"�û���½");
		window.push_back(L"˽������������(��֤ͨ��������ʹ��)_");
		window.push_back(L"http://lzgxx.com");
		window.push_back(L"13008655209");
		window.push_back(L"����������ϷIP");
		window.push_back(L"���");
		window.push_back(L"zhuru��");
		window.push_back(L"byЦЦ");
		window.push_back(L"rPE");
		window.push_back(L"rEdoX");
		window.push_back(L"Packet");
}

void LoadWindowClassData()
{
		windowclass.push_back(L"DNXXGPKGOGO");
		windowclass.push_back(L"WTWindow");
		windowclass.push_back(L"DNXXGPK");
		windowclass.push_back(L"DNXX");
}

bool TitleWindow(LPWSTR WindowTitle){
	//VIRTUALIZER_DOLPHIN_WHITE_START
	HWND WinTitle = FindWindowW(NULL, WindowTitle);
	if (WinTitle > 0){
		SendMessage(WinTitle, WM_CLOSE, 0, 0);
		return false;
	}
	//VIRTUALIZER_DOLPHIN_WHITE_END
	return true;
}


void doWindowScan()
{
	//EnumWindows(EnumWindowsProc, NULL);

	/*for (std::list<std::wstring>::iterator it = window.begin(); it != window.end(); it++)
	{
		//TitleWindow((*it).c_str()); 
	}*/
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	wchar_t wclass[255];
	wchar_t title[255];

	GetWindowTextW(hwnd, title, sizeof(title));
	GetClassNameW(hwnd, wclass, sizeof(wclass));

	for (std::list<std::wstring>::iterator it = window.begin(); it != window.end(); it++)
	{
		if (wcsstr(title, (*it).c_str()) != 0)
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
	}

	for (std::list<std::wstring>::iterator it = windowclass.begin(); it != windowclass.end(); it++)
	{
		if (wcsstr(wclass, (*it).c_str()) != 0)
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
	}

	return TRUE;
}