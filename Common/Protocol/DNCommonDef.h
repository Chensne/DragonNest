#pragma once

#include "DNItemTypeDef.h"

#include "Version.h"
namespace SecondPassword
{
	static char* GetVersion()
	{
#if defined (_WORK)
		return "1.1111.0";
#else
		return szVersion;
#endif
	}
}

/*--------------------------------------------------------------------------
					Client, Server 공통으로 쓰는 define
--------------------------------------------------------------------------*/
//DRAGON GEMS
const int MAX_GEM_LIST = 500; //to be incrased if necessary

//DARK JOBS
const int DARK_DEFAULT_LEVEL = 60;  //RLKT_DARK, Start level for Dark Class !
const int DARK_JOBMAX = 11;  //RLKT_DARK Max job + dark avenger  +silver 

const int CLIENTPACKETMAX = 4096;

const char WORLDCOUNTMAX		= 110;	// 총 월드 개수
const char VILLAGECOUNTMAX		= 12;	// 총 빌리지 서버 개수
const short VILLAGECHANNELMAX	= 255;	// 빌리지 채널맥수
const short NETLAUNCHERLISTMAX	= 100;	// 런처보고용 맥수
const short MERITINFOMAX		= 100;	// 메리트 테이블 맥스
const short STATECOUNTMAX		= 100;	// 모니터보고용 스테이트맥스
const char GAMECOUNTMAX			= 64;	// 총 게임 서버 개수
const short REPORTGAMEINFOMAX = 300;	//게임서버 인원정보 보내는 맥스

const unsigned char CHANNELNAMEMAX		= 128;		//채널이름맥스
const unsigned char SERVERINFOCOUNTMAX = 30;
const int SERVERCONTROLMAX = 200;		// 서버모니터에서 서비스 컨트롤 항목 맥스

const int EXCUTELENMAX = 64;			//
const int EXCUTECMDMAX = 2048 * 2;

const char SERVERVERSIONMAX		= 16;	// 서버 버젼 길이

const BYTE IPLENMAX				= 32;
const int MACHINEIDMAX			= 16;
const BYTE NHNNETCAFECODEMAX	= 12;
const BYTE MACADDRLENMAX		= 20;

#if defined (_KR) || defined(_KRAZ)
const BYTE IDLENMAX				= 32;	// 계정명
#elif defined(_CH)
const BYTE IDLENMAX				= 41;	// 계정명
#elif defined (_TH)
const BYTE IDLENMAX				= 33;	// 아지아솝흐트 계정사이즈 (널포인터포함)
#elif defined (_ID)
const BYTE IDLENMAX				= 13;	// 꾸레옹 계정사이즈
#else
const BYTE IDLENMAX				= 32;	// 계정명
#endif

const int ONEDAYSEC	= 60*60*24;

const int SNDAOAPASSPORTMAX		= 1024;		// SNDA OA 인증 패스포트 최대 길이 (SNDA OA 인증)

const int SECONDAUTHIGNOREPERIOD = 3;		//day
#if defined (_TH) || defined(_ID)
const BYTE PASSWORDLENMAX		= 33;		// 아지아솝흐트 패스워드 사이즈 (널포인터포함), 인도네시아 MD5(
#else		//#if defined (_TH)
const BYTE PASSWORDLENMAX		= 31;
#endif		//#if defined (_TH)
const BYTE DOMAINLENMAX			= 9;		// 아지아솝흐트 도메인 사이즈 (널포인터포함)(어디계정인지?) THPP : Thai PlayparkID, THAS : Thai AsiaSoftID, THTC : Thai TCGID
const BYTE KREONKEYMAX			= 40;		// 크레온 PC Cafe Key Max
const BYTE MAILRUOTPMAX			= 65;		// 러시아 인증 OTP길이 (NULL포함)

#if defined (_KR) || defined(_KRAZ)
const BYTE CHARNAMEMIN			= 2;	// 캐릭터이름 최소글자
#elif defined( _US ) || defined(_SG) || defined(_TH) || defined(_ID) || defined(_RU) || defined (_EU)
const BYTE CHARNAMEMIN			= 4;	// 캐릭터이름 최소글자
#else
const BYTE CHARNAMEMIN			= 2;
#endif

#if defined(_TH)
const BYTE CHARSIZEMAX		= 5;	// 태국의 글자크기 매직상수값(한 글자가 최대 5개의 버퍼 사용)
#else
const BYTE CHARSIZEMAX		= 1;
#endif

#if defined(_TH)
const BYTE NAMELENMAX			= 30;	// 캐릭명 널문자 포함
#else	// #if defined(_TH)
const BYTE NAMELENMAX			= 17;	// 캐릭명 널문자 포함
#endif	// #if defined(_TH)
const BYTE WORLDNAMELENMAX		= 64;	// 월드이름
const BYTE EXTDATANAMELENMAX	= 128;	// EXT DATA에 쓰이는 이름들 길이
const BYTE CHERRYLOGINIDMAX		= 81;	// 싱말 로그인 아이디최대(이메일도 가능) 널문자 포함

const USHORT CHATLENMAX			= 512;	// chat max
const BYTE MAILNAMELENMAX		= 33;	// 우편용 캐릭터이름 (널문자포함)

const int RESERVEDPERIOD_CHARNAME = 7; // 변경전 캐릭터 이름이 보존되는 기간
const int NEXTPERIOD_CHARNAME	= 7; // 최근 이름 변경 일자로부터 다음 이름 변경이 가능한 기간 제한. 단위 : day
const BYTE CHANGECODE_CHARNAME	= 2; // 캐릭터 이름 변경 아이템 사용 코드

#if defined(_EU)
const short PARTYNAMELENMAX		= 26;	// 25자
#else	// #if defined(_EU)
const short PARTYNAMELENMAX		= 21;	// 파티명 최대 20
#endif	// #if defined(_EU)
const int MAXPARTYPASSWORD		= 9999;
#if defined( PRE_PARTY_DB )
#else
const BYTE PARTYPASSWORDMAX		= 5;
#endif // #if defined( PRE_PARTY_DB )
//const char PARTYMAX				= 32;	// 파티원 최대(PvP포함, 공격대포함) 이제 이름 바까야 할듯? ㅋ
const char PARTYMAX				= 48;	// 파티원 최대(PvP포함, 공격대포함) 이제 이름 바까야 할듯? ㅋ

#if defined (PRE_ADD_BESTFRIEND)
const short BESTFRIENDMEMOMAX  =  40;
#endif // #if defined (PRE_ADD_BESTFRIEND)

const char PARTYCOUNTMAX		= 8;	// 최대 인원 정보는 PARTYMAX를 사용 게임내에 사용되는 Party의 MemberMax는 PARTYCOUNTMAX를 사용한다
const char NORMPARTYCOUNTMAX	= 4;	// 일반파티 최대인원
const char NORMPARTYCOUNTMIN	= 2;	// 일반파티 최소인원
const char RAIDPARTYCOUNTMAX	= 8;	// 공격대파티 최대인원
const char RAIDPARTYCOUNTMIN	= 5;	// 공격대파티 최소인원

enum ePartyType
{
	_NORMAL_PARTY		= 0,	//일반 4인파티
	_RAID_PARTY_8		= 1,	//레이드용 8인파티
#if defined( PRE_WORLDCOMBINE_PARTY )
	_WORLDCOMBINE_PARTY	= 2,	// 월드통합 파티
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	_PARTY_TYPE_MAX
};

#if defined(PRE_EXPANDGATE)
const char WORLDMAP_GATECOUNT = 15;
#else
const char WORLDMAP_GATECOUNT = 10;
#endif
const char PARTYKICKMAX = 10;			//파티 정보 이동시 가저가는 리스트
const BYTE ISOLATELISTMAX = 100;		//차단자 최대수
#ifdef PRE_PARTY_DB
#else
const int PARTYSORTCONDITIONMAX = 3;	// 파티 리스트 조건검색시 최대 검색인자 수
#endif
const int PERMITMAPPASSCLASSMAX = 10;	//맵입장 조건 검사를 패스하는 클래스 아이디 리스트 맥스

const LONG GAMEINVITEWAITTIME = 1000 * 60 * 2;

const int PARTYLISTOFFSET		= 7;	//파티리스트 페이지 오프셋
const int QUICKPARTYCOUNTMAX	= 20;	//퀵조인시 수집최대 값

const char CHATROOMMAX			= 20;	// 채팅방 최대 인원

const INT64 COINMAX				= 50000000000;	// 최대코인값 42억

const INT64 GUILDWARE_COINMAX   = COINMAX / 2;

const BYTE UNSEQUENCEPACKET		= 0;	//순차적패킷아님
const BYTE COMPRESSPACKET		= 2;	//압축되어진 패킷
const BYTE COMPRESSMINSIZE		= 30;	//압축을 하기위한 미니멈사이즈 압축을 행하였을 경우 효과를 얻기위한 최소사이즈
//절대적수치는 아님 1만바이트가 모두 순차적이지 않은 난수로 되어 있다면 지지데쓰

const short RESTRAINTREASONMAX	= 300;	//유저제재 사유(이유)에 대한 사이즈맥스값
const short RESTRAINTREASONFORDOLISMAX	= 100;	//유저제재 사유(이유)에 대한 사이즈맥스값(DOLIS표기값)
const BYTE RESTRAINTMAX			= 10;	//제재최대값

#if defined (_US) || defined (_SG) || defined(_ID)
const BYTE PROFILEGREETINGLEN	= 50;	// 프로필 자기소개(인사말) 최대 길이 (널문자 제외)
#else		//#if defined (_US) || defined (_SG)
const BYTE PROFILEGREETINGLEN	= 24;	// 프로필 자기소개(인사말) 최대 길이
#endif		//#if defined (_US) || defined (_SG)
// item
const BYTE INVENTORYMAX			= 150;	// 일반아이템 인벤용
const BYTE WAREHOUSEMAX			= 150;	// 일반아이템 창고용
const BYTE QUESTINVENTORYMAX	= 60;	// 퀘스트 인벤용

const BYTE DEFAULTINVENTORYMAX	= 30;	// 기본 인벤 개수
const BYTE DEFAULTWAREHOUSEMAX	= 10;	// 기본 창고 개수

const BYTE PERIODINVENTORYMAX	= 30;
const BYTE PERIODWAREHOUSEMAX	= 30;

const BYTE INVENTORYTOTALMAX	= INVENTORYMAX + PERIODINVENTORYMAX;
const BYTE WAREHOUSETOTALMAX	= WAREHOUSEMAX + PERIODWAREHOUSEMAX;

const BYTE CASHINVENTORYMAX		= 150;	// 캐쉬아이템 인벤용
const BYTE CASHINVENTORYDBMAX	= 200;	// 캐쉬아이템 인벤용
const BYTE CASHINVENTORYPAGEMAX = 100;	// 캐쉬인벤 1페이지 맥스값

const BYTE MAXDELEXPIREITEM = 150;

const BYTE VEHICLEINVENTORYPAGEMAX	= 50;	// 탈것 인벤 1페이지 맥스값

const BYTE MISSINGITEMMAX		= 50;	// 

#if defined( PRE_ADD_SKILLSLOT_EXTEND )
const BYTE QUICKSLOTMAX			= 60;	// 단축 슬롯창 12칸씩 2줄 x 2(기본스킬 + 확장스킬) + 1중(제스처 소셜액션용)
const BYTE QUICKSLOTSEPARATE	= 48;	// 제스쳐
#else // #if defined( PRE_ADD_SKILLSLOT_EXTEND )
const BYTE QUICKSLOTMAX			= 50;	// 단축 슬롯창 10칸씩 2줄 + 1중(제스처 소셜액션용)
const BYTE QUICKSLOTSEPARATE	= 40;	// 제스쳐
#endif	// #if defined( PRE_ADD_SKILLSLOT_EXTEND )

const BYTE SKILLMAX				= 100;	// 스킬 최대 개수
const BYTE GESTUREMAX			= 100;	// 제스처 최대 개수

const BYTE SKILLPRESETMAX		= 3;	// 저장스킬프리셋 최대갯수
const BYTE SKILLPRESETNAMEMAX	= 13;	// 저장스킬프리셋이름 최대

const BYTE CASHGLYPHSLOTMAX		= 3;	//캐쉬 문장 슬롯 최대 갯수

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
const BYTE EFFECTSKILLMAX		= 50;	// 이펙트 스킬 최대 갯수
#endif
#if defined(PRE_ADD_MACHINA) && defined(PRE_ADD_LENCEA) && defined(PRE_ADD_ASSASSIN) && defined(PRE_ADD_KALI) && defined(PRE_ADD_ACADEMIC)
const BYTE CLASSKINDMAX			= 9;	// kat 2016
#elif defined(PRE_ADD_LENCEA) && defined(PRE_ADD_ASSASSIN) && defined(PRE_ADD_KALI) && defined(PRE_ADD_ACADEMIC)
const BYTE CLASSKINDMAX			= 8;	//
#elif defined(PRE_ADD_ASSASSIN) && defined(PRE_ADD_KALI) && defined(PRE_ADD_ACADEMIC)
const BYTE CLASSKINDMAX			= 7;	// 워리어, 아처, 소서리스, 클레릭, 아카데믹, 칼리, 어쌔씬
#elif defined(PRE_ADD_KALI) && defined(PRE_ADD_ACADEMIC)
const BYTE CLASSKINDMAX			= 6;	// 워리어, 아처, 소서리스, 클레릭, 아카데믹, 칼리
#elif defined(PRE_ADD_ACADEMIC)
const BYTE CLASSKINDMAX			= 5;	// 워리어, 아처, 소서리스, 클레릭, 아카데믹
#else
const BYTE CLASSKINDMAX			= 4;	// 워리어, 아처, 소서리스, 클레릭
#endif // #ifdef PRE_ADD_ACADEMIC
//job max changed to 10 from 8
const BYTE JOBMAX				= 10;	// 전직 최대개수(?)	// 이거 네이밍이 혼란의 여지가 많다..나중에 바꾸던가..해야할듯......
const BYTE JOB_KIND_MAX			= 100;	// 최대 직업 개수
const BYTE CHARLEVELMAX			= 100;	// 캐릭터 레벨 최대값

const BYTE DailyCreateCharacterMax = 7;	// 하루에 만들 수 있는 캐릭수

#if !defined(PRE_MOD_SELECT_CHAR)
#if defined(_JP)
const BYTE CreateCharacterDefaultCountMax = 8;	// 캐릭생성 맥스 - 일본만 기본 8개로 확장 20110908
#else	// #if defined(_JP)
const BYTE CreateCharacterDefaultCountMax = 4;	// 캐릭생성 맥스
#endif	// #if defined(_JP)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined(PRE_MOD_SELECT_CHAR)
const BYTE CHARCOUNTMAX			= 50;	// 캐릭터 만드는 최대개수
#else	// #if defined(PRE_MOD_SELECT_CHAR)
const BYTE CHARCOUNTMAX			= 30;	// 캐릭터 만드는 최대개수
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined(PRE_ADD_CHANNELNAME)
const BYTE CHANNELLANGUAGEMAX	= 4;	// 채널 언어명
#endif //#if defined(PRE_ADD_CHANNELNAME)
const BYTE MAPNAMELENMAX		= 64;	// 맵이름 최대갯수

const BYTE DEFAULTITEMMAX		= 25;	// 캐릭터 생성시 넣어줄 아이템 최대개수
const BYTE DEFAULTPARTSMAX		= 10;	// 캐릭터 생성시 비교할 기본 파츠 최대개수
const BYTE DEFAULTPOSITIONMAX	= 10;	// 캐릭터 생성시 시작위치 최대개수
const BYTE DEFAULTSKILLMAX		= 10;	// 캐릭터 생성시 넣어줄 스킬
const BYTE DEFAULTUNLOCKSKILLMAX	= 20;	// 캐릭터 생성시 넣어줄 언락스킬
#if defined(PRE_ADD_SKILLSLOT_EXTEND)
const BYTE DEFAULTQUICKSLOTMAX	= 60;	// 캐릭터 생성시 퀵슬롯 최대개수
#else	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)
const BYTE DEFAULTQUICKSLOTMAX	= 50;	// 캐릭터 생성시 퀵슬롯 최대개수
#endif	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)
const BYTE WEAPONMAX			= 2;	// 무기 최대개수
const BYTE DEFAULTGESTUREMAX	= 10;	// 캐릭터 생성시 넣어줄 기본제스처(소셜액션)갯수
const BYTE DEFAULTHAIRCOLORMAX	= 5; //rlkt ioncrease
const BYTE DEFAULTSKINCOLORMAX	= 4; //here 4!
const BYTE DEFAULTEYECOLORMAX	= 5;

const BYTE SHOPTABMAX			= 6;	// 상점 텝 최대개수
const BYTE SHOPITEMMAX			= 60;	// 상점 아이템 최대 개수
//const BYTE REBUYITEMMAX			= 25;	// 재매입 개수
const BYTE SKILLSHOPITEMMAX		= 10;	// 스킬 상점 아이템 최대 개수

const BYTE SENDDROPITEMLISTMAX	= 30;	// 난입시 DropItem 한번에 보내는 최대 개수

const BYTE SENDCHATROOMVIEWMAX  = 10;	// 난입시 ChatRoomView 한번에 보내는 최대 개수

const BYTE MAILTITLELENMAX		= 30;	// 우편 제목
const BYTE MAILDATELENMAX		= 20;	// 우편 날짜
const BYTE MAILTEXTLENMAX		= 198;	// 우편 내용
const BYTE MAILBOXMAX			= 30;	// 우편함 맥스 일단 100개
const BYTE MAILATTACHITEMMAX	= 5;	// 편지쓰기 아이템 맥스 5개
const char MAILPAGEMAX = 6;

const BYTE MARKETPAGEMAX		= 5;			// 1페이지 맥스값
const BYTE MARKETMAX			= 5 * 10;		// 마켓 리스트 맥스 (5 * 10페이지)
const int MYMARKETMAX			= 25;			// 내 마켓 리스트

const int MARKETREGISTERMAX		= 100;			// 마켓등록 최대개수
const int MARKETSELLSUCCESSMAX	= 1000000000;	// 판매성공횟수
const int MARKETSELLFAILUREMAX	= 1000000000;	// 판매실패횟수
const int MARKETREGISTERTAXMAX	= 1000000000;	// 누적 등록 수수료
const int MARKETSELLTAXMAX		= 1000000000;	// 누적 판매 수수료

const int SALEABORTLISTMAX		= 30;		// 판매중지 리스트 최대값

const int CARTLISTMAX			= 10;		// 장바구니 최대개수
const int PREVIEWCARTLISTMAX	= 20;		// 미리보기 장바구니 최대개수
const int GIFTMESSAGEMAX		= 51;		// 선물창 메모 (국가별로 나누기 귀찮아서 50으로 통일)
const int GIFTBOXLISTMAX		= 100;		// 선물창
const int GIFTPAGEMAX			= 8;
#if defined(_TW) || defined (_SG)
const int COUPONMAX				= 21;		// 대만 쿠폰
#else
const int COUPONMAX				= 31;		// 쿠폰
#endif
const int PACKAGEITEMMAX		= 20;
const int COUPONITEMMAX			= 10;		// 쿠폰 아이템 최대수
const int PUBLISHERORDERKEYMAX	= 32 + 1;		// 퍼블리셔 발급 주문번호 최대 크기

const BYTE SEARCHWORDMAX		= 3;	// 검색 단어 최대개수
const BYTE SEARCHLENMAX			= 50;	// 무인상점 검색
const BYTE EXCHANGEMAX			= 10;	// 개인거래 맥스값

const BYTE COMPOUNDITEMMAX = 10;	// 문장 보옥 합성시 최대 보옥 갯수

const BYTE DUNGEONCLEARMAX = 120;		// 던젼 클리어 개수
const BYTE NESTCLEARMAX = 50;			// nest 클리어

const int MAXPARTYDICEGAMENUMBER = 100;	// 파티 아이템 분배 시 나올 수 있는 최대 주사위 숫자
const int NPCPRESENTMAX = 50;			// Npc 선물 최대 갯수

const int MISSIONMAX = 10000;
const int APPELLATIONMAX = 10000;
const BYTE PERIODAPPELLATIONMAX = 50;	//기간제 칭호 최대 갯수
const int LASTMISSIONACHIEVEMAX = 3;

const int MISSIONMAX_BITSIZE = MISSIONMAX / 8;
const int TIMEEVENTMAX = 20;

const int PCBangMissionMax = 20;
const int DAILYMISSIONMAX = 15;		// 일단 15개로 늘립니다.
const int WEEKLYMISSIONMAX = 15;	// 일단 15개로 늘립니다.
const int GUILDWARMISSIONMAX = 15;	
const int GUILDCOMMONMISSIONMAX = 5;		
const int WEEKENDEVENTMISSIONMAX = 15;
const int WEEKENDREPEATMISSIONMAX = 15;
#if defined(PRE_ADD_MONTHLY_MISSION)
const int MONTHLYMISSIONMAX = 5;
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)

const int VIEWCASHEQUIPMAX_BITSIZE = 4;
const int APPELLATIONMAX_BITSIZE = APPELLATIONMAX / 8;

const int MISSIONREWARDITEMMAX = 3;

const int MAXDICERANGE = 1000;
const int VOICEUPDATETIME = 1000;

const int LIFESPANMAX = 5 * 365 * 24 * 60;	// 5년 (분)
const int REMOVECASHWAITINGTIME = 7 * 24 * 60;	// 7일 (분으로 계산)

const int ITEMDROP_DEPTH = 8;
const int MAXITEMDROPFIELD	= 20;	// 코스튬 합성 시 드랍할 수 있는 결과물 필드의 최대 개수
#if defined(_KR) || defined(_KRAZ)
const int MAXCOSMIXSTUFF	= 2;	// 코스튬 합성 재료 아이템 최대 개수
#else
const int MAXCOSMIXSTUFF	= 5;	// 코스튬 합성 재료 아이템 최대 개수
#endif // _KR

#if defined (PRE_ADD_BESTFRIEND)
const int BESTFRIENDMEMO_MAX = 40;	// 절친 메모 최대크기
const int BESTFRIEND_CLOSETIME = 4 * 24 * 60 * 60;	// 4일 (초)
#endif // #if defined (PRE_ADD_BESTFRIEND)

#if defined( PRE_ALTEIAWORLD_EXPLORE )
const int MAXALTEIADICENUMBER = 6;	// 알테이아 주사위 최대 크기
#endif

enum eCostumeDesignMixStuffType
{
	eCDMST_ABILITY,
	eCDMST_DESIGN,
	eCDMST_MAX
};
#ifdef PRE_ADD_COSRANDMIX
const int MAX_COSTUME_RANDOMMIX_STUFF = 2;	// 코스튬 랜덤 합성 시 재료아이템 최대 개수
#endif

#if defined (PRE_ADD_CHAOSCUBE)
const int MAX_CHAOSCUBE_STUFF = 10;
#endif

// 캐쉬템으로 판매되는 가챠폰 코인 아이템 아이디.
const int JP_GACHACOIN_ITEM_ID = 1107345408;

// 영구적용 아이템으로 사용된는 비약 아이템 아이디.
const int ETERNITY_ITEM_STR_ITEM_ID = 302006280;
const int ETERNITY_ITEM_DEX_ITEM_ID = 302007304;
const int ETERNITY_ITEM_INT_ITEM_ID = 302008328;
const int ETERNITY_ITEM_CON_ITEM_ID = 302009352;
const int ETERNITY_ITEM_ALL_ITEM_ID = 302050312;
const int ETERNITY_ITEM_SP_ITEM_ID = 302051336;

const BYTE ITEMCATAGORY_MAX = 50;
const BYTE ITEMEXCHANGEGROUP_MAX = 50;
const BYTE ITEMJOB_MAX = 10;

const BYTE PVPMISSIONROOMGROUP_MAX = 50;

//community
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
const BYTE DISPLAYOPTIONCOUNT = 15;
#else
const BYTE DISPLAYOPTIONCOUNT = 14;
#endif
const BYTE COMMDISPLAYOPTIONCOUNT = 3;
const BYTE COMMOPTIONCOUNT = 8;

//Friend
const int FRIEND_MAXCOUNT = 100;
const int FRIEND_GROUP_MAX = 10;
const int FRIEND_GROUP_NAMELENMAX = 50;
const int FRIEND_MEMO_LEN_MAX = 64;

//Chat
const int CHAT_PAPERING_CHECKTIME = 3;			// 3초안에
const int CHAT_PAPERING_CHECKCOUNT = 3;			// 3번 말하면,
const int CHAT_PAPERING_RESTRICTIONTIME = 30;	// 30초 제재

//Gesture
const int GESTURE_PAPERING_CHECKTIME = 3;		// 3초안에
const int GESTURE_PAPERING_CHECKCOUNT = 3;		// 3번 사용하면,
const int GESTURE_PAPERING_RESTRICTIONTIME = 10;// 10초 제재
const int GESTURE_UNLOCKLV_CASH = -1;				// 캐시아이템을 통한 제스쳐 해제

//Guild
const int GUILDDBID_DEF = 0;				// 길드 DBID (없음)
const int GUILDUID_DEF = 0;					// 길드 UID (없음)
const int GUILDSIZE_DEF = 20;				// 길드 기본크기
const int SENDGUILDMEMBER_MAX = 100;		// 길드원 한번에 보내는 최대 크기
const int GUILDSIZE_MAX = 300;				// 길드 최대크기 (= 기본크기 + 추가크기)

//RLKT Incrase Guild Name
const int GUILDNAME_MAX = 20+1;	// 20자
/*#if defined(_RU)
const int GUILDNAME_MAX = 14+1;	// [러시아] 길드 이름 최대크기 ( 4 ~ 14자 ) + 널문자 포함
#elif defined(_SG) || defined(_US) || defined(_EU)
const int GUILDNAME_MAX = 20+1;	// 20자
#else
const int GUILDNAME_MAX = 10*CHARSIZEMAX+1;	// 길드 이름 최대크기 + 널문자 포함 (태국때문에 CHARSIZEMAX 곱해야함)
#endif */ // _RU

const int GUILDNOTICE_MAX = 40;	// 길드 공지 최대크기
const int GUILDMONEY_MAX = 1000000000;		// 길드 자금 최대크기
const int GUILDCREATECHARLEVEL_DEF = 9;		// 길드 창설가능 기본레벨
const int GUILDPARAMTEXT_MAX = 40;			// 길드 파라메터 텍스트 최대크기
const int GUILDHISTORYTEXT_MAX = 40;		// 길드 히스토리 텍스트 최대크기(길드홈페이지도 같이 쓰고있으므로 바꿀떄 체크해야함)
const int GUILDHISTORYLIST_MAX = 20;		// 길드 히스토리 목록 최대크기
const int GUILDROLENAME_MAX = 6;			// 길드 역할이름 최대크기
const int GUILDMEMBINTRODUCE_MAX = 40;		// 길드원 자기소개 최대크기
const int GUILDMNGRSIZE_DEF = 100;			// 길드 관리자의 자원준비 기본크기
const int GUILDCMMPTR_MAX = 1000000000;		// 길드 일반 포인트 최대크기
const int GUILDLDDPTR_MAX = 1000000000;		// 길드 래더 포인트 최대크기
const BYTE GUILD_WAREHOUSE_MAX = 150;		// 길드 최대 창고크기
const BYTE GUILD_WAREHOUSE_HISTORYLIST_MAX = 20; // 길드 창고 히스토리 최대 크기
const int GUILDRECRUITINTRODUCE = 190;	// 길드모집게시판 소개글
const int GUILDWARPOINT_DEF = 2500;			// 길드전 기본 포인트
const short GUILDWARSCHEDULE_DEF = -1;
const int GUILDHOMEPAGE = 40;	// 길드모집게시판 홈페이지 주소

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
const int RANDOMGIVE_CHARMITEM_MAX = 100;
#endif		//#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE

//event
#if defined(PRE_ADD_WORLD_EVENT)
const short EVENTLISTMAX = 100;			//한월드 이벤트 설정 최대값
#else
const short EVENTLISTMAX = 20;			//한월드 이벤트 설정 최대값
#endif //#if defined(PRE_ADD_WORLD_EVENT)
const short EVENTCLEARBONUSMAX = 5;			//스테이지 클리어시 이벤트 중첩최대값

const int CHATROOMNAME_MAX		= 14;	// 채팅방 이름 최대 길이
const BYTE CHATROOMPASSWORDMAX	= 4;		// 채팅방 패스워드 최대 길이

const int ITEMSKILLMAXCOOLTIME = 7 * 24 * 60 * 60;// 스킬 최대 쿨타임

enum eGuildMedalType						// 길드 메달 타입
{
	GUILDMEDAL_TYPE_GOLD = 0,				// 금
	GUILDMEDAL_TYPE_SILVER,					// 은
	GUILDMEDAL_TYPE_BRONZE,					// 동
	GUILDMEDAL_TYPE_CNT,
};

enum eGuildRoleType							// 길드 직책 타입 (P.S.> 새로운 멤버는 반드시 뒤에 추가해야 함)
{
	GUILDROLE_TYPE_MASTER = 0,				// 길드장
	GUILDROLE_TYPE_SUBMASTER,				// 길드부장
	GUILDROLE_TYPE_SENIOR,					// 선임
	GUILDROLE_TYPE_REGULAR,					// 정식
	GUILDROLE_TYPE_JUNIOR,					// 신입
	GUILDROLE_TYPE_CNT,
};

enum eGuildAuthType							// 길드 권한 타입 (P.S.> 새로운 멤버는 반드시 뒤에 추가해야 하며 int 에 비트 플래그로 기록되므로 전체 개수가 이에 영향을 받는 것에 주의 필요)
{
	GUILDAUTH_TYPE_EXILE		= 0,				// 추방
	GUILDAUTH_TYPE_NOTICE		= 1,				// 공지
	GUILDAUTH_TYPE_INVITE		= 2,				// 초대
	GUILDAUTH_TYPE_BATTLE		= 3,				// 전투
	GUILDAUTH_TYPE_WAREHOUSE	= 4,				// 창고 <미사용>
	GUILDAUTH_TYPE_SKILL		= 5,				// 스킬

	GUILDAUTH_TYPE_STOREITEM	= 6,				// 아이템 보관
	GUILDAUTH_TYPE_TAKEITEM		= 7,				// 아이템 꺼내기
	GUILDAUTH_TYPE_WITHDRAW		= 8,				// 출금
	GUILDAUTH_TYPE_HOMEPAGE		= 9,				// 홈페이지 변경 
	GUILDAUTH_TYPE_CNT,
};

enum eGuildUpdateType						// 길드 정보 변경 타입					INT_1					INT_2					INT_3			INT_4		INT64_1			TEXT(40)		
{
	GUILDUPDATE_TYPE_NOTICE = 0,			// 공지 변경							-						-						-				-			-				공지내용
	GUILDUPDATE_TYPE_ROLENAME,				// 직급 이름 변경						직급번호				-						-				-			-				직급이름
	GUILDUPDATE_TYPE_ROLEAUTH,				// 직급 권한 변경						직급번호				권한플래그				아이템제한		코인제한	-				-
	GUILDUPDATE_TYPE_GUILDWAR,				// 길드전 참여, 우승횟수 증가			참여횟수증가			우승횟수증가			-			-				-			    -
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	GUILDUPDATE_TYPE_HOMEPAGE,				// 홈페이지 변경			            -						-						-			-				-			    홈페이지
#endif
	GUILDUPDATE_TYPE_CNT,
};

enum eGuildMemberUpdateType					// 길드원 정보 변경 타입				INT_1					INT_2					INT64_1					TEXT(40)
{
	GUILDMEMBUPDATE_TYPE_INTRODUCE = 0,		// 자기소개 변경 (길드원 자신)			-						-						-						소개내용
	GUILDMEMBUPDATE_TYPE_ROLE,				// 직급 변경 (길드장 → 길드원 ?)		직급번호 (길드장 제외)	캐릭터레벨				-						-
	GUILDMEMBUPDATE_TYPE_GUILDMASTER,		// 길드장 위임 (길드장 → 길드원 ?)		-						-						-						-
	GUILDMEMBUPDATE_TYPE_CMMPOINT,			// 일반 포인트 증가/감소 (기여/사용)	증감/사용 (IN/OUT)		누적 (OUT)				-						-
	GUILDMEMBUPDATE_TYPE_JOBCODE,			// 길드원 직업 변경						직업코드
	GUILDMEMBUPDATE_TYPE_LOGINOUT,			// 길드원 로그인 로그아웃				로그아웃:_LOCATION_NONE
											//										로그인  :_LOCATION_MOVE
	GUILDMEMBUPDATE_TYPE_CNT,
};

enum eGuildHistoryType						// 길드 히스토리 타입					INT_1					INT_2					INT64_1					TEXT(40)
{
	GUILDHISTORY_TYPE_BEGIN		=0,
	GUILDHISTORY_TYPE_CREATE	=1,				// 길드 창설							-						-						-						-
	GUILDHISTORY_TYPE_LEVELUP	=2,				// 길드 레벨업							길드레벨				-						-						-
	GUILDHISTORY_TYPE_JOIN		=3,				// 길드원 가입							-						-						-						캐릭터이름
	GUILDHISTORY_TYPE_LEAVE		=4,				// 길드원 탈퇴							-						-						-						캐릭터이름
	GUILDHISTORY_TYPE_EXILE		=5,				// 길드원 추방							-						-						-						캐릭터이름
	GUILDHISTORY_TYPE_ROLECHNG	=6,				// 길드원 직급변경						직급번호				-						-						캐릭터이름
	GUILDHISTORY_TYPE_MEMBLEVELUP=7,			// 길드원 레벨업 (10레벨)				길드원레벨				-						-						캐릭터이름
	GUILDHISTORY_TYPE_ITEMENCT	=8,				// 길드원 고강화 성공					강화단계				아이템번호			길드원캐릭터ID				캐릭터이름
	GUILDHISTORY_TYPE_CLOSE		=9,				// 길드폐쇄								-						-						-					
	GUILDHISTORY_TYPE_WARESIZE	=10,			// 길드 창고크기 변경					최종크기				-						-						캐릭터이름	
	GUILDHISTORY_TYPE_RENAME	=11,			// 길드 이름 변경						-						-						-						변경전이름,변경후이름
	GUILDHISTORY_TYPE_RECRUITSTART	=12,		// 길드모집 시작						-						-						-						-
	GUILDHISTORY_TYPE_RECRUITEND	=13,		// 길드모집 종료						-						-						-						-
	GUILDHISTORY_TYPE_CNT,

	// 아래 값은 사용하지 않습니다.
	GUILDHISTORY_TYPE_SKILLGAIN,			// 길드 스킬 획득						-						-						-						스킬이름
	GUILDHISTORY_TYPE_SKILLENFC,			// 길드 스킬 강화						-						-						-						스킬이름
	GUILDHISTORY_TYPE_MARKGAIN,				// 길드 마크 획득						전경번호				후경번호				클라이언트이미지		-
	GUILDHISTORY_TYPE_MARKCHNG,				// 길드 마크 변경						전경번호				후경번호				클라이언트이미지		-	
	GUILDHISTORY_TYPE_MISSION,				// 길드 미션 성공						미션번호				-						-						미션이름	
};

const char g_GuildHistoryLevel[GUILDHISTORY_TYPE_CNT] = {
	0,		// 길드 시작
	1,		// 길드 창설
	2,		// 길드 레벨업
	3,		// 길드원 가입
	4,		// 길드원 탈퇴
	5,		// 길드원 추방
	6,		// 길드원 직급변경
	7,		// 길드원 레벨업
	8,		// 길드원 고강화 성공
	9,		// 길드원 폐쇄
	10,		// 길드원 창고크기변경
	11,		// 길드 이름 변경
	12,		// 길드모집 시작
	13,		// 길드모집 종료
};

enum eGuildRewardEffect				//길드 적용 효과 타입
{
	GUILDREWARDEFFECT_TYPE_BEGIN					= 0,
	GUILDREWARDEFFECT_TYPE_GUILDITEM				= 1,		// 치장형아이템
	GUILDREWARDEFFECT_TYPE_MAKEGUILDMARK			= 2,		// 길드마크제작
	GUILDREWARDEFFECT_TYPE_MAXGUILDWARE				= 3,		// 길드창고슬롯증가
	GUILDREWARDEFFECT_TYPE_MAXMEMBER				= 4,		// 길드원수제한증가
	GUILDREWARDEFFECT_TYPE_EXTRAEXP					= 5,		// 추가경험치
	GUILDREWARDEFFECT_TYPE_EXTRAPVPEXP				= 6,		// 추가콜로세움경험치
	GUILDREWARDEFFECT_TYPE_EXTRAFESTPOINT			= 7,		// 추가길드축제포인트
	GUILDREWARDEFFECT_TYPE_EXTRAWAREGOLD			= 8,		// 사냥종료 후 길드창고에 골드 입금--
	GUILDREWARDEFFECT_TYPE_EXTRAUNIONPOINT			= 9,		// 추가연합포인트
	GUILDREWARDEFFECT_TYPE_MOVESPEED				= 10,		// 안전지역 이동속도증가--
	GUILDREWARDEFFECT_TYPE_VEHICLESPEED				= 11,		// 탈 것 이동속도 증가--
	GUILDREWARDEFFECT_TYPE_REDUCEDURABILITYRATIO	= 12,		// 내구도 소모 비율 감소
	GUILDREWARDEFFECT_TYPE_WAREREPAIRCOST			= 13,		// 길드창고에서 수리비용 사용--
#if defined( PRE_ADD_GUILDREWARDITEM )
	GUILDREWARDEFFECT_TYPE_ADDSKILLTYPE1			= 14,		// 힘, 민첩, 지혜, 건강순이지만 서버에서는 따로 타입 안따고 데이터로 관리
	GUILDREWARDEFFECT_TYPE_ADDSKILLTYPE2			= 15,		
	GUILDREWARDEFFECT_TYPE_ADDSKILLTYPE3			= 16,		
	GUILDREWARDEFFECT_TYPE_ADDSKILLTYPE4			= 17,		
#endif
	GUILDREWARDEFFECT_TYPE_CNT,	
};
enum eGuildRewardUpdateType
{
	GUILDREWARDEFFECT_ADDTYPE_NEW					= 0,		// 생성
	GUILDREWARDEFFECT_ADDTYPE_Add					= 1,		// 추가
	GUILDREWARDEFFECT_ADDTYPE_EXCHANGE				= 2,		// 대체
};
enum eGuildCheckType
{
	GUILD_REWARDITEM_CHECKTYPE_NONE					= 0,		// 조건없음
	GUILD_REWARDITEM_CHECKTYPE_PREVITEM				= 1,		// 이전아이템(길드효과아이템체크)
	GUILD_REWARDITEM_CHECKTYPE_GUILDMARK			= 2,		// 길드마크
};
//date
const int DATELENMAX = 32;


