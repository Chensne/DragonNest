//
//	Client.cpp
//
//		-	이 파일은 게임 클라이언트에서 넥슨 메신저를 사용하는 예제입니다.
//		-	코드 중에서 주석으로 설명한 부분이 메신저 모듈 연동에 관련된 부분입니다.
//		-	유니코드를 사용하도록 빌드 옵션이 설정되어 있습니다.
//		-	테스트 편의상 몇 가지 세팅은 Client.ini 파일에서 읽어들이고 있습니다.
//

#include "stdafx.h"
#include "Client.h"
#include "../../Common/ClientUtil.h"
#define	MAX_LOADSTRING 100

#include <atlstr.h>

//
//	필요한 메신저 헤더 파일 인클루드
//
#include "NMClass/NMManager.h"
#include "NMClass/NMEventHandler.h"

//
//	메신저 관련 매크로 선언
//
#define	_nmman	CNMManager::GetInstance()
#define	_nmco	CNMCOClientObject::GetInstance()

//
//	메신저 이벤트를 처리하기 위한 윈도 메시지 정의
//
#define	WM_NMEVENT	( WM_USER + 17 )

//
//	메신저 이벤트 핸들러 선언
//
//	(참고)
//		- 여기서는 단순하게 보여주기 위해 virtual 함수를 인라인으로 구현했습니다.
//		  실제로 이렇게 구현해야 하는 것은 아닙니다.
//
class CMessengerEventHandler : public CNMEventHandler
{
public:
	//
	//	인증서버와의 접속이 끊어졌을 경우에 발생
	//
	virtual void OnAuthConnectionClosedEvent( CNMAuthConnectionClosedEvent * pEvent )
	{
		ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->uType, ClientUtil::Convert::AuthResultToString( static_cast<NMLoginAuthReplyCode>( pEvent->uType ) ) );
	}
	//	메신저서버와의 접속이 연결되었을 경우에 발생
	//
	virtual void OnMessengerReplyEvent( CNMMessengerReplyEvent * pEvent )
	{
		ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->nResultCode, ClientUtil::Convert::MessengerResultToString( static_cast<NMMessengerReplyCode>( pEvent->nResultCode ) ) );

		if ( pEvent->nResultCode == kMessengerReplyOK || pEvent->nResultCode == kMessengerReplyNewUser )
		{
			_nmco.GetMyInfo();

			//
			//	리얼유저 정보 표시
			//
			ClientUtil::Log::LogInfo( _T("    Name: %s"), _nmco.GetMyName() );
			ClientUtil::Log::LogInfo( _T("    Nickname: %s"), _nmco.GetMyNickName() );
			SYSTEMTIME st = _nmco.GetMyBirthday();
			ClientUtil::Log::LogInfo( _T("    Birthday: %04d/%02d/%02d"), st.wYear, st.wMonth, st.wDay );
		}
	}
	//
	//	메신저서버와의 접속이 끊어졌을 경우에 발생
	//
	virtual void OnMsgConnectionClosedEvent( CNMMsgConnectionClosedEvent * pEvent )
	{
		ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->uType, ClientUtil::Convert::MessengerConnectionClosedEventToString( pEvent->uType ) );
	}
};

//
//	메신저 이벤트 핸들러 인스턴스
//
CMessengerEventHandler g_msngrEventHandler;

HINSTANCE			hInst;
TCHAR				szTitle[MAX_LOADSTRING];
TCHAR				szWindowClass[MAX_LOADSTRING];
HWND				hWndLog = NULL;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

HMODULE WINAPI LoadIt()
{
	return ::LoadLibraryA( "unicows.dll" );
}

