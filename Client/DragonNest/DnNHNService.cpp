#include "StdAfx.h"
#include "DnNHNService.h"
#include "LoginSendPacket.h"
#include "LogWnd.h"
#include "DnInterface.h"
#include "DnString.h"

#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif 

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(_JP) && defined(_AUTH)

#include "../Extern/NHNPurple/HanAuthForClient/HanAuthForClient.h" 
#include "../Extern/NHNpurple/HanGameInfoForClient/HanGameInfoForClient.h"
#include "../Extern/NHNpurple/HanReportForClient/HanReportForClient.h"

// #define CLIENT_JP_TEST

DnNHNService::DnNHNService(void)
: m_PartnerType( Partner::Type::None )
{  
	m_szAuthString.clear();
	m_szMemberID.clear();

	m_Module = LoadLibraryW(L"HGPSHELP.DLL");
}

DnNHNService::~DnNHNService(void)
{
	FreeLibrary(m_Module);
}

LRESULT DnNHNService::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

int DnNHNService::OnEvent(int nType, void* pExtra)
{
	return TRUE; 
}


int DnNHNService::PreInitialize(void* pCustomData)
{
//TestCode
#ifdef CLIENT_JP_TEST

	char szGameStringOld[4096] = "hangame://j_dnestchannel://metp://119.235.231.236:8080//go/go;lang:8=JAPANESE&pubenc:1=y&pub10e:4=4355&plii:50=http://pubdownt.hangame.co.jp/downbase/purple/plii&wbers:1=y&pub1e:341=hangametest=UjU8ItIQsO7ph_ATC152%2CU4PXJ7dm60BXX8DS80afhUALHSE-P7fQn42fJUbksSYf676A%2CUz5xovmPX-Xp0868%2CM%2C1%2CY%2C111ANNN-4_P_V_U_L0X2_H2F4_F01_PE6_21DI_41HB%2CY%2C*%2CUa2IDF3F%2C%2C%2C%2C%2C%2Chgj%2C222440%2CUUUU875A%2CHGJ%2CUspnPU60RS9zhVCeRyZDJtLFje1UP7ZB4e74hVUeDefw6726%2C%2C8%2C0%2CUsznp7dFnM7W803E%2CUszsPl6yU_6Y08EA%2CUnU7FEEA%2C2&pub2e:12=hangametest=&pub3e:20=hangametest=alpha-id&pub4e:48=UspnPU60RS9jPVBksy9pItNmaelzh79VDeU-PV7k4eAgA7E9&pub5e:0=&pub6e:24=hangametest=UsznP16Y2992&k22e:7=J_DNEST&m1e:7=2160257&USN:12=100000003231&mbrAccId:12=100000003231&pubpwd:5=dummy&jidType:1=1&k111e:4=0000&k3e:9=pepsired2&k87e:3=hgc&k93e:7=Hangame&k126e:6=urlhgc&k89e:50=http://pubdownt.hangame.co.jp/downbase/purple/dist&k88e:51=				http://pubdownt.hangame.co.jp/downbase/purple/hul&nors:1=Y&k95e:25=http://www.hangame.co.jp/&fcvr:1=0&fciv:1=0&k125e:2=26&scvr:1=1&k60e:10=DragonNest&k91e:4=1.00&k90e:4=1.00&k150e:4=1.00&k37e:6=999999";
	char szGameStringNew[2048]={0,};
	char szAuthString[2048]={0,};
	char szMemberID[30]={0,};

	int iRet = 0 ; 
	//런쳐에서 해야할일 
	iRet = HanAuthInitGameString( szGameStringOld );
	iRet = HanAuthForClientGameString(szGameStringOld);

	iRet = HanAuthGetId(szGameStringOld , szMemberID , sizeof(szMemberID));
	m_szMemberID = szMemberID ;
	iRet = HanAuthRefresh( (char*)m_szMemberID.c_str() );
	UpdateGameString(szGameStringOld  , szGameStringNew  , sizeof(szGameStringNew));
	//////////////////////////////////////////////////////////////////////////

	//클라이언트 해야할일
//	iRet = HanAuthInitGameString(szGameStringNew);
	iRet = HanAuthForClientGameString(szGameStringNew);
	iRet = GetAuthString(szAuthString , sizeof(szAuthString));
	iRet = HanAuthRefresh((char*)m_szMemberID.c_str());

	m_szGameString = szGameStri`ngNew; 
	m_szAuthString = szAuthString ; 
	m_szUserID = "pepsired2";

	FileLog(szAuthString);

#else 
	
	std::string pCmdline = GetCommandLineA();
	CStringA sCmdLine = pCmdline.c_str(); 
	std::vector<std::string> tokens; 

	//게임스트링을 만 뽑기위해 
	TokenizeA( pCmdline ,  tokens , std::string(" ") );
	
	
	for(int i = 0;i< (int)tokens.size() ;i++)
	{
		if( NULL != StrStrA( tokens[i].c_str() , "metp://") ) //주소가 들어 있다
		{
			m_szAuthString  = tokens[i].c_str(); //복사  
		}
		
		if ( NULL != StrStrA( tokens[i].c_str() , "mid:"))
		{
			m_szMemberID  = tokens[i].c_str()+4;
		}
					
	}
	tokens.clear();
	
	//id 추출
	std::vector<std::string> tokenGameString; 
	TokenizeA( pCmdline ,  tokenGameString , std::string("&") );
	
	for(int i = 0 ; i < (int)tokenGameString.size() ; ++i )
	{
		if(StrStrA(tokenGameString[i].c_str() , "k3e:" ))
		{
			m_szUserID = tokenGameString[i].c_str()+6;
		}

		if(StrStrA(tokenGameString[i].c_str() , "site:" ))
		{
			m_szSiteCode = tokenGameString[i].c_str()+7;
		}
	}

	tokenGameString.clear();

	if( m_szSiteCode.empty() || m_szSiteCode == "HG" ) m_PartnerType = Partner::Type::None;
	else if( m_szSiteCode == "MS" ) m_PartnerType = Partner::Type::MS;

#endif 
	
#ifdef _HSHIELD
	WCHAR szMyID[256]={0,};
	MultiByteToWideChar( CP_ACP, 0, m_szMemberID.c_str() , -1, szMyID , 256 );
	HShieldSetup::SetMyID( szMyID );
	int nResult = 0;
	nResult = HShieldSetup::Init(GetEtDevice()->GetHWnd());
	if ( nResult < 0 )	
	{
		HShieldSetup::Stop();
		MessageBoxW(0, STR_HS_INITIALIZED_FAIL , L"DragonNest",0);

		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage(hWnd, WM_CLOSE, 0,0);
		return 0;
	}
#if defined(_DEBUG_HSHIELD)
	DebugLog("Skiped HackSheild update.");
#else
	nResult = HShieldSetup::Update();
	if ( nResult < 0 )
	{
		HShieldSetup::Stop();
		MessageBoxW(0, STR_HS_UPDATE_FAIL , L"DragonNest",0);
		
		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage(hWnd, WM_CLOSE, 0,0);
		return 0;
	}
#endif //_DEBUG_HSHIELD
	nResult = HShieldSetup::Start();
	if ( nResult < 0 )	
	{
		HShieldSetup::Stop();
		MessageBoxW(0, STR_HS_START_FAIL , L"DragonNest",0);
		
		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage(hWnd, WM_CLOSE, 0,0);
		return 0;
	}

#endif //_HSHIELD

	return TRUE;
}
void DnNHNService::FileLog(char *str , ...)
{
//	return; 

	va_list args;    
	va_start(args,str);

	static char buf[8192] = {0,} ;	
	_vsnprintf(buf, 8192, str, args);

	FILE *stream = NULL;
	if( (stream = _fsopen( "c:\\outfile.txt", "a", _SH_DENYWR )) != NULL )
	{
		fprintf( stream, "%s \n" , buf );
		fclose( stream );
	}
}

int DnNHNService::Initialize(void* pCustomData)
{
	HanAuthInit();

	HanReportInit();

	HanGameInfoInitAccGameString((char*)m_szAuthString.c_str() );		
	
	int ret  =0 ; 
	ret = HanGameInfoEnterLobby();
	FileLog("HanGameInfoEnterLobby %d" , ret );
	ret = HanGameInfoStartGame();
	FileLog("HanGameInfoStartGame %d" , ret );
	
	return TRUE; 
}

bool DnNHNService::HanAuthInit()
{
	int iRet = 0 ; 
	char szMemberID[30] = {0,};
	char szGameStringNew[2048]={0,};
	char szAuthString[2048]={0,};

#ifdef CLIENT_JP_TEST

	WCHAR szLocalIP[ 32 ]= {0,};
	CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );

	__int64 iMemberID = _atoi64(m_szMemberID.c_str());

	//	CStringA StrTmp; 
	//	StrTmp.Format( "UserID %s  memberID %s  szauthString %s stringlen %d" , m_szUserID.c_str()  , m_szMemberID.c_str() , m_szAuthString.c_str() , strlen(m_szAuthString.c_str()));
	//	MessageBoxA(NULL , StrTmp , "" ,MB_OK);

		SendCheckLoginJP( (char *)m_szUserID.c_str()  ,iMemberID , (char*)m_szAuthString.c_str() ,  szLocalIP );
	//ErrorCheck;
	HanAuthRefresh( (char*)m_szMemberID.c_str() );

	DebugLog("DnNHNService::HanAuthInit(). Send SendCheckLoginJP. " );

	//return 0;

#else 

	iRet = HanAuthInitGameString((char*)m_szAuthString.c_str());

	if( HAN_AUTHCLI_OK != iRet)
	{
		GetInterface().MessageBox( ErrorMsg(iRet), MB_OK, MESSAGEBOX_2, (CEtUICallback *)GetInterface().GetLoginDlg());
		return FALSE;
	}

	iRet = HanAuthForClientGameString((char*)m_szAuthString.c_str() );

	if( HAN_AUTHCLI_OK != iRet)	
	{
		GetInterface().MessageBox( ErrorMsg(iRet), MB_OK, MESSAGEBOX_2, (CEtUICallback *)GetInterface().GetLoginDlg());
		return FALSE;
	}

	iRet = GetAuthString(szAuthString , sizeof(szAuthString));

	if( HAN_AUTHCLI_OK != iRet)	
	{
		GetInterface().MessageBox( ErrorMsg(iRet), MB_OK, MESSAGEBOX_2, (CEtUICallback *)GetInterface().GetLoginDlg());
		return FALSE;
	}

	WCHAR szLocalIP[ 32 ]= {0,};
	CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );
	
	FileLog("GetAuthString");
	
	if( szAuthString[0] != '\0' )
	{
		//(char *pAccountName, UINT nMemberID, char *pAuthString, const WCHAR *pIp)

		__int64 iMemberID = _atoi64( m_szMemberID.c_str() );
		/*	
		CStringA StrTmp; 
		StrTmp.Format( "UserID %s  memberID %s  szauthString %s stringlen %d" , m_szUserID.c_str()  , m_szMemberID.c_str() , szAuthString ,strlen(szAuthString));
		MessageBoxA(NULL , StrTmp , "" ,MB_OK);
		*/

		SendCheckLoginJP( (char *)m_szUserID.c_str()  , iMemberID , szAuthString ,  szLocalIP, m_PartnerType );
		//ErrorCheck;
		HanAuthRefresh( (char*)m_szMemberID.c_str() );
		if( HAN_AUTHCLI_OK != iRet)	
		{
			GetInterface().MessageBox( ErrorMsg(iRet), MB_OK, MESSAGEBOX_2, (CEtUICallback *)GetInterface().GetLoginDlg());
			return FALSE;
		}

		DebugLog("DnNHNService::HanAuthInit(). Send SendCheckLoginJP. " );
		return TRUE;
	}
	else
	{
		ErrorLog("DnNexonService::Initialize() m_szPassPort.empty so we not send login." );
	}
#endif 

	return TRUE; 
}


bool  DnNHNService::HanReportInit()
{
	int Ret = 0 ; 
	char md5[128] = {0,};

	Ret = HanReportInitGameString( (char*)m_szAuthString.c_str() , false);

	Ret = HanReportConvertMD5((char *)m_szMemberID.c_str() , md5 );

	m_szIdMd5 = md5; 

	HanSendReport(eNHNEventCode::GameStart);

	return TRUE; 
}

void DnNHNService::HanSendReport(eNHNEventCode EventCode)
{
	char szString[2048]={0,};
	sprintf_s( szString , 2048 , "userid=%s&eventcode=%d" , m_szIdMd5.c_str() , (int)EventCode ); 
	HanReportSendPosLog( szString );
}

bool DnNHNService::WriteStageLog_( INT32 nStage , LPCTSTR szComment )
{
	// %참고 GameStart , Game_Exit 두개 는 넥슨쪽도 같이 사용하고 있다 .. 
	// 게임지수 보고  , 안정화 지표 보고 
	int ret = 0;
	switch(nStage) {
		case eServiceInfo::GameStart : 	
			FileLog("NHNInfoInitialize %d" , ret );
			break; 
		case eServiceInfo::JoinChannel: 
			break; 	
		case eServiceInfo::ExitChannel: 
			break;
		case eServiceInfo::Game_Exit: 
			{
				ret = HanGameInfoEndGame();
				FileLog("HanGameInfoEndGame %d" , ret  );
				ret = HanGameInfoExitLobby();
				FileLog("Game_Exit %d" , ret );
				HanSendReport(eNHNEventCode::GameExit);				
				FileLog("HanSendReport(eNHNEventCode::GameExit);" , ret );
			}
			break; 
	}

//	GetInterface().MessageBox( ErrorInfoMsg(iRet), MB_OK, MESSAGEBOX_2, (CEtUICallback *)GetInterface().GetLoginDlg());

	return TRUE;
}

int DnNHNService::Release()
{
	int a =0 ; 
	return 0; 
}

int DnNHNService::ErrorInfoMsg(int ret)
{
	switch(ret)
	{	
		case HAN_GAMEINFO_OK :				return 100401;	
		case HAN_GAMEINFO_ARGUMENT_INVALID:	return 100402; 
		case HAN_GAMEINFO_INITED_ALREADY:	return 100401;	
		case HAN_GAMEINFO_INITED_NOT:		return	100403	;
		case HAN_GAMEINFO_INITED_FAIL:		return	100404	;
		case HAN_GAMEINFO_HTTP_INITFAIL:		return	100407	;
		case HAN_GAMEINFO_HTTP_CONNECTFAIL:		return	100408	;
		case HAN_GAMEINFO_HTTP_OPENFAIL:		return	100410	;
		case HAN_GAMEINFO_HTTP_SENDFAIL:		return	100411	;
		case HAN_GAMEINFO_GAMESTRING_SERVICECODEINVALID:	return	100416	;
		case HAN_GAMEINFO_GAMESTRING_GAMEIDINVALID:			return	100413	;
		case HAN_GAMEINFO_GAMESTRING_MEMBERIDINVALID:		return	100413	;
		case HAN_GAMEINFO_DLL_UNEXPECTED:			return	100417 ; 

	}
	return HAN_AUTHCLI_OK;
}

int DnNHNService::ErrorMsg(int ret)
{
	switch(ret)
	{
		case 	HAN_AUTHCLI_OK:						return	100401	;
		case 	HAN_AUTHCLI_ARGUMENT_INVALID:		return	100402	;
		case 	HAN_AUTHCLI_INITED_NOT:				return	100403	;
		case 	HAN_AUTHCLI_INITED_FAIL:			return	100404	;
		case 	HAN_AUTHCLI_AUTHHTTP_INITFAIL:		return	100405	;
		case 	HAN_AUTHCLI_AUTHHTTP_CONNFAIL:		return	100406	;
		case 	HAN_AUTHCLI_REFRESHHTTP_INITFAIL:	return	100407	;
		case 	HAN_AUTHCLI_REFRESHHTTP_CONNFAIL:	return	100408	;
		case 	HAN_AUTHCLI_NOT_IMPLEMENTED:		return	100409	;
		case 	HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL:	return	100410	;
		case 	HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL:	return	100411	;
		case 	HAN_AUTHCLI_COOKIE_SETFAIL:			return	100412	;
		case 	HAN_AUTHCLI_GAMESTRING_IDINVALID:	return	100413	;
		case 	HAN_AUTHCLI_GAMESTRING_USNINVALID:	return	100414	;
		case 	HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID:		return	100415	;
		case 	HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID:	return	100416	;
		case 	HAN_AUTHCLI_DLL_UNEXPECTED:			return	100417	;
		case 	HAN_AUTHCLI_PARAM_INVALID:			return	100418	;
		case 	HAN_AUTHCLI_IPADDR_INVALID:			return	100419	;
		case 	HAN_AUTHCLI_MEMBERID_INVALID:		return	100420	;
		case 	HAN_AUTHCLI_PASSWORD_INCORRECT:		return	100421	;
		case 	HAN_AUTHCLI_PASSWORD_MISMATCHOVER:	return	100422	;
		case 	HAN_AUTHCLI_MEMBERID_NOTEXIST:		return	100423	;
		case 	HAN_AUTHCLI_SYSTEM_ERROR:			return	100424	;
		case 	HAN_AUTHCLI_COOKIE_SETERROR :		return	100425	;
	}
	return HAN_AUTHCLI_OK;
}

void DnNHNService::OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	switch( iMainCmd )
	{
	case SC_LOGIN:
		{
			switch( iSubCmd )
			{
			case eLogin::SC_CHECKVERSION: 
				{
				}
				break;
			case eLogin::SC_SERVERLIST:
				{
					// 게임시작
					HanSendReport(eNHNEventCode::Server);
				}
				break;
			case eLogin::SC_CHARLIST:
				{
					// 캐릭터 선택
					HanSendReport(eNHNEventCode::character);
				}
				break;
			case eLogin::SC_CHANNELLIST:
				{
					HanSendReport(eNHNEventCode::channel);
				}
				break;
			}
		}
	case SC_SYSTEM:
		{
			switch( iSubCmd )
			{
			case eSystem::SC_CONNECTVILLAGE:
				HanSendReport(eNHNEventCode::InVillage);
				break;
			case eSystem::SC_TCP_CONNECT_REQ:
				HanSendReport( eNHNEventCode::InGame );					
				break;			
			}
			break;
		}
	}
}

#endif // _JP && _AUTH