//serverlauncher nickname max
const BYTE SRVLAUNCHERNICKNAMEMAX = 50;

//serverprocess authorize string max for servermonitorcenter connection
const int SRVPROCESSAUTHORSTRMAX = 100;

//announce message max size
const int ANNOUNCEMSG_MAX = 200;

const int MONITORSERVERNODESEND_MAX = 10;

const int MONITOR_PATCHPATH_MAX = 128;


// 퀘스트 보상아이템 최대 갯수
const int QUESTREWARD_INVENTORYITEMMAX = 6;

const int MAX_QUEST_LEVEL_CAP_REWARD = 2;
const int MAX_PERIODQUEST_LIST = 100;
const int MAX_PERIODQUEST_DISTRIBUTION = 10;
const int PERIODQUEST_WAITTIME = 6000;
const int MAX_PERIODQUEST_PROCESSMIN = 2; // 2분
const int MAX_NOTICE_WORLDEVENT = 10;

const int PLAYER_MAX_LEVEL = CHARLEVELMAX;	// CHARLEVELMAX 과 같음... 

#if defined(PRE_ADD_SALE_COUPON)
const int MAX_SALE_USEITEM = 20;
#endif

#if defined(_CLIENT)
#ifdef PRE_MEMORY_SHUFFLE
#if !defined(_FINAL_BUILD)
#define SW_MSDT_ASSERT	// assert() 발생
#define SW_MSDT_NOTINCLUDE_COMPILESAFETY
#define SW_MSDT_ERRORCHECK_ORIGINVALUE
#endif	// _FINAL_BUILD
#include "MemoryShuffleDataType.h"
static MSDT_DECL(float) s_fDefaultFps = 60.f;
#else	// PRE_MEMORY_SHUFFLE
static float s_fDefaultFps = 60.f;
#endif	// PRE_MEMORY_SHUFFLE
#else	// _CLIENT
static float s_fDefaultFps = 60.f;
#endif	// _CLIENT

const int QUESTMEMOMAX = 10;
const int EVENTQUESTMAX = 20;

const int QUESTMEMOREWARDCHECK = QUESTMEMOMAX;		// 퀘스트 보상 무한수령을 방지하기 위해 퀘스트 메모의 특정 인덱스를 할당, 퀘스트 보상 지급횟수 기록용으로 사용

typedef	int		TP_QUESTID;
typedef	char	TP_QUESTMEMO;

typedef	char	TP_JOB;

enum eStoreExternalType
{
	EXTERNALTYPE_MASTER = 1,
	EXTERNALTYPE_DATABASE,
	EXTERNALTYPE_SERVICEMANAGER,
	EXTERNALTYPE_CASH,
};

enum eNation
{
	NATION_KOREA		= 1,
	NATION_KOREATEST,
	NATION_JAPAN		= 11,
	NATION_CHINA		= 21,
	NATION_TAIWAN		= 111,
	NATION_USA			= 41,
	NATION_SINGAPORE	= 51,
	NATION_THAILAND		= 61,
	NATION_INDONESIA	= 71,
	NATION_RUSSIA		= 81,
	NATION_EUROPE		= 91,
};

struct GameTaskType
{
	enum eType
	{
		Normal = 0,
		PvP,
		DarkLair,
		Farm,
		Max,
	};
};

enum eReqInfoType
{
	REQINFO_TYPE_SINGLE = 0,		// 싱글
	REQINFO_TYPE_SINGLE_SAMEDEST,	// 같은 빌리지로 이동시
	REQINFO_TYPE_PARTY,			// 파티
	REQINFO_TYPE_PVP,				// PvP
	REQINFO_TYPE_PVP_BREAKINTO,	// PvP난입
	REQINFO_TYPE_PARTY_BREAKINTO,	//파티초대 나중에 난입도 들어갈라나? 
	REQINFO_TYPE_FARM,				//농장
	REQINFO_TYPE_LADDER,			// 래더	
	REQINFO_TYPE_LADDER_OBSERVER,	// 래더 옵져버(난입)
};

enum eBridgeState {
	BridgeUnknown,
	LoginToVillage,
	VillageToGame,
	VillageToLogin,
	VillageToVillage,
	GameToVillage,
	GameToLogin,
	GameToGame,
	GameToGame_Direct,
	LoginToGame,		
	Exit,
};

enum eCommunityAcceptableOptionType
{
	_CA_OPTIONTYPE_ACCEPT = 0,
	_CA_OPTIONTYPE_DENYALL,
	_CA_OPTIONTYPE_FRIEND_GUILD_ACCEPT,
};

enum eGameOptionAcceptableCheckType
{
	_ACCEPTABLE_CHECKTYPE_PARTYINVITE,
	_ACCEPTABLE_CHECKTYPE_GUILDINVITE,
	_ACCEPTABLE_CHECKTYPE_TRADEREQUEST,
	_ACCEPTABLE_CHECKTYPE_DUELREQUEST,
	_ACCEPTABLE_CHECKTYPE_LADDERINVITE,
	_ACCEPTABEL_CHECKTYPE_QUICKPVPINVITE,
};

#if defined(_KR)
enum eAuthResult
{
	// Initialize = 41
	INIT_OK = 0,				// 정상적으로 도메인 인증 성공
	INIT_DUPLICATE = 1,			// 이미 해당 도메인번호로 인증받은 세션이 존재 (기존 도메인 접속 정보도 끊는다.)
	INIT_ILLEGAL_DOMAIN = 2,	// 인증서버에 등록되지 않은 도메인 번호
	INIT_ERROR = 99,			// 알수없는 오류

	Result_Forbidden = 0,		// 금지된 경로로 접속. 게임서버는 해당사용자를 끊고 Logout패킷을 인증서버에 보낸다.
	Result_Allowed = 1,			// 허용. 게임 가능하고, 인증서버에 더 이상 패킷을 보낼 필요 없다.
	Result_Trial = 2,			// 체험판. 게임 가능 여부는 게임 정책에 따라 판단한다. 게임을 할 수 없이 끊는다면 Logout패킷을 인증서버에 보낸다
	Result_Terminate = 3,		// 종료. 주로 시간제를 사용하다가 시간이 소진될 경우 인증서버에서 통보하기 위해 사용된다. 게임 서버는 해당 사용자에 대한 처리 작업 후 인증서버로 Logout신호를 보낸다.
	Result_Message = 4,			// 게임 중에 인증서버에서 게임서버로 메시지를 보낼 때 사용한다

	AddressDesc_M = 'M',		// 정량PC방 (Prepaid)
	AddressDesc_F = 'F',		// 정액PC방 (Flatcafe)
	AddressDesc_I = 'I',		// 체험판 (Internet)

	ChargeType_R = 'R',			// 과금 (과금을 처리하는 사용자)
	ChargeType_E = 'E',			// 비과금 (과금을 처리하지 않는 사용자)

	Option_NoOption = 0,			// 기본값
	Option_AddressNotAllowed = 1,	// 허용된 ip가 아닙니다.
	Option_AddressMaxConnected = 2,	// ip당 가능한 접속수를 넘었습니다.
	Option_AccountNotAllowed = 4,	// 허용된 id가 아닙니다
	Option_AccountMaxConnected = 5,	// 개인 최대접속 id수를 넘었습니다.
	Option_WelcomeAddress = 11,		// pc방 계약으로 접속하였습니다.
	Option_WelcomeTrial = 13,		// 체험판 접속입니다.
	Option_WelcomePrepaid = 17,		// pc방 정량제 접속입니다. 이때 argument = 남은시간
	Option_PrepaidExhausted = 19,	// pc방 정량제 시간이 소진되었습니다.
	Option_DifferentIpNotAllowed = 27,	// pc방 프리미엄 적용대상이 아닙니다. 넥슨 Pc방 고객센터로 문의바랍니다. (Local Ip와 Server Ip가 다르고 정량제 유저일 경우)
	Option_AccountMachineIDBlocked = 28,	// 불법사용자 머신ID차단
	Option_AccountShutdowned = 31,		// 청소년 선택적 셧다운 타임존에 제한된 접속입니다.
};
#endif	// _KR

enum eError
{
	ERROR_UNKNOWN_HEADER = -3,		// 처리 하지 않는 패킷 수신(이걸로 리턴하면 컨넥션이 끊어집니다 유저인경우)
	ERROR_INVALIDPACKET = -2,		// 비정상적인 패킷(이걸로 리턴하면 컨넥션이 끊어집니다 유저인경우)
	ERROR_DB = -1,					// 일반 디비 에러
	ERROR_NONE = 0,					// ok	
	ERROR_NONE_ROWCOUNT	= 1,		// 1은 비워놓쟈~ (DB쪽 ROWCOUNT체크값)

	//대략 100번까지 서버간의 에러메세지
	ERROR_GENERIC_INVALIDREQUEST = 2,		//기본적인에러임 대부분의 잘못된 요청인경우
	ERROR_GENERIC_UNKNOWNERROR = 3,			//알 수 없는 오류입니다.
	ERROR_GENERIC_DUPLICATEUSER = 4,		//중복 로그인
	ERROR_GENERIC_DUPLICATESESSIONID = 5,	//중복 세션아이디
	ERROR_GENERIC_USER_NOT_FOUND = 6,		//유저가 존재하지 않습니다.
	ERROR_GENERIC_DBCON_NOT_FOUND = 7,		//데이타베이스 연결이 원활하지 않습니다.
	ERROR_GENERIC_VILLAGECON_NOT_FOUND = 8,	//서버 연결이 원활하지 않습니다.
	ERROR_GENERIC_MASTERCON_NOT_FOUND = 9,	// 서버 연결이 원활하지 않습니다.
	ERROR_GENERIC_GAMECON_NOT_FOUND = 10,	//서버 연결이 원활하지 않습니다.
	ERROR_GENERIC_WORLDFULLY_CAPACITY = 11, //월드인원이 가득참
	ERROR_GENERIC_HACKSHIELD = 12,			// 핵쉴드 에러
	ERROR_GENERIC_TIME_WAIT = 13,			// 잠시 후 다시 시도해주세요.
	ERROR_GENERIC_LEVELLIMIT = 14,			// 레벨제한 

	ERROR_DB_SYSTEM = 99,					// 데이터베이스 시스템 에러가 발생하였습니다.

	//UI String Match Enumulation
	//Login Message Starting at 100
	ERROR_LOGIN_FAIL = 100,									// "로그인 에러 입니다"
	ERROR_LOGIN_VERSION_UNMATCH = 101,						// "버전이 맞지 않습니다"
	ERROR_LOGIN_ID_PASS_UNMATCH = 102,						// "아이디, 패스워드가 맞지 않습니다."
	ERROR_LOGIN_SELECT_SERVER = 103,						// "서버를 선택하여 주십시오."
	ERROR_LOGIN_SELECT_CHARACTER = 104,						// "캐릭터를 선택하여 주십시오."
	ERROR_LOGIN_SELECT_CHANNEL = 105,						// "채널을 선택하여 주십시오."
	ERROR_LOGIN_CHARACTERNAME_ALREADY_EXIST = 106,			// "이미 존재 하는 캐릭터 이름입니다."
	ERROR_LOGIN_CHARACTER_DELETED = 107,					// "캐릭터를 삭제하였습니다"
	ERROR_LOGIN_ALREADY_LOGEDIN = 108,						// "중복 로그인 입니다."
	ERROR_LOGIN_CHARACTERLIST_ERROR = 109,					// "캐릭터 리스트를 불러오지 못했습니다."
	ERROR_LOGIN_CANNOT_CREATE_CHARACTER_ANYMORE = 110,		// "더 이상 캐릭터를 생성 할 수 없습니다."
	ERROR_LOGIN_PARTS_NOT_FOUND = 111,						// "캐릭터 파츠가 존재하지 않습니다."
	ERROR_LOGIN_CHARACTER_INDEX_UNMATCH = 112,				// "캐릭터 인덱스가 맞지 않습니다."
	ERROR_LOGIN_CREATE_CHARACTER_FAIL = 113,				// "캐릭터 생성에 실패 하였습니다."
	ERROR_LOGIN_PROHIBITWORD = 114,							// 만들 수 없는 이름입니다. (금칙어)
	ERROR_LOGIN_FCM_PROTECTED = 115,						// 중국 피로도 경고문 (입력미완료)
	ERROR_LOGIN_FCM_LIMITED = 116,							// 중국 피로도 경고문 (미성년)
	ERROR_117 = 117,										// 접속시간이 아닙니다.
	ERROR_LOGIN_ERASE_CHARACTER_FAIL_GUILDMASTER = 118,		// 현재 길드 마스터이므로 캐릭터를 삭제할 수 없습니다.
	ERROR_LOGIN_CANNOT_CREATE_CHARACTER_TODAY = 119,		// "오늘은 더 이상 캐릭터를 생성 할 수 없습니다."
	ERROR_LOGIN_HAIRCOLOR_NOT_FOUND = 120,
	ERROR_LOGIN_SKINCOLOR_NOT_FOUND = 121,
	ERROR_LOGIN_EYECOLOR_NOT_FOUND = 122,
	ERROR_LOGIN_SDOUA_NEED_INFO = 123,						// 중국 상세정보 입력 필요
	ERROR_LOGIN_AUTHCHECK_INVALID_ARGUMENT = 124,			//	1: Argument is not valid
	ERROR_LOGIN_AUTHCHECK_INVALID_PASSPORT = 125,			//	2: NexonPassport is not valid
	//ERROR_LOGIN_AUTHCHECK_ERROR_TIMEOUT = 126,			//	3: [obsolete] same as AUTHCHECK_ERROR_EXPIRED
	ERROR_LOGIN_AUTHCHECK_EXPIRED = 126,					//	3: NexonPassport is valid, but arleady expired
	ERROR_LOGIN_AUTHCHECK_AUTH_SERVER_SHUTDOWN = 127,		//	4: Authentication server is shutdown
	ERROR_LOGIN_AUTHCHECK_UNSUPPORTED_LOCALE = 128,			//	5: Unsupported Locale ID
	ERROR_LOGIN_AUTHCHECK_SET_LOCALE_NEEDED = 129,			//	6: call AuthCheck_SetLocale first
	ERROR_LOGIN_AUTHCHECK_NO_SERVER_INFORMATION = 130,		//	7: no server information in config file
	ERROR_LOGIN_AUTHCHECK_USER_IP_MISMATCHED = 131,			//	8: client ip address is mismatched with sso servers data
	ERROR_LOGIN_AUTHCHECK_NO_DECRYPTION_KEY = 132,			//	9: There is no passport decryption key. Check AuthCheck_LoadKey function or key file.
	ERROR_LOGIN_AUTHCHECK_LOW_LEVEL_PASSPORT = 133,			//	10: passport level is lower than minimum passport level
	ERROR_LOGIN_AUTHCHECK_GAMECODE_MISMATCHED = 134,		//	11: Mismatch between the caller's gamecode and the passport's gamecode.
	ERROR_LOGIN_AUTHCHECK_SESSION_NOT_EXIST = 135,			//	12: User session data not exists. ( Maybe timeout or something. )
	ERROR_LOGIN_AUTHCHECK_DISCONNECTED = 136,				//	13: User IP or session key missmatched with current login session. Maybe logged out by another session.
	ERROR_LOGIN_AUTHCHECK_SERVER_FAILED = 137,				//	100: Internal error.
	ERROR_LOGIN_CANNOT_CREATE_DWCCHARACTER_EXIST = 138,		// DWC 캐릭터를 이미 가지고 있다.
	ERROR_LOGIN_CANNOT_FIND_DWCCHARACTER = 139,				// DWC 캐릭터를 만들었지만 캐릭터 목록에서 찾을 수 없다.
	ERROR_LOGIN_DWC_WRONG_DATE = 140,						// DWC 기간이 아니다.
	ERROR_LOGIN_CANNOT_CREATE_DWCCHARACTER_LIMITLEVEL = 141,// DWC 캐릭터를 만들기 위해 필요한 캐릭터 레벨이 부족하다.

	//Character Message Starting at 200
	ERROR_CHARACTER_QUICKSLOT_NOTFOUND = 200,				// "퀵슬롯이 존재하지 않습니다."
	ERROR_FCMSTATE = 201,									// 피로시간이므로 안됨(?)
	ERROR_CHARACTER_DATA_NOT_LOADED_SELF = 202,				// 자신의 캐릭터 데이터가 아직 로딩 중입니다.
	ERROR_CHARACTER_DATA_NOT_LOADED_OPPOSITE = 203,			// 상대방 캐릭터 데이터가 아직 로딩 중입니다.

	//Party Message Starting at 300
	ERROR_PARTY_CREATE_FAIL = 300,							//파티생성실패
	ERROR_PARTY_DELETE_FAIL = 301,							//파티 삭제가 실패하였습니다.
	ERROR_PARTY_JOINFAIL = 302,								//파티에 참가 할 수 없습니다.
	ERROR_PARTY_JOINFAIL_ZONLEVEL_SHORTAGE = 303,			//월드존레벨 미달
	ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_SHORTAGE = 304,		//캐릭터레벨미달
	ERROR_PARTY_JOINFAIL_PASSWORDINCORRECT = 305,			//비밀번호 맞지 않음
	ERROR_PARTY_LEAVEFAIL = 306,							//파티나가기 실패
	ERROR_PARTY_STARTSTAGE_FAIL = 307,						//게임을 시작 할 수 없습니다.	
	ERROR_PARTY_INVITEFAIL = 308,							//초대에 실패하였습니다.
	ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY = 309,			//대상이 파티에 속해 있어 초대할 수 없습니다.
	ERROR_PARTY_INVITEFAIL_ZONLEVEL_SHORTAGE = 310,			//존레벨이 낮아 초대할 수 없습니다.
	ERROR_PARTY_INVITEFAIL_CHARACTERLEVEL_SHORTAGE = 311,	//캐릭터레벨이 낮아 초대 할 수 없습니다.
	ERROR_PARTY_INVITEFAIL_ALREADY_INVITED = 312,			//이미 초대된 대상입니다.
	ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND = 313,			//대상이 존재하지 않습니다.
	ERROR_PARTY_KICKFAIL = 314,								//강퇴실패
	ERROR_PARTY_LEADER_APPOINTMENTFAIL = 315,				//파티장임명실패
	ERROR_PARTY_MOVECHANNELFAIL = 316,						//채널이동실패
	ERROR_PARTY_JOINFAIL_INREADYSTATE = 317,				//레디상태에서 파티참가 할 수 없음
	ERROR_PARTY_KICKOK = 318,								// 강퇴성공(?)
	ERROR_PARTY_JOINFAIL_PARTYMEMBER_ALREADY_MAX = 319,		//파티인원이 가득차서 파티에 참가 할 수 없습니다.
	ERROR_PARTY_JOINFAIL_YOU_ARE_KICKEDUSER = 320,			//추방당한 파티에 재가입할 수 없습니다.
	ERROR_PARTY_QUICKJOINFAIL_THERE_IS_NOMATCH_PARTY = 321,	//가입할만한 파티가 없숨다~
	ERROR_PARTY_CREATE_FAIL_NEED_PARTYNAME = 322,			//파티이름이 있어야 만들꺼 아니냐
	ERROR_PARTY_CREATE_FAIL_PASSWORD_ERR = 323,				//패스워드제한이라는 것도 있는 게임이야 잘넣어
	ERROR_PARTY_CREATE_FAIL_RESTRICT_LEVEL_ERROR = 324,		//제한레벨잘넣어
	ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER = 325,			//캐릭터레벨제한
	ERROR_PARTY_KICKFAIL_DEST_IS_YOU = 326,					//나자신을 킥할 수 없음
	ERROR_PARTY_MODIFYINFO_FAIL = 327,						//정보변경실패
	ERROR_PARTY_MODIFYINFO_FAIL_NEED_NAME = 328,			//파티이름이 있어야 변경가능
	ERROR_PARTY_MODIFYINFO_FAIL_PASSWORD_ERR = 329,			//패스워드잘넣어
	ERROR_PARTY_MODIFYINFO_FAIL_RESTRICT_LEVEL_ERROR = 330, //제한사항잘넣어
	ERROR_PARTY_LIST_ERROR = 331,							//차티리스트에러
	ERROR_PARTY_STARTSTAGE_FAIL_ACTLEVEL_SHORTAGE = 332,	//월드존레벨이 낮아 던전에 입장할 수 없습니다.
	ERROR_PARTY_STARTSTAGE_FAIL_FATIGUE_SHORTAGE = 333,		//피로도가 부족하여 던전에 입장하실 수 없습니다.
	ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED = 334,	//대상이 초대받을 수 있는 상황이 아님
	ERROR_PARTY_STARTSTAGE_FAIL_DUNGEONNEEDITEM_SHORTAGE = 335,	//던전입장 아이템부족
	ERROR_PARTY_DUNGEONWARPOUT_FAIL_ITEMSHARING = 336,			//던전 클리어 후 워프 실패 : 파티 아이템 분배중
	ERROR_PARTY_DUNGEONWARPOUT_FAIL_QUEST = 337,				//던전 클리어 후 워프 실패 : 파티원 중 퀘스트 미완료
	ERROR_PARTY_NOTFOUND_MEMBERINFO = 338,						//파티정보가 없습니다.
	ERROR_PARTY_CONDITION_MATCHED_LIST_NOT_FOUND = 339,			//정렬조건에 맞는 파티가 없습니다.
	ERROR_PARTY_INVITEFAIL_DIFFCHANNEL = 340,					//채널이틀릴경우 파티초대불가능.
	ERROR_PARTY_INVITEFAIL_DEST_USER_NOWREADY = 341,			//레디상태인 캐릭터는 초대할 수 없습니다.
	ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME = 342,			//초대의 대상이 다른 마을에 있음
	ERROR_PARTY_INVITEFAIL_DESTINVITE_DENIED = 343,				//대상이 초대를 거부했습니다.
	ERROR_PARTY_INVITEFAIL_ALREADYINVITED = 344,				//초대중입니다. 재초대불가능
	ERROR_PARTY_INVITEAGREEFAIL = 345,							//초대에 응했는데 파티참가에 실패함
	ERROR_PARTY_ASKJOIN_FAIL = 346,								//참가신청의 일반적인 실패
	ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND = 347,					//신청자가 파티에 참가해있지 않음
	ERROR_PARTY_ASKJOIN_DEST_DIFFMAP = 348,						//대상이 다른마을에 있습니다.
	ERROR_PARTY_ASKJOIN_RESULT_AGREE_FAIL = 349,				//파티신청에대한 수락실패
	ERROR_PARTY_ASKJOIN_RESULT_PARTYLEADER_DENIED = 350,		//파티신청 거부
	ERROR_PARTY_SWAPINDEX_FAIL = 351,							//파티위치이동 실패
	ERROR_PARTY_REFRESHGATEFAIL_REQRETREAT = 352,				//월드존 파티초대중 던전게이트요청시 뒤로 물러나기 요청
	ERROR_PARTY_CANT_ENTERGATE = 353,							//게이트에 입장에 문제가 생겨 워프시킵니다.
	ERROR_PARTY_ASKJOIN_DESTUSER_SITUATION_NOTALLOWED = 354,	//대상이 현재 파티 신청을 받을 수 없습니다.
	ERROR_PARTY_PARTYLIST_OVERFLOW = 355,						// 요청받은 페이지가 없습니다.
	ERROR_PARTY_GM_JOINFAIL_LADDERROOM = 356,					// 래더방에 있는 유저에 추적할 수 없습니다. 래더 관전을 이용해주세요. 
	ERROR_PARTY_REFRESHLIST_TIMERESTRAINT = 357,				// 파티리스트 요청 시간 제한
	ERROR_PARTY_STARTSTAGE_FAIL_EXCEED_TRY_COUNT	= 358,		// 입장 횟수 초과
	ERROR_PARTY_STARTSTAGE_FAIL_NOT_AVAILABLE_TIME	= 359,		// 입장 제한 시간


	//Item&shop Message Starting at 400
	ERROR_ITEM_FAIL = 400,							//아이템에러
	ERROR_ITEM_NOTFOUND = 401,						//존재하지 않는 아이템
	ERROR_ITEM_NOTFOUND1 = 1688801,
	ERROR_ITEM_INDEX_UNMATCH = 402,					//인덱스가 맞지 않다
	ERROR_ITEM_EQUIPMISMATCH = 403,					//장착위치가 맞지 않습니다.
	ERROR_ITEM_OVERFLOW = 404,						//아이템 갯수 넘어 갔습니다.
	ERROR_ITEM_INVENTORY_NOTENOUGH = 405,			//인벤토리공간이 부족합니다.
	ERROR_ITEM_UNCOUNTABLE = 406,					//겹쳐지는 아이템이 아닙니다.
	ERROR_ITEM_ALREADY_EXIST = 407,					//같은 인덱스의 아이템 존재함
	ERROR_ITEM_INSUFFICIENCY_MONEY = 408,			//돈부족
	ERROR_ITEM_SOULBOUND = 409,						//귀속아이템입니다.
	ERROR_ITEM_EMBLEM_COMPOUND_FAIL = 410,			//문장 조합 실패
	ERROR_ITEM_ITEM_COMPOUND_FAIL = 411,			//아이템 조합 실패
	ERROR_ITEM_REPAIR = 412,						// 수리할께 하나도 없다
	ERROR_ITEM_ENCHANTFAIL = 413,					// (해당 아이템은 강화할 수 없습니다)
	ERROR_ITEM_INSUFFICIENCYCOUNT = 414,			// 아이템 개수가 모자라다
	ERROR_ITEM_INVENTOWARE01 = 415,					// 자신이 가지고 있는 액수보다 초과함
	ERROR_ITEM_INVENTOWARE02 = 416,					// 창고의 보관가능 액수 초과
	ERROR_ITEM_WARETOINVEN01 = 417,					// 창고 액수보다 초과해서 꺼내려고함
	ERROR_ITEM_WARETOINVEN02 = 418,					// 소지 가능 금액을 초과해서 꺼낼 수 없음
	ERROR_ITEM_CAN_NOT_DISJOINT = 419,				// 분해할 수 없는 아이템임!
	ERROR_ITEM_OVERFLOWMONEY = 420,					// 돈초과
	ERROR_ITEM_REBIRTH_CASHCOIN_SHORT_FAIL = 421,	// 캐쉬코인이 부족하여 부활시킬 수 없음
	ERROR_ITEM_EMBLEM_COMPOUND_POSSIBILITY_FAIL = 422,	// 문장보옥 조합 시도 확률로 실패됨.
	ERROR_ITEM_EMBLEM_COMPOUND_NOT_ENOUGH_MONEY = 423,	// 문장보옥 조합에서 돈이 모자람.
	ERROR_ITEM_EMBLEM_COMPOUND_NOT_ENOUGH_ITEM = 424,	// 문장보옥 조합에서 재료 아이템 부족
	ERROR_ITEM_EMBLEM_COMPOUND_NOT_MATCH_SLOT = 425,	// 문장보옥 조합에서 슬롯이 맞지 않음.
	ERROR_ITEM_EMBLEM_COMPOUND_NOT_ENOUGH_ITEM_COUNT = 426, // 문장보옥 조합에서 슬롯은 맞으나 아이템 갯수가 부족함.
	ERROR_ITEM_EMBLEM_COMPOUND_NOT_MATCH_PLATE_WITH_ITEM_COMPOUND_INDEX = 427,	// 문장보옥 조합에서 플레이트가 수행하는 아이템 조합 리스트에 입력받은 조합 인덱스 없음.
	ERROR_ITEM_ITEM_COMPOUND_POSSIBILITY_FAIL = 428,	// 아이템 조합 시도 확률로 실패됨.
	ERROR_ITEM_ITEM_COMPOUND_NOT_ENOUGH_MONEY = 429,	// 아이템 조합에서 돈이 모자람.
	ERROR_ITEM_ITEM_COMPOUND_NOT_ENOUGH_ITEM = 430,	// 아이템 조합에서 재료 아이템 부족
	ERROR_ITEM_ITEM_COMPOUND_NOT_MATCH_SLOT = 431,	// 아이템 조합에서 슬롯이 맞지 않음.
	ERROR_ITEM_ITEM_COMPOUND_NOT_ENOUGH_ITEM_COUNT = 432, // 문장보옥 조합에서 슬롯은 맞으나 아이템 갯수가 부족함.
	ERROR_ITEM_ENCHANT_POSSIBILITYFAIL_BREAKITEM = 433,		// 강화확률실패 아이템파괴
	ERROR_ITEM_REMOVE_DISALLOWANCE = 434,					// 파괴불가
	ERROR_ITEM_ENCHANT_POSSIBILITYFAIL_DOWNLEVEL = 435,		// 강화확률실패 강화레벨 다운
	ERROR_ITEM_NEED_PARTY_SHARING_SOULBOUND = 436,			// 귀속 아이템으로 파티 분배가 필요
	ERROR_ITEM_INSUFFICIENCY_ITEM = 437,					// 아이템부족
	ERROR_ITEM_PVPLEVEL = 438,								// PvP 레벨 부족해서 장착할 수 없음
	ERROR_ITEM_UNABLE_EQUIP_NO_SOUL_BOUND = 439,			// 개봉되지 않아서 장착할 수 없음
	ERROR_ITEM_UNSEAL_ALREADY = 440,						// 이미 개봉된 아이템
	ERROR_ITEM_ENCHANT_POSSIBILITYFAIL_DOWNLEVELZERO = 441,	// 강화확률실패 강화레벨 다운량이 0
	ERROR_ITEM_REBIRTHCOIN_50 = 442,						// 부활서 50개까지 소유
	ERROR_ITEM_DONT_HAVE_SKILL_TO_RESET = 443,				// 초기화할 스킬이 없음.
	ERROR_ITEM_ALREADY_HAVE_HIGHER_JOB_SKILL = 444,			// 초기화 하려고 하는 스킬의 직업보다 상위 차수의 직업의 스킬을 이미 찍었음.
	ERROR_ITEM_MISMATCH_RESET_SKILL_JOB = 445,				// 스킬리셋 캐쉬템에 지정된 초기화 하려고 하는 스킬의 직업 차수와 현재 캐릭터의 직업과 맞지 않음.
	ERROR_ITEM_DONTMOVE = 446,								// 이동할 수 없습니다(?)
	ERROR_ITEM_SEAL_ALREADY = 447,							// 이미 봉인된 아이템
	ERROR_ITEM_SEALCOUNT_ZERO = 448,						// 재봉인 횟수가 0이다.
	ERROR_ITEM_COSMIX_FAIL = 449,							// 코스튬 합성 실패(일반)
	ERROR_ITEM_COSMIX_FAIL_STUFF = 450,						// 코스튬 합성 실패(재료 아이템 이상)
	ERROR_ITEM_COSMIX_STUFF_COUNT = 451,					// 코스튬 합성 재료 개수 부족 혹은 초과
	ERROR_ITEM_COSMIX_OPEN_FAIL = 452,						// 코스튬 합성창 열기 실패
	ERROR_ITEM_COSMIX_CLOSE_FAIL = 453,						// 코스튬 합성창 닫기 실패
	ERROR_ITEM_COSMIX_NO_RESULT = 454,						// 코스튬 합성 실패(결과물 정보 이상)
	ERROR_ITEM_COSMIX_OPTION = 455,							// 코스튬 합성 실패(옵션 정보 이상)
	ERROR_GLYPH_SLOTLIMITLEVEL = 456,						// 캐릭레벨이 낮아 슬롯에 넣을 수 없다
	ERROR_GLYPH_LIMITLEVEL = 457,							// 캐릭레벨이 낮아 문장을 장착(?)할 수 없다
	ERROR_ITEM_POTENTIAL_INVALIDTYPE = 458,					// 잠제력 적용 불가능한 타입입니다.
	ERROR_GLYPH_SAMETYPEPARAM = 459,						// 동일계열문장
	ERROR_ITEM_NEEDKEY = 460,								// 열쇠 필요하다
	ERROR_ITEM_APPELLATIONGAIN_EXIST = 461,					// 이미 존재하는 칭호입니다.
	ERROR_ITEM_EQUIP_CRC_FAIL = 462,						// 아이템 데이터 오류로 인한 장착 실패
	ERROR_ITEM_CANNOT_USE_INPARTY = 463,					// 파티 상태에서는 사용이 불가능한 아이템입니다.
	ERROR_ITEM_CANNOT_CHANGE_SAMENAME = 464,				// 기존 캐릭터명과 동일한 이름으로 변경 할 수 없습니다.
	ERROR_ITEM_ENCHANTJEWEL_INVALIDTYPE = 465,				// 인챈트 직행아이템 적용이 불가능한 타입입니다.
	ERROR_ITEM_INSUFFICIENCY_LADDERPOINT = 466,				// 래더포인트부족
	ERROR_ITEM_INSUFFICIENCY_UNIONPOINT = 467,				// 연합포인트부족
	ERROR_ITEM_SEAL_FAIL_BY_RANDOM = 468,					// 랜덤돌려서 봉인 실패.
	ERROR_ITEM_CANNOT_CHANGE_GUILDMARK = 469,				// 길드마크 변경 실패
	ERROR_ITEM_INSUFFICIENCY_GUILDWARPOINT = 470,			// 길드축제포인트부족
	ERROR_ITEM_INSUFFICIENCY_ITEMTYPE_REMOTEENCHANT = 471,	// 원격강화아이템 없음
	ERROR_ITEM_DESIGNMIX_FAIL = 472,						// 코스튬 디자인 합성 실패(일반)
	ERROR_ITEM_DESIGNMIX_FAIL_STUFF = 473,					// 코스튬 디자인 합성 실패(재료 아이템 이상)
	ERROR_ITEM_DESIGNMIX_STUFF_COUNT = 474,					// 코스튬 디자인 합성 재료 개수 부족 혹은 초과
	ERROR_ITEM_DESIGNMIX_OPEN_FAIL = 475,					// 코스튬 디자인 합성창 열기 실패
	ERROR_ITEM_DESIGNMIX_CLOSE_FAIL = 476,					// 코스튬 디자인 합성창 닫기 실패
	ERROR_ITEM_DESIGNMIX_NO_RESULT = 477,					// 코스튬 디자인 합성 실패(결과물 정보 이상)
	ERROR_ITEM_CHANGEJOB_ILLEGAL_STATE = 478,				// 직업 변경 아이템을 사용할 수 있는 상태가 아닐 경우	
	ERROR_ITEM_INSUFFICIENTY_JOBID = 479,					// 직업제한
	ERROR_ITEM_INSUFFICIENTY_LEVEL = 480,					// 레벨제한
	ERROR_ITEM_INSUFFICIENTY_PVPRANK = 481,					// PvP랭크제한
	ERROR_ITEM_INSUFFICIENTY_GUILDLEVEL = 482,				// 길드레벨제한
	ERROR_ITEM_INSUFFICIENTY_LADDERGRADEPOINT = 483,		// 래더평점제한
	ERROR_ITEM_BUY_OVERFLOW_COUNT = 484,					// 구입 가능 수량 초과.
	ERROR_ITEM_CANNOT_CHANGE_SAME_PET_NAME = 485,			// 기존 캐릭터명과 동일한 이름으로 변경 할 수 없습니다.
	ERROR_ITEM_ALREADY_EXPIRED = 486,						// 이미 파기 되었습니다.
	ERROR_ITEM_PREITEM_REQUEST = 487,						// 이전 아이템이 필요합니다.
	ERROR_ITEM_BETTEREFFECT_ITEM = 488,						// 더 좋은 효과가 적용 중입니다.
	ERROR_ITEM_GUILDMARK_REQUIRE	= 489,					// 길드마크가 필요합니다.			
	ERROR_ITEM_GUILDMARK_EXIST	= 490,						// 이미 길드마크 존재합니다.	
	ERROR_ITEM_RANDOMMIX_FAIL = 491,						// 코스튬 랜덤 합성 실패(일반)
	ERROR_ITEM_RANDOMMIX_FAIL_STUFF = 492,					// 코스튬 랜덤 합성 실패(재로 아이템 이상)
	ERROR_ITEM_RANDOMMIX_STUFF_COUNT = 493,					// 코스튬 랜덤 합성 재료 개수 부족 혹은 초과
	ERROR_ITEM_RANDOMMIX_OPEN_FAIL = 494,					// 코스튬 랜덤 합성창 열기 실패
	ERROR_ITEM_RANDOMMIX_CLOSE_FAIL = 495,					// 코스튬 랜덤 합성창 닫기 실패
	ERROR_ITEM_ALREADY_GUILDITEM = 496,						// 이미 가지고 있는 아이템입니다.
	ERROR_ITEM_RANDOMMIX_NORANKITEM_FAIL = 497,				// 코스튬 랜덤 합성 랭크에 맞는 아이템이 없음
	ERROR_ITEM_OVERFLOW_CHARMCOIN = 498,					// 골드량을 줄이고 다시 시도해 주세요.
	ERROR_ITEM_INSUFFICIENTY_PERIOD_RECOVERYCASH = 499,		// 기간이 지나서 복구할 수 없습니다.

