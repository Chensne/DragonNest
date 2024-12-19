#pragma once
#include "DnCustomDlg.h"
#ifdef PRE_MOD_CREATE_CHAR
#include "DnParts.h"
#endif // PRE_MOD_CREATE_CHAR


class CDnCharCreateSetupGestureDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnCharCreateSetupGestureDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCharCreateSetupGestureDlg(void);

protected:
	enum { MAX_COSTUME = 4 };
	CEtUIComboBox *m_pComboBoxServerList;
	CEtUIRadioButton*	m_pCostumeRadioButton[MAX_COSTUME];

	bool m_bCallbackProcessed;
	bool m_bRequestWaitCreate;

private:
	void SetGestureInfo();

protected:
	enum { MAX_GESTURE = 5 };
	CDnLifeSkillButton*			m_pGestureSlotButton[MAX_GESTURE];
	CEtUIStatic*				m_pGestureText[MAX_GESTURE];
	CEtUIButton*				m_pGestureButton[MAX_GESTURE];
	std::vector<std::string>	m_vecStrGestureActionName;
	bool						m_bCharCreate;


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
