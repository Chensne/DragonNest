
#pragma once

struct TNetExcuteCommand
{
	int nSID;		//생성되는 서버의 고유번호
	int nInstIdx;
	TCHAR szWorldName[WORLDNAMELENMAX];
	TCHAR szType[NAMELENMAX];
	int nAssignedLauncherID;
	WCHAR szExcuteFile[EXCUTELENMAX];
	WCHAR szCmd[EXCUTECMDMAX];
};

struct TNetLauncher
{
	int nPID;		//종속되는 파티션 아이디
	int nID;
	TCHAR szIP[IPLENMAX];
	unsigned long nIP;
	std::vector <TNetExcuteCommand*> ExcuteList;
};

enum eURLType
{
	_URLTYPE_STRUCTINFO,
	_URLTYPE_RESOURCE,
	_URLTYPE_EXE,
	_URLTYPE_PARTIAL_RESOURCE,
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

class CDataManager
{
public:
	CDataManager();
	~CDataManager();

	static CDataManager * GetInstance();
	bool LoadData();

	int VarifyNetLauncher(unsigned long nIP);

	const TCHAR * GetIP() { return m_szServiceIP; }
	const TNetLauncher * GetLauncherInfo(unsigned int nIP);
	int GetLauncherCnt() { return (int)m_NetLauncherList.size(); }

	void GetCommandList(std::vector <int> * pvList);
	void GetCommandListByPID(int nPID, std::vector <int> * pvList);
	const TNetExcuteCommand * GetCommand(int nSID);
	const TFtpInfo * GetFtpInfo() { return m_FtpInfoList[0]; }

private:
	std::vector <TFtpInfo*> m_FtpInfoList;
	std::map <unsigned long, TNetLauncher*> m_NetLauncherList;
	std::map <int, TNetExcuteCommand*> m_ExcuteCmdList;
	std::map <int, TPartitionInfo*> m_PartitionList;

	TCHAR m_szServiceIP[IPLENMAX];
	USHORT m_nServicePort;

	void ClearData();
};