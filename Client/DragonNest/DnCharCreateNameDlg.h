#pragma once
#include "DnCustomDlg.h"
#ifdef PRE_MOD_CREATE_CHAR
#include "DnParts.h"
#endif // PRE_MOD_CREATE_CHAR


class CDnCharCreateNameDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnCharCreateNameDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCharCreateNameDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBoxName;

	bool m_bCallbackProcessed;
	bool m_bRequestWaitCreate;

public:
	void EnableCharCreateBackDlgControl(bool bEnable);

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Show(bool bShow);
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0);
#ifdef PRE_MOD_CREATE_CHAR
	void SetCharCreatePartsName(CDnParts::PartsTypeEnum PartsIndex, std::wstring strPartsName);
#endif // PRE_MOD_CREATE_CHAR
};
