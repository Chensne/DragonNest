#pragma once

#include "DNPacket.h"
#include "MemPool.h"

/*--------------------------------------------------------------------------
					Server쪽에서 쓰는 모든것들(?)
--------------------------------------------------------------------------*/

// const ----------------------------------------------------------------------------------------------

#if defined(_CH)
const WCHAR* const SQLPASSWORD = L"4795CEA6409d492";
const WCHAR* const SQLHOSTNAME = L"A2A375E23370f8a";
const WCHAR* const SQLAPPNAME = L"C98D23990E1119a";
#elif defined(_JP)
const WCHAR* const SQLPASSWORD = L"945C437D6632718";
//const WCHAR* const SQLPASSWORD = L"A443131C-2988-4C3E-9974-37033B2A0A33";
const WCHAR* const SQLHOSTNAME = L"E4C874F3D920ced";
const WCHAR* const SQLAPPNAME = L"2001C72210299d0";
#elif defined(_TW)
const WCHAR* const SQLPASSWORD = L"3A6A7D31A8c6415";
const WCHAR* const SQLHOSTNAME = L"3B74FB5B269bf5d";
const WCHAR* const SQLAPPNAME = L"4A54A42ED0381e3";
#elif defined(_US) 
const WCHAR* const SQLPASSWORD = L"BACF8969BB00b2d";
const WCHAR* const SQLHOSTNAME = L"04B3944622a7c5b";
const WCHAR* const SQLAPPNAME = L"33518A83865e269";
#elif defined(_SG)
const WCHAR* const SQLPASSWORD = L"4708A804794d233";
const WCHAR* const SQLHOSTNAME = L"D3B8367356e19d7";
const WCHAR* const SQLAPPNAME = L"3A222498E96c63f";
#elif defined(_TH)
const WCHAR* const SQLPASSWORD = L"gozjtoRldi!*";
const WCHAR* const SQLHOSTNAME = L"rjfflausenlwlsek@#@";
const WCHAR* const SQLAPPNAME = L"roshatlRl!#";
#elif defined(_ID)
const WCHAR* const SQLPASSWORD = L"skQmsgozj!*sha";
const WCHAR* const SQLHOSTNAME = L"rhdqntlzlsi)*^";
const WCHAR* const SQLAPPNAME = L"rotoRjwuqjfu!#";
#elif defined(_RU)
const WCHAR* const SQLPASSWORD = L"3A95846ED27a5af";
const WCHAR* const SQLHOSTNAME = L"1869A6FAEFba675";
const WCHAR* const SQLAPPNAME = L"F980409883e225c";
#elif defined(_EU)
const WCHAR* const SQLPASSWORD = L"110C37F5B9fb934";
const WCHAR* const SQLHOSTNAME = L"D8FA71CD6Ee2479";
const WCHAR* const SQLAPPNAME = L"518BD8D722e2f6b";
#else
const WCHAR* const SQLPASSWORD = L"428A44DC4Eebd12";
const WCHAR* const SQLHOSTNAME = L"CF0EEDCEF386298";
const WCHAR* const SQLAPPNAME = L"D121240B6419392";
#endif // #if defined( _US )

const WCHAR* const SQLActozMembershipDBPassword = L"dnaccount$dnest7h.o.t.44_2";
const WCHAR* const SQLActozWorldDBPassword = L"dndb$dnest50h.o.t.44_2";
const WCHAR* const SQLActozCommonDBPassword_Test = L"devdb$game9mingi44_2";
const WCHAR* const SQLActozCommonDBPassword = L"dncomdb$game60mingi44_2";

#if defined( PRE_ADD_DWC )
const WCHAR* const DWCPREFIX = L"DWCGM_";
const int DWCPREFIXLEN			= 6;
#endif // #if defined( PRE_ADD_DWC )

const char DBSERVERMAX			= 16;
const char THREADMAX			= 8;	// DB THREAD
const char LOGINTIMELENMAX		= 20;	// last login time length max
const char TIMELENMAX = 20;				// 데이트타임 타입

const int LOGINCOUNTMAX				= 64;			// 총 로그인 서버 개수
const int LOGINCONCOUNTINFOTERM		= (1*30*1000);	// 로그인 서버 현재 동시접속자 정보 마스터 서버에 보고 제한간격 (단위:ms)
const int LOGINSERVERLISTSORTTERM	= (1*3*1000);	// 로그인 서버 세션 정렬 제한간격 (단위:ms)
const int MASTERCOUNTMAX			= 10;			// 총 마스터 서버 개수

const int  DELETECHAR_WAITTIME_MINUTE	= 7*24*60;	// 7일
const BYTE PROCESSCOUNT = 40;	// 초당 오브젝트 처리개수

const int INTERNALBUFFERLENMAX = 51200;

const UINT PROCESSBUFFERLENMAX = 1024 * 64;
const UINT WORKERBUFFERLENMAX = 1024 * 64;

const int QUESTSIZEINFO = 2775;

const int QUERYOVERTIME = 1000;

const int QUERYNAMESIZE = 200; // 쿼리 최대 크기
const int GAMEDELAYSIZE = 200; // 게임딜레이 로그 최대 크기

const int SERVERDETACHPACKETSIZE = 1024*40;	// 패킷전송시 버퍼사이즈크기
//const variable.
#ifdef _FINAL_BUILD
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_PARTY = 120 * 1000;								//최초 게임서버 컨넥트 대기 시간
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_SINGLE = 120 * 1000;
const unsigned long WAIT_FOR_ANOTHER_USER_TIME_LIMIT = 15 * 1000;						//최초인원이 컨넥을 하고 다른인원을 기다리는 시간
const unsigned long WAIT_FOR_LOAD_TIME_LIMIT = 60 * 1000;								//데이타베이스의 로딩을 기다리는 시간
const unsigned long GOGO_SING_TO_PLAY_FOR_WAIT_TIME_LIMIT = 2 * 60 * 1000;					//클라이언트 로딩과 싱크를 기다리는 시간
const unsigned long GOGO_SING_TO_FARMPLAY_FOR_WAIT_TIME_LIMIT = 5 * 60 * 1000;			//농장로딩 시간은 초큼 별개로
#else
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_PARTY = 60 * 1000;								//최초 게임서버 컨넥트 대기 시간
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_SINGLE = 120 * 1000;
const unsigned long WAIT_FOR_ANOTHER_USER_TIME_LIMIT = 60 * 1000;						//
const unsigned long WAIT_FOR_LOAD_TIME_LIMIT = 150 * 1000;								//
const unsigned long GOGO_SING_TO_PLAY_FOR_WAIT_TIME_LIMIT = 5 * 60 * 1000;				//
const unsigned long GOGO_SING_TO_FARMPLAY_FOR_WAIT_TIME_LIMIT = 5 * 60 * 1000;			//농장로딩 시간은 초큼 별개로
#endif

const int EACH_GAMESERVER_MAX_ROOM_COUNT = 20;
const int EACH_GAMESERVER_MAX_USER_COUNT = 100;

const int INTERVALDELETE_TIMINGTICK = 1000 * 60;		//1분쯤??...-_-;
const int CHECK_ZOMBIE_DISCONNECT_TICK = 1000 * 60 * 5;
const int DISTRIBUTE_IDLE_TICK = 1000 * 5;

const int DELAYED_PRCESS_CHECKTICK = 1000 * 10;
const int DELAYED_PRCESS_LOG_COUNT = 10;
const int FRAMEDEADLINE = 17;

const int EVENTMAPSTRMAX = 50;

const int CHECKLIVETICK = 60000 * 2;//  * 5;	// 핵쉴드 5분 마다 체크 -> 1분으로 바꾸라고 해서 바꿈 -> 2분으로 늘림
const int CHECKRESPONSETICK = 60000 * 3;		// 핵쉴드 응답 시간 (3분정도?)

const int CHECKGPKFIRSTTICK = 1000 * 60 * 2;	// 최초 검사타이밍
const int CHECKGPKTICK = 60000;
const int MANAGEDMAX = 20;

#if !defined(_FINAL_BUILD)
const DWORD CHECK_TCP_PING_TICK	= 1000*30*1;	// 1분
const DWORD CHECK_UDP_PING_TICK = 1000*30*1;	// 1분
#else		//#if !defined(_FINAL_BUILD)
const DWORD CHECK_TCP_PING_TICK	= 1000*60*1;	// 1분
const DWORD CHECK_UDP_PING_TICK = 1000*60*1;	// 1분
#endif		//#if !defined(_FINAL_BUILD)

#if defined(_CH)
const int CHECKLOGINUSERKICKTICK = 30 * 60 * 1000;		// 로그인 서버에 30분 동안 머물면 강퇴!(중국만)
#else
const int CHECKLOGINUSERKICKTICK = 15 * 60 * 1000;		// 로그인 서버에 15분 동안 머물면 강퇴!
#endif //#if defined(_CH)
const int CHECKNOTAUTHLOGINUSERKICKTICK = 5 * 60 * 1000;  // 로그인 서버에 인증되지 않은체 5분 동안 머물면 강퇴!

const unsigned int DEFAULTUSERSESSIONID = 10000;

const unsigned int CASHDELUSERDELAY_LIMITTICK = (4*30*1000);

// DB 누적작업 관련 개수
const int DBQUERYSTREAMDATAMAX			= (36*1024);	// DB 누적작업 데이터 최대 크기			: DB 서버에 바로 송신되는 대신 큐에 누적 후 일괄처리가 필요한 데이터 구조체 중 가장 큰 것을 기준으로 설정 → 크기가 부족할 경우 지속적인 재조정 필요 예상
const unsigned int DBQUERYAUTOCHKSUM	= 0xE41C723F;	// DB 쿼리 직렬화 처리 시 체크값

const int QUESTINVENBLANKCHECKMIN	= 2;				// 퀘스트 인벤토리 체크 시 메시지 발송이 되는 최소 개수
const float MAX_ITEMPRICE_RATE = 0.5f;

#if defined(_FINAL_BUILD)
const int BEGINAUTHLIMITSEC			= (9*10);			// 최초 인증 제한시간 (단위:초, 0 이하는 무제한 대기 ?)
const int CHECKAUTHLIMITSEC			= (6*10);			// 일반 인증 제한시간 (단위:초)
const int BEGINAUTHLIMITTERM		= (BEGINAUTHLIMITSEC*1000);		// 최초 인증 제한간격 (단위:ms)
//const int CHECKAUTHLIMITTERM		= (CHECKAUTHLIMITSEC*1000);		// 일반 인증 제한간격 (단위:ms)
const int CHECKAUTHLIMITTERM		= (1*30*1000);		// 일반 인증 제한간격 (단위:ms)		// P.S.> 던전, PVP 등 컨텐츠 참여자들의 불편사항을 고려하여 기존 시간보다 짧게 정함
#else	// _FINAL_BUILD
const int BEGINAUTHLIMITSEC			= (6*10);			// 최초 인증 제한시간 (단위:초, 0 이하는 무제한 대기 ?)
const int CHECKAUTHLIMITSEC			= (4*10);			// 일반 인증 제한시간 (단위:초)
const int BEGINAUTHLIMITTERM		= (BEGINAUTHLIMITSEC*1000);		// 최초 인증 제한간격 (단위:ms)
//const int CHECKAUTHLIMITTERM		= (CHECKAUTHLIMITSEC*1000);		// 일반 인증 제한간격 (단위:ms)
const int CHECKAUTHLIMITTERM		= (1*20*1000);		// 일반 인증 제한간격 (단위:ms)		// P.S.> 던전, PVP 등 컨텐츠 참여자들의 불편사항을 고려하여 기존 시간보다 짧게 정함
#endif	// _FINAL_BUILD

const int RESETAUTHLISTMAX			= (1000);			// 인증정보 목록 초기화 크기 (주의!!! - 본 전역상수를 사용하는 패킷들이 송신/수신 패킷 크기를 넘지 않아야 함)
const int RESETAUTHLISTTERM			= (1*1000);			// 인증정보 목록 초기화 작업주기 (단위:ms)
const int RESETAUTHSERVERTERM		= (5*1000);			// 인증정보 서버 초기화 작업주기 (단위:ms)
const int RESETAUTHWORLDCHECKTERM	= (5*1000);			// 인증정보 월드 초기화 작업주기 (단위:ms)
const int RESETAUTHWORLDLIMITTERM	= (2*30*1000);		// 인증정보 월드 초기화 제한시간 (단위:ms)

const int GETAUTHUSERCOUNT = 1000 * 10;					// 인증월드유저 카운트 획득주기

const int GUILDMGRCHKTERM = (5*1000);					// 길드 관리자 체크 주기
const int GUILDMGRUPDTERM = (10*1000);					// 길드 관리자 갱신 주기
const int GUILDCHKMEMBTERM = (30*1000);					// 길드 멤버정보 체크 주기
const int GUILDCHKRESCTERM = (30*1000);					// 길드 자원반납 체크 주기
const int GUILDREQINFOTERM = (60*1000);					// 길드 정보요청 체크 주기
const int GUILDRFSVIEWTERM = (10*1000);					// 길드 시각정보 갱신 주기
const int GUILDAPLPOINTTERM = (10*1000);				// 길드 일반/래더 포인트 DB 적용 주기
const int GUILDCHKRESCLIMIT = (2*30*1000);				// 길드 자원반납 체크 제한

const int PERIODQUESTTERM = (60*1000);					// 기간제 퀘스트 갱신 주기

const int PERIODGUILDWARTERM = (60*1000);				// 길드전 갱신 주기
const int GUILDWARMISSION_MAXGROUP = 4;

const int SERVICEMANAGER_SERVERDEALY_SIZE = 1000;
const int SERVICEMANAGER_REPORT_SIZE = 4096;

#define SWAP16(x) (((((x) & 0xff) << 8) | (((x) >> 8) & 0xff)))
#define SWAP32(x) (((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24)))
#define SWAP64(x) (((((x) & 0xFF00000000000000ULL) >> 56) | (((x) & 0x00FF000000000000ULL) >> 40) | (((x) & 0x0000FF0000000000ULL) >> 24) | (((x) & 0x000000FF00000000ULL) >>  8) | \
	(((x) & 0x00000000FF000000ULL) <<  8) | (((x) & 0x0000000000FF0000ULL) << 24) | (((x) & 0x000000000000FF00ULL) << 40) | (((x) & 0x00000000000000FFULL) << 56)))

struct DNProcessCheck
{
	enum eDNProcessCheck
	{
		Key					= 0x00000001,
		DisconnectInterval	= 60*1000*2,
		UpdateInterval		= 10*1000,
		HeartbeatKey		= 0x00000002,
		SQLHeartbeatInterval = 60*1000*10,
		SelectIPMagicInterval = 60*1000*60,
	};
};

// enum ----------------------------------------------------------------------------------------------
enum eServerType
{
	SERVERTYPE_LOGIN,		// 로그인 서버
	SERVERTYPE_DB,			// DB 서버
	SERVERTYPE_VILLAGE,		// 빌리지 서버
	SERVERTYPE_GAME,		// 게임 서버
	SERVERTYPE_MASTER,		// 마스터 서버
	SERVERTYPE_LOG,			// 로그 서버
	SERVERTYPE_MAX,
};

enum ePatchType
{
	PATCHTYPE_LOGIN,		// 로그인 서버
	PATCHTYPE_DB,			// DB 서버
	PATCHTYPE_VILLAGE,		// 빌리지 서버
	PATCHTYPE_GAME,			// 게임 서버
	PATCHTYPE_MASTER,		// 마스터 서버
	PATCHTYPE_LOG,			// 로그 서버
	PATCHTYPE_PATCHER,		// ESM
	PATCHTYPE_CASH,			// 캐쉬 서버
};

enum eConnectionKey
{
	CONNECTIONKEY_DEFAULT = 0,
	CONNECTIONKEY_USER,
	CONNECTIONKEY_LOGIN,
	CONNECTIONKEY_DB,
	CONNECTIONKEY_VILLAGE,
	CONNECTIONKEY_GAME,
	CONNECTIONKEY_MASTER,
	CONNECTIONKEY_LOG,
	CONNECTIONKEY_LAUNCHER,
	CONNECTIONKEY_SERVICEMANAGER,
	CONNECTIONKEY_AUTH,		// 인증섭(넥슨)
	CONNECTIONKEY_CASH,
	CONNECTIONKEY_BILLING,	// 빌링섭(퍼블리셔)
	CONNECTIONKEY_SERVICEPATCHER,
	CONNECTIONKEY_SERVERMONITOR,
	CONNECTIONKEY_TW_AUTHLOGIN,	// 인증섭(감마니아 1 - Authorization : 사용자 계정 인증 처리, 상태 (블록 여부, 캐쉬 잔여 포인트 ?) 체크)
	CONNECTIONKEY_TW_AUTHLOGOUT,	// 인증섭(감마니아 2 - Logout : 사용자 로그아웃 처리)
	CONNECTIONKEY_TW_QUERY,
	CONNECTIONKEY_TW_SHOPITEM,
	CONNECTIONKEY_TW_COUPON,	// 감마니아 쿠폰서버
	CONNECTIONKEY_TW_COUPON_ROLLBACK,	// 감마니아 쿠폰롤백..징그러운것들 서버를 포트별로 만들다니..ㅠ
	CONNECTIONKEY_TH_AUTH,
	CONNECTIONKEY_TH_OTP,
	CONNECTIONKEY_TH_QUERY,
	CONNECTIONKEY_TH_SHOPITEM,
	CONNECTIONKEY_DOORS,
};

