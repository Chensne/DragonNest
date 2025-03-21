
#include "stdafx.h"
#include "DNQueryTest.h"
#include "DNConnection.h"
#include <iostream>
#include "DNSQLWorld.h"
#include "DNSQLConnectionManager.h"
#include "DNIocpManager.h"
#include "Util.h"
#include "DNMessageTask.h"

// 김밥ServerID : 11
// 홍철ServerID : 18
// 하링ServerID : 19
#define TEST_WORLD	11

#if defined( PRE_PARTY_DB )
extern TDBConfig g_Config;
#endif


CDNQueryTest::CDNQueryTest()
{

}

CDNQueryTest::~CDNQueryTest()
{

}

void CDNQueryTest::ProcessQueryTest( const char* pszCmd )
{
	std::string					strString(pszCmd);
	std::vector<std::string>	vSplit;
	boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(", ") );

	if( !vSplit.empty() )
	{
		// QUERY_SELECTCHARACTER
		if( stricmp( vSplit[0].c_str(), "selectchar" ) == 0 )
		{
			_QUERY_SELECTCHARACTER( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "getcharacter7" ) == 0 )
		{
			QueryGetCharacterPartialy7( vSplit );
		}
		// QUERY_UPDATE_DARKLAIR_RESULT
		else if( stricmp( vSplit[0].c_str(), "DL" ) == 0 )
		{
			_QUERY_UPDATE_DARKLAIR_RESULT( vSplit );
		}
		// P_GetKeySettingOption
		else if( stricmp( vSplit[0].c_str(), "GetKey" ) == 0 )
		{
			_P_GetKeySettingOption( vSplit );
		}
		// P_ModKeySettionOption
		else if( stricmp( vSplit[0].c_str(), "ModKey" ) == 0 )
		{
			_P_ModKeySettionOption( vSplit );
		}
		// P_GetListEquipment
		else if( stricmp( vSplit[0].c_str(), "P_GetListEquipment" ) == 0 )
		{
			_P_GetListEquipment( vSplit );
		}
		// P_DelBlockedCharacter
		else if( stricmp( vSplit[0].c_str(), "P_DelBlockedCharacter" ) == 0 )
		{
			_P_DelBlockedCharacter( vSplit );
		}
		// P_GetListBlockedCharacter
		else if( stricmp( vSplit[0].c_str(), "P_GetListBlockedCharacter" ) == 0 )
		{
			_P_GetListBlockedCharacter( vSplit );
		}
		// P_AddBlockedCharacter
		else if( stricmp( vSplit[0].c_str(), "P_AddBlockedCharacter" ) == 0 )
		{
			_P_AddBlockedCharacter( vSplit );
		}
		// P_DBMWTest
		else if( stricmp( vSplit[0].c_str(), "P_DBMWTest" ) == 0 )
		{
			_P_DBMWTest( vSplit );
		}
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		else if( stricmp( vSplit[0].c_str(), "ModNpcLocation" ) == 0 )
		{
			QueryModNpcLocation( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "ModReputation" ) == 0 )
		{
			QueryModNpcFavor( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "GetListReputation" ) == 0 )
		{
			QueryGetListNpcFavor( vSplit );
		}
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		else if( stricmp( vSplit[0].c_str(), "AddMaster" ) == 0 )
		{
			QueryAddMasterCharacter( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "DelMaster" ) == 0 )
		{
			QueryDelMasterCharacter( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "GetMasterList" ) == 0 )
		{
			QueryGetPageMasterCharacter( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "GetMasterPupilInfo" ) == 0 )
		{
			QueryGetMasterPupilInfo( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "AddMasterAndPupil" ) == 0 )
		{
			QueryAddMasterAndPupil( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "DelMasterAndPupil" ) == 0 )
		{
			QueryDelMasterAndPupil( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "GetMasterCharacter" ) == 0 )
		{
			QueryGetMasterCharacter( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "GetListPupil" ) == 0 )
		{
			QueryGetListPupil( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "GetMyMasterAndClassmate" ) == 0 )
		{
			QueryGetListMyMasterAndClassmate( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "GetMyClassmate" ) == 0 )
		{
			QueryGetMyClassmate( vSplit );
		}
		else if( stricmp( vSplit[0].c_str(), "Graduate" ) == 0 )
			QueryGraduate( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModRespectPoint" ) == 0 )
			QueryModRespectPoint( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModMasterFavorPoint" ) == 0 )
			QueryModMasterFavorPoint( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetMasterAndFavorPoint" ) == 0 )
			QueryGetMasterAndFavorPoint( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetMasterSystemCountInfo" ) == 0 )
			QueryGetMasterSystemCountInfo( vSplit );

#if defined( PRE_ADD_SECONDARY_SKILL )
		else if( stricmp( vSplit[0].c_str(), "AddSecondarySkill" ) == 0 )
			QueryAddSecondarySkill( vSplit );
		else if( stricmp( vSplit[0].c_str(), "DelSecondarySkill" ) == 0 )
			QueryDelSecondarySkill( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListSecondarySkill" ) == 0 )
			QueryGetListSecondarySkill( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModSecondarySkillExp" ) == 0 )
			QueryModSecondarySkillExp( vSplit );
		else if( stricmp( vSplit[0].c_str(), "SetManufactureSkillRecipe" ) == 0 )
			QuerySetManufactureSkillRecipe( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ExtractManufactureSkillRecipe" ) == 0 )
			QueryExtractManufactureSkillRecipe( vSplit );
		else if( stricmp( vSplit[0].c_str(), "DelManufactureSkillRecipe" ) == 0 )
			QueryDelManufactureSkillRecipe( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListManufactureSkillRecipe" ) == 0 )
			QueryGetListManufactureSkillRecipe( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModManufactureSkillRecipe" ) == 0 )
			QueryModManufactureSkillRecipe( vSplit );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

		else if( stricmp( vSplit[0].c_str(), "GetGuild" ) == 0 )
			QueryGetGuild( vSplit );
		else if( stricmp( vSplit[0].c_str(), "CloseGuild" ) == 0 )
			QueryDismissGuild( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetGuildHis" ) == 0 )
			QueryGetGuildHistoryList( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetGuildWareHis" ) == 0 )
			QueryGetGuildWareHistoryList( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetGuildWarSchedule" ) == 0 )
			QueryGetGuildWarSchedule( vSplit );
		else if( stricmp( vSplit[0].c_str(), "EnrollGuildWar" ) == 0 )
			QueryEnrollGuildWar( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetGuildWareInfo" ) == 0 )
			QueryGetGuildWareInfo(vSplit);
		else if( stricmp( vSplit[0].c_str(), "GuildCheat" ) == 0 )
			QueryModGuildData(vSplit);
		else if( stricmp( vSplit[0].c_str(), "GetListFarm" ) == 0 )
			QueryGetListFarm( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListField" ) == 0 )
			QueryGetListField( vSplit );
		else if( stricmp( vSplit[0].c_str(), "AddField" ) == 0 )
			QueryAddField( vSplit );
		else if( stricmp( vSplit[0].c_str(), "DelField" ) == 0 )
			QueryDelField( vSplit );
		else if( stricmp( vSplit[0].c_str(), "AddFieldAttachment" ) == 0 )
			QueryAddFieldAttachment( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModFieldElapsedTime" ) == 0 )
			QueryModFieldElapsedTime( vSplit );
		else if( stricmp( vSplit[0].c_str(), "Harvest" ) == 0 )
			QueryHarvest( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListHarvestDepotItem" ) == 0 )
			QueryGetListHarvestDepotItem( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetFieldCountByCharacter" ) == 0 )
			QueryGetFieldCountByCharacter( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetFieldItemCount" ) == 0 )
			QueryGetFieldItemCount( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListFieldForCharacter" ) == 0 )
			QueryGetListFieldForCharacter( vSplit );
		else if( stricmp( vSplit[0].c_str(), "AddFieldForCharacter" ) == 0 )
			QueryAddFieldForCharacter( vSplit );
		else if( stricmp( vSplit[0].c_str(), "DelFieldForCharacter" ) == 0 )
			QueryDelFieldForCharacter( vSplit );
		else if( stricmp( vSplit[0].c_str(), "AddFieldForCharacterAttachment" ) == 0 )
			QueryAddFieldForCharacterAttachment( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModFieldForCharacterElapsedTime" ) == 0 )
			QueryModFieldForCharacterElapsedTime( vSplit );
		else if( stricmp( vSplit[0].c_str(), "HarvestForCharacter" ) == 0 )
			QueryHarvestForCharacter( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ChangeCharacterName" ) == 0 )
			QueryChangeCharacterName( vSplit );
		else if( stricmp( vSplit[0].c_str(), "AddPvPLadderResult" ) == 0 )
			QueryAddPvPLadderResult( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListPvPLadderScore" ) == 0 )
			QueryGetListPvPLadderScore( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListPvPLadderScoreByJob" ) == 0 )
			QueryGetListPvPLadderScoreByJob( vSplit );
		else if( stricmp( vSplit[0].c_str(), "InitPvPLadderGradePoint" ) == 0 )
			QueryInitPvPLadderGradePoint( vSplit );
		else if( stricmp( vSplit[0].c_str(), "UsePvPLadderPoint" ) == 0 )
			QueryUsePvPLadderPoint( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListPvPLadderRanking" ) == 0 )
			QueryGetListPvPLadderRanking( vSplit );
#ifdef PRE_ADD_CHANGEJOB_CASHITEM
		else if( stricmp( vSplit[0].c_str(), "ChangeJobCode" ) == 0 )
			QueryChangeJobCode( vSplit );
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM
		else if( stricmp( vSplit[0].c_str(), "CompleteEventQuest" ) == 0 )
			QueryCompleteEventQuest( vSplit );
		else if( stricmp( vSplit[0].c_str(), "CheckPCRoomIP" ) == 0 )
			QueryCheckPCRoomIP( vSplit );
		else if( stricmp( vSplit[0].c_str(), "AddAbuseMonitor" ) == 0 )
			QueryAddAbuseMonitor( vSplit );
		else if( stricmp( vSplit[0].c_str(), "DelAbuseMonitor" ) == 0 )
			QueryDelAbuseMonitor( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetAbuseMonitor" ) == 0 )
			QueryGetAbuseMonitor( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListVariable" ) == 0 )
			QueryGetListVariableReset( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModVariable" ) == 0 )
			QueryModVariableReset( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListRepurchaseItem" ) == 0 )
			QueryGetListRepurchaseItem( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModPvPScore" ) == 0 )
			QueryModPvPScore( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetPvPScore" ) == 0 )
			QueryGetPvPScore( vSplit );
		else if( stricmp( vSplit[0].c_str(), "AddPvPResultLog" ) == 0 )
			QueryAddPvPResultLog( vSplit );
#if defined( PRE_PARTY_DB )
		else if( stricmp( vSplit[0].c_str(), "AddParty" ) == 0 )
			QueryAddParty( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModParty" ) == 0 )
			QueryModParty( vSplit );
		else if( stricmp( vSplit[0].c_str(), "DelParty" ) == 0 )
			QueryDelParty( vSplit );
		else if( stricmp( vSplit[0].c_str(), "DelPartyForServer" ) == 0 )
			QueryDelPartyForServer( vSplit );
		else if( stricmp( vSplit[0].c_str(), "JoinParty" ) == 0 )
			QueryJoinParty( vSplit );
		else if( stricmp( vSplit[0].c_str(), "OutParty" ) == 0 )
			QueryOutParty( vSplit );
		else if( stricmp( vSplit[0].c_str(), "GetListParty" ) == 0 )
			QueryGetListParty( vSplit );
#if defined( PRE_WORLDCOMBINE_PARTY )
		else if( stricmp( vSplit[0].c_str(), "GetListPartyCode" ) == 0 )
			QueryGetListPartyCode( vSplit );
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
		else if( stricmp( vSplit[0].c_str(), "GetListPartyMember" ) == 0 )
			QueryGetListPartyMember( vSplit );
		else if( stricmp( vSplit[0].c_str(), "ModPartyLeader" ) == 0 )
			QueryModPartyLeader( vSplit );
#endif // #if defined( PRE_PARTY_DB )
		else if( stricmp( vSplit[0].c_str(), "TestTimeParam" ) == 0 )
			TestTimeParamSet( vSplit );
#if defined (PRE_ADD_BESTFRIEND)
		else if( stricmp( vSplit[0].c_str(), "GetBestFriend" ) == 0 )
			QueryGetBestFriend( vSplit );
		else if( stricmp( vSplit[0].c_str(), "RegistBestFriend" ) == 0 )
			QueryRegistBestFriend( vSplit );
		else if( stricmp( vSplit[0].c_str(), "CancelBestFriend" ) == 0 )
			QueryCancelBestFriend( vSplit );
		else if( stricmp( vSplit[0].c_str(), "CloseBestFriend" ) == 0 )
			QueryCloseBestFriend( vSplit );
		else if( stricmp( vSplit[0].c_str(), "EditBestFriendMemo" ) == 0 )
			QueryEditBestFriendMemo( vSplit );
#endif // #if defined (PRE_ADD_BESTFRIEND)
		else if( stricmp( vSplit[0].c_str(), "GetListMyTrade" ) == 0 )
			QueryGetListMyTrade( vSplit );
#if defined( PRE_ADD_LIMITED_SHOP )
		else if( stricmp( vSplit[0].c_str(), "GetLimitedItem" ) == 0 )
			QueryGetLimiatedItem( vSplit );		
		else if( stricmp( vSplit[0].c_str(), "AddLimitedItem" ) == 0 )
			QueryAddLimiatedItem( vSplit );		
		else if( stricmp( vSplit[0].c_str(), "ResetLimitedItem" ) == 0 )
			QueryResetLimiatedItem( vSplit );		
#endif	
		
		//=============================================================================
		// 여기 위에 작성해주세요.
		//=============================================================================

#if defined( _WORK )
		else if( stricmp( vSplit[0].c_str(), "PerformanceTest" ) == 0 )
			QueryPerformanceTest( vSplit );
#endif // #if defined( _WORK )
	}
}

void CDNQueryTest::_QUERY_SELECTCHARACTER( std::vector<std::string>& vSplit )
{
	if( vSplit.size() < 2 )
		return;

	std::auto_ptr<CDNConnection> pConnection(new CDNConnection());

	TQSelectCharacter Data;
	Data.cWorldSetID			= TEST_WORLD;
	Data.nAccountDBID		= 3080;
	Data.biCharacterDBID	= boost::lexical_cast<INT64>(vSplit[1].c_str());

	CDNMessageTask* pTask = pConnection->GetMessageTask(MAINCMD_STATUS);
	if (pTask)
		pTask->OnRecvMessage(0, 0, QUERY_SELECTCHARACTER, reinterpret_cast<char*>(&Data));
}

void CDNQueryTest::_QUERY_UPDATE_DARKLAIR_RESULT( std::vector<std::string>& vSplit )
{
	if( vSplit.size() < 2 )
		return;

	std::auto_ptr<CDNConnection> pConnection(new CDNConnection());

	TQUpdateDarkLairResult Data;
	Data.cWorldSetID			= TEST_WORLD;
	Data.iMapIndex			= 20;
	Data.unPlayRound		= 4;
	Data.uiPlaySec			= 2311;
	Data.cPartyUserCount	= boost::lexical_cast<int>(vSplit[1]);
	wsprintf( Data.wszPartyName, L"QueryTest" );
	Data.sUserData[0].bUpdate			= true;
	Data.sUserData[0].i64CharacterDBID	= 2443;
	Data.sUserData[0].unLevel			= 23;
	Data.sUserData[0].iJobIndex			= 0;
	wsprintf( Data.sUserData[0].wszCharacterName, L"빵꾸똥꾸" );
	if( Data.cPartyUserCount > 1 )
	{
		Data.sUserData[1].bUpdate			= true;
		Data.sUserData[1].i64CharacterDBID	= 2444;
		Data.sUserData[1].unLevel			= 11;
		Data.sUserData[1].iJobIndex			= 1;
		wsprintf( Data.sUserData[0].wszCharacterName, L"빵꾸똥꾸2" );
	}
	if( Data.cPartyUserCount > 2 )
	{
		Data.sUserData[2].bUpdate			= true;
		Data.sUserData[2].i64CharacterDBID	= 2445;
		Data.sUserData[2].unLevel			= 12;
		Data.sUserData[2].iJobIndex			= 3;
		wsprintf( Data.sUserData[0].wszCharacterName, L"빵꾸똥꾸3" );
	}
	if( Data.cPartyUserCount > 3 )
	{
		Data.sUserData[3].bUpdate			= true;
		Data.sUserData[3].i64CharacterDBID	= 2446;
		Data.sUserData[3].unLevel			= 22;
		Data.sUserData[3].iJobIndex			= 2;
		wsprintf( Data.sUserData[0].wszCharacterName, L"빵꾸똥꾸4" );
	}

	CDNMessageTask* pTask = pConnection->GetMessageTask(MAINCMD_DARKLAIR);
	if (pTask)
		pTask->OnRecvMessage( 0, 0, QUERY_UPDATE_DARKLAIR_RESULT, reinterpret_cast<char*>(&Data) );
}

void CDNQueryTest::_P_GetKeySettingOption( std::vector<std::string>& vSplit )
{
	std::auto_ptr<CDNConnection> pConnection(new CDNConnection());
	
	TQGetKeySettingOption Data;
	Data.cWorldSetID		= TEST_WORLD;
	Data.nAccountDBID	= 3080;

	CDNMessageTask* pTask = pConnection->GetMessageTask(MAINCMD_ETC);
	if (pTask)
		pTask->OnRecvMessage(0, 0, QUERY_GET_KEYSETTING_OPTION, reinterpret_cast<char*>(&Data));
}

void CDNQueryTest::_P_ModKeySettionOption( std::vector<std::string>& vSplit )
{
	std::auto_ptr<CDNConnection> pConnection(new CDNConnection());

	TQModKeySettingOption Data;
	Data.cWorldSetID		= TEST_WORLD;
	Data.nAccountDBID	= 3080;
	Data.bUseDefault	=( vSplit.size() == 1 ) ? true : false;
	if( !Data.bUseDefault )
	{
		memset( &Data.sKeySetting, boost::lexical_cast<int>(vSplit[1]), sizeof(Data.sKeySetting) );
	}

	CDNMessageTask* pTask = pConnection->GetMessageTask(MAINCMD_ETC);
	if (pTask)
		pTask->OnRecvMessage(0, 0, QUERY_MOD_KEYSETTING_OPTION, reinterpret_cast<char*>(&Data));
}

void CDNQueryTest::_P_GetListEquipment( std::vector<std::string>& vSplit )
{
	//CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );

	// std::cout << "Ret:" << pWorld->QueryGetListEquipment( 0 ) << std::endl;
}

void CDNQueryTest::_P_DelBlockedCharacter( std::vector<std::string>& vSplit )
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	if (!pWorld)
		return;

	INT64 biTemp[2] = { 123,5423 };

	std::cout << "Ret:" << pWorld->QueryDelBlockedCharacter( 0, _countof(biTemp), biTemp ) << std::endl;
}

void CDNQueryTest::_P_GetListBlockedCharacter( std::vector<std::string>& vSplit )
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	if (!pWorld)
		return;

	BYTE cCount = 0;
	TIsolateItem Isolate[ISOLATELISTMAX];

	std::cout << "Ret:" << pWorld->QueryGetListBlockedCharacter( 0, cCount, Isolate ) << std::endl;
}

void CDNQueryTest::_P_AddBlockedCharacter( std::vector<std::string>& vSplit )
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	if (!pWorld)
		return;

	INT64 biRet = 0;
	bool bNeedDel = false;

	std::cout << "Ret:" << pWorld->QueryAddBlockedCharacter( 0, L"빵꾸똥꾸", biRet, bNeedDel, ISOLATELISTMAX ) << std::endl;
}

void CDNQueryTest::_P_DBMWTest( std::vector<std::string>& vSplit )
{
	for( UINT i=0 ; i<1000 ; ++i )
	{
		CDNConnection* pCon = new CDNConnection();
		CSocketContext* pSocketContext = new CSocketContext;
		pSocketContext->SetParam( pCon );
	
		DNTPacket TxPacket;
		memset (&TxPacket, 0x00, sizeof(TxPacket));
		TxPacket.iLen		= 4+sizeof(TQMWTest);
		TxPacket.cMainCmd	= MAINCMD_ITEM;
		TxPacket.cSubCmd	= QUERY_MWTEST;
	
		TQMWTest data;
		memset (&data, 0x00, sizeof(data));
		data.cThreadID = i%8;
		data.cWorldSetID = TEST_WORLD;

		memcpy( pSocketContext->m_RecvIO.buffer,   &TxPacket, 4 );
		memcpy( pSocketContext->m_RecvIO.buffer+4, &data,	  sizeof(data) );
		pSocketContext->m_RecvIO.Len +=(4+sizeof(data));
	
		pSocketContext->m_dwKeyParam = CONNECTIONKEY_DEFAULT;
		g_pIocpManager->OnReceive( pSocketContext, 0 );

		pCon->SetDelete( true );
	}

	//delete pSocketContext;
}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDNQueryTest::QueryModNpcLocation( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};	
	
	printf( "NpcID>>" );
	cgets(szCmd);
	int iNpcID = atoi(szCmd);

	printf( "MapID>>" );
	cgets(szCmd);
	int iMapID = atoi(szCmd);

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );
	
	TQModNPCLocation pQ;
	pQ.iNPCID = iNpcID;
	pQ.iMapID = iMapID;

	int iRet = pWorld->QueryModNpcLocation( &pQ );
	
	if( iRet == ERROR_NONE )
		std::cout<< "QueryModNpcLocation 성공\r\n";
	else
		std::cout<< "QueryModNpcLocation 실패 Err:" << iRet << std::endl;
}

void CDNQueryTest::QueryModNpcFavor( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );
	
	TQModNPCFavor pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "NpcCount>>" );
	cgets(szCmd);
	pQ.cCount = atoi(szCmd);

	for( UINT i=0 ; i<pQ.cCount ; ++i )
	{
		printf( "NpcID>>" );
		cgets(szCmd);
		pQ.UpdateArr[i].iNpcID = atoi(szCmd);

		printf( "FavorPoint>>" );
		cgets(szCmd);
		pQ.UpdateArr[i].iFavorPoint = atoi(szCmd);

		printf( "MalicePoint>>" );
		cgets(szCmd);
		pQ.UpdateArr[i].iMalicePoint = atoi(szCmd);
	}

	int iRet = pWorld->QueryModNpcFavor( &pQ );
	if( iRet == ERROR_NONE )
		std::cout<< "QueryModNpcFavor 성공\r\n";
	else
		std::cout<< "QueryModNpcFavor 실패 Err:" << iRet << std::endl;
}

void CDNQueryTest::QueryGetListNpcFavor( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListNpcFavor pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "MapID>>" );
	cgets(szCmd);
	pQ.iMapID = atoi(szCmd);

	std::vector<TNpcReputation> vReputation;
	int iRet = pWorld->QueryGetListNpcFavor( &pQ, vReputation );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModNpcFavor 성공\r\n";

		for( UINT i=0 ; i<vReputation.size() ; ++i )
		{
			std::cout << "NpcID=" << vReputation[i].iNpcID << " FavorPoint=" << vReputation[i].iFavorPoint << " MalicePoint=" << vReputation[i].iMalicePoint << std::endl;
		}
	}
	else
		std::cout<< "QueryModNpcFavor 실패 Err:" << iRet << std::endl;
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDNQueryTest::QueryGetMasterPupilInfo( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetMasterPupilInfo pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	TAGetMasterPupilInfo pA;
	int iRet = pWorld->QueryGetMasterPupilInfo2( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetMasterPupilInfo 성공\r\n";

		std::cout << pA.SimpleInfo.iMasterCount << ":" << pA.SimpleInfo.iPupilCount << ":" << pA.SimpleInfo.iGraduateCount <<":" << pA.SimpleInfo.BlockDate << std::endl;
		for( UINT i=0 ; i<pA.SimpleInfo.cCharacterDBIDCount ; ++i )
		{
			std::cout << "CharDBID:" << pA.SimpleInfo.OppositeInfo[i].CharacterDBID << std::endl;
			std::wcout << L"CharName:" << pA.SimpleInfo.OppositeInfo[i].wszCharName << std::endl;
		}
	}
	else
	{
		std::cout<< "QueryGetMasterPupilInfo 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryAddMasterCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddMasterCharacter pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "SelfIntroduction>>" );
	cgets(szCmd);
	MultiByteToWideChar( CP_ACP, 0, szCmd, -1, pQ.wszSelfIntroduction, MasterSystem::Max::SelfIntrotuctionLen );

	int iRet = pWorld->QueryAddMasterCharacter( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryAddMasterCharacter 성공\r\n";
	}
	else
	{
		std::cout<< "QueryAddMasterCharacter 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryDelMasterCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelMasterCharacter pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryDelMasterCharacter( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryDelMasterCharacter 성공\r\n";
	}
	else
	{
		std::cout<< "QueryDelMasterCharacter 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetPageMasterCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetPageMasterCharacter pQ;

	printf( "Page>>" );
	cgets(szCmd);
	pQ.uiPage = atoi(szCmd);

	printf( "JobCode>>" );
	cgets(szCmd);
	pQ.cJobCode = atoi(szCmd);

	printf( "GenderCode>>" );
	cgets(szCmd);
	pQ.cGenderCode = atoi(szCmd);

	TAGetPageMasterCharacter pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGetPageMasterCharacter( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetPageMasterCharacter 성공\r\n";

		for( UINT i=0 ; i<pA.cCount ; ++i )
		{
			std::wcout	<< pA.MasterInfoList[i].wszCharName << L" 소개:" << pA.MasterInfoList[i].wszSelfIntrodution << L" Job:"<<(int)pA.MasterInfoList[i].cJobCode 
						<< L" GraduateCount:" << pA.MasterInfoList[i].iGraduateCount << std::endl;
		}
	}
	else
	{
		std::cout<< "QueryGetPageMasterCharacter 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryAddMasterAndPupil( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddMasterAndPupil pQ;
	memset (&pQ, 0x00, sizeof(pQ));

	printf( "MasterCharacterDBID>>" );
	cgets(szCmd);
	pQ.biMasterCharacterDBID = _atoi64(szCmd);

	printf( "PupilCharacterDBID>>" );
	cgets(szCmd);
	pQ.biPupilCharacterDBID = _atoi64(szCmd);

	printf( "CheckRegisterFlag>>" );
	cgets(szCmd);
	pQ.bCheckRegisterFlag = atoi(szCmd) ? true : false;

	printf( "TransactorCode>>" );
	cgets(szCmd);
	pQ.Code = static_cast<DBDNWorldDef::TransactorCode::eCode>(atoi(szCmd));

	TAAddMasterAndPupil pA;
	int iRet = pWorld->QueryAddMasterAndPupil( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryAddMasterAndPupil 성공\r\n";
		std::cout<< pA.uiOppositeAccountDBID << std::endl;
	}
	else
	{
		std::cout<< "QueryAddMasterAndPupil 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryDelMasterAndPupil( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelMasterAndPupil pQ;
	memset (&pQ, 0x00, sizeof(pQ));

	printf( "MasterCharacterDBID>>" );
	cgets(szCmd);
	pQ.biMasterCharacterDBID = _atoi64(szCmd);

	printf( "PupilCharacterDBID>>" );
	cgets(szCmd);
	pQ.biPupilCharacterDBID = _atoi64(szCmd);

	printf( "TransactorCode>>" );
	cgets(szCmd);
	pQ.Code = static_cast<DBDNWorldDef::TransactorCode::eCode>(atoi(szCmd));

	printf( "PenaltyRespectPoint>>" );
	cgets(szCmd);
	pQ.iPenaltyRespectPoint = atoi(szCmd);

	TADelMasterAndPupil pA;
	int iRet = pWorld->QueryDelMasterAndPupil( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryDelMasterAndPupil 성공\r\n";
	}
	else
	{
		std::cout<< "QueryDelMasterAndPupil 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetMasterCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetMasterCharacter pQ;

	printf( "GetType>>" );
	cgets(szCmd);
	pQ.Code = static_cast<DBDNWorldDef::GetMasterCharacterCode::eCode>(atoi(szCmd));

	printf( "MasterCharacterDBID>>" );
	cgets(szCmd);
	pQ.biMasterCharacterDBID = _atoi64(szCmd);

	printf( "PupilCharacterDBID>>" );
	cgets(szCmd);
	pQ.biPupilCharacterDBID = _atoi64(szCmd);

	switch( pQ.Code )
	{
		case DBDNWorldDef::GetMasterCharacterCode::MyInfo:
		{
			TAGetMasterCharacterType1 pA;
			int iRet = pWorld->QueryGetMasterCharacterType1( &pQ, &pA );
			if( iRet == ERROR_NONE )
			{
				std::cout<< "QueryGetMasterCharacterType1 성공\r\n";

				std::wcout <<(pA.MasterCharacterInfo.bMasterListShowFlag ? L"true" : L"false") << L"-" << pA.MasterCharacterInfo.wszSelfIntroduction << L"-" << pA.MasterCharacterInfo.iGraduateCount << L"-" << pA.MasterCharacterInfo.iRespectPoint << std::endl;
			}
			else
			{
				std::cout<< "QueryGetMasterCharacterType1 실패 Err:" << iRet << std::endl;
			}
			break;
		}
		case DBDNWorldDef::GetMasterCharacterCode::MyMasterInfo:
		{
			TAGetMasterCharacterType2 pA;
			int iRet = pWorld->QueryGetMasterCharacterType2( &pQ, &pA );
			if( iRet == ERROR_NONE )
			{
				std::cout<< "QueryGetMasterCharacterType2 성공\r\n";

				std::wcout << pA.MasterInfo.biCharacterDBID << L"-" << pA.MasterInfo.wszCharName << L"-" <<(int)pA.MasterInfo.cJob << L"-" <<(int)pA.MasterInfo.cLevel << L"-" << pA.MasterInfo.iRespectPoint << L"-" << pA.MasterInfo.iFavorPoint << std::endl;
			}
			else
			{
				std::cout<< "QueryGetMasterCharacterType2 실패 Err:" << iRet << std::endl;
			}
			break;
		}
		case DBDNWorldDef::GetMasterCharacterCode::OptionalMasterInfo:
		{
			TAGetMasterCharacterType3 pA;
			memset( &pA, 0, sizeof(pA) );

			int iRet = pWorld->QueryGetMasterCharacterType3( &pQ, &pA );
			if( iRet == ERROR_NONE )
			{
				std::cout<< "QueryGetMasterCharacterType3 성공\r\n";

				std::wcout << pA.biCharacterDBID << L"-" << pA.wszCharName << L"-" << pA.wszSelfIntroduction << L"-" <<(int)pA.cGenderCode << L"-" 
						   <<(int)pA.cLevel << L"-" <<(int)pA.cJob << L"-" << pA.iGraduateCount << L"-" << pA.iRespectPoint << std::endl;
			}
			else
			{
				std::cout<< "QueryGetMasterCharacterType3 실패 Err:" << iRet << std::endl;
			}
			break;
		}
	}
}

void CDNQueryTest::QueryGetListPupil( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListPupil pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	TAGetListPupil pA;
	memset( &pA, 0, sizeof(pA) );
	
	int iRet = pWorld->QueryGetListPupil( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetListPupil 성공\r\n";

		for( UINT i=0 ; i<pA.cCount ; ++i )
		{
			std::wcout << pA.PupilInfoList[i].biCharacterDBID << L"-" << pA.PupilInfoList[i].wszCharName << L"-" <<(int)pA.PupilInfoList[i].cLevel << L"-"
					   <<(int)pA.PupilInfoList[i].cJob << L"-" << pA.PupilInfoList[i].iFavorPoint << L"-" << pA.PupilInfoList[i].tLastConnectDate << std::endl;
		}
	}
	else
	{
		std::cout<< "QueryGetListPupil 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetListMyMasterAndClassmate( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListMyMasterAndClassmate pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	TAGetListMyMasterAndClassmate pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGetListMyMasterAndClassmate( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetListMyMasterAndClassmate 성공\r\n";

		for( UINT i=0 ; i<pA.cCount ; ++i )
		{
			std::wcout << pA.MasterAndClassmateInfoList[i].Code << L"-" << pA.MasterAndClassmateInfoList[i].biCharacterDBID << L"-" << pA.MasterAndClassmateInfoList[i].wszCharName << L"-" << pA.MasterAndClassmateInfoList[i].tLastConnectDate << std::endl;
		}
	}
	else
	{
		std::cout<< "QueryGetListMyMasterAndClassmate 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetMyClassmate( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetMyClassmate pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "ClassmateCharacterDBID>>" );
	cgets(szCmd);
	pQ.biClassmateCharacterDBID = _atoi64(szCmd);

	TAGetMyClassmate pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGetMyClassmate( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetMyClassmate 성공\r\n";

		std::wcout << pA.ClassmateInfo.biCharacterDBID << L"-" << pA.ClassmateInfo.wszCharName << L"-" <<(int)pA.ClassmateInfo.cJob << L"-" <<(int)pA.ClassmateInfo.cLevel << std::endl;
		for( UINT i=0 ; i<pA.ClassmateInfo.cMasterCount ; ++i )
		{
			std::wcout << pA.ClassmateInfo.wszMasterCharName[i] << std::endl;
		}
	}
	else
	{
		std::cout<< "QueryGetMyClassmate 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGraduate( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGraduate pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	TAGraduate pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGraduate( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGraduate 성공\r\n";
		for( UINT i=0 ; i<pA.cCount ; ++i )
		{
			std::cout << "스승CharDBID:" << pA.biMasterCharacterDBIDList[i] << std::endl;
		}
	}
	else
	{
		std::cout<< "QueryGraduate 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryModRespectPoint( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModRespectPoint pQ;

	printf( "MasterCharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "RespectPoint>>" );
	cgets(szCmd);
	pQ.iRespectPoint = atoi(szCmd);

	TAModRespectPoint pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryModRespectPoint( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModRespectPoint 성공\r\n";
	}
	else
	{
		std::cout<< "QueryModRespectPoint 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryModMasterFavorPoint( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModMasterFavorPoint pQ;

	printf( "MasterCharacterDBID>>" );
	cgets(szCmd);
	pQ.biMasterCharacterDBID = _atoi64(szCmd);

	printf( "PupilCharacterDBID>>" );
	cgets(szCmd);
	pQ.biPupilCharacterDBID = _atoi64(szCmd);

	printf( "FavorPoint>>" );
	cgets(szCmd);
	pQ.iFavorPoint = atoi(szCmd);

	TAModMasterFavorPoint pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryModMasterFavorPoint( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModMasterFavorPoint 성공\r\n";
	}
	else
	{
		std::cout<< "QueryModMasterFavorPoint 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetMasterAndFavorPoint( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetMasterAndFavorPoint pQ;

	printf( "PupilCharacterDBID>>" );
	cgets(szCmd);
	pQ.biPupilCharacterDBID = _atoi64(szCmd);

	TAGetMasterAndFavorPoint pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGetMasterAndFavorPoint( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetMasterAndFavorPoint 성공\r\n";
		for( UINT i=0 ; i<pA.cCount ; ++i )
		{
			std::cout<< "CharDBID:" << pA.MasterAndFavorPoint[i].biCharacterDBID << " FavorPoint:" << pA.MasterAndFavorPoint[i].iFavorPoint << std::endl;
		}
	}	
	else
	{
		std::cout<< "QueryGetMasterAndFavorPoint 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetMasterSystemCountInfo( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetMasterSystemCountInfo pQ;
	memset( &pQ, 0, sizeof(pQ) );

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	for( int i=0 ; i<_countof(pQ.biPartyCharacterDBID) ; ++i )
	{
		INT64 biCharacterDBID = 0;

		printf( "Party-CharacterDBID>>" );
		cgets(szCmd);
		biCharacterDBID = _atoi64(szCmd);
		if( biCharacterDBID == 0 )
			break;

		pQ.biPartyCharacterDBID[i] = biCharacterDBID;
		++pQ.cCount;
	}

	TAGetMasterSystemCountInfo pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGetMasterSystemCountInfo( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetMasterSystemCountInfo 성공\r\n";
		std::cout << "MasterCount:" << pA.iMasterCount << std::endl;
		std::cout << "PupilCount:"	<< pA.iPupilCount << std::endl;
		std::cout << "ClassmateCount:" << pA.iClassmateCount << std::endl;
	}	
	else
	{
		std::cout<< "QueryGetMasterSystemCountInfo 실패 Err:" << iRet << std::endl;
	}
}

#if defined( PRE_ADD_SECONDARY_SKILL )

void CDNQueryTest::QueryAddSecondarySkill( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddSecondarySkill pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "SkillID>>" );
	cgets(szCmd);
	pQ.iSecondarySkillID = atoi(szCmd);

	printf( "Type>>" );
	cgets(szCmd);
	pQ.Type = static_cast<SecondarySkill::Type::eType>(atoi(szCmd));

	int iRet = pWorld->QueryAddSecondarySkill( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryAddSecondarySkill 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryAddSecondarySkill 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryDelSecondarySkill( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelSecondarySkill pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "SkillID>>" );
	cgets(szCmd);
	pQ.iSecondarySkillID = atoi(szCmd);

	int iRet = pWorld->QueryDelSecondarySkill( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryDelSecondarySkill 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryDelSecondarySkill 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetListSecondarySkill( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListSecondarySkill pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	TAGetListSecondarySkill pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGetListSecondarySkill( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetListSecondarySkill 성공\r\n";

		for( UINT i=0 ; i<pA.cCount ; ++i )
		{
			char szBuf[MAX_PATH];
			//sprintf( szBuf, "[%d] SkillID=%d Exp=%d Type=%d\r\n", i+1, pA.SkillList[i].iSkillID, pA.SkillList[i].iExp, pA.SkillList[i].Type );
			sprintf( szBuf, "[%d] SkillID=%d Exp=%d\r\n", i+1, pA.SkillList[i].iSkillID, pA.SkillList[i].iExp );
			std::cout << szBuf;
		}
	}	
	else
	{
		std::cout<< "QueryGetListSecondarySkill 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryModSecondarySkillExp( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModSecondarySkillExp pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "SkillID>>" );
	cgets(szCmd);
	pQ.iSecondarySkillID = atoi(szCmd);

	printf( "AddExp>>" );
	cgets(szCmd);
	pQ.iSecondarySkillExp = atoi(szCmd);

	printf( "ExpAfter>>" );
	cgets(szCmd);
	pQ.iSecondarySkillExpAfter = atoi(szCmd);

	int iRet = pWorld->QueryModSecondarySkillExp( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModSecondarySkillExp 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryModSecondarySkillExp 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QuerySetManufactureSkillRecipe( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQSetManufactureSkillRecipe pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "LocationCode>>" );
	cgets(szCmd);
	pQ.LocationCode = static_cast<DBDNWorldDef::ItemLocation::eCode>(atoi(szCmd));

	printf( "ItemSerial>>" );
	cgets(szCmd);
	pQ.biItemSerial = _atoi64(szCmd);

	printf( "SkillID>>" );
	cgets(szCmd);
	pQ.iSecondarySkillID = atoi(szCmd);

	int iRet = pWorld->QuerySetManufactureSkillRecipe( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QuerySetManufactureSkillRecipe 성공\r\n";
	}	
	else
	{
		std::cout<< "QuerySetManufactureSkillRecipe 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryExtractManufactureSkillRecipe( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQExtractManufactureSkillRecipe pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "ItemSerial>>" );
	cgets(szCmd);
	pQ.biItemSerial = _atoi64(szCmd);

	printf( "LocationCode>>" );
	cgets(szCmd);
	pQ.LocationCode = static_cast<DBDNWorldDef::ItemLocation::eCode>(atoi(szCmd));

	int iRet = pWorld->QueryExtractManufactureSkillRecipe( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryExtractManufactureSkillRecipe 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryExtractManufactureSkillRecipe 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryDelManufactureSkillRecipe( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelManufactureSkillRecipe pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "ItemSerial>>" );
	cgets(szCmd);
	pQ.biItemSerial = _atoi64(szCmd);

	strcpy( pQ.szIP, "127.0.0.1" );

	int iRet = pWorld->QueryDelManufactureSkillRecipe( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryDelManufactureSkillRecipe 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryDelManufactureSkillRecipe 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetListManufactureSkillRecipe( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListManufactureSkillRecipe pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	TAGetListManufactureSkillRecipe pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryGetListManufactureSkillRecipe( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetListManufactureSkillRecipe 성공\r\n";

		for( UINT i=0 ; i<pA.cCount ; ++i )
		{
			char szBuf[MAX_PATH];
			sprintf( szBuf, "[%d] SkillID=%d ItemSerial=%I64d ItemID=%d Exp=%d \r\n", i+1, pA.RecipeList[i].iSkillID, pA.RecipeList[i].biItemSerial, pA.RecipeList[i].iItemID, pA.RecipeList[i].nExp );
			std::cout << szBuf;
		}
	}	
	else
	{
		std::cout<< "QueryGetListManufactureSkillRecipe 실패 Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryModManufactureSkillRecipe( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModManufactureSkillRecipe pQ;

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "ItemSerial>>" );
	cgets(szCmd);
	pQ.biItemSerial = _atoi64(szCmd);

	printf( "AddDurability>>" );
	cgets(szCmd);
	pQ.nDurability = atoi(szCmd);

	printf( "DurabilityAfter>>" );
	cgets(szCmd);
	pQ.nDurabilityAfter = atoi(szCmd);

	int iRet = pWorld->QueryModManufactureSkillRecipe( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModManufactureSkillRecipe 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryModManufactureSkillRecipe 실패 Err:" << iRet << std::endl;
	}	
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

void CDNQueryTest::QueryGetGuild( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	int nGuildDBID;
	

	printf( "GuildDBID>>" );
	cgets(szCmd);
	nGuildDBID = atoi(szCmd);

	TAGetGuildInfo pA;
	int iRet = pWorld->QueryGetGuildInfo( nGuildDBID, TEST_WORLD, &pA );

	std::vector<TGuildMember> vGuildMember;
	if(iRet == ERROR_NONE)
		iRet = pWorld->QueryGetGuildMemberList( nGuildDBID, TEST_WORLD, vGuildMember );

	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetGuild 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryGetGuild 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryDismissGuild( std::vector<std::string>& vSplit )
{

	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDismissGuild pQ;
	TADismissGuild pA;
	
	printf( "GuildDBID>>" );
	cgets(szCmd);
	pQ.nGuildDBID = atoi(szCmd);

	int iRet = pWorld->QueryDismissGuild( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryDismissGuild 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryDismissGuild 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetGuildHistoryList( std::vector<std::string>& vSplit )
{

	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetGuildHistoryList pQ;
	TAGetGuildHistoryList pA;

	printf( "StartNum>>" );
	cgets(szCmd);
	pQ.biIndex = atoi(szCmd);

	pQ.bDirection = false;

	printf( "GuildDBID>>" );
	cgets(szCmd);
	pQ.nGuildDBID = atoi(szCmd);


	int iRet = pWorld->QueryGetGuildHistoryList( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetGuildHistoryList 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryGetGuildHistoryList 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetGuildWareHistoryList( std::vector<std::string>& vSplit )
{	
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetGuildWareHistory pQ;
	TAGetGuildWareHistory pA;

	memset (&pQ, 0x00, sizeof(pQ));

	printf( "StartNum>>" );
	cgets(szCmd);
	pQ.nIndex = atoi(szCmd);

	printf( "GuildDBID>>" );
	cgets(szCmd);
	pQ.nGuildDBID = atoi(szCmd);


	int iRet = pWorld->QueryGetGuildWareHistoryList( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetGuildWareHistoryList 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryGetGuildWareHistoryList 실패Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetGuildWareInfo(std::vector<std::string>& vSplit)
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetGuildWareInfo pQ;
	TAGetGuildWareInfo pA;

	printf( "GuildID>>" );
	cgets(szCmd);
	pQ.nGuildDBID = atoi(szCmd);
	pQ.wMaxCount = 150;

	int iRet = pWorld->QueryGetGuildWareInfo( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetGuildWareInfo 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryGetGuildWareInfo 실패Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetGuildWarSchedule(std::vector<std::string>& vSplit)
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetGuildWarSchedule pQ;
	TAGetGuildWarSchedule pA;

	int iRet = pWorld->QueryGetGuildWarSchedule( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetGuildWarSchedule 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryGetGuildWarSchedule 실패Err:" << iRet << std::endl;
	}	
}
void CDNQueryTest::QueryEnrollGuildWar(std::vector<std::string>& vSplit)
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQEnrollGuildWar pQ;
	TAEnrollGuildWar pA;

	memset( &pQ, 0, sizeof(pQ) );
	int iRet = pWorld->QueryEnrollGuildWar( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryEnrollGuildWar 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryEnrollGuildWar 실패Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryModGuildData(std::vector<std::string>& vSplit)
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModGuildCheat pQ;
	TAModGuildCheat pA;
	memset( &pQ, 0, sizeof(pQ) );

	printf( "GuildDBID>>" );
	cgets(szCmd);
	pQ.nGuildDBID = atoi(szCmd);

	printf( "CheatType>>" );
	cgets(szCmd);
	pQ.cCheatType = atoi(szCmd);

	printf( "Point>>" );
	cgets(szCmd);
	pQ.nPoint = atoi(szCmd);

	printf( "Level>>" );
	cgets(szCmd);
	pQ.cLevel = atoi(szCmd);

	int iRet = pWorld->QueryModGuildCheat( &pQ, pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModGuildCheat 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryModGuildCheat 실패Err:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetListFarm( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListFarm pQ;
	TAGetListFarm pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "EnableFlag>>" );
	cgets(szCmd);
	pQ.cEnableFlag = atoi(szCmd);

	int iRet = pWorld->QueryGetListFarm( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetListFarm 성공\r\n";

		for( int i=0 ; i<pA.cCount ; ++i )
		{
			std::wcout << L"FarmDBID:" << pA.Farms[i].iFarmDBID << L" Name:" << pA.Farms[i].wszFarmName << std::endl;
		}
	}	
	else
	{
		std::cout<< "QueryGetListFarm 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetListField( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListField pQ;
	TAGetListField pA;

	printf( "FarmDBID>>" );
	cgets(szCmd);
	pQ.iFarmDBID = atoi(szCmd);

	int iRet = pWorld->QueryGetListField( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetListField 성공\r\n";

		for( int i=0 ; i<pA.cCount ; ++i )
		{
			std::cout << "FarmIndex=[" << pA.Fields[i].nFieldIndex << "]" << std::endl;
			std::cout << "CharDBID=" << pA.Fields[i].biCharacterDBID << std::endl;
			std::wcout << L"CharName=" << pA.Fields[i].wszCharName << std::endl;
			std::cout << "ItemID=" << pA.Fields[i].iItemID << std::endl;
			std::cout << "ElapsedTime=" << pA.Fields[i].iElapsedTimeSec << std::endl;
			for( int j=0 ; j<Farm::Max::ATTACHITEM_KIND ; ++j )
			{
				if( pA.Fields[i].AttachItems[j].iItemID == 0 )
					break;
				std::cout << "AttachItemID=" << pA.Fields[i].AttachItems[j].iItemID << " Count=" << pA.Fields[i].AttachItems[j].iCount << std::endl;
			}
		}
	}	
	else
	{
		std::cout<< "QueryGetListField 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryAddField( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddField pQ;
	TAAddField pA;

	memset( &pQ, 0, sizeof(pQ) );

	printf( "FarmDBID>>" );
	cgets(szCmd);
	pQ.iFarmDBID = atoi(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "ItemID>>" );
	cgets(szCmd);
	pQ.iItemID = atoi(szCmd);

	printf( "ElapsedSec>>" );
	cgets(szCmd);
	pQ.iElapsedTimeSec = atoi(szCmd);

	printf( "AttachItem>>" );
	cgets(szCmd);
	int iItemID = atoi(szCmd);
	pQ.iAttachItemID = iItemID;

	int iRet = pWorld->QueryAddField( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryAddField 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryAddField 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryDelField( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelField pQ;
	TADelField pA;

	printf( "FarmDBID>>" );
	cgets(szCmd);
	pQ.iFarmDBID = atoi(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	int iRet = pWorld->QueryDelField( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryDelField 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryDelField 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryAddFieldAttachment( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddFieldAttachment pQ;
	TAAddFieldAttachment pA;

	printf( "FarmDBID>>" );
	cgets(szCmd);
	pQ.iFarmDBID = atoi(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	printf( "AttachItemID>>" );
	cgets(szCmd);
	pQ.iAttachItemID = atoi(szCmd);

	int iRet = pWorld->QueryAddFieldAttachment( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryAddFieldAttachment 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryAddFieldAttachment 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryModFieldElapsedTime( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModFieldElapsedTime pQ;
	TAModFieldElapsedTime pA;

	memset( &pQ, 0, sizeof(pQ) );
	printf( "FarmDBID>>" );
	cgets(szCmd);
	pQ.iFarmDBID = atoi(szCmd);

	for( int i=0 ; i<_countof(pQ.Updates) ; ++i )
	{
		printf( "FieldIndex>>" );
		cgets(szCmd);
		short nFieldIndex = atoi(szCmd);

		if( nFieldIndex == 0 )
			break;

		pQ.Updates[i].nFieldIndex		= nFieldIndex;
		pQ.Updates[i].iElapsedTimeSec	= 3;
		++pQ.cCount;
	}

	int iRet = pWorld->QueryModFieldElapsedTime( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModFieldElapsedTime 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryModFieldElapsedTime 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryHarvest( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQHarvest pQ;
	TAHarvest pA;

	memset( &pQ, 0, sizeof(pQ) );

	printf( "FarmDBID>>" );
	cgets(szCmd);
	pQ.iFarmDBID = atoi(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "HarvestCharacterDBID>>" );
	cgets(szCmd);
	pQ.biHarvestCharacterDBID = _atoi64(szCmd);

	for( int i=0 ; i<_countof(pQ.HarvestItems) ; ++i )
	{
		printf( "ItemID>>" );
		cgets(szCmd);
		int iItemID = atoi(szCmd);

		if( iItemID == 0 )
			break;

		pQ.HarvestItems[i].iItemID = iItemID;
	}

	int iRet = pWorld->QueryHarvest( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryHarvest 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryHarvest 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetListHarvestDepotItem( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListHarvestDepotItem pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TAGetListHarvestDepotItem pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetListHarvestDepotItem( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryGetListHarvestDepotItem 성공\r\n";

		for( int i=0 ; i<pA.cCount ; ++i )
		{
			std::cout << "ItemSerial:" << pA.Items[i].nSerial << std::endl;
			std::cout << "ItemID:" << pA.Items[i].nItemID << std::endl;
		}
	}	
	else
	{
		std::cout<< "QueryGetListHarvestDepotItem 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetFieldCountByCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetFieldCountByCharacter pQ;
	TAGetFieldCountByCharacter pA;

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetFieldCountByCharacter( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetFieldCountByCharacter 성공\r\n";
		std::cout << "FieldCount : " << pA.iFieldCount << std::endl;
	}	
	else
	{
		std::cout<< "QueryGetFieldCountByCharacter 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetFieldItemCount( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetFieldItemCount pQ;
	TAGetFieldItemCount pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "ItemID>>" );
	cgets(szCmd);
	pQ.iItemID = atoi(szCmd);

	int iRet = pWorld->QueryGetFieldItemCount( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetFieldItemCount 성공\r\n";
		std::cout << "ItemCount=" << pA.iItemCount << std::endl;
	}	
	else
	{
		std::cout<< "QueryGetFieldItemCount 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetListFieldForCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListFieldForCharacter pQ;
	TAGetListFieldForCharacter pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetListFieldForCharacter( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListFieldForCharacter 성공\r\n";

		for( int i=0 ; i<pA.cCount ; ++i )
		{
			std::cout << "ItemID=" << pA.Fields[i].iItemID << " ElapsedTimeSec=" << pA.Fields[i].iElapsedTimeSec << std::endl;

			for( int j=0 ; j<_countof(pA.Fields[i].AttachItems) ; ++j )
			{
				if( pA.Fields[i].AttachItems[j].iItemID > 0 )
					std::cout << "AttachItemID=" << pA.Fields[i].AttachItems[j].iItemID << " Count=" << pA.Fields[i].AttachItems[j].iCount << std::endl;
			}
		}
	}	
	else
	{
		std::cout<< "QueryGetListFieldForCharacter 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryAddFieldForCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddField pQ;
	TAAddField pA;

	memset( &pQ, 0, sizeof(pQ) );
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	printf( "ItemID>>" );
	cgets(szCmd);
	pQ.iItemID = atoi(szCmd);

	printf( "ElapsedSec>>" );
	cgets(szCmd);
	pQ.iElapsedTimeSec = atoi(szCmd);

	printf( "MaxFieldCount>>" );
	cgets(szCmd);
	pQ.iMaxFieldCount = atoi(szCmd);

	printf( "AttachItem>>" );
	cgets(szCmd);
	int iItemID = atoi(szCmd);
	pQ.iAttachItemID = iItemID;

	int iRet = pWorld->QueryAddFieldForCharacter( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryAddFieldForCharacter 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryAddFieldForCharacter 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryDelFieldForCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelFieldForCharacter pQ;
	TADelFieldForCharacter pA;

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	int iRet = pWorld->QueryDelFieldForCharacter( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryDelFieldForCharacter 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryDelFieldForCharacter 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryAddFieldForCharacterAttachment( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddFieldAttachment pQ;
	TAAddFieldAttachment pA;

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	printf( "AttachItemID>>" );
	cgets(szCmd);
	pQ.iAttachItemID = atoi(szCmd);

	int iRet = pWorld->QueryAddFieldForCharacterAttachment( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryAddFieldForCharacterAttachment 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryAddFieldForCharacterAttachment 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryModFieldForCharacterElapsedTime( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModFieldForCharacterElapsedTime pQ;
	TAModFieldForCharacterElapsedTime pA;

	memset( &pQ, 0, sizeof(pQ) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	for( int i=0 ; i<_countof(pQ.Updates) ; ++i )
	{
		printf( "FieldIndex>>" );
		cgets(szCmd);
		short nFieldIndex = atoi(szCmd);

		if( nFieldIndex == 0 )
			break;

		pQ.Updates[i].nFieldIndex		= nFieldIndex;
		pQ.Updates[i].iElapsedTimeSec	= 3;
		++pQ.cCount;
	}

	int iRet = pWorld->QueryModFieldForCharacterElapsedTime( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryModFieldForCharacterElapsedTime 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryModFieldForCharacterElapsedTime 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryHarvestForCharacter( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQHarvest pQ;
	TAHarvest pA;

	memset( &pQ, 0, sizeof(pQ) );

	printf( "CharacterDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "FieldIndex>>" );
	cgets(szCmd);
	pQ.nFieldIndex = atoi(szCmd);

	for( int i=0 ; i<_countof(pQ.HarvestItems) ; ++i )
	{
		printf( "ItemID>>" );
		cgets(szCmd);
		int iItemID = atoi(szCmd);

		if( iItemID == 0 )
			break;

		pQ.HarvestItems[i].iItemID = iItemID;
		pQ.HarvestItems[i].biItemSerial	= timeGetTime();
	}

	int iRet = pWorld->QueryHarvestForCharacter( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout<< "QueryHarvestForCharacter 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryHarvestForCharacter 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryChangeCharacterName(std::vector<std::string>& vSplit)
{
	char szCmd[256] = {0};

	CDNSQLMembership* pMembershipDB  = g_SQLConnectionManager.FindMembershipDB( 0 );
	_ASSERT( pMembershipDB );

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQChangeCharacterName pQ;
	TAChangeCharacterName pA;
	
	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.nCharacterDBID = _atoi64(szCmd);


	printf( "Change CharacterName>>" );
	cgets(szCmd);
	MultiByteToWideChar( CP_ACP, 0, szCmd, -1, pQ.wszCharacterName, NAMELENMAX );

	pQ.btChangeCode = CHANGECODE_CHARNAME;
	pQ.nReservedPeriodDay = RESERVEDPERIOD_CHARNAME;
	pQ.nNextPeriodDay = NEXTPERIOD_CHARNAME;

	int iRet = pMembershipDB->QueryModCharacterName( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryModCharacterName [MemberShip] 성공\r\n";

		iRet = pWorld->QueryModCharacterName( &pQ );
		if( iRet == ERROR_NONE )
		{
			std::cout << "QueryModCharacterName [World] 성공\r\n";
		}
		else
		{
			std::cout<< "QueryModCharacterName [World] 실패 Err:" << iRet << std::endl;
		}
	}	
	else
	{
		std::cout<< "QueryGetFieldCountByCharacter [MemberShip] 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryAddPvPLadderResult( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddPvPLadderResult pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAAddPvPLadderResult pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "PvPLadderCode>>" );
	cgets(szCmd);
	pQ.cPvPLadderCode = static_cast<BYTE>(atoi(szCmd));

	printf( "PvPLadderGradePoint>>" );
	cgets(szCmd);
	pQ.iPvPLadderGradePoint = atoi(szCmd);

	printf( "HiddenPvPLadderGradePoint>>" );
	cgets(szCmd);
	pQ.iHiddenPvPLadderGradePoint = atoi(szCmd);

	printf( "Result>>" );
	cgets(szCmd);
	pQ.cResult = static_cast<BYTE>(atoi(szCmd));

	// KillDeathCount-Random
	::srand( timeGetTime() );
	pQ.cKillDeathCount =(rand()%3)+1;
	for( int i=0 ; i<pQ.cKillDeathCount ; ++i )
	{
		pQ.KillDeathCounts[i].cJobCode		= i+1;
		pQ.KillDeathCounts[i].nKillCount	= i+1;
		pQ.KillDeathCounts[i].nDeathCount	= i+1;
	}

	if( pQ.cPvPLadderCode == LadderSystem::MatchType::_1vs1 )
	{
		printf( "VSJobCode>>" );
		cgets(szCmd);
		pQ.cVersusCharacterJobCode = static_cast<BYTE>(atoi(szCmd));
	}

	int iRet = pWorld->QueryAddPvPLadderResult( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryAddPvPLadderResult 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryGetFieldCountByCharacter 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetListPvPLadderScore( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListPvPLadderScore pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAGetListPvPLadderScore pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetListPvPLadderScore( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListPvPLadderScore 성공\r\n";
		std::cout << "래더포인트:" << pA.Data.iPvPLadderPoint << std::endl;
		std::cout << "가장많이킬한직업:" <<(int)pA.Data.cMaxKillJobCode << std::endl;
		std::cout << "가장많이킬당한직업:" <<(int)pA.Data.cMaxDeathJobCode << std::endl;
		for( int i=0 ; i<pA.Data.cLadderTypeCount ; ++i )
		{
			std::cout << "==============================================================" << std::endl;
			std::cout << "PvPLadderCode:" <<(int)pA.Data.LadderScore[i].cPvPLadderCode << std::endl;
			std::cout << "PvPLadderGradePoint:" <<(int)pA.Data.LadderScore[i].iPvPLadderGradePoint << std::endl;
			std::cout << "HiddenPvPLadderGradePoint:" <<(int)pA.Data.LadderScore[i].iHiddenPvPLadderGradePoint << std::endl;
			std::cout << pA.Data.LadderScore[i].iWin << "승 " << pA.Data.LadderScore[i].iDraw << "무 " << pA.Data.LadderScore[i].iLose << "패" << std::endl;
			std::cout << "[Today]" << pA.Data.LadderScore[i].nTodayWin << "승 " << pA.Data.LadderScore[i].nTodayDraw << "무 " << pA.Data.LadderScore[i].nTodayLose << "패" << std::endl;
			std::cout << "[연속]" << pA.Data.LadderScore[i].nConsecutiveWin << "승 " << pA.Data.LadderScore[i].nConsecutiveLose << "패" << std::endl;
			std::cout << "주간래더판수:" << pA.Data.LadderScore[i].nWeeklyCount << std::endl;
			std::cout << "==============================================================" << std::endl;
		}
	}	
	else
	{
		std::cout<< "QueryGetListPvPLadderScore 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetListPvPLadderScoreByJob( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListPvPLadderScoreByJob pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAGetListPvPLadderScoreByJob pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetListPvPLadderScoreByJob( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListPvPLadderScoreByJob 성공\r\n";
		for( int i=0 ; i<pA.Data.cJobCount ; ++i )
		{
			std::cout << "==============================================================" << std::endl;
			std::cout << "JobCode:" <<(int)pA.Data.LadderScoreByJob[i].cJobCode << std::endl;
			std::cout << pA.Data.LadderScoreByJob[i].iWin << "승 " << pA.Data.LadderScoreByJob[i].iDraw << "무 " << pA.Data.LadderScoreByJob[i].iLose << "패" << std::endl;
			std::cout << "==============================================================" << std::endl;
		}
	}	
	else
	{
		std::cout<< "QueryGetListPvPLadderScoreByJob 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryInitPvPLadderGradePoint( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQInitPvPLadderGradePoint pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAInitPvPLadderGradePoint pA;
	memset( &pA, 0, sizeof(pA) );

	sprintf_s( pQ.szIP, "127.0.0.1" );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "PvPLadderCode>>" );
	cgets(szCmd);
	pQ.cPvPLadderCode = static_cast<BYTE>(atoi(szCmd));

	printf( "PvPLadderPoint(증가치)>>" );
	cgets(szCmd);
	pQ.iPvPLadderPoint = atoi(szCmd);

	printf( "초기화할래더평점>>" );
	cgets(szCmd);
	pQ.iPvPLadderGradePoint =(atoi(szCmd));

	int iRet = pWorld->QueryInitPvPLadderGradePoint( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryInitPvPLadderGradePoint 성공\r\n";
	}	
	else
	{
		std::cout<< "QueryInitPvPLadderGradePoint 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryUsePvPLadderPoint( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQUsePvPLadderPoint pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAUsePvPLadderPoint pA;
	memset( &pA, 0, sizeof(pA) );

	sprintf_s( pQ.szIP, "127.0.0.1" );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "사용한PvPLadderPoint>>" );
	cgets(szCmd);
	pQ.iPvPLadderPoint = atoi(szCmd);

	int iRet = pWorld->QueryUsePvPLadderPoint( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryUsePvPLadderPoint 성공\r\n";
		std::cout << "사용후 남은 PvP 래더 평점:" << pA.iPvPLadderPointAfter << std::endl;
	}	
	else
	{
		std::cout<< "QueryUsePvPLadderPoint 실패 Err:" << iRet << std::endl;
	}	
}

void CDNQueryTest::QueryGetListPvPLadderRanking( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListPvPLadderRanking pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAGetListPvPLadderRanking pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "LadderCode>>" );
	cgets(szCmd);
	pQ.cPvPLadderCode = static_cast<BYTE>(atoi(szCmd));

	int iRet = pWorld->QueryGetListPvPLadderRanking( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListPvPLadderRanking 성공\r\n";
	}	
	else
	{
		std::cout << "QueryGetListPvPLadderRanking 실패\r\n";
	}	
}

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
void CDNQueryTest::QueryChangeJobCode( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	// Input
	INT64 biCharacterID;

	// Output
	USHORT wSkillPoint;
	USHORT wTotalPoint;

	printf( "CharDBID>>" );
	cgets(szCmd);
	biCharacterID = _atoi64(szCmd);

	int iRet = pWorld->QueryChangeJobCode( biCharacterID, 1, 1, 11, 12, 23, 24, 2, wSkillPoint, wTotalPoint );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryChangeJobCode 성공\r\n";
		std::cout << "스킬 리셋 후 스킬 포인트 증가량:" << wSkillPoint << std::endl;
		std::cout << "스킬 리셋 후 보유한 총 스킬 포인트:" << wTotalPoint << std::endl;
	}	
	else
	{
		std::cout << "QueryChangeJobCode 실패" << iRet << std::endl;
	}	
}
#endif

void CDNQueryTest::QueryCompleteEventQuest( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQCompleteEventQuest pQ;
	TACompleteEventQuest pA;

	pQ.biCharacterDBID = 1;
	pQ.nScheduleID = 1;
	pQ.tExpireDate = 0;

	int iRet = pWorld->QueryCompleteEventQuest( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryCompleteEventQuest 성공\r\n";
		std::cout << "퀘스트 완료횟수:" << pA.wCompleteCount << std::endl;
	}	
	else
	{
		std::cout << "QueryCompleteEventQuest 실패\r\n";
	}	
}

void CDNQueryTest::QueryCheckPCRoomIP( std::vector<std::string>& vSplit )
{
	CDNSQLMembership* pMembershipDB  = g_SQLConnectionManager.FindMembershipDB( 0 );
	_ASSERT( pMembershipDB );

	bool bFlag;
	char cGrade;
	int iRet = pMembershipDB->QueryCheckPCRoomIP( "127.0.0.1", bFlag, cGrade );
	
	BYTE cTemp = cGrade;
	int a =0;
}


void CDNQueryTest::QueryAddAbuseMonitor( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddAbuseMonitor TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	TxPacket.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryAddAbuseMonitor( &TxPacket );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryAddAbuseMonitor 성공\r\n";
	}	
	else
	{
		std::cout << "QueryAddAbuseMonitor 실패\r\n";
	}	
}

void CDNQueryTest::QueryDelAbuseMonitor( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelAbuseMonitor TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	TxPacket.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryDelAbuseMonitor( &TxPacket );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryDelAbuseMonitor 성공\r\n";
	}	
	else
	{
		std::cout << "QueryDelAbuseMonitor 실패\r\n";
	}	
}

void CDNQueryTest::QueryGetAbuseMonitor( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetAbuseMonitor TxPacket;
	TAGetAbuseMonitor pA;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	TxPacket.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetAbuseMonitor( &TxPacket, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetAbuseMonitor 성공\r\n";
		std::cout << "AbuseCount=" << pA.iAbuseCount << std::endl;
		std::cout << "CallCount=" << pA.iCallCount << std::endl;
		std::cout << "PlayRestraint=" << pA.iPlayRestraintValue << std::endl;
		std::cout << "DBResetRestraint=" << pA.iDBResetRestraintValue << std::endl;
	}	
	else
	{
		std::cout << "QueryGetAbuseMonitor 실패\r\n";
	}	
}

void CDNQueryTest::QueryGetListVariableReset( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListVariableReset TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	
	TAGetListVariableReset pA;
	memset( &pA, 0, sizeof(pA) );
 
	printf( "CharDBID>>" );
	cgets(szCmd);
	TxPacket.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetListVariableReset( &TxPacket, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListVariableReset 성공\r\n";
		for( int i=0 ; i<pA.cCount ; ++i )
		{
			std::cout << "Type=" << pA.Data[i].Type << " Value=" << pA.Data[i].biValue << " Time=" << pA.Data[i].tLastModifyDate <<  std::endl;
		}
	}	
	else
	{
		std::cout << "QueryGetListVariableReset 실패\r\n";
	}	
}

void CDNQueryTest::QueryModVariableReset( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModVariableReset TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	TxPacket.biCharacterDBID = _atoi64(szCmd);

	printf( "Type>>" );
	cgets(szCmd);
	TxPacket.Type = static_cast<CommonVariable::Type::eCode>(atoi(szCmd));

	printf( "Value>>" );
	cgets(szCmd);
	TxPacket.biValue = _atoi64(szCmd);

	time_t Time;
	time(&Time);

	TxPacket.tResetTime = Time;

	int iRet = pWorld->QueryModVariableReset( &TxPacket );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryModVariableReset 성공\r\n";
	}	
	else
	{
		std::cout << "QueryModVariableReset 실패\r\n";
	}	
}

void CDNQueryTest::QueryGetCharacterPartialy7( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	printf( "CharName>>" );
	cgets(szCmd);
	
	WCHAR wszCharName[MAX_PATH];
	memset( wszCharName, 0, sizeof(wszCharName) );

	MultiByteToWideChar(CP_ACP, 0, szCmd, -1, wszCharName, _countof(wszCharName) );

	UINT uiAccountDBID=0;
	INT64 biCharacterDBID = 0;
	int iRet = pWorld->QueryGetCharacterPartialy7( wszCharName, uiAccountDBID, biCharacterDBID );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetCharacterPartialy7 성공\r\n";
	}	
	else
	{
		std::cout << "QueryGetCharacterPartialy7 실패\r\n";
	}	
}


#if defined( _WORK )

UINT __stdcall CDNQueryTest::PerformanceThread(void *pParam)
{
	int iCount = 2000;

	std::string					strString((char*)pParam);
	std::vector<std::string>	vSplit;
	boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );

	INT64	biCharacterDBID = _atoi64(vSplit[0].c_str());
	int		iThreadIndex	= atoi(vSplit[1].c_str());

	for( int i=0 ; i<iCount ; ++i )
	{
		CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( iThreadIndex, TEST_WORLD );
		_ASSERT( pWorld );

		DBPacket::TMaterialItemInfo ItemList[nItemListMaxCount];
		memset(&ItemList, 0, sizeof(ItemList));
		int nMaterlializedItemCount = 0;
		{
			std::wstring wstrItemLocationCode = L"1,2,3,4,7,8";	// 캐쉬인벤은 따로 로드한다
			int nRet = pWorld->QueryGetListMaterializedItem( biCharacterDBID, wstrItemLocationCode.c_str(), nItemListMaxCount, nMaterlializedItemCount, ItemList);
			if( nRet != ERROR_NONE )
				std::cout << "P_GetListMaterializedItem Error" << std::endl;
		}
		{
			// P_ModSkillLevel
			int nRet = pWorld->QueryModSkillLevel( biCharacterDBID, atoi(vSplit[2].c_str()), 1, 0, 0, 0, 0, 0, 0 );
			if( nRet != ERROR_NONE )
				std::cout << "P_ModSkillLevel Error" << std::endl;
		}

		// P_ModItemDurability
		{
			TQModItemDurability pQ;
			memset( &pQ, 0, sizeof(pQ) );

			pQ.biCharacterDBID	= biCharacterDBID;
			for( int i=0 ; i<nMaterlializedItemCount ; ++i )
			{
				if( ItemList[i].ItemInfo.Item.wDur > 0 )
				{
					pQ.ItemDurArr[pQ.nCount].biSerial	= ItemList[i].ItemInfo.Item.nSerial;
					pQ.ItemDurArr[pQ.nCount].wDur		= ItemList[i].ItemInfo.Item.wDur;
					++pQ.nCount;
				}
			}

			if( pQ.nCount > 0 )
			{
				int nRet = pWorld->QueryModItemDurability( &pQ );
				if( nRet != ERROR_NONE )
					std::cout << "QueryModItemDurability Error" << std::endl;
			}
		}

		{
			// P_AddMaterializedItem
			TQAddMaterializedItem pAddQ;
			memset( &pAddQ, 0, sizeof(pAddQ) );

			int cItemLocationIndex	= -1;
			int cItemLocationIndex2 = -1;

			for( int j=0 ; j<INVENTORYMAX ; ++j )
			{
				bool bExist = false;
				for( int i=0 ; i<nMaterlializedItemCount ; ++i )
				{
					if( ItemList[i].Code ==  DBDNWorldDef::ItemLocation::Inventory )
					{
						if( ItemList[i].ItemInfo.cSlotIndex == j )
						{
							bExist =true;
							break;
						}
					}
				}
				if( bExist == false )
				{
					if( cItemLocationIndex == -1 )
						cItemLocationIndex = j;
					else
					{
						cItemLocationIndex2 = j;
						break;
					}
				}
			}

			pAddQ.biCharacterDBID		= biCharacterDBID;
			pAddQ.cItemLocationIndex	= cItemLocationIndex;	// 아이템이 위치할 인덱스 값
			pAddQ.Code					= DBDNWorldDef::AddMaterializedItem::Pick;
			pAddQ.cItemLocationCode		= 2;
			pAddQ.AddItem.nSerial		= MakeSerial(iThreadIndex);
			pAddQ.AddItem.nItemID		= 78;
			pAddQ.AddItem.wCount		= 1;

			int nRet = cItemLocationIndex != -1 ?pWorld->QueryAddMaterializedItem( &pAddQ ) : ERROR_DB;
			if( nRet != ERROR_NONE )
				std::cout << "P_AddMaterializedItem Error=" << nRet << " Serial=" << pAddQ.AddItem.nSerial << std::endl;

			// P_UseItem
			if( nRet == ERROR_NONE )
			{
				TQUseItem pUseItem;
				memset( &pUseItem, 0, sizeof(pUseItem) );

				pUseItem.Code			= DBDNWorldDef::UseItem::Use;
				pUseItem.biItemSerial	= pAddQ.AddItem.nSerial;
				pUseItem.nUsedItemCount	= 1;
				wcscpy( pUseItem.wszIP, L"127.0.0.1" );

				int nRet = pWorld->QueryUseItem( &pUseItem );
				if( nRet != ERROR_NONE )
					std::cout << "P_UseItem Error" << std::endl;
			}

			// P_AddMaterializedItem
			{
				for( int i=0 ; i<1 ; ++i )
				{
					pAddQ.AddItem.wCount = 10;
					int nRet = pWorld->QueryAddMaterializedItem( &pAddQ );
					if( nRet != ERROR_NONE )
						std::cout << "P_AddMaterializedItem Error=" << nRet << " Serial=" << pAddQ.AddItem.nSerial << std::endl;
					if( nRet == ERROR_NONE )
					{
						for( int i=0 ; i<pAddQ.AddItem.wCount ; ++i )
						{
							TQResellItem pResell;
							memset( &pResell, 0, sizeof(pResell) );

							pResell.biCharacterDBID	= biCharacterDBID;
							pResell.biItemSerial	= pAddQ.AddItem.nSerial;
							pResell.nItemCount		= 1;
							pResell.biResellPrice	= 100;
							pResell.bAllFlag		= false;
							wcscpy( pResell.wszIP, L"127.0.0.1" );

							nRet = pWorld->QueryResellItem( &pResell );
							if( nRet != ERROR_NONE )
								std::cout << "P_ResellItem Error" << std::endl;
						}
					}
				}
				pAddQ.AddItem.wCount = 1;
			}

			// P_GetAbuseMonitor
			{
				TQGetAbuseMonitor pQ;
				memset( &pQ, 0, sizeof(pQ) );
				pQ.biCharacterDBID =biCharacterDBID;

				TAGetAbuseMonitor pA;
				memset( &pA, 0, sizeof(pA) );

				int nRet = pWorld->QueryGetAbuseMonitor( &pQ, &pA );
				if( nRet != ERROR_NONE )
					std::cout << "P_GetAbuseMonitor Error" << std::endl;
			}

			// QuerySelectCharacter
			{
				TASelectCharacter pA;
				memset( &pA, 0, sizeof(pA) );

				for( int i=0 ; i<1 ; ++i )
				{
					int nRet = pWorld->QuerySelectCharacter( TEST_WORLD, biCharacterDBID, 0, &pA );
					if( nRet != ERROR_NONE )
						std::cout << "QuerySelectCharacter Error" << std::endl;
				}
			}

			if( cItemLocationIndex2 != -1 )
			{
				int nRet = pWorld->QueryAddMaterializedItem( &pAddQ );
				if( nRet != ERROR_NONE )
					std::cout << "P_AddMaterializedItem Error=" << nRet << " Serial=" << pAddQ.AddItem.nSerial << std::endl;

				if( nRet == ERROR_NONE )
				{
					TQChangeItemLocation pSwitch;
					memset( &pSwitch, 0, sizeof(pSwitch) );

					for( int i=0 ; i<1 ; ++i )
					{
						pSwitch.biCharacterDBID		= biCharacterDBID;
						pSwitch.biItemSerial		= pAddQ.AddItem.nSerial;
						pSwitch.iItemID				= pAddQ.AddItem.nItemID;
						pSwitch.cItemLocationIndex	=(i%2 == 0) ? cItemLocationIndex2 : cItemLocationIndex;
						pSwitch.nItemCount			= 1;
						pSwitch.Code				= DBDNWorldDef::ItemLocation::Inventory;

						int nRet = pWorld->QueryChangeItemLocation( &pSwitch );
						if( nRet != ERROR_NONE )
							std::cout << "QueryChangeItemLocation Error" << std::endl;
					}

					// P_UseItem
					if( nRet == ERROR_NONE )
					{
						TQUseItem pUseItem;
						memset( &pUseItem, 0, sizeof(pUseItem) );

						pUseItem.Code			= DBDNWorldDef::UseItem::Use;
						pUseItem.biItemSerial	= pAddQ.AddItem.nSerial;
						pUseItem.nUsedItemCount	= 1;
						wcscpy( pUseItem.wszIP, L"127.0.0.1" );

						int nRet = pWorld->QueryUseItem( &pUseItem );
						if( nRet != ERROR_NONE )
							std::cout << "P_UseItem Error" << std::endl;
					}
				}
			}
		}
	}
	return 0;
}

void CDNQueryTest::QueryPerformanceTest( std::vector<std::string>& vSplit )
{
	// CharDBID,ThreadID,SkillID
	char *szParamLabel[] = 
	{ 
		//"27,0,2"
		"27,0,2", "39,1,2002", "920,2,3002", "1657,3,2",
		"86,4,2", "88,5,2", "1168,6,2002", "1219,7,2",
	};

	std::vector<HANDLE> vHandle;
	UINT ThreadID;
	for( int i=0 ;i<_countof(szParamLabel) ; ++i )
	{
		HANDLE hHandle =(HANDLE)_beginthreadex(NULL, 0, &PerformanceThread, szParamLabel[i], 0, &ThreadID);
		vHandle.push_back( hHandle );
	}

	for( UINT i=0 ; i<vHandle.size() ; ++i )
		WaitForSingleObject( vHandle[i], INFINITE );

	std::cout << "QueryPerformanceTest 종료" << std::endl;
}

#endif // #if defined( _WORK )

void CDNQueryTest::QueryGetListRepurchaseItem( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListRepurchaseItem pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAGetListRepurchaseItem pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetListRepurchaseItem( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListRepurchaseItem 성공\r\n";
	}	
	else
	{
		std::cout << "QueryGetListRepurchaseItem 실패\r\n";
	}	
}

void CDNQueryTest::QueryModPvPScore ( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQUpdatePvPData pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAUpdatePvPData pA;
	memset( &pA, 0, sizeof(pA) );

	pQ.biCharacterDBID = 1781;

	// test data
	for (int i=CLASS_WARRIER; i<CLASS_MAX; i++)
	{
		pQ.PvP.uiKOClassCount[i-1] = i;
		pQ.PvP.uiKObyClassCount[i-1] = i;
	}

	int iRet = pWorld->QueryModPvPScore( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryModPvPScore 성공\r\n";
	}	
	else
	{
		std::cout << "QueryModPvPScore 실패\r\n";
	}	

}

void CDNQueryTest::QueryGetPvPScore ( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );
	
	TPvPGroup pA;
	memset( &pA, 0, sizeof(pA) );


	int iRet = pWorld->QueryGetPvPScore( 1781, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetPvPScore 성공\r\n";
	}	
	else
	{
		std::cout << "QueryGetPvPScore 실패\r\n";
	}	

}

void CDNQueryTest::QueryAddPvPResultLog ( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddPvPResultLog pQ;
	memset( &pQ, 0, sizeof(pQ) );
	
	pQ.biCharacterDBID = 1781;

	// test data
	for (int i=CLASS_WARRIER; i<CLASS_MAX; i++)
	{
		pQ.iVsKOWin[i-1] = i;
		pQ.iVsKOLose[i-1] = i;
	}

	int iRet = pWorld->QueryAddPvPResultLog( &pQ);
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryAddPvPResultLog 성공\r\n";
	}	
	else
	{
		std::cout << "QueryAddPvPResultLog 실패\r\n";
	}	

}

#if defined( PRE_PARTY_DB )

void CDNQueryTest::QueryAddParty( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddParty pQ;
	memset( &pQ, 0, sizeof(pQ) );
	TAAddParty pA;
	memset( &pA, 0, sizeof(pA) );

	printf( "ServerID>>" );
	cgets(szCmd);
	pQ.Data.PartyData.iServerID = atoi(szCmd);

	printf( "PartyType>>" );
	cgets(szCmd);
	pQ.Data.PartyData.Type = static_cast<ePartyType>(atoi(szCmd));

	if( pQ.Data.PartyData.Type >= _PARTY_TYPE_MAX )
		return;

	srand( timeGetTime() );

	pQ.cWorldSetID = TEST_WORLD;
	
#if defined( PRE_STRINGCOPY_MODIFY )
	_wcscpy( pQ.Data.PartyData.wszPartyName, _countof(pQ.Data.PartyData.wszPartyName), L"Add Party", (int)wcslen(L"Add Party") );
#else
	wcscpy_s( pQ.Data.PartyData.wszPartyName, L"Add Party" );
#endif
	pQ.Data.PartyData.nPartyMaxCount = 4;
	pQ.Data.PartyData.iTargetMapIndex = 12001;
	pQ.Data.PartyData.TargetMapDifficulty = Dungeon::Difficulty::Abyss;
	pQ.Data.PartyData.iPassword = 6969;
	pQ.Data.PartyData.cMinLevel = 1;

	int iRet = pWorld->QueryAddParty( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryAddParty 성공 PartyID:" << pA.Data.PartyData.PartyID << std::endl;
	}
	else
	{
		std::cout << "QueryAddParty 실패 Ret:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryModParty( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModParty pQ;
	memset( &pQ, 0, sizeof(pQ) );

	printf( "PartyID>>" );
	cgets(szCmd);
	pQ.PartyData.PartyID = _atoi64(szCmd);

	pQ.PartyData.Type = _RAID_PARTY_8;
#if defined( PRE_STRINGCOPY_MODIFY )
	_wcscpy( pQ.PartyData.wszPartyName, _countof(pQ.PartyData.wszPartyName), L"Mod Party", (int)wcslen(L"Mod Party") );
#else
	wcscpy_s( pQ.PartyData.wszPartyName, L"Mod Party" );
#endif
	pQ.PartyData.nPartyMaxCount = 2;
	pQ.PartyData.iTargetMapIndex = 12001;
	pQ.PartyData.TargetMapDifficulty = Dungeon::Difficulty::Easy;
	pQ.PartyData.iPassword = 1111;
	pQ.PartyData.cMinLevel = 10;

	int iRet = pWorld->QueryModParty( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryModParty 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryModParty 실패 Ret:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryDelParty( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelParty pQ;
	memset( &pQ, 0, sizeof(pQ) );

	printf( "PartyID>>" );
	cgets(szCmd);
	pQ.PartyID = _atoi64(szCmd);

	int iRet = pWorld->QueryDelParty( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryDelParty 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryDelParty 실패 Ret:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryDelPartyForServer( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQDelPartyForServer pQ;
	memset( &pQ, 0, sizeof(pQ) );

	printf( "ServerID>>" );
	cgets(szCmd);
	pQ.iServerID = atoi(szCmd);

	int iRet = pWorld->QueryDelPartyForServer( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryDelPartyForServer 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryDelPartyForServer 실패 Ret:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryJoinParty( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQJoinParty pQ;
	memset( &pQ, 0, sizeof(pQ) );

	printf( "PartyID>>" );
	cgets(szCmd);
	pQ.PartyID = _atoi64(szCmd);
	pQ.iMaxUserCount = 2;

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryJoinParty( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryJoinParty 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryJoinParty 실패 Ret:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryOutParty( std::vector<std::string>& vSplit )
{
	char szCmd[256] = {0};

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQOutParty pQ;
	memset( &pQ, 0, sizeof(pQ) );

	printf( "PartyID>>" );
	cgets(szCmd);
	pQ.PartyID = _atoi64(szCmd);

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	TAOutParty pA;
	memset( &pA, 0, sizeof(pA) );

	int iRet = pWorld->QueryOutParty( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryOutParty 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryOutParty 실패 Ret:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryGetListParty( std::vector<std::string>& vSplit )
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListParty pQ;
	memset( &pQ, 0, sizeof(pQ) );

	pQ.cWorldSetID = TEST_WORLD;	

	std::vector<Party::Data> vData;
	int iRet = pWorld->QueryGetListParty( &pQ, vData );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListParty 성공" << std::endl;

		for( UINT i=0 ; i<vData.size() ; ++i )
		{
			std::cout << "===============================================================" << std::endl;
			std::cout << "PartyID=" << vData[i].PartyID << std::endl;
			std::cout << "LocationType=" << vData[i].LocationType << std::endl;
			std::cout << "LocationID=" << vData[i].iLocationID << std::endl;
			std::cout << "PartyType=" << vData[i].Type << std::endl;
			std::wcout << L"PartyName=" << vData[i].wszPartyName << std::endl;
		}
	}
	else
	{
		std::cout << "QueryGetListParty 실패 Ret:" << iRet << std::endl;
	}
}

#if defined( PRE_WORLDCOMBINE_PARTY )
void CDNQueryTest::QueryGetListPartyCode( std::vector<std::string>& vSplit )
{
	if( g_Config.nCombineWorldDBID <= 0 )
		return;

	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, g_Config.nCombineWorldDBID );
	_ASSERT( pWorld );

	TQGetListParty pQ;
	memset( &pQ, 0, sizeof(pQ) );

	pQ.cWorldSetID = g_Config.nCombineWorldDBID;	

	std::vector<Party::Data> vData;
	int iRet = pWorld->QueryGetListParty( pQ.cWorldSetID, _WORLDCOMBINE_PARTY, vData );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListParty 성공" << std::endl;

		for( UINT i=0 ; i<vData.size() ; ++i )
		{
			std::cout << "===============================================================" << std::endl;
			std::cout << "PartyID=" << vData[i].PartyID << std::endl;
			std::cout << "LocationType=" << vData[i].LocationType << std::endl;
			std::cout << "LocationID=" << vData[i].iLocationID << std::endl;
			std::cout << "PartyType=" << vData[i].Type << std::endl;
			std::wcout << L"PartyName=" << vData[i].wszPartyName << std::endl;
		}
	}
	else
	{
		std::cout << "QueryGetListParty 실패 Ret:" << iRet << std::endl;
	}
	
}
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )

void CDNQueryTest::QueryGetListPartyMember( std::vector<std::string>& vSplit )
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListPartyMember pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TAGetListPartyMember pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "PartyID>>" );
	cgets(szCmd);
	pQ.PartyID = _atoi64(szCmd);

	std::vector<Party::MemberData> vData;
	int iRet = pWorld->QueryGetListPartyMember( &pQ, &pA, vData );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListPartyMember 성공" << std::endl;

		for( UINT i=0 ; i<vData.size() ; ++i )
		{
			std::cout << "===============================================================" << std::endl;
			std::cout << "CharDBID=" << vData[i].biCharacterDBID << std::endl;
			std::wcout << L"CharName=" << vData[i].wszCharName<< std::endl;
			std::cout << "Level=" << (int)vData[i].cLevel << std::endl;
			std::cout << "Job=" << (int)vData[i].cJob << std::endl;
		}
	}
	else
	{
		std::cout << "QueryGetListPartyMember 실패 Ret:" << iRet << std::endl;
	}
}

void CDNQueryTest::QueryModPartyLeader( std::vector<std::string>& vSplit )
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQModPartyLeader pQ;
	memset( &pQ, 0, sizeof(pQ) );

	char szCmd[256] = {0};

	printf( "PartyID>>" );
	cgets(szCmd);
	pQ.PartyID = _atoi64(szCmd);

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryModPartyLeader( &pQ );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryModPartyLeader 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryModPartyLeader 실패 Ret:" << iRet << std::endl;
	}
}

#endif // #if defined( PRE_PARTY_DB )

void CDNQueryTest::TestTimeParamSet(std::vector<std::string>& vSplit)
{
	while (true)
	{
		TIMESTAMP_STRUCT tm;

		tm.year = (rand() - rand());
		tm.month = (rand() - rand());
		tm.day = (rand() - rand());
		tm.hour = (rand() - rand());
		tm.minute = (rand() - rand());
		tm.second = (rand() - rand());
		tm.fraction = (rand() - rand());
		
		__time64_t tTime64;

		CTimeParamSet TestTime (NULL, tTime64, tm, 0, 0, 0);

		if (TestTime.IsValid())
		{
			std::cout << "TestTimeParamSet 성공" << std::endl;
		}
		else
		{
			std::cout << "TestTimeParamSet 실패" << std::endl;
		}
	}
}

#if defined (PRE_ADD_BESTFRIEND)
void CDNQueryTest::QueryGetBestFriend(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetBestFriend pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TAGetBestFriend pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	
	int iRet = pWorld->QueryGetBestFriend( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetBestFriend 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryGetBestFriend 실패 Ret:" << iRet << std::endl;
	}
}
void CDNQueryTest::QueryRegistBestFriend(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQRegistBestFriend pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TARegistBestFriend pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "FromCharDBID>>" );
	cgets(szCmd);
	pQ.biFromCharacterDBID = _atoi64(szCmd);

	printf( "ToCharDBID>>" );
	cgets(szCmd);
	pQ.biToCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryRegistBestFriend( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryRegistBestFriend 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryRegistBestFriend 실패 Ret:" << iRet << std::endl;
	}
}
void CDNQueryTest::QueryCancelBestFriend(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQCancelBestFriend pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TACancelBestFriend pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "Cancel>>" );
	cgets(szCmd);
	pQ.bCancel = atoi(szCmd) > 0 ? true : false;

	int iRet = pWorld->QueryCancelBestFriend( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryCancelBestFriend 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryCancelBestFriend 실패 Ret:" << iRet << std::endl;
	}
}
void CDNQueryTest::QueryCloseBestFriend(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQCloseBestFriend pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TACloseBestFriend pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryCloseBestFriend( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryCloseBestFriend 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryCloseBestFriend 실패 Ret:" << iRet << std::endl;
	}
}
void CDNQueryTest::QueryEditBestFriendMemo(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQEditBestFriendMemo pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TAEditBestFriendMemo pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "Memo>>" );
	cgets(szCmd);
	MultiByteToWideChar( CP_ACP, 0, szCmd, -1, pQ.wszMemo, NAMELENMAX );

	int iRet = pWorld->QueryEditBestFriendMemo( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryEditBestFriendMemo 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryEditBestFriendMemo 실패 Ret:" << iRet << std::endl;
	}
}

#endif // #if defined (PRE_ADD_BESTFRIEND)


void CDNQueryTest::QueryGetListMyTrade(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetListMyTrade  pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TAGetListMyTrade  pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetListMyTrade( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "QueryGetListMyTrade 성공" << std::endl;
	}
	else
	{
		std::cout << "QueryGetListMyTrade 실패 Ret:" << iRet << std::endl;
	}
}

#if defined( PRE_ADD_LIMITED_SHOP )
void CDNQueryTest::QueryGetLimiatedItem(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQGetLimitedShopItem  pQ;
	memset( &pQ, 0, sizeof(pQ) );

	TAGetLimitedShopItem  pA;
	memset( &pA, 0, sizeof(pA) );

	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryGetDailyLimitedShopItem( &pQ, &pA );
	iRet = pWorld->QueryGetWeeklyLimitedShopItem( &pQ, &pA );
	if( iRet == ERROR_NONE )
	{
		std::cout << "GetLimitedItem 성공" << std::endl;
	}
	else
	{
		std::cout << "GetLimitedItem 실패 Ret:" << iRet << std::endl;
	}
}
void CDNQueryTest::QueryAddLimiatedItem(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQAddLimitedShopItem  pQ;
	memset( &pQ, 0, sizeof(pQ) );

	
	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	printf( "ItemID>>" );
	cgets(szCmd);
	pQ.ItemData.nItemID = atoi(szCmd);

	printf( "BuyCount>>" );
	cgets(szCmd);
	pQ.ItemData.nBuyCount = atoi(szCmd);
	
	int iRet = pWorld->QueryAdddailyLimitedShopItem( &pQ );
	iRet = pWorld->QueryAddWeeklyLimitedShopItem( &pQ );
	
	if( iRet == ERROR_NONE )
	{
		std::cout << "AddLimitedItem 성공" << std::endl;
	}
	else
	{
		std::cout << "AddLimitedItem 실패 Ret:" << iRet << std::endl;
	}
}
void CDNQueryTest::QueryResetLimiatedItem(std::vector<std::string>& vSplit)
{
	CDNSQLWorld* pWorld = g_SQLConnectionManager.FindWorldDB( 0, TEST_WORLD );
	_ASSERT( pWorld );

	TQResetLimitedShopItem  pQ;
	memset( &pQ, 0, sizeof(pQ) );


	char szCmd[256] = {0};

	printf( "CharDBID>>" );
	cgets(szCmd);
	pQ.biCharacterDBID = _atoi64(szCmd);

	int iRet = pWorld->QueryResetdailyLimitedShopItem( &pQ );
	iRet = pWorld->QueryResetWeeklyLimitedShopItem( &pQ );

	if( iRet == ERROR_NONE )
	{
		std::cout << "ResetLimitedItem 성공" << std::endl;
	}
	else
	{
		std::cout << "ResetLimitedItem 실패 Ret:" << iRet << std::endl;
	}
}
#endif
