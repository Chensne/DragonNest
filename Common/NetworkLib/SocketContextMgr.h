
#ifdef _USE_ACCEPTEX

#pragma once

#include "Singleton.h"
#include "Thread.h"

class CSocketContextMgr : public CSingleton< CSocketContextMgr >, public Thread
{
public:

	CSocketContextMgr( CIocpManager* pIOCP );
	~CSocketContextMgr();
	
	static CSocketContextMgr* CreateInstance( CIocpManager* pIOCP )
	{
		return new CSocketContextMgr( pIOCP );
	}

	bool				bInitialize();
	CSocketContext*		pGetSocketContext();
	void				OnDisconnect( CSocketContext* pSocketContext );

	// Thread Proc
	void				Run();

private:

	CIocpManager*					m_pIOCP;

	std::vector<CSocketContext*>	m_vSocketContext;
	std::list<CSocketContext*>		m_listCloseSocketContext;
	CSyncLock						m_Lock;
};

#endif // #ifdef _USE_ACCEPTEX
