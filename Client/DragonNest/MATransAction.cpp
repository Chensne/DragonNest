#include "StdAfx.h"

#include "MATransAction.h"
#include "DnActor.h"

#include "DNPacket.h"
#include "DNProtocol.h"
#include "VillageSendPacket.h"

#include "GameSendPacket.h"

#include "GameClientSession.h"
#include "TaskManager.h"
#include "DnGameTask.h"

#include "DNPacket.h"

#ifdef PACKET_DELAY
const int s_nSendPacketDelayMin = 50;
const int s_nSendPacketDelayMax = 100;

const int s_nRecvPacketDelayMin = 50;
const int s_nRecvPacketDelayMax = 100;
#endif //PACKET_DELAY

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MATransAction::MATransAction()
{
	/*
	for( int i=0; i<3 i++ ) {
		memset( m_pPacketQueueBuffer[i], 0, PACKET_QUEUE_SIZE );
		m_nPacketQueueOffset[i] = 0;
		m_nPacketQueueCount[i] = 0;
	}
	*/
	for( int i=0; i<3; i++ ) {
		m_nPacketQueueSize[i] = 0;
	}
}

MATransAction::~MATransAction()
{
}

void MATransAction::Send( DWORD dwActorProtocol, DnActorHandle hActor, CMemoryStream *pStream, ClientSessionTypeEnum SessionType )
{
}

void MATransAction::Send( DWORD dwActorProtocol, CMemoryStream *pStream, ClientSessionTypeEnum SessionType, bool bImmediate )
{
	if( SessionType == ClientSessionTypeEnum::Auto ) SessionType = m_DefaultSession;
	if( SessionType == ClientSessionTypeEnum::Unknown ) return;

	CDnActor *pActor = dynamic_cast<CDnActor *>(this); // SendSessionID
	if( !pActor ) return;                              // SendSessionID

//	DWORD dwUniqueID = pActor->GetUniqueID();
#ifdef PACKET_DELAY
	if( s_nSendPacketDelayMin == 0 && s_nSendPacketDelayMax == 0 ) {
		switch( SessionType ) {
		case ClientSessionTypeEnum::Tcp:
			SendActorMsg( pActor->GetUniqueID(), dwActorProtocol, ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL, ( pStream ) ? pStream->Tell() : 0 );
			break;
		case ClientSessionTypeEnum::Udp:
			{
				CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
				SendGameActorMsg( pActor->GetUniqueID(), (USHORT)dwActorProtocol, ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL, ( pStream ) ? pStream->Tell() : 0, _RELIABLE, pSession->GetSeqLevel() );
			}
			break;
		}
		return;
	}
	SendPacketDelayStruct Struct;
	Struct.Time = ((CDnActor*)this)->GetLocalTime();
	Struct.dwDelay = s_nSendPacketDelayMin + _rand()%((s_nSendPacketDelayMax+1) - s_nSendPacketDelayMin);
	Struct.pPacket = new char[pStream->Tell()];
	Struct.nSize = pStream->Tell();
	memcpy( Struct.pPacket, pStream->GetBuffer(), pStream->Tell() );
	Struct.dwActorProtocol = dwActorProtocol;
	Struct.dwUniqueID = pActor->GetUniqueID();
	Struct.SessionType = SessionType;
	CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
	if( pSession ) Struct.cSeqLevel = pSession->GetSeqLevel();
	m_VecSendPacketDelayList.push_back( Struct );
#else 
	switch( SessionType ) {
		case ClientSessionTypeEnum::Tcp:
			{
#ifdef PRE_ADD_TESTVOICE
				switch( dwActorProtocol )
				{
					case eActor::CS_CMDMOVE:
						{
							CPacketCompressStream Stream(pStream->GetBuffer(), 128);

							EtVector3 vPos;
							EtVector2 vDir, vLook;

							Stream.Seek( sizeof(DWORD), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
							Stream.Seek( sizeof(int), SEEK_CUR, CPacketCompressStream::INTEGER_SHORT );
							Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
							Stream.Read( &vDir, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
							Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

							OutputDebug("UpdateUser Position [X:%d][Y:%d][Z:%d][R:%d]\n", (int)vPos.x, (int)vPos.y, (int)vPos.z, (int)EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vLook ) ) ));
							break;
						}

					case eActor::CS_CMDMOVEPOS:
						{
							CPacketCompressStream Stream(pStream->GetBuffer(), 128);

							EtVector3 vPos;
							EtVector2 vDir, vLook;

							Stream.Seek( sizeof(DWORD), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
							Stream.Seek( sizeof(int), SEEK_CUR, CPacketCompressStream::INTEGER_SHORT );
							Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
							Stream.Seek( sizeof(EtVector3), SEEK_CUR, CPacketCompressStream::VECTOR3_BIT );
							Stream.Read( &vDir, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

							OutputDebug("UpdateUser Position [X:%d][Y:%d][Z:%d][R:%d]\n", (int)vPos.x, (int)vPos.y, (int)vPos.z, (int)EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vDir ) ) ));
							break;
						}

					case eActor::CS_CMDSTOP:
						{
							CPacketCompressStream Stream(pStream->GetBuffer(), 128);

							EtVector3 vPos;
							EtVector2 vDir, vLook;

							Stream.Seek( sizeof(DWORD), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
							Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

							OutputDebug("UpdateUser Position [X:%d][Y:%d][Z:%d]\n", (int)vPos.x, (int)vPos.y, (int)vPos.z);
							break;
						}
				}
#endif
				
				
				SendActorMsg( pActor->GetUniqueID(), dwActorProtocol, ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL, ( pStream ) ? pStream->Tell() : 0 ); // SendSessionID
			}
			break;
		case ClientSessionTypeEnum::Udp:
			{
				int nProir = _RELIABLE;
				/*
				switch( dwActorProtocol ) {
					case eActor::CS_CMDMOVE:
					case eActor::CS_CMDMOVEPOS:
					case eActor::CS_CMDSTOP:
					case eActor::CS_CMDLOOK:
					case eActor::CS_PLAYERPITCH:
					case eActor::CS_VIEWSYNC:
					case eActor::CS_POSREV:
						nProir = _FAST;
						break;
				}
				*/
				CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
				AddPacketQueue( (BYTE)dwActorProtocol, ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL, ( pStream ) ? pStream->Tell() : 0, nProir );
//				SendGameActorMsg( dwUniqueID, (USHORT)dwActorProtocol, ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL, ( pStream ) ? pStream->Tell() : 0, nProir);
//				OutputDebug( "Send Actor Message : %d - size : %d - Time : %d\n", dwActorProtocol, ( pStream ) ? pStream->Tell() : 0, pActor->GetLocalTime() );
			}
			break;
	}
