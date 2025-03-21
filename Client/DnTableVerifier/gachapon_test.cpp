#include "StdAfx.h"
#include "gachapon_test.h"
#include "DnCommonDef.h"

DNTableFileFormat* gachapon_test::s_pItemTable = NULL;
DNTableFileFormat* gachapon_test::s_pGachaTable = NULL;
DNTableFileFormat* gachapon_test::s_pItemDropTable = NULL;
DNTableFileFormat* gachapon_test::s_pPartsTable = NULL;

#define ITEM_T gachapon_test::s_pItemTable
#define GACHA_T gachapon_test::s_pGachaTable
#define ITEMDROP_T gachapon_test::s_pItemDropTable
#define PARTS_T gachapon_test::s_pPartsTable

// 현재 가차폰 테이블에 정의되어있는 파츠 갯수
const int NUM_GACHA_PARTS = 6;


gachapon_test::gachapon_test(void)
{
}

gachapon_test::~gachapon_test(void)
{
}


void gachapon_test::SetUpTestCase( void )
{
	// 지정된 국가 우선으로 검색하고 없으면 기본 리소스 폴더에서 찾는다.
	// ItemTable_Quest, ItemTable_Event, ItemTable_Equipment, ItemTable_Cash 시리즈별로 모두 모아서 로드 됨.
	s_pItemTable = LoadExtFile( "ItemTable.dnt" );
	s_pGachaTable = LoadExtFile( "GachaJP.dnt", "_GachaNum" );
	s_pItemDropTable = LoadExtFile( "ItemDropTable.dnt" );
	// PartsTable_Event, PartsTable_Equipment, PartsTable_Cash 시리즈 전부 모아서 로드 됨.
	s_pPartsTable = LoadExtFile( "PartsTable.dnt" );
}


void gachapon_test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pItemTable );
	SAFE_DELETE( s_pGachaTable );
	SAFE_DELETE( s_pItemDropTable );
	SAFE_DELETE( s_pPartsTable );
}


void gachapon_test::SetUp( void )
{

}


void gachapon_test::TearDown( void )
{

}


