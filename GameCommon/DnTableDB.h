#pragma once

#include "Singleton.h"
#include "DNTableFile.h"

class CDnTableDB : public CSingleton<CDnTableDB> {
public:
	CDnTableDB();
	~CDnTableDB();
	
	enum TableEnum {
		TACTOR = 0,
		TPARTS,
		TWEAPON,
		TTILE,
		TENVIEFFECT,
		TMAP,
		TPLAYERLEVEL,
		TMONSTER,
		TMONSTERCOMPOUND,
		TITEM,
		TPROP,
		TJOB,
		TSKILL,
		TITEMDROP,
		TDEFAULTCREATE,
		TNPC,
		TQUEST,
		TPLAYERWEIGHT,
		TTALKPARAM,
		TSKILLLEVEL,
		TMAPGATE,
		TSOCIAL,
		TMONSTERSKILL,
		TSHOP,
		TMONSTERPARTS, 
		TSKILLSHOP,
		TSUMMONPROP,
		TDUNGEONENTER,
		TDUNGEONCLEAR,
		TCLEARSTANDARD,
		TSYMBOLITEM,
		TCUTSCENE,
		TPLATE,
		TITEMCOMPOUND,
		TSTATEEFFECT,
		TDUNGEONMAP,
		TTODAYTIP,
		TFILE,
		TQUESTREWARD,
		TMONSTERWEIGHT,
		TQUESTCHAPTER,
		TEQUIPMATCHACTION,
		TFACE,
		TREBIRTHCOIN,
		TPVPMAP,
		TPVPGAMEMODE,
		TPVPGAMESTARTCONDITION,
		TCP,
		TGAMEROOMNAME,
		TGLOBALWEIGHT,
		TGLOBALWEIGHTINT,
#ifdef PRE_FIX_MEMOPT_ENCHANT
		TENCHANT_MAIN,
#else
		TENCHANT,
#endif
		TPOTENTIAL,
		TSETITEM,
		TEQUALLEVEL,
		TMISSION,
		TAPPELLATION,
		TPVPRANK,
		TPVPWINCONDITION,
		TITEMCOMPOUNDSHOP,
		TEQUALBASIS,		
		TDAILYMISSION,
		TSCHEDULE,
		TWORLDMAPUI,
		TTAX,
		TMISSIONSCHEDULE,
		TRADIOMSG,
		TGESTURE,
		TMONSTERGROUP,
		TMONSTERSET,
		TSKILLTREE,
		TITEMDROPGROUP,
		TGUILDUSERCAP,
		TDLMAP,
		TDLDUNGEONCLEAR,
		TCASHCOMMODITY,			//이녀석과
		TCASHGOODSINFO,
		TCASHPACKAGE,			//이녀석은 라이브도중 리로드가 추가되었다. 서버기동시 이외에 Sox파일을 다이렉트로 억세스하지말자
		TCASHLIMIT,
		TWEATHER,
		TPCCAFE,
		TFATIGUEADJUST,
		TCASHPRESENTREACT,
#ifdef PRE_ADD_GACHA_JAPAN
		TGACHA_JP,
		TGACHASETITEM_JP,
#endif
		TSEALCOUNT,
		TLEVELUPEVENT,
#if defined(PRE_ADD_VIP)
		TVIP,
		TVIPGRADE,
#endif	// #if defined(PRE_ADD_VIP)
		TCASHTAB,
		TFAIRY,
		TCOSMIX,
		TCOSMIXABILITY,
		TGLYPHSKILL,
		TGLYPHSLOT,
		TLEVELPROMO,

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		TREPUTE,
		TPRESENT,
		TNPCREACTION,
		TSTOREBENEFIT,
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		TMAIL,
		TMODDLG,
		TMASTERSYSTEM_DECREASE,
		TMASTERSYSTEM_GAIN,

