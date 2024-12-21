#pragma once
#include "DnCustomDlg.h"
#ifdef PRE_MOD_CREATE_CHAR
#include "DnParts.h"
#endif // PRE_MOD_CREATE_CHAR


class CDnCharCreateSetupDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnCharCreateSetupDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCharCreateSetupDlg(void);

protected:
	CEtUIButton *m_pButtonFacePrior;
	CEtUIButton *m_pButtonFaceNext;
	CEtUIButton *m_pButtonHairColor[5];
	CEtUIButton *m_pButtonSkinColor[4];
	CEtUIButton *m_pButtonEyeColor[5];
	CEtUIButton *m_pButtonTurn;
	CEtUIButton *m_pButtonCreate;
#ifdef PRE_MOD_SELECT_CHAR
	enum { MAX_COSTUME = 4 };
	CEtUIRadioButton*	m_pCostumeRadioButton[MAX_COSTUME];
	CEtUIComboBox *m_pComboBoxServerList;
#endif // PRE_MOD_SELECT_CHAR

	bool m_bCallbackProcessed;
	bool m_bRequestWaitCreate;

public:
	void SetClassHairColor(int nClass);
	int GetSelectedServerID();

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
