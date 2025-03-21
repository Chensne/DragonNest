#pragma once

#include "EtUIDialog.h"
#include "DnSmartMoveCursor.h"

class CDnCashShopMessageBox : public CEtUIDialog
{
public:
	enum eCSMsgBoxTextType
	{
		eMain,
		eSub,
		eMax
	};

	CDnCashShopMessageBox(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopMessageBox(void);

	void SetMsgBox(int nID, CEtUICallback* pCallBack);
	void SetMsgBoxText(eCSMsgBoxTextType type, const std::wstring& msg);

	virtual void	InitialUpdate();
	virtual void	Initialize(bool bShow);
//	virtual void	Show( bool bShow );

protected:
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);

	CEtUIButton*	m_pOKBtn;
	CEtUIButton*	m_pCancelBtn;
	CEtUIStatic*	m_pText[eMax];

	CDnSmartMoveCursor m_SmartMove;
};