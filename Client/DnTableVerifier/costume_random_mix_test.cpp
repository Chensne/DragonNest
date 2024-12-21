#include "StdAfx.h"
#include "costume_random_mix_test.h"
#include "DnCommonDef.h"

costume_random_mix_test::costume_random_mix_test(void)
{
}

costume_random_mix_test::~costume_random_mix_test(void)
{
}

void costume_random_mix_test::SetUpTestCase( void )
{
}

void costume_random_mix_test::TearDownTestCase( void )
{
}


void costume_random_mix_test::SetUp( void )
{
}


void costume_random_mix_test::TearDown( void )
{

}


TEST_F( costume_random_mix_test, COSTUME_RANDOM_MIX_DATA_TEST )
{
	if( false == ( g_strNation == _T("KOR") || g_strNation == _T("MAIN") ) )
		return;

	EXPECT_TRUE(m_CosRandMixMgr.LoadData());
}