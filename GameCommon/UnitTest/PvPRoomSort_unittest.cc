
#include "stdafx.h"
#include "DNPvPRoomManager.h"
#include "DNPvPRoom.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class PvPRoomSort_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		CDNPvPRoomManager::CreateInstance();

		MAVIPVP_CREATEROOM CreateRoom;
		memset( &CreateRoom, 0, sizeof(CreateRoom) );
		
		CreateRoom.uiPvPIndex					= 2;
		CreateRoom.sCSPVP_CREATEROOM.cMaxUser	= 1;
		CreateRoom.cGameMode					= PvPCommon::GameMode::PvP_Respawn;
		CreateRoom.sCSPVP_CREATEROOM.uiMapIndex	= 10001;
		CDNPvPRoomManager::GetInstance().CreatePvPRoom( NULL, &CreateRoom );

		CreateRoom.uiPvPIndex					= 4;
		CreateRoom.sCSPVP_CREATEROOM.cMaxUser	= 1;
		CreateRoom.cGameMode					= PvPCommon::GameMode::PvP_Round;
		CreateRoom.sCSPVP_CREATEROOM.uiMapIndex	= 10002;
		CDNPvPRoomManager::GetInstance().CreatePvPRoom( NULL, &CreateRoom );

		CreateRoom.uiPvPIndex					= 1;
		CreateRoom.sCSPVP_CREATEROOM.cMaxUser	= 0;
		CreateRoom.cGameMode					= PvPCommon::GameMode::PvP_Round;
		CreateRoom.sCSPVP_CREATEROOM.uiMapIndex	= 10000;
		CDNPvPRoomManager::GetInstance().CreatePvPRoom( NULL, &CreateRoom );

		CreateRoom.uiPvPIndex					= 3;
		CreateRoom.sCSPVP_CREATEROOM.cMaxUser	= 0;
		CreateRoom.cGameMode					= PvPCommon::GameMode::PvP_Respawn;
		CreateRoom.sCSPVP_CREATEROOM.uiMapIndex	= 10000;
		CDNPvPRoomManager::GetInstance().CreatePvPRoom( NULL, &CreateRoom );
	}
	virtual void TearDown()
	{
		CDNPvPRoomManager::DestroyInstance();
	}
};

TEST_F( PvPRoomSort_unittest, RoomIndexByAsc )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cIsAscend		= 1;
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::Index;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );
	
	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
}

TEST_F( PvPRoomSort_unittest, RoomIndexByDesc )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cIsAscend		= 0;
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::Index;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );

	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
}

TEST_F( PvPRoomSort_unittest, GameMode )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::GameMode;
	SortType.sCSPVP_ROOMLIST.uiSortData		= PvPCommon::GameMode::PvP_Respawn;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );

	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
}

TEST_F( PvPRoomSort_unittest, GameMode2 )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::GameMode;
	SortType.sCSPVP_ROOMLIST.uiSortData		= PvPCommon::GameMode::Max;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );

	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
}

TEST_F( PvPRoomSort_unittest, GameMode3 )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::GameMode;
	SortType.sCSPVP_ROOMLIST.uiSortData		= PvPCommon::GameMode::PvP_Round;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );

	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
}

TEST_F( PvPRoomSort_unittest, Map )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::MapIndex;
	SortType.sCSPVP_ROOMLIST.uiSortData		= 10000;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );

	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
}

TEST_F( PvPRoomSort_unittest, Map2 )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::MapIndex;
	SortType.sCSPVP_ROOMLIST.uiSortData		= 10001;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );

	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
}

TEST_F( PvPRoomSort_unittest, Map3 )
{
	MAVIPVP_ROOMLIST SortType;
	memset( &SortType, 0, sizeof(SortType) );
	SortType.sCSPVP_ROOMLIST.cRoomSortType	= PvPCommon::RoomSortType::MapIndex;
	SortType.sCSPVP_ROOMLIST.uiSortData		= 10002;

	CDNPvPRoomManager::GetInstance().SendPvPRoomList( NULL, &SortType );
	std::list<CDNPvPRoom*> List = CDNPvPRoomManager::GetInstance().GetList( 0 );

	std::list<CDNPvPRoom*>::iterator itor = List.begin();
	//
	EXPECT_TRUE( (*itor)->GetIndex() == 4 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 2 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 1 );
	//
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetIndex() == 3 );
}

#endif // #if !defined( _FINAL_BUILD )
