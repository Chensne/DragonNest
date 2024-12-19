
#include "stdafx.h"
#include <atlcoll.h>

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

const int _REPOSITORYCOUNT	= 1000000;
const int _TESTCOUNT		= 1000000;

class Atlmap_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
	}
	virtual void TearDown()
	{
	}

	static void SetUpTestCase()
	{
		for( UINT i=0 ; i<_REPOSITORYCOUNT ; ++i )
		{
			s_StlMap.insert( std::make_pair(i,i) );
			s_AtlMap.SetAt( i,i );
		}
	}

public:

	static std::map<UINT,UINT>	s_StlMap;
	static CAtlMap<UINT,UINT>	s_AtlMap;
};

std::map<UINT,UINT> Atlmap_unittest::s_StlMap;
CAtlMap<UINT,UINT>	Atlmap_unittest::s_AtlMap;

TEST_F( Atlmap_unittest, lookuptest )
{
	srand( timeGetTime() );

	{
		CPerformanceLog log( "stl");
		for( UINT i=0 ; i<_TESTCOUNT ; ++i )
		{
			Atlmap_unittest::s_StlMap.find( rand()%_REPOSITORYCOUNT );
		}
	}
	
	{
		CPerformanceLog log( "atl");
		for( UINT i=0 ; i<_TESTCOUNT ; ++i )
		{
			Atlmap_unittest::s_AtlMap.Lookup( rand()%_REPOSITORYCOUNT );
		}
	}
}

#endif // #if !defined( _FINAL_BUILD )
