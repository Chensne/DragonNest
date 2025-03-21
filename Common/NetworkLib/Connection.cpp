#include "StdAfx.h"
#include "Connection.h"
#include "Log.h"
#include "DNPacket.h"
#include "DNProtocol.h"
#include "DNServerDef.h"
#include "DNServerProtocol.h"
#include "DNSecure.h"
#include "SpinBuffer.h"

#ifdef _PACKET_COMP
#include "zlib.h"
#endif

#if defined(_SERVER)

CConnection::CConnection(void)
: m_nSessionID(0), m_pIocpManager(NULL), m_pRecvBuffer(NULL), m_pSendBuffer(NULL), 
  m_bActive(false), m_bDelete(false), m_pSocketContext(NULL), m_bServerConnection(false)
#ifdef _PACKET_COMP
  ,m_pCompBuffer(NULL)
  ,m_bUseComp(false)
#endif
  ,m_bConnecting(false)
  ,m_bDetachFlag(false)
  ,m_hIocp(INVALID_HANDLE_VALUE)
{
	memset(m_DebugInfo, 0, sizeof(_DEBUG_INFO[32]));
	m_szIP[0] = '\0';
	m_wszIP[0] = '\0';
	m_wPort = 0;
	m_DebugInfoCount = 0;
	m_nStagnatedCount = 0;
	m_bPushSendQueue	= false;
	SecureLib_InitSA(&m_ClientSA);
}

CConnection::~CConnection(void)
{
	Final();
}

int CConnection::Init(int nRecvLen, int nSendLen, int nCompLen)
{
	Final();

	if (nRecvLen < 8192) nRecvLen = 8192;
	if (nSendLen < 8192) nSendLen = 8192;
	if (nCompLen < 6144) nCompLen = 6144;

#if defined( _DBSERVER ) || defined( _LOGSERVER ) || defined( _CASHSERVER )
	UINT uiAccessThreadCount = THREADMAX;
#else
	UINT uiAccessThreadCount = 1;
#endif
	m_pRecvBuffer = new (std::nothrow)CTcpRecvSpinBuffer( uiAccessThreadCount, nRecvLen );
	if (!m_pRecvBuffer)
		return -1;

	m_SendSync.Lock();
	m_pSendBuffer = new CBuffer(nSendLen);
	if (!m_pSendBuffer)
	{
		m_SendSync.UnLock();
		return -1;
	}
	m_SendSync.UnLock();

#ifdef _PACKET_COMP
	//지금은 유저용 컨넥션에만 대응합니다.
	if (nCompLen > 0)
	{
		m_pCompBuffer = new (std::nothrow)CCompPacketBuffer( 1, nCompLen );
		if (!m_pCompBuffer)
			return -1;
	}
#endif

	return 0;
}

void CConnection::Final()
{
	SAFE_DELETE(m_pRecvBuffer);
#ifdef _PACKET_COMP
	SAFE_DELETE(m_pCompBuffer);
#endif

	m_SendSync.Lock();
	SAFE_DELETE(m_pSendBuffer);
	m_SendSync.UnLock();

	if (m_pSocketContext && m_pIocpManager)
		m_pIocpManager->ClearSocketContext(m_pSocketContext);

	m_bActive = m_bServerConnection = false;
}

int CConnection::AddRecvData( CSocketContext* pSocketContext )
{
#if defined(_LOGINSERVER)
	if (m_bDetachFlag == true)
		return COMPLETE;
#else 
	if (m_bDetachFlag == true)
		return COMPLETE;
#endif // #if defined(_LOGINSERVER)

	if( m_pRecvBuffer->Push( pSocketContext ) < 0 )
		return -1;

	return COMPLETE;
}

int CConnection::AddRecvData( const DNTPacket& Packet )
{
	if( m_pRecvBuffer->Push( Packet ) < 0 )
		return -1;

	return COMPLETE;
}

int CConnection::AddRecvData(  const DNEncryptPacketSeq& Packet  )
{
	if( m_pRecvBuffer->Push( Packet ) < 0 )
		return -1;

	return COMPLETE;
}

