
#include "stdafx.h"
#include "MAAiScript.h"
#include "DNGameDataManager.h"
#include "DNGameRoom.h"
#include "DNBackGroundLoader.h"
#include "DnWorldBrokenProp.h"
#include "DnWorldOperationProp.h"
#include "DnWorldSector.h"
#include "DnTrigger.h"
#include "DnTriggerObject.h"
#include "DnMainFrame.h"
#include "EtActionCoreMng.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

// ���� �̽�	:	����
// �ۼ���		:	���
// �ۼ���		:	2009.12.08
// ����			:	Ʈ���� �����ս� ���� �����׽�Ʈ

class Trigger_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
	}

	virtual void TearDown()
	{
	}

	static void SetUpTestCase()
	{
#if defined( PRE_TRIGGER_TEST )

		s_Log.Init( L"TriggerTest", LOGTYPE_FILE );

		bool bPreLoad = true;
#if defined( _DEBUG )
		bPreLoad = false;
#endif // #if defined( _DEBUG )

		s_pResMng = new CEtResourceMng( false, !bPreLoad );
		//EXPECT_TRUE( s_pResMng->AddResourcePath( "r:\\gameres\\resource\\script\\monsterai", true ) );
		//EXPECT_TRUE( s_pResMng->AddResourcePath( "r:\\gameres\\resource\\ext", true ) );
		//EXPECT_TRUE( s_pResMng->AddResourcePath( "r:\\gameres\\MapData", true ) );
		//EXPECT_TRUE( s_pResMng->AddResourcePath( "r:\\gameres\\MapData\\Resource", true ) );
		//EXPECT_TRUE( s_pResMng->AddResourcePath( "r:\\gameres\\MapData\\Resource\\PropInfo", true ) );
		EXPECT_TRUE( s_pResMng->AddResourcePath( "r:\\gameres\\MapData", true ) );
		EXPECT_TRUE( s_pResMng->AddResourcePath( "r:\\gameres\\Resource", true ) );

		// CDnTableDB ��ü ����
		s_pTableDB = new CDnTableDB();
		EXPECT_TRUE( s_pTableDB->Initialize() );

		// DataManager ����
		g_pDataManager = new CDNGameDataManager();

		// *.lua �ε�
		s_pScriptLoader = new AiScriptLoader();

		std::vector<std::string> FileList;
		FindFileListInDirectory( s_pResMng->GetFullPath( "MonsterAI" ).c_str(), "*.lua", FileList );

		for( UINT i = 0 ; i<FileList.size() ; ++i )
		{
			EXPECT_TRUE( s_pScriptLoader->LoadScript( FileList[i].c_str() ) ) << FileList[i];
		}

		// BackLoader ����
		g_pBackLoader = new CDNBackGroundLoader( false );

		if( bPreLoad )
		{
			s_pMainFrame = new CDnMainFrame;
			CEtResource::Initialize( !bPreLoad );
			CEtActionCoreMng::GetInstance().Initialize( !bPreLoad );
			s_pMainFrame->PreloadResource();
		}

#else // #if defined( PRE_TRIGGER_TEST )

		EXPECT_TRUE( false ) << "PRE_TRIGGER_TEST �������� �Ѿ� ��밡���� �����׽�Ʈ�Դϴ�.";

#endif // #if defined( PRE_TRIGGER_TEST )
	}
	static void TearDownTestCase()
	{
#if defined( PRE_TRIGGER_TEST )
		delete s_pResMng;
		delete s_pTableDB;
		delete s_pScriptLoader;
		delete g_pBackLoader;
		delete s_pMainFrame;
#endif // #if defined( PRE_TRIGGER_TEST )
	}

#if defined( PRE_TRIGGER_TEST )
public:
	static CEtResourceMng*	s_pResMng;
	static AiScriptLoader*	s_pScriptLoader;
	static CDnTableDB*		s_pTableDB;
	static CLog				s_Log;
	static CDnMainFrame*	s_pMainFrame;
#endif // #if defined( PRE_TRIGGER_TEST )
};

#if defined( PRE_TRIGGER_TEST )

CEtResourceMng* Trigger_unittest::s_pResMng			= NULL;
AiScriptLoader* Trigger_unittest::s_pScriptLoader	= NULL;
CDnTableDB*		Trigger_unittest::s_pTableDB		= NULL;
CDnMainFrame*	Trigger_unittest::s_pMainFrame		= NULL;
CLog			Trigger_unittest::s_Log;

#endif

#if defined( PRE_TRIGGER_TEST )

void CDnTestTriggerGameRoom::OnGameStatePlay(ULONG iCurTick)
{
	CDNGameRoom::OnGameStatePlay(iCurTick);

	CDnWorldSector* pSector = static_cast<CDnWorldSector*>(CDnWorld::GetInstance( this ).GetSector( 0.0f, 0.0f ));
	if( !pSector )
		return;

	for( UINT i=0 ; i<pSector->GetPropCount() ; ++i )
	{
		CDnWorldProp* pProp = dynamic_cast<CDnWorldProp*>(pSector->GetPropFromIndex( i ));
		if( pProp )
		{
			if( pProp->GetPropType() == PTE_Static )
				continue;

			// Prop ������ �μ���~!
			CDnWorldBrokenProp* pBrokenProp = dynamic_cast<CDnWorldBrokenProp*>(pProp);
			if( pBrokenProp )
			{
				CDnDamageBase::SHitParam HitParam;
				HitParam.fDurability = FLT_MAX;

				pBrokenProp->CalcDamage( NULL, HitParam );
				continue;
			}
			
			// Prop ������ �۵��ϱ�~!
			CDnWorldOperationProp* pOperationProp = dynamic_cast<CDnWorldOperationProp*>(pProp);
			if( pOperationProp )
			{
				pOperationProp->OnDispatchMessage( NULL, eProp::CS_CMDOPERATION, NULL );
				continue;
			}
		}
	}

	// ������ Npc �� ��ȭ ���� �̺�Ʈ ȣ��
	m_pWorld->OnTriggerEventCallback( "CDnGameTask::OnRecvNpcTalkMsg", 0, 0 );
	// ������ Npc �� ��ȭ ���� �̺�Ʈ ȣ��
	m_pWorld->OnTriggerEventCallback( "CDnGameTask::OnRecvNpcTalkEndMsg", 0, 0 );
	// Ʈ���� ���� �� ���� ����
	if( pSector->GetTrigger()->GetTriggerCount() == 0 )
	{
		if( m_GameState != _GAME_STATE_DESTROYED )
		{
			std::cout << "Ʈ���� ���� - �׽�Ʈ �Ϸ�" << std::endl;
			DestroyGameRoom();
		}
	}
}

