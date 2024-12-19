
#include "stdafx.h"
#include "./boost/scoped_ptr.hpp"
#include "MAAiScript.h"
#include "TDnPlayerWarrior.h"
#include "DnPlayerActor.h"
#include "DNGameRoom.h"
#include "DnSkill.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)


// 관련 이슈	: 없음
// 작성자		: 김밥
// 작성일		: 2009.10.20
// 설명			: 몬스터AI 에서 사용되는 *.lua 파일 로딩 테스트

#define PRE_TEST_15800

class MonsterAI_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		// 방 생성
		MAGAReqRoomID Packet;
		memset( &Packet, 0, sizeof(Packet) );
		m_pRoom	= new CDNGameRoom( NULL, 0, &Packet );

		// 액터 생성
		m_hActor = (new TDnPlayerWarrior<CDnPlayerActor>( m_pRoom, 0 ) )->GetMySmartPtr();
	}
	virtual void TearDown()
	{
		delete m_hActor.GetPointer();
		delete m_pRoom;
	}

	static void SetUpTestCase()
	{
		m_pResMng = new CEtResourceMng( false, true );
		EXPECT_TRUE( m_pResMng->AddResourcePath( "r:\\gameres\\resource\\script\\monsterai" ) );
		EXPECT_TRUE( m_pResMng->AddResourcePath( "r:\\gameres\\resource\\ext" ) );

		// CDnTableDB 객체 생성
		m_pTableDB = new CDnTableDB();
		EXPECT_TRUE( m_pTableDB->Initialize() );

		// *.lua 로드
		m_ScriptLoader = new AiScriptLoader();

		std::vector<std::string> FileList;
#if defined( PRE_TEST_15800 )
		FindFileListInDirectory( m_pResMng->GetFullPath( "MonsterAI" ).c_str(), "AiOrc_Black_Boss_Easy.lua", FileList );
#else
		FindFileListInDirectory( m_pResMng->GetFullPath( "MonsterAI" ).c_str(), "*.lua", FileList );
#endif // #if defined( PRE_TEST_15800 )

		for( UINT i = 0 ; i<FileList.size() ; ++i )
		{
			EXPECT_TRUE( m_ScriptLoader->LoadScript( FileList[i].c_str() ) ) << FileList[i];
		}
	}

	static void TearDownTestCase()
	{
		delete m_pResMng;
		delete m_pTableDB;
		delete m_ScriptLoader;
	}

	static CEtResourceMng*	m_pResMng;
	static AiScriptLoader*	m_ScriptLoader;
	static CDnTableDB*		m_pTableDB;
	CDNGameRoom*			m_pRoom;
	DnActorHandle			m_hActor;
};

CEtResourceMng* MonsterAI_unittest::m_pResMng		= NULL;
AiScriptLoader* MonsterAI_unittest::m_ScriptLoader	= NULL;
CDnTableDB*		MonsterAI_unittest::m_pTableDB		= NULL;

// 관련 이슈	:	없음
// 작성자		:	김밥
// 작성일		:	2009.10.20
// 설명			:	몬스터AI 에서 사용되는 *.lua 유닛 테스트
//					1. 잘못된 스킬인덱스 검사
//					2. Target 관련 설정 오류 검사
//					3. UsedSkillIndex 오류 검사
//					4. NotUsedSkillIndex 오류 검사
//					5. Turn 관련 액션 Loop 값 검사

