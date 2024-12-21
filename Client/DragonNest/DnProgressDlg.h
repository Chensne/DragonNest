#pragma once
#include "EtUIDialog.h"

class CEtUIStatic;
class CEtUIProgressBar;

class CDnProgressDlg : public CEtUIDialog
{
public:
	CDnProgressDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnProgressDlg(void);

protected:
	CEtUIStatic *m_pStaticText;
	CEtUIProgressBar *m_pProgressBarTime;

	float	m_fProgressBarTime;
	float	m_RemainTime;
	bool	m_bManualTimeCtrl;
	bool	m_bManualClose;

public:
	void SetInfo(LPCWSTR pwszMessage, float fTotalTime, float fRemainTime, bool bManualTimeCtrl, bool bManualClose, int nID = -1, CEtUICallback *pCall = NULL);
	void SetManualClose(bool bOn)			{ m_bManualClose = bOn; }
	void SetElapsedTime(float fElapsedTime);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

private:
	void ManualClose();
};
