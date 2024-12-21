#include "stdafx.h"
#include "DnCashShopDlg.h"
#include "DnCashShopSendGiftDlg.h"
#include "DnCashShopRecvGiftDlg.h"
#include "DnCashShopGiftReplyDlg.h"
#include "DnCashShopDefine.h"
#include "DnCashShopGiftAnswerDlg.h"
#ifdef PRE_ADD_CADGE_CASH
#include "DnCashShopCadgeDlg.h"
#endif // PRE_ADD_CADGE_CASH


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopCommonDlgMgr::CDnCashShopCommonDlgMgr()
{
	m_pSendGiftDlg	= NULL;
	m_pRecvGiftDlg	= NULL;
	m_pPayDlg		= NULL;
	m_pGiftReplyDlg	= NULL;
	m_pGiftAnswerDlg = NULL;
#ifdef PRE_ADD_CADGE_CASH
	m_pCadgeDlg		= NULL;
#endif // PRE_ADD_CADGE_CASH
}

CDnCashShopCommonDlgMgr::~CDnCashShopCommonDlgMgr()
{
	DeleteAllDlg();
}

void CDnCashShopCommonDlgMgr::DeleteAllDlg()
{
	SAFE_DELETE(m_pSendGiftDlg);
	SAFE_DELETE(m_pRecvGiftDlg);
	SAFE_DELETE(m_pPayDlg);
	SAFE_DELETE(m_pGiftReplyDlg);
	SAFE_DELETE(m_pGiftAnswerDlg);
#ifdef PRE_ADD_CADGE_CASH
	SAFE_DELETE(m_pCadgeDlg);
#endif // PRE_ADD_CADGE_CASH
}

void CDnCashShopCommonDlgMgr::InitializeDlgs(CDnCashShopDlg* pParent)
{
	DeleteAllDlg();

	m_pSendGiftDlg = new CDnCashShopSendGiftDlg(UI_TYPE_MODAL, NULL, SEND_GIFT_DLG, pParent);
	m_pSendGiftDlg->Initialize(false);

	m_pRecvGiftDlg = new CDnCashShopRecvGiftDlg(UI_TYPE_MODAL, NULL, RECV_GIFT_DLG, pParent);
	m_pRecvGiftDlg->Initialize(false);

	m_pPayDlg = new CDnCashShopPayDlg(UI_TYPE_MODAL, NULL, PAY_DLG, pParent);
#ifdef PRE_ADD_CADGE_CASH
	m_pPayDlg->Initialize(false, this);
#else // PRE_ADD_CADGE_CASH
	m_pPayDlg->Initialize(false);
#endif // PRE_ADD_CADGE_CASH

	m_pGiftReplyDlg = new CDnCashShopGiftReplyDlg(UI_TYPE_MODAL, NULL, RECV_GIFT_REPLY_DLG, pParent);
	m_pGiftReplyDlg->Initialize(false);

	m_pGiftAnswerDlg = new CDnCashShopGiftAnswerDlg(UI_TYPE_MODAL, NULL, RECV_GIFT_ANSWER_DLG, pParent);
	m_pGiftAnswerDlg->Initialize(false);

#ifdef PRE_ADD_CADGE_CASH
	m_pCadgeDlg = new CDnCashShopCadgeDlg(UI_TYPE_MODAL, NULL, SEND_CADGE_DLG, pParent);
	m_pCadgeDlg->Initialize(false);
#endif // PRE_ADD_CADGE_CASH
}

void CDnCashShopCommonDlgMgr::OpenSendGiftDlg(eCashUnitType type, bool bClear)
{
	m_pSendGiftDlg->SetInfo(type, bClear);
	m_pSendGiftDlg->Show(true);
}

void CDnCashShopCommonDlgMgr::ShowRecvGiftDlg(bool bShow)
{
	m_pRecvGiftDlg->Show(bShow);
}

void CDnCashShopCommonDlgMgr::OpenPayDlg(eCashUnitType type, bool bClear)
{
	m_pPayDlg->Clear(bClear);
	m_pPayDlg->Update(type);
	m_pPayDlg->Show(true);
}

void CDnCashShopCommonDlgMgr::ClosePayDlg()
{
	m_pPayDlg->Show(false);
}

bool CDnCashShopCommonDlgMgr::IsOpenDlg(CashShopCommonDlgType type) const
{
	switch(type)
	{
	case PAY_DLG:		return m_pPayDlg->IsShow();
	case RECV_GIFT_DLG: return m_pRecvGiftDlg->IsShow();
	default: _ASSERT(0);
	}

	return false;
}

void CDnCashShopCommonDlgMgr::UpdateGiftReceiverInfo(const SCCashShopCheckReceiver& info)
{
	if( m_pSendGiftDlg )
		m_pSendGiftDlg->UpdateGiftReceiverInfo( info );
#ifdef PRE_ADD_CADGE_CASH
	if( m_pCadgeDlg )
		m_pCadgeDlg->UpdateCadgeReceiverInfo( info );
#endif // PRE_ADD_CADGE_CASH
}

#ifdef PRE_ADD_CADGE_CASH

void CDnCashShopCommonDlgMgr::SetCadgeReciever( std::wstring strReciever )
{
	if( m_pSendGiftDlg )
		m_pSendGiftDlg->SetCadgeReciever( strReciever );
}

void CDnCashShopCommonDlgMgr::OpenCadgeDlg( eCashUnitType type )
{
	if( m_pCadgeDlg )
	{
		m_pCadgeDlg->Show( true );
		m_pCadgeDlg->SetCashUnitType( type );
	}
}

void CDnCashShopCommonDlgMgr::UpdateCadgeReceiverInfo(const SCCashShopCheckReceiver& info)
{
	if( m_pCadgeDlg )
		m_pCadgeDlg->UpdateCadgeReceiverInfo(info);
}

#endif // PRE_ADD_CADGE_CASH

void CDnCashShopCommonDlgMgr::UpdateRecvGiftList()
{
	m_pRecvGiftDlg->Clear();
	m_pRecvGiftDlg->UpdateInfo();
}

void CDnCashShopCommonDlgMgr::OpenGiftReplyDlg(const WCHAR* pSender)
{
	m_pGiftReplyDlg->SetInfo(pSender);
	m_pGiftReplyDlg->Show(true);
}

void CDnCashShopCommonDlgMgr::CloseGiftReplyDlg()
{
	m_pGiftReplyDlg->Show(false);
}
