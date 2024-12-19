﻿#pragma once
//2016 kat
//#define RLKT_ADD_MD5_AUTH

//2016.10.22 Add new guild UI
#define _ADD_RENEWED_GUILDUI

//2016.10.17 Add new MP calc formula
#define _ADD_NEWMPFORUMLA

//2016.10.07 Add new disjoint drop 
#define _ADD_NEWDISJOINT

//2016.10.05 Add new stage clear drop table
#define _ADD_NEWSTAGECLEARDROP

//2016.10.03 Add new mana cosumption
#define _ADD_NEW_MPCONSUME

//TODO(Cussrro): 没有对应资源
//2016.09.04 Add new login ui kor
//#define _ADD_NEWLOGINUI

#define _ADD_MAINBAR_NEW //new ui

#define _ADD_EFFECT_EMITTER_0xD
#define _ADD_ACT2
#define _ADD_ACT3
#define _ADD_ACT

// 2013. 03 -----------------------------------------------
#define PRE_FIX_STUCK_LANDING_ACTION                // Rotha        2013-03-05      // #82453 LandingAction 넘어가는 순간 액션간섭을 받아서 공중액션에 머물고 있는 현상 수정
#define PRE_FIX_NPCHIDE_ON_BREAKINTO				// kalliste		2013-03-05		// #82723 [메모리아]파티중 메모리아던전 NPC대화 퀘스트 강제종료 후 재접속시 NPC가 존재하지 않음.

// 2013. 02 -----------------------------------------------
#define PRE_MOD_PVP_ROUNDMODE_PENALTY				// devkang		2013-02-05		// #80756 PVP 라운드모드에서 승리한 팀의 승리 횟수를 기준으로 패널티 부여
#define PRE_ADD_CHECK_MOVESPEED_HACK                // Rotha        2013-02-26      // #81387 [본섭 Ver.155]PAK파일 통해 가속 관련 문제 < 관련 이동속도 핵관련 검출루틴 추가 >
#if defined(_FINAL)
#define PRE_FIX_CLIENT_FREEZING						// verygoodd21	2013-02-20		// #80025 드래곤네스트 응답없음 관련 문제 제보 (행크래쉬 시간 좁혀서 테스트)
#endif

#define PRE_DELETE_DUNGEONCLEAR						// saset		2013-02-21		// 던젼클리어데이터를 더이상 쓰지않아서 일단 삭제 (나중에 어비스 체크 살릴때 다시 쓰면 됨)
#define PRE_FIX_82147								// verygoodd21	2013-02-15		// #82147 【스킬】 팔라딘의 스킬 [에어리얼 블록] 의 재사용 대기시간이 이상
#define PRE_FIX_82502								// verygoodd21	2013-02-15		// #82502 Dungeon ranking exploit.
#define PRE_FIX_SQLCONNECTFAIL_LOG					// karl			2013-02-01		// SQL Server Connect 실패시, 상세 로그 남기도록 수정
#define PRE_FIX_LOGINSERVER_CONNECT					// karl			2013-02-14		// 캐릭터 선택창으로 돌아갈때 클라이언트가 로그인서버 1번에만 접속하는 오류 수정
#define PRE_FIX_REMOVE_STATE_EFFECT_PACKET          // Rotha        2013-02-13      // SC_CMDREMOVESTATEEFFECTFROMID 구조 개선
#define PRE_FIX_3DSOUND_DISTANCE					// kalliste		2013-02-12		// #82207 [용탈것] 제자리에서 space bar 눌렀을때 사운드 확인요청
#define PRE_FIX_SCROLL_PAGE_NUMBER					// kalliste		2013-02-12		// #82215 [길드레벨개방] 28페이지로 넘어가지지 않음
#if defined(_US)
#define PRE_MDD_USA_SECURITYNUMPAD					// elkain03		2013-02-07		// #81365 [DNNA] 2차 패스워드 설정 부분 변경 요청
#endif 
#define PRE_FIX_MASTERSYSTEM_CHECK_PUPILBLOCKDATE	// karl			2013-02-06		// #81718 스승을 삭제한지 1일이 지나지 않은유저에게 스승신청시 거부메시지가 제자쪽에만 출력됨
#define PRE_FIX_70011								// anno414		2013-02-06		// #70011 빙결을 통한 아이시 프렉션 발동시 경직모션 미발생 오류
#define PRE_ADD_LISTBOX_ELLIPSIS					// kwcsc		2013-02-13		// #82241 EU 거래소 관련 리스트박스에 말줄임 처리
#define PRE_ADD_GM_APPELLATION						// kwcsc		2013-02-14		// #82151 운영자 칭호

#define PRE_ADD_GM_APPELLATION						// kwcsc		2013-02-14		// #82151 운영자 칭호
#define PRE_ADD_GM_APPELLAThongse						// kwcsc		2013-02-14		// #82151 운영자 칭호
#define PRE_ADD_GM_APPELLAchengse						// kwcsc		2013-02-14		// #82151 운영자 칭호


// 2013. 01 -----------------------------------------------
#define PRE_FIX_61545								// 김밥,kalliste 2012-07-05		// #61545 【시스템】PT검색란에서,세 키워드 입력했을 때,해당 키워드에 해당되는 PT가 검색되지 않는다 
#define PRE_ADD_VEHICLE_SPECIAL_ACTION              // Rotha        2013-01-18      // #79817 [드래곤탈것]특정,지정맵에서 탈것의 특수액션을 제어하는 기능 개발요청드립니다.
//#define PRE_ADD_CHAGNESERVER_LOGGING				// 2hogi		2012-01-29		// 게임->빌리지, 빌리지->게임 이동간 문제발생관련 로그추가
#define PRE_FIX_CHANGESTAND_HACK                    // Rotha        2013-01-29      // #73212 스탠드 변경 상태효과를 이용한 핵체크 수정 ( 스탠드체인지중에 스킬을 사용하면 취소시키도록 합니다 )
#if defined(_WORK) || !defined(_FINAL_BUILD) || defined(_CH)
#define PRE_ADD_WORLD_EVENT							// haling		2012-10-22		// #71975 [DOLIS] 월드이벤트 개발
#endif
#if defined(_KRAZ) || defined(_JP) || defined(_WORK)
#define PRE_ADD_PACKETSIZE_CHECKER					// 2hogi		2012-12-20		// 이슈 없음 워크빌드 기준으로 패키량추적용
#endif		//#if defined(_KRAZ)
#define PRE_FIX_DUNGEONENTER_CLOSE_BY_QUEST			// kalliste		2013-01-21		// #79949 시공의 정원(Lv60) 「거짓된 본거지」에서 요일이 바뀌는 타이밍에 입장 화면에서 대기하면 Ready 상태로 된채로 포탈 이동 불가
#define PRE_FIX_PVPRANK_NOSERVICE_JOB				// kalliste		2013-01-16		// #79237 Ranking UI. Need to remove unnecessary tabs
#define PRE_FIX_APPELATION_TOOLTIP					// kalliste		2013-01-15		// #79182 컬렉션 북을 연 상태로 도움말 창을 열면, 도움말에 마우스오버시 컬렉션 북의 설명이 출력되는 현상
#if defined (_CH)
#define PRE_MOD_SDOLOGIN							// kalliste		2013-01-08		// #75041 [요청]샨다 신규 로그인 도우미 관련
#endif
#define PRE_FIX_APPELLATION_BITSET_LENGTHCHECK		// karl			2013-01-07		// 획득한 칭호인지 확인할 때, 버퍼 크기를 체크하도록 수정(버퍼 오버런 수정)
#define PRE_FIX_DEBUGSET_INIT						// stupidfox	2013-01-04		// 마스터,로그인,캐쉬 서버 InitApp 단에서 xml 파싱 실패할경우 런타임 에러로 죽는 상황
#define PRE_ADD_REDUCE_TABLE_RESOURCE				// kwcsc		2012-12-20		// 테이블 리소스 줄이기
#define PRE_ADD_LOGSERVER_HEARTBEAT					// stupidfox	2012-11-26		// 로그서버 LiveCheck 추가

