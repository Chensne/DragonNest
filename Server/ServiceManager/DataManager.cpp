
#include "stdafx.h"
#include "DataManager.h"
#include "XMLParser.h"
#include "Log.h"
#include "URLUpdater.h"
#include "CommonMacros.h"
#include <fstream>
#include <iostream>
#include "DNServerPacket.h"
#include "DNServerDef.h"


extern TServiceManagerConfig g_Config;
CDataManager::CDataManager()
{
	memset(m_szServiceIP, 0, sizeof(m_szServiceIP));
	m_nServicePort = 0;
	memset(m_DoorsInfo.wszIP, 0, sizeof(m_DoorsInfo.wszIP));
	m_DoorsInfo.nPort = 0;
}

CDataManager::~CDataManager()
{	
	ClearUserData();
	ClearData();
}

bool CDataManager::SaveUser()
{
	if (m_MonitorUserData.empty())
	{
		TMonitorUserData Eye;
		strncpy_s (Eye.szUserName, "eyeadmin", NAMELENMAX);
		strncpy_s (Eye.szPassWord, "1111", PASSWORDLENMAX);
		Eye.nMonitorLevel = SERVERMONITOR_LEVEL_SUPERADMIN;
		Eye.bDuplicateLogin = true;
		m_MonitorUserData.push_back(Eye);

		TMonitorUserData Super;
		strncpy_s (Super.szUserName, "super", NAMELENMAX);
		strncpy_s (Super.szPassWord, "1111", PASSWORDLENMAX);
		Super.nMonitorLevel = SERVERMONITOR_LEVEL_SUPERADMIN;
		Super.bDuplicateLogin = true;
		m_MonitorUserData.push_back(Super);

		TMonitorUserData Admin;
		strncpy_s (Admin.szUserName, "admin", NAMELENMAX);
		strncpy_s (Admin.szPassWord, "1111", PASSWORDLENMAX);
		Admin.nMonitorLevel = SERVERMONITOR_LEVEL_ADMIN;
		Admin.bDuplicateLogin = true;
		m_MonitorUserData.push_back(Admin);

		TMonitorUserData Monitor;
		strncpy_s (Monitor.szUserName, "monitor", NAMELENMAX);
		strncpy_s (Monitor.szPassWord, "1111", PASSWORDLENMAX);
		Monitor.nMonitorLevel = SERVERMONITOR_LEVEL_MONITOR;
		Monitor.bDuplicateLogin = true;
		m_MonitorUserData.push_back(Monitor);
	}
	
	std::ofstream outFile("./System/MonitorUser.bin", ios::binary | ios::out);
	if (!outFile)
	{
		g_Log.Log(LogType::_FILELOG, L"fail to write MonitorUser.bin\r\n");
		return false;
	}

	for (int i=0; i<(int)m_MonitorUserData.size(); i++)
		outFile.write ((char*)&m_MonitorUserData[i], sizeof(m_MonitorUserData[i]));
	outFile.close();

	return true;
}

int CDataManager::ChangeUserPassword(TChangePassword* pData)
{
	bool bChange = false;
	for (int i=0; i<(int)m_MonitorUserData.size(); i++)
	{
		if (!strcmp(m_MonitorUserData[i].szUserName, pData->szUserName))
		{
			strncpy_s(m_MonitorUserData[i].szPassWord, pData->szPassWord, PASSWORDLENMAX);
			m_MonitorUserData[i].bDuplicateLogin = true;
			m_MonitorUserData[i].nMonitorLevel = pData->nMonitorLevel;
			bChange = true;
			break;
		}
	}

	if (!bChange)
	{
		TMonitorUserData New;
		strncpy_s (New.szUserName, pData->szUserName, NAMELENMAX);
		strncpy_s (New.szPassWord, pData->szPassWord, PASSWORDLENMAX);
		New.nMonitorLevel = pData->nMonitorLevel;
		New.bDuplicateLogin = true;
		m_MonitorUserData.push_back(New);
	}
	
	SaveUser();

	return ERROR_NONE;
}

bool CDataManager::LoadUser()
{
	ClearUserData();

	ifstream inFile ("./System/MonitorUser.bin", ios::binary | ios::in);
	if (!inFile)
	{
		g_Log.Log(LogType::_FILELOG, L"fail to open MonitorUser.bin\r\n");
		SaveUser();
	}
	
	TMonitorUserData User;
    while (inFile.read((char*)&User, sizeof(User))) {
        m_MonitorUserData.push_back(User);
    }

	inFile.close();

	return true;
}

bool CDataManager::ConstructServerStruct()
{
	//sid connection data가 없을 경우 자동으로 생성한다.

	//sid con 생성용 데이타 구축
	//std::vector <std::pair<int, std::vector <const TDefaultServerConstructData*>>> vDBList, vLogList;
	std::vector <std::pair<int, std::vector <const TDefaultServerConstructData*>>> vDBList;
	std::vector <const TDefaultServerConstructData*> vLogList;
	for (int j = 0; j < (int)m_vWorldList.size(); j++)
	{
		std::vector <const TDefaultServerConstructData*> vTemp;
		GetServerCountFromPreconstruct(m_vWorldList[j], MANAGED_TYPE_DB, vTemp);
		vDBList.push_back(std::make_pair(m_vWorldList[j], vTemp));
		vTemp.clear();

		GetServerCountFromPreconstruct(m_vWorldList[j], MANAGED_TYPE_LOG, vLogList);
	}
	std::vector <const TDefaultServerConstructData*>::iterator itorLogList = vLogList.begin();
	
	//로그서버는 하나씩 세팅이 되므로 반복자에 시작위치를 세팅
	/*std::vector <std::pair<int, std::vector <const TDefaultServerConstructData*>::iterator>> vLogSIDItorList;
	for (int j = 0; j < (int)vLogList.size(); j++)
		vLogSIDItorList.push_back(std::make_pair(vLogList[j].first, vLogList[j].second.begin()));*/
	
	CURLUpdater url;
	std::map <int, TDefaultServerConstructData*>::iterator ii;
	for (int h = 0; h < 2; h++)
	{
		for (int i = 1; i < MANAGED_TYPE_MAX; i++)
		{
			for (ii = m_PreServerStructData.PreStructList.begin(); ii != m_PreServerStructData.PreStructList.end(); ii++)
			{
				if (i == 1 && (*ii).second->nServerType != MANAGED_TYPE_LOG) continue;				
				else if (i == 2 && (*ii).second->nServerType != MANAGED_TYPE_DB) continue;
				else if (i == 3 && (*ii).second->nServerType != MANAGED_TYPE_LOGIN) continue;
				else if (i == 4 && (*ii).second->nServerType != MANAGED_TYPE_CASH) continue;
				else if (i == 5 && (*ii).second->nServerType != MANAGED_TYPE_MASTER) continue;
				else if (i == 6 && (*ii).second->nServerType != MANAGED_TYPE_VILLAGE) continue;
				else if (i == 7 && (*ii).second->nServerType != MANAGED_TYPE_GAME) continue;

				std::map <int, TServerExcuteData>::iterator itor;
				if (h == 0)
				{
					itor = m_ServerExcuteData.find((*ii).second->nSID);
					if (itor != m_ServerExcuteData.end())
					{
						_ASSERT_EXPR(0, L"[ConstructServerStruct] m_ServerExcuteData can not find SID from m_PreServerStructData 1" );
						return false;
					}

					TServerExcuteData Excute;

					Excute.nSID = (*ii).second->nSID;
					Excute.nServerType = (*ii).second->nServerType;
					Excute.nPatchType = url.GetPatchServerType((*ii).second->wszType);
					Excute.nAssignedLauncherID = (*ii).second->nAssignedLauncher;
					STRCPYW(Excute.wszType, 8, (*ii).second->wszType);

					Excute.nCreateIndex = GetAssignedCreateLastIndex(Excute.nAssignedLauncherID, Excute.wszType);
					Excute.nCreateCount = Excute.nCreateIndex + 1;
					SetAssignedCreateCount(Excute.nAssignedLauncherID, Excute.wszType, Excute.nCreateIndex + 1);

					m_ServerExcuteData.insert(std::make_pair(Excute.nSID, Excute));

					itor = m_ServerExcuteData.find(Excute.nSID);
					if (itor != m_ServerExcuteData.end())
					{
						std::map<unsigned long, TNetLauncher*>::iterator iterlauncher = m_NetLauncherList.find(Excute.nAssignedLauncherID);
						if (iterlauncher == m_NetLauncherList.end())
							return false;

						(*iterlauncher).second->ExcuteList.push_back(&(*itor).second);
					}
					else
					{
						_ASSERT_EXPR(0, L"[ConstructServerStruct] m_ServerExcuteData can not find Excute.nSID" );
						return false;
					}
					
					if ((*ii).second->nServerType != MANAGED_TYPE_LOG)		//로그타입이 아니면
					{
						//check log sid connection
						bool bNeedLogCon = true;
						std::vector <TSIDBaseConnectionInfo>::iterator tempSIDCon;
						for (tempSIDCon = (*ii).second->SIDConnectionList.begin(); tempSIDCon != (*ii).second->SIDConnectionList.end(); tempSIDCon++)
						{
							if ((*tempSIDCon).nType == MANAGED_TYPE_LOG)
							{
								bNeedLogCon = false;
								break;
							}
						}
						
						if (bNeedLogCon && itorLogList != vLogList.end())
						{
							//SID connection이 존재하지 않으면 생성해줍니다.
							TSIDBaseConnectionInfo SIDConInfo;
							memset(&SIDConInfo, 0, sizeof(TSIDBaseConnectionInfo));

							SIDConInfo.nSID = (*itorLogList)->nSID;
							SIDConInfo.nType = GetServerType(L"Log");
							(*ii).second->SIDConnectionList.push_back(SIDConInfo);

							itorLogList++;
							if (itorLogList == vLogList.end())
								itorLogList = vLogList.begin();

							//int nWorldID = (*ii).second->nWorldIDList[0];		//디폴트월드를 기본으로 합니다.

							//std::vector <std::pair<int, std::vector <const TDefaultServerConstructData*>::iterator>>::iterator vWorldLogSIDItor;
							//for (vWorldLogSIDItor = vLogSIDItorList.begin(); vWorldLogSIDItor != vLogSIDItorList.end(); vWorldLogSIDItor++)
							//{
							//	if ((*vWorldLogSIDItor).first == nWorldID)
							//	{
							//		TSIDBaseConnectionInfo SIDConInfo;
							//		memset(&SIDConInfo, 0, sizeof(TSIDBaseConnectionInfo));

							//		SIDConInfo.nSID = (*(*vWorldLogSIDItor).second)->nSID;
							//		SIDConInfo.nType = GetServerType(L"Log");
							//		(*ii).second->SIDConnectionList.push_back(SIDConInfo);

							//		(*vWorldLogSIDItor).second++;
							//		for (int j = 0; j < (int)vLogList.size(); j++)
							//		{
							//			if (nWorldID == vLogList[j].first)
							//			{
							//				if ((*vWorldLogSIDItor).second == vLogList[j].second.end())
							//					(*vWorldLogSIDItor).second = vLogList[j].second.begin();
							//				break;
							//			}
							//		}
							//		break;
							//	}
							//}
						}

						if ((*ii).second->nServerType != MANAGED_TYPE_VILLAGE && (*ii).second->nServerType != MANAGED_TYPE_GAME) continue;	//게임과 빌리지만 디비컨이 필요함
						bool bNeedDBCon = true;
						for (tempSIDCon = (*ii).second->SIDConnectionList.begin(); tempSIDCon != (*ii).second->SIDConnectionList.end(); tempSIDCon++)
						{
							if ((*tempSIDCon).nType == MANAGED_TYPE_DB)
							{
								bNeedDBCon = false;
								break;
							}
						}

						if (bNeedDBCon)
						{
							//SID connection이 존재하지 않으면 생성해줍니다.
							int nWorldID = (*ii).second->nWorldIDList[0];		//디폴트월드를 기본으로 합니다.
							
							for (int j = 0; j < (int)vDBList.size(); j++)
							{
								if (vDBList[j].first == nWorldID)
								{
									for (int f = 0; f < (int)vDBList[j].second.size(); f++)
									{
										int nTempType = (*vDBList[j].second[f]).nForServerType;
										if ((*vDBList[j].second[f]).nForServerType != MANAGED_TYPE_NONE)
										{
											//전체를 커버하는 디비서버가 아니라면 확인
											if ((*ii).second->nServerType != (*vDBList[j].second[f]).nForServerType)
												continue;
										}

										TSIDBaseConnectionInfo SIDConInfo;
										memset(&SIDConInfo, 0, sizeof(TSIDBaseConnectionInfo));

										SIDConInfo.nSID = (*vDBList[j].second[f]).nSID;
										SIDConInfo.nType = GetServerType(L"DB");
										(*ii).second->SIDConnectionList.push_back(SIDConInfo);
									}
									break;
								}
							}
						}
					}
				}
				else
				{
					itor = m_ServerExcuteData.find((*ii).second->nSID);
					if (itor != m_ServerExcuteData.end())
						MakeWord((*ii).second, (*itor).second.wstrExcuteData, (*itor).second.nCreateIndex, (*itor).second.nCreateCount);
					else
					{
						_ASSERT_EXPR(0, L"[ConstructServerStruct] m_ServerExcuteData can not find nSID from m_PreServerStructData 2" );
						return false;
					}
				}
			}
		}
	}

	std::vector<std::pair<int, std::wstring>> vTTemp;

	std::map <int, TServerExcuteData>::iterator finder;
	for (finder = m_ServerExcuteData.begin(); finder != m_ServerExcuteData.end(); finder++)
	{
		if ((*finder).second.nServerType == MANAGED_TYPE_DB)
		{
			int nPort;
			GetFirstRightIntValue(L"dbp", (*finder).second.wstrExcuteData.c_str(), nPort);

			const TNetLauncher * pTTLauncher = GetLauncherInfobyID((*finder).second.nAssignedLauncherID);
			std::wstring wwstring = pTTLauncher->szIP;

			std::vector<std::pair<int, std::wstring>>::iterator iitemp;
			for (iitemp = vTTemp.begin(); iitemp != vTTemp.end(); iitemp++)
			{
				if ((*iitemp).first == nPort && !wcsicmp((*iitemp).second.c_str(), pTTLauncher->szIP))
					_ASSERT_EXPR(0, L"[ConstructServerStruct] Invalid Launcher IP" );
			}

			vTTemp.push_back(std::make_pair(nPort, wwstring));
		}

		if ((*finder).second.nServerType == MANAGED_TYPE_CASH || (*finder).second.nServerType == MANAGED_TYPE_MASTER)
		{
			//캐쉬나 마스터라면 동일머신에 두개이상 할당되어 있는지 확인한다
			if ((*finder).second.nCreateCount > 1)
			{
				int nFinderWorldID = 0;
				std::wstring wstrTempKey = (*finder).second.nServerType == MANAGED_TYPE_MASTER ? L"mwi" : L"cwid";
				GetFirstRightIntValue(wstrTempKey.c_str(), (*finder).second.wstrExcuteData.c_str(), nFinderWorldID);

				std::map <int, TServerExcuteData>::iterator verifyer;
				for (verifyer = m_ServerExcuteData.begin(); verifyer != m_ServerExcuteData.end(); verifyer++)
				{
					if ((*finder).second.nServerType == (*verifyer).second.nServerType && (*finder).second.nAssignedLauncherID == (*verifyer).second.nAssignedLauncherID\
						&& (*finder).second.nSID != (*verifyer).second.nSID)
					{
						int nVerifyerWorldID = 0;
						GetFirstRightIntValue(wstrTempKey.c_str(), (*verifyer).second.wstrExcuteData.c_str(), nVerifyerWorldID);

						if (nFinderWorldID == nVerifyerWorldID)
						{
							_ASSERT_EXPR(0, L"같은 머신에 캐쉬, 마스터서버를 복수개 지정할 경우 월드아이디를 체크해주세요!");
							return false;
						}
					}
				}
			}

			if ((*finder).second.nServerType == MANAGED_TYPE_MASTER)
			{
				int nFarmServerID = 0;
				if (MakeFirstGameCon((*finder).second.wstrExcuteData, nFarmServerID) == false)
					_ASSERT_EXPR(0, L"can not make farm game connection info!!");
				else
					g_Log.Log(LogType::_FILELOG, L"Selected Farm ServerID [%d]\r\n", nFarmServerID);
			}
		}
	}
	
	return true;
}

