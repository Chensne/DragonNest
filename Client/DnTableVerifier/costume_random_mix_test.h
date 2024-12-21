#pragma once

#include "DnCostumeRandomMixDataMgr.h"

class costume_random_mix_test : public ::testing::Test
{
public:
	costume_random_mix_test(void);
	virtual ~costume_random_mix_test(void);

	// from gtest /////////////////////////////////////////////////
	// �� �׽�Ʈ ���� �ʱ�ȭ/����
	virtual void SetUp( void );
	virtual void TearDown( void );
	//

	// �׽�Ʈ ���̽� ��ü�� �ش��ϴ� �ʱ�ȭ/����
	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
	////////////////////////////////////////////////////////////////

	CDnCostumeRandomMixDataMgr m_CosRandMixMgr;
};
