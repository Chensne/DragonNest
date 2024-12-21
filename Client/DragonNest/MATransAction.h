#pragma once

#include "DnBaseTransAction.h"

class CClientSession;
class CMemoryStream;

#define PACKET_QUEUE_SIZE 220
class MATransAction : public CDnBaseTransAction {
public:
	MATransAction();
	virtual ~MATransAction();


protected:
#ifdef PACKET_DELAY
	struct SendPacketDelayStruct {
		LOCAL_TIME Time;
		DWORD dwDelay;
		DWORD dwActorProtocol;
		DWORD dwUniqueID;
		char *pPacket;
		BYTE cSeqLevel;
		int nSize;
		ClientSessionTypeEnum SessionType;
	};

	struct RecvPacketDelayStruct {
		LOCAL_TIME Time;
		DWORD dwDelay;
		DWORD dwActorProtocol;
		char *pPacket;
		int nSize;
	};
	std::vector<SendPacketDelayStruct> m_VecSendPacketDelayList;
	std::vector<RecvPacketDelayStruct> m_VecRecvPacketDelayList;
#endif //PACKET_DELAY

	/*
	BYTE m_pPacketQueueBuffer[3][PACKET_QUEUE_SIZE];
	int m_nPacketQueueOffset[3];
	int m_nPacketQueueCount[3];
	std::vector<BYTE> m_cVecSubCmdList[3];
	std::vector<int> m_nVecOffsetList[3];
	std::vector<int> m_nVecSizeList[3];
	*/

	struct PacketQueueStruct {
		BYTE cSubCmd;
		BYTE pBuffer[PACKET_QUEUE_SIZE];
		int nSize;
	};
	std::vector<PacketQueueStruct> m_VecPacketQueueList[3];
	int m_nPacketQueueSize[3];

protected:
	void AddPacketQueue( BYTE cSubCmd, BYTE *pBuffer, int nSize, int nPrior );

public:
	virtual void Send( DWORD dwActorProtocol, DnActorHandle hActor, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto );
	virtual void Send( DWORD dwActorProtocol, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto, bool bImmediate = true );
	virtual void OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket ) {}

	void FlushPacketQueue( int nPrior );

	static bool IsIgnoreDuplicatePacket( BYTE cSubCmd );

#ifdef PACKET_DELAY
	void InsertRecvPacketDelay( DWORD dwActorProtocol, BYTE *pPacket, int nSize );
	void ProcessDelay( LOCAL_TIME LocalTime, float fDelta );
#endif //PACKET_DELAY

};