int CConnection::AddSendData(int iMainCmd, int iSubCmd, char *pData, int iLen, TParamData* pParamData, BYTE cSeq, bool bForceUncomp)
{
	if (GetDetachFlag())
		return -1;

	_ASSERT(iMainCmd < 255);
	_ASSERT(iSubCmd < 255);
	_ASSERT(iLen>=0);

	int Ret = 0;
	int nBufLen = 0;

	if (GetDelete()) return -1;
	if( !GetActive() )
		return -1;

	if (m_pIocpManager && m_pSocketContext && (m_pSocketContext->m_Socket != INVALID_SOCKET))
	{
		switch (m_pSocketContext->m_dwKeyParam)
		{
		case CONNECTIONKEY_USER:	// 유저는 암호화
			{
#ifdef _PACKET_COMP
				if (m_bUseComp && bForceUncomp == false)
				{
					if (m_pCompBuffer && iLen > COMPRESSMINSIZE)
					{
						DNTCompPacket packet;
						memset(&packet, 0, sizeof(packet));

						packet.header.cSeq = cSeq;
						packet.header.cMainCmd = static_cast<unsigned char>(iMainCmd);
						packet.header.cSubCmd = static_cast<unsigned char>(iSubCmd);

						UINT nLeftBuffetrSize = ((CCompPacketBuffer*)m_pCompBuffer)->GetLeftBufferSize();

						UINT nCompPacketSize = sizeof(packet.buf);
						compress2((Bytef*)&packet.buf, (uLongf*)&nCompPacketSize, (Bytef*)pData, (uLongf)iLen, Z_BEST_SPEED);

						packet.header.nLen = static_cast<short>(sizeof(DNTPacketCompHeader) + nCompPacketSize);

						if (packet.header.nLen > nLeftBuffetrSize)
							FlushCompData();
						Ret = m_pCompBuffer->Push(packet);
					}
					else
						return AddSendData(iMainCmd, iSubCmd, pData, iLen, pParamData, cSeq, true);
				}
				else
#endif
				{
#ifdef _PACKET_COMP
					//순서보장을 위해서 쌓여있는 버퍼가 있으면 먼저 플러싱한다.
					if (m_pCompBuffer) FlushCompData();
#endif

					if( pParamData && pParamData->bEncrypt )
					{
						m_SendSync.Lock();
						Ret = m_pSendBuffer->Push( (char*)&pParamData->sEncrypt, pParamData->sEncrypt.nLen, true );
						m_SendSync.UnLock();

						nBufLen =  pParamData->sEncrypt.nLen;
					}
					else
					{
						DNEncryptPacketSeq EnPacket = { 0, };
						EnPacket.cSeq = cSeq;
						EnPacket.Packet.iLen = static_cast<unsigned short>(sizeof(DNTPacketHeader)+iLen);
						EnPacket.Packet.cMainCmd = static_cast<unsigned char>(iMainCmd);
						EnPacket.Packet.cSubCmd = static_cast<unsigned char>(iSubCmd);

						if( iLen < 0 || iLen > sizeof(EnPacket.Packet.buf) )
						{
							_DANGER_POINT_MSG( L"if( iLen < 0 || iLen > sizeof(EnPacket.Packet.buf) )" );
							//_DANGER_POINT();
							return -1;
						}

						memcpy(&EnPacket.Packet.buf, pData, iLen);

						EnPacket.nLen = EnPacket.Packet.iLen + sizeof(BYTE) + sizeof(USHORT);
						CDNSecure::GetInstance().Tea_encrypt( reinterpret_cast<char*>(&EnPacket.Packet), EnPacket.Packet.iLen );

						if (EnPacket.nLen > 0)
						{
							DWORD dwTime = timeGetTime();
							do
							{
								bool bIncreaseBuffer = false;
								// 중국 VVIP 유저 때문에 캐시인벤리스트 예외처리
								if( iMainCmd == SC_ITEM && iSubCmd == eItem::SC_CASHINVENLIST )
									bIncreaseBuffer = true;

								m_SendSync.Lock();
								Ret = m_pSendBuffer->Push( (char*)&EnPacket, EnPacket.nLen, true, bIncreaseBuffer );
								m_SendSync.UnLock();

								// 버퍼부족
								if( Ret == -2 )
								{
									FlushSendData();
								}
							}while( Ret < 0 && timeGetTime()-dwTime<1000 );
						}
						else Ret = -1;	//encrypt failed

						if (Ret < 0) _ASSERT(0);

						nBufLen = EnPacket.nLen;
						if( pParamData && pParamData->bEncrypt == false )
						{
							memcpy( &pParamData->sEncrypt, &EnPacket, EnPacket.nLen );
							pParamData->bEncrypt = true;
						}
					}
				}
			}
			break;

#ifdef PRE_ADD_DOORS
		case CONNECTIONKEY_DOORS:
			{
				DNDoorsPacketHeader dnDoors;
				memset(&dnDoors, 0, sizeof(DNDoorsPacketHeader));

				dnDoors.datasize = static_cast<unsigned short>(sizeof(DNDoorsPacketHeader) + iLen);
				dnDoors.protocol = static_cast<unsigned short>(iMainCmd);

				m_SendSync.Lock();
				Ret = m_pSendBuffer->Push( (char*)&dnDoors, sizeof(DNDoorsPacketHeader) );	// 헤더
				if( Ret == 0 && iLen > 0 )	
					Ret = m_pSendBuffer->Push( pData, iLen );						// 데이터
				m_SendSync.UnLock();

				nBufLen = dnDoors.datasize;
			}
			break;
#endif	// #ifdef PRE_ADD_DOORS

		default:	// 그외는 암호화 안한다
			{
				DNTPacketHeader dnHeader;
				dnHeader.iLen		= static_cast<unsigned short>(sizeof(dnHeader)+iLen);
				dnHeader.cMainCmd	= static_cast<unsigned char>(iMainCmd);
				dnHeader.cSubCmd	= static_cast<unsigned char>(iSubCmd);

				m_SendSync.Lock();
				Ret = m_pSendBuffer->Push( (char*)&dnHeader, sizeof(dnHeader) );	// 헤더
				if( Ret == 0 && iLen > 0 )	
					Ret = m_pSendBuffer->Push( pData, iLen );						// 데이터
				m_SendSync.UnLock();

				nBufLen = dnHeader.iLen;
			}
			break;
		}

		if (Ret == 0)
		{
			m_pIocpManager->m_nAddSendBufSize += nBufLen;
			m_pIocpManager->AddSendCall(m_pSocketContext);
		}
		else if (Ret < 0)
			m_pIocpManager->DetachSocket(m_pSocketContext, L"AddSendData Error");
	}
	return Ret;
}

