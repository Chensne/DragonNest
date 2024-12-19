
#include "StdAfx.h"
#include "../Protocol/DNPacket.h"
#include "SocketContext.h"
#include "DoubleBuffer.h"

//==================================================================================================================================================//
// Class CDoubleBuffer
//==================================================================================================================================================//

CDoubleBuffer::CDoubleBuffer( const UINT uiSize )
: m_uiActiveBufferIndex(0)
{
	for( int i=0 ; i<2 ; ++i )
	{
		m_pBuffer[i]		= new (std::nothrow) char[uiSize];
		m_uiBufferOffset[i]	= 0;
		m_uiBufferSize[i]	= uiSize;
	}
}

CDoubleBuffer::~CDoubleBuffer()
{
	for( int i=0 ; i<2 ; ++i )
	{
		if( m_pBuffer[i] )
		{
			delete[] m_pBuffer[i];
			m_pBuffer[i] = NULL;
		}
	}
}

void CDoubleBuffer::Clear()
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	for( int i=0 ; i<2 ; ++i )
		m_uiBufferOffset[i] = 0;
}

int CDoubleBuffer::Switch( char*& pReturnBuffer, UINT& uiReturnSize )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	pReturnBuffer	= m_pBuffer[m_uiActiveBufferIndex];
	uiReturnSize	= m_uiBufferOffset[m_uiActiveBufferIndex];

	m_uiActiveBufferIndex ^= 1;

	m_uiBufferOffset[m_uiActiveBufferIndex] = 0;

	return 0;
}

#if defined( _DB )

int CDoubleBuffer::Pop( char* pDest, UINT& uiReturnSize )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	if( m_uiBufferOffset[m_uiActiveBufferIndex] > PROCESSBUFFERLENMAX )
		return -1;

	memcpy( pDest, m_pBuffer[m_uiActiveBufferIndex], m_uiBufferOffset[m_uiActiveBufferIndex] );
	uiReturnSize = m_uiBufferOffset[m_uiActiveBufferIndex];

	m_uiBufferOffset[m_uiActiveBufferIndex] = 0;

	return 0;
}

#endif

int CDoubleBuffer::Resize( const UINT32 uiSize )
{
	// �������� ����ȭ �Ǵ� �Լ�
	_ASSERT( m_uiBufferSize[m_uiActiveBufferIndex] < uiSize );

	char* pResizeBuffer = new (std::nothrow) char[uiSize];
	if( pResizeBuffer == NULL )
		return -1;

	if( m_uiBufferOffset[m_uiActiveBufferIndex] )
		memcpy( pResizeBuffer, m_pBuffer[m_uiActiveBufferIndex], m_uiBufferOffset[m_uiActiveBufferIndex] );
	
	delete[] m_pBuffer[m_uiActiveBufferIndex];
	m_pBuffer[m_uiActiveBufferIndex]		= pResizeBuffer;
	m_uiBufferSize[m_uiActiveBufferIndex]	= uiSize;

	return 0;
}

int CDoubleBuffer::RawPush( const void* pSrc, const UINT uiLen )
{
	// �������� ����ȭ �Ǵ� �Լ�
	if( m_uiBufferOffset[m_uiActiveBufferIndex] + uiLen > m_uiBufferSize[m_uiActiveBufferIndex] )
	{
		if( Resize( m_uiBufferSize[m_uiActiveBufferIndex]*2 ) < 0 )
			return -1;

		// Buffer Resize �ص� ���ڶ��ٸ� ����ó���Ѵ�.
		// Buffer �� �ٽ� Resize �ص� ������ �������� ��� �߻��� �� ���� ����̱� ������ ���� ����ó�� ���� �ʴ´�.
		if( m_uiBufferOffset[m_uiActiveBufferIndex] + uiLen > m_uiBufferSize[m_uiActiveBufferIndex] )
			return -1;
	}

	char* pBuffer = m_pBuffer[m_uiActiveBufferIndex] + m_uiBufferOffset[m_uiActiveBufferIndex];
	memcpy( pBuffer, pSrc, uiLen );
	m_uiBufferOffset[m_uiActiveBufferIndex] += uiLen;

	return 0;
}

//==================================================================================================================================================//
// Class CTcpRecvBuffer
//==================================================================================================================================================//

CTcpRecvBuffer::CTcpRecvBuffer( const UINT uiSize )
: CDoubleBuffer( uiSize )
{
}

CTcpRecvBuffer::~CTcpRecvBuffer()
{
}

int CTcpRecvBuffer::Push( CSocketContext *pSocketContext )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	int			iPushCount		= 0;
	const char* pRecvBuffer		= pSocketContext->m_RecvIO.buffer;
	UINT		uiRecvBufferLen	= pSocketContext->m_RecvIO.Len;

	while( uiRecvBufferLen )
	{
		UINT uiPacketLen = GetCompletePacketLength( pRecvBuffer, uiRecvBufferLen );
		if( uiPacketLen <= 0 )
			break;

		if( RawPush( pRecvBuffer, uiPacketLen ) < 0 )
			return -1;

		pRecvBuffer		+= uiPacketLen;
		uiRecvBufferLen	-= uiPacketLen;
		++iPushCount;
	}

	if( iPushCount > 0 )
	{
		pSocketContext->m_RecvIO.Len = uiRecvBufferLen;
		if( uiRecvBufferLen )
			memmove( &pSocketContext->m_RecvIO.buffer, pRecvBuffer, uiRecvBufferLen );
	}
	
	return iPushCount;
}

int	CTcpRecvBuffer::Push( const DNTPacket& Packet )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	return RawPush( &Packet, Packet.iLen );
}

UINT CTcpRecvBuffer::GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const
{
	// �������� ����ȭ �Ǵ� �Լ�
	//										   iLen          + cMainCmd    + cSubCmd
	static const UINT uiTcpPacketHeaderSize  = sizeof(USHORT)+sizeof(UCHAR)+sizeof(UCHAR);
	
	if( uiLen < uiTcpPacketHeaderSize )
		return 0;

	const DNTPacket* pPacket = static_cast<const DNTPacket*>(pBuffer);

	return (pPacket->iLen <= uiLen) ? pPacket->iLen : 0;
}

//==================================================================================================================================================//
// Class CRawBuffer
//==================================================================================================================================================//

CRawBuffer::CRawBuffer( const UINT uiSize )
: CDoubleBuffer( uiSize ), m_bIsLock( false )
{
}

CRawBuffer::~CRawBuffer()
{
}

int CRawBuffer::Push( const void* pSrc, const UINT uiLen )
{
	_ASSERT( m_bIsLock == true );

	// �������� LOCK �ɾ���� ��
	if( RawPush( pSrc, uiLen ) < 0 )
		return -1;

	return uiLen;
}

void CRawBuffer::Lock()
{
	m_Lock.Lock();
	m_bIsLock = true;
}

void CRawBuffer::UnLock()
{
	m_Lock.UnLock();
	m_bIsLock = false;
}

UINT CRawBuffer::GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const
{
	return uiLen;
}
