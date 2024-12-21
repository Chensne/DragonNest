#pragma once

#include "DNCommonDef.h"
#include "../../GameCommon/DNNotifierCommon.h"

/* -----------------------------------------------------------------------------
	���� ����ü�� DB����� ����ü
	������ϴ� ������ �� Ŭ�������� �˾Ƽ� �������ּ���~
----------------------------------------------------------------------------- */

#pragma pack(push, 1)

struct TQuickSlot
{
	BYTE cType;		// eQuickSlotType����
	INT64 nID; 
};

#if !defined(PRE_DELETE_DUNGEONCLEAR)
struct TDungeonClearData
{
	int nMapIndex;	// �ʾ��̵�
	char cType;		// Ŭ���� Ÿ��? (eDungeonClearType)
};
#endif	// #if defined(PRE_DELETE_DUNGEONCLEAR)

struct TNestClearData
{
	int nMapIndex;
	char cClearCount;
	char cPCBangClearCount;
};

struct TCharacterStatus
{
	__time64_t tLastFatigueDate;
	__time64_t tLastEventFatigueDate;
	__time64_t tLastRebirthCoinDate;
	__time64_t tLastConnectDate;
	__time64_t tLastTimeEventDate;
	__time64_t tLastPeriodQuestDate[PERIODQUEST_RESET_MAX];

	char cAccountLevel;						// ���, ������ ����� ��������
	WCHAR wszCharacterName[NAMELENMAX];		// ĳ����
	char cClass;							// Ŭ���� (������, ��ó, �Ҽ�����, Ŭ����)

	bool IsCheckFirstVillage;				// ĳ�� ���� �� ������ ó�� ���� üũ

	int nDefaultBody;						// ó�� �����ÿ� ������ equip (EQUIP_BODY, EQUIP_LEG, EQUIP_HAND, EQUIP_FOOT)
	int nDefaultLeg;
	int nDefaultHand;
	int nDefaultFoot;

	BYTE cLevel;							// ����
	int nExp;								// ����ġ
	BYTE cJob;								// ����

	DWORD dwHairColor;		// �Ӹ� ����
	DWORD dwEyeColor;		// �� ����
	DWORD dwSkinColor;		// �Ǻ� ����

	int nMapIndex;							// ���� �ʹ�ȣ
	int nLastVillageMapIndex;				// ������ ���� �ʹ�ȣ
	int nLastSubVillageMapIndex;			// ������ Sub���� �ʹ�ȣ
	char cLastVillageGateNo;				// ������ ���� ����Ʈ ��ȣ
	int nPosX;								// ���� X��ǥ
	int nPosY;								// ���� Y��ǥ
	int nPosZ;								// ���� Z��ǥ
	float fRotate;							// Rotate

	INT64 nCoin;							// ��
	INT64 nWarehouseCoin;					// â��

	BYTE cRebirthCoin;						// ����ī��Ʈ (Coin�̶��� �ٸ���, ������ �ǻ�Ƴ��� ���� ����)
	BYTE cPCBangRebirthCoin;				// �Ǿ��� ����ī��Ʈ (Coin�̶��� �ٸ���, ������ �ǻ�Ƴ��� ���� ����)
	USHORT wCashRebirthCoin;				// ĳ���� �� ����ī��Ʈ (Coin�̶��� �ٸ���, ������ �ǻ�Ƴ��� ���� ����)

	short wFatigue;							// �Ƿε�
	short wWeeklyFatigue;					// �ְ��Ƿε�
	short wPCBangFatigue;					// �Ǿ����Ƿε�
	short wEventFatigue;
	short wVIPFatigue;

	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];	// 4 * 8 = 32��

	BYTE cJobArray[JOBMAX];					// ���� (0: ��ó�� ����, ������ ����~ ���� ����)
	DNNotifier::Data NotifierData[DNNotifier::RegisterCount::Total];
	TQuickSlot QuickSlot[QUICKSLOTMAX];		// ���� ����â (10���� 2��)
#if !defined(PRE_DELETE_DUNGEONCLEAR)
	TDungeonClearData DungeonClear[DUNGEONCLEARMAX];	// ���� Ŭ���� (120��)
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)
	TNestClearData NestClear[NESTCLEARMAX];	// �׽�Ʈ Ŭ����
	int nDungeonClearCount;					// ���� Ŭ���� ��
	int nGlyphDelayTime;
	int nGlyphRemainTime;

	short wMarketRegisterCount;				// ���� ��� ���� Ƚ��
	int nMarketSellSuccess;					// ���� �Ǹ� ���� Ƚ��
	int nMarketSellFailure;					// ���� �Ǹ� ���� Ƚ��
	int nMarketRegisterTax;					// ���� ���� ��� ������
	int nMarketSellTax;						// ���� ���� �Ǹ� ������

	int nPetal;								// ������ (��Ż)

	char cSkillPage;						// ���� Ȱ��ȭ �Ǿ��ִ� ��ų������
