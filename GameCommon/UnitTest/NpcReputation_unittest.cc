
#include "stdafx.h"
#include "DNUserSession.h"
#include "ReputationSystemRepository.h"
#include "DNGameDataManager.h"
#include "NpcReputationProcessor.h"
#include "EtUIXML.h"
#include "DNAuthManager.h"
#include "ReputationSystemEventHandler.h"
#include "TimeSet.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

extern CEtUIXML* g_pUIXML;

class NpcReputation_unittest:public testing::Test
{
protected:

	static void SetUpTestCase()
	{
		// DataManager 생성
		g_pUIXML = new CEtUIXML;
		CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring.xml" );
		g_pUIXML->Initialize( pStream, CEtUIXML::idCategory1 );
		SAFE_DELETE( pStream );

		g_pAuthManager = new CDNAuthManager;
		g_pAuthManager->Init();

		g_pDataManager = new CDNGameDataManager();
		g_pDataManager->LoadItemData();
		g_pDataManager->LoadReputeTableData();
		g_pDataManager->LoadPresentTableData();
	}

	static void TearDownTestCase()
	{
		SAFE_DELETE( g_pDataManager );
		SAFE_DELETE( g_pUIXML );
		SAFE_DELETE( g_pAuthManager );
	}

	virtual void SetUp() 
	{
		m_pSession			= new CDNUserSession();
		m_pReputationSystem	= m_pSession->GetReputationSystem();

		m_pSession->GetItem()->CreateInvenItem1( 335545346, 1, 0, 0, 0, 0 );
	}
	virtual void TearDown()
	{
		delete m_pSession;
	}

	CDNUserSession*					m_pSession;
	CReputationSystemRepository*	m_pReputationSystem;
};

// 실패하는 TC
TEST_F( NpcReputation_unittest, FAIL_TEST )
{
	EXPECT_FALSE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 1 );
}

// 초기화 TC
TEST_F( NpcReputation_unittest, INIT_TEST )
{
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 0 );
}

// 기본 함수 TC
TEST_F( NpcReputation_unittest, FUNCTION_TEST )
{
	m_pReputationSystem->AddNpcReputation(87,IReputationSystem::NpcFavor,23);
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 23 );
	m_pReputationSystem->AddNpcReputation(87,IReputationSystem::NpcFavor,23);
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 46 );
	m_pReputationSystem->SetNpcReputation(87,IReputationSystem::NpcFavor,23);
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 23 );
}

// Processor TC
TEST_F( NpcReputation_unittest, PROCESSOR_TEST )
{
	CNpcReputationProcessor::Process( m_pSession, 87, IReputationSystem::NpcFavor, 20 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 20 );
	EXPECT_TRUE( m_pReputationSystem->m_CommitReputationSystem.size() == 1 );
}

// Present TC
TEST_F( NpcReputation_unittest, PRESENT_TEST )
{
	CNpcReputationProcessor::PresentProcess( m_pSession, 87, 1 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 10 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(45,IReputationSystem::NpcFavor) == 1 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(92,IReputationSystem::NpcFavor) == 1 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(100,IReputationSystem::NpcFavor) == 1 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(88,IReputationSystem::NpcMalice) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(91,IReputationSystem::NpcMalice) == 0 );
	EXPECT_TRUE( m_pReputationSystem->m_CommitReputationSystem.size() == 6 );
}

// Present TC
TEST_F( NpcReputation_unittest, PRESENT_TEST2 )
{
	m_pReputationSystem->SetNpcReputation(87,IReputationSystem::NpcFavor,3 );
	m_pReputationSystem->SetNpcReputation(45,IReputationSystem::NpcFavor,4 );
	m_pReputationSystem->SetNpcReputation(92,IReputationSystem::NpcFavor,5 );
	m_pReputationSystem->SetNpcReputation(100,IReputationSystem::NpcFavor,6 );
	m_pReputationSystem->SetNpcReputation(88,IReputationSystem::NpcMalice,10 );
	m_pReputationSystem->SetNpcReputation(91,IReputationSystem::NpcMalice,12 );

	EXPECT_TRUE( m_pSession->GetItem()->GetInventoryItemCount( 335545346 ) == 1 );

	CNpcReputationProcessor::PresentProcess( m_pSession, 87, 1 );

	EXPECT_TRUE( m_pSession->GetItem()->GetInventoryItemCount( 335545346 ) == 0 );

	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 13 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcMalice) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(45,IReputationSystem::NpcFavor) == 5 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(45,IReputationSystem::NpcMalice) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(92,IReputationSystem::NpcFavor) == 6 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(92,IReputationSystem::NpcMalice) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(100,IReputationSystem::NpcFavor) == 7 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(100,IReputationSystem::NpcMalice) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(88,IReputationSystem::NpcMalice) == 9 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(88,IReputationSystem::NpcFavor) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(91,IReputationSystem::NpcMalice) == 11 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(91,IReputationSystem::NpcFavor) == 0 );
}

// MaliceBomb TC
TEST_F( NpcReputation_unittest, MALICEBOMB_TEST )
{
	m_pReputationSystem->SetNpcReputation(87,IReputationSystem::NpcFavor,3 );
	m_pReputationSystem->SetNpcReputation(87,IReputationSystem::NpcMalice,9000 );
	m_pReputationSystem->SetNpcReputation(45,IReputationSystem::NpcFavor,100 );
	m_pReputationSystem->SetNpcReputation(92,IReputationSystem::NpcFavor,100 );
	m_pReputationSystem->SetNpcReputation(100,IReputationSystem::NpcFavor,100 );
	m_pReputationSystem->SetNpcReputation(88,IReputationSystem::NpcFavor,100 );
	m_pReputationSystem->SetNpcReputation(91,IReputationSystem::NpcFavor,100 );

	m_pReputationSystem->AddNpcReputation(87,IReputationSystem::NpcMalice,10000 );

	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcMalice) == 0 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(45,IReputationSystem::NpcFavor) == 70 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(92,IReputationSystem::NpcFavor) == 70 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(100,IReputationSystem::NpcFavor) == 70 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(88,IReputationSystem::NpcFavor) == 130 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(91,IReputationSystem::NpcFavor) == 130 );

	EXPECT_TRUE( m_pReputationSystem->m_CommitReputationSystem.size() == 6 );
}

