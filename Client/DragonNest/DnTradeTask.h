#pragma once
#include "Task.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"
#include "DnTradeItem.h"
#include "DnTradeSkill.h"
#include "DnTradeMail.h"
#include "DnTradeMarket.h"
#include "DnTradePrivateMarket.h"
#include "DnSpecialBox.h"

class CDnTradeTask : public CTask, public CTaskListener, public CSingleton<CDnTradeTask>
{
public:
#ifdef PRE_ADD_ONESTOP_TRADECHECK
	enum eTradeCheckType
	{
		eTRADECHECK_PRIVATE,
		eTRADECHECK_MAIL,
		eTRADECHECK_MARKET
	};
#endif

	CDnTradeTask(void);
	virtual ~CDnTradeTask(void);

protected:
	CDnTradeItem m_TradeItem;
	CDnTradeSkill m_TradeSkill;
	CDnTradeMail m_TradeMail;
	CDnTradeMarket m_TradeMarket;
	CDnTradePrivateMarket m_TradePrivateMarket;
#ifdef PRE_SPECIALBOX
	CDnSpecialBox m_SpecialBox;
#endif

public:
	CDnTradeItem& GetTradeItem()					{ return m_TradeItem; }
	CDnTradeSkill& GetTradeSkill()					{ return m_TradeSkill; }
	CDnTradeMail& GetTradeMail()					{ return m_TradeMail; }
	CDnTradeMarket& GetTradeMarket()				{ return m_TradeMarket; }
	CDnTradePrivateMarket& GetTradePrivateMarket()	{ return m_TradePrivateMarket; }

#ifdef PRE_ADD_ONESTOP_TRADECHECK
	bool IsTradable(eTradeCheckType type, const CDnSlotButton& targetSlot) const;
	bool IsTradable(eTradeCheckType type, const CDnItem& targetItem) const;
#endif

public:
	bool Initialize();
	void Finalize();

protected:
	void OnRecvTradeMessage( int nSubCmd, char *pData, int nSize );

public:
	virtual void OnDisconnectTcp( bool bValidDisconnect );
	virtual void OnDisconnectUdp();

	// ClientSession
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};

#define GetTradeTask()	CDnTradeTask::GetInstance()