	//Trade & Mail & Market Message Starting at 500
	ERROR_SHOP_NOTFOUND = 500,						//상점이 존재하지 않습니다.
	ERROR_SHOP_SLOTITEM_NOTFOUND = 501,				//상점의 물품이 존재하지 않습니다.
	ERROR_SHOP_DATANOTFOUND = 502,					//샵데이타가 없어요
	ERROR_MAIL_INSUFFICIENCY_MONEY = 503,			//소지금액부족
	ERROR_MAIL_ATTACHITEMFAIL = 504,				//아이템 첨부 실패
	ERROR_MAIL_CHARACTERNOTFOUND = 505,				//받을 사람이 없어요
	ERROR_MAIL_ATTACHEDTHING_EXIST = 506,			//첨부되어 있는 것이 있어요
	ERROR_MAIL_DOESNOTEXIST_ATTACHED_THING = 507,	//첨부되어 있는 것인 없습니다.
	ERROR_MARKET_COUNTOVER = 508,					//마켓에 더이상 등록이 안되요
	ERROR_MARKET_DOESNOTEXIST_REGISTITEM = 509,		//등록된 마켓아이템이 없습니다.
	ERROR_MARKET_SOLDITEM = 510,					//이미팔린아이템
	ERROR_MARKET_ISNOT_SOLDITEM = 511,				//팔린아이템이 아닙니다.
	ERROR_EXCHANGE_USERNOTFOUND = 512,				//거래할대상이 없음
	ERROR_EXCHANGE_FAIL = 513,						//거래실패
	ERROR_MAIL_DONTSENDYOURSELF = 514,				// 자신에게 보낼 수 없습니다.
	ERROR_SHOP_NOTFORSALE = 515,					// 파는 아이템이 아닙니다
	ERROR_MARKET_REGISTITEMFAIL = 516,				// 등록할 아이템이 아닙니다
	ERROR_EXCHANGE_ITEMFAIL = 517,					// 교환할 아이템이 아닙니다
	ERROR_EXCHANGE_SENDERCANCEL = 518,				// 거래요청 보낸사람이 다시 취소할때
	ERROR_EXCHANGE_OVERFLOWMONEY = 519,				// 소지 가능금액 넘어서 거래 실패
	ERROR_SHOP_OVERFLOWMONEY = 520,					// 소지 가능금액 넘어서 판매 실패
	ERROR_MAIL_ATTACHITEM_OVERFLOW = 521,			// 소지품 창에 빈 칸이 없습니다.
	ERROR_MAIL_ATTACHMONEY_OVERFLOW = 522,			// 소지할 수 있는 돈을 초과하여 꺼낼 수 없습니다.
	ERROR_MAIL_CANT_RECVMAIL	= 523,				// 대상이 우편을 받을 수 없습니다.
	ERROR_MAIL_CANT_RECVCASHPRESENT = 524,			// 대상이 선물을 받을 수 없습니다.
	ERROR_EXCHANGE_CANTEXCHANGE = 525,				// 대상이 거래를 요청받을 수 없습니다.
	ERROR_ITEM_INVENTOEQUIP_FAIL = 526,				// 지금은 아이템을 장비할 수 없습니다
	ERROR_ITEM_EQUIPTOINVEN_FAIL = 527,				// 지금은 장비를 해제할 수 없습니다
	ERROR_CASHSHOP_RESERVE_NOTENOUGH = 528,			// 적립금 부족합니다.
	ERROR_CASHSHOP_RESERVE_DISABLE = 529,			// 적립금으로 살 수 없는 아이템입니다.
	ERROR_CASHSHOP_OPEN = 530,						// 캐쉬샵이 열려있어 나갈 수 없습니다?
	ERROR_CASHSHOP_COUNTOVER = 531,					// 더이상 살 수 없다
	ERROR_CASHSHOP_CASH_NOTENOUGH = 532,			// 캐쉬가 부족합니다.
	ERROR_CASHSHOP_CALLGM = 533,					// 구입한 상품을 받는 데 실패했습니다. 고객센터로 문의 부탁드립니다.
	ERROR_CASHSHOP_DONTSENDYOURSELF = 534,			// 자기자신한테 선물을 보낼 수 없다.
	ERROR_GIFT_COUNTOVER_INVEN = 535,				// (인벤)더이상 받을 수 없다
	ERROR_GIFT_COUNTOVER_WARE = 536,				// (창고)더이상 받을 수 없다
	ERROR_GIFT_COUNTOVER_REBIRTHCOIN = 537,			// (부활서)더이상 받을 수 없다
	ERROR_GIFT_NOTFORSALE = 538,					// 선물할 수 없는 아이템이다
	ERROR_GACHAPON_JP_MISMATCH_JOBCLASS = 539,		// 일본 가챠폰.	결과로 나온 아이템이 요청한 직업과 같지 않음. 패킷 핵일 가능성이 높음.
	ERROR_GACHAPON_JP_NOT_ENOUGH_COIN = 540,		// 일본 가챠폰. 코인이 모자라서 실패임.
	ERROR_GACHAPON_JP_INVALID_SHOP_ID = 541,		// 일본 가챠폰. 잘못된 가챠폰 인덱스
	ERROR_CASHSHOP_PROHIBITSALE = 542,				// 판매중지된 상품입니다
	ERROR_GACHAPON_JP_INVALID_RESULT_ITEM = 543,	// 일본 가챠폰. 결과로 나온 아이템 정보를 서버에서 찾을 수 없음.
	ERROR_VIP_FAIL = 544,							// VIP만 살수있다?
	ERROR_MARKET_INVALIDPERIOD = 545,				// 사용가능한 판매기간이 아닙니다.
	ERROR_MARKET_CANNOTUSE_PREMIUM = 546,			// 프리미엄 서비스를 사용할 수 없습니다.
	ERROR_CASHSHOP_DUPLICATE_GESTURE = 547,			// 이미 가지고 있는 제스쳐는 구매할수 없습니다.
	ERROR_CASHSHOP_USE_AGREEMENT = 548,				// 캐시샵 이용동의를 하신후 이용하실수 있습니다
	ERROR_MARKET_REGISTPREMIUM = 549,				// 거래소 이용권 소지자에 한해 등록가능
	ERROR_MARKET_REGISTVIP = 550,					// VIP만 등록가능
	// GuildWare
	ERROR_ITEM_NOT_MOVE_TO_GUILDWARE = 551,			// 아이템을 길드창고에 옮길 수 없습니다.
	ERROR_ITEM_NOT_TAKE_FROM_GUILDWARE = 552,		// 길드창에서 아이템을 가져올 수 없습니다.
	ERROR_GUILDWARE_NOT_READY = 553,				// 길드창고 준비중입니다.
	ERROR_MAX_WITHDRAW_GUILDCOIN = 554,				// 하루동안 인출할 수 있는 최대 금액을 초과하였습니다.
	ERROR_MAX_TAKE_GUILDITEM = 555,					// 하루동안 꺼낼 수 있는 최대 아이템 횟수를 초과하였습니니다.
	ERROR_FULL_GUILDWARE = 556,						// 길드창고가 가득차 아이템을 넣을 수 없습니다
	ERROR_CASHSHOP_NXACREDITGIFT = 557,				// NX Credit으로는 선물할 수 없습니다
	ERROR_CASHSHOP_NXACREDIT = 558,					// 
	ERROR_ALREADY_MAXGUILDWARE = 559,				// 더 이상 길드창고를 확장할 수 없습니다.
	// Cash
	ERROR_CASHSHOP_GIFTCOUNTOVER = 560,				// "해당 상품은 중복해서 보유할 수 없으므로, 선물을 받을 수 없습니다."
	ERROR_ITEM_NOT_MOVE_TO_WARE = 561,				// 창고에 넣을 수 없는 아이템 입니다.
	
	
	//Quest Message Starting at 600
	ERROR_QUEST_CANCELFAIL = 600,					//퀘스트 삭제 실패
	ERROR_QUEST_SETACTIVEFAIL = 601,				//퀘스트 설정실패
	ERROR_QUEST_NOTFOUND = 602,						//퀘스트 없음
	ERROR_QUEST_REWARD_FAIL = 603,					//퀘스트 보상요청 실패
	ERROR_QUEST_FCM = 604,							// 피로시간 이후 퀘스트 수락/완료/보상이 제한됩니다. 휴식해라...-_;
	ERROR_QUEST_INVENTORY_NOTENOUGH = 605,			//퀘스트 인벤토리 공간이 부족합니다.
	ERROR_QUEST_INVENTORY_ALMOSTFULL = 606,			//퀘스트 인벤토리 공간이 거의 다 찼습니다.
	ERROR_QUEST_FULLOFCOUNT_PLAYING = 607,			//퀘스트 보유량을 초과하여 더 이상 퀘스트를 받을 수 없습니다.
	ERROR_QUEST_ALREADY_PLAYING = 608,				//이미 진행중인 퀘스트
	ERROR_QUEST_NEST_HELPER = 609,					//네스트 도우미 중에는 퀘스트 수락/완료/보상이 제한됩니다.
	ERROR_QUEST_CANT_ADD_QUEST_FROM_ITEM = 610,		//해당아이템으로 받을 수 있는 퀘스트는 이미 완료되었습니다.


	//Chat Message Starting at 700
	ERROR_CHAT = 700,
	ERROR_CHAT_USERNOTFOUND = 701,
	ERROR_CHAT_DONTSAYYOURSELF = 702,
	ERROR_CHAT_MOBILESENDED = 703,

	//ChatRoom Starting at 750
	ERROR_CHATROOM = 750,
	ERROR_CHATROOM_UNKNOWNUSER = 751,				// 방에 없는 유저이다.
	ERROR_CHATROOM_NOMORESPACE = 752,				// 방이 다 찼음
	ERROR_CHATROOM_ALREADYINROOM = 753,				// 이미 방에 참여중이다.
	ERROR_CHATROOM_CANNOTENTER = 754,				// 방에 입장 할수 없습니다.
	ERROR_CHATROOM_WRONGPASSWORD = 755,				// 패스워드가 틀립니다.
	ERROR_CHATROOM_FAILEDMAKEROOM = 756,			// 방 생성 실패
	ERROR_CHATROOM_INVALIDCHATROOM = 757,			// 방 오류! - 그런 방은 없습니다.
	ERROR_CHATROOM_ALREADYLEADER = 758,				// 이미 방장입니다.
	ERROR_CHATROOM_ANOTHERROOM = 759,				// 다른 채팅방에 참여중입니다.
	ERROR_CHATROOM_CANNOTLEAVELEADER = 760,			// 방장은 방에 인원이 남은경우 나갈수 없습니다.
	ERROR_CHATROOM_ONLYLEADER = 761,				// 방장만 할수 있는 기능입니다.
	ERROR_CHATROOM_KICKEDFOREVER = 762,				// 방에서 영구추방되어 들어갈 수 없습니다.

	//Skill Message Starting at 800
	ERROR_SKILL_ACQUIREFAIL = 800,								//스킥획득실패
	ERROR_SKILL_DELETEFAIL = 801,								//스킬삭제실패
	ERROR_SKILL_ISNOT_YOURS = 802,								//스킬을소유하고있지 않습니다.
	ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS = 803,					//스킬을소유하고있지 않아 스킬업실패
	ERROR_SKILL_LEVELUPFAIL_INSUFFICIENCY_SKILLPOINT = 804,		//스킬포인트부족
	ERROR_SKILL_LEVELUPFAIL_INSUFFICIENCY_CHARACTERLEVEL = 805,	//캐릭터레벨부족
	ERROR_SKILL_LEVELUPFAIL_ALREADY_MAX = 806,					//이미 맥스레벨입니다.
	ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT = 807,			// 유효하지 않은 스킬 포인트니다..(실제 사용 스킬포인트보다 보유 포인트보다 적거나 테이블의 스킬 레벨업시에 필요한 스킬 포인트 데이터와 다름)
	ERROR_SKILL_LEVELUPFAIL_EQUIPITEMSKILL = 808,				// 장비아이템 스킬은 레벨업 할 수 없습니다.
	ERROR_SKILL_LEVELUPFAIL_TOGGLE_AURA_ON = 809,				// 토글되었거나 오라가 발동중인 스킬은 레벨업 불가능.
	ERROR_SKILL_UNLOCK_ALREADY_OPEN = 810,						// 이미 언락된 스킬임.
	ERROR_SKILL_UNLOCK_FAIL = 811,		// 언락 실패... 작업중임. 추후에 세분화.
	ERROR_SKILL_UNLOCK_MISMATCH_JOB = 812,						// 직업이 맞지 않아 언락 실패.
	ERROR_SKILL_ACQUIRE_FAIL_ISNOT_YOURS = 820,					// 보유하고 있지 않은 스킬을 획득하려고 함.
	ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT = 821,		// 스킬을 획득할 때 필요한 포인트가 모자람. (테이블에 정의된 스킬 레벨 1의 필요 SP)
	ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL = 822,		// 선행 스킬을 갖고 있지 않아서 스킬 획득 실패.
	ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL = 823, // 선행 스킬의 레벨이 충족되지 않음.
	ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL = 824,		// 캐릭터 요구 레벨이 충족되지 않아서 스킬획득 실패.
	ERROR_SKILL_ACQUIRE_FAIL_MISMATCH_JOB = 825,
	ERROR_SKILL_UNLOCK_NOT_ENOUGH_MONEY = 826,					// 돈으로 스킬 언락하려하는데 돈이 모자라서 언락 실패.
	ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE = 827,					// 동시에 배울 수 없는 궁극기 스킬을 획득하려 요청이 왔을 때.

	//Friend Message Starting at 900
	ERROR_FRIEND_GETLISTFAIL = 900,						//친구리스트얻기실패
	ERROR_FRIEND_GROUP_ALREADYMAX = 901,				//더이상그룹추가할수없음
	ERROR_FRIEND_ALREADYMAX = 902,						//더이상친구추가할수없음
	ERROR_FRIEND_DESTUSER_ALREADYFRIEND = 903,			//대상과이미친구
	ERROR_FRIEND_USERNOTFOUND = 904,					//대상이없음
	ERROR_FRIEND_FRIEND_NORFOUND = 905,					//대상이 친구가 아님
	ERROR_FRIEND_GROUP_NOTFOUND = 906,					//그룹이 없습니다.
	ERROR_FRIEND_ADDFRIENDFAIL = 907,					//친구추가실패
	ERROR_FRIEND_ADDGROUPFAIL = 908,					//친구그룹추가실패
	ERROR_FRIEND_DELETEFRIENDFAIL = 909,				//친구삭제실패
	ERROR_FRIEND_DELETEGROUPFAIL = 910,					//친구그룹삭제실패
	ERROR_FRIEND_CANT_ADDFRIEND = 911,					//대상을 친구로 추가 할 수 없습니다.

	//Isolate Message Starting at 950
	ERROR_ISOLATE_REQUESTFAIL = 950,					//차단자에게 요청보냄
	ERROR_ISOLATE_ADDFAIL = 951,						//차단실패
	ERROR_ISOLATE_ALREADYADDED = 952,					//이미 차단되어 있음
	ERROR_ISOLATE_DELFAIL = 953,						//차단삭제실패	

	//Guild Message Starting at 1000
	ERROR_GUILD_YOUR_NOT_BELONGANYGUILD = 1000,			// 소속된 길드 없음
	ERROR_GUILD_YOUR_ALREADY_BELONGTOGUILD = 1001,		// 소속된 길드 있음
	ERROR_GUILD_NAME_ALREADYEXIST = 1002,				// 이미 있는 길드이름
	ERROR_GUILD_CREATEFAIL = 1003,						// 길드 생성 실패
	ERROR_GUILD_YOURNOT_INVITED = 1004,					// 초대받지 않았음			// 사용하지 않음 ?
	ERROR_GUILD_LEAVEFAIL = 1005,						// 길드 탈퇴 실패			// 사용하지 않음 ?
	ERROR_GUILD_HAS_NO_AUTHORITY = 1006,				// 권한 없음
	ERROR_GUILD_INFOUPDATEFAIL = 1007,					// 길드 정보변경 실패
	ERROR_GUILD_GRADEINFO_UPDATEFAIL = 1008,			// 길드직급 정보변경 실패	// 사용하지 않음 ?
	ERROR_GUILD_USERINFO_UPDATEFAIL = 1009,				// 길드원 정보변경 실패
	ERROR_GUILD_NOTEXIST_GUILD = 1010,					// 존재하지 않는 길드
	ERROR_GUILD_NOTEXIST_GUILDMEMBER = 1011,			// 존재하지 않는 길드원
	ERROR_GUILD_NOTEXIST_GUILDMASTER = 1012,			// 존재하지 않는 길드장
	ERROR_GUILD_ONLYAVAILABLE_GUILDMASTER = 1013,		// 길드장만 가능
	ERROR_GUILD_NOTAVAILABLE_GUILDMASTER = 1014,		// 길드장은 불가
	ERROR_GUILD_NOTEXIST_OPPOSITEUSER = 1015,			// 상대방이 존재하지 않음	
	ERROR_GUILD_ALREADYINGUILD_OPPOSITEUSER = 1016,		// 상대방이 이미 길드에 가입되어 있음
	ERROR_GUILD_REFUESED_GUILDINVITATION = 1017,		// 상대방이 초대 거절
	ERROR_GUILD_NOTOURGUILD_OPPOSITEUSER = 1018,		// 상대방이 길드원이 아님
	ERROR_GUILD_CANTINVITE_YOURSELF = 1019,				// 자기 자신은 초대할 수 없음
	ERROR_GUILD_CANTEXILE_YOURSELF = 1020,				// 자기 자신은 추방할 수 없음
	ERROR_GUILD_ALREADYINGUILD_ACCOUNT = 1021,			// 길드에 이미 가입된 계정
	ERROR_GUILD_LACKOFCOUNT_CMMPOINT = 1022,			// 길드 일반 포인트 부족
	ERROR_GUILD_FULLOFCOUNT_CMMPOINT = 1023,			// 길드 일반 포인트 초과
	ERROR_GUILD_LACKOFCOUNT_LDDPOINT = 1024,			// 길드 래더 포인트 부족
	ERROR_GUILD_FULLOFCOUNT_LDDPOINT = 1025,			// 길드 래더 포인트 초과
	ERROR_GUILD_LACKOFCOUNT_MONEY = 1026,				// 길드 자금 부족
	ERROR_GUILD_FULLOFCOUNT_MONEY = 1027,				// 길드 자금 초과
	ERROR_GUILD_STILLEXIST_MONEY = 1028,				// 길드 자원이 아직 존재함
	ERROR_GUILD_LACKOFCOUNT_LEVEL = 1029,				// 길드 레벨 최저 도달
	ERROR_GUILD_FULLOFCOUNT_LEVEL = 1030,				// 길드 레벨 최대 도달
	ERROR_GUILD_LACKOFCOUNT_ADDSIZE = 1031,				// 길드 추가 수용인원 최저 도달
	ERROR_GUILD_FULLOFCOUNT_ADDSIZE = 1032,				// 길드 추가 수용인원 최대 도달
	ERROR_GUILD_CANTADD_GUILDMEMBER = 1033,				// 길드 전체 수용인원 초과 (수용가능 인원 부족)
	ERROR_GUILD_CANTDISMISS_MONEY = 1034,				// 길드 해체 불가 (자금 존재)
	ERROR_GUILD_CANTDISMISS_ITEM = 1035,				// 길드 해체 불가 (아이템 존재)
	ERROR_GUILD_CANTDISMISS_GUILDMEMBER = 1036,			// 길드 해체 불가 (길드원 존재)
	ERROR_GUILD_FAILADD_GUILDMEMBER = 1037,				// 길드원 추가 실패
	ERROR_GUILD_ALREADYDISMISS_GUILD = 1038,			// 이미 해체된 길드
	ERROR_GUILD_INVALIDCHANGE_GUILDINFO = 1039,			// 길드 정보 변경 요청이 잘못되었음
	ERROR_GUILD_LACKOFCOND_GUILDINFO = 1040,			// 길드 정보를 변경할 수 있는 조건이 부족
	ERROR_GUILD_INVALIDCHANGE_GUILDMEMBINFO = 1041,		// 길드원 정보 변경 요청이 잘못되었음
	ERROR_GUILD_LACKOFCOND_GUILDMEMBINFO = 1042,		// 길드원 정보를 변경할 수 있는 조건이 부족
	ERROR_GUILD_NOTEXIST_GUILDINFO = 1043,				// 존재하지 않는 길드 정보 (서버상에 존재 여부)
	ERROR_GUILD_NOTEXIST_GUILDMEMBERINFO = 1044,		// 존재하지 않는 길드원 정보 (서버상에 존재 여부)
	ERROR_GUILD_ALREADYINGUILD_CHARACTER = 1045,		// 특정 길드에 이미 가입된 캐릭터
	ERROR_GUILD_LACKOFCOUNT_GUILDMEMBER = 1046,			// 길드 인원 부족
	ERROR_GUILD_LACKOFCOUNT_SELFMONEY = 1047,			// 길드 활동 자금 부족 (개인자금)
	ERROR_GUILD_LACKOFCOUNT_GUILDMONEY = 1048,			// 길드 활동 자금 부족 (길드자금)
	ERROR_GUILD_LACKOFCOUNT_CREATEMONEY = 1049,			// 길드 창설 자금 부족
	ERROR_GUILD_FAILADD_LEVEL = 1050,					// 길드 레벨 증가 실패
	ERROR_GUILD_LACKOFCOUNT_CREATELEVEL = 1051,			// 길드 창설 레벨 부족
	ERROR_GUILD_FAILADD_SIZE = 1052,					// 길드 추가 모집인원 증가 실패
	ERROR_GUILD_STILLEXIST_MEMBER = 1053,				// 길드원이 아직 존재함
	ERROR_GUILD_CANTRENAME_SAME = 1054,					// 현재 소속된 길드명을 사용할 수 없습니다.
	ERROR_GUILDWAR_NOT_REWARD_STEP = 1055,				// 길드전 보상 기간이 아님.
	ERROR_GUILD_DAILYLIMIT = 1056,						// 일일 제한에 도달하여 더 이상 길드포인트를 획득할 수 없습니다
	ERROR_GUILD_INVALIDWARE = 1057,					// 길드 창고를 이용할 수 없습니다.
	ERROR_GUILD_LACKOFCOUNT_GUILDLEVEL = 1058,			// 길드보상 아이템 구입시 길드 레벨 부족으로 실패
	ERROR_GUILD_CANTACCEPT_GUILDINVITATION = 1059,		// 대상이 현재 길드 초대를 받을 수 없습니다

	// PvP Message Starting at 1100
	ERROR_PVP_CREATEROOM_FAILED			= 1100,			// PvP 방 생성 실패
	ERROR_PVP_MODIFYROOM_FAILED = 1101,					// PvP 방 변경 실패
	ERROR_PVP_ALREADY_PVPROOM = 1102,					// 이미PvP에 있음
	ERROR_PVP_LEAVE_FAILED = 1103,						// PvP 방에서 나가기 실패
	ERROR_PVP_JOINROOM_NOTFOUNDROOM = 1104,				// PvP 조인할 방을 찾을 수 없음
	ERROR_PVP_JOINROOM_INVALIDPW = 1105,				// PvP 조인할 방 비밀번호 틀림
	ERROR_PVP_JOINROOM_MAXPLAYER = 1106,				// PvP 조일할 방 최대인원
	ERROR_PVP_JOINROOM_BANUSER = 1107,					// PvP 조일할 방 추방유저
	ERROR_PVP_JOINROOM_FAILED = 1108,					// PvP 방 조인 실패
	ERROR_PVP_CANTMOVECHANNEL_PARTYSTATE = 1109,		// 파티상태에서 채널이동 할수 없음
	ERROR_PVP_FORCELEAVEROOM_MASTERSERVER_DOWN = 1110,	// 마스터 서버 다운으로 강제로 방에서 나옴
	ERROR_PVP_CANTSTART_LESS_MINTEAMPLAYER = 1111,		// 최소 시작 팀 인원 보다 적음
	ERROR_PVP_CANTSTART_MORE_MAXDIFFTEAMPLAYER = 1112,	// 양팀 인원 차이 허용 수 넘음
	ERROR_PVP_STARTPVP_FAILED = 1113,					// PvP시작 실패
	ERROR_PVP_CANTCHANGETEAM_NOSLOT = 1114,					// 슬롯이 없어서 팀변경 못함
	ERROR_PVP_JOINROOM_LEVELLIMIT = 1115,					// PvP 레벨제한으로 입장 못함
	ERROR_PVP_JOINROOM_MAXOBSERVER = 1116,					// PvP 조인할 방 최대 옵져버
	ERROR_PVP_CANTCHANGETEAM_CHANGECAPTAIN_FAILED = 1117,	// 방장 할 사람이 없어서 옮길 수 없음
	ERROR_PVP_CANTCHANGETEAM_LEVELLIMIT = 1118,				// 게임참여 레벨제한으로 팀을 옮길 수 없음
	ERROR_PVP_JOINROOM_DONTALLOW_EVENTROOM_OBSERVER = 1119,	// 이벤트 방에서는 관전하기 불가능합니다.
	ERROR_PVP_SWAPTEAM_MEMBERINDEX_FAIL = 1120,			// PvP팀멤버인덱스 변경실패
	ERROR_PVP_CHANGEMEMBERGRADE_FAIL = 1121,			// PvP GuildWar 대장 부대장 임명 실패
	ERROR_PVP_TRYACQUIREPOINT_FAIL = 1122,				// 획득요청 실패
	ERROR_PVP_TRYACQUIRESKILL_FAIL = 1123,				// 스킬획득실패
	ERROR_PVP_TRYACQUIRESKILL_FAIL_AUTH = 1124,			// 스킬획득요청 권한이 없습니다.	
	ERROR_PVP_INITSKILL_FAIL = 1125,					// 점령전 스킬초기화 실패
	ERROR_PVP_USESKILL_FAIL = 1126,						// 점령전 스킬사용실패
	ERROR_PVP_SWAPSKILLINDEX_FAIL = 1127,				// 점령전 퀵슬롯 인덱스 이동 실패	
	ERROR_PVP_STARTPVP_FAIL_READYMASTER = 1128,			// 점령전일 경우 모든팀의 마스터가 레뒤상태여야함
	ERROR_PVP_GUILDWAR_SECONDARYMASTER_ASIGNFAIL = 1129,// 임명인원 제한 넘음
	ERROR_PVP_QUICK_USERNOTFOUND = 1131,				// 1:1 결투신청할 대상이 없음
	ERROR_PVP_QUICK_FAIL = 1132,						// 1:1 결투신청 실패
	ERROR_PVP_QUICK_CANTINVITE = 1133,					// 대상이 결투를 요청받을 수 없습니다.
	ERROR_PVP_QUICK_DENY = 1134,						// 상대방이 결투신청을 거절 하였습니다.
	ERROR_PVP_QUICK_MY_LEVEL = 1135,					// 내 레벨이 낮아 결투를 할 수 없습니다.
	ERROR_PVP_QUICK_OTHER_LEVEL = 1136,					// 상대방이 레벨이 낮아 결투를 할 수 없습니다.
	ERROR_PVP_LIST_LEVEL_LIMIT = 1137,					// 레벨이 낮아 PVP 입장할 수 없음..
	ERROR_PVP_CHANGELEVEL_FAIL = 1138,					// 채널변경실패!
	ERROR_PVP_JOINFAIL_MISMATCHCHANNEL = 1139,			// 방입장실패 채널이 맞지 않습니다.
	ERROR_PVP_SWAP_TOURNAMENT_INDEX_FAIL = 1140,		// 토너먼트 자리 바꾸기 실패	
	ERROR_PVP_GAMBLEROOM_NOTCOIN = 1141,				// 보유중인 골드가 부족하여 입장할 수 없습니다
	ERROR_PVP_GAMBLEROOM_NOTPETAL = 1142,				// 보유중인 페탈이 부족하여 입장할 수 없습니다

	// HAN auth
	ERROR_HAN_AUTHSVR_0					= 1200,			//	OK
	ERROR_HAN_AUTHSVR_OK = 1201,						//	성공
	ERROR_HAN_AUTHSVR_ARGUMENT_INVALID = 1202,			//	전달된 인자가 올바르지 않습니다.
	ERROR_HAN_AUTHSVR_INITED_ALREADY = 1203,			//	이미 모듈이 초기화 되었습니다.
	ERROR_HAN_AUTHSVR_INITED_NOT = 1204,				//	모듈이 초기화 되지 않았습니다.
	ERROR_HAN_AUTHSVR_INITED_FAIL = 1205,				//	모듈 초기화에 실패했습니다.
	ERROR_HAN_AUTHSVR_CONNPOOL_CREATEFAIL = 1206,		//	커넥션 풀 생성에 실패했습니다.
	ERROR_HAN_AUTHSVR_GETCONNECTION_FAIL = 1207,		//	커넥션 얻기에 실패했습니다
	ERROR_HAN_AUTHSVR_OPENCONNECTION_FAIL = 1208,		//	커넥션 열기에 실패했습니다.
	ERROR_HAN_AUTHSVR_SENDCONNECTION_FAIL = 1209,		//	send 에 실패했습니다.
	ERROR_HAN_AUTHSVR_RECVCONNECTION_FAIL = 1210,		//	receive 에 실패했습니다.
	ERROR_HAN_AUTHSVR_RETURNVALUE_INVALID = 1211,		//	인증서버로부터 리턴 값이 부적절합니다.
	ERROR_HAN_AUTHSVR_AUTHSTRING_INVALID = 1212,		//	AuthString 가 부적절합니다.
	ERROR_HAN_AUTHSVR_DLL_UNEXPECTED = 1213,			//	예상치 못한 오류가 발생했습니다.
	ERROR_HAN_AUTHSVR_PARAM_INVALID = 1214,				//	인증서버로 전달된 인자가 올바르지 않습니다.
	ERROR_HAN_AUTHSVR_IP_INVALID = 1215,				//	IP주소가 올바르지 않습니다.
	ERROR_HAN_AUTHSVR_MEMBERID_INVALID = 1216,			//	사용자 아이디가 올바르지 않습니다.
	ERROR_HAN_AUTHSVR_PASSWORD_INCORRECT = 1217,		//	사용자 비밀번호가 올바르지 않습니다.
	ERROR_HAN_AUTHSVR_PASSWORD_MISMATCHOVER = 1218,		//	비밀번호 입력오류횟수 초과
	ERROR_HAN_AUTHSVR_MEMBERID_NOTEXIST = 1219,			//	존재하지 않는 사용자 입니다
	ERROR_HAN_AUTHSVR_SYSTEM_ERROR = 1220,				//	인증서버에 시스템오류가 발생했습니다.
	ERROR_HAN_AUTHSVR_COOKIE_SETTINGERR = 1221,			//	쿠키가 올바르지 않습니다.
	ERROR_HAN_AUTHSVR_COOKIE_NOTEXIST = 1222,			//	쿠키가 존재하지 않습니다. 
	ERROR_HAN_AUTHSVR_COOKIE_TIMEOUT = 1223,			//	쿠키가 만료되었습니다.

	// Han Billing
	ERROR_HAN_BILLING_OK				= 1230,			// 정상
	// +) fault from Billing Server
	ERROR_HAN_BILLING_GAMECODE_NOTEXIST = 1231,				// GAMEID 가 없습니다.
	ERROR_HAN_BILLING_USERNO_NOTEXIST = 1232,					// User Number 가 없습니다.
	ERROR_HAN_BILLING_CMD_NOTEXIST = 1233,						// 빌링 커맨드가 없습니다.
	ERROR_HAN_BILLING_ORDNO_NOTEXIST = 1234,					// 주문번호가 없습니다.
	ERROR_HAN_BILLING_SCD_NOTEXIST = 1235,						// 상품코드가 없습니다.
	ERROR_HAN_BILLING_SNAME_NOTEXIST = 1236,					// 상품 이름이 없습니다.
	ERROR_HAN_BILLING_QTY_NOTEXIST = 1237,						// 상품 개수가 없습니다.
	ERROR_HAN_BILLING_AMT_NOTEXIST = 1238,						// 구매가격이 없습니다.
	ERROR_HAN_BILLING_IP_NOTEXIST = 1239,						// 클라이언트 IP 주소가 없습니다.
	ERROR_HAN_BILLING_USERNO_INVALID = 1240,					// User Number이 올바르지 않습니다.
	ERROR_HAN_BILLING_BUY_FAIL = 1241,							// 이 주문번호로는 결제 되지 않았습니다.
	ERROR_HAN_BILLING_GAMECODE_INVALID = 1242,					// 게임아이디가 올바르지 않습니다.
	ERROR_HAN_BILLING_BALANCE_INSUFFICIENT = 1243,				// 잔액이 부족합니다.
	ERROR_HAN_BILLING_CMD_UNEXPECTED = 1244, 					// 예상치 못한 커맨드를 수신했습니다.
	ERROR_HAN_BILLING_DB_CONNECTFAIL = 1245,					// 빌링 DB 접속에 실패했습니다.
	ERROR_HAN_BILLING_SVR_UNEXPECTED = 1246,					// 빌링시스템 예상치 못한 오류가 발생했습니다.
	// -) fault from function / this system
	ERROR_HAN_BILLING_ARGUMENT_INVALID = 1247,					// 전달된 인자가 올바르지 않습니다.
	ERROR_HAN_BILLING_INITED_ALREADY = 1248,					// 모듈이 이미 초기화 되었습니다.
	ERROR_HAN_BILLING_INITED_NOT = 1249,						// 모듈이 초기화 되지 않았습니다.
	ERROR_HAN_BILLING_INITED_FAIL = 1250,						// 모듈 초기화에 실패했습니다.
	ERROR_HAN_BILLING_INITED_FAILBALANCE = 1256,				// 잔액조회커넥션 초기화에 실패했습니다.
	ERROR_HAN_BILLING_INITED_FAILBUY = 1257,					// 구매커넥션 초기화에 실패했습니다.
	ERROR_HAN_BILLING_INITED_FAILCONFIRM = 1258,				// 결제확인커넥션 초기화에 실패했습니다.
	ERROR_HAN_BILLING_GETCONN_FAILBALANCE = 1259,				// 잔액조회 커넥션 얻기에 실패했습니다.
	ERROR_HAN_BILLING_GETCONN_FAILBUY = 1260,					// 구매 커넥션 얻기에 실패했습니다.
	ERROR_HAN_BILLING_GETCONN_FAILCONFIRM = 1261,				// 결제확인 커넥션 얻기에 실패했습니다.
	ERROR_HAN_BILLING_OPENREQ_FAIL = 1262,						// Request 오픈에 실패했습니다.
	ERROR_HAN_BILLING_SENDREQ_FAIL = 1263,						// 빌링 정보 Send 에 실패했습니다.
	ERROR_HAN_BILLING_RECVRESULT_FAIL = 1264,					// 빌링정보 Recv 에 실패했습니다.
	ERROR_HAN_BILLING_RECVRESULT_INVALID = 1265,				// 서버로부터 전달된 빌링정보가 올바르지 않습니다.
	ERROR_HAN_BILLING_INITED_FAILBILLNO = 1266,
	ERROR_HAN_BILLING_GETCONN_FAILBILLNO = 1267,
	ERROR_HAN_BILLING_INITED_FAILBILLNOACC = 1268,
	ERROR_HAN_BILLING_GETCONN_FAILBILLNOACC = 1269,
	ERROR_HAN_BILLING_DLL_UNEXPECTED = 1270,					// 모듈 내에서 예상치 못한 오류가 발생했습니다.
	// 4.2	HanBillingGetBillNoAcc() 고유 Return Value
	// +) fault from TGS
	ERROR_HAN_BILLING_GETBILLNO_NEED_PARAM = 1271,				// 필수 파라미터가 누락되었습니다.
	ERROR_HAN_BILLING_GETBILLNO_IP_MISMATCH = 1272,			// IP가 틀립니다.
	ERROR_HAN_BILLING_GETBILLNO_USERID_MISMATCH = 1273,		// 사용자 ID가 틀립니다.
	ERROR_HAN_BILLING_GETBILLNO_DB_CONN_ERROR = 1274,			// DB 연결에 실패했습니다.
	ERROR_HAN_BILLING_GETBILLNO_USER_NOTEXIST = 1275,			// 등록되지 않은 사용자입니다.
	ERROR_HAN_BILLING_GETBILLNO_COOKIE_ERROR = 1276,			// cookie 오류
	ERROR_HAN_BILLING_GETBILLNO_COOKIE_IS_EMPTY = 1277,		// cookie가 비었습니다.
	ERROR_HAN_BILLING_GETBILLNO_COOKIE_TIMEOUT = 1278,			// 만료된 cookie입니다.

	//Auth Message Starting at 1300
	ERROR_AUTH_CERTIFYINGSTEP1_CHECK = 1300,			// 인증정보 체크 시 오류가 발생하였습니다.
	ERROR_AUTH_SERVERID_CHECK = 1301,							// 인증정보 체크 시 오류가 발생하였습니다.
	ERROR_AUTH_CERTIFYINGKEY_CHECK = 1302,						// 인증정보 체크 시 오류가 발생하였습니다.
	ERROR_AUTH_CERTIFYINGKEY_SET = 1303,						// 인증정보 지정 시 오류가 발생하였습니다.
	ERROR_AUTH_AUTHLIMITSEC_EXCEED = 1304,						// 인증정보 체크 시 오류가 발생하였습니다.
	ERROR_AUTH_ACCOUNTID_NOTEXIST = 1305,						// 인증정보 체크 시 오류가 발생하였습니다.
	ERROR_AUTH_WORLDID_SET = 1306,								// 인증정보 지정 시 오류가 발생하였습니다.
	ERROR_AUTH_SESSIONID_SET = 1307,							// 인증정보 지정 시 오류가 발생하였습니다.
	ERROR_AUTH_CERTIFYINGSTEP2_CHECK = 1308,					// 인증정보 체크 시 오류가 발생하였습니다.
	ERROR_AUTH_WORLDID_CHECK  = 1309,							// 인증정보 체크 시 오류가 발생하였습니다.
	ERROR_AUTH_SESSIONID_CHECK = 1310,							// 인증정보 체크 시 오류가 발생하였습니다.

