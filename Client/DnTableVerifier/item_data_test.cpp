#include "StdAfx.h"
#include "item_data_test.h"

DNTableFileFormat* item_data_test::s_pItemTable = NULL;
DNTableFileFormat* item_data_test::s_pItemDropTable = NULL;
DNTableFileFormat* item_data_test::s_pItemDropGroupTable = NULL;
DNTableFileFormat* item_data_test::s_pCombinedShopTable = NULL;

#define ITEM_T item_data_test::s_pItemTable
#define ITEMDROP_T item_data_test::s_pItemDropTable
#define ITEMDROPGROUP_T item_data_test::s_pItemDropGroupTable
#define COMBINEDSHOP_T item_data_test::s_pCombinedShopTable


item_data_test::item_data_test(void)
{

}

item_data_test::~item_data_test(void)
{

}


void item_data_test::SetUpTestCase( void )
{
	//// 지정된 국가 우선으로 검색하고 없으면 기본 리소스 폴더에서 찾는다.
	//vector<string> vlFilePathsToVerify;
	//string strTableFullPath;

	//// ItemTable_Quest, ItemTable_Event, ItemTable_Equipment, ItemTable_Cash 시리즈별로 모두 모아서 로드 됨.
	//GatherConcernedExt( "ItemTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pItemTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "ItemDropTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pItemDropTable = new DNTableFileFormat( strTableFullPath.c_str() );

	//GatherConcernedExt( "ItemDropGroupTable.ext", vlFilePathsToVerify, strTableFullPath );
	//s_pItemDropGroupTable = new DNTableFileFormat( strTableFullPath.c_str() );
	
	s_pItemTable = LoadExtFile( "ItemTable.dnt" );

	s_pItemDropTable = LoadExtFile( "ItemDropTable.dnt" );

	s_pItemDropGroupTable = LoadExtFile( "ItemDropGroupTable.dnt" );

	s_pCombinedShopTable = LoadExtFile( "combinedshoptable.dnt" );
}

void item_data_test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pItemTable );
	SAFE_DELETE( s_pItemDropTable );
	SAFE_DELETE( s_pItemDropGroupTable );
	SAFE_DELETE( s_pCombinedShopTable );
}


void item_data_test::SetUp( void )
{
}


void item_data_test::TearDown( void )
{

}

void ValidateItemDropGroupTableID( const char* pLabel, int iGroupTableID )
{
	//char acBuf[ MAX_PATH ] = { 0 };
	//TCHAR atcBuf[ 256 ] = { 0 };

	//for( int k = 1; k <= 10; ++k )
	//{
	//	sprintf_s( acBuf, pLabel, k );
	//	int iDropTableIDToFind = ITEMDROPGROUP_T->GetFieldFromLablePtr( iGroupTableID, acBuf )->GetInteger();

	//	if( 0 == iDropTableIDToFind )
	//		continue;

	//	_stprintf_s( atcBuf, _T("{TableName:ItemDropGroupTable, Index:%d, Note:존재하지 않는 DropTableID(%d) 입니다.}"), iGroupTableID, iDropTableIDToFind );
	//	SCOPED_TRACE( atcBuf );
	//	EXPECT_TRUE( ITEMDROP_T->IsExistItem( iDropTableIDToFind ) );
	//}
}

// 1. Easy1~10, Normal1~10, Hard1~0, Master1~10, Abyss1~10 총 50개 항목의 인덱스가 ItemDropTableID에 없으면 Error.
TEST_F( item_data_test, GROUPTABLE_REFERENCED_INVALID_DROP_TABLE_ID )
{
	// #38719, #31905 이슈로 드랍 그룹 테이블 구조 변경됨. 변경된 것 반영.
	TCHAR atcBuf[ 256 ] = { 0 };
	int iNumGroupTable = ITEMDROPGROUP_T->GetItemCount();
	for( int i = 0; i < iNumGroupTable; ++i )
	{
		int iGroupTableID = ITEMDROPGROUP_T->GetItemID( i );
		int iDropTableIDToFind = ITEMDROPGROUP_T->GetFieldFromLablePtr( iGroupTableID, "_DropID" )->GetInteger();

		_stprintf_s( atcBuf, _T("{TableName:ItemDropGroupTable, Index:%d, Note:존재하지 않는 DropTableID(%d) 입니다.}"), iGroupTableID, iDropTableIDToFind );
		SCOPED_TRACE( atcBuf );
		EXPECT_TRUE( ITEMDROP_T->IsExistItem( iDropTableIDToFind ) );
	}

	//int iNumGroupTable = ITEMDROPGROUP_T->GetItemCount();
	//for( int i = 0; i < iNumGroupTable; ++i )
	//{
	//	int iGroupTableID = ITEMDROPGROUP_T->GetItemID( i );

	//	ValidateItemDropGroupTableID( "Easy%d", iGroupTableID );
	//	ValidateItemDropGroupTableID( "Normal%d", iGroupTableID );
	//	ValidateItemDropGroupTableID( "Hard%d", iGroupTableID );
	//	ValidateItemDropGroupTableID( "Master%d", iGroupTableID );
	//	ValidateItemDropGroupTableID( "Abyss%d", iGroupTableID );
	//}
}

