#include "stdafx.h"
#ifdef _CH

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#include "DnShandaService.h"
#include "../Extern/GPK/Include/GPKitClt.h"
#include "DNProtocol.h"
#include "DNPacket.h"
#include "SystemSendPacket.h"
#include "DnInterface.h"
#ifdef _AUTH
#include "DnSDOAService.h"
#endif // _AUTH
using namespace SGPK;

IGPKCltDynCode* pCltDynCode = NULL;
PUCHAR			DynCode = NULL;
#define	WM_SHANDA_EVENT	( WM_USER + 20 )

//----------------------------------------------------------------------------------------------------------------------------
DnShandaService::DnShandaService()
{
#ifdef _AUTH
	CDnSDOAService::CreateInstance();
#endif // _AUTH
}

DnShandaService::~DnShandaService()
{
#ifdef _AUTH
	CDnSDOAService::DestroyInstance();
#endif // _AUTH
}

int
DnShandaService::PreInitialize(void* pCustomData)
{
#ifdef _GPK
	// 릴리즈에서는 아래 url 로 해달라네요.. 뒤에 "GPKDEMO" 는 어떻게 바꿔야하는지 모르겠음. 물어봐야할듯. -> DN으로 바꿔달라네요 ㅎㅎ
	// http://lzg.autopatch.sdo.com/lzg/GPK
	// 런처에서 업데이트 하는 방식으로 바뀌어 NULL로 대체
	pCltDynCode = GPKStart( NULL, "DN" );
	if ( !pCltDynCode )
	{
		OutputDebug( "GPKStart Failed\n" );
		MessageBoxA(NULL, "GPKit Start failed.", NULL, 0);
		return -1;
	}

	OutputDebug( "GPKStart Success\n" );
	DynCode = (PUCHAR)VirtualAlloc(NULL, 32*1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#endif

#ifdef _AUTH
	if( CDnSDOAService::GetInstance().Initialize( false ) == false )
		return -1;
#endif // _AUTH

	return 0;
}

int
DnShandaService::Initialize(void* pCustomData)
{
	return 0;
}

int
DnShandaService::Release()
{
	if (pCltDynCode != NULL)
		pCltDynCode->Release();

	return 0;
}

LRESULT
DnShandaService::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_SHANDA_EVENT:
		{

		}
		break;
	}

	return 0;
}