	// NexonPCBang
	ERROR_NEXONAUTH_I0			= 1400,							// 프리미엄 적용PC방이 아닙니다. 이용중인PC방의 담당자에게 문의해 주시기 바랍니다.
	ERROR_NEXONAUTH_M17 = 1401,									// 프리미엄 해택 내용 및 이벤트 내용(이벤트 내용 이미지 구현이 어려울 경우 텍스트로 보여줌)
	ERROR_NEXONAUTH_I3 = 1402,									// PC방 정액제 기간이 만료되어 게임이 종료됩니다. 이용중인PC방의 담당자에게 문의해 주시기 바랍니다.
	ERROR_NEXONAUTH_I19 = 1403,									// PC방 정량제 시간이 만료되어 게임이 종료됩니다. 이용중인PC방의 담당자에게 문의해 주시기 바랍니다.
	ERROR_NEXONAUTH_I2 = 1404,									// IP당 허용 접속자 수가 초과 되었습니다. 이용중인PC방의 담당자에게 문의해 주시기 바랍니다
	ERROR_NEXONAUTH_I27 = 1405,									// PC방 프리미엄 적용 대상이 아닙니다. 넥슨PC방 고객센터로 문의 바랍니다
	ERROR_NEXONAUTH = 1406,										// PC방 관련 전반적인 에러값
	ERROR_NEXONAUTH_I28 = 1407,									// 불법사용자 머신ID차단
	// ERROR_NEXONAUTH_I17,										// PC방 정량제 잔여 시간이 OO시간 OO분 남았습니다
	ERROR_NEXONAUTH_SHUTDOWNED_3 = 1408,						// 청소년 보호법에 따라 0시부터 6시까지는 만 16세 이하 유저는 게임을 사용할 수 없습니다.

	// Nexon Billing 
	ERROR_NEXONBILLING_0		= 1410,					// 오류
	ERROR_NEXONBILLING_99 = 1411,								// DB오류
	ERROR_NEXONBILLING_255 = 1412,								// maintenance

	ERROR_NEXONBILLING_2 = 1413,								// 중복결제에러
	ERROR_NEXONBILLING_3 = 1414,								// 상품 총구매 수량 초과
	ERROR_NEXONBILLING_4 = 1415,								// 상품 구매 개수 초과
	ERROR_NEXONBILLING_5 = 1416,								// 재 구매 일수 미만
	ERROR_NEXONBILLING_10 = 1417,								// 추첨(복권) 상품에 대한 이벤트가 유효하지 않음
	ERROR_NEXONBILLING_11 = 1418,								// 추첨(복권) 상품에 대한 당첨 알고리즘이 유효하지 않음
	ERROR_NEXONBILLING_21 = 1419,								// cart가 없음
	ERROR_NEXONBILLING_22 = 1420,								// 사용제한된 게임
	ERROR_NEXONBILLING_23 = 1421,								// 총판매수량 제한됨
	ERROR_NEXONBILLING_24 = 1422,								// 주문수량제한됨(구매자)
	ERROR_NEXONBILLING_25 = 1423,								// 재주문가능기간아님(구매자)
	ERROR_NEXONBILLING_26 = 1424,								// 선물할수없는 상품
	ERROR_NEXONBILLING_27 = 1425,								// 주문수량제한됨(받는사람)
	ERROR_NEXONBILLING_28 = 1426,								// 재주문가능기간아님(받는사람)
	ERROR_NEXONBILLING_35 = 1427,								// 결제수단 오류
	ERROR_NEXONBILLING_51 = 1428,								// 사용된 쿠폰
	ERROR_NEXONBILLING_52 = 1429,								// 기간만료 (관리자의 의한 임의 쿠폰 기간 만료)
	ERROR_NEXONBILLING_54 = 1430,								// 쿠폰 취소 (사용자 측면에서의 만료)
	ERROR_NEXONBILLING_61 = 1431,								// 쿠폰 정보 에러( 쿠폰 서비스 코드 불일치, 아이템 정보 존재 하지 않음, 계약 상태 유효 하지 않음, 계약 정보 없음, 계약 유형 불일치 등의 오류가 발생 시)
	ERROR_NEXONBILLING_62 = 1432,								// 쿠폰번호가 올바르지 못함
	ERROR_NEXONBILLING_63 = 1433,								// 게임 불일치
	ERROR_NEXONBILLING_65 = 1434,								// 쿠폰 기간 만료.
	ERROR_NEXONBILLING_66 = 1435,								// 특정 사용자를 위한 쿠폰에서 다른 사용자가 쿠폰을 이용을 함.
	ERROR_NEXONBILLING_81 = 1436,								// 쿠폰사용회수제한체크오류
	ERROR_NEXONBILLING_82 = 1437,								// 쿠폰사용회수제한입력오류
	ERROR_NEXONBILLING_100 = 1438,								// PC방이 아닌데 PC방 쿠폰 입력 했다. 그러나 유효한 쿠폰이기는 하다.
	ERROR_NEXONBILLING_101 = 1439,								// PC방이 아닌데 PC방 쿠폰 입력 했다. 그리고 사용된 쿠폰이다.
	ERROR_NEXONBILLING_102 = 1440,								// PC방이 아닌데 PC방 쿠폰 입력 했다. 그리고 만료된 것이다.
	ERROR_NEXONBILLING_103 = 1441,								// PC방이 아닌데 PC방 쿠폰 입력 했다. 그리고 발행 취소된 쿠폰이다
	ERROR_NEXONBILLING_149 = 1442,								// 쿠폰 처리 오류(쿠폰디비등)

	ERROR_NEXONBILLING_1000 = 1443,							// 선물 한도를 초과함
	ERROR_NEXONBILLING_1001 = 1444,							// 선물 연령 제한
	ERROR_NEXONBILLING_1002 = 1445,							// 선물 발송 제한된 사용자
	ERROR_NEXONBILLING_1003 = 1446,							// 선물 수령 제한된 사용자
	ERROR_NEXONBILLING_11001 = 1447,							// 해당아이템 구매한도 초과
	ERROR_NEXONBILLING_11002 = 1448,							// 해당게임 사용자 캐시 잠금 활성화로 인한 구매불가
	ERROR_NEXONBILLING_12001 = 1449,							// 블록된 유저
	ERROR_NEXONBILLING_12002 = 1450,							// 회원정보를 찾을 수 없음
	ERROR_NEXONBILLING_12040 = 1451,							// 잔액부족
	ERROR_NEXONBILLING_12044 = 1452,							// 충전금액을 사용할 수 있는 기간이 지났을 때
	ERROR_NEXONBILLING_120043 = 1453,							// 구매 가능한 아이템이 아닙니다.
	ERROR_NEXONBILLING_120062 = 1454,							// 구매 불가능한 상태의 사용자 입니다.

	ERROR_NEXONBILLING_REFUND_2 = 1455,						// 환불가능 갯수보다 요청갯수가 큼
	ERROR_NEXONBILLING_REFUND_3 = 1456,						// 구매한지 7일이 초과된 상품
	ERROR_NEXONBILLING_REFUND_4 = 1457,						// 패키지중 일부를 수령함
	ERROR_NEXONBILLING_REFUND_5 = 1458,						// 가격이 0원인 상품
	ERROR_NEXONBILLING_REFUND_10 = 1459,						// 중복된 RequestID
	ERROR_NEXONBILLING_REFUND_11 = 1460,						// 존재하지 않는 OrderNo
	ERROR_NEXONBILLING_REFUND_12 = 1461,						// 존재하지 않는 ProductNo	

	//Nexon GioSecure
	ERROR_NEXONAUTH_GIO_TERMINATE = 1470,				// 위치기반 게임이용차단(게임 도중)
	ERROR_NEXONAUTH_GIO_BAN = 1471,						// 위치기반 게임이용차단(로그인시)	

	ERROR_NEXONAUTH_SHUTDOWNED_1 = 1472,					// 나이 정보를 확인 중 오류가 발생하였습니다. 잠시 후 다시 로그인하여 주십시오.
	ERROR_NEXONAUTH_SHUTDOWNED_2 = 1473,					// 실명인증을 받지 않은 회원이거나 넥슨닷컴 회원이 아닙니다.
	// ERROR_NEXONAUTH_SHUTDOWNED_3 = 1408, 위에 이미 있음
	ERROR_NEXONAUTH_SHUTDOWNED_4 = 1474,					// 선택적 셧다운제에 의해 게임이용이 차단됩니다.
	ERROR_NEXONAUTH_SHUTDOWNED_99 = 1475,					// 게임이용이 차단됩니다.

	//ServiceManager
	ERROR_SM_UNKNOWN_OPERATING_TYPE = 1500,				// 알 수 없는 메세지입니다.
	ERROR_SM_TERMINATE_BUSY = 1501,								// 서비스를 죽이는 중입니다.
	ERROR_SM_NETLAUNCHER_INSUFFISIENCY = 1502,					// 구성되어야 하는 런처가 전부 준비되어 있지 않습니다.
	ERROR_SM_STARTEACH_FAILED = 1503,								// 서비스 개별시작이 실패하였습니다.
	ERROR_SM_SELECTED_NETLAUNCHER_NOTFOUND = 1504,				// 서비스 런처개별시작할 런처가 존재하지 않습니다.
	ERROR_SM_NETLAUNCHER_NOTFOUND = 1505,						// 동작중인 넷런처가 없습니다.
	ERROR_SM_TERMINATE_SERVICE_NOTFOUND = 1506,				// 동작을 중지시킬 서비스가 없습니다.
	ERROR_SM_MAKEDUMP_FAIL =  1507,
	ERROR_SM_CHANNELCONTROL_FAIL = 1508,
	ERROR_SM_GAMECONTROL_FAIL = 1509,
	ERROR_SM_UNMATCHED_REQ = 1510,								// 컨테이너 버전이 틀린 요청입니다.
	ERROR_SM_LOGINFAIL = 1511,
	ERROR_SM_LOGINFAIL_VERSIONUNMATCHED = 1512,				// 버전이 틀립니다.
	ERROR_SM_YOU_ARE_NOT_ALLOWED_LEVEL = 1513,					// 해당 커맨드에 대하여 허가받지 않은 요청입니다.
	ERROR_SM_LOGINFAIL_LEVEL_DUPLICATED = 1514,				// 해당 아이디는 중복로그인이 불가합니다.

	// MasterSystem
	ERROR_MASTERSYSTEM_MASTERAPPLICATION_PENALTY = 1600,		// 스승 참여 페널티 기간이라 참여할 수 없음
	ERROR_MASTERSYSTEM_MASTERUSER_OFFLINE = 1601,			
	ERROR_MASTERSYSTEM_LEAVE_DESTUSER_OFFLINE = 1602,					// 사제 끊을 때 대상 유저가 Offline 상태임
	ERROR_MASTERSYSTEM_LEAVE_DESTUSER_CANTSTATE = 1603,				// 사제 끊을 때 대상 유저가 끊을 수 없는 상태임
	ERROR_MASTERSYSTEM_LEAVE_FAILED = 1604,							// 사제 끊기 실패( 공용 에러 )
	ERROR_MASTERSYSTEM_RECALL_SAMECHANNEL = 1605,
	ERROR_MASTERSYSTEM_RECALL_CANTSTATUS = 1606,						// 소환 할 수 없는 상태
	ERROR_MASTERSYSTEM_RECALL_FAILED = 1607,
	ERROR_MASTERSYSTEM_RECALL_REJECT = 1608,							// 소환 거부
	ERROR_MASTERSYSTEM_JOIN_DENY = 1609,								// 사제지간 거부
	ERROR_MASTERSYSTEM_CANT_JOINSTATE = 1610,							// 사제지간 맺을 수 없는 상태

	//ERROR_VEHICLE_DONOTHAVE_VEHICLE = 1700,				// 탈것을 가지고 있지 않습니다.
	//ERROR_VEHICLE_DONOTRIDE_VEHICLE = 1701,				// 탈것에 타고 있지 않습니다.

	// Gamania - AUTH, BILLING
	ERROR_GAMANIA_INVALID_ID_OR_PASSWORD = 1800,		// 잘못된 ID or 패스워드
	ERROR_GAMANIA_NO_SUCH_ACCOUNT = 1801,						// 존재하지 않는 계정입니다.
	ERROR_GAMANIA_NO_SUCH_SERVICE = 1802,						// 존재하지 않는 서비스
	ERROR_GAMANIA_NO_ENOUGH_POINT = 1803,						// 포인트가 부족합니다.
	ERROR_GAMANIA_WRONG_EVENTOTP = 1804,						// EVENT OTP가 틀렸습니다.
	ERROR_GAMANIA_WRONG_OTP = 1805,							// OTP가 틀렸습니다.
	ERROR_GAMANIA_ACCOUNT_LOCK = 1806,							// 계정이 잠겨 있습니다.
	ERROR_GAMANIA_SERVICE_NOT_AVAILABLE = 1807,				// 게임서비스가 활성화 되지 않았습니다.
	ERROR_GAMANIA_TIME_OUT = 1808,								// 타임아웃
	ERROR_GAMANIA_CONTRACT_BLOCK = 1809,						// 계약종료
	ERROR_GAMANIA_ALREADY_LOGIN = 1810,						// 이미 로그인 되어있음.
	ERROR_GAMANIA_ALREADY_WAITING = 1811,						// 이미 인증요청중
	//쿠폰관련
	ERROR_GAMANIA_COUPON_UNAVAILABE = 1812,					// 해당 쿠폰은 사용중지된 쿠폰입니다.
	ERROR_GAMANIA_COUPON_EXPIRED = 1813,						// 쿠폰 기한 만료
	ERROR_GAMANIA_COUPON_WRONG = 1814,							// 쿠폰 입력 에러
	ERROR_GAMANIA_COUPON_WRONG_WORLD = 1815,					// 이 쿠폰은 해당 서버에서 사용이 불가능합니다.

	ERROR_GAMANIA_COUPON_AVAILABE = 1816,						// 쿠폰을 사용하지 않은 상태입니다.
	ERROR_GAMANIA_COUPON_USED = 1817,							// 쿠폰이 이미 사용된 상태입니다.
	ERROR_GAMANIA_COUPON_EXCHANGING = 1818,					// 쿠폰 교환중입니다.
	ERROR_GAMANIA_COUPON_USED_DATE = 1819,						// 이미 특정 기간중 교환 완료. 자세한 내용은 홈피 참조
	ERROR_GAMANIA_COUPON_NOT_START_USE = 1820,					// 쿠폰 교환이 아직 시작되지 않음. 자세한 내용은 홈피 참조.
	ERROR_GAMANIA_COUPON_REINPUT = 1821,						// 아이템 지급 애러. 쿠폰 넘버 재 입력 바람.
	ERROR_GAMANIA_COUPON_ACCOUNT = 1822,						// 해당 쿠폰은 사용한 계정에서 사용 불가.
	ERROR_GAMANIA_COUPON_WORD = 1823,							// 쿠폰 사용 가능 획수 초과
	ERROR_GAMANIA_COUPON_COUNT = 1824,							// 쿠폰 사용 제한 횟수 초과
	ERROR_GAMANIA_SPECIAL_CHAR = 1825,							// 특수문자는 사용할 수 없습니다.

	// 보조스킬
	ERROR_SECONDARYSKILL_ADDRECIPE_FAILED	= 1900,			// 보조스킬 레시피 추가 실패
	ERROR_SECONDARYSKILL_DELETE_FAILED = 1901,						// 보조스킬 제거 실패
	ERROR_SECONDARYSKILL_DELETERECIPE_FAILED = 1902,				// 보조스킬 레시피 제거 실패
	ERROR_SECONDARYSKILL_EXTRACTRECIPE_FAILED = 1903,				// 보조스킬 레시피 추출 실패
	ERROR_SECONDARYSKILL_MANUFACTURE_FAILED = 1904,				// 보조스킬 제작 실패(일반적인 에러)
	ERROR_SECONDARYSKILL_MANUFACTURE_PROBABILITY_FAILED = 1905,	// 보조스킬 제작 확률에 의한 실패
	ERROR_SECONDARYSKILL_MANUFACTURE_DONTALLOWMAP = 1906,			// 보조스킬 제작시 허용하지 않는 맵
	ERROR_SECONDARYSKILL_ADDRECIPE_MAX = 1907,						// 최대 레시피
	ERROR_SECONDARYSKILL_ADDRECIPE_DUPLICATE = 1908,				// 레시피 중복
	ERROR_SECONDARYSKILL_DELETE_FAILED_DONTALLOWMAP = 1909,		// 보조스킬 삭제 허용하지 않는 맵입니다.

	// 농장
	ERROR_FARM_CANT_START_PLANTSTATE = 2000,				// 식물 심을 수 없는 상태
	ERROR_FARM_CANT_START_PLANTSTATE_MAXFIELDCOUNT = 2001,			// 식물 심을 수 없는 상태(최대수제한)
	ERROR_FARM_CANT_CANCEL_PLANTSTATE = 2002,						// 식물 심는거 취소할 수 없는 상태
	ERROR_FARM_CANT_COMPLETE_PLANTSTATE = 2003,					// 식물 심는거 완료할 수 없는 상태
	ERROR_FARM_CANT_COMPLETE_PLANTSTATE_MAXFIELDCOUNT = 2004,		// 식물 심는거 완료할 수 없는 상태(최대수제한)
	ERROR_FARM_CANT_START_HARVESTSTATE = 2005,						// 식물 수확할 수 없는 상태
	ERROR_FARM_CANT_CANCEL_HARVESTSTATE = 2006,					// 식물 수확 취소할 수 없는 상태
	ERROR_FARM_CANT_COMPLETE_HARVESTSTATE = 2007,					// 식물 수확 완료할 수 없는 상태
	ERROR_FARM_CANT_START_ADDWATERSTATE = 2008,					// 식물 물을 줄 수 없는 상태
	ERROR_FARM_CANT_CANCEL_ADDWATERSTATE = 2009,					// 식물 물주는거 취소할 수 없는 상태
	ERROR_FARM_CANT_COMPLETE_ADDWATERSTATE = 2010,					// 식물 물주는거 완료할 수 없는 상태
	ERROR_FARM_CANT_PLANT_OVERLAPCOUNT = 2011,						// 특정 아이템 한번에 심을 수 있는 개수 넘어감
	ERROR_FARM_INVALID_VIP = 2012,									// Vip 아님
	ERROR_FARM_CANT_START_PLANTSTATE_MAXPRIVATEFIELDCOUNT = 2013,	// 식물 심는거 완료할 수 없는 상태(개인구역최대수제한)
	ERROR_FARM_CANT_START_PLANTSTATE_DOWNSCALEFARM = 2014,			// 폐쇄중인 농장
	ERROR_FARM_INVALUD_CHAMPIONGUILD = 2015,						// 우승길드 아님
	ERROR_FARM_CANT_START_PLANTSTATE_GUILDCHAMPION_TIMELIMIT = 2016,// 우승길드 시간 제한

	//낚시
	ERROR_FISHING_FAIL = 2050,								//낚시상태에 대한 일반적인 에러
	ERROR_FISHING_INSUFFICIENCY_FISHINGSKILL_LEVEL = 2051,  //낚시스킬레벨부족
	ERROR_FISHING_INSUFFICIENCY_FISHINGSKILL_GRADE = 2052,  //낚시스킬등급부족
	ERROR_FISHING_INSUFFICIENCY_FISHING_ROD = 2053,			//낚시대가 없어요
	ERROR_FISHING_INSUFFICIENCY_FISHING_BAIT = 2054,		//낚시미끼가 없어요
	ERROR_FISHING_INSUFFICIENCY_INVENTORY = 2055,			//조과를 담을 인벤이 없어요
	ERROR_FISHING_CASTBAIT_FAIL = 2056,						//낚시시작에 대한 기본적인에러
	ERROR_FISHING_STOPFISHING_FAIL = 2057,					//낚시종료실패
	ERROR_FISHING_FAIL_ISNOT_FISHINGAREA = 2058,			//낚시영역이 아님

	// Shanda Cash
	// Author
	ERROR_SHANDAAUTHOR_100201 = 2100,						// -100201 = PT 계정 에러	ptId format error
	ERROR_SHANDAAUTHOR_100202 = 2101,								// -100202	게임 ID 에러	appId error
	ERROR_SHANDAAUTHOR_100203 = 2102,								// -100203	파티션 ID 에러	areaId error
	ERROR_SHANDAAUTHOR_100206 = 2103,								// -100206	계정이 존재하지 않음	Account not exist
	ERROR_SHANDAAUTHOR_100207 = 2104,								// -100207	파티션 ID 존재하지 않음	areaId not exist
	ERROR_SHANDAAUTHOR_100208 = 2105,								// -100208	게임 ID 존재하지 않음	appId not exist
	ERROR_SHANDAAUTHOR_100210 = 2106,								// -100210	기타 기록을 찾지 못한 에러	other error
	ERROR_SHANDAAUTHOR_100213 = 2107,								// -100213	sdid 너무 길거나 영문문자가 포함됨	sdId format error
	ERROR_SHANDAAUTHOR_100299 = 2108,								// -100299	DB 에러	DB error
	ERROR_SHANDAAUTHOR_120101 = 2109,								// -120101	숫자 계정이 너무 길거나 영문문자가 포함됨	sndaID format error
	ERROR_SHANDAAUTHOR_120102 = 2110,								// -120102	PT 계정이 너무 길거나 전부 다 숫자임	ptId format error
	ERROR_SHANDAAUTHOR_120103 = 2111,								// -120103	유저 계정에 대응되는 accountid 에러	accountID error
	ERROR_SHANDAAUTHOR_120104 = 2112,								// -120104	잔액 에러	balance error
	ERROR_SHANDAAUTHOR_120105 = 2113,								// -120105	과금 타입 에러	paytypeId error
	ERROR_SHANDAAUTHOR_120108 = 2114,								// -120108	LBS 로컬 DB에 해당 데이터가 없음	no data in LBS DB
	ERROR_SHANDAAUTHOR_120109 = 2115,								// -120109	기타 에러	other errror
	ERROR_SHANDAAUTHOR_120111 = 2116,								// -120111	게임 ID 에러	appId error
	ERROR_SHANDAAUTHOR_120112 = 2117,								// -120112	네트워크 상태 에러	Net status error
	ERROR_SHANDAAUTHOR_120113 = 2118,								// -120113	메시지 에러(메시지 내용 에러)	data body error
	ERROR_SHANDAAUTHOR_129999 = 2119,								// -129999	DB 에러	DB error
	// Account
	ERROR_SHANDAACCOUNT_102601 = 2120,								// -102601	과금 메시지 상태 에러	Status error
	ERROR_SHANDAACCOUNT_102602 = 2121,								// -102602	과금 타입 에러	Paytype error
	ERROR_SHANDAACCOUNT_102603 = 2122,								// -102603	과금 금액이 0보다 작음	Amount error
	ERROR_SHANDAACCOUNT_102613 = 2123,								// -102613	패킷 log 시간이 5일전임	Calltime error
	ERROR_SHANDAACCOUNT_102604 = 2124,								// -102604	PT 계정이 존재하지 않음	Account not exist
	ERROR_SHANDAACCOUNT_102605 = 2125,								// -102605	게임 파티션 계정이 존재하지 않음	balance not exist
	ERROR_SHANDAACCOUNT_102609 = 2126,								// -102609	sdid 너무 길거나 영문문자가 포함됨	sdId format error
	ERROR_SHANDAACCOUNT_102610 = 2127,								// -102610	소비 타입 에러	appType error
	ERROR_SHANDAACCOUNT_102611 = 2128,								// -102611	커플링 타입 에러, 반드시 1,2 여야 함	coupletype error
	ERROR_SHANDAACCOUNT_102612 = 2129,								// -102612	begintime이 endtime보다 작거나 같아야 함	begintime must samll to endtime
	ERROR_SHANDAACCOUNT_102615 = 2130,								// -102615	amount = 0	Amount is 0
	ERROR_SHANDAACCOUNT_102617 = 2131,								// -102617	기타 에러	other error
	ERROR_SHANDAACCOUNT_102699 = 2132,								// -102699	DB 에러	DB error
	ERROR_SHANDAACCOUNT_120201 = 2133,								// -120201	숫자 계정이 너무 길거나 영문문자가 포함됨	sndaID format error
	ERROR_SHANDAACCOUNT_120202 = 2134,								// -120202	PT 계정이 너무 길거나 전부 다 숫자임	ptId format error
	ERROR_SHANDAACCOUNT_120203 = 2135,								// -120203	과금 수량 포맷 에러	Amount error
	ERROR_SHANDAACCOUNT_120204 = 2136,								// -120204	패킷 업로드 시간간격 에러	period error
	ERROR_SHANDAACCOUNT_120205 = 2137,								// -120205	계정이 존재하지 않음	ID not exist
	ERROR_SHANDAACCOUNT_120206 = 2138,								// -120206	통합 기록이 없음	no union data
	ERROR_SHANDAACCOUNT_120207 = 2139,								// -120207	지급 타입 에러	paytypeId error
	ERROR_SHANDAACCOUNT_120208 = 2140,								// -120208	숫자 계정이 존재하지 않음	sndaID not exist
	ERROR_SHANDAACCOUNT_120209 = 2141,								// -120209	기타 파라미터 포맷 에러	other param format error
	ERROR_SHANDAACCOUNT_120210 = 2142,								// -120210	orderid 중복	orderid error
	ERROR_SHANDAACCOUNT_120211 = 2143,								// -120211	serviceid 에러	appId error
	ERROR_SHANDAACCOUNT_120212 = 2144,								// -120212	네트워크 상태 에러	net status error
	ERROR_SHANDAACCOUNT_120213 = 2145,								// -120213	cache flag 에러	flag error
	ERROR_SHANDAACCOUNT_120214 = 2146,								// -120214	상태 파라미터 에러	status error
	ERROR_SHANDAACCOUNT_120215 = 2147,								// -120215	한개 sessionid에 여러개 기록이 존재	Data repeat
	ERROR_SHANDAACCOUNT_120216 = 2148,								// -120216	ptid와 sessionid가 매치되지 않음	ptId and sessid error
	ERROR_SHANDAACCOUNT_120217 = 2149,								// -120217	PT 계정이 존재하지 않음	ptID not exist
	// ERROR_SHANDAACCOUNT_129999,								// -129999	시스템 에러＋sqlcode	other error
	// 유저 숫자계정			
	ERROR_SHANDABILLING_200101 = 2150,								// SDIDLONGER	-200101	입력길이 초과	long
	ERROR_SHANDABILLING_200102 = 2151,								// SDIDNULL	-200102	비어있음	null
	ERROR_SHANDABILLING_200103 = 2152,								// SDIDILLEGAL	-200103	불법 문자열이 포함됨	format error
	ERROR_SHANDABILLING_200199 = 2153,								// SDIDOTHER	-200199	기타 에러	other error
	// 유저 PT 계정			
	ERROR_SHANDABILLING_200201 = 2154,								// PTIDLONGER	-200201	입력길이 초과	long
	ERROR_SHANDABILLING_200202 = 2155,								// PTIDNULL	-200202	비어있음	null
	ERROR_SHANDABILLING_200203 = 2156,								// PTIDILLEGAL	-200203	불법 문자열이 포함됨	format error
	ERROR_SHANDABILLING_200204 = 2157,								// PTIDALLNUM	-200204	전부 다 숫자거나 숫자로 시작됨	format error
	ERROR_SHANDABILLING_200299 = 2158,								// PTIDOTHER	-200299	기타 에러	other error
	// 서비스 타입 ID			
	ERROR_SHANDABILLING_200301 = 2159,								// SERVIDLONGER	-200301	입력길이 초과	long
	ERROR_SHANDABILLING_200302 = 2160,								// SERVIDNULL	-200302	비어있음	null
	ERROR_SHANDABILLING_200303 = 2161,								// SERVIDILLEGAL	-200303	불법 문자열이 포함됨	format error
	ERROR_SHANDABILLING_200399 = 2162,								// SERVIDOTHER	-200399	기타 에러	other error
	// 게임 파티션 ID			
	ERROR_SHANDABILLING_200401 = 2163,								// AREAIDLONGER	-200401	입력길이 초과	long
	ERROR_SHANDABILLING_200402 = 2164,								// AREAIDNULL	-200402	비어있음	null
	ERROR_SHANDABILLING_200403 = 2165,								// AREAIDILLEGAL	-200403	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_200499 = 2166,								// AREAIDOTHER	-200499	기타 에러	other error
	// 세션 id			
	ERROR_SHANDABILLING_200501 = 2167,								// SESSIDLONGER	-200501	입력길이 초과	long
	ERROR_SHANDABILLING_200502 = 2168,								// SESSIDNULL	-200502	비어있음	null
	ERROR_SHANDABILLING_200503 = 2169,								// SESSIDILLEGAL	-200503	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_200504 = 2170,								// SESSIDERRFORMAT	-200504	포멧 에러(조합 불법)	format error
	ERROR_SHANDABILLING_200599 = 2171,								// SESSIDOTHER	-200599	기타 에러	other error
	// 데이터패킷 id		
	ERROR_SHANDABILLING_200601 = 2172,								// UNIQUEIDLONGER	-200601	입력길이 초과	long
	ERROR_SHANDABILLING_200602 = 2173,								// UNIQUEIDNULL	-200602	비어있음	null
	ERROR_SHANDABILLING_200603 = 2174,								// UNIQUEIDILLEGAL	-200603	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_200604 = 2175,								// UNIQUEIDERRFORMAT	-200604	포멧 에러(조합 불법)	format error
	ERROR_SHANDABILLING_200605 = 2176,								// UNIQUEIDREPEAT	-200605	uniqueid 중복됨	id repeat
	ERROR_SHANDABILLING_200699 = 2177,								// UNIQUEIDOTHER	-200699	기타 에러	null
	// 월드 ID			
	ERROR_SHANDABILLING_200801 = 2178,								// GROUPIDLONGER	-200801	입력길이 초과	long
	ERROR_SHANDABILLING_200802 = 2179,								// GROUPIDNULL	-200802	비어있음	null
	ERROR_SHANDABILLING_200803 = 2180,								// GROUPIDILLEGAL	-200803	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_200899 = 2181,								// GROUPIDOTHER	-200899	기타 에러	other error
	// 시작시간			
	ERROR_SHANDABILLING_201301 = 2182,								// BEGINTIMELONGER	-201301	입력길이 초과	long
	ERROR_SHANDABILLING_201302 = 2183,								// BEGINTIMENULL	-201302	비어있음	null
	ERROR_SHANDABILLING_201303 = 2184,								// BEGINTIMEILLEGAL	-201303	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_201304 = 2185,								// BEGINTIMEERRLONGFOMAT	-201304	포맷 에러, YYYY-MM-DD hh:mi:ss 포맷이어야 함	format error
	ERROR_SHANDABILLING_201305 = 2186,								// BEGINTIMEERRSHORTFOMAT	-201305	불법 문자 포함됨，YYYY-MM-DD 포맷이어야 함	format error
	ERROR_SHANDABILLING_201399 = 2187,								// BEGINTIMEOTHER	-201399	기타 에러	other error
	// 종료시간			
	ERROR_SHANDABILLING_201401 = 2188,								// ENDTIMELONGER	-201401	입력길이 초과	long
	ERROR_SHANDABILLING_201402 = 2189,								// ENDTIMENULL	-201402	비어있음	null
	ERROR_SHANDABILLING_201403 = 2190,								// ENDTIMEILLEGAL	-201403	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_201404 = 2191,								// ENDTIMEERRLONGFOMAT	-201404	포맷 에러, YYYY-MM-DD hh:mi:ss 포맷이어야 함	format error
	ERROR_SHANDABILLING_201405 = 2192,								// ENDTIMEERRSHORTFOMAT	-201405	불법 문자 포함됨，YYYY-MM-DD 포맷이어야 함	format error
	ERROR_SHANDABILLING_201406 = 2193,								// ENDTIMEERRVALUE	-201406	종료시간이 시작시간보다 큼	error value
	ERROR_SHANDABILLING_201499 = 2194,								// ENDTIMEOTHER	-201499	기타 에러	other error
	// 데이터 패킷 id			
	ERROR_SHANDABILLING_201501 = 2195,								// ORDERIDLONGER	-201501	입력길이 초과	long
	ERROR_SHANDABILLING_201502 = 2196,								// ORDERIDNULL	-201502	비어있음	null
	ERROR_SHANDABILLING_201503 = 2197,								// ORDERIDILLEGAL	-201503	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_201504 = 2198,								// ORDERIDERRFORMAT	-201504	포맷 에러（조합 불법）	format error
	ERROR_SHANDABILLING_201505 = 2199,								// ORDERIDREPEAT	-201505	orderid중복	ID repeat
	ERROR_SHANDABILLING_201599 = 2200,								// ORDERIDOTHER	-201599	기타 에러	other error
	// 과금 타입			
	ERROR_SHANDABILLING_201901 = 2202,								// PAYTYPELONGER	-201901	입력길이 초과	long
	ERROR_SHANDABILLING_201902 = 2203,								// PAYTYPENULL	-201902	비어있음	null
	ERROR_SHANDABILLING_201903 = 2204,								// PAYTYPEILLEGAL	-201903	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_201904 = 2205,								// PAYTYPEERRVALUE	-201904	입력 에러(특정값을 가지지 않음)	error value
	ERROR_SHANDABILLING_201999 = 2206,								// PAYTYPEOTHER	-201999	기타 에러	other error
	// 소비 타입			
	ERROR_SHANDABILLING_202001 = 2207,								// APPTYPELONGER	-202001	입력길이 초과	long
	ERROR_SHANDABILLING_202002 = 2208,								// APPTYPENULL	-202002	비어있음	null
	ERROR_SHANDABILLING_202003 = 2209,								// APPTYPEILLEGAL	-202003	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_202004 = 2210,								// APPTYPEERRVALUE	-202004	입력 에러(특정값을 가지지 않음)	error value
	ERROR_SHANDABILLING_202099 = 2211,								// APPTYPEOTHER	-202099	기타 에러	other error
	// 커플링 타입			
	ERROR_SHANDABILLING_202101 = 2212,								// COUPLETYPELONGER	-202101	입력길이 초과	long
	ERROR_SHANDABILLING_202102 = 2213,								// COUPLETYPENULL	-202102	비어있음	null
	ERROR_SHANDABILLING_202103 = 2214,								// COUPLETYPEILLEGAL	-202103	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_202104 = 2215,								// COUPLETYPEERRVALUE	-202104	입력 에러(특정값을 가지지 않음)	error value
	ERROR_SHANDABILLING_202199 = 2216,								// COUPLETYPEOTHER	-202199	기타 에러	other error
	// 금액			
	ERROR_SHANDABILLING_202201 = 2217,								// AMOUNTLONGER	-202201	입력길이 초과	long
	ERROR_SHANDABILLING_202202 = 2218,								// AMOUNTNULL	-202202	비어있음	null
	ERROR_SHANDABILLING_202203 = 2219,								// AMOUNTILLEGAL	-202203	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_202204 = 2220,								// AMOUNTNEGATIVE	-202204	마이너스 숫자 입력	error value
	ERROR_SHANDABILLING_202205 = 2221,								// AMOUNTZERO	-202205	0을 입력	error value
	ERROR_SHANDABILLING_202206 = 2222,								// AMOUNTERRVALUE	-202206	입력한 숫자가 너무 큼	error value
	ERROR_SHANDABILLING_202299 = 2223,								// AMOUNTOTHER	-202299	기타 에러	other error
	// 빌링 할인율			
	ERROR_SHANDABILLING_202301 = 2224,								// DISCOUNTLONGER	-202301	입력길이 초과	long
	ERROR_SHANDABILLING_202302 = 2225,								// DISCOUNTNULL	-202302	비어있음	null
	ERROR_SHANDABILLING_202303 = 2226,								// DISCOUNTILLEGAL	-202303	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_202399 = 2227,								// DISCOUNTOTHER	-202399	기타 에러	other error
	// 클라이언트 IP 주소			
	ERROR_SHANDABILLING_202501 = 2228,								// IPLONGER	-202501	입력길이 초과	long
	ERROR_SHANDABILLING_202502 = 2229,								// IPNULL	-202502	비어있음	null
	ERROR_SHANDABILLING_202503 = 2230,								// IPILLEGAL	-202503	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_202504 = 2231,								// IPERRVALUE	-202504	입력 에러(지정 범위내에 없음)	error value
	ERROR_SHANDABILLING_202599 = 2232,								// IPOTHER	-202599	기타 에러	other error
	// 상태			
	ERROR_SHANDABILLING_204401 = 2233,								// STATUSLONGER	-204401	입력길이 초과	long
	ERROR_SHANDABILLING_204402 = 2234,								// STATUSNULL	-204402	비어있음	null
	ERROR_SHANDABILLING_204403 = 2235,								// STATUSILLEGAL	-204403	불법 문자 포함됨	format error
	ERROR_SHANDABILLING_204404 = 2236,								// STATUSERRVALUE	-204404	입력 에러(지정한 값과 일치하지 않음)	error value
	ERROR_SHANDABILLING_204499 = 2237,								// STATUSOTHER	-204499	기타 에러	other error
	// 유저 uid			
	ERROR_SHANDABILLING_200211 = 2238,								// UIDLONGER	-200211	입력길이 초과	long
	// CBS서비스 처리			
	ERROR_SHANDABILLING_300001 = 2239,								// CBSDB_IS_DISCONNECT	-300001	cbs DB 접속 에러	sql connect error
	ERROR_SHANDABILLING_300102 = 2240,								// CBSDB_PROC_ERROR	-300002	cbs sql 실행 에러	sql error
	ERROR_SHANDABILLING_300103 = 2241,								// UNAUTHOR_OPERATION	-300003	권한 부여하지 않은 요청 패킷 타입	unauthor
	ERROR_SHANDABILLING_300201 = 2242,								// AMOUNT_NOT_MATCHING	-300201	금액 불일치	amount error
	ERROR_SHANDABILLING_390001 = 2243,								// RES_TIMEOUT	-390001	응답 시간 초과	response time out
	// 통신 반환-API		
	ERROR_SHANDABILLING_401001 = 2244,								// SOCKDISCONNECT	-401001	socket 연결 끊김	socket disconnect
	ERROR_SHANDABILLING_401002 = 2245,								// SOCKIP_OR_PORTERR	-401002	서버 ip/port 에러	IP Port error
	ERROR_SHANDABILLING_401003 = 2246,								// SOCKCONNECTFAIL	-401003	socket 연결 실패	socket connect fail
	ERROR_SHANDABILLING_401004 = 2247,								// SOCKLOCALFAILED	-401004	로컬 초기화 SOCKET 실패	initial fail
	ERROR_SHANDABILLING_401005 = 2248,								// SNDBUF_OVERFLOW	-401005	발송 buffer overflow	send buffer overflow
	ERROR_SHANDABILLING_401006 = 2249,								// RCVBUF_OVERFLOW	-401006	접수 buffer overflow	recive buffer overflow
	ERROR_SHANDABILLING_401007 = 2250,								// TMOBUF_OVERFLOW	-401007	timeout buffer overflow	timeout buffer overflow
	ERROR_SHANDABILLING_401008 = 2251,								// LSOCKDISCONNECT	-401008	통신 에러	socket error
	ERROR_SHANDABILLING_401011 = 2252,								// NOINITCLIENT	-401011	클라이언트 초기화하지 않음	client without initial
	// QUEUE 처리			
	ERROR_SHANDABILLING_402001 = 2253,								// QUEUECACHEFULL	-402001	행렬 cache full	queue cache full
	ERROR_SHANDABILLING_402002 = 2254,								// QUEUELOCKERROR	-402002	행렬 lock 에러	queue lock error
	ERROR_SHANDABILLING_402003 = 2255,								// QUEUEOPERATEERROR	-402003	행렬 Push/Pop 전송 파라미터 에러(빈 pointer)	queue param error
	ERROR_SHANDABILLING_402004 = 2256,								// QUEUEEMPTY	-402004	행렬이 비어있음, Pop 데이터 없음	queue null
	ERROR_SHANDABILLING_402005 = 2257,								// QUEUEDATAERR	-402005	cache 내용 에러, 길이가 cache 가능 범위 초과 등	queue data error
	// 데이터 전송			
	ERROR_SHANDABILLING_403001 = 2258,								// USERDATALENERROR	-403001	유저 데이터 Push 길이와 서비스 구조 길이 불일치	data length error
	ERROR_SHANDABILLING_403002 = 2259,								// COMMUDATAERROR	-403002	접수한 통신 데이터 패킷 암호화 에러	encrypt error
	// POINTER insert			
	ERROR_SHANDABILLING_404001 = 2260,								// POINTERNULL	-404001	빈 pointer 삽입(pointer가 필요한 곳에서 빈 pointer를 접수할 수 없음)	point null
	// 데이터 encode/decode			
	ERROR_SHANDABILLING_405001 = 2261,								// ENCRYPTERROR	-405001	암호화 에러	encrypt error
	ERROR_SHANDABILLING_405002 = 2262,								// DECRYPTERROR	-405002	암호해제 에러	decrypt error
	// 설정정보			
	ERROR_SHANDABILLING_406001 = 2263,								// PARAMPATHERROR	-406001	cache 혹은 annular 파일 경로 에러	path error
	ERROR_SHANDABILLING_406002 = 2264,								// CONFLOADFAILED	-406002	설정 파일 로딩 실패	fail to load ini
	ERROR_SHANDABILLING_406003 = 2265,								// CONFGETFAILED	-406003	설정 서버에서 설정 획득 실패	fail to get ini
	ERROR_SHANDABILLING_406004 = 2266,								// RELOADFAILED	-406004	로딩 실패	fail to reload
	// 설정서버 접속 정보			
	ERROR_SHANDABILLING_407001 = 2267,								// ERR_MSG_TYPE	-407001	요청 메시지 중 메시지 타입 에러	message type error
	ERROR_SHANDABILLING_407002 = 2268,								// IP_NOT_EXIST	-407002	요청 메시지 중 hostIP가 없음	hostip not exist
	ERROR_SHANDABILLING_407003 = 2269,								// ID_NOT_EXIST	-407003	요청 메시지 중 hostId가 없음	hostid not exist
	ERROR_SHANDABILLING_407004 = 2270,								// SYS_FLAG_INVALID	-407004	OS flag 에러	system flag error
	ERROR_SHANDABILLING_407005 = 2271,								// MSG_LEN_ERR	-407005	요청 메시지 길이 에러	message length error
	ERROR_SHANDABILLING_407006 = 2272,								// CANNOTCONNECTDB	-407006	DB 연결 실패	db connect error
	ERROR_SHANDABILLING_407099 = 2273,								// UNKNOWNED_ERR	-407099	기타 에러	other error
	// 기타			
	ERROR_SHANDABILLING_499999 = 2280,								// UNDECLARE_ERROR	-499999	기타 에러	other error
	
