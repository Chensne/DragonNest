
#include "stdafx.h"
#include "DnActor.h"
#include "DnGameTask.h"
#include "DNUserSession.h"
#include "DnPartyTask.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

// 관련 이슈	:	없음
// 작성자		:	김밥
// 작성일		:	2010.01.08
// 설명			:	stl 컨테이너 allocator 설정 성능 테스트

class Allocator_unittest:public testing::Test
{
protected:

	static void SetUpTestCase()
	{
	}
	static void TearDownTestCase()
	{
	}
};

template< typename T >
void TestFunc( UINT SubCount, bool bEraseCall=true )
{
	{
		CPerformanceLog log( "vector" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			std::vector<T> vRepository;
			for( UINT j=0 ; j<SubCount ; ++j )
				vRepository.push_back( T() );
			
			if( bEraseCall )
			{
				int k=0;
				for( std::vector<T>::iterator itor=vRepository.begin() ; itor!=vRepository.end() ; ++k )
				{
					if( k%2 )
						itor = vRepository.erase( itor );
					else
						++itor;
				}
			}
		}
	}
	{
		CPerformanceLog log( "allocator-vector" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			DNVector(T) vRepository;
			for( UINT j=0 ; j<SubCount ; ++j )
				vRepository.push_back( T() );

			if( bEraseCall )
			{
				int k=0;
				for( DNVector(T)::iterator itor=vRepository.begin() ; itor!=vRepository.end() ; ++k )
				{
					if( k%2 )
						itor = vRepository.erase( itor );
					else
						++itor;
				}
			}
		}
	}

	boost::singleton_pool<T,sizeof(T)>::release_memory();

/*
	{
		CPerformanceLog log( "fastallocator-vector" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			DNFastVector(T) vRepository;
			for( UINT j=0 ; j<SubCount ; ++j )
				vRepository.push_back( T() );

			if( bEraseCall )
			{
				int k=0;
				for( DNFastVector(T)::iterator itor=vRepository.begin() ; itor!=vRepository.end() ; ++k )
				{
					if( k%2 )
						itor = vRepository.erase( itor );
					else
						++itor;
				}
			}
		}
	}

	boost::singleton_pool<T,sizeof(T)>::release_memory();
*/
}

// 관련 이슈	:	없음
// 작성자		:	김밥
// 작성일		:	2010.01.08
// 설명			:	vector 테스트

TEST_F( Allocator_unittest, vector )
{
/*
	std::cout << "CDnGameTask::AreaStruct" << std::endl;
	TestFunc<CDnGameTask::AreaStruct>( 30 );
	TestFunc<CDnGameTask::AreaStruct>( 30, false );
*/

/*
	std::cout << "CDnGameTask::GenerationMonsterStruct" << std::endl;
	TestFunc<CDnGameTask::GenerationMonsterStruct>( 30 );
	TestFunc<CDnGameTask::GenerationMonsterStruct>( 30, false );
*/

/*
	std::cout << "CDnGameTask::MonsterTableStruct" << std::endl;
	TestFunc<CDnGameTask::MonsterTableStruct>( 30 );
	TestFunc<CDnGameTask::MonsterTableStruct>( 30, false );
*/
	
/*
	std::cout << "CDnGameTask::MonsterCompoundStruct" << std::endl;
	TestFunc<CDnGameTask::MonsterCompoundStruct>( 30 );
	TestFunc<CDnGameTask::MonsterCompoundStruct>( 30, false );
*/

/*
	std::cout << "CDnGameTask::MonsterSetStruct" << std::endl;
	TestFunc<CDnGameTask::MonsterSetStruct>( 30 );
	TestFunc<CDnGameTask::MonsterSetStruct>( 30, false );
*/

/*
	std::cout << "CDnGameTask::MonsterGroupStruct" << std::endl;
	TestFunc<CDnGameTask::MonsterGroupStruct>( 30 );
	TestFunc<CDnGameTask::MonsterGroupStruct>( 30, false );		
*/

/*
	struct DropTempStruct {
		bool bIsGroup;
		int nItemID;
		int nCount;
		int nOffset;
	};

	std::cout << "DropTempStruct" << std::endl;
	TestFunc<DropTempStruct>( 30 );
	TestFunc<DropTempStruct>( 30, false );		
*/

/*
	std::cout << "CDnGameTask::MonsterCompoundStruct" << std::endl;
	TestFunc<CDnGameTask::MonsterCompoundStruct>( 30 );
	TestFunc<CDnGameTask::MonsterCompoundStruct>( 30, false );	
*/

/*
	std::cout << "CDnGameTask::MonsterSetProbStruct" << std::endl;
	TestFunc<CDnGameTask::MonsterSetProbStruct>( 30 );
	TestFunc<CDnGameTask::MonsterSetProbStruct>( 30, false );	
*/

/*
	std::cout << "CDnGameTask::TreasureBoxLevelStruct" << std::endl;
	TestFunc<CDnGameTask::TreasureBoxLevelStruct>( 30 );
	TestFunc<CDnGameTask::TreasureBoxLevelStruct>( 30, false );	
*/

/*
	std::cout << "CDnItem::DropItemStruct" << std::endl;
	TestFunc<CDnItem::DropItemStruct>( 30 );
	TestFunc<CDnItem::DropItemStruct>( 30, false );	
*/

/*
	std::cout << "DnBlowHandle" << std::endl;
	TestFunc<DnBlowHandle>( 30 );
	TestFunc<DnBlowHandle>( 30, false );	
*/

/*
	std::cout << "_STORE_PACKET" << std::endl;
	TestFunc<_STORE_PACKET>( 30 );
	TestFunc<_STORE_PACKET>( 30, false );	
*/

/*
	std::cout << "std::string" << std::endl;
	TestFunc<std::string>( 30 );
	TestFunc<std::string>( 30, false );	
*/

/*
	std::cout << "std::pair<CDNTcpConnection*, CDNUserSession*>" << std::endl;
	TestFunc<std::pair<CDNTcpConnection*, CDNUserSession*>>( 30 );
	TestFunc<std::pair<CDNTcpConnection*, CDNUserSession*>>( 30, false );	
*/

/*
	std::cout << "int" << std::endl;
	TestFunc<int>( 30 );
	TestFunc<int>( 30, false );	
*/

/*
	std::cout << "void*" << std::endl;
	TestFunc<void*>( 30 );
	TestFunc<void*>( 30, false );	
*/

/*
	std::cout << "std::pair<int,int>" << std::endl;
	TestFunc<std::pair<int,int>>( 30 );
	TestFunc<std::pair<int,int>>( 30, false );	
*/

/*
	std::cout << "_KICKMEMBER" << std::endl;
	TestFunc<_KICKMEMBER>( 30 );
	TestFunc<_KICKMEMBER>( 30, false );	
*/

/*
	std::cout << "EtVector2" << std::endl;
	TestFunc<EtVector2>( 30 );
	TestFunc<EtVector2>( 30, false );	

	std::cout << "EtVector3" << std::endl;
	TestFunc<EtVector3>( 30 );
	TestFunc<EtVector3>( 30, false );	

	std::cout << "EtVector4" << std::endl;
	TestFunc<EtVector4>( 30 );
	TestFunc<EtVector4>( 30, false );	
*/
	std::cout << "DnSkillHandle" << std::endl;
	TestFunc<DnSkillHandle>( 30 );
	TestFunc<DnSkillHandle>( 30, false );	
}