void CDataManager::ClearPreConstructData()
{
	m_DefaultServerConstructInfo.Init();
	m_DefaultSQLData.Init();

	std::map <int, TDefaultServerConstructData*>::iterator ii;
	for (ii = m_PreServerStructData.PreStructList.begin(); ii != m_PreServerStructData.PreStructList.end(); ii++)
		SAFE_DELETE((*ii).second);
	m_PreServerStructData.PreStructList.clear();
}

bool CDataManager::LoadStruct()
{
	if (ConvertStruct())
		return LoadServerConstructData();
	return false;
}

bool CDataManager::ConvertStruct()
{
	bool bFlag = false;
	{
		int nVersion = 0;
		std::string strTemp;
		if (_access("./System/ServerStruct.xml", 0) == -1)
		{
			g_Log.Log(LogType::_FILELOG, L"ServerStruct.xml failed\r\n");
			return false;
		}
		else
		{
			CXMLParser testparser;
			if (testparser.Open("./System/ServerStruct.xml") == false)
			{
				g_Log.Log(LogType::_FILELOG, L"ServerStruct.xml failed\r\n");
				return false;
			}

			if (testparser.FirstChildElement("ServerStruct", true))
			{
				if (testparser.FirstChildElement("Struct"))
				{
					//구형버전인지 확인 구형이라면 컨버전작업을 한다
					if (testparser.GetAttribute("Version"))
						nVersion = _wtoi(testparser.GetAttribute("Version"));

					//버전이 나누너이지면 확인하자 일단은 리턴
					return true;
				}
			}

			USES_CONVERSION;
			time_t curtime;
			time(&curtime);

			struct tm structtime;
			localtime_s(&structtime, &curtime);
			
			strTemp = "./System/ServerStruct_Old_";
			strTemp += I2A(structtime.tm_mday);
			strTemp += I2A(structtime.tm_min);
			strTemp += I2A(structtime.tm_sec);
			strTemp += ".xml";

			rename("./System/ServerStruct.xml", strTemp.c_str());
		}

		CXMLParser parser;
		if (parser.Open(strTemp.c_str()) == false)
		{
			g_Log.Log(LogType::_FILELOG, L"ServerStruct.xml failed\r\n");
			return false;
		}

		if (parser.FirstChildElement("ServerStruct", true))
		{
			std::map <int, TNetLauncher> NetLauncherList;
			std::map <int, TNetExcuteCommand> ExcuteCmdList;

			TDefaultServerInfo DefaultInfo;
			TDefaultDatabaseInfo DefaultDB;
			//if (nVersion <= 0)
			{
				std::map <int, TNetLauncher>::iterator NetItor;
				if (parser.FirstChildElement("NetLauncherList", true) == true)
				{
					if (parser.FirstChildElement("Launcher") == true)
					{
						do 
						{
							TNetLauncher Launcher;
							memset(&Launcher, 0, sizeof(TNetLauncher));

							if (parser.GetAttribute("WID") != NULL)
								Launcher.nWID = _tstoi(parser.GetAttribute("WID"));
							if (parser.GetAttribute("PID") != NULL)
								Launcher.nPID = _tstoi(parser.GetAttribute("PID"));
							Launcher.nID = _tstoi(parser.GetAttribute("ID"));
							_tcscpy_s(Launcher.szIP, parser.GetAttribute("IP"));
							Launcher.nIP = _inet_addr(Launcher.szIP);

							if (NetLauncherList.find(Launcher.nID) != NetLauncherList.end())
							{
								_ASSERT_EXPR(0, L"Check [Launcher] NID Duplicated");
								return false;
							}
							NetLauncherList[Launcher.nID] = Launcher;
						} while (parser.NextSiblingElement("Launcher") == true);
					}
					parser.GoParent();
				}

				CURLUpdater Url;
				if (parser.FirstChildElement("ServerInfo", true) == true)
				{
					do {
					
						TNetExcuteCommand Cmd;

						Cmd.nSID = _tstoi(parser.GetAttribute("SID"));
						_tcscpy_s(Cmd.szType, parser.GetAttribute("Type"));
						
						Cmd.cType = (char)GetServerTypeForConvert(Cmd.szType);
						Cmd.nAssignedLauncherID = _tstoi(parser.GetAttribute("AssignedLauncherID"));
						_tcscpy_s(Cmd.szExcuteFile, parser.GetAttribute("ExcuteFileName"));
						_tcscpy_s(Cmd.szCmd, parser.GetAttribute("cmd"));

						if (parser.FirstChildElement("Command") == true)
						{
							do {
								std::wstring wstrCmd;
								wstrCmd = parser.GetAttribute("cmd");
								int nType = GetServerTypeForConvert(parser.GetAttribute("Type"));
								Cmd.strConList.push_back(std::make_pair(nType, wstrCmd));
							} while(parser.NextSiblingElement("Command"));
						}
						else
							return false;

						if (ExcuteCmdList.find(Cmd.nSID) != ExcuteCmdList.end())
						{
							g_Log.Log(LogType::_FILELOG, L"Check ServerStruct <ServerInfo> SID[%d] duplicated\n", Cmd.nSID);
							_ASSERT_EXPR(0, L"Check ServerStruct <ServerInfo> SID[%d] duplicated");
							return false;
						}
						
						ExcuteCmdList[Cmd.nSID] = Cmd;
						std::map <int, TNetExcuteCommand>::iterator finditor = ExcuteCmdList.find(Cmd.nSID);
						
					} while (parser.NextSiblingElement("ServerInfo"));
					parser.GoParent();
				}
			}

			DefaultInfo.nSocketMax = 3000;			//디폴트 3천으로 잡자
			DefaultInfo.nGameAffinityType = _GAMESERVER_AFFINITYTYPE_HYBRYD;

			std::vector <TWorldConnectionInfo> vWorld;
			std::map <int, TNetExcuteCommand>::iterator exeitor;
			for (exeitor = ExcuteCmdList.begin(); exeitor != ExcuteCmdList.end(); exeitor++)
			{
				if ((*exeitor).second.cType == MANAGED_TYPE_MASTER)
				{
					std::vector<std::wstring> tokens;
					TokenizeW((*exeitor).second.szCmd, tokens, L" ");

					TWorldConnectionInfo info;
					memset(&info, 0, sizeof(info));

					info.nWorldID = _wtoi(tokens[0].c_str());
					info.nVillagePort = _wtoi(tokens[1].c_str());
					info.nGamePort = _wtoi(tokens[2].c_str());

					std::map <int, TNetLauncher>::iterator netitor = NetLauncherList.find((*exeitor).second.nAssignedLauncherID);
					if (netitor != NetLauncherList.end())
					{
						STRCPYW(info.wszIP, IPLENMAX, (*netitor).second.szIP);
					}
					else
					{
						_ASSERT_EXPR(0, L"[ConvertStruct] NetLauncherList can not find nAssignedLauncherID 1" );
					}

					(*exeitor).second.nWorldID = info.nWorldID;
					vWorld.push_back(info);
				}
			}

			for (exeitor = ExcuteCmdList.begin(); exeitor != ExcuteCmdList.end(); exeitor++)
			{
				std::map <int, TNetLauncher>::iterator netitor = NetLauncherList.find((*exeitor).second.nAssignedLauncherID);
				if (netitor == NetLauncherList.end())
				{
					_ASSERT_EXPR(0, L"[ConvertStruct] NetLauncherList can not find nAssignedLauncherID 2" );
					return false;
				}

				switch ((*exeitor).second.cType)
				{
				case MANAGED_TYPE_LOGIN:
				case MANAGED_TYPE_LOG:
					{
						//로그와 로그인은 월드에 비종속적
						(*netitor).second.nWID = 0;

						std::vector<std::wstring> tokens;
						TokenizeW((*exeitor).second.szCmd, tokens, L" ");

						{
							if ((*exeitor).second.cType == MANAGED_TYPE_LOGIN)
							{
								if (DefaultInfo.nLoginMasterAcceptPort <= 0 || DefaultInfo.nLoginMasterAcceptPort > _wtoi(tokens[3].c_str()))
									DefaultInfo.nLoginMasterAcceptPort = _wtoi(tokens[3].c_str());

								if (DefaultInfo.nLoginClientAcceptPort <= 0 || DefaultInfo.nLoginClientAcceptPort > _wtoi(tokens[4].c_str()))
									DefaultInfo.nLoginClientAcceptPort = _wtoi(tokens[4].c_str());
							}
							else if ((*exeitor).second.cType == MANAGED_TYPE_LOG)
							{
								if (DefaultInfo.nLogServerAcceptPort <= 0 || DefaultInfo.nLogServerAcceptPort > _wtoi(tokens[0].c_str()))
									DefaultInfo.nLogServerAcceptPort = _wtoi(tokens[0].c_str());
							}

							std::vector <std::pair<int, std::wstring>>::iterator conitor;
							for (conitor = (*exeitor).second.strConList.begin(); conitor != (*exeitor).second.strConList.end(); conitor++)
							{
								if ((*conitor).first != MANAGED_TYPE_DB) continue;

								std::vector<std::wstring> tokens;
								TokenizeW((*conitor).second, tokens, L" ");

								if (wcsstr((*conitor).second.c_str(), L"Membership"))
								{
									if (DefaultDB.MemberShipDB.nPort <= 0)
									{
										STRCPYW(DefaultDB.MemberShipDB.wszIP, IPLENMAX, tokens[0].c_str());
										DefaultDB.MemberShipDB.nPort = _wtoi(tokens[1].c_str());
										STRCPYW(DefaultDB.MemberShipDB.wszDBID, IDLENMAX, tokens[2].c_str());
										STRCPYW(DefaultDB.MemberShipDB.wszDBName, DBNAMELENMAX, tokens[3].c_str());
									}
								}
								else if (wcsstr((*conitor).second.c_str(), L"World"))
								{
									std::map <int, TSQLConnectionInfo>::iterator dbitor = DefaultDB.WorldDB.find(_wtoi(tokens[0].c_str()));
									if (dbitor == DefaultDB.WorldDB.end())
									{
										TSQLConnectionInfo sql;
										STRCPYW(sql.wszIP, IPLENMAX, tokens[2].c_str());
										sql.nPort = _wtoi(tokens[3].c_str());
										STRCPYW(sql.wszDBID, IDLENMAX, tokens[4].c_str());
										STRCPYW(sql.wszDBName, NAMELENMAX, tokens[5].c_str());

										DefaultDB.WorldDB.insert(std::make_pair(_wtoi(tokens[0].c_str()), sql));
									}
								}
								else if (wcsstr((*conitor).second.c_str(), L"ServerLog"))
								{
									if (DefaultDB.ServerLogDB.nPort <= 0)
									{
										STRCPYW(DefaultDB.ServerLogDB.wszIP, IPLENMAX, tokens[0].c_str());
										DefaultDB.ServerLogDB.nPort = _wtoi(tokens[1].c_str());
										STRCPYW(DefaultDB.ServerLogDB.wszDBID, IDLENMAX, tokens[2].c_str());
										STRCPYW(DefaultDB.ServerLogDB.wszDBName, DBNAMELENMAX, tokens[3].c_str());
									}
								}
							}
						}
						break;
					}
				case MANAGED_TYPE_CASH:
				case MANAGED_TYPE_MASTER:
					{
						std::vector<std::wstring> tokens;
						TokenizeW((*exeitor).second.szCmd, tokens, L" ");

						if ((*exeitor).second.cType == MANAGED_TYPE_MASTER)
						{
							(*netitor).second.nWID = _wtoi(tokens[0].c_str());

							if (DefaultInfo.nMasterVillageAcceptPort <= 0 || DefaultInfo.nMasterVillageAcceptPort > _wtoi(tokens[1].c_str()))
								DefaultInfo.nMasterVillageAcceptPort = _wtoi(tokens[1].c_str());

							if (DefaultInfo.nMasterGameAcceptPort <= 0 || DefaultInfo.nMasterGameAcceptPort > _wtoi(tokens[2].c_str()))
								DefaultInfo.nMasterGameAcceptPort = _wtoi(tokens[2].c_str());
						}
						else if ((*exeitor).second.cType == MANAGED_TYPE_CASH)
						{
							(*netitor).second.nWID = _wtoi(tokens[0].c_str());

							if (DefaultInfo.nCashAcceptPort <= 0 || DefaultInfo.nCashAcceptPort > _wtoi(tokens[1].c_str()))
								DefaultInfo.nCashAcceptPort = _wtoi(tokens[1].c_str());
						}
						break;
					}
				case MANAGED_TYPE_VILLAGE:
					{
						std::vector<std::wstring> tokens1;
						TokenizeW((*exeitor).second.szCmd, tokens1, L" ");

						if (DefaultInfo.nVillageClientAcceptPort <= 0 || DefaultInfo.nVillageClientAcceptPort > _wtoi(tokens1[3].c_str()))
							DefaultInfo.nVillageClientAcceptPort = _wtoi(tokens1[3].c_str());

						(*exeitor).second.nVillageID = _wtoi(tokens1[1].c_str());

						std::vector <std::pair<int, std::wstring>>::iterator conitor;
						for (conitor = (*exeitor).second.strConList.begin(); conitor != (*exeitor).second.strConList.end(); conitor++)
						{
							if ((*conitor).first == MANAGED_TYPE_MASTER)
							{
								std::vector<std::wstring> tokens;
								TokenizeW((*conitor).second, tokens, L" ");

								std::vector <TWorldConnectionInfo>::iterator worlditor;
								for (worlditor = vWorld.begin(); worlditor != vWorld.end(); worlditor++)
								{
									if (!wcscmp((*worlditor).wszIP, tokens[0].c_str()) && (*worlditor).nVillagePort == _wtoi(tokens[1].c_str()))
									{
										(*netitor).second.nWID = (*worlditor).nWorldID;
										(*exeitor).second.nWorldID = (*worlditor).nWorldID;
										break;
									}
								}
							}
							else if ((*conitor).first == MANAGED_TYPE_DB)
							{
								std::vector<std::wstring> tokens;
								TokenizeW((*conitor).second, tokens, L" ");

								std::map <int, TNetExcuteCommand>::iterator secondexeitor;
								for (secondexeitor = ExcuteCmdList.begin(); secondexeitor != ExcuteCmdList.end(); secondexeitor++)
								{
									if ((*secondexeitor).second.cType == MANAGED_TYPE_DB)
									{
										std::map <int, TNetLauncher>::iterator secondnetitor = NetLauncherList.find((*secondexeitor).second.nAssignedLauncherID);
										if (secondnetitor == NetLauncherList.end())
										{
											_ASSERT_EXPR(0, L"[ConvertStruct] NetLauncherList can not find nAssignedLauncherID in MANAGED_TYPE_VILLAGE" );
											return false;
										}

										if (!wcscmp((*secondnetitor).second.szIP, tokens[0].c_str()))
										{
											if (_wtoi((*secondexeitor).second.szCmd) == _wtoi(tokens[1].c_str()))
											{
												std::vector <std::pair<int, int>>::iterator typeitor;
												for (typeitor = (*secondexeitor).second.vAssignedTypeList.begin(); typeitor != (*secondexeitor).second.vAssignedTypeList.end(); typeitor++)
												{
													if ((*typeitor).first != (*exeitor).second.nWorldID)
													{
														//이럼아니되옵니다. 지금은 디비서버가 월드 종속적이어야 합니다
														_ASSERT_EXPR(0, L"이럼아니되옵니다. 지금은 디비서버가 월드 종속적이어야 합니다");
														return false;
													}
												}
												(*secondexeitor).second.vAssignedTypeList.push_back(std::make_pair((*exeitor).second.nWorldID, (*exeitor).second.cType));
											}
										}
									}
								}
							}
						}
						break;
					}
				case MANAGED_TYPE_GAME:
					{
						std::vector<std::wstring> tokens1;
						TokenizeW((*exeitor).second.szCmd, tokens1, L" ");

						if (DefaultInfo.nGameClientAcceptUDPPort <= 0 || DefaultInfo.nGameClientAcceptUDPPort > _wtoi(tokens1[3].c_str()))
							DefaultInfo.nGameClientAcceptUDPPort = _wtoi(tokens1[3].c_str());

						if (DefaultInfo.nGameClientAcceptTCPPort <= 0 || DefaultInfo.nGameClientAcceptTCPPort > _wtoi(tokens1[4].c_str()))
							DefaultInfo.nGameClientAcceptTCPPort = _wtoi(tokens1[4].c_str());

						std::vector <std::pair<int, std::wstring>>::iterator conitor;
						for (conitor = (*exeitor).second.strConList.begin(); conitor != (*exeitor).second.strConList.end(); conitor++)
						{
							if ((*conitor).first == MANAGED_TYPE_MASTER)
							{
								std::vector<std::wstring> tokens;
								TokenizeW((*conitor).second, tokens, L" ");

								std::vector <TWorldConnectionInfo>::iterator worlditor;
								for (worlditor = vWorld.begin(); worlditor != vWorld.end(); worlditor++)
								{
									if (!wcscmp((*worlditor).wszIP, tokens[0].c_str()) && (*worlditor).nGamePort == _wtoi(tokens[1].c_str()))
									{
										(*netitor).second.nWID = (*worlditor).nWorldID;
										(*exeitor).second.nWorldID = (*worlditor).nWorldID;
										break;
									}
								}
							}
							else if ((*conitor).first == MANAGED_TYPE_DB)
							{
								std::vector<std::wstring> tokens;
								TokenizeW((*conitor).second, tokens, L" ");

								std::map <int, TNetExcuteCommand>::iterator secondexeitor;
								for (secondexeitor = ExcuteCmdList.begin(); secondexeitor != ExcuteCmdList.end(); secondexeitor++)
								{
									if ((*secondexeitor).second.cType == MANAGED_TYPE_DB)
									{
										std::map <int, TNetLauncher>::iterator secondnetitor = NetLauncherList.find((*secondexeitor).second.nAssignedLauncherID);
										if (secondnetitor == NetLauncherList.end())
										{
											_ASSERT_EXPR(0, L"[ConvertStruct] NetLauncherList can not find nAssignedLauncherID in MANAGED_TYPE_GAME" );
											return false;
										}

										if (!wcscmp((*secondnetitor).second.szIP, tokens[0].c_str()))
										{
											if (_wtoi((*secondexeitor).second.szCmd) == _wtoi(tokens[1].c_str()))
											{
												std::vector <std::pair<int, int>>::iterator typeitor;
												for (typeitor = (*secondexeitor).second.vAssignedTypeList.begin(); typeitor != (*secondexeitor).second.vAssignedTypeList.end(); typeitor++)
												{
													if ((*typeitor).first != (*exeitor).second.nWorldID)
													{
														//이럼아니되옵니다. 지금은 디비서버가 월드 종속적이어야 합니다.
														//테스트버전에서는 가능? 일단 이 변환은 본섭기준
														_ASSERT_EXPR(0, L"이럼아니되옵니다. 지금은 디비서버가 월드 종속적이어야 합니다");
														return false;
													}
												}
												(*secondexeitor).second.vAssignedTypeList.push_back(std::make_pair((*exeitor).second.nWorldID, (*exeitor).second.cType));
											}
										}
									}
								}
							}
						}
						break;
					}
				}
			}

			for (exeitor = ExcuteCmdList.begin(); exeitor != ExcuteCmdList.end(); exeitor++)
			{
				std::map <int, TNetLauncher>::iterator netitor = NetLauncherList.find((*exeitor).second.nAssignedLauncherID);
				if (netitor == NetLauncherList.end())
				{
					_ASSERT_EXPR(0, L"[ConvertStruct] NetLauncherList can not find nAssignedLauncherID 3" );
					return false;
				}

				switch ((*exeitor).second.cType)
				{
					case MANAGED_TYPE_DB:
					{
						int nAssingedType = 0;		//0이면 없음, 1은 게임전용, 2는 빌리지 전용 3은 둘다
						std::vector <std::pair<int, int>>::iterator typeitor;
						for (typeitor = (*exeitor).second.vAssignedTypeList.begin(); typeitor != (*exeitor).second.vAssignedTypeList.end(); typeitor++)
						{
							if ((*netitor).second.nWID != 0 && (*netitor).second.nWID != (*typeitor).first)
							{
								//역시나 이러시면 아니되옵니다.
								_ASSERT_EXPR(0, L"[ConvertStruct] ExcuteCmdList has invalid values");
								return false;
							}
							(*netitor).second.nWID = (*typeitor).first;

							if (nAssingedType == 0)
								nAssingedType = (*typeitor).second == MANAGED_TYPE_GAME ? 1 : 2;
							else if (nAssingedType > 0)
							{
								if ((nAssingedType == 1))
								{
									if ((*typeitor).second != MANAGED_TYPE_GAME)
										nAssingedType = 3;
								}
								else if ((nAssingedType == 2))
								{
									if ((*typeitor).second != MANAGED_TYPE_VILLAGE)
										nAssingedType = 3;
								}
							}
						}

						(*exeitor).second.nAssingedType = nAssingedType;

						if (DefaultInfo.nDBMiddleWareAcceptPort <= 0 || DefaultInfo.nDBMiddleWareAcceptPort > _wtoi((*exeitor).second.szCmd))
							DefaultInfo.nDBMiddleWareAcceptPort = _wtoi((*exeitor).second.szCmd);
						break;
					}
				}
			}

			//기본구조에서 얻을 정보를 다 얻었다. 새로운 xml파일을 만들자.
			USES_CONVERSION;

			CXMLCreater creater;
			if (creater.Init("ServerStruct.xml") == false)
			{
				_ASSERT_EXPR(0, L"[ConvertStruct] CXMLCreater fail to create erverStruct.xml");
				return false;
			}

			creater.AddElement("ServerStruct", true);

			creater.AddElement("Struct");
			creater.AddAttribute("Version", "1");

			//GSM용데이타
			parser.Reset();

			if (parser.FirstChildElement("ServerStruct", true))
			{
				if (parser.FirstChildElement("ServiceManager"))
				{
					creater.AddElement("ServiceManager");
					creater.AddAttribute("IP", parser.GetAttribute("IP"));
					creater.AddAttribute("Port", parser.GetAttribute("Port"));
				}

				if (parser.FirstChildElement("GSMInfo", true))
				{
					creater.AddElement("GSMInfo", true);

					if (parser.FirstChildElement("GSMDB"))
					{
						creater.AddElement("GSMDB");
						creater.AddAttribute("IP", parser.GetAttribute("IP"));
						creater.AddAttribute("DBName", parser.GetAttribute("DBName"));
					}

					creater.GoRarent();
					parser.GoParent();
				}

				if (parser.FirstChildElement("PartitionList", true))
				{
					creater.AddElement("PartitionList", true);

					if (parser.FirstChildElement("PartitionInfo", true))
					{
						creater.AddElement("PartitionInfo", true);
						creater.AddAttribute("Name", parser.GetAttribute("Name"));
						creater.AddAttribute("PID", parser.GetAttribute("PID"));

						if (parser.FirstChildElement("MasterLog"))
						{
							creater.AddElement("MasterLog");
							creater.AddAttribute("IP", parser.GetAttribute("IP"));
							creater.AddAttribute("DBName", parser.GetAttribute("DBName"));
						}

						if (parser.FirstChildElement("GameLog"))
						{
							creater.AddElement("GameLog");
							creater.AddAttribute("IP", parser.GetAttribute("IP"));
							creater.AddAttribute("DBName", parser.GetAttribute("DBName"));
							creater.AddAttribute("WorldID", parser.GetAttribute("WorldID"));
						}

						if (parser.FirstChildElement("MembershipDB"))
						{
							creater.AddElement("MembershipDB");
							creater.AddAttribute("IP", parser.GetAttribute("IP"));
							creater.AddAttribute("DBName", parser.GetAttribute("DBName"));
							creater.AddAttribute("WorldID", parser.GetAttribute("WorldID"));
						}

						if (parser.FirstChildElement("StagingDB"))
						{
							creater.AddElement("StagingDB");
							creater.AddAttribute("IP", parser.GetAttribute("IP"));
							creater.AddAttribute("DBName", parser.GetAttribute("DBName"));
						}

						if (parser.FirstChildElement("WorldDB"))
						{
							do {
								creater.AddElement("WorldDB");
								creater.AddAttribute("IP", parser.GetAttribute("IP"));
								creater.AddAttribute("DBName", parser.GetAttribute("DBName"));
								creater.AddAttribute("WorldID", parser.GetAttribute("WorldID"));
							} while (parser.NextSiblingElement("WorldDB"));
						}

						if (parser.FirstChildElement("ServerLogDB"))
						{
							do {
								creater.AddElement("ServerLogDB");
								creater.AddAttribute("IP", parser.GetAttribute("IP"));
								creater.AddAttribute("DBName", parser.GetAttribute("DBName"));
								creater.AddAttribute("WorldID", parser.GetAttribute("WorldID"));
							} while (parser.NextSiblingElement("ServerLogDB"));
						}
						creater.GoRarent();
						parser.GoParent();
					}					

					creater.GoRarent();
				}
			}

			creater.AddElement("NetLauncherList", true);
			std::map <int, TNetLauncher>::iterator netitor;
			for (netitor = NetLauncherList.begin(); netitor != NetLauncherList.end(); netitor++)
			{
				creater.AddElement("Launcher");
				creater.AddAttribute("ID", I2A((*netitor).second.nID));
				creater.AddAttribute("IP", (*netitor).second.szIP);
				creater.AddAttribute("WID", I2A((*netitor).second.nWID));
				creater.AddAttribute("PID", I2A((*netitor).second.nPID));
			}
			creater.GoRarent();

			creater.AddElement("DefaultServerInfo", true);

			creater.AddElement("SocketMax");
			creater.AddAttribute("Socket", DefaultInfo.nSocketMax);

			creater.AddElement("DBPort");
			creater.AddAttribute("Port", DefaultInfo.nDBMiddleWareAcceptPort);

			creater.AddElement("LoginMasterPort");
			creater.AddAttribute("Port", DefaultInfo.nLoginMasterAcceptPort);

			creater.AddElement("LoginClientPort");
			creater.AddAttribute("Port", DefaultInfo.nLoginClientAcceptPort);

			creater.AddElement("MasterVillagePort");
			creater.AddAttribute("Port", DefaultInfo.nMasterVillageAcceptPort);

			creater.AddElement("MasterGamePort");
			creater.AddAttribute("Port", DefaultInfo.nMasterGameAcceptPort);

			creater.AddElement("VillageClientPort");
			creater.AddAttribute("Port", DefaultInfo.nVillageClientAcceptPort);

			creater.AddElement("GameClientPortUDP");
			creater.AddAttribute("Port", DefaultInfo.nGameClientAcceptUDPPort);

			creater.AddElement("GameClientPortTCP");
			creater.AddAttribute("Port", DefaultInfo.nGameClientAcceptTCPPort);

			creater.AddElement("CashPort");
			creater.AddAttribute("Port", DefaultInfo.nCashAcceptPort);

			creater.AddElement("LogPort");
			creater.AddAttribute("Port", DefaultInfo.nLogServerAcceptPort);

			creater.GoRarent();

			creater.AddElement("DefaultDataBaseInfo", true);
			creater.AddElement("MemberShip");
			creater.AddAttribute("IP", DefaultDB.MemberShipDB.wszIP);
			creater.AddAttribute("Port", DefaultDB.MemberShipDB.nPort);
			creater.AddAttribute("DBID", DefaultDB.MemberShipDB.wszDBID);
			creater.AddAttribute("DBName", DefaultDB.MemberShipDB.wszDBName);

			if (DefaultDB.ServerLogDB.nPort > 0)
			{
				creater.AddElement("Log");
				creater.AddAttribute("IP", DefaultDB.ServerLogDB.wszIP);
				creater.AddAttribute("Port", DefaultDB.ServerLogDB.nPort);
				creater.AddAttribute("DBID", DefaultDB.ServerLogDB.wszDBID);
				creater.AddAttribute("DBName", DefaultDB.ServerLogDB.wszDBName);
			}

			std::map <int, TSQLConnectionInfo>::iterator ii;
			for (ii = DefaultDB.WorldDB.begin(); ii != DefaultDB.WorldDB.end(); ii++)
			{
				creater.AddElement("World");
				creater.AddAttribute("WorldID", (*ii).first);
				creater.AddAttribute("IP", (*ii).second.wszIP);
				creater.AddAttribute("Port", (*ii).second.nPort);
				creater.AddAttribute("DBID", (*ii).second.wszDBID);
				creater.AddAttribute("DBName", (*ii).second.wszDBName);
			}
			
			creater.GoRarent();

			for (exeitor = ExcuteCmdList.begin(); exeitor != ExcuteCmdList.end(); exeitor++)
			{
				creater.AddElement("ServerInfo", true);
				creater.AddAttribute("SID", (*exeitor).second.nSID);
				creater.AddAttribute("Type", (*exeitor).second.szType);
				creater.AddAttribute("AssignedLauncherID", (*exeitor).second.nAssignedLauncherID);
				creater.AddAttribute("ExcuteFileName", (*exeitor).second.szExcuteFile);

				if ((*exeitor).second.cType == MANAGED_TYPE_DB)
				{
					if ((*exeitor).second.nAssingedType == 0)
					{
						_ASSERT_EXPR(0, L"[ConvertStruct] Type == MANAGED_TYPE_DB && nAssingedType == 0");
						return false;
					}

					if ((*exeitor).second.nAssingedType < 3)
						creater.AddAttribute("For", (*exeitor).second.nAssingedType == 1 ? "Game" : "Village");
				}

				if ((*exeitor).second.cType == MANAGED_TYPE_VILLAGE)
				{
					creater.AddElement("VillageID");
					creater.AddAttribute("ID", (*exeitor).second.nVillageID);
				}

				creater.GoRarent();
			}

			//creater.GoRarent();
			creater.Create();

			return true;
		}
	}
	return false;
}

