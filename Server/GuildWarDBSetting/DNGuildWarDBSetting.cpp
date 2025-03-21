// DNGuildWarDBSetting.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
				std::cout << "입력 인자가 맞지 않습니다.\r\n";
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
		case 2 : std::cout << "셋팅 완료!! 마스터 서버를 다시 띄워주세요.\r\n"; break;
		case 3 : std::cout << "셋팅 완료!! 캐릭터 선택창으로 갔다가 다시 들어와 주세요.\r\n"; break;
		case 4 : std::cout << "셋팅 완료!! 마스터 서버를 다시 띄워주세요.\r\n"; break;
		case 5 : std::cout << "셋팅 완료!! 캐릭터 선택창으로 갔다가 다시 들어와 주세요.\r\n"; break;
		case 6 : std::cout << "셋팅 완료!! 길드원들이 해당서버에 한명도 존재하지 않게 만드시고 2분정도 뒤에 들어오세요.\r\n"; break;
		case 7 : std::cout << "셋팅 완료!! 캐릭터 선택창으로 갔다가 다시 들어와 주세요.\r\n"; break;
		}
	}
	else 
	{
		wprintf(L"에러입니다. 서버팀에 연락주세요. Code : %d\r\n", nRetCode );
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
		std::cout << "길드전 DB 셋팅\r\n\r\n";
		std::cout << "///////////////////////////////////////////\r\n";
		std::cout << "/ 0. 셋팅 종료\r\n";
		std::cout << "/ 1. 스케쥴셋팅\r\n";
		std::cout << "/ 2. 길드전 예선 초기화(미션점수, 통계)\r\n";
		std::cout << "/ 3. 인기투표초기화\r\n";
		std::cout << "/ 4. 예선 통계 내기\r\n";
		std::cout << "/ 5. 예선 개인보상 초기화\r\n";
		std::cout << "/ 6. 예선 길드보상 초기화\r\n";
		std::cout << "/ 7. 본선 길드 보상지급\r\n";
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
					std::cout << "신청기간(년월일시분), 예선기간, 보상(본선)기간, 보상기간끝\r\n";
					std::cout << "보상(본선)기간은 최소 5시간 이상을 넣어주세요.\r\n";
					std::cout << "ex) 201104221830, 201104230000, 201104240000, 201104250000 : \r\n";
				}
				break;
			case 2:	std::cout << "주의!!! 길드전 예선 점수 및 통계 합니다.엔터치면 GoGo!"; break;
			case 3:	std::cout << "인기투표를 초기화할 캐릭터명 입력 : "; break;
			case 4:	std::cout << "예선 통계를 냅니다. 엔터치면 GoGo!"; break;
			case 5:	std::cout << "예선 개인 보상 초기화할 캐릭터명 입력 :"; break;
			case 6:	std::cout << "예선 길드 보상 초기화할 길드명 입력 :"; break;
			case 7:	std::cout << "본선전에 해당하는 보상을 지급합니다. 자세한 내용은 기획서 참조. 엔터치면 GoGo"; break;
			}
			
			//_getws(szCmd);
			gets(szCmd);
			ProcessQuery(num, szCmd);
		}
		else
		{
			std::cout << "\r\n\t장난치지 말고 번호 다시 입력해주세요.\r\n";
		}
	}
	return 0;
}

