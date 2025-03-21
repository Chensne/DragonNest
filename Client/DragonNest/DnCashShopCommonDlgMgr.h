#pragma once

#include "DnCashShopDefine.h"
#include "DnCashShopPayDlg.h"

class CDnCashShopDlg;
class CDnCashShopSendGiftDlg;
class CDnCashShopRecvGiftDlg;
class CDnCashShopGiftReplyDlg;
class CDnCashShopGiftAnswerDlg;
#ifdef PRE_ADD_CADGE_CASH
class CDnCashShopCadgeDlg;
#endif // PRE_ADD_CADGE_CASH


class CDnCashShopCommonDlgMgr
{
public:
	enum CashShopCommonDlgType
	{
		SEND_GIFT_DLG,
		RECV_GIFT_DLG,
		PAY_DLG,
		RECV_GIFT_REPLY_DLG,
		RECV_GIFT_ANSWER_DLG,
#ifdef PRE_ADD_CADGE_CASH
		SEND_CADGE_DLG,
#endif // PRE_ADD_CADGE_CASH
	};

	CDnCashShopCommonDlgMgr();
	virtual ~CDnCashShopCommonDlgMgr();

	void	InitializeDlgs(CDnCashShopDlg* pParent);

	void	OpenSendGiftDlg(eCashUnitType type, bool bClear);
	void	ShowRecvGiftDlg(bool bShow);

	void	OpenPayDlg(eCashUnitType type, bool bClear);
	void	ClosePayDlg();
	bool	IsOpenDlg(CashShopCommonDlgType type) const;

	void	OpenGiftReplyDlg(const WCHAR* pSender);
	void	CloseGiftReplyDlg();

	//void	SetGiftAnswer(const TReplyInfo& reply);

	void	UpdateGiftReceiverInfo(const SCCashShopCheckReceiver& info);
	void	UpdateRecvGiftList();

	void	Process(float fElapsedTime);

#ifdef PRE_ADD_CADGE_CASH
	void SetCadgeReciever( std::wstring strReciever );
	void OpenCadgeDlg( eCashUnitType type );
	void UpdateCadgeReceiverInfo(const SCCashShopCheckReceiver& info);
#endif // PRE_ADD_CADGE_CASH

#ifdef PRE_RECEIVEGIFTALL
	CDnCashShopRecvGiftDlg* GetRecvGiftDlg() { return m_pRecvGiftDlg; }
#endif // PRE_RECEIVEGIFTALL

private:
	void	DeleteAllDlg();

	CDnCashShopSendGiftDlg*		m_pSendGiftDlg;
	CDnCashShopRecvGiftDlg*		m_pRecvGiftDlg;
	CDnCashShopPayDlg*			m_pPayDlg;
	CDnCashShopGiftReplyDlg*	m_pGiftReplyDlg;
	CDnCashShopGiftAnswerDlg*	m_pGiftAnswerDlg;
#ifdef PRE_ADD_CADGE_CASH
	CDnCashShopCadgeDlg*		m_pCadgeDlg;
#endif // PRE_ADD_CADGE_CASH
	//std::deque<TReplyInfo>		m_ReplyInfoList;
};