	//래더시스템
	ERROR_ALREADY_LADDERROOM = 2300,						// 이미 래더방에 있습니다.
	ERROR_INVALIDUSER_LADDERROOM = 2301,							// 래더룸에 없는 잘못된 유저입니다.
	ERROR_LADDERSYSTEM_CANT_MATCHINGCANCLE_ROOMSTATE = 2302,		// 매칭취소요청을 할 수 없는 State 입니다.
	ERROR_LADDERSYSTEM_CANT_MATCHING_ROOMSTATE = 2303,				// 매칭요청을 할 수 없는 State 입니다.
	ERROR_LADDERSYSTEM_NOLEADER = 2304,							// 래더방 리더가 아닙니다.
	ERROR_LADDERSYSTEM_CANT_ENTER_LEVELLIMIT = 2305,				// 레벨제한으로 래더 입장할 수 없음
	ERROR_LADDERSYSTEM_CANT_EXIST_ROOMLIST_PAGE = 2306,			// 존재하지 않는 룸리스트 페이지 요청
	ERROR_LADDERSYSTEM_CANT_EXIST_ROOM = 2307,						// 존재하지 않는 방입니다.
	ERROR_LADDERSYSTEM_CANT_OBSERVER_ROOMSTATE = 2308,				// 옵져버로 참여할 수 없는 방상태입니다.
	ERROR_LADDERSYSTEM_LADDERPOINT_INVALID = 2309,
	ERROR_LADDERSYSTEM_EXHIBITION_DURATION = 2310,					// 시범경기기간임
	ERROR_LADDERSYSTEM_NOTENOUGH_USER = 2311,						// 유저수가 충분하지 않음
	ERROR_LADDERSYSTEM_CANT_INVITESTATE = 2312,					// 래더방에 초대할 수 없는 상태입니다.
	ERROR_LADDERSYSTEM_CANT_RECVINVITESTATE = 2313,				// 대상자가 래더방에 초대받을 수 없는 상태입니다.
	ERROR_LADDERSYSTEM_ROOM_FULLUSER = 2314,						// 래더방이 Full임
	ERROR_LADDERSYSTEM_INVALID_INVITEUSER = 2315,					// 초대받지 않은 유저임
	ERROR_LADDERSYSTEM_CANT_JOIN_ROOMSTATE = 2316,					// 초대받아 Join 할 수 없는 상태
	ERROR_LADDERSYSTEM_INVITE_DENY = 2317,							// 초대 거부
	ERROR_LADDERSYSTEM_CANT_INVITE_LEVELLIMIT = 2318,				// 레벨제한으로 래더 초대할 수 없음
	ERROR_LADDERSYSTEM_CANT_KICKOUT_ROOMSTATE = 2319,				// 유저 내쫒을 수 없는 방상태
	ERROR_LADDERSYSTEM_KICKOUT = 2320,								// 방장에 의해 쫒겨남
	ERROR_LADDERSYSTEM_INVITING = 2321,							// 초대중인 상태임

	// 길드전
	ERROR_GUILDWAR_CANTADD_GUILDMEMBER = 2400,				// 길드전 기간에는 가입이 불가능합니다.
	ERROR_GUILDWAR_CANTLEAVE_GUILDMEMBER = 2401,			// 길드전 기간에는 탈퇴가 불가능합니다.
	ERROR_GUILDWAR_CANTEXILE_GUILDMEMBER = 2402,			// 길드전 기간에는 추방이 불가능합니다.
	ERROR_GUILDWAR_CANTDISMISS = 2403,						// 길드전 기간에는 길드해체가 불가능합니다.
	ERROR_GUILDWAR_CANTENROLL = 2404,						// 길드전 신청기간이 아닙니다.
	ERROR_GUILDWAR_ALREADY_ENROLL = 2405,					// 이미 길드전 신청을 했습니다.	
	ERROR_GUILDWAR_ALREADY_VOTE = 2406,						// 이미 인기투표를 하셨습니다.
	ERROR_GUILDWAR_CANT_VOTE = 2407,						// 인기투표를 할수가 없습니다.
	ERROR_GUILDWAR_CANT_COMPENSATION = 2408,				// 길드전 예선 보상을 받을 수가 없습니다.
	ERROR_GUILDWAR_NOT_PRE_WIN = 2409,						// 길드전 우승팀이 아닙니다.
	ERROR_GUILDWAR_SKILL_COOLTIME = 2410,					// 아직 우승스킬을 사용할 수 없습니다.

	//Cashshop
	ERROR_CASHSHOP_FAIL_PREVIEWEQUIP_GENERAL = 2500,		// 코스튬을 장착할 수 없습니다. 다시 시작해 주세요.
	ERROR_CASHSHOP_FAIL_PREVIEWEQUIP_CRC	 = 2501,
	ERROR_CASHSHOP_FARMVIP_DUPLICATED = 2502,						// 농장VIP 중복

	//Cherry Auth (SG)
	ERROR_CHERRY_AUTHFAIL = 2600,							// 싱가폴인증실패
	ERROR_CHERRY_AUTHSUCCESS = 2600,								// 성공
	ERROR_CHERRY_AUTHBLOCKED = 2600,								// 잠김?
	ERROR_CHERRY_AUTHSUSPENDED = 2600,								// 휴면?
	ERROR_CHERRY_AUTHOTHER = 2600,									// ?????
	ERROR_CHERRY_NOTALLOWED_SPECIALCHAR = 2600,					// 특수문자 사용불가?
	ERROR_CHERRY_NO_SUCH_ACCOUNT = 2600,							// 계정 없음

	//Cherry Billing (SG)
	ERROR_CHERRY_COUPON_ALREADY = 2700,						// 쿠폰이 이미 사용된 상태입니다.
	ERROR_CHERRY_COUPON_EXPRIED = 2700,							// 쿠폰 기한 만료	
	ERROR_CHERRY_COUPON_INVALID = 2700,							// 쿠폰 입력 에러
	ERROR_CHERRY_COUPON_AND_CHARACTERID_MISMATCH = 2700,			// 쿠폰 캐릭터 아이디 미스매치

	// 튕긴(?) 파티 복구 
	ERROR_CANT_RESTORE_PARTY_STATE = 2800,					// 다시 파티 합류 할 수 없는 상태(?)입니다.
	ERROR_CANT_EXSIT_BACKUPPARTYINFO = 2801,						// 복구 할 파티 정보가 없음

	// Pet
	ERROR_CANT_RECALL_PET_STATUS = 2900,					// 펫 소환할 수 없는 상태
	ERROR_PETNAME_SHORTAGE_2 = 2901,						// 펫이름이 짧아 생성 할 수 없음 (2자이상)
	ERROR_PETNAME_SHORTAGE_4 = 2902,						// 펫이름이 짧아 생성 할 수 없음 (4자이상)
	ERROR_PETNAME_PROHIBIT = 2903,						// 허용하지 않는 문자가 존재합니다.
	ERROR_PETFOOD_FAIL_MAX = 2904,						// 만복도가 가득차서 먹을 수 없음
	ERROR_PETFOOD_FAIL_NORMAL = 2905,						// 일반 먹이는 90%이상 먹을수 없음.

	// Gamania - AUTH, BILLING
	ERROR_ASIASOFT_INVALID_REQUEST = 3000,				// 잘못된 요청
	ERROR_ASIASOFT_INVALID_USERID = 3001,						// 존재하지 않는 계정입니다.
	ERROR_ASIASOFT_USER_ID_NOT_FOUND = 3002,					// 유저아이디가 없음(위와 다른게 머임? ㅋ 일단 리턴코드가 존재하므로)
	ERROR_ASIASOFT_WORNG_PASSWORD = 3003,						// 패스워드가 틀렸다함
	ERROR_ASIASOFT_DISABLE_USER = 3004,						// 이게 블럭인가? (Optional)이라고 붙어있음
	ERROR_ASIASOFT_DATABASE_ERROR = 3005,						// 아지아솝흐트 뒤뷔에러
	ERROR_ASIASOFT_UNEXPECTED_ERROR = 3006,					// 아지아솝흐트 예기치 않은 에러
	ERROR_ASIASOFT_OTP_FAILED = 3007,							// OTP안데때	

	ERROR_ASIASOFT_NO_SUCH_SERVICE = 3008,						// 존재하지 않는 서비스
	ERROR_ASIASOFT_NO_ENOUGH_POINT = 3009,						// 포인트가 부족합니다.
	ERROR_ASIASOFT_WRONG_EVENTOTP = 3010,						// EVENT OTP가 틀렸습니다.
	ERROR_ASIASOFT_WRONG_OTP = 3011,							// OTP가 틀렸습니다.
	ERROR_ASIASOFT_ACCOUNT_LOCK = 3012,							// 계정이 잠겨 있습니다.
	ERROR_ASIASOFT_SERVICE_NOT_AVAILABLE = 3013,				// 게임서비스가 활성화 되지 않았습니다.
	ERROR_ASIASOFT_TIME_OUT = 3014,								// 타임아웃
	ERROR_ASIASOFT_CONTRACT_BLOCK = 3015,						// 계약종료
	ERROR_ASIASOFT_ALREADY_LOGIN = 3016,						// 이미 로그인 되어있음.
	ERROR_ASIASOFT_ALREADY_WAITING = 3017,						// 이미 인증요청중
	ERROR_ASIASOFT_SPECIAL_CHAR = 3018,							// 특수문자는 사용할 수 없습니다.

	//Donation
	ERROR_NOT_AVAILABLE_DONATION_TIME = 3100,			// 기부를 할 수 없는 시간입니다.

	//Potential
	ERROR_EXCHANGEPOTENTIAL_OK = 3200,									// 성공~
	ERROR_EXCHANGEPOTENTIAL_FAIL = 3201,								// 잠재력 교환은 전반적인 에러 실패!
	ERROR_EXCHANGEPOTENTIAL_EXTRACT_ITEM_VALIDATION_FAIL = 3202,		// 잠재력을 뽑을 아이템 확인 실패
	ERROR_EXCHANGEPOTENTIAL_INJECT_ITEM_VALIDATION_FAIL = 3203,			// 잠재력을 넣을 아이템 확인 실패
	ERROR_EXCHANGEPOTENTIAL_ITEM_COMPARE_VALIDATION_FAIL = 3204,		// 아이템간 비교 확인 실패(랜크, 파츠, 타입등)
	ERROR_EXCHANGEPOTENTIAL_MOVEABLECOUNT_EXPIRED = 3205,				// 포텐셜이동 횟수 제한

	// ExchangeEnchant
	ERROR_EXCHANGE_ENCHANT_FAIL = 3206,									// 강화 이동 실패
	ERROR_EXCHANGE_ENCHANT_REQ_MONEY = 3207,							// 필요 소지금 부족
	ERROR_EXCHANGE_ENCHANT_REQ_ITEM = 3208,								// 필요 아이템 부족

	// BestFriend
	ERROR_BESTFRIEND_REGISTFAIL_DESTUSER_SITUATION_NOTALLOWED = 3300,	// 절친 등록을 할 수 있는 상태가 아님
	ERROR_BESTFRIEND_NOTEXIST_OPPOSITEUSER = 3301,						// 상대방이 존재하지 않음	
	ERROR_BESTFRIEND_REFUESED_REGISTRATION = 3302,						// 등록 거절
	ERROR_BESTFRIEND_ALREADY_REGISTERED = 3303,							// 이미 절친상태입니다.
	ERROR_BESTFRIEND_NOT_REGISTERED = 3304,								// 절친상태가 아닙니다.
	ERROR_BESTFRIEND_NOTSEARCH = 3305,									// 유저를 찾지 못했습니다
	ERROR_BESTFRIEND_NOTHAVE_REGISTITEM = 3306,							// 절친 등록서를 가지고 있지 않습니다.
	ERROR_BESTFRIEND_HAVE_REWARDITEM = 3307,							// 이미 절친 보상 아이템을 가지고 있습니다.
	ERROR_BESTFRIEND_SELFSEARCH = 3308,									// 자기자신을 절친검색할 수 없습니다.
	ERROR_BESTFRIEND_SELFREGIST = 3309,									// 자기자신을 절친등록할 수 없습니다.
	
	// Kreon - AUTH, BILLING
	ERROR_KREON_INVALID_REQUEST = 3400,							// 잘못된 요청
	ERROR_KREON_INVALID_USERID = 3401,							// 존재하지 않는 계정입니다.	 
	ERROR_KREON_WRONG_PASSWORD = 3402,						// 패스워드가 틀렸다함
	ERROR_KREON_BLOCK_GEMSCOOL = 3403,						// GEMSCOOL 블럭(모든 게임 블럭)
	ERROR_KREON_BLOCK_DN	   = 3404,						// 드네 블럭(드네만 블럭)
	ERROR_KREON_ACCOUNT_LOCKED = 3405,						// 계정 잠김(로그인 실패 5번 이상시)
	ERROR_KREON_INTERNAL_ERR   = 3406,						// 크레온 내부 오류
	ERROR_KREON_INTERNAL_DB_ERR  = 3407,						// 크레온 내부 DB 에러

	ERROR_KREON_NOT_ALLOWD_IP = 3408,						// 허용되지 않은 IP
	ERROR_KREON_NO_ENOUGH_POINT = 3409,						// 잔액 부족
	ERROR_KREON_LIMIT_AMOUNT = 3410,						// 회원의 등급에 설정 되어 있는 월 결제 한도 금액을 초과 했을 경우
	ERROR_KREON_PG_LIMIT_AMOUNT = 3411,						// PG의 결제 한도 금액을 초과 했을 경우	
	ERROR_KREON_BLOCK_PCCAFE = 3412,						// 접속이 차단되어 있는 PC방입니다. 관련 문의는 겜스쿨 프리미엄 와르넷 관리자에게 문의하시기 바랍니다.

	// 중복 로그인 메세지
	ERROR_DUPLICATEUSER_INGAME = 3413,						// 다른 장소에서 로그인 되어 접속이 종료됩니다.

	// MailRU - Auth
	ERROR_MAILRU_INVALID_REQUEST = 3500,					// 잘못된 요청
	ERROR_MAILRU_WORNG_PASSWORD = 3501,						// ID/OTP 해시가 틀림(인증실패)
	ERROR_MAILRU_BLOCK = 3502,								// 밴 당한 유저
	ERROR_MAILRU_ERROR = 3503,								// AUth Error	

	// EU - Auth
	ERROR_EUAUTH_SPECIALCHAR = ERROR_ASIASOFT_SPECIAL_CHAR,	// ERROR_ASIASOFT_SPECIAL_CHAR = 3018,							// 특수문자는 사용할 수 없습니다. - 어차피 다 같은 UISTRING 아시아소프트꺼 갔다 씁니다.

	// 알테이아 대륙탐험 메세지
	ERROR_ALTEIAWORLD_ENTERLIMITTIME = 3600,				// 진입불가 시간
	ERROR_ALTEIAWORLD_NOTEXISTUSER = 3601,					// 유저가 없습니다.

	// 탈리스만 메세지
	ERROR_TALISMAN_FAIL = 3700,								// 탈리스만 에러
	ERROR_TALISMAN_FAIL_LOACTE = 3701,						// 탈리스만 에러, 해당위치에서 사용할 수 없음
	ERROR_TALISMAN_DUPLICATE = 3702,						// 같은 종류 탈리스만 중복 장착 오류
	ERROR_TALISMAN_EQUIP_FAIL = 3703,						// 탈리스만 장착 실패
	ERROR_TALISMAN_SLOT_ALERADY_OPEN = 3704,				// 슬롯이 이미 열려있음
	ERROR_TALISMAN_SLOT_NOT_OPEND = 3705,					// 탈리스만 슬롯이 오픈되지 않았음
	ERROR_TALISMAN_SLOT_NOT_SERVICE = 3706,					// 서비스 되지 않는 슬롯임
	ERROR_TALISMAN_SLOT_LIMITLEVEL = 3707,					// 캐릭레벨이 낮아 슬롯에 넣을 수 없다
	ERROR_TALISMAN_LIMITLEVEL = 3708,						// 캐릭레벨이 낮아 장착할 수 없다

	// DWC 메세지
	ERROR_DWC_FAIL = 3800,									// DWC 에러
	ERROR_DWC_PERMIT_FAIL = 3801,							// DWC 권한이 없음
	ERROR_DWC_ALREADY_HASTEAM = 3802,						// 이미 팀에 가입되어 있음
	ERROR_DWC_PROHIBITWORD = 3803,							// 사용할 수 없는 이름
	ERROR_DWC_CANT_INVITE_YOURESELF = 3804,					// 스스로를 초대할 수 없음
	ERROR_DWC_DUPLICATION_JOB = 3805,						// 이미 같은 직업이 팀에 존재	
	ERROR_DWC_CANTADD_TEAMMEMBER = 3806,					// 더이상 초대할 수 없음	
	ERROR_DWC_CANTACCEPT_TEAMINVITATION = 3807,				// 대상이 현재 팀 초대를 받을 수 없음
	ERROR_DWC_CANTACCEPT_JOINTEAM = 3808,					// 팀장이 가입을 받을 수 없는상태
	ERROR_DWC_REFUESED_TEAMINVITATION = 3809,				// 팀 초대를 거절했음
	ERROR_DWC_NOT_DWC_CHARACTER = 3810,						// 대상이 존재하지 않거나 DWC 케릭터가 아닙니다.
	ERROR_DWC_HAVE_NOT_TEAM = 3811,							// 팀에 소속되어 있지 않음
	ERROR_DWC_HAS_NO_AUTHORITY = 3812,						// 명령 권한이 없음
	ERROR_DWC_NOTEXIST_TEAMINFO = 3813,						// 팀정보가 존재하지 않음
	ERROR_DWC_NOT_SAME_TEAM = 3814,							// 같은 팀이 아닙니다.	
	ERROR_DWC_LADDER_MATCH_CLOSED = 3815,					// 매칭 가능한 시간이 아닙니다.
	ERROR_DWC_CLOSED = 3816,								// DWC 기간이 아닙니다.
	ERROR_DWC_LIMIT = 3817,									// DWC 케릭터는 사용할 수 없습니다.
	ERROR_DWC_CANT_LEAVE_TEAM = 3818,						// 팀원이 플레이 중일때 팀을 탈퇴하거나 해체할 수 없음

	// Seed 메시지
	ERROR_SEED_MAXPOINT = 3900,								// 더 이상 시드를 보유할 수 없습니다.
	ERROR_SEED_INSUFFICIENCY_SEEDPOINT = 3901,				// 시드포인트부족	
	ERROR_CASHSHOP_SEED_DISABLE = 3902,						// 시드로 살 수 없는 아이템입니다.

	//etc
	// Error메세지
	ERROR_NOT_AVAILABLE_CHECK_RANKING_TIME = 7500,			// 초기화 시간 동안에는 랭킹 정보를 확인할 수 없음(다크레어)
	ERROR_FARM_ENTERANCE_CONDITION = 7531,					// 농장구역 입장조건을 만족하지 못했습니다. 다시 한번 입장조건을 확인해주세요.		

	//PrivateChatChannel 
	ERROR_ALREADY_CHANNELNAME = 7810,						// 이미 존재하는 채널 이름입니다.
	ERROR_INVALID_CHANNELNAME = 7811,						// 잘못된 형식의 채널이름입니다.
	ERROR_INVALID_CHANNELNAMELENGTH = 7812,					// 2-10자 이내의 채널 제목을 입력해주세요
	ERROR_DUPLICATE_CHANNELJOIN = 7813,						// 1개 이상의 채널에 참여할 수 없습니다.
	ERROR_NOTEXIST_CHANNELNAME = 7815,						// 존재하지 않는 채널 이름입니다.
	ERROR_INVALID_PRIVATECHATCHANNELPASSWORD = 7819,		// 잘못된 비밀번호를 입력하셨습니다.
	ERROR_NOTEXIST_INVITEUSER = 7820,						// 초대할 유저가 존재하지 않습니다.
	ERROR_ALREADY_JOINCHANNEL = 7821,						// 이미 채널에 참여하고 있습니다.
	ERROR_INVALID_PIRVATECHATMEMBERNAME = 7824,				// 추방할 유저가 존재하지 않습니다.
	ERROR_NOTEXIST_CHANNELMEMBER = 7825,					// 방장으로 위임할 유저가 존재하지 않습니다.
	ERROR_MAX_CHANNELMEMBER = 7828,							// 채널의 제한인원을 초과하였습니다.

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	// PvP - ComboExerciseMode
	ERROR_CANT_USE_RECALLMONSTER = 7938,					// 이동 중에는 사용할 수 없습니다.
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
#if defined(PRE_ADD_EQUIPLOCK)
	ERROR_ITEM_LOCKITEM_NOTUPGRADE = 8388,				// 잠금 상태에서는 강화가 불가능합니다.
	ERROR_ITEM_LOCKITEM_NOTMOVE = 8389,					// 잠금 상태에서는 장비 변경이 불가능 합니다. 잠금을 해제한 후 시도해주세요
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	// 꽉차서 끼어넣기 애매한거...
	ERROR_GIFT_COUNTOVER_GLYPH	= 10000,				// (문장)더이상 받을 수 없다
	ERROR_GIFT_COUNTOVER_FARMEXTEND = 10001,			// (농지확장)더이상 받을 수 없다.
	ERROR_CANT_USE_ITEM_DAYILY = 10002,						// 피로도아이템 횟수체크
	ERROR_ITEM_RANDOMMIX_FAIL_MONEY = 10003,			// 수수료가 부족하여 합성할 수 없습니다.
	ERROR_ITEM_RANDOMMIX_FAIL_MIXER = 10004,			// 올바른 합성기가 사용되지 않아 합성할 수 없습니다.
#if defined( PRE_ADD_PVP_EXPUP_ITEM )
	ERROR_CANT_USE_PVPEXPUPITEM_DAILY = 10005,			// 콜로세움 경험치 증가 아이템 일일 사용 횟수 초과
	ERROR_CANT_USE_PVPEXPUPITEM_LEVEL = 10006,			// 콜로세움 경험치 증가 아이템 계급 미달
#endif // #if defined( PRE_ADD_PVP_EXPUP_ITEM )
	ERROR_GIFT_COUNTOVER_TALISMAN	= 10007,			// (탈리스만)더이상 받을 수 없다

	// 디비 에러값 영역 100000 ~ 199999 (이건 쓰지마세요)
	ERROR_SECONDAUTH_CHECK_MAXFAILED	= 101125,		// 연속적으로 최대설정 수치 이상 틀려서 에러.
	ERROR_SECONDAUTH_CHECK_OLDPWINVALID = 101126,		// 기존 2차 인증 비밀번호가 일치하지 않습니다.
	ERROR_SECONDAUTH_CHECK_FAILED		= 101127,		// 2차 인증 비밀번호가 일치하지 않습니다.
	ERROR_SECONDAUTH_NOTEXIST			= 101128,		// 2차 인증 비밀번호가 설정되어 있지 않습니다.
	ERROR_DB_CHANGENAME_NEXTDAY = 101191,				// 캐릭터 이름을 다시 변경하시려면 7일간 기다려야 합니다.
	ERROR_DB_RESTRICTED_IP = 101194,					// 아이피당 접속제한 갯수초과
	ERROR_IS_NOT_DOORS_ACCOUNT = 101115,				// 도어스인증되어진 계정이 아님
	ERROR_ALREADY_EVENTREWARDRECEIVER = 101203,			// 이미 이벤트 보상을 받은 캐릭터입니다.
	ERROR_ALTEIAWORLD_AGAINSENDTICKET = 103369,			// 해당 캐릭터에겐 이미 보물지도 조각을 보냈습니다
	ERROR_ALTEIAWORLD_NOTEXIST_SENDCHARACTER = 103370,	// 보내는 캐릭터가 존재하지 않습니다.
	ERROR_ALTEIAWORLD_NOTEXIST_RECVCHARACTER = 103371,	// 받을 캐릭터가 존재하지 않습니다.
	ERROR_ALTEIAWORLD_SENDTICKET_MAXCOUNT = 103372,	// 보물지도를 보낼수 있는 최대수량이 넘었습니다.

	ERROR_DB_DWC_PERMIT_FAIL = 103357,					// DWC 케릭터가 아니거나 존재하지 않음
	ERROR_DB_DWC_NAME_ALREADYEXIST = 103358,			// 이미 존재하는 DWC 팀명
	ERROR_DB_DWC_ALREADY_HASTEAM = 103359,				// 이미 팀에 가입되어 있음
	ERROR_DB_DWC_USER_NOT_FOUND = 103360,				// 존재하지 않는 케릭터
	ERROR_DB_DWC_TEAM_NOT_FOUND = 103361,				// 존재하지 않는 팀
	ERROR_DB_DWC_DEL_AFTER_LEAVE_DWCTEAM = 103376,		// DWC 팀에 가입되어있는 케릭터는 삭제할 수 없음
};

enum eServerMsgIndex
{
	SERVERMSG_INDEX125	=	125,							// 피로도를 다 사용해서 길드 축제 미션은 더 이상 완료되지 않습니다
	SERVERMSG_INDEX626  =   626,							// 레벨 업 보상 상자를 획득 했습니다. 캐시 선물함을 확인하세요.
	SERVERMSG_INDEX6211 =   6211,							// PC방 대여 아이템이 제거되었습니다. 장착하고 있었던 경우, 장비 아이템이 사라진 상태가 되오니, 꼭 확인해 주세요.	
	SERVERMSG_INDEX7884 =   7884,							// 용자님의 귀환을 환영합니다.지금 바로 'I'키를 눌러 인벤트리 창을 열어서 캐시 탭을 확인해 보세요!
};

enum eAccountLevel
{
	AccountLevel_New = 10,			// 신입
	AccountLevel_Intern = 15,		// 인턴
	AccountLevel_Monitoring = 20,	// 모니터링
	AccountLevel_Master = 30,		// 마스터	
	AccountLevel_QA = 99,			// QA
	AccountLevel_Developer = 100,	// 개발자
	AccountLevel_DWC = 110,			// DWC 대회용 계정
};

enum eAccountCmdAuth //int형..bit연산
{	
	CmdAuth_New		   = 1,
	CmdAuth_Intern	   = 2,
	CmdAuth_Monitoring = 4,
	CmdAuth_Master	   = 8,
	CmdAuth_QA		   = 16,
	CmdAuth_Developer  = 32,	
};

//party
enum ePartyRefreshSubject
{
	PARTYREFRESH_NONE,						//딱히 표기 및 알림이 필요 없는 경우 (예:게임서버에 있다가 빌리지에 재진입시, 파티생성시 등)
	PARTYREFRESH_CHANGE_LEADER,
	PARTYREFRESH_MODIFY_INFO,
	PARTYREFRESH_RETURNVILLAGE,				//빌리지로 돌아올경우
};

enum eMapType
{
	MAPTYPE_VILLAGE,
	MAPTYPE_FIELD,
};

enum eDungeonClearType
{
	DUNGEONCLEAR_F = 0,
	DUNGEONCLEAR_D,
	DUNGEONCLEAR_C,
	DUNGEONCLEAR_B,
	DUNGEONCLEAR_A,
	DUNGEONCLEAR_S,
	DUNGEONCLEAR_SS,
	DUNGEONCLEAR_SSS,
};

enum eItemMoveType
{
	MoveType_Equip = 1,					// EQUIP <-> EQUIP
	MoveType_Inven = 2,					// INVEN <-> INVEN
	MoveType_Ware = 3,					// WARE <-> WARE
	MoveType_QuestInven = 4,			// QUEST INVEN <-> QUEST INVEN
	MoveType_EquipToInven = 5,			// EQUIP -> INVEN
	MoveType_InvenToEquip = 6,			// INVEN -> EQUIP
	MoveType_InvenToWare = 7,			// INVEN -> WARE
	MoveType_WareToInven = 8,			// WARE -> INVEN
	MoveType_Pickup = 9,				// 줍기
	MoveType_Drop = 10,					// 휴지통에 버리기
	MoveType_Glyph = 11,				// GLYPH <-> GLYPH
	MoveType_GlyphToInven = 12,			// GLYPH -> INVEN
	MoveType_InvenToGlyph = 13,			// INVEN -> GLYPH

	// 길드창고
	MoveType_GuildWare = 14,			// GUILDWARE <-> GUILDWARE
	MoveType_InvenToGuildWare = 15,		// INVEN -> GUILDWARE
	MoveType_GuildWareToInven = 16,		// GUILDWARE -> INVEN
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	MoveType_MailToGuildWare = 17,		// GuildMail -> GuildWare
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	// 서버 창고
	MoveType_InvenToServerWare = 18,	// Inven -> ServerWare
	MoveType_ServerWareToInven = 19,	// ServerWare -> Inven
	MoveType_CashToServerWare = 20,		// Cash -> ServerWare
	MoveType_ServerWareToCash = 21,		// ServerWare -> Cash

#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)

	// 캐쉬쪽
	MoveType_CashEquip = 30,			// EQUIP <-> EQUIP
	MoveType_CashInven = 31,			// INVEN <-> INVEN
	MoveType_CashEquipToCashInven = 32,	// EQUIP -> INVEN
	MoveType_CashInvenToCashEquip = 33,	// INVEN -> EQUIP
	MoveType_CashGlyph = 34,			// GLYPH <-> GLYPH
	MoveType_CashGlyphToCashInven = 35,	// GLYPH -> INVEN
	MoveType_CashInvenToCashGlyph = 36,	// INVEN -> GLYPH

	// 탈것
	MoveType_VehicleBodyToVehicleInven = 40,	// VehicleBody <-> VehicleInven
	MoveType_VehicleInvenToVehicleBody = 41,	// VehicleInven <-> VehicleBody
	MoveType_VehiclePartsToCashInven = 42,		// VehicleParts <-> CashInven
	MoveType_CashInvenToVehicleParts = 43,		// CashInven <-> VehicleParts

	// 펫
	MoveType_PetBodyToPetInven = 50,	// PetBody <-> PetInven
	MoveType_PetInvenToPetBody = 51,	// PetInven <-> PetBody
	MoveType_PetPartsToCashInven = 52,		// PetParts <-> CashInven
	MoveType_CashInvenToPetParts = 53,		// CashInven <-> PetParts

	// 탈리스만
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	MoveType_Talisman = 60,			// TALISMAN <-> TALISMAN
	MoveType_TalismanToInven = 61,	// TALISMAN -> INVEN	
	MoveType_InvenToTalisman = 62,	// INVEN -> TALISMAN	
#endif

	// 인벤에 새롭게 아이템 들어오는거 타입 (여기다 꼽사리..)
	CREATEINVEN_PICKUP = 1,			// 주워서 들어오는 경우
	CREATEINVEN_QUEST,				// 퀘스트 스크립트를 통해서 들어오는 경우
	CREATEINVEN_ETC,				// 위의 경우 이외에 나머지 경우
	CREATEINVEN_PICKUP_DICE,		// 귀속아이템 주사위 굴려서 얻는 경우
	CREATEINVEN_REPURCHASE,			// 재구매
};

enum eEquipType
{
	EQUIP_FACE,				// 얼굴
	EQUIP_HAIR,				// 머리
	EQUIP_HELMET,			// 헬멧
	EQUIP_BODY,				// 상의
	EQUIP_LEG,				// 하의
	EQUIP_HAND,				// 손
	EQUIP_FOOT,				// 발
	EQUIP_NECKLACE,
	EQUIP_EARRING,
	EQUIP_RING1,
	EQUIP_RING2,
	EQUIP_WEAPON1,			// 무기
	EQUIP_WEAPON2,

	EQUIPMAX,			// 일반아이템 장비용
};

enum eCashEquipType
{
	CASHEQUIPMIN,
	CASHEQUIP_HELMET = CASHEQUIPMIN, // 헬멧(투구)
	CASHEQUIP_BODY,				// 상의
	CASHEQUIP_LEG,				// 하의
	CASHEQUIP_HAND,				// 손(장갑)
	CASHEQUIP_FOOT,				// 발(신발)
	CASHEQUIP_NECKLACE,			// 목걸이
	CASHEQUIP_EARRING,			// 귀걸이
	CASHEQUIP_RING1,			// 반지
	CASHEQUIP_RING2,			// 반지
	CASHEQUIP_WING,				// 날개/
	CASHEQUIP_TAIL,				// 꼬리
	CASHEQUIP_FACEDECO,			// 얼굴장식
	CASHEQUIP_FAIRY,			// 정령
	CASHEQUIP_WEAPON1,			// 무기
	CASHEQUIP_WEAPON2,
	CASHEQUIP_EFFECT,			// 공통 이펙트 (탈것, 펫 등등...)
	CASHEQUIPMAX,

	HIDEHELMET_BITINDEX = 31,	// 캐시장비 전환 마지막 비트플래그(VIEWCASHEQUIPMAX_BITSIZE * 8 - 1)
};

enum eItemRank
{
	ITEMRANK_D,		// NORMAL
	ITEMRANK_C,		// MAGIC
	ITEMRANK_B,		// RARE
	ITEMRANK_A,		// EPIC
	ITEMRANK_S,		// UNIQUE
	ITEMRANK_SS,	// HEROIC
	ITEMRANK_SSS,	// NAMED
	ITEMRANK_MAX,
	ITEMRANK_NONE,	// Note : 파티 루팅 구분 용도로만 쓰임.
};

