// ESMService.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ESMFrameWork.h"

TNetLauncherPatcherConfig g_Config;

#ifdef _SERVICEMODE
#include <Winsvc.h>

SERVICE_STATUS m_ServiceStatus;
SERVICE_STATUS_HANDLE m_ServiceStatusHandle;

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
void WINAPI ServiceCtrlHandler(DWORD Opcode);
BOOL InstallService(LPCTSTR sExeFileName, LPCTSTR sServiceName, LPCTSTR sDisplayName);
BOOL DeleteService(LPCTSTR sServiceName);

void TextOut(const TCHAR * format, ...) {}		//���Ϸα� ���� �� �ֵ��� ����

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
	m_ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    m_ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    m_ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP; 
    m_ServiceStatus.dwWin32ExitCode      = 0; 
    m_ServiceStatus.dwServiceSpecificExitCode = 0; 
    m_ServiceStatus.dwCheckPoint         = 0; 
    m_ServiceStatus.dwWaitHint           = 0; 
 
    m_ServiceStatusHandle = RegisterServiceCtrlHandler("ESM Service",ServiceCtrlHandler);  
    if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
    { 
		_ASSERT_EXPR(0, L"SERVICE_STATUS_HANDLE NULL");
        return; 
    }     

    m_ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    m_ServiceStatus.dwCheckPoint         = 0; 
    m_ServiceStatus.dwWaitHint           = 0;  
    if (!SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus)) 
    { 
		_ASSERT_EXPR(0, L"SetServiceStatus error");
		return;
    } 
	
	CESMFrameWork::GetInstance()->AppInit();
    return; 
}

void WINAPI ServiceCtrlHandler(DWORD Opcode)
{
    switch(Opcode) 
    { 
        case SERVICE_CONTROL_PAUSE:
			{
				m_ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
			}
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
			{
				m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
			}
            break; 
 
        case SERVICE_CONTROL_STOP: 
			{
				m_ServiceStatus.dwWin32ExitCode = 0; 
				m_ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
				m_ServiceStatus.dwCheckPoint    = 0; 
				m_ServiceStatus.dwWaitHint      = 0; 
	 
				SetServiceStatus(m_ServiceStatusHandle,&m_ServiceStatus);

				CESMFrameWork::GetInstance()->AppClose();
			}
			break;
 
        case SERVICE_CONTROL_INTERROGATE: 
			{
			}
            break; 
    }      
    return; 
}

BOOL InstallService(LPCTSTR sExeFileName, LPCTSTR sServiceName, LPCTSTR sDisplayName)
{
	char strDir[1024];
	char strbackspace[2]; strbackspace[0] = 0x5C; strbackspace[1] = 0;

	SC_HANDLE  schSCManager,schService;

	GetCurrentDirectory(1024,strDir);

	if(strDir[strlen(strDir) - 1] != 0x5C)
		strcat(strDir, strbackspace);
	strcat(strDir, sExeFileName); 

	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);  
 
	if (schSCManager == NULL) 
		return false;

    LPCTSTR lpszBinaryPathName=strDir;
 
    schService = CreateService(schSCManager,
							   sServiceName,				//Service Name
							   sDisplayName,				// service name to display 
							   SERVICE_ALL_ACCESS,       	// desired access 
							   SERVICE_WIN32_OWN_PROCESS,	// service type 
							   SERVICE_DEMAND_START,     	// start type 
							   SERVICE_ERROR_NORMAL,     	// error control type 
							   lpszBinaryPathName,       	// service's binary 
							   NULL,						// no load ordering group 
							   NULL,						// no tag identifier 
							   NULL,						// no dependencies 
							   NULL,						// LocalSystem account 
							   NULL);						// no password 
 
    if (schService == NULL) 
        return false;  
 
    CloseServiceHandle(schService);

	return true;
}

BOOL DeleteService(LPCTSTR sServiceName)
{
	SC_HANDLE  schSCManager;
	SC_HANDLE hService;

	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
 
	if (schSCManager == NULL) 
		return false;	

	hService=OpenService(schSCManager, sServiceName, SERVICE_ALL_ACCESS);

	if (hService == NULL) 
		return false;

	if(DeleteService(hService)==0)
		return false;

	if(CloseServiceHandle(hService)==0)
		return false;
	else
		return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY DispatchTable[]={{"ESM Service",ServiceMain},{NULL,NULL}};  

	if (argc > 1)
	{
		if (!stricmp(argv[1],"install"))
		{
			//install�ε� �ƱԸ�Ʈ�� �����ϸ� �ȵ���������
			if (argc < 6)
			{
				_ASSERT_EXPR(0, L"Check cmd arg");
				return 0;
			}

#ifdef _DEBUG
#ifdef WIN64
			if (InstallService("ESMService64D.exe", "ESM Service", "EyedentityGames Service Manager") == TRUE)
#else
			if (InstallService("ESMServiceD.exe", "ESM Service", "EyedentityGames Service Manager") == TRUE)
#endif
#else
#ifdef WIN64
			if (InstallService("ESMService64.exe", "ESM Service", "EyedentityGames Service Manager") == TRUE)
#else
			if (InstallService("ESMService.exe", "ESM Service", "EyedentityGames Service Manager") == TRUE)
#endif
#endif
			{
				//�������̸� ������Ʈ���� �������
				LONG nRet = AddRegistryKey(HKEY_LOCAL_MACHINE, "Software", "EyedentityGames");
				if (nRet == ERROR_SUCCESS)
				{
					nRet = SetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "ServiceManagerIP", argv[2]);
					nRet = SetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "ServiceManagerPort", argv[3]);
					nRet = SetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "LauncherPath", argv[4]);
					nRet = SetRegistryString(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames", "LauncherName", argv[5]);
				}
			}
			else
			{
				_ASSERT_EXPR(0, L"EyedentityGames Service Manager Installservice Failed");
				return 0;
			}
		}
		else if (!stricmp(argv[1],"uninstall"))
		{
			if (DeleteService("ESM Service") == TRUE)
			{
				//�������̸� ������Ʈ������ ������
				DeleteRegistry(HKEY_LOCAL_MACHINE, "Software\\EyedentityGames");
			}
			else
			{
				_ASSERT_EXPR(0, L"EyedentityGames Service Manager UnInstallservice Failed");
				return 0;
			}
		}
	}
	else
	{
		if (StartServiceCtrlDispatcher(DispatchTable) == 0)
		{
			_ASSERT_EXPR(0, L"no ESM Service");
		}
	}
	return 0;
}
#else
int _tmain(int argc, _TCHAR* argv[])
{
	if (CESMFrameWork::GetInstance()->AppInit() == false)
	{
		_ASSERT_EXPR(0, L"Init Failed");
		return 0;
	}

	wprintf(L"exit ����� ġ�� ����\r\n");

	char szCmd[256] = {0,};	
	while (1)
	{
		if (strcmp(szCmd, "exit") == 0)	break;
		
		printf("CMD>");
		gets(szCmd);
	}

	CESMFrameWork::GetInstance()->AppClose();

	return 0;
}
#endif
