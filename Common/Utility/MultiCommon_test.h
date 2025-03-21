
#pragma once
#ifdef _MUTATE

#define MAX_SESSION_COUNT 4000

extern int s_nThreadCount;
extern int **s_nThreadSessionMatchingTable;
extern std::vector<DWORD> s_dwVecThreadMatchingTable;
extern int *s_nCurrentSessionIndex;

void InitializeMultiThread( int nThreadCount );
void SetMultiThreadHandle( int nThreadIndex, HANDLE hThreadHandle );
void SetMultiThreadHandle( int nThreadIndex, DWORD dwProcessID );
DWORD GetMultiThreadHandle( int nThreadIndex );
void FinalizeMultiThread();
void BeginSession( DWORD dwProcessID, int nSessionIndex );
void EndSession( DWORD dwProcessID, int nSessionIndex );	
int GetThreadIndex();
int GetThreadIndex( DWORD dwProcessID );
inline int GetCurrentSessionIndex() { return s_nCurrentSessionIndex[GetThreadIndex()]; }

#define STATIC_DECL( type ) \
	static type[MAX_SESSION_COUNT]

#define STATIC_INSTANCE( type ) \
	type[ s_nThreadSessionMatchingTable[GetThreadIndex()][GetCurrentSessionIndex()] ]

#define STATIC_INSTANCE_N( type, tidx, sidx) \
	type[ s_nThreadSessionMatchingTable[tidx][sidx] ]

#define STATIC_DECL_INIT( cls, type, val ) \
	type cls::##val[MAX_SESSION_COUNT]
#endif