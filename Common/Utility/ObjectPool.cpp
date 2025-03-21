#include "stdafx.h"
#include "ObjectPool.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


SyncObject::SyncObject()
{
	::InitializeCriticalSection( &m_cs );
}

SyncObject::~SyncObject()
{
	::DeleteCriticalSection( &m_cs );
}

void
SyncObject::Lock()
{
	::EnterCriticalSection( &m_cs );
}

void
SyncObject::Unlock()
{
	::LeaveCriticalSection( &m_cs );
}