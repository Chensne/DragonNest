#include "stdafx.h"
#include "DNUserSession.h"
#include "DnBubbleSystem.h"
#include "DnObserverEventMessage.h"
#include "EtResourceMng.h"
#include "DnPlayerActor.h"
#include "DnGameRoom.h"


#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

using namespace BubbleSystem;
using namespace boost;

class BubbleSystem_unittest:public testing::Test
{
public:
	static CDnBubbleSystem* s_pBubbleSystem;
	static CEtResourceMng*	m_pResMng;
	static CDnPlayerActor* s_pPlayerActor;
	static CDNGameRoom* s_pRoom;

protected:
	virtual void SetUp() 
	{

	}

	virtual void TearDown()
	{
		s_pBubbleSystem->RemoveAllBubbles();
	}

	static void SetUpTestCase( void )
	{
		m_pResMng = new CEtResourceMng( false, true );
		m_pResMng->AddResourcePath( "r:\\gameres\\resource\\ext" );

		CDnTableDB::GetInstance().CreateInstance();
		CDnTableDB::GetInstance().Initialize();

		s_pBubbleSystem = new CDnBubbleSystem;

		//// 직업 히스토리는 우선 블록 ex 를 사용하는 직업으로 가정하고 처리한다.
		//vector<int> vlJobHistory;
		//vlJobHistory.push_back( 4 );		// 클러릭
		//vlJobHistory.push_back( 20 );		// 팔라딘
		////vlJobHistory.push_back(  );			// 아직 2차 전직 모르므로 팔라딘이 사용한다고 가정하고 테이블 작업 임시로 해둠.

		// 추후에는 CDnMonsterActor 도 지원될 예정.
		MAGAReqRoomID Packet;
		memset( &Packet, 0, sizeof(Packet) );
		s_pRoom	= new CDNGameRoom( NULL, 0, &Packet );

		s_pPlayerActor = new CDnPlayerActor( s_pRoom, 1 );
		s_pPlayerActor->SetHP( 100 );
		s_pBubbleSystem->Initialize( s_pPlayerActor->GetActorHandle() );
	}

	static void TearDownTestCase( void )
	{
		delete s_pBubbleSystem;
		delete s_pPlayerActor;
		delete s_pRoom;

		CDnTableDB::GetInstance().Finalize();

		delete m_pResMng;
	}
};

CDnBubbleSystem* BubbleSystem_unittest::s_pBubbleSystem = NULL;
CEtResourceMng*	BubbleSystem_unittest::m_pResMng = NULL;
CDnPlayerActor* BubbleSystem_unittest::s_pPlayerActor = NULL;
CDNGameRoom* BubbleSystem_unittest::s_pRoom = NULL;

// 블록 성공 메시지 처리 테스트.
TEST_F( BubbleSystem_unittest, BLOCK_SUCCESS_TEST )
{
	shared_ptr<IDnObserverNotifyEvent> pNotifyEvent( new CDnBlockSuccessMessage );
	shared_ptr<CDnBlockSuccessMessage> pBlockSuccessMessage = shared_polymorphic_downcast<CDnBlockSuccessMessage>( pNotifyEvent );

	pBlockSuccessMessage->SetSkillID( 3301 );
	s_pBubbleSystem->OnEvent( pNotifyEvent );

	// 버블 하나 추가 됨.
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 1 );
}

// 일정 갯수 이상의 버블이 쌓였을 때 버블을 소모하는 스킬 사용 테스트.
TEST_F( BubbleSystem_unittest, USE_SKILL_TEST )
{
	shared_ptr<IDnObserverNotifyEvent> pNotifyEvent( new CDnUseSkillMessage );
	shared_ptr<CDnUseSkillMessage> pSkillUseEvent = shared_polymorphic_downcast<CDnUseSkillMessage>( pNotifyEvent );

	// 테이블에 테스트로 가디언 포스를 사용하기 위한 100번 버블을 10개 필요하도록 설정한 상태.

	// 처음엔 엉뚱한 스킬 사용. 데이터에 매치되는 이벤트가 없으므로 아무 일도 일어나지 않음.
	pSkillUseEvent->SetSkillID( 3028 );		// 쉴드 블로우
	s_pBubbleSystem->OnEvent( pNotifyEvent );

	// 100번 버블을 5개 추가해놓고 가디언 포스 사용.
	CDnBubbleSystem::S_CREATE_BUBBLE Info;
	Info.iBubbleTypeID = 100;
	Info.fDurationTime = 5.0f;
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );

	// 데이터에 매치되는 스킬을 사용. 데이터에 추가 되어 있는대로 지정된 버블이 소모 되며 스킬 사용.
	pSkillUseEvent->SetSkillID( 3302 );		// 가디언 포스
	s_pBubbleSystem->OnEvent( pNotifyEvent );

	// 버블은 사용되지 않았으므로 5개가 그대로 남는다.
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 5 );

	// 100번 버블을 5개 더 추가해서 조건을 충족시킨 후 가디언 포스 사용.
	// 100번 버블을 5개 추가해놓고 가디언 포스 사용.
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );

	pSkillUseEvent->SetSkillID( 3302 );		// 가디언 포스
	s_pBubbleSystem->OnEvent( pNotifyEvent );
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 0 );
}

