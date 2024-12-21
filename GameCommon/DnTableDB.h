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
		TCASHCOMMODITY,			//�̳༮��
		TCASHGOODSINFO,
		TCASHPACKAGE,			//�̳༮�� ���̺굵�� ���ε尡 �߰��Ǿ���. �����⵿�� �̿ܿ� Sox������ ���̷�Ʈ�� �＼����������
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
		TGUILDWARREWARD,	// ����� ����
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
		TGUILDREWARDITEM,	//��庸�������
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

	// FileTable ���� �Լ�
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

	//	note by kalliste : ���̺� ID���� �ϳ� ���� ���� �Ҵ��ؾ� ��
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

		// #31367 �̽��� �߰���. ������ ���� ����ȿ�� �� ���ݷ� ���� ���� ����ȿ���� ������ ���õ�.
		AttackPowerClampLowestRatio = 41,
		DamageChangeClampLowestRatio = 42,

		MarketRegisterTime_Min = 49,
		MarketRegisterTime0 = MarketRegisterTime_Min,	// [�ŷ���_����ð�0]
		MarketRegisterTime1 = 50,						// [�ŷ��� ����ð� 1]
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

		CharmItempDisplayDelayTime = 87,	//�ŷ� ������ ǥ�� ���� �ð� ����
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
		ExpUpItem = 406,				// ����ġ ���� ������(����)
		PvPExpUpItem = 407,				// �ݷμ��� ����ġ ���� ������ ��� Ƚ��

#if defined( PRE_PVP_GAMBLEROOM )		
		PvPGambleFirstWinnerRate = 408,	// ���ڸ�� ����� ��� ����
		PvPGambleSecondWinnerRate = 409,	// ���ڸ�� �ؿ���� ��� ����
		PvPGambleCumulativeRate = 410,		// ���ڸ�� ������ ����
		PvPGambleRoomMaxCount = 411,		// ���ڸ�� �ִ� ��������
