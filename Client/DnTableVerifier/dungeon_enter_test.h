#pragma once

class dungeon_enter_test : public ::testing::Test
{
public:
	static DNTableFileFormat* s_pDungeonEnterTable;

public:
	dungeon_enter_test(void);
	virtual ~dungeon_enter_test(void);

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
