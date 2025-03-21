
//#define ACTIVATE_PROTOCOLSHUFFLE		// CCNET에서 셔플처리가 완벽할때까지 DNProtocol.h만 쓸수 있도록 항상 주석으로 막습니다.

#if !defined (_FINAL_BUILD) || !defined (ACTIVATE_PROTOCOLSHUFFLE)

// PROTOCOL_ORIGIN_START	-- 셔플에 필요한 토큰입니다 지우지 말아주세요.

#pragma once
const int PROCESS_DELAY_CHECK_TICK = 3000;

//-----------------------------------------------
//					Command
// - MainCmd
// - SubCmd
//-----------------------------------------------

const USHORT IN_DISCONNECT = 200;
const USHORT IN_DESTROY = 201;

// MainCmd
enum eCSMainCmd
{
	CS_LOGIN = 1,			// 로긴서버쪽
	CS_SYSTEM,				// 아이피 포트 및 서버간 접속 관련 등등...
	CS_CHAR,				// Enter, Leave 등등 모든것
	CS_ACTOR,				// 액터관련
	CS_PROP,				// 프랍관련
	CS_PARTY,				// 파티관련
	CS_ITEM,				// 아이템관련
	CS_ITEMGOODS,			// 아이템에서 상품(?)관련된건 모두 이쪽으로 옮긴다
	CS_NPC,					// 엔피씨관련
	CS_QUEST,				// 퀘스트관련
	CS_CHAT,				// 채팅
	CS_ROOM,				// 룸과 관련된 모든것
	CS_SKILL,				// 스킬관련
	CS_TRADE,				// 상점, 우편, 무인상점 등등
	CS_FRIEND,				// 친구관련이에효
	CS_TRIGGER,				// Trigger
	CS_ACTORBUNDLE,			// Actor 
	CS_GUILD,				// guild
	CS_PVP,					// PvP
	CS_ISOLATE,				// 차단!
	CS_MISSION,				// 미션
	CS_APPELLATION,			// 호칭
	CS_GAMEOPTION,			// 게임옵션
	CS_RADIO,				// 라디오메세지
	CS_GESTURE,				// 제스처
	CS_VOICECHAT,			// 보이스채팅
	CS_CASHSHOP,
	CS_CHATROOM,			// 채팅방
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CS_REPUTATION,
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CS_MASTERSYSTEM,
#if defined( PRE_ADD_SECONDARY_SKILL )
	CS_SECONDARYSKILL,		// 보조스킬
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	CS_ETC,					// 잡동사니
	CS_GUILDRECRUIT,		// 길드 모집 게시판
	CS_CUSTOMEVENTUI,
#if defined (PRE_ADD_DONATION)
	CS_DONATION,
#endif // #if defined (PRE_ADD_DONATION)

#if defined (PRE_ADD_BESTFRIEND)
	CS_BESTFRIEND,
#endif
#ifdef PRE_ADD_DOORS
	CS_DOORS,
#endif		//#ifdef PRE_ADD_DOORS
#if defined( PRE_PRIVATECHAT_CHANNEL )
	CS_PRIVATECHAT_CHANNEL,
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	CS_ALTEIAWORLD,
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )
#if defined(PRE_ADD_DWC)
	CS_DWC,
#endif
	//#####################################################
	// 여기 위에다 추가해주세용~!
	//#####################################################

	CS_FARM,
	CS_FISHING,
	CS_TIMEEVENT,

};

// MainCmd
enum eSCMainCmd
{
	SC_LOGIN = 1,			// 로긴서버쪽
	SC_SYSTEM,				// 아이피 포트 및 서버간 접속 관련 등등...
	SC_CHAR,				// 캐릭터
	SC_ACTOR,				// 액터
	SC_PROP,				// 프랍
	SC_PARTY,				// 파티
	SC_ITEM,				// 아이템
	SC_ITEMGOODS,			// 아이템에서 상품(?)관련된건 모두 이쪽으로 옮긴다
	SC_NPC,					// 엔피씨
	SC_QUEST,				// 퀘스트
	SC_CHAT,				// 채팅
	SC_ROOM,				// 룸관련 모든것
	SC_SKILL,				// 스킬관련
	SC_TRADE,				// 상점, 우편, 무인상점 등등
	SC_FRIEND,				// 친구관련이에효
	SC_TRIGGER,				// Trigger
	SC_ACTORBUNDLE,			// Actor
	SC_GUILD,				// guild
	SC_PVP,					// PvP
	SC_ISOLATE,				// 차단!
	SC_MISSION,				// 미션
	SC_APPELLATION,			// 호칭
	SC_GAMEOPTION,			// 게임옵션
	SC_RADIO,				// 라디오!
	SC_GESTURE,				// 제스처
	SC_VOICECHAT,			// 보이스채팅
	SC_RESTRAINT,			// 유저제재
	SC_CASHSHOP,
	SC_CHATROOM,			// 채팅방
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	SC_REPUTATION,			// 평판시스템
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	SC_MASTERSYSTEM,		// 사제시스템
	SC_VEHICLE,				// 탈것
#if defined( PRE_ADD_SECONDARY_SKILL )
	SC_SECONDARYSKILL,		// 보조스킬
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	SC_ETC,					// 잡동사니
	SC_GUILDRECRUIT,		
	SC_CUSTOMEVENTUI,
#if defined (PRE_ADD_DONATION)
	SC_DONATION,
#endif // #if defined (PRE_ADD_DONATION)

#if defined (PRE_ADD_BESTFRIEND)
	SC_BESTFRIEND,
#endif
#ifdef PRE_ADD_DOORS
	SC_DOORS,
#endif		//#ifdef PRE_ADD_DOORS
#if defined( PRE_PRIVATECHAT_CHANNEL )
	SC_PRIVATECHAT_CHANNEL,
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	SC_ALTEIAWORLD,
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )
#if defined( PRE_ADD_STAMPSYSTEM )
	SC_STAMPSYSTEM,
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_DWC)
	SC_DWC,
#endif

	//#####################################################
	// 여기 위에다 추가해주세용~!
	//#####################################################

	SC_FARM,
	SC_FISHING,
	SC_TIMEEVENT,

};

// LOGIN -----------------------------------------------------------------------------

namespace eLogin {
	// Main: CS_LOGIN
	enum eCSLogin
	{
		CS_CHECKVERSION = 1,			// eNation 체크
		CS_CHECKLOGIN,					// (걍 기본)
		CS_SERVERLIST,					// 서버리스트 
		CS_SELECTSERVER,				// 캐릭터 리스트 보여주기
		CS_SELECTCHAR,					// 캐릭터 선택
		CS_CREATECHAR,					// 캐릭터 생성
		CS_DELETECHAR,					// 캐릭터 삭제
		CS_SELECTCHANNEL,				// 채널 선택
		CS_BACKBUTTON,					// 뒤로 버튼 누르기
		CS_CHANNELLIST,					//채널리스트 재요청
		CS_BACKBUTTON_LOGIN,			// 서버리스트 -> 로그인창
		CS_REVIVECHAR,
		CS_CHARACTERSORTCODE,
		CS_CREATEDWCCHAR,				// DWC 캐릭터 생성

		CS_CHECKLOGIN_KR = 30,			// (넥슨용 인증) id, pass, passport
		CS_CHECKLOGIN_CH,				// (샨다인증)
		CS_CHECKLOGIN_JP,
		CS_CHECKLOGIN_TW,				// (감마니아 인증)
		CS_CHECKLOGIN_US,				// 미국인증
		CS_CHECKLOGIN_SG,				// 췌리인증
		CS_CHECKLOGIN_TH,				// 태국인증
		CS_CHECKLOGIN_TH_OTP,			// 태국인증 OTP추가정보
		CS_CHECKLOGIN_ID,				// 인도네시아 인증
		CS_CHECKLOGIN_RU,				// 러시아 인증
		CS_CHECKLOGIN_EU,
		CS_CHECKLOGIN_KRAZ,
	};

	// Main: SC_LOGIN
	enum eSCLogin
	{
		SC_CHECKVERSION = 1,			// 버젼 체크
		SC_CHECKLOGIN,					// (그냥 기본) id, pass 체크
		SC_SERVERLIST,					// 서버리스트
		SC_CHARLIST,					// 캐릭터리스트 날려주기
		SC_CHANNELLIST,					// 채널 리스트 날리기
		SC_SELECTCHAR,					// 캐릭터 선택
		SC_CREATECHAR,					// 캐릭터 생성
		SC_DELETECHAR,					// 캐릭터 삭제
		SC_TUTORIALMAPINFO,				// 튜토리얼 맵 정보
		SC_WAITUSER,					// 당신 대기자에요 어쩌겠어요 호호호호
		SC_BACKBUTTON_LOGIN,			// 서버리스트 -> 로그인창
		SC_CHECKBLOCK,					// 계정정지
		SC_REVIVECHAR,
		SC_ASIASOFT_REQ_OTP,			// 아시아소프트 원타임패스워드요청
		SC_ASIASOFT_RET_OTP,			// OTP처리결과
		SC_CREATEDWCCHAR,				// DWC 캐릭터 생성 결과		
	};
};

// SYSTEM ---------------------------------------------------------------------------

namespace eSystem {
	// CS_SYSTEM
	enum eCSSystem
	{
		CS_CONNECTVILLAGE = 1,
		CS_VILLAGEREADY,				// village server로 갈 준비가 됐느냐
		CS_CONNECT_REQUEST,
		CS_CONNECTGAME,					//gameserver connect with tcp
		CS_READY_2_RECIEVE,
		CS_INTENDED_DISCONNECT,			//의도된 접속종료
		CS_PEER_DISCONNECT,				//Client끼리 peer가 끊겼습니다.
		CS_RECONNECTLOGIN,				// 로그인으로 다시 접속 (캐릭터 선택)
		CS_ABANDONSTAGE,				// 스테이지 포기
		CS_MOVE_PVPVILLAGETOLOBBY,		// PvP마을에서 PvP로비로 이동 요청
		CS_MOVE_PVPLOBBYTOPVPVILLAGE,	// PvP로비에서 PvP마을로 이동 요청
		CS_MOVE_PVPGAMETOPVPLOBBY,		// PvP게임에서 PvP로비로 이동 요청

		// shanda
		CS_CHN_GPKDATA,					// GPK DATA 요청
		CS_CHN_GPKAUTHDATA,				// GPK AUTH DATA
		// HackShield
		CS_MAKERESPONSE,				// 요청한 메세지 응답 (HackShield)

		CS_VIPAUTOMATICPAY,				// VIP자동결제 

		CS_TCP_PING,
#ifdef PRE_MOD_INDUCE_TCPCONNECT
		CS_TCP_CONNECT_REQ,
#endif		//#ifdef PRE_MOD_INDUCE_TCPCONNECT

		CC_PEER_CONNECT_REQUEST = 30,	//이하 peer관련 메세지 일부러 조금 떨어 뜨렸습니다. 파싱단이 갈라지고 피어끼리 쏘는 메세지라..
		CC_PEER_PING,
		CC_PEER_PONG,
	};

	// SC_SYSTEM
	enum eSCSystem
	{
		SC_VILLAGEINFO = 1,				// 빌리지 서버 정보 (IP, PORT 날리기)
		SC_CONNECTVILLAGE,
		SC_GAMEINFO,					// 게임 서버 정보 (IP, PORT 날리기)
		SC_PEER_CONNECT_REQUEST,		//GameServer -> Client Peer연결 요청
		SC_CONNECTED_RESULT,			//Client -> GameServer rudp컨넥이 완료되었다는 메세지
		SC_TCP_CONNECT_REQ,				//Client -> GameServer Tcp연결하라는 메세지
		SC_RECONNECT_REQ,				//게임서버에 rudp재연결 요청
		SC_RECONNECTLOGIN,				// 로그인으로 다시 접속
		SC_COUNTDOWNMSG,				// 클라이언트에 먼가 카운트! 할꺼 있으면 그냥 이용해주세요.

		SC_MOVE_PVPVILLAGETOLOBBY,		// PvP마을에서 PvP로비로 이동 명령

		// shanda
		SC_CHN_GPKCODE,					// GPK code 보내기
		SC_CHN_GPKDATA,					// GPK DATA 응답
		SC_CHN_FCM,						// 피로도 상태값 전달
		SC_CHN_GPKAUTHDATA,				// GPK AUTH DATA

		// HackShield
		SC_MAKEREQUEST,					// Request Message (HackShield)
		SC_CHAR_COMM_OPTION,			// 상대편 커뮤니티 관련 옵션 받기

		SC_DETACHCONNECTION_MSG,		// 디테칭메세지
		//ServerControl
		SC_ZEROPOPULATION,				// 유저컨트롤
		SC_VIPAUTOMATICPAY,				// VIP자동결제 
		SC_SERVICECLOSE,

		SC_TCP_PING,
		SC_TCP_FIELD_PING,//90cap field ui
	};
};

// CHAR -----------------------------------------------------------------------------

namespace eChar {
	// Main: CS_CHAR
	enum eCSChar
	{
		CS_ENTER = 1,					// 캐릭터 Enter됐을 때
		CS_ADDQUICKSLOT,				// 단축슬롯에 넣기
		CS_DELQUICKSLOT,				// 단축슬롯에 빼기
		CS_LOOKUSER,					// 유저 살펴보기
		CS_COMPLETELOADING,				// 로딩 완료
		CS_CLOSE_UIWINDOW,				// UI윈도우창 닫음
#if defined( PRE_ADD_NOTIFY_ITEM_COMPOUND )
		CS_COMPOUND_NOTIFY,				// 접미사 알리미 저장
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
		CS_STAGECLEAR_BONUSREWARD_SELECT, //스테이지 클리어 후 추가 보상 요청
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)

		CS_DOREBIRTH, //rlkt_rebirth@
		CS_DOSPECIALIZE,
	};

	// Main: SC_CHAR
	enum eSCChar
	{
		SC_MAPINFO = 1,					// 맵정보 날려주기
		SC_ENTER,						// 내정보
		SC_ENTERUSER,					// 나 이외 주변 user정보
		SC_LEAVEUSER,					// user나가기
		SC_ENTERNPC,					// 주변 npc정보
		SC_LEAVENPC,					// npc나가기

		SC_QUICKSLOTLIST,				// 단축 슬롯창 보내기 (어디다 분류해야할지 애매해서 여기다 껴넣기? -_-;)
		//SC_ADDQUICKSLOT,				// 단축슬롯에 넣기
		//SC_DELQUICKSLOT,				// 단축슬롯에 빼기

		SC_FATIGUE,						// 피로도 보내기
		SC_LOOKUSER,					//유저살펴보기

		SC_PVPDATA,						// PvP정보
		SC_HIDE,						// 투명정도

		SC_ENTERPROPNPC,
		SC_LEAVEPROPNPC,

		SC_PCBANG,
		SC_NESTCLEAR,					// 네스트 클리어
		SC_UPDATENESTCLEAR,				// 업뎃
		SC_MAXLEVEL_CHARACTER_COUNT,	// 만렙캐릭터

		SC_COMMONVARIABLE_LIST,
		SC_MOD_COMMONVARIABLE,
		SC_ATTENDANCEEVENT,
#if defined(PRE_ADD_REBIRTH_EVENT)
		SC_REBIRTH_MAXCOIN,				// 부활횟수 이벤트용.
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
		SC_STAGECLEAR_BONUSREWARD_SELECT, //추가 보상 결과 전송
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)


	};
};

// ACTOR ----------------------------------------------------------------------------

