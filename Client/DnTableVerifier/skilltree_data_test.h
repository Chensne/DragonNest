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
	// 매 테스트 마다 초기화/해제
	virtual void SetUp( void );
	virtual void TearDown( void );
	//

	// 테스트 케이스 전체에 해당하는 초기화/해제
	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
	////////////////////////////////////////////////////////////////
};
