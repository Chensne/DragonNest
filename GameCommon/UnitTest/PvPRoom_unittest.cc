
#include "stdafx.h"
#include "DNPvPRoom.h"
#include "DNPvPRoomManager.h"
#include "DNUserSession.h"
/*
class MockTurtle : public Turtle {
	...
		MOCK_METHOD0(PenUp, void());
	MOCK_METHOD0(PenDown, void());
	MOCK_METHOD1(Forward, void(int distance));
	MOCK_METHOD1(Turn, void(int degrees));
	MOCK_METHOD2(GoTo, void(int x, int y));
	MOCK_CONST_METHOD0(GetX, int());
	MOCK_CONST_METHOD0(GetY, int());
};

using testing::AtLeast;
using testing::Return;

TEST(PainterTest, CanDrawSomething) {

MockTurtle turtle;
EXPECT_CALL(turtle, PenDown())
.Times(AtLeast(1));

EXPECT_CALL(turtle, GetX())
.WillOnce(Return(100))
.WillOnce(Return(200))
.WillOnce(Return(300));

Painter painter(&turtle);

EXPECT_TRUE(painter.DrawCircle(0, 0, 10));
}
*/

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class MockCreateRoom
{
public:
	MOCK_METHOD0( GetGameMode, BYTE() );
	MOCK_METHOD0( GetPvPIndex, UINT() );
};

class PvPRoom_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		CDNPvPRoomManager::CreateInstance();
		m_pUserObj = new CDNUserSession;

		memset( &m_CreateRoom, 0, sizeof(m_CreateRoom) );

		m_CreateRoom.nRetCode						= ERROR_NONE;
		m_CreateRoom.sCSPVP_CREATEROOM.cRoomNameLen	= 4;
		m_CreateRoom.sCSPVP_CREATEROOM.cRoomPWLen	= 0;
	}
	virtual void TearDown()
	{
		CDNPvPRoomManager::DestroyInstance();
		delete m_pUserObj;
	}

	CDNPvPRoom*			m_pPvPRoom;
	CDNUserSession*		m_pUserObj;
	MAVIPVP_CREATEROOM	m_CreateRoom;
	MockCreateRoom		m_Mock;
};

TEST_F( PvPRoom_unittest, CreateRoom )
{
	EXPECT_CALL( m_Mock, GetGameMode() )
		.WillOnce( testing::Return( PvPCommon::GameMode::PvP_Respawn ) )
		.WillOnce( testing::Return( PvPCommon::GameMode::PvP_Round ) );

	EXPECT_CALL( m_Mock, GetPvPIndex() )
		.WillOnce( testing::Return( 1 ) )
		.WillOnce( testing::Return( 2 ) );

	for( UINT i=0 ; i<2 ; ++i )
	{
		m_CreateRoom.cGameMode	= m_Mock.GetGameMode();
		m_CreateRoom.uiPvPIndex	= m_Mock.GetPvPIndex();

		EXPECT_TRUE( CDNPvPRoomManager::GetInstance().CreatePvPRoom( m_pUserObj, &m_CreateRoom ) == ERROR_NONE );
	}
}

TEST_F( PvPRoom_unittest, RoomListPage )
{
	UINT uiRoomCount, uiMaxPage;

	//
	uiRoomCount	= PvPCommon::Common::RoomPerPage - 1;
	uiMaxPage		= uiRoomCount/PvPCommon::Common::RoomPerPage;
	if( uiMaxPage > 0 && uiRoomCount%PvPCommon::Common::RoomPerPage == 0 )
		--uiMaxPage;
	EXPECT_TRUE( uiMaxPage == 0 );

	//
	uiRoomCount	= PvPCommon::Common::RoomPerPage;
	uiMaxPage	= uiRoomCount/PvPCommon::Common::RoomPerPage;
	if( uiMaxPage > 0 && uiRoomCount%PvPCommon::Common::RoomPerPage == 0 )
		--uiMaxPage;
	EXPECT_TRUE( uiMaxPage == 0 );
	
	//
	uiRoomCount	= PvPCommon::Common::RoomPerPage + 1;
	uiMaxPage		= uiRoomCount/PvPCommon::Common::RoomPerPage;
	if( uiMaxPage > 0 && uiRoomCount%PvPCommon::Common::RoomPerPage == 0 )
		--uiMaxPage;
	EXPECT_TRUE( uiMaxPage == 1 );
}

#endif // #if defined( _FINAL_BUILD )