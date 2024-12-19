
#pragma once

enum eURLType
{
	_URLTYPE_STRUCTINFO,
	_URLTYPE_RESOURCE,
	_URLTYPE_EXE,
	//_URLTYPE_PARTIAL_RESOURCE,
};

struct TURLInfo
{
	int nType;
	TCHAR szInfo[256];
};

struct TFtpInfo
{
	TCHAR szIP[IPLENMAX];
	unsigned short nPort;
	TCHAR szID[IDLENMAX];
	TCHAR szPass[PASSWORDLENMAX];
	std::vector <TURLInfo> URLList;
};

struct TPartitionInfo
{
	TCHAR szPartitionName[256];
	int nPID;
};

struct TMonitorUserData
{
	char szUserName[NAMELENMAX];
	char szPassWord[PASSWORDLENMAX];
	int nMonitorLevel;					//eServerMonitorLevel
	bool bDuplicateLogin;				//중복로그인가능
};

//실재 구성 및 검증을 위한 xml파일을 로드만 하는 구조체 이것을 기준으로 서버 데이타를 구성을 하게 된다.
struct TStructSocketMax
{
	int nSocketMax;				//Client Accept가 있는 서버의 소켓풀 사이즈

	void InitSocketMax()
	{
		nSocketMax = 3000;
	}
};

struct TDolbyAxon
{
	int nDolbyWorldID;
	WCHAR wszPrivateDolbyIP[IPLENMAX];
	WCHAR wszPublicDolbyIP[IPLENMAX];
	int nDolbyAPort;
	int nDolbyCPort;

	TDolbyAxon()
	{
		nDolbyWorldID = 0;
		memset(wszPrivateDolbyIP, 0, sizeof(wszPrivateDolbyIP));
		memset(wszPublicDolbyIP, 0, sizeof(wszPublicDolbyIP));
		nDolbyAPort = 0;
		nDolbyCPort = 0;
	}
};

struct TStructDolbyAxon
{
	std::vector <TDolbyAxon> vDolbyAxonInfo;

	void InitDolbyAxon()
	{
		vDolbyAxonInfo.clear();
	}
};

struct TStructDBMiddleWare
{
	int nDBMiddleWareAcceptPort;		//디비미들웨어의 서버(게임 빌리지)억셉트포트

	void InitDBMiddleWare()
	{
		nDBMiddleWareAcceptPort = 0;
	}
};

struct TStructLogServer
{
	int nLogServerAcceptPort;			//로그 서버의 억셉트포트

	void InitLogServer()
	{
		nLogServerAcceptPort = 0;
	}
};

struct TStructLoginServer
{
	int nLoginMasterAcceptPort;			//로그인 서버의 마스터 억셉트포트
	int nLoginClientAcceptPort;			//로그인 서버의 클라이언트 억셉트포트

	void InitLoginServer()
	{
		nLoginClientAcceptPort = 0;
		nLoginMasterAcceptPort = 0;
	}
};

struct TStructMasterServer
{
	int nMasterVillageAcceptPort;		//마스터 서버의 빌리지 억셉트포트
	int nMasterGameAcceptPort;			//마스터 서버의 게임 억셉트포트

	void InitMasterServer()
	{
		nMasterGameAcceptPort = 0;
		nMasterVillageAcceptPort = 0;
	}
};

struct TStructVillageServer
{
	int nVillageID;						//디폴트에는 세팅되지 않는다.
	int nVillageClientAcceptPort;		//빌리지 서버의 클라이언트 억셉트포트
	int nCombineWorld;

	void InitVillageServer()
	{
		nVillageID = 0;
		nVillageClientAcceptPort = 0;
		nCombineWorld = 0;
	}
};

struct TStructGameServer
{
	bool bPreLoad;						//디폴트는 켜져있다.
	int nGameClientAcceptUDPPort;		//게임 서버의 클라이언트 UDP억셉트포트
	int nGameClientAcceptTCPPort;		//게임 서버의 클라이언트 TCP억셉트포트
	int nGameAffinityType;				//디폴트는 하이브리드
	bool bGameWorldCombine;				//월드통합게임룸

	void InitGameServer()
	{
		bPreLoad = true;
		nGameClientAcceptTCPPort = 0;
		nGameClientAcceptUDPPort = 0;
		nGameAffinityType = 1;
		bGameWorldCombine = false;
	}
};

struct TStructCashServer
{
	int nCashAcceptPort;				//캐시서버의 억셉트포트

	void InitCashServer()
	{
		nCashAcceptPort = 0;
	}
};

struct TDefaultServerInfo : public TStructSocketMax, 
	TStructDBMiddleWare,
	TStructLogServer,
	TStructLoginServer,
	TStructMasterServer,
	TStructVillageServer,
	TStructGameServer,
	TStructCashServer,
	TStructDolbyAxon
{
	TDefaultServerInfo::TDefaultServerInfo()
	{
		Init();
	}

	void Init()
	{
		InitSocketMax();
		InitDBMiddleWare();
		InitLogServer();
		InitLoginServer();
		InitMasterServer();
		InitVillageServer();
		InitGameServer();
		InitCashServer();
		InitDolbyAxon();
	}
};

