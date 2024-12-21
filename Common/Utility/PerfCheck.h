#pragma once

#include "Singleton.h"
#include "SundriesFunc.h"

#define ENABLE_PROFILER
#define PERF_CHECK_FRAME 1000

extern int g_nPerfCounter;
extern bool g_bEnableProfile;
extern void (__stdcall *s_pProfileOutputDebugFunc)(const char*, ...);

void SetProfileOutputDebugFuncPtr( void (__stdcall *Func)(const char*, ...) );

void ProfilePrint(void (__stdcall *Func)(const char*, ...) = OutputDebugFunc);
void ProfileMemPrint(void (__stdcall *Func)(const char*, ...) = OutputDebugFunc);

#define PROFILE_PRINT()		ProfilePrint()


#ifdef ENABLE_PROFILER

#define PROFILE_TIME_TEST_BLOCK_START( string )	\
{								\
	static double fIncreaseCount = 0;	\
	static bool bOutput = false;	\
	static int nCallCount = 0;		\
	static char *szFunctionString;	\
	static int nLastUpdateTime = 0;	\
	szFunctionString = string;		\
	LARGE_INTEGER liFrequency;		\
	LARGE_INTEGER liStartTime;	\
	LARGE_INTEGER liCurTime;	\
	static double fMinProfileTime = FLT_MAX; \
	static double fMaxProfileTime = FLT_MIN; \
	if( g_bEnableProfile ) { \
		QueryPerformanceFrequency(&liFrequency);	\
		QueryPerformanceCounter(&liStartTime); \
	}


#define PROFILE_TIME_TEST_BLOCK_END()	\
	if( g_bEnableProfile ) { \
		nCallCount++;	\
		QueryPerformanceCounter(&liCurTime);	\
		if( ( g_nPerfCounter - nLastUpdateTime ) / PERF_CHECK_FRAME > 0 ) {	\
			char szStr[1024];	\
			sprintf_s( szStr, "Call Count : %d / %s Process Time Per Second (ms): %.2f, ( Tick : %.2f - Min(%.2f), Max(%.2f) )\n", nCallCount, szFunctionString, fIncreaseCount, fIncreaseCount / nCallCount, fMinProfileTime / 100.f, fMaxProfileTime / 100.f );	\
			s_pProfileOutputDebugFunc( szStr ); \
			fIncreaseCount = 0.f;	\
			nCallCount = 0;			\
			fMinProfileTime = FLT_MAX; \
			fMaxProfileTime = FLT_MIN; \
			bOutput = true;		\
			nLastUpdateTime = g_nPerfCounter - ( g_nPerfCounter % PERF_CHECK_FRAME );	\
		}	\
		else {	\
			double dwTime = (double)(( liCurTime.QuadPart - liStartTime.QuadPart ) / ( liFrequency.QuadPart / 100000.f ));	\
			if( dwTime < fMinProfileTime ) fMinProfileTime = dwTime; \
			if( dwTime > fMaxProfileTime ) fMaxProfileTime = dwTime; \
			fIncreaseCount += dwTime / 100.f;	\
			bOutput = false; \
		}	\
	}	\
}


#define PROFILE_TIME_TEST( func )	\
	PROFILE_TIME_TEST_BLOCK_START( #func ) \
	func;		\
	PROFILE_TIME_TEST_BLOCK_END()

#define PROFILE_TIME_TEST_RETURN_INT( func )	\
	PROFILE_TIME_TEST_BLOCK_START( #func ) \
	int Result = func;		\
	PROFILE_TIME_TEST_BLOCK_END() \
	return Result;

#define PROFILE_TIME_TEST_RETURN_BOOL( func )	\
	PROFILE_TIME_TEST_BLOCK_START( #func ) \
	bool Result = func;		\
	PROFILE_TIME_TEST_BLOCK_END() \
	return Result;

#define PROFILE_TICK_TEST_BLOCK_START( string )	\
{								\
	static char *szFunctionString;	\
	szFunctionString = string;		\
	LARGE_INTEGER liFrequency;		\
	LARGE_INTEGER liStartTime;	\
	LARGE_INTEGER liCurTime;	\
	if( g_bEnableProfile ) { \
		QueryPerformanceFrequency(&liFrequency);	\
		QueryPerformanceCounter(&liStartTime); \
	}


#define PROFILE_TICK_TEST_BLOCK_END()	\
	if( g_bEnableProfile ) { \
		QueryPerformanceCounter(&liCurTime);	\
		double dwTime = (double)(( liCurTime.QuadPart - liStartTime.QuadPart ) / ( liFrequency.QuadPart / 100000.f ));	\
		char szProfileTestStr[256];	\
		sprintf_s( szProfileTestStr, "Profiling Code Block / %s (ms): %.2f\n", szFunctionString, dwTime / 100.f );	\
		s_pProfileOutputDebugFunc( szProfileTestStr );	\
	}	\
}

#define PROFILE_TICK_TEST_BLOCK_END_OV( OverValue )	\
	if( g_bEnableProfile ) { \
	QueryPerformanceCounter(&liCurTime);	\
	double dwTime = (double)(( liCurTime.QuadPart - liStartTime.QuadPart ) / ( liFrequency.QuadPart / 100000.f ));	\
	if( dwTime / 100.f > OverValue ) { \
		char szProfileTestStr[256];	\
		sprintf_s( szProfileTestStr, "Profiling Code Block / %s (ms): %.2f\n", szFunctionString, dwTime / 100.f );	\
		s_pProfileOutputDebugFunc( szProfileTestStr );	\
	}	\
	}	\
}


#define PROFILE_TICK_TEST( func )	\
	PROFILE_TICK_TEST_BLOCK_START( #func ) \
	func; \
	PROFILE_TICK_TEST_BLOCK_END()

#define PROFILE_TICK_TEST_OV( func, OverValue )	\
	PROFILE_TICK_TEST_BLOCK_START( #func ) \
	func; \
	PROFILE_TICK_TEST_BLOCK_END_OV( OverValue ) \

#define PROFILE_TICK_TEST_RETURN_BOOL( func )	\
	PROFILE_TICK_TEST_BLOCK_START( #func ) \
	bool bResult = func; \
	PROFILE_TICK_TEST_BLOCK_END() \
	return bResult;

#else

#define PROFILE_TIME_TEST( func )	\
{								\
	func;		\
}
#define PROFILE_TIME_TEST_RETURN_INT( func ) return func;
#define PROFILE_TIME_TEST_RETURN_BOOL( func ) return func;

#define PROFILE_TIME_TEST_BLOCK_START( string ) {}
#define PROFILE_TIME_TEST_BLOCK_END() {}
#define PROFILE_TICK_TEST_BLOCK_START( string ) {}
#define PROFILE_TICK_TEST_BLOCK_END() {}
#define PROFILE_TICK_TEST_BLOCK_END_OV( OverValue ) {}
#define PROFILE_TICK_TEST( func ) \
#define PROFILE_TICK_TEST_OV( func, OverValue ) \
{	\
	func;	\
}
#define PROFILE_TICK_TEST_RETURN_BOOL( func ) return func;

#define PROFILE_MEM_START( name ) {}
#define PROFILE_MEM_END( name ) {}

#endif // ENABLE_PROFILER

class CPerfCheck;
class CProfileMemChecker;

extern CPerfCheck	s_PerfCheck;
extern CProfileMemChecker s_ProfileMemCheck;

#ifdef _TEST_CODE_KAL
class CProfileMemChecker2;
extern CProfileMemChecker2 g_ProfileMemCheck;
#endif

//void InitProfile();