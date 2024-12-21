
#pragma once

#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include <tchar.h>

class	CSync
{
public :
	CSync();
	~CSync();

	void Enter();
	void Leave();

	void Reset(); // �� ����

private :
	CRITICAL_SECTION m_CS;
	bool m_bBusy;
} ;

class	CMultiSync
{
public :
	CMultiSync(int count = 3);
	~CMultiSync();

	void EnterExclusive();
	void LeaveExclusive();

	void Reset();

	void EnterExclusive(const TCHAR *file, int line);
	int Enter(const TCHAR *file, int line);

	int Enter();
	void Leave(int handle);

private :
	void Enter(int idx);

	CRITICAL_SECTION * m_pCS;
	int m_MaxCount;
	unsigned int m_iCnt;

	TCHAR (*m_DebugInfo)[64];
	bool * m_bBusy;

	TCHAR _DebugInfo[8][64];
	bool _Busy[8];
} ;

#define _MCS_ENTER(CS)	(CS).Enter(_T(__FILE__), __LINE__)
#define _MCS_ENTEREXCLUSIVE(CS)	(CS).EnterExclusive(_T(__FILE__), __LINE__)
