#include "Lock.h"
using namespace CleverLock;
/////////////////////////////////////////////////////////////////////////
//	CWithLock
/////////////////////////////////////////////////////////////////////////
CWithLock::CWithLock()
{
	InitializeCriticalSection( &m_csAccess );
}

CWithLock::~CWithLock()
{
	DeleteCriticalSection( &m_csAccess );
}

/////////////////////////////////////////////////////////////////////////
void CWithLock::Lock()
{
	EnterCriticalSection(&m_csAccess);
}

void CWithLock::UnLock()
{
	LeaveCriticalSection(&m_csAccess);
}

BOOL CWithLock::TryLock()
{
	return TryEnterCriticalSection(&m_csAccess);
}

HANDLE	CWithLock::GetOwnerThread()
{
	return m_csAccess.OwningThread; 
}
/////////////////////////////////////////////////////////////////////////
//	CLock
/////////////////////////////////////////////////////////////////////////
CLock::CLock( CWithLock &door, bool b_lock)
{
	m_pDoor = &door;
	m_bLocked = b_lock;
	if (m_bLocked)
		Lock(*m_pDoor);
}

CLock::~CLock()
{
	if (m_bLocked)
		UnLock(*m_pDoor);
}

/////////////////////////////////////////////////////////////////////////
void CLock::Lock()
{
	if(!m_bLocked)
	{
		m_bLocked = true;
		Lock(*m_pDoor);
	}
}

void CLock::UnLock()
{
	if (m_bLocked)
	{
		m_bLocked = false;
		UnLock(*m_pDoor);
	}
}

BOOL CLock::TryLock()
{
	if(!m_bLocked)
	{
		if ( m_pDoor->TryLock() )
		{
			m_bLocked = true;
			return TRUE;
		}
		
		return FALSE;
	}

	return TRUE;	
}

void CLock::Lock(CWithLock &door)
{
	door.Lock();
}

void CLock::UnLock(CWithLock &door)
{
	door.UnLock();
}

/////////////////////////////////////////////////////////////////////////
//	CResourceMgr
/////////////////////////////////////////////////////////////////////////
//CResourceMgr::CResourceMgr()
//{
//}
//
//CResourceMgr::~CResourceMgr()
//{
//	LOCK(*this);
//	if (m_pInstance)
//		m_pInstance = NULL;
//}
//
///////////////////////////////////////////////////////////////////////////
//CResourceMgr * CResourceMgr::m_pInstance = NULL; 
//
//CResourceMgr * CResourceMgr::GetInstance()
//{
//	LOCK_CODE();
//	if (m_pInstance == NULL)
//		m_pInstance = new CResourceMgr;
//
//	return m_pInstance;
//
//}
//
//void CResourceMgr::AddResource(CWithManagedLock * p_resource)
//{
//	LOCK(*this);
//	m_aResourceArray.Add(p_resource);
//}
//
//void CResourceMgr::RemoveResource(CWithManagedLock * p_resource)
//{
//	LOCK(*this);
//	int i,n;
//	for (i = 0, n = m_aResourceArray.GetSize(); i<n; ++i)
//	{
//		if (p_resource == m_aResourceArray[i])
//		{
//			m_aResourceArray.RemoveAt(i);
//			return;
//		}
//	}
//}
//
//CWithManagedLock * CResourceMgr::CheckDeadLock()
//{
//	LOCK(*this);
//	CWithManagedLock * Ret = NULL;
//	HANDLE hThread;
//
//	int WholeSize = m_aResourceArray.GetSize();
//
//	if (WholeSize<=0)
//		return NULL;
//
//	int ri,rn,si, CurIndex,Thread_i,Thread_n;
//	CArray<int, int> Queue;
//
//	for (ri = 0, rn = m_aResourceArray.GetSize(); ri<rn; ++ri)
//		CLock::Lock(m_aResourceArray[ri]->m_WaitQueueLock);
//
//	char  *State = new char[WholeSize];
//	memset(State, 0, WholeSize);	
//
//	for (ri = 0; ri<WholeSize && Ret == NULL; ++ri)
//	{
//		if (State[ri]!=0)
//			continue;
//
//		Queue.RemoveAll();
//		Queue.Add(ri);
//		State[ri] = 1;
//
//		for (;Queue.GetSize()>0 && Ret == NULL; )
//		{
//			CurIndex = Queue[0];
//			Queue.RemoveAt(0);
//
//			for (Thread_i = 0, Thread_n = m_aResourceArray[CurIndex]->m_aWaitingThread.GetSize(); 
//				Thread_i<Thread_n && Ret == NULL;  
//				++ Thread_i)
//			{				
//				hThread = m_aResourceArray[CurIndex]->m_aWaitingThread[Thread_i];
//				
//				for (si = ri; si<WholeSize && Ret == NULL; ++si)
//				{
//					if (hThread == m_aResourceArray[si]->GetOwnerThread() )
//					{
//						switch (State[si])
//						{
//						case 0: // 未被访问
//							Queue.Add( si);
//							State[si] = 1;
//							break;
//						case 1: //被访问过,发现循环
//							if ( CurIndex != si)
//							{
//								Ret = m_aResourceArray[si];
//								State[si] = 3;
//							}
//							break;
//						case 2: //已被证实无循环
//							break;
//						case 3: //已被证实循环点
//							break;						
//						}
//
//						break;
//					} // m_aWaitingThread == m_aResourceArray[si]->GetOwnerThread()
//				}//si<WholeSize
//			}//Thread_i<Thread_n
//		}//Queue.GetSize()>0
//
//		for (si = ri; si< WholeSize; ++si)
//		{
//			if (State[si] == 1)
//				State[si] = 2;
//		}
//	}
//	
//	delete[] State;
//	for (ri = 0, rn = m_aResourceArray.GetSize(); ri<rn; ++ri)
//		CLock::UnLock(m_aResourceArray[ri]->m_WaitQueueLock);
//
//	return Ret;
//}
//
//	
///////////////////////////////////////////////////////////////////////////
////	CWithManagedLock
///////////////////////////////////////////////////////////////////////////
//CWithManagedLock::CWithManagedLock()
//{
//	CResourceMgr::GetInstance()->AddResource(this);
//}
//
//CWithManagedLock::~CWithManagedLock()
//{
//	CResourceMgr::GetInstance()->RemoveResource(this);
//}
//
///////////////////////////////////////////////////////////////////////////
//void CWithManagedLock::ForceFree()
//{
//	for (;m_csAccess.LockCount>0;)
//		LeaveCriticalSection(&m_csAccess);
//}
//
//HANDLE CWithManagedLock::GetWaitingThread(int index)
//{
//	LOCK(m_WaitQueueLock);
//	if (index>=0 && index < m_aWaitingThread.GetSize())
//		return m_aWaitingThread[index];
//
//	return (HANDLE)0;	
//}
//
//void CWithManagedLock::Lock()
//{
//	HANDLE hThread = GetCurrentThread();
//	if (hThread == GetOwnerThread())
//	{
//		CWithLock::Lock();
//		return;
//	}
//
//    LOCK2(m_WaitQueueLock,a);
//	m_aWaitingThread.Add( hThread );
//	UNLOCK2(a);
//
//	CWithLock::Lock();
//
//	int i,n;
//	LOCK2(m_WaitQueueLock,b);
//	for (i = 0, n = m_aWaitingThread.GetSize(); i<n; ++i)
//	{
//		if (hThread == m_aWaitingThread[i])
//		{
//			m_aWaitingThread.RemoveAt(i);
//			break;
//		}
//	}	
//
//	UNLOCK2(b);
//}
