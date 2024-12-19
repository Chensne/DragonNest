
#include "stdafx.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

class NpcReputationClient_unittest:public testing::Test
{
protected:
	static void SetUpTestCase()
	{
	}

	static void TearDownTestCase()
	{
	}

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}
};

// 실패하는 TC
TEST_F( NpcReputationClient_unittest, FAIL_TEST )
{
}

// 초기화 TC
TEST_F( NpcReputationClient_unittest, INIT_TEST )
{
}

// 기본 함수 TC
TEST_F( NpcReputationClient_unittest, FUNCTION_TEST )
{
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#endif // #if !defined( _FINAL_BUILD )
