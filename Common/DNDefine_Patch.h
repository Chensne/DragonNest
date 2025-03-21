
#pragma once
//22.05.2016 enabled
#define PRE_MOD_COSRANDMIX_NPC_CLOSE				// kalliste		2012-10-25		// #71923 [SNDA]합성완료시 조합창이 바로 바뀌지 않도록 개선요청
#define PRE_ADD_STAGE_CLEAR_ADD_REWARD				// verygoodd21, karl	2012-10-08		// #70808 스테이지 완료 추가보상
#define PRE_ADD_68838								// karl			2012-09-13		// #68838 트래져 스테이지 입장방식에 관한 신규기능개발 검토요청
#define PRE_ADD_BESTFRIEND							// robust		2012-04-10		// #55767 [서버]절친시스템 개발
#define PRE_PVP_GAMBLEROOM							// dyss, kwcsc	2013-01-17		// #74525, #77974 [Main]콜로세움 토너먼트 도박모드 작업


//kat 2016 #############
#define PRE_ADD_MACHINA
#define PRE_ADD_LENCEA

///############################################################################################################################
// 패치 완료 Define
//#########################################################################################################################################
#define KR_20110504									// 김밥			2011-04-19		// 5월04일 한국 패치
#define KR_20110519									// 김밥			2011-04-19		// 5월19일 한국 패치
#define KR_20110616									// 김밥			2011-05-03		// 6월16일 한국 패치
#define KR_20110630									// Robust		2011-06-16		// 6월30일 한국 패치
#define KR_20110714									// 김밥			2011-05-13		// 7월14일 한국 패치
#define KR_20110804									// Robust		2011-07-04		// 8월04일 한국 패치
#define KR_20110811									// kalliste		2011-08-11		// 8월11일 한국 패치
#define KR_20110825									// Robust		2011-07-04		// 8월25일 한국 패치
#define KR_20110908									// 김밥			2011-09-08		// 9월08일 한국 패치
#define KR_20111006									// 김밥			2011-09-01		// 10월6일 한국 패치
#define KR_20111103									// saset		2011-11-01		// 11월03일 한국 패치
#define KR_20111110									// 김밥			2011-09-19		// 11월10일 한국 패치
#define KR_20111117									// saset		2011-11-02		// 11월17일 한국 패치
#define KR_20111124									// 김밥			2011-10-10		// 11월24일 한국 패치
#define KR_20111208									// 김밥			2011-10-21		// 12월08일 한국 패치
#define KR_20111222									// 김밥			2011-11-24		// 12월22일 한국 패치
#define KR_20120112									// haling		2011-12-21		// 1월12일 한국 패치
#define KR_20120209									// saset		2012-01-13		// 2월09일 한국 패치
#define KR_20120223									// robust		2012-01-31		// 2월23일 한국 패치
#define KR_20120308									// robust		2012-01-31		// 3월08일 한국 패치
#define KR_20120322									// robust		2012-03-09		// 3월22일 한국 패치
#define KR_20120412									// robust		2012-03-22		// 4월12일 한국 패치
#define KR_20120426									// robust		2012-03-20		// 4월 26일 한국 패치
#define KR_20120510									// robust		2012-04-26		// 5월 10일 한국 패치
#define KR_20120524									// 김밥			2012-05-10		// 5월 24일 한국 패치
#define KR_20120607									// dyss			2012-05-16		// 6월 07일 한국 패치
#define KR_20120621									// kalliste		2012-06-08		// 6월 21일 한국 패치
#define KR_20120628									// saset		2012-06-18		// 6월 28일 한국 패치
#define KR_20120705									// saset		2012-06-14		// 7월 05일 한국 패치
#define KR_20120719									// saset		2012-07-03		// 7월 19일 한국 패치
#define KR_20120809									// dyss			2012-07-19		// 8월 09일 한국 패치
#define KR_20120823									// saset		2012-07-31		// 8월 23일 한국 패치
#define KR_20120913									// dyss			2012-08-23		// 9월 13일 한국 패치
#define KR_20120927									// stupidfox	2012-09-13		// 9월 27일 한국 패치
#define KR_20121011									// dyss			2012-09-27		// 10월 11일 한국 패치
#define KR_20121025									// dyss			2012-10-11		// 10월 25일 한국 패치
#define KR_20121108									// dyss			2012-10-25		// 11월 08일 한국 패치
#define KR_20121122									// dyss			2012-11-08		// 11월 22일 한국 패치
#define KR_20121205									// saset		2012-11-14		// 12월 05일 한국 패치
#define KR_20121220									// dyss			2012-12-05		// 12월 20일 한국 패치
#define KR_20130109									// dyss			2012-12-20		// 1월 9일 한국 패치
#define KR_20130123									// dyss			2013-01-09		// 1월 23일 한국 패치
#define KR_20130206									// dyss			2013-01-24		// 2월 6일 한국 패치
#define KR_20130220									// dyss			2013-02-05		// 2월 20일 한국 패치
#define KR_20130313									// dyss			2013-02-20		// 3월 13일 한국 패치
#define KR_20130327									// saset		2013-03-06		// 3월 27일 한국 패치


//#########################################################################################################################################
// 패치 Define
//#########################################################################################################################################

#if defined(KR_20130327)
#define PRE_LEVELUPREWARD_DIRECT					// saset		2013-02-28		// #83918 [Main] 레벨업 보상 개선_캐시 인벤토리에 보상 바로 들어가게 하는 작업
#endif	// #if defined(KR_20130327)

#if defined(KR_20130313)
#define PRE_ADD_ADDITIOANL_SKILL_ACTION             // Rotha        2013-02-19      // #79082 [스킬] PlayAniProcess에서 연계될수있는 액션을 관리할수 있도록 추가 
#define PRE_ADD_SKILLSLOT_EXTEND					// verygoodd21,karl	2013-01-30	// #80813,#80811 스킬 슬롯 확장
#define PRE_FIX_PARTSMONSTER_AI_TARGETTING          // Rotha        2013-02-28      // #77819 파츠몬스터의 AI및 대상처리에 대한 기준 변경 작업.
#define PRE_ADD_RETURN_VILLAGE_SHOWNAME				// elkain03, karl	2012-12-31	// #77189 스테이지 완료 시, 출력되는 팝업 창에 유저가 돌아가게 되는 마을 위치를 표시해주는 기능 개선
#define PRE_ADD_ITEM_GAINTABLE						// kalliste		2013-02-06		// #81396 [클라이언트] 아이템 획득처의 UI String 분리작업

//[debug_h]
//#define PRE_ADD_DWC									// anno414, devkang, stupidfox 2013-01-08 // #78018 DWC콜로세움 대회관련 작업


#define PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME				// karl			2013-02-18		// #82707 [DN]주간이벤트미션 종료일 추가 작업 요청
#define PRE_FIX_83091                               // Rotha        2013-02-20      // #83091 상태효과에 의한 이상상태일경우 스탠스변경 상태효과 지워주도록 처리
#define PRE_FIX_FRIEND_SORT							// kalliste		2013-02-22		// #83578 친구목록 정렬순서를 가나다순으로 변경요청
#define PRE_ADD_DECREASE_EFFECT						// kwcsc		2013-01-15		// #78795 몬스터에 저항이 있어 효과 감소 되는 경우 "효과감소" 띄우기

//[debug_h]  董珂鬧姦딜�犬풉응�
#define PRE_ADD_CASHSHOP_RENEWAL					//RLKT ENABLED // bintitle		2013-02-06      // #79054캐시샵 개선.


//#if defined(PRE_ADD_CASHSHOP_RENEWAL) && defined(_WORK)
//#define PRE_ADD_CASHSHOPBANNER_RENEWAL			//rlkt disabled.	// bintitle		2013-02-06      // #79339 캐시샵개선-마을 배너 롤링기능 개발.
//#endif // PRE_ADD_CASHSHOP_RENEWAL
#endif
#if defined(KR_20130220)
#define PRE_FIX_81750                               // Rotha        2013-02-06      // #81750 [스킬] 스탠스 계열 유지 중 스킬 피격으로 인한 캔슬 발생 시 오류 현상.
#define PRE_ADD_START_POPUP_QUEUE					// kalliste		2013-02-07		// #81879 이벤트팝업과 스탬프팝업의 순서 변경 요청
#define PRE_MOD_SKILLCOOLTIME						// kalliste		2013-01-24		// #81845 스킬쿨타임 초 색상 변경
#define PRE_ADD_GUILD_CONTRIBUTION					// elkain03, karl 2012-01-07	// #77634, #77632 길드 공헌도 시스템
#define PRE_ADD_ENABLE_EFFECT_SIGNAL				// elkain03		2012-07-18		// #62455 파티원 파티클 효과 ON / OFF 기능
#define PRE_ADD_GUILDREWARDITEM						// dyss			2012-11-13		// #73578 길드레벨 개방 및 보상 추가
#define PRE_DRAGONBUFF								// dyss			2013-01-08		// #75876 드래곤버프
#define PRE_ADD_MISSION_COUPON						// stupidfox	2012-10-29		// #68801 미션 보상으로 할인쿠폰 지급

#if !defined(_JP)
#define PRE_ADD_EQUIPLOCK							// kalliste, karl	2013-01-21	// #76027 장비 보안강화를 위한 장비 잠금 시스템 개발
#endif	// #if !defined(_JP)

