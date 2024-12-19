#pragma once

#include "NxMemPool.h"

#pragma warning( disable : 4290 )

const int PACKET_POOL_CNT = 2048;
const int PACKETBUFFERSIZE = 8192;
const int PACKETHEADERSIZE = sizeof(unsigned short);

class NxPacketException
{
};

class NxPacket
{
public:
	
	NxPacket();
	NxPacket( const NxPacket& Src );
	virtual  ~NxPacket();

public:
	static NxPacket* Alloc();
	static void		 Free(NxPacket* pPacket);

public:
	bool			IsValidHeader();
	bool			IsValidPacket();

	
	unsigned short	GetPacketSize();
	unsigned short	GetDataFieldSize() { return GetPacketSize() - PACKETHEADERSIZE; }
	unsigned long	GetReceivedSize(){ return m_nReceivedSize; }

	void			Clear( int nBufferSize = PACKETBUFFERSIZE );

	char*			GetPacketBuffer() { return m_PacketBuffer; }
	void			CopyToBuffer( char* pBuff, int nSize ) throw ( NxPacketException* ); 

	void			ReadData( void* pBuffer, int nSize )  throw ( NxPacketException* ); 
	void			WriteData( void* pBuffer, int nSize ) throw ( NxPacketException* ); 

	void			ViewData( void* pBuffer, int nSize )  throw ( NxPacketException* ); 


public:

	NxPacket&	operator = ( NxPacket& packet );
	NxPacket&	operator << ( bool arg );
	NxPacket&	operator << ( int arg );
	NxPacket&	operator << ( long arg );
	NxPacket&	operator << ( DWORD arg );
	NxPacket&	operator << ( WORD arg );
	NxPacket&	operator << ( __int64 arg );
	NxPacket&	operator << ( const TCHAR* arg );
	NxPacket&	operator << ( tstring& arg );

	NxPacket&	operator >> ( bool& arg );
	NxPacket&	operator >> ( int& arg );
	NxPacket&	operator >> ( long& arg );
	NxPacket&	operator >> ( DWORD& arg );
	NxPacket&	operator >> ( WORD& arg );
	NxPacket&	operator >> ( __int64& arg );
	NxPacket&	operator >> ( const TCHAR* arg );
	NxPacket&	operator >> ( tstring& arg );


protected:
	typedef struct
	{
		unsigned short*		pDataSize;
	}HEADER;

	HEADER		m_PacketHeader;
	char		m_PacketBuffer[PACKETBUFFERSIZE];
	char*		m_pDataField;
	char*		m_pReadPosition;
	char*		m_pWritePosition;
	char*		m_pEndOfDataField;
	int			m_nReceivedSize;

private:
	 static NxMemPool<NxPacket, PACKET_POOL_CNT>	ms_PacketPool;
};