// 2012. 12 -----------------------------------------------
#define PRE_ADD_ACTION_DYNAMIC_OPTIMIZE				// hayannal2009	2012-12-31		// 액션시그널 동적 최적화 기능 추가.
#if defined(_WORK)
#define PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP		// elkain03		2012-07-23		// #61982 커뮤니티UI를 통하여 파티에 가입 시 파티장에게 수락/거절 팝업 메시지 출력
#endif 
#define PRE_ADD_CHAOSCUBE							// robust, bintitle 2012-03-20	// #53978 믹스박스 시스템 - 서버 / #53980 클라
#define PRE_ADD_CHOICECUBE							// bintitle.dyss 2012-05-31		// #59015 초이스큐브

#define PRE_ADD_77490								// haling		2012-12-26		// #77490 월드맵에서 더블가차 사용시 아이템이 특정 순서로만 나오는 현상
#define PRE_ADD_CHECK_PART_MONSTER					// anno414		2012-12-07		// #75630 몬스터 초상화가 2개 이상 나오지 않는 현상 확인
#define PRE_FIX_DMIX_NOACTION						// kalliste		2012-12-18		// #76943 [코스튬] 코스튬 합성 후 고래 코스튬의 모션이 보이지 않는 현상 제보
#define PRE_MOD_SERVER_WAREHOUSE_NO_REMOTE			// kalliste		2012-12-17		// #76780 원격 창고 아이템_계정 창고 사용 불가능 하도록 변경
#define PRE_FIX_76603                               // Rotha        2012-12-17      // #76603 변신 상태효과 End처리 설정
#define PRE_FIX_PLAYER_CROSSHAIR_NOBATTLE			// kalliste		2012-12-17		// #76666【시스템】데저트 드래곤의 오아시스 에리어에서 다른 캐릭터와의 「커뮤니티」메뉴를 표시시킬수 없다
#define PRE_MOD_LIKEABILITY_QUEST_TEXT				// elkain03		2012-08-13		// #65623 호감도 개방 퀘스트, 퀘스트 타입 중복 표시 개선 
#define PRE_FIX_65287								// semozz		2012-08-09		// #65287 【시스템】최상급 스킬 강화 문장 「디스페어 니들」의 대미지 상승 효과가 정확히 적용되지 않음 
#define PRE_MOD_RELEASE_SOUND_SIGNAL				// hayannal2009	2012-12-14		// 한번 플레이되고 끝나는 사운드시그널의 경우 굳이 메모리에 계속 들고있을 이유가 없다. 로딩도 빠르니 플레이 후 삭제한다.
#define PRE_MOD_PRELOAD_MONSTER						// hayannal2009	2012-12-14		// 몬스터 로딩구조 변경. 프리로드 안쓰면서, 중간중간 flush 할 수 있는 구조 추가합니다.
#define PRE_MOD_75892								// 2hogi		2012-12-11		// #75892 잠재이전 기능 개선
#define PRE_FIX_PET_UNEQUIP							// kalliste		2012-12-11		// #75550 만티펫 산타모자 착용후 해제시 머리 스타일이 원상 복구 되지 않음
#define PRE_FIX_75807								// haling		2012-12-10		// #75807 우승길드가 없을시 이전 길드우승농장 초기화 및 다음 스케쥴없이 보상지급
#define PRE_FIX_REPUTE_DISCOUNT						// kalliste		2012-12-10		// #75858 [vip 맴버쉽 관련] 황금거위 길드 판매가격 15% 상승 미적용
#define PRE_ADD_58761								// dyss			2012-07-16		// #58761 8인 네스트관문마다 클리어 타임을 체크할수 있는 시스템
#define PRE_REMOVE_MINISITE							// kalliste		2012-12-07		// #75814 기본 화면에서 DNTV 아이콘 제거
#define PRE_FIX_TEXTDLG_TIMER						// kalliste		2012-12-07		// #75793 창모양 타입 메시지 창이 사라지지 않는 현상
#define PRE_ADD_FRAMEDELAY_LOG						// 2hogi		2012-12-04		// #74675 Ver148 GameDelayed문제<1122> : 관련 원인파악용 로그추가
#define PRE_FIX_MAKECHARMITEM						// kalliste		2012-12-04		// #74682 [본섭 Ver.148]트래져 패키지 구성물 현시 문제
#define PRE_FIX_75305								// karl			2012-12-03		// #75305 [상급 봉인의 인장] 상급 봉인의 인장 사용 시 불완전한 봉인의 인장이 사용 되는 버그
#if defined(_US) || defined(_SG) || defined(_TH) || defined(_RU) || defined(_ID) || defined (_EU)
#define PRE_REMOVE_TOTAL_LEVEL_SKILL				// kwcsc		2012-12-27		// #76418 레이어2,3 국가 칼리 업데이트 전까지 통합레벨스킬 사용하지 않음 (칼리 업데이트 시 제거할 것)
#endif
#define PRE_FIX_77172								// kwcsc		2012-12-28		// #77172, 77216 화상, 인크리즈 소울 상태효과 시 점프중 데미지 받아도 끝까지 날아가게 유지

// 2012. 11 -----------------------------------------------
#define PRE_FIX_SUMMON_ACTOR_HPBAR					// anno414		2012-11-28		// #73648 이벤트 무비가 재생되면「서먼 알프레도」의 HP바가 사라짐
#define PRE_MOD_INSPECT_DURABILITY					// hayannal2009	2012-11-28		// #74128 살펴보기에서 내구도 표시 삭제.
#define PRE_ADD_HEAD_SCALE_POTION					// hayannal2009	2012-11-27		// #74414 대두물약
#define PRE_FIX_CASHSHOP_CHARGE_WNDPROC				// kalliste		2012-11-26		// #74747 [액토즈]캐시샵에서 충전하기 버튼 클릭서 Client crash 발생.
#define PRE_MOD_SKILLRESETITEM						// kalliste		2012-10-24		// #72083 [Main] 액토즈 서비스시 가격/정책/기능 변경_망각의 계약(스킬 초기화) 무료화
#if !defined(PRE_FIX_FALL_PROP_COLLISION)
#define PRE_MOD_VELOCITYACCEL_SIGNAL				// hayannal2009	2012-11-22		// 컬리전 위에서 벨로시티 엑셀 시그널 왔을때 컬리전 뚫고 아래로 빠지는 현상 수정.
#endif // #if !defined(PRE_FIX_FALL_PROP_COLLISION)
#define PRE_FIX_74387								// haling		2012-11021		// #74387 미국 (ResellItem) DB에서 응답받고 처리하도록 변경.
#define PRE_ADD_SHOWOPTION_PVPMARK					// hayannal2009	2012-11-21		// #74010 콜로세움 계급마크 보이기 on/off 옵션 추가.
#define PRE_MOD_CASHFAIRY							// hayannal2009	2012-11-20		// #73215 정령 액티브하게 따라오도록 변경.
#define PRE_MOD_74203								// 2hogi		2012-11-19		// #74203 용자 아카데미 관련 문제 : 길드생성시 금칙어 검사추가
#define PRE_MOD_HEAD_EFFECT_HEIGHT					// hayannal2009	2012-11-19		// #74090 Head Effect 높이 수정.
#define PRE_FIX_73930								// karl			2012-11-15		// #73930 【시스템】「무한의 포르미드」에서 나온후 「질투의 알브트라움」에 입장하면 HP와 MP의 보정량이 반영됨 
#define PRE_FIX_SHOWVER								// kalliste		2012-11-14		// #73803 [EU] 치트키 /showver 사용시 클라 덤프 발생
#define PRE_DEL_ONNULL								// haling		2012-07-20		// CashServer OnNull 함수 제거..
#define PRE_FIX_LIVE_CONNECT						// haling		2012-09-10		// 미국 서버포트 Live 체크 대응	

