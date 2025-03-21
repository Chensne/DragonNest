#pragma once

// Note : 커스텀 컨트롤 ID
//		EtUITool, CustomControlList.txt에 정의
//
enum emCustomControlId
{
	idStatusStatic = 1,
	idGaugeFace,
	idQuickSlotButton,
	idItemSlotButton,
	// Note : 더 이상 사용 안함. 하지만 번호 순서로 인해 삭제하면 안됨.
	//
	idPartyListBox,
	idSkillSlotButton,
	// Note : 더 이상 사용 안함. 하지만 번호 순서로 인해 삭제하면 안됨.
	//
	idPartyInfoStatic,
	idGaugeExp,
	idComboCount,
	idDamageCount,
	idChainCount,

	idZoneGateButton,
	idZoneNPCButton,
	idWorldVillageButton,
	idWorldGameButton,
	idQuestTree,
	idMenuButton,
	idMessageStatic,
	idSkillUpButton,
	idStageEnterButton,
	idBonusBoxButton,

	idMovieControl,
	idColorButton,
	idCpCount,
	idNpcAnswerHtmlTextBox,

	idMissionIconStatic,
	idCpJudgeControl,
	idRadioMsgButton,
	idGestureButton,
	idJobIconStatic,
	idSkillAcquireButton,
	idFishingProgressBar,

	idFarmAreaButton,
	idLoadingButton,
	idRotateToggleButton,

	idMenuListControl,
};

#define SLOT_ICON_XSIZE		50
#define SLOT_ICON_YSIZE		50

// Note : 아래 아이템, 스킬, 퀵슬롯 아이콘의 사이즈를 하나로...?
//
#define ITEMSLOT_ICON_XSIZE			50
#define ITEMSLOT_ICON_YSIZE			50

#define SKILLSLOT_ICON_XSIZE		50
#define SKILLSLOT_ICON_YSIZE		50
#define SKILLSLOT_ICON_XCOUNT		10

#define BUFFSLOT_ICON_XSIZE			25
#define BUFFSLOT_ICON_YSIZE			25
#define BUFFSLOT_ICON_XCOUNT		10
#define BUFFSLOT_ICON_PARTYMEMBER_XSIZE			25
#define BUFFSLOT_ICON_PARTYMEMBER_YSIZE			25

#define QUICKSLOT_ICON_XSIZE		50
#define QUICKSLOT_ICON_YSIZE		50

#define QUICKSLOT_SHORCUT_ICON_XSIZE		34
#define QUICKSLOT_SHORCUT_ICON_YSIZE		34

#define FACE_ICON_XSIZE			85
#define FACE_ICON_YSIZE			85

#define SKILL_TEXTURE_COUNT			10		// Note : 스킬 아이콘 텍스쳐 개수
#define SKILL_TEXTURE_ICON_COUNT	200		// Note : 하나의 텍스쳐에 아이콘 개수

#define BUFF_TEXTURE_COUNT			2
#define BUFF_TEXTURE_ICON_COUNT		100

#define ITEM_TEXTURE_COUNT			100		// 넉넉하게 잡아두고 로딩되는대로 사용한다.

#define ITEM_TEXTURE_ICON_COUNT		200
#define ITEM_TEXTURE_X_COUNT		10

#define SYMBOL_ICON_XSIZE			50
#define SYMBOL_ICON_YSIZE			50
#define SYMBOL_ICON_XCOUNT			10

#define MISSION_TEXTURE_ICON_COUNT	200

#define MAX_BUFF_SLOT				7 // MAX_BUBBLE_ONLY_SLOT + MAX_BUFF_ONLY_SLOT
#define MAX_BUFF_ONLY_SLOT			5
#define MAX_BUBBLE_ONLY_SLOT		2

#define MAX_BUFF_SLOT_MINIGAUGE		5

#define BUBBLE_COOLTIME_TEXTURE_IDX	9

