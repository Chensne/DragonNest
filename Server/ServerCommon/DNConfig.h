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
	TConnectionInfo VillageInfos[VILLAGECOUNTMAX];	// ������ ���
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
	WCHAR wszDBName[DBNAMELENMAX];	// ��� ���̺� �̸�
	WCHAR wszDBID[IDLENMAX];		// ��� �α��ΰ���
};

struct TLoginConfig
{
	bool bUseCmd;								//��ó�� ���ؼ� �����Ǿ�����
	int nManagedID;								//Launcher ManagedID
	TConnectionInfo ServiceInfo;				//ServiceManagerInfo

	int nVersion;								// ����
	int nNation;								// �������� (eNation����)
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

	int nMasterAcceptPort;						// ������ tcp ��Ʈ
	int nClientAcceptPort;						// Ŭ�� tcp ��Ʈ

	TConnectionInfo LogInfo;

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_GPK)
	SGPK::IGPKSvrDynCode *pDynCode;
	SGPK::IGPKCSAuth *pGpkCmd;
	//int nCodeIndex;
#endif	// _GPK

#if defined(_HSHIELD)
	AHNHS_SERVER_HANDLE	hHSServer;				// [Ȯ�� ��������] ���� �ڵ� ��ü
#endif	// _HSHIELD

#if defined(_TW)
	TConnectionInfo AuthInfoTW1;				// ������ (�����Ͼ� 1 - Authorization : ����� ���� ���� ó��, ���� (��� ����, ĳ�� �ܿ� ����Ʈ ?) üũ)
	TConnectionInfo AuthInfoTW2;				// ������ (�����Ͼ� 2 - Logout : ����� �α׾ƿ� ó��)
	int nDenyWorld;								// ������ ����
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
	int nPvPWorldID;	// ��ȸ�� WorldID
#endif
};

struct TVillageConfig
{
	bool bUseCmd;		//��ó�� ���ؼ� �����Ǿ�����
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	int nCreateCount;
	int nCreateIndex;
	int nManagedID;
	TConnectionInfo ServiceInfo;

	int nWorldSetID;								// ���� ���̵�
	int nVillageID;								// �������� ���� ���̵� (������ ������� �����ϱ����� ���̵�)

	int nIocpMax;								// iocp init max

	int nClientAcceptPort;						// Ŭ�� tcp ��Ʈ

	TConnectionInfo CashInfo;
	TConnectionInfo MasterInfo;					// ������ ����

	int nDBCount;								// �� ��� ����
	TConnectionInfo DBInfos[DBSERVERMAX];		// ��� ����

	TConnectionInfo	LogInfo;

	// ItemSerial �����µ� ���̴� SQL (�ٸ��� ���� ����ؼ� �ȵȴ�)
	TDBName MembershipDB;

	char szPrivateDolbyIp[IPLENMAX];					// ���� ������
	char szPublicDolbyIp[IPLENMAX];					// ���� ������
	int nControlPort;							// ���� ��Ʈ(��Ʈ��)
	int nAudioPort;								// ���� ��Ʈ(�����)

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_GPK)
	SGPK::IGPKSvrDynCode* pDynCode;
	SGPK::IGPKCSAuth *pGpkCmd;
	//int nCodeIndex;
#endif	// _GPK

#if defined(_HSHIELD)
	AHNHS_SERVER_HANDLE	hHSServer;				// [Ȯ�� ��������] ���� �ڵ� ��ü
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
	BYTE cAffinityType;							// ��������Ÿ��(�Ϲ�:0, PvP+�Ϲ�:1, PvP:2)	
	int nCreateCount;
	int nCreateIndex;
	int nManagedID;
	TConnectionInfo ServiceInfo;

	bool bPreLoad;								// �̸��ε�~~
	bool bAllLoaded;							// �ʱ�ε���� �Ǵ��� ���ؼ�
	//int nGameID;								// ���Ӽ� ���̵�

	int nGameServerOpenCount;					//���Ӽ��� �������� ����
	int nGameAcceptPortBegin;					//���Ӽ� udp �����Ʈ
	int nClientAcceptPort;						// Ŭ�� tcp ��Ʈ

	int nIocpMax;								// MaxSocket

	TConnectionInfo CashInfo;
	int nMasterCount;							// �� ������ ����
	TConnectionInfo MasterInfo[WORLDCOUNTMAX];	// ������ ����

	int nDBCount;								// �� ��� ����
	TConnectionInfo DBInfos[DBSERVERMAX];		// ��� ����

	TConnectionInfo	LogInfo;

	// ItemSerial �����µ� ���̴� SQL (�ٸ��� ���� ����ؼ� �ȵȴ�)
	TDBName MembershipDB;

	char szPrivateDolbyIp[IPLENMAX];					// ���� ������
	char szPublicDolbyIp[IPLENMAX];					// ���� ������
	int nControlPort;							// ���� ��Ʈ(��Ʈ��)
	int nAudioPort;								// ���� ��Ʈ(�����)

	int nProbePort;								//UDP�ּҸ� ����� ������ ���� ���κ� ����
	char szProbeIP[16];

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_GPK)
	SGPK::IGPKSvrDynCode* pDynCode;
	SGPK::IGPKCSAuth *pGpkCmd;
	//int nCodeIndex;
