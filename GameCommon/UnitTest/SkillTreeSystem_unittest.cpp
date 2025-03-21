#include "stdafx.h"
#include "DNUserSession.h"
#include "DnSkillTreeSystem.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

extern TVillageConfig g_Config;

class SkillTreeSystem_unittest:public testing::Test
{
public:
	static CDnSkillTreeSystem* s_pSkillTreeSystem;

protected:
	virtual void SetUp() 
	{

	}

	virtual void TearDown()
	{

	}

	static void SetUpTestCase( void )
	{
		s_pSkillTreeSystem = new CDnSkillTreeSystem;
		s_pSkillTreeSystem->InitializeTable();
	}

	static void TearDownTestCase( void )
	{
		delete s_pSkillTreeSystem;
	}
};

CDnSkillTreeSystem* SkillTreeSystem_unittest::s_pSkillTreeSystem = NULL;

// NOTE: 추후에 실제 스킬트리 데이터가 들어갔을 때 해당 값으로 바꿔줍니다~



// 잘못된 스킬북 아이템 ID 일 때 언락 테스트.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_INVALID_SKILLBOOK_ITEMID )
{
	//vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	//CDnSkillTreeSystem::S_TRY_UNLOCK CurrentSkillInfo( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_TRY_UNLOCK CurrentSkillInfo;
	CDnSkillTreeSystem::S_OUTPUT Output;

	CurrentSkillInfo.iCurrentCharLevel = 99;
	CurrentSkillInfo.iTryUnlockSkillID = 3;
	CurrentSkillInfo.iSkillBookItemID = 100;
	CurrentSkillInfo.iJobID = 1;

	//// 선행 부모스킬 2개 추가.
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryUnLockSkill( CurrentSkillInfo, &Output );
	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_INVALID_SKILLBOOK_ITEMID );
}

// 제대로 된 조건일 때 스킬 언락 테스트.
TEST_F( SkillTreeSystem_unittest, UNLOCK_SKILL )
{
	//vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	//CDnSkillTreeSystem::S_TRY_UNLOCK CurrentSkillInfo( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_TRY_UNLOCK CurrentSkillInfo;
	CDnSkillTreeSystem::S_OUTPUT Output;

	CurrentSkillInfo.iCurrentCharLevel = 99;
	CurrentSkillInfo.iTryUnlockSkillID = 3;
	CurrentSkillInfo.iSkillBookItemID = 300;
	CurrentSkillInfo.iJobID = 1;

	//// 선행 부모스킬 2개 추가.
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryUnLockSkill( CurrentSkillInfo, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_SUCCESS );
}

// 없는 스킬(잘못된 스킬 ID) 언락 테스트.
// 직업이 잘못된 경우도 해당됨.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_INVALID_SKILLID )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 999;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// 선행 부모스킬 2개 추가.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_CANT_FIND_SKILLNODE );
}

// 필요한 부모 스킬이 없을 때 언락 테스트.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_DONT_HAVE_PARENTSKILL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// 선행 부모스킬 1개만 추가.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL );
}

// 필요한 부모 스킬의 락이 풀리지 않았을 때 언락 테스트.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_UNLOCKED_PARENTSKILL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// 선행 부모스킬 2개 추가.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, true ) );		// 락걸린 상태로 넘겨준다.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_LOCKED_PARENTSKILL );
}

// 부모 스킬이 있되 부모 스킬 레벨이 모자랄 때 획득 테스트.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_NOT_ENOUGH_PARENTSKILL_LEVEL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// 선행 부모스킬 2개 추가.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 1, false ) );	// 부모스킬 레벨이 모자라.
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL );
}

// 캐릭터 레벨 모자랄 때 획득 테스트.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_NOT_ENOUGH_CHAR_LEVEL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 1;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// 선행 부모스킬 2개 추가.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL );
}