#define PRE_FIX_69469                               // Rotha        2012-10-30      // #69469 일부 캔슬 액션을 사용하는 스킬 사용 후 타 스킬 사용 시 잘못된 액션이 출력되는 현상.
#define PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE        // Rotha        2012-11-05      // #72992 콜로세움 이용 중 간헐적으로 동기화가 틀어지는 현상. ( 기존 스킬사용하는 시점에 포지션 동기화를 하지않았기 때문에 추가합니다. )
#define PRE_MOD_LOAD_DARKLAIR_MONSTER				// hayannal2009	2012-11-06		// 다크레어 몬스터 로딩구조 변경. 프리로드 안쓰고, 라운드마다 필요없는 Act 삭제하는 구조로 갑니다.
#define PRE_MOD_LOAD_SOUND_SIGNAL					// hayannal2009	2012-11-06		// 사운드시그널 프리로드 제거. 필요할때 로드하는 구조로 바꿉니다.
#define PRE_FIX_73183								// haling		2012-11-06		// #73183【시스템】「콜로세움 용사의 주머니」로부터 「하급 봉인의 인장」이 드롭 되면 인벤토리 일반창에 한칸의 슬롯이 사용 불가
#define PRE_FIX_63822								// 2hogi		2012-11-06		// #63822【시스템】래그 발생시 중복 사용 불가한 캐시 아이템이 중복 사용 가능 
#define PRE_FIX_MAIL_HEADICON						// kalliste		2012-11-02		// #72848 길드에 가입되어있는 원격 우편함 사용중인 유저를 다른 유저가 보면, 길드명이 뒤로 밀림
#define PRE_FIX_73312								// stupidfox	2012-11-07		// #73312 스테이지 클리어창 2회 이상 출력되는 문제(다크레어만 적용)
#define PRE_FIX_INITSTATEANDSYNC					// stupidfox	2012-11-15		// 트리거 api 로 InitStateAndSync 가 호출되면 GameRoom 프로세스와 백그라운드로더와 충돌하는 상황이 발생할수 있음
#define PRE_FIX_DELAY_MASTER_CHECKVILLAGECHANNEL	// stupidfox	2012-11-26		// 마스터 서버 CheckVillageChannel() 호출 시점을 서버 시작 후 특정 시간 이후로 -> 서버시작 시점에 visibility 가 전부 false 인 빌리지 채널이 먼저 접속할때 채널 하나 오픈되는 상황
#define PRE_FIX_WEAPON_DUPLICATE_RENDER				// kwcsc		2012-11-27		// #74530 캐릭터 선택화면 코스튬 변경 후 생성 창에서 무기가 겹쳐지는 문제

// 2012. 10 -----------------------------------------------
#define PRE_FIX_LADDERSHOP_SKILL					// kalliste		2012-10-31		// #72288 통합스킬 「래더 상점 구매 할인」습득후 할인된 포인트만 소지해도 아이템 구입할 수 없다 
#if defined(_US) || defined(_SG) || defined(_EU)
#define PRE_ADD_ENGLISH_STRING						// verygoodd21	2012-10-29		// 영어권 국가들 스트링 처리 다른것 디파인
#endif

#define PRE_FIX_72521								// stupidfox	2012-10-29		// #72521 시간제한으로 막힌 게이트의 경우 다른 조건 무시하고 게이트 활성화 되는 문제
#define PRE_FIX_70618								// verygoodd21	2012-10-23		// #70618 마나 사용 스킬 막은 트리거 다음스테이지에서 강제로 끄는 이슈
#define PRE_FIX_71954								// haling		2012-10-23		// #71954 원더풀 레이싱 종료 후 스테이지 이동 선택 시 지급된 말 사라지지 않음
#define PRE_FIX_GUIDEDARROW_ON_PVPTOURNAMENT		// kalliste		2012-10-16		// #71207【스킬】토너먼트 모드에서 보우마스터스킬「가이디드 샷」이 관전석에 있는 캐릭터를 타겟로　인식해 버림
#define PRE_ADD_DOORS								// 2hogi		2012-06-20		// #60319 [드네APP1차]메시지-서버

#define PRE_FIX_RENDER_GATENAME_POS					// kalliste		2012-10-10		// #71081 세인트 헤이븐 남문 포탈 '헤르말테 포구' 텍스트 위치 어긋남
#define PRE_FIX_CASHSHOP_FIXEDPRICE					// kalliste		2012-10-10		// #71052 찬란한 별조각 반지 금액 표시 오류
#define PRE_FIX_PROP_SHOW_BY_TRIGGER				// kalliste		2012-10-08		// #70651 'Chest'속성의 프랍에 프랍을 감추거나 보여준다 트리거가 적용되지 않는 오류
#define PRE_ADD_COSRANDMIX_ACCESSORY				// kalliste		2012-09-26		// #68900 [Main] 악세서리 합성기 개발 / 2012-10-02 중국 선패치. 이후 타국가 적용 필요
#define PRE_ADD_COSRANDMIX_MIXERTYPE				// kalliste		2012-10-10		// #70513 [Main] 악세서리 합성기 개발
#define PRE_ADD_PVP_TOURNAMENT_ADDITIONAL			// kalliste		2012-10-02		// pvp 토너먼트 추가 작업.
#define PRE_FIX_QUEST_COUNT_WORLDZONE				// kalliste		2012-10-02		// #70155【퀘스트】서브 퀘스트 「악몽의 저 쪽」이 월드 존입장 확인 윈도우 에서 수행 가능한 퀘스트수가 불통
#define PRE_FIX_QUEST_GUILD_BOARD					// stupidfox	2012-10-10		// #71094 반복퀘스트 어뷰징 관련
#define PRE_ADD_TRANSFORM_POTION                    // Rotha,haling        2012-09-13      // #66053 변신 물약 개발
#define PRE_FIX_71455_REPUTE_RENEW					// kwcsc		2012-10-16		// #71455 호감도 연합멤버쉽 관련 적용기간 만료시 처리 추가
#define PRE_FIX_71333								// dyss			2012-10-23		// #71333 프로페서K 보스 이상 관련 루프액션 

// 2012. 09 -----------------------------------------------
#define PRE_FIX_REBIRTHCOIN_REMAIN					// kalliste		2012-09-25		// #70186 드래곤네스트 부활가능횟수가 이전네스트것으로 유지되어 보이는 현상
#define PRE_FIX_RECURSIVE_DICE						// kalliste		2012-09-27		// #70119 [본선 Ver.140]무한하게 주사위 굴림 관련 문제
#define PRE_REMOVE_CASHSHOP_GIFT_CADGE_MSGMAX		// kalliste		2012-09-24		// #69952 [조르기] 선물하기,조르기 메시지가 20자가 아닌 50자까지 보낼 수 있음
#define PRE_MOD_QUEST_NOTIFIER_FULLCHECK			// elkain03		2012-09-20		// #68785 퀘스트 알림이 가득찼을 때 퀘스트창 체크박스 비활성화 문제