TEST_F( gachapon_test, GACHAPON_RESULT_ITEM_MATCH_JOB )
{
	// 일본과 대만인 경우에만 수행.
	if( false == ( g_strNation == _T("JPN") || g_strNation == _T("TWN") ) )
		return;

	char acBuffer[ 256 ] = { 0 };

	// 가챠폰 테이블을 뒤져서 존재하는 샵 아이디를 다 받아온다.
	vector<int> vlShopIDs;
	set<int> setShopIDs;
	for( int i = 0; i < GACHA_T->GetItemCount(); ++i )
	{
		int iItemID = GACHA_T->GetItemID( i );
		vlShopIDs.push_back( GACHA_T->GetFieldFromLablePtr( iItemID, "_GachaNum" )->GetInteger() );
		setShopIDs.insert( vlShopIDs.back() );
	}

	TCHAR atcBuf[ 256 ] = { 0 };
	{	
		_stprintf_s( atcBuf, _T("{GachaShopID:%d, Note:가챠폰 샵 아이디는 직업별로 4개가 되어야 합니다.}"), vlShopIDs.front() );
		SCOPED_TRACE( atcBuf );
		EXPECT_TRUE( (vlShopIDs.size() % 4) == 0 );
	}
	
	set<int>::iterator iter = setShopIDs.begin();
	for( iter; iter != setShopIDs.end(); ++iter )
	{
		int iShopID = *iter;

		// 드랍 그룹 테이블을 디벼서 나올 수 있는 모든 아이템들을 모아둔다.
		vector<int> vlGachaTableIDs;
		GACHA_T->GetItemIDListFromField( "_GachaNum", iShopID, vlGachaTableIDs );

		// 같은 가챠폰 번호 안에서는 반드시 위에서 아래로 직업 클래스 아이디 순으로 데이터가 입력되어야 합니다!!
		int iNumTableIDs = (int)vlGachaTableIDs.size();
		for( int iTableID = 0; iTableID < iNumTableIDs; ++iTableID )
		{
			int iGachaTableID = vlGachaTableIDs.at( iTableID );
			int iNeedJobClassID = iTableID+1;
			for( int iPart = 0; iPart < NUM_GACHA_PARTS; ++iPart )
			{
				sprintf_s( acBuffer, "_LinkedDrop%d", iPart+1 );
				int iLinkedDropTableID = GACHA_T->GetFieldFromLablePtr( iGachaTableID, acBuffer )->GetInteger();
				if( 0 == iLinkedDropTableID || 1 == iLinkedDropTableID )		// 없는 테이블 레이블로 찾으면 1이 나옴...(??)
					continue;

				// TODO: 드랍 테이블 재귀값은 8단계 까지임.. 추후에 필요하면 처리해야 엄청난 루프에 빠지지 않음.
				vector<int> vlToCalcDropTableIDs;
				vlToCalcDropTableIDs.push_back( iLinkedDropTableID );

				for( int i = 0; i < (int)vlToCalcDropTableIDs.size(); ++i )
				{
					int iDropTableID = vlToCalcDropTableIDs.at( i );
					// 드랍테이블은 항목이 20개
					for( int iLabel = 0; iLabel <= 20; ++iLabel )
					{
						sprintf_s( acBuffer, "_IsGroup%d", iLabel );
						int iIsGroup = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuffer )->GetInteger();
						if( iIsGroup == 1 )
						{
							sprintf_s( acBuffer, "_Item%dIndex", iLabel );
							int iReferenceDropTableID = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuffer )->GetInteger();

							if( 0 == iReferenceDropTableID )
								continue;

							vlToCalcDropTableIDs.push_back( iReferenceDropTableID );
						}
						else
						{
							sprintf_s( acBuffer, "_Item%dIndex", iLabel );
							int iItemTableID = ITEMDROP_T->GetFieldFromLablePtr( iDropTableID, acBuffer )->GetInteger();

							if( 0 == iItemTableID )
								continue;

							// 실제 필요직업이 뭔지 아이템 테이블에서 갖고 온다.
							// 0인 경우는 모두 사용 가능한 것이기에 패스하고 현재 셋팅되고 있는 직업과 실제 아이템의 직업이 맞는가 확인한다.
							int iItemNeedJobClassID = 0;
							string strNeedJobClassID = ITEM_T->GetFieldFromLablePtr( iItemTableID, "_NeedJobClass" )->GetString();
							{
								SecureZeroMemory( atcBuf, sizeof(atcBuf) );
								_stprintf_s( atcBuf, _T("{ItemTableID:%d, Note:가챠폰 아이템은 직업코드가 하나만 있어야 합니다.}"), iItemTableID );
								SCOPED_TRACE( atcBuf );
								EXPECT_TRUE( string::npos == strNeedJobClassID.find_first_of( ';' ) || strNeedJobClassID == "0" );
							}

							iItemNeedJobClassID = atoi( strNeedJobClassID.c_str() );

							if( 0 != iItemNeedJobClassID )
							{
								SecureZeroMemory( atcBuf, sizeof(atcBuf) );
								_stprintf_s( atcBuf, _T("{GachaTableID:%d, GachaTableJobID:%d, ItemTableID:%d, ItemTableJobID:%d, Note:가챠폰 테이블의 직업 위치와 실제 아이템의 직업 번호가 맞지 않습니다.}"),
											 iGachaTableID, iNeedJobClassID, iItemTableID, iItemNeedJobClassID );
								SCOPED_TRACE( atcBuf );
								EXPECT_TRUE( iNeedJobClassID == iItemNeedJobClassID );
							}

							// 어떤 파츠인지 타입을 얻어옴. 현재 루프도는 파츠 타입과 맞아야 함.
							int iPartIndexInTable = PARTS_T->GetFieldFromLablePtr( iItemTableID, "_Parts" )->GetInteger();
							EXPECT_TRUE( iPartIndexInTable == iPart );
						}
					}
				}
			}
		}
	}
}