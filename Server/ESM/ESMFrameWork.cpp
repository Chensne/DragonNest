
#include "Stdafx.h"
#include "ESMFrameWork.h"
#include <io.h>
#include <direct.h>

CESMFrameWork * CESMFrameWork::GetInstance()
{
	static CESMFrameWork s;
	return &s;
}

CESMFrameWork::CESMFrameWork()
{
}

CESMFrameWork::~CESMFrameWork()
{
}

extern TNetLauncherPatcherConfig g_Config;

bool CESMFrameWork::LoadConfig()
{
#ifdef _SERVICEMODE
	GetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "ServiceManagerIP", g_Config.szServiceManagerIP, IPLENMAX);
	char szTemp[MAX_PATH];
	GetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "ServiceManagerPort", szTemp, MAX_PATH);
	g_Config.nServiceManagerPort = atoi(szTemp);;
	GetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "LauncherPath", g_Config.szNetLauncherLocation, 1024);
	GetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "LauncherName", g_Config.szNetLauncherName, MAX_PATH);

	if (g_Config.nServiceManagerPort <= 0)
	{
		//Sleep(20000);
		return 0;
	}
#else
	if (_access(".\\Config\\DNLauncherPatcher.ini", 0) == -1)
		return false;

	g_IniFile.Open(".\\Config\\DNLauncherPatcher.ini");
	g_IniFile.GetValue("ServiceManager", "IP", g_Config.szServiceManagerIP);
	g_IniFile.GetValue("ServiceManager", "Port", &g_Config.nServiceManagerPort);
	g_IniFile.GetValue("NetLauncher", "Location", g_Config.szNetLauncherLocation);
	g_IniFile.GetValue("NetLauncher", "FileName", g_Config.szNetLauncherName);
	//파일의 위치에 컨피스 폴더생성

	std::string str;
	str = g_Config.szNetLauncherLocation;
	str += "Config";
	if (_access(str.c_str(), 0) == -1)
	{
		mkdir(str.c_str());
	}
#endif
	return true;
}

bool CESMFrameWork::AppInit()
{
	SetMiniDump();
	CProcessManager::GetInstance()->Start();
	if (LoadConfig() == false)
	{
		_ASSERT(0);
		return false;
	}

	if (CLauncherPatcherSession::GetInstance()->Start(g_Config.szServiceManagerIP, g_Config.nServiceManagerPort) == false)
	{
		_ASSERT(0);
		return false;
	}
	return true;
}

void CESMFrameWork::AppClose()
{
	CProcessManager::GetInstance()->Stop();
	CLauncherPatcherSession::GetInstance()->Stop();
}
