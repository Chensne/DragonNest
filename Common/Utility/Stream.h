#pragma once

#include <string>
#include <vector>

class CStream
{
public:
	CStream() {}
    virtual ~CStream() {}

protected:

public:
	virtual bool IsEnd() const = 0;
	virtual long Tell() = 0;
	virtual int Size() const = 0;
	virtual bool IsValid() const = 0;
	virtual void Close() {};
	virtual void Activate() {}

	virtual int Seek( long lOffset, int nOrigin ) = 0;
	virtual int Read( void *pBuffer, int nSize ) = 0;
	virtual int Write( const void *pBuffer, int nSize ) = 0;
	virtual const char *GetName() { return NULL; }

public:
	template<class TYPE> void operator << ( TYPE Value );

	void operator << ( const std::string &str );
	void operator << ( const std::wstring &str );

	template<class TYPE> void operator << ( const std::vector<TYPE> &vecVALUE );
	void operator << ( const std::vector<std::string> &vecVALUE );
	void operator << ( const std::vector<std::wstring> &vecVALUE );

	void WriteBuffer ( const void* pBuffer, int Size );

public:
	template<class TYPE> void operator >> ( TYPE &Value );
	
	void operator >> ( std::string &str );
	void operator >> ( std::wstring &str );
	
	template<class TYPE> void operator >> ( std::vector<TYPE> &vecVALUE );
	void operator >> ( std::vector<std::string> &vecVALUE );
	void operator >> ( std::vector<std::wstring> &vecVALUE );

	void ReadBuffer ( void* pBuffer, int Size );

public:
	template<class TYPE> int SeekCur();
	template<class TYPE> int SeekCur_String();
	template<class TYPE> int SeekCur_Vec();
	template<class TYPE> int SeekCur_VecString();
};

template<class TYPE> 
void CStream::operator << ( TYPE Value )
{
	Write ( &Value, sizeof(Value) );
}

template<class TYPE> 
void CStream::operator >> ( TYPE &Value )
{
	Read ( &Value, sizeof(Value) );
}

template<class TYPE>
void CStream::operator << ( const std::vector<TYPE> &vecVALUE )
{
	DWORD dwVecSize = (DWORD)vecVALUE.size();
	*this << ( dwVecSize );

	if( !vecVALUE.empty() )
	{
		WriteBuffer( &(vecVALUE[0]), DWORD(sizeof(TYPE)*vecVALUE.size()) );
	}
}

template<class TYPE>
void CStream::operator >> ( std::vector<TYPE> &vecVALUE )
{
	DWORD dwSize(0);
	*this >> ( dwSize );

	if( dwSize > 0 )
	{
		vecVALUE.clear();
		vecVALUE.reserve(dwSize);

		TYPE tVALUE;

		for ( DWORD i=0; i<dwSize; ++i )
		{
			ReadBuffer( &tVALUE, DWORD(sizeof(TYPE)) );
			vecVALUE.push_back( tVALUE );
		}
	}
}

template<class TYPE> 
int CStream::SeekCur()
{
	return Seek( sizeof(TYPE), SEEK_CUR );
}

template<class TYPE> 
int CStream::SeekCur_String()
{
	DWORD dwSize;
	*this >> dwSize;

	if( dwSize > 0 )
	{
		Seek( sizeof(TYPE)*dwSize, SEEK_CUR );
	}

	return (int)dwSize;
}

template<class TYPE>
int CStream::SeekCur_Vec()
{
	DWORD dwSize(0);
	*this >> ( dwSize );

	if( dwSize > 0 )
	{
		Seek( sizeof(TYPE)*dwSize, SEEK_CUR );
	}

	return (int)dwSize;
}

template<class TYPE> 
int CStream::SeekCur_VecString()
{
	DWORD dwSize(0);
	*this >> ( dwSize );

	if( dwSize > 0 )
	{
		for ( DWORD i=0; i<dwSize; ++i )
		{
			SeekCur_String<TYPE>();
		}
	}

	return (int)dwSize;
}

class CMemoryStream;
class CFileStream : public CStream
{
public:
	CFileStream();
	CFileStream( const char *pFileName, int nMode = OPEN_READ );
	virtual ~CFileStream();
	enum 
	{
		OPEN_READ,
		OPEN_WRITE,
		OPEN_READ_WRITE
	};

protected:
	FILE *m_pStream;
	std::string m_szFileName;
	int m_nMode;

public:
	virtual void Activate();

	void SetFileName( const char *pFileName ) { m_szFileName = pFileName; }
	const char *GetFileName() { return m_szFileName.c_str(); }

	const char *GetName() { return GetFileName(); }

	bool Open( const char *pFileName, int nMode = OPEN_READ );
	void Close();
	bool IsEnd() const;
	long Tell() ;
	int Size() const;
	bool IsValid() const;

	int Seek( long lOffset, int nOrigin );
	int Read( void *pBuffer, int nSize );
	int Write( const void *pBuffer, int nSize );
};

class CMemoryStream : public CStream
{
public:
	CMemoryStream();
	CMemoryStream( void *pBuffer, int nSize );
	virtual ~CMemoryStream() {}
	void Initialize( void *pBuffer, int nSize );

protected:
	char *m_pBuffer;
	int m_nSize;
	int m_nCurrent;
	std::string m_szName;

public:
	bool IsEnd() const;
	long Tell();
	virtual int Size() const;
	bool IsValid() const;

	virtual int Seek( long lOffset, int nOrigin );
	virtual int Read( void *pBuffer, int nSize );
	virtual int Write( const void *pBuffer, int nSize );

	char *GetBuffer() { return m_pBuffer; }

	const char *GetName() { return m_szName.c_str(); }
	void SetName( const char *szName ) { m_szName = szName; }
};

class CPacketCompressStream : public CMemoryStream
{
public:
	CPacketCompressStream();
	CPacketCompressStream( void *pBuffer, int nSize );
	virtual ~CPacketCompressStream() {}

	enum CompressTypeEnum {
		NOCOMPRESS,
		INTEGER_SHORT,
		INTEGER_CHAR,
		DWORD_SHORT,
		DWORD_CHAR,
		FLOAT_SHORT,
		VECTOR2_SHORT,
		VECTOR3_SHORT,
		VECTOR2_BIT,
		VECTOR3_BIT,
	};

protected:
	int m_nValidBufferSize;
	int m_nValidBitCurrent;
	int m_nValueCount;


public:
	virtual int Size() const;

	virtual int Read( void *pBuffer, int nSize );
	virtual int Write( const void *pBuffer, int nSize );

	int Seek( long lOffset, int nOrigin, CompressTypeEnum Type );
	int Read( void *pBuffer, int nSize, CompressTypeEnum Type, float fValue = 0.f );
	int Write( const void *pBuffer, int nSize, CompressTypeEnum Type, float fValue = 0.f );
};

class CByteStream : public CStream
{
public:
	typedef std::vector<char>	VEC_CHAR;
	typedef VEC_CHAR::iterator	VEC_CHAR_IT;

public:
	CByteStream();
	CByteStream( void *pBuff, int nSize );
	virtual ~CByteStream(){}

protected:
	VEC_CHAR m_Buffer;
	int m_nCurrent;

public:
	bool IsEnd() const;
	long Tell();
	int Size() const;
	bool IsValid() const;

	int Seek( long lOffset, int nOrigin );
	int Read( void *pBuffer, int nSize );
	int Write( const void *pBuffer, int nSize );

	char* GetBuffer() { return &m_Buffer[0]; }
};
