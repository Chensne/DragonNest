
#include "stdafx.h"
#include "multisync.h"

CSync::CSync()
{
	InitializeCriticalSection(&m_CS);
	m_bBusy = false;
}


CSync::~CSync()
{
	DeleteCriticalSection(&m_CS);
}


void CSync::Enter()
{
	EnterCriticalSection(&m_CS);
	_ASSERT(m_bBusy == false);
	m_bBusy = true;
}


void CSync::Leave()
{
	_ASSERT(m_bBusy == true);
	m_bBusy = false;
	LeaveCriticalSection(&m_CS);
}


void CSync::Reset()
{
	DeleteCriticalSection(&m_CS);
	InitializeCriticalSection(&m_CS);
	m_bBusy = false;
}


CMultiSync::CMultiSync(int count)
{
	m_MaxCount = count+1;
	m_pCS = NULL;	

	memset(_DebugInfo, 0, sizeof(_DebugInfo));
	memset(_Busy, 0, sizeof(_Busy));
	
	if (count <= sizeof(_DebugInfo)/sizeof(*_DebugInfo))
	{
		m_DebugInfo = _DebugInfo;
		m_bBusy = _Busy;
	}	else
	{
		m_DebugInfo = new TCHAR [m_MaxCount][64];
		m_bBusy = new bool [m_MaxCount];
	}
	for(int i=0; i<m_MaxCount; i++)
		m_bBusy[i] = false;
	Reset();
}


CMultiSync::~CMultiSync()
{
	for(int i=0; i<m_MaxCount; i++)
		DeleteCriticalSection(&m_pCS[i]);
	delete [] m_pCS;

	if (m_MaxCount > sizeof(_DebugInfo)/sizeof(*_DebugInfo))
	{
		delete [] m_DebugInfo;
		delete [] m_bBusy;
	}
}


void CMultiSync::Reset()
{
	if (m_pCS)
	{
		for(int i=0; i<m_MaxCount; i++)
			DeleteCriticalSection(&m_pCS[i]);
		delete [] m_pCS;
	}
	m_pCS = new CRITICAL_SECTION [m_MaxCount];
	m_iCnt = 0;
	int i;
	for(i=0; i<m_MaxCount; i++)
	{
		m_bBusy[i] = false;		
		InitializeCriticalSection(&m_pCS[i]);
	}
}


void CMultiSync::EnterExclusive()
{
	for(int i=0; i<m_MaxCount; i++)
		Enter(i);
}


void CMultiSync::LeaveExclusive()
{
	for(int i=0; i<m_MaxCount; i++)
		Leave(m_MaxCount-1-i);
}

void CMultiSync::EnterExclusive(const TCHAR *file, int line)
{
	for(int i=0; i<m_MaxCount; i++)
	{
		Enter(i);		
		_stprintf_s(m_DebugInfo[i], _T("%s:%d:%d"), _tcschr(file, '\\') ? _tcsrchr(file, '\\')+1 : file, line, timeGetTime());
	}
}


int CMultiSync::Enter(const TCHAR *file, int line)
{
	int idx = Enter();
	_stprintf_s(m_DebugInfo[idx], _T("%s:%d:%d"), _tcschr(file, '\\') ? _tcsrchr(file, '\\')+1 : file, line, timeGetTime());
	return idx;
}

int CMultiSync::Enter()
{
	int idx = (m_iCnt++%(m_MaxCount-1)) + 1;
	Enter(idx);
	return idx;
}


void CMultiSync::Leave(int handle)
{
	_ASSERT(m_bBusy[handle] == true);
	m_bBusy[handle] = false;

	m_DebugInfo[handle][0] = '\0';
	LeaveCriticalSection(&m_pCS[handle]);
}


void CMultiSync::Enter(int idx)
{
	EnterCriticalSection(&m_pCS[idx]);
	_ASSERT(m_bBusy[idx] == false);
	m_bBusy[idx] = true;
}