bool CDataManager::LoadServerConstructData()
{
	CXMLParser parser;
	if (parser.Open("./System/ServerStruct.xml") == false)
	{
		g_Log.Log(LogType::_FILELOG, L"ServerStruct.xml failed\r\n");
		return false;
	}

	//ChannelInfo도 읽어야한다.
	ClearChannelData();
	if (LoadChannelInfo() == false)
	{
		g_Log.Log(LogType::_FILELOG, L"ChannelInfo.xml failed\r\n");
		return false;
	}

	ClearData();
	ClearPreConstructData();
	if (parser.FirstChildElement("ServerStruct", true) == true)
	{	//Parse Struct
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

					std::map <int, TPartitionInfo*>::iterator PartItor = m_PartitionList.find(pPInfo->nPID);
					if (PartItor != m_PartitionList.end())
					{
						_ASSERT_EXPR(0, L"Check [PartitionInfo] PID Duplicated");
						SAFE_DELETE(pPInfo);
						return false;
					}
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

					if (parser.GetAttribute("WID") != NULL)
					{
						pNet->nWID = _tstoi(parser.GetAttribute("WID"));
						/*if (pNet->nWID <= 0)
						{
							_ASSERT_EXPR(0, L"Check [Launcher][WID] cant set 0");
							return false;
						}*/
					}
					else
					{
						_ASSERT_EXPR(0, L"Check [Launcher][WID]Not Found");
						SAFE_DELETE(pNet);
						return false;
					}

					if (parser.GetAttribute("PID") != NULL)
						pNet->nPID = _tstoi(parser.GetAttribute("PID"));
					pNet->nID = _tstoi(parser.GetAttribute("ID"));
					_tcscpy_s(pNet->szIP, parser.GetAttribute("IP"));
					pNet->nIP = _inet_addr(pNet->szIP);

					std::map <unsigned long, TNetLauncher*>::iterator NetItor = m_NetLauncherList.find(pNet->nID);
					if (NetItor != m_NetLauncherList.end())
					{
						_ASSERT_EXPR(0, L"Check [Launcher] NID Duplicated");
						SAFE_DELETE(pNet);
						return false;
					}
					m_NetLauncherList[pNet->nID] = pNet;

					std::vector <int>::iterator worldItor = std::find(m_vWorldList.begin(), m_vWorldList.end(), pNet->nWID);
					if (worldItor == m_vWorldList.end())
						m_vWorldList.push_back(pNet->nWID);
				} while (parser.NextSiblingElement("Launcher") == true);
			}
			parser.GoParent();
		}

		if (parser.FirstChildElement("DefaultServerInfo", true))
		{
			if (parser.FirstChildElement("SocketMax"))
				m_DefaultServerConstructInfo.nSocketMax = _wtoi(parser.GetAttribute("Socket"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][SocketMax]");
				return false;
			}

			if (parser.FirstChildElement("DBPort"))
				m_DefaultServerConstructInfo.nDBMiddleWareAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][DBPort]");
				return false;
			}

			if (parser.FirstChildElement("LoginMasterPort"))
				m_DefaultServerConstructInfo.nLoginMasterAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][LoginMasterPort]");
				return false;
			}

			if (parser.FirstChildElement("LoginClientPort"))
				m_DefaultServerConstructInfo.nLoginClientAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][LoginClientPort]");
				return false;
			}

			if (parser.FirstChildElement("MasterVillagePort"))
				m_DefaultServerConstructInfo.nMasterVillageAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][MasterVillagePort]");
				return false;
			}

			if (parser.FirstChildElement("MasterGamePort"))
				m_DefaultServerConstructInfo.nMasterGameAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][MasterGamePort]");
				return false;
			}

			if (parser.FirstChildElement("VillageClientPort"))
				m_DefaultServerConstructInfo.nVillageClientAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][VillageClientPort]");
				return false;
			}

			if (parser.FirstChildElement("GameClientPortUDP"))
				m_DefaultServerConstructInfo.nGameClientAcceptUDPPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][GameClientPortUDP]");
				return false;
			}

			if (parser.FirstChildElement("GameClientPortTCP"))
				m_DefaultServerConstructInfo.nGameClientAcceptTCPPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][GameClientPortTCP]");
				return false;
			}

			if (parser.FirstChildElement("CashPort"))
				m_DefaultServerConstructInfo.nCashAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][CashPort]");
				return false;
			}		

			if (parser.FirstChildElement("AffinityType"))
			{
				if (!wcsicmp(L"normal", parser.GetAttribute("Type")))
					m_DefaultServerConstructInfo.nGameAffinityType = 0;
				else if (!wcsicmp(L"hybrid", parser.GetAttribute("Type")))
					m_DefaultServerConstructInfo.nGameAffinityType = 1;
				else
					m_DefaultServerConstructInfo.nGameAffinityType = 2;
			}

			if (parser.FirstChildElement("LogPort"))
				m_DefaultServerConstructInfo.nLogServerAcceptPort = _wtoi(parser.GetAttribute("Port"));
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultServerInfo][LogPort]");
				return false;
			}

			if (parser.FirstChildElement("DolbyAxon"))
			{
				do{
					TDolbyAxon Axon;
					Axon.nDolbyWorldID = _wtoi(parser.GetAttribute("WorldID"));
					wcscpy_s(Axon.wszPrivateDolbyIP, parser.GetAttribute("PrivateIP"));
					wcscpy_s(Axon.wszPublicDolbyIP, parser.GetAttribute("PublicIP"));
					Axon.nDolbyAPort = _wtoi(parser.GetAttribute("APort"));
					Axon.nDolbyCPort = _wtoi(parser.GetAttribute("CPort"));

					m_DefaultServerConstructInfo.vDolbyAxonInfo.push_back(Axon);
				} while (parser.NextSiblingElement("DolbyAxon"));
			}
			if (parser.FirstChildElement("CombineWorld"))
			{
				m_DefaultServerConstructInfo.bGameWorldCombine = true;
			}
			/*else
			{
				_ASSERT_EXPR(0, L"Check [DolbyAxon]");
				return false;
			}*/

			//Doors IP/Port
			if(parser.FirstChildElement("DoorsInfo")== true)
			{
				m_DoorsInfo.nPort = _wtoi(parser.GetAttribute("Port"));
				wcscpy_s(m_DoorsInfo.wszIP, parser.GetAttribute("IP"));
			}

			parser.GoParent();
		}

		if (parser.FirstChildElement("DefaultDataBaseInfo", true))
		{
			if (parser.FirstChildElement("MemberShip"))
			{
				STRCPYW(m_DefaultSQLData.MemberShipDB.wszIP, IPLENMAX, parser.GetAttribute("IP"));
				m_DefaultSQLData.MemberShipDB.nPort = _wtoi(parser.GetAttribute("Port"));
				STRCPYW(m_DefaultSQLData.MemberShipDB.wszDBID, IDLENMAX, parser.GetAttribute("DBID"));
				STRCPYW(m_DefaultSQLData.MemberShipDB.wszDBName, DBNAMELENMAX, parser.GetAttribute("DBName"));
			}
			else
			{
				_ASSERT_EXPR(0, L"Check [DefaultDataBaseInfo][MemberShip]");
				return false;
			}

			if (parser.FirstChildElement("Log"))
			{
				STRCPYW(m_DefaultSQLData.ServerLogDB.wszIP, IPLENMAX, parser.GetAttribute("IP"));
				m_DefaultSQLData.ServerLogDB.nPort = _wtoi(parser.GetAttribute("Port"));
				STRCPYW(m_DefaultSQLData.ServerLogDB.wszDBID, IDLENMAX, parser.GetAttribute("DBID"));
				STRCPYW(m_DefaultSQLData.ServerLogDB.wszDBName, DBNAMELENMAX, parser.GetAttribute("DBName"));
			}

			if (parser.FirstChildElement("World"))
			{
				TSQLConnectionInfo SqlCon;
				std::map <int, TSQLConnectionInfo>::iterator ii;
				do 
				{
					//Verify
					int nWorldID = _wtoi(parser.GetAttribute("WorldID"));

					ii = m_DefaultSQLData.WorldDB.find(nWorldID);
					if (ii != m_DefaultSQLData.WorldDB.end())
					{
						_ASSERT_EXPR(0, L"Check [DefaultDataBaseInfo][World] WorldID Duplicated!");
						return false;
					}

					memset (&SqlCon, 0, sizeof(TSQLConnectionInfo));

					STRCPYW(SqlCon.wszIP, IPLENMAX, parser.GetAttribute("IP"));
					SqlCon.nPort = _wtoi(parser.GetAttribute("Port"));
					STRCPYW(SqlCon.wszDBID, IDLENMAX, parser.GetAttribute("DBID"));
					STRCPYW(SqlCon.wszDBName, DBNAMELENMAX, parser.GetAttribute("DBName"));

					m_DefaultSQLData.WorldDB.insert(std::make_pair(nWorldID, SqlCon));
				} while (parser.NextSiblingElement("World"));

				parser.GoParent();
			}
		}

		if (parser.FirstChildElement("ServerInfo", true))
		{
			int nServerType;
			int nSID, nAssignedLauncher;
			std::wstring wstrType, wstrExe, wstrExcutePath, wstrExcutePathTemp;
			std::map <int, TDefaultServerConstructData*>::iterator ii;
			bool bFirstGameServer = true;

			int nServerNo;
			wstrExcutePathTemp.clear();
			do {
				nServerType = MANAGED_TYPE_NONE;
				nSID = 0;
				nAssignedLauncher = 0;
				nServerNo = -1;

				nSID = _wtoi(parser.GetAttribute("SID"));
				wstrType = parser.GetAttribute("Type");
				nServerType = GetServerType(wstrType.c_str());
				wstrExe = parser.GetAttribute("ExcuteFileName");
				nAssignedLauncher = _wtoi(parser.GetAttribute("AssignedLauncherID"));
				
				wstrExcutePath.clear();
				if (parser.GetAttribute("x86Path"))
				{
					wstrExcutePath = parser.GetAttribute("x86Path");
					if (!wstrExcutePathTemp.empty() && wstrExcutePathTemp != wstrExcutePath)
					{
						_ASSERT_EXPR(0, L"x86Path는 한개의 경로만 사용가능 합니다.");
						return false;
					}
				}

				if (parser.GetAttribute("ServerNum"))
				{
					nServerNo = _wtoi(parser.GetAttribute("ServerNum"));
				}

				//Verify IDs
				ii = m_PreServerStructData.PreStructList.find(nSID);
				if (ii != m_PreServerStructData.PreStructList.end())
				{
					_ASSERT_EXPR(0, L"Check [ServerInfo][SID] Duplicated!");
					return false;
				}

				int nWorldID = 0;
				std::vector <int>::iterator worldItor;
				TDefaultServerConstructData * pConstructData = NULL;

				switch (nServerType)
				{
				case MANAGED_TYPE_DB:
					{
						TDBMiddleWareConstructData * pDB = new TDBMiddleWareConstructData;
						if (!pDB)
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] pDB == NULL");
							SAFE_DELETE(pDB);
							return false;
						}
						pConstructData = pDB;

						if (parser.FirstChildElement("DBport"))
						{
							pDB->nDBMiddleWareAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}

						if (parser.GetAttribute("For"))
							pDB->nForServerType = GetServerType(parser.GetAttribute("For"));
						else
							pDB->nForServerType = MANAGED_TYPE_NONE;
						break;
					}

				case MANAGED_TYPE_LOGIN:
					{
						TLoginServerConstructData * pLogin = new TLoginServerConstructData;
						if (!pLogin)
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] pLogin == NULL");
							return false;
						}
						pConstructData = pLogin;

						if (parser.FirstChildElement("SocketMax"))
						{
							pLogin->nSocketMax = _wtoi(parser.GetAttribute("Count"));
						}

						if (parser.FirstChildElement("MasterPort"))
						{
							pLogin->nLoginMasterAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}

						if (parser.FirstChildElement("ClientPort"))
						{
							pLogin->nLoginClientAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}
						break;
					}

				case MANAGED_TYPE_MASTER:
					{
						TMasterServerConstructData * pMaster = new TMasterServerConstructData;
						if (!pMaster)
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] pMaster == NULL");
							SAFE_DELETE(pMaster);
							return false;
						}
						pConstructData = pMaster;

						if (parser.FirstChildElement("SocketMax"))
						{
							pMaster->nSocketMax = _wtoi(parser.GetAttribute("Count"));
						}

						if (parser.FirstChildElement("VillagePort"))
						{
							pMaster->nMasterVillageAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}

						if (parser.FirstChildElement("GamePort"))
						{
							pMaster->nMasterGameAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}
						break;
					}

				case MANAGED_TYPE_VILLAGE:
					{
						TVillageServerConstructData * pVillage = new TVillageServerConstructData;
						if (!pVillage)
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] pVillage == NULL");
							SAFE_DELETE(pVillage);
							return false;
						}
						pConstructData = pVillage;

						if (parser.FirstChildElement("SocketMax"))
						{
							pVillage->nSocketMax = _wtoi(parser.GetAttribute("Count"));
						}

						if (parser.FirstChildElement("VillageID"))
						{
							pVillage->nVillageID = _wtoi(parser.GetAttribute("ID"));
						}
						else
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] Can't find VillageID");
							SAFE_DELETE(pVillage);
							return false;
						}

						if (parser.FirstChildElement("ClientPort"))
						{
							pVillage->nVillageClientAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}

						if( m_DefaultServerConstructInfo.bGameWorldCombine )
						{
							std::map <int, TSQLConnectionInfo>::iterator itor = m_DefaultSQLData.WorldDB.begin();
							if( itor != m_DefaultSQLData.WorldDB.end() )
							{
								pVillage->nCombineWorld = itor->first;
							}							
						}						
						break;
					}

				case MANAGED_TYPE_GAME:
					{
						TGameServerConstructData * pGame = new TGameServerConstructData;
						if (!pGame)
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] pGame == NULL");
							SAFE_DELETE(pGame);
							return false;
						}
						pConstructData = pGame;

						if (parser.FirstChildElement("SocketMax"))
						{
							pGame->nSocketMax = _wtoi(parser.GetAttribute("Count"));
						}

						if (parser.FirstChildElement("PreLoad"))
						{
							pGame->bPreLoad = !_wcsicmp(parser.GetAttribute("Load"), L"true") ? true : false;
						}

						if (parser.FirstChildElement("ClientPortUDP"))
						{
							pGame->nGameClientAcceptUDPPort = _wtoi(parser.GetAttribute("Port"));
						}

						if (parser.FirstChildElement("ClientPortTCP"))
						{
							pGame->nGameClientAcceptTCPPort = _wtoi(parser.GetAttribute("Port"));
						}
						if( bFirstGameServer )
						{
							if( m_DefaultServerConstructInfo.bGameWorldCombine )
							{
								pGame->bGameWorldCombine = true;								
							}
						}


						break;
					}

				case MANAGED_TYPE_LOG:
					{
						TLogServerConstructData * pLog = new TLogServerConstructData;
						if (!pLog)
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] pLog == NULL");
							return false;
						}
						pConstructData = pLog;

						
						if (parser.FirstChildElement("Logport"))
						{
							pLog->nLogServerAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}
						break;
					}

				case MANAGED_TYPE_CASH:
					{
						TCashServerConstructData * pCash = new TCashServerConstructData;
						if (!pCash)
						{
							_ASSERT_EXPR(0, L"[LoadServerConstructData] pCash == NULL");
							SAFE_DELETE(pCash);
							return false;
						}
						pConstructData = pCash;

						if (parser.FirstChildElement("AcceptPort"))
						{
							pCash->nCashAcceptPort = _wtoi(parser.GetAttribute("Port"));
						}
						break;
					}

				case MANAGED_TYPE_NONE:
					{
						pConstructData = new TDefaultServerConstructData;
						break;
					}
				default:
					{
						_ASSERT_EXPR(0, L"Check [ServerInfo][Type] Unmanaged Type!");
						return false;
					}
				}

				if (pConstructData == NULL) continue;
				pConstructData->nSID = nSID;
				pConstructData->nServerType = nServerType;
				wcscpy_s(pConstructData->wszType, wstrType.c_str());
				wcscpy_s(pConstructData->wszExcutePath, wstrExcutePath.c_str());
				wcscpy_s(pConstructData->wszExcuteFile, wstrExe.c_str());
				pConstructData->nServerNo = nServerNo;
				pConstructData->nAssignedLauncher = nAssignedLauncher;

				if (parser.FirstChildElement("WorldID"))
				{
					do {
						//서버별 월드 단위 검증 해야함
						nWorldID = _wtoi(parser.GetAttribute("ID"));

						worldItor = std::find(pConstructData->nWorldIDList.begin(), pConstructData->nWorldIDList.end(), nWorldID);
						if (worldItor != pConstructData->nWorldIDList.end())
						{
							_ASSERT_EXPR(0, L"Check [ServerInfo][Type:DB][WorldID] Duplicated");
							SAFE_DELETE(pConstructData);
							return false;
						}
						pConstructData->nWorldIDList.push_back(nWorldID);
					} while (parser.NextSiblingElement("WorldID"));
				}
				else
				{
					if (pConstructData->nServerType == MANAGED_TYPE_LOGIN)
					{
						//로그인은 특별대후 명시적세팅이 아닌경우 로그인은 관리하는 파티션내의 모든 월드를 연결한다.
						for (int g = 0; g < (int)m_vWorldList.size(); g++)
							pConstructData->nWorldIDList.push_back(m_vWorldList[g]);
					}
					else if (pConstructData->nServerType != MANAGED_TYPE_LOG)		//로그서버가 아닐경우에만 검사및 추가
					{
						bool bCombineCheck = false;
						if( m_DefaultServerConstructInfo.bGameWorldCombine )
						{
							if ( pConstructData->nServerType == MANAGED_TYPE_DB || pConstructData->nServerType == MANAGED_TYPE_CASH )
							{
								const TNetLauncher * pNetLauncher = GetLauncherInfobyID(pConstructData->nAssignedLauncher);

								if (pNetLauncher)
								{
									if (pNetLauncher->nWID <= 0)
									{
										_ASSERT_EXPR(0, L"Press Retry");		//이럴경우 내부에러?
										SAFE_DELETE(pConstructData);
										return false;
									}

									pConstructData->nWorldIDList.push_back(pNetLauncher->nWID);
								}
								else
								{
									_ASSERT_EXPR(0, L"Check [ServerInfo][AssignedLauncher] Not Fount!");		//이럴경우 내부에러?
									SAFE_DELETE(pConstructData);
									return false;
								}

								for(std::map <int, TSQLConnectionInfo>::iterator itor = m_DefaultSQLData.WorldDB.begin();itor != m_DefaultSQLData.WorldDB.end();itor++)
								{
									nWorldID = itor->first;

									if( nWorldID == pNetLauncher->nWID )
										continue;
									worldItor = std::find(pConstructData->nWorldIDList.begin(), pConstructData->nWorldIDList.end(), nWorldID);
									if (worldItor != pConstructData->nWorldIDList.end())
									{
										_ASSERT_EXPR(0, L"Check [ServerInfo][Type:DB][WorldID] Duplicated");
										SAFE_DELETE(pConstructData);
										return false;
									}
									pConstructData->nWorldIDList.push_back(nWorldID);

								}
								bCombineCheck = true;
							}
							else if( pConstructData->nServerType == MANAGED_TYPE_GAME && bFirstGameServer )
							{
								for(std::map <int, TSQLConnectionInfo>::iterator itor = m_DefaultSQLData.WorldDB.begin();itor != m_DefaultSQLData.WorldDB.end();itor++)
								{
									nWorldID = itor->first;

									worldItor = std::find(pConstructData->nWorldIDList.begin(), pConstructData->nWorldIDList.end(), nWorldID);
									if (worldItor != pConstructData->nWorldIDList.end())
									{
										_ASSERT_EXPR(0, L"Check [ServerInfo][Type:DB][WorldID] Duplicated");
										SAFE_DELETE(pConstructData);
										return false;
									}
									pConstructData->nWorldIDList.push_back(nWorldID);

								}
								bCombineCheck = true;
								bFirstGameServer = false;
							}
						}						

						if( !bCombineCheck )
						{
							const TNetLauncher * pNetLauncher = GetLauncherInfobyID(pConstructData->nAssignedLauncher);

							if (pNetLauncher)
							{
								if (pNetLauncher->nWID <= 0)
								{
									_ASSERT_EXPR(0, L"Press Retry");		//이럴경우 내부에러?
									SAFE_DELETE(pConstructData);
									return false;
								}

								pConstructData->nWorldIDList.push_back(pNetLauncher->nWID);
							}
							else
							{
								_ASSERT_EXPR(0, L"Check [ServerInfo][AssignedLauncher] Not Fount!");		//이럴경우 내부에러?
								SAFE_DELETE(pConstructData);
								return false;
							}
						}						
					}
				}

				if (parser.FirstChildElement("SIDConnection"))
				{
					TSIDBaseConnectionInfo SIDConInfo;
					do {
						memset(&SIDConInfo, 0, sizeof(TSIDBaseConnectionInfo));

						SIDConInfo.nSID = _wtoi(parser.GetAttribute("SID"));
						SIDConInfo.nType = GetServerType(parser.GetAttribute("Type"));

						pConstructData->SIDConnectionList.push_back(SIDConInfo);
					} while (parser.NextSiblingElement("SIDConnection"));
				}

				m_PreServerStructData.PreStructList.insert(std::make_pair(pConstructData->nSID, pConstructData));
			} while (parser.NextSiblingElement("ServerInfo"));
		}
	}

	CheckChannelInfo(); // ServerStruct 와 ChannelInfo 를 비교하여 검사한다.

	ConstructServerStruct();		//실재사용할 구성데이타를 만든다.
	ClearPreConstructData();		//구성용으로 읽었던 것은 지워버리자.

	return true;
}