enum eItemPositionType
{
	ITEMPOSITION_NONE,
	ITEMPOSITION_EQUIP = 1,			// 장비창
	ITEMPOSITION_INVEN,				// 인벤창
	ITEMPOSITION_WARE,				// 창고창
	ITEMPOSITION_QUESTINVEN,		// 퀘스트 인벤창
	// Cash관련
	ITEMPOSITION_CASHEQUIP,			// 장비창(캐쉬)
	ITEMPOSITION_CASHINVEN,			// 인벤창(캐쉬)

	ITEMPOSITION_CASHREMOVE,        // 인벤창(캐쉬삭제)

	ITEMPOSITION_GLYPH,				// 문장
	ITEMPOSITION_VEHICLE,			// 탈것인벤
	ITEMPOSITION_SERVERWARE,		// 서버창고
	ITEMPOSITION_SERVERWARECASH,	// 서버캐쉬창고

	ITEMPOSITION_TALISMAN,			// 탈리스만

	ITEMPOSITION_GUILD = 1,			// 길드창고창
};

enum eGlyphType
{
	GLYPETYPE_NONE = 0,
	GLYPETYPE_ENCHANT = 1,
	GLYPETYPE_SKILL = 2,
	GLYPETYPE_SPECIALSKILL = 3,
	GLYPHTYPE_ALL = 4,
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	GLYPHTYPE_DRAGON = 5,			// 드래곤 원정대
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
};

enum eGlyph
{
	GLYPH_ENCHANT1,			// 문장 - 강화
	GLYPH_ENCHANT2,
	GLYPH_ENCHANT3,
	GLYPH_ENCHANT4,
	GLYPH_ENCHANT5,
	GLYPH_ENCHANT6,
	GLYPH_ENCHANT7,
	GLYPH_ENCHANT8,
	GLYPH_SKILL1,			// 문장 - 스킬
	GLYPH_SKILL2,
	GLYPH_SKILL3,
	GLYPH_SKILL4,
	GLYPH_SPECIALSKILL,		// 문장 - 특수스킬
	GLYPH_CASH1,			// 문장 - 캐쉬
	GLYPH_CASH2,
	GLYPH_CASH3,
	// GLYPHMAX 와 CASH 사이에 다른거 들어오면 안됩니다. -- 김밥 --	
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	GLYPH_DRAGON1,
	GLYPH_DRAGON2,
	GLYPH_DRAGON3,
	GLYPH_DRAGON4,
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	GLYPHMAX,
};

#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
const int GLYPH_CASH_TOTAL_MAX = 3;
const int GLYPH_DRAGON_TOTAL_MAX = 4;
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)

enum eTalisman
{
	TALISMAN_CASH1 = 0,
	TALISMAN_CASH2 = 1,
	TALISMAN_CASH3 = 2,
	TALISMAN_CASH4 = 3,
	TALISMAN_CASH_MAX = TALISMAN_CASH4,

	TALISMAN_BASIC_START = 4,
	TALISMAN_BASIC_END = 11,

	TALISMAN_EXPANSION_START = 12,

	TALISMAN_MAX = 28,

	// 확장슬롯을 32개 이상 확장하려면 CDNUserItem::m_nTalismanOpenFlag 의 자료구조형식을 변경해야 합니다.
	TALISMAN_EXPANSION_SLOT_LIMIT = 32,
};

enum eTalismanSlotType
{
	TALISMAN_BASIC = 0, 
	TALISMAN_EXPANSION = 1,
	TALISMAN_CASH_EXPANSION = 2,
};

enum ePartyItemLootRule
{
	ITEMLOOTRULE_NONE = 0,
	ITEMLOOTRULE_RANDOM,
	ITEMLOOTRULE_LEADER,
	ITEMLOOTRULE_INORDER,
	ITEMLOOTRULE_OWNER,
	ITEMLOOTRULE_MAX,
};

enum eChatType
{
	CHATTYPE_NORMAL = 1,	// 일반적인 대화
	CHATTYPE_PARTY,			// 파티 대화
	CHATTYPE_PRIVATE,		// 귓속말
	CHATTYPE_GUILD,			// 길드 대화 - 길드패킷은 구조가 달라서 SCChat구조체 대신 S
	CHATTYPE_CHANNEL,		// 채널 대화(거래채널등은 추가가 될까요?)
	CHATTYPE_SYSTEM,		// 시스템
	CHATTYPE_WORLD,			// 월드 대화
	CHATTYPE_NOTICE,		// 공지
	CHATTYPE_TEAMCHAT,		// 팀채팅

	//CHATTYPE_NOTICEZONE,	// 맵공지
	CHATTYPE_GM,			// 운영자 대화
	CHATTYPE_PARTY_GM,		// 운영자 파티 대화
	CHATTYPE_PRIVATE_GM,	// 운영자 귓말
	CHATTYPE_DICE,
	CHATTYPE_CHATROOM,		// 채팅방 메시지
	CHATTYPE_RAIDNOTICE,	// 공격대 공지
	CHATTYPE_BLIND,			// 블라인드 보내는 메세지
	CHATTYPE_GUILDWAR,		// 길드전 모드 내에서 쓰이는 메세지
	CHATTYPE_PRIVATE_MOBILE,// 모바일 귓말	
	CHATTYPE_PRIVATECHANNEL,// 사설채널 대화
	CHATTYPE_GUILD_MOBILE,	// 모바일 길드 대회

	//CHATTYPE_DWCZONE
	CHATTYPE_DWC_PRIVATE,	// DWC 케릭간 귓속말 -> 서버 사이드에서만 사용합니다. 클라이언트에겐 CHATTYPE_PRIVATE 으로 전송
	CHATTYPE_DWC_TEAM,		// DWC 팀 채팅

	CHATTYPE_WORLD_POPMSG,	// 월드 대화(전령 붉은새)

	// 전체적인 시스템 메시지
	WORLDCHATTYPE_ENCHANTITEM = 1,	// 강화아이템
	WORLDCHATTYPE_MISSION,			// 미션
	WORLDCHATTYPE_CHARMITEM,		// 매력아이템
	WORLDCHATTYPE_DONATION,
	WORLDCHATTYPE_NAMEDITEM,		// 네임드아이템획득메시지
	WORLDCHATTYPE_CHAOSITEM,		// 카오스아이템
	WORLDCHATTYPE_BESTFRIEND,		// 절친 알림
	WORLDCHATTYPE_CHOICEITEM,		// 초이스아이템
	WORLDCHATTYPE_DIRECTPARTYBUFF,	// 파티원버프아이템(파티원들에게만 전송)
	WORLDCHATTYPE_COSMIX,			// 코스튬합성
	WORLDCHATTYPE_COSMIX888,			// 코스튬합성
};

enum eQuickSlotType
{
	QUICKSLOTTYPE_NONE,	// none
	QUICKSLOTTYPE_ITEM,	// item
	QUICKSLOTTYPE_SKILL,	// skill
	QUICKSLOTTYPE_GESTURE,	// Gesture
	QUICKSLOTTYPE_SECONDARYSKILL,	// SecondarySkill(낚시, 요리)
	QUICKSLOTTYPE_VEHICLE_PET,
};

struct MailType
{
	enum ReadState
	{
		NoRead = 0,		// 읽지않은상태
		Read = 1,		// 읽은상태
	};

	enum Attach
	{
		AttachItem = 1,	// 아이템 첨부상태
	};

	enum Delivery
	{
		Normal = 1,		// 일반편지
		Premium = 2,	// 프리미엄
		ReplyGift = 3,	// 선물회신
	};
	
	enum Kind
	{
		NormalMail				= 1,	// 일반편지
		MissionMail				= 2,	// 미션편지
		AdminMail				= 3,	// 관리자 메일
		EventMail				= 4,	// 이벤트 메일
		VIPMail					= 5,	// VIP
		MasterSystemMail		= 6,	// 사제시스템
		NpcReputationMail		= 7,	// NPC호감도시스템
		Quest					= 8,	// Quest
		GuildWarReward			= 9,	// 길드전 보상 우편
		SpecialBox_Account		= 10,	// 특수보관함_계정타겟 
		SpecialBox_Character	= 11,	// 특수보관함_현캐릭터타겟
		Cadge					= 18,	// 조르기
		GuildMaxLevelReward		= 19,	// 최초길드가입 만랩 길드 보상
		AppellationBookReward	= 20,	// 칭호컬렉션북 완료 보상
	};
};

enum eSortType
{
	MARKETSORT_ITEM_DESC = 0,	// 아이템 내림차순
	MARKETSORT_ITEM_ASC,		// 아이템 오름차순
	MARKETSORT_LEVEL_DESC,		// 레벨 내림차순
	MARKETSORT_LEVEL_ASC,		// 레벨 오름차순
	MARKETSORT_PRICE_DESC,		// 가격 내림차순
	MARKETSORT_PRICE_ASC,		// 가격 오름차순
	MARKETSORT_ONEPRICE_DESC,	// 개당 가격 내림차순
	MARKETSORT_ONEPRICE_ASC,	// 개당 가격 오름차순
	MARKETSORT_ITEMGRADE_DESC,	// 강화 내림차순
	MARKETSORT_ITEMGRADE_ASC,	// 강화 오름차순
};

// Quest관련
// TCount
enum EnumCountingType
{
	CountingType_None	= 0,
	CountingType_MyMonsterKill = 1,
	CountingType_AllMonsterKill = 2,
	CountingType_Item	= 3,
	CountingType_SymbolItem = 4,
	CountingType_StageClear = 5,
	CountingType_Max
};

// TQuest
enum EnumQuestType
{
	QuestType_Min = 0,
	QuestType_SubQuest = 1,
	QuestType_MainQuest = 2,
	QuestType_PeriodQuest = 3,
	QuestType_RemoteQuest = 4,
	QuestType_Max,
};

enum EnumRemoteQuestType
{
	RemoteQuestType_Normal = 0,
	RemoteQuestType_Meta = 1,
};

enum EnumQuestState
{
	QuestState_None = 0,
	QuestState_Playing = 1,
	QuestState_Recompense = 2	//원격퀘스트의 완료 플래그로만 사용중 - 2012-09-11 by stupidfox.
};

enum EnumPeriodQuestResetType
{
	PERIODQUEST_RESET_EVENT = 0,	// 이벤트
	PERIODQUEST_RESET_DAILY = 1,	// 일일
	PERIODQUEST_RESET_WORLD = 2,	// 월드
	PERIODQUEST_RESET_WEEKLY = 7,	// 주간
	PERIODQUEST_RESET_MAX,
};

enum EnumPeriodQuestTime
{
	PERIODQUEST_TIME_START = 0,
	PERIODQUEST_TIME_END = 1,
	PERIODQUEST_TIME_MAX = 2,
};

enum EnumQuestRecompenseType
{
	QuestRecompense_None = 0,
	QuestRecompense_All = 1,
	QuestRecompense_Select = 2
};

enum EnumExtraType
{
	ExtraType_None = 0,
	ExtraType_Counting = 1,
};

enum eDailyMissionType		// Daily Mission
{
	DAILYMISSION_DAY = 1,		// 일일 미션
	DAILYMISSION_WEEK = 2,		// 주간 미션
	DAILYMISSION_GUILDWAR = 3,	// 길드전 미션
	DAILYMISSION_PCBANG = 4,	// 피씨방 미션
	DAILYMISSION_GUILDCOMMON = 5, // 길드일반 미션
	DAILYMISSION_WEEKENDEVENT = 6,	// 주말 이벤트 미션
	DAILYMISSION_WEEKENDREPEAT = 7, // 반복미션
#if defined( PRE_ADD_MONTHLY_MISSION )
	DAILYMISSION_MONTH = 8,	//월간 미션
#endif	// #if defined( PRE_ADD_MONTHLY_MISSION )
};

// 중국 피로도
enum eFCMState
{
	// 피로도 상태값
	FCMSTATE_NONE = 0,
	FCMSTATE_HALF = 1,		// 50% (3 - 5시간, 30분마다 알림)
	FCMSTATE_ZERO = 2,		// 0% (5시간 이상, 15분마다 알림)
};


enum eExchangeButton
{
	EXCHANGE_REGIST = 1,	// 등록
	EXCHANGE_CONFIRM,		// 확인
};

enum eLeaveType
{
	LEAVE_LOGOUT,		// 로그아웃
	LEAVE_MOVE,				// 이동
};

// 2차인증
namespace SecondAuth
{
	struct Common
	{
		enum
		{
#if defined( _JP )
			LimitCount			= 3,		// 2차 인증 연속 실패 기준 값
#else
			LimitCount			= 10,		// 2차 인증 연속 실패 기준 값
#endif // #if defined( _JP )
			PWMaxLength			= 4,		// 2차 비밀번호 최대 길이
			NotifyDate			= 3,		// 2차 인증 알림 기준 일자
			RestraintReasonID	= 5,
#if defined( _JP )
			RestraintTimeMin	= 30,
#else
			RestraintTimeMin	= 10,
#endif // #if defined( _JP )
		};
	};

	struct DBMsg
	{
		enum
		{
			ShowDialog		= 101139,	// 2차 인증 재알림 금지 기간이 지나서 메세지 보여준다.
		};
	};
}

// PvP
namespace PvPCommon
{
	struct CaptainType
	{
		enum eCode
		{
			Captain = 0,
			GroupCaptain,
			Max,
		};
	};

	struct RoomOption
	{
		enum eCode
		{
			None				= 0,
			BreakInto			= 1,
			DropItem			= (1<<1),
			ShowHP				= (1<<2),
			RandomTeam			= (1<<3),
			NoRegulation		= (1<<4),
			AllKill_RandomOrder = (1<<5),
#ifdef PRE_MOD_PVPOBSERVER
			AllowObserver = (1<<6),
#endif		//#ifdef PRE_MOD_PVPOBSERVER
		};
	};

	struct RoomType
	{
		enum eRoomType
		{
			beginner,
			regular,
#if defined(PRE_ADD_DWC)
			dwc,	//dwc 일반
#endif
			max,
		};
	};

	struct Common
	{
		enum
		{
			MaxObserverPlayer				= 4,								// 옵져버 최대 인원
			ExtendMaxObserverPlayer = 16,										// 옵져버 최대 인원
			//MaxPlayer						= PARTYMAX+MaxObserverPlayer,		// 최대 인원( 옵져버 인원 추가 )
			MaxPlayer						= PARTYMAX,		// 최대 인원( 옵져버 인원 추가 )
			MaxPVPPlayer					= PARTYMAX,							// PVP게임형식의 최대인원
			MaxTeamPVPPlayer				= MaxPVPPlayer/4,					// 한팀당 최대 인원수 (여러팀이 싸우는 모드가 나올시에는 바뀔수 있음) 	
			MaxRoomCount					= 1000,								// 최대 방 개수
			RoomPerPage						= 10,								// 한 페이지 당 룸 개수
			PvPVillageStartPositionGateNo	= 1,								// PvP 마을 입장시 시작 위치 게이트번호
			PvPVillageToLobbyGateNo			= 2,								// PvP 마을->로비 이동시 게이트 번호
			RoomListRefreshGapTime			= 5000,								// PvP방목록 갱신 시간
			GameModeStartDelta				= 3,								// PvP게임모드 시작시 카운트 시간(초)
			RoundModeFinishDelta			= 10,								// PvP 1라운드 끝났을 때 다음 라운드까지 시간간격(초)
#if defined(PRE_ADD_MACHINA)
			MaxClass						= 9,
#elif defined(PRE_ADD_ASSASSIN) && defined(PRE_ADD_KALI) && defined(PRE_ADD_ACADEMIC)
			MaxClass						= 7,
#elif defined(PRE_ADD_KALI) && defined(PRE_ADD_ACADEMIC)
			MaxClass						= 6,								// PlayerActor 클래스 최대 수
#elif defined(PRE_ADD_ACADEMIC)
			MaxClass						= 5,								// PlayerActor 클래스 최대 수
#else
			MaxClass						= 4,								// PlayerActor 클래스 최대 수
#endif // #if defined( PRE_ADD_ACADEMIC )
			MaxRank							= 26,								// 최대 PvP Rank 등급
			RegulationLevel					= 99,								// 보정레벨
			NormalXPMinPlayerCount			= 6,								// 정상XP를 받기 위한 최소 플레이어 수
			PvPFinishAutoClose				= 18,								// PvP모드 종료 후 자동 닫기 버튼 시간(초)
			PvPFinishScoreOpenDelay			= 3,								// PvP모드 종료 후 Score판 열리는 딜레이(초)
			MonsterMutationColCount			= 20,								// 몬스터변신그룹컬럼카운트
#if defined( PRE_ADD_ACADEMIC )
			MonsterMutationSkillColCount	= 120,
#else
			MonsterMutationSkillColCount	= 30,
#endif // #if defined( PRE_ADD_ACADEMIC )
			GhouConditionColCount			= 6,
			RoundStartMutationTimeSec		= 10,
			DefaultGuildCount				= 2,								// 2개팀 이상의 복수팀 대전이 나와 변수 생기기전 까지 최소팀카운트
			MaximumGuildWarSecondCaptain	= 3,								// 길드전 부대장 맥스카운트
			MaximumCapturePosition			= 4,								// 점령(길드전) 포지션 맥스
			MaximumVitoryCondition			= 5,
			ReservedGuildSkillJobID			= 2000,								// 기존 스킬테이블 이용을 위해서 길드전용 스킬 잡아이디 예약 기존잡아이디는 999이상 사용안뎀 원선씨와 이야기 완료
			MaximumEffectCount				= 7,
			DoorCount						= 3,
			DamagePrintTime					= 60000,
			PvPBeginnerMedalMultipleFactor = 2,
			PvPStateAdjustmentTerm = (60 * 1000 * 10),
			PvPAbsoluteRankCountMax = 101,
#if defined(PRE_ADD_PVP_TOURNAMENT)
			PvPTournamentUserMax = 32,	// 32강이 최고임
#endif
		};
	};
#if defined(PRE_ADD_PVP_TOURNAMENT)
	struct Tournament
	{
		enum eStep
		{
			TOURNAMENT_NONE = 0,
			TOURNAMENT_FINAL = 2,
			TOURNAMENT_4 = 4,
			TOURNAMENT_8 = 8,
			TOURNAMENT_16 = 16,
			TOURNAMENT_32 = 32,
		};
	};
#endif

	struct BanType
	{
		enum eBanType
		{
			Normal = 0,				// 일반추방
			Permanence,				// 영구추방
		};
	};

	struct LeaveType
	{
		enum eLeaveType
		{
			Normal = 0,				// 일반
			Ban,					// 추방
			PermanenceBan,			// 영구추방
			DestroyRoom,			// 방파괴
		};
	};

	struct GameType
	{
		enum
		{
			PvP = 0,
			COOP,
			Max,
		};
	};

	struct GameMode
	{
		enum
		{
			PvP_Respawn				= 0,
			PvP_Round			    = 1,
			PvP_Captain				= 2,
			PvP_IndividualRespawn	= 3,
			COOP_Defense			= 6,
			COOP_Survival			= 7,
			PvP_Zombie_Survival		= 11,
			PvP_GuildWar			= 12,
			PvP_Occupation			= 13,
			PvP_AllKill             = 14,
			PvP_Racing				= 15,
#ifdef PRE_ADD_PVP_TOURNAMENT
			PvP_Tournament = 16,
#endif

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
			PvP_ComboExercise		= 17,
#endif
			Max,
		};
	};
	
	struct RoomSortType
	{
		enum
		{
			Index		= 0,		// 방번호 정렬
			GameMode,				// 게임모드 정렬
			MapIndex,				// 맵인덱스 정렬
			NumOfPlayer,			// 인원수 정렬
			Max,
		};
	};

	struct WaitUserList
	{
		struct Common
		{
			enum
			{
				MaxPerPage = 12,
			};
		};

		struct SortType
		{
			enum eSortType
			{
				Null		= 0,
				PvPLevel,
				Job,
				Level,
				Name,
				MAX,
			};
		};

		struct WaitUserInfo
		{
			BYTE	cPvPLevel;					// PvP 등급
			BYTE	cJob;						// 직업
			BYTE	cLevel;						// 레벨
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
			BYTE cType;							//채널타입
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
			WCHAR	wszCharName[NAMELENMAX];	// 이름
		};

		struct Repository
		{
			WaitUserInfo	WaitUserList[Common::MaxPerPage];
		};
	};

	struct ItemUsageType
	{
		enum
		{
			PvPItemOnly = 0,
			Max,
		};
	};

	struct RoomState
	{
		enum
		{
			None				= 0,				// 아무 상태 아님
			Password			= 1,				// 비밀방
			Starting			= (1<<1),			// 시작중
			Syncing				= (1<<2),			// 동기화중
			Playing				= (1<<3),			// 플레이중
			Finished			= (1<<4),			// 게임모드종료
			NoMoreBreakInto		= (1<<5),			// 더이상 난입 못함
			// Mask
			CantBanMask			= (Starting|Syncing|Playing|Finished|NoMoreBreakInto),	// 추방 할 수 없는 RoomState
			CantModifyRoom		= (Starting|Syncing|Playing|Finished|NoMoreBreakInto),	// 방편집 할 수 없는 RoomState
		};
	};

	struct UserState
	{
		enum
		{
			None		= 0,		// 아무 상태 아님
			Captain		= 1,		// 방장
			Ready		= (1<<1),	// 레뒤
			Starting	= (1<<2),	// 시작중
			Syncing		= (1<<3),	// 동기화중
			Playing		= (1<<4),	// 게임중.
			GuildWarCaptain			= (1<<5),	// 대장
			GuildWarSedcondCaptain	= (1<<6),	// 부장.
			GroupCaptain = (1<<7),	// 조장
			// Mask
			CantBanMask	= (Starting|Syncing|Playing),	// 추방 할 수 없는 UserState
			CantLeaveMask	= (Ready|Starting|Syncing|Playing),	// 나갈 수 없는 UserState
		};
	};

	struct TxtMax
	{
		enum
		{
			RoomName	= 32,
			RoomPW		= 4,
		};
	};

	struct Team
	{
		enum eTeam
		{
			StartIndex	= 1000,
			A			= 1000,
			B			= 1001,
			EndIndex	= 1001,
			Others,
			Observer	= StartIndex+PARTYMAX+100,
			Human		= A,
			Zombie		= B,
			Max,
		};

		struct TPvPTeamIndex
		{
			UINT nTeam;
			UINT nAccountDBID[PARTYMAX];
		};

		struct  TPvPGuildWarInfo
		{
			USHORT nTeam;
			UINT nGuildDBID;
			int nQualifyingScore;
		};
	};

	struct GAMA_Command
	{
		enum
		{
			DeleteGameRoom = 0,
			FinishGameMode,
			NoMoreBreakInto,
			Max,
		};
	};

	// PvPCommon::TeamIndex 는 PvPCommon::Team 을 배열로 갖고 있을 때 접근하기 위한 배열인덱스이다. PvPCommon::Team 이랑 혼동되지 않게 주의하자.
	struct TeamIndex
	{
		enum eCode
		{
			A	= 0,
			B,
			Max,
		};
	};

	struct Check
	{
		enum
		{
			None						= 0,			
			CheckStartConditionTable	= 1,			// PvPStartConditonTable 체크
			CheckTimeOver				= (1<<1),		// TimeOver 체크
			CheckTeamMemberZero			= (1<<2),		// 한팀 다 나갔는지 체크
			CheckScore					= (1<<3),		// 스코어 체크
			AllCheck					= 0xFFFF,		// 모두 체크
		};
	};

	struct FinishReason
	{
		enum eCode
		{
			AchieveWinCondition			= 0,			// 승리조건 달성
			TimeOver,									// 시간종료
			OpponentTeamAllGone,						// 상대팀 모두 나감
			OpponentTeamAllDead,						// 상대팀 모두 죽음
			OpponentCaptainGone,						// 상대팀 대장 나감
			OpponentCaptainDead,						// 상대팀 대장 죽음
			LadderDraw,									// 래더 무승부
			LadderNoGame,
			OpponentTeamBossDead,						// 상대팀 보스몹 죽음
			OpponentTeamScoreLimit,						// 상대팀 스코어리밋 도달
			OpponentTeamResourceLimit,					// 상대팀 리소스리밋 도달
			OpponentTeamQualifierScore,					// ???????????
			OpponentTeamForceWin,
#if defined( PRE_ADD_RACING_MODE )			
			RacingAllFinish,							// 모든 유저가 꼴인함.
#endif // #if defined( PRE_ADD_RACING_MODE )
			Max,
		};
	};

	struct FinishDetailReason
	{
		enum eCode
		{
			None = 0,
			SuperiorHPWin,			// HP 우세승
			SuperiorContinuousWin,	// 연승 우세승
			HPnContinuousWinDraw,	// HP,연승 동률 무승부
		};
	};

	struct BattleGroundVictoryState
	{
		enum eBattleGroundVictoryState
		{
			None = 0,					//아앙~
			BossKill = 1,				//보스몬스터킬
			Score = (1<<1),				//점수(킬카운트 등)
			Resource = (1<<2),			//자원획득
			QualifierScore = (1<<3),	//예선전 점수로 ???
		};
	};

	struct OccupationSystemState
	{
		enum eOccupationSystemState
		{
			None,
			Wait,
			Play,
			End,
		};
	};

	struct OccupationState
	{
		enum eOccupationState
		{
			None = 0,			//아무소유아님
			Own = 1,			//소유중
			Try = (1<<1),		//소유시도중
			MoveWait = (1<<2),	//소유이동대기
			CantTryMask	= (Try),	// 나갈 수 없는 UserState
		};
	};

	struct OccupantType
	{
		enum eOccupantType
		{
			Actor = 0,
			Team,
		};
	};

	struct OccupationVictoryCondition
	{
		enum eOccupationVictoryCondition
		{
			None,
			BossKill = 1,
			ScoreLimit,
			TeamQualifierScore,
			ResourceLimit,
		};
	};

	struct OccupationScoreType
	{
		enum eOccupationScoreType
		{
			None,
			Kill,
			Acquire,
			StealAcquire,
		};
	};	

	//CDNSkill에 있는 것과 같은 식이지만 테이블 분리해놨으므로 분리한다.
	struct OccupationSkillTargetType
	{
		enum eOccupationSkillTargetType
		{
			Team = 5,
		};
	};

	struct OccuaptionSkillEffectTartgetType
	{
		enum eOccuaptionSkillEffectTartgetType
		{
			Team = 5,
		};
	};

	struct OccupationLogWinType
	{
		enum eOccupationLogWinType
		{
			TimeOut = 0,
			LimitResource = 1,
		};
	};

	struct QueryUpdatePvPDataType
	{
		enum eCode
		{
			FinishGameMode = 0,
			GoPvPLobby,
			GoSelectChar,
			Disconnect,
#if defined(PRE_MOD_PVP_LADDER_XP)
			FinishLadder,	//래더게임 종료시 xp 추가
#endif
#if defined( PRE_ADD_PVP_EXPUP_ITEM )
			UseExpupItem,
#endif // #if defined( PRE_ADD_PVP_EXPUP_ITEM )
			Max,
		};
	};

#ifdef PRE_MOD_PVPRANK
	struct RankTable
	{
		enum eType
		{
			ExpValue = 0, 
			RateValue,
			AbsoluteRankValue,
			InitValue = 100,
		};
	};
#endif		//#ifdef PRE_MOD_PVPRANK

#pragma pack(push, 1)
	struct UserInfo
	{
		UINT	uiSessionID;
		int		iJobID;
		BYTE	cLevel;
		BYTE	cPvPLevel;
		USHORT	usTeam;										// PvPCommon::Team
		UINT	uiUserState;									// PvPCommon::UserState
		char cSlotIndex;
		BYTE	cCharacterNameLen;
		WCHAR	wszCharacterName[NAMELENMAX];
	};

	struct UserInfoList
	{
		UINT	uiSessionID[Common::MaxPlayer];
		int		iJobID[Common::MaxPlayer];
		BYTE	cLevel[Common::MaxPlayer];
		BYTE	cPvPLevel[Common::MaxPlayer];
		USHORT	usTeam[Common::MaxPlayer];					// PvPCommon::Team
		char cTeamIndex[Common::MaxPlayer];
		UINT	uiUserState[Common::MaxPlayer];				// PvPCommon::UserState
		BYTE	cCharacterNameLen[Common::MaxPlayer];
		WCHAR	wszBuffer[NAMELENMAX*Common::MaxPlayer];
	};

	struct RoomInfo
	{
		UINT	uiIndex;									// 방인덱스
		UINT	uiMapIndex;									// 맵인덱스
		UINT	uiGameModeTableID;							// 게임모드 테이블ID
		UINT	uiWinCondition;
		UINT	uiPlayTimeSec;
		BYTE	cMaxUserCount;								// 최대인원
		BYTE	cRoomState;									// PvPCommon::RoomState
		BYTE	cRoomNameLen;
		BYTE	cMinLevel;
		BYTE	cMaxLevel;
		USHORT	unRoomOptionBit;
		int		nEventID;
		BYTE	cMinUserCount;
		bool bIsGuildWarSystem;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		BYTE cRoomType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#ifdef PRE_MOD_PVPOBSERVER
		bool bExtendObserver;
#endif		//#ifdef PRE_MOD_PVPOBSERVER
#if defined( PRE_WORLDCOMBINE_PVP )
		int nWorldPvPRoomType;
#endif
#ifdef PRE_PVP_GAMBLEROOM
		BYTE cGambleType;
		int nGamblePrice;
#endif // PRE_PVP_GAMBLEROOM
		WCHAR	wszBuffer[TxtMax::RoomName];
	};

	struct RoomInfoList
	{
		UINT	uiPvPIndex[Common::RoomPerPage];			// PvPIndex
		UINT	uiMapIndex[Common::RoomPerPage];			// MapIndex
		UINT	uiGameModeTableID[Common::RoomPerPage];
		BYTE	cRoomState[Common::RoomPerPage];			// PvPCommon::RoomState
		BYTE	cRoomNameLen[Common::RoomPerPage];
		BYTE	cCurUserCount[Common::RoomPerPage];			// 현재 유저 수
		BYTE	cMaxUserCount[Common::RoomPerPage];			// 최대 유저 수
		USHORT	unWinCondition[Common::RoomPerPage];		// 승리조건
		BYTE	cMinLevel[Common::RoomPerPage];
		BYTE	cMaxLevel[Common::RoomPerPage];
		USHORT	unRoomOptionBit[Common::RoomPerPage];
		int		nEventID[Common::RoomPerPage];
		UINT nGuildDBID[Common::RoomPerPage][PvPCommon::TeamIndex::Max];			//0이면 무시 아닐경우 해당 길드전용방
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		BYTE cRoomType[Common::RoomPerPage];		//PvPCommon::RoomType
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#ifdef PRE_MOD_PVPOBSERVER
		bool bExtendObserver[Common::RoomPerPage];
#endif		//#ifdef PRE_MOD_PVPOBSERVER
#if defined( PRE_WORLDCOMBINE_PVP )
		int nWorldPvPRoomType[Common::RoomPerPage];
#endif
#if defined( PRE_PVP_GAMBLEROOM )
		BYTE cGambleType[Common::RoomPerPage];
		int nGamblePrice[Common::RoomPerPage];
#endif // #if defined( PRE_PVP_GAMBLEROOM )
		WCHAR	wszBuffer[Common::RoomPerPage*TxtMax::RoomName];
	};

	struct OccupationStateInfo
	{
		int nAreaID;
		int nOccupationState;		//PvPCommon::OccupationState

		UINT nOwnedUniqueID;				//소유시점 성공유저
		short nOwnedTemID;					//현재소유팀

		UINT nTryUniqueID;					//시도자 PvPCommon::OccupationState이 try상태일경우
		short nTryTeamID;
		
		UINT nMoveWaitUniqueID;				//이동대기상태 성공시킨 유저
		short nMoveWaitTeamID;				//이동대기상태팀
	};

	struct OccupationSkillSlotInfo
	{
		int nTeam;
		char cSlotIndex[QUICKSLOTMAX];
		int nSkillID[QUICKSLOTMAX];
		ULONG nSKillUseElapsedTick[QUICKSLOTMAX];			//사용뒤 경과시간 0이면 사용한적 없음
	};

	struct AllKillMode
	{
		struct Time
		{
			enum eCode
			{
				SelectPlayerTick	= 10000,	// 출전 플레이어 고르는 시간
			};
		};

		struct WinBonus
		{
			enum eCode
			{
				HPPercent		= 10,		// 승리시 보너스 HP 퍼센트
				SPPercent		= 10,		// 승리시 보너스 SP 퍼센트

			};
		};
	};

#pragma pack(pop)

	//=============================================================================================
	// Function
	//=============================================================================================

	static bool CheckTeam( const USHORT usTeam )
	{
		if( usTeam >= Team::A && usTeam <= Team::B )
			return true;
		if( usTeam == PvPCommon::Team::Observer )
			return true;
		return false;
	}

	static TeamIndex::eCode Team2Index( Team::eTeam Team )
	{
		switch( Team )
		{
			case Team::A:	return TeamIndex::A;
			case Team::B:	return TeamIndex::B;
		}
		return TeamIndex::Max;
	}

	static Team::eTeam TeamIndex2Team( TeamIndex::eCode TeamIdx )
	{
		switch( TeamIdx )
		{
			case TeamIndex::A:	return Team::A;
			case TeamIndex::B:	return Team::B;
		}
		return Team::Max;
	}

	//blondy
	const int s_iMaxRoundNum = 7; 
	//blondy end
};

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

// NPC 호감도
namespace NpcReputation
{
	struct Common
	{
		enum
		{
			MaxFavorEffectNpcCount	= 5,												// NPC 호감도에 최대로 영향줄 수 있는 수
			MaxMaliceEffectNpcCount = 5,												// NPC 호감도에 최대로 영향줄 수 있는 수
			MaxEffectNpcCount		= MaxFavorEffectNpcCount+MaxMaliceEffectNpcCount+1,

			MaxMailCount			= 12,
			MaxMailRandValue		= 1000000000,
			MaxBenefitCount			= 6,
		};
	};

	struct StoreBenefit
	{
		// 아래의 순서는 StoreBenefit 테이블의 정의와 맞춘 것이기 때문에 절대로 순서가 바뀌면 안됨.
		enum
		{
			EnchantFeeDiscount = 0,
			RepairFeeDiscount,
			SellingPriceUp,
			BuyingPriceDiscount,
			FishProficiencyUp,		// 낚시 숙련도 증가
			CookProficiencyUp,		// 요리 숙련도 증가
			CultivateProficiencyUp,	// 재배 숙력도 증가
			Max,
		};
	};
	
	struct UnionType
	{
		enum
		{
			Commercial = 0,
			Liberty,
			Royal,
			Etc,
			Max,
		};
	};
};

// npc 가 선물로 받는 아이템 종류의 최대 갯수. 현재 테이블에 6개의 종류까지 지정할 수 있다.
const int MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT = 6;

// 퀘스트 보상으로 호감도 올려주는 경우 UI 표시용으로 사용하는 잡템 ID 
// 실제 인벤에 들어가지 않게 막는다.
const int NPC_FAVOR_QUEST_REWARD_FAVOR_UP_ITEM_ID = 400204;		// 호감도 올려주는 보상. 
const int NPC_FAVOR_QUEST_REWARD_BROMIDE_ITEM_ID = 400250;		// 브로마이드 보상.

// 호감도 상점혜택 % 데이터 쌍 최대 갯수
const BYTE STORE_BENEFIT_MAX = 10;

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

// 사제 시스템
namespace MasterSystem
{
	struct Common
	{
		enum
		{
			RefreshCacheTimeSec			= 30,	// 서버<->DB M/W 간 캐싱시간(초)
		};
	};

	struct Max
	{
		enum
		{
			MasterCount				= 3,		// 최대 스승 수
#if defined(_CH) || defined(_WORK)
			PupilCount				= 10,		// 최대 제자 수
#else
			PupilCount				= 3,		// 최대 제자 수
#endif // #if defined(_CH) || defined(_WORK)
			OppositeCount			= (MasterCount > PupilCount) ? MasterCount : PupilCount,
			MasterPageRowCount		= 7,		// 스승 리스트업 Row 개수
			SelfIntrotuctionLen		= (30+1),	// 스승 등록시 자기 소개 최대 글자 수
			MasterAndClassmateCount	= MasterCount+((PupilCount-1)*MasterCount),
			RequestLocationCount	= ((MasterAndClassmateCount > PupilCount) ? MasterAndClassmateCount : PupilCount),
		};
	};

	struct Penalty
	{
		enum
		{
			PupilLeavePenaltyDay		= 1,									// 제자가 스승을 떠났을 때 페널티 날짜(일)
			PupilLeavePenaltySec		= PupilLeavePenaltyDay*24*60*60,		// 제자가 스승을 떠났을 때 페널티 날짜(초)
			MasterLeavePenaltyDay		= 3,									// 스승이 제자를 추방했을 때 페널티 날짜(일)
			PupilLeaveSkipPenaltyDay	= 3,									// 스승이 제자 추방할 때 페널티 안받는 날짜(일)
			PupilLeaveSkipPenaltySec	= PupilLeaveSkipPenaltyDay*24*60*60,	// 스승이 제자 추방할 때 페널티 안받는 날짜(초)
		};
	};

	struct MasterClassmateKind
	{
		enum eCode
		{
			Master = 1,
			Classmate,
			Max,
		};
	};

	struct RequestLocation
	{
		enum eCode
		{
			Pupil = 1,
			MasterAndClassmate,
			ClassmateInfo,
			MyMasterInfo,
		};
	};

	struct Reward
	{
		enum
		{
			PupilCountExpBonusRate		= 10,
		};
	};

	struct EventType
	{
		enum eCode
		{
			Init	= 0,
			VillageFirstConnect,
			Join,
			Graduate,
			Graduated,
			Leave,
			DungeonClear,
		};
	};
};

namespace LadderSystem
{
	struct Common
	{
		enum eCode
		{
			StartMsgCount			= 5,
			RoomListPerPage			= 12,
			MaxObserver				= 4,
			RankingTop				= 10,
			InviteValidTick			= 5000, // 초대 유효 시간
		};
	};
	
	struct Stats
	{
		enum eCode
		{
			InitGradePoint	= 1500,	// 초기 평점
			MinGradePoint	= 600,	// 최소 평점
		};
	};

