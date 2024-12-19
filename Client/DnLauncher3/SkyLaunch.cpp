#ifdef _USA
#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include "resource.h"
#include "SkyLaunch.h"
#include "Settings.h"
#include "www.h"
#include "ShellHook.h"
#include "base64.h"
#include "Define.h"

HWND LoginWindow;
HINSTANCE gHInst;
int gCmdShow;

using namespace std;

std::string ip;
std::string port;
std::string srvPassword;


bool FileExists(std::string path)
{
	std::ifstream f(path);
	if (f)
	{
		return true;
	}
	return false;
}

DWORD WINAPI InitWindow(LPVOID)
{
//	Sleep(2000);

//	MessageBoxA(NULL, "Show magic window.", "DNLogin", MB_OK);
	DialogBox(GetModuleHandle("DNLogin.dll"), MAKEINTRESOURCE(IDC_LOGIN), 0, &WindowController);

//	LoginWindow = CreateDialog(NULL, MAKEINTRESOURCE(ID_LOGINW), 0, WindowController);
//	ShowWindow(LoginWindow, gCmdShow);

	/*MSG  msg;
	int status;
	while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
	{

		if (!IsDialogMessage(LoginWindow, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}*/

	return NULL;
}



BOOL CALLBACK WindowController(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
						  LoginWindow = hwnd;
						  //center window!
							RECT rc;
						  GetWindowRect(hwnd, &rc);

						  int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
						  int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

						  SetWindowPos(hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
						

						  if (FileExists(".//Settings.dat"))
						  {
							  //load settings.
							  settings.LoadSettings();

							  //write settings
							  if (strlen(data.Username) > 0 && strlen(data.Password) > 0)
							  {
								  HWND HWNDUsername = GetDlgItem(hwnd, ID_USERNAME);
								  HWND HWNDPassword = GetDlgItem(hwnd, ID_PASSWORD);

								  SetWindowText(HWNDUsername, data.Username);
								  SetWindowText(HWNDPassword, data.Password);
							  }


							  HWND hwndCheckBox = GetDlgItem(hwnd, LoadUI); //newui
							  SendMessage(hwndCheckBox, BM_SETCHECK, data.ShowNewUI, 0);
							  HWND hwndCheckBox_REMEMBER_ME = GetDlgItem(hwnd, SaveAccount); //Remember Me
							  SendMessage(hwndCheckBox_REMEMBER_ME, BM_SETCHECK, data.RememberMe, 0);
						  }else{
							  strcpy_s(data.Username, ""); //setup default account and password.
							  strcpy_s(data.Password, "");
						  }
						 

	}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
			case ID_PLAY:
				PlayButtonPress();
			break;

			return TRUE;
		}
		break;
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;

	case WM_CLOSE:
		ExitProcess(0);
		//PostQuitMessage(0);
		//DestroyWindow(hwnd);
		//EndDialog(hwnd, NULL);
		return TRUE;
		
	}
	return FALSE;

}



int StringToWString(std::wstring &ws, const std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}


void PlayButtonPress()
{
	bool AllOK = false;
	char username[18];
	char password[18];
	HWND HWNDUsername = GetDlgItem(GetActiveWindow(), ID_USERNAME);
	HWND HWNDPassword = GetDlgItem(GetActiveWindow(), ID_PASSWORD);

	GetWindowText(HWNDUsername, username, 18);
	GetWindowText(HWNDPassword, password, 18);

	if (strlen(username) < 4 || strlen(username) > 18)
	{
		MessageBox(NULL, "Username must between 4 and 18 characters!", "Attention!", MB_ICONEXCLAMATION);
		return;
	}
	if (strlen(password) < 6 || strlen(password) > 18)
	{
		MessageBox(NULL, "Passsword must between 6 and 18 characters!", "Attention!", MB_ICONEXCLAMATION);
		return;
	}

	//checkbox save account
	HWND hwndCheckBox = GetDlgItem(GetActiveWindow(), LoadUI); //newui
	data.ShowNewUI = SendMessage(hwndCheckBox, BM_GETCHECK, 0, 0);
	
	HWND hwndCheckBox_REMEMBER_ME = GetDlgItem(GetActiveWindow(), SaveAccount); //Remember Me
	data.RememberMe = SendMessage(hwndCheckBox_REMEMBER_ME, BM_GETCHECK, 0, 0);

	if (data.RememberMe == false)
	{
		strcpy_s(data.Username, ""); //setup account and password.
		strcpy_s(data.Password, "");
	}
	else{
		strcpy_s(data.Username, username); //setup account and password.
		strcpy_s(data.Password, password);
	}
	settings.SaveSettings();



	//
	//char buff[127];
	//wsprintf(buff, "user %s pass %s PARAM: %ws", username, password, ParamAddr);
	//MessageBoxA(NULL, buff, "AAAA", MB_OK);


	char buffData[128];
	wsprintf(buffData, "login&%s&%s&4", username, password);
	std::string logindata = buffData;

	AllOK = gHTTP.CheckLogin(base64_encode((const unsigned char*)logindata.c_str(),logindata.size()), logindata.size());

	if (AllOK == true)
	{
		wchar_t buffParams[256];
		HWND hwndCheckBoxNewUI = GetDlgItem(GetActiveWindow(), LoadUI); //newui
		int uiResult = SendMessage(hwndCheckBoxNewUI, BM_GETCHECK, 0, 0);

		if (uiResult == BST_CHECKED)
		{
			wsprintfW(buffParams, L"/newui /logintoken:%S&%S /ip:%S /port:%S /Lver:2 /use_packing /gamechanneling:0", username, srvPassword.c_str(), ip.c_str(), port.c_str());
		}
		else{
			wsprintfW(buffParams, L"/logintoken:%S&%S /ip:%S /port:%S /Lver:2 /use_packing /gamechanneling:0", username, srvPassword.c_str(), ip.c_str(), port.c_str());
		}


		std::wstring Params = buffParams;
#ifdef RO
		ShellExecuteW(NULL, L"open", L"Dragon.exe", Params.c_str(), NULL, SW_SHOWNORMAL);
#endif
		ExitProcess(0);
	}else{
		MessageBoxA(NULL, "Invalid Username or Password!", "Error!", MB_OK);
	}
}

#endif