namespace eActor {
	// CS_ACTOR
	enum eCSActor
	{
		CS_CMDMOVE = 1,
		CS_CMDSTOP,
		CS_CMDACTION,
		CS_CMDMIXEDACTION,
		CS_CMDMOVETARGET,
		CS_CMDMOVETARGET_NAVI,
		CS_CMDLOOKTARGET,
		CS_ONDAMAGE,
		CS_PROJECTILE,
		CS_INVALID_PROJECTILE,	// 발사체가 갖고 있는 최대 히트 수가 되면 서버에서 프로젝타일 없애라고 보냅니다. 클라에서 보내진 않음.
		CS_CMDLOOK,
		CS_PLAYERPITCH,
		CS_CMDENTEREDGATE,
		CS_USESKILL,
		CS_AGGROTARGET,
		CS_VIEWSYNC,
		CS_CMDTOGGLEBATTLE,
		CS_CMDPASSIVESKILLACTION,
		CS_CMDADDSTATEEFFECT,
		CS_CMDREMOVESTATEEFFECT,
		CS_CMDREMOVESTATEEFFECTFROMID,
		CS_CMDMODIFYSTATEEFFECT,
		CS_ADDEXP,
		CS_LEVELUP,
		CS_ADDCOIN,
		CS_PARTYCHAIN,
		CS_CMDSUICIDE,
		CS_SETHPMP_DELTA,
		CS_SHADOW,
		CS_POSREV,
		CS_RECOVERYSP,
		CS_CMDENABLEOPERATOR,
		CS_REFRESHHPSP,
		//		CS_POSITION,
		CS_PARTSDAMAGE,
		CS_ADD_STATE_EFFECT_DENIED,
		CS_CMDWARP,
		CS_PLAYERDIE,
		CS_CP,
		CS_ONDROP,
		CS_CMDCHANGETEAM,
		CS_CMDFORCEVELOCITY,
		CS_CMDCHATBALLOON,
#if defined(PRE_ADD_MULTILANGUAGE)
		CS_CMDCHATBALLOON_AS_INDEX,
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		CS_SHOWEXPOSUREINFO,
		CS_CMDSHOW,
		CS_CMDHIDE,
		CS_CMDTOGGLEWEAPONORDER,
		CS_CMDTOGGLEPARTSORDER,
		CS_CHANGEJOB,
		CS_RESET_SKILL,
		CS_CMDMOVEPOS,
		CS_CMDPICKUPITEM,
		CS_SLAVE_OF,				// 소환된 몬스터라고 주인 액터 유니크 아이디를 서버에서 날려줌. 클라에서 쏘지 않으므로 이 프로토콜은 쓰이지 않음. 인덱스 맞춰주기 위해서..
		CS_SKILLUSING_FAILED,		// 스킬 사용 실패 서버에서 리턴.
		CS_CMDESCAPE,
		CS_STATEEFFECT_PROB_SUCCESS,	// 확률 발동으로 어떤 일을 수행하는 상태효과가 서버로부터 발동되었다고 클라한테 알려줌. 현재 서버에서 클라로 쏘기만 할때 사용됨.
		CS_MONSTER_PROJECTILE_FROM_PROJECTILE,		// // #15557 이슈 관련. 몬스터가 발사체에서 발사체를 쏘는 경우.
		CS_FINISH_PROCESSSKILL,
		CS_COOLTIME_RESET,
		CS_CANNONPOSSESS_REQ,		// 클라이언트가 대포를 소유하겠다고 게임서버로 요청. 클라이언트에서만 send 한다.
		CS_CANNONPOSSESS_RES,		// 클라이언트의 대포 점유 요청에 대한 서버의 응답. 게임서버에서만 send 한다.
		CS_CANNONRELEASE,			// 클라이언트에서 유저가 대포를 놓았음.
		CS_CANNONROTATESYNC,		// 대포를 사용하고 있는 액터와 대포의 회전값을 100ms 마다 패킷을 보낸다.
		CS_CANNONTARGETING,			// 대포를 조작하는 클라이언트에서 현재 대포의 방향을 기반으로 낙하지점을 계산해서 서버로 보냄.
		CS_SLAVE_RELEASE,			// SC_SLAVE_OF 의 반대 패킷. 서버에서만 날려줌.
		CS_COOLTIMEPARRY_SUCCESS,	// 클라이언트가 SEND 하지 않습니다.
		CS_FREEZINGPRISON_DURABILITY,	//FREEZINGPRISON의 내구도 표시.
		CS_FISHINGROD_CAST,
		CS_FISHINGROD_LIFT,
		CS_FISHINGROD_HIDE,
		CS_SYNCPRESSEDPOS,
		CS_ADDBUBBLE,				// 게임 서버에서만 사용. 클라이언트에게 버블 추가되었음을 알린다.
		CS_REMOVEBUBBLE,			// 게임 서버에서만 사용. 클라이언트에게 버블이 제거 되었음을 알린다.
		CS_REFRESH_BUBBLE_DURATIONTIME, // 게임 서버에서만 사용. 클라이언트에게 버블 중첩 최대 갯수에 도달했으므로 시간만 리셋하라고 알린다.
		CS_SHOCK_INFECTION,			// 게임 서버에서 클라이언트로만..(ShockInfectionBlow에서 프로젝타일 생성을 알리기위한 패킷..)
		CS_PASSIVESKILL_COOLTIME,	// 게임 서버에서 클라이언트로 패시브 스킬 쿨타임 표시 요청(Actor가 LocalPlayer일때만 동작..)
		CS_SHOW_STATE_EFFECT,		// 패시브 상태 효과중 확률로 발동 되는 시점에만 이펙트 표시를 하기위한 패킷
		CS_DO_TEMP_JOBCHANGE,		// 임시로 전직. 서버에서 클라로만 보냅니다.
		CS_RESTORE_TEMP_JOBCHANGE,	// 임시로 전직한 것 복구. 서버에서 클라로만 보냅니다.
		CS_ADD_TEMP_SKILL,			// 임시로 스킬 추가. 서버에서 클라로만 보냅니다.
		CS_REMOVE_TEMP_SKILL,		// 임시로 추가한 스킬 제거. 서버에서 클라로만 보냅니다.
		CS_CMDUPDATESTATEBLOW,		// 접두어 상태 효과 추가하고, 상태효과들 강제로 업데이트 시키기위해 패킷 전송(S->C로만)
		CS_FINISH_AURASKILL,		// 오라 스킬 도중에 MP체크로 중지 될때 S->C로 패킷 전송..
		CS_CMDSHOOTMODE,
		CS_BLOW_GRAPHIC_ERASE,
		CS_CATCH_ACTOR,				// 몬스터가 캐릭터를 잡음. 서버에서 클라로만 보낸다. #28532
		CS_CATCH_ACTOR_FAILED,		// 클라이언트가 SEND 하지 않습니다.
		CS_RELEASE_ACTOR,			// 몬스터가 캐릭터를 놓는다. 서버에서 클라로만 보낸다. #28532
		CS_MONSTERPARTS_STATE,
		CS_VEHICLE_RIDE_COMPLETE,   // 탈것을 정상적으로 탔다는 메세지를 서버에 알려준다.
		CS_GHOST_TYPE,				// 클라이언트가 SEND 하지 않습니다.
		CS_UDP_PING,
		CS_ACTION_CHANGE,			// 아카데믹 왁스 스킬에서 동작 변경.(서버->클라이언트로만)
		CS_CHAINATTACK_PROJECTILE,	// 핑퐁밤/체인 관련 프로젝타일 생성용(서버 -> 클라이언트로만..)
		CS_SUMMONOFF,				// 소환 몬스터 소환 해제(클라이언트 -> 서버로만)
		CS_PROBINVINCIBLE_SUCCESS,	// #41440 226번 상태효과로 데미지 무효화 성공. 근원 아이템에서 사용하는 효과.
		CS_FORCE_ADD_SKILL,         
		CS_FORCE_REMOVE_SKILL, 
		CS_MAILBOX_OPEN,			//마을에서 단축키로 우편함 열기(클라이언트 -> 서버로, 서버는 브로드캐스팅만..)
		CS_MAILBOX_CLOSE,			//마을에서 단축키로 우편함 열기 후 닫을때(클라이언트 -> 서버로, 서버는 브로드캐스팅만..)
		CS_APPLY_SUMMON_MONSTER_EX_SKILL,		//소환 몬스터 EX스킬 적용을 위해 (서버 -> 클라이언트로)
		CS_TOTAL_LEVEL,					//캐릭터의 통합 레벨 정보 (서버 -> 클라이언트로만 패킷 보냄)
		CS_ADD_TOTAL_LEVEL_SKILL,		//통합레벨 스킬 슬롯에 적용시 클라이언트 -> 서버로 요청 패킷
		CS_REMOVE_TOTAL_LEVEL_SKILL,	//통합레벨 스킬 슬롯에 제거시 클라이언트 -> 서버로 요청 패킷
		CS_TOTAL_LEVEL_SKILL_ACTIVE_LIST,	//통합레벨 스킬 등록 리스트 정보 (서버 -> 클라이언트로만 보냄)
		CS_TOTAL_LEVEL_SKILL_CASHSLOT_ACTIVATE,	//통합레벨 스킬 캐쉬 슬롯 활성화 정보(서버 -> 클라이언트로..)
		CS_SKIP_END_ACTION,	// #68898 Input 시그널에 추가된 정보 전송(Input 시그널 처리 시점의 스킬의 partialPalyAniProcessor의 EndAction을 스킵 하도록함.)
		CS_ACTOR_STATE_MAX,  // 마지막에 남겨주세요 nextome
	};

	// eCSActor / eSCActor 에 둘다 선언 할껏.
	// SC_ACTOR
	enum eSCActor
	{
		SC_CMDMOVE = 1,
		SC_CMDSTOP,
		SC_CMDACTION,
		SC_CMDMIXEDACTION,
		SC_CMDMOVETARGET,
		SC_CMDMOVETARGET_NAVI,
		SC_CMDLOOKTARGET,
		SC_ONDAMAGE,
		SC_PROJECTILE,
		SC_INVALID_PROJECTILE,	// 발사체가 갖고 있는 최대 히트 수가 되면 서버에서 프로젝타일 없애라고 보냅니다. 클라에서 보내진 않음.
		SC_CMDLOOK,
		SC_PLAYERPITCH,
		SC_CMDENTEREDGATE,
		SC_USESKILL,
		SC_AGGROTARGET,
		SC_VIEWSYNC,
		SC_CMDTOGGLEBATTLE,
		SC_CMDPASSIVESKILLACTION,
		SC_CMDADDSTATEEFFECT,
		SC_CMDREMOVESTATEEFFECT,
		SC_CMDREMOVESTATEEFFECTFROMID,
		SC_CMDMODIFYSTATEEFFECT,
		SC_ADDEXP,
		SC_LEVELUP,
		SC_ADDCOIN,
		SC_PARTYCHAIN,
		SC_CMDSUICIDE,
		SC_SETHPMP_DELTA,
		SC_SHADOW,
		SC_POSREV,
		SC_RECOVERYSP,
		SC_CMDENABLEOPERATOR,
		SC_REFRESHHPSP,
		SC_PARTSDAMAGE,
		SC_ADD_STATE_EFFECT_DENIED,
		SC_CMDWARP,
		SC_PLAYERDIE,				// 현재는 PvP 시에만 사용되고 있습니다.
		SC_CP,
		SC_ONDROP,
		SC_CMDCHANGETEAM,
		SC_CMDFORCEVELOCITY,
		SC_CMDCHATBALLOON,
#if defined(PRE_ADD_MULTILANGUAGE)
		SC_CMDCHATBALLOON_AS_INDEX,
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		SC_SHOWEXPOSUREINFO,
		SC_CMDSHOW,
		SC_CMDHIDE,
		SC_CMDTOGGLEWEAPONORDER,
		SC_CMDTOGGLEPARTSORDER,
		SC_CHANGEJOB,
		SC_RESET_SKILL,
		SC_CMDMOVEPOS,
		SC_CMDPICKUPITEM,
		SC_SLAVE_OF,				// 소환된 몬스터라고 주인 액터 유니크 아이디를 서버에서 날려줌.
		SC_SKILLUSING_FAILED,		// 스킬 사용 실패 서버에서 리턴.
		SC_CMDESCAPE,
		SC_STATEEFFECT_PROB_SUCCESS,	// 확률 발동으로 어떤 일을 수행하는 상태효과가 서버로부터 발동되었다고 클라한테 알려줌. 현재 서버에서 클라로 쏘기만 할때 사용됨.
		SC_MONSTER_PROJECTILE_FROM_PROJECTILE,		// // #15557 이슈 관련. 몬스터가 발사체에서 발사체를 쏘는 경우.
		SC_FINISH_PROCESSSKILL,
		SC_COOLTIME_RESET,
		SC_CANNONPOSSESS_REQ,		// 클라이언트가 대포를 소유하겠다고 게임서버로 요청. 클라이언트에서만 send 한다.
		SC_CANNONPOSSESS_RES,		// 클라이언트의 대포 점유 요청에 대한 서버의 응답. 게임서버에서만 send 한다.
		SC_CANNONRELEASE,			// 클라이언트에서 대포를 놓았음.
		SC_CANNONROTATESYNC,		// 대포를 사용하고 있는 액터와 대포의 회전값을 100ms 마다 패킷을 보낸다.
		SC_CANNONTARGETING,			// 클라이언트에서 현재 대포의 방향을 기반으로 낙하지점을 계산해서 서버로 보냄.
		SC_SLAVE_RELEASE,			// SC_SLAVE_OF 의 반대 패킷. 서버에서만 날려줌.
		SC_COOLTIMEPARRY_SUCCESS,	// 서버에서 패링 성공시 클라이언트로 알려준다.
		SC_FREEZINGPRISON_DURABILITY,	//FREEZINGPRISON의 내구도 표시.(S -> C로만)
		SC_FISHINGROD_CAST,
		SC_FISHINGROD_LIFT,
		SC_FISHINGROD_HIDE,
		SC_SYNCPRESSEDPOS,
		SC_ADDBUBBLE,				// 게임 서버에서만 사용. 클라이언트에게 버블 추가되었음을 알린다.
		SC_REMOVEBUBBLE,			// 게임 서버에서만 사용. 클라이언트에게 버블이 제거 되었음을 알린다.
		SC_REFRESH_BUBBLE_DURATIONTIME, // 게임 서버에서만 사용. 클라이언트에게 버블 중첩 최대 갯수에 도달했으므로 시간만 리셋하라고 알린다.
		SC_SHOCK_INFECTION,			// 게임 서버에서 클라이언트로만..(ShockInfectionBlow에서 프로젝타일 생성을 알리기위한 패킷..)
		SC_PASSIVESKILL_COOLTIME,	// 게임 서버에서 클라이언트로 패시브 스킬 쿨타임 표시 요청(Actor가 LocalPlayer일때만 동작..)
		SC_SHOW_STATE_EFFECT,		// 패시브 상태 효과중 확률로 발동 되는 시점에만 이펙트 표시를 하기위한 패킷
		SC_DO_TEMP_JOBCHANGE,		// 임시로 전직. 서버에서 클라로만 보냅니다.
		SC_RESTORE_TEMP_JOBCHANGE,	// 임시로 전직한 것 복구. 서버에서 클라로만 보냅니다.
		SC_ADD_TEMP_SKILL,			// 임시로 스킬 추가. 서버에서 클라로만 보냅니다.
		SC_REMOVE_TEMP_SKILL,		// 임시로 추가한 스킬 제거. 서버에서 클라로만 보냅니다.
		SC_CMDUPDATESTATEBLOW,		// 접두어 상태 효과 추가하고, 상태효과들 강제로 업데이트 시키기위해 패킷 전송(S->C로만)
		SC_FINISH_AURASKILL,		// 오라 스킬 도중에 MP체크로 중지 될때 S->C로 패킷 전송..
		SC_CMDSHOOTMODE,
		SC_BLOW_GRAPHIC_ERASE,
		SC_CATCH_ACTOR,				// 몬스터가 캐릭터를 잡음. 서버에서 클라로만 보낸다. #28532
		SC_CATCH_ACTOR_FAILED,		// 몬스터가 캐릭터 잡기 실패. 현재 패링이나 블록으로 인한 액션 실행으로 처리됨. #28532
		SC_RELEASE_ACTOR,			// 몬스터가 캐릭터를 놓는다. 서버에서 클라로만 보낸다. #28532
		SC_MONSTERPARTS_STATE,
		SC_VEHICLE_RIDE_COMPLETE,   // 서버가 SEND 하지 않습니다.
		SC_GHOST_TYPE,				// 클라이언트가 SEND 하지 않습니다.
		SC_UDP_PING,
		SC_ACTION_CHANGE,			// 아카데믹 왁스 스킬에서 동작 변경.(서버->클라이언트로만)
		SC_CHAINATTACK_PROJECTILE,	// 핑퐁밤/체인 관련 프로젝타일 생성용(서버 -> 클라이언트로만..)
		SC_SUMMONOFF,				// 소환 몬스터 소환 해제(클라이언트 -> 서버로만)
		SC_PROBINVINCIBLE_SUCCESS,	// #41440 226번 상태효과로 데미지 무효화 성공. 근원 아이템에서 사용하는 효과.
		SC_FORCE_ADD_SKILL,         // Actor의 스킬을 넣거나 뺀다 / 서버에서만 보낸다.
		SC_FORCE_REMOVE_SKILL,      // 
		SC_MAILBOX_OPEN,			//마을에서 단축키로 우편함 열기
		SC_MAILBOX_CLOSE,			//마을에서 단축키로 우편함 열기 후 닫을때
		SC_APPLY_SUMMON_MONSTER_EX_SKILL,		//소환 몬스터 EX스킬 적용을 위해 (서버 -> 클라이언트로)
		SC_TOTAL_LEVEL,					//캐릭터의 통합 레벨 정보 (서버 -> 클라이언트로만 패킷 보냄)
		SC_ADD_TOTAL_LEVEL_SKILL,		//통합레벨 스킬 슬롯에 적용시 서버 -> 클라이언트로 결과 패킷
		SC_REMOVE_TOTAL_LEVEL_SKILL,	//통합레벨 스킬 슬롯에 제거시 서버 -> 클라이언트로 결과 패킷
		SC_TOTAL_LEVEL_SKILL_ACTIVE_LIST,	//통합레벨 스킬 등록 리스트 정보 (서버 -> 클라이언트로만 보냄)
		SC_TOTAL_LEVEL_SKILL_CASHSLOT_ACTIVATE,	//통합레벨 스킬 캐쉬 슬롯 활성화 정보(서버 -> 클라이언트로..)
		SC_SKIP_END_ACTION,	// #68898 Input 시그널에 추가된 정보 전송(Input 시그널 처리 시점의 스킬의 partialPalyAniProcessor의 EndAction을 스킵 하도록함.)
		SC_ACTOR_STATE_MAX, // 마지막에 남겨주세요 nextome
	};
};

