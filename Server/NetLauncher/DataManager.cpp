
#include "stdafx.h"
#include "DataManager.h"
#include "XMLParser.h"
#include "Log.h"

CDataManager::CDataManager()
{
	memset(m_szServiceIP, 0, sizeof(m_szServiceIP));
	m_nServicePort = 0;
}

CDataManager::~CDataManager()
{	
}

bool CDataManager::LoadData()
{
	ClearData();

	CXMLParser parser;
	if (parser.Open("./System/ServerStruct.xml") == false)
	{
		g_Log.Log(_RED, L"ServerStruct.xml failed\r\n");
		return false;
	}
	
	if (parser.FirstChildElement("ServerStruct", true) == true)
	{	//Parse Struct
		if (parser.FirstChildElement("FtpInfo", true) == true)
		{
			do {
				TFtpInfo * pFtp = new TFtpInfo;
				_tcscpy_s(pFtp->szIP, parser.GetAttribute("IP"));
				pFtp->nPort = _tstoi(parser.GetAttribute("Port"));
				_tcscpy_s(pFtp->szID, parser.GetAttribute("ID"));
				_tcscpy_s(pFtp->szPass, parser.GetAttribute("Pass"));

				if (parser.FirstChildElement("URL") == true)
				{
					do {
						TURLInfo url;

						if (!_tcscmp(_T("info"), parser.GetAttribute("Type")))
							url.nType = _URLTYPE_STRUCTINFO;
						else if (!_tcscmp(_T("PatchRes"), parser.GetAttribute("Type")))
							url.nType = _URLTYPE_RESOURCE;
						else if (!_tcscmp(_T("PatchExe"), parser.GetAttribute("Type")))
							url.nType = _URLTYPE_EXE;
						else if (!_tcscmp(_T("PatchPartial"), parser.GetAttribute("Type")))
							url.nType = _URLTYPE_PARTIAL_RESOURCE;
						else
							_ASSERT(0);

						_tcscpy(url.szInfo, parser.GetAttribute("addr"));
						pFtp->URLList.push_back(url);
					} while (parser.NextSiblingElement("URL"));
				}
				m_FtpInfoList.push_back(pFtp);
			}
			while (parser.NextSiblingElement("FtpInfo"));
			parser.GoParent();
		}

		if (parser.FirstChildElement("ServiceManager") == true)
		{
			_tcscpy_s(m_szServiceIP, parser.GetAttribute("IP"));
			m_nServicePort = _tstoi(parser.GetAttribute("Port"));
		}

		if (parser.FirstChildElement("PartitionList", true) == true)
		{
			if (parser.FirstChildElement("PartitionInfo") == true)
			{
				do {
					TPartitionInfo * pPInfo = new TPartitionInfo;
					memset(pPInfo, 0, sizeof(TPartitionInfo));

					_tcscpy_s(pPInfo->szPartitionName, parser.GetAttribute("Name"));
					pPInfo->nPID = _tstoi(parser.GetAttribute("PID"));

					m_PartitionList[pPInfo->nPID] = pPInfo;
				} while (parser.NextSiblingElement("PartitionInfo") == true);
			}
			parser.GoParent();
		}

		if (parser.FirstChildElement("NetLauncherList", true) == true)
		{
			if (parser.FirstChildElement("Launcher") == true)
			{
				do 
				{
					TNetLauncher * pNet = new TNetLauncher;
					memset(pNet, 0, sizeof(TNetLauncher));

					pNet->nPID = _tstoi(parser.GetAttribute("PID"));
					pNet->nID = _tstoi(parser.GetAttribute("ID"));
					_tcscpy(pNet->szIP, parser.GetAttribute("IP"));
					pNet->nIP = _inet_addr(pNet->szIP);

					m_NetLauncherList[pNet->nID] = pNet;
				} while (parser.NextSiblingElement("Launcher") == true);
			}
			parser.GoParent();
		}

		if (parser.FirstChildElement("ServerInfo", true) == true)
		{
			const WCHAR * pType;
			do {
				pType = parser.GetAttribute("Type");

				TNetExcuteCommand * pCmd = new TNetExcuteCommand;
				memset(pCmd, 0, sizeof(TNetExcuteCommand));

				pCmd->nSID = _tstoi(parser.GetAttribute("SID"));
				_tcscpy_s(pCmd->szType, parser.GetAttribute("Type"));
				pCmd->nAssignedLauncherID = _tstoi(parser.GetAttribute("AssignedLauncherID"));
				_tcscpy_s(pCmd->szExcuteFile, parser.GetAttribute("ExcuteFileName"));
				_tcscpy_s(pCmd->szCmd, parser.GetAttribute("cmd"));

				if (!wcscmp(L"Village", parser.GetAttribute("Type")))
				{
					int n = 0;
					std::map <unsigned long, TNetLauncher*>::iterator iN = m_NetLauncherList.find(pCmd->nAssignedLauncherID);
					if (iN != m_NetLauncherList.end())
					{
						for (int h = 0; h < (int)(*iN).second->ExcuteList.size(); h++)
							if (!wcscmp((*iN).second->ExcuteList[h]->szType, L"Village"))
								n++;
					}
					pCmd->nInstIdx = n;
				}
				else
					pCmd->nInstIdx = -1;
				
				if (!wcscmp(L"Master", parser.GetAttribute("Type")) || !wcscmp(L"Village", parser.GetAttribute("Type")) || !wcscmp(L"Game", parser.GetAttribute("Type")))
				{
					if (parser.FirstChildElement("WorldName") == true)
						parser.Scan("%s", pCmd->szWorldName);
					else
						return false;
				}

				std::map<unsigned long, TNetLauncher*>::iterator ii = m_NetLauncherList.find(pCmd->nAssignedLauncherID);
				if (ii == m_NetLauncherList.end())
					return false;

				if (parser.FirstChildElement("Command") == true)
				{
					do {
						_swprintf(pCmd->szCmd+ _tcslen(pCmd->szCmd), _T(" %s"), parser.GetAttribute("cmd")); 
					} while(parser.NextSiblingElement("Command"));
				}
				else
					return false;
				
				m_ExcuteCmdList[pCmd->nSID] = pCmd;
				(*ii).second->ExcuteList.push_back(pCmd);
			} while (parser.NextSiblingElement("ServerInfo"));
		}
	}

	return true;
}

