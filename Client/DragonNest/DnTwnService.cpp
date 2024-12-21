#include "StdAfx.h"
#include "DnTwnService.h"
#include "LoginSendPacket.h"
#include "DnString.h"
#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif 

#if defined(_TW) && defined(_AUTH)

int	DnTwnService::Initialize( void* pCustomData )
{
	if( m_bWebLogin )
	{
		WCHAR szLocalIP[ 32 ];
		CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );
		SendCheckLoginTW( m_strId.c_str(), m_strPassword.c_str(), szLocalIP );
	}
	return 0;
}

int DnTwnService::PreInitialize( void* pCustomData )
{
	TCHAR*	pCmdLine = ::GetCommandLine();
	std::wstring wszCmdLine; 
	wszCmdLine = pCmdLine;
	std::vector<std::wstring> tokens;
	TokenizeW( wszCmdLine, tokens, std::wstring(L" ") );

	for( DWORD i=0; i<tokens.size(); i++ )
	{//wtf
		if( wcscmp( tokens[i].c_str(), L"/a" ) == 0 )
		{
			if( i+1 <tokens.size() )
				m_strId = tokens[i+1].c_str();
		}

		if( wcscmp( tokens[i].c_str(), L"/p" ) == 0 )
		{
			if( i+1 <tokens.size() )
				m_strPassword = tokens[i+1].c_str();
		}
	}

	tokens.clear();

	if( m_strId.length() > 0 && m_strPassword.length() > 0 )
		m_bWebLogin = TRUE;

#ifdef _HSHIELD
	int nResult = 0;
	nResult = HShieldSetup::Init( GetEtDevice()->GetHWnd() );
	if( nResult < 0 )
	{
		HShieldSetup::Stop();
		MessageBoxW( 0, STR_HS_INITIALIZED_FAIL, L"DragonNest", 0 );

		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
		return 0;
	}
#if defined(_DEBUG_HSHIELD)
	DebugLog( "Skiped HackSheild update." );
#else
	nResult = HShieldSetup::Update();
	if( nResult < 0 )
	{
		HShieldSetup::Stop();
		MessageBoxW( 0, STR_HS_UPDATE_FAIL, L"DragonNest", 0 );

		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
		return 0;
	}
#endif //_DEBUG_HSHIELD
	nResult = HShieldSetup::Start();
	if( nResult < 0 )
	{
		HShieldSetup::Stop();
		MessageBoxW( 0, STR_HS_START_FAIL, L"DragonNest", 0 );

		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
		return 0;
	}
	HShieldSetup::SetMyID( m_strId.c_str() );
#endif //_HSHIELD

	return TRUE;
}

#endif // _TW