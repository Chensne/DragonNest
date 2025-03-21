#pragma once

#include "EtUIDialog.h"
#include "DnCashShopDefine.h"
#include "DnSmartMoveCursor.h"

class CDnCashShopGiftFailDlg : public CEtUIDialog
{
public:
	CDnCashShopGiftFailDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCashShopGiftFailDlg(void) {}
	virtual void	Initialize(bool bShow);
	void			SetInfo(const std::wstring& msg, const std::vector<CASHITEM_SN>& itemList, UINT uType, int nID = -1, CEtUICallback *pCall = NULL);
	virtual void	Show( bool bShow );

protected:
	virtual void	InitialUpdate();
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	void			ShowBtns(bool bShow);

	CEtUIButton*		m_pCloseBtn;
	CEtUIButton*		m_pCloseSubBtn;
	CEtUIButton*		m_pOKBtn;
	CEtUIButton*		m_pCancelBtn;
	CEtUITextBox*		m_pItemsTextBox;
	CEtUIStatic*		m_pFailStatic;
	CDnSmartMoveCursor	m_SmartMove;
	bool				m_bIgnoreEnterHotkey;
};