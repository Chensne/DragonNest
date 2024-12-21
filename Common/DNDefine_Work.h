//kat 2016
//#define PRE_ADD_DRAGON_GEM							// anno414, devkang  2013-01-29	// 용옥(이슈발행되면 수정하겠습니다) 
#define PRE_ADD_TALISMAN_SYSTEM						// anno414		2012-12-10		// #69319 [Main] 탈리스만 아이템  //stupidfox
//#define PRE_MOD_GACHA_SYSTEM						// kalliste		2013-02-26		// #82401 2013 아바가차 시스템 변경_프로그램
#define PRE_ALTEIAWORLD_EXPLORE						// dyss			2012-11-07		// #66758 [Main] 알테이아 대륙 제작 작업(#72749,#72750)
//#define PRE_ADD_MAINQUEST_UI						// anno414,devkang		2013-02-26		// #83028 메인 퀘스트UI 개편

#pragma once

//#########################################################################################################################################
// 개발중인 Define
//#########################################################################################################################################

//#if defined( _WORK )
//#define PRE_FIX_RANDOM_MAPINDEX						// karl
#define PRE_SKIP_REWARDBOX							// dyss			2013-03-05		// #83958 랭크 보상 선택 시 소요시간 단축
#define PRE_MOD_GACHA_SYSTEM						// kalliste		2013-02-26		// #82401 2013 아바가차 시스템 변경_프로그램
#define PRE_ADD_CASHINVENTAB						// bintitle		2013-02-28		// #77919 인벤토리 캐시 탭 정리 개발.
#define PRE_MOD_STAGE_EASY_DIFFICULTY_HIDE			// elkain03		2013-02-27		// #83338 [70Lv개방_아누아렌델] 난이도 UI에서 '쉬움' 난이도 Hide 작업
#define PRE_ADD_RENEWUI								// kalliste		2013-02-26		// #83694 [UI개편]드네 UI의 리뉴얼 리소스관리 필요작업
#define PRE_ADD_MAINQUEST_UI						// anno414,devkang		2013-02-26		// #83028 메인 퀘스트UI 개편
#define PRE_FIX_PVP_LOBBY_RENEW						// kalliste		2013-02-26		// #83581 콜로세움 로비 UI 에서 생성 방의 난입 가능 여부 확인_UI (3/27 업데이트 예정)
#define PRE_ADD_SQL_RENEW							// haling		2013-02-21		// SQLConnection Renew..
//#define PRE_ADD_CRAZYDUC_UI							// elkain03		2013-02-21		// #82688 [전 국가] 3주년 이벤트_크레이지덕 네스트 mod 제작 
#define PRE_ADD_SEETMAINTENANCEFLAG					// 2hogi		2013-02-21		// #83094 점검시 드네앱제어를 자동화하는 방안
#define PRE_ADD_REMOTE_OPENSHOP						// elkian03		2013-02-18		// #80593 [Main] 원격 연합상점
#define PRE_ADD_CHAT_MISSION						// stupidfox	2013-02-20		// #82755 채팅 미션
#define PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME			// karl			2013-02-18		// #82707 [DN]주간이벤트미션 종료일 추가 작업 요청
#define PRE_ADD_PVPRANK_INFORM						// kalliste		2013-02-19		// #81769 콜로세움 등급안내_클라이언트
#define PRE_ADD_LEVELUP_REWARD_NOTIFIER				// elkain03		2013-02-14		// #81608 레벨업 보상 바로 받기
#define PRE_FIX_HITFINISH							// dyss			2013-02-18		// #82720 AI 'g_Lua_BeHitSkill' 여러개의 스킬 셋팅가능하게 수정
//#define PRE_FIX_DROPITEM_OWNER					// dyss			2013-02-14		// 드랍아이템 소유권 세션아이디에서 캐릭터아이디로 변경
//#define PRE_ADD_OVERLAP_SETEFFECT					// dyss,anno414 2013-02-14		// #38238 중복세트효과 시스템개발
#define PRE_ADD_STAGE_WEIGHT						// devkang,rotha 2013-02-14		// #79289 어비스 파티 인원수에 따른 난이도 및 드랍 변경
#define PRE_FIX_QUESTCOUNT							// saset		2013-02-14		// #81987 [Main] 퀘스트 소지 개수 증가 요청
#define PRE_ADD_DRAGON_GEM							// anno414, devkang  2013-01-29	// 용옥(이슈발행되면 수정하겠습니다)