TEST_F( NpcReputation_unittest, PACKET_TEST )
{
	TAGetListNpcFavor pA;
	std::vector<TNpcReputation> vReputation;
	for( UINT i=0 ; i<40 ; ++i )
		vReputation.push_back( TNpcReputation(0,0,0) );

	int iAddSendData = 0;
	int iSendCount	 = 0;
	do 
	{
		if( vReputation.size()-iSendCount > _countof(pA.ReputationArr) )
		{
			pA.bLastPage	= false;
			pA.cCount		= _countof(pA.ReputationArr);
		}
		else
		{
			pA.bLastPage	= true;
			pA.cCount		= static_cast<BYTE>(vReputation.size()-iSendCount);
		}

		// 패킷만들기~
		for( UINT i=0 ; i<pA.cCount ; ++i )
			pA.ReputationArr[i] = vReputation[iSendCount+i];

		iSendCount += pA.cCount;

		int iSize = sizeof(pA)-sizeof(pA.ReputationArr)+pA.cCount*sizeof(pA.ReputationArr[0]);
		++iAddSendData;
	} while( iSendCount < static_cast<int>(vReputation.size()) );

	EXPECT_TRUE( iAddSendData == 2 );
}

TEST_F( NpcReputation_unittest, PACKET_TEST2 )
{
	TAGetListNpcFavor pA;
	std::vector<TNpcReputation> vReputation;
	for( UINT i=0 ; i<30 ; ++i )
		vReputation.push_back( TNpcReputation() );

	int iAddSendData = 0;
	int iSendCount	 = 0;
	do 
	{
		if( vReputation.size()-iSendCount > _countof(pA.ReputationArr) )
		{
			pA.bLastPage	= false;
			pA.cCount		= _countof(pA.ReputationArr);
		}
		else
		{
			pA.bLastPage	= true;
			pA.cCount		= static_cast<BYTE>(vReputation.size()-iSendCount);
		}

		// 패킷만들기~
		for( UINT i=0 ; i<pA.cCount ; ++i )
			pA.ReputationArr[i] = vReputation[iSendCount+i];

		iSendCount += pA.cCount;

		int iSize = sizeof(pA)-sizeof(pA.ReputationArr)+pA.cCount*sizeof(pA.ReputationArr[0]);
		++iAddSendData;
	} while( iSendCount < static_cast<int>(vReputation.size()) );

	EXPECT_TRUE( iAddSendData == 1 );
}

TEST_F( NpcReputation_unittest, PACKET_TEST3 )
{
	TAGetListNpcFavor pA;
	std::vector<TNpcReputation> vReputation;
	for( UINT i=0 ; i<92 ; ++i )
		vReputation.push_back( TNpcReputation() );

	int iAddSendData = 0;
	int iSendCount	 = 0;
	do 
	{
		if( vReputation.size()-iSendCount > _countof(pA.ReputationArr) )
		{
			pA.bLastPage	= false;
			pA.cCount		= _countof(pA.ReputationArr);
		}
		else
		{
			pA.bLastPage	= true;
			pA.cCount		= static_cast<BYTE>(vReputation.size()-iSendCount);
		}

		// 패킷만들기~
		for( UINT i=0 ; i<pA.cCount ; ++i )
			pA.ReputationArr[i] = vReputation[iSendCount+i];

		iSendCount += pA.cCount;

		int iSize = sizeof(pA)-sizeof(pA.ReputationArr)+pA.cCount*sizeof(pA.ReputationArr[0]);
		++iAddSendData;
	} while( iSendCount < static_cast<int>(vReputation.size()) );

	EXPECT_TRUE( iAddSendData == 4 );
}

TEST_F( NpcReputation_unittest, SEND_TEST )
{
	std::vector<TNpcReputation> vTest;

	for( UINT i=0 ; i<10 ; ++i )
		vTest.push_back( TNpcReputation() );

	m_pSession->SendReputationList( vTest );

	for( UINT i=0 ; i<20 ; ++i )
		vTest.push_back( TNpcReputation() );

	m_pSession->SendReputationList( vTest );

	for( UINT i=0 ; i<1 ; ++i )
		vTest.push_back( TNpcReputation() );

	m_pSession->SendReputationList( vTest );

	for( UINT i=0 ; i<30 ; ++i )
		vTest.push_back( TNpcReputation() );

	m_pSession->SendReputationList( vTest );
}

TEST_F( NpcReputation_unittest, CONNECTDATE_TEST )
{
	m_pReputationSystem->SetNpcReputation(87,IReputationSystem::NpcFavor,300 );
	m_pReputationSystem->SetNpcReputation(87,IReputationSystem::NpcMalice,9000 );

	CTimeSet timeSet;
	
	__time64_t tt = timeSet.GetTimeT64_LC();
	tt -= ((60*60*24)+(60*60));
	
	m_pReputationSystem->GetEventHandler()->OnConnect(tt,false);

	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcFavor) == 200 );
	EXPECT_TRUE( m_pReputationSystem->GetNpcReputation(87,IReputationSystem::NpcMalice) == 8900 );
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#endif // #if !defined( _FINAL_BUILD )
