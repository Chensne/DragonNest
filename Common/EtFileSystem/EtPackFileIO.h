#pragma once

#include "EtFileIO.h"

class CEtFileHandle;
class CEtPackingFile;

class CEtPackFileIO : public CEtFileIO
{
public:
	CEtPackFileIO();
	virtual ~CEtPackFileIO();

	virtual bool	Open(const TCHAR* szFileName, AccessType nAccessMode);
	virtual bool	Close();

	virtual int		Read(void* pBuffer, int nLen);
	virtual int		Write(const void* pBuffer, int nLen );
	virtual int		Seek(int nOffset, SeekType nPosition);
	virtual int		Tell();

private:

	CEtFileHandle*		m_pFileHandle;

};
