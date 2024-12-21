// DNGuildWarDBSetting.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "DNSQLWorldGuildWar.h"
#include "DNSQLMembershipGuildWar.h"
#include "Inifile.h"
//#include ""

CDNSQLWorldGuildWar g_WorldDB;
CDNSQLMembershipGuildWar g_MemberShipDB;

struct TConfig
{	
	TDBName MembershipDB;
	TDBName WorldDB;
	int nScheduleID;
};

TConfig g_Config;

void ProcessQuery(int num, const char* pszCmd)
{
	int nRetCode = 0;
	switch(num)
	{
	case 1 :
		{
			std::string					strString(pszCmd);
			std::vector<std::string>	vSplit;
			boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );

			if( vSplit.empty() || vSplit.size() != 4)
			{				
				std::cout << "�Է� ���ڰ� ���� �ʽ��ϴ�.\r\n";
				return;
			}
			nRetCode = g_WorldDB.QueryAddGuildWarSchedule(vSplit, g_Config.nScheduleID);			
		}
		break;
	case 2 : nRetCode=g_WorldDB.QueryInitGuildWarTrial(); break;
	case 3 : nRetCode=g_WorldDB.QueryInitGuildWarVote(pszCmd); break;
	case 4 : nRetCode=g_WorldDB.QuerySetGuildWarStats(); break;
	case 5 : nRetCode=g_WorldDB.QueryInitGuildWarRewardCharacter(pszCmd); break;
	case 6 : nRetCode=g_WorldDB.QueryInitGuildWarRewardGuild(pszCmd); break;
	case 7 :
		{			
			nRetCode = g_WorldDB.QuerySetGuildWarRewardFinal();
			if( nRetCode == ERROR_NONE )
			{
				nRetCode = g_MemberShipDB.QuerySetGuildWarRewardFinal();
			}
		}
		break;
	}

	if( nRetCode == ERROR_NONE )
	{
		switch(num)
		{
		case 1 : 
		case 2 : std::cout << "���� �Ϸ�!! ������ ������ �ٽ� ����ּ���.\r\n"; break;
		case 3 : std::cout << "���� �Ϸ�!! ĳ���� ����â���� ���ٰ� �ٽ� ���� �ּ���.\r\n"; break;
		case 4 : std::cout << "���� �Ϸ�!! ������ ������ �ٽ� ����ּ���.\r\n"; break;
		case 5 : std::cout << "���� �Ϸ�!! ĳ���� ����â���� ���ٰ� �ٽ� ���� �ּ���.\r\n"; break;
		case 6 : std::cout << "���� �Ϸ�!! �������� �ش缭���� �Ѹ� �������� �ʰ� ����ð� 2������ �ڿ� ��������.\r\n"; break;
		case 7 : std::cout << "���� �Ϸ�!! ĳ���� ����â���� ���ٰ� �ٽ� ���� �ּ���.\r\n"; break;
		}
	}
	else 
	{
		wprintf(L"�����Դϴ�. �������� �����ּ���. Code : %d\r\n", nRetCode );
	}
}

bool LoadConfig()
{
	wstring wszFileName = L"./Config.ini";
	if (!g_IniFile.Open(wszFileName.c_str())){
		std::cout << "Config File not Found!!\r\n";
		return false;
	}
	WCHAR wszStr[128] = {0,};
	WCHAR wszBuf[128] = {0,};
	char szData[128] = { 0, }, *pStr = NULL;	

	g_IniFile.GetValue(L"DB_DNMembership", L"DBIP", wszBuf);
	WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.MembershipDB.szIP, sizeof(g_Config.MembershipDB.szIP), NULL, NULL);
	g_IniFile.GetValue(L"DB_DNMembership", L"DBPort", &g_Config.MembershipDB.nPort);
	g_IniFile.GetValue(L"DB_DNMembership", L"DBID", g_Config.MembershipDB.wszDBID);
	g_IniFile.GetValue(L"DB_DNMembership", L"DBName", g_Config.MembershipDB.wszDBName);

	g_IniFile.GetValue(L"DB_DNWorld", L"DBIP", wszBuf);
	WideCharToMultiByte(CP_ACP, NULL, wszBuf, -1, g_Config.WorldDB.szIP, sizeof(g_Config.WorldDB.szIP), NULL, NULL);
	g_IniFile.GetValue(L"DB_DNWorld", L"DBPort", &g_Config.WorldDB.nPort);
	g_IniFile.GetValue(L"DB_DNWorld", L"DBID", g_Config.WorldDB.wszDBID);
	g_IniFile.GetValue(L"DB_DNWorld", L"DBName", g_Config.WorldDB.wszDBName);	

	g_IniFile.GetValue(L"Schedule", L"ID", &g_Config.nScheduleID);
	return true;
}