#if defined( PRE_PRIVATECHAT_CHANNEL )
	INT64 nPrivateChatChannel;				// ���� �ҼӵǾ��ִ� �缳 ä�� ��ȣ
#endif
#if defined( PRE_ADD_STAMPSYSTEM )
	__time64_t tLastStampDate;				// ���������� ������ �ý��ۿ� ����� ���� �ð�
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
};

struct TDailyMission 
{
	int nMissionID;
	bool bAchieve;
	int nCounter;
};

struct TMissionGroup
{
	int nMissionScore;						// �̼�����
	short wLastMissionAchieve[LASTMISSIONACHIEVEMAX];	// ������ ��ǥ�޼�(?)

	//�Ʒ� �÷��׵��� 8bit ������ ��� ���� ��Ʈ ���� ó���� (ex �̼� ���۰� 0 : 1000 0000 �� ǥ��, 1 �� 0100 0000 )
	char MissionGain[MISSIONMAX_BITSIZE];		// ȹ�濩��
	char MissionAchieve[MISSIONMAX_BITSIZE];	// ��ǥ�޼�(on/off)

	TDailyMission DailyMission[DAILYMISSIONMAX];
	TDailyMission WeeklyMission[WEEKLYMISSIONMAX];
	TDailyMission WeekendEventMission[WEEKENDEVENTMISSIONMAX];
	TDailyMission GuildWarMission[GUILDWARMISSIONMAX];
	TDailyMission PCBangMission[PCBangMissionMax];
	TDailyMission GuildCommonMission[GUILDCOMMONMISSIONMAX];
	TDailyMission WeekendRepeatMission[WEEKENDREPEATMISSIONMAX];
#if defined(PRE_ADD_MONTHLY_MISSION)
	TDailyMission MonthlyMission[MONTHLYMISSIONMAX];
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)

	__time64_t tDailyMissionDate;
	__time64_t tWeeklyMissionDate;
#if defined(PRE_ADD_MONTHLY_MISSION)
	__time64_t tMonthlyMissionDate;
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
};

struct TAppellationGroup
{
	int nSelectAppellation;							// ���õ� ȣĪ
	int nCoverAppellation;							// Ŀ�� ȣĪ
	char Appellation[APPELLATIONMAX_BITSIZE];		// ȣĪ
};

struct TPvPGroup
{
	BYTE	cLevel;												// ����
	UINT	uiXP;												// XP
	UINT	uiPlayTimeSec;										// �÷���Ÿ��(��)
	UINT	uiWin;												// �¸�
	UINT	uiLose;												// �й�
	UINT	uiDraw;												// ���º�
	UINT	uiGiveUpCount;										// �߰���Ż��
	UINT	uiKOClassCount[PvPCommon::Common::MaxClass];		// �� Ŭ������ KO��Ų Ƚ��
	UINT	uiKObyClassCount[PvPCommon::Common::MaxClass];		// �� Ŭ�������� KO���� ȸ��
	INT64	biTotalKillPoint;
	INT64	biTotalAssistPoint;
	INT64	biTotalSupportPoint;
	int nExpAbsoluteRank;		//���� 101������� ��ũ���� �Ѿ�´�. ���� Ŭ���̾�Ʈ�� �������� ��ũ�ʹ� ����
	float fExpRateRank;			//Ư�� exp�̻��� �������� ���� �ο��� �� �ۼ�Ʈ�� ��� �������� �Ѿ�´�
};

struct TItem
{
	int nItemID;
	INT64 nSerial;		// �ø���
	int nRandomSeed;
	USHORT wDur;			// ������
	short wCount;		// ����
	int nCoolTime;		// ���� ��Ÿ��
	char cLevel;		// ����
	char cPotential;	// �����
	bool bSoulbound;	// �������� (���� �� �ͼ�)
	char cOption;
	char cSealCount;	// �к�ī��Ʈ
	bool bEternity;		// ����������
	__time64_t tExpireDate;
 	int nLookItemID;
	int nLifespan;
	bool bExpireComplete;
	char cPotentialMoveCount;		//�̵�ī��Ʈ 0�̸� �̵����� ���� 0�̻��̸� �̵��� ȸ��
	char cDragonJewelType;
};

struct TFarmWareHouseItem:public TItem
{
	INT64		biUniqueID;
	__time64_t	tMaterializeDate;
};