enum eAcceptOpenVerifyType
{
	ACCEPTOPEN_VERIFY_TYPE_APPINITCOMPLETE = 0,
	ACCEPTOPEN_VERIFY_TYPE_MASTERCONNECTION,
	ACCEPTOPEN_VERIFY_TYPE_RESETAUTH,
	ACCEPTOPEN_VERIFY_TYPE_MAX,
};

enum eServerLogLevel
{
	ServerLogLevel_1 = 1,
	ServerLogLevel_2 = 2,
	ServerLogLevel_3 = 3,
};

enum eUserState		// login에서 쓰는 상태값
{
	STATE_NONE,

	// login
	STATE_CHECKVERSION,
	STATE_CHECKLOGIN,		// 아이디 패스워드 확인상태
	STATE_SERVERLIST,
	STATE_WAITUSER,			// 대기유저
	STATE_CHARLIST,			// 캐릭터 리스트 상태
	STATE_CHANNELLIST,
	STATE_CONNECTVILLAGE,	// 빌리지 서버에 접속가능상태
	STATE_CONNECTGAME,		// 겜 서버에 접속가능상태
	STATE_RECONNECTLOGIN,	// 리커넥트 상태

	// master
	STATE_CHECKVILLAGE,		//빌리지로 넘어가기전의 상태입니다. 유저가 끊길경우 고아가 되므로 이 스테이트를 통해서 관리됩니다.
	STATE_CHECKGAME,		//게임으로 넘어가기 전의 상태입니다. 상동
	STATE_CHECKRECONNECTLOGIN,	//캐릭터선택창으로 넘어가기 전의 상태입니다. 상동
	STATE_VILLAGE,			// village상태인가
	STATE_GAME,				// game상태인가
	STATE_LOGIN,			// login상태인가

	// village
	STATE_LOADUSERDATA,		// 유저데이터 DB에서 불러왔는지
	STATE_READY,			// 필드 나갈 준비 완료
	STATE_READYTOVILLAGE,	// village 딴 장비로 옮길때 (준비단계)
	STATE_READYTOGAME,		// game으로 넘어갈때 (준비단계)
	STATE_READYTOLOGIN,		// login으로 넘어갈때 (준비단계)
	STATE_MOVETOVILLAGE,	// village 딴 장비로 옮길때
	STATE_MOVETOGAME,		// game으로 넘어갈때
	STATE_MOVETOLOGIN,		// login으로 넘어갈때
	STATE_MOVESAMECHANNEL,	// 같은 채널 이동

#if defined(_HSHIELD)
	HSHIELD_NONE = 0,
	HSHIELD_RESPONSEVERSION = 1,	// checkversion 패킷 받은 상태
	HSHIELD_RECONNECTLOGIN,		// 재접속 패킷 받은 상태
	HSHIELD_LOADUSER,			// 디비 로드 끝낸상태
	HSHIELD_REQUEST,			// _AhnHS_MakeRequest 요청상태
	HSHIELD_RESPONSE,			// 응답온 상태
#endif	// _HSHIELD

};

//enumulation
//GameRoom
enum eGameRoomState
{
	_GAME_STATE_NONE,					//생성!
	_GAME_STATE_READY2CONNECT,			//초기화 완료 컨넥션 대기! 상태
	_GAME_STATE_CONNECT2CHECKAUTH,		//DB 를 이용한 인증정보 체크
	_GAME_STATE_CONNECT2LOAD,			//연결 및 검증 완료! 로드상태로 전이
	_GAME_STATE_LOAD2SYNC,				//클라이언트 연결완료 준비상태
	_GAME_STATE_SYNC2SYNC,
	_GAME_STATE_SYNC2PLAY,				//플레이대기중
	_GAME_STATE_PLAY,					//게임중!

	_GAME_STATE_CANCEL_LOADING,			//백그라운드 로더와 싱크
	_GAME_STATE_DESTROYED,				//방깨졌삼!
	_GAME_STATE_FARM_NONE,				//농장용 스테이트입니다. 생성 루틴이 달라 일반과 다르게 흘러갑니다 _GAME_STATE_PLAY부터는 동일
	_GAME_STATE_FARM_READY2LOAD,		//디비에 농장관련 데이타를 요청한 상태
	_GAME_STATE_FARM_LOAD2PLAY,			//디비에서 농장관련 데이타를 받았다. 세팅하고 마스터에 알리자
	_GAME_STATE_FARM_PAUSE,				//농장 포우즈~
	_GAME_STATE_PVP_SYNC2GAMEMODE,		//피빕피일경우 유저가 없는 상태에서 시작시점이 생김(길드전) 접속스테이트를 건너뛰고 모드받고 로드로 넘어가는 중간스테이트
};

//GameSession
enum eGameSessionState
{
	SESSION_STATE_NONE,
	SESSION_STATE_READY,				//객체생성만 되어 있음! 연결은 안되어 있음
	SESSION_STATE_RUDP_CONNECTED,	
	SESSION_STATE_CONNECTED,			//연결 되었음!
	SESSION_STATE_LOAD,					//연결 되면 디비에 요청합니다.
	SESSION_STATE_LOADED,				//게임로딩완료!
	SESSION_STATE_WAIT_TO_READY,		//Ready 상태 기다림
	SESSION_STATE_READY_TO_SYNC,		//Sync 준비 완료!
	SESSION_STATE_SYNC_READY_2_DELAY,	//서버가 메세지처리준비가 안되상태에서 메세지를 받았습니다.
	SESSION_STATE_READY_TO_PLAY,		//Sync 2 Play
	SESSION_STATE_GAME_PLAY,			//게임플레이중!
	SESSION_STATE_READY_TO_VILLAGE,		//game -> village (준비단계)
	SESSION_STATE_READY_TO_LOGIN,		//game -> login char select (준비단계)
	SESSION_STATE_RECONNECTLOGIN,		//game -> login char select
	SESSION_STATE_SERVER_CHANGE,		//서버이동중! 이동완료되면 끊김
	SESSION_STATE_DISCONNECTED,			//짤린상태! (tcp까지 전부 끊긴 상태)
	//SESSION_STATE_CRASH,				//크래쉬!
};

// GameServer Parse Type
enum eGameMsgType
{
	MSG_TYPE_MASTER,
	MSG_TYPE_DATABASE,
};

enum eFieldMessage
{
	FM_CREATE,
	FM_DESTROY,
	FM_SHOW,
	FM_HIDE,
	FM_MOVE,
	FM_ACTORMSG,
	FM_CHAT,
	FM_CHANGEEQUIP,
	FM_ADDEXP,
	FM_LEVELUP,
	FM_CHANGEAPPELLATION,
	FM_CHANGEHIDE,
	FM_CHANGECASHEQUIP,
	FM_CHANGEGUILDSELFVIEW,
	FM_CHANGEJOB,
	FM_EFFECT,
	FM_CHANGECOLOR,
	FM_CHANGEGLYPH,
	FM_CHATROOMVIEW,
	FM_CHATROOMUSER,
	FM_CHANGEVEHICLE,
	FM_CHANGEVEHICLECOLOR,
	FM_CHANGEPET,
	FM_CHANGEPETBODY,
	FM_RENAME_PET,
	FM_ENTERPARTY,
	FM_LEAVEPARTY,
	FM_CHARACTER_RENAME,
	FM_GUILDWARWINSKILL,
	FM_USE_SOURCE,
	FM_USE_EFFECTSKILL,
	FM_HAVE_NAMEDITEM,
	FM_CHANGE_TRANSFORM,
	FM_CHANGETALISMAN,
	FM_CHANGEDWCTEAM,
};

enum eClass
{
	CLASS_NONE = 0,
	CLASS_WARRIER = 1,
	CLASS_ARCHER,
	CLASS_SOCERESS,
	CLASS_CLERIC,
#if defined( PRE_ADD_ACADEMIC )
	CLASS_ACADEMIC,
#endif // #if defined( PRE_ADD_ACADEMIC )
#if defined( PRE_ADD_KALI )
	CLASS_KALI,
#endif // #if defined( PRE_ADD_KALI )
	CLASS_MAX,
};

enum eJob
{
	JOB_NONE,

	JOB_WARRIER = 1,
	JOB_ARCHER,
	JOB_SOCERESS,
	JOB_CLERIC,

	JOB_BLAZEWARRIER = 11,
	JOB_SWORDMAN,
	JOB_CHAINSOLDIER,

	JOB_RANGER = 21,
	JOB_HUNTER,
	JOB_KEEPER,

	JOB_WIZARD = 31,
	JOB_MAGICIAN,
	JOB_WITCH,

	JOB_PRIEST = 41,
	JOB_PALADIN,
	JOB_MONK,

	JOB_MAX = 47,
};

enum eAppendItemString
{
	Append_ItemSN,
	Append_Price,
	Append_Limit,
	Append_ItemID,
	Append_Option,
	Append_Refundable,
#ifdef PRE_ADD_LIMITED_CASHITEM
	Append_QuantityLimited,
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	Append_Max,
};

enum eModItemBitmap
{
	ModItem_Durability = 1,
	ModItem_Level = (1<<1),
	ModItem_Potential = (1<<2),
	ModItem_SoulBound = (1<<3),
	ModItem_SealCount = (1<<4),
	ModItem_Option = (1<<5),
	ModItemAll = ModItem_Durability|ModItem_Level|ModItem_Potential|ModItem_SoulBound|ModItem_SealCount|ModItem_Option,
};

enum eWeaponType
{
	WEAPON_SWORD,			// 대검
	WEAPON_GAUNTLET,		// 건틀렛
	WEAPON_AXE,				// 도끼
	WEAPON_HAMMER,			// 해머
	WEAPON_SMALLBOW,		// 소궁
	WEAPON_BIGBOW,			// 대궁
	WEAPON_CROSSBOW,		// 석궁
	WEAPON_STAFF,			// 스테프
	WEAPON_BOOK,			// 마법서
	WEAPON_ORB,				// 수정구
	WEAPON_PUPPET,			// 주술인형
	WEAPON_MACE,			// 메이스
	WEAPON_FLAIL,			// 프레일
	WEAPON_WAND,			// 완드
	WEAPON_SHIELD,			// 방패
	WEAPON_ARROW,			// 화살
	WEAPON_CANNON,			// 캐논
	WEAPON_BUBBLEGUN,		// 버블건
	WEAPON_GLOVE,			// 글러브
	WEAPON_FAN,             // 부채
	WEAPON_CHAKRAM,         // 차크람
	WEAPON_CHARM,           // 참
	WEAPON_SCIMITER,		// 시미터
	WEAPON_DAGGER,			// 대거
	WEAPON_CROOK,			// 크룩
};

enum eParts
{
	PARTS_FACE,				// 
	PARTS_HAIR,
	PARTS_HELMET,			// 투구
	PARTS_BODY,				// 상체
	PARTS_LEG,				// 하체
	PARTS_HAND,				// 손
	PARTS_FOOT,				// 발
	PARTS_NECKLACE,
	PARTS_EARRING,
	PARTS_RING,
	PARTS_RING2,
	PARTS_CREST,
	PARTS_FULLSET,			// 풀세트
};

enum eQuestDataType
{
	QUESTDATA_STATE = 1,	// 퀘스트 상태
	QUESTDATA_STEP,			// 현재 퀘스트 스텝
	QUESTDATA_JOURNAL,		// 현재 퀘스트 저널
};

enum eEtcType	//  DNEtc table Type
{
	ETC_NONE,
	ETC_QUICKSLOT,			// TQuickSlot QuickSlot[QUICKSLOTMAX];		// 단축 슬롯창 (10개씩 2줄)
	ETC_JOB,				// USHORT wJobArray[JOBMAX];				// 직업 (0: 맨처음 직업, 나머지 쭉쭉~ 전직 포함)
	ETC_NOTIFIER,			// int Notifier[Notifier_MaxCount];	// 알리미 (quest, mission)
	ETC_DUNGEONCLEAR,		// TDungeonClearData DungeonClear[DUNGEONCLEARMAX];	// 던전 클리어 (120개)
	ETC_ETERNITYITEM,		// USHORT wEternityItem[ETERNITYITEMMAX];	// 영구아이템
	ETC_EQUIPDELAYTIME,		// int nEquipDelayTime[EQUIPMAX];			// equip index에 관련된 cooltime
	ETC_EQUIPREMAINTIME,	// int nEquipRemainTime[EQUIPMAX];
};

enum eAuthType
{
	AUTHTYPE_BEGINAUTH,		// 인증 시작
	AUTHTYPE_STOREAUTH,		// 인증 저장
	AUTHTYPE_CHECKAUTH,		// 인증 체크
	AUTHTYPE_RESETAUTH,		// 인증 리셋
	AUTHTYPE_CNT,
};

enum eAbuseCode
{
	ABUSE_NONE = 0,
	ABUSE_MOVE_SERVERS,		// 마을 <-> 던젼 워프
	ABUSE_MOVE_INSKY,		// 공중이동
	ABUSE_HACKSHIELD,		// 핵쉴드에서 부정이용자
	ABUSE_TWN_EXTENDLOG,	// 대만용 추가 로그값 #48714 관계 있음
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	ABUSE_CHARACTER_CHECKSUM, // #80315 캐릭터 체크섬 이상 발견
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )
};

struct WorldUserState
{
	enum eCode
	{
		Add = 0,
		Delete,
		Modify,
	};
};

namespace DBDNMembership
{
	struct OrderStatusCode
	{
		enum eOrderStatusCode
		{
			SuccessPayment = 1,		// 결제성공 & pickup 대기
			Success = 2,			// 구매완료
			FailPayment = 3,		// 결제실패
		};
	};
};

namespace DBDNWorldDef
{
	struct CharacterExpChangeCode
	{
		enum eCharacterExpChangeCode 
		{
			Quest = 1,			// : 퀘스트 보상	-> FKey = QuestID
			Dungeon = 2,		// : 던전 보상		-> FKey = PartyID or NULL
			Cheat = 3,			// : 치트			-> FKey = NULL
			Admin = 4,			// : 보정 변경
			Eternity = 5,		// 영구아이템		-> FKey = ItemSerial
			PvP	= 6,			// : PvP 피로도 보상 -> FKey = NULL
			DungeonMonster = 7,	// :던젼 몬스터 사냥 경험치 -> FKey = PartyID or NULL( 서버에서 구분자로 쓰임..DB에 넘어갈때는 Dungeon 으로 Convert 된다. )
			Item = 8,			// : 경험치증가 아이템 -> FKey = ItemID
		};
	};

	struct CharacterLevelChangeCode
	{
		enum eCharacterLevelChangeCode
		{
			Normal = 1,		// : 일반
			Cheat = 2,		// : 치트
			Admin = 3,		// : 관리자 변경
		};
	};

	struct CoinChangeCode
	{
		enum eCoinChangeCode	// 주의 : 던전 보상, 우편첨부, 창고 입출금, 아이템 되팔기, 교환 입출금, 교환 수수료, 강화 수수료, 스킬 언락, 상점 구입, 우편 수수료 인 경우 각각의 SP에 포함되어 있습니다.
		{
			DoNotDBSave = 0,
			QuestReward = 1,					// : 퀘스트 보상 (FKey: QuestID)
			PickUp = 2,							// : 픽업 (FKey: PartyID or NULL)
			Cheat = 3,							// : 치트 (FKey: NULL)
			Admin = 4,							// : 관리자 변경 (FKey: AuditLogID)
			Use = 5,							// : 사용 (FKey: NULL)
												// 6: 우편송신첨부 (FKey: MailID)
												// 7: 우편수신첨부 (FKey: MailID)
			InvenToWare = 8,					// : 창고 입금 (FKey: NULL)
			WareToInven = 9,					// : 창고 출금 (FKey: NULL)
												// 10: 아이템 되팔기 (FKey: NpcID)
												// 11: 교환 입금 (FKey: CharacterID)
												// 12: 교환 출금 (FKey: CharacterID)
			QuestSub = 13,						// 퀘스트 차감 (FKey: QuestID)
			RepairItem = 14,					// 아이템 수리 (FKey: ItemSerial)
												// 15: 교환 수수료 (FKey: NULL)
												// 16: 강화 수수료 (FKey: ItemEnchantLogID)
												// 17: 스킬 언락 (FKey: SkillID)
												// 18: 상점 구입 (FKey: ItemSerial)
			DisjointTax = 19,					// 분해수수료 (FKey: NULL)
												// 20: 우편 수수료 (FKey: MailID)
												// 21: 무인상점등록 수수료 (FKey: TradeID)
												// 22: 무인상점 아이템 구 (FKey: TradeID)입
												// 23: 무인상점 아이템 판매금 (FKey: TradeID)
												// 24: 무인상점 거래 성립 수수료 (FKey: TradeID)
			CompoundTax = 25,					// 조합수수료 (FKey: NULL)
												// 26: 캐릭터 삭제 (FKey: NULL)
			MaxLevelExperienceTransGold = 27,	// 만랩 경험치 골드 보상 (FKey: NULL)
			Present = 28,						// 선물 (FKey: NpcID)
												// 29: 길드 창설 (FKey: GuildID)
			GuildLevelup = 30,					// 30: 길드 레벨 업 (FKey: GuildID)
			InvenToGuildWare = 31,				// 31: 길드 창고 입금 (FKey: GuildID)
			GuildWareToInven = 32,				// 32: 길드 창고 출금 (FKey: GuildID)
			GuildRewardBuy = 33,				// 33: 길드 보상 구매 (FKey: GuildItemID)
			Donation = 34,						// 기부
												// 35: (DOORS) 등록수수료
												// 36: (DOORS) 입찰수수료
												// 37: (DOORS) 낙찰수수료
												// 38: (DOORS) 낙찰결제
												// 39: (DOORS) 낙찰정산
												// 40: (DOORS) 등록수수료취소
												// 41: (DOORS) 입찰수수료취소
												// 42: (DOORS) 낙찰수수료취소
												// 43: (DOORS) 낙찰결제취소
												// 44: (DOORS) 낙찰정산취소
			CharmItem = 45,						// 보물상자 (FKey: ItemID)
			ChaosItem = 46,						// 카오스 큐브 (FKey: ItemID)
			SpecialBox = 47,					// 특수보관함 (FKey: EventRewardID)
			// 48: 월드창고 사용 수수료
			// 49: C2C 보관
			// 50: C2C 회수
			// 51: C2C 거래
			TalismanSlotOpen = 52,				// 52: 탈리스만 슬롯 오픈(FKey: 슬롯 인덱스)
			TalismanSlotChange = 53,			// 53: 탈리스만 슬롯 장착 위치 이동

