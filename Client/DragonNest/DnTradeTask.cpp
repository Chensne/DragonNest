#include "StdAfx.h"
#include "DnTradeTask.h"
#ifdef PRE_ADD_ONESTOP_TRADECHECK
#include "DnSlotButton.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTradeTask::CDnTradeTask(void) : CTaskListener(true)
{
}

CDnTradeTask::~CDnTradeTask(void)
{
	Finalize();
}

bool CDnTradeTask::Initialize()
{
	if( !m_TradeItem.Initialize() )
		return false;

	if (!m_TradeMail.Initialize())
		return false;

	return true;
}

void CDnTradeTask::Finalize()
{
	m_TradeItem.Finalize();
	m_TradeMail.Finalize();
}

void CDnTradeTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	m_TradePrivateMarket.Process( fDelta );
}

void CDnTradeTask::OnDisconnectTcp( bool bValidDisconnect )
{
}

void CDnTradeTask::OnDisconnectUdp()
{
}

void CDnTradeTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	bool bProcessDispatch = false;

	switch( nMainCmd ) 
	{
		case SC_TRADE:	OnRecvTradeMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;	break;
	}
	if( bProcessDispatch ) {
		WriteLog( 1, ", Info, CDnTradeTask::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}
}

void CDnTradeTask::OnRecvTradeMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd )
	{
	case eTrade::eSCTrade::SC_SHOP_OPEN:	m_TradeItem.OnRecvShopOpen( (SCShopOpen*)pData );		break;
	case eTrade::eSCTrade::SC_SHOP_BUY:		m_TradeItem.OnRecvShopBuy( (SCShopBuyResult*)pData );	break;
	case eTrade::eSCTrade::SC_SHOP_SELL:	m_TradeItem.OnRecvShopSell( (SCShopSellResult*)pData );	break;
	case eTrade::eSCTrade::SC_SHOP_GETLIST_REPURCHASE: m_TradeItem.OnRecvShopGetRepurchaseList( (SCShopRepurchaseList*)pData ); break;
	case eTrade::eSCTrade::SC_SHOP_REPURCHASE: m_TradeItem.OnRecvShopRepurchase( (SCShopRepurchase*)pData ); break;
	case eTrade::eSCTrade::SC_SKILLSHOP_OPEN:	m_TradeSkill.OnRecvOpenSkillShop( (SCSkillShopOpen*)pData );		break;
	case eTrade::eSCTrade::SC_MAILBOX:				m_TradeMail.OnRecvMailBox( (SCMailBox*)pData );		break;
	case eTrade::eSCTrade::SC_MAIL_SEND:			m_TradeMail.OnRecvMailSend( (SCSendMail*)pData );			break;
	case eTrade::eSCTrade::SC_MAIL_READ:			m_TradeMail.OnRecvMailRead( (SCReadMail*)pData );			break;
#ifdef PRE_ADD_CADGE_CASH
	case eTrade::eSCTrade::SC_MAIL_READCADGE:		m_TradeMail.OnRecvCadgeMailRead( (SCReadCadgeMail*)pData );			break;
#endif // PRE_ADD_CADGE_CASH
	case eTrade::eSCTrade::SC_MAIL_DELETE:			m_TradeMail.OnRecvMailDelete( (SCMailResult*)pData );		break;
	case eTrade::eSCTrade::SC_MAIL_ATTACHALL:		m_TradeMail.OnRecvMailAttachAll( (SCAttachAllMailResult*)pData );	break;
	case eTrade::eSCTrade::SC_MAIL_ATTACH:			m_TradeMail.OnRecvMailAttach( (SCMailResult*)pData );	break;
	case eTrade::eSCTrade::SC_MAIL_NOTIFY:			m_TradeMail.OnRecvMailNotify( (SCNotifyMail*)pData );		break;

	case eTrade::eSCTrade::SC_EXCHANGE_REQUEST:		m_TradePrivateMarket.OnRecvPrivateMarket( (SCExchangeRequest*)pData );			break;
	case eTrade::eSCTrade::SC_EXCHANGE_START:		m_TradePrivateMarket.OnRecvPrivateMarketStart( (SCExchangeStart*)pData );		break;
	case eTrade::eSCTrade::SC_EXCHANGE_ADDITEM:		m_TradePrivateMarket.OnRecvPrivateMarketAddItem( (SCExchangeAddItem*)pData );	break;
	case eTrade::eSCTrade::SC_EXCHANGE_DELETEITEM:	m_TradePrivateMarket.OnRecvPrivateMarketDeleteItem( (SCExchangeDeleteItem*)pData );	break;
	case eTrade::eSCTrade::SC_EXCHANGE_ADDCOIN:		m_TradePrivateMarket.OnRecvPrivateMarketCoin( (SCExchangeAddCoin*)pData );		break;
	case eTrade::eSCTrade::SC_EXCHANGE_CONFIRM:		m_TradePrivateMarket.OnRecvPrivateMarketConfirm( (SCExchangeConfirm*)pData );	break;
	case eTrade::eSCTrade::SC_EXCHANGE_CANCEL:		m_TradePrivateMarket.OnRecvPrivateMarketCancel();								break;
	case eTrade::eSCTrade::SC_EXCHANGE_COMPLETE:	m_TradePrivateMarket.OnRecvPrivateMarketComplete( (SCExchangeComplete*)pData );	break;
	case eTrade::eSCTrade::SC_EXCHANGE_REJECT:		m_TradePrivateMarket.OnRecvPrivateMarketReject( (SCExchangeReject*)pData );		break;

	case eTrade::SC_MARKETLIST:		m_TradeMarket.OnRecvMarketBuyList( (SCMarketList*)pData) ;  break;	
	case eTrade::SC_MYMARKETLIST:	m_TradeMarket.OnRecvMarketSellList(  (SCMyMarketList*)pData ); break;
	case eTrade::SC_MARKET_CALCULATIONLIST: m_TradeMarket.OnRecvMarketCalculationList( (SCMarketCalculationList*)pData ); break;

	case eTrade::SC_MARKET_REGISTER: m_TradeMarket.OnRecvMarketRegist( (SCMarketResult*)pData );  break;	
	case eTrade::SC_MARKET_INTERRUPT: m_TradeMarket.OnRecvMarketInterrupt( (SCMarketInterrupt*)pData ); break;
	case eTrade::SC_MARKET_BUY: m_TradeMarket.OnRecvMarketBuy( (SCMarketBuyResult*)pData ); break;	
	case eTrade::SC_MARKET_CALCULATION: m_TradeMarket.OnRecvMarketCalculation( (SCMarketResult*)pData ); break;
	case eTrade::SC_MARKET_CALCULATIONALL: m_TradeMarket.OnRecvMarketCalculationAll( (SCMarketCalculationAll*)pData ); break;	
	case eTrade::SC_MARKET_NOTIFY: m_TradeMarket.OnRecvMarketCalcNotify( (SCNotifyMarket*)pData ); break;
	case eTrade::SC_MARKET_PETALBALANCE: m_TradeMarket.OnRecvMarketPetalBalance( (SCMarketPetalBalance*)pData ); break;
	case eTrade::SC_MARKET_PRICE: m_TradeMarket.OnRecvMarketPrice( (SCMarketPrice*)pData ); break;
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	case eTrade::SC_MARKETMINILIST: m_TradeMarket.OnRecvMarketMiniList( (SCMarketMiniList*)pData) ;  break;	
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
#if defined(PRE_SPECIALBOX)
	case eTrade::SC_SPECIALBOX_LIST: m_SpecialBox.OnRecvSpecialBoxList( (SCSpecialBoxList*)pData ); break;
	case eTrade::SC_SPECIALBOX_ITEMLIST: m_SpecialBox.OnRecvSpecialBoxItemList( (SCSpecialBoxItemList*)pData ); break;
	case eTrade::SC_SPECIALBOX_RECEIVEITEM: m_SpecialBox.OnRecvSpecialBoxReceiveItem( (SCReceiveSpecialBoxItem*)pData ); break;
	case eTrade::SC_SPECIALBOX_NOTIFY: m_SpecialBox.OnRecvSpecialBoxNotify( (SCNotifySpecialBox*)pData ); break;
#endif	// #if defined(PRE_SPECIALBOX)
	}
}

#ifdef PRE_ADD_ONESTOP_TRADECHECK
bool CDnTradeTask::IsTradable(eTradeCheckType type, const CDnSlotButton& targetSlot) const
{
	if (targetSlot.IsEmptySlot())
		return false;

	MIInventoryItem::InvenItemTypeEnum itemType = targetSlot.GetItemType();
	if (itemType == MIInventoryItem::Item)
	{
		const CDnItem* pItem = static_cast<const CDnItem*>(targetSlot.GetItem());
		if (pItem == NULL)
			return false;

		return IsTradable(type, *pItem);
	}

	return true;
}

bool CDnTradeTask::IsTradable(eTradeCheckType type, const CDnItem& targetItem) const
{
	if (targetItem.GetLookItemID() != ITEMCLSID_NONE)
		return false;
	return true;
}
#endif