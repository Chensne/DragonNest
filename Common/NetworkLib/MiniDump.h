
#pragma once
#include <windows.h>
#include <stdio.h>
#include <DbgHelp.h>
#include <crtdbg.h>

enum _DUMPTYPE
{
	_DUMPTYPE_MINI,
	_DUMPTYPE_MIDI,
	_DUMPTYPE_MAXI,
};

void SetMiniDump();
void StoreMiniDump(const TCHAR * szFName, _MINIDUMP_TYPE dumptype);

//with callback
LONG StoreMiniDump(_DUMPTYPE eType, EXCEPTION_POINTERS * pE);
BOOL CALLBACK MiniDumpCallBack(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput);
bool IsDataSectionNeeded(const WCHAR* pModuleName);