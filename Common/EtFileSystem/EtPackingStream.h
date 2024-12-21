#pragma once

#include "Stream.h"
//----------------------------------------------------------------------------------------
class CEtFileHandle;
class CPackingStream : public CStream
{
public:
	CPackingStream();
	CPackingStream( const char *pFileName );
	virtual ~CPackingStream();
	
protected:
	CEtFileHandle*	m_pStream;
	std::string m_szFileName;

public:
	virtual void Activate();

	void SetFileName( const char *pFileName ) { m_szFileName = pFileName; }
	const char *GetFileName() { return m_szFileName.c_str(); }

	const char *GetName() { return GetFileName(); }

	bool Open( const char *pFileName );
	void Close();
	bool IsEnd() const;
	long Tell() ;
	int Size() const;
	bool IsValid() const;

	int Seek( long lOffset, int nOrigin );
	int Read( void *pBuffer, int nSize );
	int Write( const void *pBuffer, int nSize );
};