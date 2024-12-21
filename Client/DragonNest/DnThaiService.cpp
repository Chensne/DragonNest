#include "StdAfx.h"
#include "DnThaiService.h"
#include "LoginSendPacket.h"
#include "DnString.h"
#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif 

#if defined(_TH) && defined(_AUTH)

int	DnThaiService::Initialize( void* pCustomData )
{
	return 0;
}

int DnThaiService::PreInitialize( void* pCustomData )
{
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

void DnThaiService::HShieldSetMyID( std::wstring strID )
{
#ifdef _HSHIELD
	m_strId = strID;
	HShieldSetup::SetMyID( strID.c_str() );
#endif // _HSHIELD
}

#endif // _TH