#endif // PACKET_DELAY
}

void MATransAction::AddPacketQueue( BYTE cSubCmd, BYTE *pBuffer, int nSize, int nPrior )
{
	if( ( sizeof(DWORD) + sizeof(BYTE) + sizeof(WORD) + nSize ) >= PACKET_QUEUE_SIZE ) assert(0);
	if( m_nPacketQueueSize[nPrior] + ( sizeof(DWORD) + sizeof(BYTE) + sizeof(WORD) + nSize ) >= PACKET_QUEUE_SIZE ) {
		FlushPacketQueue( nPrior );
	}

	if( IsIgnoreDuplicatePacket( cSubCmd ) ) {
		for( DWORD i=0; i<m_VecPacketQueueList[nPrior].size(); i++ ) {
			PacketQueueStruct *pStruct = &m_VecPacketQueueList[nPrior][i];
			if( pStruct->cSubCmd == cSubCmd ) {
				m_nPacketQueueSize[nPrior] -= sizeof(DWORD) + sizeof(WORD) + sizeof(BYTE) + pStruct->nSize;
				m_VecPacketQueueList[nPrior].erase( m_VecPacketQueueList[nPrior].begin() + i );
			}
		}
	}
	PacketQueueStruct Struct;
	Struct.cSubCmd = cSubCmd;
	Struct.nSize = nSize;
	memcpy( Struct.pBuffer, pBuffer, nSize );
	m_VecPacketQueueList[nPrior].push_back( Struct );
	m_nPacketQueueSize[nPrior] += sizeof(DWORD) + sizeof(WORD) + sizeof(BYTE) + nSize;
}