			GamblePrice=55,						// 55: 도박모드 참가비
			GambleFirstWinner=56,				// 56: 도박모드 1등 골드
			GambleSecondWinner=57,				// 56: 도박모드 2등 골드
			DWC = 58,							// 58: DWC 캐릭터 기본 소유 골드
		};
		enum ePetalChangeCode
		{
			GambleEnterPetal= 9,				// 9: 도박모드 페탈 입장료
			GambleFirstWinPetal= 12,			// 12: 도박모드 1등 페탈 상금
			GambleSecondWinPetal= 13,			// 13: 도박모드 2등 페탈 상금
		};
	};

	struct CoinTypeCode
	{
		enum eCoinTypeCode
		{
			Coin = 1,				// : 코인
			WarehouseCoin = 2,		// : 창고 코인
			RebirthCoin = 3,		// : 부활 코인
			PCBangRebirthCoin = 4,	// : PC방 부활 코인
		};
	};

	struct EquipmentAttributeCode
	{
		enum eEquipmentAttributeCode 
		{
			DelayTime = 1,	// : 딜레이 시간
			RemainTime = 2,	// : Remain 시간
		};
	};

	struct FatigueTypeCode
	{
		enum eFatigueTypeCode
		{
			PCBang = 1,		// : PC방 피로도
			Daily = 2,		// : 일일 피로도
			Weekly = 3,		// : 주간 피로도
			Event = 4,		// 이벤트 피로도
			VIP = 5,		// VIP피로도
		};
	};

	struct PublisherCode
	{
		enum ePublisherCode
		{
			Studio = 0,
			KR = 1,
			CH = 2,
			JP = 3,
			TW = 4,
			US = 5,
			SG = 6,
			TH = 7,
			ID = 8,
			RU = 9,
			EU = 10,
		};
	};

	struct QuestStatusCode
	{
		enum eQuestStatusCode
		{
			Gain = 0,		// : 퀘스트획득
			Progress = 1,	// : 퀘스트진행
		};
	};

	struct SkillPointCode
	{
		enum eSkillPointCode
		{
			LevelUp = 1,		// 스킬레벨업
			EternityItem = 2,	// 영구적용아이템
			Repair = 10,		// 걍 강제로 복구 세팅
		};
	};

	struct SkillChangeCode
	{
		enum eSkillChangeCode
		{
			Gain = 1,		// : 스킬포인트 획득
			Use = 2,		// : 스킬레벨업 & 스킬포인트 사용
			Reset = 3,		// : 스킬포인트 리셋
			GainByQuest = 4,	// : 스킬 획득 (퀘스트)
			GainByDungeon = 5,	// : 스킬 획득 (던전 보상)
			GainByBook = 6,		// : 스킬 획득 (스킬북)
			GainByEvent = 7,	// : 스킬 획득 (이벤트)
			GainByAdmin = 8,	// : 스킬 획득 (by 관리자)
			DelByDrop = 9,		// : 스킬 삭제 (드랍)
			DelByQuest = 10,	// : 스킬 삭제 (퀘스트)
			DelByEvent = 11,	// : 스킬 삭제 (이벤트 만료)
			DelByAdmin = 12,	// : 스킬 삭제 (by 관리자)
			ModSkillLevelByAdmin = 13,	// : 스킬레벨 조정 (by 관리자)
			GainByBuy = 14,		// 14스킬획득(구입)
		};
	};

	struct TradeStatusCode
	{
		enum eTradeStatusCode
		{
			Sell = 0,		// : 판매중
			CompleteSellByItem = 1,	// : 판매완료(아이템 수령)
			CompleteSellByCoin = 2,	// : 판매완료(판매금 수령)
			CancelSell = 3,	// : 판매취소
			Expire = 4,		// : 기간만료
		};
	};

	struct AddMaterializedItem
	{
		enum eCode
		{
			Pick			= 1,	// 1=줍기			-> FKey: PartyID or NULL
			Mission			= 2,	// 2=미션			-> FKey: MissionID
			QuestReward		= 3,	// 3=퀘스트보상		-> FKey: QuestID
			Present			= 4,	// 4=선물			-> FKey: PurchaseID
			PointBuy		= 5,	// 5=인게임포인트 구입	-> FKey: NPCID
			CashBuy			= 6,	// 6=캐쉬 구입		-> FKey: PurchaseID
			ReserveMoneyBuy	= 7,	// 7=페탈 구입	-> FKey: PurchaseID
			GetGachaResultCashItem_JP = 9,	// 9=일본 가챠 돌려서 나온 캐쉬 아이템.
			GMGive			= 10,	// 10=관리자 지급	-> FKey: AuditLogID
			ItemDecompose	= 11,	// 11=아이템 분해	-> FKey: NPCID
			// ItemEnchantFail = 12,	// 12=아이템 강화 실패	-> FKey: ItemEnchantID
			ItemCompound	= 13,	// 조합
			RandomItem		= 14,	// 랜덤아이템
			DungeonReward	= 15,	// 던젼보상
			PvPReward		= 16,	// pvp보상
			CharacterCreate = 17,	// 기본지급
			SystemMail = 18,		// 시스템메일 (미션보상)
			FixedItem = 19,			// 캐쉬템 (얼굴, 머리) - 기존얼굴/머리 지우고 덮어쓰기 -> FKey: 캐쉬템 사용아이템시리얼 (인벤에 있는애)
			CostumeMix = 21,		// 코스튬 합성
			FishingReward = 22,		// 낚시보상
			Harvest = 23,			// 수확
			Cheat = 24,				// 치트
			PvPLadderPoint = 25,	// 래더포인트
			UnionPoint = 26,		// 연합포인트
			Trigger = 27,			// 트리거
			GuildWarFestivalPoint = 28, // 길드 축제 포인트
			Repurchase = 29,
			GuildReversionItem = 30,
			CostumeRandomMixItem = 31,
			BestFriendItem = 34,
			COMEBACKREWARD = 35,	// 복귀유저보상
			SpecialBoxReward = 36,
			NewbieGameQuitReward = 37, //신규 유저 접속 종료할 때 보상
			NewbiewReConnectReward = 38, //신규 유저 재접속시 보상
			ComebackGameQuitReward = 39, //귀환자 접속 종료할 때 보상
			SeedPointBuy = 40, //시드로 구입
		};
	};

	struct MailTypeCode
	{
		enum eCode
		{
			Normal					= MailType::NormalMail,			// 일반 메일
			Mission					= MailType::MissionMail,		// 미션 보상
			Admin					= MailType::AdminMail,			// 관리자 메일
			EventMail				= MailType::EventMail,			// 이벤트 메일
			VIP						= MailType::VIPMail,			// VIP
			MasterSystem			= MailType::MasterSystemMail,	// 사제시스템
			NpcReputaion			= MailType::NpcReputationMail,	// NPC호감도시스템
			Quest					= MailType::Quest,				// 퀘스트
			GuildWarReward  		= MailType::GuildWarReward,		// 길드전 보상
			SpecialBox_Account		= MailType::SpecialBox_Account,	// 특수보관함_계정타겟 
			SpecialBox_Character	= MailType::SpecialBox_Character,	// 특수보관함_현캐릭터타겟
			Cadge					= MailType::Cadge,				// 조르기
			GuildMaxLevelReward		= MailType::GuildMaxLevelReward,// 최초길드가입 만랩 길드 보상
			AppellationBookReward	= MailType::AppellationBookReward, // 칭호 컬렉션북 완료 보상
		};
	};

	struct UseItem
	{
		enum eUseItemCode
		{
			DoNotDBSave = 0,
			Use = 1,		// 1=소모성 아이템 사용
			Destroy = 2,	// 2=버리기
			DeCompose = 4,	// 4=분해
			Present = 5,	// 5=선물
			GuildReversionItem = 6,	// 6=길드탈퇴나 추방으로 길드귀속아이템 삭제
		};
	};

	struct ItemLocation
	{
		enum eCode
		{
			Equip = 1,
			Inventory = 2,
			Warehouse = 3,
			QuestInventory = 4,
			CashInventory = 5,
			GiftRepository = 6,
			CashEquip = 7,
			Glyph = 8,
			VehicleInventory = 9,
			PetInventory = 10,
			HarvestDepot = 11,	// 수확창고
			ServerWare = 17,	// 서버 일반 아이템 창고
			ServerWareCash = 18,	// 서버 캐쉬 아이템 창고
			Talisman = 19,	// 탈리스만
			PeriodInventory = 20,
			PeriodWarehouse = 21,
		};
	};

	struct PetType
	{
		enum eCode
		{
			Vehicle = 1,
			Pet = 2,
			SATIETY_PET = 3,
		};
	};

	struct PayMethodCode
	{
		enum eCode
		{
			Coin = 1,				// 1: 코인
			Cash = 2,				// 2: 캐쉬
			Petal = 3,				// 3: 적립금
			Coupon = 4,				// 4: 쿠폰
			Admin = 5,				// 5: 관리자 지급
			Quest = 6,				// 6: 퀘스트보상
			Mission = 7,			// 7: 미션보상
			LevelupEvent = 8,		// 8=캐릭터 레벨 업 보상
			VIP = 9,				// 9 : VIP 선물
			PvP = 10,				// 10 : PVP 래더 포인트
			Union_Commerical = 11,	// 11 : 상인 연합 포인트
			Union_Royal = 12,		// 12 : 왕성 연합 포인트
			Union_Liberty = 13,		// 13 : 자유 연합 포인트
			Cash_NexonUSA = 14,		// 14 : 넥슨아메리카 캐쉬
			Event = 15,				// 15 : 이벤트
			GuildWar_Festival = 16,	// 16 : 길드전 축제 포인트
			GuildWar_Reward = 17,	// 17 : 길드전 보상
			Comeback = 19,			// 18 : 복귀유저보상
			BestFriend = 20,		// 20 : 절친
			BeginnerGuild = 21,		// 21 : 초보자길드 선물
			GuildSupport = 22,		// 22 : 길드지원보상
			ActiveMission = 23,		// 23 : 액티브미션 보상
#if defined( PRE_ADD_STAMPSYSTEM )
			Stamp = 24,				// 24 : 스탬프 보상
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined( PRE_ADD_NEW_MONEY_SEED )
			SeedPoint = 25,			// 25 : 시드로 구입
#endif
		};
	};

	struct CashRebirthCode
	{
		enum eCode
		{
			Cash = 1,
			Petal = 2,
			Admin = 3,
			Item = 4,
			Seed = 5,
		};
	};

	struct EffectItemGetCode
	{
		enum eCode
		{
			Cash = 1,
			Petal = 2,
			Cooper = 3,
			Admin = 4,
			Union = 5,
			Item = 6,
			Guild = 7,
			BestFriend = 8,
			Seed = 9,
		};
	};

	struct EffectItemLifeSpanType
	{
		enum eCode
		{
			New = 0,
			Extend = 1,
			Update = 2,
		};
	};

	struct DifficultyCode
	{
		enum eCode
		{
			Easy = 1,
			Normal,
			Hard,
			Master,
			Abyss,
		};
	};

	struct ClearGradeCode
	{
		enum eCode
		{
			SSS = 10, 
			SS,
			S,
			A,
			B,
			C,
			D,
		};
	};

	struct RewardBoxCode
	{
		enum eCode
		{
			Wood = 1,
			Bronze,
			Silver,
			Gold,
		};
	};

	struct WhereToGoCode
	{
		enum eCode
		{
			StageEntrance = 1,
			Village,
		};
	};

	struct PvPModeCode
	{
		enum eCode
		{
			Round = 1,
			Respawn,
			Captain,
			IndividualRespawn,
			Zombie,
			Ladder1vs1,
			Ladder2vs2,
			Ladder3vs3,
			Ladder4vs4,
			GuildWar,
			Occupation,
			AllKill,
			Racing,
			Tournament,
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
			ComboExercise,
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
#if defined(PRE_ADD_DWC)
			DWCRegular,			
			DWCPractice,
#endif
		};
	};

	struct PvPTeamCode
	{
		enum eCode
		{
			A = 1,
			B,
		};
	};

	struct PvPResultCode
	{
		enum eCode
		{
			Win = 1,
			Lose,
			Draw,
			GiveUp,
		};
	};

	struct RestraintTargetCode
	{
		enum eCode
		{
			Account = 1,	// 계정
			Character,		// 캐릭터
		};
	};

	struct RestraintTypeCode
	{
		enum eCode
		{
			ConnectRestraint = 1,
			ChatRestraint,
			TradeRestraint,
		};
	};

	struct RestraintDolisReasonCode
	{
		enum eCode
		{
			DefaultCode = 110,
			AbuseRestraintCode = 111,
			AbuseTradeRestraintCode = 112,
		};
	};

	struct TransactorCode
	{
		enum eCode
		{
			Master = 1,
			Pupil,
		};
	};

	struct GetMasterCharacterCode
	{
		enum eCode
		{
			MyInfo = 1,				// 자신의 스승 정보
			MyMasterInfo,			// 내 스승 정보
			OptionalMasterInfo,		// 임의의 특정 스승 정보
		};
	};
#if defined( PRE_ADD_SECONDARY_SKILL )

	struct SecondarySkillTypeCode
	{
		enum eCode
		{
			ProductionSkill		= SecondarySkill::Type::ProductionSkill,		// 생산스킬
			ManufactureSkill	= SecondarySkill::Type::ManufactureSkill,		// 제작스킬
			CommonSkill			= SecondarySkill::Type::CommonSkill,			// 공용스킬
		};
	};
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

	struct AppellationKind
	{
		enum eAppellationKind
		{
			Select = 1,
			Cover = 2
		};
	};

	struct EtcPointCode
	{
		enum eCode
		{
			None = 0,
			PvP = 1,
			Union_Commercial = 2,
			Union_Royal = 3,
			Union_Liberty = 4,
			GuildWar_Festival = 5,	// 길드전 축제 포인트			
			SeedPoint = 6,			// 시드 머니
			Max,
		};
	};
	struct GuildWarRewardType
	{
		enum eGuildWarRewardType
		{
			GuildWarFestivalWin = 1,	// 길드축제우승
			GuildWarFestivalLose,		// 길드축제패배
			GuildWarFinalUnder4,		// 본선 4강미만
			GuildWarFinal4,				// 본선 4강보상
			GuildWarFinal2,				// 본선 2위보상
			GuildWarFinalWin,			// 본선 우승보상
			GuildWarFinalAbnormalLose,	// 본선 비정상패배(동점때 예선점수?)
		};
	};

	struct EventRewardType
	{
		enum eEventRewardType
		{
			Admin = 1,
			System = 2,
		};
	};
};

//Exception Report
enum eExceptionRepert
{
	_EXCEPTIONTYPE_NONE = 0,

	//for game
	_EXCEPTIONTYPE_ROOMCRASH,
	_EXCEPTIONTYPE_SESSIONCRASH,
	_EXCEPTIONTYPE_ROOMDESTRUCTOR,
	_EXCEPTIONTYPE_GAME_DELAYED,

	//for db
	_EXCEPTIONTYPE_DBMW_DELAYED,
};

//GameServerAffinity Type
enum eGameServerAffinityType
{
	_GAMESERVER_AFFINITYTYPE_NORMAL = 0,		//그냥 일반 던전이미 머니 그런거
	_GAMESERVER_AFFINITYTYPE_HYBRYD,			//피비피도 받고 다 받아요
	_GAMESERVER_AFFINITYTYPE_PVP,				//피비피만 받아요
	_GAMESERVER_AFFINITYTYPE_FARM,				//게임서버 월드 통합처리용 농장 전용 겜섭!
};

// Cash
enum eCashEtc
{
	Reason_GameClient = 1,
	Reason_Web = 2,
	Reason_PCBangGameClient = 3,
	Reason_PCBangWeb = 4,

	Category_Main = 1,
	Category_Costume = 2,
	Category_Item = 3,
	Category_Package = 4,

	CashItemState_None = 1,
	CashItemState_Hot = 2,
	CashItemState_New = 3,
	CashItemState_Normal = 4,
	CashItemState_Discount = 5,
};