		TVEHICLE,
		TVEHICLEPARTS,
		TPOTENTIALJEWEL,
		TEVENTPOPUP,
		TCHARMITEM,
		TCHARMITEMKEY,
#if defined( PRE_ADD_SECONDARY_SKILL )
		TSecondarySkill,
		TSecondarySkillLevel,
		TSecondarySkillRecipe,
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
		TSKILLVIDEO,
		TGUILDWAREHOUSE,
		TFARMCULTIVATE,
		TFARMSKIN,
		TFISHING,
		TFISHINGPOINT,
		TFISHINGAREA,
		TFISHINGROD,
		TQUESTPERIOD,
		TGLOBALEVENTQUEST,
		TSTAFFROLL,
		TCOLLECTIONBOOK,
		TEXCHANGETRADE,
		TSKILLBUBBLE,
		TSKILLBUBBLEDEFINE,
		TENCHANTJEWEL,
		TPVPMISSIONROOM,
		TEQUALEXPONENT,
		TITEMCOMPOUNDGROUP,
		TEXPADJUST,
		TSTAGEREWARD,
		TMONSTERSKILL_TRANS,
		TMONSTER_TRANS,
		TMONSTERGROUP_TRANS,
		TGHOULMODE_CONDITION,
		TPVPGAMEMODESETTING,
		TPVPGAMEMODESKILLSETTING,
		TMONSTER_TRANS_RADIOMSG,
		TMONSTER_TRANS_RADIOBASE,
		TCONNECTINGTIME,
		THELP,
		TGUIDEPOPUP,
		TREPUTEBENEFIT,
		THELPKEYWORD,
		TINSTANTITEM,
		TGUILDMARK,
		TBATTLEGROUNDMODE,
		TBATTLEGROUNDRESWAR,
		TBATTLEGROUNDSKILLTREE,
		TBATTLEGROUNDSKILLLEVEL,
		TBATTLEGROUNDSKILL,
		TRESOURCEWARSKIN,
		TUNION,
		TGUILDWARREWARD,	// 길드전 보상
		TPLAYERCOMMONLEVEL,
		TGACHAINFO,
		TPETLEVEL,
		TPETSKILLLEVEL,
		TMASTERSYSFEEL,
		TATTENDANCEEVENT,
#ifdef PRE_ADD_CASH_AMULET
		TCASHCHATBALLOONTABLE,
#endif
		TTRIGGERVARIABLETABLE,
		TLOADINGANI,
		TITEMOPTION,
		TVILLAGEALLOWEDSKILL,
		TCOMBINEDSHOP,
		TGLYPHCHARGE,
#ifdef PRE_FIX_MEMOPT_ENCHANT
		TENCHANT_STATE,
		TENCHANT_NEEDITEM,
#endif
#ifdef PRE_ADD_SHUTDOWN_CHILD
		TSHUTDOWNMSG,
#endif
		TPLAYERCUSTOMEVENTUI,
		TQUEST_LEVELCAP_REWARD,
		TGUILDLEVEL,
		TGUILDWARPOINT,
		TGUILDREWARDITEM,	//길드보상아이템
		TWING,
#ifdef PRE_ADD_MODIFY_PLAYER_CANNON
		TCANNON,
#endif
#ifdef PRE_ADD_COSRANDMIX
		TCOSMIXINFO,
		TCOSRANDOMMIXINFO,
#endif
#if defined(PRE_ADD_SALE_COUPON)
		TSALECOUPON,
#endif
#ifdef PRE_ADD_DONATION
		TCONTRIBUTION,
#endif
		TITEMPERIOD,
		TCONTROLKEY,
#if defined(PRE_ADD_COUNTRY_BY_COUNTRY_CONTROL)
		TCONTROLKEY_ESP,
		TCONTROLKEY_FRA,
		TCONTROLKEY_GER,
#endif	// #if defined(PRE_ADD_COUNTRY_BY_COUNTRY_CONTROL)

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		TNAMEDITEM,
#endif
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		TPOTENTIAL_TRANS,
#endif		//#ifdef PRE_ADD_EXCHANGE_POTENTIAL
#if defined( PRE_PARTY_DB )
		TPARTYSORTWEIGHT,
#endif // #if defined( PRE_PARTY_DB )
		TCASHERASABLETYPE,
		TPETFOOD,
		TPETCHAT,
#if defined (PRE_ADD_CHAOSCUBE)
		TCHAOSCUBESTUFF,
		TCHAOSCUBERESULT,
#endif
#if defined(PRE_MOD_SELECT_CHAR)
		TCAMERA,
		TTITLE,
		TDEFAULTCREATECOSTUME,
#endif // PRE_MOD_SELECT_CHAR
#if defined(PRE_ADD_INSTANT_CASH_BUY)
		TCASHBUYSHORTCUT,
#endif // PRE_ADD_INSTANT_CASH_BUY
#if defined( PRE_WORLDCOMBINE_PARTY )
		TWORLDCOMBINEPARTY,
#endif
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM)
		TPCBANGRENTALITEM,
#endif
#if defined(PRE_ADD_63603)
		TITEMUSEINFO,
#endif // PRE_ADD_63603
#if defined( PRE_ADD_EXCHANGE_ENCHANT )
		TENCHANTTRANSFER,
#endif
#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
		TITEMCATEGORYDESC,
#endif
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		TTOTALLEVELSKILL,
		TTOTALLEVELLSKILLSLOT,
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
#if defined(PRE_ADD_WEEKLYEVENT)
		TWEEKLYEVENT,
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
#if defined( PRE_ADD_PRESET_SKILLTREE )
		TSKILLTREE_ADVICE,
#endif		// #if defined( PRE_ADD_PRESET_SKILLTREE )
#if defined(PRE_SPECIALBOX)
		TKEEPBOXPROVIDEITEM,
#endif	// #if defined(PRE_SPECIALBOX)
#if defined( PRE_ADD_SHORTCUT_HELP_DIALOG )
		THELPSHORTCUT,
#endif
#ifdef PRE_ADD_JOINGUILD_SUPPORT
		TGUILDSUPPORT,
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
#ifdef PRE_ADD_STAGE_DAMAGE_LIMIT
		TSTAGEDAMAGELIMIT,
#endif
#if defined(PRE_ADD_REMOTE_QUEST)
		TREMOTEQUEST,
#endif

#if defined(PRE_ADD_ACTIVEMISSION)
		TACTIVEMISSION,
		TACTIVESET,
#endif // PRE_ADD_ACTIVEMISSON

#if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
		TCHARMCOUNT,
#endif	// #if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)

#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
		TBONUSDROP,
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
#if defined( PRE_WORLDCOMBINE_PVP )
		TWORLDPVPMISSIONROOM,
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		TDROPITEMENCHANT,
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
#if defined( PRE_ALTEIAWORLD_EXPLORE )
		TALTEIAWORLDMAP,		
		TALTEIARANK,
		TALTEIAWEEKREWARD,
		TALTEIATIME,
		TALTEIAPREVIEWREWARD,
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )
#if defined( PRE_ADD_STAMPSYSTEM )
		TSTAMPCHALLENGE,
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined( PRE_ADD_TALISMAN_SYSTEM )
		TTALISMANITEM,
		TTALISMANSLOT,
