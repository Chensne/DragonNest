#include "stdafx.h"
#include "DNC2C.h"
#include "Log.h"
#include "DNDivisionManager.h"
#include "DNVillageConnection.h"
#include "DNGameConnection.h"
#include "DNUser.h"

#if defined( PRE_ADD_CHNC2C )
CDNC2C* g_pDnC2C;
#pragma comment(lib, "../ServerCommon/CH/ogsdk.lib")

// CallBack
void MessageProc(
				 OpenGame::ISDOGService * pService,
				 const char * pMethod,
				 OpenGame::ISDProperty * pBody)
{
	if (0 == strncmp(pMethod, "character.addCoin", 18))
	{
		//process request
		std::string t_strSeqID = pBody->GetStringValue("seq_id", "");
		OpenGame::ISDProperty* pParams = pBody->GetPropValue("params");

		std::string t_strBookID = pParams->GetStringValue("book_id", "");
		std::string t_strCharacterID = pParams->GetStringValue("character_id", "");
		std::string t_strQuantity = pParams->GetStringValue("quantity", "");
		int t_Quantity = atoi(t_strQuantity.c_str());
		t_Quantity = t_Quantity*10000; //Gold�� ���

		OpenGame::SDReleaseProperty(pParams);

		//logic
		INT64 biCharacterDBID = _atoi64(t_strCharacterID.c_str());
		CDNUser *pUser = g_pDivisionManager->GetUserByCharacterDBID(biCharacterDBID);
		BYTE nVillageID = 0;
		USHORT nGameID = 0;
		UINT nAccountDBID = 0;
		if( pUser ) // ������ ���ӳ��� �ִ� ���¸�..�ش� ������ ��û�ϰ� �ƴϸ� �ݷ� �������� ����.
		{
			nAccountDBID = pUser->GetAccountDBID();
			if( pUser->GetUserState() == STATE_VILLAGE )			
				nVillageID = pUser->GetVillageID();
			else if( pUser->GetUserState() == STATE_GAME )				
				nGameID = pUser->GetGameID();
			else
				nVillageID = g_pDivisionManager->GetPvPLobbyVillageID();
		}
		else
			nVillageID = g_pDivisionManager->GetPvPLobbyVillageID();

		if( nVillageID > 0 )
		{
			CDNVillageConnection* pVillageConnection = g_pDivisionManager->GetVillageConnectionByVillageID( nVillageID );
			if( pVillageConnection == NULL || pVillageConnection->GetActive() == false )
			{
				g_Log.Log(LogType::_FILEDBLOG, 0, 0, biCharacterDBID, 0, L"C2C addCoin not villageserver cid:%S, seq_id:%S, Quantity:%d!!\r\n"
					, t_strCharacterID.c_str(), t_strSeqID.c_str(), t_Quantity);
				g_pDnC2C->SendReduceCoin(-99, t_strSeqID.c_str());
				return;
			}
			pVillageConnection->SendC2CAddCoin(nAccountDBID, biCharacterDBID, t_Quantity, t_strSeqID.c_str(), t_strBookID.c_str() );
		}
		else if ( nGameID > 0 )
		{
			CDNGameConnection* pGameConnection = g_pDivisionManager->GetGameConnectionByGameID( nGameID );
			if( pGameConnection == NULL || pGameConnection->GetActive() == false )
			{
				g_Log.Log(LogType::_FILEDBLOG, 0, 0, biCharacterDBID, 0, L"C2C addCoin not gameserver cid:%S, seq_id:%S, QUantity:%d!!\r\n"
					, t_strCharacterID.c_str(), t_strSeqID.c_str(), t_Quantity);
				g_pDnC2C->SendReduceCoin(-99, t_strSeqID.c_str());
				return;
			}
			pGameConnection->SendC2CAddCoin(nAccountDBID, biCharacterDBID, t_Quantity, t_strSeqID.c_str(), t_strBookID.c_str() );
		}

		//response
		//g_pDnC2C->SendAddCoin(0, t_strSeqID.c_str());
	}
	else if(0 == strncmp(pMethod, "character.reduceCoin", 21))
	{
		//process request
		std::string t_strSeqID = pBody->GetStringValue("seq_id", "");
		OpenGame::ISDProperty* pParams = pBody->GetPropValue("params");

		std::string t_strBookID = pParams->GetStringValue("book_id", "");
		std::string t_strCharacterID = pParams->GetStringValue("character_id", "");
		std::string t_strQuantity = pParams->GetStringValue("quantity", "");
		int t_Quantity = atoi(t_strQuantity.c_str());
		t_Quantity = t_Quantity*10000; //Gold�� ���

		OpenGame::SDReleaseProperty(pParams);

		//logic
		INT64 biCharacterDBID = _atoi64(t_strCharacterID.c_str());
		CDNUser *pUser = g_pDivisionManager->GetUserByCharacterDBID(biCharacterDBID);
		BYTE nVillageID = 0;
		USHORT nGameID = 0;
		UINT nAccountDBID = 0;
		if( pUser ) // ������ ���ӳ��� �ִ� ���¸�..�ش� ������ ��û�ϰ� �ƴϸ� �ݷ� �������� ����.
		{
			nAccountDBID = pUser->GetAccountDBID();
			if( pUser->GetUserState() == STATE_VILLAGE )			
				nVillageID = pUser->GetVillageID();
			else if( pUser->GetUserState() == STATE_GAME )				
				nGameID = pUser->GetGameID();
			else
				nVillageID = g_pDivisionManager->GetPvPLobbyVillageID();
		}
		else
			nVillageID = g_pDivisionManager->GetPvPLobbyVillageID();

		if( nVillageID > 0 )
		{
			CDNVillageConnection* pVillageConnection = g_pDivisionManager->GetVillageConnectionByVillageID( nVillageID );
			if( pVillageConnection == NULL || pVillageConnection->GetActive() == false )
			{
				g_Log.Log(LogType::_FILEDBLOG, 0, 0, biCharacterDBID, 0, L"C2C reduceCoin not villageserver cid:%S, seq_id:%S, Quantity:%d!!\r\n"
					, t_strCharacterID.c_str(), t_strSeqID.c_str(), t_Quantity);
				g_pDnC2C->SendReduceCoin(-99, t_strSeqID.c_str());
				return;
			}
			pVillageConnection->SendC2CReduceCoin(nAccountDBID, biCharacterDBID, t_Quantity, t_strSeqID.c_str(), t_strBookID.c_str() );
		}
		else if ( nGameID > 0 )
		{
			CDNGameConnection* pGameConnection = g_pDivisionManager->GetGameConnectionByGameID( nGameID );
			if( pGameConnection == NULL || pGameConnection->GetActive() == false )
			{
				g_Log.Log(LogType::_FILEDBLOG, 0, 0, biCharacterDBID, 0, L"C2C reduceCoin not gameserver cid:%S, seq_id:%S, QUantity:%d!!\r\n",
					t_strCharacterID.c_str(), t_strSeqID.c_str(), t_Quantity);
				g_pDnC2C->SendReduceCoin(-99, t_strSeqID.c_str());
				return;
			}
			pGameConnection->SendC2CReduceCoin(nAccountDBID, biCharacterDBID, t_Quantity, t_strSeqID.c_str(), t_strBookID.c_str() );
		}
		//response
		//g_pDnC2C->SendReduceCoin(0, t_strSeqID.c_str());
	}
	else if(0 == strncmp(pMethod, "character.getBalanceInfo", 25))
	{
		//process request
		std::string t_strSeqID = pBody->GetStringValue("seq_id", "");
		OpenGame::ISDProperty* pParams = pBody->GetPropValue("params");

		std::string t_strCharacterID = pParams->GetStringValue("character_id", "");

		OpenGame::SDReleaseProperty(pParams);

		//logic		
		INT64 biCharacterDBID = _atoi64(t_strCharacterID.c_str());
		// ������ �ܾ׿�û�� DB�� �Ѵ�.(�ݷμ��� ���������� ��û)
		CDNVillageConnection* pVillageConnection = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
		if( pVillageConnection == NULL || pVillageConnection->GetActive() == false )
		{
			g_Log.Log(LogType::_FILELOG, 0, 0, biCharacterDBID, 0, "C2C getBalanceInfo not villageserver cid:%S, seq_id:%S!!\r\n", t_strCharacterID.c_str(), t_strSeqID.c_str());
			g_pDnC2C->SendGetBalance(-99, 0, t_strSeqID.c_str());
			return;
		}		
		pVillageConnection->SendC2CGetCoinBalance(biCharacterDBID, t_strSeqID.c_str());
		//response
		//g_pDnC2C->SendGetBalance(0, 1500, t_strSeqID.c_str());
	}
}