#if defined(_KR) || defined(_KRAZ) || defined(_JP) || defined(_WORK)
#define	PRE_FIX_TODAYTOOLTIP_READ_TABLE				// anno414		2012-06-28		// #60978 투데이툴팁의 내용을 읽어오는 테이블 항목 변경.
#endif

#define PRE_FIX_69108								// stupidfox	2012-09-14		// #69108 다크레어 랭킹 남지 않는 부분처리
#define PRE_FIX_PVP_SPECTATOR_MOVE					// kalliste		2012-09-11		// PVP 관전 이동 문제 수정.
#define PRE_FIX_GUILDCREATE_DISABLEBTN				// kalliste		2012-09-06		// #68528 길드명을 1글자만 입력시, "창설"버튼이 비활성화 되어있지만 창설이 가능함
#define PRE_MOD_SET_ACHIEVED_MISSION_GAINFLAG		// stupidfox	2012-09-07		// DB 상에서 Achieve 가 되어 있는데 Gain 이 안된 항목 Gain 처리

// 2012. 08 -----------------------------------------------
#define PRE_FIX_66175								// semozz		2012-08-23		// #66175 Ex스킬의 UsableChecker를 베이스 스킬과 교체..
#define PRE_FIX_BOW_ACTION_MISMATCH					// kalliste		2012-08-14		// #66029【그 외】아처로 크로스보우 와 2012여름 한정 무기 코스튬을 장비할 때 캐시샵의 미리보기 화면에서 무기의 동작이 이상
#define PRE_FIX_63356								// semozz		2012-08-13		// #63356 【스킬】아크비숍 네스트 후지노스복부에, 머셔너리의 스킬 「서클 봄버」가 히트 하지 않는다

#if defined(_CH) || defined(_KR) || defined(_KRAZ) || defined(_WORK)
#define PRE_MOD_CHRAMBOX_ROTATE						// elkain03		2012-08-20		// #66350 보물상자 사용시 캐릭터 머리위에 상자 이미지가 반대로 나옴
#endif 

#define PRE_FIX_MOMSTER_PROCESS_DELTA               // Rotha        2012-05-16      // #56383 이슈 관련 몬스터 프레임율 줄이지 않도록 설정.

#define PRE_FIX_CHANGEJOBITEM_CHECK_NEEDJOB			// kalliste		2012-08-13		// #65337【UI】직업 변경권Tip에　칼리이름이 없음 
#define PRE_FIX_COSMIX_NORESPONSE_ON_ERROR			// kalliste		2012-08-12		// #66018 칼리로 「믹스박스」사용후 게임 조작 불가
#define PRE_FIX_65655								// haling		2012-08-10		// #65655 길드전 신청 기간 중 길드 탈퇴 가능(길드 가입시에 캐릭터 삭제불가)
#define PRE_FIX_PACKAGE_RESULT_CREATEITEM			// kalliste		2012-08-09		// #65696 레벨업보상상자 오픈시, 확인창에 보조무기의 타입이 나오지 않는 현상

#define PRE_FIX_LOAD_GACHA_DATA						// kalliste		2012-08-01		// #64478 [TDN]가챠폰NPC를 통해 코스튬 획득 불가.
#define PRE_ADD_FISHING_RESULT_SOUND				// anno414		2012-07-31		// #63752[Fishing] "Success" and "Fail" sound effects are missing when fishing ( 성공,실패 사운드 추가 )
#define PRE_FIX_GAMESERVER_USE_GHOST_MODE           // Rotha        2012-07-25      // #61945 이슈관련 게임서버와 클라이언트의 동기와 맞추는 설정 < SwapSingleSkin Gameserver 적용 >
#define PRE_FIX_FARMERRORMSG_USA					// kalliste		2012-07-23		// #63422 Game crashes when planting two plants at the right most farming section of the map.

// 2012. 07 -----------------------------------------------
#define PRE_FIX_63219								// semozz		2012-07-25		// #63219 【스킬】이동 키를 누르면서 「디바인 벤전스」를 발동하면 모션은 발생하지만, 대미지가 없음
#define PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP			// kalliste		2012-07-19		// #63377 마린룩 세트 망각의 계약 거래여부 - 모든 국가에서 ItemReversion 이 ITEMREVERSION_TRADE 이더라도 기간제 아이템은 거래불가 툴팁 표시
#define PRE_FIX_61438								// semozz		2012-07-18		// #61438 [칼리] 서먼퍼펫 8인네스트 소환위치 오류
#define PRE_FIX_NESTREBIRTH							// kalliste		2012-07-18		// #62432 [RB Ver.128]씨드래곤 네스트 하드코어 부활 횟수 현시 문제
#define PRE_FIX_62845								// anno414		2012-07-17		// #62845 트리거 루아 오류로 트리거 날아갔을 시 오류 메시지 출력
#define PRE_FIX_CREATE_ITEM_TOOLTIP_POS				// anno414		2012-07-13		// #62632 특정 해상도에서, 아이템 생산 윈도우에서, 툴팁을 PVP로 변경시 오른쪽으로 툴팁 이동.
#define PRE_FIX_52267								// semozz		2012-07-11		// #52267 【스킬】PVP에서, 알케미스트의 스킬 「인젝션」의 회복율이 Tip와 다르다
#define PRE_FIX_62309								// semozz		2012-07-11		// #62309 【스킬】던젼 또는 네스트에서, 폭발계 스킬을 발동했을 때에 대미지에 속성 표시가 없다
#define PRE_FIX_PARTYINDEX_DUMP						// kalliste		2012-07-10		// 파티 인덱스 관련 덤프 수정
#define PRE_FIX_62052								// semozz		2012-07-10		// #51052 【스킬】특정 순서로 궁극 스킬을 연속 사용 가능
#define PRE_FIX_61821								// semozz		2012-07-05		// #61821 【UI】패시브 스킬 발동 후,쿨 타임중에 스킬 레벨업하면,쿨 타임 아이콘이 사라진다 
#define PRE_FIX_SKILL_TOOLTIP_REFRESH				// anno414		2012-07-04		// #61556 스킬 Level을 높일때 툴팁 내용이 바로 갱신되지 않는 버그.
#define PRE_FIX_NONESTTOOLTIP_NODRAGONNEST			// kalliste		2012-07-03		// #61540【UI】타이푼킴 네스트에서, 부활 제한 회수의 설명문이 없다
#define PRE_FIX_CHARDELWAITDLG_TIME_DUMP			// kalliste		2012-07-02		// localtime 함수 이용 관련 덤프 수정.

// 2012. 06 -----------------------------------------------
#define PRE_FIX_GUILDWARSKILLTREE_BUFFEROVERRUN		// kalliste		2012-06-28		// 문자열 버퍼 오버런 관련 수정.
#define PRE_FIX_60525								// semozz		2012-06-28		// #60525 봉인된 장비 아이템 접미 강화 적용
#define PRE_FIX_PROPMON_PROJ						// kalliste		2012-06-15		// #59952 카타콤 집회장 던전내 뱀 석상에 피격시 클라이언트가 종료되는 문제
#define PRE_FIX_CANCELTRADE							// kalliste		2012-06-14		// #57369 특수 순서에서 캐시샵이 열리지 않게 된다
#define PRE_ADD_HSHIELD_LOG							// verygoodd21	2012-06-07		// #59359 핵쉴드 _AhnHS_MakeResponse 함수에서 덤프 나는것 개선작업
#define PRE_FIX_59680								// semozz		2012-06-13		// #59680 [칼리]서먼 퍼펫 시전시 힐 스킬 오류
#define PRE_FIX_59308								// semozz		2012-06-14		// #59308 [칼리]서먼 퍼펫의 크리티컬 표기 오류
#define PRE_FIX_59939								// semozz		2012-06-15		// #59939 [칼리]저주효과에 대한 오류

