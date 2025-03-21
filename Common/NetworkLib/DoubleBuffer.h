
#pragma once

//==================================================================================================================================================//
// Class CDoubleBuffer
//==================================================================================================================================================//

class CDoubleBuffer
{
public:

	CDoubleBuffer( const UINT uiSize );
	virtual ~CDoubleBuffer();

	void			Clear();
	int				Switch( char*& pReturnBuffer, UINT& uiReturnSize );
#if defined( _DB )
	int				Pop( char* pDest, UINT& uiReturnSize );
#endif
	virtual int		Push( CSocketContext* pSocketContext ){ return -1; }
	virtual int		Push( const DNTPacket& Packet ){ return -1; }
	virtual int		Push( const void* pSrc, const UINT uiLen ){ return -1; }
	// 기본적으로 모든 public 함수들은 내부적으로 Lock을 걸어준다.
	// 수동으로 Lock/UnLock 을 해줘야 하는 버퍼인 경우 상속받아 처리한다.
	virtual void	Lock(){};
	virtual void	UnLock(){};

private:

	int				Resize( const UINT32 uiSize );

protected:
	
	virtual UINT	GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const = 0;
	int				RawPush( const void* pSrc, const UINT uiLen );

	UINT			m_uiActiveBufferIndex;
	UINT			m_uiBufferSize[2];
	char*			m_pBuffer[2];
	UINT			m_uiBufferOffset[2];
	CSyncLock		m_Lock;
};

//==================================================================================================================================================//
// Class CTcpRecvBuffer
//==================================================================================================================================================//

class CTcpRecvBuffer : public CDoubleBuffer
{
public:

	CTcpRecvBuffer( const UINT uiSize );
	~CTcpRecvBuffer();

	int				Push( CSocketContext* pSocketContext );
	int				Push( const DNTPacket& Packet );

private:

	UINT			GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const;
};

//==================================================================================================================================================//
// Class CRawBuffer
//==================================================================================================================================================//

class CRawBuffer : public CDoubleBuffer
{
public:

	CRawBuffer( const UINT uiSize );
	~CRawBuffer();

	int				Push( const void* pSrc, const UINT uiLen );
	void			Lock();
	void			UnLock();

private:

	UINT			GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const;

	bool			m_bIsLock;
};