struct TSQLConnectionInfo
{
	int nPort;
	WCHAR wszIP[IPLENMAX];
	WCHAR wszDBID[IDLENMAX];
	WCHAR wszDBName[DBNAMELENMAX];
};

struct TDoorsConnectionInfo
{
	int nPort;
	WCHAR wszIP[IPLENMAX];
};

struct TDefaultDatabaseInfo
{
	TSQLConnectionInfo MemberShipDB;			//멤버쉽컨넥션은 하나이다.
	TSQLConnectionInfo ServerLogDB;					//로그디비는 하나만 사용한다.
	std::map <int, TSQLConnectionInfo> WorldDB;	//복수개의 월드컨넥션 정보를 갖는다. first key = worldid

	void Init()
	{
		memset(&MemberShipDB, 0, sizeof(TSQLConnectionInfo));
		memset(&ServerLogDB, 0, sizeof(TSQLConnectionInfo));
		WorldDB.clear();
	}
};

struct TSIDBaseConnectionInfo
{
	int nType;
	int nSID;
};

struct TDefaultServerConstructData
{
	int nSID;
	WCHAR wszType[16];
	int nServerType;
	int nAssignedLauncher;
	WCHAR wszExcutePath[MAX_PATH];
	WCHAR wszExcuteFile[EXCUTELENMAX];
	int nForServerType;					//sid base dbconnection을 자동생성하기 위한 데이톼
	int nServerNo;						//서버넘버 (캐시용 : 타 서버 필요시 확장가능)
	std::vector <int> nWorldIDList;
	std::vector <TSIDBaseConnectionInfo> SIDConnectionList;
};

struct TDBMiddleWareConstructData : public TDefaultServerConstructData, TStructDBMiddleWare
{
	TDBMiddleWareConstructData()
	{
		InitDBMiddleWare();
	}
};

struct TLoginServerConstructData : public TDefaultServerConstructData, TStructLoginServer, TStructSocketMax
{
	TLoginServerConstructData()
	{
		InitSocketMax();
		InitLoginServer();
	}
};

struct TMasterServerConstructData : public TDefaultServerConstructData, TStructMasterServer, TStructSocketMax
{
	TMasterServerConstructData()
	{
		InitSocketMax();
		InitMasterServer();
	}
};

struct TGameServerConstructData : public TDefaultServerConstructData, TStructGameServer, TStructSocketMax
{
	TGameServerConstructData()
	{
		InitSocketMax();
		InitGameServer();
	}
};

struct TVillageServerConstructData : public TDefaultServerConstructData, TStructVillageServer, TStructSocketMax
{
	TVillageServerConstructData()
	{
		InitSocketMax();
		InitVillageServer();
	}
};

struct TLogServerConstructData : public TDefaultServerConstructData, TStructLogServer
{
	TLogServerConstructData()
	{
		InitLogServer();
	}
};

struct TCashServerConstructData : public TDefaultServerConstructData, TStructCashServer
{
	TCashServerConstructData()
	{
		InitCashServer();
	}
};

struct TPreServerStructData
{
	std::map <int, TDefaultServerConstructData*> PreStructList;
};

struct TServerExcuteData
{
	int nSID;		//생성되는 서버의 고유번호
	WCHAR wszType[8];
	int nPatchType;
	int nServerType;
	int nAssignedLauncherID;
	int nCreateCount;
	int nCreateIndex;
	std::wstring wstrExcuteData;

	TServerExcuteData()
	{
		nSID = 0;
		memset(wszType, 0, sizeof(wszType));
		nServerType = 0;
		nAssignedLauncherID = 0;
		nCreateCount = 0;
		nCreateIndex = 0;
		nPatchType = -1;
	}
};

struct TNetLauncher
{
	int nWID;		//종속되는 월드 아이디
	int nPID;		//종속되는 파티션 아이디
	int nID;
	TCHAR szIP[IPLENMAX];
	WCHAR wszPublicIP[IPLENMAX];
	unsigned long nIP;
	std::vector <TServerExcuteData*> ExcuteList;
};

struct tMonitorChannelInfo
{
	int nWorldID;
	int nManagedID;
	sChannelInfo Info;
};


//for convert
struct TNetExcuteCommand
{
	int nSID;		//생성되는 서버의 고유번호
	TCHAR szType[8];
	char cType;
	int nAssignedLauncherID;
	WCHAR szExcuteFile[EXCUTELENMAX];
	WCHAR szCmd[EXCUTECMDMAX];
	std::vector <std::pair<int, std::wstring>> strConList;
	int nWorldID;
	int nFor;
	int nVillageID;

	//for dbserver
	std::vector <std::pair<int, int>> vAssignedTypeList;
	int nAssingedType;