// 2012. 05 -----------------------------------------------
#define PRE_TESTLOG_58816							// kalliste		2012-05-30		// #58816 GameServer ROOM Crash문제<0526>(테스트로그)
#define PRE_FIX_SOURCEITEM_TOOLTIP					// kalliste		2012-05-23		// #57755【시스템】장비한 근원 아이템 Tips 표시 상태로, 효과 지속 시간이 0로 되면, 클라이언트가 강제 종료
#define PRE_FIX_57852								// semozz		2012-05-18		// #57852 【UI】(Wide) 이외 해상도에서, 우편, 선물 미니 아이콘 설명 내용　비표시

#define PRE_FIX_57706								// semozz		2012-05-14		// #57706 [스킬개편]타임 계열 스킬 중복 사용 시 지속 시간 오류 현상
#define PRE_ADD_CUTSCENE_PADSKIP					// kalliste		2012-05-12		// #57758 【시스템】게임 패드의 「시스템 메뉴」키로 이벤트 무비를 스킵 할 수 없다 
#if !defined (_FINAL_BUILD)
#define PRE_SOURCEITEM_TIMECHEAT					// kalliste		2012-05-11		// 근원 아이템 툴팁 시간 확인을 위한 치트
#endif
#define PRE_ADD_HITDIRECTION						// jhk8211		2012-05-10		// #56267 [데저트 드래곤] 바라보는 방향에 따라 Hit여부 체크 기능

#define PRE_FIX_55618								// semozz		2012-05-08		// #55618 알프레도 움직임 이상 현상(속도 동기화)

// 2012. 04 -----------------------------------------------
#define PRE_FIX_QUICKPVP_EXCLUSIVEREQ				// kalliste		2012-04-23		// #56593【시스템】특수 순서에서 결투 신청을 받으면 강제 종료 / #56606【시스템】「결투 신청」시스템을 이용해, 상대 PC를 강제 종료시킬 수 있다
#define PRE_FIX_GM_BREAKINTO_DGNCLEAR				// kalliste		2012-04-06		// #55464 Live Server Ver 59: GM Bug

// 2012. 03 -----------------------------------------------
#define PRE_MOD_KALI_WEAPON							// hayannal2009	2012-03-09		// 칼리 주무기 장착시에 왼손에도 메시 복사해서 보여주기.
#define PRE_MOD_ASSASSIN_WEAPON	                    //[OK]

#if defined (_TW) || defined (_CH) || defined (_ID) || defined (_RU) || defined (_EU) || defined (_TH)
#define PRE_ADD_48714								// 2hogi		2012-02-22		// #48714 [TDN]AbuseCounter_로그 보강 요청 <-대만 #53145 계정 제재 시 게임내 공지 요청 <-중국
#endif		//#if defined (_TW)
#if defined(_US) || defined(_EU) || defined(_TH) || defined(_ID) || defined(_RU)
#define PRE_FIX_52464								// verygoodd21	2012-03-06		// #52464 캐쉬슬롯 툴팁 안보이도록 하는 작업
#endif

// 2012. 02 -----------------------------------------------
#define PRE_FIX_52329								// semozz		2012-02-24		// #52329 [기간테스헬] 판넬 스킬을 걸린상태로 벽쪽에 붙으면 판넬과 미사일이 보이지 않음
#if !defined(_FINAL_BUILD) && ( defined(_KR) || defined(_KRAZ) || defined(_WORK) )
#define PRE_ADD_EXPORT_DPS_INFORMATION              // Rotha        2012-02-08      // 스킬 개선 관련 스킬 데미지 측정기
#endif
#define PRE_FIX_51999								// semozz		2012-02-21		// #51999 【시스템】특정 순서에서, 스킬 윈도우상 스킬 아이콘이 소실된다.
#define PRE_FIX_TRY_REBIRTH_BEFORE_SYNC				// kalliste		2012-02-21		// #52168 스테이지에서 맵이동시 특정 순서에 따라 부활불가
#define PRE_FIX_51727								// semozz		2012-02-20		// #51727 [RB Ver.108]스킬 배울 때 툴팁 이상 문제
#define PRE_FIX_51988								// semozz		2012-02-20		// #51988 RemoveStateIndex 기능 막음
#define PRE_FIX_50470								// semozz		2012-02-17		// #50470 【배틀】그린 드래곤 네스트에서, 접두어 「지혜」의 효과를 추가하면 대 대미지를 줄 수 있다 
#define PRE_FIX_NOMOVE_BROWSER						// kalliste		2011-08-03		// #39446 브라우저 움직임 제거
#define PRE_FIX_50007								// semozz		2012-01-17		// #50007 노예대장의 관문에서 댄스스킬을 맞은채로 죽으면 죽는 모션이 나오지 않는 현상
#define PRE_FIX_50482								// semozz		2012-02-06		// #50482 FreezingPrision상태효과 중복시 팀 반전 효과 오류 수정용

// 2012. 01 -----------------------------------------------
#define PRE_ADD_HIT_PROP_ADD_SE						// jhk8211		2012-01-02		// #49083 프랍에서 actor 처럼 hit 와 상태효과를 동시에 줄 수 있도록 처리. 기능 관련 이슈라 미리 소스에 포함.
#define PRE_FIX_50460								// semozz		2012-01-31		// #50460 【시스템】다크 레어 「증오의 페사데로」에서, 예상 보수 화면의 스테이지 아이콘 을 클릭하면, 「남은 시간」이 리셋트 된다
#if defined(_FINAL_BUILD) && defined(_KR) || defined(_KRAZ)
#define PRE_ADD_CL_DISCONNECT_LOG					// jhk8211		2012-01-16		// 클라이언트 로그를 버그 리포터를 통해 전송하도록 작업 및 disconnect 로그 남김.
#endif // #if defined(_FINAL_BUILD) && defined(_KR) || defined(_KRAZ)
#define PRE_FIX_49403								// bintitle		2012-01-19		// #49403【시스템】특수 순서에서, 돈을 소모했음에도 불구하고, 비행선을 탈 수 없다.
#define PRE_FIX_49208								// semozz		2012-01-18		// #49208 Elemental Lord invincible BUG(이미 OnBegin된 상태효과가 Duplicate되고, CanBegin에 실패시 삭제 오류 수정)
#define PRE_FIX_ONCANCEL_USEITEM					// kalliste		2012-01-17		// #48765【시스템】특수 순서에서, 신규 추가 코스튬 합성 시스템을 사용하면, PC가 조작불가
#define PRE_ADD_49660								// jhk8211		2012-01-16		// #49660 디파인 퍼니쉬먼트로 반사되는 상태효과는 빛속성 공격력을 반영하도록 수정.

#define PRE_FIX_48724								// semozz		2012-01-04		// #48724 스턴 상태에서 "/탈출"CMD_MSG_ESCAPE 사용시 스턴 상태가 해제된다.