// PROP -----------------------------------------------------------------------------

// prop
namespace eProp {
	// CS_PROP
	enum eCSProp
	{
		CS_ONDAMAGE,
		CS_CMDACTION,
		CS_CMDSHOW,
		CS_CMDOPERATION,
		CS_CMDENABLEOPERATOR,
		CS_CMDKEEPCLICK,
		CS_CMDKEEPCLICKSTART,
		CS_CMDKEEPCLICKCANCLE,
		CS_QUERY_RANDOM_CREATED, // #42473 _VisibleProb 이 100%가 아닌 경우 서버의 랜덤 결과로 생성되었는지 클라이언트에서 조회.
		CS_CMDCHATBALLOON_AS_INDEX,
		CS_SEND_SHOWINFO,
		CS_PROP_STATE_MAX,		// 마지막에 남겨주세요
	};

	// SC_PROP
	enum eSCProp
	{
		SC_ONDAMAGE,
		SC_CMDACTION,
		SC_CMDSHOW,
		SC_CMDOPERATION,
		SC_CMDENABLEOPERATOR,
		SC_CMDKEEPCLICK,
		SC_CMDKEEPCLICKSTART,
		SC_CMDKEEPCLICKCANCLE,
		SC_QUERY_RANDOM_CREATED, // #42473 _VisibleProb 이 100%가 아닌 경우 서버의 랜덤 결과로 생성되었는지 클라이언트에서 조회.
		SC_CMDCHATBALLOON_AS_INDEX,
		SC_SEND_SHOWINFO,
		SC_PROP_STATE_MAX,		// 마지막에 남겨주세요
	};
};

// PARTY ----------------------------------------------------------------------------

namespace eParty {
	// Main: CS_PARTY
	enum eCSParty
	{
		CS_CREATEPARTY = 1,			// 파티 생성
		CS_JOINPARTY,				// 파티 쪼~인
		CS_PARTYOUT,				// 파티 나가기
		CS_PARTYLISTINFO,			// 파티 리스트 보여주기
		CS_REFRESHGATEINFO,			// 파티원들 다 모여서 gate앞에 섰을때 정보 갱신
		CS_STARTSTAGE,				// 필드로 고고싱~
		CS_CANCELSTAGE,				// 필드로 가는거 취소
		CS_PARTYINVITE,				//파티초대
		CS_PARTYINVITE_DENIED,		//초대 거부할때 보내주세효
		CS_PARTYINVITE_OVERSERVER_RESULT,		//게임서버에서의 초대요청을 받았을때(빌리지에서)처리
		CS_PARTYMEMBER_INFO,		//파티멤버 정보 요청
		CS_PARTYMEMBER_KICK,		//멤버 킥
		CS_PARTYLEADER_SWAP,		//파티장 위임
		CS_CHANNELLIST,				//게임중 빌리지에서 채널변경용
		CS_SELECTCHANNEL,			//이동 할 채널 요청
		CS_PARTYMODIFY,				//파티정보 수정
		CS_SELECTDUNGEON,			//던전시작할때에 파티장이 창고르는거 보내주는거어
		CS_JOINGETREVERSEITEM,		//귀속아이템 주사위 굴리기 참여 여부
		CS_READYREQUEST,			//파티원들에게 포탈에 모여달라는 요청
		CS_ABSENT,					//게임서버에서 자리비움시
		CS_VERIFYTARGETDUNGEON,		//파티생성 또는 정보 변경시 목표던전 설정 검증
		CS_PARTYASKJOIN,			//파티장(원)이름을 키로 파티에 가입신청
		CS_PARTYASKJOINDECISION,		//파티장이 신청을 받은 경우 결과값
		CS_SWAPMEMBERINDEX,			//파티멤버인덱스변경
		CS_CONFIRM_LASTDUNGEONINFO,	// 이전 파티 합류 응답
#if defined( PRE_PARTY_DB ) && defined( _WORK )
		CS_CREATEPARTY_CHEAT,
#endif // #if defined( PRE_PARTY_DB ) && defined( _WORK )
		CS_SELECTSTAGE,
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
		CS_PARTYINFO,
#endif
		CS_GAMETOGAMESTAGE_ENTER, //Cristal Stream packet!
	};

	// Main: SC_PARTY
	enum eSCParty
	{
		SC_CREATEPARTY = 1,				// 파티생성
		SC_JOINPARTY,					// 파티쪼~인
		SC_REFRESHPARTY,				// 파티갱신
		SC_PARTYOUT,					// 파티종료
		SC_PARTYLISTINFO,				// 파티 리스트 보여주기
		SC_REFRESHGATEINFO,				// 파티원들 필드 나갈준비 게이트 갱신
		SC_GATEINFO,					// 파티원들 게이트 정보
		SC_EQUIPDATA,					//장착장비데이타
		SC_STARTSTAGE,					// 필드로 고고싱
		SC_STARTVILLAGE,				// 빌리지 서버 준비됐으니 들어와라~
		SC_STARTSTAGEDIRECT,			// 던전에서 월드맵으로.
		SC_CANCELSTAGE,					// 필드로 가는거 취소
		SC_CHANGEPARTYLEADER,			// 파티 리더 바뀜
		SC_PARTYINVITE,					//파티에서 유저에게 초대메세지 날림
		SC_PARTYINVITEFAIL,				//파티초대 실패
		SC_PARTYMEMBER_INFO,			//파티 멤버 정보
		SC_PARTYINFO_ERROR,				//파티정보에러
		SC_PARTYMEMBERKICKED,
		SC_PARTYLEADERSWAP,
		SC_SKILLDATA,					// 장착장비데이타와 같을때 쏴주는 패시브 스킬 데이타
		SC_CHANNELLIST,					//게임중 빌리지에서 채널변경용
		SC_MOVECHANNELFAIL,
		SC_DEFAULTPARTSDATA,			// 처음 생성시 파츠 인덱스 데이타
		SC_SELECTDUNGEON,				//던전인포(던전입장시고르는거)
		SC_PARTYMODIFY,					//파티정보 변경시		
		SC_VERIFYJOINPARTY,				//미리검증용
		SC_PARTYMEMBER_MOVE,			// 파티멤버 이동관련
		SC_ETCDATA,						// 기타 자잘한 파티 정보 마춰줘야할것들은 일루보낸다.
		SC_PARTYINVITEDENIED,			// 초대한 누군가가 거절했다
		SC_PARTYMEMBER_PART,			// 파티 멤버 정보 부분 업뎃
		SC_GUILDDATA,					// 파티원 각각의 길드정보를 모아서 보냄
		SC_EQUIPCASHDATA,
		SC_WEAPONORDERDATA,
		SC_REQUEST_JOINGETREVERSIONITEM,	// 파티원들에게 귀속아이템 획득 주사위 참가 여부 문의
		SC_NOTIFY_ROLLGETREVERSIONITEM,		// 파티원이 귀속아이템 획득 주사위 굴렸음 통보
		SC_NOTIFY_RESULTGETREVERSIONITEM,	// 파티원들에게 귀속아이템 결과 통보
		SC_PARTYREQUEST,					// 파티원에게 레디요청
		SC_ABSENT,							// 파티원에서 자리비움을 알림
		SC_VERIFYTARGETDUNGEON,				// 파티원 또는 개인(파티생성시)의 던전확인
		SC_REFRESHREBIRTHCOIN,				// 게임서버에서 클라이언트 UI업데이트용
		SC_PARTYBONUSVALUE,					// 파티에 특정보너스가 걸려질 경우 업데이트용
		SC_GLYPHDATA,
		SC_PARTYINVITENOTICE,				// 게임서버에서 파티초대일경우 알림처리
		SC_VEHICLEEQUIPDATA,
		SC_ASKJOINPARTYTOLEADER,			// 파티장에게 파티신청
		SC_ASKJOINPARTYRESULT,				// 파티신청결과메세지
		SC_ASKJOINPARTYDECISIONINFO,		// 파티장의 결정
		SC_PARTYUPDATEFIELDUI,				// 마을에서 파티유아이갱신용
		SC_SWAPMEMBERINDEX,					// 인덱스 스왑 갱신(리후레쉬로 보내도 데지만 좀 잦을 거 같아서 띄어냄)
		SC_PETEQUIPDATA,
		SC_CHECK_LASTDUNGEONINFO,			// 이전 던젼 합류 체크
		SC_CONFIRM_LASTDUNGEONINFO,			// 이전 던젼 합류 응답
		SC_SELECTSTAGE,
#if defined ( PRE_ADD_BESTFRIEND)
		SC_BESTFRIENDDATA,
#endif
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
		SC_PARTYINFO,
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		SC_TALISMANDATA,
#endif
	};
};

// ITEM -----------------------------------------------------------------------------
// item쪽은 CS, SC 숫자가 겹치면 안되기 때문에 상품까지 넣다보면 byte(cmd값)가 넘어갈 듯. ItemGoods로 빼버림

namespace eItem {
	// Main: CS_ITEM
	enum eCSItem
	{
		CS_MOVEITEM = 1,				// inven <-> ware
		CS_REMOVEITEM,					// 휴지통에 버리기
		CS_PICKUP,						// SC_PICKUP 을 서버에서 사용하기때문에 인덱스가 겹치므로 더미느낌으로 넣어둬야해요.
		CS_REFRESHQITEM,				// 퀘스트 아이템
		CS_REMOVEQUTEM,					// 퀘스트 아이템
		CS_USEITEM,						// 물약같이 사용하는 아이템
		CS_EMBLEM_COMPOUND_OPEN_REQ,	// 문장 보옥 윈도우 오픈 요청
		CS_ITEM_COMPOUND_OPEN_REQ,		// 아이템 조합 윈도우 오픈 요청
		CS_EMBLEM_COMPOUND_REQ,			// 문자 보옥 요청
		CS_ITEM_COMPOUND_REQ,			// 아이템 합성 요청
		CS_EMBLEM_COMPOUND_CANCEL_REQ,	// 문장 보옥 취소 요청
		CS_ITEM_COMPOUND_CANCEL_REQ,	// 아이템 합성 취소 요청
		CS_MODITEMEXPIREDATE,			// 아이템 속성을 수정. 현재는 유료펫의 유효기간연장으로 사용중임. by supertj@20110524 
		CS_REBIRTH,						// 부활(마을로)
		CS_REBIRTH_COIN,				// 부활, 코인사용

		CS_MOVECOIN,					// 창고돈

		// 랜덤아이템 관련 왔다갔다.
		CS_COMPLETE_RANDOMITEM,			// 랜덤 아이템 연출 완료
		CS_CANCEL_RANDOMITEM,			// 랜덤 아이템 캔슬

		// 아이템 분해. 월드존에서만 하기 때문에 게임서버에서만 처리합니다.
		CS_DISJOINT_REQ,

		// 강화
		CS_ENCHANT,						// 강화버튼
		CS_ENCHANTCOMPLETE,				// 강화완료
		CS_ENCHANTCANCEL,				// 강화취소버튼

		CS_SORTINVENTORY,				// 아이템정렬
		CS_SORTCASHINVENTORY,			// 캐쉬아이템정렬

		CS_MOVECASHITEM,

		CS_MOVEGUILDITEM,				// 길드아이템 이동
		CS_MOVEGUILDCOIN,				// 길드코인 이동
		CS_DELETE_PET_SKILL,			// 펫 스킬 삭제..
		CS_SORTWAREHOUSE,				// 개인창고 정렬
		CS_REMOVECASH,

		CS_USE_AUTOUNPACK_CASHITEM_REQ, // 자동열기아이템 사용요청.
#if defined(PRE_ADD_SERVER_WAREHOUSE)
		CS_MOVE_SERVERWARE,				// 서버창고 일반 아이템 이동
		CS_MOVE_CASHSERVERWARE,			// 서버창고 캐쉬 아이템 이동
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		CS_OPEN_TALISMANSLOT,
#endif
#if defined(PRE_ADD_EQUIPLOCK)
		CS_ITEM_LOCK_REQ,				// 아이템 잠금 요청
		CS_ITEM_UNLOCK_REQ,				// 아이템 잠금 해제 요청
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
#if defined(PRE_PERIOD_INVENTORY)
		CS_SORTPERIODINVENTORY,
		CS_SORTPERIODWAREHOUSE,
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	};