#define RADIOMSG_TEXTURE_COUNT		1
#define RADIOMSG_TEXTURE_ICON_COUNT	200
#define RADIOMSGSLOT_ICON_XSIZE		50
#define RADIOMSGSLOT_ICON_YSIZE		50

#define GESTURE_TEXTURE_COUNT		1
#define SECONDARYSKILL_TEXTURE_COUNT	1
#define GESTURE_TEXTURE_ICON_COUNT	200
#define GESTURESLOT_ICON_XSIZE		50
#define GESTURESLOT_ICON_YSIZE		50

#define JOB_NORMALSIZE_ICON_XSIZE	55
#define JOB_NORMALSIZE_ICON_YSIZE	55

#define JOB_SMALLSIZE_ICON_XSIZE	28
#define JOB_SMALLSIZE_ICON_YSIZE	28

#define GUILDMARK_TEXTURE_COUNT		50
#define GUILDMARK_TEXTURE_ICON_COUNT	64
#define GUILDMARK_ICON_XSIZE		64
#define GUILDMARK_ICON_YSIZE		64

enum GAUGE_FACE_TYPE
{
	FT_NONE = -1,
	FT_PLAYER,
	FT_MINI_PLAYER,
	FT_ENEMY,
	FT_MINI_ENEMY,
	FT_MONSTER_PARTS,
};

enum ITEM_SLOT_TYPE
{
	ST_ITEM_NONE,
	ST_QUICKSLOT,
	ST_INVENTORY,
	ST_INVENTORY_QUEST,
	ST_INVENTORY_CASH,
	ST_INVENTORY_VEHICLE,

	ST_INVENTORY_CASHREMOVE, // PRE_ADD_CASHREMOVE

	// 이 enum으로 캐릭터창의 ItemSlot인지 등을 구분할때 쓴다.(장착할 수 있는 밝은 표시)
	ST_CHARSTATUS,
	ST_SKILL,
	ST_QUEST,
	ST_STORAGE_PLAYER,
	ST_STORAGE_GUILD,
	ST_STORAGE_FARM,
	ST_STORE,
	ST_SKILL_STORE,

	// 별다른 처리가 없더라도 비교 툴팁이 뜨려면, NONE으로 해선 안된다.
	// 현재 판매확인창에서만 비교툴팁이 장비더라도 안뜨게 되어있다.
	ST_EXCHANGE,
	ST_ITEM_DISJOINT,
	ST_PLATE,
	ST_JEWEL,	// 문장보옥 합성창에 들어가는 보옥은 플레이트와 달리 테두리를 안찍는다.
	ST_ITEM_UPGRADE,
	ST_ITEM_UPGRADE_JEWEL,	// 재료템
	ST_ITEM_COMPOUND,
	ST_ITEM_COMPOUND_JEWEL,	// 재료템

	ST_INSPECT,	// 살펴보기

	ST_RADIOMSG,			// 라디오메세지 스킬창
	ST_RADIOMSG_QUICKSLOT,	// 라디오메세지 퀵슬롯
	ST_GESTURE,				// 제스처 인벤창
	ST_SECONDARYSKILL,		// 2차 스킬창(낚시, 요리)
	ST_LIFESKILL_QUICKSLOT,	// LifeSkill퀵슬롯(Normal_Stand에서만 발동하는 스킬 퀵슬롯-제스처,SecondarySkill 포함관계)
	ST_MARKET_REGIST,		// 무인상점 등록 아이템
	ST_INVENTORY_CASHSHOP,
	ST_ASKLOOKCHANGE,

	ST_NPCREWARD,			// npc선택지 보상 슬롯
	ST_MISSIONREWARD,		// 미션 보상 슬롯
	ST_NPC_PRESENT,			// NPC 가 받는 선물 슬롯

	ST_ITEM_COSTUMEMIX_STUFF,
	ST_ITEM_COSTUMEMIX_RESULT,

	ST_INVENTORY_CASHSHOP_REFUND,

