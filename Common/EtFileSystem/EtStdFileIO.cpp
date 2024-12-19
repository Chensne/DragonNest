#include "stdafx.h"
#include "EtStdFileIO.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//-------------------------------------------------------------------------
CEtStdFileIO::CEtStdFileIO()
{
	m_hFile = NULL;
}

CEtStdFileIO::~CEtStdFileIO()
{
	Close();
}


bool
CEtStdFileIO::Open(const TCHAR* szFileName, AccessType nAccessMode)
{
	Close();
	switch( nAccessMode ) 
	{
	case ET_READ:
		{
			m_hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
								 FILE_ATTRIBUTE_NORMAL,NULL );
		}
		break;
	case ET_WRITE:
		{
			m_hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
								 FILE_ATTRIBUTE_NORMAL,NULL );
		}
		break;
	case ET_ALL:
		{
			m_hFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
								 FILE_ATTRIBUTE_NORMAL,NULL );
		}
		break;
	
	}

	if ( m_hFile == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	return true;
}

bool	
CEtStdFileIO::Close()
{
	if ( CloseHandle(m_hFile) == TRUE )
		return true;

	return false;
}

int		
CEtStdFileIO::Read(void* pBuffer, int nLen)
{
	DWORD nReadSize = 0;
	ReadFile(m_hFile, pBuffer, nLen, &nReadSize, NULL );
	return nReadSize;
}

int
CEtStdFileIO::Write(const void* pBuffer, int nLen )
{
	DWORD nWriteSize = 0;
	WriteFile(m_hFile, pBuffer, nLen, &nWriteSize, 0);
	return nWriteSize;
}

int
CEtStdFileIO::Seek(int nOffset, SeekType nPosition)
{
	LARGE_INTEGER li;

	li.QuadPart = nOffset;
	li.LowPart = SetFilePointer(m_hFile, li.LowPart, &li.HighPart, nPosition);
	
	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		li.QuadPart = -1;
	}

	return (int)li.QuadPart;
}


int
CEtStdFileIO::Tell()
{
	return Seek(0, ET_SEEK_CURRENT);
}
