#pragma once

class CDNUserSession;

namespace EventSystem
{
	enum EventTypeEnum {	
		OnNone,
		OnLevelUp,				// V(o) G(o)
		OnDungeonEnter,			// V(o) G(o)
		OnDungeonClear,			// V(x) G(o)
		OnItemGain,				// V(o) G(o)
		OnItemUse,				// V(o) G(o)
		OnItemEquip,			// V(o) G(o)
		OnItemCompound,			// V(o) G(x)
		OnItemDisjoint,			// V(x) G(o)
		OnItemEnchantSuccess,	// V(o) G(o)
		OnItemEnchantFailed,	// V(o)	G(o)	// 10
		OnItemRepair,			// V(o) G(o)
		OnNpcTalk,				// V(o) G(o)
		OnMissionClear,			// V(o) G(o)
		OnQuestClear,			// V(o) G(o)
		OnAppelationGain,		// 안씀
		OnRebirth,				// V(x) G(o)
		OnDie,					// V(x) G(o)
		OnRebirthAny,			// V(x) G(o)
		OnKillMonster,			// V(x) G(o)
		OnGuildCreate,			// V(o) G(x)	// 20
		OnGuildJoin,			// V(o) G(x)
		OnJobChange,			//
		OnAirshipRiding,		// 안씀
		OnSocialAction,			// V(o) G(o)
		OnMarketBuy,			// V(o) G(x)
		OnMarketSell,			// V(o) G(x)
		OnExchange,				// V(o) G(o)
		OnPvPVictory,			// V(x) G(o)
		OnPvPLoss,				// V(x) G(o)
		OnCoopClear,			// 안씀			// 30
		OnCoopFailed,			// 안씀
		OnSkillUse,				// V(x) G(o)
		OnSkillLevelUp,			// V(o) G(o)
		OnSkillAdd,				// V(o) G(x)
		OnWarehouseKeep,		// V(o) G(x)
		OnPvPFinished,			// V(x) G(o)
		OnKillPlayer,			// V(x) G(o)
		OnQuestGain,			// V(o) G(o)
		OnDungeonGiveup,		// V(x) G(o)	// F9 눌러서 스테이지 포기할때
		OnFriendFull,			// V(o) G(o)	// 40
		OnDarklairClearRound,	// V(x) G(o)
		OnPvPKillCaptain,		// V(x) G(o)
		OnNpcReputaionChange,	// V(o) G(o)
		OnCashShopOpen,			// V(x) G(o)	// 캐시샵오픈시
		OnMasterSystemGraduate, // V(o) G(o)	// 45)제자 졸업
		OnMasterSystemGraduated,// V(o) G(o)	// 46)제자 졸업 시킴
		OnMasterSystemJoin,		// V(o) G(o)	// 47)사제 관계 맺기
		OnPvPEventFinished,		// V(x) G(o)	// 48)PvP이벤트방 클리어
		OnKillMonster2,			// V(x) G(o)	// 49)해당 KillMonster 는 죽었을때 막타가 유저였을 경우 등 모든 조건 체크하지 않음.
		OnDailyFatigue,							// 50)일일 피로도 소모시
		OnWeeklyFatigue,						// 51)주간 피로도 소모시
		OnPCBangFatigue,						// 52)PC방 피로도 소모시
		OnVIPFatigue,							// 53)VIP 피로도 소모시 
		OnCountingDungeonClear,					// 54)연속던전클리어시
		OnDailyMissionClear,					// 55)일일미션 클리어시
		OnWeeklyMissionClear,					// 56)주간미션 클리어시
		OnPCBang_Item = 57,						// 57)PC방일경우-아이템지급
		OnPCBang_Fatigue = 58,					// 58)PC방일경우-피로도지급
		OnPCBang_RebirthCoin = 59,				// 59)PC방일경우-생명석지급
		OnRoundFinished = 60,	// V(x) G(o)	// 60)PvP 라운드가 끝날때
		OnOccupyArea = 61,		// V(x) G(o)	// 61)거점 점령
		OnStealArea = 62,		// V(x) G(o)	// 62)거점 쟁탈
		OnTrigger = 63,			// V(x) G(o)	// 63)트리거 발동시
		OnLadderFinished = 64,	// V(x) G(o)	// 64)래더 클리어
		OnPetLevelUp = 65,		// V(o) G(o)	// 65)펫 레벨업시
		OnGiftToCadger = 66,					// 66)조르기 선물시
		OnSetRevengeTarget = 67,				// 67)복수 대상이 생겼을 시
		OnPvPLevelChange = 68,					// 68)PvP 등급 변경시
		OnHPChanged = 69,		// V(x) G(o)	// 69)HP 변경시
		OnMPChanged = 70,		// V(x) G(o)	// 70)MP 변경시
		OnPetExpired = 71,		// V(o) G(o)	// 장착중인 펫 기간 만료시
		OnActiveMissionClear = 72, // V(x) G(o)	// 72)액티브 미션 클리어시
		OnChatMission = 73,						// 73)채팅 미션 클리어시
		achievetype_unk1,
		achievetype_unk2,
		achievetype_unk3,
		achievetype_unk4,
		achievetype_unk5,
		achievetype_unk6,
		achievetype_unk7,
		achievetype_unk8,
		achievetype_unk9,
		achievetype_unk10,
		achievetype_unk11,
		EventTypeEnum_Amount,
	};

