#pragma once


// �������� ������, ������ ��� ���̺� �ε����� �����ϴ� �͵鿡 ���� ����
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
	// �� �׽�Ʈ ���� �ʱ�ȭ/����
	virtual void SetUp( void );
	virtual void TearDown( void );
	//

	// �׽�Ʈ ���̽� ��ü�� �ش��ϴ� �ʱ�ȭ/����
	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
	////////////////////////////////////////////////////////////////
};
