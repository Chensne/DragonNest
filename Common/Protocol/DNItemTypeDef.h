#pragma once

enum eItemTypeEnum
{
	ITEMTYPE_WEAPON = 0,		// 무기/공격보조구
	ITEMTYPE_PARTS = 1,			// 방어구
	ITEMTYPE_NORMAL = 2,		// 기타
	ITEMTYPE_SKILL = 3,			// 스킬
	ITEMTYPE_JEWEL = 4,			// 보옥
	ITEMTYPE_PLATE = 5,			// 문장
	ITEMTYPE_INSTANT = 6,		// 줍자마자 즉시효과(바닥에 고깃덩어리)
	ITEMTYPE_QUEST = 7,			// 퀘스트
	ITEMTYPE_RANDOM = 8,		// 랜덤아이템
	ITEMTYPE_HAIRDYE = 9,		// 염색약
	ITEMTYPE_SKILLBOOK = 10,	// 스킬 북
	// 캐쉬 아이템 관련 타입
	ITEMTYPE_ENCHANT_SUCCESSUP = 11, // 강화확률 증가
	ITEMTYPE_ENCHANT_BREAKGAURD = 12,// 강화 실패시 파괴 방지
	ITEMTYPE_REBIRTH_COIN = 13,		// 부활코인
	ITEMTYPE_INVENTORY_SLOT = 14,	// 인벤토리 증가
	ITEMTYPE_WAREHOUSE_SLOT = 15,	// 창고 증가
	ITEMTYPE_GESTURE = 16,			// 제스쳐
	ITEMTYPE_CHARACTER_SLOT = 17,	// 캐릭터 생성 슬롯 확장
	ITEMTYPE_SKINDYE = 18,			// 피부색
	ITEMTYPE_FACIAL = 19,			// 얼굴표정 타입 변경
	ITEMTYPE_HAIRDRESS = 20,		// 머리모양 변경
	ITEMTYPE_EYEDYE = 21,			// 눈깔색
	ITEMTYPE_FORCEUSE_SKILL = 22,	// 고랩스킬 강제사용
	ITEMTYPE_PREMIUM_POST = 23,		// 프리미엄 우편
	ITEMTYPE_FATIGUEUP = 24,		// 피로도 증가
	ITEMTYPE_FORCEUSE_EQUIP = 25,	// 고랩장비 강제사용
	ITEMTYPE_PREMIUM_TRADE = 26,	// 프리미엄 무인거래소
	ITEMTYPE_SEAL = 27,				// 밀봉
	ITEMTYPE_REWARDITEM_IDENTIFY = 28, // 보상 아이템 돋보기
	ITEMTYPE_RESET_SKILL = 29,		// 스킬 포인트 리셋
	ITEMTYPE_WORLDMSG = 30,			// 월드존 대화
	ITEMTYPE_SLIDEMSG = 31,			// 슬라이드 대화
	ITEMTYPE_GACHACOIN = 32,		// 가챠폰 코인
	ITEMTYPE_VIP = 33,				// VIP아이템
	ITEMTYPE_PETEGG = 34,			// 펫 알
	ITEMTYPE_PET = 35,				// 펫
	ITEMTYPE_PETPARTS = 36,			// 펫 파츠
	ITEMTYPE_PETFEED = 37,			// 펫 먹이
	ITEMTYPE_GLYPH = 38,			// 문장
	ITEMTYPE_COSTUMEMIX = 39,
	ITEMTYPE_VOICEFONT = 40,
	ITEMTYPE_SURROUNDCHAT = 41,
	ITEMTYPE_NPC_VOICEPLAYER = 42,	// NPC 호감도 보상으로 주어지는 NPC 목소리 재생기
	ITEMTYPE_POTENTIAL_JEWEL = 43,
	ITEMTYPE_VEHICLE = 44,
	ITEMTYPE_VEHICLEPARTS = 45,
	ITEMTYPE_CHARM = 46,
	ITEMTYPE_EXPAND = 47,
	ITEMTYPE_VEHICLEHAIRCOLOR = 48,
	ITEMTYPE_PETALTOKEN = 49,
	ITEMTYPE_SECONDARYSKILL_RECIPE = 50,	// 보조스킬 레시피
	ITEMTYPE_APPELLATION = 51,				// 호칭아이템
	ITEMTYPE_SEED = 52,						// 씨앗
	ITEMTYPE_GUILDRENAME = 53,				// 길드명 변경
	ITEMTYPE_GROWING_BOOST = 54,			// 성장촉진제
	ITEMTYPE_WATER = 55,					// 물
	ITEMTYPE_FISHINGROD = 56,				// 낚시대
	ITEMTYPE_WATERBOTTLE = 57,				// 물통
	ITEMTYPE_ENCHANT_SHIELD = 58,			// 강화 실패시 (다운 x, 파괴 x)
	ITEMTYPE_GUILDWARE_SLOT = 59,			// 길드창고 증가.
	ITEMTYPE_FISHINGBAIT = 60,				// 낚시미끼
	ITEMTYPE_CHARNAME = 61,
	ITEMTYPE_VEHICLEEFFECT = 62,
	ITEMTYPE_SEADRAGONREBIRTH = 63,			//씨드래곤 네스트에서 부활 아이템
	ITEMTYPE_CROP = 64,						// 농작물
	ITEMTYPE_COOKING = 65,					// 요리
	ITEMTYPE_FISH = 66,						// 수산물
	ITEMTYPE_ENCHANT_JEWEL = 67,			// 인챈트 직행 아이템
	ITEMTYPE_GUILDMARK = 68,				// 길드마크
	ITEMTYPE_UNION_MEMBERSHIP = 69,			// 연합 멤버쉽
	ITEMTYPE_REMOTE_ENCHANT = 70,			// 원격 강화 아이템
	ITEMTYPE_HELPBOOK = 71,					// 레벨업 보상 알림창 열기 기능
	ITEMTYPE_RETURN_HOME = 72,				// 길드전 귀환 아이템
	ITEMTYPE_COSTUMEDESIGN_MIX = 73,		// 코스튬 디자인 합성
	ITEMTYPE_CHANGEJOB = 74,				// 전직 아이템 #33233
	ITEMTYPE_REPAIR_EQUIPITEM = 75,			// 장비아이템 수리 아이템
	ITEMTYPE_FARM_VIP = 76,					// 농장VIP
	ITEMTYPE_REMOTE_ITEMCOMPOUND = 77,		// 원격 아이템 제작
	ITEMTYPE_PET_EXPIRE = 78,				// 펫 기간연장 아이템
	ITEMTYPE_INFINITY_RESET_SKILL = 79,		// 무제한 스킬 리셋 아이템
	ITEMTYPE_PETCOLOR_BODY = 80,			// 펫 염색1
	ITEMTYPE_PETCOLOR_TATOO = 81,			// 펫 염색2
	ITEMTYPE_GLYPH_SLOT = 82,				// 문장 슬롯
	ITEMTYPE_IMMEDIATEREBIRTH = 83,			// 길드전전용(지금은 변경가능) 즉시부활아이템
	ITEMTYPE_PERIOD_APPELLATION = 84,		// 기간제칭호
	ITEMTYPE_PERIOD_PLATE = 85,				// 기간제문장슬롯
	ITEMTYPE_INCREASE_LIFE = 86,			// 생명석 증가
	ITEMTYPE_PERIODEXPITEM = 87,			// 경험치증가 아이템
	ITEMTYPE_PET_RENAME = 88,
	ITEMTYPE_EXPAND_SKILLPAGE = 89,			//이중스킬트리 확장 아이템
	ITEMTYPE_SOURCE = 90,					// 다크레어 보상 - 근원 아이템
	ITEMTYPE_PET_EXP = 91,					// 펫 경험치 증가
	ITEMTYPE_PET_SKILL = 92,				// 펫 스킬 아이템
	ITEMTYPE_PET_SKILLSLOT = 93,			// 펫 스킬 슬롯
	ITENTYPE_FARM_PRIVATE_EXTNED = 94,		// 농장(개인구역) 확장
	ITEMTYPE_FREE_PASS = 95,
	ITEMTYPE_UNLIMITED_FREE_PASS = 96,
	ITEMTYPE_SEALABLENORMAL = 97,			// 탈것 마패 타입.
	ITEMTYPE_REMOTE_WAREHOUSE = 98,			// 원격 창고 아이템
	ITEMTYPE_COSTUMERANDOM_MIX = 99,		// 코스튬 랜덤 합성
	ITEMTYPE_SALE_COUPON = 100,				// 캐쉬 할인구매 쿠폰
	ITEMTYPE_DONATION_COUPON = 101,			// 기부 교환권
	ITEMTYPE_AUTOUNPACK = 102,				// 보물상자 자동열기 아이템.
	ITEMTYPE_POTENTIAL_EXTRACTOR = 103,		// 잠재력 추출기	
	ITEMTYPE_PET_FOOD = 104,				// 펫 먹이
	ITEMTYPE_GLOBAL_PARTY_BUFF = 105,       // 파티원 버프주는 아이이템 타입 / 마을에서도 사용가능
	ITEMTYPE_CHAOSCUBE = 106,				// 카오스큐브 - 재료아이템을 다른아이템으로 변환.
	ITEMTYPE_FIXEDPRICE = 107,				// 호감도로 인한 판매금액의 변화가 없는 아이템
	ITEMTYPE_BESTFRIEND = 108,				// 절친 등록서
	ITEMTYPE_GIFTBOX = 109,					// 절친 보상상자박스
	ITEMTYPE_MENUBOX = 110,					// 절친 보상상자 메뉴 설정
	ITEMTYPE_BESTFRIENDBUFFITEM = 111,		// 절친 버프 아이템
	ITEMTYPE_ALLGIVECHARM = 112,			// 오픈시 연결된 아이템 전부다 주는 타입
	ITEMTYPE_CHOICECUBE = 113,				// 재료아이템을 넣고 결과아이템 중 한개를 유저가 선택해서 받는 큐브
	ITEMTYPE_REBIRTH_COIN_EX = 114,			// 부활코인 (캐쉬아이템화)
	ITEMTYPE_INVENTORY_SLOT_EX = 115,		// 인벤토리 증가 (캐쉬아이템화)
	ITEMTYPE_WAREHOUSE_SLOT_EX = 116,		// 창고 증가 (캐쉬아이템화)
	ITEMTYPE_EXPUP_ITEM = 117,				// 경험치증가 아이템(절대값으로 증가)
	ITEMTYPE_ULTIMATEFATIGUEUP = 118,		// 무제한 피로도 물약
	ITEMTYPE_EXPAND_TS = 119,
	ITEMTYPE_TOTALSKILLLEVEL_SLOT = 120,	// 통합스킬캐시슬롯
	ITEMTYPE_TRANSFORMPOTION = 121,			// 변신 물약
	ITEMTYPE_CHARMRANDOM = 122,				// 랜덤 개수로 랜덤 박스에서 아이템 얻기  
	ITEMTYPE_DIRECT_PARTYBUFF = 123,		// 아이템 소지시 스테이지 입장시 자동으로 버프 적용(종료시 자동으로 없어짐)
	ITEMTYPE_PERIOD_PLATE_EX = 124,			// 기간제 문장 슬롯 (캐쉬아이템화)
	ITEMTYPE_COMEBACK_EFFECTITEM = 125,		// 귀환자아이템
	ITEMTYPE_STAGE_COMPLETE_USEITEM = 126,	// 트래져 스테이지 클리어시 소모되는 아이템(던전 클리어시 소모되는 입장권)
	ITEMTYPE_ADD_QUEST = 127,				// 퀘스트 획득 아이템
	ITEMTYPE_PARTS_RING = 128,				// 반지 아이템.
	ITEMTYPE_SERVERWARE_COIN = 129,			// 서버 창고 코인 아이템
	ITEMTYPE_HEAD_SCALE_POTION = 130,		// 대두물약
	ITEMTYPE_PVPEXPUP = 131,				// 콜로세움 경험치 증가 아이템
#if defined(PRE_ADD_TALISMAN_SYSTEM) 
	ITEMTYPE_TALISMAN = 132,				// 탈리스만 아이템
	ITEMTYPE_PERIOD_TALISMAN_EX = 133,		// 기간제 캐쉬 탈리스만 슬롯
#endif
#if defined(PRE_PERIOD_INVENTORY)
	ITEMTYPE_PERIOD_INVENTORY = 134,		// 기간제 인벤토리
	ITEMTYPE_PERIOD_WAREHOUSE = 135,		// 기간제 창고
#endif	// #if defined(PRE_PERIOD_INVENTORY)
#if defined( PRE_ADD_NEW_MONEY_SEED )
	ITEMTYPE_NEWMONEY_SEED = 136,			// 캐릭터귀속화폐(시드)
#endif
	ITEMTYPE_VILLAGEBUFF_CASHTABOPEN = 137, // 빌리지 버프 타입에 캐시 카테고리 오픈되는 기능
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	ITEMTYPE_SETITEM = 138,					// 하나의 코스튬에 다중 세트효과
#endif
#if defined( PRE_ADD_DRAGON_GEM )
	ITEMTYPE_DRAGON_GEM = 139,				// 용옥 아이템
	ITEMTYPE_DRAGON_GEM_REMOVE = 140,		// 용옥 제거 아이템
#endif
	ITEMTYPE_VEHICLE_SHARE = 155,
};