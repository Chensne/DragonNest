#pragma once

#include "DNCommonDef.h"
#include "../../GameCommon/DNNotifierCommon.h"

/* -----------------------------------------------------------------------------
	여기 구조체는 DB저장용 구조체
	저장안하는 변수는 각 클래스에서 알아서 생성해주세용~
----------------------------------------------------------------------------- */

#pragma pack(push, 1)

struct TQuickSlot
{
	BYTE cType;		// eQuickSlotType참조
	INT64 nID; 
};

#if !defined(PRE_DELETE_DUNGEONCLEAR)
struct TDungeonClearData
{
	int nMapIndex;	// 맵아이디
	char cType;		// 클리어 타입? (eDungeonClearType)
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

	char cAccountLevel;						// 운영자, 개발자 등등의 계정레벨
	WCHAR wszCharacterName[NAMELENMAX];		// 캐릭명
	char cClass;							// 클래스 (워리어, 아처, 소서리스, 클래릭)

	bool IsCheckFirstVillage;				// 캐릭 생성 후 빌리지 처음 접속 체크

	int nDefaultBody;						// 처음 생성시에 설정한 equip (EQUIP_BODY, EQUIP_LEG, EQUIP_HAND, EQUIP_FOOT)
	int nDefaultLeg;
	int nDefaultHand;
	int nDefaultFoot;

	BYTE cLevel;							// 레벨
	int nExp;								// 경험치
	BYTE cJob;								// 직업

	DWORD dwHairColor;		// 머리 색깔
	DWORD dwEyeColor;		// 눈 색깔
	DWORD dwSkinColor;		// 피부 색깔

	int nMapIndex;							// 현재 맵번호
	int nLastVillageMapIndex;				// 마지막 마을 맵번호
	int nLastSubVillageMapIndex;			// 마지막 Sub마을 맵번호
	char cLastVillageGateNo;				// 마지막 마을 게이트 번호
	int nPosX;								// 현재 X좌표
	int nPosY;								// 현재 Y좌표
	int nPosZ;								// 현재 Z좌표
	float fRotate;							// Rotate

	INT64 nCoin;							// 돈
	INT64 nWarehouseCoin;					// 창고돈

	BYTE cRebirthCoin;						// 코인카운트 (Coin이랑은 다른것, 죽으면 되살아날때 쓰는 코인)
	BYTE cPCBangRebirthCoin;				// 피씨방 코인카운트 (Coin이랑은 다른것, 죽으면 되살아날때 쓰는 코인)
	USHORT wCashRebirthCoin;				// 캐쉬로 산 코인카운트 (Coin이랑은 다른것, 죽으면 되살아날때 쓰는 코인)

	short wFatigue;							// 피로도
	short wWeeklyFatigue;					// 주간피로도
	short wPCBangFatigue;					// 피씨방피로도
	short wEventFatigue;
	short wVIPFatigue;

	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];	// 4 * 8 = 32개

	BYTE cJobArray[JOBMAX];					// 직업 (0: 맨처음 직업, 나머지 쭉쭉~ 전직 포함)
	DNNotifier::Data NotifierData[DNNotifier::RegisterCount::Total];
	TQuickSlot QuickSlot[QUICKSLOTMAX];		// 단축 슬롯창 (10개씩 2줄)
#if !defined(PRE_DELETE_DUNGEONCLEAR)
	TDungeonClearData DungeonClear[DUNGEONCLEARMAX];	// 던전 클리어 (120개)
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)
	TNestClearData NestClear[NESTCLEARMAX];	// 네스트 클리어
	int nDungeonClearCount;					// 던젼 클리어 수
	int nGlyphDelayTime;
	int nGlyphRemainTime;

	short wMarketRegisterCount;				// 마켓 등록 가능 횟수
	int nMarketSellSuccess;					// 마켓 판매 성공 횟수
	int nMarketSellFailure;					// 마켓 판매 실패 횟수
	int nMarketRegisterTax;					// 마켓 누적 등록 수수료
	int nMarketSellTax;						// 마켓 누적 판매 수수료

	int nPetal;								// 적립금 (페탈)

	char cSkillPage;						// 현재 활성화 되어있는 스킬페이지
#if defined( PRE_PRIVATECHAT_CHANNEL )
	INT64 nPrivateChatChannel;				// 현재 소속되어있는 사설 채팅 번호