#define PRE_ADD_CHARACTERCHECKSUM					// devkang		2013-01-25		// #80315 캐릭터 정보 DB 임의 수정 체크 
#define PRE_MOD_QUEST_COUNTING						// devkang		2013-01-23		// #80165 퀘스트 카운팅을 지정한 타겟 카운트 까지만 처리 하도록 수정	
#define PRE_MOD_PVP_ROUNDMODE_PENALTY				// devkang		2013-02-05		// #80756 PVP 라운드모드에서 승리한 팀의 승리 횟수를 기준으로 패널티 부여
#define PRE_ADD_DOORS_SELFKICK						// 2hogi		2013-02-07		// #78286 작업(Task) #76376: 드네앱 0.8 기능 개발하기 비정상 접속 종료
#define PRE_ADD_STAGE_USECOUNT_ITEM					// dyss			2013-02-12		// #82099 데저트 드래곤 전용 요리의 사용횟수 처리 요청(스테이지 아이템 사용횟수 제한)
#endif


#if defined(KR_20130206)
#define PRE_MOD_DUPLICATION_LOGIN_MES				// karl			2013-01-10		// #78491 동일 계정 접속 시 문구 확인 요청
#define PRE_ADD_LEVELUP_CASHGIFT_NOTIFY				// karl			2012-11-22		// #72716 [Main][전국가]레벨업 보상 상자(Box in Box)에서 상자 획득시 시스템 메세지 출력
#define PRE_ADD_SALE_COUPON_CATEGORY				// kwcsc		2013-01-22		// #80207 쿠폰아이템 클릭 시 정해진 서브카테고리까지 열리도록 함
#endif

#if defined(KR_20130123)
#define PRE_ADD_SKILLCOOLTIME						// kalliste		2012-12-18		// #76255 스킬쿨타임 초 표시 개발 요청
#define PRE_ADD_MONTHLY_MISSION						// elkain03, karl	2012-12-11	// #76067, #76066 월간 미션 시스템
#define PRE_ADD_ITEM_ENCHANT_TOOLTIP				// hayannal2009	2012-12-13		// #76093 아이템 보호젤리 관련 툴팁 추가.
#define PRE_ADD_LADDER_TAB_TOOLTIP                  // Rotha        2012-11-20      // #73279 래더 탭 마우스 오버시 입장조건 툴팁으로 알려주기
#endif

#if defined(KR_20130109)
#define PRE_MOD_PARTY_CREATE_SORT					// kalliste		2013-01-02		// #77508 [드래곤원정대]파티생성에 드래곤 원정대가 없는 현상.
#define PRE_ADD_PVP_EXPUP_ITEM						// hayannal2009	2012-12-03		// #75243 콜로세움 경험치 증가 아이템
#define PRE_ADD_DRAGON_FELLOWSHIP_GLYPH				// haling, verygoodd21	2012-12-03	// #74079 [Main]드래곤원정대 보상인 고유 문장을 착용할 수 있는 문장 슬롯 추가
#define PRE_ADD_CP_RANK								// haling, verygoodd21	2012-12-14		// #73387 [Main] CP 2차 개선
#define PRE_ADD_DRAGON_FELLOWSHIP					// kwcsc, karl	2012-11-20		// #74091 [드래곤 원정대] 드래곤 원정대 맵 서브타입(11) 추가 (#75660 서버)
#define PRE_RECEIVEGIFTALL							// saset, anno414	2012-11-14		// #63242 [Main] 선물함 한 번에 받기
#define PRE_MOD_PVP_ROOM_CREATE						// anno414		2012-11-13		// #69125 [Main] 콜로세움 방 개설 시 모드 중심으로 개설
#define PRE_FIX_76282								// devkang		2012-12-18		// #76282 PVP 중간에 포기하고 나갈때 처리 방법 변경
#define PRE_ADD_CHAR_STATUS_SECOND_RENEW            // Rotha        2012-12-06      // #60650 상태창 개선 2차
#define PRE_ADD_STATUS_BUFF_INFORMATION             // Rotha        2012-05-30      // #57461 상태창에 버프 스텟만 따로 표시하도록 요청
#define PRE_FIX_CHARSTATUS_REFRESH                  // Rotha        2012-06-13      // CharStatus 구조개선.
#define PRE_ADD_BASIC_STATE_LIMIT                   // Rotha        2012-08-22      // #66173 [밸런스, 전투밸런스] 스텟 버프에 의한 최소/최대치 제한.
#define PRE_ADD_ELEMENT_DEFENSE_LIMIT               // Rotha        2012-09-21      // #67654 속성 방어력이 100%에 도달할 경우 해당 속성의 공격은 데미지를 받지 않는 현상
#define PRE_ADD_SLOW_DEBUFF_LIMIT					// kwcsc		2012-11-15		// #67997 감속 관련 디버프류의 최저계수 제한
#define PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER			// anno414		2012-10-30		// #71731 [알테이아 대륙탐험]NPC 이미지가 있는 블라인 캡션 추가
#define PRE_ADD_RENEW_RANDOM_MAP                    // Rotha        2012-11-26      // #74693 [드래곤 원정대] 랜덤 맵 연결 테이블 세팅을 맵ID로 전환
#define PRE_FIX_74404								// dyss			2012-11-27		// #74404 상점구매제한 아이템 상점별로 처리로 수정
#define PRE_ADD_STAMPSYSTEM							// devkang		2012-11-16		// #73767 스탬프 시스템
#endif

#if defined(KR_20121220)
#define PRE_ADD_PROJECTILE_SE_INFO                  // Rotha        2012-11-19      // #72402 발사체가 발사되는 시점의 상태효과 정보를 저장하도록 구조개선.
#define PRE_MOD_GLOBAL_SKILLICON					// verygoodd21	2012-12-17		// #76703 "습득가능아이콘"의 표시 기준 변경
#define PRE_ADD_AI_NOAGGRO_STAND					// kalliste		2012-12-13		// #76297 [데저트 드래곤] 몬스터에게 어그로가 없을 때 몬스터가 배회하지 않도록 하는 기능
#define PRE_ADD_MISSION_HELPER_AUTO_REGISTER_LEVEL	// hayannal2009	2012-11-22		// #74479 미션도우미 자동등록레벨 제한.
#define PRE_ADD_JOBCHANGE_RESTRICT_JOB				// kalliste		2012-11-14		// #72833 [댄서] 직업 변경권을 통해 댄서로 전환 막기
#define PRE_ADD_ACTIVEMISSION						// bintitle.	2012.09.24		// #69363 액티브미션-클라이언트, #69362 서버 - stupidfox
#if defined(_WORK) || defined(_KR) || defined(_KRAZ) || defined(_CH) || defined(_TW) || defined(_JP)
#define PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL		// elkain03		2012-10-11		// #69922 제작창 UI 리뉴얼
#endif 
#if defined(_WORK) || defined(_KR) || defined(_KRAZ) || defined(_CH) || defined(_TW) || defined(_JP)
#define PRE_MOD_ALWAYS_SHOW_DROP_ITEM				// elkain03		2012-11-21		// #74000 [편의성]드랍 아이템명 항상 보이도록 수정 요청
#endif 
#define PRE_MOD_APPELLATIONBOOK_RENEWAL				// anno414, karl	2012-09-17		// #64745, #69504 [Main] 콜렉션 칭호 시스템 기능 개선 및 추가 콜렉션 칭호 제작 요청
#define PRE_MOD_ALWAY_SEE_SELL_PRICE_TOOLTIP		// anno414		2012-11-08		// #73192 아이템 툴 팁에 상점 판매가격 상시 표시 기능 개발
#define PRE_FIX_67546								// dyss			2012-09-07		// #67546 DB미들웨어 쓰레드 분산 작업

//rlkt 2016
//#if !defined(_TW)
#define PRE_WORLDCOMBINE_PVP						// dyss			2012-09-18		// #55446 월드 통합 전장 PVP
//#endif	// #if !defined(_JP)

#define PRE_ADD_PART_SWAP_RESTRICT_TRIGGER			// anno414		2012-12-05		// #75545 [트리거]장비교체 혀용 여부를 On/Off 시키는 트리거 개발
#endif