// 관련 이슈	:	없음
// 작성자		:	김밥
// 작성일		:	2010.01.08
// 설명			:	list 테스트

TEST_F( Allocator_unittest, list )
{
	struct Data
	{
		char buf[256];
	};

	{
		CPerformanceLog log( "list" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			std::list<Data> vRepository;
			for( UINT j=0 ; j<50 ; ++j )
				vRepository.push_back( Data() );
			int k=0;
			for( std::list<Data>::iterator itor=vRepository.begin() ; itor!=vRepository.end() ; ++k )
			{
				if( k%2 )
					itor = vRepository.erase( itor );
				else
					++itor;
			}
		}
	}
	{
		CPerformanceLog log( "allocator-list" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			DNList(Data) vRepository;
			for( UINT j=0 ; j<50 ; ++j )
				vRepository.push_back( Data() );
			int k=0;
			for( DNList(Data)::iterator itor=vRepository.begin() ; itor!=vRepository.end() ; ++k )
			{
				if( k%2 )
					itor = vRepository.erase( itor );
				else
					++itor;
			}
		}
	}

	boost::singleton_pool<Data,sizeof(Data)>::release_memory();

	{
		CPerformanceLog log( "fastallocator-list" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			DNFastList(Data) vRepository;
			for( UINT j=0 ; j<50 ; ++j )
				vRepository.push_back( Data() );
			int k=0;
			for( DNFastList(Data)::iterator itor=vRepository.begin() ; itor!=vRepository.end() ; ++k )
			{
				if( k%2 )
					itor = vRepository.erase( itor );
				else
					++itor;
			}
		}
	}

	boost::singleton_pool<Data,sizeof(Data)>::release_memory();

	// 기본타입
	{
		CPerformanceLog log( "list" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			std::list<int> vRepository;
			for( UINT j=0 ; j<300 ; ++j )
				vRepository.push_back( j );
		}
	}
	{
		CPerformanceLog log( "allocator-list" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			DNList(int) vRepository;
			for( UINT j=0 ; j<300 ; ++j )
				vRepository.push_back( j );
		}
	}

	boost::singleton_pool<int,sizeof(int)>::release_memory();

	{
		CPerformanceLog log( "fastallocator-list" );
		for( UINT i=0 ; i<1000 ; ++i )
		{
			DNFastList(int) vRepository;
			for( UINT j=0 ; j<300 ; ++j )
				vRepository.push_back( j );
		}
	}

	boost::singleton_pool<int,sizeof(int)>::release_memory();
}

#endif // #if !defined( _FINAL_BUILD )
