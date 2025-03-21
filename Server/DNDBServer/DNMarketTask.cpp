#include "StdAfx.h"
#include "DNMarketTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "DNExtManager.h"
#include "Log.h"

CDNMarketTask::CDNMarketTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNMarketTask::~CDNMarketTask(void)
{
}

void CDNMarketTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;
	TQHeader *pHeader = (TQHeader*)pData;
	if( pHeader->nAccountDBID > 0)
	{
		if( g_pSPErrorCheckManager->bIsError(pHeader->nAccountDBID) == true )
		{
			//g_Log.Log(LogType::_ERROR, pHeader->cWorldSetID, pHeader->nAccountDBID, 0, 0, L"[SP_ERRORCHECK] Main:%d, Sub:%d\r\n", nMainCmd, nSubCmd);
			return;
		}
	}

	switch (nSubCmd)
	{
	case QUERY_GETPAGETRADE:
		{
			TQGetPageTrade *pMarket = (TQGetPageTrade*)pData;

			TAGetPageTrade Market;
			memset(&Market, 0, sizeof(TAGetPageTrade));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryGetPageTrade(pMarket, &Market);

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETPAGETRADE:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETPAGETRADE:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;

	case QUERY_GETCOUNTTRADE:
		break;

	case QUERY_GETLISTMYTRADE:
		{
			TQGetListMyTrade *pMarket = (TQGetListMyTrade*)pData;

			TAGetListMyTrade Market;
			memset(&Market, 0, sizeof(TAGetListMyTrade));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryGetListMyTrade(pMarket, &Market);

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETLISTMYTRADE:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETLISTMYTRAD:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;

	case QUERY_GETCOUNTMYTRADE:
		break;

	case QUERY_ADDTRADE:
		{
			TQAddTrade *pMarket = (TQAddTrade*)pData;

			TAAddTrade Market;
			memset(&Market, 0, sizeof(TAAddTrade));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			Market.cInvenIndex = pMarket->cInvenIndex;
			Market.wCount = pMarket->wCount;
			Market.nPrice = pMarket->nPrice;
			Market.cInvenType = pMarket->cInvenType;
			Market.biSerial = pMarket->nSerial;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryAddTrade(pMarket, &Market);

				switch (Market.nRetCode)
				{
				case ERROR_NONE:
				case 103169: // 거래소 등록 가능 횟수를 초과하였습니다.
				case 103186: // 일주일간 거래소에 등록 가능한 총 횟수를 초과하였습니다.
					break;

				default:
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_ADDTRADE:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
					break;
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_ADDTRADE:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;

	case QUERY_CANCELTRADE:
		{
			TQCancelTrade *pMarket = (TQCancelTrade*)pData;

			TACancelTrade Market;
			memset(&Market, 0, sizeof(TACancelTrade));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			Market.nMarketDBID = pMarket->nMarketDBID;
			Market.cInvenSlotIndex = pMarket->cInvenSlotIndex;
			Market.bCashItem = pMarket->bCashItem;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryCancelTrade(pMarket, &Market);
				Market.Item.bEternity = true;
				Market.Item.nLifespan = LIFESPANMAX;
				if (Market.bCashItem) Market.Item.bSoulbound = true;

				switch (Market.nRetCode)
				{
				case ERROR_NONE:
				case 103173: // 무인 상점에 등록 취소할 아이템이 존재하지 않습니다.
				case 103174: // 이미 팔린 아이템입니다.
					break;

				default:
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_CANCELTRADE:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
					break;
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_CANCELTRADE:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;

	case QUERY_BUYTRADEITEM:
		{
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
			TQBuyTradeItem *pMarket = (TQBuyTradeItem*)pData;

			TABuyTradeItem Market;
			memset(&Market, 0, sizeof(TABuyTradeItem));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			Market.nMarketDBID = pMarket->nMarketDBID;
			Market.cInvenSlotIndex = pMarket->cInvenSlotIndex;
			Market.bMini = pMarket->bMini;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				TAGetTradeItemID Trade;
				memset(&Trade, 0, sizeof(TAGetTradeItemID));
				Trade.nAccountDBID = pMarket->nAccountDBID;
				Trade.nMarketDBID = pMarket->nMarketDBID;

				Trade.nRetCode = pWorldDB->QueryGetTradeItemID(pMarket->nMarketDBID, &Trade);

				if (Trade.nRetCode == ERROR_NONE){
					pMarket->bCashItem = g_pExtManager->IsCashItem(Trade.nMarketItemID);
					if ((!pMarket->bCashItem) && (Market.cInvenSlotIndex == (BYTE)-1)){
						Market.nRetCode = ERROR_ITEM_INVENTORY_NOTENOUGH;
						m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
						return;
					}

#if defined(PRE_ADD_PETALTRADE)
					if (Trade.cMethodCode != DBDNWorldDef::PayMethodCode::Petal)
						Trade.nPrice = 0;

					// 페탈상품이면 페탈부터 차감..
					if( Trade.nPrice > 0)
					{
						pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);

						if (pMembershipDB)			
							Market.nRetCode = pMembershipDB->QueryPurchaseTradeItemByPetal(pMarket->biCharacterDBID, Trade.nPrice);			
						else			
							g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM] MemberShipDB not found\r\n", pMarket->nAccountDBID);

						if( Market.nRetCode != ERROR_NONE )
						{
							m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
							return;
						}				
					}
#endif

					Market.nRetCode = pWorldDB->QueryBuyTradeItem(pMarket, &Market);
					Market.Item.bEternity = true;
					Market.Item.nLifespan = LIFESPANMAX;
					if (pMarket->bCashItem) Market.Item.bSoulbound = true;

					if (Market.nRetCode != ERROR_NONE)
					{
						switch (Market.nRetCode)
						{
						case 103174: // 이미 팔린 아이템입니다.  
						case 103175: // 등록되어 있지 않은 아이템입니다.  
						case 103325: // 자신이 등록한 아이템을 구입할 수 없습니다.
							break;

						default:
							g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
							break;
						}

#if defined(PRE_ADD_PETALTRADE)
						if( Trade.nPrice > 0 )
						{
							// 여기서 에러나면 수거한 페탈 지급
							int nTotalPetal = 0;
							int nRetCode = ERROR_DB;
							if (pMembershipDB)					
								nRetCode = pMembershipDB->QueryAddPetalIncome(pMarket->biCharacterDBID, 0, Trade.nPrice, nTotalPetal, true);

							if( nRetCode != ERROR_NONE )
								// 여까지 에러나면 대책없음..
								g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM] AddPetal Fail Petal:%d \r\n", pMarket->nAccountDBID, Trade.nPrice);
						}						
#endif
					}
				}
				else{
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETTRADEITEMID:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
					Market.nRetCode = Trade.nRetCode;
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));

#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
			TQBuyTradeItem *pMarket = (TQBuyTradeItem*)pData;

			TABuyTradeItem Market;
			memset(&Market, 0, sizeof(TABuyTradeItem));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			Market.nMarketDBID = pMarket->nMarketDBID;
			Market.cInvenSlotIndex = pMarket->cInvenSlotIndex;
#if defined(PRE_ADD_PETALTRADE)
			// 페탈상품이면 페탈부터 차감..
			if( pMarket->nPetalPrice > 0)
			{
				pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);

				if (pMembershipDB)			
					Market.nRetCode = pMembershipDB->QueryPurchaseTradeItemByPetal(pMarket->biCharacterDBID, pMarket->nPetalPrice);			
				else			
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM] MemberShipDB not found\r\n", pMarket->nAccountDBID);

				if( Market.nRetCode != ERROR_NONE )
				{
					m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
					return;
				}				
			}
#endif
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB)
			{
				Market.nRetCode = pWorldDB->QueryBuyTradeItem(pMarket, &Market);
				Market.Item.bEternity = true;
				Market.Item.nLifespan = LIFESPANMAX;
				if (pMarket->bCashItem) Market.Item.bSoulbound = true;

				if (Market.nRetCode != ERROR_NONE)
				{
					switch (Market.nRetCode)
					{
					case 103174: // 이미 팔린 아이템입니다.  
					case 103175: // 등록되어 있지 않은 아이템입니다.  
					case 103325: // 자신이 등록한 아이템을 구입할 수 없습니다.
						break;

					default:
						g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
						break;
					}

#if defined(PRE_ADD_PETALTRADE)
					if( pMarket->nPetalPrice > 0 )
					{
						// 여기서 에러나면 수거한 페탈 지급
						int nTotalPetal = 0;
						int nRetCode = ERROR_DB;
						if (pMembershipDB)					
							nRetCode = pMembershipDB->QueryAddPetalIncome(pMarket->biCharacterDBID, 0, pMarket->nPetalPrice, nTotalPetal, true);			

						if( nRetCode != ERROR_NONE )
							// 여까지 에러나면 대책없음..
							g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM] AddPetal Fail Petal:%d \r\n", pMarket->nAccountDBID, pMarket->nPetalPrice);
					}					
#endif
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_BUYTRADEITEM:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
		}
		break;

	case QUERY_GETLISTTRADEFORCALCULATION:
		{
			TQGetListTradeForCalculation *pMarket = (TQGetListTradeForCalculation*)pData;

			TAGetListTradeForCalculation Market;
			memset(&Market, 0, sizeof(TAGetListTradeForCalculation));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryGetListTradeForCalculation(pMarket, &Market);

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETLISTTRADEFORCALCULATION:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETLISTTRADEFORCALCULATION:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;

	case QUERY_GETCOUNTTRADEFORCALCULATION:
		{

		}
		break;

	case QUERY_TRADECALCULATE:
		{
			TQTradeCalculate *pMarket = (TQTradeCalculate*)pData;

			TATradeCalculate Market;
			memset(&Market, 0, sizeof(TATradeCalculate));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			Market.nMarketDBID = pMarket->nMarketDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB)
			{
				Market.nRetCode = pWorldDB->QueryTradeCalculate(pMarket, &Market);

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_TRADECALCULATE:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
#if defined(PRE_ADD_PETALTRADE)
				else
				{
					if( Market.cPayMethodCode == DBDNWorldDef::PayMethodCode::Petal)
					{
						//페탈정산이면 페탈값 더해주기.
						Market.nRetCode = ERROR_DB;
						pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
						int nTotalPetal = 0;
						if (pMembershipDB)			
							Market.nRetCode = pMembershipDB->QueryAddPetalIncome(pMarket->biCharacterDBID, 0, Market.nCalculatePrice, nTotalPetal, true);			
						else			
							g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_TRADECALCULATE] MemberShipDB not found\r\n", pMarket->nAccountDBID);
					}
				}
#endif
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_TRADECALCULATE:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;

	case QUERY_TRADECALCULATEALL:
		{
			TQTradeCalculateAll *pMarket = (TQTradeCalculateAll*)pData;

			TATradeCalculateAll Market;
			memset(&Market, 0, sizeof(TATradeCalculateAll));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryTradeCalculateAll(pMarket, &Market);

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_TRADECALCULATEALL:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
#if defined(PRE_ADD_PETALTRADE)
				else
				{
					if( Market.nCalculatePetal > 0) // 페탈 정산이 있으면
					{
						//페탈정산이면 페탈값 더해주기.
						Market.nRetCode = ERROR_DB;
						pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
						int nTotalPetal = 0;
						if (pMembershipDB)
							Market.nRetCode = pMembershipDB->QueryAddPetalIncome(pMarket->biCharacterDBID, 0, Market.nCalculatePetal, nTotalPetal, true);			
						else			
							g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_TRADECALCULATEALL] MemberShipDB not found\r\n", pMarket->nAccountDBID);
					}
				}
#endif
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_TRADECALCULATEALL:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;

#if !defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	case QUERY_GETTRADEITEMID:
		{
			TQGetTradeItemID *pMarket = (TQGetTradeItemID*)pData;

			TAGetTradeItemID Market;
			memset(&Market, 0, sizeof(TAGetTradeItemID));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nMarketDBID = pMarket->nMarketDBID;
			Market.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryGetTradeItemID(pMarket->nMarketDBID, &Market);

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETTRADEITEMID:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETTRADEITEMID:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
		}
		break;
#endif	// #if !defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

#if defined(PRE_ADD_PETALTRADE)
	case QUERY_GETPETALBALANCE :
		{
			TQHeader *pMarket = (TQHeader*)pData;

			TAGetPetalBalance Market;
			memset(&Market, 0, sizeof(TAGetPetalBalance));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB)			
				Market.nRetCode = pMembershipDB->QueryGetPetalBalance(pMarket->nAccountDBID, Market.nPetalBalance );			
			else			
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETPETALBALANCE] MemberShipDB not found\r\n", pMarket->nAccountDBID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));			
		}
		break;
