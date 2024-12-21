#include <windows.h>
#include "GuardThread.h"
#include "LoadedModuleScanner.h"
#include "MemoryScanner.h"
#include "ProcessIDScanner.h"
#include "WindowScanner.h"



#define SECURITY_WIN32 
#define SCAN_INTERVAL 2000

void StartThread()
{
	do
	{
		MemoryScanner(); //Memory Scanner with memory footprint
		doProcScan();	 //Executable name 
		doWindowScan();  //Window name scan

		Sleep(SCAN_INTERVAL);
	} while (1);
}

void InitGuard()
{
	//Process Scanner Data
	LoadProcessNameData();
	//Window Scanner Data
	LoadWindowNameData();
	LoadWindowClassData();

	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(StartThread), NULL, 0, 0);
}