	enum EventValueTypeEnum {		
		None,
		PlayerLevel = 1,	// 플레이어 레벨
		PartyCount = 2,		// 파티원수
		MapID = 3,			// 맵 ID
		MapLevel = 4,		// 맵 Level
		InvenCoin = 5,		// 인벤토리 돈
		WarehouseCoin = 6,	// 창고에 있는 돈
		JobID = 7,			// 현재 직업 ID
		ClassID = 8,		// 현재 클래스 ID
		MonsterID = 9,		// 몬스터 ID
		NpcID = 10,			// NPC ID
		SkillID = 11,		// 스킬 ID
		SkillLevel = 12,	// 스킬 Level
		ItemID = 13,		// 아이템 ID
		StageClearRank = 14,	// 스테이지 클리어 랭크
		StageClearTime = 15,	// 스테이지 클리어 타임
		MaxComboCount = 16,		// 스테이지 클리어시 최대 콤보 카운트
		UseCoinCount = 17,		// 거래, 수리 등등 사용된 돈
		EnchantLevel = 18,		// 강화시 강화레벨
		TotalDamageCount = 19,	// 스테이지 클리어시 피격 횟수
		MissionID = 20,			// 달성 미션 ID
		DieCount = 21,			// 사망 횟수
		GenocideCount = 22,		// 몬스터 동시에 몇마리 사냥했나.
		PartyMemberDieCount = 23,// 파티원 사망 횟수
		KOClassIDinPvP = 24,		// pvp정보. 죽인 클래스ID
		KOClassCountinPvP = 25,	// pvp정보. 죽인 클래스 수량
		KObyClassIDinPvP = 26,	// pvp정보. 죽은 클래스ID
		KObyClassCountinPvP = 27,// pvp정보. 죽은 클래스 수량
		PvPWin = 28,
		PvPLose = 29,
		AllPartsMinLevel = 30,	// 장비5종 최소 강화 레벨
		QuestID = 31,			// 퀘스트 ID
		DarkLairRoundCount = 32,
		SocialActionID = 33,		// 제스처 ID
		KOCountinPvP = 34,		// pvp정보. 킬카운트
		KObyCountinPvP = 35,		// pvp정보. 데스카운트
		StartPartyCount = 36,	// 스테이지 시작할때의 파티원수
		NpcReputaionFavor = 37,
		NpcReputaionMalice = 38,		
		MasterCount = 39,		// 스승 수
		PupilCount = 40,			// 제자 수
		GraduateCount = 41,		// 졸업생 수
		PartyMasterCount = 42,	// 파티내 스승 수
		PartyPupilCount = 43,	// 파티내 제자 수
		PartyClassmateCount = 44,// 파티내 동기 수
		MapSubType = 45,			// 맵 SubType
		DungeonClearCount = 46,	// 던젼 클리어 수
		DailyFatigue = 47,		// 일일 피로도
		WeeklyFatigue = 48,		// 주간 피로도 
		PCBangFatigue = 49,		// PC방 피로도
		VIPFatigue = 50,			// VIP 피로도
		ItemType = 51,			// 아이템 Type
		PCBangGrade = 52,	// 피씨방 등급(1-일반 2-레드 3-실버 4-골드) PCBang::Grade
		PvPGameMode = 53,	// PvP 게임모드(0-리스폰, 1-라운드, 2-대장전, 3-개인전, 11-좀비, 12-길드전,13-점령전)
		PvPTeam		= 54,	// PvP Team 체크(A = 1000, B = 1001, Human = 1000, Zombie = 1001)
		PvPRoundCount = 55,	// PvP 라운드수 
		PvPWinCondition=56,	// PvP 승리 조건(1-타임오버, 2-상대팀 모두 나감, 3-상대팀 모두 죽음, 4-상대팀 대장 나감, 5-상대팀 대장 죽음..기타.
		HolyWaterUse=57,	// 구울 성수 사용 횟수
		GhoulModeWin=58,	// 구울 모두 승리 횟수
		BossKillCount=59,	// 보스킬카운트
		PvPModePlayCount = 60,	// PvP 게임모드별 플레이 수
		PvPModeWinCount = 61,	// PvP 게임모드별 승리 수
		PvPModeOccupyCount = 62,// PvP 게임모드별 점령 수
		PvPModeStealCount = 63,// PvP 게임모드별 쟁탈 수
		PvPModeKillCount = 64,// PvP 게임모드별 킬 수
		MonsterGrade = 65,	// 몬스터 등급
		MonsterRaceID = 66,	// 몬스터 종족 ID
		DiffMapToUserLevel= 67,	// 맵 권장레벨과 유저레벨 차이값(절대값)
		GuildPartyCount= 68,	// 파티내 길드원 수
		PetLevel=69,			// 가지고 있는 펫중 가장 높은 레벨
		PartyMasterAndPupilCount = 70, // 파티내 스승+제자 수
		PartyType = 71,		//파티타입 (ePartyType 값 참조)
		PvPRevengeSuccess = 72, //PvP 모드에서 킬 or Die 시 리벤지 성공여부
		PvPLevel = 73, //PvP 등급
		PvPTournamentWin = 74,	// PvP 토너먼트 승리(승, 패)
		PvPTournamentType = 75, // PvP 토너먼트 타입(32강, 16강, 8강, 4강)
		PvPTournamentAdvance = 76, // PvP 토너먼트 진출(16강, 8강, 4강, 결승)		
		ItemCount = 77,	// 아이템 갯수 (OnItemGain 이벤트에서만 해당 아이템ID 로 처리)
		PlayerHpPercent = 78,	// 플레이어 HP
		PlayerMpPercent = 79,	// 플레이어 MP
		TotalRebirthCount = 80, // 남은 부활 갯수
		SkillPoint = 81, // 현재 선택 트리의 남은 스킬포인트
		PvPWorldCombine = 82, // 월드통합 전장
		ActiveMissionID = 83, // 액티브 미션 ID ( Event Variable )
		UIStringMID = 84,	//UIString.xml 내 MID
		unk1,
		unk2,
		unk3,
		unk4,
		unk5,
		unk6,
		unk7,
		unk8,
		unk9,
		unk10,
		unk11,
		unk12,
		unk13,
		unk14,
		unk15,
		unk16,
		unk17,
		EventValueTypeEnum_Amount,
	};
};

class CDNEventSystem:public TBoostMemoryPool<CDNEventSystem>
{

public:
	CDNEventSystem( CDNUserSession *pUser );
	virtual ~CDNEventSystem();

public:	

	struct EventValueFuncStruct {
		EventSystem::EventValueTypeEnum Type;
		int(_cdecl *pFunc)( CDNUserSession *pSession );
	};
	static EventValueFuncStruct s_EventValueFuncList[];

protected:
	CDNUserSession *m_pUserSession;
	
	int m_nEventParamValue[EventSystem::EventValueTypeEnum_Amount];

public:
	void RegisterEventParam( EventSystem::EventValueTypeEnum Index, int nValue );
	int GetEventParam( EventSystem::EventValueTypeEnum Index );

	bool OnEvent( EventSystem::EventTypeEnum Event );
	bool OnEvent( EventSystem::EventTypeEnum Event, int nParamCount, ... );

	bool CheckEventCondition( const TEventCondition *pEventCondition );
};
