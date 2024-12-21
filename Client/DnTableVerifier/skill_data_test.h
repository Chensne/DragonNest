#pragma once

//class CDnCutSceneActor;
class CEtActionBase;

class skill_data_test : public ::testing::Test
{
public:
	//static DNTableFileFormat* s_pUseSkillActors;
	//static DNTableFileFormat* s_pMonsterActorTable;		// ��ų�� ����ϴ� ���͵��� ������ ���� ���߿�.
	static DNTableFileFormat* s_pSkillTable;
	static DNTableFileFormat* s_pSkillLevelTable;
	static DNTableFileFormat* s_pItemTable;					// ��ų�� ������ ����.
	static set<int> s_setSkillBookItemID;			// ��ų �� ������ ID ��Ƴ���.

	static CEtActionBase* s_pWarrior;
	static CEtActionBase* s_pArcher;
	static CEtActionBase* s_pCleric;
	static CEtActionBase* s_pSoceress;

protected:
	

public:
	skill_data_test(void);
	virtual ~skill_data_test(void);

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
