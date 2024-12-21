#pragma once
#include "EtUIDialog.h"

#ifdef PRE_MOD_CHATBG

class CDnChatBoardDlg : public CEtUIDialog
{
public:
	CDnChatBoardDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnChatBoardDlg() {}

	void Initialize(bool bShow);
	void InitialUpdate();
	void UpdateDlgCoord(float fX, float fY, float fWidth, float fHeight);

private:
	CEtUIStatic* m_pBackBoard;
};

#endif