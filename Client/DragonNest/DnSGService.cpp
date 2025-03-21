#include "stdafx.h"
#ifdef _SG

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#include "DnSGService.h"
#include "../Extern/GPK/Include/GPKitClt.h"
//#include "DNProtocol.h"
//#include "DNPacket.h"
#include "SystemSendPacket.h"
//#include "DnInterface.h"

using namespace SGPK;

IGPKCltDynCode* pCltDynCode = NULL;
PUCHAR			DynCode = NULL;

//----------------------------------------------------------------------------------------------------------------------------
DnSGService::DnSGService()
{
}

DnSGService::~DnSGService()
{
}

int DnSGService::PreInitialize(void* pCustomData)
{
#ifdef _GPK
	// 런처에서 업데이트 하는 방식으로 바뀌어 NULL로 대체
	pCltDynCode = GPKStart( NULL, "DN" );
	if ( !pCltDynCode )
	{
		OutputDebug( "GPKStart Failed\n" );
		MessageBoxA( NULL, "GPKit Start failed.", NULL, 0 );
		return -1;
	}

	OutputDebug( "GPKStart Success\n" );
	DynCode = (PUCHAR)VirtualAlloc( NULL, 32*1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
#endif

	return 0;
}

int DnSGService::Release()
{
	if( pCltDynCode != NULL )
		pCltDynCode->Release();

	return 0;
}

void DnSGService::OnDispatchMessage( int iMainCmd, int iSubCmd, char * pData, int iLen )
{
#ifdef _GPK
	if( iMainCmd == SC_SYSTEM )
	{
		switch( iSubCmd )
		{
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
		}
	}
#endif // _GPK
}

#endif // _SG