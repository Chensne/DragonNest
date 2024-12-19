#pragma once

#include "EtFileIO.h"

class CEtStdFileIO : public CEtFileIO
{
public:
	CEtStdFileIO();
	virtual ~CEtStdFileIO();

	virtual bool	Open(const TCHAR* szFileName, AccessType nAccessMode);
	virtual bool	Close();

	virtual int		Read(void* pBuffer, int nLen);
	virtual int		Write(const void* pBuffer, int nLen );
	virtual int		Seek(int nOffset, SeekType nPosition);
	virtual int		Tell();

private:

	HANDLE		m_hFile;
};