	// Main: SC_ITEM
	enum eSCItem
	{
		SC_INVENLIST = 128,				// inven 전체 정보 날려주기
		SC_CASHINVENLIST,				// 캐쉬 inven 전체 정보 날려주기
		SC_EQUIPLIST,
		SC_MOVEITEM,					// 아이템 이동 관련: INVEN <-> EQUIP, EQUIP <-> EQUIP, INVEN <-> INVEN
		SC_PICKUP,						// 아이템 바닥에서 줍기
		SC_REMOVEQUESTITEM,				// 퀘스트 아이템
		SC_REMOVEITEM,					// 아이템 휴지통에 버리기
		SC_CHANGEEQUIP,					// 주변애들한테 equip정보 날려주기
		SC_CHANGECASHEQUIP,				// 주변애들한테 cash equip정보 날려주기
		SC_REFRESHINVEN,				// inven에 아이템 넣고 빼기
		SC_REFRESHCASHINVEN,			// cash inven 수정
		SC_CREATE_DROPITEM,
		SC_DROPITEM_MSG,
		SC_WAREHOUSELIST,				// 창고
		SC_USEITEM,						// 물약 같은애들 사용하는거
		SC_EMBLEM_COMPOUND_OPEN_RES,	// 문장 보옥 윈도우 오픈 요청
		SC_ITEM_COMPOUND_OPEN_RES,		// 아이템 조합 윈도우 오픈 요청
		SC_EMBLEM_COMPOUND_RES,			// 문장 보옥 결과 응답
		SC_ITEM_COMPOUND_RES,			// 아이템 조합 결과 응답
		SC_EMBLEM_COMPOUND_CANCEL_RES,	// 문장 보옥 취소 요청
		SC_ITEM_COMPOUND_CANCEL_RES,	// 아이템 합성 취소 요청
		SC_INVENTORYMAXCOUNT,			// 인벤 최대 개수
		SC_WAREHOUSEMAXCOUNT,			// 창고 최대 개수
		SC_QUESTINVENLIST,				// 퀘스트 인벤 날려주기
		SC_REFRESHQUESTINVEN,			// 퀘스트 인벤 갱신 (생성, 삭제)

		SC_REBIRTH,						// 부활(마을로)
		SC_REBIRTH_COIN,				// 부활, 코인사용
		SC_REFRESHEQUIP,				// 이큅 리플레쉬
		SC_REPAIR_EQUIP,				// 장착중수리
		SC_REPAIR_ALL,					// 전체수리

		SC_MOVECOIN,					// 인벤 <-> 창고코인

		SC_MOVEGUILDITEM,				// 길드아이템 이동
		SC_MOVEGUILDCOIN,				// 길드코인 이동
		SC_REFRESH_GUILDITEM,			// 길드아이템 정보 업데이트
		SC_REFRESH_GUILDCOIN,			// 길드코인 정보 업데이트

		// 랜덤아이템 관련 왔다갔다.
		SC_REQUEST_RANDOMITEM,			// 랜덤 아이템 연출 시작
		SC_COMPLETE_RANDOMITEM,			// 랜덤 아이템 결과값

		// 아이템 분해. 월드존에서만 하기 때문에 게임서버에서만 처리합니다.
		SC_DISJOINT_RES,

		// 강화
		SC_ENCHANT,						// 강화버튼
		SC_ENCHANTCOMPLETE,				// 강화완료
		SC_ENCHANTCANCEL,				// 강화취소버튼

		SC_GLYPHTIMEINFO,				// 문장 Dealy, Remain 관련 정보
		SC_SORTINVENTORY,				// 아이템정렬
		SC_SORTCASHINVENTORY,

		SC_DECREASE_DURABILITY_INVENTORY,
		SC_BROADCASTINGEFFECT,			// 아이템 이펙트연출

		SC_CREATE_DROPITEMLIST,

		SC_MOVECASHITEM,
		SC_CHANGEGLYPH,					// 주변애들한테 glyph정보 날려주기

		SC_VEHICLEEQUIPLIST,			// 탈것 착용 정보 날려주기
		SC_VEHICLEINVENLIST,			// 펫&탈것 인벤 정보 날려주기.
		SC_CHANGEVEHICLEPARTS,			// 주변애들한테 Vehicle 정보 날리기
		SC_CHANGEVEHICLECOLOR,			// 
		SC_REFRESHVEHICLEINVEN,			// 
		SC_CHANGEPETPARTS,				// 주변애들한테 Pet 정보 날리기
		SC_CHANGEPETBODY,				// 주변에 펫 통짜로 보내기 (Body로 보낼때)
		SC_PETEQUIPLIST,				// 펫 소환 정보 날려주기.
		SC_ADDPETEXP,					// 펫 경험치

#if defined( PRE_ADD_VIP_FARM )
		SC_EFFECTITEM_INFO,
#endif // #if defined( PRE_ADD_VIP_FARM )
		SC_GLYPHEXTENDCOUNT,			// 문장 추가 슬롯 개수
		SC_GLYPHEXPIREDATA,				// 기간제 문장 슬롯 정보 전송
		SC_INCREASELIFE,
		SC_USE_SOURCE,
		SC_DELETE_PET_SKILL,
		SC_REFRESHWAREHOUSE,
		SC_SORTWAREHOUSE,
		SC_REMOVECASH,

		SC_USE_AUTOUNPACK_CASHITEM, // 자동열기아이템 사용요청결과.

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		SC_EFFECTSKILLITEM,
		SC_DELEFFECTSKILLITEM,
		SC_SHOWEFFECT,
		SC_SENDNAMEDITEMID,
#endif
		SC_PET_CURRENT_SATIETY,		// 펫 만복도 수치 변경
		SC_PET_FOOD_EAT,			// 펫 먹이 결과
#if defined(PRE_ADD_EXPUP_ITEM)
		SC_DAILY_LIMIT_ITEM,		// 하루 사용제한이 걸린 아이템 에러메시지용
#endif
		SC_MODITEMEXPIREDATE,		// 유효기간연장 요청 응답 패킷 - stupidfox #60598
#if defined( PRE_ADD_LIMITED_SHOP )
		SC_LIMITEDSHOPITEMDATA,		// 구인갯수 제한 아이템 목록
#endif
#if defined( PRE_ADD_TRANSFORM_POTION)
		SC_CHANGE_TRANSFORM,		// 변신
#endif
#if defined( PRE_ADD_SERVER_WAREHOUSE)
		SC_SERVERWARE_LIST,				// 서버창고 리스트
		SC_CASHSERVERWARE_LIST,			// 서버창고 캐쉬 리스트
		SC_MOVE_SERVERWARE,				// 서버창고 일반 아이템 이동
		SC_MOVE_CASHSERVERWARE,			// 서버창고 캐쉬 아이템 이동
#endif // #if defined( PRE_ADD_SERVER_WAREHOUSE)
#if defined(PRE_ADD_PVP_EXPUP_ITEM)
		SC_USE_PVPEXPUP,			// 콜로세움 경험치 증가 아이템 사용 결과
#endif // #if defined(PRE_ADD_PVP_EXPUP_ITEM)
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		SC_OPEN_TALISMANSLOT,
		SC_CHANGETALISMAN,			// 주변애들한테 Talisman 정보 날려주기
		SC_TALISMANEXPIREDATA,
#endif
#if defined(PRE_ADD_EQUIPLOCK)
		SC_ITEM_LOCK_RES,				// 아이템 잠금 결과 리턴
		SC_ITEM_UNLOCK_RES,				// 아이템 잠금 해제 결과 리턴
		SC_EQUIPITEM_LOCKLIST,			// 잠금 아이템 리스트 전송
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
#if defined(PRE_PERIOD_INVENTORY)
		SC_PERIODINVENTORY,
		SC_PERIODWAREHOUSE,
		SC_SORTPERIODINVENTORY,
		SC_SORTPERIODWAREHOUSE,
#endif	// #if defined(PRE_PERIOD_INVENTORY)
#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
		SC_STAGE_USELIMITITEM,
#endif
#ifdef PRE_ADD_DRAGON_GEM
		SC_ADD_DRAGONGEM,
		SC_DEL_DRAGONGEM,
		SC_LIST_DRAGONGEM,
		SC_REQLIST_DRAGONGEM,
#endif
	};
};


// ITEMGOODS -----------------------------------------------------------------------------
// 아이템쪽에 캐쉬템 많아지면 byte(cmd값)넘어갈것 같아서 이쪽으로 뺀다.

namespace eItemGoods {
	// Main: CS_ITEMGOODS
	enum eCSItemGoods
	{
		CS_UNSEAL,						// 아이템 봉인해제
		CS_SEAL,						// 아이템 밀봉
#ifdef PRE_ADD_GACHA_JAPAN
		CS_JP_GACHAPON_REQ,				// 일본 가챠폰 아이템 요청.
#endif //PRE_ADD_GACHA_JAPAN

		// 코스튬 합성
		CS_COSMIXOPEN,
		CS_COSMIXCLOSE,
		CS_COSMIXCOMPLETECLOSE,
		CS_COSMIXCOMPLETE,

		CS_POTENTIALJEWEL,
		CS_POTENTIALJEWEL888,
		CS_POTENTIALJEWEL999,

		// Charm
		CS_CHARMITEMREQUEST,			// Charm 아이템 연출 시작
		CS_CHARMITEMCOMPLETE,			// Charm 아이템 연출 완료
		CS_CHARMITEMCANCEL,				// Charm 아이템 캔슬


#if defined (PRE_ADD_CHAOSCUBE)
		// Chaos Cube
		CS_CHAOSCUBEREQUEST,			// Chaos Cube 연출 시작
		CS_CHAOSCUBECOMPLETE,			// Chaos Cube 연출 완료
		CS_CHAOSCUBECANCEL,				// Chaos Cube 캔슬
#endif

		// GuildReName
		CS_GUILDRENAME,					// 길드명변경 캐쉬템
		CS_CHARACTERRENAME,				// 캐릭터명 변경 캐쉬템
		CS_ENCHANTJEWEL,
		CS_GUILDMARK,					// 길드마크 변경
		CS_CHANGEPETNAME,				// 펫이름변경
		CS_USE_CHANGEJOB_CASHITEM_REQ,	// 바로 전직 아이템 사용 요청.

		// 코스튬 디자인 합성
		CS_DESIGNMIXCLOSE,
		CS_DESIGNMIXCOMPLETECLOSE,
		CS_DESIGNMIXCOMPLETE,

		CS_REMOVE_PREFIX,				//접미사? 정보 삭제
		CS_PETSKILLITEM,				// 펫 스킬아이템 장착
		CS_PETSKILLEXPAND,				// 펫 스킬확장아이템		

		CS_WARP_VILLAGE_LIST,
		CS_WARP_VILLAGE,

		CS_RANDOMMIXCLOSE,
		CS_RANDOMMIXCOMPLETECLOSE,
		CS_RANDOMMIXCOMPLETE,

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		CS_EXCHANGE_POTENTIAL,			// 잠재이동
#endif			//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

#if defined (PRE_ADD_BESTFRIEND)
		CS_BESTFRIENDREQUEST,
		CS_BESTFRIENDCOMPLETE,
		CS_BESTFRIENDCANCEL,		// CS_CHARMITEMCANCEL, CS_CHAOSCUBECANCEL -> CS_RANDOMITEMCANCEL로 추후에 통합요망
#endif
#if defined(PRE_ADD_EXCHANGE_ENCHANT)
		CS_EXCHANGE_ENCHANT,			// 강화이동
#endif
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
		CS_POTENTIALJEWEL_ROLLBACK,			//잠재력 부여 취소
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	};

	// Main: SC_ITEMGOODS
	enum eSCItemGoods
	{
		SC_UNSEAL,						// 아이템 봉인해제
		SC_SEAL,						// 아이템 밀봉
		SC_REQUEST_CASHWORLDMSG,		// 캐시아이템 월드대화
		SC_CHANGECOLOR,					// HAIR, EYE, SKIN COLOR값 바뀌는거

#ifdef PRE_ADD_GACHA_JAPAN
		SC_JP_GACHAPON_OPEN_SHOP_RES,	// 일본 가챠폰 상점 오픈.
		SC_JP_GACHAPON_RES,				// 일본 가챠폰 아이템 요청 응답.
#endif //PRE_ADD_GACHA_JAPAN

		// 코스튬 합성
		SC_COSMIXOPEN,
		SC_COSMIXCLOSE,
		SC_COSMIXCOMPLETECLOSE,
		SC_COSMIXCOMPLETE,

		// 포텐셜 부치기
		SC_POTENTIALJEWEL,

		// Charm
		SC_CHARMITEMREQUEST,			// Charm 아이템 연출 시작
		SC_CHARMITEMCOMPLETE,			// Charm 아이템 결과값

#if defined (PRE_ADD_CHAOSCUBE)
		// Chaos Cube
		SC_CHAOSCUBEREQUEST,			// Chaos Cube 연출 시작
		SC_CHAOSCUBECOMPLETE,			// Chaos Cube 결과값
#endif

		SC_PETALTOKENRESULT,			// 페탈 상품권 산거 채팅창에..
		SC_APPELLATIONGAINRESULT,
		SC_GUILDRENAME,					// 길드명변경 결과
		SC_CHARACTERRENAME,				// 캐릭터명 변경 결과
		SC_ENCHANTJEWEL,
		SC_GUILDMARK,					// 길드마크변경결과
		SC_USE_CHANGEJOB_CASHITEM_RES,	// 바로 전직 아이템 사용 클라이언트로 응답.
		SC_CHANGEPETNAME,				// 펫이름변경

		// 코스튬 디자인 합성
		SC_DESIGNMIXOPEN,
		SC_DESIGNMIXCLOSE,
		SC_DESIGNMIXCOMPLETECLOSE,
		SC_DESIGNMIXCOMPLETE,

		SC_REMOVE_PREFIX,				//접미사? 정보 삭제

		SC_EXPANDSKILLPAGERESULT,		// 이중스킬트리 아이템 사용 결과
		SC_PETSKILLITEM,				// 펫 스킬아이템 사용 결과
		SC_PETSKILLEXPAND,				// 펫 스킬 확장 아이템 사용결과

		SC_WARP_VILLAGE_LIST,
		SC_WARP_VILLAGE,

		SC_RANDOMMIXOPEN,
		SC_RANDOMMIXCLOSE,
		SC_RANDOMMIXCOMPLETECLOSE,
		SC_RANDOMMIXCOMPLETE,

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		SC_EXCHANGE_POTENTIAL,			// 잠재이동 결과
#endif			//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

#if defined (PRE_ADD_BESTFRIEND)
		SC_BESTFRIENDREQUEST,
		SC_BESTFRIENDCOMPLETE,
#endif
#if defined(PRE_ADD_EXCHANGE_ENCHANT)
		SC_EXCHANGE_ENCHANT,			// 강화이동
#endif
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
		SC_PCCAFE_RENT_ITEM,			// PC방 렌탈 아이템..
#endif // #if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
		SC_POTENTIALJEWEL_ROLLBACK,			//잠재력 부여 취소
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	};
};

// NPC ------------------------------------------------------------------------------

namespace eNpc {
	// Main: CS_NPC
	enum eCSNpc
	{
		CS_NPCTALK = 1,
		CS_NPCTALKEND,
	};

	// Main: SC_NPC
	enum eSCNpc
	{
		SC_NPCTALK = 1,
		SC_SHOWWAREHOUSE,			// 창고창열기
		SC_OPENCOMPOUND_EMBLEM,		// 문장보옥 합성창 열기
		SC_OPENUPGRADE_JEWEL,		// 보옥 합성창 열기
		SC_OPENMAILBOX,				// 우편함 열기
		SC_OPENDISJOINT_ITEM,		// 분해창 열기
		SC_OPENUPGRADE_ITEM,		// 아이템 강화창 열기
		SC_OPENCOMPOUND_ITEM,		// 아이템 합성창 열기
		SC_OPENCASHSHOP,			// 캐쉬 아이템 상점 열기
		SC_OPENGUILDMGRBOX,			// 길드 관리 대화상자 열기
		SC_OPEN_DARKLAIRRANKBOARD,	// 다크레어 랭킹 게시판 열기
		SC_OPEN_PVPLADDERRANKBOARD,	// PvP래더 랭킹 게시판 열기
		SC_OPEN_FARMWAREHOUSE,		// 농장 창고 열기
		SC_OPEN_MOVIEBROWSER,
		SC_OPENCOMPOUND2_ITEM,		// 아이템 합성창2 열기
		SC_OPEN_BROWSER,
		SC_OPEN_GLYPH_LIFT,			// 문장 해제
		SC_OPEN_INVENTORY,			// 인벤창 열기
		SC_OPEN_EXCHANGE_ENCHANT,	// 강화이동 열기
#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
		SC_OPEN_TEXTURE_DIALOG,		// 텍스쳐 다이얼로그 열기
		SC_CLOSE_TEXTURE_DIALOG,	// 텍스쳐 다이얼로그 닫기
#endif
#if defined(PRE_ADD_DWC)
		SC_OPEN_DWCTEAMCREATE,		// DWC 팀생성 다이얼로그 열기
#endif
		SC_OPEN_CHANGEJOB_DIALOG,	// 텍스쳐 다이얼로그 닫기
	};
};