// 2011. 12 -----------------------------------------------
#define PRE_FIX_NEXTSKILLINFO						// semozz		2011-12-26		// 스킬 레벨 데이타가 연속으로 있지않을 수 있어서 다음 레벨 정보? 설정 부분 수정
#define PRE_FIX_48494								// semozz		2011-12-22		// #48494 오라 타입의 지속시간을 무한대로 표시
#define PRE_ADD_GUILD_GESTURE						// semozz		2011-12-22		// 길드 제스처 추가
//#define PRE_FIX_48551								// jhk8211		2011-12-20		// #48551 핑퐁밤 예외처리 조금 더 가볍게.
#define PRE_FIX_HOLD_COSTUMEMIX_INVEN				// kalliste		2011-12-14		// #48009 디자인합성창 출력 시도 시 합성창이 자동으로 종료됨
#define PRE_FIX_COMBOBOX_ELLIPSIS					// kalliste		2011-12-08		// #47480 파티 목적 검색란에서, 생략 기호 부분에 마우스 커서를 맞추어도, 생략 내용이 비표시
#define PRE_ADD_COSMIX_SORT							// kalliste		2011-12-02		// #46494 코스튬 합성 윈도우에서, 17차 코스튬가 제일 앞에 표시됨 
#define PRE_FIX_45899								// semozz		2011-12-02		// #45899 【스킬】접두사 장신구를 장비하는 순간에, 접미사에 해당하는 스킬을 발동하면, 공격이 적에 해당되지 않는다 
#define PRE_FIX_LOADINGSCREEN						// jhk8211		2011-12-02		// 이슈번호 없음. 맵 로딩시 렌더링 하지 않도록 처리하기 위해 맵 로딩 스크린 한번만 렌더링 하도록 처리.

// 2011. 11 -----------------------------------------------
//#define PRE_ADD_VSYNC_OFF							// jhk8211		2011-11-28		// 이슈번호 없음. 클라 덤프 때문에 vsync 우선 무조건 켜놓음.
#define PRE_FIX_GAMEQUIT_TO_VILLAGE					// kalliste		2011-11-23		// #46534 월드존 및 던전에서 파티상태일 때 게임종료하면 마을로 돌아가는 문제
#define PRE_FIX_46730								// semozz		2011-11-28		// #46730 DropItem 밀림현상 - RandomSeed값으로 위치 보정 기능 수정(DropItem 시그널 bFixPosition추가)
#define PRE_FIX_BLOCK_CONDITION						// semozz		2011-11-24		// #25384 관련 HitSignal의 Damage비율이 0일때 블럭 성공 여부 수정(다른 편인 경우는 블럭 가능/다른 편인 경우는 블럭 안되도록..)
#if !defined (_KR) && !defined(_KRAZ) && !defined (_CH) && !defined (_EU) && !defined (_JP) && !defined (_US) && !defined (_SG)
#define PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST		// hgoori		2011-10-06		// 캐쉬 서버 중복 요청 차단. (한국, 일본, 미국, 싱가폴 제외 모든 국가 해당)
#endif // #if !defined (_KR) && !defined (_JP) && !defined (_US) && !defined (_SG)
#define PRE_FIX_46381								// semozz		2011-11-21		// #46381 엘리멘탈로드 - mp 부족으로 인하여 쉴드가 계속 유지됨
#define PRE_FIX_PARTY_STAGECLEAR_CHANGEMASTER		// kalliste		2011-11-10		// #44641【시스템】특정 순서조작로, 「F12 키」를 클릭할 때, 「M:337」의 메세지가 표시된다 (C/G)
#define PRE_ADD_ONESTOP_TRADECHECK					// kalliste		2011-11-10		// 거래 가능 체크 통합 인터페이스 추가
#define PRE_ADD_DMIX_DESIGNNAME						// kalliste		2012-02-01		// #50436 [02월 22일] 아바타 디자인/등급 합성 아이템 개선 제안_수정제안
#define PRE_FIX_HOMINGTARGET						// semozz		2011-11-09		// #44998 가이디드 샷 타겟 설정 문제로 타겟팅된 목표 표시를 위함.
#define PRE_FIX_44884								// semozz		2011-11-08		// #44884 【시스템】카오스 필드 바이라에서, 특정 순서로 「밀러도 가런- 오크」의 동작이 정지해, 공격이 히트 하지 않게 된다
#if defined (_KR) || defined(_KRAZ) || defined (_JP) || defined (_TW) || defined (_WORK)
#define PRE_MOD_RESTRICT_IDENTITY_IP				// 2hogi		2011-11-07		// #44912 오토 차단을 위한 IP 당 클라이언트 접속 수 제한 기능
#endif		//#ifdef _KR

// 2011. 10 -----------------------------------------------
#define PRE_FIX_44486								// semozz		2011-10-31		// #44486 기교의 아이템 장/탁찰시 쿨타임 리셋..
#define PRE_FIX_43986								// semozz		2011-10-26		// {캐시} 매력 아이템 기능 추가_룰렛 기능
#define PRE_FIX_PROP_RANDOM_VISIBLE					// jhk8211		2011-10-04		// #42473 재현이 되지 않아 각 클라이언트에서 SyncComplete 처리된 후 서버로 물어보는 루틴 추가. 기본적으로 모두 생성 후에 서버쪽에서 생성되지 않았다면 hide 시킴.

// 2011. 09 -----------------------------------------------
#if !defined (_WORK) && defined (_GAMESERVER)
#define PRE_ADD_THREADAFFINITY						// 2hogi		2011-09-27		// #42461 Thread Affinity Setting 관련 보완처리
#endif		//#if defined (_WORK) && defined (_SERVER)

#define PRE_FIX_MEMOPT_SIGNALH						// kalliste		2011-09-23		// 메모리 최적화

// 2011. 08 -----------------------------------------------
#define PRE_FIX_CLIENT_MEMOPTIMIZE					// siva			2011-08-03		// 클라이언트 메모리 최적화 작업
#define PRE_FIX_GAMESERVER_OPTIMIZE					// siva			2011-08-01		// 게임서버 최적화 관련.

// 2011. 07 -----------------------------------------------
#define PRE_FIX_GAMESERVER_PERFOMANCE				// jhk8211		2001-07-25		// 게임서버 퍼포먼스 개선
#define PRE_MOD_ANIMATIONUI_PLAY_CONTROL			// kalliste		2011-07-15		// #37737 애니메이션 UI 템플릿 속성 추가작업 (note:해당 UI툴로 제작한 animation 제작될 때 까지 업데이트 대기)
#if defined( _KR ) || defined( _US )
#define PRE_ADD_RELOGIN_PROCESS						// kwcsc		2011-10-14		// #43496 중복 로그인 실패 시 재로그인 시도 방식으로 변경
#endif // #if defined( _KR ) || defined( _US )
#define PRE_ADD_DONT_USE_FREETYPE					// jhk8211		2011-07-08		// #35682 freetype 사용 안함. 윈도우 폰트 출력 사용. 태국어에서 사용한다. 프리타입에서 사용하던 사이즈와 기준이 다르다.
#if defined(_US) || defined(_RU)
#define PRE_ADD_HEARTBEAT_TO_SQL					// 2hogi		2011-07-01		// #36759 로그인서버 Membership,WorldDB 간 Heartbeat SP 작업
#endif		//#ifdef _US

// 2011. 06 -----------------------------------------------
#define PRE_FIX_SKILLLIST							// jhk8211		2011-06-24		// 게임서버에서 덤프나는 경우가 있어 스킬 핸들 리스트 관리 방식 변경.

// 2011. 04 -----------------------------------------------
#define PRE_ADD_CHANGEJOB_CASHITEM					// jhk8211		2011-04-29		// 직업 변경 캐쉬 아이템 기능 추가.
#define PRE_ADD_MONSTER_CATCH						// jhk8211		2011-04-14		// #28532 몬스터가 플레이어를 잡는 기능 구현
#if defined( _US ) || defined( _SG )
#define PRE_USA_FATIGUE								// 김밥			2011-04-14		// #31469 북미 피로도 시스템
#if defined( PRE_USA_FATIGUE )
#define PRE_FATIGUE_DROPITEM_PENALTY							// 김밥			2012-03-27		// 피로도 DropItem 패널티
#endif // #if defined( PRE_USA_FATIGUE )
#endif // #if defined( _US ) || defined( _SG )

