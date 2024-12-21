#include "StdAfx.h"
#include "LoginClientSession.h"
#include "ClientSessionManager.h"
#include "DnLoginTask.h"
#ifdef _TW
#include "DnTwnService.h"
#endif // _TW
#if defined(_US) || defined(_KR)
#include "DnNexonService.h"
#include "DnInterface.h"
#include "DnLoginDlg.h"
#include "DnServerListDlg.h"
#endif // _US, _KR
#if defined(_TH) && defined(_AUTH)
#include "DnInterface.h"
#include "DnLoginDlg.h"
#include "DnThaiService.h"
#endif // _TH, _AUTH
#if defined(_ID) && defined(_AUTH)
#include "DnIdnService.h"
#endif // _TH, _AUTH
#if defined (_RU) && defined(_AUTH)
#include "DnRusService.h"
#endif // _RU, _AUTH
#if defined (_EU) && defined(_AUTH)
#include "DnEUService.h"
#endif // _RU, _AUTH
#if defined (_KRAZ) && defined(_AUTH)
#include "DnKRAZService.h"
#endif // _KRAZ, _AUTH

#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif 

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CLoginClientSession::CLoginClientSession() : CTaskListener(false)
{
}

CLoginClientSession::~CLoginClientSession()
{
}

void CLoginClientSession::RequestLogin( const TCHAR *szID, const TCHAR *szPass )
{
	WCHAR szLocalIP[ 32 ];
	CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );

#if defined(_TW) && defined(_AUTH)
	DnTwnService* twnservice = static_cast<DnTwnService*>( g_pServiceSetup );
	if( !twnservice->IsWebLogin() )
	{
#if defined(_HSHIELD)
		HShieldSetup::SetMyID( szID );
#endif
		SendCheckLoginTW( szID, szPass, szLocalIP );
	}
#elif defined(_TH) && defined(_AUTH)
	//태국은 웹로긴하지 않는 다고함
	int nDomain = 0;
	if( GetInterface().GetLoginDlg() )
		nDomain = GetInterface().GetLoginDlg()->GetDomain();

	DnThaiService* pDnThaiService = static_cast<DnThaiService*>( g_pServiceSetup );
	if( pDnThaiService )
		pDnThaiService->HShieldSetMyID( szID );

	SendCheckLoginTH( szID, szPass, szLocalIP, nDomain );
#elif defined(_US) && defined(_AUTH)
	DnNexonService* pDnNexonService = static_cast<DnNexonService*>( g_pServiceSetup );
	bool bStandAloneMode = false;
	if( pDnNexonService )
	{
		if( pDnNexonService->IsStandAloneMode() )
			bStandAloneMode = true;

		if( bStandAloneMode )
		{
			std::wstring szMsg;
			BOOL bNGMLoginAuth = pDnNexonService->NGMAuthStandAloneMode( szID, szPass, szMsg );
			if( bNGMLoginAuth )
			{
				SendCheckLoginUS( (WCHAR*)pDnNexonService->GetNexonPassPort().c_str(), szLocalIP );
				if( GetInterface().GetServerListDlg() )
					GetInterface().GetServerListDlg()->ShowBackButton( true );
			}
			else
			{
				GetInterface().MessageBox( szMsg.c_str(), MB_OK );
				if( GetInterface().GetLoginDlg() )
					GetInterface().GetLoginDlg()->EnableLoginButton( true );
			}
		}
		else
		{
#ifdef PRE_ADD_RELOGIN_PROCESS
			SendCheckLoginUS( (WCHAR*)pDnNexonService->GetNexonPassPort().c_str(), szLocalIP );
#else // PRE_ADD_RELOGIN_PROCESS
			SendLogin( szID, szPass, szLocalIP );
#endif // PRE_ADD_RELOGIN_PROCESS
		}
	}
#elif defined(_KR) && defined(_AUTH)
#ifdef PRE_ADD_RELOGIN_PROCESS
	DnNexonService* pDnNexonService = static_cast<DnNexonService*>( g_pServiceSetup );
	if( pDnNexonService )
		SendCheckLoginKR( (WCHAR*)pDnNexonService->GetNexonPassPort().c_str(), szLocalIP );
#endif // PRE_ADD_RELOGIN_PROCESS
#elif defined (_ID) && defined(_AUTH)
	char md5pass[PASSWORDLENMAX];
	memset(md5pass, 0, sizeof(md5pass));
	WideCharToMultiByte(CP_ACP, 0, szPass, -1, md5pass, PASSWORDLENMAX-1, NULL, NULL);

	std::string passResult;
	CommonUtil::GetMD5(passResult, md5pass);

	DnIdnService* pDnIdnService = static_cast<DnIdnService*>( g_pServiceSetup );
	if( pDnIdnService )
		pDnIdnService->HShieldSetMyID( szID );

	SendCheckLoginIND(szID, passResult.c_str(), szLocalIP);
#elif defined (_RU) && defined(_AUTH)
	DnRusService* pDnRusService = static_cast<DnRusService*>( g_pServiceSetup );
	if( pDnRusService )
		pDnRusService->HShieldSetMyID( szID );

	SendCheckLoginRUS( szID, szPass, szLocalIP );