void MATransAction::FlushPacketQueue( int nPrior )
{
	if( nPrior == -1 ) {
		for( int i=0; i<3; i++ ) FlushPacketQueue(i);
		return;
	}

	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( !pActor ) return;

	DWORD dwUniqueID = pActor->GetUniqueID();

	if( m_nPacketQueueSize[nPrior] == 0 || m_VecPacketQueueList[nPrior].empty() ) return;
	if( m_VecPacketQueueList[nPrior].size() == 1 ) {
		PacketQueueStruct *pStruct = &m_VecPacketQueueList[nPrior][0];

		CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
		SendGameActorMsg( dwUniqueID, pStruct->cSubCmd, ( pStruct->nSize > 0 ) ? pStruct->pBuffer : NULL, pStruct->nSize, nPrior, pSession->GetSeqLevel() );

		//OutputDebug( "Send Actor Message : %d - size : %d - %d\n", pStruct->cSubCmd, pStruct->nSize, nPrior );
	}
	else {
		BYTE pBuffer[PACKET_QUEUE_SIZE];
		int nOffset = 0;
		for( DWORD i=0; i<m_VecPacketQueueList[nPrior].size(); i++ ) {
			PacketQueueStruct *pStruct = &m_VecPacketQueueList[nPrior][i];
			memcpy( pBuffer + nOffset, &dwUniqueID, sizeof(DWORD) ); nOffset += sizeof(DWORD); // 서버가 브로드해주기땜에 서버가 보낼때에 마춰서 그냥 넣어주자.
			memcpy( pBuffer + nOffset, &pStruct->cSubCmd, sizeof(BYTE) ); nOffset += sizeof(BYTE);
			memcpy( pBuffer + nOffset, &pStruct->nSize, sizeof(WORD) ); nOffset += sizeof(WORD);
			memcpy( pBuffer + nOffset, pStruct->pBuffer, pStruct->nSize ); nOffset += pStruct->nSize;
		}
		CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
		SendGameActorBundleMsg( dwUniqueID, (USHORT)m_VecPacketQueueList[nPrior].size(), pBuffer, m_nPacketQueueSize[nPrior], nPrior, pSession->GetSeqLevel() );

		//OutputDebug( "Send Actor Bundle Message : %d - size : %d\n", m_VecPacketQueueList[nPrior].size(), m_nPacketQueueSize[nPrior] );
	}

	m_nPacketQueueSize[nPrior] = 0;
	m_VecPacketQueueList[nPrior].clear();
	/*
	if( m_nPacketQueueCount[nPrior] == 0 ) return;
	SendGameActorBundleMsg( this, m_nPacketQueueCount[nPrior], m_pPacketQueueBuffer[nPrior], m_nPacketQueueOffset[nPrior], nPrior );

	m_nPacketQueueOffset[nPrior] = 0;
	m_nPacketQueueCount[nPrior] = 0;
	m_cVecSubCmdList[nPrior].clear();
	m_nVecOffsetList[nPrior].clear();
	m_nVecSizeList[nPrior].clear();
	*/
}

// 중복되는 패킷일경우 중복 체크 할것인지 여부
// True 면 중복체크 해서 지워주는거임.
bool MATransAction::IsIgnoreDuplicatePacket( BYTE cSubCmd )
{
	switch( cSubCmd ) {
		case eActor::CS_PROJECTILE:
			return false;
		case eActor::CS_USESKILL:
			return false;
	}
	return true;
}

#ifdef PACKET_DELAY
void MATransAction::InsertRecvPacketDelay( DWORD dwActorProtocol, BYTE *pPacket, int nSize )
{
	if( s_nRecvPacketDelayMax == 0 && s_nRecvPacketDelayMin == 0 ) {
		OnDispatchMessage( dwActorProtocol, pPacket );
		return;
	}

	RecvPacketDelayStruct Struct;
	Struct.Time = ((CDnActor*)this)->GetLocalTime();
	Struct.dwDelay = s_nRecvPacketDelayMin + _rand()%((s_nRecvPacketDelayMax+1) - s_nRecvPacketDelayMin);
	Struct.pPacket = new char[nSize];
	Struct.nSize = nSize;
	memcpy( Struct.pPacket, pPacket, nSize );
	Struct.dwActorProtocol = dwActorProtocol;

	m_VecRecvPacketDelayList.push_back( Struct );
}

void MATransAction::ProcessDelay( LOCAL_TIME LocalTime, float fDelta )
{
	for( DWORD i=0; i<m_VecSendPacketDelayList.size(); i++ ) {
		SendPacketDelayStruct *pStruct = &m_VecSendPacketDelayList[i];
		if( (int)( LocalTime - pStruct->Time ) > (int)pStruct->dwDelay ) {

			switch( pStruct->SessionType ) {
				case ClientSessionTypeEnum::Tcp:
					SendActorMsg( pStruct->dwUniqueID, pStruct->dwActorProtocol, (BYTE*)pStruct->pPacket, pStruct->nSize );
					break;
				case ClientSessionTypeEnum::Udp:
					{
						CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
						SendGameActorMsg( pStruct->dwUniqueID, (USHORT)pStruct->dwActorProtocol, (BYTE*)pStruct->pPacket, pStruct->nSize, _RELIABLE, pStruct->cSeqLevel);
					}
					break;
			}

			SAFE_DELETE( pStruct->pPacket );
			m_VecSendPacketDelayList.erase( m_VecSendPacketDelayList.begin() + i );
			i--;
			continue;
		}
	}

	for( DWORD i=0; i<m_VecRecvPacketDelayList.size(); i++ ) {
		RecvPacketDelayStruct *pStruct = &m_VecRecvPacketDelayList[i];
		if( (int)( LocalTime - pStruct->Time ) > (int)pStruct->dwDelay ) {
			OnDispatchMessage( pStruct->dwActorProtocol, (BYTE*)pStruct->pPacket );

			SAFE_DELETE( pStruct->pPacket );
			m_VecRecvPacketDelayList.erase( m_VecRecvPacketDelayList.begin() + i );

			i--;
			continue;
		}
	}
}
#endif // PACKET_DELAY
