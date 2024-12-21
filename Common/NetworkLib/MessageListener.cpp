#include "stdafx.h"
#include "MessageListener.h"
#include "ClientSessionManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CTaskListener::CTaskListener(bool bIsUdp)
{
	m_bIsUdp = bIsUdp;
	CClientSessionManager::GetInstance().AddTaskListener( this );
}

CTaskListener::~CTaskListener()
{
	CClientSessionManager::GetInstance().RemoveTaskListener( this );
}