// 1. _Item1Prob ~ _Item20Prob의 총 합이 10억 (1,000,000,000) 이 넘는 경우 Error
TEST_F( item_data_test, DROP_TABLE_PROB_OVER )
{
	char acBuf[ 256 ] = { 0 };
	TCHAR atcBuf[ 256 ] = { 0 };
	int iNumDropTable = ITEMDROP_T->GetItemCount();
	for( int i = 0; i < iNumDropTable; ++i )
	{
		int iDropTableID = ITEMDROP_T->GetItemID( i );
		int iWholeProb = 0;
		for( int k = 1; k <= 20; ++k )
		{
			sprintf_s( acBuf, "_Item%dProb", k );
			int iProb = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuf )->GetInteger();
			iWholeProb += iProb;	
		}
	
		_stprintf_s( atcBuf, _T("{TableName:ItemDropTable, Index:%d, Note:확률 값의 합이 10억 보다 큽니다. 현재 합계- %d}"), iDropTableID, iWholeProb );
		SCOPED_TRACE( atcBuf );
		EXPECT_LE( iWholeProb, 1000000000 );
	}
}
//
//// 2. _IsGroup 의 값이 n 일때 _Item1Index ~ _Item20Index에 설정된 Index에 해당하는 아이템이 없는 경우 Error.
//TEST_F( item_data_test, INVALID_REFERENCE_ITEM_TABLE_ID )
//{
//	char acBuf[ 256 ] = { 0 };
//	TCHAR atcBuf[ 256 ] = { 0 };
//	int iNumDropTable = ITEMDROP_T->GetItemCount();
//	for( int i = 0; i < iNumDropTable; ++i )
//	{
//		int iDropTableID = ITEMDROP_T->GetItemID( i );
//		int iWholeProb = 0;
//		for( int k = 1; k <= 20; ++k )
//		{
//			sprintf_s( acBuf, "_IsGroup%d", k );
//			int iIsGroup = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuf )->GetInteger();
//			if( iIsGroup == 0 )
//			{
//				sprintf_s( acBuf, "_Item%dIndex", k );
//				int iItemTableID = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuf )->GetInteger();
//
//				if( 0 == iItemTableID )
//					continue;
//
//				_stprintf_s( atcBuf, _T("[DropTableID: %d] 존재하지 않는 아이템 ID 입니다. 참조 아이템ID: %d"), iDropTableID, iItemTableID );
//				SCOPED_TRACE( atcBuf );
//				EXPECT_TRUE( ITEM_T->IsExistItem( iItemTableID ) );
//			}
//		}
//	}
//}

void ValidateDropTableReferenceItemID( int iDropTableID/*, vector<int>& vlParentDropTableStack*/ )
{
	char acBuf[ 256 ] = { 0 };
	TCHAR atcBuf[ 256 ] = { 0 };
	for( int k = 1; k <= 20; ++k )
	{
		sprintf_s( acBuf, "_IsGroup%d", k );
		int iIsGroup = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuf )->GetInteger();
		if( iIsGroup == 1 )
		{
			sprintf_s( acBuf, "_Item%dIndex", k );
			int iReferenceDropTableID = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuf )->GetInteger();

			if( 0 == iReferenceDropTableID )
				continue;

			//vector<int>::iterator iter = find( vlParentDropTableStack.begin(), vlParentDropTableStack.end(), iReferenceDropTableID );
			//bool bDuplicatedSelfReference = (vlParentDropTableStack.end() != iter);
			//if( bDuplicatedSelfReference )
			//{
			//	continue;
			//}

			_stprintf_s( atcBuf, _T("{TableName:ItemDropTable, Index:%d, _Item%dIndex:%d, Note:존재하지 않는 DropTableID 입니다.}"), iDropTableID, k, iReferenceDropTableID );
			SCOPED_TRACE( atcBuf );
			bool bExistDropItemID = ITEMDROP_T->IsExistItem( iReferenceDropTableID );
			EXPECT_TRUE( bExistDropItemID );

			//if( bExistDropItemID )
			//{
				//vlParentDropTableStack.push_back( iReferenceDropTableID );
				//ValidateDropTableReferenceItemID( iReferenceDropTableID/*, vlParentDropTableStack*/ );
				//vlParentDropTableStack.pop_back();
			//}
		}
		else
		{
			sprintf_s( acBuf, "_Item%dIndex", k );
			int iItemTableID = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuf )->GetInteger();

			if( 0 == iItemTableID )
				continue;

			_stprintf_s( atcBuf, _T("{TableName:ItemDropTable, Index:%d, _Item%dIndex:%d, Note:존재하지 않는 ItemTableID 입니다.}"), iDropTableID, k, iItemTableID );
			SCOPED_TRACE( atcBuf );
			EXPECT_TRUE( ITEM_T->IsExistItem( iItemTableID ) );
		}
	}
}

