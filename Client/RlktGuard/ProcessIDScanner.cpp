#include "ProcessIDScanner.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <list>
#include <string>


std::list<std::string> process;
 
void LoadProcessNameData()
{
	process.push_back("cheatengine-i386.exe");
	process.push_back("cheatengine-x86_64.exe");
	process.push_back("ollydbg.exe");
	process.push_back("wpepro.net.exe");
	process.push_back("x64_dbg.exe");
	process.push_back("x32_dbg.exe");
	process.push_back("x96_dbg.exe");
	process.push_back("procexp64.exe");
	process.push_back("procexp.exe");
	process.push_back("immunitydebugger.exe");
	process.push_back("petools.exe");
	process.push_back("processhacker.exe");
	process.push_back("tuoji_80ex_D1.exe");
	process.push_back("tuoji_80ex_D1.exe");
}


void ScanProcId(const char* ProcName){
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;
	
	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hSnapshot, &pe32)){
		do{
			//VIRTUALIZER_DOLPHIN_WHITE_START
			//make it toLOWER!!@
			for (DWORD i = 0; i < strlen(pe32.szExeFile); ++i)
				pe32.szExeFile[i] = tolower(pe32.szExeFile[i]);

			if (_stricmp(pe32.szExeFile, ProcName) == 0)
			{
				
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);  // Close detected process
				TerminateProcess(hProcess, NULL);
				Sleep(300);
				
				DWORD exitCode = 0;
				if (GetExitCodeProcess(hProcess, &exitCode) == FALSE)
				{
				}
				//VIRTUALIZER_DOLPHIN_WHITE_END
				if (exitCode != STILL_ACTIVE)
				{
#ifdef DEBUG
					MessageBoxA(NULL, "Antihack , process was successfully terminated.\nYou can play.", ProcName, MB_OK);
				//	Finalize_OpenPage();
#endif
					break;
				}
				else{
#ifdef DEBUG
					MessageBoxA(NULL, "Cannot kill hack process.", "Memory Scanner", MB_OK);
#endif
					//Finalize_OpenPage(pe32.szExeFile);
					//security s;
					//VIRTUALIZER_DOLPHIN_WHITE_START
					//s.crash();
					//VIRTUALIZER_DOLPHIN_WHITE_END
					ExitProcess(0);
				}
#ifdef DEBUG
				MessageBoxA(NULL, "[Scanner] Found process name", ProcName, MB_OK);
#endif
				
			}
		} while (Process32Next(hSnapshot, &pe32));
	}
	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);
}


void doProcScan()
{

	for (std::list<std::string>::iterator it = process.begin(); it != process.end(); it++)
	{
		ScanProcId((*it).c_str()); //hack
	}
}