#endif
	case QUERY_GETTRADEPRICE:
		{
			TQGetTradePrice *pMarket = (TQGetTradePrice*)pData;

			TAGetTradePrice Market;
			memset(&Market, 0, sizeof(TAGetTradePrice));
			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nItemID = pMarket->nItemID;
			Market.cLevel = pMarket->cLevel;
			Market.cOption = pMarket->cOption;
			Market.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryGetTradePrice(pMarket, &Market);

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETTRADEPRICE:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETTRADEPRICE:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));		
		}
		break;

	case QUERY_GETLISTMINITRADE:
		{
#if defined( PRE_ADD_DIRECT_BUY_UPGRADEITEM )
			TQGetListMiniTrade *pMarket = (TQGetListMiniTrade*)pData;

			TAGetListMiniTrade Market;
			memset(&Market, 0, sizeof(TAGetListMiniTrade));

			Market.nAccountDBID = pMarket->nAccountDBID;
			Market.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pMarket->cWorldSetID);
			if (pWorldDB){
				Market.nRetCode = pWorldDB->QueryGetListMiniTrade(pMarket, &Market);
				Market.nMarketTotalCount = Market.cMarketCount;

				if (Market.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETLISTMINITRADE:%d] Result:%d\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID, Market.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pMarket->cWorldSetID, pMarket->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETLISTMINITRADE:%d] pWorldDB not found\r\n", pMarket->nAccountDBID, pMarket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Market, sizeof(Market));
#endif

		}
		break;
	}
}