#if defined(KR_20121205)
#define PRE_MOD_DARKLAIR_RECONNECT					// elkain03		2012-10-22		// #71970 다크레어 재접속 유저에게 우측상단 라운드가 표시되지 않으며, 각 라운드가 파이널라운드로 표기됨
#define PRE_MOD_ITEM_COMPOUND_TOOLTIP				// elkain03		2012-11-31		// #75170 '접미사 소멸의 큐브' 툴팁에 (잠재력)으로 되어있음
#define PRE_ADD_MONSTER_PARTS_UI_TRIGGER			// elkain03		2012-10-04		// #70240 몬스터 파츠 HP를 화면 하단 게이지에 표시하는 트리거 요청
#define PRE_DETAILCLEARRANK							// saset		2012-11-13		// #73745 스테이지 클리어 보상 상자 랭크 세분화
#define PRE_ADD_FORCE_RIDE_ENABLE_TRIGGER			// verygoodd21	2012-08-17		// #66097 [데저트 드래곤]트리거-탈것 가능/불가능 전환 트리거 제작
#define PRE_MOD_LOCK_TARGET_LOOK					// verygoodd21	2012-08-14		// #65845 [데저트 드래곤] LockTargetLook 기능 추가 부탁드립니다.
#define PRE_ADD_CP_RENEWAL							// verygoodd21	2012-11-14		// #73287 CP 리뉴얼 개발
#define PRE_MOD_POTENTIAL_JEWEL_RENEWAL				// anno414, karl	2012-10-12		// #71351, #71757 잠재력 부여 기능 개선
#define PRE_ADD_PVP_COMBOEXERCISE					// devkang		2012-10-10		// #69059 콜로세움 콤보 연습모드 추가
#define PRE_FIX_73833                               // Rotha        2012-11-28      // #73833 [데저트드래곤] 아카데믹 포스아웃으로 보스몬스터들이 밀림
#define PRE_ADD_71342                               // Rotha        2012-10-22      // #71342 콜로세움 - 대장지키기 모드에서 대장 위치 표시_클라
#define PRE_ADD_PVP_SHOW_PARTY_GAUGE                // Rotha        2012-10-26      // #71579 콜로세움 팀전에서 같은 팀 체력 표시_클라
#define PRE_ADD_STAGE_LIMIT_INTERFACE               // Rotha        2012-11-02      // #72747 전투밸런스_공격력/데미지/힐링 최대치 제한_UI
#define PRE_ADD_STAGE_DAMAGE_LIMIT                  // Rotha        2012-09-10      // #67194 [전투밸런스, 최대데미지제한] 최대 데미지 제한에 대한 상세 기획서 작성.
#define PRE_ADD_MONSTER_NORMAL_ACTION_PRIORITY      // Rotha        2012-08-29      // #67720 A.I 기본액션에 우선순위 설정
#define PRE_FIX_INVINCIBLE_AT_BLOW_SEND_RESIST      // Rotha        2012-06-29      // #61324 (SKILL_DEFENSE 효과 추가 개발) 스킬 제한하는 효과 범위 확대및 Regist표시가능한 인자 설정.
#define PRE_ADD_TRIGGER_BY_HIT_CONDITION            // Rotha        2012-09-27      // #70206 [데저트 드래곤] Hit시그널이 특정 대상을 성공적으로 공격했을 때 TriggerEvent를 발생하는 기능
#define PRE_ADD_MAX_DAMAGE_LIMIT_BLOW               // Rotha        2012-10-17      // #71659 데져트 드래곤 최대 피해 제한 상태효과
#define PRE_FIX_67656                               // Rotha        2012-08-30      // #67656 상태효과에 의해서 데미지를 입힐경우 파츠데미지에도 데미지를 적용하도록 설정.
#define PRE_FIX_66687								// semozz		2012-08-20		// #66687 [데저트 드래곤]파츠 HP를 %로 변환시키는 스킬효과 개발
#define PRE_ADD_57090								// semozz		2012-04-30		// #57090 [데저트 드래곤]특정 대상이 공격다앟지 않게 하는 기능
#define PRE_ADD_IGNORE_RESTORE_HP					// kwcsc		2012-10-17		// #71663 [데저트 드래곤]힐 면역 관련 상태효과 추가
#define PRE_FIX_63315								// bintitle     2012-07.27      // #63315 [데저트 드래곤] CanHit 시그널이 False인 상태를 무시하는 현상.
#define PRE_ADD_MARK_PROJECTILE						// kwcsc		2012-08-16		// #65889 데저트 드래곤 발사체 낙하위치 표시 액션, 시그널 추가
#define PRE_ADD_DAMAGEDPARTS						// bintitle		2012-07-11		// #60073 [데저트 드래곤] 몬스터 파츠에 추가 기능 구현.
#define PRE_ADD_DESERTDRAGON						// robust, bintitle 2012-05-22	// #57365 [데저트 드래곤]현재 날씨(ENV)를 체크할 수 있는 트리거 추가
#define PRE_MOD_AIMULTITARGET						// robust		2012-02-15		// #51072 AI 멀티타겟처리 수정 (어그로->랜덤, 소환체 처리여부) : 대져트드래곤 패치때 같이 들어가야함!!
#define PRE_FIX_67719								// dyss			2012-08-27		// #67719 [데저트 드래곤]랜덤 타켓 스킬에도 적용
#define PRE_FIX_68096								// dyss			2012-08-30		// #68096 [데저트 드래곤]AI에서 globalcooltime을 동시에 여러개 사용
#define PRE_ADD_SERVER_WAREHOUSE					// haling,kalliste		2012-10-10		// #69258 [Main] 서버 창고

#if defined(_JP)
#define PRE_REMOVE_SERVER_WAREHOUSE					// kalliste		2012-11-07		// #73303 [Main] 서버 창고 제거
#endif	// #if defined(_JP)

#define PRE_ADD_PRIVATECHAT_CHANNEL					// dyss			2012-10-16		// ##70610 사설채널 개선
#endif	// #if defined(KR_20121205)

#if defined(KR_20121122)
#define PRE_ADD_MAILBOX_OPEN						// semozz		2012-07-12		// #58230 [Main]우편함UI 마을내에서 아무데서나 열 수 있도록
#define PRE_ADD_INTEGERATE_EVENTUI					// semozz		2012-07-12		// #58230 [Main]우편함UI 마을내에서 아무데서나 열 수 있도록 - 이벤트 창 통합
#define PRE_ADD_INTEGERATE_QUEST_REPUT				// semozz		2012-07-12		// #58230 [Main]우편함UI 마을내에서 아무데서나 열 수 있도록 - 퀘스트/호감도 창 통합
#define PRE_FIX_SYNC_ENCHANT_SKILL					// kalliste		2012-11-05		// #72640【스킬】콜로세움에서 인퀴지터 스킬 「마인드 브레이커 EX」의 감전 카운트 아이콘이 표시되지 않음
#define PRE_ADD_PVP_TOURNAMENT_WINNERXP				// kalliste		2012-10-17		// #71205【UI】토너먼트 모드 보상 화면에서 전 3위의 합계 Pts, 콜로세움 EXP 항목이 표시되지 않는다 (11/22 국내포함 예정)
#define PRE_ADD_COMPARETOOLTIP						// bintitle. 2012-09-18         // #69087 [Main] 악세서리 반지 아이템 능력 치 비교 시, UI가 화면을 벗어나는 문제 수정.
#endif

#if defined(KR_20121108)
#define PRE_ADD_EXSKILL_TOOLTIP						// kwcsc		2012-10-25		// #72315 Ex2차 스킬만 툴팁 변경 (구분방법 없어 SlotIndex로 하드코딩됨)
#define PRE_ADD_REPUTATION_EXPOSURE					// anno414, devkang	2012-09-11	// #48214 [Main] 호감도 콜렉션 강화
#define PRE_ADD_PVP_HELP_MESSAGE					// anno414		2012-09-06		// #66948 [Main] 콜로세움 안내 시스템 (도움말 및 추가기능)
#define PRE_MOD_OPTION_EFFECT_QUALITY				// anno414		2012-07-05		// #61805 이펙트 조절 옵션 따로 빼기
#define PRE_MOD_PVP_LADDER_XP						// stupidfox	2012-10-11		// #69122 콜로세움 래더 진행 시 콜로 경험치 제공
#define PRE_ADD_NEST_DIFFICULTY_TRANSFORM_FORCING	// elkain03		2012-09-27		// #69577 [네스트60Lv]우측 상단의 난이도 UI 스트링 변환 기능
#if !defined(_ID) && !defined(_RU) && !defined(_US)
#define PRE_ADD_2vs2_LADDER                         // Rotha        2012-10-09      // #70691 [클라]콜로세움 래더 2:2 모드 추가 작업
#endif // !defined(_ID) && !defined(_RU) && !defined(_US)
#define PRE_FIX_PIERCE_WITH_HIT_AREA                // Rotha        2012-10-31      // #72551 히트 에어리어로 발사체 히트처리하는 경우에 관통이 적용되지않는 부분 수정.
#define PRE_MOD_71820								// devkang		2012-11-06		// #71820 길드원 만렙 달성 시 길드장에게 차원의 열쇠가 지급되지 않음
#endif

#if defined(KR_20121025)

#define PRE_ADD_SMARTMOVE_PACKAGEBOX				// kalliste		2012-10-18		// #71852 [할로윈] 할로윈 특별 판도라 박스 오픈시, 확인창에 스마트무브되도록 수정요청
#define PRE_ADD_SHORTCUT_HELP_DIALOG				// anno414		2012-08-30		// #62824 게임 내 시스템UI 도움말 연결 시스템
#define PRE_MOD_SHOW_CHAR_INFO_IN_VILLAGE			// elkain03		2012-10-18		// #71777 마을에서 파티원 초상화를 이용하여 정보 확인이 불가능한 현상
#define	PRE_ADD_MISSION_NEST_TAB					// elkain03		2012-09-04		// #67924 미션 UI 내 네스트 탭 추가 	
#if !defined(_EU)
#define PRE_ADD_QUESTCOUNT_IN_STAGELIST				// elkain03		2012-09-20		// #68877 맵에 표시되는 스테이지 명 옆에 퀘스트량 추가 
#endif 
#define PRE_ADD_LOWLEVEL_QUEST_HIDE					// elkain03		2012-09-18		// #68061 NPC와 퀘스트 대화 시 일정 레벨 이상 차이나면 저레벨퀘 표시 	
#define PRE_ADD_PREDICTIVE_TEXT						// elkain03		2012-07-04		// #61563 파티 생성창 목표 스테이지 키 입력을 통한 문자열 자동완성  

