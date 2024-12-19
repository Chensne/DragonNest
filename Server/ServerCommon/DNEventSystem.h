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
		OnAppelationGain,		// �Ⱦ�
		OnRebirth,				// V(x) G(o)
		OnDie,					// V(x) G(o)
		OnRebirthAny,			// V(x) G(o)
		OnKillMonster,			// V(x) G(o)
		OnGuildCreate,			// V(o) G(x)	// 20
		OnGuildJoin,			// V(o) G(x)
		OnJobChange,			//
		OnAirshipRiding,		// �Ⱦ�
		OnSocialAction,			// V(o) G(o)
		OnMarketBuy,			// V(o) G(x)
		OnMarketSell,			// V(o) G(x)
		OnExchange,				// V(o) G(o)
		OnPvPVictory,			// V(x) G(o)
		OnPvPLoss,				// V(x) G(o)
		OnCoopClear,			// �Ⱦ�			// 30
		OnCoopFailed,			// �Ⱦ�
		OnSkillUse,				// V(x) G(o)
		OnSkillLevelUp,			// V(o) G(o)
		OnSkillAdd,				// V(o) G(x)
		OnWarehouseKeep,		// V(o) G(x)
		OnPvPFinished,			// V(x) G(o)
		OnKillPlayer,			// V(x) G(o)
		OnQuestGain,			// V(o) G(o)
		OnDungeonGiveup,		// V(x) G(o)	// F9 ������ �������� �����Ҷ�
		OnFriendFull,			// V(o) G(o)	// 40
		OnDarklairClearRound,	// V(x) G(o)
		OnPvPKillCaptain,		// V(x) G(o)
		OnNpcReputaionChange,	// V(o) G(o)
		OnCashShopOpen,			// V(x) G(o)	// ĳ�ü����½�
		OnMasterSystemGraduate, // V(o) G(o)	// 45)���� ����
		OnMasterSystemGraduated,// V(o) G(o)	// 46)���� ���� ��Ŵ
		OnMasterSystemJoin,		// V(o) G(o)	// 47)���� ���� �α�
		OnPvPEventFinished,		// V(x) G(o)	// 48)PvP�̺�Ʈ�� Ŭ����
		OnKillMonster2,			// V(x) G(o)	// 49)�ش� KillMonster �� �׾����� ��Ÿ�� �������� ��� �� ��� ���� üũ���� ����.
		OnDailyFatigue,							// 50)���� �Ƿε� �Ҹ��
		OnWeeklyFatigue,						// 51)�ְ� �Ƿε� �Ҹ��
		OnPCBangFatigue,						// 52)PC�� �Ƿε� �Ҹ��
		OnVIPFatigue,							// 53)VIP �Ƿε� �Ҹ�� 
		OnCountingDungeonClear,					// 54)���Ӵ���Ŭ�����
		OnDailyMissionClear,					// 55)���Ϲ̼� Ŭ�����
		OnWeeklyMissionClear,					// 56)�ְ��̼� Ŭ�����
		OnPCBang_Item = 57,						// 57)PC���ϰ��-����������
		OnPCBang_Fatigue = 58,					// 58)PC���ϰ��-�Ƿε�����
		OnPCBang_RebirthCoin = 59,				// 59)PC���ϰ��-��������
		OnRoundFinished = 60,	// V(x) G(o)	// 60)PvP ���尡 ������
		OnOccupyArea = 61,		// V(x) G(o)	// 61)���� ����
		OnStealArea = 62,		// V(x) G(o)	// 62)���� ��Ż
		OnTrigger = 63,			// V(x) G(o)	// 63)Ʈ���� �ߵ���
		OnLadderFinished = 64,	// V(x) G(o)	// 64)���� Ŭ����
		OnPetLevelUp = 65,		// V(o) G(o)	// 65)�� ��������
		OnGiftToCadger = 66,					// 66)������ ������
		OnSetRevengeTarget = 67,				// 67)���� ����� ������ ��
		OnPvPLevelChange = 68,					// 68)PvP ��� �����
		OnHPChanged = 69,		// V(x) G(o)	// 69)HP �����
		OnMPChanged = 70,		// V(x) G(o)	// 70)MP �����
		OnPetExpired = 71,		// V(o) G(o)	// �������� �� �Ⱓ �����
		OnActiveMissionClear = 72, // V(x) G(o)	// 72)��Ƽ�� �̼� Ŭ�����
		OnChatMission = 73,						// 73)ä�� �̼� Ŭ�����
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
		PlayerLevel = 1,	// �÷��̾� ����
		PartyCount = 2,		// ��Ƽ����
		MapID = 3,			// �� ID
		MapLevel = 4,		// �� Level
		InvenCoin = 5,		// �κ��丮 ��
		WarehouseCoin = 6,	// â�� �ִ� ��
		JobID = 7,			// ���� ���� ID
		ClassID = 8,		// ���� Ŭ���� ID
		MonsterID = 9,		// ���� ID
		NpcID = 10,			// NPC ID
		SkillID = 11,		// ��ų ID
		SkillLevel = 12,	// ��ų Level
		ItemID = 13,		// ������ ID
		StageClearRank = 14,	// �������� Ŭ���� ��ũ
		StageClearTime = 15,	// �������� Ŭ���� Ÿ��
		MaxComboCount = 16,		// �������� Ŭ����� �ִ� �޺� ī��Ʈ
		UseCoinCount = 17,		// �ŷ�, ���� ��� ���� ��
		EnchantLevel = 18,		// ��ȭ�� ��ȭ����
		TotalDamageCount = 19,	// �������� Ŭ����� �ǰ� Ƚ��
		MissionID = 20,			// �޼� �̼� ID
		DieCount = 21,			// ��� Ƚ��
		GenocideCount = 22,		// ���� ���ÿ� ��� ����߳�.
		PartyMemberDieCount = 23,// ��Ƽ�� ��� Ƚ��
		KOClassIDinPvP = 24,		// pvp����. ���� Ŭ����ID
		KOClassCountinPvP = 25,	// pvp����. ���� Ŭ���� ����
		KObyClassIDinPvP = 26,	// pvp����. ���� Ŭ����ID
		KObyClassCountinPvP = 27,// pvp����. ���� Ŭ���� ����
		PvPWin = 28,
		PvPLose = 29,
		AllPartsMinLevel = 30,	// ���5�� �ּ� ��ȭ ����
		QuestID = 31,			// ����Ʈ ID
		DarkLairRoundCount = 32,
		SocialActionID = 33,		// ����ó ID
		KOCountinPvP = 34,		// pvp����. ųī��Ʈ
		KObyCountinPvP = 35,		// pvp����. ����ī��Ʈ
		StartPartyCount = 36,	// �������� �����Ҷ��� ��Ƽ����
		NpcReputaionFavor = 37,
		NpcReputaionMalice = 38,		
		MasterCount = 39,		// ���� ��
		PupilCount = 40,			// ���� ��
		GraduateCount = 41,		// ������ ��
		PartyMasterCount = 42,	// ��Ƽ�� ���� ��
		PartyPupilCount = 43,	// ��Ƽ�� ���� ��
		PartyClassmateCount = 44,// ��Ƽ�� ���� ��
		MapSubType = 45,			// �� SubType
		DungeonClearCount = 46,	// ���� Ŭ���� ��
		DailyFatigue = 47,		// ���� �Ƿε�
		WeeklyFatigue = 48,		// �ְ� �Ƿε� 
		PCBangFatigue = 49,		// PC�� �Ƿε�
		VIPFatigue = 50,			// VIP �Ƿε�
		ItemType = 51,			// ������ Type
		PCBangGrade = 52,	// �Ǿ��� ���(1-�Ϲ� 2-���� 3-�ǹ� 4-���) PCBang::Grade
		PvPGameMode = 53,	// PvP ���Ӹ��(0-������, 1-����, 2-������, 3-������, 11-����, 12-�����,13-������)
		PvPTeam		= 54,	// PvP Team üũ(A = 1000, B = 1001, Human = 1000, Zombie = 1001)
		PvPRoundCount = 55,	// PvP ����� 
		PvPWinCondition=56,	// PvP �¸� ����(1-Ÿ�ӿ���, 2-����� ��� ����, 3-����� ��� ����, 4-����� ���� ����, 5-����� ���� ����..��Ÿ.
		HolyWaterUse=57,	// ���� ���� ��� Ƚ��
		GhoulModeWin=58,	// ���� ��� �¸� Ƚ��
		BossKillCount=59,	// ����ųī��Ʈ
		PvPModePlayCount = 60,	// PvP ���Ӹ�庰 �÷��� ��
		PvPModeWinCount = 61,	// PvP ���Ӹ�庰 �¸� ��
		PvPModeOccupyCount = 62,// PvP ���Ӹ�庰 ���� ��
		PvPModeStealCount = 63,// PvP ���Ӹ�庰 ��Ż ��
		PvPModeKillCount = 64,// PvP ���Ӹ�庰 ų ��
		MonsterGrade = 65,	// ���� ���
		MonsterRaceID = 66,	// ���� ���� ID
		DiffMapToUserLevel= 67,	// �� ���巹���� �������� ���̰�(���밪)
		GuildPartyCount= 68,	// ��Ƽ�� ���� ��
		PetLevel=69,			// ������ �ִ� ���� ���� ���� ����
		PartyMasterAndPupilCount = 70, // ��Ƽ�� ����+���� ��
		PartyType = 71,		//��ƼŸ�� (ePartyType �� ����)
		PvPRevengeSuccess = 72, //PvP ��忡�� ų or Die �� ������ ��������
		PvPLevel = 73, //PvP ���
		PvPTournamentWin = 74,	// PvP ��ʸ�Ʈ �¸�(��, ��)
		PvPTournamentType = 75, // PvP ��ʸ�Ʈ Ÿ��(32��, 16��, 8��, 4��)
		PvPTournamentAdvance = 76, // PvP ��ʸ�Ʈ ����(16��, 8��, 4��, ���)		
		ItemCount = 77,	// ������ ���� (OnItemGain �̺�Ʈ������ �ش� ������ID �� ó��)
		PlayerHpPercent = 78,	// �÷��̾� HP
		PlayerMpPercent = 79,	// �÷��̾� MP
		TotalRebirthCount = 80, // ���� ��Ȱ ����
		SkillPoint = 81, // ���� ���� Ʈ���� ���� ��ų����Ʈ
		PvPWorldCombine = 82, // �������� ����
		ActiveMissionID = 83, // ��Ƽ�� �̼� ID ( Event Variable )
		UIStringMID = 84,	//UIString.xml �� MID
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