#endif // #if defined( PRE_ADD_TALISMAN_SYSTEM )
#if defined( PRE_ADD_DWC )
		TDWCCREATETABLE,
#endif // #if defined( PRE_ADD_DWC )
#if defined( PRE_PVP_GAMBLEROOM )
		TPVPGAMBLEROOM,
#endif
#ifdef PRE_ADD_ITEM_GAINTABLE
		TITEMGAIN,
#endif
#if defined( PRE_ADD_STAGE_WEIGHT )
		TSTAGEWEIGHT,
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
		TOVERLAPSETEFFECT,
#endif
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
		TLEVELUPCHECKBOX,
#endif
#ifdef PRE_ADD_PVPRANK_INFORM
		TPVPRANKREWARD,
#endif
#if defined(PRE_ADD_CHAT_MISSION)
		TMISSIONTYPING,
#endif

#ifdef PRE_ADD_CRAZYDUC_UI
		TEVENTMONSTER,
#endif 

#ifdef PRE_ADD_CASHINVENTAB
		TCASHINVENTAB,
#endif / /PRE_ADD_CASHINVENTAB

		TREBIRTH,
		TREBIRTHREWARD,
		TRLKTSETTINGS,
		TCHARCREATE,
		TPARTSSKINCOMBINE,
		TSTAGECLEARDROP,
		TDISJOINTINFO, //disjoint item info
		TDRAGONJEWELSLOTTABLE,
		TDRAGONJEWELTABLE,
	};

	enum MailTableInfoEnum 
	{
		MailInfoRewardCoin,
		MailInfoRewardItemID,
		MailInfoRewardItemCount,
	};

protected:
	void FindExtFileList( const char* szFileName, std::vector<CFileNameString>& szVecList );

	std::map<TableEnum, DNTableFileFormat*> m_MapTables;
#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	DNTableFileFormat* LoadTable( const char *szFileName, std::string strGenerationLabel=std::string(), bool bDontUseItemIDData = true );
#else // PRE_ADD_REDUCE_TABLE_RESOURCE
	DNTableFileFormat* LoadTable( const char *szFileName, std::string strGenerationLabel=std::string() );
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE
	void UnloadTable(TableEnum Index);
	void AddTable(TableEnum Index, DNTableFileFormat *pSox );
	void RemoveTable(TableEnum Index);

private:
	DNTableFileFormat* ForceLoadSox( const char *szFileName, std::string &szGenerationLabelStr);
	DNTableFileFormat* RawLoadSox(const char *szFileName, std::vector<CStream *> &pVecStream);

public:
	DNTableFileFormat* GetTable( TableEnum Index );
	DNTableFileFormat* ReLoadSox(TableEnum Index);

	bool Initialize();
	void Finalize();

	// FileTable 도움 함수
	const char* GetFileName( int nIndex );
	int GetMailInfo( int nIndex, MailTableInfoEnum eMailInfo, bool bCashInfo = false, int nInfoIndex = 0 );

#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	void SetTableFileInfo();
	void AddTableFileInfo( TableEnum Index, const char* strFileName, const char* strGenerationLabel = NULL, bool bUseGame = false, bool bDontUseItemIDData = true );
	void AddTableByIndex( TableEnum eTableIndex );
	void SetTableAutoUnloadForGame( bool bTableUseInGame );
	void SetTableAutoUnloadByIndex( TableEnum eTableIndex, float fHoldingTime );
	void Process( float fElapsedTime );

private:
	struct stFileInfo
	{
		std::string m_strFileName;
		std::string m_strGenerationLabel;
		bool m_bUseGame;
		bool m_bDontUseItemIDData;
	};
	std::map<TableEnum, stFileInfo> m_MapTableFileInfo;
	bool m_bTableUseInGame;
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE
};

#define GetTableDB()	CDnTableDB::GetInstance()
#define GetDNTable(table)	CDnTableDB::GetInstance().GetTable( table )

class CPlayerWeightTable : public CSingleton<CPlayerWeightTable> {
public:
	CPlayerWeightTable();
	virtual ~CPlayerWeightTable();

	enum WeightTableIndex {
		HP = 0,
		SP,
		RecoverySP,
		StrengthAttack,
		AgilityAttack,
		IntelligenceAttack,
		PhysicalDefense,
		MagicDefense,
		Critical,
		CriticalResistance,
		Stiff,
		StiffResistance,
		Stun,
		StunResistance,
		MoveSpeed,
		MoveSpeedRevision,
		DownDelay,
		ElementAttack,
		ElementDefense,
		ElementDefenseMin,
		ElementDefenseMax,
		SpiritDefault,
		SpiritIncreaseRevision,
		SpiritDecreaseRevision,

		Amount,

	};

protected:
	float m_fPlayerWeightValue[10][Amount];

public:
	float GetValue( int nClassID, WeightTableIndex Index ) { return m_fPlayerWeightValue[nClassID-1][Index]; }
};

#ifdef _GAMESERVER
class CDNGameRoom;
#else
class CDnGameTask;
#endif

class CGlobalWeightTable : public CSingleton<CGlobalWeightTable> {
public:
	CGlobalWeightTable();
	virtual ~CGlobalWeightTable();