// 특정 스킬을 갖고 있는 상태에서 다른 스킬을 사용하여 블록을 획득하는 조건일 경우 테스트,
TEST_F( BubbleSystem_unittest, OWN_THIS_SKILL_TEST )
{
	// 3301 오토블록 EX 를 획득했을 때 3021 블록 스킬로 블록을 성공했을 경우 특정 버블 추가.
	shared_ptr<IDnObserverNotifyEvent> pNotifyEvent( new CDnBlockSuccessMessage );
	shared_ptr<CDnBlockSuccessMessage> pBlockSuccessMessage = shared_polymorphic_downcast<CDnBlockSuccessMessage>( pNotifyEvent );

	// 그냥 블록 성공. 아무 버블도 안 얻음.
	pBlockSuccessMessage->SetSkillID( 3021 );
	s_pBubbleSystem->OnEvent( pNotifyEvent );
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 0 );

	// 3301 오토블록 EX 를 갖고 있을 때 블록 성공.
	// 100번 버블을 획득한다.
	s_pPlayerActor->MASkillUser::AddSkill( 3301, 1, CDnSkill::PVE );
	s_pBubbleSystem->OnEvent( pNotifyEvent );
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 1 );

	// 3301 오토블록 EX 를 갖고 있을 때 패링 성공.
	pNotifyEvent.reset( new CDnParringSuccessMessage );
	shared_ptr<CDnParringSuccessMessage> pParringSuccessMessage = shared_polymorphic_downcast<CDnParringSuccessMessage>( pNotifyEvent );
	pParringSuccessMessage->SetSkillID( 3221 );		// 스탠스 오브 페이스
	s_pBubbleSystem->OnEvent( pParringSuccessMessage );
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 2 );
	
	// 3301 오토블록 EX 를 갖고 있을 때 쿨타임 패링 성공.
	pNotifyEvent.reset( new CDnCooltimeParringSuccessMessage );
	shared_ptr<CDnCooltimeParringSuccessMessage> pCooltimeParringSuccessMessage = shared_polymorphic_downcast<CDnCooltimeParringSuccessMessage>( pNotifyEvent );
	pCooltimeParringSuccessMessage->SetSkillID( 3220 ); // 에어리얼 블록
	s_pBubbleSystem->OnEvent( pCooltimeParringSuccessMessage );
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 3 );
}


// 추가된 버블이 일정 시간 프로세스를 돌면서 제대로 없어지는지 테스트.
TEST_F( BubbleSystem_unittest, BUBBLE_PROCESS_TEST )
{
	CDnBubbleSystem::S_CREATE_BUBBLE Info;
	Info.iBubbleTypeID = 100;
	Info.fDurationTime = 3.0f;
	s_pBubbleSystem->AddBubble( Info );
	s_pBubbleSystem->AddBubble( Info );

	Info.iBubbleTypeID = 101;
	s_pBubbleSystem->AddBubble( Info );
	Info.fDurationTime = 5.0f;
	s_pBubbleSystem->AddBubble( Info );
	Info.fDurationTime = 7.0f;
	s_pBubbleSystem->AddBubble( Info );

	Info.iBubbleTypeID = 102;
	Info.fDurationTime = 3.0f;
	s_pBubbleSystem->AddBubble( Info );
	Info.fDurationTime = 4.0f;
	s_pBubbleSystem->AddBubble( Info );

	// 10초간 루프 돈다.
	LOCAL_TIME StartTime = timeGetTime();
	LOCAL_TIME PrevTime = 0;
	while( true )
	{
		LOCAL_TIME LocalTime = timeGetTime()-StartTime;
		float fDelta = float(StartTime - PrevTime) / 1000.0f;
		s_pBubbleSystem->Process( LocalTime, fDelta );
		if( 10000 < timeGetTime() - StartTime )
			break;

		PrevTime = LocalTime;
	}

	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 100 ), 0 );
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 101 ), 0 );
	EXPECT_EQ( s_pBubbleSystem->GetBubbleCountByTypeID( 102 ), 0 );
}

#endif // #if !defined( _FINAL_BUILD )