struct TQuestItem
{
	int nItemID;
	INT64 nSerial;	// �ø���
	short wCount;	// ����
};

// ����Ʈ���� �ʿ��� ������/���� ī���� ���� 6����Ʈ
const int QUEST_EXTRACOUNT_MAX = 10;

struct TCount
{
	char cType;		// cType �� ���͸� ���� �ε��� �������̸� ������ �ε��� (EnumCountingType)
	int nIndex;
	unsigned short nCnt;
	unsigned short nTargetCnt;
};

const int ExtraSize = sizeof(TCount) * QUEST_EXTRACOUNT_MAX;

struct TQuest
{
	int				nQuestID;				// ����Ʈ �ε���
	char			cQuestState;			// ����Ʈ ���� (EnumQuestState)
	short			nQuestStep;				// ���� ����Ʈ ����
	char			cQuestJournal;			// ���� ����Ʈ ����
	__time64_t		tRegistDate;			// ��Ͻð�
	char			Extra[ExtraSize];		// TCount cnt[QUEST_EXTRACOUNT_MAX]	// ���� 210 bytes
	TP_QUESTMEMO	nMemo[QUESTMEMOMAX];	 // �޸� (�ε��� 1~10, ��ȹ�� ��û����)
};

const int MAX_QUEST_INDEX = 10000;
const int COMPLETEQUEST_BITSIZE = (MAX_QUEST_INDEX / CHAR_BIT);
#if defined(PRE_FIX_QUESTCOUNT)
const int MAX_PLAY_QUEST = 40;
#else	// #if defined(PRE_FIX_QUESTCOUNT)
const int MAX_PLAY_QUEST = 25;
#endif	// #if defined(PRE_FIX_QUESTCOUNT)
const int MAX_BROWSER_URL = 64;

struct TQuestGroup
{
	TQuest Quest[MAX_PLAY_QUEST];
	char CompleteQuest[COMPLETEQUEST_BITSIZE];
};

struct TSkill
{
	int nSkillID;		// ��ų���̵�
	BYTE cSkillLevel;	// ��ų����
	int nCoolTime;		// ��Ÿ��
	bool bLock;			// �⺻�� true.. ���Ŀ� �ʿ������ ��.
};

struct TSkillGroup
{
	bool bResetSkill;						// ��ų�ʱ�ȭ �ߴ��� (�⺻�� false �ѹ��ϸ� true)
	USHORT wSkillPoint;						// ��ų����Ʈ
	TSkill SkillList[SKILLMAX];				// ��ų
};

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
struct TEffectSkillData
{
	INT64 nItemSerial;
	int nItemID;
	int nSkillID;
	int nSkillLevel;
	__time64_t tExpireDate;	
	int nRemainTime;
	bool bEternity;	// false:�Ⱓ��, true:����
	bool bApplySkill;
};
#endif

struct TGesture
{
	int nGestureID;		//����ó���̵�
};

struct TTimeEvent 
{
	int nItemID;
	INT64 nRemainTime;
	bool bCheckFlag;
};

struct TTimeEventGroup
{
	TTimeEvent Event[TIMEEVENTMAX];
};

struct TUserData
{
	TCharacterStatus Status;
	TPvPGroup PvP;	
	TSkillGroup Skill[DualSkill::Type::MAX];	
	TQuestGroup	Quest;
	TAppellationGroup Appellation;
	TMissionGroup Mission;
	TTimeEventGroup TimeEvent;
};

struct TMasterSystemOppositeInfo
{
	INT64 CharacterDBID;
	WCHAR wszCharName[NAMELENMAX];
	int	nFavorPoint;
};

struct TMasterSystemSimpleInfo
{
	int	iMasterCount;
	int	iPupilCount;
	int	iGraduateCount;
	__time64_t BlockDate;
	BYTE cCharacterDBIDCount;
	TMasterSystemOppositeInfo OppositeInfo[MasterSystem::Max::OppositeCount];
};

struct TMasterSystemData
{
	TMasterSystemSimpleInfo SimpleInfo;
};

struct TRestraint
{
	UINT nRestraintKey;
	int nRestraintType;		//eRestraintType
	__time64_t _tBegineTime;
	__time64_t _tEndTime;
	WCHAR wszRestraintReason[RESTRAINTREASONMAX];
};

struct TRestraintData
{
	TRestraint Restraint[RESTRAINTMAX];
};

struct TChracterLevel
{
	INT64 biCharacterDBID;
	BYTE cLevel;
};

struct TUnionMembership
{
	BYTE cType;
	int nItemID;
	__time64_t tExpireDate;
};

struct ItemCount
{
	int id;
	int count;
};

#pragma pack(pop)
