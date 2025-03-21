#pragma once
#include "DnOptionDlg.h"

class CDnGameControlComboDlg;

class CDnGameControlKeyDlg : public CDnOptionDlg, public CEtUICallback
{
public:
	CDnGameControlKeyDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGameControlKeyDlg();

private:
	typedef CDnOptionDlg BaseClass;

protected:
	enum
	{
		NUM_INPUT = 9,
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
	int m_iPrevSelect;
	bool m_bPressKey;
	BYTE m_cKey;
	LOCAL_TIME m_PressTime;

public:
	void Initialize( bool bShow ) override;

private:
	// DnGameControlUIDlg, DnGameControlQuickSlotDlg 의
	// MsgProc, ProcessCommand, Process, Show 함수 등이 거의 판박이지만,
	// 혹시 다르게 처리해야할게 있을까봐(혹은 기획이 변경될까봐) 우선은 cpp마다 작성을 했다.
	// 나중에 좀 더 확실해지면 개선해야한다.
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

	bool IsUsedKey( BYTE cDIK );
	bool IsUsedButton( BYTE cFirstKey, BYTE cSecondKey);

	virtual void ProcessCombo(const int index);
	void GetComboDialog(CDnGameControlComboDlg * pDialog);
};