void CDataManager::GetServerCountFromPreconstruct(int nWorldID, int nType, std::vector <const TDefaultServerConstructData*> &vConstDatalist)
{
	std::map <int, TDefaultServerConstructData*>::iterator ii;
	for (ii = m_PreServerStructData.PreStructList.begin(); ii != m_PreServerStructData.PreStructList.end(); ii++)
	{
		if ((*ii).second->nServerType == nType)
		{
			if (nType == MANAGED_TYPE_LOG)
				vConstDatalist.push_back((*ii).second);
			else if (nType == MANAGED_TYPE_DB)
			{
				if ((*ii).second->nWorldIDList[0] == nWorldID)
					vConstDatalist.push_back((*ii).second);
			}
		}
	}
}

int CDataManager::GetAssingedUserCount(int nWorldID, int nVillageID)
{
	const TVillageInfo * pVillageInfo = GetVillageInfo(nWorldID, nVillageID);

	int nSumCount = 0, nSumCollo = 0;

	if (pVillageInfo)
	{
		std::vector <TChannelInfo>::const_iterator ii;
		for (ii = pVillageInfo->vOwnedChannelList.begin(); ii != pVillageInfo->vOwnedChannelList.end(); ii++)
		{
			if (!wcsicmp((*ii).wszMapName, L"ColosseumLobbyMap"))
			{
				//콜로세움로비일경우에는 조금 다르게 처리
				nSumCollo = (*ii).nChannelMaxUser;
			}
			else
				nSumCount += (*ii).nChannelMaxUser;
		}
	}

	//너무 소켓풀값이 커지는 경우를 막아두긴 한건데 먼가 스세템적으로 처리가 되어야할 거 같음.....
	if (nSumCollo > 5000)
		return nSumCount + 5000;
	return nSumCount + nSumCollo;
}