	ST_FARM_CONDITION,
	ST_LIFESKILL_QUICKSLOT_EX,	// LifeSkill퀵슬롯에 들어가는 아이템 퀵슬롯(탈것, 펫)

	ST_ITEM_COSTUME_DMIX_STUFF,
	ST_ITEM_COSTUME_DMIX_RESULT,

	ST_ITEM_COSTUME_RANDOMMIX_STUFF,
	ST_ITEM_COSTUME_RANDOMMIX_RESULT,

	ST_SET_FONTCOLOR,		// Element 의 FontColor 가 아닌 m_dwFontColor 값으로 폰트색상출력.
	ST_CASHINSTANCE_ITEM,	// 캐시샵 간편구매 아이템 슬롯

	ST_ITEM_UPGRADE_EXCHANGE_SOURCE,
	ST_ITEM_UPGRADE_EXCHANGE_TARGET,
	ST_ITEM_UPGRADE_EXCHANGE_STUFF,
	ST_ITEM_NOTIFY_COMPOUND,

	ST_TOTAL_LEVEL_SKILL,
	ST_SPECIALBOX_REWARD,

	ST_STORAGE_WORLDSERVER_NORMAL,
	ST_STORAGE_WORLDSERVER_CASH,

#ifdef PRE_ADD_TALISMAN_SYSTEM
	ST_TALISMAN,
#endif

#if defined( PRE_PERIOD_INVENTORY )
	ST_INVENTORY_PERIOD,
	ST_STORAGE_PERIOD,
#endif	// #if defined( PRE_PERIOD_INVENTORY )
};

enum QUICKSLOT_STATE
{
	QUICKSLOT_NORMAL,
	QUICKSLOT_USABLE,
	QUICKSLOT_DELAY,
	QUICKSLOT_COMPLETE,
	QUICKSLOT_END,
	QUICKSLOT_LOWSP
};

enum ITEMSLOT_STATE
{
	ITEMSLOT_ENABLE,
	ITEMSLOT_DISABLE,
	ITEMSLOT_EXPIRE,
};

enum SKILLSLOT_STATE
{
	SKILLSLOT_ENABLE,
	SKILLSLOT_DISABLE,
};

enum COLOR_TYPE
{
	COLOR_NONE = -1,
	COLOR_WHITE = 0,
	COLOR_ORANGE,
	COLOR_RED,
	COLOR_DAHONG,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_BLUE,
	COLOR_PURPLE,
	COLOR_BLUE1,
	COLOR_RED1,
	COLOR_COUNT,
};

inline void CalcButtonUV( int nIndex, EtTextureHandle hTexture, SUICoord &UVCoord, int nIconWidth, int nIconHeight )
{
	if( !hTexture ) return;

	int nCountX = hTexture->Width() / nIconWidth;
	int nCountY = hTexture->Height() / nIconHeight;
	float fTexWidth = nIconWidth / ( float )hTexture->Width();
	float fTexHeight = nIconHeight / ( float )hTexture->Height();

	UVCoord.SetCoord( (nIndex%nCountX)*fTexWidth, (nIndex/nCountX)*fTexHeight, fTexWidth, fTexHeight );
}

inline void CalcButtonUV( int nIndex, EtTextureHandle hTexture, int &nX, int &nY, int nIconWidth, int nIconHeight )
{
	if( !hTexture ) return;

	int nCountX = hTexture->Width() / nIconWidth;
	int nCountY = hTexture->Height() / nIconHeight;

	nX = (nIndex%nCountX) * nIconWidth;
	nY = (nIndex/nCountX) * nIconWidth;
}


// 딱히 둘데가 없어서 여기 둔다.
// 커스텀 버튼 말고는 핫키 표시하는 컨트롤 없으려나..
void CalcHotKeyUV( int nVKHotKey, EtTextureHandle hTexture, SUICoord &UVCoord, int nIconWidth, int nIconHeight );