// QUEST ----------------------------------------------------------------------------

namespace eQuest {
	// Main: CS_QUEST
	enum eCSQuest
	{
		CS_ALL_QUEST_INFO = 1,
		CS_CANCEL_QUEST,			// 퀘스트 포기 요청
		CS_SELECT_QUEST_REWARD,		// 퀘스트 보상 요청하기
		CS_COMPLETE_CUTSCENE,
		CS_SKIP_CUTSCENE,
		CS_SHORTCUT_QUEST,			// 퀘스트 단축 처리
#if defined(PRE_ADD_REMOTE_QUEST)
		CS_ACCEPT_REMOTE_QEUST,
		CS_COMPLETE_REMOTE_QUEST,
		CS_CANCEL_REMOTE_QUEST,
#endif
	};

	// Main: SC_QUEST
	enum eSCQuest
	{
		// SC_ALL_QUEST_INFO = 1,
		SC_QUESTINFO,
		SC_QUESTCOMPLETEINFO,
		SC_REFRESH_QUEST,
		SC_COMPLETE_QUEST,
		SC_MARKING_COMPLETE_QUEST,
		SC_REMOVE_QUEST,
		SC_CANCEL_QUEST,
		SC_PLAYCUTSCENE,		// 동영상재생
		SC_QUEST_REWARD,		// 퀘스트 보상창 열기
		SC_QUESTRESULT,
		SC_COMPLETE_CUTSCENE,
		SC_SKIP_CUTSCENE,
		SC_SKIPALL_CUTSCENE,
		SC_ASSIGN_PERIODQUEST,	// 기간제 퀘스트 할당
		SC_SCORE_PERIODQUEST,	// 기간제 퀘스트 점수 확인
		SC_NOTICE_PERIODQUEST,	// 기간제 퀘스트 공지
#if defined(PRE_ADD_REMOTE_QUEST)
		SC_GAIN_REMOTE_QUEST,	// 원격 퀘스트 획득
		SC_REMOVE_REMOTE_QEUST,	// 원격 퀘스트 삭제
		SC_COMPLETE_REMOTE_QEUST,//원격 퀘스트 완료 통지
#endif

	};
};

// CHAT -----------------------------------------------------------------------------

namespace eChat 
{
	// CS_CHAT,				// 쳇~! 관련 뭐든것
	enum eCSChat
	{
		CS_CHATMSG = 1,				// 채팅
		CS_CHAT_PRIVATE,			// 귓속말
		CS_DICE,
		CS_CHAT_CHATROOM,			// 채팅방
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
		CS_CHAT_SPAMMER,			// 저는 스패머 입니다.
#endif
	};

	// SC_CHAT
	enum eSCChat
	{
		SC_CHATMSG = 1,				// 채팅관련
		SC_WORLDSYSTEMMSG = 2,		// 월드전체 시스템(?)메시지처리 (예: 강화x레벨 이상 성공, 미션 x번 성공 등...)
		SC_NOTICE,					// 채팅관련에서 뺍니다 항목이 늘어서 훔훔
		SC_GUILDCHAT,				// 길드채팅
		SC_CANCELNOTICE,			// 슬라이드공지 취소
		SC_SERVERMSG,				// 서버에서 보내는 메시지(UIString.xml, UIString_Server.xml 에 있는 메시지넘버를 전송)
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
		SC_DOORS_GUILDCHAT,				// 길드채팅
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	};
};

// ROOM -----------------------------------------------------------------------------

namespace eRoom {
	// CS_ROOM
	enum eCSRoom
	{
		CS_SYNC_WAIT = 1,
		CS_REQUEST_DUNGEONFAILED,
		CS_WARP_DUNGEON,			//던전이동시에 사용하던 패킷 이름이 실패더라~ 그냥 와아프로 변신
		CS_SELECT_REWARDITEM,
		CS_IDENTIFY_REWARDITEM,
		CS_DLCHALLENGE_RESPONSE,
	};

	// SC_ROOM
	enum eSCRoom
	{
		SC_SYNC_WAIT = 1,
		SC_SYNC_START,
		SC_GENERATION_MONSTER_MSG,
		SC_GATEINFO_MSG,
		SC_DUNGEONCLEAR_MSG,
		SC_CHANGE_GATESTATE_MSG,
		SC_GENERATION_PROP_MSG,
		SC_CHANGE_GAMESPEED,
		SC_DUNGEONHISTORY,
		SC_WARP_DUNGEONCLEAR, // 던전 클리어 후 모은 파티원들이 OK 를 누르면 방장에게 어디로 워프할지 요청
		SC_GATEEJECTINFO_MSG,
		//
		SC_SYNC_MEMBERINFO,
		SC_SYNC_MEMBERTEAM,
		SC_SYNC_MEMBERDEFAULTPARTS,
		SC_SYNC_MEMBEREQUIP,
		SC_SYNC_MEMBERSKILL,
		SC_SYNC_MEMBER_BREAKINTO,
		SC_SYNC_MEMBERHPSP,
		SC_SYNC_MEMBERBATTLEMODE,
		SC_SYNC_DROPITEMLIST,
		SC_DUNGEONFAILED_MSG,
		SC_OPENDUNGEONLEVEL_MSG,
		SC_SELECT_REWARDITEM,
		SC_DUNGEONCLEAR_SELECTREWARDITEM,
		SC_DUNGEONCLEAR_REWARDITEM,
		SC_DUNGEONCLEAR_REWARDITEMRESULT,
		SC_REFRESH_MEMBER,
		SC_SYNC_DATUMTICK,
		SC_DLDUNGEONCLEAR_MSG,
		SC_DLDUNGEONCLEAR_RANKINFO,
		SC_DUNGEONCLEAR_CANNOTWARP,
		SC_DUNGEONCLEAR_CANNOTWARP_QUEST,
		SC_DUNGEONCLEAR_ENABLE_LEADERWARP,
		SC_DUNGEONCLEAR_IDENTIFYREWARDITEM,
		//Room SeqLevel
		SC_ROOMSYNC_SEQLEVEL,
		SC_SYNC_CHATROOMINFO,
		SC_START_DRAGONNEST,
		SC_DLCHALLENGE_REQUEST,
		SC_DLCHALLENGE_RESPONSE,
		SC_SYNC_DUNGEONTIMEATTACK,
		SC_STOP_DUNGEONTIMEATTACK,
		SC_SPECIAL_REBIRTHITEM,
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		SC_DUNGEONCLEAR_REWARDBOXTYPE,
#endif
#if defined(PRE_MOD_DARKLAIR_RECONNECT)
		SC_DLROUND_INFO,
#endif
#if defined(PRE_ADD_CP_RANK)
		SC_ABYSS_STAGE_CLEAR_BEST,
#endif
	};
};

// SKILL ----------------------------------------------------------------------------

namespace eSkill{
	// CS_SKILL
	enum eCSSkill
	{
		CS_ADDSKILL,
		CS_DELSKILL,
		CS_SKILLLEVELUP_REQ,
		CS_ACQUIRESKILL_REQ,
		CS_UNLOCKSKILL_BY_MONEY_REQ,	// 마을에서 스킬 트레이너로 돈 내고 스킬 언락 요청시.
		CS_USE_SKILLRESET_CASHITEM_REQ,		// 스킬 리셋 아이템 최종 사용 요청
		CS_CHANGE_SKILLPAGE,			// 이중스킬트리 페이지 변경
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION)
		CS_RESERVATION_SKILL_LIST_REQ,		// 스킬 습득 및 레벨업
#endif
#ifdef PRE_ADD_PRESET_SKILLTREE
		CS_LIST_SKILLSET,					// 스킬셋 리스트
		CS_SAVE_SKILLSET,					// 스킬셋 저장
		CS_DELETE_SKILLSET,					// 스킬셋 삭제

#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE
	};

	// SC_SKILL
	enum eSCSkill
	{
		SC_SKILLLIST,
		SC_ADDSKILL,
		SC_DELSKILL,
		SC_SKILLLEVELUP,
		SC_OTHERPLAYERSKILLLEVELUP,
		SC_PUSHSKILLPOINT,	// setsp 치트키와 스킬 포인트 보정 처리에서 클라이언트에게 보내줄 때 사용함. #29463
		SC_UNLOCKSKILL,		// 스킬북 사용으로 스킬이 언락 됨.
		SC_ACQUIRESKILL,	// 서버쪽에서 스킬이 획득 허락 떨어짐.
		SC_SKILL_RESET,		// 스킬 리셋.
		SC_UNLOCKSKILL_BY_MONEY,	// 마을에서 스킬 트레이너로 돈 내고 스킬 언락 요청시 서버로부터 응답.
		SC_CANUSE_SKILLRESET_CASH_ITEM,	// 스킬리셋 아이템을 사용할 수 있는지 서버로 조회
		SC_USE_SKILLRESET_CASHITEM_RES,	// 스킬리셋 캐쉬 아이템 사용 결과 응답
		SC_SKILLPAGE_COUNT,				// 스킬트리 페이지 갯수
		SC_CHANGE_SKILLPAGE_RES,		// 이중스킬트리 페이지 변경 응답
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION)
		SC_RESERVATION_SKILL_LIST_ACK,		// 스킬 습득 및 레벨업
#endif
#ifdef PRE_ADD_PRESET_SKILLTREE
		SC_LIST_SKILLSET,					// 스킬셋 리스트
		SC_SAVE_SKILLSET,					// 스킬셋 저장
		SC_DELETE_SKILLSET,					// 스킬셋 삭제
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE
	};
};

// TRADE ----------------------------------------------------------------------------

namespace eTrade{
	// CS_TRADE
	enum eCSTrade
	{
		// Shop
		CS_SHOP_BUY,			// 구매
		CS_SHOP_SELL,			// 판매
		CS_SHOP_GETLIST_REPURCHASE,
		CS_SHOP_REPURCHASE,
		CS_REPAIR_EQUIP,		// 장착중수리
		CS_REPAIR_ALL,			// 전체수리

		// 우편
		CS_MAILBOX,				// 우편리스트 요청
		CS_MAIL_SEND,			// 우편 보내기
		CS_MAIL_READ,			// 우편 읽기
		CS_MAIL_DELETE,			// 우편 삭제
		CS_MAIL_ATTACH,			// 우편에 첨부된 돈, 아이템 찾기
		CS_MAIL_ATTACHALL,		// 우편에 첨부된 돈, 아이템 찾기
		CS_MAIL_READCADGE,		// 조르기우편 읽기

		// 개인거래 (물건 교환)
		CS_EXCHANGE_REQUEST,	// 거래 요청
		CS_EXCHANGE_ACCEPT,		// 거래 수락
		CS_EXCHANGE_ADDITEM,	// 아이템 등록
		CS_EXCHANGE_DELETEITEM,	// 아이템 등록해제
		CS_EXCHANGE_ADDCOIN,	// 코인 등록
		CS_EXCHANGE_CONFIRM,	// 확인
		CS_EXCHANGE_CANCEL,		// 취소

		// 무인상점
		CS_MARKETLIST,				// 마켓리스트 요청
		CS_MYMARKETLIST,			// 내가 올려놓은 물건 리스트 요청
		CS_MARKET_REGISTER,			// 마켓 아이템 등록
		CS_MARKET_INTERRUPT,		// 마켓 등록 중단
		CS_MARKET_BUY,				// 마켓 구매
		CS_MARKET_CALCULATIONLIST,	// 마켓정산리스트
		CS_MARKET_CALCULATION,		// 마켓정산 (1개)
		CS_MARKET_CALCULATIONALL,	// 마켓모두정산
		CS_MARKET_PETALBALANCE,		// 페탈 조회
		CS_MARKET_PRICE,			// 시세 조회
		CS_MARKETMINILIST,			// 특정아이템 거래소에 요청 (미니거래소)

		// specialbox
		CS_SPECIALBOX_LIST,			// 특수보관함 요청
		CS_SPECIALBOX_ITEMLIST,		// 선택/전부받기 버튼 클릭 (받을 아이템리스트 요청)
		CS_SPECIALBOX_RECEIVEITEM,	// 아이템 받기

		CS_SHOP_REMOTEOPEN,
	};

	// SC_TRADE
	enum eSCTrade
	{
		// Shop
		SC_SHOP_OPEN,			// 샵 열기
		SC_SHOP_BUY,			// 구매
		SC_SHOP_SELL,			// 판매
		SC_SHOP_REBUY,			// 재매입
		SC_SHOP_REPAIR,			// 수리
		SC_SHOP_GETLIST_REPURCHASE,
		SC_SHOP_REPURCHASE,

		// SkillShop
		SC_SKILLSHOP_OPEN,		// 돈으로 언락하는 트레이너 npc 에게 말 걸어서 스킬창 띄우도록 허락됨.
		//SC_SKILLSHOPLIST,		// 리스트 보내주기
		//SC_SKILLSHOP_BUY,		// 구매

		// 우편
		SC_MAILBOX,				// 우편리스트 보내주기
		SC_MAIL_SEND,			// 우편 보내기
		SC_MAIL_READ,			// 우편 읽기
		SC_MAIL_DELETE,			// 우편 삭제
		SC_MAIL_ATTACH,			// 우편에 첨부된 돈, 아이템 찾기
		SC_MAIL_ATTACHALL,		// 우편에 첨부된 돈, 아이템 찾기
		SC_MAIL_NOTIFY,			// 우편 왔다고 통보하기
		SC_MAIL_READCADGE,		// 조르기우편 읽기

		// 개인거래 (물건 교환)
		SC_EXCHANGE_REQUEST,	// 거래 요청
		SC_EXCHANGE_REJECT,		// 거래 거부
		SC_EXCHANGE_START,		// 거래 시작
		SC_EXCHANGE_ADDITEM,	// 아이템 등록
		SC_EXCHANGE_DELETEITEM,	// 아이템 등록해제
		SC_EXCHANGE_ADDCOIN,	// 코인 등록
		SC_EXCHANGE_CONFIRM,	// 확인
		SC_EXCHANGE_CANCEL,		// 취소
		SC_EXCHANGE_COMPLETE,	// 완료

		// 무인상점
		SC_MARKETLIST,			// 마켓리스트 보내주기
		SC_MYMARKETLIST,		// 내 마켓리스트 보내주기
		SC_MARKET_REGISTER,		// 아이템 등록 결과값
		SC_MARKET_INTERRUPT,	// 마켓 등록 중단
		SC_MARKET_BUY,			// 마켓 구매
		SC_MARKET_CALCULATIONLIST,	// 마켓정산리스트
		SC_MARKET_CALCULATION,		// 마켓정산 (1개)
		SC_MARKET_CALCULATIONALL,	// 마켓모두정산
		SC_MARKET_NOTIFY,		// 마켓 관련 통보
		SC_MARKET_PETALBALANCE,		// 페탈 통보
		SC_MARKET_PRICE,			// 시세 통보
		SC_MARKETMINILIST,			// 특정아이템 거래소에 요청 (미니거래소)

