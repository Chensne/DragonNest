#include "StdAfx.h"
#include "skilltree_data_test.h"

DNTableFileFormat* skilltree_data_test::s_pSkillTable = NULL;
DNTableFileFormat* skilltree_data_test::s_pSkillTreeTable = NULL;

#define SKILLTREE_T skilltree_data_test::s_pSkillTreeTable
#define SKILL_T skilltree_data_test::s_pSkillTable

skilltree_data_test::skilltree_data_test(void)
{
	
}

skilltree_data_test::~skilltree_data_test(void)
{

}


void skilltree_data_test::SetUpTestCase( void )
{
	s_pSkillTable = LoadExtFile( "SkillTable.dnt" );
	s_pSkillTreeTable = LoadExtFile( "SkillTreeTable.dnt" );
}

void skilltree_data_test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pSkillTreeTable );
	SAFE_DELETE( s_pSkillTable );
}


void skilltree_data_test::SetUp( void )
{

}


void skilltree_data_test::TearDown( void )
{

}

// 같은 스킬 ID 를 두 번 참조함.
TEST_F( skilltree_data_test, REFERENCE_SKILLID_DUPLICATED )
{
	TCHAR atcTraceBuf[ 1024 ] = { 0 };
	set<int> setReferencedSkillID;
	int iNumItemCount = SKILLTREE_T->GetItemCount();
	for( int i = 0; i < iNumItemCount; ++i )
	{
		int iTreeTableID = SKILLTREE_T->GetItemID( i );
		int iSkillID = SKILLTREE_T->GetFieldFromLablePtr( iTreeTableID, "_SkillTableID" )->GetInteger();

		{
			_stprintf_s( atcTraceBuf, _T("{TableName:SkillTreeTable, Index:%d, Note: 스킬트리테이블에서 참조하는 스킬 ID 가 0 입니다.}"), iTreeTableID );
			SCOPED_TRACE( atcTraceBuf );
			EXPECT_NE( iSkillID, 0 );
		}

		if( 0 < setReferencedSkillID.count( iSkillID ) )
		{
			_stprintf_s( atcTraceBuf, _T("{TableName:SkillTreeTable, Index:%d, SkillTableID:%d, Note: 스킬트리테이블에서 참조하는 스킬 ID 가 중복입니다.}"), iTreeTableID, iSkillID );
			SCOPED_TRACE( atcTraceBuf );
			EXPECT_TRUE( 0 == setReferencedSkillID.count( iSkillID ) );
		}
		else
		{
			setReferencedSkillID.insert( iSkillID );
		}
	}
}


// 스킬 데이터쪽에 없는 스킬을 스킬 테이블에서 참조.
TEST_F( skilltree_data_test, NOT_EXIST_SKILL_REFERENCED )
{
	TCHAR atcTraceBuf[ 1024 ] = { 0 };
	int iNumItemCount = SKILLTREE_T->GetItemCount();
	for( int i = 0; i < iNumItemCount; ++i )
	{
		int iTreeTableID = SKILLTREE_T->GetItemID( i );
		int iSkillID = SKILLTREE_T->GetFieldFromLablePtr( iTreeTableID, "_SkillTableID" )->GetInteger();

		{
			_stprintf_s( atcTraceBuf, _T("{TableName:SkillTreeTable, Index:%d, SkillTableID:%d, Note: 스킬트리테이블에서 참조하는 스킬 ID 가 스킬 테이블에 존재하지 않습니다.}"), iTreeTableID, iSkillID );
			SCOPED_TRACE( atcTraceBuf );
			EXPECT_TRUE( SKILL_T->IsExistItem( iSkillID ) );
		}
	}
}