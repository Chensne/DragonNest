#include "StdAfx.h"
#include "../Protocol/DNPacket.h"
#include "SocketContext.h"
#include "SpinBuffer.h"
#include "DnServerDef.h"
#include "DnServerPacketCash.h"
#include "Log.h"
#if defined(_SERVER)

//==================================================================================================================================================//
// Class ScopeSpinBufferSwitch
//==================================================================================================================================================//

ScopeSpinBufferSwitch::ScopeSpinBufferSwitch( CSpinBuffer* pSpinBuffer )
: m_pSpinBuffer( pSpinBuffer ), m_pBuffer( NULL ), m_uiSize( 0 ), m_uiBufferIndex( 0 )
{
	m_uiBufferIndex = m_pSpinBuffer->Switch( m_pBuffer, m_uiSize );
}

ScopeSpinBufferSwitch::~ScopeSpinBufferSwitch()
{
	if( m_uiBufferIndex != UINT_MAX )
		m_pSpinBuffer->SwitchClear( m_uiBufferIndex );
}

//==================================================================================================================================================//
// Class CSpinBuffer
//==================================================================================================================================================//

CSpinBuffer::CSpinBuffer( const UINT uiAccessThreadCount, const UINT uiSize )
: m_uiActiveBufferIndex(0), m_uiMaxBufferCount(0)
{
	_ASSERT( uiAccessThreadCount );
	
	m_uiMaxBufferCount = uiAccessThreadCount+1;

	m_vBufferSize.reserve( m_uiMaxBufferCount );
	m_vBufferOffset.reserve( m_uiMaxBufferCount );
	m_vBuffer.reserve( m_uiMaxBufferCount );

	for( UINT i=0 ; i<m_uiMaxBufferCount ; ++i )
	{
		m_vBuffer.push_back( new (std::nothrow) char[uiSize] );
		m_vBufferOffset.push_back( 0 );
		m_vBufferSize.push_back( uiSize );

		if( i )
			m_qBuffer.push( i );
	}
}

CSpinBuffer::~CSpinBuffer()
{
	for( UINT i=0 ; i<m_uiMaxBufferCount ; ++i )
	{
		if( m_vBuffer[i] )
			delete[] m_vBuffer[i];
	}
}

void CSpinBuffer::Clear()
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	for( UINT i=0 ; i<m_uiMaxBufferCount ; ++i )
		m_vBufferOffset[i] = 0;
}

// [ReturnValue] ReturnBuffer BufferIndex
UINT CSpinBuffer::Switch( char*& pReturnBuffer, UINT& uiReturnSize )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	_ASSERT( !m_qBuffer.empty() );

	UINT uiReturnBufferIndex = m_uiActiveBufferIndex;

	pReturnBuffer	= m_vBuffer[m_uiActiveBufferIndex];
	uiReturnSize	= m_vBufferOffset[m_uiActiveBufferIndex];

	if( uiReturnSize == 0 )
		return UINT_MAX;

	m_uiActiveBufferIndex = m_qBuffer.front();
	m_qBuffer.pop();
	m_vBufferOffset[m_uiActiveBufferIndex] = 0;

	return uiReturnBufferIndex;
}

void CSpinBuffer::SwitchClear( const UINT uiBufferIndex )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	m_qBuffer.push( uiBufferIndex );
}

int CSpinBuffer::Resize( const UINT32 uiSize )
{
	// �������� ����ȭ �Ǵ� �Լ�
	_ASSERT( m_vBufferSize[m_uiActiveBufferIndex] < uiSize );

	char* pResizeBuffer = new (std::nothrow) char[uiSize];
	if( pResizeBuffer == NULL )
		return -1;

	if( m_vBufferOffset[m_uiActiveBufferIndex] )
		memcpy( pResizeBuffer, m_vBuffer[m_uiActiveBufferIndex], m_vBufferOffset[m_uiActiveBufferIndex] );
	
	delete[] m_vBuffer[m_uiActiveBufferIndex];
	m_vBuffer[m_uiActiveBufferIndex]		= pResizeBuffer;
	m_vBufferSize[m_uiActiveBufferIndex]	= uiSize;

	return 0;
}

