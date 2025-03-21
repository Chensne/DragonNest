#pragma once
#include "DNPacket.h"
#define SHORT_RETNAME_PACKET( cls ) bool CheckValidPacket( cls *pPacket ) { return CheckValidPacketShort(pPacket); } 
class CDnMarketTabDlg;

class CDnTradeMarket
{
public:
	CDnTradeMarket(void);
	virtual ~CDnTradeMarket(void);

protected:
	CDnMarketTabDlg *m_pMarketDialog;
	bool m_bRequestWait;

public:
	void SetMarketDialog( CDnMarketTabDlg *pDialog ) { m_pMarketDialog = pDialog; }

	void ServerMessageBox( int nRetCode );
	
	template <class T>
	bool CheckValidPacket( T *pPacket ) {
		if( !pPacket ) {
			ASSERT( pPacket );
			return false;
		}
		if( pPacket->nRetCode != ERROR_NONE ) {
			ServerMessageBox( pPacket->nRetCode );
			return false;
		}
		return true;
	}

	SHORT_RETNAME_PACKET( SCMarketResult );
	SHORT_RETNAME_PACKET( SCMarketCalculationAll );
	template <class T>
	bool CheckValidPacketShort( T *pPacket ) {
		if( !pPacket ) {
			ASSERT( pPacket );
			return false;
		}
		if( pPacket->nRet != ERROR_NONE ) {
			ServerMessageBox( pPacket->nRet );
			return false;
		}
		return true;
	}

public:
	void RequestMarketBuyList( int nPageNum, char cMinLevel, char cMaxLevel, char *cItemGradeArray, char cJob, short nMainType, char cDetailType, WCHAR *pwszSearch1, WCHAR *pwszSearch2, WCHAR *pwszSearch3, char cSortType, int nExchangeItemID, bool bLowJobGroup, WCHAR *pwszSearch, char cPayMethodCode );
	void RequestMarketSellList();
	void RequestMarketCalculationList();

	void RequestMarketBuy( int nMarketDBID );
	void RequestMarketRegister( INT64 biInvenSerial, short wCount, int nPrice, char cPeriodIndex, char cInvenType, BYTE cInvenIndex, bool bPremium, char cPayMethodCode );
	void RequestMarketInterrupt( int nMarketDBID );
	void RequestMarketCalculation( int nMarketDBID );
	void RequestMarketCalculationAll();
	void RequestMarketPetalBalance();
	void RequestMarketPrice( int nMarketDBID, int nItemID, BYTE cLevel, BYTE cOption );

public:
	void OnRecvMarketBuyList( SCMarketList *pPacket );
	void OnRecvMarketSellList( SCMyMarketList *pPacket );
	void OnRecvMarketCalculationList( SCMarketCalculationList *pPacket );

	void OnRecvMarketBuy( SCMarketBuyResult *pPacket );
	void OnRecvMarketCalculation( SCMarketResult *pPacket );
	void OnRecvMarketCalculationAll( SCMarketCalculationAll *pPacket );
	void OnRecvMarketRegist( SCMarketResult *pPacket );
	void OnRecvMarketInterrupt( SCMarketInterrupt *pPacket );
	void OnRecvMarketCalcNotify( SCNotifyMarket *pPacket );
	void OnRecvMarketPetalBalance( SCMarketPetalBalance * pPacket );
	void OnRecvMarketPrice( SCMarketPrice * pPacket );
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	void OnRecvMarketMiniList( SCMarketMiniList *pPacket );
	void RequestMarketMiniBuy( int nMarketDBID );
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
};