#define PRE_FIX_REMOVE_AURA_ONDIE					// kalliste		2013-02-05		// #81122 [챔피온몬스터] 지속타임 '오라'를 사용할 경우 몬스터가 죽어도 효과가 사라지지 않는 현상
#define PRE_ADD_MONSTER_CHEAT						// elkain03		2013-02-05		// #81658 몬스터 동작 확인용 치트키 개선
#define PRE_MOD_MONSTER_NAME_COLOR					// elkain03		2013-01-31		// #81119 [챔피온몬스터] 챔피온 몬스터 이름 색상 변경
#define PRE_ADD_COSTUMEMIX_TOOLTIP					// elkain03		2013-01-29		// #80837 합성 툴팁 시스템 추가(클라이언트)
//#define PRE_PERIOD_INVENTORY						// saset, verygoodd21		2013-01-28		// #77252 [Main] 기간제 인벤토리&창고 개발
//#define PRE_ADD_DRAGON_GEM							// anno414, devkang  2013-01-29	// 용옥(이슈발행되면 수정하겠습니다)
#define PRE_ADD_NPC_MARK							// elkain03		2013-01-22		// #77193 [Main][액토즈] 상점 NPC 표시 강화
//#define PRE_ADD_NEW_MONEY_SEED						// dyss, kwcsc	2013-01-24		// #78800, #78801 캐릭터 기반 귀속 화폐 - 시드
#define PRE_MOD_SECURITYNUMPAD_RENEWAL				// elkain03		2013-01-16		// 2차 비밀번호 보안 개선
#ifdef PRE_ADD_DOORS
#define PRE_ADD_DOORS_GUILDCHAT_DISCONNECT			// 2hogi		2013-01-15		// #79019 길드채닝 & #78286 비정상 접속 종료
#endif		//PRE_ADD_DOORS
#define PRE_ITEMBUFF_COOLTIME						// dyss			2013-01-16		// 아이템스킬 쿨타임 추가
#define PRE_ADD_RELOAD_ACTFILE						// elkain03		2012-12-24		// #72043 [이팩트] 스넵샷 이팩트 로딩 관련
//#define PRE_MAILRENEWAL								// saset		2012-12-11		// #76079 우편 기능 개선
#define PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG			// anno414		2012-12-03		// #74544 스크립트에서 이미지 출력하는 api 요청
#define PRE_ADD_CHANGE_MONSTER_SKIN					// elkain03		2012-11-23		// #73607 몬스터 스킨 기능 개발 
#define PRE_ADD_GAMEQUIT_REWARD						// bintitle, karl	2012-11-19	// #74038, #74039 게임 종료 시 상품지급 안내 팝업 출력 
#define PRE_SAMPLEITEMNPC							// saset		2012-11-02		// #72345 [TF-LSE]신규 상품 체험 NPC (Server)
#define PRE_MOD_INDUCE_TCPCONNECT					// 2hogi		2012-11-01		// 이슈번호 없음 UDP컨넥후 TCP컨넥처리 지연 완화 및 원인파악용 로그추가
#define PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD			// karl			2012-10-24		// #71767 스테이지/어비스 플레이 시, 인챈트된 장비 아이템이 드랍되는 기능 개발
#define PRE_ADD_HEAL_TABLE_REGULATION				// verygoodd21	2012-10-09		// #70599 힐 회복량 제한.
#define PRE_ADD_QUEST_CHECKCAHEITEM					// devkang		2012-10-05		// #24574 퀘스트 캐시 아이템 조건 데이터 추가		
#define PRE_ADD_WEEKLYEVENT							// saset, rotha		2012-08-07		// #64206 신규 이벤트 3종 - 여신의 가호
#define PRE_MOD_CORRECT_UISOUND_PLAY				// elkain03		2012-08_28		// #66964 호감도 창 오픈후 탭이동시 올바른 사운드 미출력 
//#define PRE_MOD_SYNCPACKET							// 2hogi		2012-08-22		// 이슈없음 게임서버 진입시 클라이언트 크래쉬 완화처리
//#define PRE_ADD_MAIL_UI_IMPROVEMENTS				// elkain03		2012-08-13		// #58230 우편함 UI마을내에서 열수 있도록 ( UI 개선 )		
#define PRE_ADD_EXCHANGE_ENCHANT					// haling,kalliste		2012-07-24		// #63036, #63034 강화이동 시스템- 중국만 적용
#define PRE_ADD_ENABLE_EFFECT_SIGNAL				// elkain03		2012-07-18		// #62455 파티원 파티클 효과 ON / OFF 기능 
#define PRE_MOD_CASHSHOP_GIFTBOTTON_TWINKLE_SPEEDUP // elkain03		2012-07-12		// #62255 캐쉬샵 선물함 버튼 깜빡임 속도 증가 
#define PRE_ADD_SALE_COUPON_LEVEL_LIMIT				// stupidfox	2012-11-22		// #73949 레벨제한이 걸린 할인쿠폰 기능
#define PRE_ADD_TALISMAN_SYSTEM						// anno414		2012-12-10		// #69319 [Main] 탈리스만 아이템  //stupidfox

