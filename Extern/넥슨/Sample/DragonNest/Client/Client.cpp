//
//	Client.cpp
//
//		-	�� ������ ���� Ŭ���̾�Ʈ���� �ؽ� �޽����� ����ϴ� �����Դϴ�.
//		-	�ڵ� �߿��� �ּ����� ������ �κ��� �޽��� ��� ������ ���õ� �κ��Դϴ�.
//		-	�����ڵ带 ����ϵ��� ���� �ɼ��� �����Ǿ� �ֽ��ϴ�.
//		-	�׽�Ʈ ���ǻ� �� ���� ������ Client.ini ���Ͽ��� �о���̰� �ֽ��ϴ�.
//

#include "stdafx.h"
#include "Client.h"
#include "../../Common/ClientUtil.h"
#define	MAX_LOADSTRING 100

#include <atlstr.h>

//
//	�ʿ��� �޽��� ��� ���� ��Ŭ���
//
#include "NMClass/NMManager.h"
#include "NMClass/NMEventHandler.h"

//
//	�޽��� ���� ��ũ�� ����
//
#define	_nmman	CNMManager::GetInstance()
#define	_nmco	CNMCOClientObject::GetInstance()

//
//	�޽��� �̺�Ʈ�� ó���ϱ� ���� ���� �޽��� ����
//
#define	WM_NMEVENT	( WM_USER + 17 )

//
//	�޽��� �̺�Ʈ �ڵ鷯 ����
//
//	(����)
//		- ���⼭�� �ܼ��ϰ� �����ֱ� ���� virtual �Լ��� �ζ������� �����߽��ϴ�.
//		  ������ �̷��� �����ؾ� �ϴ� ���� �ƴմϴ�.
//
class CMessengerEventHandler : public CNMEventHandler
{
public:
	//
	//	������������ ������ �������� ��쿡 �߻�
	//
	virtual void OnAuthConnectionClosedEvent( CNMAuthConnectionClosedEvent * pEvent )
	{
		ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->uType, ClientUtil::Convert::AuthResultToString( static_cast<NMLoginAuthReplyCode>( pEvent->uType ) ) );
	}
	//	�޽����������� ������ ����Ǿ��� ��쿡 �߻�
	//
	virtual void OnMessengerReplyEvent( CNMMessengerReplyEvent * pEvent )
	{
		ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->nResultCode, ClientUtil::Convert::MessengerResultToString( static_cast<NMMessengerReplyCode>( pEvent->nResultCode ) ) );

		if ( pEvent->nResultCode == kMessengerReplyOK || pEvent->nResultCode == kMessengerReplyNewUser )
		{
			_nmco.GetMyInfo();

			//
			//	�������� ���� ǥ��
			//
			ClientUtil::Log::LogInfo( _T("    Name: %s"), _nmco.GetMyName() );
			ClientUtil::Log::LogInfo( _T("    Nickname: %s"), _nmco.GetMyNickName() );
			SYSTEMTIME st = _nmco.GetMyBirthday();
			ClientUtil::Log::LogInfo( _T("    Birthday: %04d/%02d/%02d"), st.wYear, st.wMonth, st.wDay );
		}
	}
	//
	//	�޽����������� ������ �������� ��쿡 �߻�
	//
	virtual void OnMsgConnectionClosedEvent( CNMMsgConnectionClosedEvent * pEvent )
	{
		ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->uType, ClientUtil::Convert::MessengerConnectionClosedEventToString( pEvent->uType ) );
	}
};

