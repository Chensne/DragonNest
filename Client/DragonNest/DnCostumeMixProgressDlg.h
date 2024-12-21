#pragma once

#include "EtUIDialog.h"

class CDnCostumeMixProgressDlg : public CEtUIDialog
{
public:
	CDnCostumeMixProgressDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCostumeMixProgressDlg(void);

protected:
	CEtUIProgressBar*	m_pProgressBar;
	float				m_TotalTime;
	float				m_RemainTime;

protected:
	void ShowControl(bool bShow);

public:
	void SetMessageBox(float totalTime, float elapsedTime, int nID = -1, CEtUICallback *pCall = NULL);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process(float fElapsedTime);
};