// EtPackingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "EtFileSystem.h"
#include "EtPackingStream.h"


int _tmain(int argc, _TCHAR* argv[])
{
	//CEtFileSystem::AddPackingFile(_T("./Resource/Npc.Nfs"));
	//CEtFileSystem::AddPackingFile(_T("./Resource/Quest.Nfs"));
	//CEtFileSystem::AddPackingFile(_T("./Resource/Script.Nfs"));
	CEtFileSystem::GetInstance().AddPackingFolder(_T(".\\Resource"));


	CPackingStream stream("\\Script\\AiBat.lua");
	int nSize = stream.Size();

	char* pBuffer = new char[nSize];

	stream.Read(pBuffer, nSize);
	delete[] pBuffer;

	CEtFileSystem::GetInstance().RemoveAll();
	
	return 0;
}