		// specialbox
		SC_SPECIALBOX_LIST,			// 특수보관함 요청
		SC_SPECIALBOX_ITEMLIST,		// 선택/전부받기 버튼 클릭 (받을 아이템리스트 요청)
		SC_SPECIALBOX_RECEIVEITEM,	// 아이템 받기
		SC_SPECIALBOX_NOTIFY,		// 보관함 알림이
	};
};

// FRIEND 후렌드 ---------------------------------------------------------------------
namespace eFriend
{
	enum eCSFriend
	{
		CS_FRIEND_GROUP_ADD,			//친구 그룹 생성
		CS_FRIEND_GROUP_DELETE,			//친구 그룹 삭제
		CS_FRIEND_GROUP_STATEUPDATE,	//친구 그룹 상태변경(지금은 아마도 그냥 이름 변경)
		CS_FRIEND_ADD,					//친구추가
		CS_FRIEND_DELETE,				//친구삭제
		CS_FRIEND_STATEUPDATE,			//친구 상태변경(지금은 아마도 그룹변경)
		CS_FRIEND_INFO,					//선택한 친구의 세부 정보 요청
		CS_FRIEND_REQ_LIST,				//친구리스트요청(그룹포함)	(최초한번만 확장하면 관련부분 확인요망)
		CS_FRIEND_REQ_LOCATION,			//친구위치 업데이트요청
	};

	enum eSCFriend
	{
		SC_FRIEND_GROUP_LIST,			//그룹리슷흐 먼저 나갑니다.
		SC_FRIEND_LIST,					//클라이언트의 요청시점으로 변경
		SC_FRIEND_GROUP_ADDED,
		SC_FRIEND_ADDED,
		SC_FRIEND_GROUP_DELETED,
		SC_FRIEND_ADDED_DELETED,
		SC_FRIEND_GROUP_UPDATED,
		SC_FRIEND_UPDATED,
		SC_FRIEND_INFO,					//친구의 세부정보오~
		SC_FRIEND_RESULT,
		SC_FRIEND_ADDNOTICE,
		SC_FRIEND_LOCATIONLIST,
	};
};


// ISOLATE -----------------------------------------------------------------------------
namespace eIsolate
{
	enum eCSIsolate
	{
		CS_ISOLATE_GET,
		CS_ISOLATE_ADD,
		CS_ISOLATE_DELETE,
	};

	enum eSCIsolate
	{
		SC_ISOLATE_LIST,
		SC_ISOLATE_ADD,
		SC_ISOLATE_DELETE,
		SC_ISOLATE_RESULT,	//message
	};
};

// GUILD -----------------------------------------------------------------------------
namespace eGuild
{
	enum eCSGuild			// CS_GUILD
	{
		CS_CREATEGUILD,				// 길드 창설 요청
		CS_DISMISSGUILD,			// 길드 해체 요청
		CS_INVITEGUILDMEMBREQ,		// 길드원 초대 요청
		CS_INVITEGUILDMEMBACK,		// 길드원 초대 응답
		CS_LEAVEGUILDMEMB,			// 길드원 탈퇴 요청
		CS_EXILEGUILDMEMB,			// 길드원 추방 요청
		CS_CHANGEGUILDINFO,			// 길드 정보 변경 요청
		CS_CHANGEGUILDMEMBINFO,		// 길드원 정보 변경 요청
		CS_GETGUILDHISTORYLIST,		// 길드 히스토리 목록 요청
		CS_GETGUILDINFO,			// 길드 정보 요청
		CS_OPEN_GUILDWARE,			// 길드창고 오픈
		CS_GET_GUILDWARE_HISTORY,	// 길드창고 히스토리
		CS_CLOSE_GUILDWARE,			// 길드창고 닫기
		CS_ENROLL_GUILDWAR,			// 길드전 참가신청
		CS_GUILDWAR_VOTE,			// 길드전 인기 투표
		CS_GUILDWAR_COMPENSATION,	// 길드전 예선 보상받기
		CS_GUILDWAR_WIN_SKILL,		// 길드전 우승스킬 사용(시전)
		CS_GUILD_BUY_REWARDITEM,		// 길드보상아이템 구입
		CS_PLAYER_REQUEST_GUILDINFO,	// 살펴보기용 길드인포
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
		CS_GUILD_CONTRIBUTION_RANK,	//길드 공헌도 랭킹 조회 요청
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
	};
	enum eSCGuild			// SC_GUILD
	{
		SC_CREATEGUILD,				// 길드 창설 결과
		SC_DISMISSGUILD,			// 길드 해체 결과
		SC_INVITEGUILDMEMBREQ,		// 길드원 초대 요청
		SC_INVITEGUILDMEMBACK,		// 길드원 초대 결과
		SC_LEAVEGUILDMEMB,			// 길드원 탈퇴 결과
		SC_EXILEGUILDMEMB,			// 길드원 추방 결과
		SC_CHANGEGUILDINFO,			// 길드 정보 변경 결과
		SC_CHANGEGUILDMEMBINFO,		// 길드원 정보 변경 결과
		SC_GETGUILDHISTORYLIST,		// 길드 히스토리 목록 응답
		SC_GETGUILDINFO,			// 길드 정보 응답
		SC_CHANGEGUILDSELFVIEW,		// 길드 시각정보 (개인/단체) 변경 알림
		SC_GUILDMEMBLOGINLIST,		// 길드원 로그인 목록 알림
		SC_OPEN_GUILDWARE,			// 길드창고 오픈 결과
		SC_GET_GUILDWARE_HISTORY,	// 길드창고 히스토리 결과
		SC_CHANGEGUILDNAME,			// 길드이름 변경
		SC_EXTEND_GUILDWARESIZE,	// 길드창고 사이즈 확장
		SC_CHANGEGUILDMARK,			// 길드마크 변경
		SC_CHANGE_WAREVENT,			// 길드전 이벤트 알림
		SC_ENROLL_GUILDWAR,			// 길드전 참가신청 결과		
		// 데이터를 서버에서 받아 창을 연다
		SC_GUILDWAR_STATUS_OPEN,			// 길드전 예선 진행현황
		SC_GUILDWAR_TRIAL_RESULT_OPEN,		// 길드전 예선 결과
		SC_GUILDWAR_TOURNAMENT_INFO_OPEN,	// 길드전 토너먼트(대진표) 정보
		SC_GUILDWAR_VOTE,					// 길드전 인기 투표 결과		
		SC_GUILDWAR_COMPENSATION,			// 길드전 예선 보상받기 결과
		SC_GUILDWAR_FESTIVAL_POINT,			// 길드전 축제 포인트
		SC_GUILDWAR_VOTE_TOP,				// 지난 길드전 인기 투표 1위
		SC_GUILDWAR_PRE_WIN,				// 지난 길드전 우승길드임(true, false 있음)
		SC_GUILDWAR_WIN_SKILL,				// 길드전 우승스킬 사용 결과(시전자용 결과)
		SC_GUILDWAR_USER_WINSKILL,			// 우승길드에서 우승스킬을 사용함(주변인용)
		SC_GUILDWAR_EVENT_TIME,				// 길드전 일정 알림.
		SC_GUILDWAR_TOURNAMENT_WIN,			// 길드전 본선 승리 길드 알림.
		SC_GUILDWAR_TRIAL_RESULT_UIOPEN,		// 길드전 예선 결과
		SC_GUILDWAR_TOURNAMENT_INFO_UIOPEN,	// 길드전 토너먼트(대진표) 정보
		SC_GUILDWAR_BUYED_ITEM_COUNT,		// 길드전 보상기간때 구입한 아이템 갯수.
		SC_GUILDWAR_BUYED_ITEM_COUNT_LIST,	// 길드전 보상기간때 구입한 아이템 갯수 리스트.
		SC_GUILDLEVELUP,			// 길드레벨업
		SC_UPDATEGUILDEXP,			// 길드 업데이트 경험치
		SC_GUILD_GET_REWARDITEM,	// 길드보상아이템 적용 목록 전송
		SC_GUILD_BUY_REWARDITEM,	// 길드보상아이템 구입		
		SC_GUILD_ADD_REWARDITEM,	// 길드보상아이템 효과 추가		
		SC_GUILD_EXTEND_GUILDSIZE,	// 길드	최대 인원수 증가		
		SC_PLAYER_REQUEST_GUILDINFO,	// 살펴보기용 길드인포
		SC_GET_GUILDMEMBER,			// 길드 멤버 정보전송
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
		SC_GUILD_CONTRIBUTION_POINT, //길드 공헌도(개인)
		SC_GUILD_CONTRIBUTION_RANK,	//길드 공헌도 랭킹 전송
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
	};
};

// PvP --------------------------------------------------------------------------
namespace ePvP
{
	enum eCSPvP
	{
		CS_CREATEROOM,				// PvP 방 생성
		CS_MODIFYROOM,				// PvP 방 수정
		CS_LEAVEROOM,				// PvP 방 나가기
		CS_CHANGECAPTAIN,			// PVP 방장 변경
		CS_ROOMLIST,				// PVP 방 리스트
		CS_JOINROOM,				// PVP 방 들어가기
		CS_READY,					// PVP 방 레디
		CS_START,					// PVP 방 시작
		CS_RANDOMJOINROOM,			// PvP 방 랜덤 들어가기
		CS_CHANGETEAM,				// PvP 팀 변경
		CS_BAN,						// PvP 방 추방
		CS_WAITUSERLIST,			// PvP 대기실 유저 리스트

		// 래더시스템 관련 프로토콜
		CS_ENTER_LADDERCHANNEL,
		CS_LEAVE_LADDERCHANNEL,
		CS_LADDER_MATCHING,
		CS_LADDER_PLAYING_ROOMLIST,
		CS_LADDER_OBSERVER,
		CS_LADDER_INVITE,
		CS_LADDER_INVITE_CONFIRM,
		CS_LADDER_KICKOUT,
		CS_FATIGUE_OPTION,			// PvP 피로도 소모 On,Off
		CS_SWAPMEMBERINDEX,
		CS_GUILDWAR_CHANGEMEMBER_GRADE,
		CS_TRYCAPTURE,				// 점령전 점령지획득시도
		CS_CANCELCAPTURE,			// 시도취소
		CS_TRYACQUIRE_SKILL,		// 점령전 스킬 획득시도
		CS_INITSKILL,				// 점령전 스킬 초기화
		CS_USESKILL,				// 점령전 스킬 사용
		CS_SWAPSKILLINDEX,			// 점령전 스킬 슬롯위치변경
		CS_CONCENTRATE_ORDER,		// 길드장 집결명령
#if defined(PRE_ADD_QUICK_PVP)
		CS_QUICKPVP_INVITE,			// 결투신청 요청		
		CS_QUICKPVP_RESULT,			// 결투신청 응답
#endif
		CS_PVP_ALLKILL_SELECTPLAYER,		
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		CS_PVP_LIST_OPEN_UI,		// PVP 방 리스트 오픈 요청(레벨체크)
#endif
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		CS_PVP_CHANGECHANNEL,
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined(PRE_ADD_PVP_TOURNAMENT)
		CS_PVP_SWAP_TOURNAMENT_INDEX,	// PvP 토너먼트 자리 바꾸기 (Random일때는 불가)
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
		CS_PVP_WORLDPVPROOM_STARTMSG,	// 월드통합전장 시작메시지
#endif
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
		CS_PVP_COMBOEXERCISE_RECALLMONSTER,	// 몬스터 소환
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
	};

	enum eSCPvP
	{
		SC_CREATEROOM,				// PvP 방 생성
		SC_MODIFYROOM,				// PvP 방 수정
		SC_LEAVEROOM,				// PVP 방 나가기
		SC_ROOMLIST,				// PVP 방 리스트
		SC_JOINROOM,				// PVP 방 들어가기
		SC_JOINUSER,				// PVP 방 유저 들어옴( 이미 방에 있는 유저에게 JOIN유저 정보 보냄 )
		SC_CHANGETEAM,				// PvP 팀 변경
		SC_USERSTATE,				// 유저 상태
		SC_START,					// PvP 시작
		SC_STARTMSG,				// PvP 시작 메세지
		SC_ROOMSTATE,				// 방 상태
		SC_MODE_STARTTICK,			// PvPMode 시작틱
		SC_FINISH_PVPMODE,			// PvP 종료
		SC_START_PVPROUND,			// PvP 라운드 시작
		SC_FINISH_PVPROUND,			// PvP 라운드 종료
		SC_MODE_SCORE,				// PvPMode 스코어
		SC_RESPAWN_POINT,			// PvP 리스폰 포인트
		SC_XPSCORE,					// PvP XP스코어
		SC_BREAKINTO_SUCCESS,		// 난입성공
		SC_ROOMINFO,
		SC_ADDPOINT,
		SC_LEVEL,					// PvP 등급
		SC_SELECTCAPTAIN,			// PvP 대장전 모드에서 대장 선택 됨
		SC_CAPTAINKILL_COUNT,		// 대장킬 정보 보냄
		SC_WAITUSERLIST,				// PvP 유저 리스트
		SC_SELECTZOMBIE,			// PvP 좀비전 모드에서 좀비 선택 됨
		SC_ZOMBIEKILL_COUNT,		// 좀비킬 정보 보냄
		SC_OCCUPATIONSCORE,

		// 래더시스템 관련 프로토콜
		SC_ENTER_LADDERCHANNEL,
		SC_LEAVE_LADDERCHANNEL,
		SC_LADDER_NOTIFY_LEAVEUSER,
		SC_LADDER_NOTIFY_LEADER,
		SC_LADDER_NOTIFY_ROOMSTATE,
		SC_LADDER_MATCHING,
		SC_LADDER_NOTIFY_GAMEMODE_TABLEID,
		SC_LADDER_PLAYING_ROOMLIST,
		SC_LADDER_OBSERVER,
		SC_LADDER_SCOREINFO,
		SC_LADDER_SCOREINFO_BYJOB,
		SC_LADDER_MATCHING_AVGSEC,
		SC_LADDER_POINT_REFRESH,
		SC_LADDER_INVITE,
		SC_LADDER_INVITE_CONFIRM_REQ,
		SC_LADDER_INVITE_CONFIRM,
		SC_LADDER_NOTIFY_JOINUSER,
		SC_LADDER_REFRESH_USERINFO,
		SC_LADDER_KICKOUT,
		SC_FATIGUE_REWARD,			// PvP 피로도 소모를 통한 보상
		SC_PVP_MEMBERINDEX,
		SC_PVP_MEMBERGRADE,
		SC_PVP_OCCUPATION_MODESTATE,
		SC_PVP_OCCUPATION_POINTSTATE,
		SC_PVP_OCCUPATION_TEAMSTATE,
		SC_PVP_OCCUPATION_SKILLSTATE,
		SC_PVP_TRYACQUIREPOINT,
		SC_PVP_TRYACQUIRESKILL,
		SC_PVP_INITSKILL,
		SC_PVP_USESKILL,
		SC_PVP_SWAPSKILLINDEX,
		SC_CONCENTRATE_ORDER,
		SC_PVP_GHOULSCORES,
		SC_PVP_HOLYWATER_KILLCOUNT, //성수사용으로 KILL 카운트 추가.
		SC_PVP_CLIMAXMODE,

#if defined(PRE_ADD_QUICK_PVP)
		SC_QUICKPVP_INVITE,			// 결투신청 요청		
		SC_QUICKPVP_RESULT,			// 결투신청 응답
#endif
		SC_PVP_ALLKILL_SHOW_SELECTPLAYER,
		SC_PVP_ALLKILL_GROUPCAPTAIN,
		SC_PVP_ALLKILL_ACTIVEPLAYER,
		SC_PVP_ALLKILL_CONTINUOUSWIN,
		SC_PVP_ALLKILL_FINISH_DETAILREASON,
		SC_PVP_ALLKILL_BATTLEPLAYER,
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		SC_PVP_LIST_OPEN_UI,		// PVP 방 리스트 오픈 요청(레벨체크)
#endif
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		SC_PVP_CHANGE_CHANNEL,
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined(PRE_ADD_RACING_MODE)
		SC_PVP_RACING_RAPTIME,					// PvP Racing 랩타임
		SC_PVP_RACING_FIRST_END,				// PvP 1위가 들어왔어요.
#endif
#if defined(PRE_ADD_REVENGE)
		SC_PVP_SET_REVENGE_TARGET,
		SC_PVP_SUCCESS_REVENGE,
#endif
#if defined(PRE_ADD_PVP_TOURNAMENT)
		SC_PVP_SWAP_TOURNAMENT_INDEX,	// PvP 토너먼트 자리 바꾸기 (Random일때는 불가)
		SC_PVP_TOURNAMENT_MATCHLIST,	// PvP 토너먼트 대진표 정보
		SC_PVP_TOURNAMENT_DEFAULTWIN,	// PvP 토너먼트 부전승
		SC_PVP_TOURNAMENT_IDLE_TICK,	// PvP 토너먼트 대기 틱
		SC_PVP_TOURNAMENT_TOP4,			// PvP 토너먼트 4위까지.
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
		SC_PVP_WORLDPVPROOM_STARTMSG,	// 월드통합전장 스타트메시지
		SC_PVP_WORLDPVPROOM_START_RESULT,	// 월드통합전장 스타트 결과값
		SC_PVP_WORLDPVPROOM_JOIN_RESULT,	// 월드통합전장 조인 결과(실패일경우 전송)
		SC_PVP_WORLDPVPROOM_ALLKILLTEAMINFO,	// 월드통합전장 올킬전 팀정보
		SC_PVP_WORLDPVPROOM_TOURNAMENTUSERINFO,	// 월드통합전장 토너먼트유저정보
#endif
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
		SC_PVP_COMBOEXERCISE_ROOMMASTERINFO, // 방장 정보(시작할때)
		SC_PVP_COMBOEXERCISE_CHANGEROOMMASTER, // 방장 교체
		SC_PVP_COMBOEXERCISE_RECALLMONSTERRESULT,	// 몬스터 소환 결과
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
	};
};