int CSpinBuffer::RawPush( const void* pSrc, const UINT uiLen )
{
	// �������� ����ȭ �Ǵ� �Լ�
	if( m_vBufferOffset[m_uiActiveBufferIndex] + uiLen > m_vBufferSize[m_uiActiveBufferIndex] )
	{
		if( Resize( m_vBufferSize[m_uiActiveBufferIndex]*2 ) < 0 )
			return -1;

		// Buffer Resize �ص� ���ڶ��ٸ� ����ó���Ѵ�.
		// Buffer �� �ٽ� Resize �ص� ������ �������� ��� �߻��� �� ���� ����̱� ������ ���� ����ó�� ���� �ʴ´�.
		if( m_vBufferOffset[m_uiActiveBufferIndex] + uiLen > m_vBufferSize[m_uiActiveBufferIndex] )
		{
			g_Log.Log(LogType::_ERROR, L"[RawPush] Offset:%u, uiLen:%u, Max:%u\r\n", m_vBufferOffset[m_uiActiveBufferIndex], uiLen, m_vBufferSize[m_uiActiveBufferIndex]);
			return -1;
		}
	}

	char* pBuffer = m_vBuffer[m_uiActiveBufferIndex] + m_vBufferOffset[m_uiActiveBufferIndex];
	memcpy( pBuffer, pSrc, uiLen );
	m_vBufferOffset[m_uiActiveBufferIndex] += uiLen;

	return 0;
}

//==================================================================================================================================================//
// Class CTcpRecvSpinBuffer
//==================================================================================================================================================//

CTcpRecvSpinBuffer::CTcpRecvSpinBuffer( const UINT uiAccessThreadCount, const UINT uiSize )
: CSpinBuffer( uiAccessThreadCount, uiSize )
{
}

CTcpRecvSpinBuffer::~CTcpRecvSpinBuffer()
{
}

int CTcpRecvSpinBuffer::Push( CSocketContext *pSocketContext )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	int			iPushCount		= 0;
	const char* pRecvBuffer		= pSocketContext->m_RecvIO.buffer;
	UINT		uiRecvBufferLen	= pSocketContext->m_RecvIO.Len;

	while( uiRecvBufferLen )
	{
		UINT uiPacketLen = 0;
		if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_AUTH)
			uiPacketLen = GetCompleteAuthPacketLength( pRecvBuffer, uiRecvBufferLen );
		else if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_BILLING)
			uiPacketLen = GetCompleteBillingPacketLength( pRecvBuffer, uiRecvBufferLen );
#if defined(_TW)
		else if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_AUTHLOGIN || pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_AUTHLOGOUT ||
			pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_QUERY || pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_SHOPITEM	||
			pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_COUPON || pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_COUPON_ROLLBACK)
			uiPacketLen = GetCompleteAuthPacketLength_TW( pRecvBuffer, uiRecvBufferLen );
#elif defined(_TH)
		else if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_TH_AUTH || pSocketContext->m_dwKeyParam == CONNECTIONKEY_TH_QUERY || pSocketContext->m_dwKeyParam == CONNECTIONKEY_TH_SHOPITEM)
			uiPacketLen = GetCompleteAuthPacketLength_TH_Auth(pRecvBuffer, uiRecvBufferLen);
		else if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_TH_OTP)
			uiPacketLen = GetCompleteAuthPacketLength_TH_OTP(pRecvBuffer, uiRecvBufferLen);
#endif	// #if defined(_TW)
		else
			uiPacketLen = GetCompletePacketLength( pRecvBuffer, uiRecvBufferLen );

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

int	CTcpRecvSpinBuffer::Push( const DNTPacket& Packet )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	return RawPush( &Packet, Packet.iLen );
}

int	CTcpRecvSpinBuffer::Push( const DNEncryptPacketSeq& Packet )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	return RawPush( &Packet, Packet.nLen );
}

UINT CTcpRecvSpinBuffer::GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const
{
	// �������� ����ȭ �Ǵ� �Լ�
	//										   iLen          + cMainCmd    + cSubCmd
	static const UINT uiTcpPacketHeaderSize  = sizeof(USHORT)+sizeof(UCHAR)+sizeof(UCHAR);
	
	if( uiLen < uiTcpPacketHeaderSize )
		return 0;

	const DNTPacket* pPacket = static_cast<const DNTPacket*>(pBuffer);

	return (pPacket->iLen <= uiLen) ? pPacket->iLen : 0;
}