void 
DnShandaService::OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	switch( iMainCmd )
	{
	case SC_SYSTEM:
		{
			switch( iSubCmd )
			{
#ifdef _GPK
			case eSystem::SC_CHN_GPKCODE:
				{
					OutputDebug( "Recv SC_CHN_GPKCODE \n" );
					SCGPKCode* pGPKCode = (SCGPKCode*)pData;
					bool bResult = pCltDynCode->SetDynCode(&(pGPKCode->Code[0]), pGPKCode->nCodeLen);

					OutputDebug( "SetDynCode(%d) %s\n", pGPKCode->nCodeLen, (char*)pGPKCode->Code );

					if ( bResult )
					{
						OutputDebug( "SetDynCode Success \n" );
					}
					else
					{
						OutputDebug( "SetDynCode failed \n" );
					}

					/*
					char sendbuf[GPKDATALENMAX] = "Client DATA...";
					int  sendDataLen = int(strlen(sendbuf) + 1);

					OutputDebug( "Prev Encode [%s] \n", sendbuf );
					bResult = pCltDynCode->Encode((unsigned char*)sendbuf, sendDataLen);

					OutputDebug( "Encode [%s] \n", sendbuf );
					if ( bResult )
					{
						OutputDebug( "Encode Success \n" );
					}
					else
					{
						OutputDebug( "Encode failed \n" );
					}

					SendGPKData((char*)sendbuf);
					OutputDebug( "SendGPKData..\n" );
					*/
				}
				break;
			case eSystem::SC_CHN_GPKDATA:
				{
					SCGPKData* pGpkData = (SCGPKData*)pData;

					OutputDebug( "Prev Decode [%s] \n", pGpkData->Data );
					bool bResult = pCltDynCode->Decode((unsigned char*)pGpkData->Data, GPKDATALENMAX);
					
					if ( bResult )
					{
						OutputDebug( "Decode Success \n" );
						OutputDebug( "Decode [%s] \n", pGpkData->Data );
					}
					else
					{
						OutputDebug( "Decode failed \n" );
					}

					if (strcmp(pGpkData->Data, "ServerCheck!!"))
					{
						SendGPKData((char*)pGpkData->Data);
						return;
					}

					SCGPKData packet;
					memset(&packet, 0, sizeof(SCGPKData));

					_strcpy(packet.Data, _countof(packet.Data), "ClientCheck!!!", (int)strlen("ClientCheck!!!"));

					OutputDebug( "Prev Encode [%s] \n", packet.Data );
					bResult = pCltDynCode->Encode((unsigned char*)packet.Data, GPKDATALENMAX);

					OutputDebug( "Encode [%s] \n", packet.Data );
					if ( bResult )
					{
						OutputDebug( "Encode Success \n" );
					}
					else
					{
						OutputDebug( "Encode failed \n" );
					}

					SendGPKData((char*)packet.Data);
					OutputDebug( "SendGPKData..\n" );

					/*OutputDebug( "Prev Decode [%s] \n", pGpkData->Data );
					bool bResult = pCltDynCode->Decode((unsigned char*)pGpkData->Data, pGpkData->nLen);
					
					if ( bResult )
					{
						OutputDebug( "Decode Success \n" );
						OutputDebug( "Decode [%s] \n", pGpkData->Data );
					}
					else
					{
						OutputDebug( "Decode failed \n" );
					}

					char sendbuf[GPKDATALENMAX] = "Client DATA...";
					int  sendDataLen = int(strlen(sendbuf) + 1);

					OutputDebug( "Prev Encode [%s] \n", sendbuf );
					bResult = pCltDynCode->Encode((unsigned char*)sendbuf, sendDataLen);

					OutputDebug( "Encode [%s] \n", sendbuf );
					if ( bResult )
					{
						OutputDebug( "Encode Success \n" );
					}
					else
					{
						OutputDebug( "Encode failed \n" );
					}

					SendGPKData((char*)sendbuf);
					OutputDebug( "SendGPKData..\n" );*/
				}
				break;

			case eSystem::SC_CHN_GPKAUTHDATA:
				{
					SCGPKAuthData *pGpkData = (SCGPKAuthData*)pData;
					SendGPKAuthData(pGpkData->Data, pGpkData->nLen);
				}
				break;
#endif // _GPK

			case eSystem::SC_CHN_FCM:
				{
					SCFCMState* pFCMData = (SCFCMState*)pData;

					WCHAR wszTemp[256];
					WCHAR wszAdd[256];

					if ((pFCMData->nOnlineMin >= 60) && (pFCMData->nOnlineMin < 120)){
						// 당신은 1시간 플레이했음;
						swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101 ), pFCMData->nOnlineMin / 60 );
						GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
					}
					else if ((pFCMData->nOnlineMin >= 120) && (pFCMData->nOnlineMin < 180)){
						// 당신은 2시간 플레이했음;
						swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101 ), pFCMData->nOnlineMin / 60 );
						GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
					}
					else if ((pFCMData->nOnlineMin >= 180) && (pFCMData->nOnlineMin < 190)){	// 1분, 2분, 5분.. 늦을때 있기때문에 별도처리 해달라고 한다.
						// 당신은 3시간 플레이 했음. 휴식좀.
						swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101 ), pFCMData->nOnlineMin / 60 );
						swprintf_s( wszAdd, _countof(wszAdd), L"%s %s", wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 102 ) );
						GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszAdd );
						// 피로시간이므로 50% 임.
						swprintf_s( wszTemp, _countof(wszTemp), L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 103 ) );
						GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
					}
					else if ((pFCMData->nOnlineMin >= 190) && (pFCMData->nOnlineMin < 300)){
						// 피로시간이므로 50% 임.
						swprintf_s( wszTemp, _countof(wszTemp), L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 103 ) );
						GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
					}
					else if (pFCMData->nOnlineMin >= 300){
						// 불건강 시간임! 0%임.
						swprintf_s( wszTemp, _countof(wszTemp), L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 104 ) );
						GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
					}
				}
				break;
			}
		}
		break;
	}
}

int 
DnShandaService::OnEvent(int nType, void* pExtra)
{
	return 0;
}

bool DnShandaService::WriteStageLog_( INT32 nStage, LPCTSTR szComment )
{
#ifdef _AUTH
	if( nStage == IServiceSetup::GameStart )
	{
		CDnSDOAService::GetInstance().OnCreateDevice();
	}
	else if( nStage == IServiceSetup::Game_Exit )
	{
		CDnSDOAService::GetInstance().OnDestroyDevice();
		CDnSDOAService::GetInstance().Finalize();
	}
#endif // _AUTH

	return false;
}

#endif // _CH