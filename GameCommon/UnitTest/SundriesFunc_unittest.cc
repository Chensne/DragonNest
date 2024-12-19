
#include "stdafx.h"
#include "SundriesFunc.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

struct TestStruct : public DNEncryptPacketSeq, public TBoostMemoryPool<TestStruct>
{
};

class SundriesFunc_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
	}
	virtual void TearDown()
	{
	}
	
	void StackFunc();
	void HeapFunc();
	void MemPoolFunc();

	
};

TEST_F( SundriesFunc_unittest, GetParsePipe )
{
	std::string strString( "1,2,3,4,5,6,7,8,9,10" );

	for( UINT i=0 ; i<10 ; ++i )
	{
		std::string strValue	= _GetSubStrByCount( i, const_cast<char*>(strString.c_str()), ',' );
		std::string strValue2	= _GetSubStrByCountSafe( i, const_cast<char*>(strString.c_str()), ',' );

		EXPECT_TRUE( strValue == strValue2 );
	}

	for( UINT i=0 ; i<10 ; ++i )
	{
		std::string strValue	= _GetSubStrByCount( i, const_cast<char*>(strString.c_str()), ',' );

		int compare = (i == 9) ? 0 : i+1;
		std::string strValue2	= _GetSubStrByCountSafe( compare, const_cast<char*>(strString.c_str()), ',' );

		EXPECT_FALSE( strValue == strValue2 );
	}
}

void SundriesFunc_unittest::StackFunc()
{
	DNEncryptPacketSeq p;
	memset( &p, 0, sizeof(DNEncryptPacketSeq) );
}

void SundriesFunc_unittest::HeapFunc()
{
	DNEncryptPacketSeq* p = new DNEncryptPacketSeq;
	memset( p, 0, sizeof(TestStruct) );
	delete p;
}

void SundriesFunc_unittest::MemPoolFunc()
{
	TestStruct* p = new TestStruct;
	memset( p, 0, sizeof(TestStruct) );
	delete p;
}

TEST_F( SundriesFunc_unittest, MemPoolTest )
{
	int nCount = 100000;

	{
		CPerformanceLog log( "Stack" );
		for( INT i=0 ; i<nCount ; ++i )
			StackFunc();
	}

	{
		CPerformanceLog log( "Heap" );
		for( INT i=0 ; i<nCount ; ++i )
			HeapFunc();
	}

	{
		CPerformanceLog log( "MemPool" );
		for( INT i=0 ; i<nCount ; ++i )
			MemPoolFunc();
	}
}

#endif // #if !defined( _FINAL_BUILD )