// Trigger --------------------------------------------------------------------------
namespace eTrigger
{
	enum eCSTrigger
	{
	};
	enum eSCTrigger
	{
		SC_TRIGGER_CALLACTION,
		SC_CHANGE_MY_BGM,
		SC_RADIO_IMAGE,
		SC_FILE_TABLE_BGM_OFF,
		SC_TRIGGER_FORCE_ENABLE_RIDE,
	};
};

//-----------------------------------------------------------------------------------

// Mission --------------------------------------------------------------------------
namespace eMission
{
	enum eCSMission
	{
#if defined(PRE_ADD_CHAT_MISSION)
		CS_CHAT_MISSION,			// 채팅미션 완료 패킷
#endif
	};

	enum eSCMission
	{
		SC_MISSION_LIST,
		SC_GAIN_MISSION,
		SC_ACHIEVE_MISSION,

		SC_DAILYMISSION_LIST,
		SC_COUNT_DAILYMISSION,
		SC_ACHIEVE_DAILYMISSION,

		SC_ALARM_DAILYMISSION,
		SC_EVENT_POPUP,
		SC_CONNECTING_TIME,
		SC_HELP_ALARM,
#if defined(PRE_ADD_ACTIVEMISSION)
		SC_GAIN_ACTIVE_MISSION,
		SC_ACHIEVE_ACTIVE_MISSION,
#endif
	};
};
//-----------------------------------------------------------------------------------


// Appellation ----------------------------------------------------------------------
namespace eAppellation
{
	enum eCSAppellation
	{
		CS_SELECT_APPELLATION,
		CS_COLLECTIONBOOK,
	};
	enum eSCAppellation
	{
		SC_APPELLATION_LIST = 128,
		SC_GAIN_APPELLATION,
		SC_SELECT_APPELLATION, 	// 로컬 호칭 선택
		SC_CHANGE_APPELLATION,	// 다른 플레이어의 호칭이 바꼇을때
		SC_PERIOD_APPELLATION,	// 기간제 칭호 시간
		SC_RESET_PCBANGAPPELLATION,	//피씨방 칭호 초기화 알림
	};
};

//-----------------------------------------------------------------------------------

// GameOption -----------------------------------------------------------------------
// CS_GAMEOPTION

namespace eGameOption
{
	enum eCSGameOption
	{
		CS_GAMEOPTION_UPDATEOPTION,
		CS_GAMEOPTION_REQCOMMOPTION,
		CS_GAMEOPTION_QUEST_NOTIFIER,
		CS_GAMEOPTION_MISSION_NOTIFIER,
		CS_GAMEOPTION_UPDATE_KEYSETTING,
		CS_UPDATE_SECONDAUTH_PASSWORD,
		CS_UPDATE_SECONDAUTH_LOCK,
		CS_GAMEOPTION_VALIDATE_SECONDAUTH,
		CS_INIT_SECONDAUTH,
		CS_GAMEOPTION_UPDATE_PADSETTING,
		CS_GAMEOPTION_GET_PROFILE,
		CS_GAMEOPTION_SET_PROFILE,
	};

	enum eSCGameOption
	{
		SC_GAMEOPTION_USERCOMMOPTION,
		SC_GAMEOPTION_REFRESH_NOTIFIER,
		SC_GAMEOPTION_SELECT_KEYSETTING,
		SC_NOTIFY_SECONDAUTHINFO,
		SC_UPDATE_SECONDAUTH_PASSWORD,
		SC_UPDATE_SECONDAUTH_LOCK,
		SC_NOTIFY_SECONDAUTH_RESTRAINT,			// 잠금상태여서 제한 상태임 알림.
		SC_GAMEOPTION_VALIDATE_SECONDAUTH,
		SC_INIT_SECONDAUTH,
		SC_GAMEOPTION_SELECT_PADSETTING,
		SC_GAMEOPTION_GET_PROFILE,
		SC_GAMEOPTION_SET_PROFILE,
		SC_GAMEOPTION_DISPLAY_PROFILE,
	};
};

// RadioMessage ---------------------------------------------------------------------
// CS_RADIO
namespace eRadio
{
	enum eCSRadio
	{
		CS_USERADIO,
	};

	enum eSCRadio
	{
		SC_USERADIO,
	};
};

//Gesture ---------------------------------------------------------------------------
namespace eGesture
{
	enum eCSGesture
	{
		CS_USEGESTURE,
	};

	enum eSCGesture
	{
		SC_CASHGESTURELIST,
		SC_CASHGESTUREADD,
		SC_USEGESTURE,
		SC_EFFECTITEMGESTURELIST,
		SC_EFFECTITEMGESTUREADD,
	};
};

//VoiceChat -------------------------------------------------------------------------
namespace eVoiceChat
{	//CS_VOICECHAT
	enum eCSVoiceChat
	{
		CS_VOICEAVAILABLE,		//가능여부
		CS_VOICEMUTE,
		CS_VOICECOMPLAINTREQ,
	};

	enum eSCVoiceChat
	{
		SC_VOICECHATINFO,					// dolbyserver
		SC_TALKINGINFO,
		SC_VOICEMEMBERINFO,
	};
};

//Restraint -------------------------------------------------------------------------
namespace eRestraint
{
	enum eCSRestraint
	{
	};

	enum eSCRestraint
	{
		SC_RESTRAINTADD,
		SC_RESTRAINTDEL,
		SC_RESTRAINTRETCODE,
		SC_RESTRAINTLIST,
	};
};

// CashShop -------------------------------------------------------------------------
namespace eCashShop
{
	// CS_CASHSHOP
	enum eCSCashShop
	{
		CS_SHOPOPEN,		// 샵 열기
		CS_SHOPCLOSE,		// 샵 닫기
		CS_BUY,				// 구매하기
		CS_PACKAGEBUY,		// 패키지 아이템 구매하기
		CS_BALANCEINQUIRY,	// cash, petal 잔액조회
		CS_CHECKRECEIVER,	// 받는이 체크 (선물할때 필요)
		CS_GIFT,			// 선물하기
		CS_PACKAGEGIFT,		// 패키지 선물하기
		CS_GIFTLIST,		// 선물함 (받은선물) 요청
		CS_RECEIVEGIFT,		// 선물 받기
		CS_RECEIVEGIFTALL,	// 선물 받기

		CS_COUPON,			// 쿠폰입력
		CS_VIPBUY,							// VIP상품구매
		CS_VIPGIFT,						// VIP상품구매

		CS_GIFT_RETURN,		// 선물 반송(대만)

#if defined(PRE_ADD_CASH_REFUND)
		CS_MOVE_CASHINVEN,	// 캐쉬 인벤으로 옮기기 요청
		CS_CASH_REFUND,		// 캐쉬 환불 요청
#endif
		CS_CHARGE_TIME,
		CS_EQUIPCRC,
#ifdef PRE_ADD_LIMITED_CASHITEM
		CS_LIMITEDCASHITEM,	// 갯수한정판캐쉬아이템 정보
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
		CS_CADGE,			// 조르기
		CS_GIFTLIST_BY_SHORTCUT,		// 선물함 (받은선물) 요청(선물함 표시 아이콘 클릭)
	};

	// SC_CASHSHOP
	enum eSCCashShop
	{
		SC_SHOPOPEN,		// 샵 열기
		SC_SHOPCLOSE,		// 샵 닫기
		SC_BUY,				// 구매하기
		SC_PACKAGEBUY,		// 패키지 아이템 구매하기
		SC_BALANCEINQUIRY,	// cash, petal 잔액조회
		SC_CHECKRECEIVER,	// 받는이 체크 (선물할때 필요)
		SC_GIFT,			// 선물하기
		SC_PACKAGEGIFT,		// 패키지 선물하기
		SC_GIFTLIST,		// 선물함 (받은선물)
		SC_RECEIVEGIFT,		// 선물 받기
		SC_RECEIVEGIFTALL,		// 선물 받기

		SC_COUPON,			// 쿠폰입력
		SC_NOTIFYGIFT,		// 선물 통보

		SC_VIPBUY,							// vip 정보날려주기
		SC_VIPGIFT,							// vip 정보날려주기
		SC_VIPINFO,							// 선물함에서 vip찾을때

		SC_SALEABORTLIST,	// 판매중지목록
		SC_GIFTRETURN,		// 선물반송(대만)

#if defined(PRE_ADD_CASH_REFUND)
		SC_PAYMENT_LIST,			// 단품 결재리스트
		SC_PAYMENT_PACKAGELIST,		// 패키지 결재리스트
		SC_MOVE_CASHINVEN,			// 캐쉬 인벤으로 옮기기 결과
		SC_CASH_REFUND,				// 캐쉬 환불 결과	
#endif
		SC_CHARGE_TIME,
		SC_EQUIPCRC,
#ifdef PRE_ADD_LIMITED_CASHITEM
		SC_LIMITEDCASHITEM,			// 갯수한정판캐쉬아이템 정보
		SC_CHANGEDLIMITEDCASHITEM,	// 갯수한정변경
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
		SC_CADGE,			// 조르기
		SC_GIFTLIST_BY_SHORTCUT,		// 선물함 (받은선물) 요청(선물함 표시 아이콘 클릭)(서버에서는 기존의 sC_GIFTLIST로 패킷 보냄..)
	};
};

// ChatRoom -------------------------------------------------------------------------
namespace eChatRoom
{
	enum eCSChatRooom
	{
		CS_CREATECHATROOM,			// 채팅방 생성
		CS_ENTERCHATROOM,			// 채팅방 입장
		CS_CHANGEROOMOPTION,		// 채팅방 옵션 변경
		CS_KICKUSER,				// 유저 강퇴
		CS_LEAVEROOM,				// 채팅방에서 나간다
	};

	enum eSCChatRoom
	{
		SC_CREATECHATROOM,			// 채팅방 생성
		SC_ENTERCHATROOM,			// 채팅방 입장
		SC_ENTERUSERCHATROOM,		// 채팅방에 다른 유저가 입장
		SC_CHANGEROOMOPTION,		// 채팅방 옵션 변경
		SC_KICKUSER,				// 유저 강퇴
		SC_LEAVEUSER,				// 채팅방에서 유저가 나감 (나감, 강퇴 등)

		SC_CHATROOMVIEW,			// 채팅방 생성/삭제 (ChatRoomID == 0 이면 삭제)
	};
}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

namespace eReputation
{
	enum eCSReputation
	{
		CS_GIVENPCPRESENT,
	};

	enum eSCReputation
	{
		SC_REPUTATIONLIST,
		SC_MODREPUTATION,
		SC_OPENGIVENPCPRESENT,
		SC_REQUESTSENDSELECTEDPRESENT,
		SC_SHOWNPCEFFECT,
		SC_UNIONPOINT,
		SC_UNIONMEMBERSHIPLIST,
		SC_BUYUNIONMEMBERSHIPRESULT,
#if defined( PRE_ADD_NEW_MONEY_SEED )
		SC_SEEDPOINT,
#endif
	};
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

namespace eMasterSystem
{
	enum eCSMasterSystem
	{
		CS_MASTERLIST,			// 스승 목록 창 정보
		CS_MASTERCHARACTERINFO,	// 스승정보
		CS_INTRODUCTION_ONOFF,	// 소개 등록/해제
		CS_MASTER_APPLICATION,	// 스승 신청
		CS_JOIN,				// 사제 참여
		CS_MASTERANDCLASSMATE,	// 스승,동문 목록
		CS_CLASSMATEINFO,		// 동문정보
		CS_MYMASTERINFO,		// 내 스승 정보( 제자 )
		CS_LEAVE,				// 사제 관계 끊기
		CS_INVITE_PUPIL,		// 제자 초대
		CS_INVITE_PUPIL_CONFIRM,// 제자 초대 응답
		CS_JOIN_DIRECT,			// 스승 참여
		CS_JOIN_DIRECT_CONFIRM,
		CS_RECALL_MASTER,		// 스승소환
		CS_BREAKINTO_PUPIL,		// 제자채널로이동
		CS_JOIN_CONFIRM,		// 사제 참여 수락 여부
	};

	enum eSCMasterSystem
	{
		SC_SIMPLEINFO,			// 간략정보 
		SC_MASTERLIST,			// 스승 목록 페이지
		SC_MASTERCHARACTERINFO,	// 스승정보
		SC_PUPILLIST,			// 제자 리스트
		SC_INTRODUCTION_ONOFF,	// 소개 등록/해제
		SC_MASTER_APPLICATION,	// 스승 신청
		SC_JOIN,				// 사제 참여
		SC_MASTERANDCLASSMATE,	// 스승,동문 목록
		SC_CLASSMATEINFO,		// 동문 정보
		SC_MYMASTERINFO,		// 내 스승 정보( 제자 )	
		SC_LEAVE,				// 사제 관계 끊기
		SC_GRADUATE,			// 졸업
		SC_COUNTINFO,			// 
		SC_RESPECTPOINT,
		SC_FAVORPOINT,
		SC_INVITE_PUPIL,
		SC_INVITE_PUPIL_CONFIRM,// 제자 초대 확인
		SC_JOIN_DIRECT,
		SC_JOIN_DIRECT_CONFIRM,
		SC_RECALL_MASTER,
		SC_CONNECT,
		SC_JOIN_COMFIRM,		// 사제 참여 수락여부
	};
}

#if defined( PRE_ADD_SECONDARY_SKILL )

namespace eSecondarySkill
{
	enum eCSSecondarySkill
	{
		CS_DELETE,
		CS_ADD_RECIPE,
		CS_DELETE_RECIPE,
		CS_EXTRACT_RECIPE,
		CS_MANUFACTURE,
		CS_CANCEL_MANUFACTURE,
	};