#endif

#if defined(KR_20121011)

#define PRE_MOD_68531_NEW							// verygoodd21	2012-09-24		// #68531 기교, 코스튬 착용 후 스킬 레벨을 MAX만큼 올릴 수 없는 현상 (수정 요청으로 디파인 추가)
#define PRE_FIX_PETALGIFTCARD						// saset		2012-10-02		// #70497 페탈상품권 관련 소스 수정
#define PRE_ADD_REMOTE_QUEST						// stupidfox	2012-08-23		// #65255 원격 퀘스트 시스템
#if !defined(_JP) && !defined(_ID) && !defined(_RU)
#define PRE_ADD_JOINGUILD_SUPPORT					// 2hogi		2012-09-07		// #68260 작업(Task) #68259: 일반길드 가입지원 시스템
#define PRE_ADD_BEGINNERGUILD						// 2hogi		2012-06-11		// #59431 초보자 길드 시스템_DB, SERVER
#endif	// #if !defined(_JP)
#define PRE_FIX_68828								// haling, kalliste		2012-09-18		// #68828 트레저 박스에서 아이템 먹었을 때 시스템 메시지 출력 [클라]

#endif

#if defined(KR_20120927)

#define PRE_ADD_68286								// semozz		2012-09-12		// #68286 [Main] 메일/선물함 아이콘 클릭하면 해당 기능 실행시키는 작업
#define PRE_ADD_DARKLAIR_HEAL_REGULATION            // Rotha        2012-09-19      // #69663 도전형 다크레어 보정관련 / 힐보정 추가.
#define PRE_FIX_DISSOLVE_SKILL_FAILED               // Rotha        2012-09-21      // #53146 릴리브같은 해제종류의 상태효과들의 스킬 실패처리 변경.
#define PRE_FIX_69709								// haling		2012-09-20		// #69709 특정 강화 단계의 분해제한 기능.(#41409관련)
#define PRE_FIX_QUEST_ITEM_ONCOUNT					// haling		2012-09-19		// #69723 퀘스트 아이템 관련 체크시 갯수만큼 루프 도는거 수정.
#define PRE_ADD_CHALLENGE_DARKLAIR					// verygoodd21	2012-08-16		// #64675 도전형 다크레어 개발
#define PRE_ADD_EQUIPED_ITEM_ENCHANT				// stupidfox	2012-08-20		// #66349 장착 중인 장비 강화 

#define PRE_ADD_GETITEM_WITH_RANDOMVALUE 			// elkain03, 2hogi		2012-08-03		// #64713 랜덤 박스 랜덤 개수로 아이템 획득하는 기능 개발
#define PRE_FIX_BUFFITEM							// dyss			2012-08-21		// #63228 비약류 개선 - 영약개발 관련 네임드아이템 수정

#if !defined(_JP) && !defined(_TH) && !defined(_ID) && !defined(_RU)
#define PRE_ADD_CADGE_CASH							// kwcsc, saset	2012-06-11		// #57611, #57612 캐시 조르기 기능 추가.
#endif	// #if !defined(_JP)

#define PRE_SPECIALBOX								// saset, Rotha		2012-08-29		// #63017, #63016 특수 보관함
#define PRE_ADD_DIRECT_BUY_UPGRADEITEM				// anno414, saset		2012-08-14		// #65639 강화창에서 거래소이용하여 강화재료 원격구매
#define PRE_ADD_CASH_COSTUME_AURA					// anno414		2012-07-26		// 캐시 코스튬 오오라 효과 만들기
#endif

#if defined(KR_20120913)

#define PRE_ADD_64990								// semozz		2012-08-29		// #64990 [스킬]AI스크립트 PvE, PvP 구분
#define PRE_FIX_68898								// semozz		2012-09-11		// #68898 [EX스킬]인페르노 EX End액션이 두번 출력되는 문제(Input 시그널 추가 기능)
#define PRE_FIX_64312								// semozz		2012-07-31		// #64312 [EX스킬]소환수 EX스킬 관련 구현
#define PRE_FIX_68645								// semozz		2012-09-10		// #68645 [EX스킬] 세크레드 헤머링 EX 오류 현상.
#if !defined(PRE_MOD_68531_NEW)
#define PRE_MOD_68531								// verygoodd21	2012-09-05		// #68531 기교, 코스튬 착용 후 스킬 레벨을 MAX만큼 올릴 수 없는 현상
#endif	// #if !defined(PRE_MOD_68531_NEW)
#define PRE_ADD_SKILL_RESET_COMMENT					// verygoodd21	2012-07-31		// #63994 저레벨 스킬초기화 무료 지급 안내 - 클라
#define PRE_ADD_ONLY_SKILLBOOK						// verygoodd21	2012-08-27		// #67454 스킬북으로만 스킬을 배우도록 하는 시스템 개발
#define PRE_ADD_PRESET_SKILLTREE					// verygoodd21	2012-08-25		// #66961 스킬창 프리셋 제공
#define PRE_ADD_PVP_RANKING							// stupidfox	2012-08-02		// #62216 랭킹 시스템 - PVP 랭킹
#define PRE_MOD_MISSION_HELPER						// stupidfox	2012-07-27		// #63227 미션 도우미 개선 
#define PRE_FIX_62281								// stupidfox	2012-07-12		// 파티 리뉴얼. 파티 초대 실패 상황 변경
#define PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP           // Rotha        2012-06-28      // #60990 퀘스트 저널에서 가르키고있는 맵에따라서 정렬되도록 설정합니다.
#define PRE_ADD_QUEST_BACK_DIALOG_BUTTON			// anno414		2012-07-09		// #62032 NPC 대화화면의 돌아가기 기능 추가
#define PRE_ADD_TOTAL_LEVEL_SKILL					// semozz		2012-08-01		// #64098 [클라]통합레벨 시스템 작업
#define PRE_PRIVATECHAT_CHANNEL						// dyss, kwcsc	2012-07-16		// #59031, #62366  사설채팅 채널 추가 작업
#define PRE_ADD_REPAIR_NPC                          // Rotha, haling        2012-07-16      // #62618, #62617 월드존수리NPC배치
#define PRE_ADD_NONE_EFFECT_APPELLATION_NOTIFY		// anno414		2012-08-17		// #66997 커버칭호 '능력치 없음' 텍스트 추가-클라이언트
#define PRE_FIX_PROTRAIT_FOR_GHOST                  // Rotha        2012-01-11      // #48625 유령일때 초상화가 갱신되지않는 부분 개선.
#define PRE_ADD_PVP_TOURNAMENT						// haling,kalliste		2012-08-06		// #64890, #64891 콜로세움 토너먼트 모드 개발

#endif

#if defined(KR_20120823)

#define PRE_MOD_ITEMUPGRADE_COOLTIME				// kalliste		2012-07-13		// #62593 강화 시 대기시간 줄여주기 - 클라
#define PRE_ADD_ITEMCAT_TOOLTIP_INFO				// kalliste		2012-07-30		// #63358 [Main] 강화/잠재/접미 부분 툴팁 더 명확히 안내해 주기
#define PRE_ADD_INSTANT_CASH_BUY					// kwcsc, saset	2012-05-14		// #56200, #56201 캐시템 간단 구매 기능 추가. #63311 [Main] 할인쿠폰 사용 인식개선, #64475 [Main] 펫을 클릭하면 바로 뿔피리 사용할 수 있게 하는 기능 추가
#define PRE_ADD_PET_EXTEND_PERIOD					// kwcsc		2012-08-08		// #64485 펫 뿔피리 사용 관련 기능 추가
#define PRE_ADD_DIRECTNBUFF							//  dyss,Rotha	2012-08-06		// #64968 귀환자 보상 아이템 버프 작업
#define PRE_ADD_SKILL_LEVELUP_RESERVATION           // haling,verygoodd21         2012-07-31      // #63234 스킬 창 개선 1차 - 스킬 설정 후 적용 프로세스
#define PRE_FIX_63975                               // Rotha        2012-07-30      // #63975 PC방에서 수령 완료시 빌보드 삭제(미니맵 캐릭터)
#define PRE_ADD_VIEW_OPITION_PARTY_HEAL             // Rotha        2012-07-13      // #51827 파티원의 힐정보를 표시하도록 하는 옵션 추가.

#endif	// #if defined(KR_20120823)

#if defined(KR_20120809)

