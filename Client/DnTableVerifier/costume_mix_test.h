#pragma once

class costume_mix_test : public ::testing::Test
{
public:
	static DNTableFileFormat* s_pCostumeMixTable;
	static DNTableFileFormat* s_pItemDropTable;
	static DNTableFileFormat* s_pCostumeMixAbilityTable;
	static DNTableFileFormat* s_pPotentialTable;

public:
	costume_mix_test(void);
	virtual ~costume_mix_test(void);

	// from gtest /////////////////////////////////////////////////
	// 매 테스트 마다 초기화/해제
	virtual void SetUp( void );
	virtual void TearDown( void );
	//

	// 테스트 케이스 전체에 해당하는 초기화/해제
	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
	////////////////////////////////////////////////////////////////

	static DNTableFileFormat& GetCostumeMixTable() { return *s_pCostumeMixTable; }
	static DNTableFileFormat& GetItemDropTable() { return *s_pItemDropTable; }
	static DNTableFileFormat& GetCostumeMixAbilityTable() { return *s_pCostumeMixAbilityTable; }
	static DNTableFileFormat& GetPotentialTable() { return *s_pPotentialTable; }
};