#if defined(_CH)
enum eChSndaAuthFlag
{
	eChSndaAuthFlag_UseEKey		= (1 << 0),		// EKey 기능 사용 여부
	eChSndaAuthFlag_UseECard	= (1 << 1),		// ECard 기능 사용 여부
};
#endif	// _CH

// structure -----------------------------------------------------------------------------------------------

struct TChannelInfo
{
	USHORT nChannelID;			//할당된 채널(필드)아이디
	USHORT nChannelIdx;
	USHORT nMapIdx;				//담당하는 맵의 아이디
	USHORT nChannelMaxUser;
	USHORT nCurrentUser;
	int nAttribute;				//DirtyBit
	int nMeritBonusID;			//Merit bonus
	bool bVisibility;
	WCHAR wszMapName[MAPNAMELENMAX];
	int	nLimitLevel;
	bool bShow;
	USHORT nDependentMapID;
	int	nServerID;
	BYTE cThreadID;
#if defined(PRE_ADD_CHANNELNAME)
	WCHAR wszLanguageName[CHANNELLANGUAGEMAX];
#endif //#if defined(PRE_ADD_CHANNELNAME)
	int GetUserPercentage() const
	{
		if( nChannelMaxUser > 0 )
			return nCurrentUser*100/nChannelMaxUser;
		return -1;
	}
};

struct TVillageInfo
{
	BYTE cVillageID;
	int nManagedID;
	char szIP[IPLENMAX];
	USHORT nPort;

	std::vector <TChannelInfo> vOwnedChannelList;
	TVillageInfo()
	{
		cVillageID = 0;
		nManagedID = 0;
		memset(szIP, 0, sizeof(szIP));
		nPort = 0;
	}
};

struct TGameInfo
{
	int nServerID;
	int nServerAttribute;
	ULONG nIP;
	USHORT nPort;
	USHORT nTcpPort;

	int nRoomCnt;
	int nUserCnt;
	bool bHasMargin;
	UINT nFrame;

	bool bComplete;
	UINT uiLastDelayReportTick;
};

// Server Info
struct TServerInfo
{
	bool bMergedWorld;
	int nWorldSetID;							// 통합 월드 아이디
	char cWorldID;
	WCHAR wszWorldName[WORLDNAMELENMAX];
	UINT nWorldMaxUser;				//대기자용
	USHORT nDefaultMaxUser;
	bool bOnline;				// online, offline 상태표시용
	bool bOnTop;
	std::vector <TVillageInfo> vOwnedVillageList;

	TServerInfo() 
	{
		bMergedWorld = false;
		nWorldSetID = 0;
		cWorldID = 0;
		memset(wszWorldName, 0, sizeof(wszWorldName));
		nWorldMaxUser = 0;
		nDefaultMaxUser = 0;
		bOnline = false;
		bOnTop = false;
	}
};

// 
struct TChannelInfoEx
{
	TChannelInfo Channel;
	BYTE cVillageID;
	char szIP[IPLENMAX];
	USHORT nPort;
};
typedef std::map<int, TChannelInfoEx> TMapChannel;

// 서버에서는 float로 바로 쓰지않는다. 클라에서 받은 좌표에 *1000을 해서 사용함
struct TPosition
{
	int nX;
	int nY;
	int nZ;
};

struct TLocation
{
	UINT nMapIndex;
	TPosition Pos;
};

// npc, user object생성에 해당. 공통된 부분만 
struct TBaseData
{
	UINT nObjectID;
	WCHAR wszName[NAMELENMAX];
	int nChannelID;
	TPosition CurPos, TargetPos;
	TPosition MoveDir;
	float fRotate;
	bool bBattleMode;
};

struct TParamData
{
	eChatType eChatType;
	int nChatSize;
	WCHAR wszChatMsg[CHATLENMAX];
	char Buffer[128];
	int nBufSize;
	USHORT wActorProtocol;
	TItemInfo ItemInfo;
	TItemInfo CashItemInfo;
	BYTE cLevel;
	UINT nExp;
	int nJob;
	int nSelectAppellation;
	int nCoverAppellation;
	bool bHide;
	bool bEncrypt;
	DNEncryptPacketSeq	sEncrypt;
	char cType;
	char cState;
	DWORD dwColor;
	int nChatRoomParam;
	int nIndex;
	DWORD dwSendSessionID; // SendSessionID
	TItem Vehicle;
	TVehicleCompact PetInfo;
	char cPetPartsType;
};

struct TDefaultCreateData
{
	BYTE cClassID;										// 클래스 ID
	int nCreateMapIndex;								// 처음 생성시 시작 맵 인덱스
	int nCreateMapStartPositionX[DEFAULTPOSITIONMAX];	// 처음 생성시 시작 위치 X
	int nCreateMapStartPositionY[DEFAULTPOSITIONMAX];	// 처음 생성시 시작 위치 Y
	int nCreateMapStartRadius[DEFAULTPOSITIONMAX];		// 반경

	int nCreateTutorialMapIndex;			// 처음 생성시 튜토리얼 맵 인덱스 (GameServer쪽)
	int nCreateTutorialGateNo;				// 처음 생성시 튜토리얼 게이트 (GameServer쪽)

	int nDefaultBody;						// 기본몸
	int nDefaultLeg;						// 기본다리
	int nDefaultHand;						// 기본손
	int nDefaultFoot;						// 기본발
	int nFace[DEFAULTPARTSMAX];				// 얼굴
	int nHair[DEFAULTPARTSMAX];				// 머리
	int nHelmet[DEFAULTPARTSMAX];			// 헬멧
	int nBody[DEFAULTPARTSMAX];				// 상의
	int nLeg[DEFAULTPARTSMAX];				// 하의
	int nHand[DEFAULTPARTSMAX];				// 손
	int nFoot[DEFAULTPARTSMAX];				// 발
	int nWeapon[WEAPONMAX];					// 무기

	DWORD dwHairColor[DEFAULTHAIRCOLORMAX];
	DWORD dwSkinColor[DEFAULTSKINCOLORMAX];
	DWORD dwEyeColor[DEFAULTEYECOLORMAX];

	int nDefaultItemID[DEFAULTITEMMAX];		// 소유아이템
	int nDefaultItemCount[DEFAULTITEMMAX];	// 소유아이템 개수

	int nDefaultSkillID[DEFAULTSKILLMAX];	// 소유스킬

	TQuickSlot DefaultQuickSlot[DEFAULTQUICKSLOTMAX];	// 퀵슬롯개수
	int nDefaultGestureID[DEFAULTGESTUREMAX];	// 소유 제스처
};

#if defined( PRE_ADD_DWC )
struct TDWCCreateData
{
	BYTE cClassID;										// 기본 클래스 ID
	BYTE cJobCode1;										// 1차 전직 클래스 ID
	BYTE cJobCode2;										// 2차 전직 클래스 ID
	BYTE cLevel;
	int nExp;
	int nCreateMapIndex;								// 처음 생성시 시작 맵 인덱스
	int nCreateMapStartPositionX[DEFAULTPOSITIONMAX];	// 처음 생성시 시작 위치 X
	int nCreateMapStartPositionY[DEFAULTPOSITIONMAX];	// 처음 생성시 시작 위치 Y
	int nCreateMapStartRadius[DEFAULTPOSITIONMAX];		// 반경

	int nDefaultBody;						// 기본몸
	int nDefaultLeg;						// 기본다리
	int nDefaultHand;						// 기본손
	int nDefaultFoot;						// 기본발
	int nEquipArray[EQUIPMAX]; // 장착 장비

	DWORD dwHairColor;
	DWORD dwSkinColor;
	DWORD dwEyeColor;

	int nDefaultItemID[DEFAULTITEMMAX];		// 소유아이템
	int nDefaultItemCount[DEFAULTITEMMAX];	// 소유아이템 개수

	int nDefaultSkillID[DEFAULTSKILLMAX];	// 소유스킬

	TQuickSlot DefaultQuickSlot[DEFAULTQUICKSLOTMAX];	// 퀵슬롯개수
	int nDefaultGestureID[DEFAULTGESTUREMAX];	// 소유 제스처
	short nSkillPoint; // 스킬 포인트
	int nGold; // 골드 - 필요 없다면 제거하자

};
#endif // #if defined( PRE_ADD_DWC )

struct TMapInfo
{
	int nMapID;
	char szMapName[10][256];
	BYTE GateMapCount[WORLDMAP_GATECOUNT];
	USHORT GateMapIndexs[WORLDMAP_GATECOUNT][WORLDMAP_GATECOUNT];
	BYTE GateStartIndexs[WORLDMAP_GATECOUNT][WORLDMAP_GATECOUNT];
	int nWorldLevel;
	GlobalEnum::eMapTypeEnum MapType;
	GlobalEnum::eMapSubTypeEnum MapSubType;
	int nMaxClearCount;
	int nAllowMapType;
	std::vector<int> vMapAreaIndex;
	bool bExpandable;
	bool bUserReturnSystem;	// 유저 복귀시스템
	bool bVehicleMode;
	bool bPetMode;
	bool bAllowFreePass;
#if defined( PRE_PARTY_DB )
	std::wstring wstrMapName;
	int iEnterConditionTable;
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_NORMALSTAGE_REGULATION )
	char cRevisionNum;	//보정넘버
#endif
#if defined(PRE_ADD_VEHICLE_SPECIAL_ACTION)
	bool bVehicleSpecalActionMode;
#endif
};

struct TCoinCountData
{
	BYTE cLevel;
	int nRebirthCoin[WORLDCOUNTMAX];		// 부활코인
	int nCashRebirthCoin[WORLDCOUNTMAX];	// 부활캐쉬코인
	int nRebirthCoinLimit;					// 무료코인소지제한개수
	int nCashRebirthCoinLimit;				// 유료코인소지제한개수
	int nVIPRebirthCoin;					// VIP부활코인
};

struct TActorData
{
	int nActorID;
	BYTE cClass;					// 유닛 타입 종류
};

const int QUEST_MAX_CNT = 20;
struct TNpcData
{
	int nNpcID;
	int nParam[2];
	WCHAR wszName[EXTDATANAMELENMAX];
	char szTalkFile[256];				// 대사파일이름
	char szScriptFile[256];				// script 파일이름
	int QuestIndexArr[QUEST_MAX_CNT];	// 퀘스트인덱스
	TActorData ActorData;
};

struct TItemData
{
	int nItemID;
	WCHAR wszItemName[EXTDATANAMELENMAX];
	int nType;							// 타입
	int nTypeParam[3];					// 타입파라미터
	BYTE cLevelLimit;					// 레벨제한
	BYTE cRank;							// 등급
	BYTE cReversion;					// 귀속여부
	bool IsCash;						// 캐쉬템인가
	bool IsDestruction;					// 파괴여부
	bool IsAuthentication;				// 인증여부
	int nAmount;						// 금액
	int nSellAmount;					// 판매금액
	int nOverlapCount;					// 중첩개수
	std::vector<int> nNeedJobClassList;	// 직업제한

	int nSkillID;						// 스킬 인덱스
	char cSkillLevel;					// 스킬 레벨
	char cSkillUsingType;				// 스킬 사용 타입
	int nMaxCoolTime;					// 최대 쿨타임 (변하지 않는 값)
	int nEnchantID;						// 인챈트아이디
	int	nAllowMapType;					// 사용가능한 맵 타입
	char cSealCount;					// 밀봉 횟수
	int nSealID;						// SealCountTableID
	int nNeedBuyItemID;					// 상점에서 구입할 때 필요한 아이템ID
	int nNeedBuyItemCount;				// 상점에서 구입할 때 필요한 아이템개수
	int nNeedPvPRank;					// 장착할 때 필요한 PvP 랭크
	bool IsSealed;						// 획득 시 밀봉여부
	int nCashTradeCount;				// 캐시아이템 교환개수
	bool IsEnableCostumeMix;
	int nApplicableValue;
	bool IsCollectingEvent;			// 아이템 수집 API를 호출여부
	int nExchangeCode;				// 아이템 카테고리 ID
	int iNeedBuyLadderPoint;		
	int iUseLevelLimit;
	int iNeedBuyUnionPoint;
	int iNeedBuyGuildWarPoint;
	bool bRebuyable;
	int nPeriod;
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	bool IsWStorage;					// 서버 창고에 넣을 수 있는 예외
#endif

	void Reset() 
	{
		memset( wszItemName, 0, sizeof(wszItemName) );
		memset( nTypeParam, 0, sizeof(nTypeParam) );
		nNeedJobClassList.clear();
		std::vector<int>().swap(nNeedJobClassList);

		nItemID			= 0;
		nType			= 0;
		cLevelLimit		= 0;
		cRank			= 0;
		cReversion		= 0;
		IsCash			= 0;
		IsDestruction	= 0;
		IsAuthentication= 0;
		nAmount			= 0;
		nSellAmount		= 0;
		nOverlapCount	= 0;
		nSkillID		= 0;
		cSkillLevel		= 0;
		cSkillUsingType = 0;
		nMaxCoolTime	= 0;
		nAllowMapType	= 0;
		cSealCount		= 0;
		nNeedBuyItemID		= 0;
		nNeedBuyItemCount	= 0;
		nNeedPvPRank		= 0;
		IsSealed			= 0;
		nCashTradeCount = 0;
		IsEnableCostumeMix	= 0;
		nApplicableValue = 0;
		IsCollectingEvent = 0;
		nExchangeCode = 0;
		iNeedBuyUnionPoint = 0;
		iNeedBuyGuildWarPoint = 0;
		bRebuyable = false;
		nPeriod = 0;
	}
};

struct TWeaponData
{
	int nWeaponIndex;
	char cEquipType;					// 장착타입
	int nLength;						// 무기 길이
	int nDurability;					// 내구도
	int nDurabilityRepairCoin;			// 수리비
	int nPermitJob[10];
	bool bOneType;
};

struct TPartData
{
	int nPartIndex;
	int nParts;							// 파츠
	int nDurability;					// 내구도
	int nDurabilityRepairCoin;			// 수리비
	int nPermitJob[10];
	std::vector<int> nSubPartsList;		// 서브파츠 (한벌옷)

	TPartData()
	{
		nPartIndex = 0;
		nParts = 0;
		nDurability = 0;
		nDurabilityRepairCoin = 0;
		memset(nPermitJob, 0, sizeof(nPermitJob));
	}
};

struct TVehiclePartsData
{
	int nItemID;
	int nVehicleClassID;
	int nVehiclePartsType;
	std::string strSkinName;
	std::string strAniName;
	std::string strActName;
	int nSkinColor;
	TVehiclePartsData()
	{
		nItemID = 0;
		nVehicleClassID = 0;
		nVehiclePartsType = 0;
		nSkinColor = 0;
	}
};

struct TVehicleData
{
	int nItemID;
	int nVehicleClassID;
	int nVehicleActorID;
	int nDefaultBody;
	int nDefaultParts1;
	int nDefaultParts2;
	int nVehicleSummonTime;
	int nVehicleDefaultSpeed;
	int nPetLevelTypeID;	// pet 등급(?)값 등급이 다른 펫을 참조하기위한 키값
	int nPetSkillID1;		// 
	int nPetSkillID2;
	int nRange;				// 스캔 범위값. 이를테면 줍기 같은거 
	int nFoodID;
};

const BYTE PetStateMax = 10;
struct TPetLevelDetail
{
	int nTableIndex;
	int nPetLevel;
	int nPetExp;
	int nState[PetStateMax];
	char *szStateMin[PetStateMax];
	char *szStateMax[PetStateMax];
	int nVipExp;
};

struct TPetLevelData
{
	int nPetLevelType;
	std::vector<TPetLevelDetail> VecPetLevel;
};

struct TPetFoodData
{
	int nItemID;
	int nTickTime; // 소모 시간(second)
	int nFullTic;  // 틱당 소모량
	BYTE cFullTimeLogOut; // 로그아웃시 기존 만복도 나누기값
	int nFullMaxCount;		// 만복도 최대 max수치
	int nUseExp[Pet::MAX_SATIETY_SELECTION];
	int nUseState[Pet::MAX_SATIETY_SELECTION];
};

struct TPotentialJewelData
{
	int nItemID;
	int nApplyApplicableValue;
	int nPotentialNo;
	bool bErasable;
	bool bCanApplySealedItem;

#if defined(PRE_ADD_REMOVE_PREFIX)
	bool bSuffix;
#endif // PRE_ADD_REMOVE_PREFIX
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	int nRollbackAmount; // 잠재력 부여 롤백 수수료
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
};

struct TEnchantJewelData
{
	int nItemID;
	int nApplyApplicableValue;
	int nRequireMaxItemLevel;
	int nRequireEnchantLevel;
	int nEnchantLevel;
	bool bCanApplySealedItem;
};

struct TItemDropData
{
	int nItemDropID;
	bool bIsGroup[20];
	int nIndex[20];
	int nProb[20];
	int nInfo[20];
};

struct TGlobalWeightData
{
	int nID;
	float fValue;
};

struct TGlobalWeightIntData
{
	int nID;
	int nValue;
};

struct TLevelData
{
	int nIndex;
	short wStrength;					// 힘
	short wAgility;						// 민첩
	short wIntelligence;				// 지능
	short wStamina;						// 건강
	int nExperience;					// 경험치
	int nFatigue;						// 피로도
};

struct TStat
{
	USHORT wStr;
	USHORT wDex;
	USHORT wInt;
	USHORT wHealth;
	USHORT wLuck;
};

