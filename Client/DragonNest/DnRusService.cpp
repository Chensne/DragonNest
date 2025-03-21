#include "StdAfx.h"
#include "DnRusService.h"
#include "LoginSendPacket.h"
#include "DnString.h"
#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif 

#if defined(_RU) && defined(_AUTH)

int	DnRusService::Initialize( void* pCustomData )
{
	WCHAR szLocalIP[ 32 ];
	CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );
	SendCheckLoginRUS( m_strId.c_str(), m_strPassword.c_str(), szLocalIP );
	return 0;
}

int DnRusService::PreInitialize( void* pCustomData )
{
	TCHAR*	pCmdLine = ::GetCommandLine();
	std::wstring wszCmdLine; 
	wszCmdLine = pCmdLine;
	std::vector<std::wstring> tokens;
	TokenizeW( wszCmdLine, tokens, std::wstring(L" ") );

	for( DWORD i=0; i<tokens.size(); i++ )
	{
		if( StrStrW( tokens[i].c_str(), L"/id:" ) != 0 )
		{
			std::vector<std::wstring> vecTemp;
			TokenizeW( tokens[i].c_str(), vecTemp, std::wstring(L":") );
			if( vecTemp.size() > 1 )
				m_strId = vecTemp[1].c_str();
			vecTemp.clear();
		}

		if( StrStrW( tokens[i].c_str(), L"/pass:" ) != 0 )
		{
			std::vector<std::wstring> vecTemp;
			TokenizeW( tokens[i].c_str(), vecTemp, std::wstring(L":") );
			if( vecTemp.size() > 1 )
				m_strPassword = vecTemp[1].c_str();
			vecTemp.clear();
		}
	}

	tokens.clear();
	
	if( m_strId.length() == 0 || m_strPassword.length() == 0 )
		return 0;
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
	//	HShieldSetup::SetMyID( m_strId.c_str() );
#endif //_HSHIELD

	return TRUE;
}

void DnRusService::HShieldSetMyID( std::wstring strID )
{
#ifdef _HSHIELD
	m_strId = strID;
	HShieldSetup::SetMyID( strID.c_str() );
#endif // _HSHIELD
}

#endif // _RU, _AUTH