void CDataManager::MakeWord(const TDefaultServerConstructData * pConstruct, std::wstring &wstrMark, int nCreateIndex, int nCreateCount)
{
	//DefaultSetting
	std::vector <TSIDBaseConnectionInfo>::const_iterator sidItor;

	USES_CONVERSION;

	wstrMark = L"/usecmd";

	if (wcslen(g_Config.wszRegion) > 0 && wcsicmp(g_Config.wszRegion, L"dev"))
	{
		wstrMark += L"/nation=";
		wstrMark += L"_";
		wstrMark += g_Config.wszRegion;
	}

	wstrMark += L"/sid=";
	wstrMark += I2W(pConstruct->nSID);

	if (wcslen(pConstruct->wszExcutePath) > 0)
	{
		wstrMark += L"/epath=";
		wstrMark += pConstruct->wszExcutePath;
	}	
	
	wstrMark += L"/exe=";
	wstrMark += pConstruct->wszExcuteFile;
	
	switch (pConstruct->nServerType)
	{
	case MANAGED_TYPE_LOG:
		{
			TLogServerConstructData * pLog = (TLogServerConstructData*)pConstruct;

			wstrMark += L"/lgp=";
			wstrMark += I2W(pLog->nLogServerAcceptPort > 0 ? pLog->nLogServerAcceptPort : m_DefaultServerConstructInfo.nLogServerAcceptPort + nCreateIndex);

			if (m_DefaultSQLData.ServerLogDB.nPort > 0)
			{
				wstrMark += L"/sldb=";
				MakeDBConInfo(&m_DefaultSQLData.ServerLogDB, wstrMark);
			}
			
			break;
		}

	case MANAGED_TYPE_DB:
		{
			TDBMiddleWareConstructData * pDB = (TDBMiddleWareConstructData*)pConstruct;

			wstrMark += L"/dbp=";
			wstrMark += I2W(pDB->nDBMiddleWareAcceptPort > 0 ? pDB->nDBMiddleWareAcceptPort : m_DefaultServerConstructInfo.nDBMiddleWareAcceptPort + nCreateIndex);
			
			for (sidItor = pConstruct->SIDConnectionList.begin(); sidItor != pConstruct->SIDConnectionList.end(); sidItor++)
			{
				if (MakeConInfo((*sidItor).nType, (*sidItor).nSID, wstrMark) == false)
				{
					_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_DB");
					return ;
				}
			}

			wstrMark += L"/msdb=";
			MakeDBConInfo(&m_DefaultSQLData.MemberShipDB, wstrMark);
			MakeWorldDBConInfo(pDB->nWorldIDList, wstrMark);
			break;
		}

	case MANAGED_TYPE_LOGIN:
		{
			TLoginServerConstructData * pLogin = (TLoginServerConstructData*)pConstruct;

			wstrMark += L"/smc=";
			wstrMark += I2W(pLogin->nSocketMax > 0 ? pLogin->nSocketMax : m_DefaultServerConstructInfo.nSocketMax);

			wstrMark += L"/lmp=";
			wstrMark += I2W(pLogin->nLoginMasterAcceptPort > 0 ? pLogin->nLoginMasterAcceptPort : m_DefaultServerConstructInfo.nLoginMasterAcceptPort + nCreateIndex);

			wstrMark += L"/lcp=";
			wstrMark += I2W(pLogin->nLoginClientAcceptPort > 0 ? pLogin->nLoginClientAcceptPort : m_DefaultServerConstructInfo.nLoginClientAcceptPort + nCreateIndex);
			
			for (sidItor = pConstruct->SIDConnectionList.begin(); sidItor != pConstruct->SIDConnectionList.end(); sidItor++)
			{
				if (MakeConInfo((*sidItor).nType, (*sidItor).nSID, wstrMark) == false)
				{
					_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_LOGIN");
					return ;
				}
			}

			wstrMark += L"/msdb=";
			MakeDBConInfo(&m_DefaultSQLData.MemberShipDB, wstrMark);
			MakeWorldDBConInfo(pLogin->nWorldIDList, wstrMark);
			break;
		}

	case MANAGED_TYPE_CASH:
		{
			TCashServerConstructData * pCash = (TCashServerConstructData*)pConstruct;

			wstrMark += L"/cwid=";
			wstrMark += I2W(pCash->nWorldIDList[0]);

			if (pConstruct->nServerNo >= 0)
			{
				wstrMark += L"/svrno=";
				wstrMark += I2W(pConstruct->nServerNo);
			}
			else
			{
				wstrMark += L"/svrno=";
				wstrMark += I2W(pCash->nWorldIDList[0] - 1);
			}

			wstrMark += L"/cp=";
			wstrMark += I2W(pCash->nCashAcceptPort > 0 ? pCash->nCashAcceptPort : m_DefaultServerConstructInfo.nCashAcceptPort + nCreateIndex);

			for (sidItor = pConstruct->SIDConnectionList.begin(); sidItor != pConstruct->SIDConnectionList.end(); sidItor++)
			{
				if (MakeConInfo((*sidItor).nType, (*sidItor).nSID, wstrMark) == false)
				{
					_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_CASH");
					return ;
				}
			}

			wstrMark += L"/msdb=";
			MakeDBConInfo(&m_DefaultSQLData.MemberShipDB, wstrMark);
			MakeWorldDBConInfo(pCash->nWorldIDList, wstrMark);
			break;
		}

	case MANAGED_TYPE_MASTER:
		{
			TMasterServerConstructData * pMaster = (TMasterServerConstructData*)pConstruct;

			wstrMark += L"/mwi=";			
			wstrMark += I2W(pMaster->nWorldIDList[0]);

			wstrMark += L"/mvp=";
			wstrMark += I2W(pMaster->nMasterVillageAcceptPort > 0 ? pMaster->nMasterVillageAcceptPort : m_DefaultServerConstructInfo.nMasterVillageAcceptPort + nCreateIndex);

			wstrMark += L"/mgp=";
			wstrMark += I2W(pMaster->nMasterGameAcceptPort > 0 ? pMaster->nMasterGameAcceptPort : m_DefaultServerConstructInfo.nMasterGameAcceptPort + nCreateIndex);

			if(wcslen(m_DoorsInfo.wszIP) > 0 && m_DoorsInfo.nPort > 0){
				//Doors 정보
				wstrMark += L"/door=";
				wstrMark += m_DoorsInfo.wszIP;
				wstrMark += L",";
				wstrMark += I2W(m_DoorsInfo.nPort);
				
				//MemberShipDB 정보
				if(wcslen(m_DefaultSQLData.MemberShipDB.wszIP) > 0 && m_DefaultSQLData.MemberShipDB.nPort > 0){
					wstrMark += L"/did1=";
					wstrMark += m_DefaultSQLData.MemberShipDB.wszIP;
					wstrMark += L",";
					wstrMark += I2W(m_DefaultSQLData.MemberShipDB.nPort);
				}
				
				//WorldDB 정보
				if(!m_DefaultSQLData.WorldDB.empty()){
					std::map <int, TSQLConnectionInfo>::iterator worldItor;
					worldItor = m_DefaultSQLData.WorldDB.find(pMaster->nWorldIDList[0]);
					if (worldItor != m_DefaultSQLData.WorldDB.end())
					{
						wstrMark += L"/did2=";
						wstrMark += worldItor->second.wszIP;
						wstrMark += L",";
						wstrMark += I2W(worldItor->second.nPort);
					}
				}
			}

			bool bMakeLogin = true;
			for (sidItor = pConstruct->SIDConnectionList.begin(); sidItor != pConstruct->SIDConnectionList.end(); sidItor++)
			{
				if ((*sidItor).nType == MANAGED_TYPE_LOGIN)
					bMakeLogin = false;

				if (MakeConInfo((*sidItor).nType, (*sidItor).nSID, wstrMark) == false)
				{
					_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_MASTER");
					return ;
				}
			}

			if (bMakeLogin && MakeConInfo(MANAGED_TYPE_LOGIN, 0, wstrMark, false, &pMaster->nWorldIDList) == false)
			{
				_ASSERT_EXPR(0, L"[MakeWord] bMakeLogin && MakeConInfo(MANAGED_TYPE_LOGIN, 0, wstrMark, false, &pMaster->nWorldIDList) == false");
				return ;
			}
			break;
		}

	case MANAGED_TYPE_VILLAGE:
		{
			TVillageServerConstructData * pVillage = (TVillageServerConstructData*)pConstruct;			

			int nVillageUserCount = GetAssingedUserCount(pVillage->nWorldIDList[0], pVillage->nVillageID);			
			int nStructUserCount = pVillage->nSocketMax > 0 ? pVillage->nSocketMax : m_DefaultServerConstructInfo.nSocketMax;
			int nSocketMax = nVillageUserCount >= nStructUserCount ? nVillageUserCount + 500 : nStructUserCount;

			wstrMark += L"/smc=";
			wstrMark += I2W(nSocketMax);

			wstrMark += L"/vwid=";
			wstrMark += I2W(pVillage->nWorldIDList[0]);

			wstrMark += L"/vcwid=";
			wstrMark += I2W(pVillage->nCombineWorld);

			wstrMark += L"/vid=";
			wstrMark += I2W(pVillage->nVillageID);

			wstrMark += L"/vcp=";
			wstrMark += I2W(pVillage->nVillageClientAcceptPort > 0 ? pVillage->nVillageClientAcceptPort : m_DefaultServerConstructInfo.nVillageClientAcceptPort + nCreateIndex);
			
			MakeDolbyInfo(pVillage->nWorldIDList[0], wstrMark);
			
			for (sidItor = pConstruct->SIDConnectionList.begin(); sidItor != pConstruct->SIDConnectionList.end(); sidItor++)
			{
				if (MakeConInfo((*sidItor).nType, (*sidItor).nSID, wstrMark) == false)
				{
					_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_VILLAGE 0");
					return ;
				}
			}

			if (MakeConInfo(MANAGED_TYPE_CASH, 0, wstrMark, false, &pVillage->nWorldIDList) == false)
			{
				_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_VILLAGE 1");
				return ;
			}

			if (MakeConInfo(MANAGED_TYPE_MASTER, 0, wstrMark, false, &pVillage->nWorldIDList) == false)
			{
				_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_VILLAGE 2");
				return ;
			}
			break;
		}

	case MANAGED_TYPE_GAME:
		{
			TGameServerConstructData * pGame = (TGameServerConstructData*)pConstruct;			

			wstrMark += L"/smc=";
			wstrMark += I2W(pGame->nSocketMax > 0 ? pGame->nSocketMax : m_DefaultServerConstructInfo.nSocketMax);

			wstrMark += L"/gpl=";
			wstrMark += pGame->bPreLoad == true ? L"1" : L"0";

			for (int j = 0; j < (int)pGame->nWorldIDList.size(); j++)
			{
				wstrMark += L"/gwid=";
				wstrMark += I2W(pGame->nWorldIDList[j]);
			}

			MakeDolbyInfo(pGame->nWorldIDList[0], wstrMark);

			wstrMark += L"/gat=";
			wstrMark += I2W(pGame->nGameAffinityType);

			wstrMark += L"/gucp=";
			//wstrMark += I2W(pGame->nGameClientAcceptUDPPort > 0 ? pGame->nGameClientAcceptUDPPort : m_DefaultServerConstructInfo.nGameClientAcceptUDPPort + (nCreateIndex * 10));
			wstrMark += I2W(pGame->nGameClientAcceptUDPPort > 0 ? pGame->nGameClientAcceptUDPPort : m_DefaultServerConstructInfo.nGameClientAcceptUDPPort + (nCreateIndex * 15));

			wstrMark += L"/gtcp=";
			wstrMark += I2W(pGame->nGameClientAcceptTCPPort > 0 ? pGame->nGameClientAcceptTCPPort : m_DefaultServerConstructInfo.nGameClientAcceptTCPPort + nCreateIndex);

			wstrMark += L"/gcwg=";
			wstrMark += pGame->bGameWorldCombine == true ? L"1" : L"0";

			for (sidItor = pConstruct->SIDConnectionList.begin(); sidItor != pConstruct->SIDConnectionList.end(); sidItor++)
			{
				if (MakeConInfo((*sidItor).nType, (*sidItor).nSID, wstrMark) == false)
				{
					_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_GAME 0");
					return ;
				}
			}

			if (MakeConInfo(MANAGED_TYPE_CASH, 0, wstrMark, false, &pGame->nWorldIDList) == false)
			{
				_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_GAME 1");
				return ;
			}

			if (MakeConInfo(MANAGED_TYPE_MASTER, 0, wstrMark, true, &pGame->nWorldIDList) == false)
			{
				_ASSERT_EXPR(0, L"[MakeWord] fail MakeConInfo in MANAGED_TYPE_GAME 2");
				return ;
			}

			bool bIsFirstGame = IsFirstGameCon(pGame->nSID);
			if (bIsFirstGame)
			{
				wstrMark += L"/gsat=";
				wstrMark += I2W(_GAMESERVER_AFFINITYTYPE_FARM);
			}
			break;
		}

	default:
		{
			_ASSERT_EXPR(0, L"[MakeWord] Invalid Type");
			return ;
		}
	}

	wstrMark += L"/pci=";
	wstrMark += I2W(nCreateIndex);

	wstrMark += L"/pcc=";
	wstrMark += I2W(nCreateCount);

	//mark set []는 제외
	//서버아이티(매네이지드아이디) [/sid=1000]
	//실행파일이름 [/exe=DBServer64.exe]

	//소켓맥스카운트 [/smc=1000]
	//프로세스 생성 인덱스 [/pci=0]
	//프로세스 생성 숫자   [/pcc=1]

	//서비스매니저아이피 [/sip=10.0.3.22]
	//서비스매니저포트 [/sp=80000]

	//데이타베이스
	//멤버쉽디비 [/msdb=192.168.0.20,43002,DragonNest,DNMemberShip]
	//월드디비 [/wdb=9,192.168.0.20,43002,DragonNest,DNWorld]
	//서버로그디비 [/sldb=192.168.0.20,43011,DragonNest,DNServerLog]

	//돌비정보 [/daip=10.0.3.22,30000,33000]		audioport, controlport

	//디비미들웨어 포트 [/dbp=2600]

	//공통 컨넥션
	//로그컨넥션 [/log=10.0.3.22,2613]
	//로그인컨넥션 [/login=10.0.3.22,7000]
	//캐쉬컨넥션 [/cash=10.0.3.22,12600]
	//마스터컨넥션 [/master=10.0.3.22,8000]
	//디비컨네션 [/db=10.0.3.22,2600]

	//로그인서버 마스터포트 [/lmp=7000]
	//로그인서버 클라이언트포트 [/lcp=14300]

	//마스터서버 월드아이디 [/mwi=1]
	//마스터서버 빌리지포트 [/mvp=8000]
	//마스터서버 게임포트 [/mgp=9000]
	
	//빌리지서버 월드아이디 [/vwid=1]
	//빌리지서버 빌리지아이디 [/vid=1]
	//빌리지서버 클라이언트포트 [/vcp=14400]

	//게임서버 월드아이디 [/gwid=1]
	//게임서버 프리로드 [/gpl=1]
	//게임서버 UDP클라이언트포트 [/gucp=15100]
	//게임서버 TCP클라이언트포트 [/gtcp=14500]
	//게임서버 할당타입 [/gat=1]

	//캐쉬서버 월드아이디 [/cwid=1]
	//캐쉬서버 포트 [/cp=12600]

	//로그서버 포트 [/lgp=300]
}

