#pragma once

#include "DNCommonDef.h"
#include "DNServerDef.h"

#if defined(_GPK)
#include "../../Extern/GPK/Include/GPKitSvr.h"
#endif	// _GPK

#if defined(_HSHIELD)
#include "../../Extern/HShield/SDKs/Include/AntiCpXSvr.h"	// HackShield
#endif	// _HSHIELD

struct TConnectionInfo
{
	char szIP[IPLENMAX];
	int nPort;
};

struct TWorldInfo
{
	int nWorldSetID;
	WCHAR wszWorldName[WORLDNAMELENMAX];
	TConnectionInfo VillageInfos[VILLAGECOUNTMAX];	// 빌리지 대수
};

struct TServiceManagerConfig
{
	WCHAR wszRegion[32];
	int nServicePort;
	int nLauncherPort;
	int nServicePatcherPort;
	int nGSMPort;
	int nMonitorPort;
	WCHAR wszGSMIP[IPLENMAX];
	WCHAR wszGSMCodePage[256];
	WCHAR wszPatchBaseURL[256];
	WCHAR wszPatchURL[256];
	WCHAR wszPatchDir[256];
	WCHAR wszNoticePath[FILENAME_MAX];
	char szVersion[SERVERVERSIONMAX];
};

struct TNetLauncherConfig
{
	char szVersion[SERVERVERSIONMAX];
	char szServiceManagerIP[IPLENMAX];
	int nServiceManagerPort;
	char szExcutePath[1024];
};

struct TNetLauncherPatcherConfig
{
	char szVersion[SERVERVERSIONMAX];
	char szServiceManagerIP[IPLENMAX];
	int nServiceManagerPort;
	char szNetLauncherLocation[1024];
	char szNetLauncherName[MAX_PATH];
};

const BYTE DSNLENMAX = 30;
const BYTE DBNAMELENMAX = 30;

struct TDBName: TConnectionInfo
{
	int nWorldSetID;
	WCHAR wszDBName[DBNAMELENMAX];	// 디비 테이블 이름
	WCHAR wszDBID[IDLENMAX];		// 디비 로그인계정
};

struct TLoginConfig
{
	bool bUseCmd;								//런처를 통해서 생성되었는지
	int nManagedID;								//Launcher ManagedID
	TConnectionInfo ServiceInfo;				//ServiceManagerInfo

	int nVersion;								// 버젼
	int nNation;								// 국가정보 (eNation참조)
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	USHORT nMajorVersion;
	USHORT nMinorVersion;

	int nIocpMax;								// iocp init max
	int nSQLMax;								// SQL max
	int nWorkerThreadMax;						// worker thread max

#if defined(_KRAZ)
	bool bTestServer;
	TDBName ActozCommonDB;
#endif	// #if defined(_KRAZ)
	TDBName MembershipDB;
	TDBName WorldDB[WORLDCOUNTMAX];
	int nWorldDBCount;

	int nMasterAcceptPort;						// 마스터 tcp 포트
	int nClientAcceptPort;						// 클라 tcp 포트

	TConnectionInfo LogInfo;

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_GPK)
	SGPK::IGPKSvrDynCode *pDynCode;
	SGPK::IGPKCSAuth *pGpkCmd;
	//int nCodeIndex;
#endif	// _GPK

#if defined(_HSHIELD)
	AHNHS_SERVER_HANDLE	hHSServer;				// [확장 서버연동] 서버 핸들 개체
#endif	// _HSHIELD

#if defined(_TW)
	TConnectionInfo AuthInfoTW1;				// 인증섭 (감마니아 1 - Authorization : 사용자 계정 인증 처리, 상태 (블록 여부, 캐쉬 잔여 포인트 ?) 체크)
	TConnectionInfo AuthInfoTW2;				// 인증섭 (감마니아 2 - Logout : 사용자 로그아웃 처리)
	int nDenyWorld;								// 제외할 월드
#endif	// _TW
#if defined(_TH)
	TConnectionInfo AuthInfoTH;
	TConnectionInfo OTPInfoTH;
#endif	//#if defined(_TH)

#if defined(_US)
	TConnectionInfo PIInfo;
	char szRequestPage[256];
	char szUserAgent[50];
	char szServiceCode[50];
	int nLocaleID;
#endif	// #if defined(_US)

#ifdef _SG
	int nCherryGameID;
	char szCherryAuthAddr[64];
#endif		//#ifdef _SG
#if defined(PRE_IDN_PVP)
	int nPvPWorldID;	// 대회용 WorldID
