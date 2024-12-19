#include "StdAfx.h"
#include "reward_data_test.h"

DNTableFileFormat* reward_data_test::s_pMissionTable = NULL;
DNTableFileFormat* reward_data_test::s_pDailyMissionTable = NULL;
DNTableFileFormat* reward_data_test::s_pQuestRewardTable = NULL;
DNTableFileFormat* reward_data_test::s_pStageClearTable = NULL;
DNTableFileFormat* reward_data_test::s_pMonsterTable = NULL;
DNTableFileFormat* reward_data_test::s_pItemTable = NULL;
DNTableFileFormat* reward_data_test::s_pItemDropTable = NULL;
DNTableFileFormat* reward_data_test::s_pItemGroupTable = NULL;

reward_data_test::reward_data_test(void)
{

}

reward_data_test::~reward_data_test(void)
{

}


void reward_data_test::SetUpTestCase( void )
{
	//// 지정된 국가 우선으로 검색하고 없으면 기본 리소스 폴더에서 찾는다.
	//vector<string> vlFilePathsToVerify;
	//string strTableFullPath;
	//
	//GatherConcernedExt( "MissionTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pMissionTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "DailyMissionTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pDailyMissionTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "QuestRewardTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pQuestRewardTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "StageClearTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pStageClearTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "MonsterTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pMonsterTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "ItemTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pItemTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "ItemDropTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pItemDropTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "ItemDropGroupTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pItemGroupTable = new DNTableFileFormat( strTableFullPath.c_str() );

	s_pMissionTable = LoadExtFile( "MissionTable.dnt" );
	s_pDailyMissionTable = LoadExtFile( "DailyMissionTable.dnt" );
	s_pQuestRewardTable = LoadExtFile( "QuestRewardTable.dnt" );
	s_pStageClearTable = LoadExtFile( "StageClearTable.dnt" );
	s_pMonsterTable = LoadExtFile( "MonsterTable.dnt" );
	s_pItemTable = LoadExtFile( "ItemTable.dnt" );
	s_pItemDropTable = LoadExtFile( "ItemDropTable.dnt" );
	s_pItemGroupTable = LoadExtFile( "ItemDropGroupTable.dnt", "_DropGroupID" );
}

void reward_data_test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pMissionTable );
	SAFE_DELETE( s_pDailyMissionTable );
	SAFE_DELETE( s_pQuestRewardTable );
	SAFE_DELETE( s_pStageClearTable );
	SAFE_DELETE( s_pMonsterTable );
	SAFE_DELETE( s_pItemTable );
	SAFE_DELETE( s_pItemDropTable );
	SAFE_DELETE( s_pItemGroupTable );
}


void reward_data_test::SetUp( void )
{
}


void reward_data_test::TearDown( void )
{

}

//TEST_F( reward_data_test, MISSION_REWARDITEMID_VALIDCHECK )
//{
//	TCHAR atcBuf[ 512 ] = { 0 };
//	TCHAR atcLabel[ 64 ] = { 0 };
//	char szLabel[ 64 ] = { 0 };
//	int nCount = s_pMissionTable->GetItemCount();
//	for( int i=0; i<nCount; i++ ) {
//		int nItemID = s_pMissionTable->GetItemID(i);
//		for( int k=0; k<3; k++ ) {
//			_stprintf_s( atcLabel, _T("_RewardItem%d"), k + 1 );
//			sprintf_s( szLabel, "_RewardItem%d", k + 1 );
//			int nItemTableID = s_pMissionTable->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
//			if( nItemTableID == 0 ) continue;
////			if( s_pItemTable->IsExistItem( nItemTableID ) ) continue;
//
//			SecureZeroMemory( atcBuf, sizeof(atcBuf) );
//			_stprintf_s( atcBuf, _T("{TableName:MissionTable, Index:%d, Label:%s Note:존재하지 않는 보상 ItemTableID(%d) 입니다.}"), nItemID, atcLabel, nItemTableID );
//			SCOPED_TRACE( atcBuf );
//			EXPECT_TRUE( s_pItemTable->IsExistItem( nItemTableID ) );
//		}
//	}
//}
//
//TEST_F( reward_data_test, DAILYMISSION_REWARDITEMID_VALIDCHECK )
//{
//	TCHAR atcBuf[ 512 ] = { 0 };
//	TCHAR atcLabel[ 64 ] = { 0 };
//	char szLabel[ 64 ] = { 0 };
//	int nCount = s_pDailyMissionTable->GetItemCount();
//	for( int i=0; i<nCount; i++ ) {
//		int nItemID = s_pDailyMissionTable->GetItemID(i);
//		for( int k=0; k<3; k++ ) {
//			_stprintf_s( atcLabel, _T("_RewardItem%d"), k + 1 );
//			sprintf_s( szLabel, "_RewardItem%d", k + 1 );
//			int nItemTableID = s_pDailyMissionTable->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
//			if( nItemTableID == 0 ) continue;
//
//			SecureZeroMemory( atcBuf, sizeof(atcBuf) );
//			_stprintf_s( atcBuf, _T("{TableName:DailyMissionTable, Index:%d, Label:%s, Note:존재하지 않는 보상 ItemTableID(%d) 입니다.}"), nItemID, atcLabel, nItemTableID );
//			SCOPED_TRACE( atcBuf );
//			EXPECT_TRUE( s_pItemTable->IsExistItem( nItemTableID ) );
//		}
//	}
//}