#if (defined(_KR) || defined(_US)) && (defined(_LOGINSERVER) || defined(_MASTERSERVER) || defined(_CASHSERVER))

int CConnection::AddSendData(char *pData, int nLen)
{
	int Ret = 0;

	if (GetDelete()) return -1;
	if (m_pIocpManager && m_pSocketContext && (m_pSocketContext->m_Socket != INVALID_SOCKET))
	{
		m_SendSync.Lock();
		Ret = m_pSendBuffer->Push( pData, nLen );						// 데이터
		m_SendSync.UnLock();

		if (Ret == 0)
		{
			m_pIocpManager->m_nAddSendBufSize += nLen;
			m_pIocpManager->AddSendCall(m_pSocketContext);
		}
		else if (Ret < 0)
			m_pIocpManager->DetachSocket(m_pSocketContext, L"AddSendData Error");
	}

	return Ret;
}

#endif


// 2009.01.23 김밥
// FlushSendData() 함수에서 정상적으로 Send가 성공했거나 보낼 데이터가 없으면 0을 리턴하고 그 이외의 경우에는 0 이 아닌값을 리턴해서
// SendThread 에서 queue 비워지지 않게 처리해준다.

int CConnection::FlushSendData()
{
	if (m_pSocketContext && m_pIocpManager)
	{
		//현재 구조로는 send Thread단에서의 억세스입니다. SendBuffer에 기본적으로 자주 억세스 되는 곳은 ProcessThread와
		//SendThread가 되겠습니다아~
		ScopeLock<CSyncLock> Lock( m_SendSync );

		int nSize = GetSendCount();
		if (nSize <= 0)
		{
			if (GetConnectionKey() == CONNECTIONKEY_USER && GetDetachFlag())
				return -1;
			return 0;
		}

#ifdef _USE_SENDCONTEXTPOOL

		ScopeLock<CSyncLock> SendLock(m_pSocketContext->m_SendIOLock);
		
		while (1)
		{
			nSize = GetSendCount();
			if (nSize > 0)
			{
				TIOContext * pContext = m_pSocketContext->GetSendIO();
				if (pContext)
				{
					pContext->Len = ViewSendData(pContext->buffer, INTERNALBUFFERLENMAX);
					if(pContext->Len > 0)
					{
						int PostSendRet = m_pIocpManager->PostSend(m_pSocketContext, pContext);
						//PostSend를 Call하면서 실재 AsyncroSend를 하게 됩니다.
						if( PostSendRet == 0 )
						{
							//buffer skip
							SkipSendData(pContext->Len);
							//SendCount Test Codes
							m_pIocpManager->m_nPostSendSize += pContext->Len;
							nSize = 0;
						}
						else
						{
							m_pSocketContext->ReleaseSendIO(pContext, pContext->Len); //Context를 돌려주고 다시 돌게 한다.
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
							if( PostSendRet == IN_DISCONNECT )
								return IN_DISCONNECT;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
							return 1;
						}
					}
					else
					{
						//카운트 체크를 미리하고 들어 왔는데 이러면 안뎀.
						_DANGER_POINT();
						m_pSocketContext->ReleaseSendIO(pContext, pContext->Len); //Context를 돌려주고 다시 돌게 한다.
						return 1;
					}
				}
				else
				{
					// 로그 추가
					//_DANGER_POINT();
					return 1;
				}
			}
			else 
			{
				if (GetConnectionKey() == CONNECTIONKEY_USER && GetDetachFlag())
					return -1;
				return 0;
			}
		}
#else
		if (m_pSocketContext->m_SendIO.Len == 0)
		{
			//IOLen의 Decrease는 IOCP(하위개념)단에서 SendComplete가 뜨면 줄어듭니다. 그러니까 이렇게 되면 컴플릿이 뜨지 않으면,
			//SendProcess는 Block되진 않으나 다음 턴으로 넘어 가게 됩니다.
			//그럼 complete가 떨어지지 않는다면 계속 쌓이게 되는 구조......
			m_pSocketContext->m_SendIO.Len = ViewSendData(m_pSocketContext->m_SendIO.buffer, INTERNALBUFFERLENMAX);
			if( m_pSocketContext->m_SendIO.Len > 0 )
			{
				//PostSend를 Call하면서 실재 AsyncroSend를 하게 됩니다.
				if( m_pIocpManager->PostSend(m_pSocketContext) == 0 )
				{
					//SendCount Test Codes
					m_pIocpManager->m_nPostSendSize += m_pSocketContext->m_SendIO.Len;
				}
			}

			if (GetConnectionKey() == CONNECTIONKEY_USER && GetDetachFlag() && GetSendCount() <= 0)
				return -1;
			return 0;
		}
#endif
	}

	return 1;
}

