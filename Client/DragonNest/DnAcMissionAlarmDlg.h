#pragma once

#ifdef PRE_ADD_ACTIVEMISSION

#include "EtUIDialog.h"

class CDnItem;

class CDnAcMissionAlarmDlg : public CEtUIDialog
{
public:
	CDnAcMissionAlarmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnAcMissionAlarmDlg(void){}

protected:
	CEtUIStatic * m_pStaticDesc;

	float m_fShowTime;	

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	

	void SetAlarm( std::wstring & str, float fadeTime );
};


#endif // PRE_ADD_ACTIVEMISSION