	enum eSCSecondarySkill
	{
		SC_CREATE,
		SC_DELETE,
		SC_UPDATE_EXP,
		SC_LIST,
		SC_ADD_RECIPE,
		SC_DELETE_RECIPE,
		SC_EXTRACT_RECIPE,
		SC_UPDATE_RECIPE_EXP,
		SC_RECIPE_LIST,
		SC_MANUFACTURE,
		SC_CANCEL_MANUFACTURE,
	};
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

namespace eEtc
{
	enum eCSEtc
	{
		CS_DARKLAIR_RANK_BOARD,
		CS_PVPLADDER_RANK_BOARD,
		CS_WINDOW_STATE,
#if defined(PRE_ADD_PVP_RANKING)
		CS_PVP_RANK_BOARD, // 내정보
		CS_PVP_RANK_LIST,  // 페이지정보( 길드검색 )
		CS_PVP_RANK_INFO,  // 캐릭명검색.
		CS_PVP_LADDER_RANK_BOARD,
		CS_PVP_LADDER_RANK_LIST,
		CS_PVP_LADDER_RANK_INFO,
#endif
#if defined( PRE_ADD_GAMEQUIT_REWARD )
		CS_GAMEQUIT_REWARDCHECK_REQ,	//종료 보상 대상자인지 확인 요청
		CS_GAMEQUIT_REWARD_REQ,			//종료 보상 요청
#endif	//	#if defined( PRE_ADD_GAMEQUIT_REWARD )
	};

	enum eSCEtc
	{
		SC_DARKLAIR_RANK_BOARD,
		SC_PVPLADDER_RANK_BOARD,
#if defined( PRE_USA_FATIGUE )
		SC_CHANNELINFO_FATIGUEINFO,
#endif // #if defined( PRE_USA_FATIGUE )
#if defined(PRE_ADD_PVP_RANKING)
		SC_PVP_RANK_BOARD,
		SC_PVP_RANK_LIST,
		SC_PVP_RANK_INFO,
		SC_PVP_LADDER_RANK_BOARD,
		SC_PVP_LADDER_RANK_LIST,
		SC_PVP_LADDER_RANK_INFO,
#endif
#if defined( PRE_ADD_GAMEQUIT_REWARD )
		SC_GAMEQUIT_REWARDCHECK_RES,	//종료 보상 대상자인지 응답
		SC_GAMEQUIT_REWARD_RES,			//종료 보상 응답
#endif	//	#if defined( PRE_ADD_GAMEQUIT_REWARD )
#if defined( PRE_DRAGONBUFF )
		SC_WORLDBUFFMSG,
#endif
	};
}

namespace eGuildRecruit
{
	enum eCSGuildRecruit
	{
		CS_GUILDRECRUIT_LIST,			// 길드게시판에 등록한 길드 목록 요청(기본)-----		
		CS_GUILDRECRUIT_CHARACTER,		// 자신의 길드에 가입신청한 목록 요청(기본)-----
		CS_GUILDRECRUIT_MYLIST,			// 자신이 길드에 가입신청한 목록 요청(기본)----
		CS_GUILDRECRUIT_REQUESTCOUNT,	// 자신이 길드에 가입신청한 횟수
		CS_GUILDRECRUIT_REGISTERINFO,	// 길드모집게시판에 등록한 내용 확인--
		CS_GUILDRECRUIT_REGISTER,		// 길드모집게시판에 길드 등록 및 해지----
		CS_GUILDRECRUIT_REQUEST,		// 길드모집게시판을 통한 길드 가입 신청 및 해지--
		CS_GUILDRECRUIT_ACCEPT,			// 가입 승인
	};
	enum eSCGuildRecruit
	{
		SC_GUILDRECRUIT_LIST,			// 길드게시판에 등록한 길드 목록 전송--
		SC_GUILDREQUEST_CHARACTER,		// 자신의 길드에 가입신청한 목록 전송--
		SC_GUILDRECRUIT_MYLIST,			// 자신이 길드에 가입신청한 목록 요청--
		SC_GUILDRECRUIT_REQUESTCOUNT,	// 자신이 길드에 가입신청한 횟수
		SC_GUILDRECRUIT_REGISTERINFO,	// 길드모집게시판에 등록한 내용 확인--
		SC_GUILDRECRUIT_REGISTER,		// 길드모집게시판에 길드 등록 및 해지 결과--
		SC_GUILDRECRUIT_REQUEST,		// 길드모집게시판을 통한 길드 가입 신청 및 해지 결과--
		SC_GUILDRECRUIT_ACCEPRESULT,	// 가입 승인 결과(가입 승인/거절을 요청한 길드원에게 전송)
		SC_GUILDRECRUIT_MEMBERRESULT,	// 가입 승인 결과(가입 신청을 한 유저에게 전송
	};
}

#if defined (PRE_ADD_DONATION)
namespace eDonation
{
	enum eCSDonation
	{
		CS_DONATE,
		CS_DONATION_RANKING,
	};

	enum eSCDonation
	{
		SC_DONATION_OPEN,
		SC_DONATE,
		SC_DONATION_RANKING,
		SC_DONATION_TOP_RANKER,
	};
}
#endif // #if defined (PRE_ADD_DONATION)

#if defined (PRE_ADD_BESTFRIEND)


namespace eBestFriend
{
	enum eCSBestFriend
	{
		CS_GETINFO,
		CS_SEARCH,
		CS_REGIST,
		CS_ACCEPT,
		CS_EDITMEMO,
		CS_CANCELBF,
	};

	enum eSCBestFriend
	{
		SC_GETINFO,
		SC_SEARCH,
		SC_REGISTREQ,
		SC_REGISTACK,
		SC_COMPLETE,
		SC_EDITMEMO,
		SC_CANCELBF,
		SC_CLOSEBF,
	};
}
#endif // #if defined (PRE_ADD_BESTFRIEND)

#ifdef PRE_ADD_DOORS
namespace eDoors
{
	enum eCSDoors
	{
		CS_GET_AUTHKEY,
		CS_CANCEL_AUTH,
		CS_GET_MOBILEAUTHFLAG,
	};

	enum eSCDoors
	{
		SC_GET_AUTHKEY,
		SC_CANCEL_AUTH,
		SC_GET_MOBILEAUTHFLAG,
	};
};
#endif		//#ifdef PRE_ADD_DOORS

#if defined( PRE_PRIVATECHAT_CHANNEL )
// CS_PRIVATECHAT_CHANNEL
namespace ePrivateChatChannel
{
	enum eCSPrivateChatChannel
	{
		CS_PRIVATECHAT_CHANNEL_ADD,	    // 사설채팅채널 개설
		CS_PRIVATECHAT_CHANNEL_JOIN,	// 사설채팅채널 참여
		CS_PRIVATECHAT_CHANNEL_INVITE,	// 사설채팅채널 초대
		CS_PRIVATECHAT_CHANNEL_INVITEREQ,// 사설채팅채널 초대 결과
		CS_PRIVATECHAT_CHANNEL_OUT,		// 사설채팅채널 나가기
		CS_PRIVATECHAT_CHANNEL_MOD,		// 사설채팅채널 정보변경
		CS_PRIVATECHAT_CHANNEL_KICK,	// 사설채팅채널 추방
	};

	enum eSCPrivateChatChannel
	{
		SC_PRIVATECHAT_CHANNEL_INFO,			// 사설채팅채널 정보		
		SC_PRIVATECHAT_CHANNEL_JOINRESULT,		// 사설채팅채널 참여 결과
		SC_PRIVATECHAT_CHANNEL_INVITERESULT,	// 사설채팅채널 초대 결과
		SC_PRIVATECHAT_CHANNEL_OUTRESULT,		// 사설채팅채널 나가기 결과
		SC_PRIVATECHAT_CHANNEL_KICKRESULT,		// 사설채팅채널 추방 결과
		SC_PRIVATECHAT_CHANNEL_ADD,				// 사설채팅채널 참여 멤버 정보
		SC_PRIVATECHAT_CHANNEL_DEL,				// 사설채팅채널 나간 멤버 정보
		SC_PRIVATECHAT_CHANNEL_MOD,				// 사설채팅채널 마스터변경
		SC_PRIVATECHAT_CHANNEL_KICK,			// 사설채팅채널 추방 멤버 정보
		SC_PRIVATECHAT_CHANNEL_RESULT,			// 사설채널채팅 요청 결과값(CS 메시지에 대한 결과값)
	};
};
#endif //#if defined( PRE_PRIVATECHAT_CHANNEL )

#if defined( PRE_ALTEIAWORLD_EXPLORE )
namespace eAlteiaWorld
{
	enum eCSAlteiaWorld
	{
		CS_ALTEIAWORLD_INFO,		// 알테이아 정보
		CS_ALTEIAWORLD_JOIN,	    // 알테이아 조인
		CS_ALTEIAWORLD_SENDTICKET,  // 알테이아 입장권 선물
		CS_ALTEIAWORLD_DICE,		// 알테이아 주사위 굴림
		CS_ALTEIAWORLD_NEXTMAP,		// 알테이아 다음 맵으로 이동
		CS_ALTEIAWORLD_QUITINFO,	// 알테이아 정보창 나가기
	};

	enum eSCAlteiaWorld
	{
		SC_ALTEIAWORLD_JOIN_INFO,	// 알테이아 입장탭 정보
		SC_ALTEIAWORLD_PRIVATEGOLDKEYRANKING_INFO,	// 알테이아 개인 황금열쇠랭킹 정보
		SC_ALTEIAWORLD_PRIVATEPLAYTIMERANKING_INFO,	// 알테이아 개인 클리어시간랭킹 정보
		SC_ALTEIAWORLD_GUILDGOLDKEYRANKING_INFO,	// 알테이아 길드 황금열쇠랭킹 정보
		SC_ALTEIAWORLD_SENDTICKET_INFO,	// 알테이아 입장권보내기 정보
		SC_ALTEIAWORLD_JOIN_RESULT,	// 알테이아 조인 결과
		SC_ALTEIAWORLD_SENDTICKET,	// 알테이아 입장권 받음
		SC_ALTEIAWORLD_SENDTICKET_RESULT,	// 알테이아 입장권 전송 결과(보낸사람한테)
		SC_ALTEIAWORLD_DICE_RESULT,		// 알테이아 주사위 굴림 결과
	};
};
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
namespace eStampSystem
{
	enum eSCStampSystem
	{
		SC_STAMPSYSTEM_INIT,
		SC_STAMPSYSTEM_ADD_COMPLETE,
		SC_STAMPSYSTEM_CLEAR_COMPLETE,
		SC_STAMPSYSTEM_CHANGE_WEEKDAY,
		SC_STAMPSYSTEM_CHANGE_TABLE,
	};
}
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_ADD_DWC)
namespace eDWC
{
	enum eCSDWC
	{
		CS_CREATE_DWCTEAM,			// 팀 생성
		CS_LEAVE_DWCTEAM_MEMB,		// 길드원 탈퇴 요청
		CS_INVITE_DWCTEAM_MEMBREQ,	// 길드원 초대 요청
		CS_INVITE_DWCTEAM_MEMBACK,	// 길드원 초대 응답
		CS_GET_DWCTEAM_INFO,
		CS_GET_DWC_RANKPAGE,
		CS_GET_DWC_FINDRANK,
	};

	enum eSCDWC
	{
		SC_CREATE_DWCTEAM,					// 팀 생성
		SC_DISMISS_DWCTEAM,					// 팀 해체
		SC_LEAVE_DWCTEAM_MEMB,				// 길드원 탈퇴
		SC_INVITE_DWCTEAM_MEMBREQ,			// 길드원 초대 요청
		SC_INVITE_DWCTEAM_MEMBACK,			// 길드원 초대 응답 or 요청 결과
		SC_INVITE_DWCTEAM_MEMBACK_RESULT,	// 길드원 초대 응답 결과
		SC_GET_DWCTEAM_INFO,
		SC_GET_DWCTEAM_MEMBER,
		SC_GET_DWC_RANKPAGE,
		SC_CHANGE_DWCTEAM_MEMBERSTATE,
		SC_DWC_TEAMNAME_INFO,
		SC_GET_DWC_FINDRANK,
		SC_DWCCHANNELINFO,
		SC_CHANGE_DWCTEAM,
	};
}
#endif

//#####################################################
// 여기 위에다 추가해주세용~!
//#####################################################


// Farm -------------------------------------------------------------------------
namespace eFarm
{
	enum eCSFarm
	{
		CS_START,
		CS_PLANT,
		CS_HARVEST,
		CS_ADD_WATER,
		CS_WAREHOUSE_LIST,
		CS_TAKE_WAREHOUSE_ITEM,
		CS_FARMINFO,
	};

	enum eSCFarm
	{
		SC_FARMINFO,
		SC_FARMPLANTEDINFO,
		SC_BEGIN_AREASTATE,
		SC_PLANT,
		SC_HARVEST,
		SC_ADDWATER,
		SC_AREAINFO,
		SC_AREAINFO_LIST,
		SC_WAREHOUSE_LIST,
		SC_TAKE_WAREHOUSE_ITEM,
		SC_FIELDCOUNTIFNO,
		SC_WAREHOUSE_ITEMCOUNT,
		SC_ADDWATER_ANOTHERUSER,
#if defined( PRE_ADD_VIP_FARM )
		SC_START,
#endif // #if defined( PRE_ADD_VIP_FARM )
		SC_CHANGE_OWNERNAME,
		SC_OPEN_FARMLIST,
	};
};

//Fishing
namespace eFishing
{
	enum eCSFishing
	{
		CS_READYFISHING,		//낚시가능 영역에 있다면 시작 (받아서 체크해야함) 
		CS_CASTBAIT,			//한턴시작 (한턴한턴의 시작시점)
		CS_STOPFISHING,			//낚시종료
		CS_PULLINGROD,			//중간중간 유저메세지 처리
		CS_FISHINGREWARD,		//낚시결과요청
	};

	enum eSCFishing
	{
		SC_READYFISHING,		//시작에 대한 응답
		SC_FISHINGPATTERN,		//패턴정보
		SC_CASTBAITRESULT,		//시작에대한결과(실패시)
		SC_STOPFISHINGRESULT,	//낚시종료
		SC_FISHINGREWARD,		//낚시에 대한 결과~
		SC_FISHINGSYNC,
		SC_FISHINGPLAYER,
		SC_FISHINGEND,
	};
};



namespace eTimeEvent
{
	enum eCSTimeEvent
	{
	};
	enum eSCTimeEvent
	{
		SC_TIMEEVENTLIST,
		SC_ACHIEVE_EVENT,
		SC_EXPIRE_EVENT,
	};
};

namespace ePlayerCustomEventUI
{
	enum eCSPlayerCustomEventUI
	{
		CS_GETPLAYERUI,
		CS_DISMISSGUILD
	};

	enum eSCPlayerCustomEventUI
	{
		SC_GETPLAYERUI,
	};
};


// PROTOCOL_ORIGIN_END		-- 셔플에 필요한 토큰입니다 지우지 말아주세요.

#else	// #if !defined (_FINAL_BUILD) || !defined (ACTIVATE_PROTOCOLSHUFFLE)

// 국가별 프로토콜 파일 추가
#if defined (_KR) || defined(_KRAZ)
#include "DNProtocol_KOR.h"
#elif defined (_JP)
#include "DNProtocol_JPN.h"
#elif defined (_CH)
#include "DNProtocol_CHN.h"
#elif defined (_TW)
#include "DNProtocol_TWN.h"
#elif defined (_US)
#include "DNProtocol_USA.h"
#elif defined (_SG)
#include "DNProtocol_SIN.h"
#endif

#endif	// #if !defined (_FINAL_BUILD) || !defined (ACTIVATE_PROTOCOLSHUFFLE)

