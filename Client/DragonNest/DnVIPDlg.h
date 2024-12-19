#pragma once

#include "EtUIDialog.h"

#ifdef PRE_ADD_VIP

class CDnVIPDlg : public CEtUIDialog, public CEtUICallback
{
	enum
	{
		eMSGBOX_AUTOPAYCANCEL
	};
public:
	CDnVIPDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnVIPDlg(void) {}

	virtual void	Initialize(bool bShow);
	virtual void	InitialUpdate();
	virtual void	Show(bool bShow);
	virtual void	Process(float fElapsedTime);
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);

	void			SetInfo(bool bAutoPay, const __time64_t& expireDate, DWORD expireDateColor, const std::wstring& contents);

protected:
	virtual void	OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	CEtUIButton*		m_pBtnAutoPayCancel;
	CEtUIButton*		m_pBtnClose;
	CEtUITextBox*		m_pTextBoxContents;
	CEtUIStatic*		m_pStaticAutoPay;
};

#endif // PRE_ADD_VIP