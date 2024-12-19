#pragma once

#include "DnCustomDlg.h"
#include "DnCashShopDefine.h"
#include "DnItem.h"

class CDnCashShopRecvGiftDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		MESSAGEBOX_GIFT_CONFIRM,
	};

	struct SCashShopGiftInfoUnit
	{
		INT64				giftDBID;
		CEtUIStatic*		pSender;
		CEtUIStatic*		pRecvDate;
		CEtUIStatic*		pExpireDate;
		CEtUIStatic*		pSelected;

		SCashShopGiftInfoUnit()
		{
			giftDBID = -1;
			pSender = pRecvDate = pExpireDate = pSelected = NULL;
		}

		void Clear()
		{
			giftDBID = -1;
			pSender->ClearText();
			pRecvDate->ClearText();
			pExpireDate->ClearText();
			pSelected->Show(false);
		}

		void Show(bool bShow)
		{
			pSender->Show(bShow);
			pRecvDate->Show(bShow);
			pExpireDate->Show(bShow);
		}

		bool IsInside(float fx, float fy) const
		{
			if (pSender == NULL || pRecvDate == NULL || pExpireDate == NULL)
				return false;
			SUICoord coord, from, to;
			pSender->GetUICoord(from);
			pExpireDate->GetUICoord(to);
			coord.fX = from.fX;
			coord.fY = from.fY;
			coord.fWidth = (to.fX + to.fWidth) - from.fX;
			coord.fHeight = from.fHeight;

			return coord.IsInside(fx, fy);
		}

		bool IsEmpty() const
		{
			const std::wstring& str = pSender->GetText();
			return str.empty();
		}

		void Set(const SCashShopRecvGiftBasicInfo& info);
	};

	struct SCashShopGiftSlotUnit
	{
		CDnItem*			pItem;
		CEtUIStatic*		pBase;
		CEtUIStatic*		pName;
		CDnItemSlotButton*	pSlotBtn;
		CEtUIStatic*		pAbility;
		CEtUIStatic*		pPeriod;
		CEtUIStatic*		pPrice;
		CEtUIStatic*		pCount;

		SCashShopGiftSlotUnit()
		{
			pItem = NULL;
			pBase = pName = pAbility = pPeriod = pPrice = pCount = NULL;
			pSlotBtn = NULL;
		}

		~SCashShopGiftSlotUnit()
		{
			SAFE_DELETE(pItem);
		}

		void Clear()
		{
			pName->ClearText();
			pSlotBtn->ResetSlot();
			pAbility->ClearText();
			pPeriod->ClearText();
			pPrice->ClearText();
			pCount->ClearText();
		}

		void Show(bool bShow)
		{
			pBase->Show(bShow);
			pName->Show(bShow);
			pSlotBtn->Show(bShow);
			pAbility->Show(bShow);
			pPeriod->Show(bShow);
			pPrice->Show(bShow);
			pCount->Show(bShow);
		}

		void Set(CASHITEM_SN sn, ITEMCLSID itemId);
	};
public:
	CDnCashShopRecvGiftDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopRecvGiftDlg() {}

	virtual void	Initialize(bool bShow);
	virtual void	Show(bool bShow);
	void			UpdateInfo();
	void			Clear();
	// CEtUICallback
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

protected:
	virtual void	InitCustomControl(CEtUIControl *pControl);
	virtual void	InitialUpdate();
	virtual void	Process(float fElapsedTime);
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual bool	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void			UpdatePageControllers(const RECV_GIFT_LIST& itemList);
	void			UpdatePage();

	void			PrevPage();
	void			NextPage();

	void			SetReactionUI(bool bEnable);

#if defined(PRE_ADD_GIFT_RETURN)
	bool			IsNonAccept(int nPayMethodCode);
#endif

	std::vector<SCashShopGiftInfoUnit>	m_SenderInfoList;
	SCashShopGiftSlotUnit				m_SlotUnit;

	CEtUIButton*		m_pPrevBtn;
	CEtUIButton*		m_pNextBtn;
	CEtUIStatic*		m_pPageNum;

	CEtUIStatic*		m_pSenderMemoName;
	CEtUITextBox*		m_pSenderMemoBox;

	CEtUIButton*		m_pRecvBtn;
	//CEtUIButton*		m_pDelBtn;
	CEtUIButton*		m_pCloseBtn;
#if defined(PRE_ADD_GIFT_RETURN)
	CEtUIButton*		m_pNonAcceptBtn;
#endif

	CEtUIIMEEditBox*		m_pReplyEditBox;
	CEtUIRadioButton*		m_pImpressionBtn[MAX_GIFT_IMPRESSION_RADIO_BTN];
	CEtUIStatic*			m_pImpressionIcon[MAX_GIFT_IMPRESSION_RADIO_BTN];
	CEtUIStatic*			m_pInfoBaseBoard;
	int						m_CurrentImprsnId;

	CEtUIStatic*			m_pReactStatic[3];
	CEtUIStatic*			m_pReactTitle;
	CEtUIStatic*			m_pReplyTitle;
	CEtUIStatic*			m_pReplyBackGround;

#ifdef PRE_RECEIVEGIFTALL
	CEtUIButton*			m_pButtonTakeAll;
#endif 

	float				m_RefreshServerInfoTimer;
	bool				m_bRequesting;
	int					m_CurrentPage;
	int					m_MaxPage;

	std::map<int, int>	m_DataSlotIdxMatchList;	// key : slot index

	int					m_IndexSelected;
	
};