TEST_F( reward_data_test, STAGECLEAR_REWARDITEMID_VALIDCHECK )
{
	TCHAR atcBuf[ 512 ] = { 0 };
	TCHAR atcLabel[ 64 ] = { 0 };
	char szLabel[ 64 ] = { 0 };
	int nCount = s_pStageClearTable->GetItemCount();
	static char *szBoxStr[] = { "Bronze", "Silver", "Gold", "Platinum" };
	for( int i=0; i<nCount; i++ ) {
		int nItemID = s_pStageClearTable->GetItemID(i);
		for( int k=0; k<4; k++ ) {
			_stprintf_s( atcLabel, _T("_%sItemTableID"), szBoxStr[k] );
			sprintf_s( szLabel, "_%sItemTableID", szBoxStr[k] );
			int nItemDropTableID = s_pStageClearTable->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
			if( nItemDropTableID == 0 ) continue;

			SecureZeroMemory( atcBuf, sizeof(atcBuf) );
			_stprintf_s( atcBuf, _T("{TableName:StageClearTable, Index:%d, Label:%s, Note:존재하지 않는 보상 ItemDropTableID(%d) 입니다.}"), nItemID, atcLabel, nItemDropTableID );
			SCOPED_TRACE( atcBuf );
			EXPECT_TRUE( s_pItemDropTable->IsExistItem( nItemDropTableID ) );
		}
	}
}


TEST_F( reward_data_test, QUEST_REWARDITEMID_VALIDCHECK )
{
	TCHAR atcBuf[ 512 ] = { 0 };
	int nCount = s_pQuestRewardTable->GetItemCount();
	for( int i=0; i<nCount; i++ ) {
		TCHAR atcLabel[ 64 ] = { 0 };
		TCHAR atcLabel2[ 64 ] = { 0 };
		char szLabel[ 64 ] = { 0 };
		char szLabel2[ 64 ] = { 0 };

		int nItemID = s_pQuestRewardTable->GetItemID(i);
		for( int k=0; k<6; k++ ) {
			_stprintf_s( atcLabel, _T("_ItemIndex%d"), k + 1 );
			sprintf_s( szLabel, "_ItemIndex%d", k + 1 );
			int nItemTableID = s_pQuestRewardTable->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
			if( nItemTableID == 0 ) continue;
			
			_stprintf_s( atcLabel2, _T("_ItemCount%d"), k + 1 );
			sprintf_s( szLabel2, "_ItemCount%d", k + 1 );
			int nItemCount = s_pQuestRewardTable->GetFieldFromLablePtr( nItemID, szLabel2 )->GetInteger();

			SecureZeroMemory( atcBuf, sizeof(atcBuf) );
			_stprintf_s( atcBuf, _T("{TableName:QuestRewardTable, Index:%d, %s:%d, Note:존재하지 않는 Item보상 입니다.}"), nItemID, atcLabel, nItemTableID );
			SCOPED_TRACE( atcBuf );
			EXPECT_TRUE( s_pItemTable->IsExistItem( nItemTableID ) );

			SecureZeroMemory( atcBuf, sizeof(atcBuf) );
			_stprintf_s( atcBuf, _T("{TableName:QuestRewardTable, Index:%d, %s:%d, Note:보상 아이템 카운트가 0이면 안됨.}"), nItemID, atcLabel2, nItemTableID );
			SCOPED_TRACE( atcBuf );
			EXPECT_TRUE( ( nItemCount > 0 ) );
		}
	}
}

TEST_F( reward_data_test, MONSTER_REWARDITEMID_VALIDCHECK )
{
	vector<int> vlDropGroupTableID;

	TCHAR atcBuf[ 512 ] = { 0 };
	int nCount = s_pMonsterTable->GetItemCount();
	for( int i=0; i<nCount; i++ ) {
		int nItemID = s_pMonsterTable->GetItemID(i);
		int nItemDropGroupTableID = s_pMonsterTable->GetFieldFromLablePtr( nItemID, "_ItemDropGroupID" )->GetInteger();
		if( nItemDropGroupTableID <= 0 ) continue;

		SecureZeroMemory( atcBuf, sizeof(atcBuf) );
		_stprintf_s( atcBuf, _T("{TableName:MonsterTable, _ItemDropGroupID:%d, Note:존재하지 않는 드랍 그룹입니다.}"), nItemDropGroupTableID );
		SCOPED_TRACE( atcBuf );
		
		vlDropGroupTableID.clear();
		s_pItemGroupTable->GetItemIDListFromField( "_DropGroupID", nItemDropGroupTableID, vlDropGroupTableID );

		EXPECT_FALSE( vlDropGroupTableID.empty() );
	}
}