#define PRE_MOD_OPERATINGFARM						// 2hogi		2012-05-14		// #57848 [월드통합파티룸] 농장 처리
#define PRE_MOD_LEVELUP_REWARD_SMARTMOVE_APP		// elkain03		2012-07-02		// #61437 GM 창고, 가방, 부활서 사용시 스마트 무브 기능 적용 
#define PRE_ADD_CASHSHOP_SUBCAT_COLOR				// elkain03		2012-06-26		// #54411 캐쉬샵 서브카테고리 폰트 색상 변경
#define PRE_FIX_VILLAGEZOMBIE						// 2hogi		2012-06-15		// #58730 로그인 이상 문제<0524>
#define PRE_ADD_MTRANDOM_CLIENT						// kalliste		2012-04-20		// 지역적으로 사용가능한 랜덤 클래스 제작
#define PRE_ADD_LIMITED_CASHITEM					// 2hogi,kwcsc		2012-03-09		// #53070, #53069 캐시샵 : 수량한정판매
#define PRE_MOD_SYSTEM_STATE						// kalliste		2011-11-22		// #46510 클라이언트 시스템 상태 정리 작업

//#########################################################################################################################################

//#define PRE_THREAD_ROOMDESTROY						// 김밥			2011-08-09		// #39501 게임서버 최적화-Room 파괴 Thread 로 변경

//#########################################################################################################################################

#define PRE_ADD_SHOW_MONACTION						// kalliste		2011-12-06		// #46533 [스냅샷개선] 몬스터 액션 확인용 치트키
#define PRE_ADD_CASHFISHINGITEM						// 2hogi		2011-11-29		// #42584 [유료화]농장 - 유료 낚시대 아이템
#define PRE_FIX_ATOF_ROUNDOFF						// jhk8211		2011-11-24		// #30806, #45525 비율 아이템 데이터 로드시 소수점 버려짐 문제 보완.
#define PRE_ADD_PRIVATEFARM_EXTEND_CASH				// 김밥			2011-10-31		// [유료화]농장 - 개인농장 추가 확장
#define PRE_ADD_SERVER_LOAD_SHORTENING				// hgoori		2011-10-18		// 서버 로딩 시간 단축 (주로 개발용에서만 쓰이며 특정 내용은 테스트후 릴리즈에도 적용.)
//#define STRESS_TEST								// 김밥			2010-01-06		// 게임서버 스트레스 테스트
//#define PRE_ENABLE_QUESTCHATLOG					// 김밥			2009-12-29		// 퀘스트 관련 채팅로그 출력여부
//#define PRE_TRIGGER_TEST							// 김밥			2009-12-08		// 트리거 테스트

// #define PRE_MOD_ENABLE_DARKLAIR_RAID				// kalliste		2011-03-14		// #30427 다크레어 8인 파티 활성화 작업 (보상이 결정 안됨으로 인해 적용 무기한 보류)
#define PRE_TEST_ANIMATION_UI						// kalliste		2011-07-21		// 그래픽팀 애니메이션 UI 확인용 디파인
#define PRE_OPT_CLIENT_QUEST_SCRIPT					// verygoodd21	2011-08-12		// #39490 클라이언트 최적화 작업 - 퀘스트 스크립트 최적화 작업.

#define PRE_FIX_MOVEBACK							// dyss			2011-11-10		// #45268 붙은 상태에서 Move_Back시 위치 틀어지는 부분 수정
#define PRE_FIX_48517								// dyss			2011-12-21		// 던전 클리어 후 소유권없는 입장권 나오는 부분 수정

//#define PRE_ADD_MULTILANGUAGE						// kwcsc, 2hogi		2012-01-10		// 다국어 지원 디파인
#ifdef PRE_ADD_MULTILANGUAGE
#define PRE_ADD_ITEMNAME_AUTOCOMPLETE				// 2hogi		2012-01-26		// 다국어 지원관련 아이템이름 자동완성작업
#endif		//PRE_ADD_MULTILANGUAGE

#define PRE_ADD_NODELETEGUILD						// dyss			2012-01-13		// 길드 자원해제 안되게 막음
#define PRE_ADD_CHANGEGUILDROLE						// dyss			2012-01-13		// #49264 길드부장 직급을 길드장과 동일하게 해주고 직급변경메뉴에서 삭제
#define PRE_MOD_GAMESERVERSHOP						// robust		2012-02-09		// #51536 게임서버에서 상점 기능 이용하기