	TNetExcuteCommand()
	{
		nSID = 0;
		memset(szType, 0, sizeof(szType));
		cType = 0;
		nAssignedLauncherID = 0;
		memset(szExcuteFile, 0, sizeof(szExcuteFile));
		memset(szCmd, 0, sizeof(szCmd));
		nWorldID = 0;
		nFor = MANAGED_TYPE_NONE;
		nAssingedType = 0;
		nVillageID = 0;
	}
};

struct TWorldConnectionInfo
{
	int nWorldID;
	int nVillagePort;
	int nGamePort;
	WCHAR wszIP[IPLENMAX];
};

#include "DNDataManagerBase.h"
class CDataManager : public CDNDataManagerBase
{
public:
	CDataManager();
	~CDataManager();

	static CDataManager * GetInstance();

	bool LoadUser();
	bool LoadStruct();
	bool ConvertStruct();
	bool LoadServerConstructData();

	bool SaveUser();
	int ChangeUserPassword(TChangePassword* pData);

	int VerifyNetLauncher(unsigned long nIP);
	int GetLauncherCount() { return (int)m_NetLauncherList.size(); }

	const TCHAR * GetIP() { return m_szServiceIP; }
	const TNetLauncher * GetLauncherInfo(unsigned int nIP);
	const TNetLauncher * GetLauncherInfobyID(int nNID);
	bool IsNeedOtherExcutePath(int nID);
	int GetLauncherCnt() { return (int)m_NetLauncherList.size(); }
	void SetLauncherPublicIP(int nID, char* szPublicIP);
	void GetLauncherIds(std::vector <TNetLauncherInfo> * pList);
	void GetLauncherIds(std::map <int, TNetLauncherInfo> * pList);
	void GetAssingedPatchTypeList(int nID, std::list <char> * pList);
	int GetAssignedCreateLastIndex(int nID, const TCHAR * pType);
	void SetAssignedCreateCount(int nID, const TCHAR * pType, int nCreateCount);

	void GetCommandList(std::vector <int> * pvList);
	void GetCommandListByPID(int nPID, std::vector <int> * pvList);
	void GetExeCommandList(int nNID, std::vector <TServerExcuteData> * pvList);
	const TServerExcuteData * GetCommand(int nSID);

	//
	bool CheckMonitorUser(const char * pName, const char * pPass, int &nLevel, bool &bCanDuplicate);

	inline WCHAR* GetDoorsIP(void){ return m_DoorsInfo.wszIP; }
	inline int GetDoorsPort(void){ return m_DoorsInfo.nPort; }

#if defined (_SERVICEMANAGER_EX)
public:
	void GetPartitionList(OUT vector<const TPartitionInfo*>& list) const;
	void GetWorldList(OUT vector<const TServerInfo*>& list) const;
	void GetLauncherList(OUT vector<const TNetLauncher*>& list) const;
	void GetPartitionText(OUT wchar_t text[256]) const;

	const TDefaultServerInfo& GetDefaultServerInfo() const { return m_DefaultServerConstructInfo; }
	const TDefaultDatabaseInfo& GetDefaultDatabaseInfo() const { return m_DefaultSQLData; }	
#endif // #if defined (_SERVICEMANAGER_EX)

	bool IsFirstGameCon(int nSID);

private:
	map <unsigned long, TNetLauncher*> m_NetLauncherList;
	map <int, TPartitionInfo*> m_PartitionList;
	vector <TMonitorUserData> m_MonitorUserData;

	TDefaultServerInfo m_DefaultServerConstructInfo;
	TDefaultDatabaseInfo m_DefaultSQLData;
	TPreServerStructData m_PreServerStructData;
	vector <int> m_vWorldList;	
	TDoorsConnectionInfo m_DoorsInfo;

	map <int, TServerExcuteData> m_ServerExcuteData;
	bool ConstructServerStruct();
	void ClearPreConstructData();

	void GetServerCountFromPreconstruct(int nWorldID, int nType, vector <const TDefaultServerConstructData*> &vConstDatalist);
	
	int GetAssingedUserCount(int nWorldID, int nVillageID);

	void MakeWord(const TDefaultServerConstructData * pConstruct, wstring &wstrMark, int nCreateIndex, int nCreateCount);

	void MakeDBConInfo(const TSQLConnectionInfo * pInfo, wstring &wstrOut);
	void MakeWorldDBConInfo(vector <int> &vWorldIDs, wstring &wstrOut);
	bool MakeConInfo(int nType, int nSID, wstring &wstr, bool bCalledGame = false, vector <int> * pvWorldID = NULL);
	void MakeDolbyInfo(int nWorldID, wstring &wstr);
	bool MakeFirstGameCon(wstring &wstr, int &nFarmServerID);

	TCHAR m_szServiceIP[IPLENMAX];
	USHORT m_nServicePort;

	void ClearData();
	void ClearUserData();

	void CheckChannelInfo() const;
};