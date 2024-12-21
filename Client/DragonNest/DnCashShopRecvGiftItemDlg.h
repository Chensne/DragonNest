#pragma once

#include "EtUIDialog.h"
#include "DnCashShopDefine.h"

class CDnCashShopRecvGiftItemDlg : public CEtUIDialog
{
public:
	CDnCashShopRecvGiftItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopRecvGiftItemDlg(void) {}

	virtual void	Initialize(bool bShow);
	void			SetInfo(const SCashShopRecvGiftBasicInfo& info);
	INT64			GetGiftDBID() const { return m_giftDBID; }
	const WCHAR*	GetSenderName() const { return m_pNameStatic->GetText(); }

protected:
	virtual void	InitialUpdate();
// 	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);

private:
	void			Clear();

	INT64			m_giftDBID;

	CEtUIStatic*	m_pNameStatic;
	CEtUIStatic*	m_pDateStatic;
	CEtUIStatic*	m_pRestStatic;
};