	//	note by kalliste : 테이블 ID보다 하나 적은 숫자 할당해야 함
	enum WeightTableIndex {
		CriticalIncrease,
		DefenseValue,
		CriticalValue,
		StiffRevision,
		StunRevisioin,
		StateRevision,
		StageGiveupDurabilityPenalty,
		CriticalResistance,
		DefenseMax,
		CriticalMax,
		CriticalResistanceMax,
		StiffMax,
		StunRevision2,
		StunMax,
		ScaleWeightValue,
		SpiritMax,
		ExpPartyBonus,
		ExpPenaltyValue1,
		ExpPenaltyValue2,
		ExpPenaltyMin,
		PvPRevision,
		RepairDurabilityRevision,
		MonsterDeadDurabilityRevision,
		EnchantWorldNoticeValue,
		DropWorldNoticeValue,
		PlayerLevelLimit,
		EventToday,
		DLRevision,
		CostumeMix_MaxStuffCount_KOR,
		CostumeMix_MaxStuffCount_JPN,
		FinalDamageValue1,
		FinalDamageValue2,
		FinalDamageValue3,
		CharacterSlotMax,
		FinalDamageValue4,
		CriticalVelocityRevision,
		StunVelocityRevision,
		DNTVBrowserWidth = 37,
		DNTVBrowserHeight = 38,
		NxCreditUsableMinLevel = 39,
		ElementMinRevision = 40,

		// #31367 이슈로 추가됨. 데미지 변경 상태효과 및 공격력 비율 변경 상태효과의 최저값 셋팅됨.
		AttackPowerClampLowestRatio = 41,
		DamageChangeClampLowestRatio = 42,

		MarketRegisterTime_Min = 49,
		MarketRegisterTime0 = MarketRegisterTime_Min,	// [거래소_무료시간0]
		MarketRegisterTime1 = 50,						// [거래소 유료시간 1]
		MarketRegisterTime2 = 51,
		MarketRegisterTime3 = 52,
		MarketRegisterTime4 = 53,
		MarketRegisterTime5 = 54,
		MarketRegisterTime6 = 55,
		MarketRegisterTime7 = 56,
		MarketRegisterTime8 = 57,
		MarketRegisterTime9 = 58,
		MarketRegisterTime_Max = MarketRegisterTime9,

		MarketItemRegisterCount_Basic = 59,
		MarketRegisterCount_Basic = 60,

		CoolTimeAccellSE_Max = 61,

		DailyMailCount = 69,

		PlayerAbsenceTime = 70,
		PartyLeaderAbsenceTime = 71,

		MaxLevelExpTransGoldValue = 75,

		CharmItemEmptySlotCount = 77,
		CharmItemOpenTime = 78,

		MasterSystem_MasterMinLevel = 81,
		MasterSystem_PupilMaxLevel = 82,
		MasterSystem_GraduateLevel = 83,
		MasterSystem_ClassmateCountExpBonusRate = 84,
		MasterSystem_TreasureBoxBonusRate = 85,
		MasterSystem_MasterCountExpBonusRate = 86,

		CharmItempDisplayDelayTime = 87,	//매력 아이템 표시 간격 시간 설정
		CostumeRandomMixPreviewChangePartsTime = 88,

		MarketPremiumTime_Min = 89,
		MarketPremiumTime0 = MarketPremiumTime_Min,
		MarketPremiumTime1 = 90,
		MarketPremiumTime2 = 91,
		MarketPremiumTime3 = 92,
		MarketPremiumTime4 = 93,
		MarketPremiumTime5 = 94,
		MarketPremiumTime6 = 95,
		MarketPremiumTime7 = 96,
		MarketPremiumTime8 = 97,
		MarketPremiumTime9 = 98,
		MarketPremiumTime_Max = MarketPremiumTime9,

		PvPLevelWeight = 99,
		LevelWeight = 100,

		DefenseConstant = 200,
		CriticalConstant = 201,
		FinalDamageConstant = 202,

		CriticalIncrease_PVP = 203,
		StiffMax_PVP = 204,

		DWC_DefenseConstant = 205,
		DWC_CriticalConstant = 206,
		DWC_FinalDamageConstant = 207,

#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
		DragonFellowShipGlyph = 249,
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
		CompoundLimitMaxLevel = 250, 
#endif 
		FarmChatRoomDistance = 303,
		FarmChatBalloonDistance = 304,

		PartyMarkerDistance = 349,

		FatigueLimit = 400,
		ShutDownSystemStartTime = 401,
		ShutDownSystemEndTime = 402,
		ShutDownSystemLimitAge = 403,
		PotentialMovealbleCount = 404,
		AllGiveCharmItemOpenTime = 405,
		ExpUpItem = 406,				// 경험치 증가 아이템(절댓값)
		PvPExpUpItem = 407,				// 콜로세움 경험치 증가 아이템 사용 횟수

#if defined( PRE_PVP_GAMBLEROOM )		
		PvPGambleFirstWinnerRate = 408,	// 도박모드 우승자 상금 비율
		PvPGambleSecondWinnerRate = 409,	// 도박모드 준우승자 상금 비율
		PvPGambleCumulativeRate = 410,		// 도박모드 적립금 비율
		PvPGambleRoomMaxCount = 411,		// 도박모드 최대 생성갯수
#endif
		
		RangeFallGravityAdditionalProjectileRange = 499,		// #36693 RangeFallGravity 로 DestroyOrbit 이 셋팅된 발사체인 경우 주어질 추가 사거리 값.
		MonsterElementMinRevision = 500,	//몬스터 속성 방어 최소 수치

		Element_Attack_Min = 501, // PlayerState
		Element_Attack_Max = 502, // PlayerState

		Element_Defense_Min = 503, // PlayerState + MonsterState
		Element_Defense_Max = 504, // PlayerState + MonsterState 

		Physical_AttackRatio_Min = 505, // PlayerState
		Physical_AttackRatio_Max = 506, // PlayerState
		Magical_AttackRatio_Min = 507, // PlayerState
		Magical_AttackRatio_Max = 508, // PlayerState 
		
