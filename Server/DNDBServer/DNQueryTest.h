
#pragma once

#include "Singleton.h"

class CDNQueryTest:public CSingleton<CDNQueryTest>
{
public:

	CDNQueryTest();
	~CDNQueryTest();
	
	void ProcessQueryTest( const char* pszCmd );

private:

	void _QUERY_SELECTCHARACTER( std::vector<std::string>& vSplit );
	void _QUERY_UPDATE_DARKLAIR_RESULT( std::vector<std::string>& vSplit );
	void _P_GetKeySettingOption( std::vector<std::string>& vSplit );
	void _P_ModKeySettionOption( std::vector<std::string>& vSplit );

	void _P_GetListEquipment( std::vector<std::string>& vSplit );
	void _P_DelBlockedCharacter( std::vector<std::string>& vSplit );
	void _P_GetListBlockedCharacter( std::vector<std::string>& vSplit );
	void _P_AddBlockedCharacter( std::vector<std::string>& vSplit );
	void _P_DBMWTest( std::vector<std::string>& vSplit );

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	void QueryModNpcLocation( std::vector<std::string>& vSplit );
	void QueryModNpcFavor( std::vector<std::string>& vSplit );
	void QueryGetListNpcFavor( std::vector<std::string>& vSplit );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	void QueryGetMasterPupilInfo( std::vector<std::string>& vSplit );
	void QueryAddMasterCharacter( std::vector<std::string>& vSplit );
	void QueryDelMasterCharacter( std::vector<std::string>& vSplit );
	void QueryGetPageMasterCharacter( std::vector<std::string>& vSplit );
	void QueryGetMasterCharacter( std::vector<std::string>& vSplit );
	void QueryGetListPupil( std::vector<std::string>& vSplit );
	void QueryAddMasterAndPupil( std::vector<std::string>& vSplit );
	void QueryDelMasterAndPupil( std::vector<std::string>& vSplit );
	void QueryGetListMyMasterAndClassmate( std::vector<std::string>& vSplit );
	void QueryGetMyClassmate( std::vector<std::string>& vSplit );
	void QueryGraduate( std::vector<std::string>& vSplit );
	void QueryModRespectPoint( std::vector<std::string>& vSplit );
	void QueryModMasterFavorPoint( std::vector<std::string>& vSplit );
	void QueryGetMasterAndFavorPoint( std::vector<std::string>& vSplit );
	void QueryGetMasterSystemCountInfo( std::vector<std::string>& vSplit );

#if defined( PRE_ADD_SECONDARY_SKILL )
	void QueryAddSecondarySkill( std::vector<std::string>& vSplit );
	void QueryDelSecondarySkill( std::vector<std::string>& vSplit );
	void QueryGetListSecondarySkill( std::vector<std::string>& vSplit );
	void QueryModSecondarySkillExp( std::vector<std::string>& vSplit );
	void QuerySetManufactureSkillRecipe( std::vector<std::string>& vSplit );
	void QueryExtractManufactureSkillRecipe( std::vector<std::string>& vSplit );
	void QueryDelManufactureSkillRecipe( std::vector<std::string>& vSplit );
	void QueryGetListManufactureSkillRecipe( std::vector<std::string>& vSplit );
	void QueryModManufactureSkillRecipe( std::vector<std::string>& vSplit );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

	void QueryGetGuild( std::vector<std::string>& vSplit );
	void QueryDismissGuild( std::vector<std::string>& vSplit );
	void QueryGetGuildHistoryList( std::vector<std::string>& vSplit );
	void QueryGetGuildWareHistoryList( std::vector<std::string>& vSplit );
	void QueryGetGuildWarSchedule(std::vector<std::string>& vSplit);
	void QueryEnrollGuildWar(std::vector<std::string>& vSplit);
	void QueryGetGuildWareInfo(std::vector<std::string>& vSplit);
	void QueryModGuildData(std::vector<std::string>& vSplit);

	void QueryGetListFarm( std::vector<std::string>& vSplit );
	void QueryGetListField( std::vector<std::string>& vSplit );
	void QueryAddField( std::vector<std::string>& vSplit );
	void QueryDelField( std::vector<std::string>& vSplit );
	void QueryAddFieldAttachment( std::vector<std::string>& vSplit );
	void QueryModFieldElapsedTime( std::vector<std::string>& vSplit );
	void QueryHarvest( std::vector<std::string>& vSplit );
	void QueryGetListHarvestDepotItem( std::vector<std::string>& vSplit );
	void QueryGetFieldCountByCharacter( std::vector<std::string>& vSplit );
	void QueryGetFieldItemCount( std::vector<std::string>& vSplit );