void CDataManager::MakeDBConInfo(const TSQLConnectionInfo * pInfo, std::wstring &wstrOut)
{
	USES_CONVERSION;
	wstrOut += pInfo->wszIP;
	wstrOut += L",";
	wstrOut += I2W(pInfo->nPort);
	wstrOut += L",";
	wstrOut += pInfo->wszDBID;
	wstrOut += L",";
	wstrOut += pInfo->wszDBName;
}

void CDataManager::MakeWorldDBConInfo(std::vector <int> &vWorldIDs, std::wstring &wstrOut)
{
	USES_CONVERSION;
	std::vector <int>::iterator worldItor;
	std::map <int, TSQLConnectionInfo>::iterator sqlItor;
	for (worldItor = vWorldIDs.begin(); worldItor != vWorldIDs.end(); worldItor++)
	{
		sqlItor = m_DefaultSQLData.WorldDB.find((*worldItor));
		if (sqlItor != m_DefaultSQLData.WorldDB.end())
		{
			wstrOut += L"/wdb=";
			wstrOut += I2W((*worldItor));
			wstrOut += L",";
			MakeDBConInfo(&(*sqlItor).second, wstrOut);
		}
	}
}

bool CDataManager::MakeConInfo(int nType, int nSID, std::wstring &wstr, bool bCalledGame, std::vector <int> * pvWorldID)
{
	std::wstring tempKey;
	if (nSID > 0)
	{
		std::map <int, TDefaultServerConstructData*>::iterator ii = m_PreServerStructData.PreStructList.find(nSID);
		if (ii != m_PreServerStructData.PreStructList.end())
		{
			if ((*ii).second->nServerType == nType)
			{
				if ((*ii).second->nServerType != MANAGED_TYPE_LOG && (*ii).second->nServerType != MANAGED_TYPE_DB && (*ii).second->nServerType != MANAGED_TYPE_LOGIN)
				{
					_ASSERT_EXPR(0, L"[MakeConInfo] (*ii).second->nServerType != MANAGED_TYPE_LOG && (*ii).second->nServerType != MANAGED_TYPE_DB && (*ii).second->nServerType != MANAGED_TYPE_LOGIN");
					return false;
				}

				switch ((*ii).second->nServerType)
				{
				case MANAGED_TYPE_LOG:
					{
						wstr += L"/log=";
						tempKey = L"lgp";
						break;
					}

				case MANAGED_TYPE_DB:
					{
						wstr += L"/db=";
						tempKey = L"dbp";
						break;
					}

				case MANAGED_TYPE_LOGIN:
					{
						wstr += L"/login=";
						tempKey = L"lmp";
						break;
					}

				default:
					return false;
				}

				//wstr += (*ii).second->nServerType == MANAGED_TYPE_LOG ? L"/log=" : L"/db=";
				const TNetLauncher * pLauncher = GetLauncherInfobyID((*ii).second->nAssignedLauncher);
				if (pLauncher)
				{
					wstr += pLauncher->szIP;
					wstr += L",";

					std::map <int, TServerExcuteData>::iterator itor = m_ServerExcuteData.find(nSID);
					if (itor != m_ServerExcuteData.end())
					{
						//if (GetFirstRightValue(nType == MANAGED_TYPE_LOG ? L"lgp" : L"dbp", (*itor).second.wstrExcuteData, wstr, true) == false)
						if (GetFirstRightValue(tempKey.c_str(), (*itor).second.wstrExcuteData, wstr, true) == false)
						
						{
							_ASSERT_EXPR(0, L"[MakeConInfo] GetFirstRightValue(tempKey.c_str(), (*itor).second.wstrExcuteData, wstr, true) == fals");
							return false;
						}
					}
					else
					{
						_ASSERT_EXPR(0, L"[MakeConInfo] m_ServerExcuteData can not find nSID");
						return false;
					}
					return true;
				}
				else
				{
					_ASSERT_EXPR(0, L"[MakeConInfo] pLauncher == NULL");
					return false;
				}
			}
		}
	}
	else
	{
		if (pvWorldID == NULL)
		{
			_ASSERT_EXPR(0, L"[MakeConInfo] pvWorldID == NULL");
			return false;
		}

		std::vector <int>::iterator worlditor;
		std::map <int, TDefaultServerConstructData*>::iterator preconitor;
		bool bFlag = false;
		if (nType == MANAGED_TYPE_LOGIN || nType == MANAGED_TYPE_CASH || nType == MANAGED_TYPE_MASTER)
		{
			std::map <int, TServerExcuteData>::iterator ii;
			for (ii = m_ServerExcuteData.begin(); ii != m_ServerExcuteData.end(); ii++)
			{
				preconitor = m_PreServerStructData.PreStructList.end();
				if ((*ii).second.nServerType == nType)
				{
					bFlag = false;
					const TNetLauncher * pLauncher = GetLauncherInfobyID((*ii).second.nAssignedLauncherID);
					if (pLauncher == NULL)
					{
						_ASSERT_EXPR(0, L"[MakeConInfo] pLauncher == NULL");
						return false;
					}

					//std::wstring tempKey;
					switch ((*ii).second.nServerType)
					{
					case MANAGED_TYPE_LOGIN:
						{
							wstr += L"/login=";
							tempKey =  L"lmp";
							break;
						}

					case MANAGED_TYPE_CASH:
						{
							preconitor = m_PreServerStructData.PreStructList.find((*ii).second.nSID);
							if (preconitor == m_PreServerStructData.PreStructList.end())
							{
								_ASSERT_EXPR(0, L"[MakeConInfo] m_PreServerStructData != m_ServerExcuteData");
								return false;
							}

							if( m_DefaultServerConstructInfo.bGameWorldCombine )
							{
								for (worlditor = pvWorldID->begin(); worlditor != pvWorldID->end(); worlditor++)
								{
									if( (*preconitor).second->nWorldIDList[0] == *worlditor )									
									{
										bFlag = true;										
									}
									break;
								}
							}
							else
							{
								for (worlditor = pvWorldID->begin(); worlditor != pvWorldID->end(); worlditor++)
								{
									std::vector <int>::iterator finditor = std::find((*preconitor).second->nWorldIDList.begin(), (*preconitor).second->nWorldIDList.end(), (*worlditor));
									if (finditor != (*preconitor).second->nWorldIDList.end())
									{
										bFlag = true;
										break;
									}
								}
							}
							
							if (bFlag == false) continue;

							wstr += L"/cash=";
							tempKey =  L"cp";
							break;
						}

					case MANAGED_TYPE_MASTER:
						{
							preconitor = m_PreServerStructData.PreStructList.find((*ii).second.nSID);
							if (preconitor == m_PreServerStructData.PreStructList.end())
							{
								_ASSERT_EXPR(0, L"[MakeConInfo] m_PreServerStructData != m_ServerExcuteData");
								return false;
							}

							for (worlditor = pvWorldID->begin(); worlditor != pvWorldID->end(); worlditor++)
							{
								std::vector <int>::iterator finditor = std::find((*preconitor).second->nWorldIDList.begin(), (*preconitor).second->nWorldIDList.end(), (*worlditor));
								if (finditor != (*preconitor).second->nWorldIDList.end())
								{
									bFlag = true;
									break;
								}
							}
							if (bFlag == false) continue;

							wstr += L"/master=";
							tempKey =  bCalledGame == false ? L"mvp" : L"mgp";
							break;
						}

					default:
						{
							_ASSERT_EXPR(0, L"[MakeConInfo] Invalid Server Type");
							return false;
						}
					}

					wstr += pLauncher->szIP;
					wstr += L",";

					if (GetFirstRightValue(tempKey.c_str(), (*ii).second.wstrExcuteData, wstr, true) == false)
					{
						_ASSERT_EXPR(0, L"[MakeConInfo] GetFirstRightValue(tempKey.c_str(), (*ii).second.wstrExcuteData, wstr, true) == false");
						return false;
					}
				}
			}
		}
		else
		{
			_ASSERT_EXPR(0, L"[MakeConInfo] Invalid nType");
			return false;
		}
	}
	return true;
}