int _tmain(int argc, _TCHAR* argv[])
{		
	if( !LoadConfig() )
	{
		return 0;
	}
	setlocale(LC_ALL, "Korean");
	g_WorldDB.Connect(g_Config.WorldDB.szIP, g_Config.WorldDB.nPort, g_Config.WorldDB.wszDBName, g_Config.WorldDB.wszDBID);
	g_MemberShipDB.Connect(g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);

	char szCmd[256] = {0};	
	int num = 0;
	while (1)
	{
		std::cout << "\r\n\r\n";
		std::cout << "����� DB ����\r\n\r\n";
		std::cout << "///////////////////////////////////////////\r\n";
		std::cout << "/ 0. ���� ����\r\n";
		std::cout << "/ 1. ���������\r\n";
		std::cout << "/ 2. ����� ���� �ʱ�ȭ(�̼�����, ���)\r\n";
		std::cout << "/ 3. �α���ǥ�ʱ�ȭ\r\n";
		std::cout << "/ 4. ���� ��� ����\r\n";
		std::cout << "/ 5. ���� ���κ��� �ʱ�ȭ\r\n";
		std::cout << "/ 6. ���� ��庸�� �ʱ�ȭ\r\n";
		std::cout << "/ 7. ���� ��� ��������\r\n";
		std::cout << "///////////////////////////////////////////\r\n\r\n";

		printf("CMD>");
		scanf("%d", &num);		
		fflush(stdin);
		if( num == 0)		
			break;
		else if( num > 0 && num < 8)
		{
			switch(num)
			{
			case 1:
				{
					std::cout << "��û�Ⱓ(����Ͻú�), �����Ⱓ, ����(����)�Ⱓ, ����Ⱓ��\r\n";
					std::cout << "����(����)�Ⱓ�� �ּ� 5�ð� �̻��� �־��ּ���.\r\n";
					std::cout << "ex) 201104221830, 201104230000, 201104240000, 201104250000 : \r\n";
				}
				break;
			case 2:	std::cout << "����!!! ����� ���� ���� �� ��� �մϴ�.����ġ�� GoGo!"; break;
			case 3:	std::cout << "�α���ǥ�� �ʱ�ȭ�� ĳ���͸� �Է� : "; break;
			case 4:	std::cout << "���� ��踦 ���ϴ�. ����ġ�� GoGo!"; break;
			case 5:	std::cout << "���� ���� ���� �ʱ�ȭ�� ĳ���͸� �Է� :"; break;
			case 6:	std::cout << "���� ��� ���� �ʱ�ȭ�� ���� �Է� :"; break;
			case 7:	std::cout << "�������� �ش��ϴ� ������ �����մϴ�. �ڼ��� ������ ��ȹ�� ����. ����ġ�� GoGo"; break;
			}
			
			//_getws(szCmd);
			gets(szCmd);
			ProcessQuery(num, szCmd);
		}
		else
		{
			std::cout << "\r\n\t�峭ġ�� ���� ��ȣ �ٽ� �Է����ּ���.\r\n";
		}
	}
	return 0;
}