#endif
};

struct TVillageConfig
{
	bool bUseCmd;		//런처를 통해서 생성되었는지
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	int nCreateCount;
	int nCreateIndex;
	int nManagedID;
	TConnectionInfo ServiceInfo;

	int nWorldSetID;								// 월드 아이디
	int nVillageID;								// 빌리지섭 구분 아이디 (여러대 띄웠을때 구별하기위한 아이디)

	int nIocpMax;								// iocp init max

	int nClientAcceptPort;						// 클라 tcp 포트

	TConnectionInfo CashInfo;
	TConnectionInfo MasterInfo;					// 마스터 정보

	int nDBCount;								// 총 디비 개수
	TConnectionInfo DBInfos[DBSERVERMAX];		// 디비 정보

	TConnectionInfo	LogInfo;

	// ItemSerial 얻어오는데 쓰이는 SQL (다른건 절대 사용해선 안된다)
	TDBName MembershipDB;

	char szPrivateDolbyIp[IPLENMAX];					// 돌비섭 아이피
	char szPublicDolbyIp[IPLENMAX];					// 돌비섭 아이피
	int nControlPort;							// 돌비섭 포트(컬트롤)
	int nAudioPort;								// 돌비섭 포트(오디오)

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_GPK)
	SGPK::IGPKSvrDynCode* pDynCode;
	SGPK::IGPKCSAuth *pGpkCmd;
	//int nCodeIndex;
#endif	// _GPK

#if defined(_HSHIELD)
	AHNHS_SERVER_HANDLE	hHSServer;				// [확장 서버연동] 서버 핸들 개체
#endif	// _HSHIELD

#if defined( PRE_WORLDCOMBINE_PARTY )
	int nCombinePartyWorld;
#endif
};

struct TGameConfig
{
	bool bUseCmd;
	char szVersion[SERVERVERSIONMAX];	//exe version
	char szResVersion[SERVERVERSIONMAX];	//ResVersion
	BYTE cAffinityType;							// 서버대응타입(일반:0, PvP+일반:1, PvP:2)	
	int nCreateCount;
	int nCreateIndex;
	int nManagedID;
	TConnectionInfo ServiceInfo;

	bool bPreLoad;								// 미리로드~~
	bool bAllLoaded;							// 초기로드시점 판단을 위해서
	//int nGameID;								// 게임섭 아이디

	int nGameServerOpenCount;					//게임서버 서버오픈 갯수
	int nGameAcceptPortBegin;					//게임섭 udp 억셉포트
	int nClientAcceptPort;						// 클라 tcp 포트

	int nIocpMax;								// MaxSocket

	TConnectionInfo CashInfo;
	int nMasterCount;							// 총 마스터 개수
	TConnectionInfo MasterInfo[WORLDCOUNTMAX];	// 마스터 정보

	int nDBCount;								// 총 디비 개수
	TConnectionInfo DBInfos[DBSERVERMAX];		// 디비 정보

	TConnectionInfo	LogInfo;

	// ItemSerial 얻어오는데 쓰이는 SQL (다른건 절대 사용해선 안된다)
	TDBName MembershipDB;

	char szPrivateDolbyIp[IPLENMAX];					// 돌비섭 아이피
	char szPublicDolbyIp[IPLENMAX];					// 돌비섭 아이피
	int nControlPort;							// 돌비섭 포트(컬트롤)
	int nAudioPort;								// 돌비섭 포트(오디오)

	int nProbePort;								//UDP주소를 제대로 얻어오기 위한 프로브 정보
	char szProbeIP[16];

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_GPK)
	SGPK::IGPKSvrDynCode* pDynCode;
	SGPK::IGPKCSAuth *pGpkCmd;
	//int nCodeIndex;
#endif	// _GPK

#if defined(_HSHIELD)
	AHNHS_SERVER_HANDLE	hHSServer;				// [확장 서버연동] 서버 핸들 개체
#endif	// _HSHIELD
#if defined( _WORK )
	bool bDisableFarm;
#endif // #if defined( _WORK )
#if defined( PRE_FIX_WORLDCOMBINEPARTY )
	bool bWorldCombineGameServer;
#endif
};

struct TMasterConfig
{
	bool bUseCmd;								//런처를 통해서 생성되었는지
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	int nManagedID;
	TConnectionInfo ServiceInfo;

	int nWorldSetID;								// 월드 아이디 (마스터 섭은 월드에 한대만 존재)