UINT CTcpRecvSpinBuffer::GetCompleteAuthPacketLength( const void* pBuffer, const UINT uiLen ) const
{
	// �������� ����ȭ �Ǵ� �Լ�
	//										   Header       + Length
	static const UINT uiTcpPacketHeaderSize  = sizeof(BYTE) + sizeof(USHORT);
	
	if( uiLen < uiTcpPacketHeaderSize )
		return 0;

	const TAuthHeader* pPacket = static_cast<const TAuthHeader*>(pBuffer);
	USHORT wLength = SWAP16(pPacket->wLength) + uiTcpPacketHeaderSize;

	return (wLength <= uiLen) ? wLength : 0;
}

UINT CTcpRecvSpinBuffer::GetCompleteBillingPacketLength( const void* pBuffer, const UINT uiLen ) const
{
	// �������� ����ȭ �Ǵ� �Լ�
	//										   Header       + Length
	static const UINT uiTcpPacketHeaderSize  = sizeof(BYTE) + sizeof(UINT);

	if( uiLen < uiTcpPacketHeaderSize )
		return 0;

	const TBillingHeader* pPacket = static_cast<const TBillingHeader*>(pBuffer);
	UINT nLength = SWAP32(pPacket->nLength) + uiTcpPacketHeaderSize;

	return (nLength <= uiLen) ? nLength : 0;
}

#if defined(_TW)
UINT CTcpRecvSpinBuffer::GetCompleteAuthPacketLength_TW( const void* pBuffer, const UINT uiLen ) const
{
	// GASH ������ '\r\n' �� ��Ŷ�� ������ üũ

	UINT nLength = 0;
	for (INT iIndex = 0 ; (static_cast<int>(uiLen) - 1) > iIndex ; ++iIndex) {
		if ('\r' == static_cast<const char*>(pBuffer)[iIndex] && '\n' == static_cast<const char*>(pBuffer)[iIndex + 1]) {
			nLength = iIndex + 2;
			break;
		}
	}

	return (nLength <= uiLen) ? nLength : 0;
}
#elif defined(_TH)
UINT CTcpRecvSpinBuffer::GetCompleteAuthPacketLength_TH_Auth( const char* pBuffer, const UINT uiLen ) const
{
	UINT nLength = 0;
	for (INT iIndex = 0 ; (static_cast<int>(uiLen)) > iIndex ; ++iIndex) {
		if (';' == pBuffer[iIndex]) {
			nLength = iIndex + 1;
			break;
		}
	}

	return (nLength <= uiLen) ? nLength : 0;
}

UINT CTcpRecvSpinBuffer::GetCompleteAuthPacketLength_TH_OTP( const void* pBuffer, const UINT uiLen ) const
{
	return uiLen;
}
#endif	// #if defined(_TH)

//==================================================================================================================================================//
// Class CCompPacketBuffer
//==================================================================================================================================================//
CCompPacketBuffer::CCompPacketBuffer( const UINT uiAccessThreadCount, const UINT uiSize )
: CSpinBuffer( uiAccessThreadCount, uiSize )
{
}

CCompPacketBuffer::~CCompPacketBuffer()
{
}

int CCompPacketBuffer::Push( const DNTCompPacket& Packet )
{
	ScopeLock<CSyncLock> Lock(m_Lock);

	return RawPush(&Packet, Packet.header.nLen);
}

UINT CCompPacketBuffer::GetLeftBufferSize()
{
	ScopeLock<CSyncLock> Lock(m_Lock);
	return m_vBufferSize[m_uiActiveBufferIndex] - m_vBufferOffset[m_uiActiveBufferIndex];
}


//==================================================================================================================================================//
// Class CRawSpinBuffer
//==================================================================================================================================================//

CRawSpinBuffer::CRawSpinBuffer( const UINT uiAccessThreadCount, const UINT uiSize )
: CSpinBuffer( uiAccessThreadCount, uiSize ), m_bIsLock( false )
{
}

CRawSpinBuffer::~CRawSpinBuffer()
{
}

int CRawSpinBuffer::Push( const void* pSrc, const UINT uiLen )
{
	_ASSERT( m_bIsLock == true );

	// �������� LOCK �ɾ���� ��
	if( RawPush( pSrc, uiLen ) < 0 )
		return -1;

	return uiLen;
}

void CRawSpinBuffer::Lock()
{
	m_Lock.Lock();
	m_bIsLock = true;
}

void CRawSpinBuffer::UnLock()
{
	m_Lock.UnLock();
	m_bIsLock = false;
}

UINT CRawSpinBuffer::GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const
{
	return uiLen;
}

#endif	// #if defined(_SERVER)
