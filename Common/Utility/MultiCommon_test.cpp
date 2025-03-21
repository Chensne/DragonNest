
#include "stdafx.h"
#include "MultiCommon_test.h"
#ifdef _MUTATE
#include "SundriesFunc.h"
#include <process.h>

int s_nThreadCount = 1;
int **s_nThreadSessionMatchingTable = NULL;
std::vector<DWORD> s_dwVecThreadMatchingTable;
int *s_nCurrentSessionIndex = NULL;

DWORD s_dwCurrentThreadID = 0;

void InitializeMultiThread( int nThreadCount )
{
	if( s_nThreadSessionMatchingTable != NULL ) return;

	s_nThreadCount = nThreadCount;
	s_nThreadSessionMatchingTable = new int *[nThreadCount];
	int nThreadPerSessionCount = MAX_SESSION_COUNT / nThreadCount;

	for( int i=0; i<nThreadCount; i++ ) {
		s_nThreadSessionMatchingTable[i] = new int[nThreadPerSessionCount];

		for( int j=0; j<nThreadPerSessionCount; j++ ) {
			s_nThreadSessionMatchingTable[i][j] = ( i * nThreadPerSessionCount ) + j;
		}
	}

	s_dwVecThreadMatchingTable.resize(nThreadCount);

	s_nCurrentSessionIndex = new int[nThreadCount];
	memset( s_nCurrentSessionIndex, -1, sizeof(int) * nThreadCount );
}

void FinalizeMultiThread()
{
	if( s_nThreadSessionMatchingTable == NULL ) return;

	for( int i=0; i<s_nThreadCount; i++ ) {
		SAFE_DELETEA( s_nThreadSessionMatchingTable[i] );
	}
	SAFE_DELETEA( s_nThreadSessionMatchingTable );
	SAFE_DELETEA( s_nCurrentSessionIndex );

}

void BeginSession( DWORD dwProcessID, int nSessionIndex )
{
	if( s_nThreadSessionMatchingTable == NULL ) return;

	int nThreadIndex = GetThreadIndex( dwProcessID );
	if( s_nCurrentSessionIndex[nThreadIndex] != -1 ) {
		ASSERT( 0&&"Thread Dog Twist" );
		return;
	}

	s_dwCurrentThreadID = dwProcessID; // 이건 싱글로 돌릴때 임시로 넣어놓는 코드다. 아래 GetThreadIndex 주석두 풀어줘야한다.
	s_nCurrentSessionIndex[nThreadIndex] = nSessionIndex;
}

void EndSession( DWORD dwProcessID, int nSessionIndex )
{
	if( s_nThreadSessionMatchingTable == NULL ) return;

	int nThreadIndex = GetThreadIndex( dwProcessID );
	if( s_nCurrentSessionIndex[nThreadIndex] != nSessionIndex ) {
		ASSERT( 0&&"Thread Dog Twist" );
		return;
	}

	s_nCurrentSessionIndex[nThreadIndex] = -1;
}

void SetMultiThreadHandle( int nThreadIndex, HANDLE hThreadHandle )
{
//	s_dwVecThreadMatchingTable[nThreadIndex] = GetProcessIdOfThread( hThreadHandle );
}

void SetMultiThreadHandle( int nThreadIndex, DWORD dwProcessID )
{
	s_dwVecThreadMatchingTable[nThreadIndex] = dwProcessID;
}

DWORD GetMultiThreadHandle( int nThreadIndex )
{
	return s_dwVecThreadMatchingTable[nThreadIndex];
}

int GetThreadIndex()
{
	DWORD dwProcessID = s_dwCurrentThreadID;//GetCurrentThreadId();
	for( DWORD i=0; i<s_dwVecThreadMatchingTable.size(); i++ ) {
		if( s_dwVecThreadMatchingTable[i] == dwProcessID ) return (int)i;
	}
	return -1;
}

int GetThreadIndex( DWORD dwProcessID )
{
	for( DWORD i=0; i<s_dwVecThreadMatchingTable.size(); i++ ) {
		if( s_dwVecThreadMatchingTable[i] == dwProcessID ) return (int)i;
	}
	return -1;
}
#endif