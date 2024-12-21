#pragma once
#include "EtUIDialog.h"
#include "DnLoginTask.h"
#include "DnDoorsMobileDefine.h"

#ifdef PRE_ADD_DOORS

class CDnDoorsMobileMessageBoxDlg;
class CDnDoorsMobileAuthButtonDlg : public CEtUIDialog
{
public:
	CDnDoorsMobileAuthButtonDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnDoorsMobileAuthButtonDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void ShowDoorsMobileMsgBoxDlg(bool bShow, DoorsMobileMsgBoxDlgType type);
	void OnSetDoorsAuthMobileMsgBox(const WCHAR* pKey);
	void OnCloseMessageBox();

	void EnableAuthButton(bool bEnable);

private:
	CEtUIButton* m_pAuthBtn;
	CDnDoorsMobileMessageBoxDlg* m_pAuthMessageBox;
};

#endif // PRE_ADD_DOORS