struct TSkillLevelData
{
	char cSkillLevel;					// 스킬 레벨
	char cLevelLimit;					// 제한 레벨
	//int nNeedJobClass;				// 직업제한 -> 스킬 테이블로 옮겨졌음.
	int nDelayTime;						// 스킬딜레이(ms)
	int nAddRange;						// 추가 사거리(cm)
	int nNeedItemID;					// 소모아이템
	int nNeedItemDecreaseCount;			// 소모 아이템 개수
	int nDecreaseHP;					// 소모HP
	int nDecreaseSP;					// 소모SP
	int nNeedSkillPoint;
	int nEffectClassValue1Duration;
};

struct TSkillData
{
	int nSkillID;						// 스킬id
	char cNeedWeaponType[ 2 ];			// 필요무기
	int nMaxLevel;						// 최대레벨
	char cSkillType;					// 스킬타입
	char cDurationType;					// 지속타입
	char cTargetType;					// 타겟타입
	char cDissolvable;					// 해제 가능 여부
	int nNeedJobID;						// 직업 제한
	bool bDefaultLocked;				// 디폴트로 락이 된 건지 여부
	int nUnlockSkillBookItemID;			// 언락하는 스킬북의 아이템 ID
	int nUnlockPrice;					// 돈으로 언락할 때 비용.
	int nExclusiveID;					// 이 아이디가 같은 값인 스킬은 같이 배울 수 없다.
	int nGlobalCoolTime;				// 글로벌 스킬 쿨타임 (#36858)
	int nGlobalSkillGroup;				// 글로벌 스킬 그룹 (#36858)
	std::vector<TSkillLevelData> vLevelDataList;

	TSkillData()
	{
		nSkillID =0;
		memset(cNeedWeaponType, 0, sizeof(cNeedWeaponType));		
		nMaxLevel = 0;
		cSkillType = 0;
		cDurationType = 0;
		cTargetType = 0;
		cDissolvable = 0;
		nNeedJobID = 0;
		bDefaultLocked = false;
		nUnlockSkillBookItemID = 0;
		nUnlockPrice = 0;
		nExclusiveID = 0;
		nGlobalCoolTime = 0;
		nGlobalSkillGroup = 0;
	}
};

struct TSkillTreeData
{
	int nSkillID;
	int nTreeSlotIndex;
	int nParentSkillID[ 3 ];
	int nNeedParentSkillLevel[ 3 ];
#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
	int nNeedBasicSP;
	int nNeedFirstSP;
	int nNeedSecondSP;
#endif	//	#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
#if defined( PRE_ADD_ONLY_SKILLBOOK )
	bool bNeedSkillBook;
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )
};

// npc 대화 관련 구조체 
struct TALK_QUESTION
{
	std::wstring szQuestion;
};

struct TALK_ANSWER 
{
	std::wstring szLinkTarget;
	std::wstring szLinkIndex;
	std::wstring szAnswer;
	bool bOtherTargetLink;	// 다른 토크파일로 링크 하는 경우 true
};

struct TALK_PARAGRAPH
{
	std::wstring				szIndex;
	TALK_QUESTION				Question;
	std::vector<TALK_ANSWER>	Answers;
};

// key - 문단스트링인덱스, second - 문단
typedef std::map<std::wstring, TALK_PARAGRAPH> TALK_PARAGRAPH_MAP;
typedef TALK_PARAGRAPH_MAP::iterator TALK_PARAGRAPH_MAP_IT;

// key - 대사xml 파일 이름, second - 대사전체
typedef std::map<std::wstring, TALK_PARAGRAPH_MAP> TALK_MAP;
typedef TALK_MAP::iterator TALK_MAP_IT;

#include <set>

// key - 문단응답스트링인덱스 (단지 클라이언트 대사요청 무결성 체크 오류메시지를 막는 용도)
typedef std::set<std::wstring> TALK_ANSWER_SET;
typedef TALK_ANSWER_SET::iterator TALK_ANSWER_SET_IT;

// key - 대사xml 파일 이름, second - 문단응답스트링집합 (단지 클라이언트 대사요청 무결성 체크 오류메시지를 막는 용도)
typedef std::map<std::wstring, TALK_ANSWER_SET>	TALK_ANSWER_MAP;
typedef TALK_ANSWER_MAP::iterator TALK_ANSWER_MAP_IT;

// 퀘스트 보상 아이템 테이블 정보
// 돈과 아이템 6종류 까지 세팅할 수 있다. nItemID 과 nItemCount는 세트이다.

struct TQuestRewardBase
{
	struct _ITEMSET
	{
		int	nItemID;
		int	nItemCount;
		int	nItemOptionTableID;
	};
};

struct TQuestReward : public TQuestRewardBase
{
	UINT nIndex;			// 보상테이블 인덱스
	UINT nStringIndex;	// 보상테이블 스트링
	char cType;			// 보상타입 1이면 전체보상 / 2면 선택보상
	char cSelectMax;		// 선택보상일경우 몇개까지 선택할것인지.
	UINT nCoin;			// 보상금액
	int	nExp;			// 보상 경험치
	int	nQuestID;		// 퀘스트 ID
	int	nQuestStep;		// 퀘스트 완료 시 스텝
	BYTE nClass;			// 보상 클래스
	_ITEMSET ItemArray[QUESTREWARD_INVENTORYITEMMAX];
	_ITEMSET LevelCapItemArray[MAX_QUEST_LEVEL_CAP_REWARD];
	int nMailID;
	int nFatigue;
};

struct TQuestLevelCapReward : public TQuestRewardBase
{
	UINT uIndex;
	char cType;
	char cClassID;
	_ITEMSET Items[MAX_QUEST_LEVEL_CAP_REWARD];
};

struct TCompleteEventQuest
{
	short wCompleteCount;
	int nScheduleID;
};

// Shop
struct TShopItem
{
	int nItemID;		// 상점 아이템 아이디
	int nCount;			// 상점 아이템 수량
	int nMaxCount;		// 아이템 최대수량
	int nPrice;			// 아이템 가격
	int buyLimitCount;
#if defined (PRE_ADD_COMBINEDSHOP_PERIOD)
	int nPeriod;
#endif
#if defined( PRE_ADD_LIMITED_SHOP )
	int nShopLimitReset;
#endif
};

struct TShopTabData
{
	int nTabID;
	std::vector<TShopItem> vShopItem;	// SHOPTABMAX
	TShopTabData()
	{
		Clear();
	}
	void Clear()
	{
		nTabID = 0;
		vShopItem.clear();
	}
};

struct TShopData
{
	int nShopID;		// 샵 아이디 (npc가 가지고 있는 아이디와 비교해서 연결)
	std::vector<TShopTabData> vShopTabData;	// SHOPITEMMAX
	TShopData()
	{
		nShopID = 0;
	}
};

// Skill Shop
struct TSkillShopItem
{
	int nSkillID;		// 스킬아이디
	int nPrice;			// 스킬 가격
};

struct TSkillShopData
{
	int nShopID;		// 샵 아이디 (npc가 가지고 있는 아이디와 비교해서 연결)
	TSkillShopItem SkillShopItem[SKILLSHOPITEMMAX];	// 최대 10개밖에 안된다
};

// Enchant
const int ENCHANTITEMMAX = 5;
struct TEnchantLevelData
{
	int nItemID;
	char cLevel;
#ifdef PRE_FIX_MEMOPT_ENCHANT
	int nNeedItemTableID;
	int nStateTableID;
	int nNeedCoin;
#else
	char cEnchantProb;
	char cBreakRatio;
	char cMinDownLevel;
	char cMaxDownLevel;
	int nNeedItemID[ENCHANTITEMMAX];
	int nNeedItemCount[ENCHANTITEMMAX];
	int nNeedCoin;
	int nFailResultItemID;
	int nPotentialID;
	char cPotentialProb;
	int nProtectItemCount;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	int nShieldItemCount;	// 상위강화보호젤리용(다운그레이드x)
#endif
#endif // PRE_FIX_MEMOPT_ENCHANT
};

struct TEnchantData
{
	int nEnchantID;
	char cCount;
	std::map<char, TEnchantLevelData*> pMapLevelData;
};

#ifdef PRE_FIX_MEMOPT_ENCHANT
struct TEnchantNeedItemData
{
	char cEnchantProb;
	char cBreakRatio;
	char cMinDownLevel;
	char cMaxDownLevel;
	int nNeedItemID[ENCHANTITEMMAX];
	int nNeedItemCount[ENCHANTITEMMAX];
	float fUpStateRatio;
	int nFailResultItemID;
	int nProtectItemCount;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	int nShieldItemCount;	// 상위강화보호젤리용(다운그레이드x)
#endif
};
#endif // PRE_FIX_MEMOPT_ENCHANT

struct TPotentialDataItem
{
	int nItemID;
	int nPotentailOffset;
	int nPotentialNo;
	int nProb;

	int nSkillID;
	int nSkillLevel;
	int nSkillUsingType;
};

struct TPotentialData
{
	int nPotentialID;
	int nTotalProb;
	std::vector<TPotentialDataItem*> pVecItemData;
};

// Exchange
struct TExchangeData
{
	int nInvenIndex;	// 인벤 인덱스
	short wCount;		// 몇개 넣었는지
	INT64 biSerial;
};

#ifdef PRE_ADD_GACHA_JAPAN
// gachapon japan
struct TGachaponShopInfoByJob_JP
{
	int nGachaponTableID;		// 가챠폰 테이블 ID
	int nGachaponIndex;			// 가챠폰 샵 인덱스.
	int nJobClassID;			// 이 가챠폰에서 드랍해주는 아이템들의 직업
	int nPartsLinkDropTableID[ CASHEQUIPMAX ];
};

struct TGachaponData_JP
{
	int nGachaponIndex;			// 가챠폰 샵 인덱스. npc 테이블에 쓰인 인덱스를 이걸로 찾음. 테이블 인덱스 아님.
	TGachaponShopInfoByJob_JP GachaponShopInfo[ CLASSKINDMAX ];
};
#endif // PRE_ADD_GACHA_JAPAN

struct TCostumeMix
{
	int nMinScore;
	int nDropID;
};

//community
//getsimple characterdata patialy enum type
enum eReqCharacterPartialy
{
	_REQCHARACTERPARTIALY_FRIEND, 
};

//Friend
struct TFriendGroup:public TBoostMemoryPool<TFriendGroup>
{
	UINT nGroupDBID;
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];
};

struct TFriend:public TBoostMemoryPool<TFriend>
{
	UINT nFriendAccountDBID;
	INT64 biFriendCharacterDBID;
	UINT nGroupDBID;

	TCommunityLocation Location;
	
	WCHAR wszCharacterName[NAMELENMAX];
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];
	WCHAR wszFriendMemo[FRIEND_MEMO_LEN_MAX];
#ifdef PRE_ADD_DOORS
	bool bMobileAuthentication;
#endif		//#ifdef PRE_ADD_DOORS
};

//UserState
enum eCummunityState		//초대 받는다 안받는다 쪽지 받겠다 안받겠다 등등
{
	_COMMUNITY_NONE = 0,
	_COMMUNITY_MSG_PERMIT,		//모든 메세지 받아효~
	_COMMUNITY_MSG_NOTPERMIT,//메세지 안받아효~ 나좀 내버려 두삼!
};

struct sWorldUserState
{
	WCHAR wszCharacterName[NAMELENMAX];
	INT64 biCharacterDBID;			//일단은 혹쉬나 해서 훗훗
	int	nChannelID;				//
	int	nMapIdx;
	int	nLocationState;			//eLocationState
	int	nCummunityState;		//eCummunityState
};

//ServiceManager NoticeType
enum eSMNoticeType
{
	_NOTICETYPE_ALL = 0,
	_NOTICETYPE_WORLD,
	_NOTICETYPE_EACHSERVER,
	_NOTICETYPE_CHANNEL,
	_NOTICETYPE_ZONE,
};

//VoiceChannelReqType
enum eVoiceChannelReqType
{
	_VOICECHANNEL_REQTYPE_PARTY,
	_VOICECHANNEL_REQTYPE_PVP,
	//머 이런식으로 생기지 않을까나..
	//_VOICECHANNELTYPE_GUILD,
	//_VOICECHANNELTYPE_PRIVATE,
};

//Invite PartyInfo
struct TInvitePartyInfo
{
	TPARTYID PartyID;
	int nMapIdx;
	BYTE cLimitLevelMin;
#if defined( PRE_PARTY_DB )
#else
	BYTE cLimitLevelMax;
#endif // #if defined( PRE_PARTY_DB )
	BYTE cPartyMemberMax;
	BYTE cPartyMemberCount;
	BYTE cPartyAvrLevel;
	WCHAR wszPartyName[PARTYNAMELENMAX];
};

// PvP
struct TPvPMapTable
{
	UINT uiGameType;				// PvPCommon::GameType
	std::vector<UINT> vGameModeTableID;		// PvPGameModeTableID
	std::vector<UINT> vNumOfPlayerOption;		// 인원수 옵션
	bool bIsBreakInto;			// 난입가능Flag
	UINT uiItemUsageType;		// PvPCommon::ItemUsageType
	bool bIsAllowItemDrop;		// 아이템드롭 Flag
	bool bIsGuildBattleGround;	// 길드전에서 사용할수 있는지 Flag
	bool bIsReleaseShow;			// 릴리즈버전에서노출
	LadderSystem::MatchType::eCode	MatchType;

	// Function
	bool CheckMaxUser( const BYTE cMaxUser ) const
	{
		for( UINT i=0 ; i<vNumOfPlayerOption.size() ; ++i )
		{
			if( vNumOfPlayerOption[i] == cMaxUser )
				return true;
		}
		return false;
	}

	bool CheckGameModeTableID( const UINT uiTableID ) const
	{
		for( UINT i=0 ; i<vGameModeTableID.size() ; ++i )
		{
			if( vGameModeTableID[i] == uiTableID )
				return true;
		}
		return false;
	}

	bool CheckBreakIntoOption( const BYTE cBreakIntoFlag ) const
	{
		if( cBreakIntoFlag )
		{
			if( !bIsBreakInto )
				return false;
		}
		return true;
	}
};

struct TPvPGameModeTable
{
	int	nItemID;
	UINT uiGameMode;		// PvPCommon::GameMode
	std::vector<UINT> vWinCondition;
	std::vector<UINT> vPlayTimeSec;
	UINT uiRespawnTimeSec;
	UINT uiRespawnNoDamageTimeSec;
	UINT uiRespawnHPPercent;
	UINT uiRespawnMPPercent;
	UINT uiWinXPPerRound;
	UINT uiLoseXPPerRound;
	UINT uiItemDropTableID;
	bool bIsLevelRegulation;
	bool bIsReleaseShow;			// 릴리즈버전에서노출
	UINT uiMedalExp;
	UINT uiNumOfPlayersMin;
	UINT uiNumOfPlayersMax;
	std::vector<UINT> vVictoryExp;
	std::vector<UINT> vVictoryBonusRate;
	std::vector<UINT> vDefeatExp;
	std::vector<UINT> vDefeatBonusRate;
	LadderSystem::MatchType::eCode	LadderMatchType;
	int	iRewardItemID;
	int	nFatigueConTime;
	int	nFatigueConValue;
	int	nFatigueConExp;
	int	nFatigueConMedal;
	int	iNeedInven;
	int	iSkillSetting;
	bool bIsSelectableRegulation;
	bool bIsDefaultRegulation;
	int nBattleGroundTableID;
	bool bAllowedUserCreateMode;
#if defined( PRE_ADD_RACING_MODE ) || defined( PRE_ADD_PVP_TOURNAMENT )
	int nDefaultRewardItem;
	int nDefaultRewardValue;
	std::vector<int> vRankRewardItem;
	std::vector<int> vRankRewardValue;
#endif // #if defined( PRE_ADD_RACING_MODE ) || defined( PRE_ADD_PVP_TOURNAMENT )

	// Function
	bool CheckWinCondition( const UINT uiWinCondition ) const
	{
		if( vWinCondition.empty() )
			return true;

		for( UINT i=0 ; i<vWinCondition.size() ; ++i )
		{
			if( vWinCondition[i] == uiWinCondition )
				return true;
		}
		return false;
	}

	bool CheckPlayTimeSec( const UINT uiPlayTime ) const
	{
		if( vPlayTimeSec.empty() )
			return true;

		for( UINT i=0 ; i<vPlayTimeSec.size() ; ++i )
		{
			if( vPlayTimeSec[i] == uiPlayTime )
				return true;
		}
		return false;
	}

	// 실패시 -1 리턴
	int GetWinconditionIndex( const UINT uiWinCondition ) const
	{
		for( UINT i=0 ; i<vWinCondition.size() ; ++i )
		{
			if( vWinCondition[i] == uiWinCondition )
				return i;
		}
		return -1;
	}

	bool CheckMaxUser( const UINT uiMaxUser ) const 
	{
		return (uiMaxUser >= uiNumOfPlayersMin && uiMaxUser <= uiNumOfPlayersMax);
	}
};

struct TPvPGameStartConditionTable
{
	UINT uiMinTeamPlayerNum;		// 최소 시작 팀 인원
	UINT uiMaxTeamPlayerDiff;	// 최대 양팀 인원 허용 수

