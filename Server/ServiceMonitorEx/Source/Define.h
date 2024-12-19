

#pragma once


#define	DF_NETCLIENTSESSION_DEFAULT_RECVBUFSIZE	(1024*10)
#define	DF_NETCLIENTSESSION_DEFAULT_SENDBUFSIZE	(1024*10)

#define DF_CONFIGFILE_NAME				_T("Config.ini")	// 환경설정 파일 이름 (INI 포맷)

enum EF_CONST		// 상수 정의
{
	EV_SERVERNAME_MAXLEN				= 64,			// 서버 이름 길이

	EV_SVCINFOUPD_DEF					= 0,			// 서비스 정보 갱신 기본값
	EV_CTNRVERSION_DEF					= 0 ,			// 컨테이너 버전 기본값

	EV_WORLDID_DEF						= 0,			// 월드 번호 기본값 (없음 의미)
	EV_WORLDID_ALL						= 0,			// 월드 번호 기본값 (전체 의미)
	EV_SERVERID_DEF						= 0,			// 서버 고유 ID 기본값 (없음 의미)
	EV_SERVERID_ALL						= 0,			// 서버 고유 ID 기본값 (전체 의미)
	EV_CHANNELID_DEF					= 0,			// 채널 번호 기본값
	EV_MERITID_DEF						= 0,			// 메리트 ID (없음 의미)
	EV_NETLUNCHER_DEF					= 0,			// 넷런처 ID (없음 의미)

	EV_LEVEL_DEF						= 1,			// 레벨 기본값
	EV_MAPIDX_DEF						= 0,			// 맵 인덱스 기본값 (없음 의미)

	EV_CHNLATTR_DEF						= 0,			// 채널 속성 기본값

	EV_TIMERID_DEF						= 1,			// 타이머 ID 기본값

	EV_PORTNO_DEF						= 8650,			// 포트 번호 기본값

	EV_IPADDR_MAXLEN					= 32,			// IP 주소 최대길이 ('DNCommonDef.h' 참조)

	EV_RSCSVNRVS_MAXLEN					= 32,			// 리소스 SVN 리비전 최대길이
	EV_SRVVER_MAXLEN					= 32,			// 서버 버전 최대 길이 ('DNCommonDef.h' 참조, SERVERVERSIONMAX)

	EV_PAGEIDX_DEF						= -1,			// 월드 뷰 페이지 인덱스 기본값 (없음 의미)

	EV_REQUESTSERVICESTRUCT_TICKTERM	= (60*1000),	// 서비스 구조 요청 간격

	EV_REFRESHWORLDVIEW_TICKTERM		= (5*1000),		// 월드 정보 갱신 요청 간격

	EV_RECONNECT_TICKTERM				= (5*1000),		// 서비스매니져에 재연결 요청 간격
};

enum EF_UI_SIZE		// UI 크기 정의 (공용)
{
	EV_SERVERDRAW_START_X				= 30,			// 서버 표시 시작 X
	EV_SERVERDRAW_START_Y				= 30,			// 서버 표시 시작 Y

	EV_SERVERDRAW_GAP_X					= 5,			// 서버 표시 사이 간격 X
	EV_SERVERDRAW_GAP_Y					= 10,			// 서버 표시 사이 간격 Y

	EV_SERVERDRAW_PAD_X					= 20,			// 서버 표시 외부 간격 X
	EV_SERVERDRAW_PAD_Y					= 20,			// 서버 표시 외부 간격 Y

	EV_SERVERDRAW_BASE_WTH				= 80,			// 서버 표시 기본 너비
	EV_SERVERDRAW_BASE_HGT				= 35,			// 서버 표시 기본 너비

//	EV_SERVERDRAW_WORLDINFO_WTH			= 300,			// 서버 표시 월드 정보 너비
	EV_SERVERDRAW_WORLDINFO_HGT			= 30,			// 서버 표시 월드 정보 높이

//	EV_SERVERDRAW_TYPENAME_WTH			= 300,			// 서버 표시 타입 이름 너비
	EV_SERVERDRAW_TYPENAME_HGT			= 20,			// 서버 표시 타입 이름 높이
	EV_SERVERDRAW_TYPENAME_PAD_LFT		= 10,			// 서버 표시 타입 이름 외부 간격 Y

//	EV_SERVERDRAW_SEPERATOR_WTH			= 300,			// 서버 표시 분리자 너비
	EV_SERVERDRAW_SEPERATOR_HGT			= 15,			// 서버 표시 분리자 높이

