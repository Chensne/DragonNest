#pragma once


// 원선씨 요청으로 제작.
// 이슈 #9619 관련.
class item_data_test : public ::testing::Test
{
public:
	static DNTableFileFormat* s_pItemTable;
	static DNTableFileFormat* s_pItemDropTable;
	static DNTableFileFormat* s_pItemDropGroupTable;
	static DNTableFileFormat* s_pCombinedShopTable;

public:
	item_data_test(void);
	virtual ~item_data_test(void);

	// from gtest /////////////////////////////////////////////////
	// 매 테스트 마다 초기화/해제
	virtual void SetUp( void );
	virtual void TearDown( void );
	//

	// 테스트 케이스 전체에 해당하는 초기화/해제
	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
	////////////////////////////////////////////////////////////////
};