//
//	�޽��� �̺�Ʈ �ڵ鷯 �ν��Ͻ�
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
	//	[�ʼ�] �޽��� �Ŵ��� �ʱ�ȭ
	//
	_nmman.Init();

	//
	//	NOTE:
	//
	//		-	���ǻ� �����ϰ� �����ڵ� ������ Client.ini ���Ϸκ��� �е��� �� �ξ����ϴ�
	//		-	�ϵ��ڵ� �ϼŵ� ��� �����ϴ�
	//

	NMLOCALEID	uLocaleId = ClientUtil::Settings::GetLocaleId();
	NMGameCode	uGameCode = ClientUtil::Settings::GetGameCode();

	ClientUtil::Log::LogTime( _T("Client begins...") );
	ClientUtil::Log::LogInfo( _T("    Locale ID: %s"), ClientUtil::Convert::LocaleIdToString( uLocaleId ) );
	ClientUtil::Log::LogInfo( _T("    Game Code: %s"), ClientUtil::Convert::GameCodeToString( uGameCode ) );

	//
	//	[�ʼ�] ������ ����
	//
	if ( bResult )
	{
		bResult = _nmco.SetLocale( uLocaleId );
		ClientUtil::Log::LogTime( _T("_nmco.SetLocale( %s ): %s!"), ClientUtil::Convert::LocaleIdToString( uLocaleId ), ClientUtil::Convert::ResultToString( bResult ) );
	}

	//
	//	[�ʼ�] �޽��� ��� �ʱ�ȭ
	//
	if ( bResult )
	{
		bResult = _nmco.Initialize( uGameCode );
		ClientUtil::Log::LogTime( _T("_nmco.Initialize( %s ): %s!"), ClientUtil::Convert::GameCodeToString( uGameCode ), ClientUtil::Convert::ResultToString( bResult ) );
	}

	//
	//	[����] Ŭ���̾�Ʈ ��ġ
	//
	//		-	��ġ�� �ʿ����� ���δ� �޽����� ������� ���ӿ��� �����մϴ�.
	//		-	�� ���������� �Ʒ� bNeedPatch ���� ���� ��ġ ���θ� �����մϴ�.
	//
	BOOL bNeedPatch = FALSE;

	if ( bNeedPatch )
	{
		//
		//	NGM ��ġ ���� Ȯ��
		//
		//		-	NGM �� �̿��ؼ� ���� Ŭ���̾�Ʈ�� ��ġ�ϴ� ������ ��쿡
		//			NGM �� ��ġ�Ǿ� ���� ������ ��ġ�� �Ұ����մϴ�.
		//
		if ( bResult )
		{
			bResult = NMCOHelpers::IsNGMInstalled();
			ClientUtil::Log::LogTime( _T("NMCOHelpers::IsNGMInstalled(): %s!"), ClientUtil::Convert::ResultToString( bResult ) );
		}

		//
		//	��ġ ����
		//
		//		-	szPatchURL �� szPatchDir �� ���������� �����Ͽ� �׽�Ʈ �Ͻñ� �ٶ��ϴ�.
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
			//	���� Ŭ���̾�Ʈ ����
			//
			//		-	���� ���� Ŭ���̾�Ʈ�� ExecuteNGMPatcher() ȣ�� �� �����ؾ� �մϴ�.
			//			�׷��� NGM�� ���� Ŭ���̾�Ʈ ������ ��ġ�� �� �ֱ� �����Դϴ�.
			//
			//		-	��, �� ���������� ���α׷��� �������� �ʰ� ��� �����ϰڽ��ϴ�.
			//
		}
	}

	//
	//	[�ʼ�] �޽��� �̺�Ʈ �ڵ鷯 ���
	//
	//		-	�޽������� �߻��ϴ� ���� �̺�Ʈ�� �޾� �� �� �ֵ��� �̺�Ʈ �ڵ鷯�� ����մϴ�
	//
	if ( bResult )
	{
		bResult = _nmco.RegisterCallbackMessage( hWnd, WM_NMEVENT );
		ClientUtil::Log::LogTime( _T("_nmco.RegisterCallbackMessage( 0x%08x, WM_NMEVENT ): %s!"), hWnd, ClientUtil::Convert::ResultToString( bResult ) );
	}

	//
	//	[�ʼ�] �������� �α���
	//
	//		-	�� �Լ��� ���������� ����ǹǷ� �Լ��� ������ ������ ���μ����� ��ϵ˴ϴ�.
	//
	if ( bResult )
	{
		switch ( uLocaleId )
		{
		case kLocaleID_KR:
			{
				//
				//	-	Ŀ�ǵ�������κ��� �н���Ʈ ��Ʈ���� ������ ���ϴ�.
				//	-	�Ʒ� �ڵ�� "GameClient.exe PassportString" ��������, 
				//	-	Ŭ���̾�Ʈ ��� �� �κп� ��ĭ ���� �ٷ� �н���Ʈ ��Ʈ���� �Ѿ���� ��츦 �����ϰ� �ֽ��ϴ�.
				//	-	�Ϲ������δ� �� ������ ���� ���� ������ Ŀ�ǵ������ ���� �н���Ʈ ���� ������ ���������� �����ϴ� ���� �����ϹǷ�,
				//	-	Ư�� ������ �ʿ��� ��� �÷������������� ��û�ϵ��� �մϴ�.
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
					//	-	Ŀ�ǵ���ο��� �н���Ʈ ��Ʈ�� ������ ������ ���, ����Ī �������� �����մϴ�.
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
					//	-	Ŀ�ǵ���ο��� �н���Ʈ ��Ʈ�� ������ ������ ���, ���ĵ��� �������� �����մϴ�.
					//	-	���ǻ� �α��� ���� ������ Client.ini ���Ϸκ��� �е��� �� �ξ����ϴ�
					//	-	�ϵ��ڵ� �ϼŵ� ��� �����ϴ�
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
					//	�������� ���� ȹ��
					//
					_nmco.GetMyInfo();

					//
					//	�������� ���� ǥ��
					//
					ClientUtil::Log::LogInfo( _T("    NexonSN: 0x%08x"), _nmco.GetMyOid() );
					ClientUtil::Log::LogInfo( _T("    NexonID: %s"), _nmco.GetMyLoginId() );
					ClientUtil::Log::LogInfo( _T("    Sex: %s"), ClientUtil::Convert::SexTypeToString( _nmco.GetMySex() ) );
					ClientUtil::Log::LogInfo( _T("    Age: %d"), _nmco.GetMyAge() );

					//
					//	�ؽ��н���Ʈ ���
					//
					TCHAR szNexonPassport[ NXPASSPORT_SIZE ];
					ClientUtil::Log::LogInfo( _T("    Nexon Passport: %s"), _nmco.GetNexonPassport( szNexonPassport ) );
					
					//
					//	[�ʼ�] ���� ������ ���� 2�� ����
					//
					//		-	���� �������� ������ ���÷� ���۵Ǿ� ���� ������,
					//		-	���� Ŭ���̾�Ʈ�� ���⼭ �޾ƿ� �ؽ� �н���Ʈ�� ���� ������ �����Ͽ� ���� ������ �ؽ� ���� ���� ���� 2�� ������ ��ģ��.
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
	//	[����] �޽��� ���� �α���
	//
	//		-	�� �Լ��� �񵿱������� ����ǹǷ�
	//			OnMessengerReplyEvent �̺�Ʈ�� �޾ƾ� ���� �޽��� ���� ���θ� �� �� �ֽ��ϴ�
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
			//	������������ �α׾ƿ� �մϴ�
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
		//	������������ �α׾ƿ�
		//
		_nmco.LogoutAuth();
		//
		//	�޽��� �̺�Ʈ �ڵ鷯 ��� ����
		//
		_nmco.ResetCallbackMessage( hWnd );
		//
		//	�޽��� �Ŵ��� ����
		//
		_nmman.Finalize();

		PostQuitMessage(0);
		break;
	case WM_NMEVENT:
		//
		//	�޽��� �̺�Ʈ ó��
		//
		g_msngrEventHandler.HandleNMEvent( wParam, lParam );
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
