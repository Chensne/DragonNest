#pragma once
#include "EtUIDialog.h"
#include "DnInterface.h"
#include "DnSmartMoveCursor.h"

class CDnProgressMBox : public CEtUIDialog
{
public:
	CDnProgressMBox( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnProgressMBox(void);

protected:
	CEtUIButton*		m_pButtonDefault;
	CEtUIButton*		m_pButtonOther;
	CEtUIStatic*		m_pStaticMessage;
	CEtUIStatic*		m_pStaticTitle;
	CEtUIProgressBar*	m_pProgressBar;

	CDnSmartMoveCursor	m_SmartMove;

	float				m_TotalTime;
	float				m_RemainTime;
	bool				m_bManual;

protected:
	void ShowControl(bool bShow);

public:
	void SetTitle(LPCWSTR wszTitle, DWORD dwColor = EtInterface::textcolor::WHITE);
	void CDnProgressMBox::SetMessageBox(LPCWSTR pwszMessage, float totalTime, float elapsedTime, bool bManual, int nID = -1, CEtUICallback *pCall = NULL);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