	struct Exchange
	{
		enum eCode
		{
			MinGamePlay		= 10,	// 최소 게임 수
		};
	};

	struct MatchResult
	{
		enum eCode
		{
			None = 0,
			Win,
			Lose,
			Draw,
		};
	};

	struct MatchType
	{
		enum eCode
		{
			None = 0,
			_1vs1 = 1,
			_2vs2,
			_3vs3,
			_4vs4,
#if defined(_RU)
			MaxCount = _1vs1,
#else	
			MaxCount = _3vs3,
#endif
			MaxMatchType = _4vs4,	// 최대 인원 매치 타입
#if defined(PRE_ADD_DWC)
			_3vs3_DWC	= 100,
			_3vs3_DWC_PRACTICE = 101,
#endif
		};
	};

	static bool bIsServiceMatchType( LadderSystem::MatchType::eCode Type )
	{
		switch( Type )
		{
#if defined(PRE_ADD_DWC)
			case MatchType::_3vs3_DWC:
			case MatchType::_3vs3_DWC_PRACTICE:
#endif
			case MatchType::_1vs1:
#if defined(_RU)
			// None
#else
			case MatchType::_2vs2:
			case MatchType::_3vs3:
			case MatchType::_4vs4:
#endif
				return true;
		}
		return false;
	}

	static int GetNeedTeamCount( LadderSystem::MatchType::eCode Type )
	{
		switch( Type )
		{
		case LadderSystem::MatchType::_1vs1:
		case LadderSystem::MatchType::_2vs2:
		case LadderSystem::MatchType::_3vs3:
		case LadderSystem::MatchType::_4vs4:
		{
			return static_cast<int>(Type);
		}
#if defined(PRE_ADD_DWC)			
		case LadderSystem::MatchType::_3vs3_DWC:
		case LadderSystem::MatchType::_3vs3_DWC_PRACTICE:
		{
			return LadderSystem::MatchType::_3vs3;
		}
#endif
		default: return 0;
		}
	}

	struct RoomState
	{
		enum eCode
		{
			None			= 0,
			WaitUser		= 1,		// 팀설정 위해 유저 기다리는 상태( 이때부터 정상방으로 간주됨 )
			WaitMatching	= (1<<1),	// 매칭대기중
			Matched			= (1<<2),	// 매치됨
			Starting		= (1<<3),	// 시작준비중
			Syncing			= (1<<4),	// 동기화중
			Playing			= (1<<5),	// 플레이중
			GameFinished	= (1<<6),	// 게임끝남
			GameRoomDestroy	= (1<<7),	// 게임룸파괴
			Observer		= (1<<8),	// 옵져버
		};
	};

	struct Reason
	{
		enum eCode
		{
			Disconnect,		// 접속끊음
			LeaveRoom,		// 스스로 나감(?)
			KickOut,		// 쫒겨남
		};
	};

	struct RoomStateReason
	{
		enum eCode
		{
			ERROR_NONE = 0,
			CANT_CREATE_GAMEROOM = 1,
			MAX_OBSERVER,
		};
	};
};

namespace RankingSystem
{
	const BYTE RANKINGPAGEMAX			= 10;					// 한페이지에 출력될 Item 최대개수.
	const BYTE RANKINGPAGECOUNT			= 5;					// 리스트 Page 최대개수.
	const BYTE RANKINGMAX				= RANKINGPAGEMAX * RANKINGPAGECOUNT; // 랭킹 리스트 Item 최대맥스.
};

// DarkLair
namespace DarkLair
{
	struct Common
	{
		enum eCommon
		{
			MaxPlayer = 4,	// 다크레어 최대 이용 인원
			DarkLairPositionGateNo = 1,
		};
	};

	struct Rank
	{
		enum eRank
		{
			SelectTop			= 5,		// 몇 위 까지 보여줄것인지 수
			SelectRankBoardTop	= 10,		// 다크레어 랭킹 게시판에 보여질 개수
			MaxRank			= 10000,	// 최대랭킹
		};
	};
	enum
	{
		TopFloorAbstractMapIndex = 1000000000
	};
};

// Vehicle
namespace Vehicle
{
	struct Slot
	{
		enum eVehicleSlot
		{
			Body = 0,
			Saddle = 1,	// 탈것: 안장
			Hair = 2,	// 탈것: 갈기(useitem)
			Max,

			Sundries = 255,
		};
	};

	struct Common
	{
		enum eCommon
		{
			VehicleEquipBody = 255,
		};

		enum eVehicleValue
		{
			ROTATE_ANGLEPOWER = 50,
			ROTATE_YAWDELTA = 2,
		};
	};

	struct Parts
	{
		enum eVehicleParts
		{
			Default = -1,
			Saddle = 0,
			Hair = 1,
		};

		enum eVehiclePartsColorType
		{
			BodyColor = 0,
		};
	};
};

// Pet
namespace Pet
{
	struct Slot
	{
		enum ePetSlot
		{
			Body = Vehicle::Slot::Body,
			Accessory1 = Vehicle::Slot::Saddle, //악세사리1
			Accessory2 = Vehicle::Slot::Hair, //악세사리2
			Max = Vehicle::Slot::Max,

			Sundries = Vehicle::Slot::Sundries,
		};	
	};

	struct Skill
	{
		enum ePetSkill
		{
			Primary = 0,
			Secondary  = 1,
			Max = 2,
		};
	};

	struct Type
	{
		enum ePetType
		{
			ePETTYPE_EXPIREDATE		= 0x00000001,
			ePETTYPE_SATIETY		= 0x00000010,
		};
	};
	struct Satiety
	{
		enum eSatietySelection
		{
			SatietySelection0 = 0,
			SatietySelection1 = 30,
			SatietySelection2 = 50,
			SatietySelection3 = 80,
			SatietySelection4 = 100
		};
	};	
	const int MAX_SATIETY_SELECTION	 = 5;

	struct Common
	{
		enum eCommon
		{
			PetEquipBody = 254,
		};
	};

	struct Parts
	{
		enum ePetParts
		{
			Default = Vehicle::Parts::Default,
			PetAccessory1 = Vehicle::Parts::Saddle,
			PetAccessory2 = Vehicle::Parts::Hair,
		};

		enum ePetPartsColorType
		{
			BodyColor = 0,
			NoseColor = 1,
		};
	};	

	const float PetExpRate = 0.1f;	
};

#if defined( PRE_ADD_SECONDARY_SKILL )

namespace SecondarySkill
{
	struct Max
	{
		enum eMax
		{
			LearnProductionSkill	= 1,
			LearnManufactureSkill	= 1,
			LearnCommonSkill		= 1,
			LearnSecondarySkill		= LearnProductionSkill+LearnManufactureSkill+LearnCommonSkill,	// 동시에 배울수 있는 최대 보조 스킬 수
			LearnCookingRecipe		= 20,															// 요리 레시피 수
			LearnManufactureRecipe	= LearnCookingRecipe,											// 생산스킬 중 최대 레시피 스킬 수
		};
	};

	struct Type
	{
		enum eType
		{
			ManufactureSkill	= 1,	// 제작스킬
			ProductionSkill		= 2,	// 생산스킬
			CommonSkill			= 3,	// 공용스킬
		};
	};

	struct SubType
	{
		enum eType
		{
			CookingSkill		= 1,	// 요리스킬
			CultivationSkill	= 2,	// 재배스킬
			FishingSkill		= 3,	// 낚시스킬
		};
	};

	struct ExecuteType
	{
		enum eType
		{
			Passive				= 1,	// 액티브
			Active				= 2,	// 패시브
		};
	};

	struct Grade
	{
		enum eType
		{
			Beginning			= 1,	// 초급
			Intermediate,				// 중급
			Advanced,					// 고급
			Max,
		};
	};

	struct ManufactureSkill
	{
		enum eType
		{
			MaxManufactureItemTypeCount	= 5,	// 최대 제작 아이템 결과물 타입 수
			MaxToolItemCount			= 5,	// 최대 도구 아이템 수
			MaxMaterialItemCount		= 5,	// 최대 재료 아이템 수
		};
	};
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

namespace Farm
{
	struct Common
	{
		enum eCommon
		{
#if defined( _FINAL_BUILD )
			SEED_UPDATE_SEC			= (5*60),		// 씨앗 업데이트 간격
#else
			SEED_UPDATE_SEC			= (1*60),		// 씨앗 업데이트 간격
#endif // #if defined( _FINAL_BUILD )
#if defined( PRE_ADD_VIP_FARM )
			VIP_GROWING_BOOST_ITEMID	= 348334082,			// VIP 농장용 성장 촉진제
			VIP_FARM_ITEMID				= 1107396608,			// VIP 농장 입장 아이템
#endif // #if defined( PRE_ADD_VIP_FARM )
#ifdef PRE_MOD_OPERATINGFARM
			OPERATINGFARM_LIMIT = (1000 * 60 * 5),
#endif		//#ifdef PRE_MOD_OPERATINGFARM
		};
	};

	struct AreaType
	{
		enum eType
		{
			Public = 0,				// 공용구역
			Private = 1,			// 개인구역
		};
	};

#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	struct Attr
	{
		enum eType
		{
			None			= 0,
			Vip				= 1,
			GuildChampion	= 2,
			DownScale		= 4,
		};
	};
#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )

	struct Max
	{
		enum eMax
		{
			HARVESTITEM_COUNT				= 3,			// 한 씨앗에서 최대로 나올 수 있는 수
			HARVESTDEPOT_COUNT				= 35,			// 수확 창고 페이지 얻어오는 최대 수량
			PLANTING_TICK					= 3000,			// 씨앗 Planting 시간
			HARVESTING_TICK					= 3000,			// 씨앗 Harvesting 시간
			ADDWATERING_TICK				= 3000,			// AddWatering 시간
			EXTRA_SERVER_TICK				= 1500,			// 서버에서 사용하는 추가 시간
			ATTACHITEM_KIND					= 20,			// 부가아이템 최대 종류 수량
			PLANTING_ATTACHITEM				= 30,			// 식물심기 할 때 부가아이템 최대 수량
			FARMCOUNT						= 100,			// 최대 농장 수
			FIELDCOUNT						= 200,			// 최대 필드 수
			FIELDCOUNT_PERCHARACTER			= 30,			// 캐릭터가 소유권을 가질 수 있는 최대 필드 수
			FARMNAMELEN						= (24+1),		// 농장 최대 이름 길이
			WAREHOUSE_DEFAULT_KEEPING_DAY	= 15,			// 농장창고 기본 보관일
			PRIVATEFIELDCOUNT				= 5,			// 개인최대구역
			PRIVATEPLANTCOUNT				= 1,			// 일반유저 개인구역 최대수
		};
	};

	struct AreaState
	{
		enum eState
		{
			NONE,			// 아무상태 아님
			PLANTING,		// 씨앗 심는 중
			GROWING,		// 씨앗 심고 자라는 중
			COMPLETED,		// 열매(?) 맺은 상태
			HARVESTING,		// 수확하는 상태
			HARVESTED,		// 수확 된 상태
			// SubState
			ADDWATER,		// 물주는중
		};
	};

	struct ActionType
	{
		enum eType
		{
			START = 0,		// 시작
			CANCEL,			// 취소
			COMPLETE,		// 완료
		};
	};

	struct ServerSyncType
	{
		enum eType
		{
			FIELDCOUNT = 0,
			FIELDLIST,
			WAREHOUSE_ITEMCOUNT,
		};
	};

	const std::string	StartPositionStr	= "StartPosition";
	const float			ElapsedSyncPer		= 0.9f;
};

namespace Fishing
{
	struct Status
	{
		enum eStatus
		{
			FISHING_STATUS_NONE,
			FISHING_STATUS_IDLE,
			FISHING_STATUS_READY,		//바로시작가능한 상태
			FISHING_STATUS_FISHING,
			FISHING_STATUS_REWARD,
		};
	};

	struct Max
	{
		enum eMax
		{
			FISHINGPATTERNMAX = 10,			// 낚시포인트테이블 패턴최대
		};
	};

	struct Cast
	{
		enum eCast
		{
			CASTNONE,						//
			CASTAUTO,						//자동낚시
			CASTMANUAL,						//수동낚시
		};
	};

	struct Control
	{
		enum eControl
		{
			CONTROL_NONE,					//놓았을때
			CONTROL_PULLING,				//눌렀을때
		};
	};

	const int FISHINGCHECKPERIODTICK = 200;
	const int FISHINGMAXGAUGE = 1000 * 100;
	const int FISHINGSYNCTERM = 1 * 1000;
};



namespace PCBang
{
	struct Type
	{
		enum ePCBangType			// PCBang 혜택 타입
		{
			Exp = 0,				// PCBang 경험치
			Appellation = 1,		// PCBang 호칭
			Fatigue = 2,			// 피로도
			RebirthCoin = 3,		// 
			MissionReward = 4,
			NestClearCount = 5,
			ClearBox = 6,			// pcbang상자 갯수
			MissionFatigue = 7,		// 피로도 미션
			MissionRebirthCoin = 8,	// 부활석 미션
			MissionRewardCash = 9,
		};
	};

	struct Grade
	{
		enum ePCBangGrade
		{
			None = 0,
			Normal = 1,	// 일반
			Red = 2,
			Silver = 3,
			Gold = 4,
			Premium = 5,
		};
	};
};

namespace Cash
{
	struct PaymentRules
	{
		enum ePaymentRules
		{
			None = 0,
			Total = 1,
			Credit = 2,
			Prepaid = 3,
			Petal = 4,
			Seed = 5,
		};
	};
};

#ifdef PRE_ADD_LIMITED_CASHITEM
namespace LimitedCashItem
{
	struct Common
	{
		enum eCommon
		{
			DitributingTerm = 5000,
			DefaultPacketCount = 200,
		};
	};

	struct TLimitedQuantityCashItem
	{
		int nProductID;
		int nSaleCount;				//팔린갯수다
	};

	struct TChangedLimitedQuantity
	{
		int nProductID;
		short nMaxCount;
	};
};
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

// GlobalEnum.h 이동
namespace GlobalEnum 
{
	enum TileTypeEnum 
	{
		None,
		DryGround,
		ShortGrass,
		LongGrass,
		Stone,
		WetGround,
		Sand,
		Snow,
		Water,
		Temp1,
		Temp2,
		Temp3,
		Temp4,
		Temp5,
		TileTypeEnum_Amount = 14,
	};

	enum ClientSessionTypeEnum 
	{
		Unknown,
		Tcp,
		Udp,
		Auto,
	};

	//	Village Channel Info
	enum eChannelAttribute
	{
		CHANNEL_ATT_NORMAL			= 0x00,
		CHANNEL_ATT_PVP				= 0x01,
		CHANNEL_ATT_PVPLOBBY		= 0x02,
		CHANNEL_ATT_MERITBONUS		= 0x04,
		CHANNEL_ATT_GM				= 0x08,
		CHANNEL_ATT_DARKLAIR		= 0x10,
		CHANNEL_ATT_FARMTOWN		= 0x20,
		CHANNEL_ATT_DWC				= 0X40,
		CHANNEL_ATT_USELASTMAPINDEX = (CHANNEL_ATT_PVP|CHANNEL_ATT_GM|CHANNEL_ATT_DARKLAIR|CHANNEL_ATT_PVPLOBBY|CHANNEL_ATT_FARMTOWN),
	};

	enum eMeritBonus
	{
		MERIT_BONUS_NONE = 0,
		MERIT_BONUS_COMPLETEEXP = 1,
	};

	enum eMapTypeEnum
	{
		MAP_UNKNOWN,
		MAP_VILLAGE,
		MAP_WORLDMAP,
		MAP_DUNGEON,
	};

	enum eMapSubTypeEnum 
	{
		MAPSUB_NONE,
		MAPSUB_NEST,
		MAPSUB_PVPVILLAGE,
		MAPSUB_PVPLOBBY,
		MAPSUB_EVENT,
		MAPSUB_FARM,
		MAPSUB_CHAOSFIELD,
		MAPSUB_DARKLAIR,
		MAPSUB_NESTNORMAL,		// #47865 네스트 연습모드용 맵 서브타입.
		MAPSUB_DARKLAIRWORLDZONE,
		MAPSUB_TREASURESTAGE,
		MAPSUB_FELLOWSHIP,
	};

	struct AllowItemMapType
	{
		enum
		{
			Village		= 0x0001,
			WorldMap	= 0x0002,
			Dungeon		= 0x0004,
			PvP			= 0x0008,
			DarkLair	= 0x0010,
			Nest		= 0x0020,
		};
	};

	enum eDefaultStartGate
	{
		DEFAULT_STARTPOSITION_GATENO = 1,
	};
};

//UserState
enum eLocationState
{
	_LOCATION_NONE = 0,		//옵후 라인 입니다
	_LOCATION_MOVE,			//서버 이동중인 상태도 체킹해야 할까? 해서 일단 맹글어 봅뉘다
	_LOCATION_LOGIN,
	_LOCATION_VILLAGE,
	_LOCATION_GAME,
};

#if defined(PRE_ADD_WORLD_EVENT)
namespace WorldEvent
{	
	enum eJobClassType
	{
		All = 0,
		Warrior = 1,
		Archer = 2,
		Soceress = 3,
		Cleric = 4,
		Academic = 5,
		Kali = 6,
	};

	enum eDetailEventType
	{
		EVENT_NONE = 0, //0  
		EVENT1 = 1, //1. 몬스터사망시 경험치(파티)			
		EVENT2 = 2, //2. 몬스터사망시 경험치						
		EVENT3 = 3, //3. 스테이지클리어 경험치			
		EVENT4 = 4, //4. 연합 포인트획득량 증가
		EVENT5 = 5, //5. 아이템 드랍률 증가
		EVENT6 = 6, //6. 피로도 지급
		EVENT7 = 7, //7. 강화 확률 증가		
		EVENT8 = 8, //8. 2차인증
		EVENT9 = 9, //9. 파티유지
		EVENT10 = 10, //10. 친구끼리 파티
	};	
};
#else //#endif //#if defined(PRE_ADD_WORLD_EVENT)
//Event Type
enum eEventType1
{
	_EVENT_1_NONE = 0,
	_EVENT_1_EXP = 1,
	_EVENT_1_FATIGUE = 2,
	_EVENT_1_DROPITEM = 3,
	_EVENT_1_QUEST = 4,
	_EVENT_1_PVPEVENTROOM = 5,
#if defined( PRE_ADD_REBIRTH_EVENT )
	_EVENT_1_REBIRTH = 6, // 부활제한 해제 이벤트
#endif // 
};

enum eEventType2
{
	_EVENT_2_NONE = 0,
	_EVENT_2_MONSTERDIE = 1,	//exp
	_EVENT_2_CLEAR = 2,		//exp
	_EVENT_2_EXTEND_FATIGUE = 3, //event용 추가 피로도
};

enum eEventType3
{
	_EVENT_3_NONE = 0,
	_EVENT_3_PARTY = 1,
	_EVENT_3_SECONDAUTH = 2,
	_EVENT_3_UPKEEPPARTY = 3,			//파티유지용
	_EVENT_3_FRIENDPARTY = 4,			//절친?용
	_EVENT_3_EKEYANDECARD = 5,			//중국용 ekey, ecard용
	_EVENT_3_CLASSEXP = 6,				//클래스별 차등 보너스 경험치 설정
};
#endif //#if defined(PRE_ADD_WORLD_EVENT)

enum eRebirthType
{
	_REBIRTH_SELF,
	_REBIRTH_REBIRTHER,
	_REBIRTH_REBIRTHEE
};

enum eTaxType
{
	TAX_NONE,
	TAX_POST,			// 우편세금
	TAX_TRADEREGISTER,	// 무인거래소 등록 수수료
	TAX_TRADESELLING,	// 무인거래소 판매 수수료
	TAX_POSTDEFAULT,	// 우편 발송시 기본 수수료 (1회 1번 우표값)
	TAX_DEAL,			// 유저간 거래 세금
	TAX_RESERVE,			// [사용안함]
	TAX_TRADEREGISTER_CASH,	// 무인거래소 캐시아이템 사용시 등록 수수료
	TAX_TRADEPETALSELLING,	// 무인거래소 페탈 판매 수수료
	TAX_WSTORAGE,			// 서버창고 수수료
};

enum eMissionType
{
	MissionType_Normal,		// 일반 미션
	MissionType_MetaShow,	// 메타업적이면서 클라이언트에 보임
	MissionType_MetaHide,	// 메타업적이면서 클라이언트에 안보임
	MissionType_Quest,		// 퀘스트에서 연계되서 사용되는 미션. 클라이언트에서 안보임
	MissionType_EventPopUp, // 이벤트 팝업 (MissionTable 테이블에 팝업용 필드 대신 Type값 이용)
	MissionType_HelpAlarm,	// 헬프 알람
	MissionType_TimeEvent,	// 타임 이벤트 미션
};

enum eRestraintType
{
	_RESTRAINTTYPE_NONE = 0,		//none
	_RESTRAINTTYPE_BLOCK,		//account block
	_RESTRAINTTYPE_CHAT,		//chat block
	_RESTRAINTTYPE_TRADE,		//trade blcok(include market)
};

enum eVoiceChannelType
{
	_VOICECHANNEL_TYPE_SPARTIAL,			//with 3d sound
	_VOICECHANNEL_TYPE_NONESPARTIAL,		//none 3d sound
};

enum eVoiceJoinType
{
	_VOICEJOINTYPE_NONE,
	_VOICEJOINTYPE_TALKONLY,
	_VOICEJOINTYPE_LISTENONLY,
	_VOICEJOINTYPE_TALK_N_LISTEN,
};

enum eBroadcastingEffect
{
	EffectType_None = 0,
	EffectType_Enchant = 1,
	EffectType_Compound = 2,
	EffectType_Emblem = 3,
	EffectType_Random = 4,
#ifdef PRE_ADD_SECONDARY_SKILL
	EffectType_Plant = 5,
	EffectType_Water = 6,
	EffectType_Harvest = 7,
	EffectType_Cooking_Making = 8,
	EffectType_Cooking_Finish = 9,
	EffectType_Fishing = 10,
#endif // PRE_ADD_SECONDARY_SKILL

	EffectState_None = 0,
	EffectState_Start = 1,		// 시도
	EffectState_Success = 2,	// 성공
	EffectState_Fail = 3,		// 강화파괴, 제작실패(리소스 이름때문에 변경)
	EffectState_SoftFail = 4,	// 강화중간실패(등급 하락)
	EffectState_Open = 5,		// 랜덤아이템 open시도
	EffectState_VeryGood = 6,	// 랜덤아이템 
	EffectState_Good = 7,		// 랜덤아이템
	EffectState_Normal = 8,		// 랜덤아이템
	EffectState_Bad = 9,		// 랜덤아이템
	EffectState_Cancel = 10,	// 취소
};

enum eChatRoomType		// 채팅방 타입
{
	CHATROOMTYPE_NONE = 0,			// 초기값(의미없음)
	CHATROOMTYPE_NORMAL = 1,		// 일반
	CHATROOMTYPE_TRADE = 2,			// 거래
	CHATROOMTYPE_PR = 3,			// PR
};

enum eChatRoomAllow		// 입장가능 타입
{
	CHATROOMALLOW_ALL,				// 모두 
	CHATROOMALLOW_FRIEND,			// 친구
	CHATROOMALLOW_GUILD,			// 길드
	CHATROOMALLOW_FRIENDANDGUILD,	// 친구 & 길드
};

enum eChatRoomLeave
{
	CHATROOMLEAVE_NONE			= 0,	// 의미없는 초기값
	CHATROOMLEAVE_KICKED,
	CHATROOMLEAVE_LEAVE,
	CHATROOMLEAVE_DESTROYROOM,
	CHATROOMLEAVE_FOREVERKICKED,
};

typedef INT64	MONEY_TYPE;
typedef int		TAX_TYPE;

typedef int		ITEMCLSID;
typedef INT64	ITEMSERIALID;

typedef INT64	CASH_TYPE;
typedef int		CASHITEM_SN;

#define ITEMCLSID_NONE	0


inline int GetCashRemainTime(INT64 biSerial)
{
	return (int)(biSerial & 0x00000000ffffffff);
}

enum eItemReversion
{
	ITEMREVERSION_NONE = 0,
	ITEMREVERSION_BELONG = 1,	//귀속
	ITEMREVERSION_TRADE = 2,
	ITEMREVERSION_GUILD = 3,	//길드귀속아이템
	//ITEMREVERSION_GAIN = 2,	// 귀속인가 아닌가로 시스템 통합 commented by kalliste 100113
};

//ServiceManager
enum eServiceState
{
	_SERVICE_STATE_OFFLINE = 0,
	_SERVICE_STATE_CREATE,
	_SERVICE_STATE_ONLINE,
	_SERVICE_STATE_TERMINATED,
};

enum eServiceStartType
{
	_SERVICE_START_ALL,
	_SERVICE_START_EACH,
	_SERVICE_START_EACH_BY_LAUNCHERID,
};

enum eServiceStopType
{
	_SERVICE_STOP_ALL,
	_SERVICE_STOP_ALL_FORCE,
	_SERVICE_STOP_EACH,
	_SERVICE_STOP_EACH_FORCE,
	_SERVICE_STOP_EACH_BY_LAUNCHERID,
	_SERVICE_STOP_EACH_FORCE_BY_LAUNCHERID,
};

enum eBanFileType
{
	_BANTYPE_ACCOUNTNAME,
	_BANTYPE_ACCOUNTID,
	_BANTYPE_CHARACTERNAME,
	_BANTYPE_MAX,
};

const int SMIDLENMAX	= 16;		// 서비스 매니저 / 모니터 인증 ID 최대 길이
const int SMPASSLENMAX	= 16;		// 서비스 매니저 / 모니터 인증 비번 최대 길이

#define	MAKE_SM_SVRCHN(pSVRID, pCHNID)	MAKELONGLONG(pCHNID, pSVRID)	// 채널 정보 관리 시 키로 사용하는 서버/채널 ID 를 조합
#define	GET_SM_SVR(pSVRCHN)				HILONG(pSVRCHN)					// 채널 정보 관리 시 키에서 서버 ID 를 추출
#define	GET_SM_CHN(pSVRCHN)				LOLONG(pSVRCHN)					// 채널 정보 관리 시 키에서 채널 ID 를 추출

const int WAITPROCESSMAX = 500;			//대기열 프로세스맥스

enum ePromotionType
{
	PROMOTIONTYPE_NONE = 0,
	PROMOTIONTYPE_MONSTERKILL,
	PROMOTIONTYPE_STAGECLEAR,
	PROMOTIONTYPE_DECREASEFATIGUE,
	PROMOTIONTYPE_MAX,
};

enum ePromotionConditionType
{
	PROMOTION_CONDTIONTYPE_NONE = 0,
	PROMOTION_CONDTIONTYPE_MAXLEVEL,
};

enum eNpcTalkString 
{
	NPCTALK_START = 0,			// NPCTALK_START은 0으로 항상 유지바랍니다.
	NPCTALK_COMPLETECUTSCENE,
	NPCTALK_END,
};

static std::wstring NpcTalkStringGroup[NPCTALK_END]= 
{
	L"start",					// 0번째 스트링은 항상 "start"로 유지바랍니다.
	L"completecutscene",
};

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
enum eEnchantGuardType
{
	ENCHANT_ITEM_NONE = 0,
	ENCHANT_ITEM_GUARD,				// 강화 보호젤리 (파괴x)
	ENCHANT_ITEM_SHIELD,			// 상위 강화보호 젤리(파괴x, 다운x)	
};
#endif

namespace Partner
{
	struct Type
	{
		enum eType
		{
			None = 0,
			MS,
			Max,
		};
	};
}

const int GUILDWAR_FINALS_TEAM_MAX = 16;	// 16강 기준 16팀
const int GUILDWAR_DAILY_AWARD_MAX = 7;		// 일자별 시상내역
const int REQ_GUILDWARINFO_TICK_MAX=30*1000; // 응답 재요청 Tick
const int GUILDWAR_DB_JOB_SYSTEM_RESULT_REQ_MAX=60*1000;		// 신청기간, 본선 시작후 1분 경과때 마다 DBJob 결과 가져오기
const int GUILDWAR_TOURNAMENT_GROUP_MAX = GUILDWAR_FINALS_TEAM_MAX/2; // 토너먼트 그룹
const int GUILDWAR_TRIAL_POINT_TEAM_MAX = 24;	// 예선전 점수 보여줄 24개팀.

#if defined(_FINAL_BUILD)
const int GUILDWAR_FINAL_ROOMSTRAT_TICK_MAX=5*60*1000;			// 방이 만들어 진후 방이 시작되는 시간..5분
const int GUILDWAR_PRE_WIN_SKILL_COOLTIME=60*2*1000;			// 길드전 우승스킬 쿨타임 2분
const int GUILDWAR_TRIAL_POINT_REFRESH_TICK_MAX=10*60*1000;		// 10분에 한번씩 가져오기
const int GUILDWAR_SECRET_TERM_SECOND=24*60*60;					// 시크릿 발동, 해제 시간..(second)
#else
const int GUILDWAR_FINAL_ROOMSTRAT_TICK_MAX=60*1000;			// 방이 만들어 진후 방이 시작되는 시간..
const int GUILDWAR_PRE_WIN_SKILL_COOLTIME=30*1000;				// 길드전 우승스킬 쿨타임
const int GUILDWAR_TRIAL_POINT_REFRESH_TICK_MAX=60*1000;		// 1분에 한번씩 가져오기
const int GUILDWAR_SECRET_TERM_SECOND=5*60;						// 시크릿 발동, 해제 시간..(second)
#endif // #if defined(_FINAL_BUILD)

const float GUILDWAR_SECRET_SATRT_RATE = 0.1f;	// 시크릿 발동
const float GUILDWAR_SECRET_END_RATE = 0.02f;	// 시크릿 해제

struct sGroup
{
	int nTeamNum;			// Nubmer니까 인덱스 구할려면 -1
	int nOpeningPoint;		// 예선점수
	int nTournamentPoint;	// 중간점수
};

struct sTournamentGroup
{
	sGroup ATeam;
	sGroup BTeam;
	UINT  unPvPIndex;
	DWORD dwStartTick;
};

enum eGuildWarStepType
{
	GUILDWAR_STEP_NONE = 0,
	GUILDWAR_STEP_PREPARATION,	// 신청기간
	GUILDWAR_STEP_TRIAL,		// 예선기간
	GUILDWAR_STEP_REWARD,		// 보상기간 (본선포함)
	GUILDWAR_STEP_END,
};

enum eGuildWarEventType
{
	GUILDWAR_EVENT_START = 0,	// 시작
	GUILDWAR_EVENT_END = 1,		// 끝
	GUILDWAR_EVENT_MAX = 2,		
};

//차수 1=결승, 2=4강, 3=8강, 4=16강
enum eGuildFinalPart
{	
	GUILDWAR_FINALPART_NONE =0,
	GUILDWAR_FINALPART_FINAL,
	GUILDWAR_FINALPART_4,
	GUILDWAR_FINALPART_8,
	GUILDWAR_FINALPART_16,	
	GUILDWAR_FINALPART_MAX,
};

enum eGuildWarRankingType
{
	GUILDWAR_RANKINGTYPE_TOTAL = 0,	// 대상
	GUILDWAR_RANKINGTYPE_SUPPRESSION,	// 토벌
	GUILDWAR_RANKINGTYPE_CAPTURE,		// 포획
	GUILDWAR_RANKINGTYPE_HARVEST,		// 포획
	GUILDWAR_RANKINGTYPE_COOKING,		// 요리
	GUILDWAR_RANKINGTYPE_PVP,			// 전투
	GUILDWAR_RANKINGTYPE_MAX,
};

enum eGuildWarSettingStep
{
	REQ_NONE,				// 아무 상태아님
	REQ_SCHEDULE,			// 전체 스케쥴 요청단계	
	REQ_FINAL_SCHEDULE,		// 본선 스케쥴 요청단계	
	REQ_PRE_WIN,			// 지난 차수 승리길드 요청
	REQ_TEAM_POINT,			// 청팀, 홍팀 점수요청	
	REQ_ALL_COMPLETE,		// 모든 정보 셋팅 완료
};

enum eGuildWarTeamType
{
	GUILDWAR_TEAM_BLUE = 1,	// 블루팀
	GUILDWAR_TEAM_RED,		// 레드팀
};

enum eGuildWarRewardForCharacter
{
	GUILDWAR_REWARD_CHAR_PRESENT = 0,   // 예선 선물
	GUILDWAR_REWARD_CHAR_MAIL,			// 예선 메일
	GUILDWAR_REWARD_CHAR_POINT,			// 예선 축제포인트
	GUILDWAR_REWARD_CHAR_MAX,		
};

enum eGuildWarRewardForGuild
{
	GUILDWAR_REWARD_GUILD_SKILL = 0,	// 본선 스킬?
	GUILDWAR_REWARD_GUILD_PRESENT,		// 본선 선물(Queue)
	GUILDWAR_REWARD_GUILD_MAIL,			// 본선 메일(Queue)
	GUILDWAR_REWARD_GUILD_TRIAL_POINT,	// 예선 길드 포인트 
	GUILDWAR_REWARD_GUILD_FINAL_POINT,	// 본선 길드 포인트
	GUILDWAR_REWARD_GUILD_MAX,
};

enum eGuildPointType					// 길드 포인트타입
{
	GUILDPOINTTYPE_STAGE = 1,			// 스테이지 
	GUILDPOINTTYPE_MISSION = 2,			// 미션
	GUILDPOINTTYPE_WAR = 3,				// 길드전
	GUILDPOINTTYPE_MAX = 4,
};

struct TAppellationTime
{
	int nArrayID;									//칭호
	__time64_t tExpireDate;							//기간
};

struct TCashGlyphData
{
	BYTE cActiveGlyph;				// 문장 슬롯 활성화 상태
	__time64_t	tGlyphExpireDate;	// 문장 슬롯 기간
};

#if defined( PRE_ADD_SHORTCUT_HELP_DIALOG )
enum eShortCutHelpDlgType
{
	HELP_SHORTCUT_CONTROL_KEY = 1,
	HELP_SHORTCUT_CHAR_SET_PLATE,
	HELP_SHORTCUT_CREATE_PLATE,
	HELP_SHORTCUT_DESTROY_PLATE,
	HELP_SHORTCUT_SET_APPELLATION,
	HELP_SHORTCUT_SKILL,
	HELP_SHORTCUT_POTENTIAL,
	HELP_SHORTCUT_ITEM_UPGRADE,
	HELP_SHORTCUT_ITEM_CREATE,
	HELP_SHORTCUT_ITEM_DISJOINT,
	HELP_SHORTCUT_PARTY_PLAY,
	HELP_SHORTCUT_FRIEND,
	HELP_SHORTCUT_PUPILLIST,
	HELP_SHORTCUT_GUILD,
	HELP_SHORTCUT_QUEST,
	HELP_SHORTCUT_STAGE,
	HELP_SHORTCUT_MARKET,
	HELP_SHORTCUT_MAIL,
	HELP_SHORTCUT_VEHICLE,
	HELP_SHORTCUT_PET,
	HELP_SHORTCUT_REPUTATION,
	HELP_SHORTCUT_FARM,
	HELP_SHORTCUT_FISHING,
	HELP_SHORTCUT_COOK,
	HELP_SHORTCUT_MASTER,
	HELP_SHORTCUT_PVP,
	HELP_SHORTCUT_PRIVATE_CHAT,
#ifdef PRE_ADD_TALISMAN_SYSTEM
	HELP_SHORTCUT_TALISMAN,
#endif
};
#endif // PRE_ADD_SHORTCUT_HELP_DIALOG

#if defined( PRE_ADD_INSTANT_CASH_BUY )
enum eInstantCashShopBuyType
{
	INSTANT_BUY_NONE = -1,
	INSTANT_BUY_SEAL = 0,
	INSTANT_BUY_ITEMUPGRADE,
	INSTANT_BUY_DEATH,
	INSTANT_BUY_INVEN,
	INSTANT_BUY_STORAGE,
	INSTANT_BUY_POTENTRANS,
	INSTANT_BUY_PET,
	INSTANT_BUY_TRADE,
	INSTANT_BUY_SKILL,
	INSTANT_BUY_FARM,
	INSTANT_BUY_AUTOUNPACK,
	INSTANT_BUY_PLATE,
	INSTANT_BUY_PETEXTEND,
};

enum eInstantShowType
{
	INSTANT_BUY_HIDE = 0,
	INSTANT_BUY_SHOW,
	INSTANT_BUY_DISABLE
};

const int InstantCashShopTypeMin = 50;

#endif // PRE_ADD_INSTANT_CASH_BUY

enum eDragonNestType
{
	None = 0,
	SeaDragon = 1,
	GreenDragon = 2,
	DesertDragon = 3,
	Max,
};

enum eTrace
{
	TRACE_CHARACTERNAME = 0,
	TRACE_ACCOUNTDBID,
	TRACE_CHRACTERDBID,
};

enum eWindowState
{
	WINDOW_NONE					= 0,
	WINDOW_EXCHANGE				= 1, 		// 거래	
	WINDOW_CASHSHOP				= (1<<1),	// 캐쉬샵
	WINDOW_COSMIX				= (1<<2),	// 코스튬 합성
	WINDOW_DESIGNMIX			= (1<<3),	// 코스튬 디자인 합성
	WINDOW_BLIND				= (1<<4),	// 블라인드 상태(NPC대화, 일반강화, 아이템 조합, 메일, 상점, 무인상점)
	WINDOW_PROGRESS				= (1<<5),	// 프로그레스 바 상태(랜덤주머니, 펫 등등)
	WINDOW_ISACCEPT				= (1<<6),	// 수락 창이 뜬 상태..
	WINDOW_RANDOMMIX			= (1<<7),	// 코스튬 랜덤 합성
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	WINDOW_ALTEIAWORLD			= (1<<8),	// 알테이아대륙탐험 창 열린 상태
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )
};

struct TSourceData
{
	int nItemID;
	int nRemainTime;
};