TEST_F( MonsterAI_unittest, VerifyLua )
{
	DNTableFileFormat* pSkillSox = m_pTableDB->GetTable( CDnTableDB::TSKILL );
	EXPECT_TRUE( pSkillSox != NULL );

	for( std::map<std::string, ScriptData>::iterator itor=m_ScriptLoader->m_ScriptMap.begin() ; itor!=m_ScriptLoader->m_ScriptMap.end() ; ++itor )
	{
		// 1.잘못된 스킬인덱스 검사
#if defined(PRE_ADD_64990)
		for (int nIndex = 0; nIndex < 2; ++nIndex)
		{
			for( UINT i=0 ; i<(*itor).second.m_SkillTable[nIndex].size() ; ++i )
			{
				EXPECT_TRUE( pSkillSox->IsExistItem( (*itor).second.m_SkillTable[nIndex][i].nSkillIndex ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << (*itor).second.m_SkillTable[nIndex][i].nSkillIndex;

				for( UINT j=0 ; j<(*itor).second.m_SkillTable[nIndex][i].pMAAiCheckerManager->m_vActorChecker.size() ; ++j )
				{
					// 3. UsedSkillIndex 오류 검사
					CMAAiActorUsedSkillChecker* pChecker = dynamic_cast<CMAAiActorUsedSkillChecker*>((*itor).second.m_SkillTable[nIndex][i].pMAAiCheckerManager->m_vActorChecker[j]);
					if( pChecker )
					{
						for( UINT k=0 ; k<pChecker->m_vSkillTableID.size() ; ++k )
							EXPECT_TRUE( pSkillSox->IsExistItem( pChecker->m_vSkillTableID[k] ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << pChecker->m_vSkillTableID[k];
					}
					// 4. NotUsedSkillIndex 오류 검사
					CMAAiActorNotUsedSkillChecker* pChecker2 = dynamic_cast<CMAAiActorNotUsedSkillChecker*>((*itor).second.m_SkillTable[nIndex][i].pMAAiCheckerManager->m_vActorChecker[j]);
					if( pChecker2 )
					{
						for( UINT k=0 ; k<pChecker2->m_vSkillTableID.size() ; ++k )
							EXPECT_TRUE( pSkillSox->IsExistItem( pChecker2->m_vSkillTableID[k] ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << pChecker2->m_vSkillTableID[k];
					}
				}
			}
		}
#else
		for( UINT i=0 ; i<(*itor).second.m_SkillTable.size() ; ++i )
		{
			EXPECT_TRUE( pSkillSox->IsExistItem( (*itor).second.m_SkillTable[i].nSkillIndex ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << (*itor).second.m_SkillTable[i].nSkillIndex;

			for( UINT j=0 ; j<(*itor).second.m_SkillTable[i].pMAAiCheckerManager->m_vActorChecker.size() ; ++j )
			{
				// 3. UsedSkillIndex 오류 검사
				CMAAiActorUsedSkillChecker* pChecker = dynamic_cast<CMAAiActorUsedSkillChecker*>((*itor).second.m_SkillTable[i].pMAAiCheckerManager->m_vActorChecker[j]);
				if( pChecker )
				{
					for( UINT k=0 ; k<pChecker->m_vSkillTableID.size() ; ++k )
						EXPECT_TRUE( pSkillSox->IsExistItem( pChecker->m_vSkillTableID[k] ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << pChecker->m_vSkillTableID[k];
				}
				// 4. NotUsedSkillIndex 오류 검사
				CMAAiActorNotUsedSkillChecker* pChecker2 = dynamic_cast<CMAAiActorNotUsedSkillChecker*>((*itor).second.m_SkillTable[i].pMAAiCheckerManager->m_vActorChecker[j]);
				if( pChecker2 )
				{
					for( UINT k=0 ; k<pChecker2->m_vSkillTableID.size() ; ++k )
						EXPECT_TRUE( pSkillSox->IsExistItem( pChecker2->m_vSkillTableID[k] ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << pChecker2->m_vSkillTableID[k];
				}
			}
		}
#endif // PRE_ADD_64990

		// 2. Target 관련 설정 오류 검사
		for( UINT i=0 ; i<(*itor).second.m_AITable.size() ; ++i )
		{
			for( UINT j=0 ; j<(*itor).second.m_AITable[i].VecActionTable.size() ; ++j )
			{
				ActionTable& AT = (*itor).second.m_AITable[i].VecActionTable[j];

#if defined (PRE_MOD_AIMULTITARGET)
				EXPECT_TRUE( !(AT.fRandomTarget != 0.f && AT.nMultipleTarget > 0) ) << (*itor).first;
#else
				EXPECT_TRUE( !(AT.fRandomTarget != 0.f && AT.bMultipleTarget == true) ) << (*itor).first;
#endif

				for( UINT k=0 ; k<AT.pMAAiCheckerManager->m_vActorChecker.size() ; ++k )
				{
					// 3. UsedSkillIndex 오류 검사
					CMAAiActorUsedSkillChecker* pChecker = dynamic_cast<CMAAiActorUsedSkillChecker*>(AT.pMAAiCheckerManager->m_vActorChecker[k]);
					if( pChecker )
					{
						for( UINT x=0 ; x<pChecker->m_vSkillTableID.size() ; ++x )
							EXPECT_TRUE( pSkillSox->IsExistItem( pChecker->m_vSkillTableID[x] ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << pChecker->m_vSkillTableID[x];
					}
					// 4. NotUsedSkillIndex 오류 검사
					CMAAiActorNotUsedSkillChecker* pChecker2 = dynamic_cast<CMAAiActorNotUsedSkillChecker*>(AT.pMAAiCheckerManager->m_vActorChecker[k]);
					if( pChecker2 )
					{
						for( UINT x=0 ; x<pChecker2->m_vSkillTableID.size() ; ++x )
							EXPECT_TRUE( pSkillSox->IsExistItem( pChecker2->m_vSkillTableID[x] ) ) << "FileName:"<< (*itor).first << " SkillIndex:" << pChecker2->m_vSkillTableID[x];
					}
				}

				// 5. Turn 관련 액션 Loop 값 검사
				if( strstr( AT.szActionName.c_str(), "Turn" ) )
				{
					EXPECT_TRUE( AT.nLoop == -1 ) << "FileName:"<< (*itor).first << " Turn 액션은 Loop 가 -1 이어야합니다.";
				}
			}
		}
	}
}

// 관련 이슈	: 없음
// 작성자		: 김밥
// 작성일		: 2009.10.20
// 설명			: 몬스터AI 에서 사용되는 SelfHPPercentRange( HP >= Min && HP <= Max ) 체크 하는 클래스 유닛테스트

TEST_F( MonsterAI_unittest, CMAAiActorHPPercentCheckerRange )
{
	boost::scoped_ptr<CMAAiActorHPPercentCheckerRange> pChecker( new CMAAiActorHPPercentCheckerRange( 3, 50 ) );

	m_hActor->SetMaxHP( 100 );
	m_hActor->SetHP( 10 );
	EXPECT_TRUE( pChecker->bIsCheck( m_hActor,0 ) );

	m_hActor->SetHP( 51 );
	EXPECT_FALSE( pChecker->bIsCheck( m_hActor,0 ) );
}

// 관련 이슈	: 없음
// 작성자		: 김밥
// 작성일		: 2009.10.21
// 설명			: 몬스터AI 에서 사용되는 SelfHPPercent( CheckHP >= HP ) 체크 하는 클래스 유닛테스트

TEST_F( MonsterAI_unittest, CMAAiActorHPPercentChecker )
{
	boost::scoped_ptr<CMAAiActorHPPercentChecker> pChecker( new CMAAiActorHPPercentChecker( 30 ) );
	boost::scoped_ptr<CMAAiActorHPPercentChecker> pChecker2( new CMAAiActorHPPercentChecker( 5 ) );

	m_hActor->SetMaxHP( 100 );
	m_hActor->SetHP( 10 );
	
	EXPECT_TRUE( pChecker->bIsCheck( m_hActor,0 ) );		// 30>=10
	EXPECT_FALSE( pChecker2->bIsCheck( m_hActor,0 ) );	//  5>=10
}

// 관련 이슈	: 없음
// 작성자		: 김밥
// 작성일		: 2009.10.27
// 설명			: 몬스터AI 에서 사용되는 사용중인 스킬 체크 하는 클래스 유닛테스트

TEST_F( MonsterAI_unittest, CMAAiActorUsedSkillChecker )
{
	std::vector<UINT> vSkill;
	vSkill.push_back( 1 );
	vSkill.push_back( 2 );
	vSkill.push_back( 3 );

	boost::scoped_ptr<CMAAiActorUsedSkillChecker> pChecker( new CMAAiActorUsedSkillChecker( vSkill ) );
	
	m_hActor->SetHP( 100 );

	for( UINT i=0 ; i<vSkill.size() ; ++i )
	{
		CDnSkill::SkillInfo sSkillInfo;
		sSkillInfo.iSkillID = vSkill[i];
		EXPECT_FALSE( pChecker->bIsCheck( m_hActor,0 ) );
		EXPECT_TRUE( m_hActor->CDnActor::CmdAddStateEffect( &sSkillInfo, static_cast<STATE_BLOW::emBLOW_INDEX>(vSkill[i]), 0, "" ) != -1 );
	}

	EXPECT_TRUE( pChecker->bIsCheck( m_hActor,0 ) );
}

// 관련 이슈	: 없음
// 작성자		: 김밥
// 작성일		: 2009.10.28
// 설명			: 몬스터AI 에서 사용되는 사용중이지 않은 스킬 체크 하는 클래스 유닛테스트

TEST_F( MonsterAI_unittest, CMAAiActorNotUsedSkillChecker )
{
	std::vector<UINT> vSkill;
	vSkill.push_back( 1 );
	vSkill.push_back( 2 );
	vSkill.push_back( 3 );

	boost::scoped_ptr<CMAAiActorNotUsedSkillChecker> pChecker( new CMAAiActorNotUsedSkillChecker( vSkill ) );

	m_hActor->SetHP( 100 );

	EXPECT_TRUE( pChecker->bIsCheck( m_hActor,0 ) );

	for( UINT i=0 ; i<vSkill.size() ; ++i )
	{
		CDnSkill::SkillInfo sSkillInfo;
		sSkillInfo.iSkillID = vSkill[i];
		EXPECT_TRUE( m_hActor->CDnActor::CmdAddStateEffect( &sSkillInfo, static_cast<STATE_BLOW::emBLOW_INDEX>(vSkill[i]), 0, "" ) != -1 );
		EXPECT_FALSE( pChecker->bIsCheck( m_hActor,0 ) );
	}
}

// 관련 이슈	: 없음
// 작성자		: 김밥
// 작성일		: 2009.10.27
// 설명			: 몬스터AI 에서 사용되는 SP 체크 하는 클래스 유닛테스트

TEST_F( MonsterAI_unittest, CMAAiActorSPChecker )
{
	boost::scoped_ptr<CMAAiActorSPChecker> pChecker( new CMAAiActorSPChecker(5) );

	m_hActor->SetSP( 10 );
	EXPECT_TRUE( pChecker->bIsCheck( m_hActor,0 ) );
	m_hActor->SetSP( 5 );
	EXPECT_TRUE( pChecker->bIsCheck( m_hActor,0 ) );
	m_hActor->SetSP( 3 );
	EXPECT_FALSE( pChecker->bIsCheck( m_hActor,0 ) );
}

// 관련 이슈	: #15800
// 작성자		: 김밥
// 작성일		: 2009.06.09
// 설명			: SetUpTestCase 에서만 unittest 진행되고 아무것도 하지 않는 TC

TEST_F( MonsterAI_unittest, NullTC )
{

}

#endif // #if !defined( _FINAL_BUILD )
