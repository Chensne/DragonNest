
#pragma once

#if defined(_SERVER)

//==================================================================================================================================================//
// Class ScopeSpinBufferSwitch
//==================================================================================================================================================//

class CSpinBuffer;

class ScopeSpinBufferSwitch
{
public:

	ScopeSpinBufferSwitch( CSpinBuffer* pSpinBuffer );
	~ScopeSpinBufferSwitch();

	char*			pGetBuffer(){ return m_pBuffer; }
	UINT			uiGetSize(){ return m_uiSize; }

private:

	CSpinBuffer*	m_pSpinBuffer;
	char*			m_pBuffer;
	UINT			m_uiSize;
	UINT			m_uiBufferIndex;
};

//==================================================================================================================================================//
// Class CSpinBuffer
//==================================================================================================================================================//

class CSpinBuffer
{
public:

	CSpinBuffer( const UINT uiAccessThreadCount, const UINT uiSize );
	virtual ~CSpinBuffer();

	void				Clear();
	UINT				Switch( char*& pReturnBuffer, UINT& uiReturnSize );
	void				SwitchClear( const UINT uiBufferIndex );
	
	virtual int			Push( CSocketContext* pSocketContext ){ return -1; }
	virtual int			Push( const DNTPacket& Packet ){ return -1; }
	virtual int			Push( const DNTCompPacket& Packet ){ return -1; }
	virtual int			Push( const DNEncryptPacketSeq& Packet ){ return -1; }
	virtual int			Push( const void* pSrc, const UINT uiLen ){ return -1; }
	// 기본적으로 모든 public 함수들은 내부적으로 Lock을 걸어준다.
	// 수동으로 Lock/UnLock 을 해줘야 하는 버퍼인 경우 상속받아 처리한다.
	virtual void		Lock(){};
	virtual void		UnLock(){};

	int					RawPush( const void* pSrc, const UINT uiLen );

private:

	int					Resize( const UINT32 uiSize );

protected:
	
	virtual UINT		GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const = 0;
	CSyncLock			m_Lock;
	UINT				m_uiActiveBufferIndex;
	UINT				m_uiMaxBufferCount;
	std::vector<UINT>	m_vBufferSize;
	std::vector<UINT>	m_vBufferOffset;
	std::vector<char*>	m_vBuffer;
	std::queue<UINT>	m_qBuffer;
};

//==================================================================================================================================================//
// Class CTcpRecvSpinBuffer
//==================================================================================================================================================//

class CTcpRecvSpinBuffer : public CSpinBuffer
{
public:

	CTcpRecvSpinBuffer( const UINT uiAccessThreadCount, const UINT uiSize );
	~CTcpRecvSpinBuffer();

	// 수동락
	void			Lock(){ m_Lock.Lock(); }
	void			UnLock(){ m_Lock.UnLock(); }

	int				Push( CSocketContext* pSocketContext );
	int				Push( const DNTPacket& Packet );
	int				Push( const DNEncryptPacketSeq& Packet );	

	UINT			GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const;
	UINT			GetCompleteAuthPacketLength( const void* pBuffer, const UINT uiLen ) const;
	UINT			GetCompleteBillingPacketLength( const void* pBuffer, const UINT uiLen ) const;
#if defined(_TW)
	UINT			GetCompleteAuthPacketLength_TW( const void* pBuffer, const UINT uiLen ) const;
#elif defined(_TH)
	UINT GetCompleteAuthPacketLength_TH_Auth( const char* pBuffer, const UINT uiLen ) const;
	UINT GetCompleteAuthPacketLength_TH_OTP( const void* pBuffer, const UINT uiLen ) const;
#endif	// #if defined(_TW)
};

//==================================================================================================================================================//
// Class CCompPacketBuffer
//==================================================================================================================================================//
class CCompPacketBuffer : public CSpinBuffer
{
public:
	CCompPacketBuffer( const UINT uiAccessThreadCount, const UINT uiSize );
	~CCompPacketBuffer();

	int Push( const DNTCompPacket& Packet );
	UINT GetLeftBufferSize();

private:
	UINT			GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const { return uiLen; }
};

//==================================================================================================================================================//
// Class CRawSpinBuffer
//==================================================================================================================================================//

class CRawSpinBuffer : public CSpinBuffer
{
public:

	CRawSpinBuffer( const UINT uiAccessThreadCount, const UINT uiSize );
	~CRawSpinBuffer();

	int				Push( const void* pSrc, const UINT uiLen );
	void			Lock();
	void			UnLock();

private:

	UINT			GetCompletePacketLength( const void* pBuffer, const UINT uiLen ) const;

	bool			m_bIsLock;
};

#endif	// #if defined(_SERVER)
