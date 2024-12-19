#pragma once
#include <assert.h>

extern void (*s_passert)( const wchar_t*, const wchar_t*, unsigned );
void SetAssertFuncPtr( void (*Func)( const wchar_t*, const wchar_t*, unsigned ) );
void EnableAssert( bool bEnable );

#ifdef ASSERT
#undef ASSERT
#endif

#if defined( _FINAL_BUILD )
#define ASSERT( x ) ((void)0)
#else
#if defined (ASSERT)
#undef ASSERT
#define ASSERT( x ) (void)( (!!(x)) || (s_passert(_CRT_WIDE(#x), _CRT_WIDE(__FILE__), __LINE__), 0) )
#else
#define ASSERT( x ) (void)( (!!(x)) || (s_passert(_CRT_WIDE(#x), _CRT_WIDE(__FILE__), __LINE__), 0) )
#endif
#endif // #if defined( _FINAL_BUILD )

#ifdef _ASSERT
#undef _ASSERT
#define _ASSERT ASSERT
#endif

#ifdef assert
#undef assert
#endif
#define assert ASSERT

#define AssertEqual( x, y )  ASSERT( (x) == (y) );
#define AssertNotEqual( x, y )  ASSERT( (x) != (y) );
#define AssertRange( n, x, y )  ASSERT( (n) >= (x) && (n) <= (y) );