#ifdef _USE_SENDCONTEXTPOOL
bool CConnection::SendComplete(TIOContext * pContext, int nSize)
{
	ScopeLock<CSyncLock> Lock( m_SendSync );
	return m_pSocketContext->ReleaseSendIO(pContext, nSize);
}
#else
bool CConnection::SendComplete(int nSize)
{
	ScopeLock<CSyncLock> Lock( m_SendSync );

	SkipSendData(nSize);
	m_pSocketContext->m_SendIO.Len -= nSize;
	if (m_pSocketContext->m_SendIO.Len == 0)
		return false;
	return true;
}
#endif

int CConnection::GetSendCount()
{
	return m_pSendBuffer->GetCount();
}

int CConnection::ViewSendData(char *pData, int nMaxSize)
{
	// 상위에서 동기화 되는 함수
	int Size = m_pSendBuffer->GetCount();
	if (Size > 0){
		if (Size > nMaxSize) Size = nMaxSize;
		m_pSendBuffer->View(pData, Size);
	}
	return Size;
}

void CConnection::SkipSendData(int nSize)
{
	// 상위에서 동기화 되는 함수
	m_pSendBuffer->Skip(nSize);
}

bool CConnection::FlushRecvData(ULONG nCurTick)
{
#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock( m_FlushRecvDataLock );
#endif
	ScopeSpinBufferSwitch Scope( m_pRecvBuffer );

	char*	pBuffer = Scope.pGetBuffer();
	UINT	uiSize	= Scope.uiGetSize();
	int nCnt = 0, nElapsed = 0, nBufLen = 0;

	int nRet = ERROR_UNKNOWN_HEADER, nElapsedTick = 0;
	while( uiSize )
	{
		if (m_pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER){	// 유저는 암호화
			DNEncryptPacketSeq * pEnPacket = (DNEncryptPacketSeq*)pBuffer;
			int nLen = pEnPacket->nLen - sizeof(BYTE) - sizeof(USHORT);

			// BufferOverrun
			if( nLen <= 0 || static_cast<int>(uiSize) < pEnPacket->nLen )
			{
				g_Log.Log( LogType::_ERROR, L"## FlushRecvData() BufferOverRun Check-1!!!" );
				return false;
			}

			CDNSecure::GetInstance().Tea_decrypt( reinterpret_cast<char*>(&pEnPacket->Packet), nLen );
			if( pEnPacket->Packet.cMainCmd == IN_DISCONNECT || nLen <= 0 )
				return false;

			nElapsedTick = 0;
			nRet = FlushRecvMessage( (char*)&pEnPacket->Packet, nElapsedTick, nCurTick );

			//유저인 경우에만 끊어버린다.
			//if (nRet == ERROR_INVALIDPACKET || nRet == ERROR_UNKNOWN_HEADER)
			if (nRet == ERROR_INVALIDPACKET )
			{
				DNTPacket * pPacket = (DNTPacket*)&pEnPacket->Packet;
				g_Log.Log( LogType::_ERROR, 0, 0, 0, GetSessionID(), L"## Invalid Packet RETCODE:%d SID:%u MCMD:%d SCMD:%d\r\n", nRet, GetSessionID(), pPacket->cMainCmd, pPacket->cSubCmd);
				return false;		//여기서 끊어지면 처리하지 않는 패킷을 수신하거나 패킷이 맞지 않는것.
			}
			nElapsed += nElapsedTick;
			nBufLen = pEnPacket->nLen;
		}
#ifdef PRE_ADD_DOORS
		else if (m_pSocketContext->m_dwKeyParam == CONNECTIONKEY_DOORS)
		{
			DNDoorsPacket * pPacket = reinterpret_cast<DNDoorsPacket*>(pBuffer);
			nRet = MessageProcess(pPacket->header.protocol, 0, pPacket->data, pPacket->header.datasize - (int)(sizeof(*pPacket) - sizeof(pPacket->data)));;
			nBufLen = pPacket->header.datasize;
		}
#endif		//#ifdef PRE_ADD_DOORS
		else {	// 그외는 암호화 안한다
			DNTPacket* pHeader = reinterpret_cast<DNTPacket*>(pBuffer);
			if( pHeader->cMainCmd == IN_DISCONNECT )
				return false;

			nElapsedTick = 0;
			nRet = FlushRecvMessage( pBuffer, nElapsedTick, nCurTick );
			nElapsed += nElapsedTick;
			nBufLen = pHeader->iLen;
		}

		// BufferOverrun
		if( static_cast<int>(uiSize) < nBufLen )
		{
			g_Log.Log( LogType::_ERROR, L"## FlushRecvData() BufferOverRun Check-2!!!" );
			return false;
		}

		pBuffer += nBufLen;
		uiSize  -= nBufLen;

		nCnt++;
	}

	if (nCnt > STORED_RECV_LIMIT && nElapsed > STORED_RECV_ELAPSED_TICK)
	{
		if (m_nStagnatedCount > STAGNATED_DISCONNECT_COUNT)
			m_pIocpManager->StagnatePacket(nCnt, nElapsed, m_pSocketContext);
		else
			m_nStagnatedCount++;
	}
	else
		m_nStagnatedCount = 0;

	return true;
}

