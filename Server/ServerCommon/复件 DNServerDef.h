#pragma once

#include "DNPacket.h"
#include "MemPool.h"

/*--------------------------------------------------------------------------
					Server�ʿ��� ���� ���͵�(?)
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
const char TIMELENMAX = 20;				// ����ƮŸ�� Ÿ��

const int LOGINCOUNTMAX				= 64;			// �� �α��� ���� ����
const int LOGINCONCOUNTINFOTERM		= (1*30*1000);	// �α��� ���� ���� ���������� ���� ������ ������ ���� ���Ѱ��� (����:ms)
const int LOGINSERVERLISTSORTTERM	= (1*3*1000);	// �α��� ���� ���� ���� ���Ѱ��� (����:ms)
const int MASTERCOUNTMAX			= 10;			// �� ������ ���� ����

const int  DELETECHAR_WAITTIME_MINUTE	= 7*24*60;	// 7��
const BYTE PROCESSCOUNT = 40;	// �ʴ� ������Ʈ ó������

const int INTERNALBUFFERLENMAX = 51200;

const UINT PROCESSBUFFERLENMAX = 1024 * 64;
const UINT WORKERBUFFERLENMAX = 1024 * 64;

const int QUESTSIZEINFO = 2775;

const int QUERYOVERTIME = 1000;

const int QUERYNAMESIZE = 200; // ���� �ִ� ũ��
const int GAMEDELAYSIZE = 200; // ���ӵ����� �α� �ִ� ũ��

const int SERVERDETACHPACKETSIZE = 1024*40;	// ��Ŷ���۽� ���ۻ�����ũ��
//const variable.
#ifdef _FINAL_BUILD
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_PARTY = 120 * 1000;								//���� ���Ӽ��� ����Ʈ ��� �ð�
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_SINGLE = 120 * 1000;
const unsigned long WAIT_FOR_ANOTHER_USER_TIME_LIMIT = 15 * 1000;						//�����ο��� ������ �ϰ� �ٸ��ο��� ��ٸ��� �ð�
const unsigned long WAIT_FOR_LOAD_TIME_LIMIT = 60 * 1000;								//����Ÿ���̽��� �ε��� ��ٸ��� �ð�
const unsigned long GOGO_SING_TO_PLAY_FOR_WAIT_TIME_LIMIT = 2 * 60 * 1000;					//Ŭ���̾�Ʈ �ε��� ��ũ�� ��ٸ��� �ð�
const unsigned long GOGO_SING_TO_FARMPLAY_FOR_WAIT_TIME_LIMIT = 5 * 60 * 1000;			//����ε� �ð��� ��ŭ ������
#else
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_PARTY = 60 * 1000;								//���� ���Ӽ��� ����Ʈ ��� �ð�
const unsigned long RUDP_CONNECT_TIME_LIMIT_FOR_SINGLE = 120 * 1000;
const unsigned long WAIT_FOR_ANOTHER_USER_TIME_LIMIT = 60 * 1000;						//
const unsigned long WAIT_FOR_LOAD_TIME_LIMIT = 150 * 1000;								//
const unsigned long GOGO_SING_TO_PLAY_FOR_WAIT_TIME_LIMIT = 5 * 60 * 1000;				//
const unsigned long GOGO_SING_TO_FARMPLAY_FOR_WAIT_TIME_LIMIT = 5 * 60 * 1000;			//����ε� �ð��� ��ŭ ������
#endif

const int EACH_GAMESERVER_MAX_ROOM_COUNT = 20;
const int EACH_GAMESERVER_MAX_USER_COUNT = 100;

const int INTERVALDELETE_TIMINGTICK = 1000 * 60;		//1����??...-_-;
const int CHECK_ZOMBIE_DISCONNECT_TICK = 1000 * 60 * 5;
const int DISTRIBUTE_IDLE_TICK = 1000 * 5;

const int DELAYED_PRCESS_CHECKTICK = 1000 * 10;
const int DELAYED_PRCESS_LOG_COUNT = 10;
const int FRAMEDEADLINE = 17;

const int EVENTMAPSTRMAX = 50;

const int CHECKLIVETICK = 60000 * 2;//  * 5;	// �ٽ��� 5�� ���� üũ -> 1������ �ٲٶ�� �ؼ� �ٲ� -> 2������ �ø�
const int CHECKRESPONSETICK = 60000 * 3;		// �ٽ��� ���� �ð� (3������?)

const int CHECKGPKFIRSTTICK = 1000 * 60 * 2;	// ���� �˻�Ÿ�̹�
const int CHECKGPKTICK = 60000;
const int MANAGEDMAX = 20;

#if !defined(_FINAL_BUILD)
const DWORD CHECK_TCP_PING_TICK	= 1000*30*1;	// 1��
const DWORD CHECK_UDP_PING_TICK = 1000*30*1;	// 1��
#else		//#if !defined(_FINAL_BUILD)
const DWORD CHECK_TCP_PING_TICK	= 1000*60*1;	// 1��
const DWORD CHECK_UDP_PING_TICK = 1000*60*1;	// 1��
#endif		//#if !defined(_FINAL_BUILD)

#if defined(_CH)
const int CHECKLOGINUSERKICKTICK = 30 * 60 * 1000;		// �α��� ������ 30�� ���� �ӹ��� ����!(�߱���)
#else
const int CHECKLOGINUSERKICKTICK = 15 * 60 * 1000;		// �α��� ������ 15�� ���� �ӹ��� ����!
#endif //#if defined(_CH)
const int CHECKNOTAUTHLOGINUSERKICKTICK = 5 * 60 * 1000;  // �α��� ������ �������� ����ü 5�� ���� �ӹ��� ����!

const unsigned int DEFAULTUSERSESSIONID = 10000;

const unsigned int CASHDELUSERDELAY_LIMITTICK = (4*30*1000);

// DB �����۾� ���� ����
const int DBQUERYSTREAMDATAMAX			= (36*1024);	// DB �����۾� ������ �ִ� ũ��			: DB ������ �ٷ� �۽ŵǴ� ��� ť�� ���� �� �ϰ�ó���� �ʿ��� ������ ����ü �� ���� ū ���� �������� ���� �� ũ�Ⱑ ������ ��� �������� ������ �ʿ� ����
const unsigned int DBQUERYAUTOCHKSUM	= 0xE41C723F;	// DB ���� ����ȭ ó�� �� üũ��

const int QUESTINVENBLANKCHECKMIN	= 2;				// ����Ʈ �κ��丮 üũ �� �޽��� �߼��� �Ǵ� �ּ� ����
const float MAX_ITEMPRICE_RATE = 0.5f;

#if defined(_FINAL_BUILD)
const int BEGINAUTHLIMITSEC			= (9*10);			// ���� ���� ���ѽð� (����:��, 0 ���ϴ� ������ ��� ?)
const int CHECKAUTHLIMITSEC			= (6*10);			// �Ϲ� ���� ���ѽð� (����:��)
const int BEGINAUTHLIMITTERM		= (BEGINAUTHLIMITSEC*1000);		// ���� ���� ���Ѱ��� (����:ms)
//const int CHECKAUTHLIMITTERM		= (CHECKAUTHLIMITSEC*1000);		// �Ϲ� ���� ���Ѱ��� (����:ms)
const int CHECKAUTHLIMITTERM		= (1*30*1000);		// �Ϲ� ���� ���Ѱ��� (����:ms)		// P.S.> ����, PVP �� ������ �����ڵ��� ��������� ����Ͽ� ���� �ð����� ª�� ����
#else	// _FINAL_BUILD
const int BEGINAUTHLIMITSEC			= (6*10);			// ���� ���� ���ѽð� (����:��, 0 ���ϴ� ������ ��� ?)
const int CHECKAUTHLIMITSEC			= (4*10);			// �Ϲ� ���� ���ѽð� (����:��)
const int BEGINAUTHLIMITTERM		= (BEGINAUTHLIMITSEC*1000);		// ���� ���� ���Ѱ��� (����:ms)
//const int CHECKAUTHLIMITTERM		= (CHECKAUTHLIMITSEC*1000);		// �Ϲ� ���� ���Ѱ��� (����:ms)
const int CHECKAUTHLIMITTERM		= (1*20*1000);		// �Ϲ� ���� ���Ѱ��� (����:ms)		// P.S.> ����, PVP �� ������ �����ڵ��� ��������� ����Ͽ� ���� �ð����� ª�� ����
#endif	// _FINAL_BUILD

const int RESETAUTHLISTMAX			= (1000);			// �������� ��� �ʱ�ȭ ũ�� (����!!! - �� ��������� ����ϴ� ��Ŷ���� �۽�/���� ��Ŷ ũ�⸦ ���� �ʾƾ� ��)
const int RESETAUTHLISTTERM			= (1*1000);			// �������� ��� �ʱ�ȭ �۾��ֱ� (����:ms)
const int RESETAUTHSERVERTERM		= (5*1000);			// �������� ���� �ʱ�ȭ �۾��ֱ� (����:ms)
const int RESETAUTHWORLDCHECKTERM	= (5*1000);			// �������� ���� �ʱ�ȭ �۾��ֱ� (����:ms)
const int RESETAUTHWORLDLIMITTERM	= (2*30*1000);		// �������� ���� �ʱ�ȭ ���ѽð� (����:ms)

const int GETAUTHUSERCOUNT = 1000 * 10;					// ������������ ī��Ʈ ȹ���ֱ�

const int GUILDMGRCHKTERM = (5*1000);					// ��� ������ üũ �ֱ�
const int GUILDMGRUPDTERM = (10*1000);					// ��� ������ ���� �ֱ�
const int GUILDCHKMEMBTERM = (30*1000);					// ��� ������� üũ �ֱ�
const int GUILDCHKRESCTERM = (30*1000);					// ��� �ڿ��ݳ� üũ �ֱ�
const int GUILDREQINFOTERM = (60*1000);					// ��� ������û üũ �ֱ�
const int GUILDRFSVIEWTERM = (10*1000);					// ��� �ð����� ���� �ֱ�
const int GUILDAPLPOINTTERM = (10*1000);				// ��� �Ϲ�/���� ����Ʈ DB ���� �ֱ�
const int GUILDCHKRESCLIMIT = (2*30*1000);				// ��� �ڿ��ݳ� üũ ����

const int PERIODQUESTTERM = (60*1000);					// �Ⱓ�� ����Ʈ ���� �ֱ�

const int PERIODGUILDWARTERM = (60*1000);				// ����� ���� �ֱ�
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
	SERVERTYPE_LOGIN,		// �α��� ����
	SERVERTYPE_DB,			// DB ����
	SERVERTYPE_VILLAGE,		// ������ ����
	SERVERTYPE_GAME,		// ���� ����
	SERVERTYPE_MASTER,		// ������ ����
	SERVERTYPE_LOG,			// �α� ����
	SERVERTYPE_MAX,
};

enum ePatchType
{
	PATCHTYPE_LOGIN,		// �α��� ����
	PATCHTYPE_DB,			// DB ����
	PATCHTYPE_VILLAGE,		// ������ ����
	PATCHTYPE_GAME,			// ���� ����
	PATCHTYPE_MASTER,		// ������ ����
	PATCHTYPE_LOG,			// �α� ����
	PATCHTYPE_PATCHER,		// ESM
	PATCHTYPE_CASH,			// ĳ�� ����
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
	CONNECTIONKEY_AUTH,		// ������(�ؽ�)
	CONNECTIONKEY_CASH,
	CONNECTIONKEY_BILLING,	// ������(�ۺ���)
	CONNECTIONKEY_SERVICEPATCHER,
	CONNECTIONKEY_SERVERMONITOR,
	CONNECTIONKEY_TW_AUTHLOGIN,	// ������(�����Ͼ� 1 - Authorization : ����� ���� ���� ó��, ���� (��� ����, ĳ�� �ܿ� ����Ʈ ?) üũ)
	CONNECTIONKEY_TW_AUTHLOGOUT,	// ������(�����Ͼ� 2 - Logout : ����� �α׾ƿ� ó��)
	CONNECTIONKEY_TW_QUERY,
	CONNECTIONKEY_TW_SHOPITEM,
	CONNECTIONKEY_TW_COUPON,	// �����Ͼ� ��������
	CONNECTIONKEY_TW_COUPON_ROLLBACK,	// �����Ͼ� �����ѹ�..¡�׷���͵� ������ ��Ʈ���� ����ٴ�..��
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

enum eUserState		// login���� ���� ���°�
{
	STATE_NONE,

	// login
	STATE_CHECKVERSION,
	STATE_CHECKLOGIN,		// ���̵� �н����� Ȯ�λ���
	STATE_SERVERLIST,
	STATE_WAITUSER,			// �������
	STATE_CHARLIST,			// ĳ���� ����Ʈ ����
	STATE_CHANNELLIST,
	STATE_CONNECTVILLAGE,	// ������ ������ ���Ӱ��ɻ���
	STATE_CONNECTGAME,		// �� ������ ���Ӱ��ɻ���
	STATE_RECONNECTLOGIN,	// ��Ŀ��Ʈ ����

	// master
	STATE_CHECKVILLAGE,		//�������� �Ѿ������ �����Դϴ�. ������ ������ ��ư� �ǹǷ� �� ������Ʈ�� ���ؼ� �����˴ϴ�.
	STATE_CHECKGAME,		//�������� �Ѿ�� ���� �����Դϴ�. ��
	STATE_CHECKRECONNECTLOGIN,	//ĳ���ͼ���â���� �Ѿ�� ���� �����Դϴ�. ��
	STATE_VILLAGE,			// village�����ΰ�
	STATE_GAME,				// game�����ΰ�
	STATE_LOGIN,			// login�����ΰ�

	// village
	STATE_LOADUSERDATA,		// ���������� DB���� �ҷ��Դ���
	STATE_READY,			// �ʵ� ���� �غ� �Ϸ�
	STATE_READYTOVILLAGE,	// village �� ���� �ű涧 (�غ�ܰ�)
	STATE_READYTOGAME,		// game���� �Ѿ�� (�غ�ܰ�)
	STATE_READYTOLOGIN,		// login���� �Ѿ�� (�غ�ܰ�)
	STATE_MOVETOVILLAGE,	// village �� ���� �ű涧
	STATE_MOVETOGAME,		// game���� �Ѿ��
	STATE_MOVETOLOGIN,		// login���� �Ѿ��
	STATE_MOVESAMECHANNEL,	// ���� ä�� �̵�

#if defined(_HSHIELD)
	HSHIELD_NONE = 0,
	HSHIELD_RESPONSEVERSION = 1,	// checkversion ��Ŷ ���� ����
	HSHIELD_RECONNECTLOGIN,		// ������ ��Ŷ ���� ����
	HSHIELD_LOADUSER,			// ��� �ε� ��������
	HSHIELD_REQUEST,			// _AhnHS_MakeRequest ��û����
	HSHIELD_RESPONSE,			// ����� ����
#endif	// _HSHIELD

};

//enumulation
//GameRoom
enum eGameRoomState
{
	_GAME_STATE_NONE,					//����!
	_GAME_STATE_READY2CONNECT,			//�ʱ�ȭ �Ϸ� ���ؼ� ���! ����
	_GAME_STATE_CONNECT2CHECKAUTH,		//DB �� �̿��� �������� üũ
	_GAME_STATE_CONNECT2LOAD,			//���� �� ���� �Ϸ�! �ε���·� ����
	_GAME_STATE_LOAD2SYNC,				//Ŭ���̾�Ʈ ����Ϸ� �غ����
	_GAME_STATE_SYNC2SYNC,
	_GAME_STATE_SYNC2PLAY,				//�÷��̴����
	_GAME_STATE_PLAY,					//������!

	_GAME_STATE_CANCEL_LOADING,			//��׶��� �δ��� ��ũ
	_GAME_STATE_DESTROYED,				//�������!
	_GAME_STATE_FARM_NONE,				//����� ������Ʈ�Դϴ�. ���� ��ƾ�� �޶� �Ϲݰ� �ٸ��� �귯���ϴ� _GAME_STATE_PLAY���ʹ� ����
	_GAME_STATE_FARM_READY2LOAD,		//��� ������� ����Ÿ�� ��û�� ����
	_GAME_STATE_FARM_LOAD2PLAY,			//��񿡼� ������� ����Ÿ�� �޾Ҵ�. �����ϰ� �����Ϳ� �˸���
	_GAME_STATE_FARM_PAUSE,				//���� ������~
	_GAME_STATE_PVP_SYNC2GAMEMODE,		//�Ǻ����ϰ�� ������ ���� ���¿��� ���۽����� ����(�����) ���ӽ�����Ʈ�� �ǳʶٰ� ���ް� �ε�� �Ѿ�� �߰�������Ʈ
};

//GameSession
enum eGameSessionState
{
	SESSION_STATE_NONE,
	SESSION_STATE_READY,				//��ü������ �Ǿ� ����! ������ �ȵǾ� ����
	SESSION_STATE_RUDP_CONNECTED,	
	SESSION_STATE_CONNECTED,			//���� �Ǿ���!
	SESSION_STATE_LOAD,					//���� �Ǹ� ��� ��û�մϴ�.
	SESSION_STATE_LOADED,				//���ӷε��Ϸ�!
	SESSION_STATE_WAIT_TO_READY,		//Ready ���� ��ٸ�
	SESSION_STATE_READY_TO_SYNC,		//Sync �غ� �Ϸ�!
	SESSION_STATE_SYNC_READY_2_DELAY,	//������ �޼���ó���غ� �ȵǻ��¿��� �޼����� �޾ҽ��ϴ�.
	SESSION_STATE_READY_TO_PLAY,		//Sync 2 Play
	SESSION_STATE_GAME_PLAY,			//�����÷�����!
	SESSION_STATE_READY_TO_VILLAGE,		//game -> village (�غ�ܰ�)
	SESSION_STATE_READY_TO_LOGIN,		//game -> login char select (�غ�ܰ�)
	SESSION_STATE_RECONNECTLOGIN,		//game -> login char select
	SESSION_STATE_SERVER_CHANGE,		//�����̵���! �̵��Ϸ�Ǹ� ����
	SESSION_STATE_DISCONNECTED,			//©������! (tcp���� ���� ���� ����)
	//SESSION_STATE_CRASH,				//ũ����!
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
	WEAPON_SWORD,			// ���
	WEAPON_GAUNTLET,		// ��Ʋ��
	WEAPON_AXE,				// ����
	WEAPON_HAMMER,			// �ظ�
	WEAPON_SMALLBOW,		// �ұ�
	WEAPON_BIGBOW,			// ���
	WEAPON_CROSSBOW,		// ����
	WEAPON_STAFF,			// ������
	WEAPON_BOOK,			// ������
	WEAPON_ORB,				// ������
	WEAPON_PUPPET,			// �ּ�����
	WEAPON_MACE,			// ���̽�
	WEAPON_FLAIL,			// ������
	WEAPON_WAND,			// �ϵ�
	WEAPON_SHIELD,			// ����
	WEAPON_ARROW,			// ȭ��
	WEAPON_CANNON,			// ĳ��
	WEAPON_BUBBLEGUN,		// �����
	WEAPON_GLOVE,			// �۷���
	WEAPON_FAN,             // ��ä
	WEAPON_CHAKRAM,         // ��ũ��
	WEAPON_CHARM,           // ��
	WEAPON_SCIMITER,		// �ù���
	WEAPON_DAGGER,			// ���
	WEAPON_CROOK,			// ũ��
};

enum eParts
{
	PARTS_FACE,				// 
	PARTS_HAIR,
	PARTS_HELMET,			// ����
	PARTS_BODY,				// ��ü
	PARTS_LEG,				// ��ü
	PARTS_HAND,				// ��
	PARTS_FOOT,				// ��
	PARTS_NECKLACE,
	PARTS_EARRING,
	PARTS_RING,
	PARTS_RING2,
	PARTS_CREST,
	PARTS_FULLSET,			// Ǯ��Ʈ
};

enum eQuestDataType
{
	QUESTDATA_STATE = 1,	// ����Ʈ ����
	QUESTDATA_STEP,			// ���� ����Ʈ ����
	QUESTDATA_JOURNAL,		// ���� ����Ʈ ����
};

enum eEtcType	//  DNEtc table Type
{
	ETC_NONE,
	ETC_QUICKSLOT,			// TQuickSlot QuickSlot[QUICKSLOTMAX];		// ���� ����â (10���� 2��)
	ETC_JOB,				// USHORT wJobArray[JOBMAX];				// ���� (0: ��ó�� ����, ������ ����~ ���� ����)
	ETC_NOTIFIER,			// int Notifier[Notifier_MaxCount];	// �˸��� (quest, mission)
	ETC_DUNGEONCLEAR,		// TDungeonClearData DungeonClear[DUNGEONCLEARMAX];	// ���� Ŭ���� (120��)
	ETC_ETERNITYITEM,		// USHORT wEternityItem[ETERNITYITEMMAX];	// ����������
	ETC_EQUIPDELAYTIME,		// int nEquipDelayTime[EQUIPMAX];			// equip index�� ���õ� cooltime
	ETC_EQUIPREMAINTIME,	// int nEquipRemainTime[EQUIPMAX];
};

enum eAuthType
{
	AUTHTYPE_BEGINAUTH,		// ���� ����
	AUTHTYPE_STOREAUTH,		// ���� ����
	AUTHTYPE_CHECKAUTH,		// ���� üũ
	AUTHTYPE_RESETAUTH,		// ���� ����
	AUTHTYPE_CNT,
};

enum eAbuseCode
{
	ABUSE_NONE = 0,
	ABUSE_MOVE_SERVERS,		// ���� <-> ���� ����
	ABUSE_MOVE_INSKY,		// �����̵�
	ABUSE_HACKSHIELD,		// �ٽ��忡�� �����̿���
	ABUSE_TWN_EXTENDLOG,	// �븸�� �߰� �αװ� #48714 ���� ����
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	ABUSE_CHARACTER_CHECKSUM, // #80315 ĳ���� üũ�� �̻� �߰�
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
			SuccessPayment = 1,		// �������� & pickup ���
			Success = 2,			// ���ſϷ�
			FailPayment = 3,		// ��������
		};
	};
};

namespace DBDNWorldDef
{
	struct CharacterExpChangeCode
	{
		enum eCharacterExpChangeCode 
		{
			Quest = 1,			// : ����Ʈ ����	-> FKey = QuestID
			Dungeon = 2,		// : ���� ����		-> FKey = PartyID or NULL
			Cheat = 3,			// : ġƮ			-> FKey = NULL
			Admin = 4,			// : ���� ����
			Eternity = 5,		// ����������		-> FKey = ItemSerial
			PvP	= 6,			// : PvP �Ƿε� ���� -> FKey = NULL
			DungeonMonster = 7,	// :���� ���� ��� ����ġ -> FKey = PartyID or NULL( �������� �����ڷ� ����..DB�� �Ѿ���� Dungeon ���� Convert �ȴ�. )
			Item = 8,			// : ����ġ���� ������ -> FKey = ItemID
		};
	};

	struct CharacterLevelChangeCode
	{
		enum eCharacterLevelChangeCode
		{
			Normal = 1,		// : �Ϲ�
			Cheat = 2,		// : ġƮ
			Admin = 3,		// : ������ ����
		};
	};

	struct CoinChangeCode
	{
		enum eCoinChangeCode	// ���� : ���� ����, ����÷��, â�� �����, ������ ���ȱ�, ��ȯ �����, ��ȯ ������, ��ȭ ������, ��ų ���, ���� ����, ���� ������ �� ��� ������ SP�� ���ԵǾ� �ֽ��ϴ�.
		{
			DoNotDBSave = 0,
			QuestReward = 1,					// : ����Ʈ ���� (FKey: QuestID)
			PickUp = 2,							// : �Ⱦ� (FKey: PartyID or NULL)
			Cheat = 3,							// : ġƮ (FKey: NULL)
			Admin = 4,							// : ������ ���� (FKey: AuditLogID)
			Use = 5,							// : ��� (FKey: NULL)
												// 6: ����۽�÷�� (FKey: MailID)
												// 7: �������÷�� (FKey: MailID)
			InvenToWare = 8,					// : â�� �Ա� (FKey: NULL)
			WareToInven = 9,					// : â�� ��� (FKey: NULL)
												// 10: ������ ���ȱ� (FKey: NpcID)
												// 11: ��ȯ �Ա� (FKey: CharacterID)
												// 12: ��ȯ ��� (FKey: CharacterID)
			QuestSub = 13,						// ����Ʈ ���� (FKey: QuestID)
			RepairItem = 14,					// ������ ���� (FKey: ItemSerial)
												// 15: ��ȯ ������ (FKey: NULL)
												// 16: ��ȭ ������ (FKey: ItemEnchantLogID)
												// 17: ��ų ��� (FKey: SkillID)
												// 18: ���� ���� (FKey: ItemSerial)
			DisjointTax = 19,					// ���ؼ����� (FKey: NULL)
												// 20: ���� ������ (FKey: MailID)
												// 21: ���λ������ ������ (FKey: TradeID)
												// 22: ���λ��� ������ �� (FKey: TradeID)��
												// 23: ���λ��� ������ �Ǹű� (FKey: TradeID)
												// 24: ���λ��� �ŷ� ���� ������ (FKey: TradeID)
			CompoundTax = 25,					// ���ռ����� (FKey: NULL)
												// 26: ĳ���� ���� (FKey: NULL)
			MaxLevelExperienceTransGold = 27,	// ���� ����ġ ��� ���� (FKey: NULL)
			Present = 28,						// ���� (FKey: NpcID)
												// 29: ��� â�� (FKey: GuildID)
			GuildLevelup = 30,					// 30: ��� ���� �� (FKey: GuildID)
			InvenToGuildWare = 31,				// 31: ��� â�� �Ա� (FKey: GuildID)
			GuildWareToInven = 32,				// 32: ��� â�� ��� (FKey: GuildID)
			GuildRewardBuy = 33,				// 33: ��� ���� ���� (FKey: GuildItemID)
			Donation = 34,						// ���
												// 35: (DOORS) ��ϼ�����
												// 36: (DOORS) ����������
												// 37: (DOORS) ����������
												// 38: (DOORS) ��������
												// 39: (DOORS) ��������
												// 40: (DOORS) ��ϼ��������
												// 41: (DOORS) �������������
												// 42: (DOORS) �������������
												// 43: (DOORS) �����������
												// 44: (DOORS) �����������
			CharmItem = 45,						// �������� (FKey: ItemID)
			ChaosItem = 46,						// ī���� ť�� (FKey: ItemID)
			SpecialBox = 47,					// Ư�������� (FKey: EventRewardID)
			// 48: ����â�� ��� ������
			// 49: C2C ����
			// 50: C2C ȸ��
			// 51: C2C �ŷ�
			TalismanSlotOpen = 52,				// 52: Ż������ ���� ����(FKey: ���� �ε���)
			TalismanSlotChange = 53,			// 53: Ż������ ���� ���� ��ġ �̵�

			GamblePrice=55,						// 55: ���ڸ�� ������
			GambleFirstWinner=56,				// 56: ���ڸ�� 1�� ���
			GambleSecondWinner=57,				// 56: ���ڸ�� 2�� ���
			DWC = 58,							// 58: DWC ĳ���� �⺻ ���� ���
		};
		enum ePetalChangeCode
		{
			GambleEnterPetal= 9,				// 9: ���ڸ�� ��Ż �����
			GambleFirstWinPetal= 12,			// 12: ���ڸ�� 1�� ��Ż ���
			GambleSecondWinPetal= 13,			// 13: ���ڸ�� 2�� ��Ż ���
		};
	};

	struct CoinTypeCode
	{
		enum eCoinTypeCode
		{
			Coin = 1,				// : ����
			WarehouseCoin = 2,		// : â�� ����
			RebirthCoin = 3,		// : ��Ȱ ����
			PCBangRebirthCoin = 4,	// : PC�� ��Ȱ ����
		};
	};

	struct EquipmentAttributeCode
	{
		enum eEquipmentAttributeCode 
		{
			DelayTime = 1,	// : ������ �ð�
			RemainTime = 2,	// : Remain �ð�
		};
	};

	struct FatigueTypeCode
	{
		enum eFatigueTypeCode
		{
			PCBang = 1,		// : PC�� �Ƿε�
			Daily = 2,		// : ���� �Ƿε�
			Weekly = 3,		// : �ְ� �Ƿε�
			Event = 4,		// �̺�Ʈ �Ƿε�
			VIP = 5,		// VIP�Ƿε�
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
			Gain = 0,		// : ����Ʈȹ��
			Progress = 1,	// : ����Ʈ����
		};
	};

	struct SkillPointCode
	{
		enum eSkillPointCode
		{
			LevelUp = 1,		// ��ų������
			EternityItem = 2,	// �������������
			Repair = 10,		// �� ������ ���� ����
		};
	};

	struct SkillChangeCode
	{
		enum eSkillChangeCode
		{
			Gain = 1,		// : ��ų����Ʈ ȹ��
			Use = 2,		// : ��ų������ & ��ų����Ʈ ���
			Reset = 3,		// : ��ų����Ʈ ����
			GainByQuest = 4,	// : ��ų ȹ�� (����Ʈ)
			GainByDungeon = 5,	// : ��ų ȹ�� (���� ����)
			GainByBook = 6,		// : ��ų ȹ�� (��ų��)
			GainByEvent = 7,	// : ��ų ȹ�� (�̺�Ʈ)
			GainByAdmin = 8,	// : ��ų ȹ�� (by ������)
			DelByDrop = 9,		// : ��ų ���� (���)
			DelByQuest = 10,	// : ��ų ���� (����Ʈ)
			DelByEvent = 11,	// : ��ų ���� (�̺�Ʈ ����)
			DelByAdmin = 12,	// : ��ų ���� (by ������)
			ModSkillLevelByAdmin = 13,	// : ��ų���� ���� (by ������)
			GainByBuy = 14,		// 14��ųȹ��(����)
		};
	};

	struct TradeStatusCode
	{
		enum eTradeStatusCode
		{
			Sell = 0,		// : �Ǹ���
			CompleteSellByItem = 1,	// : �ǸſϷ�(������ ����)
			CompleteSellByCoin = 2,	// : �ǸſϷ�(�Ǹű� ����)
			CancelSell = 3,	// : �Ǹ����
			Expire = 4,		// : �Ⱓ����
		};
	};

	struct AddMaterializedItem
	{
		enum eCode
		{
			Pick			= 1,	// 1=�ݱ�			-> FKey: PartyID or NULL
			Mission			= 2,	// 2=�̼�			-> FKey: MissionID
			QuestReward		= 3,	// 3=����Ʈ����		-> FKey: QuestID
			Present			= 4,	// 4=����			-> FKey: PurchaseID
			PointBuy		= 5,	// 5=�ΰ�������Ʈ ����	-> FKey: NPCID
			CashBuy			= 6,	// 6=ĳ�� ����		-> FKey: PurchaseID
			ReserveMoneyBuy	= 7,	// 7=��Ż ����	-> FKey: PurchaseID
			GetGachaResultCashItem_JP = 9,	// 9=�Ϻ� ��í ������ ���� ĳ�� ������.
			GMGive			= 10,	// 10=������ ����	-> FKey: AuditLogID
			ItemDecompose	= 11,	// 11=������ ����	-> FKey: NPCID
			// ItemEnchantFail = 12,	// 12=������ ��ȭ ����	-> FKey: ItemEnchantID
			ItemCompound	= 13,	// ����
			RandomItem		= 14,	// ����������
			DungeonReward	= 15,	// ��������
			PvPReward		= 16,	// pvp����
			CharacterCreate = 17,	// �⺻����
			SystemMail = 18,		// �ý��۸��� (�̼Ǻ���)
			FixedItem = 19,			// ĳ���� (��, �Ӹ�) - ������/�Ӹ� ����� ����� -> FKey: ĳ���� �������۽ø��� (�κ��� �ִ¾�)
			CostumeMix = 21,		// �ڽ�Ƭ �ռ�
			FishingReward = 22,		// ���ú���
			Harvest = 23,			// ��Ȯ
			Cheat = 24,				// ġƮ
			PvPLadderPoint = 25,	// ��������Ʈ
			UnionPoint = 26,		// ��������Ʈ
			Trigger = 27,			// Ʈ����
			GuildWarFestivalPoint = 28, // ��� ���� ����Ʈ
			Repurchase = 29,
			GuildReversionItem = 30,
			CostumeRandomMixItem = 31,
			BestFriendItem = 34,
			COMEBACKREWARD = 35,	// ������������
			SpecialBoxReward = 36,
			NewbieGameQuitReward = 37, //�ű� ���� ���� ������ �� ����
			NewbiewReConnectReward = 38, //�ű� ���� �����ӽ� ����
			ComebackGameQuitReward = 39, //��ȯ�� ���� ������ �� ����
			SeedPointBuy = 40, //�õ�� ����
		};
	};

	struct MailTypeCode
	{
		enum eCode
		{
			Normal					= MailType::NormalMail,			// �Ϲ� ����
			Mission					= MailType::MissionMail,		// �̼� ����
			Admin					= MailType::AdminMail,			// ������ ����
			EventMail				= MailType::EventMail,			// �̺�Ʈ ����
			VIP						= MailType::VIPMail,			// VIP
			MasterSystem			= MailType::MasterSystemMail,	// �����ý���
			NpcReputaion			= MailType::NpcReputationMail,	// NPCȣ�����ý���
			Quest					= MailType::Quest,				// ����Ʈ
			GuildWarReward  		= MailType::GuildWarReward,		// ����� ����
			SpecialBox_Account		= MailType::SpecialBox_Account,	// Ư��������_����Ÿ�� 
			SpecialBox_Character	= MailType::SpecialBox_Character,	// Ư��������_��ĳ����Ÿ��
			Cadge					= MailType::Cadge,				// ������
			GuildMaxLevelReward		= MailType::GuildMaxLevelReward,// ���ʱ�尡�� ���� ��� ����
			AppellationBookReward	= MailType::AppellationBookReward, // Īȣ �÷��Ǻ� �Ϸ� ����
		};
	};

	struct UseItem
	{
		enum eUseItemCode
		{
			DoNotDBSave = 0,
			Use = 1,		// 1=�Ҹ� ������ ���
			Destroy = 2,	// 2=������
			DeCompose = 4,	// 4=����
			Present = 5,	// 5=����
			GuildReversionItem = 6,	// 6=���Ż�� �߹����� ���ͼӾ����� ����
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
			HarvestDepot = 11,	// ��Ȯâ��
			ServerWare = 17,	// ���� �Ϲ� ������ â��
			ServerWareCash = 18,	// ���� ĳ�� ������ â��
			Talisman = 19,	// Ż������
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
			Coin = 1,				// 1: ����
			Cash = 2,				// 2: ĳ��
			Petal = 3,				// 3: ������
			Coupon = 4,				// 4: ����
			Admin = 5,				// 5: ������ ����
			Quest = 6,				// 6: ����Ʈ����
			Mission = 7,			// 7: �̼Ǻ���
			LevelupEvent = 8,		// 8=ĳ���� ���� �� ����
			VIP = 9,				// 9 : VIP ����
			PvP = 10,				// 10 : PVP ���� ����Ʈ
			Union_Commerical = 11,	// 11 : ���� ���� ����Ʈ
			Union_Royal = 12,		// 12 : �ռ� ���� ����Ʈ
			Union_Liberty = 13,		// 13 : ���� ���� ����Ʈ
			Cash_NexonUSA = 14,		// 14 : �ؽ��Ƹ޸�ī ĳ��
			Event = 15,				// 15 : �̺�Ʈ
			GuildWar_Festival = 16,	// 16 : ����� ���� ����Ʈ
			GuildWar_Reward = 17,	// 17 : ����� ����
			Comeback = 19,			// 18 : ������������
			BestFriend = 20,		// 20 : ��ģ
			BeginnerGuild = 21,		// 21 : �ʺ��ڱ�� ����
			GuildSupport = 22,		// 22 : �����������
			ActiveMission = 23,		// 23 : ��Ƽ��̼� ����
#if defined( PRE_ADD_STAMPSYSTEM )
			Stamp = 24,				// 24 : ������ ����
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined( PRE_ADD_NEW_MONEY_SEED )
			SeedPoint = 25,			// 25 : �õ�� ����
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
			Account = 1,	// ����
			Character,		// ĳ����
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
			MyInfo = 1,				// �ڽ��� ���� ����
			MyMasterInfo,			// �� ���� ����
			OptionalMasterInfo,		// ������ Ư�� ���� ����
		};
	};
#if defined( PRE_ADD_SECONDARY_SKILL )

	struct SecondarySkillTypeCode
	{
		enum eCode
		{
			ProductionSkill		= SecondarySkill::Type::ProductionSkill,		// ���꽺ų
			ManufactureSkill	= SecondarySkill::Type::ManufactureSkill,		// ���۽�ų
			CommonSkill			= SecondarySkill::Type::CommonSkill,			// ���뽺ų
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
			GuildWar_Festival = 5,	// ����� ���� ����Ʈ			
			SeedPoint = 6,			// �õ� �Ӵ�
			Max,
		};
	};
	struct GuildWarRewardType
	{
		enum eGuildWarRewardType
		{
			GuildWarFestivalWin = 1,	// ����������
			GuildWarFestivalLose,		// ��������й�
			GuildWarFinalUnder4,		// ���� 4���̸�
			GuildWarFinal4,				// ���� 4������
			GuildWarFinal2,				// ���� 2������
			GuildWarFinalWin,			// ���� ��º���
			GuildWarFinalAbnormalLose,	// ���� �������й�(������ ��������?)
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
	_GAMESERVER_AFFINITYTYPE_NORMAL = 0,		//�׳� �Ϲ� �����̹� �Ӵ� �׷���
	_GAMESERVER_AFFINITYTYPE_HYBRYD,			//�Ǻ��ǵ� �ް� �� �޾ƿ�
	_GAMESERVER_AFFINITYTYPE_PVP,				//�Ǻ��Ǹ� �޾ƿ�
	_GAMESERVER_AFFINITYTYPE_FARM,				//���Ӽ��� ���� ����ó���� ���� ���� �׼�!
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
	eChSndaAuthFlag_UseEKey		= (1 << 0),		// EKey ��� ��� ����
	eChSndaAuthFlag_UseECard	= (1 << 1),		// ECard ��� ��� ����
};
#endif	// _CH

// structure -----------------------------------------------------------------------------------------------

struct TChannelInfo
{
	USHORT nChannelID;			//�Ҵ�� ä��(�ʵ�)���̵�
	USHORT nChannelIdx;
	USHORT nMapIdx;				//����ϴ� ���� ���̵�
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
	int nWorldSetID;							// ���� ���� ���̵�
	char cWorldID;
	WCHAR wszWorldName[WORLDNAMELENMAX];
	UINT nWorldMaxUser;				//����ڿ�
	USHORT nDefaultMaxUser;
	bool bOnline;				// online, offline ����ǥ�ÿ�
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

// ���������� float�� �ٷ� �����ʴ´�. Ŭ�󿡼� ���� ��ǥ�� *1000�� �ؼ� �����
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

// npc, user object������ �ش�. ����� �κи� 
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
	BYTE cClassID;										// Ŭ���� ID
	int nCreateMapIndex;								// ó�� ������ ���� �� �ε���
	int nCreateMapStartPositionX[DEFAULTPOSITIONMAX];	// ó�� ������ ���� ��ġ X
	int nCreateMapStartPositionY[DEFAULTPOSITIONMAX];	// ó�� ������ ���� ��ġ Y
	int nCreateMapStartRadius[DEFAULTPOSITIONMAX];		// �ݰ�

	int nCreateTutorialMapIndex;			// ó�� ������ Ʃ�丮�� �� �ε��� (GameServer��)
	int nCreateTutorialGateNo;				// ó�� ������ Ʃ�丮�� ����Ʈ (GameServer��)

	int nDefaultBody;						// �⺻��
	int nDefaultLeg;						// �⺻�ٸ�
	int nDefaultHand;						// �⺻��
	int nDefaultFoot;						// �⺻��
	int nFace[DEFAULTPARTSMAX];				// ��
	int nHair[DEFAULTPARTSMAX];				// �Ӹ�
	int nHelmet[DEFAULTPARTSMAX];			// ���
	int nBody[DEFAULTPARTSMAX];				// ����
	int nLeg[DEFAULTPARTSMAX];				// ����
	int nHand[DEFAULTPARTSMAX];				// ��
	int nFoot[DEFAULTPARTSMAX];				// ��
	int nWeapon[WEAPONMAX];					// ����

	DWORD dwHairColor[DEFAULTHAIRCOLORMAX];
	DWORD dwSkinColor[DEFAULTSKINCOLORMAX];
	DWORD dwEyeColor[DEFAULTEYECOLORMAX];

	int nDefaultItemID[DEFAULTITEMMAX];		// ����������
	int nDefaultItemCount[DEFAULTITEMMAX];	// ���������� ����

	int nDefaultSkillID[DEFAULTSKILLMAX];	// ������ų

	TQuickSlot DefaultQuickSlot[DEFAULTQUICKSLOTMAX];	// �����԰���
	int nDefaultGestureID[DEFAULTGESTUREMAX];	// ���� ����ó
};

#if defined( PRE_ADD_DWC )
struct TDWCCreateData
{
	BYTE cClassID;										// �⺻ Ŭ���� ID
	BYTE cJobCode1;										// 1�� ���� Ŭ���� ID
	BYTE cJobCode2;										// 2�� ���� Ŭ���� ID
	BYTE cLevel;
	int nExp;
	int nCreateMapIndex;								// ó�� ������ ���� �� �ε���
	int nCreateMapStartPositionX[DEFAULTPOSITIONMAX];	// ó�� ������ ���� ��ġ X
	int nCreateMapStartPositionY[DEFAULTPOSITIONMAX];	// ó�� ������ ���� ��ġ Y
	int nCreateMapStartRadius[DEFAULTPOSITIONMAX];		// �ݰ�

	int nDefaultBody;						// �⺻��
	int nDefaultLeg;						// �⺻�ٸ�
	int nDefaultHand;						// �⺻��
	int nDefaultFoot;						// �⺻��
	int nEquipArray[EQUIPMAX]; // ���� ���

	DWORD dwHairColor;
	DWORD dwSkinColor;
	DWORD dwEyeColor;

	int nDefaultItemID[DEFAULTITEMMAX];		// ����������
	int nDefaultItemCount[DEFAULTITEMMAX];	// ���������� ����

	int nDefaultSkillID[DEFAULTSKILLMAX];	// ������ų

	TQuickSlot DefaultQuickSlot[DEFAULTQUICKSLOTMAX];	// �����԰���
	int nDefaultGestureID[DEFAULTGESTUREMAX];	// ���� ����ó
	short nSkillPoint; // ��ų ����Ʈ
	int nGold; // ��� - �ʿ� ���ٸ� ��������

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
	bool bUserReturnSystem;	// ���� ���ͽý���
	bool bVehicleMode;
	bool bPetMode;
	bool bAllowFreePass;
#if defined( PRE_PARTY_DB )
	std::wstring wstrMapName;
	int iEnterConditionTable;
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_NORMALSTAGE_REGULATION )
	char cRevisionNum;	//�����ѹ�
#endif
#if defined(PRE_ADD_VEHICLE_SPECIAL_ACTION)
	bool bVehicleSpecalActionMode;
#endif
};

struct TCoinCountData
{
	BYTE cLevel;
	int nRebirthCoin[WORLDCOUNTMAX];		// ��Ȱ����
	int nCashRebirthCoin[WORLDCOUNTMAX];	// ��Ȱĳ������
	int nRebirthCoinLimit;					// �������μ������Ѱ���
	int nCashRebirthCoinLimit;				// �������μ������Ѱ���
	int nVIPRebirthCoin;					// VIP��Ȱ����
};

struct TActorData
{
	int nActorID;
	BYTE cClass;					// ���� Ÿ�� ����
};

const int QUEST_MAX_CNT = 20;
struct TNpcData
{
	int nNpcID;
	int nParam[2];
	WCHAR wszName[EXTDATANAMELENMAX];
	char szTalkFile[256];				// ��������̸�
	char szScriptFile[256];				// script �����̸�
	int QuestIndexArr[QUEST_MAX_CNT];	// ����Ʈ�ε���
	TActorData ActorData;
};

struct TItemData
{
	int nItemID;
	WCHAR wszItemName[EXTDATANAMELENMAX];
	int nType;							// Ÿ��
	int nTypeParam[3];					// Ÿ���Ķ����
	BYTE cLevelLimit;					// ��������
	BYTE cRank;							// ���
	BYTE cReversion;					// �ͼӿ���
	bool IsCash;						// ĳ�����ΰ�
	bool IsDestruction;					// �ı�����
	bool IsAuthentication;				// ��������
	int nAmount;						// �ݾ�
	int nSellAmount;					// �Ǹűݾ�
	int nOverlapCount;					// ��ø����
	std::vector<int> nNeedJobClassList;	// ��������

	int nSkillID;						// ��ų �ε���
	char cSkillLevel;					// ��ų ����
	char cSkillUsingType;				// ��ų ��� Ÿ��
	int nMaxCoolTime;					// �ִ� ��Ÿ�� (������ �ʴ� ��)
	int nEnchantID;						// ��æƮ���̵�
	int	nAllowMapType;					// ��밡���� �� Ÿ��
	char cSealCount;					// �к� Ƚ��
	int nSealID;						// SealCountTableID
	int nNeedBuyItemID;					// �������� ������ �� �ʿ��� ������ID
	int nNeedBuyItemCount;				// �������� ������ �� �ʿ��� �����۰���
	int nNeedPvPRank;					// ������ �� �ʿ��� PvP ��ũ
	bool IsSealed;						// ȹ�� �� �к�����
	int nCashTradeCount;				// ĳ�þ����� ��ȯ����
	bool IsEnableCostumeMix;
	int nApplicableValue;
	bool IsCollectingEvent;			// ������ ���� API�� ȣ�⿩��
	int nExchangeCode;				// ������ ī�װ� ID
	int iNeedBuyLadderPoint;		
	int iUseLevelLimit;
	int iNeedBuyUnionPoint;
	int iNeedBuyGuildWarPoint;
	bool bRebuyable;
	int nPeriod;
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	bool IsWStorage;					// ���� â�� ���� �� �ִ� ����
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
	char cEquipType;					// ����Ÿ��
	int nLength;						// ���� ����
	int nDurability;					// ������
	int nDurabilityRepairCoin;			// ������
	int nPermitJob[10];
	bool bOneType;
};

struct TPartData
{
	int nPartIndex;
	int nParts;							// ����
	int nDurability;					// ������
	int nDurabilityRepairCoin;			// ������
	int nPermitJob[10];
	std::vector<int> nSubPartsList;		// �������� (�ѹ���)

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
	int nPetLevelTypeID;	// pet ���(?)�� ����� �ٸ� ���� �����ϱ����� Ű��
	int nPetSkillID1;		// 
	int nPetSkillID2;
	int nRange;				// ��ĵ ������. �̸��׸� �ݱ� ������ 
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
	int nTickTime; // �Ҹ� �ð�(second)
	int nFullTic;  // ƽ�� �Ҹ�
	BYTE cFullTimeLogOut; // �α׾ƿ��� ���� ������ �����Ⱚ
	int nFullMaxCount;		// ������ �ִ� max��ġ
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
	int nRollbackAmount; // ����� �ο� �ѹ� ������
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
	short wStrength;					// ��
	short wAgility;						// ��ø
	short wIntelligence;				// ����
	short wStamina;						// �ǰ�
	int nExperience;					// ����ġ
	int nFatigue;						// �Ƿε�
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
	char cSkillLevel;					// ��ų ����
	char cLevelLimit;					// ���� ����
	//int nNeedJobClass;				// �������� -> ��ų ���̺�� �Ű�����.
	int nDelayTime;						// ��ų������(ms)
	int nAddRange;						// �߰� ��Ÿ�(cm)
	int nNeedItemID;					// �Ҹ������
	int nNeedItemDecreaseCount;			// �Ҹ� ������ ����
	int nDecreaseHP;					// �Ҹ�HP
	int nDecreaseSP;					// �Ҹ�SP
	int nNeedSkillPoint;
	int nEffectClassValue1Duration;
};

struct TSkillData
{
	int nSkillID;						// ��ųid
	char cNeedWeaponType[ 2 ];			// �ʿ乫��
	int nMaxLevel;						// �ִ뷹��
	char cSkillType;					// ��ųŸ��
	char cDurationType;					// ����Ÿ��
	char cTargetType;					// Ÿ��Ÿ��
	char cDissolvable;					// ���� ���� ����
	int nNeedJobID;						// ���� ����
	bool bDefaultLocked;				// ����Ʈ�� ���� �� ���� ����
	int nUnlockSkillBookItemID;			// ����ϴ� ��ų���� ������ ID
	int nUnlockPrice;					// ������ ����� �� ���.
	int nExclusiveID;					// �� ���̵� ���� ���� ��ų�� ���� ��� �� ����.
	int nGlobalCoolTime;				// �۷ι� ��ų ��Ÿ�� (#36858)
	int nGlobalSkillGroup;				// �۷ι� ��ų �׷� (#36858)
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

// npc ��ȭ ���� ����ü 
struct TALK_QUESTION
{
	std::wstring szQuestion;
};

struct TALK_ANSWER 
{
	std::wstring szLinkTarget;
	std::wstring szLinkIndex;
	std::wstring szAnswer;
	bool bOtherTargetLink;	// �ٸ� ��ũ���Ϸ� ��ũ �ϴ� ��� true
};

struct TALK_PARAGRAPH
{
	std::wstring				szIndex;
	TALK_QUESTION				Question;
	std::vector<TALK_ANSWER>	Answers;
};

// key - ���ܽ�Ʈ���ε���, second - ����
typedef std::map<std::wstring, TALK_PARAGRAPH> TALK_PARAGRAPH_MAP;
typedef TALK_PARAGRAPH_MAP::iterator TALK_PARAGRAPH_MAP_IT;

// key - ���xml ���� �̸�, second - �����ü
typedef std::map<std::wstring, TALK_PARAGRAPH_MAP> TALK_MAP;
typedef TALK_MAP::iterator TALK_MAP_IT;

#include <set>

// key - �������佺Ʈ���ε��� (���� Ŭ���̾�Ʈ ����û ���Ἲ üũ �����޽����� ���� �뵵)
typedef std::set<std::wstring> TALK_ANSWER_SET;
typedef TALK_ANSWER_SET::iterator TALK_ANSWER_SET_IT;

// key - ���xml ���� �̸�, second - �������佺Ʈ������ (���� Ŭ���̾�Ʈ ����û ���Ἲ üũ �����޽����� ���� �뵵)
typedef std::map<std::wstring, TALK_ANSWER_SET>	TALK_ANSWER_MAP;
typedef TALK_ANSWER_MAP::iterator TALK_ANSWER_MAP_IT;

// ����Ʈ ���� ������ ���̺� ����
// ���� ������ 6���� ���� ������ �� �ִ�. nItemID �� nItemCount�� ��Ʈ�̴�.

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
	UINT nIndex;			// �������̺� �ε���
	UINT nStringIndex;	// �������̺� ��Ʈ��
	char cType;			// ����Ÿ�� 1�̸� ��ü���� / 2�� ���ú���
	char cSelectMax;		// ���ú����ϰ�� ����� �����Ұ�����.
	UINT nCoin;			// ����ݾ�
	int	nExp;			// ���� ����ġ
	int	nQuestID;		// ����Ʈ ID
	int	nQuestStep;		// ����Ʈ �Ϸ� �� ����
	BYTE nClass;			// ���� Ŭ����
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
	int nItemID;		// ���� ������ ���̵�
	int nCount;			// ���� ������ ����
	int nMaxCount;		// ������ �ִ����
	int nPrice;			// ������ ����
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
	int nShopID;		// �� ���̵� (npc�� ������ �ִ� ���̵�� ���ؼ� ����)
	std::vector<TShopTabData> vShopTabData;	// SHOPITEMMAX
	TShopData()
	{
		nShopID = 0;
	}
};

// Skill Shop
struct TSkillShopItem
{
	int nSkillID;		// ��ų���̵�
	int nPrice;			// ��ų ����
};

struct TSkillShopData
{
	int nShopID;		// �� ���̵� (npc�� ������ �ִ� ���̵�� ���ؼ� ����)
	TSkillShopItem SkillShopItem[SKILLSHOPITEMMAX];	// �ִ� 10���ۿ� �ȵȴ�
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
	int nShieldItemCount;	// ������ȭ��ȣ������(�ٿ�׷��̵�x)
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
	int nShieldItemCount;	// ������ȭ��ȣ������(�ٿ�׷��̵�x)
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
	int nInvenIndex;	// �κ� �ε���
	short wCount;		// � �־�����
	INT64 biSerial;
};

#ifdef PRE_ADD_GACHA_JAPAN
// gachapon japan
struct TGachaponShopInfoByJob_JP
{
	int nGachaponTableID;		// ��í�� ���̺� ID
	int nGachaponIndex;			// ��í�� �� �ε���.
	int nJobClassID;			// �� ��í������ ������ִ� �����۵��� ����
	int nPartsLinkDropTableID[ CASHEQUIPMAX ];
};

struct TGachaponData_JP
{
	int nGachaponIndex;			// ��í�� �� �ε���. npc ���̺� ���� �ε����� �̰ɷ� ã��. ���̺� �ε��� �ƴ�.
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
enum eCummunityState		//�ʴ� �޴´� �ȹ޴´� ���� �ްڴ� �ȹްڴ� ���
{
	_COMMUNITY_NONE = 0,
	_COMMUNITY_MSG_PERMIT,		//��� �޼��� �޾�ȿ~
	_COMMUNITY_MSG_NOTPERMIT,//�޼��� �ȹ޾�ȿ~ ���� ������ �λ�!
};

struct sWorldUserState
{
	WCHAR wszCharacterName[NAMELENMAX];
	INT64 biCharacterDBID;			//�ϴ��� Ȥ���� �ؼ� ����
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
	//�� �̷������� ������ �����..
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
	std::vector<UINT> vNumOfPlayerOption;		// �ο��� �ɼ�
	bool bIsBreakInto;			// ���԰���Flag
	UINT uiItemUsageType;		// PvPCommon::ItemUsageType
	bool bIsAllowItemDrop;		// �����۵�� Flag
	bool bIsGuildBattleGround;	// ��������� ����Ҽ� �ִ��� Flag
	bool bIsReleaseShow;			// �����������������
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
	bool bIsReleaseShow;			// �����������������
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

	// ���н� -1 ����
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
	UINT uiMinTeamPlayerNum;		// �ּ� ���� �� �ο�
	UINT uiMaxTeamPlayerDiff;	// �ִ� ���� �ο� ��� ��

	short Check( const UINT uiATeam, const UINT uiBTeam, const bool bRandomTeam ) const
	{
		// �ּ� ���� �� �ο� �˻�
		if( uiATeam < uiMinTeamPlayerNum || uiBTeam < uiMinTeamPlayerNum )
			return ERROR_PVP_CANTSTART_LESS_MINTEAMPLAYER;

		if (!bRandomTeam)	// �������� �ƴ� ��� ó���Ѵ�.
		{
			// �ִ�  ���� �ο� ��� �� �˻�
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
	BYTE cType;			//PvPCommon::RankTable ����
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

// �̼� �ý��� ����
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
	int nType;			// �̼�Ÿ��(eMissionType)
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
	int nMailID[2];			// 0 - 50% �Ѱ��� �� ������ ���� ID, 1- Īȣ �÷��� �ϼ��� ���� MailID 
#endif  //#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
};

// TaxTable
struct TTaxData
{
	char cTaxType;			// �������� (TaxType_PostTax)
	int nLevelStart;		// ���� ����
	int nLevelEnd;			// �� ����
	int nAmountStart;		// ���۱ݾ� (������)
	int nAmountEnd;			// ���ݾ�
	float fRate;			// ����
	int nTaxAmount;			// ����
};

// Seal
const BYTE SEALENCHANTMAX = 16;	// �밭ȭ���� 15������.
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
	char cType;		// PCBang ���� ���� (ePCBangType)
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
	int nItemID[COMMODITYITEMMAX];	// ItemTable ������ ID
	int nLinkSN[COMMODITYLINKMAX];	// �����ǰ
	std::string strName;
	short wPeriod;					// �Ⱓ
	int nPrice;						// ����
	int nCount;						// ���Ž� �־����� ����
	int nPriority;					// ĳ�ü��� ǥ�õǴ� �켱����
	bool bOnSale;					// �ǸŻ���
	bool bOnSaleReal;				// ��¥(?)�ǸŻ���
	char cState;					// HOT, NEW�� ���� ǥ�ÿ�
	bool bLimit;					// ���� �Ǹ�
	bool bReserveGive;				// ������ ���� ����
	int nReserve;					// ���Ž� �־����� ������
	int nValidity;					// ��ȿ�Ⱓ
	bool bReserveAble;				// ���������� ��� �ִ��� ����
	bool bPresentAble;				// ���� ���� ����
	char cItemSort;					// ����������
	int nPriceFix;					// ����
	bool bCartAble;					// ��ٱ��Ͽ� ���� �� �ִ���
	bool bVIPSell;					// VIP ���� �Ǹ� ����
	int nVIPLevel;					// VIP ���
	bool bAutomaticPay;				// �ڵ����� ����
	int nAutomaticPaySale;			// �ڵ����� �� ��������
	int nVIPPoint;					// ���Ž� �����ϴ� VIPPoint
	int nOverlapBuy;				// �ߺ����Ź���
#if defined(PRE_ADD_CASH_REFUND)
	bool bNoRefund;					// û�� öȸ �Ұ��ɿ���(true : �Ұ���)
#endif
#if defined(PRE_ADD_CASHSHOP_CREDIT)
	bool bCreditAble;				// �������
	int nCreditAbleLevel;			// ������� ũ��������
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
		bNoRefund = true;		// �⺻ �Ұ���
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
	int nSN;		// ��Ű���Ǹ�SN
	std::vector<int> nVecCommoditySN;

	TCashPackageData()
	{
		nSN = 0;
		nVecCommoditySN.clear();
	}
};

struct TCashLimitData
{
	int nSN;			// �����Ǹ��� ��ǰ SN
	time_t tStartDate;  // _StartDate	������
	time_t tEndDate;	// _EndDate	������	
	int nBuyAbleCount;	// ĳ���ʹ� ���Ű���Ƚ��
#if defined(PRE_ADD_LIMITED_CASHITEM)
	int nLimitedSellCount;		//��������������!
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
};

struct TVIPData
{
	int nVipID;				// VIPTable������ ID
	int nMonthItemSN;		// ĳ�� ������ ��ǰ �ѹ� (30�� ������ �ѹ� �ֱ�)
	int nMailID[4];			// 0:���񽺸��� 7����, 1:���񽺸��� 1����, 2:���񽺸����, 3:�ڵ������� ĳ�� �����Ҷ�
	int nAutoPayItemSN;		// �ڵ������Ҷ� ������ ������SN	
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
		Levelup = 1,			// �Ϲ� �������̺�Ʈ
		ShandaPromotion = 2,	// ���� ���θ���̺�Ʈ
		CombackUser = 3,		// ���������̺�Ʈ
		LevelupNew = 4,			// �������̺�Ʈ ���� (�ѹ� ������ ���̻� ���޴� ����)
		ComeBackUserInven = 5,		// ��ȯ�ڿ��� �ٷ� �־��ִ� ������ Ÿ��
		NewbieQuitReward = 6, //�ű� ���� ���� ���� ����
		NewbieReconnectReward = 7, //�ű� ���� ������ ����
		ComeBackUserQuitReward = 8, //��ȯ�� ���� ���� ����
	};

	int nLevel;			// Player ĳ������ ����
	char cClassID;		// Ŭ����
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
	int iNpcPresentRepute1; // ����1 ���� ȣ���� �޼� ����
	int iNpcPresentRepute2; // ����2 ���� ȣ���� �޼� ����
	int iNpcPresentID1; // NpcPresentRepute1 �޼� �� NPC�� �ִ� ���� 1 ItemID
	int iNpcPresentID2; // NpcPresentRepute1 �޼� �� NPC�� �ִ� ���� 2 ItemID
#else
	std::vector<int>	iQuestIDs;
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
	
	int	iMaxFavor;			// ȣ�� �ִ�ġ
	int	iTakeFavorPerDay;	// 1�ϴ� ���̴� ȣ�� ��ġ
	int	iMaxMalice;			// ���� �ִ�ġ
	int	iAddMalicePerDay;	// 1�ϴ� �ö󰡴� ���� ��ġ
	int	iPresentIDArr[6];	// ���� �� �ִ� ���� ����Ʈ
	std::vector<int> vFavorNpcID;		// ȣ�� NPC ����Ʈ
	std::vector<int> vMaliceNpcID;		// ���� NPC ����Ʈ
	int	iFavorGroupBomb;
	int	iMaliceGroupBomb;
	int	iMailID[NpcReputation::Common::MaxMailCount];
	int	iMailRand[NpcReputation::Common::MaxMailCount];
	int	iPlusItemID;
	int	iPlusProb;
	int	iUnionID;
	int	iPresentPointArr[6]; // �������� ������ �ִ� ��������Ʈ

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
				// Ȥ�� ���� ������ �˻� �ѹ���!
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
	// �Ʒ��� ������ StoreBenefit ���̺��� ���ǿ� ���� ���̱� ������ ����� ������ �ٲ�� �ȵ�.
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
	int nSampleVersion;	// ���þ����� ȸ��
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
	int	iMaxTime;	// ����(ms)
	int	iSuccessProbability;
	int	iSuccessUpExp;
	int	iRecipeUpExp;
	int	iMaterialItemID[SecondarySkill::ManufactureSkill::MaxMaterialItemCount];
	int	iMaterialItemCount[SecondarySkill::ManufactureSkill::MaxMaterialItemCount];
	int	iSuccessCount;

	// ����ġ�� ����� ItemID ���
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
	int iSkillPointPlant;			// ���ѽɱ� �� ����ġ
	int iSkillPointWater;			// ���ֱ� �� ����ġ
	int iCatalystCount;				// �ʿ� ���������� ����
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
	int nSecondarySkillClass;			//���ú�����ų���
	int nSecondarySkillLevel;			//���ú�����ų����
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
	char cGlyphType;		// 1: ��ȭ���� 2: ��ų����  3: Ư����ų����	(eGlyphType)
	char cGlyphRank;		// 1: ���� 2: ���� 3: ���� 4: ����ũ (eItemRank)
	int nCharge;
};

struct TCharmItem
{
#if defined(PRE_FIX_68828)
	int nCharmID;	// �����̺� ID
#endif
	int nItemID;	// ������
	int nCount;		// ������ ȹ���� ����
	int nProb;		// Ȯ��
	int nPeriod;	// �Ⱓ
	int nAccumulationProb;
	int nGold;
	bool bMsg;
};

struct TCharmItemData
{
	int nCharmNo;	// �ŷ¾����� ��ȣ
	std::vector<TCharmItem> CharmItemList;
};

const BYTE CharmItemKeyMax = 5;
struct TCharmItemKeyData
{
	int nCharmItemID;	// ���� ������ ���̵�
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
	int nItemID;	// ������
	int nCount;		// ������ ȹ���� ����
	int nProb;		// Ȯ��
	int nPeriod;	// �Ⱓ
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
	SERVERMONITOR_LEVEL_MONITOR,				//���������� �����ǻ���
	SERVERMONITOR_LEVEL_ADMIN,					//���������� �������� �� ���μ��� ���۸� ����
	SERVERMONITOR_LEVEL_SUPERADMIN,				//���������� �������� �� �����Ʈ�� ����
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
// ����� ���� ���ϰ���
struct TGuildWarRewardData
{
	char cType;			// ���� Ÿ��..eGuildWarRewardType
	char cClass;		// Ŭ����(������,��ó, �Ҽ�����, Ŭ����)
	int nMailID;		// ���� ���̵�
	int nPresentID;		// ������ ���̵�
	int nGuildFestivalPoint;	// �������� ������ �����������Ʈ
	int nGuildPoint;			// �������� ������ �������Ʈ
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
	int nBossID[PvPCommon::TeamIndex::Max];									//�����������̵�
	int nBossSpawnAreaID[PvPCommon::TeamIndex::Max];						//���������� ������ġ
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
	int nGainResourceTermTick;			//���ɽ� ȹ����
	int nGainResource;					//ȹƯ�ҽ� ȹ�淮
	int nRequireTryTick;				//ȹ��õ��� Ŭ���������� �����ʿ�ð�
	int nCompleteOccupationTick;		//Ŭ���Ϸ��� ���ɱ� �Ѿ�� ���� ���ð�
	int nBonusBuffID;
	//Ŭ���̸���~
	int nClimaxGainTermTick;
	int nClimaxGainVal;
	int nClimaxTryTick;
	int nClimaxCompleteOccupationTick;
};

struct TBattleGroundEffectValue
{
	int nSkillID;
	int nSkillLevel;
	int nNeedSkillPoint;			//ȹ����ʿ�����Ʈ
	int nUseResPoint;			//��ų���� �Ҹ� ������ ����Ʈ
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
		nNeedSkillPoint = 0;			//ȹ����ʿ�����Ʈ
		nUseResPoint = 0;			//��ų���� �Ҹ� ������ ����Ʈ		
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
	int nSkillType;				//CDnSkill::SkillTypeEnum		//������ ��ų�� ��ųŸ�Կ� �״� ������ ���� ������ �߰�Ȯ��� �ʿ��ұ� �ؼ� �о��
	int nSkillDurationType;		//CDnSkill::DurationTypeEnum
	int nSkillTargetType;		//CDnSkill::TargetTypeEnum
	int nEffectType[PvPCommon::Common::MaximumEffectCount];
	int nEffectApplyType[PvPCommon::Common::MaximumEffectCount];
	int nProcess;				//���μ���Ÿ���� �ϳ��� ���
	int nCanDuplicate;			//��ø����?
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
	int iFatigue10000Ratio;	// 10000����
	float fMasterGainExp;
	//int nMasterGainItemID;
	//int nMasterGainItemCount;
	float fAddGainExp;
	float fDefense;
	float fCritical;
	float fFinalDamage;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int iTalismanCost;	// Ż������ ���� ����, ��ġ��ȯ ���
#endif
};

struct TMasterSysFeelTableInfo
{
	int nMinFeel;
	int nMaxFeel;
	int nAddExp; //(%����)
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
	DBJOB_GUILDWAR_INIT = 0,		// ����� �ʱ�ȭ
	DBJOB_GUILDWAR_STATS,		// ����� ���
	DBJOB_SYSTEM_MAX,
};

enum eDBJobSystemStatus
{
	JOB_STATUS_RESERVE,		// ����
	JOB_STATUS_COMPLETED,	// �Ϸ�
	JOB_STATUS_DOING,		// ������
	JOB_STATUS_FAIL,		// ����
};

struct TGuildRewardItemData
{
	int		nItemID;			// ��庸�����̺� �ִ� ���̵�
	short	nNeedGuildLevel;	// �����ϱ����� ��巹��
	int		nNeedGold;			// �����ϱ����� ���
	int		nItemType;			// ��庸������� Ÿ��
	int		nTypeParam1;
	int		nTypeParam2;
	bool	bCheckInven;		// �̹� �����Ѱ��� üũ
	bool	bEternity;			// 0:�Ⱓ�� 1:����
	int		nPeriod;			// �Ⱓ
	bool	bCheckMaster;		// ������üũ ����
	int		nCheckType;			// �ʿ� ���� üũ(0:NONE 1:ITEMID 2:��帶ũ
	int		nCheckID;			// �ʿ��� ���� �����۾��̵�
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
	int nItemID;							// ���� ID
	int nUseItemSN[MAX_SALE_USEITEM];		// ������ ����� ������ SN
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
	int nItemID;		// ���ӵ�����۾��̵�
	int nMaxCount;		// �ִ� ����
	int nSwapItemID;	// ��ü�� �����۾��̵�
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
	BYTE cRank;	// ������ ���
	BYTE cLevelLimit;	// ���� ����
	BYTE cEnchantLevel;	// ��ȭ ����
	int nNeedCoin;	// ������
	int nNeedItemID1;	// �ʿ������1
	short wNeedItemCount1;	// �ʿ������ ī��Ʈ1
	int nNeedItemID2;	// �ʿ������2
	short wNeedItemCount2;	// �ʿ������ ī��Ʈ2
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
		BYTE cIndex;				// �ε���
		BYTE cGroupIndex;			// �׷��ε���
		WCHAR wszPartyName[PARTYNAMELENMAX];
		int nWorldMap;			// ������
		int nTargetMap;			// Ÿ�ٸ�(���ذ�)
		ePartyType PartyType;	// ��ƼŸ��
		BYTE cPartyMemberMax;
		int nItemID;			// ����Ǿ�����
		TDUNGEONDIFFICULTY Difficulty;
		TPARTYITEMLOOTRULE ItemLootRule;			//ePartyItemLootRule ����
		TITEMRANK ItemRank;				//�����۷�ũ(��Ʈ���ȹ����)
		BYTE cUserLvLimitMin;		//�θƽ����� �ִܴ�.
		int iBitFlag;
		int nSkillID[MAXSKILLCOUNT];				// ��ų���̵�		
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
		int nValue;			// %��
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
		Event_1 = 1,			// 1. hp��� �� �϶�
		Event_2 = 2,			// 2. ���ݷ� ��ȭ �� ��ȭ
		Event_3 = 3,			// 3. ���� ��ȭ �� ��ȭ
		Event_4 = 4,			// 4. ��ȭ Ȯ�� ����
		Event_5 = 5,			// 5. ����ġ ����
		Event_6 = 6,			// 6. ��Ƽ���� ����ġ ����
		Event_7 = 7,			// 7. ģ�� ����ġ ���� (��ģ�ƴ�, ����ģ��)
		Event_8 = 8,			// 8. ȣ���� ����
		Event_9 = 9,			// 9. ���� ����Ʈȹ�淮 ����
		Event_10 = 10,			// 10. ������ ����� ����
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
		int nBlowValue;		// ���� ������ ���̵������� ���
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
	int nJoinMinLevel;			//���ʱ�尡�Խ� �ּҷ���
	int nJoinMaxLevel;			//���ʱ�尡�Խ� �ִ뷹��
	int nFirstJoinMailID;		//���ʰ��Խ� ������Ͼ��̵�
	int nMaxLevelGuildSupportMailID;	//���ʰ����� ��忡�� ������ ������̵�
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
	int nMapID; //��ID
	int nDropType;
	int nNeedItemID[3]; //�ʿ��� ���� ������ ID
	int nDropItemGroupID[3]; //����� ��������۱׷�ID
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
	int nDropID; // ItemDropTable�� ID�� ��Ī�Ǵ� ��
	int nEnchantCount; //��æƮ�� ����� ī��Ʈ
	int nEnchantOption[20]; //��æƮ ����
	int nEnchantProb[20]; //��æƮ�� ���õ� Ȯ��
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

	struct TStampChallenge // ������ ��������
	{
		BYTE cType;
		std::set<int> setAssignmentID; // �������� ���
		int nMailID4; // 3�� ����
		int nMailID7; // 5�� ����
	};

	struct TStampTableData // ������ ���̺�
	{
		INT64 biStartTime;
		std::vector<TStampChallenge> vChallengeList;
	};
}
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
struct TPrevPotentialData
{
	int nItemID; //������ ID
	INT64 nSerial; //�ø���
	char cPotential; //���� ����µ�����
	char cNowPotential; //���� �ο��� �����
	char cPotentialMoveCount; //(����)���� ���� ī��Ʈ
	INT64 nPotentialItemSerial; //����� �ο��� ����� �ڵ� �ø���
	int nPotentialItemID; //����� �ο��� ����� ������ ID
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
	enum eCheckMinTime	// �ּ� Ŭ���� Ÿ��
	{
		CheckMinTime = 60 * 1000, // 1��
	};
	enum eAbuseCount // ������ AbuseCount��
	{
		AbuseCount = 10,
	};
	// CheckTime�� ������� ���� ��(������ID�� �������� �Ǻ��Ѵ�
	struct IgnoreMapIndex
	{
		enum eIgnoreMapIndex
		{
			MAP_CHAOSE_FIELD_BossA = 15157,	//ȥ���� ƴ �ĸ���
			MAP_CHAOSE_FIELD_32Lv_BossA = 15167,	//ȥ���� ƴ ���̶�
			MAP_CHAOSE_FIELD_24Lv_BossA = 15174,	// ȥ���� ƴ ī����
		};
	};
};
#endif	// #if defined(PRE_ADD_STAGECLEAR_TIMECHECK)