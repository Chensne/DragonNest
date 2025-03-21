#pragma once

//class CDnCutSceneActor;
class CEtActionBase;

class skill_data_test : public ::testing::Test
{
public:
	//static DNTableFileFormat* s_pUseSkillActors;
	//static DNTableFileFormat* s_pMonsterActorTable;		// 스킬을 사용하는 몬스터들을 얻어오는 것은 나중에.
	static DNTableFileFormat* s_pSkillTable;
	static DNTableFileFormat* s_pSkillLevelTable;
	static DNTableFileFormat* s_pItemTable;					// 스킬북 검증을 위해.
	static set<int> s_setSkillBookItemID;			// 스킬 북 아이템 ID 모아놓음.

	static CEtActionBase* s_pWarrior;
	static CEtActionBase* s_pArcher;
	static CEtActionBase* s_pCleric;
	static CEtActionBase* s_pSoceress;

protected:
	

public:
	skill_data_test(void);
	virtual ~skill_data_test(void);

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