#if defined (_DBSERVER) || defined (_LOGSERVER) || defined(_CASHSERVER)

bool CConnection::FlushDBData( int nThreadID )
{
	ScopeSpinBufferSwitch Scope( m_pRecvBuffer );

	char*	pBuffer	= Scope.pGetBuffer();
	UINT32	uiSize	= Scope.uiGetSize();
	
	while( uiSize )
	{
		DNTPacket* pHeader = reinterpret_cast<DNTPacket*>(pBuffer);
		if( pHeader->cMainCmd == IN_DISCONNECT )
			return false;

		DBMessageProcess( pBuffer, nThreadID );

		pBuffer += pHeader->iLen;
		uiSize  -= pHeader->iLen;
	}

	return true;
}

#endif	// #if defined (_DBSERVER)

#if defined(_KR) && (defined(_LOGINSERVER) || defined(_MASTERSERVER))
bool CConnection::FlushAuthData()
{
	ScopeSpinBufferSwitch Scope( m_pRecvBuffer );

	char*	pBuffer = Scope.pGetBuffer();
	UINT	uiSize	= Scope.uiGetSize();

	while( uiSize )
	{
		TAuthHeader *pHeader = reinterpret_cast<TAuthHeader*>(pBuffer);
		USHORT wLength = SWAP16(pHeader->wLength) + 3;
		MessageProcess(0, 0, pBuffer, uiSize);

		pBuffer += wLength;
		uiSize  -= wLength;
	}

	return true;
}