#define PRE_ADD_PARTSITEM_TOOLTIP_INFO				// kalliste		2012-07-30		// #63423 [Main] 장비 아이템에 접미사/강화/잠재력 관련 정보 넣기
#define PRE_FIX_COLOCHAT_ICON						// kalliste		2012-07-31		// #55511 채팅창탭 아이콘화 및 귓속말탭 추가_UI / #63969 콜로세움 게임에 진입하면 채팅탭이름에 글자가 추가되는 현상
#define PRE_ADD_63603								// semozz		2012-07-24		// #63603 [Main]아이템별 인터렉션 정보 보여주기 - 클라이언트 작업
#define PRE_FIX_63375								// semozz		2012-07-20		// #63375 [트레져스테이지] 스테이지 플레이후 스테이지입구 복귀시 HP/MP가 보정전으로 회복되지 않는 현상
#define PRE_MOD_RC_MODE_OBSERVER_BE_NOT_OPEN_PETDLG // elkian03     2012-07-13		// #62470 콜로세움 관전자 입장시 펫 다이얼로그 강제로 열리던 현상 수정  
#define PRE_MOD_PARTY_TOGGLE_OPT					// elkain03		2012-07-12	    // #62319 파티 생성시 "기타" 토글버튼 오류 수정 
#if defined(_KR) || defined(_KRAZ) || defined(_JP) || defined(_CH) || defined(_TW) || defined(_SG) || defined(_WORK)
#define PER_ADD_PROP_TRAP_DIFFICULT_SET				// anno414		2012-06-29		// #61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#endif
#define PRE_ADD_SUBTYPE_TREASURESTAGE				// verygoodd21	2012-07-13		// #62775 맵 서브타입 요청
#if defined(PRE_ADD_SUBTYPE_TREASURESTAGE)
#define PRE_ADD_TSCLEARCOUNTEX						// saset		2012-07-24		// #63549 트레져 스테이지 입장 횟수 확장개발
#endif	// #if defined(PRE_ADD_SUBTYPE_TREASURESTAGE)
#define PRE_FIX_ESCAPE                              // Rotha        2012-02-27      // #52349 탈출기능 수정요청
#define PRE_FIX_ELEMENT_REGULATION_RENEW            // Rotha        2012-07-12      // #60463 [스킬, 속성] 속성 공격력 /방어도 수식 기존 돌아가는 구조에 맞게 재설정 작업.
#define PRE_SHARE_MAP_CLEARCOUNT					// stupidfox	2012-07-02		// #61150 네스트 맵 클리어 카운트 공유
#define PRE_ADD_62072								// stupidfox	2012-07-10		// #62072 스테이지 클리어 카운트 정보 트리거 api 추가
#define PRE_ADD_REVENGE								// stupidfox	2012-07-17		// #62999 콜로세움 리벤지 
#define PRE_ADD_PVPLEVEL_MISSION					// stupidfox	2012-08-01		// #64706 PVP 등급 달성 보상 미션 추가
#define PRE_ADD_EXPUP_ITEM							// haling		2012-06-20		// #60218 소모성 경험치 추가 아이템 개발 [서버](절대값으로 경험치 증가)
#define PRE_ADD_RACING_MODE							// haling, verygoodd21 2012-05-08	// #55504 [Main] 콜로세움 원더풀 레이싱 모드 제작
#define PRE_ADD_DUNGEONCLEARINFO					// dyss			2012-01-03		// #48575 던전클리어인포 유저에서 게임룸으로 이동
#define PRE_FIX_49129								// dyss			2012-01-03		// #49129 api_trigger_GetPartyActor 기능 확장
#define PRE_EXPANDGATE								// dyss			2012-06-18		// #60097 셋팅 게이트 갯수 추가 요청(10개->15개)
#define PRE_NORMALSTAGE_REGULATION					// dyss			2012-06-18		// #60092 일반 스테이지 보정 적용 기능 요청
#define PRE_ADD_LIMITED_SHOP						// dyss			2012-07-17		// #61850 갯수제한 아이템 상점 개발(일일, 주간 리셋)
#define PRE_WORLDCOMBINE_PARTY						// 김밥			2012-04-26		// #56555 월드 통합 파티 룸 개발
#define PRE_FIX_WORLDCOMBINEPARTY					// dyss			2012-09-12		// 월드통합파티룸 하나의 게임서버로 고정시킴
#define PRE_FIX_63305								// semozz		2012-07-20		// #63305 강해져야 할 이유 퀘스트 진행후 스테이지입구로 이동하면 HP/MP가 차지 않는 현상
#define PRE_FIX_40328                               // Rotha        2012-02-27      // #40328 NPC 불필요한 선택지 숨기기
#define PRE_ADD_CHAT_RENEWAL						// bintitle.	2012-04-12		// #55596 채팅창탭 아이콘화 및 귓속말탭추가.
#define PRE_ADD_COMBINEDSHOP_PERIOD					// stupidfox	2012-07-05		// #60272 인게임 상점에서 기간제 캐시 아이템 판매, #63956 [Main]인게임 아이템 기간제 판매

#define PRE_MOD_PVPRANK								// 2hogi		2012-07-23		// #62825 콜로세움 계급 마이그레이션 및 개선 : 서버
#define PRE_MOD_PVPOBSERVER							// 2hogi		2012-07-17		// #63098 콜로세움 관전자 수 확장 및 개선_서버
#if defined(PRE_MOD_PVPOBSERVER)
#define PRE_ADD_IMPROVE_MASTER_OBSERVER             // Rotha        2012-07-23      // #63217 콜로세움 방송(중계)모드_클라이언트
#endif

#define PRE_ADD_NOTIFY_ITEM_COMPOUND                // Rotha        2012-07-26      // #63414 접미사 제작 알리미

#endif	// KR_20120809

#if defined(KR_20120719)
#define PRE_FIX_61382								// semozz		2012-07-02		// #61382 [칼리]꼭두각시 효과 수정 진행
#define PRE_ADD_58449                               // Rotha        2012-05-21      // #58449 PC방 이벤트에 의한 NPC 알림 타입 추가.
#define PRE_FIX_PASSIVE_BUFF                        // Rotha        2012-06-14      // #59927 패시브 버프일경우 버프 타입으로 체크하도록 수정
#define PRE_ADD_DOORS_PROJECT						// haling		2012-03-23		// #54291 Doors 캐릭터 능력치 저장

#endif	// KR_20120719

#if defined(KR_20120705)

#define PRE_ADD_EASYGAMECASH						// dyss			2012-05-25		// #56185 캐시로 게임 쉽게 만들어주기(오픈시 연결된 모든 아이템을 열어주는 상자 개발)
#define PRE_ADD_COSRANDMIX							// kalliste		2011-12-09		// #47470 국내 코스튬 합성 작업 요청
#define PRE_ADD_56253								// semozz		2012-04-19		// #56253 [칼리]지속타입 "타임토글"의 효과 수정
#define PRE_ADD_ANIOBJ_SHADOWOFF					// kalliste		2012-04-12		// #55520 [칼리]ActionObject로 출력되는 매쉬의 그림자 지우기
#define PRE_FIX_55461								// semozz		2012-04-05		// #55461 [칼리]점프킥 hit시 다른 액션이행하는 부분 문제 수정요청
#define PRE_FIX_55383								// semozz		2012-04-04		// #55383 [타이푼킴네스트] Aura스킬의 적용타입(enemy)과 StateEffect가 정상적으로 적용되지 않는 문제.
#define PRE_FIX_55378								// semozz		2012-04-04		// #55378 [타이푼킴]몬스터가 Charger스킬을 사용할시 정상적으로, 발사체가 나오지 않는현상.
#define PRE_ADD_55295								// semozz		2012-04-03		// #55295 [칼리]발사체 Hit액션의 벡터값 수정하기
//#define PRE_ADD_50903								// semozz		2012-02-03		// #50903 [타이푼킴네스트] 추가스킬효과개발 ID:236 "데미지분산"
#define PRE_ADD_50917								// semozz		2012-01-31		// #50917 [타이푼킴네스트] 추가스킬효과개발 ID:238 "몬스터무기교체"
#define PRE_ADD_50907								// semozz		2012-02-02		// #50907 [타이푼킴네스트] 추가스킬효과개발 ID:237 "무장해제"
#define PRE_ADD_50923								// semozz		2012-01-31		// #50923 [김박사네스트]추가 스킬 효과 개발


#define PRE_ADD_BUFF_ADD_INFORMATION                // Rotha        2012-03-09      // #51835 버프 개선작업 / 버프에 툴팁 띄우는 작업.
#define PRE_ADD_NAMEDITEM_SYSTEM					// dyss			2012-02-13		// #51841 네임드아이템 작업
#define PRE_ADD_GUILD_EASYSYSTEM					// dyss			2012-05-16		// #57338 길드 편의성 보강
#define PRE_FIX_59347								// semozz		2012-06-07		// #59347 [칼리]"꼭두각시"스킬효과 문제 수정요청
#define PRE_FIX_59238								// semozz		2012-06-04		// #59238 칼리 서먼퍼펫소환시 소울게이트 중복히트 오류
#define PRE_FIX_CLIENT_SKILL_MAX                    // Rotha        2012-06-20      // #60103 스킬 MaxLevel 에 대한 수치가 클라이언트에서는 상이하게 동작해서 표시부분에서만 MaxLevel을 테이블수치를 사용해서 적용하도록 설정.
#define PRE_MOD_SELL_SEALEDITEM						// 2hogi		2012-04-30		// #51583 [Main] 봉인된 아이템을 개봉하지 않은 채 상점에 팔기

#define PRE_MOD_60583								// stupidfox	2012-06-27		// #60583 아이템으로 물품 구매시 봉인 해제되어 있는 아이템 먼저 소모하기
#define PRE_FIX_CLASS_EXP							// haling		2012-06-20		// #36358 캐릭터별 차등 보너스 경험치 설정 시스템(로우로 떨어뜨리게 변경합니다.)

#if defined(PRE_ADD_GUILD_EASYSYSTEM)
#if defined(_JP)
#define PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE		// hayannal2009	2012-08-06		// #64972 길드 홈페이지 기능 제거.
#endif
#endif
#endif	// #if defined(KR_20120705)

#if defined (KR_20120628)

#define PRE_ADD_SELECTIVE_SHUTDOWN					// saset, bintitle 2012-06-13   // #59492 한국 선택적 셧다운제.