	short Check( const UINT uiATeam, const UINT uiBTeam, const bool bRandomTeam ) const
	{
		// 최소 시작 팀 인원 검사
		if( uiATeam < uiMinTeamPlayerNum || uiBTeam < uiMinTeamPlayerNum )
			return ERROR_PVP_CANTSTART_LESS_MINTEAMPLAYER;

		if (!bRandomTeam)	// 랜덤팀이 아닌 경우 처리한다.
		{
			// 최대  양팀 인원 허용 수 검사
			UINT uiDiff = ::abs( static_cast<int>(uiATeam-uiBTeam) );
			if( uiDiff > uiMaxTeamPlayerDiff )
				return ERROR_PVP_CANTSTART_MORE_MAXDIFFTEAMPLAYER;
		}
		

		return ERROR_NONE;
	}
};

struct TPvPRankTable
{
	UINT uiXP;
#ifdef PRE_MOD_PVPRANK
	BYTE cType;			//PvPCommon::RankTable 참조
	BYTE cMinRange;		//rate or rank
	BYTE cMaxRange;		//rate or rank

	TPvPRankTable()
	{
		uiXP = 0;
		cType = PvPCommon::RankTable::InitValue;
		cMinRange = 0;
		cMaxRange = 0;
	}
#endif		//#ifdef PRE_MOD_PVPRANK
};

struct TAppellationData
{
	int nAppellationID;
	int nArrayIndex;
	int nColor;
	int nLevelLimit;
	int nType;
#if defined( PRE_ADD_NEWCOMEBACK )
	int nSkillItemID;	
#endif
#if defined( PRE_MOD_APPELLATIONBOOK_RENEWAL)
	std::vector<int> nUseCollectionBookIndex;
#endif	// #if defined( PRE_MOD_APPELLATIONBOOK_RENEWAL)
};

// 미션 시스템 관련
struct TEventCondition
{
	char cEvent;
	char cType[5];
	int  nParam[5];
	char cOperator[5];
	bool bCheckType;
};

struct TMissionData
{
	bool bActivate;
	int nType;			// 미션타입(eMissionType)
	int nMailID;
	int nRewardAppellation;
	int nRewardPoint;
	bool bIsAchieveNotice;
	TEventCondition GainCondition;
	TEventCondition AchieveCondition;
};

struct TDailyMissionData
{
	int nMailID;
	int nRewardAppellation;
	int nRewardPoint;
	TEventCondition AchieveCondition;
	int nCounterParam;
	char cCounterOperator;
	int nItemID;
	int nType;
	int nTypeParam;
	__time64_t tStartDate;
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
	__time64_t tEndDate;
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
};

#if defined(PRE_ADD_REMOTE_QUEST)
struct TRemoteQuestData
{
	bool bActivate;
	int nIndex;
	int nQuestID;
	bool bPreQuestCheckType;
	std::vector<int> nVecPreQuestCondition;
	TEventCondition GainCondition;
};
#endif

#if defined(PRE_ADD_ACTIVEMISSION)
struct TActiveMissionData
{
	int nIndex;
	bool bActivate;
	int nMailID;
	TEventCondition AchieveCondition;
};
#endif

// Collection
const char CollectionAppellationMax = 10;
struct TCollectionBookData
{
	int nID;
	std::vector<int> nVecAppellationArrayIndex;
	int nRewardAppellationArrayIndex;
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
	int nMailID[2];			// 0 - 50% 넘겼을 때 보내줄 메일 ID, 1- 칭호 컬렉션 완성시 보상 MailID 
#endif  //#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
};

// TaxTable
struct TTaxData
{
	char cTaxType;			// 세금종류 (TaxType_PostTax)
	int nLevelStart;		// 시작 레벨
	int nLevelEnd;			// 끝 레벨
	int nAmountStart;		// 시작금액 (골드단위)
	int nAmountEnd;			// 끝금액
	float fRate;			// 세율
	int nTaxAmount;			// 세액
};

// Seal
const BYTE SEALENCHANTMAX = 16;	// 노강화부터 15강까지.
struct TSealCountData
{
	int nType1;
	int nType2;
	int nLevel[SEALENCHANTMAX];
	char cSuccessRate[SEALENCHANTMAX];
};

// PCBang
const BYTE PCParamMax = 2;
struct TPCBangParam
{
	int nParam[PCParamMax];
};

struct TPCBangData
{
	char cType;		// PCBang 혜택 종류 (ePCBangType)
	std::vector<TPCBangParam> VecPCBangParam;
	std::vector<TPCBangParam> VecPremiumPCBangParam;
	std::vector<TPCBangParam> VecGoldPCBangParam;
	std::vector<TPCBangParam> VecSilverPCBangParam;
	std::vector<TPCBangParam> VecRedPCBangParam;
};

// CashShop
const BYTE COMMODITYITEMMAX = 5;
const BYTE COMMODITYLINKMAX = 5;
struct TCashCommodityData
{
	int nSN;
#if defined(_JP)
	std::string strJPSN;
#endif	// #if defined(_JP)
	BYTE cCategory;
	int nSubCategory;
	int nItemID[COMMODITYITEMMAX];	// ItemTable 아이템 ID
	int nLinkSN[COMMODITYLINKMAX];	// 연계상품
	std::string strName;
	short wPeriod;					// 기간
	int nPrice;						// 가격
	int nCount;						// 구매시 주어지는 개수
	int nPriority;					// 캐시샵에 표시되는 우선순위
	bool bOnSale;					// 판매상태
	bool bOnSaleReal;				// 진짜(?)판매상태
	char cState;					// HOT, NEW등 상태 표시용
	bool bLimit;					// 한정 판매
	bool bReserveGive;				// 적립금 제공 여부
	int nReserve;					// 구매시 주어지는 적립금
	int nValidity;					// 유효기간
	bool bReserveAble;				// 적립금으로 살수 있는지 여부
	bool bPresentAble;				// 선물 가능 여부
	char cItemSort;					// 아이템종류
	int nPriceFix;					// 정가
	bool bCartAble;					// 장바구니에 담을 수 있는지
	bool bVIPSell;					// VIP 전용 판매 여부
	int nVIPLevel;					// VIP 등급
	bool bAutomaticPay;				// 자동결제 여부
	int nAutomaticPaySale;			// 자동결제 후 할인적용
	int nVIPPoint;					// 구매시 지급하는 VIPPoint
	int nOverlapBuy;				// 중복구매방지
#if defined(PRE_ADD_CASH_REFUND)
	bool bNoRefund;					// 청약 철회 불가능여부(true : 불가능)
#endif
#if defined(PRE_ADD_CASHSHOP_CREDIT)
	bool bCreditAble;				// 미쿡빌링
	int nCreditAbleLevel;			// 미쿡빌링 크레딧레벨
#endif	// #if defined(PRE_ADD_CASHSHOP_CREDIT)
#if defined(PRE_ADD_SALE_COUPON)
	bool bUseCoupon;
#endif //#if defined(PRE_ADD_SALE_COUPON)
#if defined( PRE_ADD_NEW_MONEY_SEED )
	bool bSeedAble;
	int nSeed;
	bool bSeedGive;
#endif

	TCashCommodityData()
	{
		nSN=0; cCategory=0; nSubCategory =0; 
		memset(nItemID, 0, sizeof(nItemID));
		memset(nLinkSN, 0, sizeof(nLinkSN));
		wPeriod=0; nPrice=0; nCount=0; nPriority=0;
		bOnSale=false; bOnSaleReal=false; cState=0;
		bLimit=false; bReserveGive=false; nReserve=0;
		nValidity=0; bReserveAble=false; bPresentAble=false;
		cItemSort=0; nPriceFix=0; bCartAble=false;
		bVIPSell=false; nVIPLevel=0; bAutomaticPay=false;
		nAutomaticPaySale=0; nVIPPoint=0;
		nOverlapBuy =0;

#if defined(PRE_ADD_CASH_REFUND)
		bNoRefund = true;		// 기본 불가능
#endif
#if defined(PRE_ADD_CASHSHOP_CREDIT)
		bCreditAble = false;
		nCreditAbleLevel = 0;
#endif	// #if defined(PRE_ADD_CASHSHOP_CREDIT)
#if defined(PRE_ADD_SALE_COUPON)
		bUseCoupon = false;
#endif // #if defined(PRE_ADD_SALE_COUPON)
#if defined( PRE_ADD_NEW_MONEY_SEED )
		bSeedAble = false;
		nSeed = 0;
		bSeedGive = false;
#endif
	}
};

struct TCashPackageData
{
	int nSN;		// 패키지판매SN
	std::vector<int> nVecCommoditySN;

	TCashPackageData()
	{
		nSN = 0;
		nVecCommoditySN.clear();
	}
};

struct TCashLimitData
{
	int nSN;			// 한정판매할 상품 SN
	time_t tStartDate;  // _StartDate	시작일
	time_t tEndDate;	// _EndDate	종료일	
	int nBuyAbleCount;	// 캐릭터당 구매가능횟수
#if defined(PRE_ADD_LIMITED_CASHITEM)
	int nLimitedSellCount;		//갯수한정아이템!
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
};

struct TVIPData
{
	int nVipID;				// VIPTable에서의 ID
	int nMonthItemSN;		// 캐시 아이템 상품 넘버 (30일 결제시 한번 주기)
	int nMailID[4];			// 0:서비스만료 7일전, 1:서비스만료 1일전, 2:서비스만료시, 3:자동결제시 캐시 부족할때
	int nAutoPayItemSN;		// 자동결제할때 결제할 아이템SN	
};

// DL Table
struct TDLMapData
{
	int nTotalRound;
	int nFloor;

#if defined( PRE_ADD_CHALLENGE_DARKLAIR )
	bool bChallengeDarkLair;
#endif	// #if defined( PRE_ADD_CHALLENGE_DARKLAIR )
};

struct TDLClearDataItem
{
	int nLevelMin;
	int nLevelMax;
	int nRoundMin;
	int nRoundMax;
	int nRewardExperience;
	int nShowBoxCount;
	int nSelectBoxCount;
	char cTreasureBoxRatio[4];
	int nRewardItemID[4];
	int nReturnWorldSetID;
	int nReturnWorldGateID;
	int nRewardGP;
};

struct TDLClearData
{
	std::vector<TDLClearDataItem*> pVecClearItem;
};

// Levelup Event 
struct TLevelupEvent
{
	enum eEventType
	{
		Levelup = 1,			// 일반 레벨업이벤트
		ShandaPromotion = 2,	// 샨다 프로모션이벤트
		CombackUser = 3,		// 복귀유저이벤트
		LevelupNew = 4,			// 레벨업이벤트 새거 (한번 받으면 더이상 못받는 버젼)
		ComeBackUserInven = 5,		// 귀환자에게 바로 넣어주는 아이템 타입
		NewbieQuitReward = 6, //신규 유저 접속 종료 보상
		NewbieReconnectReward = 7, //신규 유저 재접속 보상
		ComeBackUserQuitReward = 8, //귀환자 접속 종료 보상
	};

	int nLevel;			// Player 캐릭터의 레벨
	char cClassID;		// 클래스
	BYTE cJob;
	int nMailID;
	int nCashMailID;
	int nEventType;		// eEventType

	TLevelupEvent()
	{
		nLevel = 0;
		cClassID = 0;
		nMailID = 0;
		nCashMailID = 0;
		cJob = 0;
	}
};

struct TItemOptionTableData
{
	int iEnchant;
	int iOption;
	int iPotential;
	TItemOptionTableData()
	{
		iEnchant = 0;
		iOption = 0;
		iPotential = 0;
	}
};

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct TReputeTableData
{
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
	int iMissionID;
	int iNpcPresentRepute1; // 선물1 지급 호감도 달성 조건
	int iNpcPresentRepute2; // 선물2 지급 호감도 달성 조건
	int iNpcPresentID1; // NpcPresentRepute1 달성 시 NPC가 주는 선물 1 ItemID
	int iNpcPresentID2; // NpcPresentRepute1 달성 시 NPC가 주는 선물 2 ItemID
#else
	std::vector<int>	iQuestIDs;
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
	
	int	iMaxFavor;			// 호의 최대치
	int	iTakeFavorPerDay;	// 1일당 깎이는 호의 수치
	int	iMaxMalice;			// 악의 최대치
	int	iAddMalicePerDay;	// 1일당 올라가는 악의 수치
	int	iPresentIDArr[6];	// 받을 수 있는 선물 리스트
	std::vector<int> vFavorNpcID;		// 호의 NPC 리스트
	std::vector<int> vMaliceNpcID;		// 악의 NPC 리스트
	int	iFavorGroupBomb;
	int	iMaliceGroupBomb;
	int	iMailID[NpcReputation::Common::MaxMailCount];
	int	iMailRand[NpcReputation::Common::MaxMailCount];
	int	iPlusItemID;
	int	iPlusProb;
	int	iUnionID;
	int	iPresentPointArr[6]; // 선물마다 받을수 있는 연합포인트

	TReputeTableData()
	{
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
		iMissionID			= 0;
		iNpcPresentRepute1	= 0;
		iNpcPresentRepute2	= 0;
		iNpcPresentID1		= 0;
		iNpcPresentID2		= 0;
#else
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
		iMaxFavor			= 0;
		iTakeFavorPerDay	= 0;
		iMaxMalice			= 0;
		iAddMalicePerDay	= 0;
		iFavorGroupBomb		= 0;
		iMaliceGroupBomb	= 0;
		for( UINT i=0 ; i<_countof(iPresentIDArr) ; ++i )
			iPresentIDArr[i] = 0;
		for( UINT i=0 ; i<NpcReputation::Common::MaxMailCount ; ++i )
		{
			iMailID[i]		= 0;
			iMailRand[i]	= 0;
		}
		vFavorNpcID.clear();
		vMaliceNpcID.clear();

		iPlusItemID			= 0;
		iPlusProb			= 0;
		iUnionID			= 0;
		for( UINT i=0 ; i<_countof(iPresentPointArr) ; ++i )
			iPresentPointArr[i] = 0;
	}

	int GetUnionPointByPresent (const int iPresentID)
	{
		for( UINT i=0 ; i<_countof(iPresentIDArr) ; ++i )
		{
			if( iPresentIDArr[i] == iPresentID )
			{
				// 혹시 몰라서 사이즈 검사 한번더!
				if (0 <= i && i < _countof(iPresentPointArr))
					return iPresentPointArr[i];
			}
		}
		return 0;
	}

	bool CheckPresentID( const int iPresentID )
	{
		for( UINT i=0 ; i<_countof(iPresentIDArr) ; ++i )
		{
			if( iPresentIDArr[i] == iPresentID )
				return true;
		}
		return false;
	}

	bool bIsFavorNpc( const int iNpcID )
	{
		for( UINT i=0 ; i<vFavorNpcID.size() ; ++i )
		{
			if( vFavorNpcID[i] == iNpcID )
				return true;
		}
		return false;
	}

	bool bIsMaliceNpc( const int iNpcID )
	{
		for( UINT i=0 ; i<vMaliceNpcID.size() ; ++i )
		{
			if( vMaliceNpcID[i] == iNpcID )
				return true;
		}
		return false;
	}
};

struct TPresentTableData
{
	enum eType
	{
		Normal	= 0,
		Cash	= 1,
		Coin	= 2,
		Max,
	};

	eType Type;
	int	iTypeID;
	int	iCount;
	int	iAddFavorPoint;
	int	iTakeMalicePoint;
	int	iFavorGroupPoint;
	int	iMaliceGroupPoint;

	TPresentTableData()
	{
		Type = Max;
		iTypeID = 0;
		iCount = 0;
		iAddFavorPoint = 0;
		iTakeMalicePoint = 0;
		iFavorGroupPoint = 0;
		iMaliceGroupPoint = 0;
	}
};

struct TMailTableData
{
	DBDNWorldDef::MailTypeCode::eCode	Code;
	bool IsCash;
	int	nSenderUIStringIndex;
	int	nTitleUIStringIndex;
	int	nTextUIStringIndex;
	int	ItemIDArr[MAILATTACHITEMMAX];
	int	ItemCountArr[MAILATTACHITEMMAX];
	int	ItemSNArr[MAILATTACHITEMMAX];
	int	nPresentMoney;
#if defined(PRE_SPECIALBOX)
	int nKeepBoxReceive;
	int nKeepBoxType;
	int nKeepBoxLevelMin;
	int nKeepBoxLevelMax;
	int nKeepBoxClass;
#endif	// #if defined(PRE_SPECIALBOX)
	
	TMailTableData()
	{
		Code = static_cast<DBDNWorldDef::MailTypeCode::eCode>(0);
		IsCash = false;
		nSenderUIStringIndex = 0;
		nTitleUIStringIndex = 0;
		nTextUIStringIndex = 0;
		nPresentMoney = 0;
		memset( ItemIDArr, 0, sizeof(ItemIDArr) );
		memset( ItemCountArr, 0, sizeof(ItemCountArr) );
		memset( ItemSNArr, 0, sizeof(ItemSNArr) );
#if defined(PRE_SPECIALBOX)
		nKeepBoxReceive = 0;
		nKeepBoxType = 0;
		nKeepBoxLevelMin = 0;
		nKeepBoxLevelMax = 0;
		nKeepBoxClass = 0;
#endif	// #if defined(PRE_SPECIALBOX)
	}
};

