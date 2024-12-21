
#include "stdafx.h"
#include "MultiSync.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CMultiSync::CMultiSync(int count)
{
	m_MaxCount = count+1;
	m_pCS = NULL;	

	memset(_DebugInfo, 0, sizeof(_DebugInfo));
	if (count <= sizeof(_DebugInfo)/sizeof(*_DebugInfo))
		m_DebugInfo = _DebugInfo;
	else
		m_DebugInfo = new TCHAR [m_MaxCount][64];

	Reset();
}


CMultiSync::~CMultiSync()
{
	for(int i=0; i<m_MaxCount; i++)
		DeleteCriticalSection(&m_pCS[i]);
	delete [] m_pCS;

	if (m_MaxCount > sizeof(_DebugInfo)/sizeof(*_DebugInfo))
		delete [] m_DebugInfo;
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
		InitializeCriticalSection(&m_pCS[i]);
}


void CMultiSync::EnterExclusive()
{
	for(int i=0; i<m_MaxCount; i++)
		EnterCriticalSection(&m_pCS[i]);
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
		EnterCriticalSection(&m_pCS[i]);
		_stprintf(m_DebugInfo[i], _T("%s:%d:%d"), _tcschr(file, '\\') ? _tcsrchr(file, '\\')+1 : file, line, timeGetTime());
	}
}


int CMultiSync::Enter(const TCHAR *file, int line)
{
	int idx = Enter();
	_stprintf(m_DebugInfo[idx], _T("%s:%d:%d"), _tcschr(file, '\\') ? _tcsrchr(file, '\\')+1 : file, line, timeGetTime());
	return idx;
}

int CMultiSync::Enter()
{
	while(1)
	{
		for (int i = 0; i < m_MaxCount; i++)
			if (TryEnterCriticalSection(&m_pCS[i]) != NULL)
				return i;
	}
	_ASSERT(0);
	return -1;
}


void CMultiSync::Leave(int handle)
{
	m_DebugInfo[handle][0] = '\0';
	LeaveCriticalSection(&m_pCS[handle]);
}