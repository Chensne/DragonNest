#pragma once

//class CDnCutSceneActor;
class CEtActionBase;

class skilltree_data_test : public ::testing::Test
{
public:
	static DNTableFileFormat* s_pSkillTreeTable;
	static DNTableFileFormat* s_pSkillTable;

protected:
	

public:
	skilltree_data_test(void);
	virtual ~skilltree_data_test(void);

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