	EV_SERVERDRAW_SERVERID_LO_PAD_Y		= 2,			// 서버 표시 채널 번호 여유 간격 Y (LO)
	EV_SERVERDRAW_SERVERID_VI_PAD_Y		= 2,			// 서버 표시 채널 번호 여유 간격 Y (VI)
	EV_SERVERDRAW_SERVERID_GA_PAD_Y		= 2,			// 서버 표시 채널 번호 여유 간격 Y (GA)
	EV_SERVERDRAW_SERVERID_DB_PAD_Y		= 2,			// 서버 표시 채널 번호 여유 간격 Y (LO)

	EV_SERVERDRAW_USERCNT_LO_PAD_Y		= 20,			// 서버 표시 사용자 수 여유 간격 Y (LO)
	EV_SERVERDRAW_USERCNT_LO_GAP_X		= 4,			// 서버 표시 사용자 수 사이 간격 X (LO)
	EV_SERVERDRAW_USERCNT_LO_HGT		= 20,			// 서버 표시 사용자 수 높이 (LO)

	EV_SERVERDRAW_USERCNT_VI_PAD_Y		= 20,			// 서버 표시 사용자 수 여유 간격 Y (VI)
	EV_SERVERDRAW_USERCNT_VI_GAP_X		= 4,			// 서버 표시 사용자 수 사이 간격 X (VI)
	EV_SERVERDRAW_USERCNT_VI_HGT		= 20,			// 서버 표시 사용자 수 높이 (VI)

	EV_SERVERDRAW_DELAYCNT_DB_PAD_Y		= 20,			// 서버 표시 딜레이 개수 여유 간격 Y (DB)
	EV_SERVERDRAW_DELAYCNT_DB_GAP_X		= 4,			// 서버 표시 딜레이 개수 사이 간격 X (DB)
	EV_SERVERDRAW_DELAYCNT_DB_HGT		= 20,			// 서버 표시 딜레이 개수 높이 (DB)

	EV_SERVERDRAW_USERCNT_GA_PAD_Y		= 20,			// 서버 표시 사용자 수 여유 간격 Y (GA)
	EV_SERVERDRAW_USERCNT_GA_GAP_X		= 4,			// 서버 표시 사용자 수 사이 간격 X (GA)
	EV_SERVERDRAW_USERCNT_GA_GAP_Y		= 4,			// 서버 표시 사용자 수 사이 간격 Y (GA)
	EV_SERVERDRAW_USERCNT_GA_HGT		= 20,			// 서버 표시 사용자 수 높이 (GA)

	EV_SERVERDRAW_ROOMCNT_GA_PAD_Y		= 40,			// 서버 표시 방 개수 여유 간격 Y (GA)
	EV_SERVERDRAW_ROOMCNT_GA_GAP_X		= 4,			// 서버 표시 방 개수 사이 간격 X (GA)
	EV_SERVERDRAW_ROOMCNT_GA_HGT		= 20,			// 서버 표시 방 개수 높이 (GA)

	EV_SERVERDRAW_DELAYCNT_GA_PAD_Y		= 60,			// 서버 표시 딜레이 개수 여유 간격 Y (GA)
	EV_SERVERDRAW_DELAYCNT_GA_GAP_X		= 4,			// 서버 표시 딜레이 개수 사이 간격 X (GA)
	EV_SERVERDRAW_DELAYCNT_GA_HGT		= 20,			// 서버 표시 딜레이 개수 높이 (GA)

	EV_SERVERDRAW_SCROLL_PAD_X			= 200,			// 서버 표시 스크롤 여유 간격 X
	EV_SERVERDRAW_SCROLL_PAD_Y			= 300,			// 서버 표시 스크롤 여유 간격 Y
};

enum EF_IMAGELISTTYPE		// 이미지 리스트 타입 (IDB_IMG_SMALL, IDB_IMG_LARGE)
{
	EV_IML_MONITOR			= 0,
	EV_IML_WORLDVIEW		= 1,
	EV_IML_INFORMATION		= 9,
	EV_IML_ERRORLOG			= 2,
	EV_IML_ALERT			= 8,
	EV_IML_PARTITION		= 10,
};

enum EF_SERVERTYPE			// 서버 타입
{
	EV_SVT_NONE,
	EV_SVT_LO,				// Login
	EV_SVT_DB,				// DB (Middleware)
	EV_SVT_MA,				// Master
	EV_SVT_VI,				// Village
	EV_SVT_GA,				// Game
	EV_SVT_LG,				// Log
	EV_SVT_CA,				// Cash
	EV_SVT_SM,				// Service Manager
	EV_SVT_CNT,
};