		Freezing_AddDamageRatio = 509,     // 상태효과 제어

		Freezing_AdddamageRatio_Max = 510, // 상태효과 제어

		Change_DamageTaken_Physical_Min = 511, // Player  // 상태효과 제어
		Change_DamageTaken_Physical_Max = 512, // Monster // 상태효과 제어
		Change_DamageTaken_Magical_Min = 513,  // Player  // 상태효과 제어
		Change_DamageTaken_Magical_Max = 514,  // Monster // 상태효과 제어

		Element_EqualBasis = 515,            // Element 보정 값 , 기존 Level / Basis 값을 쓰는 값들과 달리 Element는 고정형으로 사용한다 = < 기획에서 정함 >
		Element_EqualLevel = 516,            // Element 보정 값 , 기존 Level / Basis 값을 쓰는 값들과 달리 Element는 고정형으로 사용한다 = < 기획에서 정함 >
		Element_Expotent_Limit = 517,        // Element 보정영향 받지않는 최소치를 정하기로 기획팀에서 정함

		Strength_Ratio_Max = 518, 
		Strength_Ratio_Min = 519, 
		Agility_Ratio_Max = 520, 
		Agility_Ratio_Min = 521, 
		Intelligence_Ratio_Max = 522, 
		Intelligence_Ratio_Min = 523, 
		Stamina_Ratio_Max = 524, 
		Stamina_Ratio_Min = 525,
		Element_Defense_Limit = 526,
		Chalange_DarkLair_Heal_Limit = 527,

		MoveSpeedRatio_Limit_Max = 528,
		MoveSpeedRatio_Limit_Min = 529,
		FrameSpeedRatio_Limit_Max = 530,
		FrameSpeedRatio_Limit_Min = 531,
		Freezing_AddDamageRatio_PVP = 532,
		Freezing_AdddamageRatio_PVP_Max = 533,


		PvPLadder_LimitLevel = 999,
		PvPLadder_ForceMatchingSec = 1000,
		PvPLadder_RegularSeason = 1001,
		PvPLadder_IncreaseHiddenPointPerSec = 1002,
		PvPLadder_SectionRange= 1003,
		PvPLadder_MinSampleCount = 1004,

		LimitLevel_SendMail = 1005,
		LimitLevel_Market = 1006,
		LimitLevel_Exchange = 1007,
		LimitLevel_MasterSystem = 1008,

		Disjoint_ItemLimitLevel = 1010,
		Disjoint_LimitUserLevel = 1011,

		Shop_Repurchase_Fee = 1012,
		GuildWar_Rank_Blind = 1013,
		PvPLadder_LimitPoint = 1014,
		GuildWarMission_LevelLimit = 1015,
		GuildWarMission_Rate = 1016,
		FatigueDropItemPenalty_Rate = 1017,
		Colosseum_Limit_Rank = 1018,

		Login_Title_MapIndex = 1019,
		Login_Char_MapIndex = 1020,
		Login_Max_CreateCharCount = 1021,
		Login_Base_CreateCharCount = 1022,
		BestFriend_CloseMail = 1023,
		BeginnerGuild_GraduateLevel = 1024,
		BeginnerGuild_UserMax = 1025,	// 초보자길드 제한인원 (현재 클라이언트에서는 미사용)
		BeginnerGuild_MailIndex = 1026, // 초보자길드 가입 보상 관련 메일 인덱스 (현재 클라이언트에서는 미사용)

		Login_Gesture1 = 1027,
		Login_Gesture2 = 1028,
		Login_Gesture3 = 1029,
		Login_Gesture4 = 1030,
		Login_Gesture5 = 1031,

		DoorsMobile_AuthRefreshSec = 1032,

		ITEMUPGRADE_COOLTIME_NOMAL_MAGIC_ITEM = 1033,
		ITEMUPGRADE_COOLTIME_RARE_EPIC_ITEM = 1034,
		ITEMUPGRADE_COOLTIME_ABOVE_UNIQUE = 1035,

		Skill_ResetComment_Level = 1036,
		MasterSystem_MasterAndPupilAllowLevelGap = 1037,
		WorldMessageRedCoolTime = 1038,
		
		MissionHelper_AutoRegisterLevel = 1042,
		ItemUnLockWaitTime = 1043,				// 아이템 잠금 해제 대기 시간

		DragonBuffWaitTime = 1044,				// 드래곤버프퀘스트 대기 시간

		SERVER_WAREHOUSE_POPUP = 1099,			// 템 꺼낼때 확인창 출력여부(0-출력x, 1-출력)
		SERVER_WAREHOUSE_INVENCOUNT = 1100,		// 기본 서버 일반창고 제공칸수
		SERVER_WAREHOUSE_CASHCOUNT = 1101,		// 기본 서버 캐쉬창고 제공칸수
		SERVER_WAREHOUSE_PAYMENT = 1102,		// 사용할 유료화 타입(0-무료(수수료), 1=코인소모)
		SERVER_WAREHOUSE_IN_LEVELLIMIT = 1103,	// 창고에 아이템 넣을때 레벨 제한
		SERVER_WAREHOUSE_OUT_LEVELLIMIT = 1104,	// 창고에 아이템 꺼낼때 레벨 제한