struct TStoreBenefitData
{
	// 아래의 순서는 StoreBenefit 테이블의 정의와 맞춘 것이기 때문에 절대로 순서가 바뀌면 안됨.
	enum eType
	{
		EnchantFeeDiscount = NpcReputation::StoreBenefit::EnchantFeeDiscount,
		RepairFeeDiscount = NpcReputation::StoreBenefit::RepairFeeDiscount,
		SellingPriceUp = NpcReputation::StoreBenefit::SellingPriceUp,
		BuyingPriceDiscount = NpcReputation::StoreBenefit::BuyingPriceDiscount,
		FishProficiencyUp = NpcReputation::StoreBenefit::FishProficiencyUp,
		CookProficiencyUp = NpcReputation::StoreBenefit::CookProficiencyUp,
		CultivateProficiencyUp = NpcReputation::StoreBenefit::CultivateProficiencyUp,
		Max = NpcReputation::StoreBenefit::Max, 
	};

	int iNpcID;
	eType Type;
	int aiFavorThreshold[ STORE_BENEFIT_MAX ];
	int aiAdjustPercent[ STORE_BENEFIT_MAX ];

	TStoreBenefitData( void ) : iNpcID( 0 ), Type( Max )
	{
		memset( aiFavorThreshold, 0, sizeof(aiFavorThreshold) );
		memset( aiAdjustPercent, 0, sizeof(aiAdjustPercent) );
	}
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct TShopPurchaseType
{
	Shop::PurchaseType::eCode PurchaseType;
	int iPurchaseItemID;
	int iPurchaseItemValue;
};

struct TCombinedShopTableData
{
	TShopItem ShopItem;
	TShopPurchaseType PurchaseType[Shop::Max::PurchaseType];
	Shop::PurchaseLimitType::eCode PurchaseLimitType;
	int iPurchaseLimitValue;
#if defined(PRE_SAMPLEITEMNPC)
	int nSampleVersion;	// 샘플아이템 회차
#endif	//#if defined(PRE_SAMPLEITEMNPC)
};

struct TMasterSystemDecreaseTableData
{
	int iPupilLevel;
	int iPupilFavor;
	int iFavorUpDecreaseRespectPoint;
	int iFavorDownDecreaseRespectPoint;
	int iMasterSuccessMailID;
	int iMasterFailureMailID;
	int iPupilSuccessMailID;
	int iPupilFailureMailID;

	int GetDecreaseRespectPoint( int iFavorPoint )
	{
		if( iFavorPoint >= iPupilFavor)
			return iFavorUpDecreaseRespectPoint;
		return iFavorDownDecreaseRespectPoint;
	}
};

struct TMasterSystemGainTableData
{
	int	iFavorPoint;
	int	iRepectPoint;
};

#if defined( PRE_ADD_SECONDARY_SKILL )

struct TSecondarySkillTableData
{
	int	iSkillID;
	SecondarySkill::Type::eType Type;
	SecondarySkill::SubType::eType SubType;
	SecondarySkill::ExecuteType::eType ExecuteType;
};

struct TSecondarySkillLevelTableData
{
	int	iSkillID;
	SecondarySkill::Grade::eType Grade;
	int	iLevel;
	int	iExp;
	int	iNextID;
	int	iParam;
};

struct TSecondarySkillRecipeTableData
{
public:
	int iManufactureItemID[SecondarySkill::ManufactureSkill::MaxManufactureItemTypeCount];
	int	iMinExp[SecondarySkill::ManufactureSkill::MaxManufactureItemTypeCount];
	int	iMaxExp[SecondarySkill::ManufactureSkill::MaxManufactureItemTypeCount];
	SecondarySkill::Grade::eType	RequireGrade;
	int	iRequireSkillLevel;
	int	iToolItemID[SecondarySkill::ManufactureSkill::MaxToolItemCount];
	int	iMasterExp;
	int	iStartExp;
	int	iMaxTime;	// 단위(ms)
	int	iSuccessProbability;
	int	iSuccessUpExp;
	int	iRecipeUpExp;
	int	iMaterialItemID[SecondarySkill::ManufactureSkill::MaxMaterialItemCount];
	int	iMaterialItemCount[SecondarySkill::ManufactureSkill::MaxMaterialItemCount];
	int	iSuccessCount;

	// 경험치로 생산될 ItemID 얻기
	int GetManufactureItemID( int iExp, int iMaxValue )
	{
		int iExpRate = static_cast<int>(iExp/static_cast<float>(iMaxValue)*100);

		for( int i=0 ; i<SecondarySkill::ManufactureSkill::MaxManufactureItemTypeCount ; ++i )
		{
			if( iExpRate >= iMinExp[i] && iExpRate <= iMaxExp[i] )
				return iManufactureItemID[i];
		}

		_ASSERT_EXPR(0, L"[GetManufactureItemID] can not find ItemID");
		return -1;
	}
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )


#ifdef PRE_ADD_CASHFISHINGITEM
struct TFishingMeritInfo
{
	int nRodItemID;

	bool bUseCashBait;
	INT64 biBaitSerial;
	int nBaitIndex;

	int nMeritReduceFishingTime;
	int nMeritSuccessRate;
};
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM

struct TFarmCultivateTableData
{
	int	iCultivateMaxTimeSec;
	int	iMaxWater;
	int iConsumeWater;
	int iHarvestNeedItemID;
	int iHarvestDropRate[Farm::Max::HARVESTITEM_COUNT];
	int iHarvestDropTableID;
	int iSkillPointPlant;			// 씨앗심기 시 경험치
	int iSkillPointWater;			// 물주기 시 경험치
	int iCatalystCount;				// 필요 성장촉진제 개수
	SecondarySkill::Grade::eType	RequiredSkillGrade;
	int								iRequiredSkillLevel;
	int	iOverlap;
};

struct TFishingTableData
{
	int nMaxTime;
#ifdef PRE_ADD_CASHFISHINGITEM
	int nAutoMaxTime;
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
	int nPullingTargetMin;
	int nPullingTargetMax;
	int nPullingTargetSpeed;
	int nPullingTargetUpTime;
	int nSkillPointSuccess;
	int nSkillPointFailure;
	int nSuccessProbability;
	int nSuccessDropTableID;
};

struct TFishingPattern
{
	int nPatternID;
	int nProbabillity;
};

struct TFishingPointTableData
{
	int nSecondarySkillClass;			//낚시보조스킬등급
	int nSecondarySkillLevel;			//낚시보조스킬레벨
	int nRequireItemType1;
	int nRequireItemType2;
	TFishingPattern Pattern[Fishing::Max::FISHINGPATTERNMAX];
};

struct TFishingAreaTableData
{
	int nMatchedMapID;
	int nMatchedFishingAreaID;
	int nFishingPointID;
};



struct TJobTableData
{
	char	cJobNumber;
	char	cBaseClass;
	char	cParentJob;
	float	afMaxUsingSP[ 5 ];
	char	cClass;
};

struct TGlyphSkillData
{
	int nGlyphID;
	int nGlyphType;
	int nSkillID;
	int nSkillLevelID;
};

struct TGlyphSlotData
{
	int nID;
	char cSlotID;
	char cLevelLimit;
	char cSlotType;
	bool IsCash;
};

struct TGlyphChargeData
{
	char cGlyphType;		// 1: 강화문장 2: 스킬문장  3: 특수스킬문장	(eGlyphType)
	char cGlyphRank;		// 1: 매직 2: 레어 3: 에픽 4: 유니크 (eItemRank)
	int nCharge;
};

struct TCharmItem
{
#if defined(PRE_FIX_68828)
	int nCharmID;	// 참테이블 ID
#endif
	int nItemID;	// 아이템
	int nCount;		// 유저가 획득할 개수
	int nProb;		// 확률
	int nPeriod;	// 기간
	int nAccumulationProb;
	int nGold;
	bool bMsg;
};

struct TCharmItemData
{
	int nCharmNo;	// 매력아이템 번호
	std::vector<TCharmItem> CharmItemList;
};

const BYTE CharmItemKeyMax = 5;
struct TCharmItemKeyData
{
	int nCharmItemID;	// 상자 아이템 아이디
	std::vector<int> nKeyList;
};

struct TCharmCountData
{
	int nID;
	int nMin;
	int nMax;
};

#if defined (PRE_ADD_CHAOSCUBE)
struct TChaosStuffItem
{
	int nItemID;
	int nCount;
	int nChaosNo;
};

struct TChaosResultItem
{
	int nItemID;	// 아이템
	int nCount;		// 유저가 획득할 개수
	int nProb;		// 확률
	int nPeriod;	// 기간
	int nAccumulationProb;
	int nGold;
	bool bMsg;
};

struct TChaosItemData
{
	int nChaosNo;
	std::vector<TChaosResultItem> ChaosItemList;

};
#endif // #if defined (PRE_ADD_CHAOSCUBE)

//ServerMonitor
enum eServerMonitorLevel
{
	SERVERMONITOR_LEVEL_NONE = 0,
	SERVERMONITOR_LEVEL_MONITOR,				//동접정보와 서버의상태
	SERVERMONITOR_LEVEL_ADMIN,					//동접정보와 서버상태 및 프로세스 시작만 가능
	SERVERMONITOR_LEVEL_SUPERADMIN,				//동접정보와 서버상태 및 모든컨트롤 가능
};

enum eServerMonitorNotice
{
	SERVERMONITOR_UPDATENOTICE_CHANNELINFO,
	SERVERMONITOR_UPDATENOTICE_SERVERINFO,
	SERVERMONITOR_UPDATENOTICE_MERITINFO,
	SERVERMONITOR_UPDATENOTICE_WOLRDINFO,
};

enum eSIDConnectionType
{
	SID_CONNECTION_TYPE_LOG = 1,
	SID_CONNECTION_TYPE_DB,
};

struct TPromotionData
{
	int nID;
	int nConditionType;				//ePromotionConditionType
	int nConditionValue;
	int nPromotionType;				//ePromotionType
	int nRewardValue;				//Percent
};

struct TUserPromotionData
{
	int nType;		//ePromotionType
	int nValue;		//Percent
};

struct TSchedule
{
	char cHour;
	char cMinute;
	int nCycle;
};


struct TGuildLevel
{
	int nLevel;
	int nReqGold;
	int nReqGuildPoint;
	int nDailyGuildPointLimit;
	int nDailyMissionPointLimit;
	bool bApplicable;
};

struct TGuildWarPoint
{
	int nRank;
	int nRewardGuildPoint;
};

struct TGuildWarMapInfo
{
	UINT uiMapTableID;
	char cGuildWarUseMonth;
};

struct TGuildWarEventInfo
{
	//char cEventStep;
	__time64_t tBeginTime;
	__time64_t tEndTime;
};
// 길드전 보상 메일관련
struct TGuildWarRewardData
{
	char cType;			// 보상 타입..eGuildWarRewardType
	char cClass;		// 클래스(워리어,아처, 소서리스, 클래릭)
	int nMailID;		// 우편 아이디
	int nPresentID;		// 선물함 아이디
	int nGuildFestivalPoint;	// 본선에서 지급할 길드축제포인트
	int nGuildPoint;			// 본선에서 지급할 길드포인트
};


struct TPeriodQuestDate
{
	int nPeriodType;
	__time64_t tAssignDate;
};

struct TExchangeTrade
{
	int nItemID;
	int nCategoryType;
	int nGroupID;
	int ExchangeType;
};

struct TPvPMissionRoom
{
	int nItemID;
	int nGroupID;
	int nMapID;
	int nModeID;
	int nWinCondition;
	int nPlayTime;
	int nMaxPlayers;
	int nStartPlayers;
	int nRoomNameIndex;
	bool bDropItem;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	int nChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
};

typedef std::vector<TPvPMissionRoom> TVecPvPMissionRoom;

struct TGhoulConditionData
{
	int nItemID;
	int nItemIdx;
	int nPlayerMaxNum[PvPCommon::Common::GhouConditionColCount];
	int nGhoulCount[PvPCommon::Common::GhouConditionColCount];
	int nMatchingMutationGroupID[PvPCommon::Common::GhouConditionColCount];
};

struct TMonsterMutationGroup
{
	int nGroupID;
	int nGourpIdx;
	int nMonsterID[PvPCommon::Common::MonsterMutationColCount];
	int nProbability[PvPCommon::Common::MonsterMutationColCount];
};

struct TMonsterMutationData
{
	int nMutationID;
	int nMutationIdx;
	int nMutationActorID;

	int nMutationHP;
	int nMutationStrPMax;
	int nMutationStrMMax;
	int nMutationDefP;
	int nMutationDefM;
	int nMutationStiff;

	int nMutationStiffResis;
	int nMutationCriticalResis;
	int nMutationStunResis;

	int nMutationWeaponIdx1;
	int nMutationWeaponIdx2;
	int nMutationSkillTableID;
	int nMutationSuperArmor;

	int nSizeMin;
	int nSizeMax;
	int nMoveSpeed;
};

struct TMonsterMutationSkill
{
	int nMonsterID;
	int nMonsterIdx;
	int nMonsterSkillIndex[PvPCommon::Common::MonsterMutationSkillColCount];
	int nMonsterSkillLevel[PvPCommon::Common::MonsterMutationSkillColCount];
};

struct TOccupationModeInfo
{
	int nVictoryCondition[PvPCommon::Common::MaximumVitoryCondition];		//PvPCommon::BattleGroundVictoryState
	int nVictoryParam[PvPCommon::Common::MaximumVitoryCondition];		//PvPCommon::BattleGroundVictoryState
	int nBossID[PvPCommon::TeamIndex::Max];									//생성보스아이디
	int nBossSpawnAreaID[PvPCommon::TeamIndex::Max];						//보스생성시 스폰위치
};

struct TBattleGourndModeInfo
{
	int nID;
	int nWaitingMin;
	int nFinishingMin;
	int nPvPSkillPoint;	
	int nMaximumResourceLimit;
	int nAllOcuupationBonusGain;
	int nOccupationBonusGain[PvPCommon::Common::MaximumCapturePosition];

	int nCaptureScore;
	int nStealScore;
	int nKillScore;
	int nClimaxTime;
	int nClimaxRespawnTime;

	TOccupationModeInfo ModeInfo;
};

struct TPositionAreaInfo
{
	int nID;
	int nMapID;
	int nPvPModeID;
	int nAreaID;
	int nGainResourceTermTick;			//점령시 획득텀
	int nGainResource;					//획특텀시 획득량
	int nRequireTryTick;				//획득시도시 클릭상태지속 유지필요시간
	int nCompleteOccupationTick;		//클릭완료후 점령권 넘어가기 전의 대기시간
	int nBonusBuffID;
	//클라이막스~
	int nClimaxGainTermTick;
	int nClimaxGainVal;
	int nClimaxTryTick;
	int nClimaxCompleteOccupationTick;
};

struct TBattleGroundEffectValue
{
	int nSkillID;
	int nSkillLevel;
	int nNeedSkillPoint;			//획득시필요포인트
	int nUseResPoint;			//스킬사용시 소모 점령전 포인트
	std::string strEffectValue[PvPCommon::Common::MaximumEffectCount];
	int nEffectValueDuration[PvPCommon::Common::MaximumEffectCount];
	//int nMonsterID;
	//int nSummonDist;
	std::string strActionName;
	int nCoolTime;

	void Reset()
	{
		nSkillID = 0;
		nSkillLevel = 0;
		nNeedSkillPoint = 0;			//획득시필요포인트
		nUseResPoint = 0;			//스킬사용시 소모 점령전 포인트		
		for (int i = 0; i < PvPCommon::Common::MaximumEffectCount; i++)
		{
			strEffectValue[i].clear();;
			nEffectValueDuration[i] = 0;
		}

		//nMonsterID = 0;
		//nSummonDist = 0;
		strActionName.clear();
		nCoolTime = 0;
	}
};

struct TBattleGroundSkillInfo
{
	int nSkillID;
	int nSkillMaxLevel;
	int nSkillType;				//CDnSkill::SkillTypeEnum		//점령전 스킬은 스킬타입에 그닥 영향을 받지 않지만 추가확장시 필요할까 해서 읽어둠
	int nSkillDurationType;		//CDnSkill::DurationTypeEnum
	int nSkillTargetType;		//CDnSkill::TargetTypeEnum
	int nEffectType[PvPCommon::Common::MaximumEffectCount];
	int nEffectApplyType[PvPCommon::Common::MaximumEffectCount];
	int nProcess;				//프로세스타입은 하나만 사용
	int nCanDuplicate;			//중첩가능?
};

struct TBattleGroundSkill
{
};

struct TUnionReputeBenefitData
{
	int nItemID;
	int nPeriod;
	int nBenefitType[NpcReputation::Common::MaxBenefitCount];
	int nBenefitNum[NpcReputation::Common::MaxBenefitCount];
};

struct TGuildMarkData
{
	int nItemID;
	int nType;
	bool bCash;
	bool bMarkOnly;
	bool bMarkView;
};

struct TPlayerCommonLevelTableInfo
{
	int iFatigue10000Ratio;	// 10000배율
	float fMasterGainExp;
	//int nMasterGainItemID;
	//int nMasterGainItemCount;
	float fAddGainExp;
	float fDefense;
	float fCritical;
	float fFinalDamage;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int iTalismanCost;	// 탈리스만 장착 해제, 위치교환 비용
#endif
};

struct TMasterSysFeelTableInfo
{
	int nMinFeel;
	int nMaxFeel;
	int nAddExp; //(%값임)
};

struct TGlobalEventData
{
	int nItemID;
	int nCollectItemID;
	int nScheduleID;
	int nCollectTotalCount;
	int nNoticeCount[MAX_NOTICE_WORLDEVENT];
	bool bCheckNotice[MAX_NOTICE_WORLDEVENT];