#define PRE_FIX_SKILL_FAILED_BY_ACTION              // Rotha        2012-11-14      // #64223 패시브 액션 실패시 패킷보내도록 설정.
#define PRE_ADD_TRANSFORM_MONSTER_ACTOR             // Rotha        2012-12-07      // #75694 몬스터 변형
#define PRE_ADD_COSTUME_SKILL                        // Rotha        2012-01-13      // #76986 코스튬 스킬 - 클라
#define PRE_FIX_BREAKINTO_BLOW_SYNC                 // Rotha        2013-01-21      // #79746 난입시 상태효과 싱크및 토글스킬 싱크안맞는 부분 수정.
//#define PRE_FIX_FALL_PROP_COLLISION                 // Rotha        2013-01-22      // #79931 MAWalkMovement m_vVelocity.y == 0.f 인경우에 프랍아래로 떨어지는 부분 수정
#define PRE_ADD_ACTION_OBJECT_END_ACTION            // Rotha        2013-01-29      // #80226 ActionObject End액션추가
#define PRE_ADD_ENCHANTSKILL_BUBBLE_ACTION          // Rotha        2013-02-05      // #81670 ChangeActionStrByBubble 문의 및 개발요청
#define PRE_FIX_PROJECTILE_PREFIX_APPLY_POINT       // Rotha        2013-02-20      // #79334 접미사 발동 발사체 적용타이밍 변경
#define PRE_ADD_PLAYER_HITBACK_PROJECTILE           // Rotha        2013-03-04      // #83688 [어쌔신] 발사체 히트 시 부모 캐릭터에게 호밍 발사체를 생성하는 기능 
#define PRE_FIX_STANDCHANGE_SKILL_START_ACTION      // Rotha        2013-03-06      // #84010 스탠드 체인지스킬류의 시작액션이 상태효과에 의해서 제어되어 생기는 문제 수정.

// Stat / 보정 관련 디파인
#define PRE_FIX_STATUS_USE_BASE_REGULATION          // Rotha        2012-06-08      // #59472 상태창에 보정이 들어갈경우 기본이되는 스텟도 보정이 되도록 요청.
//#define PRE_ADD_CHANGE_ATK_FORMULA_BY_STATE       // Rotha        2012-08-30      // #67938 버프 스킬로 인하여 증가하는 물/마공 수치에 대한 전투공식 변경.
//#define PRE_ADD_BASE_STAT_REGULATION              // Rotha        2012-09-14      // #68152 기본 스텟 보정 추가.
#define PRE_ADD_PVP_STIFF_MAX                       // Rotha        2012-01-29      // #79428 경직보정 PVP 추가

// 상우님 디파인 관리를 위해서 모아둡니다.
#define PRE_FIX_55855								// semozz		2012-04-13		// #55855 네비메쉬/데칼이팩트 관련된 오류현상 이슈.
//#define PRE_ADD_65808								// semozz		2012-08-30		// #65808 [스킬문장]몬스터 소환을 이용하는 스킬도 스킬문장이 적용되도록 구현

#define PRE_MOD_SALE_COUPON_LEVEL_LIMIT				// kwcsc		2012-11-23		// 레벨제한이 걸린 할인 쿠폰 기능 개발
#define PRE_ADD_GRAVITY_PROPERTY					// kwcsc		2012-11-29		// #69547 Gravity 시그널 파라미터 추가
#define PRE_ADD_FADE_TRIGGER						// kwcsc		2012-12-18		// #76886 페이드 인, 페이드 아웃 캡션 출력 트리거 추가
#define PRE_ADD_WORLD_MSG_RED						// kwcsc, karl	2013-02-20		// #82873, #82875 전령 붉은새 시스템 개발

//////////////////////////////////////////////////////////////////////////////////
// 신규 캐릭터 관련
//////////////////////////////////////////////////////////////////////////////////
// 어쌔씬
#if defined(_KRAZ) || defined(_CH) || defined(_JP) || defined(_TW) || defined(_US) || defined(_ID) || defined(_TH) || defined(_SG) || defined(_RU) || defined(_EU)
//#define PRE_REMOVE_ASSASSIN							// verygoodd21 2013-01-02	// 신규 캐릭터 어쌔씬 해외 제거 디파인. 어쌔씬 배포 후 사용될 예정.
#endif

#if !defined(PRE_REMOVE_ASSASSIN)
#define PRE_ADD_ASSASSIN							// verygoodd21 2013-01-02	// 7번째 클래스. 어쌔씬.
#endif

// 어쌔씬 관련 스킬부분 디파인
#define PRE_ADD_HPBELOWDOSKILLBLOW					// kwcsc		2013-02-28		// #84056 [어쎄신스킬] 특정HP이하에서 정해진 스킬이 발동되는 상태효과 추가
#define PRE_ADD_PROJECTILE_RANDOM_WEAPON			// kwcsc		2013-03-04		// #84053 [어쎄신스킬] 발사체 아이디를 2개이상 입력 정해진 확율로 발사되는 방식


//#endif // #if defined( _WORK )