#endif
		
		RangeFallGravityAdditionalProjectileRange = 499,		// #36693 RangeFallGravity �� DestroyOrbit �� ���õ� �߻�ü�� ��� �־��� �߰� ��Ÿ� ��.
		MonsterElementMinRevision = 500,	//���� �Ӽ� ��� �ּ� ��ġ

		Element_Attack_Min = 501, // PlayerState
		Element_Attack_Max = 502, // PlayerState

		Element_Defense_Min = 503, // PlayerState + MonsterState
		Element_Defense_Max = 504, // PlayerState + MonsterState 

		Physical_AttackRatio_Min = 505, // PlayerState
		Physical_AttackRatio_Max = 506, // PlayerState
		Magical_AttackRatio_Min = 507, // PlayerState
		Magical_AttackRatio_Max = 508, // PlayerState 
		
		Freezing_AddDamageRatio = 509,     // ����ȿ�� ����

		Freezing_AdddamageRatio_Max = 510, // ����ȿ�� ����

		Change_DamageTaken_Physical_Min = 511, // Player  // ����ȿ�� ����
		Change_DamageTaken_Physical_Max = 512, // Monster // ����ȿ�� ����
		Change_DamageTaken_Magical_Min = 513,  // Player  // ����ȿ�� ����
		Change_DamageTaken_Magical_Max = 514,  // Monster // ����ȿ�� ����

		Element_EqualBasis = 515,            // Element ���� �� , ���� Level / Basis ���� ���� ����� �޸� Element�� ���������� ����Ѵ� = < ��ȹ���� ���� >
		Element_EqualLevel = 516,            // Element ���� �� , ���� Level / Basis ���� ���� ����� �޸� Element�� ���������� ����Ѵ� = < ��ȹ���� ���� >
		Element_Expotent_Limit = 517,        // Element �������� �����ʴ� �ּ�ġ�� ���ϱ�� ��ȹ������ ����

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
		BeginnerGuild_UserMax = 1025,	// �ʺ��ڱ�� �����ο� (���� Ŭ���̾�Ʈ������ �̻��)
		BeginnerGuild_MailIndex = 1026, // �ʺ��ڱ�� ���� ���� ���� ���� �ε��� (���� Ŭ���̾�Ʈ������ �̻��)

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
		ItemUnLockWaitTime = 1043,				// ������ ��� ���� ��� �ð�

		DragonBuffWaitTime = 1044,				// �巡���������Ʈ ��� �ð�

		SERVER_WAREHOUSE_POPUP = 1099,			// �� ������ Ȯ��â ��¿���(0-���x, 1-���)
		SERVER_WAREHOUSE_INVENCOUNT = 1100,		// �⺻ ���� �Ϲ�â�� ����ĭ��
		SERVER_WAREHOUSE_CASHCOUNT = 1101,		// �⺻ ���� ĳ��â�� ����ĭ��
		SERVER_WAREHOUSE_PAYMENT = 1102,		// ����� ����ȭ Ÿ��(0-����(������), 1=���μҸ�)
		SERVER_WAREHOUSE_IN_LEVELLIMIT = 1103,	// â�� ������ ������ ���� ����
		SERVER_WAREHOUSE_OUT_LEVELLIMIT = 1104,	// â�� ������ ������ ���� ����

		AlteiaWorldDailyPlayCount = 1105,		// �����̾� ���� �÷���Ƚ��
		AlteiaWorldJoinTicketCount = 1106,		// �����̾� ����� �ʿ��� ����Ǽ�
		AlteiaWorldSendTicketCount = 1107,		// �����̾� ������ �ִ� ����Ǽ�
		AlteiaWorldDailyTicketCount = 1108,		// �����̾� ���� ���� ����Ǽ�
		AlteiaWorldEnterLimitStartTime = 1109,	// �����̾� ���Ա��� ���۽ð�(��)
		AlteiaWorldEnterLimitEndTime = 1110,	// �����̾� ���Ա��� ���ð�(��)
		AlteiaWorldEnterLimitLevel = 1111,		// �����̾� ���� ���� ����
		AlteiaWorldAutoDiceTimer = 1112,		// �����̾� �ڵ� �ֻ��� ���� Ÿ�̸�

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

	//	note by kalliste : ���̺� ID���� �ϳ� ���� ���� �Ҵ��ؾ� ��
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
		MaxSeedPoint = 10011,	// �õ��ִ�ũ��(�ͼ�ȭ��)

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
		LevelDExp,		// �Ƿε� �Ҹ� ���� �Ϸ� ���� ����ġ.

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
		GUILDITEM					= 1,	// ġ����������
		MAKE_GUILDMARK				= 2,	// ��帶ũ����
		MAX_GUILDWARE				= 3,	// ���â��������
		MAX_MEMBER					= 4,	// ��������������
		EXTRA_EXP					= 5,	// �߰�����ġ
		EXTRA_PVPEXP				= 6,	// �߰��ݷμ������ġ
		EXTRA_FESTPOINT				= 7,	// �߰������������Ʈ
		GUILDWARE_GOLD				= 8,	// ������� �� ���â�� ��� �Ա�
		EXTRA_UNIONPOINT			= 9,	// �߰���������Ʈ
		MOVE_SPEED					= 10,	// �������� �̵��ӵ�����
		VEHICLE_SPEED				= 11,	// Ż �� �̵��ӵ� ����
		REDUCE_DURABILITYRATIO		= 12,	// ������ �Ҹ� ���� ����
		GUILD_GESTURE				= 13,	// ��� ����ó
#if defined( PRE_ADD_GUILDREWARDITEM )
		GUILD_ADDSKILL1				= 14,	// ��ų1
		GUILD_ADDSKILL2				= 15,	// ��ų2
		GUILD_ADDSKILL3				= 16,	// ��ų3
		GUILD_ADDSKILL4				= 17,	// ��ų4
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
		bool _CheckInven;		//���Խ� ĳ���κ��丮�� TypeParam1�� �������� ������ ���� �ȵ�.

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
			
			//�ִ�/�ּ� ��ų ���� ����..
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