enum EF_SERVERSTATE			// 서버 상태 (eServiceState 와 같아야 함)
{
	EV_SVS_NONE,			// 비연결 상태
	EV_SVS_BOOT,			// 시동중 상태
	EV_SVS_CNNT,			// 연결 상태
	EV_SVS_TMNT,			// 예외종료 상태
	EV_SVS_HIDE,			// 숨김 상태 (게임서버만 해당됨)	// ※ 주의 - eServiceState 에는 없는 상태
	EV_SVS_CNT,
};

enum EF_SERVICEMNGRTYPE		// 서비스 정보 관리 타입
{
	EV_SMT_REAL,			// 실제
	EV_SMT_TEMP,			// 임시
	EV_SMT_CNT,
};

enum EF_SERVICERECVTYPE		// 서비스 정보 수신 타입
{
	EV_SRT_STRUCT,			// 서버 구조 구축
	EV_SRT_REALTIME,		// 서버 정보 실시간 수신
	EV_SRT_CNT,
};

enum EF_TIMERID				// 타이머 ID
{
	EV_TMD_REQUESTSERVICEINFO = EV_TIMERID_DEF,		// 서비스 정보 요청
	EV_TMD_REFRESHSERVICESTATUS,					// 서비스 상태 갱신
	EV_TMD_REFRESHSERVICEINFO,						// 서비스 정보 갱신
	EV_TMD_CHECKSERVERSTRUCT,						// 서버 구조 체크
	EV_TMD_CHECKSERVERSTATE,						// 서버 상태 체크
	EV_TMD_SHOWSERVERSTATE,							// 서버 상태 표시
	EV_TMD_SHOWETCINFO,								// 기타 정보 표시
	EV_TMD_SHOWERRORLOG,							// 에러 로그 표시
	EV_TMD_UPDATECONNECTDIALOG,						// 연결 대화상자 UI 갱신
	EV_TMD_CNT,
};

enum EF_CONTROLTYPE			// 제어 타입
{
	EV_CTT_PROCONOFF,			// Process On / Off
	EV_CTT_WORLDUSERLIMIT,		// World User Limit
	EV_CTT_VICHNLSHOWHIDE,		// Village Server Channel Show / Hide
	EV_CTT_VICHNLMERIT,			// Village Server Merit
	EV_CTT_GASHOWHIDE,			// Game Server Show Hide
	EV_CTT_GAMERIT,				// Game Server Merit
	EV_CTT_PROCMAKEDUMP,		// Process Make Dump
	EV_CTT_CNT,
};

static const CSize g_ServerTypeSize[EV_SVT_CNT] = {		// 서버 타입 별 크기 (너비, 높이)
	CSize(	0,		0	),			
	CSize(	110,	45	),		// Login
	CSize(	110,	45	),		// DB (Middleware)
	CSize(	80,		25	),		// Master
	CSize(	110,	45	),		// Village
	CSize(	110,	85	),		// Game
	CSize(	80,		25	),		// Log
	CSize(	80,		25	),		// Cash
	CSize(	80,		25	),		// Service Manager
};

static const TCHAR g_ServerTypeFullName[EV_SVT_CNT][EV_SERVERNAME_MAXLEN] = {	// 서버 타입 별 이름 (전체)
	_T(""),
	_T("Login Server"),			// Login
	_T("Middleware Server"),	// DB (Middleware)
	_T("Master Server"),		// Master
	_T("Village Server"),		// Village
	_T("Game Server"),			// Game
	_T("Log Server"),			// Log
	_T("Cash Server"),			// Cash
	_T("Service Manager"),		// Service Manager
};

static const TCHAR g_ServerTypeMiniName[EV_SVT_CNT][EV_SERVERNAME_MAXLEN] = {	// 서버 타입 별 이름 (축약)
	_T(""),
	_T("LO"),					// Login
	_T("MD"),					// DB (Middleware)
	_T("MA"),					// Master
	_T("VI"),					// Village
	_T("GA"),					// Game
	_T("LG"),					// Log
	_T("CA"),					// Cash
	_T("SM"),					// Service Manager
};

