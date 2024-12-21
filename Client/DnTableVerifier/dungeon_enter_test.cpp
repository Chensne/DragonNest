#include "StdAfx.h"
#include "dungeon_enter_test.h"

DNTableFileFormat* dungeon_enter_test::s_pDungeonEnterTable= NULL;

#define DUNGEONENTER_T dungeon_enter_test::s_pDungeonEnterTable

dungeon_enter_test::dungeon_enter_test(void)
{

}

dungeon_enter_test::~dungeon_enter_test(void)
{

}


void dungeon_enter_test::SetUpTestCase( void )
{
	// 지정된 국가 우선으로 검색하고 없으면 기본 리소스 폴더에서 찾는다.
	s_pDungeonEnterTable = LoadExtFile( "StageEnterTable.dnt" );
}

void dungeon_enter_test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pDungeonEnterTable );
}


void dungeon_enter_test::SetUp( void )
{
}


void dungeon_enter_test::TearDown( void )
{

}

TEST_F( dungeon_enter_test, NEEDITEM_IS_NOT_ZERO_BUT_COUNT_IS_ZERO )
{
	TCHAR atcBuf[ 512 ];
	int iNumItemCount = DUNGEONENTER_T->GetItemCount();
	for( int iDungeon = 0; iDungeon < iNumItemCount; ++iDungeon )
	{
		int iDungeonID = DUNGEONENTER_T->GetItemID( iDungeon );

		// 현재 안쓰는 행인지 판단하려면 이 값말고는 없는 듯 하다.
		int iMinLevel = DUNGEONENTER_T->GetFieldFromLablePtr( iDungeonID, "_LvlMin" )->GetInteger();
		if( iMinLevel == -1 ) continue;

		// 필요아이템과 아이템수량을 얻어서,
		int iNeedItemID = DUNGEONENTER_T->GetFieldFromLablePtr( iDungeonID, "_NeedItemID" )->GetInteger();
		int iNeedItemCount = DUNGEONENTER_T->GetFieldFromLablePtr( iDungeonID, "_NeedItemCount" )->GetInteger();

		bool bOK = true;
		if( iNeedItemID > 0 && iNeedItemCount == 0 )
			bOK = false;

		{
			ZeroMemory( atcBuf, sizeof(atcBuf) );
			_stprintf_s( atcBuf, _T("{TableName:StageEnterTable, Index:%d, Note:필요아이템엔 값이 있는데 필요아이템수량이 0 으로 설정되어있습니다.}"), iDungeonID );
			SCOPED_TRACE( atcBuf );
			EXPECT_TRUE( bOK );
		}
	}
}