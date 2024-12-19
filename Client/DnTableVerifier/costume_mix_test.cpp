#include "StdAfx.h"
#include "costume_mix_test.h"
#include "DnCommonDef.h"

DNTableFileFormat* costume_mix_test::s_pCostumeMixTable = NULL;
DNTableFileFormat* costume_mix_test::s_pItemDropTable = NULL;
DNTableFileFormat* costume_mix_test::s_pCostumeMixAbilityTable = NULL;
DNTableFileFormat* costume_mix_test::s_pPotentialTable = NULL;

costume_mix_test::costume_mix_test(void)
{
}

costume_mix_test::~costume_mix_test(void)
{
}

void costume_mix_test::SetUpTestCase( void )
{
	s_pItemDropTable = LoadExtFile( "ItemDropTable.dnt" );
	s_pCostumeMixTable = LoadExtFile( "cashitemcompoundtable.dnt" );
	s_pCostumeMixAbilityTable = LoadExtFile( "cashcompoundinfotable.dnt" );
	s_pPotentialTable = LoadExtFile( "PotentialTable.dnt" );
}

void costume_mix_test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pItemDropTable );
	SAFE_DELETE( s_pCostumeMixTable );
	SAFE_DELETE( s_pCostumeMixAbilityTable );
	SAFE_DELETE( s_pPotentialTable );
}


void costume_mix_test::SetUp( void )
{

}


void costume_mix_test::TearDown( void )
{

}


TEST_F( costume_mix_test, COSTUME_MIX_DATA_TEST )
{
	if( false == ( g_strNation == _T("JPN") || g_strNation == _T("TWN") || g_strNation == _T("MAIN") ) )
		return;
}