// 3. _IsGroup 의 값이 y 일때 _Item1Index ~ _Item20Index에 설정된 Index에 해당하는 ItemDropTableID가 없는 경우 Error.TEST_F( item_data_test, INVALID_REFERENCE_ITEM_TABLE_ID )
TEST_F( item_data_test, INVALID_REFERENCE_DROP_GROUP_TABLE_ID )
{
	int iNumDropTable = ITEMDROP_T->GetItemCount();
	for( int i = 0; i < iNumDropTable; ++i )
	{
		int iDropTableID = ITEMDROP_T->GetItemID( i );
		
		// 드랍 테이블이 자신의 인덱스를 참조하므로 무한참조로 이어지는지 체크.
		//vector<int> vlDropTableIDStack;
		//vlDropTableIDStack.push_back( iDropTableID );
		ValidateDropTableReferenceItemID( iDropTableID/*, vlDropTableIDStack*/ );
		//_ASSERT( (int)vlDropTableIDStack.size() == 1 );
	}
}

// #45783 CombinedShop 테이블 검증 추가 요청
TEST_F( item_data_test, INVALID_ID_REFERENCE_COMBINED_SHOP_TABLE_ID )
{
	TCHAR atcBuf[ 256 ] = { 0 };

	int iNumCombinedShopTable = COMBINEDSHOP_T->GetItemCount();
	for( int i = 0; i < iNumCombinedShopTable; ++i )
	{
		int iCombinedShopTableID = COMBINEDSHOP_T->GetItemID( i );
		
		// 아이템 테이블에 존재하는지 체크
		int iItemTableID = COMBINEDSHOP_T->GetFieldFromLablePtr( iCombinedShopTableID, "_itemindex" )->GetInteger();
		_stprintf_s( atcBuf, _T("{TableName:CombinedShopTable, Index:%d, Note:존재하지 않는 ItemTableID(%d) 입니다.}"), iCombinedShopTableID, iItemTableID );
		SCOPED_TRACE( atcBuf );
		EXPECT_TRUE( ITEM_T->IsExistItem( iItemTableID ) );
	}
}

// #48623 상점 판매 가격이 더 비싼 관련 테이블 데이터 검증
TEST_F( item_data_test, SELLING_PRICE_ERROR )
{
	TCHAR atcBuf[ 256 ] = { 0 };

	int iNumItems = COMBINEDSHOP_T->GetItemCount();
	for( int i = 0; i < iNumItems; ++i )
	{
		int iItemID = COMBINEDSHOP_T->GetItemID( i );

		int aiPurchaseType[ 2 ] = { 0 };
		aiPurchaseType[ 0 ] = COMBINEDSHOP_T->GetFieldFromLablePtr( iItemID, "_PurchaseType1" )->GetInteger();
		aiPurchaseType[ 1 ] = COMBINEDSHOP_T->GetFieldFromLablePtr( iItemID, "_PurchaseType2" )->GetInteger();

		// 1 이 골드임. 골드만 검증하면 됨.
		for( int k = 0; k < 2; ++k )
		{
			int iItemTableID = COMBINEDSHOP_T->GetFieldFromLablePtr( iItemID, "_itemindex" )->GetInteger();
			if( 1 == aiPurchaseType[ k ] )
			{
				char acBuf[ 128 ] = { 0 };
				sprintf_s( acBuf, "_PurchaseItemValue%d", k+1 );
				int iBuyPrice = COMBINEDSHOP_T->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();
				int iSellPriceTwice = ITEM_T->GetFieldFromLablePtr( iItemTableID, "_SellAmount" )->GetInteger() * 2;

				_stprintf_s( atcBuf, _T("{TableName:CombinedShopTable, Index:%d, Note:상점 판매금액이 구입가보다 높습니다.}"), iItemTableID );
				SCOPED_TRACE( atcBuf );
				EXPECT_GE( iBuyPrice, iSellPriceTwice );
			}
			
		}
	}

	//// 앞으로 아이템 테이블 것은 안쓴다지만.. 쓰는 곳도 있다하니 일단 추가.
	//iNumItems = ITEM_T->GetItemCount();
	//for( int i = 0; i < iNumItems; ++i )
	//{
	//	int iItemID = ITEM_T->GetItemID( i );
	//	int iBuyPrice = ITEM_T->GetFieldFromLablePtr( iItemID, "_Amount" )->GetInteger();
	//	int iSellPriceTwice = ITEM_T->GetFieldFromLablePtr( iItemID, "_SellAmount" )->GetInteger();

	//	_stprintf_s( atcBuf, _T("{TableName:ItemTable, Index:%d, Note:상점 판매금액이 구입가보다 높습니다.}"), iItemID );
	//	SCOPED_TRACE( atcBuf );
	//	EXPECT_GT( iBuyPrice, iSellPriceTwice );
	//}
}