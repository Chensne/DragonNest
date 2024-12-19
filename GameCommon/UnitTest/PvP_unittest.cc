
#include "stdafx.h"
#include "DNDivisionManager.h"
#include "DNPvP.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class PvP_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		g_pDivisionManager = new CDivisionManager();
		EXPECT_TRUE( g_pDivisionManager );

		// dummp 유저 추가
#if defined(PRE_MOD_SELECT_CHAR)

#else	// #if defined(PRE_MOD_SELECT_CHAR)
#if defined(PRE_ADD_DWC)
#ifdef PRE_ADD_MULTILANGUAGE
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 0, 0, 0, L"dummy", L"dummy", 0, false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0 ,MultiLanguage::eDefaultLanguage ) );
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 1, 1, 0, L"dummy2", L"dummy2", 0, false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0 ,MultiLanguage::eDefaultLanguage ) );
#else		//#ifdef PRE_ADD_MULTILANGUAGE
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 0, 0, 0, L"dummy", L"dummy", 0, false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0, 0 ) );
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 1, 1, 0, L"dummy2", L"dummy2", 0, false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0, 0 ) );
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#else	//#if defined(PRE_ADD_DWC)
#ifdef PRE_ADD_MULTILANGUAGE
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 0, 0, 0, L"dummy", L"dummy", false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0 ,MultiLanguage::eDefaultLanguage ) );
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 1, 1, 0, L"dummy2", L"dummy2", false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0 ,MultiLanguage::eDefaultLanguage ) );
#else		//#ifdef PRE_ADD_MULTILANGUAGE
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 0, 0, 0, L"dummy", L"dummy", false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0, 0 ) );
		EXPECT_TRUE( g_pDivisionManager->AddUser( 0, 0, 0, 0, 1, 1, 0, L"dummy2", L"dummy2", false, "127.0.0.1", "127.0.0.1", 0, (BYTE*)"test",0 ,0, 0 ) );
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
#endif	//#if defined(PRE_ADD_DWC)
	}
	virtual void TearDown()
	{
		SAFE_DELETE( g_pDivisionManager );
	}
};

// 관련 이슈	: 버그 #6746( http://pms.eyedentitygames.com/issues/6746 )
// 작성자		: 김밥
// 작성일		: 2009.10.14
// 설명			: 게임서버에서 게임룸이 파괴되었을 때 RoomState,UserState 가 올바르게 재설정되는지 체크하는 유닛테스트

TEST_F( PvP_unittest, DeleteGameRoom )
{
	CDNPvP* pPvPRoom = new CDNPvP();
	EXPECT_TRUE( pPvPRoom );

	VIMAPVP_CREATEROOM CreateRoom;
	memset( &CreateRoom, 0, sizeof(CreateRoom) );
	
	EXPECT_TRUE( pPvPRoom->bCreate( 0, 0, &CreateRoom, 0, false ) );

	CDNUser* pUser	= g_pDivisionManager->GetUserByAccountDBID( 0 );
	CDNUser* pUser2 = g_pDivisionManager->GetUserByAccountDBID( 1 );
	EXPECT_TRUE( pUser );
	EXPECT_TRUE( pUser2 );

	EXPECT_EQ( pPvPRoom->Join( NULL, pUser, PvPCommon::UserState::Captain ), ERROR_NONE );
	EXPECT_EQ( pPvPRoom->Join( NULL, pUser2, PvPCommon::UserState::Ready ), ERROR_NONE );
	
	// Playing 상태에서 게임룸 파괴
	g_pDivisionManager->m_mPvP.insert( std::make_pair( 0,pPvPRoom) );
	pPvPRoom->SetRoomState( pPvPRoom->GetRoomState()|PvPCommon::RoomState::Playing );
	g_pDivisionManager->SetDestroyPvPGameRoom( 0, 0 );
	EXPECT_EQ( pPvPRoom->GetRoomState(), PvPCommon::RoomState::None );
	EXPECT_EQ( pUser->GetPvPUserState(), PvPCommon::UserState::Captain );
	EXPECT_EQ( pUser2->GetPvPUserState(), PvPCommon::UserState::None );

	// Syncing 상태에서 게임룸 파괴
	pUser2->SetPvPUserState( PvPCommon::UserState::Ready );

	g_pDivisionManager->m_mPvP.insert( std::make_pair( 0,pPvPRoom) );
	pPvPRoom->SetRoomState( pPvPRoom->GetRoomState()|PvPCommon::RoomState::Syncing );
	g_pDivisionManager->SetDestroyPvPGameRoom( 0, 0 );
	EXPECT_EQ( pPvPRoom->GetRoomState(), PvPCommon::RoomState::None );
	EXPECT_EQ( pUser->GetPvPUserState(), PvPCommon::UserState::Captain );
	EXPECT_EQ( pUser2->GetPvPUserState(), PvPCommon::UserState::None );

	// Starting 상태에서 게임룸 파괴
	pUser2->SetPvPUserState( PvPCommon::UserState::Ready );

	g_pDivisionManager->m_mPvP.insert( std::make_pair( 0,pPvPRoom) );
	pPvPRoom->SetRoomState( pPvPRoom->GetRoomState()|PvPCommon::RoomState::Starting );
	g_pDivisionManager->SetDestroyPvPGameRoom( 0, 0 );
	EXPECT_EQ( pPvPRoom->GetRoomState(), PvPCommon::RoomState::None );
	EXPECT_EQ( pUser->GetPvPUserState(), PvPCommon::UserState::Captain );
	EXPECT_EQ( pUser2->GetPvPUserState(), PvPCommon::UserState::None );
}

#endif // #if !defined( _FINAL_BUILD )