#endif	// #if defined (KR_20120628)

#if defined (KR_20120621)

#define PRE_FIX_CHARRENAME_MSG						// kalliste		2012-06-07		// #59426 [한국] 캐릭터명 1글자 짜리로 변경시 UI 스크립트 오류
#define PRE_ADD_PARTYTARGETMAPTOGGLE				// kalliste		2012-06-07		// #58336 [파티리뉴얼] 스테이지/네스트/모두 외에 기타 등의 추가분류 필요

#endif // #if defined (KR_20120621)

#if defined (KR_20120607)

#define PRE_FIX_59336								// semozz		2012-06-05		// #59336 엘리멘탈로드 프리징스파이크 히트수에 따른 데미지 감소 오류
#define PRE_FIX_ELEMENT_REGULATION                  // Rotha        2012-06-02      // #58966 [보정] 래더, 길드전, 다크레어 속성 장비 보정 적용. 

#if !defined(_RU)
#define PRE_ADD_COLOSSEUM_BEGINNER					// jhk8211, 2hogi		2012-04-25		// #55974 콜로세움 초보자 채널 제작.
#endif
#define PRE_FIX_58505								// semozz		2012-05-23		// #58505 [스킬, 접미사] 접미사 스킬 리밸런싱

#if !defined(_JP) && !defined(_RU) && !defined(_ID) && !defined(_US) && !defined(_EU)
#define PRE_ADD_EXCHANGE_POTENTIAL					// 2hogi		2012-02-22		// #52315 #52314 잠재이전
#endif	// #if !defined(JP)

#if defined(_WORK) || defined(_KR) || defined(_KRAZ) || defined(_CH) || defined(_JP) || defined(_TW) || defined(_TH) || defined(_ID)
#define PRE_ADD_COMEBACK							// bintitle, 2hogi		2012-04-16	    // #53590, #53588 복귀유저 보상지급시스템.
#endif // #if defined(_KR) || defined(_KRAZ) || defined(_CH) || defined(_JP) || defined(_TW)
#define PRE_ADD_PREFIXSKILL_PVP						// semozz		2012-01-11		// #45842 [시스템]래더에서 접미사 효과 보정 추가
#define PRE_ADD_REVEAL_REGULATION_VALUE             // Rotha        2012-05-14      // #22342 보정되어 적용되는 값 클라이언트에서 보여주도록 설정
#define PRE_FIX_MODIFY_AVERAGE_DAMAGE_REGULATION    // Rotha        2011-12-14      // #47134 물공/ 마공 보정 평균치로 적용 시키도록 설정.
#endif

#if defined (KR_20120524)

#define PRE_PARTY_DB								// 김밥			2012-01-09		// 파티 DB화

#endif // #if defined (KR_20120524)

#if defined (KR_20120510)
#define PRE_ADD_PVP_DUAL_INFO                       // Rotha        2012-03-29      // #48293 PVP Ladder 1:1 or 올킬전에서 대전자의 이름표시기능. 

#if defined(_SG) || defined(_US) || defined(_TH) || defined(_ID) || defined(_RU) || defined(_EU)
#define PRE_ADD_HID_DUALSKILL                       // Rotha        2013-01-18      // #78319 이중스킬 정보 숨기기
#endif

#define PRE_ADD_SKILLBUF_RENEW						// jhk8211		2011-09-19		// #38650 스킬 개편. 버프/디버프와 스킬 상태효과 계산 적용 위치 구분.
#if defined (PRE_ADD_SKILLBUF_RENEW)
#define PRE_FIX_51491								// semozz		2012-04-18		// #51491 [스킬] 크리티컬 공식 수정 - 크리티컬 저항시 데미지 계산 변경
#define PRE_FIX_53274								// semozz		2012-03-08		// #53274 [스킬]타임 리스트릭션과 타임 인터럽션에 A,D연타 기능 추가(FrameStopBlow에 기능 추가)
#define PRE_ADD_BUFF_STATE_LIMIT                    // Rotha        2012-02-17      // #52322 스킬 개선 관련 버프에 의해 증가되는 State의 Limit을 설정
#define PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP			// semozz		2012-01-18		// #49584 [스킬개편]스킬 레벨업 조건				
#define PRE_ADD_49166								// semozz		2012-01-09		// [스킬]클레릭의 블록 발생 조건 변경

#define PRE_FIX_53266								// semozz		2012-03-08		// #53266 [스킬]화상 데미지 처리 추가 구현 요청
#define PRE_ADD_50903								// semozz		2012-02-03		// #50903 [타이푼킴네스트] 추가스킬효과개발 ID:236 "데미지분산"

#endif // PRE_ADD_SKILLBUF_RENEW

#endif // #if defined (KR_20120510)

#if defined (KR_20120426)
#endif // #if defined (KR_20120426)

#if defined (KR_20120412)
// 기부시스템 싱가폴, 인도네시아, 태국은 나중에 들어갈 예정입니다.
#if defined (_WORK) || defined (_KR) || defined(_KRAZ) || defined (_CH) || defined (_JP) || defined (_TW) || defined (_US)
#define PRE_ADD_DONATION							// hgoori, bintitle	2011-12-27		// 기부
#endif

#define PRE_ADD_CASHREMOVE							// saset, bintitle	2012-01-26	// #50317 캐시인벤토리 아이템 삭제 기능.

#define PRE_ADD_JELLYCOUNT							// bintitle     2012-02-08		// #51457 강화시 보호젤리 확인 편의성 개선.
#define PRE_FIX_51048								// semozz		2012-03-07		// #51048 몬스터 패시브 스킬 경우 상태효과 추가시 서버->클라 패킷 전송이 없음. 클라이언트와 동기화를 위해 서버->클라 패킷 전송
#define PRE_ADD_PVP_VILLAGE_ACCESS                  // Rotha,haling    2012-01-26      // #50888 마을에서 콜로세움 접근 개선
#define PRE_ADD_54166								// semozz		2012-03-28		// #54166 2차 판도라 박스 시스템 개편_선택적 구성품 보여주기 기능 추가
#endif // #if defined (KR_20120412)

#if defined (KR_20120308)
//#define PRE_ADD_UISTRING_DIVIDE						// jhk8211		2012-02-27		// #47557 UIString.xml 파일 아이템 전용으로 분리됨.
#define PRE_ADD_AUTOUNPACK							// bintitle     2012-01-11      // #49509 다량의 보물상자 자동열기. 
#define PRE_MOD_NESTREBIRTH							// kalliste		2012-01-19		// #47733 씨드래곤 네스트 부활 툴팁 오류
#endif  // #if defined (KR_20120308)

#if defined (KR_20120223)
#define PRE_ADD_NEXON_GIO_SECURE					// haling		2012-02-09		// #50869 [한국] 넥슨 위치기반 로그인 보안솔루션 적용
#define PRE_ADD_48682								// bintitle     2012-01-05      // 툴팁개선 - 아이템 사용가능지역 텍스트 시스템화.
#define PRE_ADD_QUICK_PVP							// haling		2011-11-01		// #44645 마을에서 [결투신청] 기능 구현 요청
#define PRE_ADD_SALE_COUPON							// haling		2011-12-19		// #48428 [서버] 신규 쿠폰 시스템 개발
#define PRE_MOD_INTEG_SYSTEM_STATE					// kalliste		2011-11-22		// #44645 마을에서 [결투신청] 기능 구현 요청
#define PRE_ADD_REBIRTH_EVENT						// haling		2011-11-28		// #45852 지정된 시간동안 특정 네스트의 부활 제한을 풀어주는 이벤트 시스템
#define PRE_MOD_REPUTE_NOMALICE2					// kalliste		2012-02-15		// #51874 비호감도 삭제 버그 수정 - 정렬 콤보박스에 비호감도 남아있음

#endif // #if defined (KR_20120223)

#if defined(KR_20120209)
#define PRE_FIX_PARTYRESTORE_DGNCLEAR				// kalliste		2011-12-22		// #48496 스테이지 클리어 프로세스바가 종료되기 직전 파티난입시 보상결과창 이상
#define PRE_FIX_48865                               // Rotha        2012-01-02      // #48865 퀘스트 알림 닫기 기능 추가.
#define PRE_ADD_MODIFY_PLAYER_CANNON                // Rotha        2011-12-09      // #47261 대포 추가작업.
#define PRE_FIX_REPUTE_PRESENTCOUNT					// kalliste		2012-01-04		// #48973 [호감도개선] 선물하기 갯수등록창에 빈칸으로 선물시 오류, #48975 [호감도개선] 연합이 없는 NPC에게 선물시 소속연합없음 메시지 뜨지 않음
#define PRE_REMOVE_NPCREPUTATION_MALICE				// 김밥			2011-12-13		// #47802 호감도 개선_비호감도 삭제
#define PRE_MOD_REPUTE_NOMALICE						// kalliste		2011-12-13		// #47802 호감도 개선_비호감도 삭제
#define PRE_MOD_CHATBG								// kalliste		2011-11-29		// #30688 친절한 드네씨 관련 채팅창 개선

#define PRE_ADD_GUILD_RENEWSYSTEM					// robust		2011-12-12		// 길드개편 시스템

#endif	// #if defined(KR_20120209)

#if defined(KR_20120112)
#define PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL			// jhk8211		2011-12-14		// #44260 사각형 히트 시그널 collision mesh 와 제대로 mindistance 제외 안되는 것 테스트를 위해 수정.
#endif // #if defined(KR_20120112)