// 모든 조건 갖춰졌을 때 스킬 언락 테스트.
TEST_F( SkillTreeSystem_unittest, TRY_UNLOCK )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// 선행 부모스킬 2개 추가.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_SUCCESS );
}

// 부모 스킬이 없는 스킬 언락 테스트
TEST_F( SkillTreeSystem_unittest, TRY_DONT_HAVE_PARENT_SKILL_UNLOCK )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 4;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_SUCCESS );
}


// 스킬 트리 테이블에 정의된 대로 트리가 제대로 렌더링 되도록 정보를 돌려주는지 테스트.
TEST_F( SkillTreeSystem_unittest, RENDER_TREE )
{
	int iJobID = 1;

	int iNodeCount = SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeCount( iJobID );
	EXPECT_EQ( iNodeCount, 4 );

	int iSlotCount = SkillTreeSystem_unittest::s_pSkillTreeSystem->GetSlotCount( iJobID );
	EXPECT_EQ( iSlotCount, 32 );

	// 잘못된 잡 아이디 넘겨줬을 때,
	CDnSkillTreeSystem::S_NODE_RENDER_INFO NodeRenderInfo;
	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 0, 2, &NodeRenderInfo );
	EXPECT_EQ( NodeRenderInfo.iSkillID, 0 );

	// 순차적으로 제대로 된 값이 넘어오는지 확인.
	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 0, iJobID, &NodeRenderInfo );
	EXPECT_NE( NodeRenderInfo.iSkillID, 0 );
	EXPECT_EQ( NodeRenderInfo.iSlotIndex, 0 );
	EXPECT_EQ( NodeRenderInfo.vlChildSlotIndexInJob.front(), 4 );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 1, iJobID, &NodeRenderInfo );
	EXPECT_NE( NodeRenderInfo.iSkillID, 0 );
	EXPECT_EQ( NodeRenderInfo.iSlotIndex, 1 );
	EXPECT_EQ( NodeRenderInfo.vlChildSlotIndexInJob.front(), 4 );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 2, iJobID, &NodeRenderInfo );
	EXPECT_EQ( NodeRenderInfo.iSkillID, 0 );
	EXPECT_EQ( NodeRenderInfo.iSlotIndex, 0 );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 3, iJobID, &NodeRenderInfo );
	EXPECT_EQ( NodeRenderInfo.iSkillID, 0 );
	EXPECT_EQ( NodeRenderInfo.iSlotIndex, 0 );

	// 2개의 부모가 있는 스킬.
	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 4, iJobID, &NodeRenderInfo );
	EXPECT_EQ( NodeRenderInfo.iSkillID, 3 );
	EXPECT_EQ( NodeRenderInfo.iSlotIndex, 4 );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 5, iJobID, &NodeRenderInfo );
	EXPECT_EQ( NodeRenderInfo.iSkillID, 0 );
	EXPECT_EQ( NodeRenderInfo.iSlotIndex, 0 );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 6, iJobID, &NodeRenderInfo );
	EXPECT_EQ( NodeRenderInfo.iSkillID, 4 );
	EXPECT_EQ( NodeRenderInfo.iSlotIndex, 6 );

	//SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 7, iJobID, &NodeRenderInfo );
	//EXPECT_NE( NodeRenderInfo.iSkillID, 0 );
	//EXPECT_EQ( NodeRenderInfo.iSlotIndex, 7 );
}

TEST_F( SkillTreeSystem_unittest, FIND_SKILL_BY_SKILLBOOK )
{
	int iSkillID = SkillTreeSystem_unittest::s_pSkillTreeSystem->FindSkillBySkillBook( 300 );
	EXPECT_EQ( iSkillID, 3 );
}

// 스킬 리셋 테스트. SP 모두 반환되며 락 정보는 DB에 유지 됨.
// 전직시에 스킬 리셋됨.
TEST_F( SkillTreeSystem_unittest, RESET )
{
	
}

#endif // #if !defined( _FINAL_BUILD )