CDNC2C::CDNC2C()
{
	m_bStart = false;	
}

CDNC2C::~CDNC2C()
{
	m_pService->Release();
}

void CDNC2C::SendGetBalance(int nRetCode, INT64 nCoin, const char* szSeqID)
{
	char szTemp[64] = {0,};

	OpenGame::ISDProperty * pGetBalance = OpenGame::SDCreateProperty();
	OpenGame::ISDProperty * pResponse = OpenGame::SDCreateProperty();
	OpenGame::ISDProperty * pData = OpenGame::SDCreateProperty();

	pGetBalance->AddStringValue("method", "character.getBalanceInfo");
	pGetBalance->AddStringValue("seq_id", szSeqID);	
	
	pResponse->AddIntValue("return_code", nRetCode);
	if( nRetCode == 0 )			
		pResponse->AddStringValue("return_message", "sucess");		
	else
		pResponse->AddStringValue("return_message", "fail");

	sprintf_s(szTemp, "%I64d", nCoin/10000); //Gold�� ����
	pData->AddStringValue("balance_info", szTemp);

	pResponse->AddPropValue("data", pData);
	pGetBalance->AddPropValue("response", pResponse);

	m_pService->SendPacket(pGetBalance);

	OpenGame::SDReleaseProperty(pData);
	OpenGame::SDReleaseProperty(pResponse);
	OpenGame::SDReleaseProperty(pGetBalance);
}