		AlteiaWorldDailyPlayCount = 1105,		// 알테이아 일일 플레이횟수
		AlteiaWorldJoinTicketCount = 1106,		// 알테이아 입장시 필요한 입장권수
		AlteiaWorldSendTicketCount = 1107,		// 알테이아 보낼수 있는 입장권수
		AlteiaWorldDailyTicketCount = 1108,		// 알테이아 일일 지급 입장권수
		AlteiaWorldEnterLimitStartTime = 1109,	// 알테이아 진입금지 시작시간(분)
		AlteiaWorldEnterLimitEndTime = 1110,	// 알테이아 진입금지 끝시간(분)
		AlteiaWorldEnterLimitLevel = 1111,		// 알테이아 입장 가능 레벨
		AlteiaWorldAutoDiceTimer = 1112,		// 알테이아 자동 주사위 굴림 타이머

		Amount = 1200,		
	};
protected:
	float m_fGlobalWeightValue[Amount];

public:

	float GetValue( WeightTableIndex Index );
};


class CGlobalWeightIntTable : public CSingleton<CGlobalWeightIntTable> {
public:
	CGlobalWeightIntTable();
	virtual ~CGlobalWeightIntTable();

	//	note by kalliste : 테이블 ID보다 하나 적은 숫자 할당해야 함
	enum WeightTableIndex {
		AlteiaWorldBuffItemID = 99,
		AlteiaWorldGoldKeyItemID = 100,
		CheckInvenCountBasis = 9999,
		InvenCountUnderBasis = 10000,
		InvenCountOverBasis = 10001,
		WareCountUnderBasis = 10002,
		WareCountOverBasis = 10003,
		CharmItemEmptyCoinAmount = 10004,
		ComebackGiveTerm = 10005,
		CadgeMaxCountOneDay = 10006,
		ChaosCubeEmptySlotCount = 10007,
		ComebackEffectItemID = 10008,

		NewbieRewardTime = 10010,
		MaxSeedPoint = 10011,	// 시드최대크기(귀속화폐)

		Amount = 10100,
	};
protected:
	int m_nGlobalWeightValue[Amount];

public:
	int GetValue( WeightTableIndex Index );
};


class CPlayerLevelTable : public CSingleton<CPlayerLevelTable> {
public:
	CPlayerLevelTable();
	virtual ~CPlayerLevelTable();

	enum PlayerLevelTableIndex {
		Strength,
		Agility,
		Intelligence,
		Stamina,
		Experience,
		SuperAmmor,
		SkillPoint,
		Fatigue,
		DeadDurabilityRatio,
		KillScore,
		AssistScore,
		AssistDecision,
		AggroPer,
		WeekFatigue,
		SPDecreaseRatio,
#ifdef PRE_MONITOR_SUPER_NOTE
		IllegalAmount,
		BanAmount,
#endif
		AggroPerPvE,
#if defined(PRE_ADD_VIP)
		VIPFatigue,
		VIPExp,
#endif	// #if defined(PRE_ADD_VIP)
		LevelDExp,		// 피로도 소모를 통한 하루 습득 경험치.

		Amount,
	};

protected:
	struct LevelValue {
		union {
			int nValue[Amount];
			float fValue[Amount];
		};
	};
	std::map<int, LevelValue *> m_nMapTable;

public:
	int GetValue( int nClassID, int nLevel, PlayerLevelTableIndex Index );
	float GetValueFloat( int nClassID, int nLevel, PlayerLevelTableIndex Index );
};

#define GetPlayerLevelTable()	CPlayerLevelTable::GetInstance()

class CEqualLevelTable : public CSingleton<CEqualLevelTable> {
public:
	CEqualLevelTable();
	virtual ~CEqualLevelTable();

	enum EqualLevelTableIndex {
		PhysicalAttackMinRevision,
		PhysicalAttackMaxRevision,
		MagicAttackMinRevision,
		MagicAttackMaxRevision,
		PhysicalDefenseRevision,
		MagicDefenseRevision,
		MaxHPRevision,
		MaxSPRevision,
		RecoverySPRevision,
		StiffRevision,
		StiffResistanceRevision,
		CriticalRevision,
		CriticalResistanceRevision,
		StunRevision,
		StunResistanceRevision,
		SuperAmmorRevision,
		MoveSpeedRevision,
		ElementAttackRevision,
		ElementDefenseRevision,
		FinalDamageRevision,
		StrengthRevision,
		AgilityRevision,
		IntelligenceRevision,
		StaminaRevision,
		Amount,
	};

protected:
	struct LevelValue {
		union {
			int nValue[Amount];
			float fValue[Amount];
		};
	};

	int m_iMaxMode;
	std::map<int, LevelValue *>* m_pMapTable;

public:
//	int GetValue( int nClassID, EqualLevelTableIndex Index );

	int GetValue( int nMode, int nJobID, EqualLevelTableIndex Index );
	float GetValueFloat( int nMode, int nJobID, EqualLevelTableIndex Index );
};

class CEqualBasisTable : public CSingleton<CEqualBasisTable> {
public:
	CEqualBasisTable();
	virtual ~CEqualBasisTable();

	enum EqualBasisTableIndex {
		PhysicalAttackMinRevision,
		PhysicalAttackMaxRevision,
		MagicAttackMinRevision,
		MagicAttackMaxRevision,
		PhysicalDefenseRevision,
		MagicDefenseRevision,
		MaxHPRevision,
		MaxSPRevision,
		RecoverySPRevision,
		StiffRevision,
		StiffResistanceRevision,
		CriticalRevision,
		CriticalResistanceRevision,
		StunRevision,
		StunResistanceRevision,
		SuperAmmorRevision,
		MoveSpeedRevision,
		FinalDamageRevision,
		ElementAttackRevision,
		ElementDefenseRevision,
		StrengthRevision,
		AgilityRevision,
		IntelligenceRevision,
		StaminaRevision,
		Amount,
	};

protected:
	struct LevelValue {
		int nValue[Amount];
	};
	std::map<int, LevelValue *> m_nMapTable;

public:
	int GetValue( int nClassID, int nLevel, EqualBasisTableIndex Index );
};