int CDataManager::VarifyNetLauncher(unsigned long nIP)
{
	std::map<unsigned long, TNetLauncher*>::iterator ii;
	for (ii = m_NetLauncherList.begin(); ii != m_NetLauncherList.end(); ii++)
	{
		if ((*ii).second->nIP == nIP)
			return (*ii).second->nID;
	}
	return 0;
}

const TNetLauncher * CDataManager::GetLauncherInfo(unsigned int nIP)
{
	std::map<unsigned long, TNetLauncher*>::iterator ii = m_NetLauncherList.find(nIP);
	if (ii != m_NetLauncherList.end())
		return (*ii).second;
	return NULL;
}

void CDataManager::GetCommandList(std::vector <int> * pvList)
{
	std::map <int, TNetExcuteCommand*>::iterator ii;
	for (ii = m_ExcuteCmdList.begin(); ii != m_ExcuteCmdList.end(); ii++)
		pvList->push_back((*ii).first);
}

void CDataManager::GetCommandListByPID(int nPID, std::vector <int> * pvList)
{
	std::map <int, TPartitionInfo*>::iterator ii = m_PartitionList.find(nPID);
	if (ii != m_PartitionList.end())
	{
		std::map <unsigned long, TNetLauncher*>::iterator ih;
		for (ih = m_NetLauncherList.begin(); ih != m_NetLauncherList.end(); ih++)
		{
			if ((*ih).second->nPID == (*ii).second->nPID)
			{
				std::map <int, TNetExcuteCommand*>::iterator ij;
				for (ij = m_ExcuteCmdList.begin(); ij != m_ExcuteCmdList.end(); ij++)
					if ((*ij).second->nAssignedLauncherID == (*ih).second->nID)
						pvList->push_back((*ij).first);
			}
		}
	}
}

const TNetExcuteCommand * CDataManager::GetCommand(int nSID)
{
	std::map <int, TNetExcuteCommand*>::iterator ii = m_ExcuteCmdList.find(nSID);
	return ii != m_ExcuteCmdList.end() ? (*ii).second : NULL;
}


void CDataManager::ClearData()
{
	for (int i = 0; i < (int)m_FtpInfoList.size(); i++)
		SAFE_DELETE(m_FtpInfoList[i]);
	m_FtpInfoList.clear();

	std::map <unsigned long, TNetLauncher*>::iterator in;
	for (in = m_NetLauncherList.begin(); in != m_NetLauncherList.end(); in++)
		SAFE_DELETE((*in).second);
	m_NetLauncherList.clear();

	std::map <int, TNetExcuteCommand*>::iterator ii;
	for (ii = m_ExcuteCmdList.begin(); ii != m_ExcuteCmdList.end(); ii++)
		SAFE_DELETE((*ii).second);
	m_ExcuteCmdList.clear();
}

CDataManager * CDataManager::GetInstance()
{
	static CDataManager s;
	return &s;
}