void CDNC2C::SendAddCoin(int nRetCode, const char* szSeqID)
{
	//response
	OpenGame::ISDProperty * pAddCoin = OpenGame::SDCreateProperty();
	OpenGame::ISDProperty * pResponse = OpenGame::SDCreateProperty();

	pAddCoin->AddStringValue("method", "character.addCoin");
	pAddCoin->AddStringValue("seq_id", szSeqID);

	pResponse->AddIntValue("return_code", nRetCode);
	if( nRetCode == 0 )			
		pResponse->AddStringValue("return_message", "sucess");		
	else
		pResponse->AddStringValue("return_message", "fail");

	pAddCoin->AddPropValue("response", pResponse);

	m_pService->SendPacket(pAddCoin);

	OpenGame::SDReleaseProperty(pResponse);
	OpenGame::SDReleaseProperty(pAddCoin);
}

void CDNC2C::SendReduceCoin(int nRetCode, const char* szSeqID)
{
	//response
	OpenGame::ISDProperty * pReduceCoin = OpenGame::SDCreateProperty();
	OpenGame::ISDProperty * pResponse = OpenGame::SDCreateProperty();

	pReduceCoin->AddStringValue("method", "character.reduceCoin");
	pReduceCoin->AddStringValue("seq_id", szSeqID);

	pResponse->AddIntValue("return_code", nRetCode);
	if( nRetCode == 0 )			
		pResponse->AddStringValue("return_message", "sucess");		
	else
		pResponse->AddStringValue("return_message", "fail");

	pReduceCoin->AddPropValue("response", pResponse);

	m_pService->SendPacket(pReduceCoin);

	OpenGame::SDReleaseProperty(pResponse);
	OpenGame::SDReleaseProperty(pReduceCoin);

}

bool CDNC2C::Init(const std::string & strRemoteIP, int RemotePort, const std::string & strMatrixID, const std::string & DeviceID)
{	
	m_pService = OpenGame::SDGetOGService();
	if(m_pService == NULL)
	{
		g_Log.Log(LogType::_FILELOG, L"C2C get OGService error!!\r\n");		
		return false;
	}
	
	if(m_pService->Init(&MessageProc, 
		strRemoteIP.c_str(),
		RemotePort,
		strMatrixID.c_str(),
		DeviceID.c_str(), true) < 0)
	{
		g_Log.Log(LogType::_FILELOG, L"C2C init OGService error!!\r\n");		
		return false;
	}
	m_bStart = true;
	
	return true;
}

void CDNC2C::Run()
{
	if( m_bStart )
		m_pService->Run();
}
#endif //#if defined( PRE_ADD_CHNC2C )