#endif	// _GPK

#if defined(_HSHIELD)
	AHNHS_SERVER_HANDLE	hHSServer;				// [Ȯ�� ��������] ���� �ڵ� ��ü
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
	bool bUseCmd;								//��ó�� ���ؼ� �����Ǿ�����
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	int nManagedID;
	TConnectionInfo ServiceInfo;

	int nWorldSetID;								// ���� ���̵� (������ ���� ���忡 �Ѵ븸 ����)

	int nVillageAcceptPort;
	int nGameAcceptPort;

	TConnectionInfo LoginInfo[LOGINCOUNTMAX];

	std::string szResourcePath;					// resource path
	std::string szResourceNation;

#if defined(_KR)
	TConnectionInfo AuthInfo;					// ������
	int nAuthDomainSN;							// ���� ������ SN
#endif	// _KR

#if defined(_CH)
	bool bFCM;									// �Ƿε� ����
#endif	// _CH

	TConnectionInfo	LogInfo;
#ifdef PRE_ADD_DOORS
	TConnectionInfo	DoorsInfo;
	TConnectionInfo DoorsIdenty[2];
#endif		//#ifdef PRE_ADD_DOORS

#if defined(_TW)
	TConnectionInfo AuthInfoTW;					// ������ (�����Ͼ� - Logout : ����� �α׾ƿ� ó��)
#endif	// _TW

#if defined(_US)
	TConnectionInfo PIInfo;
	char szRequestPage[256];
	char szUserAgent[50];
	char szServiceCode[50];
#endif	// #if defined(_US)
#if defined(_TH)
	TConnectionInfo AsiaSoftPCCafe;				// �ƽþ� ����Ʈ PC��
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
	bool bUseCmd;					// ��ó�� ���ؼ� �����Ǿ�����
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
	bool bUseCmd;					// ��ó�� ���ؼ� �����Ǿ�����
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	int nManagedID;
	TConnectionInfo ServiceInfo;

	int	nAcceptPort;
	int nThreadMax;

	TDBName MasterLogDB;			// MasterLogDB (�ؽ� web���񽺿� �ʿ��� ���)
	TDBName ServerLogDB;
	TDBName LogDB[WORLDCOUNTMAX];	// LogDB
	int	nLogDBCount;
	std::string szResourcePath;		// resource path
};

struct TCashConfig
{
	bool bUseCmd;								//��ó�� ���ؼ� �����Ǿ�����
	int nManagedID;								//Launcher ManagedID
	TConnectionInfo ServiceInfo;				//ServiceManagerInfo

	int nVersion;								// ����
	int nNation;								// �������� (eNation����)
	char szVersion[SERVERVERSIONMAX];
	char szResVersion[SERVERVERSIONMAX];
	USHORT nMajorVersion;
	USHORT nMinorVersion;

	int nWorldSetID;								// ���� ���̵�
	int nThreadMax;

	TDBName MembershipDB;
	TDBName WorldDB[WORLDCOUNTMAX];
	int nWorldDBCount;

	int nAcceptPort;

	TConnectionInfo LogInfo;

	std::string szResourcePath;		// resource path
	std::string szResourceNation;

#if defined(_KR) || defined(_US)
	TConnectionInfo CashInfo;		// ĳ�� ���� ����
	int nServerNo;					// ���� ��ȣ
#elif defined(_KRAZ)
	bool bTestServer;
#elif defined(_JP)
	WCHAR wszServiceType[10];			// alpha, real 
#elif defined(_TW) || defined(_TH)
	TConnectionInfo QueryPointInfo;	 // ����Ʈ ��ȸ ���� ����
	TConnectionInfo ShopItemInfo;	 // ������ ���� ���� ����
#if defined(_TW)
	TConnectionInfo CouponInfo;		 // ���� ���� ����
	TConnectionInfo CouponRollBackInfo;		 // ���� �ѹ� ���� ����
	int				nProtocolType;	 // �������� Ÿ�� 1:old, 2:new
#endif //#if defined(_TW)
#elif defined(_SG)
	std::string CouponServerInfo;
	std::string AuthToken;
#elif defined(_ID)
	std::string QueryPointURL;		// G-Cash �ܾ� ��ȸ
	std::string ItemShopURL;			// ������ ���� URL
#endif	// _KR
};
