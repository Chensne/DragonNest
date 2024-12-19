#pragma once
#include "DnCustomDlg.h"

#include "DnTradeMail.h"
#include "DnCommonUtil.h"
#ifdef PRE_PARTY_DB
#include "DnTextAutoCompleteUIMgr.h"
#endif

class CEtUIButton;
class CEtUIIMEEditBox;
class CEtUILineIMEEditBox;
class CDnMoneyInputDlg;
class CEtUIStatic;
class CEtUICheckBox;
class CDnItemSlotButton;
class CDnItem;
class CDnMailSendConfirmDlg;
class CDnStoreConfirmExDlg;

class CDnMailWriteDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		MONEY_INPUT_DIALOG,
		SEND_CONFIRM_DIALOG,
		ITEM_ATTACH_CONFIRM_DIALOG,
		ERROR_NO_NAME_MSGBOX,
		ERROR_NO_TITLE_MSGBOX,
		ERROR_CANNOT_SEND_SELF,
	};

	enum eAttachType
	{
		ATTACHTYPE_DRAG,
		ATTACHTYPE_AUTO
	};

	enum eAttachSlotType
	{
		eSLOT_PREMIUM,
		eSLOT_NORMAL,
	};

public:
	CDnMailWriteDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnMailWriteDlg();

	virtual void		Initialize(bool bShow);
	virtual void		InitialUpdate();

	virtual void		Show(bool bShow);
	void				ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	void				Process( float fElapsedTime );
	void				OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	bool				MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void				SetName(const wchar_t* szName);
	void				SetTitle(const wchar_t* szTitle);
	void				Clear(bool bAttachCancel);
	bool				AddAutoAttachItem(CDnSlotButton* pFromSlot, bool bItemSplit);
	bool				AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount);

	void				GetEmptyAttachSlot(CDnItemSlotButton** ppEmptySlot);
	bool				IsAttachableItem(CDnItem* pItem) const;

	bool				IsEnablePremiumMail() const;
	void				CheckPremiumMail();

	void				Lock()				{ m_bLock = true; EnableButtons(false); }
	void				UnLock()			{ m_bLock = false; EnableButtons(true); }
	bool				IsLocked() const	{ return m_bLock; }

private:
	void				SetTax(const TAX_TYPE& amount);
	void				SetCoin(const INT64& amount);
	void				SetFriendComboBox();
	MONEY_TYPE			CalcAttachAmount() const;
	void				OnAddEditBoxString(const std::wstring& strName);

	bool				IsDuplicateAttachItem(INT64 serialItemId) const;
	bool				IsAnyAttach() const;

	int					GetSlotIndex(const CDnItemSlotButton* pSlot) const;

	bool				RestoreFromSlot(int idx, bool bAttachCancel);
	bool				RestoreFromSlot(CDnItemSlotButton* pAttachSlot, bool bAttachCancel);
	bool				RemoveAttachItem(CDnItemSlotButton* pAttachSlot, bool bAttachCancel);
	bool				RemoveAttachItem_All(bool bAttachCancel);

	void				ShowAttachSlot(eAttachSlotType type);
	void				SetDaySendEnableCount(bool bPremium);
	void				ShowLetterPaper(bool bPremium);
	void				SetPremium(bool bPremium);

	void				EnableButtons(bool bEnable);
	TAX_TYPE			CalcTax();
	void				CopyMailContentsToBuffer(LPWSTR pBuffer, UINT bufferCount);

	CDnMailSendConfirmDlg*				m_pConfirmDlg;

	CEtUIButton*						m_pButtonSend;
	CEtUIButton*						m_pButtonMoneyInput;
	CEtUIButton*						m_pButtonMoneyDel;

	CEtUIIMEEditBox*					m_pEditBoxName;
	CEtUIComboBox*						m_pFriendComboBox;
	CEtUIIMEEditBox*					m_pEditBoxTitle;
	CEtUILineIMEEditBox*				m_pLineEditBox;
	CEtUILineIMEEditBox*				m_pPremiumLineEditBox;

	CDnMoneyInputDlg*					m_pMoneyInputDlg;

	CEtUIStatic*						m_pCoinG;
	CEtUIStatic*						m_pCoinS;
	CEtUIStatic*						m_pCoinB;

	CEtUIStatic*						m_pCoinTaxG;
	CEtUIStatic*						m_pCoinTaxS;
	CEtUIStatic*						m_pCoinTaxB;

	std::vector<CEtUIStatic*>			m_AttachSlotBGList;
	CEtUICheckBox*						m_PremiumBtn;
	CEtUIStatic*						m_PremiumStatic;
	CEtUIStatic*						m_PremiumNoteStatic;
	eAttachSlotType						m_AttachSlotTypes[MAILATTACHITEMMAX];

	CEtUIStatic*						m_pEnableSendCountIcon;
	CEtUIStatic*						m_pEnableSendCountStatic;
	CEtUIStatic*						m_pNormalLetterPaper;
	CEtUIStatic*						m_pPremiumLetterPaper;
	std::vector<CDnItem*>				m_nPremiumLetterItemCache;

	std::vector<CDnItemSlotButton*>		m_ItemSlotList;
	CommonUtil::MONEY					m_MoneyCache;
	CommonUtil::TAX						m_TaxCache;
	std::wstring						m_AutoCompleteTextCache;

	// 교환창에 올린 아이템들이 인벤창에서 비활성화되어 보이도록 기억해둔다.
	std::vector<CDnSlotButton*>			m_vecSlotButton;
	bool								m_bLock;

	CDnStoreConfirmExDlg*				m_pSplitConfirmExDlg;		// Note : 중첩아이템 확인창(개수 입력)

#ifdef PRE_PARTY_DB
	CDnTextAutoCompleteUIMgr			m_AutoCompleteMgr;
#else
	std::multimap<wchar_t, std::wstring>	m_FriendNameList;
#endif
};