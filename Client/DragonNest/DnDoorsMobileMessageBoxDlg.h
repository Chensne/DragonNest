#pragma once
#include "EtUIDialog.h"
#include "DnLoginTask.h"
#include "DnDoorsMobileDefine.h"

#ifdef PRE_ADD_DOORS

class CDnDoorsMobileAuthButtonDlg;
class CDnDoorsMobileMessageBoxDlg : public CEtUIDialog
{
public:
	CDnDoorsMobileMessageBoxDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnDoorsMobileMessageBoxDlg(void);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);

	void ResetType(DoorsMobileMsgBoxDlgType type);
	DoorsMobileMsgBoxDlgType IsType() const { return m_Type; }
	void SetAuthData(const WCHAR* pKey);
	void SetParentAuthDlg(CDnDoorsMobileAuthButtonDlg* pParentDlg);

private:
	CEtUIButton* m_pAuthOrCancelAuthBtn;
	CEtUIStatic* m_pKeyStatic;
	CEtUIStatic* m_pKeyTitleStatic;
	CEtUIStatic* m_pTimerStatic;
	CEtUIButton* m_pCloseBtn;
	CDnDoorsMobileAuthButtonDlg* m_pParentAuthDlg;

	DoorsMobileMsgBoxDlgType m_Type;
	float m_fTimer;
	int m_PrevTimeCheck;
};

#endif // PRE_ADD_DOORS