	time_t tCollectStartDate;
	time_t tCollectEndDate;
	time_t tRewardDate;
};

struct TEveryDayEventData
{
	int nEventID;
	int nEventOnOff;
	int nSquenceDateCount;
	int nMailID;
	int nCashMailID;
	int nNumber;
	time_t tStartDate;
	time_t tEndDate;
};

enum eDBJobSystem  //g_szDBJobSystem
{
	DBJOB_GUILDWAR_INIT = 0,		// 길드전 초기화
	DBJOB_GUILDWAR_STATS,		// 길드전 통계
	DBJOB_SYSTEM_MAX,
};

enum eDBJobSystemStatus
{
	JOB_STATUS_RESERVE,		// 예약
	JOB_STATUS_COMPLETED,	// 완료
	JOB_STATUS_DOING,		// 실행중
	JOB_STATUS_FAIL,		// 실패
};

struct TGuildRewardItemData
{
	int		nItemID;			// 길드보상테이블에 있는 아이디
	short	nNeedGuildLevel;	// 구입하기위한 길드레벨
	int		nNeedGold;			// 구입하기위한 골드
	int		nItemType;			// 길드보상아이템 타입
	int		nTypeParam1;
	int		nTypeParam2;
	bool	bCheckInven;		// 이미 구입한건지 체크
	bool	bEternity;			// 0:기간제 1:영구
	int		nPeriod;			// 기간
	bool	bCheckMaster;		// 마스터체크 여부
	int		nCheckType;			// 필요 조건 체크(0:NONE 1:ITEMID 2:길드마크
	int		nCheckID;			// 필요한 이전 아이템아이디
};
struct TExpData 
{
	int nExperience;
	int nEventExperience;
	int nPcBangExperience;
	int nVIPExperience;
	int nPromotionExperience;
	int nItemExperience;
	int nGuildExp;
	TExpData()
	{
		nExperience = 0;
		nEventExperience = 0;
		nPcBangExperience = 0;
		nVIPExperience = 0;
		nPromotionExperience = 0;
		nItemExperience = 0;
		nGuildExp = 0;
	}
	void set(float Exp, float EventBonusExp = 0, float PcBangExp = 0, float VIPExp = 0, float PromotionBonusExp = 0, float ItemExp = 0, float GuildExp = 0)
	{
		nExperience = (int)Exp;
		nEventExperience = (int)EventBonusExp;
		nPcBangExperience = (int)PcBangExp;
		nVIPExperience = (int)VIPExp;
		nPromotionExperience = (int)PromotionBonusExp;
		nItemExperience = (int)ItemExp;
		nGuildExp = (int)GuildExp;
	}
};
#if defined(PRE_ADD_SALE_COUPON)
struct TSaleCouponData
{
	int nItemID;							// 쿠폰 ID
	int nUseItemSN[MAX_SALE_USEITEM];		// 쿠폰이 적용될 아이템 SN
};
#endif

#if defined( PRE_PARTY_DB )

namespace Party
{
	struct QueryOutPartyType
	{
		enum eCode
		{
			Normal = 0,
			Disconnect,
			Kick,
			EternityKick,
		};
	};

	struct AddPartyAndMemberGame
	{
		Party::Data PartyData;		
		UINT nKickedMemberList[PARTYKICKMAX];
		INT64 biCharacterDBID[PARTYMAX];
		UINT nSessionID[PARTYMAX];	
#if defined( PRE_ADD_NEWCOMEBACK )
		bool bCheckComebackAppellation[PARTYMAX];
#endif
	};

	struct PartyMemberVillageData
	{
		UINT nAccountDBID;
		INT64 biCharacterDBID;
		BYTE cMemberIndex;
		BYTE cVoiceAvailable;
#if defined( PRE_ADD_NEWCOMEBACK )
		bool bCheckComebackAppellation;
#endif
		UINT nMutedList[PARTYCOUNTMAX];
	};
	struct AddPartyAndMemberVillage
	{
		Party::Data PartyData;
		UINT nVoiceChannelID;
		WCHAR wszPartyName[PARTYNAMELENMAX];
		UINT nKickedMemberList[PARTYKICKMAX];
		PartyMemberVillageData MemberData[PARTYMAX];
	};
};

#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
struct TNamedItemData
{
	int nItemID;		// 네임드아이템아이디
	int nMaxCount;		// 최대 갯수
	int nSwapItemID;	// 대체할 아이템아이디
};
namespace EffectSkillNameSpace
{
	struct SearchType
	{
		enum eType
		{
			ItemType = 0,
			ItemID,
			SkillID,			
		};
	};	
	struct BuffType
	{
		enum eType
		{
			Partybuff = 0,
			SelfBuff,
		};
	};
	struct ShowEffectType
	{
		enum eType
		{
			NONEEFFECT = 0,
			BUFFEFFECT,
			SPELLEFFECT,
		};
	};
};
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
struct TPotentialTransferData
{
	int nItemID;
	int nExtractItemLevel;
	int nExtractItemRank;
	int nInjectItemLevel;
	int nInjectItemRank;
	int nConsumptionCount;
	int nMainType;
	int nSubType;
};
#endif		//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

#if defined(PRE_ADD_INSTANT_CASH_BUY)
const int CashBuyShortcutMax = 10;
struct TCashBuyShortcutData
{
	int nType;
	int nAllowMapTypes;
	int nShow;
	std::vector<int> nSNList;
};
#endif	// #if defined(PRE_ADD_INSTANT_CASH_BUY)

#if defined(PRE_ADD_EXCHANGE_ENCHANT)
struct TExchangeEnchantData
{
	BYTE cRank;	// 아이템 등급
	BYTE cLevelLimit;	// 제한 레벨
	BYTE cEnchantLevel;	// 강화 레벨
	int nNeedCoin;	// 수수료
	int nNeedItemID1;	// 필요아이템1
	short wNeedItemCount1;	// 필요아이템 카운트1
	int nNeedItemID2;	// 필요아이템2
	short wNeedItemCount2;	// 필요아이템 카운트2
};
#endif //#if defined(PRE_ADD_EXCHANGE_ENCHANT)

#if defined( PRE_WORLDCOMBINE_PARTY )
namespace WorldCombineParty
{
	enum eType
	{
		MAXSKILLCOUNT = 2,
	};

	struct WrldCombinePartyData
	{
		BYTE cIndex;				// 인덱스
		BYTE cGroupIndex;			// 그룹인덱스
		WCHAR wszPartyName[PARTYNAMELENMAX];
		int nWorldMap;			// 월드존
		int nTargetMap;			// 타겟맵(기준값)
		ePartyType PartyType;	// 파티타입
		BYTE cPartyMemberMax;
		int nItemID;			// 입장권아이템
		TDUNGEONDIFFICULTY Difficulty;
		TPARTYITEMLOOTRULE ItemLootRule;			//ePartyItemLootRule 참조
		TITEMRANK ItemRank;				//아이템랭크(루트룰기획참조)
		BYTE cUserLvLimitMin;		//민맥스값이 있단다.
		int iBitFlag;
		int nSkillID[MAXSKILLCOUNT];				// 스킬아이디		
	};		
}
#endif

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
struct TPcBangRentItem
{
	int nItemID;
	BYTE cLevelStart;
	BYTE cLevelEnd;
	BYTE cClassID;
	BYTE cJob;
	BYTE cGrade;
	int nItemOption;
};
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
struct TPrivateChatChannelInfo
{	
	INT64 nPrivateChatChannelID;
	WCHAR wszName[PrivateChatChannel::Common::MaxNameLen];
	INT64 biMasterCharacterDBID;
	int nMemberCount;
	int nPassWord;	
};

struct TPrivateChatChannelMember
{
	time_t tJoinDate;
	UINT nAccountDBID;
	INT64 biCharacterDBID;	
	bool bMaster;
	WCHAR wszCharacterName[NAMELENMAX];
};

struct TPrivateMemberDelServer
{
	INT64 nPrivateChatChannelID;	
	INT64 biCharacterDBID;
	WCHAR wszCharacterName[NAMELENMAX];
};
#endif

#if defined(PRE_ADD_WEEKLYEVENT)
namespace WeeklyEvent
{
	struct TWeeklyEventData
	{
		int nStartTime;
		int nEndTime;
		BYTE cRaceType;		// eRaceType
		BYTE cClassType;	// eClass
		int nEventType;
		int nValue;			// %값
	};

	struct TWeeklyEvent
	{
		int nDayOfWeek;		// eDayType
		std::vector<TWeeklyEventData> VecEventData;
	};

	enum eDayType
	{
		Monday = 1,
		Tuesday = 2,
		Wednesday = 3,
		Thursday = 4,
		Friday = 5,
		Saturday = 6,
		Sunday = 7,
	};

	enum eRaceType
	{
		Player = 1,
		Monster = 2,
	};

	enum eEventType
	{
		Event_1 = 1,			// 1. hp상승 및 하락
		Event_2 = 2,			// 2. 공격력 강화 및 약화
		Event_3 = 3,			// 3. 방어력 강화 및 약화
		Event_4 = 4,			// 4. 강화 확률 증가
		Event_5 = 5,			// 5. 경험치 증가
		Event_6 = 6,			// 6. 파티유지 경험치 증가
		Event_7 = 7,			// 7. 친구 경험치 증가 (절친아님, 서로친구)
		Event_8 = 8,			// 8. 호감도 증가
		Event_9 = 9,			// 9. 연합 포인트획득량 증가
		Event_10 = 10,			// 10. 아이템 드랍률 증가
	};
}
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
namespace TotalLevelSkill
{
	struct TTotalSkillLevelTable
	{
		int nSkillID;
		int nTotalLevelLimit;
		int nUseLevelLimit;
		int nSkillType;		
	};
	struct TTotalSkillBlowTable
	{
		int nSkillID;
		int nBlowID;
		float fBlowValue;
		int nBlowValue;		// 농장 아이템 아이디떄문에 사용
	};
	struct TTotalSkillSlotTable
	{
		int nSlotIndex;
		int nLevelLimit;
		bool bCash;
	};
}
#endif

#if defined(PRE_SPECIALBOX)
struct TProvideItemData
{
	int nCashSN;
	int nItemID;
	int nCount;
	int nSealCount;
	int nOption;
};

struct TKeepBoxProvideItemData
{
	int nType;
	std::vector<TProvideItemData> VecProvideItem;
};
#endif	// #if defined(PRE_SPECIALBOX)

#ifdef PRE_ADD_JOINGUILD_SUPPORT
struct TGuildSupportData
{
	int nID;
	int nJoinMinLevel;			//최초길드가입시 최소레벨
	int nJoinMaxLevel;			//최초길드가입시 최대레벨
	int nFirstJoinMailID;		//최초가입시 보상메일아이디
	int nMaxLevelGuildSupportMailID;	//최초가입한 길드에서 만랩시 보상아이디
};
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

#if defined( PRE_FIX_67546 )

struct TChannelDBInfo
{
	int nDBConnectionID;
	BYTE cThreadID;
};

struct TDBConnectionInfo
{	
	int nThreadCount[THREADMAX];
};

#endif
#if defined( PRE_WORLDCOMBINE_PVP )

struct TWorldPvPMissionRoom
{
	int nItemID;	
	int nMapID;
	BYTE cModeID;
	int nWinCondition;
	int nPlayTime;
	BYTE cMaxPlayers;
	int nStartPlayers;
	int nRoomNameIndex;
	bool bDropItem;
	USHORT unRoomOptionBit;
	WorldPvPMissionRoom::Common::eReqType eWorldReqType;
	int nMinLevel;
	int nMaxLevel;
	int nChannelType;
	WCHAR wszRoomName[PARTYNAMELENMAX];
	int nRoomPW;
};

struct TWorldPvPRoomDBData
{
	int nDBRoomID;
	int nWorldID;
	int nServerID;
	int nRoomID;
	BYTE cModeID;
	WCHAR wszRoomName[PARTYNAMELENMAX];
	BYTE cMaxPlayers;
	int nMapID;
	int nWinCondition;
	UINT nPlayTimeSec;
	bool bBreakIntoFlag;
	bool bDropItem;
	bool bRegulationFlag;
	bool bObserverFlag;
	bool bRandomFlag;
	BYTE cMinLevel;
	BYTE cMaxLevel;
	int nPassWord;
	int nRoomMemberCount;
	BYTE cChannelType;
};

typedef std::vector<TWorldPvPMissionRoom> TVecWorldPvPMissionRoom;

#endif

#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
struct TBonusDropTable
{
	int nIndex;
	int nMapID; //맵ID
	int nDropType;
	int nNeedItemID[3]; //필요한 열쇠 아이템 ID
	int nDropItemGroupID[3]; //드랍될 드랍아이템그룹ID
};
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)

namespace ActozCommon
{
	struct CharacterStatus
	{
		enum eCharacterStatus
		{
			Revive = 0,
			Create = 0,
			Delete = 1,
		};
	};

	struct UpdateType
	{
		enum eUpdateType
		{
			Login = 1,
			Logout = 2,
			Levelup = 3,
			Sync = 4,
		};
	};
};

#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
struct TItemDropEnchantData
{
	int nIndex; //IDX
	int nDropID; // ItemDropTable의 ID와 매칭되는 값
	int nEnchantCount; //인챈트가 몇개인지 카운트
	int nEnchantOption[20]; //인챈트 정보
	int nEnchantProb[20]; //인챈트가 선택될 확률
};
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)

#if defined( PRE_ALTEIAWORLD_EXPLORE )
struct TAlteiaWorldMapInfo
{
	int nIndex;
	int nMapID;
	TDUNGEONDIFFICULTY Difficulty;
};
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
namespace StampSystem
{
	struct ChallengeType
	{
		enum eType
		{
			DailyQuest	 = 1,
			DailyMission = 2,
		};
	};

	struct RewardCount
	{
		enum eCount
		{
			Day3 = 3,
			Day5 = 5,
		};
	};

	struct TStampChallenge // 스탬프 도전과제
	{
		BYTE cType;
		std::set<int> setAssignmentID; // 도전과제 목록
		int nMailID4; // 3일 보상
		int nMailID7; // 5일 보상
	};

	struct TStampTableData // 스탬프 테이블
	{
		INT64 biStartTime;
		std::vector<TStampChallenge> vChallengeList;
	};
}
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
struct TPrevPotentialData
{
	int nItemID; //아이템 ID
	INT64 nSerial; //시리얼
	char cPotential; //이전 잠재력데이터
	char cNowPotential; //새로 부여된 잠재력
	char cPotentialMoveCount; //(이전)잠재 이전 카운트
	INT64 nPotentialItemSerial; //잠재력 부여에 사용한 코드 시리얼
	int nPotentialItemID; //잠재력 부여에 사용한 아이템 ID
};
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)

struct TEffectItemData
{
	int nItemSN;
	int nItemID;
	int nCount;
	bool bFail;
	INT64 nGiftDBID;
};

#if defined(PRE_ADD_TALISMAN_SYSTEM)
struct TTalismanData
{
	int nTalismanID;
	int nType;
	int nPeriod;
};

struct TTalismanSlotData
{
	bool bService;
	int nLevel;
	int nAmount;
	int nItem;
	int nNeedItemCount;
	float fEfficiency;
	int nType;
};
#endif

#if defined( PRE_PVP_GAMBLEROOM )

struct TPvPGambleRoomData
{
	int nRoomIndex;	
	bool bRegulation;
	BYTE cGambleType;
	int nPrice;	
};

struct TPvPGambleRoomWinData
{
	INT64 biCharacterDBID;
	int nPrice;
};

#endif // #if defined( PRE_PVP_GAMBLEROOM )

#if defined(PRE_ADD_EQUIPLOCK)
struct TDBLockItemInfo
{
	BYTE cItemCode;
	BYTE cItemSlotIndex;
	INT64 biItemSerial;
	EquipItemLock::TLockItem ItemData;
};
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

#if defined( PRE_ADD_STAGE_WEIGHT )
struct TStageWeightData
{
	float fHPRate;
	float fAttackRate;
	float fSuperArmorRate;
	float fItemDropRate;
};
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )

#if defined( PRE_ADD_CHARACTERCHECKSUM )
struct CheckSumReason
{
	enum eReason
	{
		LogoutCharacter			= 1,
		C2C_KeepGameMoney		= 2,
		C2C_TransferGameMoney	= 3,
	};
};
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

#if defined(PRE_ADD_STAGECLEAR_TIMECHECK)
namespace StageClearCheckTime
{
	enum eCheckMinTime	// 최소 클리어 타임
	{
		CheckMinTime = 60 * 1000, // 1분
	};
	enum eAbuseCount // 증가할 AbuseCount값
	{
		AbuseCount = 10,
	};
	// CheckTime을 사용하지 않을 맵(보스맵ID를 기준으로 판별한다
	struct IgnoreMapIndex
	{
		enum eIgnoreMapIndex
		{
			MAP_CHAOSE_FIELD_BossA = 15157,	//혼돈의 틈 파르마
			MAP_CHAOSE_FIELD_32Lv_BossA = 15167,	//혼돈의 틈 바이라
			MAP_CHAOSE_FIELD_24Lv_BossA = 15174,	// 혼돈의 틈 카말라
		};
	};
};
#endif	// #if defined(PRE_ADD_STAGECLEAR_TIMECHECK)