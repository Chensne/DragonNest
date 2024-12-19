#pragma once

#include "DnCustomDlg.h"
#include "DnTradeTask.h"
#include "DnTradeMail.h"
#include "DnCommonUtil.h"

class CDnItemSlotButton;
class CEtUIStatic;
class CEtUITextBox;
class CEtUIButton;

class CDnMailReadDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	enum 
	{
		MESSAGEBOX_DEL_CONFIRM
	};

	enum eAttachSlotType
	{
		eSLOT_PREMIUM,
		eSLOT_NORMAL,
	};

	CDnMailReadDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL);
	virtual ~CDnMailReadDlg() {}

	virtual void	Initialize(bool bShow);
	virtual void	InitialUpdate();

	virtual void	Show(bool bShow);
	void			OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg);
	void			Update(const CDnTradeMail::SReadMailInfo& info);
	bool			IsUpdated() const;
	int				GetMailID() const;
	void			Clear();
	void			ClearAttach();
	int				RemoveAttachItem(INT64 attachItemSerial);
	int				GetAttachItemSlotIndex(INT64 serial) const;

	void			EnableButtons(bool bEnable);

	void			Lock()				{ m_bLock = true;	EnableButtons(false); }
	void			UnLock()			{ m_bLock = false;	EnableButtons(true); }
	bool			IsLocked() const	{ return m_bLock; }

protected:
	void			Process( float fElapsedTime );
	void			ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);

private:
	void			SetCoin(const INT64& amount);
	int				GetMailIndex() const;
	bool			IsAttachEmpty() const;
	void			SetPremium(bool bSet);
	void			ShowAttachSlot(eAttachSlotType type);
	bool			IsNeedSingleSlot(bool bPremium) const;

	CEtUIStatic*	m_pStaticName;
	CEtUIStatic*	m_pStaticTitle;
	CEtUIStatic*	m_pStaticSendDay;

	CEtUIStatic*	m_pNormalLetterPaper;
	CEtUIStatic*	m_pPremiumLetterPaper;
	CEtUIStatic*	m_pNoticeLetterPaper;
#ifdef PRE_ADD_CADGE_CASH
	CEtUIStatic*	m_pCadgeLetterPaper;
	CEtUIButton*	m_pBuyButton;
#endif // PRE_ADD_CADGE_CASH

	std::vector<CEtUIStatic*>			m_AttachSlotBGList;
	eAttachSlotType						m_AttachSlotTypes[MAILATTACHITEMMAX];

	CEtUITextBox*	m_pTextBoxText;

	CEtUIStatic*	m_pCoinG;
	CEtUIStatic*	m_pCoinS;
	CEtUIStatic*	m_pCoinB;

	CEtUIButton*	m_pButtonGetAttach;
	CEtUIButton*	m_pButtonReply;
	CEtUIButton*	m_pButtonDelete;

	std::vector<CDnItemSlotButton*> m_ItemSlotList;

	//int					m_CurMailID;
	CommonUtil::MONEY	m_MoneyCache;
	bool				m_bLock;

	MailType::Kind		m_MailKind;
};