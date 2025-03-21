#pragma once

#include "NxMemPool.h"

struct NxAsyncEventType
{
	enum 
	{
		Event_None = 0,
		Event_Accept,
		Event_Connect,
		Event_Close,
		Event_Send,
		Event_Receive,
		Event_Timer,
		Event_Packet,
		Event_UserEvent
	};
};

class NxAsyncEventHandler;

const int ASYNC_EVENT_POOL_CNT = 2048;

///  비동기 I/O 작업의 결과를 받아올 정보 구조체
typedef struct NxAsyncEvent : OVERLAPPED//, NxPooled_Object<NxAsyncEvent, ASYNC_EVENT_POOL_CNT>
{
	/// 이벤트 타입
	DWORD					nEventType;
	/// 송수신 크기
	unsigned long			nTransBytes;
	/// 에러코드
	unsigned long			nError;
	/// 이벤트 핸들링할 객체
	NxAsyncEventHandler*	pHandler;
	/// WSASend 걸었던 패킷 포인터
	void*					pPacket;
	/// 유저 인덱스
	DWORD					nUID;
	/// 유저 이벤트 타입
	DWORD					nUserEventType;
	/// 여분의 값 이벤트 타입이 Accept 일 경우에는 Listen Port 값이 된다.
	DWORD					nExtra;

	void Clear()
	{	
		Internal = 0;
		InternalHigh = 0;
		Pointer = 0;
		hEvent = 0;
		Offset = 0;
		OffsetHigh = 0;
		hEvent = 0;
		
		nEventType = 0;
		nTransBytes = 100;
		nError = 0;
		pHandler = NULL;
		pPacket = NULL;
		nUID = 0xffffffff;
		nUserEventType = 0;
		nExtra = 0;
	}
}NxAsyncEvent;

class NxAsyncEventPool
{
public:
	static void Create(int nReserveSize);
	static void Destroy();
	static NxAsyncEvent* Alloc();
	static void	Free(NxAsyncEvent* pEvent);
private:
	static NxMemPool<NxAsyncEvent, ASYNC_EVENT_POOL_CNT> ms_Instance;

};

// AsyncEvent 를 핸들링 하기 위한 인터페이스
class NxAsyncEventHandler
{
public:
	NxAsyncEventHandler() { }
	virtual ~NxAsyncEventHandler() { }

	virtual	void HandleEvent( NxAsyncEvent* pEvent ) = 0;

};