void CDataManager::MakeDolbyInfo(int nWorldID, std::wstring &wstr)
{
	std::vector <TDolbyAxon>::iterator ii;
	for (ii = m_DefaultServerConstructInfo.vDolbyAxonInfo.begin(); ii != m_DefaultServerConstructInfo.vDolbyAxonInfo.end(); ii++)
	{
		if ((*ii).nDolbyWorldID == nWorldID)
		{
			USES_CONVERSION;
			wstr += L"/daip=";
			wstr += (*ii).wszPrivateDolbyIP;
			wstr += L",";
			wstr += (*ii).wszPublicDolbyIP;
			wstr += L",";
			wstr += I2W((*ii).nDolbyAPort);
			wstr += L",";
			wstr += I2W((*ii).nDolbyCPort);
			return ;
		}
	}
}

bool CDataManager::MakeFirstGameCon(wstring &wstr, int &nFarmServerID)
{
	std::map <int, TServerExcuteData>::iterator verifyer;
	for (verifyer = m_ServerExcuteData.begin(); verifyer != m_ServerExcuteData.end(); verifyer++)
	{
		std::wstring wstrTemp;
		if ((*verifyer).second.nServerType == MANAGED_TYPE_GAME && GetFirstRightValue(L"gsat", (*verifyer).second.wstrExcuteData, wstrTemp))
		{
			USES_CONVERSION;
			wstr += L"/fsid=";
			wstr += I2W((*verifyer).second.nSID);
			nFarmServerID = (*verifyer).second.nSID;
			return true;
		}
	}
	return false;
}

