#include "stdafx.h"
#include "NxSyncObject.h"


NxSyncObject::NxSyncObject()
{
	::InitializeCriticalSection( &m_cs );
}

NxSyncObject::~NxSyncObject()
{
	::DeleteCriticalSection( &m_cs );
}

void
NxSyncObject::Lock()
{
	::EnterCriticalSection( &m_cs );
}

void
NxSyncObject::Unlock()
{
	::LeaveCriticalSection( &m_cs );
}