class CFatigueAdjustmentTable : public CSingleton<CFatigueAdjustmentTable> {
public:
	CFatigueAdjustmentTable();
	virtual ~CFatigueAdjustmentTable();

protected:
	std::vector<float*> m_fVecValue;

public:
	float GetValue( int nPartyCount, int nStageConstructionLevel );
};

#ifdef PRE_ADD_VIP
class CVIPGradeTable : public CSingleton<CVIPGradeTable>
{
public:
	struct SVIPGradeUnit
	{
		int level;
		int nameUIStringID;
		int ptsMin;
		int ptsMax;

		SVIPGradeUnit()
		{
			level =
			nameUIStringID =
			ptsMin =
			ptsMax = 0;
		}
	};

	CVIPGradeTable();
	virtual ~CVIPGradeTable() {}

protected:
	std::vector<SVIPGradeUnit>	m_VIPGradeList;

public:
	const SVIPGradeUnit* GetValue(int pts) const;
};
#endif // PRE_ADD_VIP

class CEqualExponentTable : public CSingleton<CEqualExponentTable> {
public:
	CEqualExponentTable();
	virtual ~CEqualExponentTable();

	enum EqualExponentTableIndex {
		PhysicalAttackMinRevision,
		PhysicalAttackMaxRevision,
		MagicAttackMinRevision,
		MagicAttackMaxRevision,
		PhysicalDefenseRevision,
		MagicDefenseRevision,
		MaxHPRevision,
		MaxSPRevision,
		RecoverySPRevision,
		StiffRevision,
		StiffResistanceRevision,
		CriticalRevision,
		CriticalResistanceRevision,
		StunRevision,
		StunResistanceRevision,
		SuperAmmorRevision,
		MoveSpeedRevision,
		ElementAttackRevision,
		ElementDefenseRevision,
		FinalDamageRevision,
		StrengthRevision,
		AgilityRevision,
		IntelligenceRevision,
		StaminaRevision,
		Amount,
	};

protected:
	struct ModeValue {
		float fValue[Amount];
	};
	std::vector<ModeValue> m_VecTable;

public:
	float GetValue( int nMode, EqualExponentTableIndex Index );
};

namespace GuildReward
{
	enum GuildRewardType
	{
		NONE						= 0,
		GUILDITEM					= 1,	// 치장형아이템
		MAKE_GUILDMARK				= 2,	// 길드마크제작
		MAX_GUILDWARE				= 3,	// 길드창고슬롯증가
		MAX_MEMBER					= 4,	// 길드원수제한증가
		EXTRA_EXP					= 5,	// 추가경험치
		EXTRA_PVPEXP				= 6,	// 추가콜로세움경험치
		EXTRA_FESTPOINT				= 7,	// 추가길드축제포인트
		GUILDWARE_GOLD				= 8,	// 사냥종료 후 길드창고에 골드 입금
		EXTRA_UNIONPOINT			= 9,	// 추가연합포인트
		MOVE_SPEED					= 10,	// 안전지역 이동속도증가
		VEHICLE_SPEED				= 11,	// 탈 것 이동속도 증가
		REDUCE_DURABILITYRATIO		= 12,	// 내구도 소모 비율 감소
		GUILD_GESTURE				= 13,	// 길드 제스처
#if defined( PRE_ADD_GUILDREWARDITEM )
		GUILD_ADDSKILL1				= 14,	// 스킬1
		GUILD_ADDSKILL2				= 15,	// 스킬2
		GUILD_ADDSKILL3				= 16,	// 스킬3
		GUILD_ADDSKILL4				= 17,	// 스킬4
#endif
		MAX_COUNT,
	};

	enum GuildRewardPrerequisiteType
	{
		PrerequisiteType_NONE = 0,
		PrerequisiteType_REWARDID,
		PrerequisiteType_GUILDMARK,
	};

	struct GuildRewardInfo
	{
		int ID;
		int _NameID;
		int _DescriptionID;
		std::string _NameIDParam;
		std::string _DescriptionIDParam;
		int _TooltipItemID;
		int _NeedGold;
		int _NeedGuildLevel;
		int _Type;
		int _TypeParam1;
		int _TypeParam2;
		int _Period;
		bool _GuildMasterLimit;
		bool _IsApplicable;
		int _PrerequisiteType;
		int _PrerequisiteID;
		int _NeedJobClass;
		bool _CheckInven;		//구입시 캐시인벤토리에 TypeParam1의 아이템이 있으면 구입 안됨.

		GuildRewardInfo()
		{
			ID = -1;
			_NameID = -1;
			_DescriptionID = -1;
			_NameIDParam = "";
			_DescriptionIDParam = "";
			_TooltipItemID = -1;
			_NeedGold = 0;
			_NeedGuildLevel = 0;
			_Type = GuildRewardType::NONE;
			_TypeParam1 = 0;
			_TypeParam2 = 0;
			_Period = 0;
			_GuildMasterLimit = false;
			_IsApplicable = false;
			_PrerequisiteType = 0;
			_PrerequisiteID = -1;
			_NeedJobClass = -1;
			_CheckInven = false;
		}

		bool IsFunctionality()
		{
			return (_Type != GUILDITEM);
		}