int CDataManager::VerifyNetLauncher(unsigned long nIP)
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

const TNetLauncher * CDataManager::GetLauncherInfobyID(int nNID)
{
	std::map<unsigned long, TNetLauncher*>::iterator ii;
	for (ii = m_NetLauncherList.begin(); ii != m_NetLauncherList.end(); ii++)
	{
		if ((*ii).second->nID == nNID)
			return (*ii).second;
	}
	return NULL;
}

bool CDataManager::IsNeedOtherExcutePath(int nID)
{
	bool bRet = false;
	std::map <int, TServerExcuteData>::iterator ii;
	for (ii = m_ServerExcuteData.begin(); ii != m_ServerExcuteData.end(); ii++)
	{
		if ((*ii).second.nAssignedLauncherID == nID)
		{
			std::wstring wstrExcutePath;
			GetFirstRightValue(L"epath", (*ii).second.wstrExcuteData, wstrExcutePath);

			bRet = wstrExcutePath.empty() ? false : true;
			if (bRet) return bRet;
		}
	}
	return bRet;
}

void CDataManager::SetLauncherPublicIP(int nID, char* szPublicIP)
{
	if (m_NetLauncherList.empty())
		return;

	if (m_NetLauncherList[nID])
		MultiByteToWideChar(CP_ACP, 0, szPublicIP, -1, m_NetLauncherList[nID]->wszPublicIP, IPLENMAX );
}

void CDataManager::GetLauncherIds(std::vector <TNetLauncherInfo> * pList)
{
	std::map<unsigned long, TNetLauncher*>::iterator ii;
	for (ii = m_NetLauncherList.begin(); ii != m_NetLauncherList.end(); ii++)
	{
		TNetLauncherInfo Info;
		memset(&Info, 0, sizeof(TNetLauncherInfo));

		Info.nNID = (*ii).second->nID;
		wcscpy_s(Info.wszIP, (*ii).second->szIP);
		wcscpy_s(Info.wszPublicIP, IPLENMAX, (*ii).second->wszPublicIP);
		pList->push_back(Info);
	}
}

void CDataManager::GetLauncherIds(std::map <int, TNetLauncherInfo> * pList)
{
	std::map<unsigned long, TNetLauncher*>::iterator ii;
	for (ii = m_NetLauncherList.begin(); ii != m_NetLauncherList.end(); ii++)
	{
		TNetLauncherInfo Info;
		memset(&Info, 0, sizeof(TNetLauncherInfo));

		Info.nNID = (*ii).second->nID;
		wcscpy_s(Info.wszIP, (*ii).second->szIP);
		wcscpy_s(Info.wszPublicIP, IPLENMAX, (*ii).second->wszPublicIP);
		pList->insert(std::make_pair(Info.nNID, Info));
	}
}

void CDataManager::GetAssingedPatchTypeList(int nID, std::list <char> * pList)
{
	std::map <int, TServerExcuteData>::iterator ii;
	for (ii = m_ServerExcuteData.begin(); ii != m_ServerExcuteData.end(); ii++)
	{
		if ((*ii).second.nAssignedLauncherID == nID)
		{
			bool bCheck = false;
			std::list <char>::iterator il;
			for (il = pList->begin(); il != pList->end(); il++)
			{
				if ((*il) == (*ii).second.nPatchType)
				{
					bCheck = true;
					break;
				}
			}

			if (bCheck == false)
				pList->push_back((*ii).second.nPatchType);
		}
	}
}

int CDataManager::GetAssignedCreateLastIndex(int nID, const TCHAR * pType)
{
	int nIdx = 0;
	std::map<unsigned long, TNetLauncher*>::iterator ii;
	for (ii = m_NetLauncherList.begin(); ii != m_NetLauncherList.end(); ii++)
	{
		TNetLauncher * pLauncher = (*ii).second;
		if (pLauncher->nID == nID)
		{
			std::vector <TServerExcuteData*>::iterator ih;
			for (ih = pLauncher->ExcuteList.begin(); ih != pLauncher->ExcuteList.end(); ih++)
			{
				TServerExcuteData * pCmd = (*ih);
				if (!_tcsicmp(pCmd->wszType, pType) && pCmd->nCreateIndex + 1 > nIdx)
					nIdx = pCmd->nCreateIndex + 1;
			}
		}
	}
	return nIdx;
}

void CDataManager::SetAssignedCreateCount(int nID, const TCHAR * pType, int nCreateCount)
{
	std::map<unsigned long, TNetLauncher*>::iterator ii;
	for (ii = m_NetLauncherList.begin(); ii != m_NetLauncherList.end(); ii++)
	{
		TNetLauncher * pLauncher = (*ii).second;
		if (pLauncher->nID == nID)
		{
			std::vector <TServerExcuteData*>::iterator ih;
			for (ih = pLauncher->ExcuteList.begin(); ih != pLauncher->ExcuteList.end(); ih++)
			{
				TServerExcuteData * pCmd = (*ih);
				if (!_tcsicmp(pCmd->wszType, pType) && pCmd->nCreateCount < nCreateCount)
					pCmd->nCreateCount = nCreateCount;
			}
		}
	}
}

void CDataManager::GetCommandList(std::vector <int> * pvList)
{
	std::map <int, TServerExcuteData>::iterator ii;
	for (ii = m_ServerExcuteData.begin(); ii != m_ServerExcuteData.end(); ii++)
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
				std::map <int, TServerExcuteData>::iterator ij;
				for (ij = m_ServerExcuteData.begin(); ij != m_ServerExcuteData.end(); ij++)
					if ((*ij).second.nAssignedLauncherID == (*ih).second->nID)
						pvList->push_back((*ij).first);
			}
		}
	}
}

void CDataManager::GetExeCommandList(int nNID, std::vector <TServerExcuteData> * pvList)
{
	std::map <unsigned long, TNetLauncher*>::iterator ih;
	for (ih = m_NetLauncherList.begin(); ih != m_NetLauncherList.end(); ih++)
	{
		if ((*ih).second->nID == nNID)
		{
			std::map <int, TServerExcuteData>::iterator ij;
			for (ij = m_ServerExcuteData.begin(); ij != m_ServerExcuteData.end(); ij++)
				if ((*ij).second.nAssignedLauncherID == (*ih).second->nID)
					pvList->push_back(ij->second);
		}
	}
}

const TServerExcuteData * CDataManager::GetCommand(int nSID)
{
	std::map <int, TServerExcuteData>::iterator ii = m_ServerExcuteData.find(nSID);
	return ii != m_ServerExcuteData.end() ? &(*ii).second : NULL;
}

bool CDataManager::CheckMonitorUser(const char * pName, const char * pPass, int &nLevel, bool &bCanDuplicate)
{
	std::vector <TMonitorUserData>::iterator ii;
	for (ii = m_MonitorUserData.begin(); ii != m_MonitorUserData.end(); ii++)
	{
		if (!strcmp((*ii).szUserName, pName))
		{
			if (!strcmp((*ii).szPassWord, pPass))
			{
				nLevel = (*ii).nMonitorLevel;
				bCanDuplicate = (*ii).bDuplicateLogin;
				return true;
			}
		}
	}
	return false;
}

void CDataManager::ClearData()
{
	std::map <int, TPartitionInfo*>::iterator PartItor;
	for (PartItor = m_PartitionList.begin(); PartItor != m_PartitionList.end(); PartItor++)
		SAFE_DELETE((*PartItor).second);
	m_PartitionList.clear();

	std::map <unsigned long, TNetLauncher*>::iterator in;
	for (in = m_NetLauncherList.begin(); in != m_NetLauncherList.end(); in++)
		SAFE_DELETE((*in).second);
	m_NetLauncherList.clear();
	m_ServerExcuteData.clear();
}

void CDataManager::ClearUserData()
{
	m_MonitorUserData.clear();
}

CDataManager * CDataManager::GetInstance()
{
	static CDataManager s;
	return &s;
}

#if defined (_SERVICEMANAGER_EX)
void CDataManager::GetPartitionList(OUT vector<const TPartitionInfo*>& list) const
{
	for each (const map<int, TPartitionInfo*>::value_type v in m_PartitionList)
	{
		list.push_back(v.second);
	}
}
void CDataManager::GetPartitionText(OUT wchar_t text[256]) const
{
	if (m_PartitionList.empty())
	{
		::wsprintf(text, L"[%s] PID : %d, IP : %s", L"Partition", 1, m_szServiceIP);
		return;
	}

	vector<const TPartitionInfo*> list;
	for each (const map<int, TPartitionInfo*>::value_type v in m_PartitionList)
		list.push_back(v.second);

	if (list.empty())
		return;

	const TPartitionInfo* pInfo = list[0];
	if (pInfo)
		::wsprintf(text, L"[%s] PID : %d IP : %s", pInfo->szPartitionName, pInfo->nPID, m_szServiceIP);
}

void CDataManager::GetLauncherList(OUT vector<const TNetLauncher*>& list) const
{
	for each (const map<unsigned long, TNetLauncher*>::value_type v in m_NetLauncherList)
	{
		list.push_back(v.second);
	}
}

void CDataManager::GetWorldList(OUT vector<const TServerInfo*>& list) const
{
	for each (int v in m_vWorldList)
	{
		const TServerInfo* pWorld = GetServerInfo(v);
		if (pWorld)
			list.push_back(pWorld);
	}
}
#endif // #if defined (_SERVICEMANAGER_EX)

bool CDataManager::IsFirstGameCon(int nSID)
{
	int nFirstGameCon = 0;
	std::map <int, TServerExcuteData>::iterator verifyer;
	for (verifyer = m_ServerExcuteData.begin(); verifyer != m_ServerExcuteData.end(); verifyer++)
	{
		if ((*verifyer).second.nServerType == MANAGED_TYPE_GAME)
		{
			nFirstGameCon = (*verifyer).second.nSID;
			break;
		}
	}
	
	return (nFirstGameCon == nSID);
}

void CDataManager::CheckChannelInfo() const
{
	USES_CONVERSION;
	std::wstring wstrErrorPopup( L"ChannelInfo Not Found!!!!\r\n\r\n" );
	std::wstring wstrErrorLog( L"ChannelInfo Not Found!!!! " );
	int nErrorCount = 0;

	typedef		std::map<int, TDefaultServerConstructData*>::const_iterator
				citerator;
	
	citerator iter = m_PreServerStructData.PreStructList.begin();
	while( iter != m_PreServerStructData.PreStructList.end() )
	{
		const TDefaultServerConstructData* pConstructData = (*iter).second;	
		++ iter;

		if( pConstructData->nServerType != MANAGED_TYPE_VILLAGE )
		{
			continue;
		}

		const TVillageServerConstructData* pVillage = static_cast<const TVillageServerConstructData*>(pConstructData);
		if( pVillage->nWorldIDList.empty() )
		{
			continue;
		}

		if( GetVillageInfo( pVillage->nWorldIDList[0], pVillage->nVillageID ) )
		{
			continue;
		}

		if( 0 < nErrorCount )
		{
			wstrErrorPopup += L"\r\n";
			wstrErrorLog += L",";
		}

		std::wstring wstrInfo;
		wstrInfo += L"[WorldID:";
		wstrInfo += I2W(pVillage->nWorldIDList[0]);
		wstrInfo += L"][VillageID:";
		wstrInfo += I2W(pVillage->nVillageID);
		wstrInfo += L"]";

		wstrErrorPopup += wstrInfo;
		wstrErrorLog += wstrInfo;

		++ nErrorCount;
	}

	if(0 == nErrorCount)
	{
		return;
	}

	wstrErrorPopup += L"\r\n";
	wstrErrorLog += L"\r\n";

#if defined (_SERVICEMANAGER_EX)
	MessageBox( NULL, wstrErrorPopup.c_str(), L"Warning", MB_OK | MB_ICONWARNING );
#else
#endif

	g_Log.Log(LogType::_ERROR, L"%s", wstrErrorLog.c_str());
}