void CDnTestTriggerGameRoom::FinishTrigger( CEtTriggerObject* pObject )
{
	double dTime = pObject->m_dTime;

	Trigger_unittest::s_Log.LogA( "[%.3f ms]\t\tCategory:%s\t\tTriggerName:%s\r\n", dTime, pObject->GetCategory(), pObject->GetTriggerName() );
#if defined( _DEBUG )
	std::cout <<  pObject->GetCategory() << " == " << pObject->GetTriggerName() << std::endl;
#endif

	m_vFinish.push_back( std::make_pair(pObject,dTime) );
}

void CDnTestTriggerGameRoom::SetRoomState( eGameRoomState State )
{
	m_GameState = State;
	if( m_GameState == _GAME_STATE_DESTROYED )
	{
		//

		// �Ϸ�
		std::cout << "Ʈ���� �׽�Ʈ �Ϸ�" << std::endl;
	}
}

#endif // #if defined( PRE_TRIGGER_TEST )


// ���� �̽�	:	����
// �ۼ���		:	���
// �ۼ���		:	2009.12.08
// ����			:	01.DataManager �ε�

TEST_F( Trigger_unittest, LoadData )
{
#if defined( PRE_TRIGGER_TEST )
	EXPECT_TRUE( g_pDataManager->LoadMapInfo() );
#else // #if defined( PRE_TRIGGER_TEST )
	EXPECT_TRUE( false ) << "PRE_TRIGGER_TEST �������� �Ѿ� ��밡���� �����׽�Ʈ�Դϴ�.";
#endif #if defined( PRE_TRIGGER_TEST )
}


// ���� �̽�	:	����
// �ۼ���		:	���
// �ۼ���		:	2009.12.08
// ����			:	01.Ʈ���� �����ս� �׽�Ʈ

TEST_F( Trigger_unittest, PerformanceTest )
{
#if defined( PRE_TRIGGER_TEST )

	DNTableFileFormat* pMapSox = s_pTableDB->GetTable( CDnTableDB::TMAP );
	EXPECT_TRUE( pMapSox != NULL );

	std::list<int> listErrorMap;
	//listErrorMap.push_back( 281 );	// Ʈ���ſ��� ������ üũ�ϴµ� ������ ����
	//listErrorMap.push_back( 15038 );	// Ʈ���ſ��� ������ üũ�ϴµ� ������ ����

	int iStartMapIndex = 0;

	for( TMapInfoMap::iterator itor=g_pDataManager->m_pMapInfo.begin() ; itor!=g_pDataManager->m_pMapInfo.end() ; ++itor )
	{
		TMapInfo* pMapInfo = (*itor).second;
		if( pMapInfo->MapType != GlobalEnum::eMapTypeEnum::MAP_DUNGEON )
			continue;
		if( strlen( pMapInfo->szMapName[0] ) == 0 )
			continue;
		if( pMapInfo->nMapID < iStartMapIndex )
			continue;
		if( pMapSox->GetFieldFromLablePtr( pMapInfo->nMapID, "_IncludeBuild" )->GetInteger() == 0 )
			continue;
		if( std::find( listErrorMap.begin(), listErrorMap.end(), pMapInfo->nMapID ) != listErrorMap.end() )
			continue;

		// �����
		MAGAReqRoomID Packet;
		memset( &Packet, 0, sizeof(Packet) );

		Packet.GameTaskType		= GameTaskType::Normal;
		Packet.cReqGameIDType	= REQINFO_TYPE_SINGLE;
		Packet.nMapIndex		= pMapInfo->nMapID;
		Packet.cGateNo			= 1;
		Packet.bDirectConnect	= true;

		std::cout << "===== MapIdx :" << pMapInfo->nMapID << std::endl;
		Trigger_unittest::s_Log.LogA( "#############################################################\r\n" );
		Trigger_unittest::s_Log.LogA( "MapIndex = %d\r\n", pMapInfo->nMapID );
		Trigger_unittest::s_Log.LogA( "#############################################################\r\n" );

		CDNGameRoom* pGameRoom	= new CDnTestTriggerGameRoom( NULL, 0, &Packet );
		do 
		{
			pGameRoom->Process();
		}while( pGameRoom->GetRoomState() != _GAME_STATE_DESTROYED );

		delete pGameRoom;
	}

#else // #if defined( PRE_TRIGGER_TEST )

	EXPECT_TRUE( false ) << "PRE_TRIGGER_TEST �������� �Ѿ� ��밡���� �����׽�Ʈ�Դϴ�.";

#endif // #if defined( PRE_TRIGGER_TEST )
}

#endif // #if !defined( _FINAL_BUILD )