#elif defined(_CASHSERVER) && (defined(_KR) || defined(_US))
bool CConnection::FlushBillingData()
{
	ScopeSpinBufferSwitch Scope( m_pRecvBuffer );

	char*	pBuffer = Scope.pGetBuffer();
	UINT	uiSize	= Scope.uiGetSize();

	while( uiSize )
	{
		TBillingHeader *pHeader = reinterpret_cast<TBillingHeader*>(pBuffer);
		UINT nLength = SWAP32(pHeader->nLength) + 5;
		MessageProcess(0, 0, pBuffer, uiSize);

		pBuffer += nLength;
		uiSize  -= nLength;
	}

	return true;
}

#endif

#if defined(_TW)
bool CConnection::FlushAuthData_TW()
{
	ScopeSpinBufferSwitch Scope( m_pRecvBuffer );

	char*	pBuffer = Scope.pGetBuffer();
	UINT	uiSize	= Scope.uiGetSize();

	while( uiSize )
	{
		// \r\n 을 기준으로 파싱하도록 수정 필요 !!!

		USHORT wLength = 0;
		for (INT iIndex = 0 ; (static_cast<int>(uiSize) - 1) > iIndex ; ++iIndex) {
			if ('\r' == pBuffer[iIndex] && '\n' == pBuffer[iIndex + 1]) 
			{
				wLength = iIndex + 2;
				pBuffer[iIndex] = '\0';
				break;
			}
		}
		if (!wLength) {
			return false;
		}

		MessageProcess(0, 0, pBuffer, uiSize);

		pBuffer += wLength;
		uiSize  -= wLength;
	}

	return true;
}
#endif	// #if defined(_TW)

#if defined(_TH)
bool CConnection::FlushAuthData_TH()
{
	ScopeSpinBufferSwitch Scope( m_pRecvBuffer );

	char*	pBuffer = Scope.pGetBuffer();
	UINT	uiSize	= Scope.uiGetSize();

	while( uiSize )
	{
		USHORT wLength = 0;
		for (INT iIndex = 0 ; (static_cast<int>(uiSize)) > iIndex ; ++iIndex) 
		{
			if (';' == pBuffer[iIndex])
			{
				wLength = iIndex + 1;
				pBuffer[iIndex] = '\0';
				break;
			}
		}
		if (!wLength) 
		{
			return false;
		}

		MessageProcess(0, 0, pBuffer, wLength);

		pBuffer += wLength;
		uiSize  -= wLength;
	}
	return true;
}

bool CConnection::FlushOTPData_TH()
{
	ScopeSpinBufferSwitch Scope( m_pRecvBuffer );

	char*	pBuffer = Scope.pGetBuffer();
	UINT	uiSize	= Scope.uiGetSize();

	pBuffer[uiSize-1] = '\0';

	MessageProcess(0, 0, pBuffer, uiSize);

	pBuffer += uiSize;
	uiSize  -= uiSize;
	return true;
}
#endif	// #if defined(_TW)

#ifdef _PACKET_COMP
void CConnection::SetPacketComp(bool bComp)
{
	if (m_pCompBuffer == NULL) return;
	if (bComp == false)
	{
		FlushCompData();
		m_bUseComp = false;
	}
	else
		m_bUseComp = true;
}

