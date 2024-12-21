#include "StdAfx.h"
#include "DnKRAZService.h"
#include "LoginSendPacket.h"
#include "DnString.h"
#include "Shellapi.h"


#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif 

#if defined(_KRAZ) && defined(_AUTH)

DnKRAZService::DnKRAZService()
{
}

DnKRAZService::~DnKRAZService()
{
}

int	DnKRAZService::Initialize( void* pCustomData )
{
	WCHAR szLocalIP[ 32 ];
	SecureZeroMemory( szLocalIP, sizeof( szLocalIP ) );
	CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );

#if defined(_FINAL_BUILD)
#else
	ToWideString(CGlobalValue::GetInstance().m_szRequestTime,	m_RequestTime);
	ToWideString(CGlobalValue::GetInstance().m_szKeyID,			m_keyID);
#endif
	SendCheckLoginKRAZ((WCHAR*)m_keyID.c_str(), (WCHAR*)m_RequestTime.c_str(), szLocalIP);

	return 0;
}

int DnKRAZService::PreInitialize( void* pCustomData )
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
#endif //_HSHIELD

	//----------------------------------------------------------------------------------------------------
	// ����� ���̳� ���勚�� ���´�..
	// ����׶��� �׽�Ʈ����, DnCmdLineParse���� �Ľ��Ѵ�.. Global������ �����Ѵ�. �򰥷��� �ּ��� ���.
#if defined(_FINAL_BUILD)
	TCHAR* pCmdLine = (TCHAR*)pCustomData;
	if(!CmdLineParse(pCmdLine))
	{
		return FALSE;
	}
#endif

	return TRUE;
}

void DnKRAZService::HShieldSetMyID( std::wstring strID )
{
#ifdef _HSHIELD
	m_strId = strID;
	HShieldSetup::SetMyID( strID.c_str() );
#endif
}

bool DnKRAZService::CmdLineParse( TCHAR *szString )
{
	// - 2012.11.20 -
	// RequestTime�� ���鿡�� �����ٰ� �ڴʰ� ��������. �����մϴ�.
	if( szString == NULL )
	{
		// NULL�̸�, Ŭ���̾�Ʈ ����.
		MessageBox( NULL, L"Ȩ���������� �����ư�� �����ּ���.", L"DragonNest", MB_OK );
		return false;
	}

	std::vector<std::wstring> vwStringParam;
	TokenizeW(szString, vwStringParam, L" ");
	if(vwStringParam.empty() || vwStringParam.size() < 2)
		return false;
	
	RemoveStringW(vwStringParam[0], std::wstring(L"/requesttime:"));
	RemoveStringW(vwStringParam[1], std::wstring(L"/keyid:"));
	m_RequestTime	= vwStringParam[0];
	m_keyID			= vwStringParam[1];

	return true;


	/*
	���α��ν�  
	[Actoz]���� �����ִ� �Ķ���ʹ� ������ ����.
	
	ex)
	2012-10-10\_322\_702\_700 55212457EDD1739BF29F68AA654F9EBE6E8A5203FBCC2DA098C251646A53B15DD66D 31 00160F52 0
	========================= ----------------------------------------------------------------------------------
	RequestTime(25��)		  KeyID(������)
	
	�Ķ���;ȿ� ���� ������ ��ǥ����, �׳� ��¥�� �پ �����ش�.. 
	���ε��� ��󳾼� ���� ������.
	�׳� ������� �������ۿ� ���, �̸� ���س��� ��Ģ�� �����, �Ľ��� ���������� ���� �ʴ´�..

	��Ģ)
	RequestTime KeyID	 ��Ÿ����.
	[25��]      [������] [������ʹ� /����]
	*/

	/*
	TCHAR szRequestTime[256] = { 0 , };
	TCHAR szKeyID[256]		 = { 0 , };

	int nSize = (int)_tcslen(szString);
	
	// 1. RequestTime (25��)
	for(int i = 0 ; i < nSize ; ++i)
	{
		if( i > 24 ) break;
		szRequestTime[i] = szString[i];
	}
	m_RequestTime = szRequestTime;

	// 2. KeyID (������)
	for(int i=0, j=26 ; j < nSize ; ++i, ++j)
	{
		if(szString[j] == '/')
			break;
		szKeyID[i] = szString[j];
	}
	char szTemp[256] = { 0 , };
	WideCharToMultiByte( CP_ACP, 0, szKeyID, -1, szTemp, 256, NULL, NULL);
	TrimString(szTemp);
	MultiByteToWideChar( CP_ACP, 0, szTemp, -1, szKeyID, 256);
	m_keyID = szKeyID;

	// 3.��Ÿ������ �մ��� DnCmdLineParse���� �̹� ����Ǿ� �ִ�.
	*/
}

#endif // _KRAZ & _AUTH