// 기본 DLL 파일입니다.

#include "stdafx.h"
#include "wEtFileSystem.h"


using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;


namespace wEtPackingFile
{
	wCEtPackingFile::wCEtPackingFile() : m_pEtPackingFile( new CEtPackingFile )
	{
	}

	wCEtPackingFile::~wCEtPackingFile()
	{
		if( m_pEtPackingFile )
		{
			delete m_pEtPackingFile;
			m_pEtPackingFile = NULL;
		}
	}

	bool wCEtPackingFile::NewFileSystem( System::String^ strPath )
	{
		if( !m_pEtPackingFile )
			return false;

		char* pCharPath = (char*)Marshal::StringToHGlobalAnsi( strPath ).ToPointer();
		bool bRetValue = m_pEtPackingFile->NewFileSystem( pCharPath );
		Marshal::FreeHGlobal( IntPtr( pCharPath ) );
		return bRetValue;
	}

	void wCEtPackingFile::CloseFileSystem()
	{
		if( m_pEtPackingFile )
			m_pEtPackingFile->CloseFileSystem();
	}

	bool wCEtPackingFile::ChangeDir( System::String^ strPath )
	{
		if( !m_pEtPackingFile )
			return false;

		char* pCharPath = (char*)Marshal::StringToHGlobalAnsi( strPath ).ToPointer();
		bool bRetValue = m_pEtPackingFile->ChangeDir( pCharPath );
		Marshal::FreeHGlobal( IntPtr( pCharPath ) );
		return bRetValue;
	}

	bool wCEtPackingFile::AddFile( System::String^ strName )
	{
		if( !m_pEtPackingFile )
			return false;

		char* pCharName = (char*)Marshal::StringToHGlobalAnsi( strName ).ToPointer();
		bool bRetValue = m_pEtPackingFile->AddFile( pCharName );
		Marshal::FreeHGlobal( IntPtr( pCharName ) );
		return bRetValue;
	}

	DWORD wCEtPackingFile::GetFileSystemSize()
	{
		if( !m_pEtPackingFile )
			return 0;
			
		return m_pEtPackingFile->GetFileSystemSize();
	}
}