	int nVillageAcceptPort;
	int nGameAcceptPort;

	TConnectionInfo LoginInfo[LOGINCOUNTMAX];

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_KR)
	TConnectionInfo AuthInfo;					// 인증섭
	int nAuthDomainSN;							// 인증 도메인 SN
#endif	// _KR

#if defined(_CH)
	bool bFCM;									// 피로도 유무
#endif	// _CH

	TConnectionInfo	LogInfo;
#ifdef PRE_ADD_DOORS
	TConnectionInfo	DoorsInfo;
	TConnectionInfo DoorsIdenty[2];
#endif		//#ifdef PRE_ADD_DOORS

#if defined(_TW)
	TConnectionInfo AuthInfoTW;					// 인증섭 (감마니아 - Logout : 사용자 로그아웃 처리)
#endif	// _TW

#if defined(_US)
	TConnectionInfo PIInfo;
	char szRequestPage[256];
	char szUserAgent[50];
	char szServiceCode[50];
#endif	// #if defined(_US)
#if defined(_TH)
	TConnectionInfo AsiaSoftPCCafe;				// 아시아 소프트 PC방
#endif
#if defined( PRE_WORLDCOMBINE_PARTY )
	int nCombinePartyWorld;
#endif
#ifdef PRE_MOD_OPERATINGFARM
	int nFarmServerID;
#endif		//#ifdef PRE_MOD_OPERATINGFARM
};

struct TDBConfig
{
	bool bUseCmd;					// 런처를 통해서 생성되었는지
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	int nManagedID;
	TConnectionInfo ServiceInfo;

	int nAcceptPort;
	int nThreadMax;

#if defined(_KRAZ)
	bool bTestServer;
	TDBName ActozCommonDB;
#endif	// #if defined(_KRAZ)
	TDBName MembershipDB;
	TDBName WorldDB[WORLDCOUNTMAX];
	int nWorldDBCount;
#if defined( PRE_WORLDCOMBINE_PARTY )
	int nCombineWorldDBID;
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	TConnectionInfo LogInfo;

	std::string szResourcePath;		// resource path
	std::string szResourceNation;
};

struct TLogConfig
{
	bool bUseCmd;					// 런처를 통해서 생성되었는지
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	int nManagedID;
	TConnectionInfo ServiceInfo;

	int	nAcceptPort;
	int nThreadMax;

	TDBName MasterLogDB;			// MasterLogDB (넥슨 web서비스에 필요한 디비)
	TDBName ServerLogDB;
	TDBName LogDB[WORLDCOUNTMAX];	// LogDB
	int	nLogDBCount;
	std::string szResourcePath;		// resource path
};

struct TCashConfig
{
	bool bUseCmd;								//런처를 통해서 생성되었는지
	int nManagedID;								//Launcher ManagedID
	TConnectionInfo ServiceInfo;				//ServiceManagerInfo

	int nVersion;								// 버젼
	int nNation;								// 국가정보 (eNation참조)
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	USHORT nMajorVersion;
	USHORT nMinorVersion;

	int nWorldSetID;								// 월드 아이디
	int nThreadMax;

	TDBName MembershipDB;
	TDBName WorldDB[WORLDCOUNTMAX];
	int nWorldDBCount;

	int nAcceptPort;

	TConnectionInfo LogInfo;

	std::string szResourcePath;		// resource path
	std::string szResourceNation;

#if defined(_KR) || defined(_US)
	TConnectionInfo CashInfo;		// 캐쉬 서버 정보
	int nServerNo;					// 서버 번호
#elif defined(_KRAZ)
	bool bTestServer;
#elif defined(_JP)
	WCHAR wszServiceType[10];			// alpha, real 
#elif defined(_TW) || defined(_TH)
	TConnectionInfo QueryPointInfo;	 // 포인트 조회 서버 정보
	TConnectionInfo ShopItemInfo;	 // 아이템 구매 서버 정보
#if defined(_TW)
	TConnectionInfo CouponInfo;		 // 쿠폰 서버 정보
	TConnectionInfo CouponRollBackInfo;		 // 쿠폰 롤백 서버 정보
	int				nProtocolType;	 // 프로토콜 타입 1:old, 2:new
#endif //#if defined(_TW)
#elif defined(_SG)
	std::string CouponServerInfo;
	std::string AuthToken;
#elif defined(_ID)
	std::string QueryPointURL;		// G-Cash 잔액 조회
	std::string ItemShopURL;			// 아이템 구매 URL
#endif	// _KR
};
