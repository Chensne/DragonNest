#pragma once

#include "EtUIDialog.h"

class CDnGuildRenameConfirmDlg : public CEtUIDialog
{
public:
	CDnGuildRenameConfirmDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL);
	virtual ~CDnGuildRenameConfirmDlg() {}

	virtual void	Initialize(bool bShow);
	virtual void	InitialUpdate();
	
	virtual void	Show(bool bShow);
	void			SetGuildName(const WCHAR* wszGuildName);

private:	
	CEtUIStatic*	m_pStaticRecheck;	
};