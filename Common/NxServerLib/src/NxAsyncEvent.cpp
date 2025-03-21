#include "stdafx.h"

#include "NxAsyncEvent.h"

NxMemPool<NxAsyncEvent, ASYNC_EVENT_POOL_CNT> NxAsyncEventPool::ms_Instance;

void 
NxAsyncEventPool::Create(int nReserveSize)
{
	ms_Instance.Reserve(nReserveSize);
}

void
NxAsyncEventPool::Destroy()
{
	ms_Instance.Destroy();
}
NxAsyncEvent*
NxAsyncEventPool::Alloc()
{
	return ms_Instance.Alloc();
}

void
NxAsyncEventPool::Free(NxAsyncEvent* pEvent)
{
	ms_Instance.Free(pEvent);
}