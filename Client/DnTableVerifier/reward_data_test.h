#pragma once


// 보상으로 아이템, 아이템 드랍 테이블 인덱스를 참조하는 것들에 대한 검증
class reward_data_test : public ::testing::Test
{
public:
	static DNTableFileFormat* s_pMissionTable;
	static DNTableFileFormat* s_pDailyMissionTable;
	static DNTableFileFormat* s_pQuestRewardTable;
	static DNTableFileFormat* s_pStageClearTable;
	static DNTableFileFormat* s_pMonsterTable;

	static DNTableFileFormat *s_pItemTable;
	static DNTableFileFormat *s_pItemDropTable;
	static DNTableFileFormat *s_pItemGroupTable;

public:
	reward_data_test(void);
	virtual ~reward_data_test(void);

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