	void QueryGetListFieldForCharacter( std::vector<std::string>& vSplit );
	void QueryAddFieldForCharacter( std::vector<std::string>& vSplit );
	void QueryDelFieldForCharacter( std::vector<std::string>& vSplit );
	void QueryAddFieldForCharacterAttachment( std::vector<std::string>& vSplit );
	void QueryModFieldForCharacterElapsedTime( std::vector<std::string>& vSplit );
	void QueryHarvestForCharacter( std::vector<std::string>& vSplit );

	void QueryChangeCharacterName(std::vector<std::string>& vSplit);
	void QueryAddPvPLadderResult( std::vector<std::string>& vSplit );
	void QueryGetListPvPLadderScore( std::vector<std::string>& vSplit );
	void QueryGetListPvPLadderScoreByJob( std::vector<std::string>& vSplit );
	void QueryInitPvPLadderGradePoint( std::vector<std::string>& vSplit );
	void QueryUsePvPLadderPoint( std::vector<std::string>& vSplit );
	void QueryGetListPvPLadderRanking( std::vector<std::string>& vSplit );

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	void QueryChangeJobCode( std::vector<std::string>& vSplit );
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM
	void QueryCompleteEventQuest( std::vector<std::string>& vSplit );
	void QueryCheckPCRoomIP( std::vector<std::string>& vSplit );

	void QueryAddAbuseMonitor( std::vector<std::string>& vSplit );
	void QueryDelAbuseMonitor( std::vector<std::string>& vSplit );
	void QueryGetAbuseMonitor( std::vector<std::string>& vSplit );

	void QueryGetListVariableReset( std::vector<std::string>& vSplit );
	void QueryModVariableReset( std::vector<std::string>& vSplit );

	void QueryGetCharacterPartialy7( std::vector<std::string>& vSplit );

	void QueryGetListRepurchaseItem( std::vector<std::string>& vSplit );
	void QueryGetPvPScore ( std::vector<std::string>& vSplit );
	void QueryModPvPScore ( std::vector<std::string>& vSplit );
	void QueryAddPvPResultLog ( std::vector<std::string>& vSplit );
#if defined( PRE_PARTY_DB )
	void QueryAddParty( std::vector<std::string>& vSplit );
	void QueryModParty( std::vector<std::string>& vSplit );
	void QueryDelParty( std::vector<std::string>& vSplit );
	void QueryDelPartyForServer( std::vector<std::string>& vSplit );
	void QueryJoinParty( std::vector<std::string>& vSplit );
	void QueryOutParty( std::vector<std::string>& vSplit );
	void QueryGetListParty( std::vector<std::string>& vSplit );
#if defined( PRE_WORLDCOMBINE_PARTY )
	void QueryGetListPartyCode( std::vector<std::string>& vSplit );
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	void QueryGetListPartyMember( std::vector<std::string>& vSplit );
	void QueryModPartyLeader( std::vector<std::string>& vSplit );
#endif // #if defined( PRE_PARTY_DB )
	void TestTimeParamSet(std::vector<std::string>& vSplit);
#if defined (PRE_ADD_BESTFRIEND)
	void QueryGetBestFriend(std::vector<std::string>& vSplit);
	void QueryRegistBestFriend(std::vector<std::string>& vSplit);
	void QueryCancelBestFriend(std::vector<std::string>& vSplit);
	void QueryCloseBestFriend(std::vector<std::string>& vSplit);
	void QueryEditBestFriendMemo(std::vector<std::string>& vSplit);
#endif // #if defined (PRE_ADD_BESTFRIEND)

	void QueryGetListMyTrade(std::vector<std::string>& vSplit);

#if defined( PRE_ADD_LIMITED_SHOP )
	void QueryGetLimiatedItem(std::vector<std::string>& vSplit);
	void QueryAddLimiatedItem(std::vector<std::string>& vSplit);	
	void QueryResetLimiatedItem(std::vector<std::string>& vSplit);	
#endif

#if defined( _WORK )
	static UINT __stdcall PerformanceThread(void *pParam);
	void QueryPerformanceTest( std::vector<std::string>& vSplit );
#endif // #if defined( _WORK )

};
