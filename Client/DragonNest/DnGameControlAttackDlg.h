#pragma once
#include "DnOptionDlg.h"

class CDnGameControlComboDlg;

class CDnGameControlAttackDlg : public CDnOptionDlg, public CEtUICallback
{
public:
	CDnGameControlAttackDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	~CDnGameControlAttackDlg();

private:
	typedef CDnOptionDlg BaseClass;

protected:
	enum
	{
#if defined(PRE_ADD_AIM_ASSIST_BUTTON)
		NUM_INPUT = 5,
#else
		NUM_INPUT = 4,
#endif
	};

	SInputItem m_InputItem[NUM_INPUT];

	CEtUIStatic *m_pStaticIcon;
	CEtUIStatic *m_pStaticText;
	bool m_bTextHide;
	float m_fTextHideTime;

	int m_nInputModeItemIndex;
	bool m_bInputMode;

	// 키세팅은 창이 나눠져있기때문에 기본값으로 복구하는 코드가 다른 ResetDefault와는 조금 다르다.
	bool m_bResetDefaultAllKeySetting;

	CDnGameControlComboDlg * m_pGameControlComboDlg;
	bool m_bPressKey;
	BYTE m_cKey;
	LOCAL_TIME m_PressTime;


public:
	void Initialize( bool bShow ) override;

private:	
	void InitialUpdate() override;
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg ) override;
	void Process( float fElapsedTime ) override;
	void Show( bool bShow ) override;
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 ) override;

public:
	void ImportSetting() override;
	void ExportSetting() override;
	bool IsChanged() override;

	void ResetDefault();

	void SetInputMode( bool bMode, bool bProcessBlink = true );
	bool IsInputMode() { return m_bInputMode; }

	bool IsUsedButton( BYTE cFirstKey, BYTE cSecondKey );

	virtual void ProcessCombo(const int index);
	void GetComboDialog(CDnGameControlComboDlg * pDialog);
};