#if defined( KR_20111222 )
#define PRE_ADD_CHAR_SATUS_RENEW                    // Rotha        2011-10-12      // #38752 캐릭터 상태창 개선 
#endif // #if defined( KR_20111222 )

#if defined( KR_20111208 )

#define PRE_UNIONSHOP_RENEWAL						// 김밥			2011-10-21		// #43891 [상점 편의성 개선]연합상점 개편상점 방식으로 변경
#define PRE_ADD_36870								// 김밥			2011-09-26		// #36870 Round Action 로그 추가

#endif // #if defined( KR_20111208 )

#if defined( KR_20111124 )

#define PRE_ADD_VEHICLE_ACTION_STRING               // Rotha        2011-11-23      // #46519 탈것종류에 따라서 탑승자의 액션을 다양하게 적용시킬수 있도록 설정.
#define PRE_ADD_AURA_FOR_MONSTER                    // Rotha        2011-08-31      // #40899 [G-드래곤]몬스터가 [오라]형 스킬이 적용되도록 하는 작업
#define PRE_FIX_REFLECT_SE_LIMIT					// jhk8211		2011-10-24		// #44245 디바인 피니쉬먼트 스킬효과 수정. 최대 데미지 반사값 제한.
#define PRE_ADD_LEGEND_ITEM_UI						// verygoodd21	2011-09-30		// #42663 레전드 아이템 등급 추가에 필요한 거래소 및 아이템 UI 개선 요청

#endif // #if defined( KR_20111124 )

#if defined( KR_20111117 )

#if defined(_KR) || defined(_KRAZ)
#define PRE_ADD_SHUTDOWN_CHILD						// kalliste		2011-10-27		// [한국] 셧다운 제도
#endif	// #if defined(_KR)

#endif	// #if defined( KR_20111117 )

#if defined( KR_20111110 )
#define PRE_ADD_LEVELUP_GUIDE						// kalliste		2011-07-22		// #30684 친절한 드네씨 관련_레벨업 및 스킬 알리미
#endif // #if defined( KR_20111110 )

#if defined( KR_20111006 )

#define PRE_ADD_CASH_AMULET                         // Rotha        2011-08-16      // #13810 캐시 : 목걸이 아이템
#define PRE_ADD_37745                               // Rotha        2011-08-29      // #37745 비전투시 무기를 보여주지않는 기능 추가
#define PRE_ADD_REMOVE_PREFIX						// semozz		2011-09-20		// #41778 접미사 해제 아이템[제거의 큐브] 제작 관련 기능 개발 요청
#endif // #if defined( KR_20111006 )

#if defined( KR_20110908 )

#define PRE_FIX_MEMOPT_EXT							// kalliste		2011-08-26		// 메모리 최적화 : 테이블 다이어트
#define PRE_MOD_OPTION_TEXTURE_QUALITY				// kwcsc		2011-08-16		// #39853 클라이언트 최적화 작업 (텍스쳐 퀄리티 옵션으로 선택할 수 있게 함)

#endif // #if defined( KR_20110908 )

#if defined( KR_20110825 )
#define PRE_ADD_39644                               // Rotha        2011-08-11      // #39644 소환몬스터 무적 상태효과 시그널 파라미터 추가.
#define PRE_ADD_PREFIX_SYSTE_RENEW					// semozz		2011-06-14		// 접두사 스킬 발동 조건 변경
#endif  // #if defined( KR_20110825 )

#if defined( KR_20110811 )

#define PRE_ADD_AUTO_DICE                           // Rotha        2011-08-08      // #26939 주사위 선택 자동 설정.
#endif // #if defined( KR_20110811 )

#if defined( KR_20110804 )
#define PRE_ADD_SKILL_ADDTIONAL_BUFF                // Rotha        2011-07-05      // #35691 스킬에 특정상황에서 하나의 스킬이 다중 대상에 각기 다른 목적으로 적용됨에 따라 생기는 예외처리 
#endif  // #if defined( KR_20110804 )

#if defined( KR_20110714 )
#endif // #if defined( KR_20110714 )

#if defined( KR_20110630 )
#if !defined(PRE_ADD_2vs2_LADDER)
#define PRE_FIX_TEAM_LADDER_3vs3                    // Rotha        2011-05-25      // #34428 팀레더 시스템 2:2를 제외한 3:3 만 적용하는 경우 생기는 예외처리.
#endif
#endif // #if defined( KR_20110630 )

#if defined( KR_20110616 )
#endif // #if defined( KR_20110616 )

#if defined( KR_20110504 )
#define PRE_ADD_LOTUSGOLEM							// kalliste		2011-03-09		// #29998 고대늪지골렘 Parts구현
#endif // #if defined( KR_20110504 )





///////////////////////////////////////////////////////////////////////////////////
///Define 정리
///////////////////////////////////////////////////////////////////////////////////
// 기부시스템 싱가폴, 인도네시아, 태국은 나중에 들어갈 예정입니다.
#if defined (_WORK) || defined (_KR) || defined(_KRAZ) || defined (_CH) || defined (_JP) || defined (_TW) || defined (_US)
#define PRE_ADD_DONATION							// hgoori, bintitle	2011-12-27		// 기부
#endif

// 던전 클리어 타임이 1분이 안될경우 어뷰즈 카운트 증가 하는 시스템(미국, 싱말 적용)
#if defined(_SG) || defined(_US)
#define PRE_ADD_STAGECLEAR_TIMECHECK				// karl			2013-02-06		// #81706 [DNNA] 어뷰즈 카운터에 던전 완료 시간 적용
#endif	// #if defined(_SG) || defined(_US)
//////////////////////////////////////////////////////////////////////////////////
// 농장 관련
//////////////////////////////////////////////////////////////////////////////////
#if defined(_US) || defined(_TH) || defined(_SG)
#define PRE_ADD_PRIVATEFARM_EXTEND_CASH				// 김밥			2011-10-31		// [유료화]농장 - 개인농장 추가 확장
#define PRE_ADD_CASHFISHINGITEM						// 2hogi		2011-11-29		// #42584 [유료화]농장 - 유료 낚시대 아이템
#endif //#if defined(_US) || defined(_TH)

#if defined(_KR) || defined(_KRAZ) || defined( _WORK )
#define PRE_REMOVE_FARM_WATER						// 김밥			2011-11-21		// #45543 농장 물 주가 제거
#endif // #if defined(_KR)

#define PRE_ADD_FARM_DOWNSCALE						// 김밥			2011-06-24		// #36223 농장 축소 작업

#if defined( _CH ) || defined( _TW )
#define PRE_ADD_VIP_FARM							// 김밥			2011-05-17		// #33887 농장 편의성 개선(VIP 멤버쉽)
#endif // #if defined( _CH ) || defined( _TW )

//////////////////////////////////////////////////////////////////////////////////
// 길드전 관련
//////////////////////////////////////////////////////////////////////////////////
#if defined(_WORK) || defined(_KR) || defined(_KRAZ) || defined(_CH) || defined (_JP) || defined(_TW) || defined(_US) || defined(_SG) || defined(_ID) || defined (_TH)
#define PRE_ADD_GUILDWAR_ONSCHEDULE					// haling		2011-10-14		// 길드전 스케쥴 switch..이걸로 On,Off 합니다. On시 길드전 스케쥴 얻어옴
#define PRE_REMOVE_GUILD_WAR_UI						// kwcsc						이 디파인 없으면 길드전 관련 UI가 노출되지 않도록 합니다.
#endif //defined(_WORK) || defined(_KR) || defined(_CH) || defined (_JP)

//////////////////////////////////////////////////////////////////////////////////
// 레더 시스템 관련
//////////////////////////////////////////////////////////////////////////////////

#if defined(_EU)
#define PRE_ADD_PVP_HIDE_LADDERSYSTEM // 전체적인 시스템을 숨김.
#define PRE_ADD_PVP_HIDE_LADDERTAB    // 시즌 종료시 콜로세움 탭만 숨김.
#endif

//////////////////////////////////////////////////////////////////////////////////
// 신규 캐릭터 관련
//////////////////////////////////////////////////////////////////////////////////
// 아카데믹
#if defined(_EU)
#define PRE_REMOVE_ACADEMIC							// kwcsc	2011-11-15		// 아카데믹 안들어가는 국가들 캐시샵에 노출안되게 막음 (아카데믹 들어가면 뺄 것)
#else
#define PRE_ADD_ACADEMIC							// hayannal2009	2011-05-02		// 5번째 클래스. 아카데믹.
#endif	// #if defined(_ID) || defined(_RU) || defined(_EU)

// 칼리
#if defined(_US) || defined(_ID) || defined(_RU) || defined(_EU)
#define PRE_REMOVE_KALI								// jhk8211		2012-01-31		// 신규 캐릭터 칼리 해외 제거 디파인. 칼리 배포 후 사용될 예정.
#endif	//
#if !defined(PRE_REMOVE_KALI)
#define PRE_MOD_SELECT_CHAR							// kwcsc, saset	2012-03-20		// #53376 캐릭터 선택, 생성 화면 개편

#define PRE_ADD_KALI								// jhk8211		2012-01-31		// #50492 신규 캐릭터 칼리 추가.  엥쟌
#define PRE_ADD_ASSASSIN                            // 땍屢늬와	


