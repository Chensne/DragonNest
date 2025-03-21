// NATProbeService.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include <Windows.h>
#include <Winsvc.h>
#include <time.h>
#include <stdio.h>

#include "probe.h"

SERVICE_STATUS m_ServiceStatus;
SERVICE_STATUS_HANDLE m_ServiceStatusHandle;
BOOL bRunning=true;

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
void WINAPI ServiceCtrlHandler(DWORD Opcode);
BOOL InstallService(LPCTSTR sExeFileName, LPCTSTR sServiceName, LPCTSTR sDisplayName);
BOOL DeleteService(LPCTSTR sServiceName);

#ifndef _CONSOLEMODE
int main(int argc, char* argv[])
{
	if(argc>1)
	{
		if(strcmp(argv[1],"-i")==0)
		{
			if(InstallService("NATProbeService.exe", "NAT Probe", "NAT Test Probe Service"))
				printf("\n\nService Installed Sucessfully\n");
			else
				printf("\n\nError Installing Service\n");
		}
		else if(strcmp(argv[1],"-d")==0)
		{
			if(DeleteService("NAT Probe"))
				printf("\n\nService UnInstalled Sucessfully\n");
			else
				printf("\n\nError UnInstalling Service\n");
		}
		else if(strcmp(argv[1], "-standalone") == NULL)
		{
			printf("NAT Probe stand alone mode activated.\n");
			probemain(argc - 1, &(argv[1]));
		}
		else
		{
			printf("\n\nUnknown Switch Usage\n\nFor Install use NATProbeService.exe -i\n\nFor UnInstall use NATProbeService.exe -d\n");
			printf("\nIf u wanna run NAT probe as a stand-alone executables, use \nNATProbeService.exe -standalone tcpport udpport\n");
		}
	}
	else
	{
		SERVICE_TABLE_ENTRY DispatchTable[]={{"NAT Probe",ServiceMain},{NULL,NULL}};  
		StartServiceCtrlDispatcher(DispatchTable); 
	}
	return 0;
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{

    //DWORD status; 
    //DWORD specificError; 

    m_ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    m_ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    m_ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP; 
    m_ServiceStatus.dwWin32ExitCode      = 0; 
    m_ServiceStatus.dwServiceSpecificExitCode = 0; 
    m_ServiceStatus.dwCheckPoint         = 0; 
    m_ServiceStatus.dwWaitHint           = 0; 
 
    m_ServiceStatusHandle = RegisterServiceCtrlHandler("NAT Probe",ServiceCtrlHandler);  
    if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
    { 
        return; 
    }     

    m_ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    m_ServiceStatus.dwCheckPoint         = 0; 
    m_ServiceStatus.dwWaitHint           = 0;  
    if (!SetServiceStatus (m_ServiceStatusHandle, &m_ServiceStatus)) 
    { 

    } 
	
	bRunning=true;
	
	probemain(argc, argv);
	/*
	while(bRunning)
	{		
		Sleep(3000);	
		//Place Your Code for processing here....
	}
	*/
    return; 
}


void WINAPI ServiceCtrlHandler(DWORD Opcode)
{
    switch(Opcode) 
    { 
        case SERVICE_CONTROL_PAUSE: 
            m_ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
            m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
            break; 
 
        case SERVICE_CONTROL_STOP: 
            m_ServiceStatus.dwWin32ExitCode = 0; 
            m_ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
            m_ServiceStatus.dwCheckPoint    = 0; 
            m_ServiceStatus.dwWaitHint      = 0; 
 
            SetServiceStatus (m_ServiceStatusHandle,&m_ServiceStatus);

			bRunning=false;

			break;
 
        case SERVICE_CONTROL_INTERROGATE: 
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
#endif
