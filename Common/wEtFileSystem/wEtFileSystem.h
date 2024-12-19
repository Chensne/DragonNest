// wEtFileSystem.h

#pragma once

using namespace System;

#include "../EtFileSystem/EtFileSystem.h"
#pragma comment ( lib, "advapi32.lib" )


namespace wEtPackingFile 
{
	public ref class wCEtPackingFile
	{
	protected:
		CEtPackingFile* m_pEtPackingFile;

	public:
		wCEtPackingFile();
		virtual ~wCEtPackingFile();

	public:
		bool NewFileSystem( System::String^ strPath );
		void CloseFileSystem();
		bool ChangeDir( System::String^ strPath );
		bool AddFile( System::String^ strName );
		DWORD GetFileSystemSize();
	};
}
