#pragma once

class gachapon_test : public ::testing::Test
{
public:
	static DNTableFileFormat* s_pItemTable;
	static DNTableFileFormat* s_pGachaTable;
	static DNTableFileFormat* s_pItemDropTable;
	static DNTableFileFormat* s_pPartsTable;

public:
	gachapon_test(void);
	virtual ~gachapon_test(void);

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
