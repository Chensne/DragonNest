
#include "StdAfx.h"
#include "Singleton.h"
#include "Thread.h"
#include "SocketContext.h"
#include "IocpManager.h"
#include "SocketContextMgr.h"
#include "Log.h"

#ifdef _USE_ACCEPTEX

CSocketContextMgr::CSocketContextMgr( CIocpManager* pIOCP )
: Thread( "SocketContextMgr" ), m_pIOCP( pIOCP )
{
}

CSocketContextMgr::~CSocketContextMgr()
{
	Thread::m_bRunning = false;
	Thread::WaitForTerminate();

	for each( CSocketContext* pContext in m_vSocketContext )
		delete pContext;
	m_vSocketContext.clear();
}

bool CSocketContextMgr::bInitialize()
{
	if( !Thread::Start() )
		return false;

	return true;
}

CSocketContext* CSocketContextMgr::pGetSocketContext()
{
	CSocketContext* pSocketContext = new (std::nothrow) CSocketContext();
	if( pSocketContext )
	{
		ScopeLock<CSyncLock> Lock( m_Lock );
		m_vSocketContext.push_back( pSocketContext );
	}

	return pSocketContext;
}

void CSocketContextMgr::OnDisconnect( CSocketContext* pSocketContext )
{
	ScopeLock<CSyncLock> Lock( m_Lock );

	pSocketContext->OnDisconnect();

	m_listCloseSocketContext.push_back( pSocketContext );
}

void CSocketContextMgr::Run()
{
	while( m_bRunning )
	{
		std::list<CSocketContext*> aList;
		{
			ScopeLock<CSyncLock> Lock( m_Lock );

			std::copy(m_listCloseSocketContext.begin(), m_listCloseSocketContext.end(), std::back_inserter(aList));
		}

		if (!aList.empty()) {
			std::list<CSocketContext*>::iterator aIt = aList.begin();
			while (aList.end() != aIt) {
				CSocketContext* pSocketContext = (*aIt);
				DN_ASSERT(NULL != pSocketContext,	"Invalid!");

				if((1) > ::timeGetTime() - pSocketContext->dwGetCloseTime()) {
					break;
				}

				if( m_pIOCP ) {
					bool bRetVal = m_pIOCP->bOnReUse(pSocketContext);
					if (!bRetVal) {
						aIt = aList.erase(aIt);
						continue;
					}
				}

				++aIt;
			}
		}

		if (!aList.empty()) {
			ScopeLock<CSyncLock> Lock( m_Lock );

			std::list<CSocketContext*>::iterator aIt1 = aList.begin();
			for (; aList.end() != aIt1 ; ++aIt1) {
				CSocketContext* pSocketContext = (*aIt1);
				DN_ASSERT(NULL != pSocketContext,	"Invalid!");

				std::list<CSocketContext*>::iterator aIt2 = std::find(m_listCloseSocketContext.begin(), m_listCloseSocketContext.end(), pSocketContext);
				if (aIt2 != m_listCloseSocketContext.end()) {
					m_listCloseSocketContext.erase(aIt2);
				}
			}
		}

		Sleep( 1000 );
	}
}

#endif // #ifdef _USE_ACCEPTEX