extern "C" HMODULE (__stdcall *_PfnLoadUnicows)(void) = &LoadIt;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	LoadIt();
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CLIENT);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CLIENT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_CLIENT;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	hWndLog = CreateWindow(_T("LISTBOX"), _T(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, hInstance, NULL);

	if (!hWnd || !hWndLog)
	{
		return FALSE;
	}

	ClientUtil::Log::InitLog( hWndLog );

	BOOL bResult = TRUE;

	//
	//	[필수] 메신저 매니저 초기화
	//
	_nmman.Init();

	//
	//	NOTE:
	//
	//		-	편의상 로케일과 게임코드 설정을 Client.ini 파일로부터 읽도록 해 두었습니다
	//		-	하드코딩 하셔도 상관 없습니다
	//

	NMLOCALEID	uLocaleId = ClientUtil::Settings::GetLocaleId();
	NMGameCode	uGameCode = ClientUtil::Settings::GetGameCode();

	ClientUtil::Log::LogTime( _T("Client begins...") );
	ClientUtil::Log::LogInfo( _T("    Locale ID: %s"), ClientUtil::Convert::LocaleIdToString( uLocaleId ) );
	ClientUtil::Log::LogInfo( _T("    Game Code: %s"), ClientUtil::Convert::GameCodeToString( uGameCode ) );

	//
	//	[필수] 로케일 설정
	//
	if ( bResult )
	{
		bResult = _nmco.SetLocale( uLocaleId );
		ClientUtil::Log::LogTime( _T("_nmco.SetLocale( %s ): %s!"), ClientUtil::Convert::LocaleIdToString( uLocaleId ), ClientUtil::Convert::ResultToString( bResult ) );
	}

	//
	//	[필수] 메신저 모듈 초기화
	//
	if ( bResult )
	{
		bResult = _nmco.Initialize( uGameCode );
		ClientUtil::Log::LogTime( _T("_nmco.Initialize( %s ): %s!"), ClientUtil::Convert::GameCodeToString( uGameCode ), ClientUtil::Convert::ResultToString( bResult ) );
	}

	//
	//	[선택] 클라이언트 패치
	//
	//		-	패치가 필요한지 여부는 메신저와 상관없이 게임에서 결정합니다.
	//		-	이 예제에서는 아래 bNeedPatch 값에 따라 패치 여부를 결정합니다.
	//
	BOOL bNeedPatch = FALSE;

	if ( bNeedPatch )
	{
		//
		//	NGM 설치 여부 확인
		//
		//		-	NGM 을 이용해서 게임 클라이언트를 패치하는 게임의 경우에
		//			NGM 이 설치되어 있지 않으면 패치가 불가능합니다.
		//
		if ( bResult )
		{
			bResult = NMCOHelpers::IsNGMInstalled();
			ClientUtil::Log::LogTime( _T("NMCOHelpers::IsNGMInstalled(): %s!"), ClientUtil::Convert::ResultToString( bResult ) );
		}

		//
		//	패치 실행
		//
		//		-	szPatchURL 및 szPatchDir 은 게임팀에서 수정하여 테스트 하시기 바랍니다.
		//
		if ( bResult )
		{
			TCHAR*	szPatchURL	= _T( "patch url" );
			TCHAR*	szPatchDir	= _T( "patch dir" );
			TCHAR	szCmdLine[ 2048 ];

			::_tcscpy( szCmdLine , ::GetCommandLine() );

			bResult = NMCOHelpers::ExecuteNGMPatcher( uGameCode, szPatchURL, szPatchDir, szCmdLine );
			ClientUtil::Log::LogTime( _T("NMCOHelpers::ExecuteNGMPatcher(): %s!" ), ClientUtil::Convert::ResultToString( bResult ) );
			ClientUtil::Log::LogInfo( _T("    GameCode: %s" ), ClientUtil::Convert::GameCodeToString( uGameCode ) );
			ClientUtil::Log::LogInfo( _T("    PatchURL: %s" ), szPatchURL );
			ClientUtil::Log::LogInfo( _T("    PatchDir: %s" ), szPatchDir );
			ClientUtil::Log::LogInfo( _T("    CmdLine:  %s" ), szCmdLine );

			//
			//	게임 클라이언트 종료
			//
			//		-	실제 게임 클라이언트는 ExecuteNGMPatcher() 호출 후 종료해야 합니다.
			//			그래야 NGM이 게임 클라이언트 파일을 패치할 수 있기 때문입니다.
			//
			//		-	단, 이 예제에서는 프로그램을 종료하지 않고 계속 진행하겠습니다.
			//
		}
	}

	//
	//	[필수] 메신저 이벤트 핸들러 등록
	//
	//		-	메신저에서 발생하는 각종 이벤트를 받아 볼 수 있도록 이벤트 핸들러를 등록합니다
	//
	if ( bResult )
	{
		bResult = _nmco.RegisterCallbackMessage( hWnd, WM_NMEVENT );
		ClientUtil::Log::LogTime( _T("_nmco.RegisterCallbackMessage( 0x%08x, WM_NMEVENT ): %s!"), hWnd, ClientUtil::Convert::ResultToString( bResult ) );
	}

	//
	//	[필수] 인증서버 로그인
	//
	//		-	이 함수는 동기적으로 실행되므로 함수가 리턴할 때까지 프로세스는 블록됩니다.
	//
	if ( bResult )
	{
		switch ( uLocaleId )
		{
		case kLocaleID_KR:
			{
				//
				//	-	커맨드라인으로부터 패스포트 스트링을 추출해 냅니다.
				//	-	아래 코드는 "GameClient.exe PassportString" 형식으로, 
				//	-	클라이언트 경로 뒷 부분에 한칸 띄우고 바로 패스포트 스트링이 넘어오는 경우를 가정하고 있습니다.
				//	-	일반적으로는 위 형식을 가장 많이 쓰지만 커맨드라인을 통한 패스포트 전달 형식은 게임팀에서 지정하는 것이 가능하므로,
				//	-	특정 형식이 필요한 경우 플랫폼개발팀으로 요청하도록 합니다.
				//
				TCHAR*	pCmdLine = ::GetCommandLine();
				TCHAR*	pChar = ::_tcsrchr( pCmdLine, _T( '"' ) );
				TCHAR	szCmdPassport[ NXPASSPORT_SIZE ] = { _T( '\0' ) };

				if ( pChar == NULL )
				{
					pChar = pCmdLine;
				}

				if ( _tcslen( pChar ) > 0 && ::_tcschr( pChar , _T( ' ' ) ) != NULL )
				{
					::_tcscpy( szCmdPassport , ::_tcschr( pChar , _T( ' ' ) ) + 1 );
				}

				if ( ::_tcslen( szCmdPassport ) > 0 )
				{
					//
					//	-	커맨드라인에서 패스포트 스트링 추출이 성공한 경우, 웹런칭 형식으로 동작합니다.
					//
					NMLoginAuthReplyCode resultAuth = _nmco.AttachAuth( szCmdPassport );

					if ( resultAuth == kLoginAuth_OK )
					{
						ClientUtil::Log::LogTime( _T("_nmco.AttachAuth( \"%s\" ): OK!"), szCmdPassport );
						bResult = TRUE;
					}
					else
					{
						ClientUtil::Log::LogTime( _T("_nmco.AttachAuth( \"%s\" ): Failed!!!"), szCmdPassport );
						ClientUtil::Log::LogInfo( _T("    Result: 0x%08x, \"%s\""), resultAuth, ClientUtil::Convert::AuthResultToString( resultAuth ) );
						bResult = FALSE;
					}
				}
				else
				{
					//
					//	-	커맨드라인에서 패스포트 스트링 추출이 실패한 경우, 스탠드얼론 형식으로 동작합니다.
					//	-	편의상 로그인 계정 정보를 Client.ini 파일로부터 읽도록 해 두었습니다
					//	-	하드코딩 하셔도 상관 없습니다
					//
					CAtlString strNexonID	= ClientUtil::Settings::GetNexonId();
					CAtlString strPassword	= ClientUtil::Settings::GetPassword();
						
					NMLoginAuthReplyCode resultAuth = _nmco.LoginAuth( strNexonID, strPassword );
					if ( resultAuth == kLoginAuth_OK )
					{
						ClientUtil::Log::LogTime( _T("_nmco.LoginAuth( \"%s\", \"%s\" ): OK!"), strNexonID, strPassword );
						bResult = TRUE;
					}
					else
					{
						ClientUtil::Log::LogTime( _T("_nmco.LoginAuth( \"%s\", \"%s\" ): Failed!!!"), strNexonID, strPassword );
						ClientUtil::Log::LogInfo( _T("    Result: 0x%08x, \"%s\""), resultAuth, ClientUtil::Convert::AuthResultToString( resultAuth ) );
						bResult = FALSE;
					}
				}

				if ( bResult )
				{
					//
					//	리얼유저 정보 획득
					//
					_nmco.GetMyInfo();

					//
					//	리얼유저 정보 표시
					//
					ClientUtil::Log::LogInfo( _T("    NexonSN: 0x%08x"), _nmco.GetMyOid() );
					ClientUtil::Log::LogInfo( _T("    NexonID: %s"), _nmco.GetMyLoginId() );
					ClientUtil::Log::LogInfo( _T("    Sex: %s"), ClientUtil::Convert::SexTypeToString( _nmco.GetMySex() ) );
					ClientUtil::Log::LogInfo( _T("    Age: %d"), _nmco.GetMyAge() );

					//
					//	넥슨패스포트 얻기
					//
					TCHAR szNexonPassport[ NXPASSPORT_SIZE ];
					ClientUtil::Log::LogInfo( _T("    Nexon Passport: %s"), _nmco.GetNexonPassport( szNexonPassport ) );
					
					//
					//	[필수] 게임 서버를 통한 2차 인증
					//
					//		-	게임 서버와의 인증은 샘플로 제작되어 있지 않지만,
					//		-	게임 클라이언트는 여기서 받아온 넥슨 패스포트를 게임 서버로 전송하여 게임 서버와 넥슨 인증 서버 간의 2차 인증을 거친다.
					//
				}
			}
			break;
		default:
			{
				ClientUtil::Log::LogTime( _T("_nmco.LoginAuth(): Failed!!!") );
				ClientUtil::Log::LogInfo( _T("    Not supported game code: %d"), uGameCode );
				bResult = FALSE;
			}
			break;
		}
	}

	//
	//	[선택] 메신저 서버 로그인
	//
	//		-	이 함수는 비동기적으로 실행되므로
	//			OnMessengerReplyEvent 이벤트를 받아야 실제 메신저 접속 여부를 알 수 있습니다
	//
	if ( bResult )
	{
		bResult = _nmco.LoginMessenger();
		ClientUtil::Log::LogTime( _T("_nmco.LoginMessenger(), %s!"), ClientUtil::Convert::ResultToString( bResult ) );
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		switch (wmId)
		{
		case ID_FILE_LOGOUTAUTH:
			//
			//	인증서버에서 로그아웃 합니다
			//
			{
				BOOL bResult = _nmco.LogoutAuth();
				ClientUtil::Log::LogTime( _T("_nmco.LogoutAuth(): %s!"), ClientUtil::Convert::ResultToString( bResult ) );
			}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		{
			UINT16 uWidth = LOWORD( lParam );
			UINT16 uHeight = HIWORD( lParam );

			::MoveWindow( ClientUtil::Log::GetLogWindowHandle(), 0, 0, uWidth, uHeight, TRUE );
		}
		break;
	case WM_DESTROY:

		_nmco.LogoutMessenger();
		//
		//	인증서버에서 로그아웃
		//
		_nmco.LogoutAuth();
		//
		//	메신저 이벤트 핸들러 등록 제거
		//
		_nmco.ResetCallbackMessage( hWnd );
		//
		//	메신저 매니저 종료
		//
		_nmman.Finalize();

		PostQuitMessage(0);
		break;
	case WM_NMEVENT:
		//
		//	메신저 이벤트 처리
		//
		g_msngrEventHandler.HandleNMEvent( wParam, lParam );
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