#define PRE_FIX_SP_REVISION_CHANGE_SP				// jhk8211		2012-06-13		// #57860 스킬 포인트 보정 루틴에서 사용하는 DB SP 변경.
#define PRE_ADD_SP_REVISION							// jhk8211		2011-04-04		// #29463 스킬 포인트 체크 루틴 DB 쪽 적용이 늦어져 디파인 추가.

// 2011. 02 -----------------------------------------------
//#define PRE_ADD_LOOP_PROJECTILE						// jhk8211		2011-02-15		// 간격이 짧은 루프액션에서 쏘는 발사체(핀포인트샷) 인증을 서버가 따라가지 못하는 경우 감안.

// 2011. 01 -----------------------------------------------
#if defined( _DBSERVER ) || defined (_LOGINSERVER) || defined(_CASHSERVER)
#define PRE_ADD_QUERYTIMELOG							// 김밥			2011-01-03		// DB 쿼리 시간 로그 남기기	'로그인서버도 로그의 필요성이 생겨 추가 2hogi'
#endif // #if defined( _DBSERVER ) || defined (_LOGINSERVER)
#define PRE_ADD_TRANSPARENCY_COSTUME				// hayannal2009	2011-01-19		// #27573 투명코스츔 추가.
#if (defined _CH || defined _TW || defined _SG) || defined _WORK
#define PRE_ADD_PETALTRADE							// verygoodd21	2011-01-06		// 거래소에서 골드 대신 페탈로도 거래 가능하게 하는 것
#if !defined(_CH)
#define PRE_FIX_34367								// haling		2012-01-11		// [건의]거래소_페탈 상품권<->페탈 구매 가능 설정관련(중국 제외한 국가의 페탈상품권 페탈등록 막기)
#endif //#if !defined(_CH)
#endif //#if (defined _CH || defined _TW) || defined _WORK
#if !defined (_JP)
#define PRE_MOD_ALLOW_INSPECT						// hayannal2009	2010-01-05		// #27007 살펴보기 차단기능 제거. 커뮤니티옵션에서 완전히 제거하는게 아니라 UI상에서 하이드 시키는 형태입니다.(다음에 되돌릴 수 있게..)
#endif

// 2010. 12 -----------------------------------------------
#if defined (_KR) || defined(_KRAZ) || defined (_CH)
#define PRE_FIX_26377								// Robust		2010-12-20		// #26377 [RB Ver.48] 일부 NPC와 대화 시 유저 가이드 팝업페이지 출력 전에 오래동안 클라이언트가 렉이 걸리는 현사
#endif  // #if defined (_KR) || defined (_CH)

// 2010. 11 -----------------------------------------------

// 요렇게 한 세트~
#define PRE_ADD_SECONDARY_SKILL						// 김밥			2010-09-06		// 보조 스킬 시스템
#if defined( PRE_ADD_SECONDARY_SKILL )
#define PRE_ADD_COOKING_SYSTEM						// kwcsc		2010-10-30		// #17486 요리 시스템 추가
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

// 탈것 한세트
// #define PRE_FIX_HIDE_VEHICLETAB                     // Rotha        2011-05-30      // 탈것 및 팻 인벤토리 Status 탭 숨기기. 

//#define PRE_FIX_HITSTIFF							// jhk8211		2010-12-09		// #25855 프레임변경/프레임정지 상태효과 발동중엔 역경직이 적용되지 않도록 처리. -> #31862 이슈에 의해 기획쪽에서 직접 컨트롤 하기 위해 뺌.
#ifdef _CH
#define PRE_ADD_IGNORESECONDAUTH_EKEYECARD			// 2hogi		2010-12-08		// ekey/ecard사용시 이차비밀번호 사용않함 PRE_ADD_23829와 분리
#define PRE_ADD_23829								// 2hogi		2010-12-03		// #23829 2차 비밀번호 관련 추가 개발 요청
#endif
#define PRE_MOD_PROJECTILE_HACK						// siva			2010-11-24		// #24912 오토 핵 관련 발사채 수정.

#if !defined(_WORK)
#define PRE_ADD_LOGIN_USERCOUNT						// haling		2010-11-26		// #25021 LoginServer 동접(인증 통과한 애들만 동접으로 포함하기..15분 이상 접속한 유저 짤라내기.)
#endif
#if defined(_CH) || defined(_TW)
#define PRE_ADD_ENCHANTSHIELD_CASHITEM				// haling		2010-11-02		// #23820 상위 강화보호 캐시아이템
#endif

#define PRE_ADD_NPC_REPUTATION_SYSTEM				// 김밥			2010-07-14		// NPC 평판 시스템
#define PRE_MOD_WARRIOR_RIGHTHAND					// hayannal2009	2010-07-19		// 워리어 건틀렛 장착시에 오른손 글러브 메시 렌더링 생략.
#if defined(_CH) || defined(_TW) || defined(_SG)
#define PRE_ADD_VIP									// kalliste		2010-06-08		// #15073 중국 관련 VIP 서비스 제작
#define PRE_FIX_VIP_MAXPTS							// kalliste		2010-08-02		// #19094 VIP 아이템을 구입하여 획득한 포인트 문제
#define PRE_ADD_VIP_CLEAR_BOX						// kalliste		2010-12-21		// #21982 [유료화] VIP 유저 일반 스테이지 보상상자 2개 오픈 기능 개발 검토 요청 (G/C)
#endif	// _CH
#ifdef _CH
//#define PRE_CHN_OBTCASHSHOP							// kalliste		2010-05-27		// #15146 중국 : 캐시샵 선물함 제외 비활성화
#endif // _CH 
//#define PRE_ADD_PETALSHOP							// kalliste		2010-05-06		// #14093 일본 : OBT용 페탈샵 작업
#if defined(_JP) || defined(_TW) || defined (_WORK)
#define PRE_ADD_GACHA_JAPAN							// jhk8211		2010-04-21		// 일본 버전용 가챠폰 한국엔 절대 포함되면 안됨.
#endif //#if defined(_JP) || defined(_TW)

#if defined( _KR ) || defined(_KRAZ) || defined( _JP )
#define PRE_ADD_JAPAN_AUTO_AIMING					// jhk8211		2010-04-01		// 일본 버전용 자동 타겟팅 관련 기능.
#endif
#define PRE_ADD_AIM_ASSIST_BUTTON					// hayannal2009	2010-03-30		// 에임 어시스트 버튼 기능
//#define PRE_REMOVE_DOORS_UITEMP						// kalliste		2012-09-24		// 27일 빌드에서 유저 사용 막기 위해 임시로 앱인증 버튼 삭제
//#define PRE_SWAP_QUICKSLOT							// hayannal2009	2010-03-24		// 컨트롤키로 퀵슬롯 페이지 전환
//#define PRE_REMOVE_CASHSHOP							// kalliste		2010-03-23		// #12308 캐시샵 UI 제거
//#define PRE_ADD_NO_HELP_DLG							// kalliste		2011-06-07		// 도움말 창 비활성화 #35210 [FEEDBACK] Help Menu (J) should be disabled for CBT
// #define PRE_ADD_NO_REPUTATION_DLG					// kalliste		2011-06-03		// 호감도 앨범 비활성화 #34721 The friendship menu should be removed for CBT

//#if defined(_FINAL_BUILD)
#define PRE_MEMORY_SHUFFLE							// siva			2010-03-11		// 메모리 셔플 관련 State 값 위주로 붙여놔봅니다.
//#endif	// #if defined(_FINAL_BUILD)
//#define PRE_FIX_LOADING_TASK						// mapping		2010-02-23		// 로딩태스크와 렌더프레임 간섭안하게 수정
#define PRE_ADD_CYCLEREPORT							// 2hogi		2010-02-20		// 주기적으로 servicemanger에서 gsm으로 state report

