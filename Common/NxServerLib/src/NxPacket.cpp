#include "stdafx.h"
#include "NxPacket.h"
#include <assert.h>
#include <tchar.h>

NxMemPool<NxPacket, PACKET_POOL_CNT>	NxPacket::ms_PacketPool;

NxPacket* 
NxPacket::Alloc()
{
	return ms_PacketPool.Alloc();
}

void	
NxPacket::Free(NxPacket* pPacket)
{
	ms_PacketPool.Free(pPacket);
}

//-------------------------------------------------------------------------------------------

NxPacket::NxPacket()
: m_pDataField( 0 ), m_pReadPosition( 0 ), m_pWritePosition( 0 ), m_nReceivedSize( 0 )
{

	Clear();
}


NxPacket::NxPacket( const NxPacket& Src )
: m_pDataField( 0 ), m_pReadPosition( 0 ), m_pWritePosition( 0 ),
m_nReceivedSize( 0 )
{

	Clear();
	assert( m_pReadPosition <= m_pEndOfDataField );

	::CopyMemory( m_PacketBuffer, Src.m_PacketBuffer, PACKETBUFFERSIZE );

	assert( m_pReadPosition <= m_pEndOfDataField );

	m_nReceivedSize = Src.m_nReceivedSize;

	DWORD nOffset = 0;

	nOffset = ( DWORD )Src.m_pReadPosition - ( DWORD )Src.m_pDataField;
	m_pReadPosition += nOffset;

	assert( m_pReadPosition <= m_pEndOfDataField );

	nOffset = ( DWORD )Src.m_pWritePosition - ( DWORD )Src.m_pDataField;
	m_pWritePosition += nOffset;
}

NxPacket::~NxPacket()
{
}

bool 
NxPacket::IsValidHeader()
{
	if( GetPacketSize() >= PACKETHEADERSIZE )
		return true;

	return false;
}

bool
NxPacket::IsValidPacket()
{
	if( IsValidHeader() == false || m_nReceivedSize < PACKETHEADERSIZE || m_nReceivedSize < GetPacketSize() )
		return false;

	return true;
}

void 
NxPacket::Clear( int nBufferSize )
{
	::ZeroMemory( m_PacketBuffer, nBufferSize );

	m_PacketHeader.pDataSize	= ( unsigned short* )m_PacketBuffer + 0;					//  packetSize size = 2

	m_pDataField = &m_PacketBuffer[sizeof(unsigned short)];
	m_pReadPosition = m_pWritePosition = m_pDataField;
	m_pEndOfDataField = &m_pDataField[nBufferSize];

	assert( m_pReadPosition <= m_pEndOfDataField );


	m_nReceivedSize = 0;
}

unsigned short 
NxPacket::GetPacketSize()
{
	return *m_PacketHeader.pDataSize;
}

void
NxPacket::CopyToBuffer( char* pBuffer, int nSize )
{
	if ( nSize > PACKETBUFFERSIZE )
	{
		assert ( false && "Packer Buffer Overflow!!" );
	}

	Clear();
	::CopyMemory( m_PacketBuffer + m_nReceivedSize, pBuffer, nSize );
	m_nReceivedSize += nSize;
}

void
NxPacket::ReadData( void* pBuffer, int nSize )
{
	if( m_pReadPosition + nSize > m_pDataField + GetDataFieldSize() ||
		m_pReadPosition + nSize > m_pEndOfDataField )
	{
		throw ( new NxPacketException() );
	}

	::CopyMemory( pBuffer, m_pReadPosition, nSize );
	m_pReadPosition += nSize;
}

void	
NxPacket::ViewData( void* pBuffer, int nSize )
{
	if( m_pReadPosition + nSize > m_pDataField + GetDataFieldSize() ||
		m_pReadPosition + nSize > m_pEndOfDataField )
	{
		throw ( new NxPacketException() );
	}

	::CopyMemory( pBuffer, m_pReadPosition, nSize );
}

void 
NxPacket::WriteData( void* pBuffer, int nSize )
{
	if( m_pWritePosition + nSize > m_pEndOfDataField )
	{
		throw ( new NxPacketException() );
	}


	::CopyMemory( m_pWritePosition, pBuffer, nSize );
	m_pWritePosition += nSize;
	m_nReceivedSize += nSize;

	*m_PacketHeader.pDataSize += nSize;
}

NxPacket&
NxPacket::operator = ( NxPacket& packet )
{
	Clear();
	::CopyMemory( m_PacketBuffer, packet.GetPacketBuffer(), packet.GetPacketSize() );

	return *this;
}

NxPacket&
NxPacket::operator << ( const TCHAR* arg )
{
	WriteData( (void*)arg,  (int)(_tcslen(arg) * sizeof( TCHAR ) + sizeof( TCHAR ))  );

	return *this;
}

NxPacket& 
NxPacket::operator >> (  const TCHAR* arg )
{
	ReadData( (void*)arg, (int)(_tcslen((const TCHAR*)m_pReadPosition) * sizeof( TCHAR ) + sizeof( TCHAR )) );

	return *this;
}


NxPacket&	
NxPacket::operator << ( tstring& arg )
{
	*this << (TCHAR*)arg.c_str();

	return *this;
}

NxPacket&	
NxPacket::operator >> ( tstring& arg )
{
	TCHAR Buffer[2048] = {0} ;

	*this >> Buffer;
	arg = Buffer;

	return *this;
}


NxPacket&
NxPacket::operator << ( bool arg )
{
	WriteData( &arg, sizeof( bool ) );

	return *this;
}

NxPacket& 
NxPacket::operator >> ( bool& arg )
{
	ReadData( &arg, sizeof( bool ) );

	return *this;
}


NxPacket& 
NxPacket::operator << ( int arg )
{
	WriteData( &arg, sizeof( int ) );

	return *this;
}

NxPacket& 
NxPacket::operator >> ( int& arg )
{
	ReadData( &arg, sizeof( int ) );

	return *this;
}

NxPacket&
NxPacket::operator << ( long arg )
{
	WriteData( &arg, sizeof( long ) );

	return *this;
}

NxPacket& 
NxPacket::operator >> ( long& arg )
{
	ReadData( &arg, sizeof( long ) );

	return *this;
}

NxPacket& 
NxPacket::operator << ( DWORD arg )
{
	WriteData( &arg, sizeof( DWORD ) );

	return *this;
}

NxPacket& 
NxPacket::operator >> ( DWORD& arg )
{
	ReadData( &arg, sizeof( DWORD ) );

	return *this;
}


NxPacket& 
NxPacket::operator << ( WORD arg )
{
	WriteData( &arg, sizeof( WORD ) );

	return *this;
}

NxPacket& 
NxPacket::operator >> ( WORD& arg )
{
	ReadData( &arg, sizeof( WORD ) );

	return *this;
}

NxPacket& 
NxPacket::operator << ( __int64 arg )
{
	WriteData( &arg, sizeof( __int64 ) );

	return *this;
}

NxPacket& 
NxPacket::operator >> ( __int64& arg )
{
	ReadData( &arg, sizeof( __int64 ) );

	return* this;
}
