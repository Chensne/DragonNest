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

// NOTE: ���Ŀ� ���� ��ųƮ�� �����Ͱ� ���� �� �ش� ������ �ٲ��ݴϴ�~



// �߸��� ��ų�� ������ ID �� �� ��� �׽�Ʈ.
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

	//// ���� �θ�ų 2�� �߰�.
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryUnLockSkill( CurrentSkillInfo, &Output );
	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_INVALID_SKILLBOOK_ITEMID );
}

// ����� �� ������ �� ��ų ��� �׽�Ʈ.
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

	//// ���� �θ�ų 2�� �߰�.
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	//vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );

	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryUnLockSkill( CurrentSkillInfo, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_SUCCESS );
}

// ���� ��ų(�߸��� ��ų ID) ��� �׽�Ʈ.
// ������ �߸��� ��쵵 �ش��.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_INVALID_SKILLID )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 999;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// ���� �θ�ų 2�� �߰�.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_CANT_FIND_SKILLNODE );
}

// �ʿ��� �θ� ��ų�� ���� �� ��� �׽�Ʈ.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_DONT_HAVE_PARENTSKILL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// ���� �θ�ų 1���� �߰�.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL );
}

// �ʿ��� �θ� ��ų�� ���� Ǯ���� �ʾ��� �� ��� �׽�Ʈ.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_UNLOCKED_PARENTSKILL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// ���� �θ�ų 2�� �߰�.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, true ) );		// ���ɸ� ���·� �Ѱ��ش�.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_LOCKED_PARENTSKILL );
}

// �θ� ��ų�� �ֵ� �θ� ��ų ������ ���ڶ� �� ȹ�� �׽�Ʈ.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_NOT_ENOUGH_PARENTSKILL_LEVEL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// ���� �θ�ų 2�� �߰�.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 1, false ) );	// �θ�ų ������ ���ڶ�.
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL );
}

// ĳ���� ���� ���ڶ� �� ȹ�� �׽�Ʈ.
TEST_F( SkillTreeSystem_unittest, TRY_ACQUIRE_NOT_ENOUGH_CHAR_LEVEL )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 1;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// ���� �θ�ų 2�� �߰�.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL );
}


// ��� ���� �������� �� ��ų ��� �׽�Ʈ.
TEST_F( SkillTreeSystem_unittest, TRY_UNLOCK )
{
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfo;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfo );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = 99;
	TryAcquire.iTryAcquireSkillID = 3;
	TryAcquire.iHasSkillPoint = 100;
	TryAcquire.iJobID = 1;

	// ���� �θ�ų 2�� �߰�.
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 1, 3, false ) );
	vlPossessedSkillInfo.push_back( CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO( 2, 2, false ) );
	SkillTreeSystem_unittest::s_pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	EXPECT_EQ( Output.eErrorCode, CDnSkillTreeSystem::E_NONE );
	EXPECT_EQ( Output.eResult, CDnSkillTreeSystem::R_SUCCESS );
}

// �θ� ��ų�� ���� ��ų ��� �׽�Ʈ
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


// ��ų Ʈ�� ���̺� ���ǵ� ��� Ʈ���� ����� ������ �ǵ��� ������ �����ִ��� �׽�Ʈ.
TEST_F( SkillTreeSystem_unittest, RENDER_TREE )
{
	int iJobID = 1;

	int iNodeCount = SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeCount( iJobID );
	EXPECT_EQ( iNodeCount, 4 );

	int iSlotCount = SkillTreeSystem_unittest::s_pSkillTreeSystem->GetSlotCount( iJobID );
	EXPECT_EQ( iSlotCount, 32 );

	// �߸��� �� ���̵� �Ѱ����� ��,
	CDnSkillTreeSystem::S_NODE_RENDER_INFO NodeRenderInfo;
	SkillTreeSystem_unittest::s_pSkillTreeSystem->GetNodeRenderInfo( 0, 2, &NodeRenderInfo );
	EXPECT_EQ( NodeRenderInfo.iSkillID, 0 );

	// ���������� ����� �� ���� �Ѿ������ Ȯ��.
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

	// 2���� �θ� �ִ� ��ų.
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

// ��ų ���� �׽�Ʈ. SP ��� ��ȯ�Ǹ� �� ������ DB�� ���� ��.
// �����ÿ� ��ų ���µ�.
TEST_F( SkillTreeSystem_unittest, RESET )
{
	
}

#endif // #if !defined( _FINAL_BUILD )
