#include "stdafx.h"
#include "EtPackFileIO.h"
#include "EtFileSystem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtPackFileIO::CEtPackFileIO()
: m_pFileHandle(NULL)
{

}

CEtPackFileIO::~CEtPackFileIO()
{
	Close();

}


bool
CEtPackFileIO::Open(const TCHAR* szFileName, AccessType nAccessMode)
{
	Close();
	m_pFileHandle = CEtFileSystem::GetInstance().OpenFile(szFileName);
	
	if ( m_pFileHandle == NULL )
		return false;

	return true;
}

bool	
CEtPackFileIO::Close()
{
	if ( m_pFileHandle )
	{
		CEtFileSystem::GetInstance().CloseFile(m_pFileHandle);
		return true;
	}
	return false;
}

int		
CEtPackFileIO::Read(void* pBuffer, int nLen)
{
	if ( m_pFileHandle )
	{
		return m_pFileHandle->Read(pBuffer, nLen);
	}
	return -1;
}

int
CEtPackFileIO::Write(const void* pBuffer, int nLen )
{
	return -1;
}

int
CEtPackFileIO::Seek(int nOffset, SeekType nPosition)
{
	if ( m_pFileHandle )
	{
		return m_pFileHandle->Seek(nOffset, nPosition);
	}
	return -1;
}


int
CEtPackFileIO::Tell()
{
	return Seek(0, ET_SEEK_CURRENT);
}