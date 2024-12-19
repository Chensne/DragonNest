
#include "stdafx.h"
#include "DNUserSession.h"
#include "DNPvPRoomSortObject.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class PvPWaitUserSort_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		// PvPLevel,Level,Job,이름
		AddUser( 1,11,12,L"김밥" );
		AddUser( 5,3,1,L"김지호" );
		AddUser( 3,55,23,L"1234" );
		AddUser( 10,1,3,L"abcd" );
		AddUser( 1,55,1,L"강명재" );
	}
	virtual void TearDown()
	{
		for( std::list<CDNUserSession*>::iterator itor=m_UserList.begin() ; itor!=m_UserList.end() ; ++itor )
			delete (*itor);
	}

	void AddUser( BYTE cPvPLevel, BYTE cLevel, BYTE cJob, WCHAR* wszName )
	{
		CDNUserSession* pUserSession = new CDNUserSession();
		pUserSession->GetPvPData()->cLevel		= cPvPLevel;
		pUserSession->GetStatusData()->cLevel	= cLevel;
		pUserSession->GetStatusData()->cJob		= cJob;
		_wcscpy( pUserSession->GetStatusData()->wszCharacterName, _countof(pUserSession->GetStatusData()->wszCharacterName), wszName, (int)wcslen(wszName) );
		m_UserList.push_back( pUserSession );
	}

	std::list<CDNUserSession*> m_UserList;
};

// 실패하는 TC
TEST_F( PvPWaitUserSort_unittest, FAIL_OrderByPvPLevelAsc )
{
	m_UserList.sort( PvPUserSort::OrderByPvPLevelAsc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_FALSE( (*itor)->GetPvPData()->cLevel == 3 );

	std::advance( itor, 1 );
	EXPECT_FALSE( (*itor)->GetPvPData()->cLevel == 10 );

	std::advance( itor, 1 );
	EXPECT_FALSE( (*itor)->GetPvPData()->cLevel == 1 );

	std::advance( itor, 1 );
	EXPECT_FALSE( (*itor)->GetPvPData()->cLevel == 1 );

	std::advance( itor, 1 );
	EXPECT_FALSE( (*itor)->GetPvPData()->cLevel == 5 );
}

// PvPLevel
TEST_F( PvPWaitUserSort_unittest, OrderByPvPLevelAsc )
{
	m_UserList.sort( PvPUserSort::OrderByPvPLevelAsc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();
	
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 1 );
	
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 1 );
	
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 3 );
	
	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 5 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 10 );
}

TEST_F( PvPWaitUserSort_unittest, OrderByPvPLevelDesc )
{
	m_UserList.sort( PvPUserSort::OrderByPvPLevelDesc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 10 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 5 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 3 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetPvPData()->cLevel == 1 );
}

// Level
TEST_F( PvPWaitUserSort_unittest, OrderByLevelAsc )
{
	m_UserList.sort( PvPUserSort::OrderByLevelAsc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 3 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 11 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 55 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 55 );
}


TEST_F( PvPWaitUserSort_unittest, OrderByLevelDesc )
{
	m_UserList.sort( PvPUserSort::OrderByLevelDesc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 55 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 55 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 11 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 3 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cLevel == 1 );
}

// Job
TEST_F( PvPWaitUserSort_unittest, OrderByJobAsc )
{
	m_UserList.sort( PvPUserSort::OrderByJobAsc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 3 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 12 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 23 );
}

TEST_F( PvPWaitUserSort_unittest, OrderByJobDesc )
{
	m_UserList.sort( PvPUserSort::OrderByJobDesc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 23 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 12 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 3 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );
}

// 이름
TEST_F( PvPWaitUserSort_unittest, OrderByCharNameAsc )
{
	m_UserList.sort( PvPUserSort::OrderByCharNameAsc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 23 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 3 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 12 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );
}

TEST_F( PvPWaitUserSort_unittest, OrderByCharNameDesc )
{
	m_UserList.sort( PvPUserSort::OrderByCharNameDesc() );

	std::list<CDNUserSession*>::iterator itor = m_UserList.begin();

	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 12 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 1 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 3 );

	std::advance( itor, 1 );
	EXPECT_TRUE( (*itor)->GetStatusData()->cJob == 23 );
}

#endif // #if !defined( _FINAL_BUILD )