static const TCHAR g_ControlTypeName[EV_CTT_CNT][MAX_PATH] = {	// 제어 타입 별 이름
	_T("Process On / Off"),						// EV_CTT_PROCONOFF
	_T("World User Limit"),						// EV_CTT_WORLDUSERLIMIT
	_T("Village Server Channel Show / Hide"),	// EV_CTT_VICHNLSHOWHIDE
	_T("Village Server Channel Merit"),			// EV_CTT_VICHNLMERIT
	_T("Game Server Show / Hide"),				// EV_CTT_GASHOWHIDE
	_T("Game Server Merit"),					// EV_CTT_GAMERIT
	_T("Process Make Dump"),					// EV_CTT_PROCMAKEDUMP
};

static const TCHAR g_ServerStateName[EV_SVS_CNT][MAX_PATH] = {	// 서버 상태 별 명칭
	_T("Closed"),				// EV_SVS_NONE
	_T("Starting"),				// EV_SVS_BOOT
	_T("Running"),				// EV_SVS_CNNT
	_T("Terminated"),			// EV_SVS_TMNT
	_T("Hide"),					// EV_SVS_HIDE
};

static const COLORREF g_ServerStateTxColor[EV_SVS_CNT][2] = {	// 서버 상태 글자 색상 (해제/선택)
	RGB(255, 255, 255),			// 비연결 상태 (해제)
	RGB(  0,   0,   0),			// 비연결 상태 (선택)
	RGB(  0,   0,   0),			// 시동중 상태 (해제)
	RGB(  0,   0,   0),			// 시동중 상태 (선택)
	RGB(255, 255, 255),			// 연결 상태 (해제)
	RGB(  0,   0,   0),			// 연결 상태 (선택)
	RGB(255, 255, 255),			// 예외종료 상태 (해제)
	RGB(  0,   0,   0),			// 예외종료 상태 (선택)
	RGB(255, 255, 255),			// 숨김 상태 (해제)
	RGB(  0,   0,   0),			// 숨김 상태 (선택)
};

static const COLORREF g_ServerStateBkColor[EV_SVS_CNT] = {		// 서버 상태 배경 색상
	RGB(  0,   0, 160),			// 비연결 상태
//	RGB(215,  28,  32),			// 비연결 상태
	RGB(255, 255,  38),			// 시동중 상태
	RGB(16,  188,  41),			// 연결 상태
	RGB(215,  28,  32),			// 예외종료 상태
	RGB(172, 172, 172),			// 숨김 상태
};

static const COLORREF g_ServiceExceptionLogColor[3][2] = {		// 서비스 예외 로그 글자 색상 (글자/배경)
	RGB(  0,   0,   0),			// 일반 (글자)
	RGB(255, 255, 255),			// 일반 (배경)
	RGB(  0,   0,   0),			// 예외 (글자)
	RGB(255, 255,   0),			// 예외 (배경)
	RGB(255, 255, 255),			// 심각 (글자)
	RGB(184,  20,  57),			// 심각 (배경)
};

static const COLORREF g_ServerUserCountColor = RGB(167, 250, 248);

static const COLORREF g_ServerRoomCountColor = RGB(167, 250, 248);

static const DWORD g_TimerInterval[EV_TMD_CNT] = {	// 이벤트 타이머 별 간격
	(1000),		// 서비스 정보 요청
	(1000),		// 서비스 상태 갱신
	(1000),		// 서비스 정보 갱신
	(1000),		// 서버 구조 체크
	(1000),		// 서버 상태 체크
	(100),		// 서버 상태 표시
	(1000),		// 기타 정보 표시
	(1000),		// 에러 로그 표시
	(1000),		// 연결 대화상자 UI 갱신
};

static const TCHAR g_ExceptionReportName[][MAX_PATH] = {		// 예외 리포트 이름 (eExceptionRepert 와 같아야 함)
	_T("None"),						// _EXCEPTIONTYPE_NONE				// ??
	_T("Room Crashed"),				// _EXCEPTIONTYPE_ROOMCRASH			// GA
	_T("Session Crashed"),			// _EXCEPTIONTYPE_SESSIONCRASH		// GA
	_T("Room Destroyed"),			// _EXCEPTIONTYPE_ROOMDESTRUCTOR	// GA
	_T("Game Frame Delayed"),		// _EXCEPTIONTYPE_GAME_DELAYED		// GA
	_T("DB Middleware Delayed"),	// _EXCEPTIONTYPE_DBMW_DELAYED		// DB
// 	_T("<N/A>"),					// N/A
// 	_T("<N/A>"),					// N/A
// 	_T("<N/A>"),					// N/A
// 	_T("<N/A>"),					// N/A
};

