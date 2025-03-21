#pragma once

#include "DnCustomDlg.h"
#include "DnGameControlTask.h"

class CDnGameKeySetDlg;

class CDnGameKeySetListDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnGameKeySetListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnGameKeySetListDlg();

public:
	void SetData( CDnGameControlTask::SKeyData * pData, CDnGameKeySetDlg * pGameKeySetDlg );
	void SetKeyString();

	bool EnableKeyListData();
	void DisableKeyListData( const bool bNotice );

protected:
	void ProcessPad();
	void RequestChangePad( const BYTE cFirstKey, const BYTE cSecondKey );
	void SetKeySuccess();

protected:
	CDnGameKeySetDlg * m_pGameKeySetDlg;

	CEtUIButton * m_pButtonDummy;
	CEtUIStatic * m_pStaticKeyBox;
	CEtUIStatic * m_pStaticDisableText;
	CEtUIStatic * m_pStaticEnableText;
	CEtUICheckBox * m_pCheckBox;

	bool m_bEnable;

	CDnGameControlTask::SKeyData m_Data;

	bool m_bPressKey;
	BYTE m_cKey;
	LOCAL_TIME m_PressTime;
public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void InitCustomControl(CEtUIControl *pControl);
	virtual void Show(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};