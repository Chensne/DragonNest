#pragma once

#include "EtUIDialog.h"
#include "DnCashShopDefine.h"

class CDnCashShopSendGiftDlg : public CEtUIDialog, public CEtUICallback
{
	enum
	{
		MESSAGEBOX_GIFT_CONFIRM,
		MESSAGEBOX_NO_USER_EXIST,
		LEVEL_ALERT_DLG,
	};
public:
	CDnCashShopSendGiftDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopSendGiftDlg() {}

	virtual void	Initialize(bool bShow);
	virtual void	Show(bool bShow);

	void			UpdateGiftReceiverInfo(const SCCashShopCheckReceiver& info);
	void			SetInfo(eCashUnitType type, bool bClear);
#ifdef PRE_ADD_CADGE_CASH
	void			SetCadgeReciever( std::wstring strReciever );
#endif // PRE_ADD_CADGE_CASH

protected:
	virtual void	InitialUpdate();
	virtual void	Process(float fElapsedTime);
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void	OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	void			Clear();
	void			SetFriendComboBox();
	void			OnAddEditBoxString(const std::wstring& strName);
	bool			CheckReceiverInfo();
	bool			DoSendGift();

	CEtUIStatic*			m_pLevelTitle;
	CEtUIStatic*			m_pLevel;

	CEtUIStatic*			m_pClassTitle;
	CEtUIStatic*			m_pClass;

	CEtUIIMEEditBox*		m_pEditBoxName;
	CEtUIComboBox*			m_pFriendComboBox;
	CEtUIButton*			m_pCheckUserInfoBtn;
	CEtUIButton*			m_pSendBtn;
	CEtUIButton*			m_pCancelBtn;

	CEtUIIMEEditBox*		m_pMemoEditBox;
	std::multimap<wchar_t, std::wstring>	m_FriendNameList;

	std::wstring			m_AutoCompleteTextCache;
	float					m_CheckUserInfoBtnCooltime;

	bool					m_bUserAuthorized;
	std::wstring			m_AuthorizedUserName;
	eCashUnitType			m_Mode;

	int						m_ReceiverJob;
	int						m_ReceiverLevel;
};