#define PRE_MOD_PVP_DBSP							// robust		2012-03-08		// #52889 신규 캐릭터 생성에 따른 SP처리요청
#define PRE_CHARLIST_SORTING						// kwcsc, saset		2012-09-18		// #62192, #68598 캐릭터 선택화면 정렬 개선
#define PRE_ADD_CHARSELECT_PREVIEW_CASHITEM			// anno414		2012-08-28		// #67840 캐릭터 선택창 캐시코스튬 미리 입어보기 기능 추가
#if defined(PRE_ADD_COMEBACK)
#if defined(_KR) || defined(_KRAZ) || defined( _CH ) || defined(_TW) || defined( _WORK )
#define PRE_ADD_NEWCOMEBACK							// dyss, bintitle			2012-08-27		// #65499,#65498 귀환자 시스템 개선 작업
#endif
#endif	// #if defined(PRE_ADD_COMEBACK)
#endif	// #if !defined(PRE_REMOVE_KALI)

/////////////////////////////////////////////////////////////////////////////////
// 국가별 관리
/////////////////////////////////////////////////////////////////////////////////


// 한국
#if defined (_KR) || defined(_KRAZ) || defined(_WORK)
#define PRE_LOGINDUMP_LOG							// dyss			2013-03-04		// 로그인서버 덤프 확인용 로그 추가(확인되면 삭제해야함)
#endif
#if defined (_KR) || defined(_KRAZ)
#define PRE_ADD_ABUSE_ACCOUNT_RESTRAINT				// 2hogi		2011-06-23		// #36080 어뷰즈카운트가 걸린캐릭터가 2개이상 있으면 계정블럭 처리 추가.
#define PRE_ADD_CASH_REFUND							// haling		2010-10-13		// #22331 캐쉬 청약철회
#define PRE_ADD_PCBANG_RENTAL_ITEM					// haling		2012-05-22		// #58451 PC방 무기, 칼것 대여 시스템 개발-서버
#endif		//#if defined (_KR)

#if defined PRE_ADD_CASH_REFUND
#define PRE_ADD_CASHSHOP_REFUND_CL						// kalliste		2010-10-21		// 청약철회 클라이언트
#define PRE_ADD_CASHSHOP_ACTOZ						// bintitle		2012-11-05		// #72888 액토즈이전 - 캐시샵UI 변경.
#endif

// 중국
#if defined(_CH) || defined (_WORK)
#define PRE_MOD_COSRANDMIX_NPC_CLOSE				// kalliste		2012-10-25		// #71923 [SNDA]합성완료시 조합창이 바로 바뀌지 않도록 개선요청
#define PRE_ADD_STAGE_CLEAR_ADD_REWARD				// verygoodd21, karl	2012-10-08		// #70808 스테이지 완료 추가보상
#define PRE_ADD_68838								// karl			2012-09-13		// #68838 트래져 스테이지 입장방식에 관한 신규기능개발 검토요청
#define PRE_ADD_BESTFRIEND							// robust		2012-04-10		// #55767 [서버]절친시스템 개발
#define PRE_PVP_GAMBLEROOM							// dyss, kwcsc	2013-01-17		// #74525, #77974 [Main]콜로세움 토너먼트 도박모드 작업
#endif
#if defined (_CH)
//#define PRE_ADD_SHANDA_GPLUS						// jhk8211		2012-03-07		// #52877 GPlus(G+) 개발 요청
#define PRE_ADD_CH_EKEYCLEARBONUS					// kalliste		2010-08-24		// #16474 SNDA OA ekey, ecard 설정 유저 스테이지 클리어 추가 경험치 지급 기능 개발
#define PRE_ADD_CHNC2C								// haling		2012-11-27		// #72583 [CHN] C2C 시스템 관련 인터페이스 제공 요청안
#endif // #if defined (_CH)

// 일본
#if defined(_JP) || defined(_WORK)
#define PRE_ADD_MUTE_USERCHATTING					// karl			2012-11-02		// #72847 [JPN][신규기능] 민폐 행위 방지 GM명령어(체팅금지) 개발
#endif
#if defined (_JP)
#define PRE_ADD_CASHSHOP_JPN_REVISEDLAW				// kalliste		2012-09-12		// #68404 [9월 19일] 가챠관련 개정법률 시행에 따른 버튼 제작 건
#define PRE_ADD_ABUSER_BAN							// haling		2010-10-13		// #22419 일본 툴사용의심자 강제블럭(캐릭명 11글자, 헤어컬러)
#endif

// 대만
#if defined(_TW)
#define PRE_ADD_LOGINLOGOUT_LOG						// haling		2010-12-20		// 로그인 로그아웃 짝 맞추기.
#define PRE_ADD_SENDLOGOUT							// haling		2010-11-26		// #24551 Gash 로그아웃 패킷 중복 발송 방지
#define PRE_ADD_GIFT_RETURN							// haling		2010-10-12		// #18248 대만 선물반송
#endif

// 미국
#if defined(_US)
#define PRE_ADD_SHA256								// haling		2012-10-04		// #69571 [DNNA] 미국 개인정보 보호법 개정 관련 (DB)
#define PRE_MOD_MASTER_GENDER						// verygoodd21	2012-01-12		// #49583 사제 시스템에서 성별 없애는 작업
#define PRE_FIX_USA_PROFILE							// hayannal2009	2011-09-29		// USA에선 성별이 빠진다.
#define PRE_ADD_ANTI_CHAT_SPAM						// jhk8211		2011-08-22		// #39253 미국 채팅 스팸 방지.
#define PRE_ADD_36935								// Robust		2011-07-11		//  #36935 GM Account levels update from CS team (미국 GM커맨드 및 액션 추가)
#define PRE_ADD_CASHSHOP_CREDIT						// kwcsc		2011-3-24		// #31032 미국 케시샵 신용카드 캐시 추가 작업
#define PRE_MOD_PETAL_WRITE							// kwcsc		2011-07-27		// 페탈 표기 천단위 콤마 추가로 변경
#define PRE_ADD_STEAMWORKS							// kwcsc		2012-10-31		// #71472 미국 Steam 연동
#ifdef PRE_ADD_STEAMWORKS
#define PRE_ADD_STEAM_USERCOUNT						// 2hogi		2013-01-18		// 79840 Steam유저 유저카운트 로깅
#endif		//#ifdef PRE_ADD_STEAMWORKS
#endif

// 싱가폴
#if defined(_SG)
#define PRE_MOD_SG_WITH_MACADDR						// 2hogi		2012-01-04		// #43829 Mac Address Tracking via DOLIS
#define PRE_FIX_SG_NEWLINECHAR						// kalliste		2011-08-19
#endif //#if defined(_SG)

// 태국
#if defined(_TH)
#endif // #if defined(_TH)

// 인도네시아
#if defined(_ID)
#define PRE_IDN_PVP									// saset		2012-09-25		// #70172 [IDN]DNNC대회전용 서버 구축-프로그램
#define PRE_FIX_54701								// anno414		2012-07-12		// #54701 [Localization]ProhibitWord/Help translation ( 기존의 필터링된 단어(*)를 특정단어로 변경 )
#define PRE_MOD_IDN_GRAPHICOPTION_LOW				// kalliste		2012-06-29		// 인니 그래픽 옵션 일괄 하옵션 조정
#endif
#if defined(_ID) || defined(_WORK)
#define PRE_ADD_POSITIONHACK_POS_LOG				// karl			2013-02-27		// #83369 길드전 PositionHack 관련 Log 추가
#endif

// 러시아
#if defined(_RU) || defined(_WORK)
#define PRE_MOD_CHANGE_SKILLTOOLTIP					// elkain03		2012-08-08		// #65224 스킬 툴팁 변경 요청 작업
#define PRE_MOD_IMPROVEMENT_EXP_SHOW				// elkain03		2012-08-21		// #65602 경험치 획득 표기 개선 ( 공백 추가 ) 
#define PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT	// elkain03		2012-08-08		// #65221 코드상 상수 텍스트에서 UIString 텍스트로 변경 
#define PRE_ADD_EXCEPTIONAL_MAKESTRING				// kwcsc		2012-07-25		// #58808 조합형 스트링으로 번역 불가한 예외적인 스트링들을 따로 관리함
#endif
#if defined(_RU)
#define PRE_ADD_68196								// verygoodd21	2012-09-14		// #68196 미션 달성 깃발 표시 안한
#endif

// 유럽
#if defined(_EU) || defined(_WORK)
#define PRE_ADD_CHANNELNAME							// haling		2013-02-13		// #82299 [EU] Language support with Channel Name
#endif // #if defined(_EU) || defined(_WORK)
#if defined(_EU)
#define PRE_REMOVE_EU_CBTUI_1206					// anno414		2012-11-01		// #72738 [EU_CBT타겟] UI 탭 제거
#define PRE_ADD_MULTILANGUAGE						// kwcsc, 2hogi		2012-01-10		// 다국어 지원 디파인
#define PRE_ADD_ITEMNAME_AUTOCOMPLETE				// 2hogi		2012-01-26		// 다국어 지원관련 아이템이름 자동완성작업
#define PRE_ADD_WORD_GENDER							// kwcsc		2012-09-12		// #68584 EU, RUS 단어 성별 관련 접미사 태그 적용
#define PRE_ADD_COUNTRY_BY_COUNTRY_CONTROL			// verygoodd21	2012-11-07		// 다국어 클라이언트 콘트롤 셋팅
#endif