#elif defined (_EU) && defined(_AUTH)
	DnEUService* pDnEUService = static_cast<DnEUService*>( g_pServiceSetup );
	if( pDnEUService )
		pDnEUService->HShieldSetMyID( szID );

	SendCheckLoginEU( szID, szPass, szLocalIP );
#elif defined (_KRAZ) && defined(_AUTH)
	DnKRAZService* pDnKRAZService = static_cast<DnKRAZService*>( g_pServiceSetup );
	if( pDnKRAZService )
	{
#ifdef _FINAL_BUILD
		SendCheckLoginKRAZ((WCHAR*)pDnKRAZService->GetKeyID().c_str(), (WCHAR*)pDnKRAZService->GetRequestTime().c_str(), szLocalIP);
#else
		std::wstring wstrKeyID;
		std::wstring wstrRequestTime;
		ToWideString(CGlobalValue::GetInstance().m_szKeyID, wstrKeyID);
		ToWideString(CGlobalValue::GetInstance().m_szRequestTime, wstrRequestTime);

		SendCheckLoginKRAZ((WCHAR*)wstrKeyID.c_str(), (WCHAR*)wstrRequestTime.c_str(), szLocalIP);
#endif // _FINAL_BUILD
	}
#else
	SendLogin( szID, szPass, szLocalIP );
#endif
	FUNC_LOG();
}

void CLoginClientSession::OnConnectTcp()
{
	OutputDebug( "OnConnect\n" );
	FUNC_LOG();
}

void CLoginClientSession::OnDisconnectTcp( bool bValidDisconnect )
{
	if( !bValidDisconnect ) {
		OutputDebug( "OnDisconnect\n" );
		FUNC_LOG();
	}
}

void CLoginClientSession::OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	switch(iMainCmd) {
		case SC_LOGIN:	OnRecvLoginMessage( iSubCmd, pData, iLen ); break;
		case SC_SYSTEM: OnRecvSystemMessage( iSubCmd, pData, iLen ); break;
#ifdef PRE_ADD_DOORS
		case SC_DOORS: OnRecvDoorsMessage(iSubCmd, pData, iLen); break;
#endif
	}
}

void CLoginClientSession::OnRecvLoginMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eLogin::SC_CHECKVERSION: OnRecvLoginCheckVersion( (SCCheckVersion *)pData ); break;
		case eLogin::SC_CHECKLOGIN: OnRecvLoginCheckLogin( (SCCheckLogin*)pData ); break;
		case eLogin::SC_CHARLIST: OnRecvLoginCharList( (SCCharList*)pData ); break;
		case eLogin::SC_WAITUSER: OnRecvLoginWaitUser( (SCWaitUser*)pData ); break;
		case eLogin::SC_CHECKBLOCK: OnRecvLoginBlock( (SCCHeckBlock*)pData ); break;
		case eLogin::SC_CREATECHAR: OnRecvLoginCharCreate( (SCCreateChar *)pData ); break;
		case eLogin::SC_DELETECHAR: OnRecvLoginCharDelete( (SCDeleteChar *)pData ); break;
		case eLogin::SC_SERVERLIST: OnRecvLoginServerList( (SCServerList *)pData ); break;
		case eLogin::SC_CHANNELLIST: OnRecvLoginChannelList( (SCChannelList *)pData ); break;
		case eLogin::SC_BACKBUTTON_LOGIN: OnRecvLoginBackButton(); break;
		case eLogin::SC_REVIVECHAR: OnRecvLoginReviveChar( (SCReviveChar*)pData ); break;
#if defined (PRE_ADD_DWC)
		case eLogin::SC_CREATEDWCCHAR:  OnRecvDWCCharCreate((SCCreateDWCChar*)pData ); break;
		//case eLogin::SC_DWCCHANNELINFO: OnRecvDWCChannelInfo((SCDWCChannelInfo*)pData); break;
#endif		// PRE_ADD_DWC

#if defined (_TH)
		case eLogin::SC_ASIASOFT_REQ_OTP: OnRecvLoginAsiaSoftReqOTP(); break;
		case eLogin::SC_ASIASOFT_RET_OTP: OnRecvLoginAsiaSoftRetOTP((SCOTRPResult*)pData); break;
#endif		//#if defined (_TH)
	}
}

void CLoginClientSession::OnRecvSystemMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eSystem::SC_GAMEINFO: OnRecvSystemGameInfo( (SCGameInfo *)pData ); break;
		case eSystem::SC_VILLAGEINFO: OnRecvSystemVillageInfo( (SCVillageInfo*)pData ); break;
	}
}
#ifdef PRE_ADD_DOORS
void CLoginClientSession::OnRecvDoorsMessage(int nSubCmd, char *pData, int nSize)
{
	switch(nSubCmd)
	{
	case eDoors::SC_GET_AUTHKEY: OnRecvDoorsAuthKey((SCDoorsGetAuthKey*)pData); break;
	case eDoors::SC_CANCEL_AUTH: OnRecvDoorsCancelAuth((SCDoorsCancelAuth*)pData); break;
	case eDoors::SC_GET_MOBILEAUTHFLAG: OnRecvDoorsAuthFlag((SCDoorsGetAuthFlag*)pData); break;
	default: _ASSERT(0);
	}
}
#endif