namespace PartsMonster
{
	struct Common
	{
		enum UI // UI 에서만 표시되는 파츠의 최대치 , 몬스터의 실제 파츠갯수와는 상관없다.
		{
			Max_PartsSize = 4,
		};
	};
}

enum ePartyModifyResult
{
	_PARTY_MOD_RET_NONE = 0,
	_PARTY_MOD_RET_ADD,
	_PARTY_MOD_RET_DEL,
};

namespace CommonVariable
{
	struct Type
	{
		enum eCode
		{
			AddFatigue					= 1,	// 피로도 추가
			LastPartyDungeonInfo		= 2,	// 파티상태 이전 던전 서버 정보
			AttendanceEvent				= 3,	// 출석이벤트
			PvPOccupation_PlayCount 	= 4,	// 점령전 플레이 수
			PvPOccupation_WinCount		= 5,	// 점령전 승리 수
			PvPOccupation_KillCount 	= 6,	// 점령전 킬 수
			PvPOccupation_OccupyArea	= 7,	// 점령전 거점 점령
			PvPOccupation_StealArea		= 8,	// 점령전 거점 쟁탈
			PvPAllKill_PlayCount		= 9,	// 올킬전 플레이 수
			PvPAllKill_WinCount			= 10,	// 올킬전 승리 수
			PvPAllKill_KillCount		= 11,	// 올킬전 킬 수
			MOVESERVER_PARTYID = 12,		// 서버 이동할때 임시로 저장할 PartyID
			BESTFRIEND_GIFTBOXID = 13,		// 절친 선물 박스ID
			EXPUPITEM_COUNT = 14,			// 절대값 경험치증가 아이템
			LevelupEventNew = 15,			// 레벨업이벤트 선물(한번 받으면 더이상 받을 수 없는거 체크)
			LimitedShopWeek = 16,			// 주간구매제한 상점 초기화
			NotifyCompound = 17,
			TotalLevelSkillMedal = 18,		// 통합스킬레벨 메달획득갯수 일일제한
			SampleDataVersion = 19,
			SampleItem1 = 20,
			SampleItem2 = 21,
			SampleItem3 = 22,
			SampleItem4 = 23,
			SampleItem5 = 24,
			SampleItem6 = 25,
			SampleItem7 = 26,
			SampleItem8 = 27,
			SampleItem9 = 28,
			SampleItem10 = 29,
			MuteChatting = 30,				// 체팅 금지
			PvPExpupItem_UseCount = 31,		// 콜로세움 경험치 증가 아이템 사용횟수
			Max,
		};
	};

	struct Data
	{
		Type::eCode Type;
		INT64 biValue;
		__time64_t tLastModifyDate;
	};
}

namespace GhoulMode
{
	struct PointType
	{
		enum eCode
		{
			PlayCount = 0,			// 플레이 횟수
			GhoulWin,				// 구울 승리
			HumanWin,				// 인간 승리
			TimeOver,				// 인간 승리(타임 오버)
			GhoulKill,				// 구울 Kill
			HumanKill,				// 인간 Kill
			HolyWaterUse,			// 성수 사용
			Max,
		};
	};
}

namespace BreakInto
{
	struct Type
	{
		enum eCode
		{
			None			= 0,
			GMTrace			= 1,	// GM 추적
			WorldZoneParty	= 2,	// 월드존 초대
			PartyRestore	= 3,	// 파티 복구(?)
#if defined( PRE_PARTY_DB )
			PartyJoin		= 4,	// 파티참여
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_WORLDCOMBINE_PVP )
			WorldPvPRoom	= 5,
#endif
		};
	};
}

namespace AppellationColor
{
	struct Type
	{
		enum eCode
		{
			White			= 0,
			Purple			= 1,
			Orange			= 2,	//기간제 캐쉬 칭호
			Green			= 3,	//PC방 전용 칭호
		};
	};
}

namespace AppellationType
{
	struct Type
	{
		enum eCode
		{
			Normal	= 0,
			Cash	= 1,
			Period	= 2,
			PCBang	= 3,
			Guild	= 4,
			BestFriend = 5,
			ComeBack = 6,	//귀환자 칭호
#ifdef PRE_ADD_GM_APPELLATION
			GM = 7,
#endif // PRE_ADD_GM_APPELLATION

#ifdef PRE_ADD_GM_APPELLAThongse
			hong = 88,
#endif // PRE_ADD_GM_APPELLATION

#ifdef PRE_ADD_GM_APPELLAchengse
			jin = 99,
#endif // PRE_ADD_GM_APPELLATION
		};
	};	
}

namespace IncreaseLifeType
{
	struct Type
	{
		enum eCode
		{
			LifeStone	= 0,		//생명석
			RebirthStone	= 1,		//부활석			
		};
	};
}

namespace Ghost
{
	struct Type
	{
		enum eCode
		{
			Normal			= 0,
			PartyRestore	= 1,
		};
	};	
}

namespace Shop
{
	struct Type
	{
		enum eCode
		{
			Normal = 0,			// 일반샵
			LadderPoint,		// 래더포인트샵
			GuildPoint,			// 길드전포인드샵
			ReputePoint,		// 호감도 포인트샵
			
			Combined = 100,		// 일반 통합(?) 샵
			CombinedLadderPoint,// 래더 포인트 통합(?) 샵
			CombinedGuildPoint,	// 길드전포인트 통합(?) 샵
			CombinedReputePoint,// 호감도포인트 통합(?) 샵
			CombinedSeedPoint,	// 시드샵
		};
	};

	struct Max
	{
		enum eCode
		{
			PurchaseType = 2,
		};
	};

	struct PurchaseType
	{
		enum eCode
		{
			None = 0,
			Gold,				// 골드
			ItemID,				// ItemID
			LadderPoint,		// 래더포인트
			UnionPoint,			// 연합포인트
			GuildPoint,			// 길드포인트
			Sample,				// 샘플아이템
			Seed,				// 귀속화폐(시드)
		};
	};

	struct PurchaseLimitType
	{
		enum eCode
		{
			None = 0,
			JobID,				// JobID
			Level,				// 레벨
			PvPRank,			// PvP계급
			GuildLevel,			// 길드레벨
			LadderGradePoint,	// 래더평점( 1vs1 2vs2 .. 등 중 한개라도 넘으면 됨 )
		};
	};

	struct Repurchase
	{
		enum eCode
		{
			MaxList = 30,
		};
	};
}

namespace DualSkill
{
	struct Type
	{
		enum eSkillPage
		{
			None = -1,
			Primary = 0,
			Secondary = 1,
			MAX,
		};
	};
}

#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
namespace ReservationSKillList
{
	struct Type
	{
		enum eSkillReservation
		{
			UnLock = 1,		// 최초 습득..Money 까임
			Acquire = 2,	// 초기화후 습득
			LevelUp = 3,	// 스킬 레벨업.
		};		
	};
	const int ReservationMax = 40;
}
#endif

namespace Item
{
	struct SkillUsingType
	{
		enum eCode 
		{
			None,
			ApplyStateBlow,
			ApplySkill,
			Eternity,				// 효과 영구 적용 아이템
			SkillLevelUp,			// 스킬 레벨업용 아이템
			PrefixSkill,			// 접두사 스킬용 아이템
		};
	};
}

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
namespace SimpleConfig
{
	struct Type
	{
		enum eType
		{
			None = 0,
			ConnectIPCountMax,
		};
	};

	struct DefaultValue
	{
		enum Value
		{
			None = 0,
			ConnectIPCountMax = 100,
		};
	};

	struct ConfigData
	{
		char cCode;
		int nValue;
	};
	const int ConfigDataCountMax = 100;
};
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

namespace PlayerCustomEventUI
{
	const int PlayerCustomEventUIMax = 100;
};

namespace GuildRecruitSystem
{
	struct Max
	{
		enum eType
		{
			MaxRequestCount			= 3,		// 길드모집게시판 최대 신청 갯수
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			GuildRecruitRowCount	= 5,		// 길드모집게시판 Row 개수
#else
			GuildRecruitRowCount	= 6,		// 길드모집게시판 Row 개수
#endif
			GuildRecruitCharacterRowCount = 50,	// 길드모집게시판을 통해서 길드가입신청한 유저 목록 Row 개수
		};
	};
	struct RegisterType
	{
		enum eType
		{
			RegisterOn,		// 등록
			RegisterMod,	// 수정
			RegisterOff,	// 삭제
		};
	};
	struct RequestType
	{
		enum eType
		{
			RequestOn,		// 등록
			RequestOff,		// 해지
		};
	};
	struct AcceptType
	{
		enum eType
		{
			AcceptOn,		// 가입 승인
			AcceptOff,		// 가입 거절
		};
	};
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	struct PuposeCodeType	//	길드목적
	{
		enum eType
		{
			AllCode,	    //	전체
			FriendlyCode,	//	친목
			WarCode,		//	전투
			NestCode,		//	네스트
			PvPCode,		//	콜로세움
		};
	};
	struct SortType			//	정렬방식
	{
		enum eType
		{
			GuildLevelDesc = 1,	//	길드레벨 내림차순
			GuildLevelAsc,	//	길드레벨 오름차순
			GuildMemberDesc,//	현재인원수 내림차순
			GuildMemberAsc, //  현재인원수 오름차순
			GuildNameDesc,	//  길드명 내림차순
			GuildNameAsc,	//  길드명 오름차순
		};
	};
#endif
};

namespace AsiaAuth
{
	struct Domain
	{
		enum eDomain
		{
			ThailandPlayParkID,
			ThailandAsiaSoftID,
			ThailandTCGID,
			ThailandDomainMax,
		};		
	};

	static std::string strDomain[Domain::ThailandDomainMax] = 
	{
		"THPP",
		"THAS",
		"THTC",
	};

	struct Common
	{
		enum eCommon
		{
			OTPRetCommond = 1002,
			OTPLenMax = 6,
			OTPTimeOut = 1000 * 10,
		};
	};

	struct Auth
	{
		enum eAuth
		{
			AuthReturnCode = 0,
			AuthDescription,
			AuthMaster_ID,
			AuthAkeyFlag,
			AuthMaster_Akey_ID,
			AuthSessionID,
			AuthFormatCountMax,
		};
	};

	struct OTP
	{
		enum eOTP
		{
			OTPCommand = 0,
			OTPLength,
			OTPStatus,
			OTPMessage,
			OTPEndPoint,
			OTPFormatCountMax,
		};
	};

	struct AuthResultCode
	{
		enum eAuthResultCode
		{
			None = -1234567,
			Success = 0,
			Invalid_Request = -1001,
			Invalid_UserID = -1002,
			User_Not_Found = -1003,
			Worng_Password = -1004,
			Disable_User = -1005,
			Database_Error = -9009,
			Unexpected_Error = -9010,
		};
	};

	struct TAsiaSoftAuthInfo
	{
		int nAuthReturnCode;		//참조 AsiaAuth::AuthResultCode
		std::string strDescription;
		std::string strDomain;
		std::string strMasterID;
		std::string strAkeyMasterID;		//with itp
		UINT nSessionID;
		bool bAkeyOTPFlag;					//need otp

		TAsiaSoftAuthInfo()
		{
			nAuthReturnCode = AsiaAuth::AuthResultCode::None;
			nSessionID = 0;
			bAkeyOTPFlag = false;
		}
	};
};

namespace WarpVillage
{
	struct WarpVillageInfo
	{
		int nMapIndex;
		BYTE btLimitLevel;

		WarpVillageInfo() {}
		WarpVillageInfo(int nIndex, BYTE btLevel)
			: nMapIndex(nIndex), btLimitLevel(btLevel)
		{

		}
	};
};

namespace NestClear
{
	struct Type
	{
		enum eNestClearType
		{
			Normal = 1,		// 일반유저
			PCBang = 2,		// 피씨방에서 클리어 할 경우
		};
	};
};

namespace Gesture
{
	struct Type
	{
		enum eGestureType
		{
			Normal = 0,
			GuildWarWin = 1,
			GuildRewardItem = 2,
		};
	};
}

#if defined (PRE_ADD_DONATION)
namespace Donation
{
	const BYTE MAX_RANKING = 10;

	struct Ranking
	{
		INT64 biCharacterID;
		WCHAR wszCharacterName[NAMELENMAX];
		BYTE btJobCode;
		WCHAR wszGuildName[GUILDNAME_MAX];
		INT64 nCoin;
	};
};
#endif // #if defined (PRE_ADD_DONATION)

namespace Dungeon
{
	struct Difficulty
	{
		enum eCode
		{
			Easy = 0,
			Normal = 1,
			Hard = 2,
			Master = 3,
			Abyss = 4,
			Chaos = 5,
			Hell = 6,
			Max,
		};
	};
}

#if defined( PRE_PARTY_DB )

typedef INT64 TPARTYID;

namespace Party
{
	struct Interval
	{
		enum eCode
		{
			GetListPartyTick = (3*1000),
		};
	};

	struct Max
	{
		enum eCode
		{
			SearchWordLen = (20+1),	// 검색어 최대 길이
			WordSpacing= 5,			// 검색어 띄어쓰기 최대 수
			SearchWordHistory = 5,	// 검색어 히스토리 최대 수
			DetailSearchMapCount = 1, // 상세 검색 가능 맵 최대 수
			WorldCombinePartyList = 2, // 월드통합파티 최대 개수
			WorldCombinePartyBonus = 2 // 월드통합파티 보너스 최대 개수
		};
	};

	struct LocationType
	{
		enum eCode
		{
			Village = 0,
			Worldmap,
		};
	};

	struct Constants
	{
		enum eCode
		{
			INVALID_PASSWORD = 0,
			PARTY_LIST_REFRESH_SEC_CLIENT = 5,
			MIN_SEARCH_WORD = 2,
			INVALID_WORLDCOMBINE_TABLE_INDEX = -1,
			INVALID_SERVERID = 0,
#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
			INCORRECT_PASSWORD = 10,
#endif 
		};
	};

	struct OrderType
	{
		enum eCode
		{
			LevelDesc = 0,				// 입장레벨 내림차순
			LevelAsc,					// 입장레벨 오름차순
			MemberCountDesc,			// 인원수 내림차순
			MemberCountAsc,				// 인원수 오름차순
			TargetMapDesc,				// 목표 스테이지 내림차순
			TargetMapAsc,				// 목표 스테이지 오름차순
			TargetMapDifficultyDesc,	// 목표 스테이지 난이도 내림차순
			TargetMapDifficultyAsc,		// 목표 스테이지 난이도 오름차순
			BonusDesc,					// 보너스 내림차순
			BonusAsc,					// 보너스 오름차순
			Max,
			Default = LevelDesc,
		};
	};

	struct StageOrderType
	{
		enum eCode
		{
			TargetMapAllDesc = 0,		// 모든 맵 내림차순
			TargetMapNestDesc,			// 네스트맵 내림차순
			TargetMapStageDesc,			// 스테이지 내림차순
			TargetMapEtcDesc,			// 기타 내림차순
			Max,
			Default = TargetMapAllDesc,
		};
	};

	struct KickKind
	{
		enum eCode
		{
			NormalKick = 0,
			PermanentKick = 1,
		};
	};

	struct Data
	{
		TPARTYID PartyID;
		int iServerID;
		Party::LocationType::eCode LocationType;
		int iLocationID;	// 마을:ChannelID, 월드존,던젼:MapID
		ePartyType Type;
		WCHAR wszPartyName[PARTYNAMELENMAX];
		short nPartyMaxCount;
		int iTargetMapIndex;
		Dungeon::Difficulty::eCode TargetMapDifficulty;
		int iBitFlag;
		int iPassword;
		BYTE cMinLevel;
		ePartyItemLootRule LootRule;
		eItemRank LootItemRank;
		INT64 biLeaderCharacterDBID;
		int iCurMemberCount;
		int iUpkeepCount;
		int iRoomID;
#if defined( PRE_WORLDCOMBINE_PARTY )
		int nWorldSetID;
		int nPrimaryIndex;	// 월드통합파티 기준값(타켓맵)	
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
		bool bCheckComeBackParty;
#endif
#ifdef PRE_ADD_BEGINNERGUILD
		bool bPartyBeginnerGuild;		//초보길드원끼리 파티원인지
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

		void Clear()
		{
			// Zero 초기값들
			PartyID = 0;
			iServerID = 0;
			iLocationID = 0;
			memset( wszPartyName, 0, sizeof(wszPartyName) );
			iTargetMapIndex = 0;
			TargetMapDifficulty = Dungeon::Difficulty::Easy;
			iBitFlag = 0;
			cMinLevel = 0;
			LootRule = ITEMLOOTRULE_NONE;
			LootItemRank = ITEMRANK_D;
			biLeaderCharacterDBID = 0;
			iCurMemberCount = 0;
			iUpkeepCount = 0;
			iRoomID = 0;
			
			// 그외
			iPassword = Constants::INVALID_PASSWORD;
			Type = _NORMAL_PARTY;
			LocationType = LocationType::Village;
			nPartyMaxCount = PARTYMAX;
#if defined( PRE_WORLDCOMBINE_PARTY )
			nWorldSetID = 0;
			nPrimaryIndex = 0;
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
			bCheckComeBackParty = false;
#endif
#ifdef PRE_ADD_BEGINNERGUILD
			bPartyBeginnerGuild = false;
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
		}
	};

	struct MemberData
	{
		INT64 biCharacterDBID;
		WCHAR wszCharName[NAMELENMAX];
		BYTE cLevel;
		BYTE cJob;
		bool bLeader;
#if defined( PRE_WORLDCOMBINE_PARTY )
		int nWorldSetID;
#endif
		bool bCheckComeback;
	};

	struct BitFlag
	{
		enum eCode
		{
			JobDice = 1,			// 직업주사위 우선
			Password = (1<<1),		// 패스워드 설정 파티
			WorldmapAllow = (1<<2),	// 월드존 파티 허용
			VoiceChat = (1<<3),		// 보이스채팅
		};
	};

	struct SortWeightIndex
	{
		enum eCode
		{
			SearchWordTitle = 10001,
			SearchWordStage = 10002,
			SearchWordDifficulty = 10003,
			
			PublicParty = 20001,
			PrivateParty = 20002,

			DifficultyAbyss = 30001,
			DifficultyMaster = 30002,
			DifficultyHard = 30003,
			DifficultyNormal = 30004,
			DifficultyEasy = 30005,
			DifficultyNone = 30006,

			DifficultyAbyssOrderbyAsc = 31001,
			DifficultyMasterOrderbyAsc = 31002,
			DifficultyHardOrderbyAsc = 31003,
			DifficultyNormalOrderbyAsc = 31004,
			DifficultyEasyOrderbyAsc = 31005,
			DifficultyNoneOrderbyAsc = 31006,

			DifficultyAbyssOrderbyDesc = 32001,
			DifficultyMasterOrderbyDesc = 32002,
			DifficultyHardOrderbyDesc = 32003,
			DifficultyNormalOrderbyDesc = 32004,
			DifficultyEasyOrderbyDesc = 32005,
			DifficultyNoneOrderbyDesc = 32006,

			LevelLimit = 40001,

			LevelLimitOrderbyAsc = 41001,
			LevelLimitOrderbyDesc = 42001,

			SameVillage = 50001,
			ManaRidge = 50002,
			PrairieTown = 50003,
			CataractGateWay = 50004,
			SaintHaven = 50005,
			LotusMash = 50006,

			PartyMemberCount = 60001,

			PartyMemberCountOrderbyAsc = 61001,
			PartyMemberCountOrderbyDesc = 62001,

			TargetStage = 70001,

			TargetStageAsc = 71001,

			TargetStageDesc = 72001,

			NestToggle = 100001,
			StageToggle = 100002,
			EtcToggle = 100003,
		};
	};

#if defined( PRE_WORLDCOMBINE_PARTY )
	static bool bIsWorldCombineParty( ePartyType Type )
	{
		switch( Type )
		{
			case _WORLDCOMBINE_PARTY:
				return true;
		}

		return false;
	}
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
};

typedef Dungeon::Difficulty::eCode TDUNGEONDIFFICULTY;
typedef ePartyItemLootRule TPARTYITEMLOOTRULE;
typedef eItemRank TITEMRANK;
typedef INT64 TINSTANCEID;	// PartyID,AccountID,RoomID 등등 다양한 용도에 의해 쓰임

#else

typedef UINT TPARTYID;
typedef char TDUNGEONDIFFICULTY;
typedef BYTE TPARTYITEMLOOTRULE;
typedef BYTE TITEMRANK;
typedef UINT TINSTANCEID;	// PartyID,AccountID,RoomID 등등 다양한 용도에 의해 쓰임

#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_ADD_REVENGE )
namespace Revenge
{
	struct RevengeTarget
	{
		enum eCode
		{
			eRevengeTarget_None = 0,
			eRevengeTarget_Me,
			eRevengeTarget_Target,
		};		
	};

	struct TargetReason
	{
		enum eCode
		{
			SetNewRevengeTarget = 0,	//새로운 리벤지 타겟 대상 등록
			SuccessRevenge = 1,			//리벤지 성공
			TargetLeaveGame = 2,		//리벤지 타겟이 게임에서 나감
			TargetLeaveRoom = 3,		//리벤지 타겟이 PvP 룸에서 나감			
			NotPartedInPlay = 4,		//타겟이 게임에 참여하지 않음(관전자)
			ResetRevengeTarget = 10,	//본인이 게임, 룸 에서 나가거나 관전자라서 리셋
		};

	};
	
	enum
	{
		RevengeRewardMedalCount = 1,	//리번지 성공시 보상 메달 수
		RevengeConditionCount = 3,		//리벤지에 등록되기 위한 킬 수
	};
};
#endif	// #if define( PRE_ADD_REVENGE )

#define CONVERT_TO_DUNGEONDIFFICULTY(n) static_cast<TDUNGEONDIFFICULTY>(n)
#define CONVERT_TO_PARTYITEMLOOTRULE(n) static_cast<TPARTYITEMLOOTRULE>(n)
#define CONVERT_TO_ITEMRANK(n) static_cast<TITEMRANK>(n)

namespace MultiLanguage
{
	//머 예를들어 EU다 하면
#ifdef _EU
	//Test
	struct SupportLanguage
	{
		enum eSupportLanguage
		{
			Eng = 0,		//0 is default
			Ger,
			Fra,
			Esp,
			NationMax,
		};
	};

	static std::string NationString[SupportLanguage::NationMax] = 
	{
		//eSupportNation의 갯수와 짝을 맞춘다~ 로드할때 접미 문장만들기용
		"_ENG",
		"_GER",
		"_FRA",
		"_ESP",
	};
#else		//#ifdef _EU
	//Test
	struct SupportLanguage
	{
		enum eSupportLanguage
		{
			Kor = 0,		//0 is default
			NationMax,
		};
	};

	static std::string NationString[SupportLanguage::NationMax] = 
	{
		//eSupportNation의 갯수와 짝을 맞춘다~ 로드할때 접미 문장만들기용
		"_KOR",
	};
#endif		//#ifdef _EU

	struct Common
	{
		enum eCommon
		{
			SerachCountMax = 10,
		};
	};

	static SupportLanguage::eSupportLanguage eDefaultLanguage = static_cast<SupportLanguage::eSupportLanguage>(0);
};

#ifdef PRE_ADD_48714
namespace AbuseLog
{
	struct Common
	{
		enum eCommon
		{
#ifdef _TW
			AbuseLog_Reached_MailID = 441,			//바뀌지 않는다! 바뀌면 안덴다~
#else		//#ifdef _TW
			AbuseLog_Reached_MailID = 442,			//바뀌지 않는다! 바뀌면 안덴다~
#endif		//#ifdef _TW

		};
	};
};
#endif		//#ifdef PRE_ADD_48714

#ifdef PRE_ADD_49660
// 클러릭 디바인 퍼니쉬먼트 스킬 ID
const int DVINE_PUNISHMENT_SKILL_ID = 3205;
#endif // #ifdef PRE_ADD_49660
#if defined(PRE_ADD_DOORS_PROJECT)
namespace Doors
{
	struct CharacterAbility
	{
		enum eStateType
		{
			Strength =0,		// 힘
			Agility,			// 민첩
			Intelligence,		// 지능
			Stamina,			// 건강
			AttackPMin,			// 물리공격력 Min
			AttackPMax,			// 물리공격력 Max
			AttackMMin,			// 마법공격력 Min
			AttackMMax,			// 마법공격력 Max
			DefenseP,			// 물리방어력
			DefenseM,			// 마법방어력
			Stiff,				// 경직력
			StiffResistance,	// 경직저항
			Critical,			// 크리티컬
			CriticalResistance, // 크리티컬 저항
			Stun,				// 스턴
			StunResistance,		// 스턴 저항
			FinalDamage,		// 파이널 데미지
			FireAttack,			// 불속성 공격력
			IceAttack,		 	// 물속성 공격력
			LightAttack,		// 빛속성 공격력
			DarkAttack,			// 암흑속성 공격력
			FireDefense,		// 불속성 공격력
			IceDefense,		 	// 물속성 공격력
			LightDefense,		// 빛속성 공격력
			DarkDefense,		// 암흑속성 공격력
			MoveSpeed,			// 이동속도
			MaxHP,				// Max HP
			MaxMP,				// Max MP	
			MaxType,			
		};
	};
}
#endif

namespace BeginnerGuild
{
	struct Type
	{
		enum eType
		{
			Normal = 0,
			Beginner = 1,
		};
	};

	struct StringIndex
	{
		enum eStringIndex
		{
			GuildTitle = 1594,			//초보자길드 타이틀 uistring Index
			GuildNotice = 1386,			//초보자길드 공지 uistring Index
			GuildRegisterContents = 1598,
		};
	};

	struct Common
	{
		enum eCommon
		{
			PartyBonusRate = 5,			//초보자길드원끼리 파티시 추가 경험치
		};
	};
}

namespace WillSendMail
{
	struct Common
	{
		enum eCommon
		{
			WIllMailPacketMax = 20,			//뒤집어 쓰겠지?
			WillSendMail_Memo_Size = 20,	//디비는 30으로 잡혀있다.
			WillSendMail_NoRecord_RetCode = 103326,
		};
	};

	struct TWillSendInfo
	{
		int nMailID;
		WCHAR wszMemo[WillSendMail::Common::WillSendMail_Memo_Size];
	};
};

namespace Doors
{
	struct Common
	{
		enum eCommon
		{
			AuthenticationKey_LenMax = 6,
		};
	};
};

#if defined( PRE_ADD_LIMITED_SHOP )
namespace LimitedShop
{
	struct LimitedItemData
	{
#if defined( PRE_FIX_74404 )
		int nShopID;
#endif // #if defined( PRE_FIX_74404 )
		int nItemID;
		int nBuyCount;
		int nResetCycle;
	};
	struct LimitedItemType
	{
		enum eCycle
		{
			Day = 1,
			Week = 2,
		};
	};	
}
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
namespace PrivateChatChannel
{
	struct Common
	{
		enum eCommon
		{
			WaitDBResult = -1,
			MinNameLen = 2,
			MaxNameLen = 11,
#if defined( _WORK )
			MaxMemberCount = 10,
#else
			MaxMemberCount = 100,
#endif
			GetDBMaxChannel = 100,
			MAXWORLDCOUNT = 5,
		};
		enum eModType
		{
			ChangePassWord = 1,
			ChangeMaster,
			ExitMember,
			OutMember,				
			KickMember,
			JoinMember,
			EnterMember,			
		};
		enum eReturnType
		{
			NeedPassWord = 1,
		};
	};
	struct TMemberInfo
	{
		bool bMaster;
		INT64 biCharacterDBID;
		WCHAR wszCharacterName[NAMELENMAX];
	};
}
#endif
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
namespace TotalLevelSkill
{
	struct Common
	{
		enum eCommon
		{
			MAXSLOTCOUNT = 4,
			MAX_STATE_EFFECT_COUNT = 7,
		};		
		enum eVillageEffectType
		{
			RepuTationIncrease = 260,	// 호감도 증가
			GrowingBoost = 262,			// 농장 재배시간 단축
			LadderPointSale = 263,		// 래더포인트 할인
			CookExpIncrease = 265,		// 요리숙련도 증가
		};
	};
	struct TTotalLevelSkillData
	{
		int nSlotIndex;
		int nSkillID;
	};
}
#endif

namespace SpecialBox
{
	struct Common
	{
		enum eCommon
		{
			ListMax = 40,				// 리스트 최대값
			RewardAllMax = 10,			// 보상 최대값
			RewardSelectMax = 50,		// 선택보상 최대값
		};

		enum eRequestDelay
		{
			DelayTime = 5,
		};
	};

	struct TargetTypeCode
	{
		enum eTargetTypeCode
		{
			AccountAll = 1,			// 1=계정전체
			AccountSelect = 2,		// 2=계정지정
			CharacterAll = 3,		// 3=캐릭터전체
			CharacterSelect = 4,	// 4=캐릭터지정
			ConditionSelect = 5,	// 5=조건지정
		};
	};

	struct ReceiveTypeCode
	{
		enum eReceiveTypeCode
		{
			All = 1,
			Select = 2,
		};
	};
};

#if defined( PRE_WORLDCOMBINE_PVP )

namespace WorldPvPMissionRoom
{
	struct Common
	{
		enum eCommon
		{
			MaxDBDataCount = 100,
			GetWorldPvPRoomTick = 30*1000,
			WaitStartWorldPvPRoomSecond = 10,
			WorldPvPRoomStartIndex = PvPCommon::Common::MaxRoomCount,			
			GMWorldPvPRoomStartIndex = PvPCommon::Common::MaxRoomCount + 100,
		};
		enum eReqType
		{
			NoneType = 0,
			MissionRoom,
			GMRoom,
		};		
	};
}

#endif

namespace Login
{
	struct CharacterSortCode
	{
		enum eCharacterSortCode
		{
			None = 0,
			World = 1,		// 1=월드, 
			Create = 2, 	// 2=생성, 
			Name = 3,		// 3=이름, 
			Level = 4,		// 4=레벨, 
			Job = 5,		// 5=직업, 
			LastLogin = 6,	// 6=최근 접속
			Max,
		};
	};
};

namespace CostumeMix
{
	struct RandomMix
	{
		enum eOpenType
		{
			OpenFail = -1,
			OpenByItem = 0,
			OpenByNpc
		};

		enum eConst
		{
			MixerCanMixPartsAll = -1,
			MixerCanMixBasicParts = 0,
		};
	};
}

#if defined(PRE_ADD_ACTIVEMISSION)
namespace ActiveMission
{
	enum ActiveMissionGainRate
	{
		NoneTypeActiveMissionGainRate = 13,
		NoneTypeActiveMissionGainBonusRate = 15,
		NestTypeActiveMissionGainRate = 5,
		NestTypeActiveMissionGainBonusRate = 8,
	};	

	enum ActiveMissionState
	{
		NOT_ASSIGNED = 0,
		GAIN_MISSION = 1,
		COMPLETE_MISSION = 2,
	};
};
#endif

#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
namespace PropDef
{
	struct Option
	{
		enum eTypeWithPriority
		{
			eMinVisibleOption,
			eRandomCreate = eMinVisibleOption,
			eTrigger,
			eMaxVisibleOption
		};

		enum eValue
		{
			NotSet = -1,
			False,
			True,
		};
	};
}
#endif

namespace Actoz
{
	struct ShutdownType
	{
		enum eShutdownType
		{
			Force = 1,		// 강제적
			Selective = 2,	// 선택적
		};
	};
}

namespace MapGateCondition
{
	//오픈 시간 설정
	enum eOpenDateTimeType
	{
		oDailyHours = 0,	//매일 특정시간에만 열림
		oDayOfWeek = 1,	//지정한 요일에만 열림(0~6 : 일~토)
		oDayOfWeekAndHours = 2,	//특정 요일 & 시간
		oDayOfWeekDuringFatigueTime = 3, //특정 요일 부터 피로도 리셋 시간까지
	};

	//클로즈 시간 설정
	enum eCloseDateTimeType
	{
		cMonthlyHours = 10, //특정 날짜 & 시간
	};
}

#ifdef PRE_ADD_SERVER_WAREHOUSE
namespace Storage
{
	struct WorldServerStorage
	{
		enum eChargeType
		{
			eFeeCharge,
			eFree
		};

		enum eInvenType
		{
			eNormal,
			eCash
		};
	};
}
#endif

#if defined( PRE_ADD_GAMEQUIT_REWARD )
namespace GameQuitReward
{
	struct RewardType
	{
		enum eType		// 게임 종료 보상
		{
			None,					//아무 보상없음
			NewbieReward,			//신규 유저 보상
			ComeBackReward,			//귀환자 보상
		};
	};
}
#endif	// #if defined( PRE_ADD_GAMEQUIT_REWARD )

#if defined( PRE_ALTEIAWORLD_EXPLORE )
namespace AlteiaWorld
{
	struct Info
	{
		enum eInfoType
		{
			JoinInfo,
			PrivateGoldKeyRankInfo,
			PrivatePlayTimeRankInfo,
			GuildGoldKeyRankInfo,
			SendTicketInfo,
		};
	};	

	struct Common
	{
		enum eCommon
		{
			MaxRankCount = 10,				// 랭킹 최대 수
			MaxSendCount = 10,				// 입장권 선물 최대갯수
			MoveNextMapTick = 10000,			// 주사위 굴리고 다음맵으로 이동하기까지의 시간
		};
	};

	struct ResetType
	{
		enum eResetType
		{
			DailyPlayCount = 1,	// 일일 플레이횟수
			WeeklyPlayCount,	// 주간 플레이횟수
			SendTicketList,		// 입장권 보낸 목록
		};
	};

	struct GoldKeyRankMemberInfo
	{
		int nRankIndex;
		BYTE cJob;
		WCHAR wszCharacterName[NAMELENMAX];
		WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
		int nGoldKeyCount;
	};

	struct PlayTimeRankMemberInfo
	{
		int nRankIndex;
		BYTE cJob;
		WCHAR wszCharacterName[NAMELENMAX];
		WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
		UINT nPlaySec;
	};

	struct GuildGoldKeyRankInfo
	{
		int nRankIndex;
		int nGuildID;
		WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
		short wGuildMark;					// 길드마크 문양
		short wGuildMarkBG;					// 길드마크 배경
		short wGuildMarkBorder;				// 길드마크 테두리				
		int nGoldKeyCount;
	};
}
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
namespace StampSystem
{
	struct Common
	{
		enum eCommon
		{
			MaxWeekDayCount = 7,	// 요일 갯수 
			MaxChallengeCount = 8,  // 도전과제 최대갯수
			MaxStampSlotCount = MaxChallengeCount * MaxWeekDayCount, // 스탬프 슬롯 갯수
		};
	};

	struct StampCompleteInfo
	{
		BYTE cChallengeIndex;	// 도전과제 Index
		int nWeekDay;			// 요일 (일=1,월=2,화=3,수=4,목=5,금=6,토=7)
	};
};
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_ADD_DWC)
namespace DWC
{
	enum eDWCWorld
	{
		DWCWorldID = 1,
	};

	enum eDWCMapID
	{
		DWCMapID = 44,
	};

	enum eDWCMember
	{
		DWC_MAX_MEMBERISZE = 3,
	};

	enum eDWCCharLevel
	{
		DWC_MAX_LEVEL = 40,
	};

	struct Common
	{
		enum eCommon
		{
			RankPageMaxSize = RankingSystem::RANKINGMAX,
		};
	};

	enum DWCStatus
	{
		STATUS_NONE = 0, // 대회 기간이 아님.
		STATUS_NORMAL = 1,
		STATUS_PAUSE = 2,
		STATUS_FORCE_TERMINATED = 3,
	};

	struct FindRankType
	{
		enum eFindRankType
		{
			TeamName = 0,
			CharacterName,
		};
	};
}
#endif

#if defined(PRE_ADD_GUILD_CONTRIBUTION)
namespace GuildContribution
{
	struct TGuildContributionRankingData
	{
		INT64 biCharacterDBID;
		int nRank;
		WCHAR wszCharacterName[NAMELENMAX];
		BYTE cJobCode;
		int nWeeklyGuildContributionPoint;
	};
}
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)

#if defined( PRE_PVP_GAMBLEROOM )

const int SENDGAMBLEROOMWAITTICK = 5 * 60 * 1000;	// 5분

namespace PvPGambleRoom
{
	enum eGambleType
	{
		NoneType = 0,
		Gold,
		Petal,
	};
	enum eType
	{
		firstWinner = 0,
		SecondWinner = 1,
		Max = 2,	// 보상 받는 유저수
	};
}
#endif // #if defined( PRE_PVP_GAMBLEROOM )

#if defined(PRE_ADD_EQUIPLOCK)
namespace EquipItemLock
{
	enum eLockStatus
	{
		None = 0,
		Lock = 1,
		RequestUnLock = 2,
	};

	struct TLockItem
	{
		EquipItemLock::eLockStatus eItemLockStatus; //0 : 아무상태도 아님, 1 잠금, 2 잠금해제
		__time64_t tUnLockDate;	//잠금 상태면 잠금일, 잠금 해제일이면 잠금 해제일로 사용함
		__time64_t tUnLockRequestDate;	//잠금 요청일
	};

	struct TLockItemInfo
	{
		BYTE cItemSlotIndex;
		TLockItem ItemData;
	};
}
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

#if defined(PRE_ADD_WORLD_MSG_RED)
namespace WorldChatUseItem
{
	struct CashItemTypeParam
	{
		enum eItemTypeParam
		{
			BlueBird = 1,
			YellowBird = 2,
			RedBird = 3,
		};
	};
}
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)

#if defined( PRE_ADD_MAINQUEST_UI )
namespace QuestRenewal
{
	struct UIStyle
	{
		enum eStyle
		{
			Basic = 0,
			Renewal = 1,
		};
	};

	struct SelectType
	{
		enum eType
		{
			Select = 0,		// 선택지

			Continue = 2,	// 계속
			Accept = 3,		// 수락
			Complete = 4,	// 완료
			Finish = 5,		// 대화종료
			Return = 6,		// 돌아가기
		};
	};
}
#endif // #if defined( PRE_ADD_MAINQUEST_UI ))