void CConnection::FlushCompData()
{
	if (m_pCompBuffer == NULL) return;
	ScopeSpinBufferSwitch Scope(m_pCompBuffer);

	char*	pBuffer	= Scope.pGetBuffer();
	UINT32	uiSize	= Scope.uiGetSize();
	if (uiSize <= 0) return;

	DNEncryptPacketSeq EnPacket = { 0, };

	m_SendSync.Lock();

	EnPacket.cSeq = COMPRESSPACKET;
	memcpy(&EnPacket.Buf, pBuffer, uiSize);

	EnPacket.nLen = EnPacket.Packet.iLen + sizeof(BYTE) + sizeof(USHORT);
	CDNSecure::GetInstance().Tea_encrypt( reinterpret_cast<char*>(&EnPacket.Packet), EnPacket.Packet.iLen );

	int Ret = 0;
	if (EnPacket.nLen > 0)	Ret = m_pSendBuffer->Push( (char*)&EnPacket, EnPacket.nLen, true );
	else Ret = -1;

	m_SendSync.UnLock();

	if (Ret < 0)	_ASSERT(0);
	int nBufLen = EnPacket.nLen;

	if (Ret == 0)
	{
		m_pIocpManager->m_nAddSendBufSize += nBufLen;
		m_pIocpManager->AddSendCall(m_pSocketContext);
	}
	else if (Ret < 0)
		m_pIocpManager->DetachSocket(m_pSocketContext, L"AddSendData Error");
}
#endif

int CConnection::FlushRecvMessage(char * pData, int &nElapsedTick, ULONG nCurTick)
{
	DNTPacket * pPacket = (DNTPacket*)pData;

	int cnt = m_DebugInfoCount++&31;
	m_DebugInfo[cnt]._DebugMainCmd = pPacket->cMainCmd;
	m_DebugInfo[cnt]._DebugSubCmd = pPacket->cSubCmd;
	m_DebugInfo[cnt]._DebugTick = nCurTick;

	int nRet = MessageProcess(pPacket->cMainCmd, pPacket->cSubCmd, pPacket->buf, pPacket->iLen - (int)(sizeof(*pPacket) - sizeof(pPacket->buf)));

	//test
	if (m_pIocpManager && m_pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
		m_pIocpManager->m_nProcessBufSize += pPacket->iLen;

	return nRet;
}

void CConnection::SetSocketContext(CIocpManager *pIocpManager, CSocketContext *pSocketContext)
{
	m_pIocpManager = pIocpManager;
	m_pSocketContext = pSocketContext;
}	

void CConnection::BufferClear()
{
	m_SendSync.Lock();
	if (m_pSendBuffer) m_pSendBuffer->Clear();
	m_SendSync.UnLock();

	m_pRecvBuffer->Clear();
}

void CConnection::RecvBufferClear()
{
	m_pRecvBuffer->Clear();
}

void CConnection::SendBufferClear()
{
	m_SendSync.Lock();
	if (m_pSendBuffer) m_pSendBuffer->Clear();
	m_SendSync.UnLock();
}

void CConnection::DetachConnection(wchar_t *pwszIdent)
{
	printf("[DETACH_CONNECTION] : %ws \n",pwszIdent);
	if( pwszIdent && wcslen(pwszIdent) > 0)	
		m_pSocketContext->SetDetachReason(pwszIdent);

	m_pIocpManager->DetachSocket(m_pSocketContext, pwszIdent == NULL ? const_cast<wchar_t*>(m_wstrDelayDetachReson.c_str()) : pwszIdent);	
}

void CConnection::DelayDetachConnection(wchar_t *pwszIdent)
{
	if (GetConnectionKey() == CONNECTIONKEY_USER)
	{
		m_wstrDelayDetachReson.clear();
		m_wstrDelayDetachReson = pwszIdent;
		m_bDetachFlag = true;
	}
}

void CConnection::SetDelete(bool bDelete)
{
	m_DeleteLock.Lock();
	m_bDelete = bDelete;
	m_DeleteLock.UnLock();
}

bool CConnection::GetDelete()
{
	m_DeleteLock.Lock();
	bool bDelete = m_bDelete;
	m_DeleteLock.UnLock();

	return bDelete;
}

DWORD CConnection::GetConnectionKey()
{
	if (m_pSocketContext) return m_pSocketContext->m_dwKeyParam;
	else return CONNECTIONKEY_DEFAULT;
}

#endif	// #if defined(_SERVER)