		GuildRewardInfo& operator= (const GuildRewardInfo& rValue)
		{
			ID = rValue.ID;

			_NameID = rValue._NameID;
			_DescriptionID = rValue._DescriptionID;
			_NameIDParam = rValue._NameIDParam;
			_DescriptionIDParam = rValue._DescriptionIDParam;
			_TooltipItemID = rValue._TooltipItemID;
			_NeedGold = rValue._NeedGold;
			_NeedGuildLevel = rValue._NeedGuildLevel;
			_Type = rValue._Type;
			_TypeParam1 = rValue._TypeParam1;
			_TypeParam2 = rValue._TypeParam2;
			_Period = rValue._Period;
			_GuildMasterLimit = rValue._GuildMasterLimit;
			_IsApplicable = rValue._IsApplicable;
			_PrerequisiteType = rValue._PrerequisiteType;
			_PrerequisiteID = rValue._PrerequisiteID;
			_NeedJobClass = rValue._NeedJobClass;
			_CheckInven = rValue._CheckInven;

			return(*this);
		}
	};
}

class CGuildRewardTable : public CSingleton<CGuildRewardTable> {
public:
	typedef std::map<int, GuildReward::GuildRewardInfo> GUILDREWARD_LIST;

	CGuildRewardTable();
	virtual ~CGuildRewardTable();

protected:
	GUILDREWARD_LIST m_GuildRewardList;

public:
	bool AddGuildRewardInfo(int nID, const GuildReward::GuildRewardInfo& info);
	GuildReward::GuildRewardInfo* GetGuildRewardInfo(int nID);
	GUILDREWARD_LIST& GetGuildRewardList() { return m_GuildRewardList; }
};

#if defined(PRE_FIX_NEXTSKILLINFO)
typedef std::map<int, int>	SKILL_LEVEL_TABLE_IDS;
struct SKILL_LEVEL_INFO
{
	int _MinLevel;
	int _MaxLevel;
	SKILL_LEVEL_TABLE_IDS _SkillLevelTableIDs;

	SKILL_LEVEL_INFO()
	{
		_MinLevel = 0;
		_MaxLevel = 0;
	}

	void AddSkillLevelTableID(int nLevel, int nTableID)
	{
		if (_SkillLevelTableIDs.insert(std::make_pair(nLevel, nTableID)).second == true)
			UpdateMinMaxLevelInfo();
	}

	void UpdateMinMaxLevelInfo()
	{
		int nTempMinLevel = 999;
		int nTempMaxLevel = -1;

		SKILL_LEVEL_TABLE_IDS::iterator iter = _SkillLevelTableIDs.begin();
		SKILL_LEVEL_TABLE_IDS::iterator endIter = _SkillLevelTableIDs.end();
		for( ; iter != endIter; ++iter)
		{
			int iSkillLevel = iter->first;
			
			//최대/최소 스킬 레벨 설정..
			if (nTempMinLevel > iSkillLevel)
				nTempMinLevel = iSkillLevel;

			if (nTempMaxLevel < iSkillLevel)
				nTempMaxLevel = iSkillLevel;
		}

		if (nTempMinLevel != 999)
			_MinLevel = nTempMinLevel;
		if (nTempMinLevel != -1)
			_MaxLevel = nTempMaxLevel;
	}
};

typedef std::map<int, SKILL_LEVEL_INFO> SKILL_LEVEL_TABLEID_LIST;
#endif // PRE_FIX_NEXTSKILLINFO

#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
class CItemCategoryInfo : public CSingleton<CItemCategoryInfo>
{
public:
	CItemCategoryInfo();

	int GetDescIndex(int itemCategoryType) const;

private:
	std::map<int, int> m_ItemCategoryInfo; // key : _ItemCategoryType, data : UISTRING index
};
#endif // PRE_ADD_ITEMCAT_TOOLTIP_INFO


//rlkt_ok!
class CPartsSkinCombineTable : public CSingleton<CPartsSkinCombineTable> {
public:
	CPartsSkinCombineTable();
	virtual ~CPartsSkinCombineTable();

protected:
	struct SkinValue {
		int nSkinIndex;
		int nClassID;
		int nSkinID;
	};
	std::map<int, SkinValue *> m_nMapTable;

public:
	int GetValue(int nSkinIndex, int nClassID);
};


class CStageDropTable : public CSingleton<CStageDropTable> {
public:
	CStageDropTable();
	virtual ~CStageDropTable();

protected:
	struct DropValue {
		int _StageClearDropId;
		int _MinLevel;
		int _MaxLevel;
		int _Job;
		int _DropID;
	};
	std::map<int, DropValue *> m_nMapTable;

public:
	int GetValue(int nDropIndex, int nLevel, int nJob);
};

class CDisjointInfoTable : public CSingleton<CDisjointInfoTable> {
public:
	CDisjointInfoTable();
	virtual ~CDisjointInfoTable();

protected:
	struct DisjointorInfo {
		int _EnchantLevel;
		int _DisjointDrop;
		int _ItemID[20];
	};
	std::map<int, DisjointorInfo *> m_nMapTable;

public:
	std::vector<int> GetValue(int nDisjointID, int nItemLevel);
};

class CDragonJewelSlotTable : public CSingleton<CDragonJewelSlotTable> {
public:
	CDragonJewelSlotTable();
	virtual ~CDragonJewelSlotTable();

protected:
	struct DragonJewelSlotInfo {
		int JewelType;
		float SuccessRatio;
		int SlotType[4];
		int Enhanced;
	};
	std::map<int, DragonJewelSlotInfo *> m_nMapTable;

public:
	std::vector<int> GetValue(int nJewelType);
};