#if !defined( _FINAL_BUILD )
#define PRE_FILELOG									// 김밥			2010-02-19		// 서버 FileLog 남기고 남기기
#endif // #if defined( _FINAL_BUILD )
//#define PRE_ADD_CUTSCENE_ADD_SCALELOCK_INFO			// jhk8211		2010-02-19		// 컷신 툴에 Scale Lock 기능 추가. 클라이언트 컷신 태스크쪽에서 해당 정보 처리.
#define PRE_CRASH_CHECK_BACKUP						// mapping		2010-02-18		// 크래쉬 잡을 용도로 디버그 변수 등록용
#define	PRE_MONITOR_SUPER_NOTE						// kalliste		2010-02-15		// 돈복사 체크 코드
//#define PRE_PLAYER_CAMERA									// realgaia		2010-02-02		// 플레이어 카메라 보완

//#define PRE_TEST_PACKETMODULE						// 김밥			2010-01-12		// 패킷 모듈 테스트

//#define PRE_TRIGGER_LOG								// 김밥			2009-12-24		// 트리거 관련 로그 기능 추가 -> 게임룸 파괴될 때 한번 로그 남김
//#define PRE_QUESTSCRIPT_LOG							// 김밥			2009-12-16		// 퀘스트 스크립트 성능 로그
//#define PRE_TRIGGER_UNITTEST_LOG					// 김밥			2009-12-15		// 트리거 로그 남기기 -> 트리거 하나 실행될 때 마다 파일 로그 남김


#if (defined(_KR) || defined(_WORK)) && (defined(WIN64) || defined(_CLIENT))
#define _USE_VOICECHAT
#endif // #if defined(_KR) || defined(_WORK)

#include "DNDefine_local.h"
#include "DNDefine_Work.h"
#include "DNDefine_Patch.h"

#ifdef PRE_REMOVE_RUS_CBTUI_0905
#define PRE_FIX_PARTY_NO_RAID2						// kalliste		2011-05-02		// #33053 8인 공격대 파티를 생성할 수 있음
#endif

#ifdef PRE_REMOVE_EU_CBTUI_1206
#define PRE_FIX_PARTY_NO_RAID2						// kalliste		2011-05-02		// #33053 8인 공격대 파티를 생성할 수 있음
#else
#define PRE_ADD_INVEN_EXTENSION						// hayannal2009	2012-01-06		// #49177 인벤확장 유도
#endif // _EU

//#########################################################################################################################################
// 절대 빼면 안되는 Define
//#########################################################################################################################################

#if defined(_CLIENT)
#define _PACKET_COMP

#endif	// _CLIENT

#if defined( _LOGINSERVER )

const int _SERVER_TYPEID = 1;

#define PRE_FIX_SOCKETCONTEXT_DANGLINGPTR			// 김밥, 2hogi			2012-02-13		// #이슈번호없음 센드쓰레드와 워커쓰레드간 댕글링문제 발생하여 추가 수정

#define _SERVER
#define _USE_ACCEPTEX
#if !defined(_WORK)
#define _SERVERLOG
#endif	// _WORK

#elif defined(_DBSERVER)

#define PRE_FIX_SOCKETCONTEXT_DANGLINGPTR			// 김밥			2010-04-13		// #이슈번호없음 SocketContext DanglingPtr 수정
const int _SERVER_TYPEID = 2;

#define _SERVER
#if !defined(_WORK)
#define _SERVERLOG
#endif	// _WORK

#elif defined(_MASTERSERVER)

const int _SERVER_TYPEID = 3;

#define _SERVER
#if !defined(_WORK)
#define _SERVERLOG
#endif	// _WORK

#elif defined(_VILLAGESERVER)

#define _USE_SENDCONTEXTPOOL
#define PRE_FIX_SOCKETCONTEXT_DANGLINGPTR			// 김밥			2010-04-13		// #이슈번호없음 SocketContext DanglingPtr 수정

const int _SERVER_TYPEID = 4;

#define _SERVER
#define TIXML_USE_STL
#define _PACKET_COMP
#if !defined(_WORK)
#define _SERVERLOG
#endif	// _WORK

#elif defined(_GAMESERVER)
#define _USE_SENDCONTEXTPOOL
#define PRE_FIX_SOCKETCONTEXT_DANGLINGPTR			// 김밥			2010-04-13		// #이슈번호없음 SocketContext DanglingPtr 수정
#define PRE_ADD_SECURITY_UPDATEFLAG					// 김밥			2010-09-08		// 보안관련 모듈 UserState 에 따라 조절하게 추가

const int _SERVER_TYPEID = 5;

#define _SERVER
#define TIXML_USE_STL
#define _PACKET_COMP
#define _SKIP_BLOCK
#if !defined(_WORK)
#define _SERVERLOG
#endif	// _WORK

#elif defined(_LOGSERVER)

#define PRE_FIX_SOCKETCONTEXT_DANGLINGPTR			// 김밥			2010-04-13		// #이슈번호없음 SocketContext DanglingPtr 수정

const int _SERVER_TYPEID = 6;

#define _SERVER

#elif defined(_CASHSERVER)

const int _SERVER_TYPEID = 7;

#define _SERVER

#elif defined(_SERVICEMANAGER)

const int _SERVER_TYPEID = 8;
#define PRE_FIX_SOCKETCONTEXT_DANGLINGPTR			// 김밥			2010-04-13		// #이슈번호없음 SocketContext DanglingPtr 수정 2hogi가 킴

#define _SERVER

#else

const int _SERVER_TYPEID = 0;

#endif

#if defined _JP || defined _TW
#define PRE_JPN_CHARGE								// kalliste		2010-05-26		// #14673 과금 윈도우의 표시 방법에 대해
#endif // _JP

//#define PRE_MOD_CREATE_CHAR								// kwcsc		2010-10-18		// #18093 아래 define에서 캐릭터 생성 파츠명 변경만 분리
//#define PRE_MOD_NAVIGATION_PATH						// kwcsc		2010-08-31		// #19554 맵UI에서 자동이동 기능
//#define _TOOLCOMPILE

// 게임서버 성는개선 define
#define PRE_MOD_GAMESERVER_EXCEPT_MASTERMANAGER		// 2hogi		2011-12-21		// #48509 게임서버 MasterLock 제거
#if !defined (_SERVICEMANAGER) && !defined (_SERVICEMANAGER_EX)
#define PRE_DELETE_LOGLOCK							// 김밥			2011-12-21		// Log Lock 제거
#endif // #if !defined (_SERVICEMANAGER) && !defined (_SERVICEMANAGER_EX)
#define PRE_WORLDUSERSTATE_OPTIMIZE					// 김밥			2011-12-14		// WorldUserState 게임서버 최적화

// 만복도 펫 판매 유무에 따라 옵션창이 다름
#if defined(_CH) || defined(_WORK)
#define _USE_PET_FEEDING							// kwcsc		2012-06-29		// 만복도 펫 판매 유무
#endif // _CH, _WORK

#define PRE_MOD_MAGICALBREEZE_CHANGE_BLOW			// kwcsc		2012-09-05		// #67841 메지컬 브리즈 관련 버프류는 상태물->마 상태효과 변경부분 제거
#define PRE_MOD_FROSTBITE_EXCEPTION					// kwcsc		2012-09-19		// #68598 아이시프랙션 정상 동작 후 느려진 상태에 동상이 걸리는거 예외처리로 막음