#endif
#if defined( PRE_ADD_STAMPSYSTEM )
	__time64_t tLastStampDate;				// 마지막으로 스탬프 시스템에 기록을 남긴 시간
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
	int nMissionScore;						// 미션점수
	short wLastMissionAchieve[LASTMISSIONACHIEVEMAX];	// 마지막 목표달성(?)

	//아래 플래그들은 8bit 단위로 끊어서 왼쪽 비트 부터 처리함 (ex 미션 시작값 0 : 1000 0000 에 표기, 1 은 0100 0000 )
	char MissionGain[MISSIONMAX_BITSIZE];		// 획득여부
	char MissionAchieve[MISSIONMAX_BITSIZE];	// 목표달성(on/off)

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
	int nSelectAppellation;							// 선택된 호칭
	int nCoverAppellation;							// 커버 호칭
	char Appellation[APPELLATIONMAX_BITSIZE];		// 호칭
};

struct TPvPGroup
{
	BYTE	cLevel;												// 레벨
	UINT	uiXP;												// XP
	UINT	uiPlayTimeSec;										// 플레이타임(초)
	UINT	uiWin;												// 승리
	UINT	uiLose;												// 패배
	UINT	uiDraw;												// 무승부
	UINT	uiGiveUpCount;										// 중간이탈수
	UINT	uiKOClassCount[PvPCommon::Common::MaxClass];		// 각 클래스를 KO시킨 횟수
	UINT	uiKObyClassCount[PvPCommon::Common::MaxClass];		// 각 클래스에게 KO당한 회수
	INT64	biTotalKillPoint;
	INT64	biTotalAssistPoint;
	INT64	biTotalSupportPoint;
	int nExpAbsoluteRank;		//상위 101등까지만 랭크값이 넘어온다. 실재 클라이언트에 보여지는 랭크와는 별개
	float fExpRateRank;			//특정 exp이상의 유저들은 상위 인원중 몇 퍼센트에 드는 유저인지 넘어온다
};

struct TItem
{
	int nItemID;
	INT64 nSerial;		// 시리얼
	int nRandomSeed;
	USHORT wDur;			// 내구도
	short wCount;		// 개수
	int nCoolTime;		// 남은 쿨타임
	char cLevel;		// 레벨
	char cPotential;	// 잠재력
	bool bSoulbound;	// 개봉여부 (착용 시 귀속)
	char cOption;
	char cSealCount;	// 밀봉카운트
	bool bEternity;		// 영구아이템
	__time64_t tExpireDate;
 	int nLookItemID;
	int nLifespan;
	bool bExpireComplete;
	char cPotentialMoveCount;		//이동카운트 0이면 이동한적 없음 0이상이면 이동한 회수
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
	INT64 nSerial;	// 시리얼
	short wCount;	// 개수
};

// 퀘스트에서 필요한 아이템/몬스터 카운팅 정보 6바이트
const int QUEST_EXTRACOUNT_MAX = 10;

struct TCount
{
	char cType;		// cType 이 몬스터면 몬스터 인덱스 아이템이면 아이템 인덱스 (EnumCountingType)
	int nIndex;
	unsigned short nCnt;
	unsigned short nTargetCnt;
};

const int ExtraSize = sizeof(TCount) * QUEST_EXTRACOUNT_MAX;

struct TQuest
{
	int				nQuestID;				// 퀘스트 인덱스
	char			cQuestState;			// 퀘스트 상태 (EnumQuestState)
	short			nQuestStep;				// 현재 퀘스트 스텝
	char			cQuestJournal;			// 현재 퀘스트 저널
	__time64_t		tRegistDate;			// 등록시간
	char			Extra[ExtraSize];		// TCount cnt[QUEST_EXTRACOUNT_MAX]	// 여분 210 bytes
	TP_QUESTMEMO	nMemo[QUESTMEMOMAX];	 // 메모 (인덱스 1~10, 기획팀 요청사항)
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
	int nSkillID;		// 스킬아이디
	BYTE cSkillLevel;	// 스킬레벨
	int nCoolTime;		// 쿨타임
	bool bLock;			// 기본이 true.. 추후에 필요없어질 듯.
};

struct TSkillGroup
{
	bool bResetSkill;						// 스킬초기화 했는지 (기본은 false 한번하면 true)
	USHORT wSkillPoint;						// 스킬포인트
	TSkill SkillList[SKILLMAX];				// 스킬
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
	bool bEternity;	// false:기간제, true:영구
	bool bApplySkill;
};
#endif

struct TGesture
{
	int nGestureID;		//제스처아이디
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
