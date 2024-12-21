#pragma once

#include "DnCostumeRandomMixDataMgr.h"

class costume_random_mix_test : public ::testing::Test
{
public:
	costume_random_mix_test(void);
	virtual ~costume_random_mix_test(void);

	// from gtest /////////////////////////////////////////////////
	// 매 테스트 마다 초기화/해제
	virtual void SetUp( void );
	virtual void TearDown( void );
	//

	// 테스트 케이스 전체에 해당하는 초기화/해제
	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
	////////////////////////////////////////////////////////////////

	CDnCostumeRandomMixDataMgr m_CosRandMixMgr;
};
