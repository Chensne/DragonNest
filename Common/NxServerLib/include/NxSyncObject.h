#pragma once

//----------------------------------------------------------------------------

class NxSyncObject
{
public:
	NxSyncObject();
	~NxSyncObject();

	void		Lock();
	void		Unlock();

private:
	CRITICAL_SECTION	m_cs;
};

//----------------------------------------------------------------------------

template < class _T >
class NxGuard
{
public:
	NxGuard( _T& LockObj )
		: m_MonitorObj( &LockObj )
	{
		m_MonitorObj->Lock();
	}

	~NxGuard()
	{
		m_MonitorObj->Unlock();
	}

private:
	_T*		m_MonitorObj;
};